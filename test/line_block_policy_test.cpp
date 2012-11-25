#include "gtest/gtest.h"
#include "line_block_policy.hpp"
#include "msg_buf.hpp"

#include <sstream>

class LineBlockPolicyTest : public ::testing::Test, public wissbi::LineBlockPolicy {
    protected:
    virtual void SetUp() {
    }

    virtual void TearDown() {
    }
};

TEST_F(LineBlockPolicyTest, Empty) {
    wissbi::MsgBuf msg_buf;
    std::istringstream iss;

    EXPECT_FALSE(wissbi::LineBlockPolicy::Read(iss, &msg_buf));
    EXPECT_EQ(0, msg_buf.len);
}

TEST_F(LineBlockPolicyTest, OneMsg) {
    wissbi::MsgBuf msg_buf;
    std::string input("hello world");
    std::istringstream iss(input);

    EXPECT_TRUE(wissbi::LineBlockPolicy::Read(iss, &msg_buf));
    EXPECT_EQ(input.length(), msg_buf.len);
    EXPECT_STREQ(input.c_str(), std::string(msg_buf.buf, msg_buf.len).c_str());

    EXPECT_FALSE(wissbi::LineBlockPolicy::Read(iss, &msg_buf));
    EXPECT_EQ(0, msg_buf.len);
}

TEST_F(LineBlockPolicyTest, MoreMsg) {
    wissbi::MsgBuf msg_buf;
    std::string input1("hello world");
    std::string input2("foo");
    std::string input3("bar");
    std::istringstream iss(input1 + "\n" + input2 + "\n" + input3);

    EXPECT_TRUE(wissbi::LineBlockPolicy::Read(iss, &msg_buf));
    EXPECT_EQ(input1.length(), msg_buf.len);
    EXPECT_STREQ(input1.c_str(), std::string(msg_buf.buf, msg_buf.len).c_str());

    EXPECT_TRUE(wissbi::LineBlockPolicy::Read(iss, &msg_buf));
    EXPECT_EQ(input2.length(), msg_buf.len);
    EXPECT_STREQ(input2.c_str(), std::string(msg_buf.buf, msg_buf.len).c_str());

    EXPECT_TRUE(wissbi::LineBlockPolicy::Read(iss, &msg_buf));
    EXPECT_EQ(input3.length(), msg_buf.len);
    EXPECT_STREQ(input3.c_str(), std::string(msg_buf.buf, msg_buf.len).c_str());

    EXPECT_FALSE(wissbi::LineBlockPolicy::Read(iss, &msg_buf));
    EXPECT_EQ(0, msg_buf.len);
}
