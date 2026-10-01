#pragma once
#include "search/alns/operators/Destroy/DestroyOperator.hpp"

namespace soict::alns {
class TimeBucketRemoval final : public DestroyOperator {
public:
    std::string_view name() const override { return "time_bucket"; }
    std::vector<std::size_t> apply(SearchState&, std::size_t, std::mt19937_64&) const override;
};
} // namespace soict::alns
