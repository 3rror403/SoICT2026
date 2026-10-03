#pragma once
#include "search/alns/operators/LocalSearch/Move.hpp"
#include "search/alns/operators/Repair/InsertionSearch.hpp"
#include <queue>

namespace soict::alns {
struct ReplacementChoice {
    std::size_t outgoing;
    InsertionChoice insertion;
    double gain;
};

class ReplaceRequest final : public Move {
public:
    // The original scan remains available for differential validation.
    explicit ReplaceRequest(bool use_smd = true) : use_smd_(use_smd) {}
    std::string_view name() const override { return "replace-request"; }
    bool improve(SearchState&) const override;
    [[nodiscard]] std::optional<ReplacementChoice> best_choice(const SearchState&) const;
private:
    bool use_smd_;
    [[nodiscard]] std::optional<ReplacementChoice> best_choice_reference(const SearchState&) const;
    struct Entry {
        bool valid = false;
        double threshold = 0;
        std::optional<ReplacementChoice> choice;
    };
    struct Row {
        std::uint64_t route_version = 0;
        std::vector<Entry> entries;
        std::uint64_t reduced_version = 0;
        RouteCache reduced;
        double removal_gain = 0;
    };
    mutable std::vector<Row> rows_;

    struct Descriptor {
        std::uint64_t generation = 0;
        std::optional<ReplacementChoice> choice;
    };
    struct QueuedChoice {
        ReplacementChoice choice;
        std::uint64_t route_version, generation;
    };
    struct LowerPriority {
        bool operator()(const QueuedChoice&, const QueuedChoice&) const;
    };
    using Queue = std::priority_queue<QueuedChoice, std::vector<QueuedChoice>, LowerPriority>;
    mutable const Instance* smd_instance_ = nullptr;
    mutable std::vector<std::uint64_t> route_versions_;
    mutable std::vector<bool> unserved_;
    mutable std::vector<std::vector<Descriptor>> descriptors_; // [taxi][incoming]
    mutable Queue queue_;
    void refresh_descriptors(const SearchState&, std::size_t taxi,
        const std::vector<std::size_t>& outgoing,
        const std::vector<std::size_t>& incoming) const;
    void synchronize(const SearchState&) const;
    void discard_stale() const;
    void enqueue(std::size_t taxi, std::size_t incoming) const;
    void compact_queue() const;
};
} // namespace soict::alns
