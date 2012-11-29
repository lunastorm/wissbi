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

TEST_F(SysvMqIOPolicyTest, OneConstructor) {
    SysvMq mq;
    EXPECT_GE(mq.mqid(), 0);

    ostringstream oss;
    oss << "ipcs -q | grep " << mq.mqid();
    EXPECT_EQ(0, system(oss.str().c_str()));

    ostringstream oss2;
    oss2 << "ls " << mq.key_file();
    EXPECT_EQ(0, system(oss2.str().c_str()));
}

TEST_F(SysvMqIOPolicyTest, OneDestructor) {
    int mqid;
    string key_file;
    {
        SysvMq mq;
        EXPECT_GE(mq.mqid(), 0);
        mqid = mq.mqid();
        key_file = mq.key_file();
        ostringstream oss;
        oss << "ipcs -q | grep " << mq.mqid();
        EXPECT_EQ(0, system(oss.str().c_str()));
    }
    ostringstream oss;
    oss << "ipcs -q | grep " << mqid;
    EXPECT_NE(0, system(oss.str().c_str()));

    ostringstream oss2;
    oss2 << "ls " << key_file;
    EXPECT_NE(0, system(oss2.str().c_str()));
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

TEST_F(SysvMqIOPolicyTest, MultiInstanceConstructDestruct) {
    int mqid1;
    int mqid2;
    string key_file;
    {
        SysvMq mq1;
        EXPECT_GE(mq1.mqid(), 0);
        mqid1 = mq1.mqid();
        key_file = mq1.key_file();
        ostringstream oss;
        oss << "ipcs -q | grep " << mq1.mqid();
        EXPECT_EQ(0, system(oss.str().c_str()));

        SysvMq mq2;
        EXPECT_GE(mq2.mqid(), 0);
        mqid2 = mq2.mqid();
        ostringstream oss2;
        oss2 << "ipcs -q | grep " << mq2.mqid();
        EXPECT_EQ(0, system(oss2.str().c_str()));
    }
    ostringstream oss1;
    oss1 << "ipcs -q | grep " << mqid1;
    EXPECT_NE(0, system(oss1.str().c_str()));

    ostringstream oss2;
    oss2 << "ipcs -q | grep " << mqid2;
    EXPECT_NE(0, system(oss2.str().c_str()));

    ostringstream oss3;
    oss3 << "ls " << key_file;
    EXPECT_NE(0, system(oss3.str().c_str()));
}

TEST_F(SysvMqIOPolicyTest, MultiInstancePutGet) {
    SysvMq mq1;
    EXPECT_GE(mq1.mqid(), 0);
    SysvMq mq2;
    EXPECT_GE(mq2.mqid(), 0);

    MsgBuf msg_buf;
    string msg("Hello World");
    strncpy(msg_buf.buf, msg.c_str(), msg.length());
    msg_buf.len = msg.length();

    EXPECT_TRUE(mq1.Put(msg_buf));
    MsgBuf new_buf;
    EXPECT_TRUE(mq2.Get(&new_buf));
    EXPECT_STREQ(msg.c_str(), string(new_buf.buf, new_buf.len).c_str());
}

TEST_F(SysvMqIOPolicyTest, GetCount) {
    SysvMq mq;
    EXPECT_GE(mq.mqid(), 0);
    EXPECT_EQ(0, mq.GetCount());

    MsgBuf msg_buf;
    string msg("Hello World");
    strncpy(msg_buf.buf, msg.c_str(), msg.length());
    msg_buf.len = msg.length();

    EXPECT_TRUE(mq.Put(msg_buf));
    EXPECT_EQ(1, mq.GetCount());

    EXPECT_TRUE(mq.Put(msg_buf));
    EXPECT_EQ(2, mq.GetCount());

    EXPECT_TRUE(mq.Get(&msg_buf));
    EXPECT_EQ(1, mq.GetCount());

    EXPECT_TRUE(mq.Get(&msg_buf));
    EXPECT_EQ(0, mq.GetCount());
}
