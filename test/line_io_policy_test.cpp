#include "gtest/gtest.h"
#include "io_policy/line.hpp"

using wissbi::MsgBuf;
using wissbi::io_policy::Line;
using namespace std;

class LineIOPolicyTest : public ::testing::Test , public wissbi::io_policy::Line {
    protected:
    virtual void SetUp() {
        set_istream(&iss);
    }

    virtual void TearDown() {
    }

    istringstream iss;
};

TEST_F(LineIOPolicyTest, GetEmpty) {
    MsgBuf msg_buf;
    EXPECT_FALSE(Get(&msg_buf));
    EXPECT_EQ(0, msg_buf.len);
}

TEST_F(LineIOPolicyTest, GetOneMsg) {
    MsgBuf msg_buf;
    string msg("hello world");
    iss.str(msg);

    EXPECT_TRUE(Get(&msg_buf));
    EXPECT_EQ(msg.length(), msg_buf.len);
    EXPECT_STREQ(msg.c_str(), string(msg_buf.buf, msg_buf.len).c_str());

    EXPECT_FALSE(Get(&msg_buf));
    EXPECT_EQ(0, msg_buf.len);
}

TEST_F(LineIOPolicyTest, GetMoreMsg) {
    MsgBuf msg_buf;
    string msg1("hello world");
    string msg2("foo");
    string msg3("bar");
    iss.str(msg1 + "\n" + msg2 + "\n" + msg3);

    EXPECT_TRUE(Get(&msg_buf));
    EXPECT_EQ(msg1.length(), msg_buf.len);
    EXPECT_STREQ(msg1.c_str(), string(msg_buf.buf, msg_buf.len).c_str());

    EXPECT_TRUE(Get(&msg_buf));
    EXPECT_EQ(msg2.length(), msg_buf.len);
    EXPECT_STREQ(msg2.c_str(), string(msg_buf.buf, msg_buf.len).c_str());

    EXPECT_TRUE(Get(&msg_buf));
    EXPECT_EQ(msg3.length(), msg_buf.len);
    EXPECT_STREQ(msg3.c_str(), string(msg_buf.buf, msg_buf.len).c_str());

    EXPECT_FALSE(Get(&msg_buf));
    EXPECT_EQ(0, msg_buf.len);
}

