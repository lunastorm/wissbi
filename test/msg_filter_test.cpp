#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "msg_filter.hpp"

using wissbi::MsgBuf;
using wissbi::MsgFilter;
using ::testing::_;
using ::testing::Return;
using namespace std;

class MockInputPolicy {
    public:
    MOCK_METHOD1(Get, bool(MsgBuf *msg_buf));
    MOCK_METHOD1(Put, bool(const MsgBuf &msg_buf));
};

class MockOutputPolicy {
    public:
    MOCK_METHOD1(Get, bool(MsgBuf *msg_buf));
    MOCK_METHOD1(Put, bool(const MsgBuf &msg_buf));
};

class MsgFilterTest : public ::testing::Test {
    protected:
    virtual void SetUp() {
    }

    virtual void TearDown() {
    }
};

TEST_F(MsgFilterTest, ReadFail) {
    MsgFilter<MockInputPolicy, MockOutputPolicy> filter;
    EXPECT_CALL(static_cast<MockInputPolicy&>(filter), Get(_)).WillOnce(Return(false));
    EXPECT_CALL(static_cast<MockOutputPolicy&>(filter), Put(_)).Times(0);
    EXPECT_FALSE(filter.Filter());
}

TEST_F(MsgFilterTest, Filter) {
    MsgFilter<MockInputPolicy, MockOutputPolicy> filter;
    MsgBuf *buf_ptr;
    EXPECT_CALL(static_cast<MockInputPolicy&>(filter), Get(_)).WillOnce(Return(true));
    EXPECT_CALL(static_cast<MockOutputPolicy&>(filter), Put(_)).WillOnce(Return(true));
    filter.Filter();
}

TEST_F(MsgFilterTest, FilterLoop) {
    MsgFilter<MockInputPolicy, MockOutputPolicy> filter;
    MsgBuf *buf_ptr;
    EXPECT_CALL(static_cast<MockInputPolicy&>(filter), Get(_)).Times(4)
        .WillOnce(Return(true)).WillOnce(Return(true)).WillOnce(Return(true)).WillRepeatedly(Return(false));
    EXPECT_CALL(static_cast<MockOutputPolicy&>(filter), Put(_)).Times(3).WillRepeatedly(Return(true));
    filter.FilterLoop();
}

TEST_F(MsgFilterTest, FilterFuncFalse) {
    bool called = false;
    MsgFilter<MockInputPolicy, MockOutputPolicy> filter;
    filter.set_filter_func([&called](MsgBuf& msg){
        called = true;
        return false;
    });

    MsgBuf *buf_ptr;
    EXPECT_CALL(static_cast<MockInputPolicy&>(filter), Get(_)).WillOnce(Return(true));
    EXPECT_CALL(static_cast<MockOutputPolicy&>(filter), Put(_)).Times(0);
    EXPECT_FALSE(filter.Filter());
    EXPECT_TRUE(called);
}

TEST_F(MsgFilterTest, FilterFuncTrue) {
    bool called = false;
    MsgFilter<MockInputPolicy, MockOutputPolicy> filter;
    filter.set_filter_func([&called](MsgBuf& msg){
        called = true;
        return true;
    });

    MsgBuf *buf_ptr;
    EXPECT_CALL(static_cast<MockInputPolicy&>(filter), Get(_)).WillOnce(Return(true));
    EXPECT_CALL(static_cast<MockOutputPolicy&>(filter), Put(_)).WillOnce(Return(true));
    EXPECT_TRUE(filter.Filter());
    EXPECT_TRUE(called);
}

TEST_F(MsgFilterTest, PreFilterTrue) {
    bool called = false;
    MsgFilter<MockInputPolicy, MockOutputPolicy> filter;
    filter.set_pre_filter_func([&called](){
        called = true;
        return true;
    });

    MsgBuf *buf_ptr;
    EXPECT_CALL(static_cast<MockInputPolicy&>(filter), Get(_)).WillOnce(Return(true));
    EXPECT_CALL(static_cast<MockOutputPolicy&>(filter), Put(_)).WillOnce(Return(true));
    EXPECT_TRUE(filter.Filter());
    EXPECT_TRUE(called);
}

TEST_F(MsgFilterTest, PreFilterFalse) {
    bool called = false;
    MsgFilter<MockInputPolicy, MockOutputPolicy> filter;
    filter.set_pre_filter_func([&called](){
        called = true;
        return false;
    });

    MsgBuf *buf_ptr;
    EXPECT_FALSE(filter.Filter());
    EXPECT_TRUE(called);
}

TEST_F(MsgFilterTest, PostFilterTrue) {
    bool called = false;
    MsgFilter<MockInputPolicy, MockOutputPolicy> filter;
    filter.set_post_filter_func([&called](bool filter_result, MsgBuf& msg){
        called = true;
        return true;
    });

    MsgBuf *buf_ptr;
    EXPECT_CALL(static_cast<MockInputPolicy&>(filter), Get(_)).WillOnce(Return(true));
    EXPECT_CALL(static_cast<MockOutputPolicy&>(filter), Put(_)).WillOnce(Return(true));
    EXPECT_TRUE(filter.Filter());
    EXPECT_TRUE(called);
}

TEST_F(MsgFilterTest, PostFilterFalse) {
    bool called = false;
    MsgFilter<MockInputPolicy, MockOutputPolicy> filter;
    filter.set_post_filter_func([&called](bool filter_result, MsgBuf& msg){
        called = true;
        return false;
    });

    MsgBuf *buf_ptr;
    EXPECT_CALL(static_cast<MockInputPolicy&>(filter), Get(_)).WillOnce(Return(true));
    EXPECT_CALL(static_cast<MockOutputPolicy&>(filter), Put(_)).WillOnce(Return(true));
    EXPECT_FALSE(filter.Filter());
    EXPECT_TRUE(called);
}

TEST_F(MsgFilterTest, PostFilterGetFalseReturnTrue) {
    bool called = false;
    MsgFilter<MockInputPolicy, MockOutputPolicy> filter;
    filter.set_post_filter_func([&called](bool filter_result, MsgBuf& msg){
        called = true;
        if(!filter_result) {
            return true;
        }
        return false;
    });

    MsgBuf *buf_ptr;
    EXPECT_CALL(static_cast<MockInputPolicy&>(filter), Get(_)).WillOnce(Return(true));
    EXPECT_CALL(static_cast<MockOutputPolicy&>(filter), Put(_)).WillOnce(Return(false));
    EXPECT_TRUE(filter.Filter());
    EXPECT_TRUE(called);
}
