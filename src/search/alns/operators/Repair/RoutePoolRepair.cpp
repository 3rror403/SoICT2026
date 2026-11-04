#include "search/alns/operators/Repair/RoutePoolRepair.hpp"
#include "search/alns/operators/Repair/RoutePoolMaster.hpp"
#include <algorithm>
#include <cmath>
#include <functional>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <ostream>
#include <numeric>
#include <stdexcept>
#include <random>
#include "search/alns/operators/LocalSearch/LocalSearch.hpp"
#include "search/alns/operators/LocalSearch/RemoveRequest.hpp"
#include "search/alns/operators/LocalSearch/AddRequest.hpp"
#include "search/alns/operators/LocalSearch/PassengerRelocate.hpp"
#include "search/alns/operators/LocalSearch/ParcelRelocate.hpp"
#include "search/alns/operators/LocalSearch/CleanCut2OptStar.hpp"
#include "search/alns/operators/LocalSearch/SwapRequest.hpp"
#include "search/alns/operators/LocalSearch/ReplaceRequest.hpp"
#include "search/alns/operators/Destroy/RandomRemoval.hpp"
#include "search/alns/operators/Destroy/RelatedRemoval.hpp"
#include "search/alns/operators/Destroy/WorstRemoval.hpp"
#include "search/alns/MoveCost.hpp"

