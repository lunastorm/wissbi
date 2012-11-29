#include "gtest/gtest.h"
#include "io_policy/udt.hpp"
#include <arpa/inet.h>
#include <thread>

using wissbi::MsgBuf;
using wissbi::io_policy::Udt;
using namespace std;

class UdtIOPolicyTest : public ::testing::Test, public Udt {
    protected:
    virtual void SetUp() {
        UDT::startup();
        serv_ = UDT::socket(AF_INET, SOCK_DGRAM, 0);
        ASSERT_NE(UDT::ERROR, serv_);

        serv_addr_.sin_family = AF_INET;
        serv_addr_.sin_port = htons(0);
        serv_addr_.sin_addr.s_addr = inet_addr("127.0.0.1");
        memset(&(serv_addr_.sin_zero), '\0', 8);
        ASSERT_NE(UDT::ERROR, UDT::bind(serv_, (sockaddr*)&serv_addr_, sizeof(serv_addr_)));
        ASSERT_NE(UDT::ERROR, UDT::listen(serv_, 10));
        int len;
        UDT::getsockname(serv_, (sockaddr*)&serv_addr_, &len);
        thread([this]{
            sockaddr_in their_addr;
            int len;
            sock_ = UDT::accept(serv_, (sockaddr*)&their_addr, &len);
        }).detach();
    }

    virtual void TearDown() {
        UDT::close(serv_);
        UDT::close(sock_);
        UDT::cleanup();
    }

    UDTSOCKET serv_;
    UDTSOCKET sock_;
    sockaddr_in serv_addr_;
};

TEST_F(UdtIOPolicyTest, PutEmpty) {
    MsgBuf msg_buf;
    msg_buf.len = 0;
    Connect(reinterpret_cast<sockaddr*>(&serv_addr_));
    EXPECT_FALSE(Put(msg_buf));
}

TEST_F(UdtIOPolicyTest, PutOne) {
    string msg("Hello World");
    MsgBuf msg_buf;
    strncpy(msg_buf.buf, msg.c_str(), msg.length());
    msg_buf.len = msg.length();

    MsgBuf new_buf;
    Connect(reinterpret_cast<sockaddr*>(&serv_addr_));
    EXPECT_TRUE(Put(msg_buf));
    int res = UDT::recvmsg(sock_, new_buf.buf, wissbi::MAX_MSG_SIZE);
    ASSERT_NE(UDT::ERROR, res);
    EXPECT_STREQ(msg.c_str(), string(new_buf.buf, res).c_str());
}

TEST_F(UdtIOPolicyTest, GetOne) {
    string msg("Hello World");
    MsgBuf msg_buf;
    strncpy(msg_buf.buf, msg.c_str(), msg.length());
    msg_buf.len = msg.length();

    MsgBuf new_buf;
    Connect(reinterpret_cast<sockaddr*>(&serv_addr_));
    ASSERT_NE(UDT::ERROR, UDT::sendmsg(sock_, msg_buf.buf, msg_buf.len));
    EXPECT_TRUE(Get(&new_buf));
    EXPECT_STREQ(msg.c_str(), string(new_buf.buf, new_buf.len).c_str());
}


