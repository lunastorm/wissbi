#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "metric_reporter.hpp"

using namespace std;
using namespace wissbi;

class MockInputFilter : public MsgFilter<io_policy::String, io_policy::Tee<io_policy::SysvMq>> {
    public:
    MOCK_METHOD1(Send, void(const string& msg));
    MOCK_METHOD0(Filter, bool());
};

class MetricReporterTest : public ::testing::Test {
    protected:
    virtual void SetUp() {
    }

    virtual void TearDown() {
    }
};

TEST_F(MetricReporterTest, SingleMetric) {
    map<string, FilterMetric> metric_map;
    MockInputFilter mocked_input;
    MetricReporter reporter(metric_map, mocked_input, "enqueue");

    {
        ::testing::InSequence dummy;
        EXPECT_CALL(mocked_input, Send("foo/out,enqueue,5566"));
        EXPECT_CALL(mocked_input, Filter()).WillOnce(::testing::Return(true));
    }

    FilterMetric& metric = metric_map["foo/out"];
    metric.last_processed += 5566;
    reporter.Report();
}

TEST_F(MetricReporterTest, Direction) {
    map<string, FilterMetric> metric_map;
    MockInputFilter mocked_input;
    MetricReporter reporter(metric_map, mocked_input, "dequeue");

    {
        ::testing::InSequence dummy;
        EXPECT_CALL(mocked_input, Send("foo/out,dequeue,5566"));
        EXPECT_CALL(mocked_input, Filter()).WillOnce(::testing::Return(true));
    }

    FilterMetric& metric = metric_map["foo/out"];
    metric.last_processed += 5566;
    reporter.Report();
}

TEST_F(MetricReporterTest, MetricReset) {
    map<string, FilterMetric> metric_map;
    MockInputFilter mocked_input;
    MetricReporter reporter(metric_map, mocked_input, "enqueue");

    {
        ::testing::InSequence dummy;
        EXPECT_CALL(mocked_input, Send("foo/out,enqueue,5566"));
        EXPECT_CALL(mocked_input, Filter()).WillOnce(::testing::Return(true));
        EXPECT_CALL(mocked_input, Send("foo/out,enqueue,1234"));
        EXPECT_CALL(mocked_input, Filter()).WillOnce(::testing::Return(true));
    }

    FilterMetric& metric = metric_map["foo/out"];
    metric.last_processed += 5566;
    reporter.Report();
    metric.last_processed += 1234;
    reporter.Report();
}

TEST_F(MetricReporterTest, IgnoreZero) {
    map<string, FilterMetric> metric_map;
    MockInputFilter mocked_input;
    MetricReporter reporter(metric_map, mocked_input, "enqueue");

    {
        ::testing::InSequence dummy;
        EXPECT_CALL(mocked_input, Send("foo/out,enqueue,5566"));
        EXPECT_CALL(mocked_input, Filter()).WillOnce(::testing::Return(true));
        EXPECT_CALL(mocked_input, Send("foo/out,enqueue,1234"));
        EXPECT_CALL(mocked_input, Filter()).WillOnce(::testing::Return(true));
    }

    FilterMetric& metric = metric_map["foo/out"];
    metric.last_processed += 5566;
    reporter.Report();
    reporter.Report();
    metric.last_processed += 1234;
    reporter.Report();
}

TEST_F(MetricReporterTest, MultipleMetrics) {
    map<string, FilterMetric> metric_map;
    MockInputFilter mocked_input;
    MetricReporter reporter(metric_map, mocked_input, "enqueue");

    {
        ::testing::InSequence dummy;
        EXPECT_CALL(mocked_input, Send("bar,enqueue,1234"));
        EXPECT_CALL(mocked_input, Filter()).WillOnce(::testing::Return(true));
        EXPECT_CALL(mocked_input, Send("foo,enqueue,5566"));
        EXPECT_CALL(mocked_input, Filter()).WillOnce(::testing::Return(true));
        EXPECT_CALL(mocked_input, Send("zoo,enqueue,12345"));
        EXPECT_CALL(mocked_input, Filter()).WillOnce(::testing::Return(true));
    }

    FilterMetric& bar_metric = metric_map["bar"];
    bar_metric.last_processed += 1234;
    FilterMetric& foo_metric = metric_map["foo"];
    foo_metric.last_processed += 5566;
    FilterMetric& zoo_metric = metric_map["zoo"];

    reporter.Report();
    reporter.Report();
    zoo_metric.last_processed += 12345;
    reporter.Report();
}

