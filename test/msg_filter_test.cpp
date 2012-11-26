#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "line_block_policy.hpp"
#include "msg_filter.hpp"
#include <iostream>

using ::testing::_;
using ::testing::Return;

class MockPolicy {
    public:
    MOCK_METHOD2(Read, bool(std::istream& is, wissbi::MsgBuf* msg_buf));
};

class MsgFilterTest : public ::testing::Test {
    protected:
    virtual void SetUp() {
    }

    virtual void TearDown() {
    }
};

TEST_F(MsgFilterTest, ReadFail) {
    wissbi::MsgFilter<MockPolicy> msg_filter(std::cin);

    EXPECT_CALL(msg_filter, Read(_, NULL)).WillOnce(Return(false));
    EXPECT_FALSE(msg_filter.Filter());
}

