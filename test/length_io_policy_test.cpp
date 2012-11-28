#include "gtest/gtest.h"
#include "io_policy/length.hpp"

using wissbi::MsgBuf;
using wissbi::io_policy::Length;
using namespace std;

class LengthIOPolicyTest : public ::testing::Test, public Length {
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

TEST_F(LengthIOPolicyTest, PutEmptyMsg) {
    MsgBuf msg_buf;
    msg_buf.len = 0;
    EXPECT_FALSE(Put(msg_buf));
    EXPECT_EQ(0, oss.str().length());
}

TEST_F(LengthIOPolicyTest, PutOneMsg) {
    string msg(" 12345 Hello ");
    MsgBuf msg_buf;
    strncpy(msg_buf.buf, msg.c_str(), msg.length());
    msg_buf.len = msg.length();

    EXPECT_TRUE(Put(msg_buf));
    ostringstream oss_tmp;
    oss_tmp << msg.length() << " " << msg;
    EXPECT_STREQ(oss_tmp.str().c_str(), oss.str().c_str());
}

TEST_F(LengthIOPolicyTest, PutMoreMsg) {
    string msg1(" 12345 Hello ");
    string msg2("FooBar 12345 Hello ");
    string msg3(",,,123214jhakgdha,, ");

    MsgBuf msg_buf;
    strncpy(msg_buf.buf, msg1.c_str(), msg1.length());
    msg_buf.len = msg1.length();
    EXPECT_TRUE(Put(msg_buf));

    strncpy(msg_buf.buf, msg2.c_str(), msg2.length());
    msg_buf.len = msg2.length();
    EXPECT_TRUE(Put(msg_buf));

    strncpy(msg_buf.buf, msg3.c_str(), msg3.length());
    msg_buf.len = msg3.length();
    EXPECT_TRUE(Put(msg_buf));

    ostringstream oss_tmp;
    oss_tmp << msg1.length() << " " << msg1 << msg2.length() << " " << msg2 << msg3.length() << " " << msg3;
    EXPECT_STREQ(oss_tmp.str().c_str(), oss.str().c_str());
}

TEST_F(LengthIOPolicyTest, GetEmpty) {
    MsgBuf msg_buf;
    EXPECT_FALSE(Get(&msg_buf));
    EXPECT_EQ(0, msg_buf.len);
}

TEST_F(LengthIOPolicyTest, GetOneMsg) {
    string msg("Hello World");
    ostringstream oss_tmp;
    oss_tmp << msg.length() << " " << msg;
    iss.str(oss_tmp.str());

    MsgBuf msg_buf;
    EXPECT_TRUE(Get(&msg_buf));
    EXPECT_EQ(msg.length(), msg_buf.len);
    EXPECT_STREQ(msg.c_str(), string(msg_buf.buf, msg_buf.len).c_str());

    EXPECT_FALSE(Get(&msg_buf));
    EXPECT_EQ(0, msg_buf.len);
}

TEST_F(LengthIOPolicyTest, MoreMsg) {
    MsgBuf msg_buf;
    string msg1("Hello World");
    string msg2(" ,Foo456, ");
    string msg3(" ,123Bar, ");
    ostringstream oss_tmp;
    oss_tmp << msg1.length() << "," << msg1;
    oss_tmp << msg2.length() << "," << msg2;
    oss_tmp << msg3.length() << "," << msg3;
    iss.str(oss_tmp.str());

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

