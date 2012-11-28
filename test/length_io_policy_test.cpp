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

