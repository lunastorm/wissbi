#include "gtest/gtest.h"
#include "io_policy/line.hpp"

using wissbi::MsgBuf;
using wissbi::io_policy::Line;
using namespace std;

class LineIOPolicyTest : public ::testing::Test , public Line {
    protected:
    virtual void SetUp() {
        set_istream(&iss);
        set_ostream(&oss);
    }

    virtual void TearDown() {
    }

    istringstream iss;
    ostringstream oss;
};

TEST_F(LineIOPolicyTest, PutOneMsg) {
    MsgBuf msg_buf;
    string msg("foo bar,12345 Hello World");
    strncpy(msg_buf.buf, msg.c_str(), msg.length());
    msg_buf.len = msg.length();

    EXPECT_TRUE(Put(msg_buf));
    EXPECT_STREQ((msg + "\n").c_str(), oss.str().c_str());
}

TEST_F(LineIOPolicyTest, PutMoreMsg) {
    MsgBuf msg_buf;
    string msg1("foo bar,12345 Hello World");
    string msg2("1234567");
    string msg3("1234567asdflkjadf");
    strncpy(msg_buf.buf, msg1.c_str(), msg1.length());
    msg_buf.len = msg1.length();

    EXPECT_TRUE(Put(msg_buf));

    strncpy(msg_buf.buf, msg2.c_str(), msg2.length());
    msg_buf.len = msg2.length();

    EXPECT_TRUE(Put(msg_buf));

    strncpy(msg_buf.buf, msg3.c_str(), msg3.length());
    msg_buf.len = msg3.length();

    EXPECT_TRUE(Put(msg_buf));
    EXPECT_STREQ((msg1 + "\n" + msg2 + "\n" + msg3 + "\n").c_str(), oss.str().c_str());
}

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

