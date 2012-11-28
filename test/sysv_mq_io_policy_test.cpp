#include "gtest/gtest.h"
#include "io_policy/sysv_mq.hpp"
#include <string.h>

using wissbi::MsgBuf;
using wissbi::io_policy::SysvMq;
using namespace std;

class SysvMqIOPolicyTest : public ::testing::Test {
    protected:
    virtual void SetUp() {
    }

    virtual void TearDown() {
    }
};

TEST_F(SysvMqIOPolicyTest, Constructor) {
    SysvMq mq;
    EXPECT_GE(mq.mqid(), 0);

    ostringstream oss;
    oss << "ipcs -q | grep " << mq.mqid();
    EXPECT_EQ(0, system(oss.str().c_str()));
}

TEST_F(SysvMqIOPolicyTest, Destructor) {
    int mqid;
    {
        SysvMq mq;
        EXPECT_GE(mq.mqid(), 0);
        mqid = mq.mqid();
        ostringstream oss;
        oss << "ipcs -q | grep " << mq.mqid();
        EXPECT_EQ(0, system(oss.str().c_str()));
    }
    ostringstream oss;
    oss << "ipcs -q | grep " << mqid;
    EXPECT_NE(0, system(oss.str().c_str()));
}

TEST_F(SysvMqIOPolicyTest, PutMsg) {
    SysvMq mq;
    EXPECT_GE(mq.mqid(), 0);

    MsgBuf msg_buf;
    string msg("Hello World");
    strncpy(msg_buf.buf, msg.c_str(), msg.length());
    msg_buf.len = msg.length();

    EXPECT_TRUE(mq.Put(msg_buf));
    ssize_t n = msgrcv(mq.mqid(), &msg_buf, wissbi::MAX_MSG_SIZE, 0, 0);
    EXPECT_EQ(msg.length(), n);
    EXPECT_STREQ(msg.c_str(), string(msg_buf.buf, n).c_str());
}

TEST_F(SysvMqIOPolicyTest, GetMsg) {
    SysvMq mq;
    EXPECT_GE(mq.mqid(), 0);

    MsgBuf msg_buf;
    string msg("Hello World");
    msg_buf.mtype = 1;
    strncpy(msg_buf.buf, msg.c_str(), msg.length());
    msgsnd(mq.mqid(), &msg_buf, msg.length(), 0);

    MsgBuf new_buf;
    EXPECT_TRUE(mq.Get(&new_buf));
    EXPECT_EQ(msg.length(), new_buf.len);
    EXPECT_STREQ(msg.c_str(), string(new_buf.buf, new_buf.len).c_str());
}

