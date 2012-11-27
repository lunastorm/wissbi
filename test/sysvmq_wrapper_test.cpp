#include "gtest/gtest.h"
#include "sysvmq_wrapper.hpp"
#include <string.h>

class SysVMqWrapperTest : public ::testing::Test {
    protected:
    virtual void SetUp() {
    }

    virtual void TearDown() {
    }
};

TEST_F(SysVMqWrapperTest, Constructor) {
    wissbi::SysVMqWrapper mq;
    EXPECT_GE(mq.mqid(), 0);

    std::ostringstream oss;
    oss << "ipcs -q | grep " << mq.mqid();
    EXPECT_EQ(0, system(oss.str().c_str()));
}

TEST_F(SysVMqWrapperTest, Destructor) {
    int mqid;
    {
        wissbi::SysVMqWrapper mq;
        EXPECT_GE(mq.mqid(), 0);
        mqid = mq.mqid();
        std::ostringstream oss;
        oss << "ipcs -q | grep " << mq.mqid();
        EXPECT_EQ(0, system(oss.str().c_str()));
    }
    std::ostringstream oss;
    oss << "ipcs -q | grep " << mqid;
    EXPECT_NE(0, system(oss.str().c_str()));
}

TEST_F(SysVMqWrapperTest, PutMsg) {
    wissbi::SysVMqWrapper mq;
    EXPECT_GE(mq.mqid(), 0);

    wissbi::MsgBuf msg_buf;
    std::string msg("Hello World");
    strncpy(msg_buf.buf, msg.c_str(), msg.length());
    msg_buf.len = msg.length();

    EXPECT_TRUE(mq.Put(msg_buf));
    ssize_t n = msgrcv(mq.mqid(), &msg_buf, wissbi::MAX_MSG_SIZE, 0, 0);
    EXPECT_EQ(msg.length(), n);
    EXPECT_STREQ(msg.c_str(), std::string(msg_buf.buf, n).c_str());
}

TEST_F(SysVMqWrapperTest, GetMsg) {
    wissbi::SysVMqWrapper mq;
    EXPECT_GE(mq.mqid(), 0);

    wissbi::MsgBuf msg_buf;
    std::string msg("Hello World");
    msg_buf.mtype = 1;
    strncpy(msg_buf.buf, msg.c_str(), msg.length());
    msgsnd(mq.mqid(), &msg_buf, msg.length(), 0);

    wissbi::MsgBuf new_buf;
    EXPECT_TRUE(mq.Get(&new_buf));
    EXPECT_EQ(msg.length(), new_buf.len);
    EXPECT_STREQ(msg.c_str(), std::string(new_buf.buf, new_buf.len).c_str());
}

