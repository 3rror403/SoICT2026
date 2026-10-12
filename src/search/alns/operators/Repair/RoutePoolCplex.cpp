#include "search/alns/operators/Repair/RoutePoolMaster.hpp"
#include <ilcplex/cplex.h>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <string>

namespace soict::alns {
namespace {
void check_status(int status) {
    if (status) throw std::runtime_error("CPLEX status " + std::to_string(status));
}

class PackingProblem {
    CPXENVptr env_ = nullptr;
    CPXLPptr problem_ = nullptr;
    void close() noexcept {
        if (problem_) CPXfreeprob(env_, &problem_);
        if (env_) CPXcloseCPLEX(&env_);
    }
public:
    PackingProblem(const std::vector<MasterColumn>& columns,
                   std::size_t taxis, std::size_t requests, bool integer) {
        int status = 0;
        env_ = CPXopenCPLEX(&status);
        if (!env_) throw std::runtime_error("CPLEX environment " + std::to_string(status));
        try {
            check_status(CPXsetintparam(env_, CPXPARAM_Threads, 1));
            check_status(CPXsetintparam(env_, CPXPARAM_ScreenOutput, CPX_OFF));
            check_status(CPXsetintparam(env_, CPXPARAM_ClockType, 2));
            problem_ = CPXcreateprob(env_, &status, "route-packing");
            check_status(status);
            const auto nc = static_cast<int>(columns.size()), rows = static_cast<int>(taxis+requests);
            std::vector<int> start(nc), count(nc), indices;
            std::vector<double> objective(nc), lower(nc, 0), upper(nc, 1);
            for (int j=0;j<nc;++j) {
                start[j]=static_cast<int>(indices.size());
                indices.push_back(static_cast<int>(columns[j].taxi));
                for (auto r:columns[j].requests) indices.push_back(static_cast<int>(taxis+r));
                count[j]=static_cast<int>(indices.size())-start[j];objective[j]=columns[j].profit;
            }
            std::vector<double> entries(indices.size(),1), rhs(rows,1);
            std::vector<char> sense(rows,'L');
            check_status(CPXcopylp(env_,problem_,nc,rows,CPX_MAX,objective.data(),rhs.data(),sense.data(),
                start.data(),count.data(),indices.data(),entries.data(),lower.data(),upper.data(),nullptr));
            if (integer) {
                std::vector<char> types(nc,'B');
                check_status(CPXcopyctype(env_,problem_,types.data()));
                check_status(CPXsetdblparam(env_,CPXPARAM_MIP_Tolerances_MIPGap,0));
                check_status(CPXsetdblparam(env_,CPXPARAM_MIP_Tolerances_AbsMIPGap,0));
            }
        } catch (...) { close(); throw; }
    }
    ~PackingProblem() {close();}
    PackingProblem(const PackingProblem&) = delete;
    PackingProblem& operator=(const PackingProblem&) = delete;
    CPXENVptr env() const {return env_;}
    CPXLPptr problem() const {return problem_;}
    bool allowance(Deadline deadline) {
        const auto remaining=std::chrono::duration<double>(deadline-std::chrono::steady_clock::now()).count();
        if (remaining<=0.01) return false;
        check_status(CPXsetdblparam(env_,CPXPARAM_TimeLimit,remaining-0.01));return true;
    }
};
}

std::vector<double> cplex_route_prices(const std::vector<MasterColumn>& columns,
        std::size_t taxis, std::size_t requests, Deadline deadline) {
    std::vector<double> prices(requests,0);
    if (columns.empty() || std::chrono::steady_clock::now()>=deadline) return prices;
    PackingProblem model(columns,taxis,requests,false);
    if (!model.allowance(deadline)) return prices;
    check_status(CPXdualopt(model.env(),model.problem()));
    std::vector<double> dual(taxis+requests,0);
    const auto status=CPXgetpi(model.env(),model.problem(),dual.data(),0,static_cast<int>(dual.size())-1);
    if (!status) for (std::size_t r=0;r<requests;++r)
        if (std::isfinite(dual[taxis+r])) prices[r]=std::max(0.0,dual[taxis+r]);
    std::cerr<<"path_price_cplex_status="<<CPXgetstat(model.env(),model.problem())<<'\n';
    return prices;
}

static MasterResult solve_packing(const std::vector<MasterColumn>& columns,
        std::size_t taxis, std::size_t requests, const std::vector<int>& incumbent, Deadline deadline) {
    MasterResult result;result.selected=incumbent;
    for (auto id:incumbent) if (id>=0) result.profit+=columns[id].profit;
    result.root_upper=std::numeric_limits<double>::infinity();
    if (columns.empty()) {result.complete=true;return result;}
    if (std::chrono::steady_clock::now()>=deadline) {result.timed_out=true;return result;}
    PackingProblem model(columns,taxis,requests,true);
    std::vector<int> indices(columns.size());std::iota(indices.begin(),indices.end(),0);
    std::vector<double> initial(columns.size(),0);
    for (auto id:incumbent) if (id>=0) initial[id]=1;
    const int start=0,effort=CPX_MIPSTART_CHECKFEAS;
    check_status(CPXaddmipstarts(model.env(),model.problem(),1,static_cast<int>(columns.size()),&start,
        indices.data(),initial.data(),&effort,nullptr));
    if (!model.allowance(deadline)) {result.timed_out=true;return result;}
    check_status(CPXmipopt(model.env(),model.problem()));
    const auto status=CPXgetstat(model.env(),model.problem());
    std::vector<double> values(columns.size(),0);
    bool verified=false;
    if (!CPXgetx(model.env(),model.problem(),values.data(),0,static_cast<int>(values.size())-1)) {
        std::vector<unsigned char> occupied(requests,0);std::vector<int> selected(taxis,-1);
        double profit=0;bool valid=true;
        for (std::size_t j=0;j<values.size();++j) if (values[j]>0.5) {
            const auto& c=columns[j];if (selected[c.taxi]>=0) {valid=false;break;}
            for (auto r:c.requests) {if (occupied[r]) valid=false;occupied[r]=1;}
            selected[c.taxi]=static_cast<int>(j);profit+=c.profit;
        }
        double claimed=0;
        verified=valid && std::isfinite(profit) && !CPXgetobjval(model.env(),model.problem(),&claimed)
            && std::abs(claimed-profit)<=1e-5;
        if (verified && profit>result.profit+1e-9) {result.profit=profit;result.selected=std::move(selected);}
    }
    double upper=0;
    if (!CPXgetbestobjval(model.env(),model.problem(),&upper) && std::isfinite(upper)) result.root_upper=upper;
    result.nodes=static_cast<std::size_t>(std::max<CPXLONG>(0,CPXgetnodecnt(model.env(),model.problem())));
    result.complete=verified && status==CPXMIP_OPTIMAL;
    result.timed_out=status==CPXMIP_TIME_LIM_FEAS || status==CPXMIP_TIME_LIM_INFEAS ||
        std::chrono::steady_clock::now()>=deadline;
    std::cerr<<"route_pool_cplex_status="<<status<<" route_pool_cplex_incumbent="<<result.profit
             <<" route_pool_cplex_upper="<<result.root_upper<<'\n';
    return result;
}

MasterResult cplex_route_master(const std::vector<MasterColumn>& columns,
        std::size_t taxis, std::size_t requests, const std::vector<int>& incumbent, Deadline deadline) {
    MasterResult best;best.selected=incumbent;best.root_upper=std::numeric_limits<double>::infinity();
    for (auto id:incumbent) if (id>=0) best.profit+=columns[id].profit;
    if (columns.empty()) {best.complete=true;return best;}
    if (std::chrono::steady_clock::now()>=deadline) {best.timed_out=true;return best;}
    const bool short_budget=deadline-std::chrono::steady_clock::now()<=std::chrono::seconds(10);
    const auto price_deadline=std::min(deadline,std::chrono::steady_clock::now()+std::chrono::seconds(2));
    const auto prices=cplex_route_prices(columns,taxis,requests,price_deadline);
    best.request_prices=prices;
    std::vector<std::vector<std::size_t>> banks(taxis);std::vector<double> score(columns.size());
    for (std::size_t j=0;j<columns.size();++j) {
        score[j]=columns[j].profit;for (auto r:columns[j].requests) score[j]-=prices[r];banks[columns[j].taxi].push_back(j);
    }
    for (auto& bank:banks) std::sort(bank.begin(),bank.end(),[&](auto a,auto b){return score[a]!=score[b]?score[a]>score[b]:a<b;});
    double checked_upper=std::accumulate(prices.begin(),prices.end(),0.0);
    for (const auto& bank:banks) if (!bank.empty()) checked_upper+=std::max(0.0,score[bank.front()]);
    std::cerr<<"route_pool_full_price_bound="<<checked_upper<<'\n';
    for (std::size_t width:{32,64,0}) {
        if (short_budget && width!=32) break;
        const auto now=std::chrono::steady_clock::now();
        const auto remaining=std::chrono::duration<double>(deadline-now).count();
        if (remaining<0.05 || (!width && remaining<5)) break;
        std::vector<MasterColumn> reduced;std::vector<std::size_t> global;
        std::vector<int> initial(taxis,-1);
        for (std::size_t taxi=0;taxi<taxis;++taxi) {
            const auto count=width?std::min(width,banks[taxi].size()):banks[taxi].size();
            const auto add=[&](std::size_t id) {
                if (best.selected[taxi]==static_cast<int>(id)) initial[taxi]=static_cast<int>(reduced.size());
                reduced.push_back(columns[id]);global.push_back(id);
            };
            for (std::size_t j=0;j<count;++j) add(banks[taxi][j]);
            if (best.selected[taxi]>=0 && initial[taxi]<0) add(static_cast<std::size_t>(best.selected[taxi]));
        }
        const bool full=reduced.size()==columns.size();
        const auto allowance=full?remaining:std::min(width==32?4.0:6.0,remaining*(width==32?0.4:0.7));
        // Reconstructing time_point::max() through floating-point seconds can
        // round past the clock's representable range. A full-bank stage uses
        // the original deadline directly; ranked stages have finite caps.
        const auto stage_deadline=full?deadline:std::min(deadline,now+
            std::chrono::duration_cast<Deadline::duration>(std::chrono::duration<double>(allowance)));
        std::cerr<<"route_pool_cplex_scope="<<(full?"full":"ranked")<<" route_pool_cplex_columns="<<reduced.size()<<'\n';
        auto solved=solve_packing(reduced,taxis,requests,initial,stage_deadline);
        best.nodes+=solved.nodes;
        if (solved.profit>best.profit+1e-9) {
            best.profit=solved.profit;
            for (std::size_t taxi=0;taxi<taxis;++taxi)
                best.selected[taxi]=solved.selected[taxi]>=0?static_cast<int>(global[solved.selected[taxi]]):-1;
        }
        if (full) {best.complete=solved.complete;best.root_upper=solved.root_upper;best.timed_out=solved.timed_out;break;}
    }
    best.timed_out|=std::chrono::steady_clock::now()>=deadline;
    return best;
}
}
