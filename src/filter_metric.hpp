#ifndef WISSBI_FILTER_METRIC_HPP_
#define WISSBI_FILTER_METRIC_HPP_

#include <atomic>

namespace wissbi {

typedef struct FilterMetric {
    FilterMetric() :
        name(""),
        last_processed(0),
        total_processed(0) {}

    explicit FilterMetric(const std::string& in_name) :
        name(in_name),
        last_processed(0),
        total_processed(0) {}

    FilterMetric(const FilterMetric& rhs) :
        name(rhs.name),
        last_processed(rhs.last_processed.load()),
        total_processed(rhs.total_processed.load()) {}

    std::string name;
    std::atomic_uint last_processed;
    std::atomic_ullong total_processed;
} FilterMetric;

}

#endif  // WISSBI_FILTER_METRIC_HPP_
