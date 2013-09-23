#ifndef WISSBI_METRIC_REPORTER_HPP_
#define WISSBI_METRIC_REPORTER_HPP_

#include "filter_metric.hpp"
#include "msg_filter.hpp"
#include "io_policy/tee.hpp"
#include "io_policy/sysv_mq.hpp"
#include "io_policy/string.hpp"
#include <list>
#include <sstream>

namespace wissbi {

class MetricReporter {
  typedef MsgFilter<io_policy::String, io_policy::Tee<io_policy::SysvMq> >
      InputFilter;
  typedef std::list<FilterMetric> MetricList;

public:
  MetricReporter(MetricList &metric_list, InputFilter &input_filter,
                 const std::string &direction)
      : metric_list_(metric_list), input_filter_(input_filter),
        direction_(direction) {}

  ~MetricReporter() {}

  void Report() {
    for (FilterMetric &metric : metric_list_) {
      unsigned int last_processed = metric.last_processed.exchange(0);
      if (!last_processed) {
        continue;
      }
      std::ostringstream oss;
      oss << metric.name << "," << direction_ << "," << last_processed;
      input_filter_.Send(oss.str());
      input_filter_.Filter();
    }
  }

private:
  MetricList &metric_list_;
  InputFilter &input_filter_;
  std::string direction_;
};

}

#endif // WISSBI_METRIC_REPORTER_HPP_