namespace soict::alns {
namespace {
// Erase the first minimum in map order and return the surviving minimum.
// The caller has exceeded a positive cap, so at least two entries exist.
template<class Bank, class Score>
double erase_worst(Bank& bank, Score score) {
    auto worst = bank.begin(), second = bank.end();
    for (auto it = std::next(worst); it != bank.end(); ++it) {
        if (score(it->second) < score(worst->second)) {
            second = worst;
            worst = it;
        } else if (second == bank.end() || score(it->second) < score(second->second)) {
            second = it;
        }
    }
    const auto floor = score(second->second);
    bank.erase(worst);
    return floor;
}
}

RoutePoolRepair::RoutePoolRepair(const Instance& instance, std::size_t limit, std::uint64_t seed)
    : instance_(instance), limit_(limit), floors_(instance.taxis.size(),-std::numeric_limits<double>::infinity()), seed_(seed),
      pool_(instance.taxis.size()), versions_(pool_.size(), 0) {
    if (!limit) throw std::invalid_argument("route pool limit must be positive");
}

void RoutePoolRepair::add(const SearchState& state, std::size_t taxi, bool enforce_limit) {
    const auto& nodes = state.cache(taxi).nodes();
    if (nodes.empty()) return;
    const auto k = state.taxi_count(), n = state.passenger_count();
    auto& requests = request_scratch_;
    requests.clear();
    double revenue = 0;
    for (const auto node : nodes) {
        std::size_t request;
        if (node < k + n) request = node - k;
        else if (node >= k + 2*n && node < k + 2*n + instance_.parcels.size())
            request = n + node - k - 2*n;
        else continue;
        requests.push_back(request);
        revenue += state.revenue(request);
    }
    std::sort(requests.begin(), requests.end());
    const auto profit = revenue - state.cache(taxi).cost();
    add_column(taxi, requests, nodes, profit, enforce_limit);
}

void RoutePoolRepair::add_column(std::size_t taxi, const std::vector<std::size_t>& requests,
        const std::vector<std::size_t>& nodes, double profit, bool enforce_limit) {
    if (!std::isfinite(profit) || profit <= 1e-9) return;
    auto& bank = pool_[taxi];
    if (enforce_limit && bank.size() >= limit_ && profit < floors_[taxi]) return;
    const auto found = bank.lower_bound(requests);
    if (found != bank.end() && found->first == requests) {
        if (found->second.profit >= profit) return;
        // Preserve the route buffer when a better sequence serves the same set.
        found->second.route.assign(nodes.begin(), nodes.end());
        found->second.profit = profit;
    } else {
        bank.emplace_hint(found, requests, Column{nodes, profit});
    }
    ++bank_revision_;
    if (!enforce_limit) floors_[taxi] = -std::numeric_limits<double>::infinity();
    if (enforce_limit && bank.size() > limit_) {
        floors_[taxi] = erase_worst(bank, [](const auto& column) { return column.profit; });
    }
}

void RoutePoolRepair::generate(Deadline deadline, std::size_t prefix_limit, bool priced) {
    if (!prefix_limit) throw std::invalid_argument("path prefix limit must be positive");
    generated_prefixes_ = completed_taxis_ = 0;
    if (std::chrono::steady_clock::now() >= deadline) return;
    const auto& in = instance_;
    const auto n = in.passengers.size(), nr = n + in.parcels.size(), nv = in.node_count();
    auto prices=priced?estimate_prices(deadline):std::vector<double>(nr,0);
    const double scales[]={1.0,0.8,1.2};
    if (priced) {
        const auto scale=scales[generation_calls_%3];for (auto& price:prices) price*=scale;
        std::cerr<<"path_price_scale="<<scale<<'\n';
    }
    const auto generation_seed=seed_+0x9e3779b97f4a7c15ULL*generation_calls_++;
    struct Request {
        std::size_t pickup, delivery;
        double ready, latest, drop_ready, drop_latest, service, revenue, weight;
    };
    std::vector<Request> requests;
    requests.reserve(nr);
    for (std::size_t r = 0; r < nr; ++r) {
        if (r < n) {
            const auto& p = in.passengers[r];
            requests.push_back({in.passenger_pickup(r), in.passenger_dropoff(r),
                p.pickup.earliest, p.pickup.latest, 0, std::numeric_limits<double>::infinity(),
                p.service, p.revenue-prices[r], 0});
        } else {
            const auto& p = in.parcels[r-n];
            requests.push_back({in.parcel_pickup(r-n), in.parcel_dropoff(r-n),
                p.pickup.earliest, p.pickup.latest, p.delivery.earliest, p.delivery.latest,
                p.service, p.revenue-prices[r], p.weight});
        }
    }
    const auto m=in.parcels.size();
    if (!generation_index_ready_) {
    // Unrestricted shortest paths give optimistic travel times even on
    // directed/nonmetric matrices. Landmark triangle differences therefore
    // bound every legal service path without assuming direct arcs are metric.
    const auto preprocess_start=std::chrono::steady_clock::now();
    const auto preprocess_seconds=std::min(0.6,0.25*std::chrono::duration<double>(deadline-preprocess_start).count());
    const auto preprocess_deadline=preprocess_start+std::chrono::duration_cast<Deadline::duration>(
        std::chrono::duration<double>(std::max(0.0,preprocess_seconds)));
    std::vector<std::vector<double>> landmarks;
    std::vector<double> spread(nv,std::numeric_limits<double>::infinity());
    std::vector<unsigned char> used_landmark(nv,0);
    std::size_t landmark=0;
    for (std::size_t l=0;l<std::min<std::size_t>(8,nv);++l) {
        if (std::chrono::steady_clock::now()>=preprocess_deadline) break;
        std::vector<double> distance(in.travel_time.begin()+landmark*nv,in.travel_time.begin()+(landmark+1)*nv);
        distance[landmark]=0;std::vector<unsigned char> visited(nv,0);bool complete=true;
        for (std::size_t step=0;step<nv;++step) {
            if ((step&63)==0 && std::chrono::steady_clock::now()>=preprocess_deadline) {complete=false;break;}
            std::size_t v=nv;double value=std::numeric_limits<double>::infinity();
            for (std::size_t u=0;u<nv;++u) if (!visited[u] && distance[u]<value) {v=u;value=distance[u];}
            if (v==nv) break;
            visited[v]=1;
            for (std::size_t u=0;u<nv;++u) if (!visited[u])
                distance[u]=std::min(distance[u],value+in.travel_time[v*nv+u]);
        }
        if (!complete) break;
        used_landmark[landmark]=1;
        for (std::size_t u=0;u<nv;++u) spread[u]=std::min(spread[u],distance[u]);
        landmarks.push_back(std::move(distance));double farthest=-1;
        for (std::size_t u=0;u<nv;++u) if (!used_landmark[u] && spread[u]>farthest) {farthest=spread[u];landmark=u;}
    }
    std::vector<double> delivery_lower(nv*m,0);
    for (std::size_t node=0;node<nv;++node) for (std::size_t r=0;r<m;++r) {
        auto& lower=delivery_lower[node*m+r];const auto drop=in.parcel_dropoff(r);
        for (const auto& d:landmarks) {
            const double guard=1e-8+64*std::numeric_limits<double>::epsilon()*nv*std::max({1.0,d[drop],d[node]});
            lower=std::max(lower,d[drop]-d[node]-guard);
        }
    }
    std::cerr<<"path_landmarks="<<landmarks.size()<<" path_landmark_seconds="
             <<std::chrono::duration<double>(std::chrono::steady_clock::now()-preprocess_start).count()<<'\n';
    std::vector<std::vector<Arc>> next(nv);
    for (std::size_t node = 0; node < nv; ++node) {
        if (std::chrono::steady_clock::now() >= deadline) return;
        auto& arcs = next[node];
        for (std::size_t r = 0; r < nr; ++r) {
            const auto travel = in.travel_time[node*nv+requests[r].pickup];
            const auto guard = 8*std::numeric_limits<double>::epsilon()*
                std::max({1.0,std::abs(requests[r].latest),std::abs(travel)});
            const auto latest = requests[r].latest - travel + 1e-8 + guard;
            if (latest >= -1e-8) arcs.push_back({r, latest});
        }
        std::sort(arcs.begin(), arcs.end(), [](const auto& a, const auto& b) {
            return a.latest_departure > b.latest_departure ||
                (a.latest_departure == b.latest_departure && a.request < b.request);
        });
    }
    next_=std::move(next);delivery_lower_=std::move(delivery_lower);generation_index_ready_=true;
    std::cerr<<"path_index_seconds="<<std::chrono::duration<double>(std::chrono::steady_clock::now()-preprocess_start).count()<<'\n';
    }
    const auto& next=next_;const auto& delivery_lower=delivery_lower_;
    generated_prefixes_ = completed_taxis_ = 0;
    std::vector<unsigned char> picked(nr, 0), opened(nr, 0);
    std::vector<std::size_t> route, served, key;
    route.reserve(2*nr); served.reserve(nr); key.reserve(nr);
    for (std::size_t taxi = 0; taxi < pool_.size(); ++taxi) {
        if (std::chrono::steady_clock::now() >= deadline) break;
        // Fairly divide a finite generation budget across the remaining taxis.
        const auto now = std::chrono::steady_clock::now();
        const auto taxi_deadline = deadline == Deadline::max() ? deadline :
            now + (deadline-now)/static_cast<Deadline::duration::rep>(pool_.size()-taxi);
        std::size_t prefixes = 0;
        std::map<std::vector<std::size_t>,std::pair<double,Column>> priced_bank;
        double priced_floor=-std::numeric_limits<double>::infinity();
        std::size_t prefix_ceiling = prefix_limit;
        auto branch_deadline=taxi_deadline;
        bool exhausted = false, incomplete = false;
        std::size_t first_remaining=0;
        for (const auto& arc:next[taxi]) {
            const auto& q=requests[arc.request];
            if (q.weight<=in.taxis[taxi].parcel_capacity+1e-8 &&
                in.travel_time[taxi*nv+q.pickup]<=q.latest+1e-8) ++first_remaining;
        }
        const auto visit = [&](auto&& self, std::size_t last, double time, double cost,
                               double revenue, double load, std::size_t open_count) -> void {
            if (exhausted) return;
            if (prefixes >= prefix_ceiling || ((prefixes & 63) == 0 &&
                std::chrono::steady_clock::now() >= branch_deadline)) {
                exhausted = true; return;
            }
            ++prefixes;
            for (auto r:served) if (opened[r] && time+delivery_lower[last*m+r-n]>requests[r].drop_latest+1e-8)
                return;
            if (!open_count && revenue-cost > 1e-9 &&
                (priced ? (priced_bank.size()<limit_ || revenue-cost>=priced_floor) :
                    (pool_[taxi].size()<limit_ || revenue-cost>=floors_[taxi]))) {
                key.assign(served.begin(), served.end());
                std::sort(key.begin(), key.end());
                if (!priced) add_column(taxi, key, route, revenue-cost, true);
                else {
                    const auto score=revenue-cost;
                    const auto found=priced_bank.lower_bound(key);
                    const bool existing=found!=priced_bank.end() && found->first==key;
                    if (!existing || found->second.first<score) {
                        double profit=-cost;for (auto r:key)
                            profit+=r<n?in.passengers[r].revenue:in.parcels[r-n].revenue;
                        if (existing) {
                            found->second.first=score;
                            found->second.second.route.assign(route.begin(),route.end());
                            found->second.second.profit=profit;
                        } else priced_bank.emplace_hint(found,key,std::make_pair(score,Column{route,profit}));
                        if (priced_bank.size()>limit_) {
                            priced_floor=erase_worst(priced_bank,[](const auto& entry){return entry.first;});
                        }
                    }
                }
            }
            // An overflowing departure can still end a complete route: the
            // full evaluator checks arrivals, with no return leg. It cannot
            // be extended to another finite arrival.
            if (!std::isfinite(time)) return;
            auto count=next[last].size();
            std::uint64_t hash=0;
            if (priced) {
                const auto eligible=std::partition_point(next[last].begin(),next[last].end(),
                    [&](const auto& arc){return time<=arc.latest_departure;});
                count=static_cast<std::size_t>(eligible-next[last].begin());
                hash=generation_seed+taxi*0x94d049bb133111ebULL+last*0xbf58476d1ce4e5b9ULL;
                hash=(hash^(hash>>30))*0xbf58476d1ce4e5b9ULL;hash^=hash>>27;
            }
            const auto delivery_events=served.size(),events=delivery_events+count;
            const auto offset=events && (hash&1)?static_cast<std::size_t>(hash%events):0;
            for (std::size_t i=0;i<events;++i) {
                const auto event=offset?(i+offset)%events:i;
                if (event<delivery_events) {
                const auto r = served[event];
                if (!opened[r]) continue;
                const auto& q = requests[r];
                const double arrival = time + in.travel_time[last*nv+q.delivery];
                if (!std::isfinite(arrival) || arrival > q.drop_latest + 1e-8) continue;
                opened[r] = 0; route.push_back(q.delivery);
                self(self, q.delivery, std::max(arrival,q.drop_ready)+q.service,
                     cost+in.travel_cost[last*nv+q.delivery], revenue, load-q.weight, open_count-1);
                route.pop_back(); opened[r] = 1;
                if (exhausted) return;
                continue;
                }
                const auto& arc=next[last][event-delivery_events];
                if (!priced && time>arc.latest_departure) break;
                const auto r = arc.request;
                if (picked[r]) continue;
                const auto& q = requests[r];
                if (load+q.weight > in.taxis[taxi].parcel_capacity+1e-8) continue;
                const double arrival = time+in.travel_time[last*nv+q.pickup];
                // Test the actual arrival as well: cancellation in the sorted
                // departure threshold is not a substitute for feasibility.
                if (arrival > q.latest+1e-8) continue;
                const bool first=priced && served.empty();
                if (first) {
                    const auto seed_start=std::chrono::steady_clock::now();
                    if (prefixes>=prefix_limit || seed_start>=taxi_deadline) {incomplete=true;return;}
                    // A difficult first pickup must not consume the entire
                    // taxi budget and hide every other entry into its route.
                    // Redistribute unused work from completed branches.
                    prefix_ceiling=prefixes+std::max<std::size_t>(1,(prefix_limit-prefixes)/first_remaining);
                    branch_deadline=taxi_deadline==Deadline::max()?taxi_deadline:
                        seed_start+(taxi_deadline-seed_start)/static_cast<Deadline::duration::rep>(first_remaining);
                }
                const double departure = std::max(arrival,q.ready)+q.service;
                const double expense = cost+in.travel_cost[last*nv+q.pickup];
                picked[r] = 1; served.push_back(r); route.push_back(q.pickup);
                if (r < n) {
                    route.push_back(q.delivery);
                    const auto drop_arrival = departure+in.travel_time[q.pickup*nv+q.delivery];
                    if (std::isfinite(drop_arrival))
                        self(self,q.delivery,drop_arrival+q.service,
                             expense+in.travel_cost[q.pickup*nv+q.delivery], revenue+q.revenue,load,open_count);
                    route.pop_back();
                } else {
                    opened[r] = 1;
                    self(self,q.pickup,departure,expense,revenue+q.revenue,load+q.weight,open_count+1);
                    opened[r] = 0;
                }
                route.pop_back(); served.pop_back(); picked[r] = 0;
                if (first) {incomplete|=exhausted;exhausted=false;--first_remaining;}
                else if (exhausted) return;
            }
        };
        visit(visit,taxi,0,0,0,0,0);
        generated_prefixes_ += prefixes;
        total_generated_prefixes_ += prefixes;
        completed_taxis_ += !exhausted && !incomplete;
        for (const auto& [key,c]:priced_bank) add_column(taxi,key,c.second.route,c.second.profit,false);
    }
}

std::vector<double> RoutePoolRepair::estimate_prices(Deadline deadline) const {
    const auto nr = instance_.passengers.size()+instance_.parcels.size();
    if (prices_revision_==bank_revision_ && cached_prices_.size()==nr) {
        std::cerr<<"path_price_reused=1\n";
        return cached_prices_;
    }
    struct PricedColumn {
        const std::vector<std::size_t>& requests;
        double profit;
    };
    std::vector<PricedColumn> columns;
    std::vector<std::size_t> taxis;
    for (std::size_t t = 0; t < pool_.size(); ++t) for (const auto& [key,c] : pool_[t]) {
        columns.push_back({key,c.profit});taxis.push_back(t);
    }
    if (route_master_uses_cplex() && std::chrono::steady_clock::now()<deadline) {
        std::vector<MasterColumn> master;master.reserve(columns.size());
        for (std::size_t j=0;j<columns.size();++j) master.push_back({taxis[j],columns[j].requests,columns[j].profit});
        const auto lp_deadline=std::min(deadline,std::chrono::steady_clock::now()+std::chrono::seconds(2));
        auto prices=route_master_prices(master,pool_.size(),nr,lp_deadline);
        std::vector<double> maximum(pool_.size(),0);
        for (std::size_t j=0;j<columns.size();++j) {
            double value=columns[j].profit;for (auto r:columns[j].requests) value-=prices[r];
            maximum[taxis[j]]=std::max(maximum[taxis[j]],value);
        }
        std::cerr<<"path_price_lp_checked="<<std::accumulate(prices.begin(),prices.end(),0.0)+
            std::accumulate(maximum.begin(),maximum.end(),0.0)<<'\n';
        cached_prices_=prices;prices_revision_=bank_revision_;
        return prices;
    }
    std::vector<std::size_t> order(columns.size());std::iota(order.begin(),order.end(),0);
    std::sort(order.begin(),order.end(),[&](auto a,auto b) { return columns[a].profit > columns[b].profit; });
    std::vector<unsigned char> occupied(nr,0), assigned(pool_.size(),0);
    double lower = 0;
    for (auto j : order) {
        if (assigned[taxis[j]]) continue;
        bool free = true;for (auto r : columns[j].requests) free &= !occupied[r];
        if (!free) continue;
        assigned[taxis[j]] = 1;for (auto r : columns[j].requests) occupied[r] = 1;
        lower += columns[j].profit;
    }
    std::vector<double> prices(nr,0), best_prices = prices;
    double best = std::numeric_limits<double>::infinity();
    for (std::size_t iteration = 0; iteration < 192; ++iteration) {
        std::vector<double> maximum(pool_.size(),0);
        std::vector<const PricedColumn*> chosen(pool_.size(),nullptr);
        for (std::size_t j = 0; j < columns.size(); ++j) {
            const auto& c = columns[j];double value = c.profit;
            for (auto r : c.requests) value -= prices[r];
            if (value > maximum[taxis[j]]) { maximum[taxis[j]] = value;chosen[taxis[j]] = &c; }
        }
        const auto dual = std::accumulate(prices.begin(),prices.end(),0.0)+
                          std::accumulate(maximum.begin(),maximum.end(),0.0);
        if (dual < best) { best = dual;best_prices = prices; }
        std::vector<std::size_t> uses(nr,0);
        for (const auto c : chosen) if (c) for (auto r : c->requests) ++uses[r];
        double norm = 0;
        for (std::size_t r = 0; r < nr; ++r) if (prices[r] > 0 || uses[r] > 1) {
            const auto g = static_cast<double>(uses[r])-1;norm += g*g;
        }
        if (norm == 0 || dual <= lower+1e-7 || !std::isfinite(dual)) break;
        const auto step = 0.7*(dual-lower)/norm;
        for (std::size_t r = 0; r < nr; ++r)
            prices[r] = std::max(0.0,prices[r]+step*(static_cast<double>(uses[r])-1));
    }
    if ((route_master_uses_cplex() || pool_.size()<=96) &&
        std::chrono::steady_clock::now()+std::chrono::milliseconds(250)<deadline) {
        std::vector<MasterColumn> subset;
        for (std::size_t taxi=0;taxi<pool_.size();++taxi) {
            std::vector<std::pair<double,const Bank::value_type*>> ranked;
            for (const auto& entry:pool_[taxi]) {
                const auto& [key,c]=entry;
                double score=c.profit;for (auto r:key) score-=best_prices[r];ranked.push_back({score,&entry});
            }
            const auto count=route_master_uses_cplex()?ranked.size():std::min<std::size_t>(64,ranked.size());
            std::partial_sort(ranked.begin(),ranked.begin()+count,ranked.end(),[](const auto& a,const auto& b){return a.first>b.first;});
            for (std::size_t j=0;j<count;++j) subset.push_back({taxi,ranked[j].second->first,ranked[j].second->second.profit});
        }
        const auto lp_deadline=std::min(deadline,std::chrono::steady_clock::now()+std::chrono::seconds(2));
        auto proposed=route_master_prices(subset,pool_.size(),nr,lp_deadline);
        double value=std::accumulate(proposed.begin(),proposed.end(),0.0);
        for (const auto& bank:pool_) {
            double maximum=0;
            for (const auto& [key,c]:bank) {
                double score=c.profit;for (auto r:key) score-=proposed[r];maximum=std::max(maximum,score);
            }
            value+=maximum;
        }
        std::cerr<<"path_price_subgradient="<<best<<" path_price_lp_checked="<<value<<'\n';
        if (value<best) best_prices=std::move(proposed);
    }
    return best_prices;
}

Solution RoutePoolRepair::generate_fleets(Deadline deadline, std::shared_ptr<const EmptyTaxiIndex> index,
                                       const Solution* initial, bool refine, bool clean_cut, std::size_t refinement_interval,
                                       bool light_refinement, std::size_t refinement_cycles) {
    if (!refinement_interval) throw std::invalid_argument("fleet refinement interval must be positive");
    if (!refinement_cycles) throw std::invalid_argument("fleet refinement cycles must be positive");
    const auto phase_start=std::chrono::steady_clock::now();
    const auto nr = instance_.passengers.size()+instance_.parcels.size();
    Solution blank;if (initial) blank=*initial;else blank.routes.resize(pool_.size());SearchState empty(instance_,blank);
    if (!index) index=build_empty_taxi_index(empty,std::min(deadline,std::chrono::steady_clock::now()+std::chrono::seconds(1)));
    empty.set_empty_taxi_index(std::move(index));
    std::vector<std::size_t> requests(nr);std::iota(requests.begin(),requests.end(),0);
    std::mt19937_64 rng(seed_+0x9e3779b97f4a7c15ULL*fleet_calls_++);
    std::size_t builds=0;
    double refinement_seconds=0,refinement_gain=0;
    SearchState state=empty,best=empty;PositiveInsertionCache cache;
    RandomRemoval random_removal;RelatedRemoval related_removal;WorstRemoval worst_removal;
    std::vector<std::unique_ptr<Move>> refinement;
    refinement.push_back(std::make_unique<RemoveRequest>());
    refinement.push_back(std::make_unique<AddRequest>());
    if (!light_refinement) {
        refinement.push_back(std::make_unique<PassengerRelocate>());
        refinement.push_back(std::make_unique<ParcelRelocate>());
    }
    if (clean_cut) refinement.push_back(std::make_unique<CleanCut2OptStar>());
    if (!light_refinement) refinement.push_back(std::make_unique<SwapRequest>());
    refinement.push_back(std::make_unique<ReplaceRequest>());
    std::vector<double> move_seconds(refinement.size(),0), move_gain(refinement.size(),0);
    while (std::chrono::steady_clock::now()<deadline) {
        if (builds) {
            if (builds%8==0 || state.benefit()<best.benefit()*0.995) state.copy_from(best);
            const auto removed=std::max<std::size_t>(4,state.served_count()/12);
            if (builds%4==1) random_removal.apply(state,removed,rng);
            else if (builds%4==2) related_removal.apply(state,removed,rng);
            else if (builds%4==3) worst_removal.apply(state,removed,rng);
            else for (std::size_t i=0,count=2+rng()%std::max<std::size_t>(2,pool_.size()/12);i<count;++i) {
                const auto taxi=rng()%pool_.size();std::vector<std::size_t> removed;
                for (auto r:requests) if (state.location(r).taxi==static_cast<int>(taxi)) removed.push_back(r);
                if (!removed.empty() && !state.remove_requests(taxi,removed))
                    throw std::logic_error("complete generated route removal was infeasible");
            }
        }
        std::shuffle(requests.begin(),requests.end(),rng);
        for (auto r:requests) {
            if (std::chrono::steady_clock::now()>=deadline) break;
            if (state.location(r).taxi>=0) continue;
            const auto choice=cache.best(state,r);if (!choice || choice->gain<=1e-9) continue;
            if (!state.insert(choice->request,choice->taxi,choice->pickup,choice->delivery))
                throw std::logic_error("generated fleet insertion disagrees with route evaluation");
            // Preserve complementary fleet routes even when their individual
            // profit is below the absolute bank floor.
            add(state,choice->taxi,false);
        }
        // A request rejected early in a construction can become profitable
        // after later insertions share its access travel. Fill to a fixed point.
        for (;;) {
            if (std::chrono::steady_clock::now()>=deadline) break;
            std::optional<InsertionChoice> choice;
            for (auto r:requests) if (state.location(r).taxi<0) {
                const auto& option=cache.best(state,r);
                if (option && option->gain>1e-9 && (!choice || option->gain>choice->gain)) choice=option;
            }
            if (!choice) break;
            if (!state.insert(choice->request,choice->taxi,choice->pickup,choice->delivery))
                throw std::logic_error("generated completion disagrees with route evaluation");
            add(state,choice->taxi,false);
        }
        // Expensive refinement is occasional, rather than every proposal.
        if (refine && builds%refinement_interval==0 && std::chrono::steady_clock::now()+std::chrono::milliseconds(250)<deadline) {
            const auto refinement_start=std::chrono::steady_clock::now();
            const auto before=state.benefit();
            for (std::size_t cycle=0;cycle<refinement_cycles;++cycle) {
                bool changed=false;
                for (std::size_t j=0;j<refinement.size();++j) {
                    const auto start=std::chrono::steady_clock::now();
                    const auto before_move=state.benefit();
                    changed=refinement[j]->improve(state)||changed;
                    move_seconds[j]+=std::chrono::duration<double>(std::chrono::steady_clock::now()-start).count();
                    move_gain[j]+=state.benefit()-before_move;
                }
                if (!changed) break;
            }
            refinement_seconds+=std::chrono::duration<double>(std::chrono::steady_clock::now()-refinement_start).count();
            refinement_gain+=state.benefit()-before;
            for (auto taxi:state.active_taxis()) add(state,taxi,false);
        }
        if (state.benefit()>best.benefit()+1e-9) {
            best.copy_from(state);
            std::cerr<<"path_fleet_best="<<best.benefit()<<" path_fleet_round="<<builds<<'\n';
        }
        ++builds;
    }
    std::cerr<<"path_fleet_builds="<<builds<<" path_fleet_seconds="<<
        std::chrono::duration<double>(std::chrono::steady_clock::now()-phase_start).count()
        <<" path_fleet_refinement_seconds="<<refinement_seconds<<" path_fleet_refinement_gain="<<refinement_gain<<'\n';
    for (std::size_t j=0;j<refinement.size();++j)
        std::cerr<<"path_refinement_move="<<refinement[j]->name()<<" path_refinement_seconds="<<move_seconds[j]
                 <<" path_refinement_gain="<<move_gain[j]<<'\n';
    return best.solution();
}

void RoutePoolRepair::generate_replacements(Deadline deadline, const SearchState& fleet) {
    const auto phase_start=std::chrono::steady_clock::now();
    auto prices=estimate_prices(deadline);
    const double scales[]={1.0,0.8,1.2};const auto scale=scales[pricing_calls_%3];
    for (auto& price:prices) price*=scale;std::cerr<<"path_price_scale="<<scale<<'\n';
    const auto nr=fleet.request_count(),n=fleet.passenger_count(),k=pool_.size();
    std::mt19937_64 rng(seed_+0x9e3779b97f4a7c15ULL*pricing_calls_++);
    std::size_t moves=0;
    for (std::size_t taxi=0;taxi<k;++taxi) {
        const auto now=std::chrono::steady_clock::now();if (now>=deadline) break;
        const auto taxi_deadline=now+(deadline-now)/static_cast<Deadline::duration::rep>(k-taxi);
        Solution initial;initial.routes.resize(k);initial.routes[taxi]=fleet.cache(taxi).nodes();
        std::vector<std::pair<double,const Column*>> ranked;
        for (const auto& [key,c]:pool_[taxi]) {
            double score=c.profit;for (auto r:key) score-=prices[r];ranked.push_back({score,&c});
        }
        const auto count=std::min<std::size_t>(3,ranked.size());
        std::partial_sort(ranked.begin(),ranked.begin()+count,ranked.end(),[](const auto& a,const auto& b){return a.first>b.first;});
        std::vector<std::vector<std::size_t>> seeds{initial.routes[taxi]};
        for (std::size_t j=0;j<count;++j) seeds.push_back(ranked[j].second->route);
        SearchState state(instance_,initial,fleet.insertion_scratch_handle());
        state.set_empty_taxi_index(fleet.empty_taxi_index());
        std::size_t restarts=0;
        for (std::size_t iteration=0;iteration<48 && std::chrono::steady_clock::now()<taxi_deadline;++iteration) {
            double gain=1e-8;std::optional<InsertionChoice> insertion;std::optional<std::size_t> outgoing;
            std::vector<std::size_t> replacement_nodes;
            for (std::size_t r=0;r<nr;++r) if (state.location(r).taxi<0) {
                if ((r&31)==0 && std::chrono::steady_clock::now()>=taxi_deadline) break;
                const auto choice=best_insertion_on_route(state,r,taxi,state.cache(taxi),gain+prices[r]);
                if (choice && choice->gain-prices[r]>gain) {gain=choice->gain-prices[r];insertion=choice;outgoing.reset();}
            }
            std::vector<std::size_t> served;
            for (std::size_t r=0;r<nr;++r) if (state.location(r).taxi>=0) served.push_back(r);
            for (auto r:served) {
                const auto removal=state.removal_gain(r);
                if (removal && *removal+prices[r]>gain) {
                    gain=*removal+prices[r];outgoing=r;insertion.reset();replacement_nodes.clear();
                }
                if (std::chrono::steady_clock::now()>=taxi_deadline) break;
                const auto loc=state.location(r);auto nodes=state.cache(taxi).nodes();
                nodes.erase(nodes.begin()+loc.dropoff);nodes.erase(nodes.begin()+loc.pickup);
                RouteCache reduced(instance_,taxi,nodes);
                const auto base_gain=removal_savings_cost(instance_,taxi,state.cache(taxi).nodes(),loc.pickup,loc.dropoff)-state.revenue(r);
                for (std::size_t incoming=0;incoming<nr;++incoming) if (state.location(incoming).taxi<0) {
                    if ((incoming&31)==0 && std::chrono::steady_clock::now()>=taxi_deadline) break;
                    const auto choice=best_insertion_on_route(state,incoming,taxi,reduced,gain-base_gain-prices[r]+prices[incoming]);
                    if (choice && choice->gain+base_gain+prices[r]-prices[incoming]>gain) {
                        gain=choice->gain+base_gain+prices[r]-prices[incoming];outgoing=r;insertion=choice;replacement_nodes=nodes;
                    }
                }
            }
            if (gain<=1e-8) {
                if (++restarts>8) break;
                initial.routes[taxi]=seeds[restarts%seeds.size()];
                SearchState restart(instance_,initial,fleet.insertion_scratch_handle());
                restart.set_empty_taxi_index(fleet.empty_taxi_index());state.copy_from(restart);
                served.clear();for (std::size_t r=0;r<nr;++r) if (state.location(r).taxi>=0) served.push_back(r);
                if (!served.empty()) {
                    std::shuffle(served.begin(),served.end(),rng);
                    served.resize(std::min(served.size(),static_cast<std::size_t>(1+rng()%3)));
                    state.remove_requests(taxi,served);
                }
                continue;
            }
            if (outgoing && insertion) {
                const auto r=insertion->request;
                const auto drop=r<n?instance_.passenger_dropoff(r):instance_.parcel_dropoff(r-n);
                replacement_nodes.insert(replacement_nodes.begin()+insertion->delivery,drop);
                replacement_nodes.insert(replacement_nodes.begin()+insertion->pickup,state.pickup_node(r));
                // Validate the completed replacement directly. A nonmetric
                // removal-only shortcut need not be feasible.
                Solution candidate;candidate.routes.resize(k);candidate.routes[taxi]=replacement_nodes;
                SearchState changed(instance_,candidate,fleet.insertion_scratch_handle());
                changed.set_empty_taxi_index(fleet.empty_taxi_index());state.copy_from(changed);
            } else if (outgoing) {
                if (!state.remove(*outgoing)) throw std::logic_error("priced removal disagrees with evaluation");
            } else if (!state.insert(insertion->request,taxi,insertion->pickup,insertion->delivery))
                throw std::logic_error("priced insertion disagrees with evaluation");
            add(state,taxi,false);++moves;
        }
    }
    std::cerr<<"path_priced_moves="<<moves<<" path_priced_seconds="<<
        std::chrono::duration<double>(std::chrono::steady_clock::now()-phase_start).count()<<'\n';
}

void RoutePoolRepair::remember(const SearchState& state) {
    if (&state.instance() != &instance_) throw std::logic_error("route pool instance mismatch");
    for (const auto taxi : state.active_taxis()) {
        if (versions_[taxi] == state.cache(taxi).version()) continue;
        versions_[taxi] = state.cache(taxi).version();
        add(state, taxi, true);
    }
}

void RoutePoolRepair::set_limit(std::size_t limit) {
    if (!limit) throw std::invalid_argument("route pool limit must be positive");
    limit_ = limit;
    std::fill(floors_.begin(),floors_.end(),-std::numeric_limits<double>::infinity());
    for (auto& bank : pool_) while (bank.size() > limit_) {
        const auto worst = std::min_element(bank.begin(), bank.end(), [](const auto& a, const auto& b) {
            return a.second.profit < b.second.profit;
        });
        bank.erase(worst);
        ++bank_revision_;
    }
}

void RoutePoolRepair::write(std::ostream& out) const {
    out << "taxi,benefit,route\n" << std::setprecision(17);
    for (std::size_t taxi = 0; taxi < pool_.size(); ++taxi)
        for (const auto& [key, column] : pool_[taxi]) {
            out << taxi + 1 << ',' << column.profit << ",\"";
            for (std::size_t i = 0; i < column.route.size(); ++i) {
                if (i) out << ' ';
                out << instance_.vertex_id(column.route[i]);
            }
            out << "\"\n";
        }
}

bool RoutePoolRepair::improve(SearchState& state, Deadline deadline) {
    if (&state.instance() != &instance_) throw std::logic_error("route pool instance mismatch");
    // Restore every incumbent route even if the bank's size cap removed it.
    for (const auto taxi : state.active_taxis()) add(state, taxi, false);
    struct SelectionColumn {
        std::vector<std::size_t> requests, route;
        double profit;
    };
    std::vector<std::vector<SelectionColumn>> columns(pool_.size());
    std::vector<int> selected(pool_.size(), -1), incumbent(pool_.size(), -1);
    std::vector<unsigned char> occupied(state.request_count(), 0);
    std::vector<std::size_t> frequency(state.request_count(), 0), order;
    column_count_ = 0;
    double best_profit = 0;
    for (std::size_t taxi = 0; taxi < pool_.size(); ++taxi) {
        auto& row = columns[taxi];
        row.reserve(pool_[taxi].size());
        for (const auto& [key, column] : pool_[taxi]) row.push_back({key,column.route,column.profit});
        std::sort(row.begin(), row.end(), [](const auto& a, const auto& b) {
            return a.profit > b.profit || (a.profit == b.profit && a.requests < b.requests);
        });
        column_count_ += row.size();
        std::vector<unsigned char> present(state.request_count(), 0);
        for (std::size_t i = 0; i < row.size(); ++i) {
            for (auto r : row[i].requests) present[r] = 1;
            bool current = !row[i].requests.empty();
            for (auto r : row[i].requests) current &= state.location(r).taxi == static_cast<int>(taxi);
            if (current) {
                // The served-set key must equal the entire incumbent route.
                std::size_t count = 0;
                for (std::size_t r = 0; r < state.request_count(); ++r)
                    count += state.location(r).taxi == static_cast<int>(taxi);
                if (count == row[i].requests.size()) { incumbent[taxi] = static_cast<int>(i); best_profit += row[i].profit; }
            }
        }
        for (std::size_t r = 0; r < present.size(); ++r) frequency[r] += present[r];
    }
    // Constrain the taxis with the most shared requests before independent ones.
    std::vector<std::size_t> conflict(pool_.size(), 0);
    for (std::size_t taxi = 0; taxi < columns.size(); ++taxi)
        for (const auto& column : columns[taxi])
            for (auto r : column.requests) conflict[taxi] += frequency[r] - 1;
    for (std::size_t taxi = 0; taxi < columns.size(); ++taxi)
        if (!columns[taxi].empty()) order.push_back(taxi);
    std::stable_sort(order.begin(), order.end(), [&](auto a, auto b) { return conflict[a] > conflict[b]; });
    const auto compatible = [&](const SelectionColumn& column) {
        for (auto r : column.requests) if (occupied[r]) return false;
        return true;
    };
    nodes_ = 0; timed_out_ = false;complete_ = false;
    const auto warmup_start = std::chrono::steady_clock::now();
    const auto warmup_deadline = order.size() > 48 ?
        std::min(deadline,warmup_start+std::chrono::seconds(2)) : deadline;
    // Repair small groups around the feasible incumbent before searching the
    // complete master. This supplies good feasible bounds under a short limit.
    const auto optimize_group = [&](const std::vector<std::size_t>& group) {
        std::fill(occupied.begin(), occupied.end(), 0);
        for (std::size_t taxi = 0; taxi < incumbent.size(); ++taxi) {
            if (incumbent[taxi] < 0 || std::find(group.begin(), group.end(), taxi) != group.end()) continue;
            for (auto r : columns[taxi][incumbent[taxi]].requests) occupied[r] = 1;
        }
        double old_profit = 0;
        std::vector<int> chosen(group.size(), -1), best_group;
        for (auto taxi : group) {
            best_group.push_back(incumbent[taxi]);
            if (incumbent[taxi] >= 0) old_profit += columns[taxi][incumbent[taxi]].profit;
        }
        double group_profit = old_profit;
        std::function<void(std::size_t,double)> visit = [&](std::size_t depth, double profit) {
            if (timed_out_) return;
            ++nodes_;
            if ((nodes_ & 63) == 0 && std::chrono::steady_clock::now() >= warmup_deadline) {
                timed_out_ = true; return;
            }
            if (depth == group.size()) {
                if (profit > group_profit + 1e-9) { group_profit = profit; best_group = chosen; }
                return;
            }
            double bound = profit;
            for (std::size_t d = depth; d < group.size(); ++d)
                for (const auto& column : columns[group[d]])
                    if (compatible(column)) { bound += column.profit; break; }
            if (bound <= group_profit + 1e-9) return;
            const auto taxi = group[depth];
            for (std::size_t i = 0; i < columns[taxi].size(); ++i) {
                const auto& column = columns[taxi][i];
                if (!compatible(column)) continue;
                chosen[depth] = static_cast<int>(i);
                for (auto r : column.requests) occupied[r] = 1;
                visit(depth + 1, profit + column.profit);
                for (auto r : column.requests) occupied[r] = 0;
                if (timed_out_) break;
            }
            chosen[depth] = -1;
            if (!timed_out_) visit(depth + 1, profit);
        };
        if (std::chrono::steady_clock::now() < warmup_deadline) visit(0,0);
        else timed_out_ = true;
        if (group_profit <= old_profit + 1e-9) return false;
        for (std::size_t i = 0; i < group.size(); ++i) incumbent[group[i]] = best_group[i];
        best_profit += group_profit - old_profit;
        return true;
    };
    const auto pair_start = std::chrono::steady_clock::now();
    bool changed = true;
    while (changed && !timed_out_) {
        changed = false;
        for (std::size_t a = 0; a < order.size() && !timed_out_; ++a)
            for (std::size_t b = a + 1; b < order.size() && !timed_out_; ++b)
                changed = optimize_group({order[a],order[b]}) || changed;
    }
    const auto triple_start = std::chrono::steady_clock::now();
    std::cerr << "route_pool_pair_seconds=" << std::chrono::duration<double>(triple_start-pair_start).count()
              << " route_pool_pair_nodes=" << nodes_ << " route_pool_pair_benefit=" << best_profit << '\n';
    const auto pair_nodes = nodes_;
    for (std::size_t a = 0; order.size()<=48 && a < order.size() && !timed_out_; ++a)
        for (std::size_t b = a + 1; b < order.size() && !timed_out_; ++b)
            for (std::size_t c = b + 1; c < order.size() && !timed_out_; ++c)
                optimize_group({order[a],order[b],order[c]});
    const auto dual_start = std::chrono::steady_clock::now();
    std::cerr << "route_pool_triple_seconds=" << std::chrono::duration<double>(dual_start-triple_start).count()
              << " route_pool_triple_nodes=" << nodes_-pair_nodes << " route_pool_triple_benefit=" << best_profit << '\n';
    if (column_count_ > 2000) {
        std::vector<MasterColumn> master;
        std::vector<std::pair<std::size_t,std::size_t>> locations;
        std::vector<int> initial(incumbent.size(),-1);
        for (std::size_t taxi = 0; taxi < columns.size(); ++taxi)
            for (std::size_t i = 0; i < columns[taxi].size(); ++i) {
                if (incumbent[taxi] == static_cast<int>(i)) initial[taxi] = static_cast<int>(master.size());
                const auto& c = columns[taxi][i];
                master.push_back({taxi,c.requests,c.profit}); locations.emplace_back(taxi,i);
            }
        const auto guides = columns.size() > 48 && !route_master_uses_cplex() ? estimate_prices() : std::vector<double>{};
        const auto solved = columns.size() > 48 ?
            improve_route_master(master,columns.size(),state.request_count(),initial,deadline,seed_,guides) :
            solve_route_master(master,columns.size(),state.request_count(),initial,deadline);
        if (solved.request_prices.size()==state.request_count()) {
            cached_prices_=solved.request_prices;prices_revision_=bank_revision_;
        }
        best_profit = solved.profit; timed_out_ = solved.timed_out;complete_ = solved.complete; nodes_ += solved.nodes;
        std::fill(incumbent.begin(),incumbent.end(),-1);
        for (auto id : solved.selected) if (id >= 0) {
            const auto [taxi,index] = locations[id]; incumbent[taxi] = static_cast<int>(index);
        }
        std::cerr << "route_pool_lp_nodes=" << solved.nodes << " route_pool_lp_pivots=" << solved.pivots
                  << " route_pool_lp_upper=" << solved.root_upper << '\n';
    } else {
    // Nonnegative request prices give a valid Lagrangian upper bound:
    // sum(price) + sum_t max(0, profit(route) - sum_request price).
    // Use projected subgradients to tighten it; every iterate is valid, so
    // convergence is unnecessary for correctness or safe early termination.
    std::vector<double> prices(state.request_count(), 0), best_prices = prices;
    double best_dual = std::numeric_limits<double>::infinity();
    std::vector<std::vector<double>> reduced(columns.size());
    std::vector<std::vector<std::size_t>> branches(columns.size());
    for (std::size_t iteration = 0; iteration < 192 && !timed_out_; ++iteration) {
        if (std::chrono::steady_clock::now() >= deadline) { timed_out_ = true; break; }
        double dual = std::accumulate(prices.begin(), prices.end(), 0.0);
        std::vector<std::size_t> uses(prices.size(), 0);
        for (auto taxi : order) {
            double maximum = 0; int chosen = -1;
            for (std::size_t i = 0; i < columns[taxi].size(); ++i) {
                const auto& column = columns[taxi][i];
                double value = column.profit;
                for (auto r : column.requests) value -= prices[r];
                if (value > maximum) { maximum = value; chosen = static_cast<int>(i); }
            }
            dual += maximum;
            if (chosen >= 0) for (auto r : columns[taxi][chosen].requests) ++uses[r];
        }
        if (dual < best_dual) { best_dual = dual; best_prices = prices; }
        double norm = 0;
        for (std::size_t r = 0; r < prices.size(); ++r)
            if (prices[r] > 0 || uses[r] > 1) {
                const double gradient = static_cast<double>(uses[r]) - 1;
                norm += gradient * gradient;
            }
        if (!norm || dual <= best_profit + 1e-7) break;
        const double step = 0.9 * (dual - best_profit) / norm;
        for (std::size_t r = 0; r < prices.size(); ++r)
            prices[r] = std::max(0.0, prices[r] + step * (static_cast<double>(uses[r]) - 1));
    }
    const auto tree_start = std::chrono::steady_clock::now();
    std::cerr << "route_pool_dual_seconds=" << std::chrono::duration<double>(tree_start-dual_start).count()
              << " route_pool_upper=" << best_dual << " route_pool_lower=" << best_profit << '\n';
    prices = std::move(best_prices);
    const double price_total = std::accumulate(prices.begin(), prices.end(), 0.0);
    for (auto taxi : order) {
        auto& values = reduced[taxi];
        for (const auto& column : columns[taxi]) {
            double value = column.profit;
            for (auto r : column.requests) value -= prices[r];
            values.push_back(value);
        }
        auto& indices = branches[taxi];
        indices.resize(values.size()); std::iota(indices.begin(), indices.end(), 0);
        std::stable_sort(indices.begin(), indices.end(), [&](auto a, auto b) { return values[a] > values[b]; });
    }
    std::fill(occupied.begin(), occupied.end(), 0);
    std::function<void(std::size_t,double,double)> search = [&](std::size_t depth, double profit, double price_used) {
        if (timed_out_) return;
        ++nodes_;
        if ((nodes_ & 63) == 0 && std::chrono::steady_clock::now() >= deadline) {
            timed_out_ = true; return;
        }
        if (depth == order.size()) {
            if (profit > best_profit + 1e-9) { best_profit = profit; incumbent = selected; }
            return;
        }
        double bound = profit;
        for (std::size_t d = depth; d < order.size(); ++d)
            for (const auto& column : columns[order[d]])
                if (compatible(column)) { bound += column.profit; break; }
        if (bound <= best_profit + 1e-9) return;
        double priced_bound = profit + price_total - price_used;
        for (std::size_t d = depth; d < order.size(); ++d) {
            const auto taxi = order[d];
            double maximum = 0;
            for (auto i : branches[taxi]) {
                if (reduced[taxi][i] <= 0) break;
                if (compatible(columns[taxi][i])) { maximum = reduced[taxi][i]; break; }
            }
            priced_bound += maximum;
        }
        // Conservatively tolerate floating-point accumulation in the dual.
        if (priced_bound < best_profit - 1e-7) return;
        const auto taxi = order[depth];
        for (auto i : branches[taxi]) {
            const auto& column = columns[taxi][i];
            if (!compatible(column)) continue;
            selected[taxi] = static_cast<int>(i);
            double used = price_used;
            for (auto r : column.requests) { occupied[r] = 1; used += prices[r]; }
            search(depth + 1, profit + column.profit, used);
            for (auto r : column.requests) occupied[r] = 0;
            if (timed_out_) break;
        }
        selected[taxi] = -1;
        if (!timed_out_) search(depth + 1, profit, price_used); // unused taxi is always permitted
    };
    if (std::chrono::steady_clock::now() < deadline) search(0,0,0);
    else timed_out_ = true;
    complete_ = !timed_out_;
    }
    if (best_profit <= state.benefit() + 1e-9) return false;
    Solution candidate;
    candidate.routes.resize(state.taxi_count());
    for (std::size_t taxi = 0; taxi < incumbent.size(); ++taxi)
        if (incumbent[taxi] >= 0) candidate.routes[taxi] = columns[taxi][incumbent[taxi]].route;
    const auto evaluated = candidate.evaluate(instance_);
    if (!evaluated.feasible || std::abs(evaluated.benefit - best_profit) > 1e-6)
        throw std::logic_error("route pool selection disagrees with full evaluation");
    SearchState repaired(instance_, std::move(candidate), state.insertion_scratch_handle());
    repaired.set_empty_taxi_index(state.empty_taxi_index());
    state.copy_from(repaired);
    return true;
}
} // namespace soict::alns
