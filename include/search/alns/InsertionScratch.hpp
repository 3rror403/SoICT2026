#pragma once

#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

namespace soict::alns {
// Sequential insertion scans share one fixed allocation for the whole solve.
// A route contains at most two service nodes per request. The extra two slots
// cover every insertion gap and the parcel suffix's infinity sentinel.
class InsertionScratch {
public:
    struct Buffers {
        double* pickup;
        double* delivery;
        double* adjacent;
        double* suffix;
    };

    explicit InsertionScratch(std::size_t requests)
        : requests_(requests), stride_(checked_stride(requests)), storage_(4 * stride_) {}

    [[nodiscard]] std::size_t request_count() const { return requests_; }
    [[nodiscard]] std::size_t max_route_nodes() const { return stride_ - 2; }
    [[nodiscard]] std::size_t bytes() const { return storage_.size() * sizeof(double); }

    [[nodiscard]] Buffers buffers(std::size_t route_nodes) {
        if (route_nodes > max_route_nodes())
            throw std::out_of_range("insertion route exceeds fixed scratch capacity");
        auto* data = storage_.data();
        return {data, data + stride_, data + 2 * stride_, data + 3 * stride_};
    }

private:
    static std::size_t checked_stride(std::size_t requests) {
        constexpr auto limit = std::numeric_limits<std::size_t>::max() / sizeof(double) / 4;
        if (requests > (limit - 2) / 2)
            throw std::length_error("insertion scratch size overflow");
        return 2 * requests + 2;
    }
    std::size_t requests_, stride_;
    // Never resized: scan results own their values and do not retain pointers.
    std::vector<double> storage_;
};
} // namespace soict::alns
