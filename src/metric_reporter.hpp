#ifndef WISSBI_METRIC_REPORTER_HPP_
#define WISSBI_METRIC_REPORTER_HPP_

#include "filter_metric.hpp"
#include "msg_filter.hpp"
#include "io_policy/tee.hpp"
#include "io_policy/sysv_mq.hpp"
#include "io_policy/string.hpp"
#include <map>
#include <sstream>

namespace wissbi {

class MetricReporter {
    typedef MsgFilter<io_policy::String, io_policy::Tee<io_policy::SysvMq>> InputFilter;
    typedef std::map<std::string, FilterMetric> MetricMap;

    public:
    MetricReporter(MetricMap& metric_map, InputFilter& input_filter, const std::string& direction) :
        metric_map_(metric_map), input_filter_(input_filter), direction_(direction)
    {
    }

    ~MetricReporter()
    {
    }

    void Report()
    {
        for(auto iter = metric_map_.begin(); iter != metric_map_.end() ; ++iter) {
            unsigned int last_processed = metric_map_[iter->first].last_processed.exchange(0);
            if(!last_processed) {
                continue;
            }
            std::ostringstream oss;
            oss << iter->first << "," << direction_ << "," << last_processed;
            input_filter_.Send(oss.str());
            input_filter_.Filter();
        }
    }

    private:
    MetricMap& metric_map_;
    InputFilter& input_filter_;
    std::string direction_;
};

}

#endif  // WISSBI_METRIC_REPORTER_HPP_
