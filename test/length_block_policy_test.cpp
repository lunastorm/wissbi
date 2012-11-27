#include "gtest/gtest.h"
#include "block_policy/length.hpp"

class LengthBlockPolicyTest : public ::testing::Test, public wissbi::block_policy::Length {
    protected:
    virtual void SetUp() {
    }

    virtual void TearDown() {
    }
};

TEST_F(LengthBlockPolicyTest, Empty) {
    wissbi::MsgBuf msg_buf;
    std::istringstream iss;

    EXPECT_FALSE(wissbi::block_policy::Length::Read(iss, &msg_buf));
    EXPECT_EQ(0, msg_buf.len);
}

TEST_F(LengthBlockPolicyTest, OneMsg) {
    wissbi::MsgBuf msg_buf;
    std::string msg("Hello World");
    std::ostringstream oss;
    oss << msg.length() << " " << msg;
    std::istringstream iss(oss.str());

    EXPECT_TRUE(wissbi::block_policy::Length::Read(iss, &msg_buf));
    EXPECT_EQ(msg.length(), msg_buf.len);
    EXPECT_STREQ(msg.c_str(), std::string(msg_buf.buf, msg_buf.len).c_str());

    EXPECT_FALSE(wissbi::block_policy::Length::Read(iss, &msg_buf));
    EXPECT_EQ(0, msg_buf.len);
}

TEST_F(LengthBlockPolicyTest, MoreMsg) {
    wissbi::MsgBuf msg_buf;
    std::string msg1("Hello World");
    std::string msg2(",Foo456,");
    std::string msg3(",123Bar,");
    std::ostringstream oss;
    oss << msg1.length() << "," << msg1;
    oss << msg2.length() << "," << msg2;
    oss << msg3.length() << "," << msg3;
    std::istringstream iss(oss.str());

    EXPECT_TRUE(wissbi::block_policy::Length::Read(iss, &msg_buf));
    EXPECT_EQ(msg1.length(), msg_buf.len);
    EXPECT_STREQ(msg1.c_str(), std::string(msg_buf.buf, msg_buf.len).c_str());

    EXPECT_TRUE(wissbi::block_policy::Length::Read(iss, &msg_buf));
    EXPECT_EQ(msg2.length(), msg_buf.len);
    EXPECT_STREQ(msg2.c_str(), std::string(msg_buf.buf, msg_buf.len).c_str());

    EXPECT_TRUE(wissbi::block_policy::Length::Read(iss, &msg_buf));
    EXPECT_EQ(msg3.length(), msg_buf.len);
    EXPECT_STREQ(msg3.c_str(), std::string(msg_buf.buf, msg_buf.len).c_str());

    EXPECT_FALSE(wissbi::block_policy::Length::Read(iss, &msg_buf));
    EXPECT_EQ(0, msg_buf.len);
}

