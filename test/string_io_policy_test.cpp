#include "gtest/gtest.h"
#include "io_policy/string.hpp"

using wissbi::MsgBuf;
using wissbi::io_policy::String;
using namespace std;

class StringIOPolicyTest : public ::testing::Test, public String {
    protected:
    virtual void SetUp() {
    }

    virtual void TearDown() {
    }
};

TEST_F(StringIOPolicyTest, SendOneMsg) {
    string msg(" hello world\n foobar ");
    MsgBuf msg_buf;
    Send(msg);
    EXPECT_TRUE(Get(&msg_buf));
    EXPECT_STREQ(msg.c_str(), string(msg_buf.buf, msg_buf.len).c_str());
}

TEST_F(StringIOPolicyTest, SendManyMsgs) {
    string msg(" hello world\n foobar ");
    MsgBuf msg_buf;
    Send(msg);
    EXPECT_TRUE(Get(&msg_buf));
    EXPECT_STREQ(msg.c_str(), string(msg_buf.buf, msg_buf.len).c_str());

    string msg2("123\n456\n789\n123456789");
    Send(msg2);
    EXPECT_TRUE(Get(&msg_buf));
    EXPECT_STREQ(msg2.c_str(), string(msg_buf.buf, msg_buf.len).c_str());

    string msg3("askljdhfljakshdfjsklhgjkdshgjksdgh");
    Send(msg3);
    EXPECT_TRUE(Get(&msg_buf));
    EXPECT_STREQ(msg3.c_str(), string(msg_buf.buf, msg_buf.len).c_str());
}

TEST_F(StringIOPolicyTest, ReceiveOneMsg) {
    string msg(" hello world\n foobar ");
    MsgBuf msg_buf;
    strncpy(msg_buf.buf, msg.c_str(), msg.length());
    msg_buf.len = msg.length();

    EXPECT_TRUE(Put(msg_buf));
    EXPECT_STREQ(msg.c_str(), Receive().c_str());
}
