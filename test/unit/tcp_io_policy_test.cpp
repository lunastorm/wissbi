#include "gtest/gtest.h"
#include "io_policy/tcp.hpp"
#include <sys/time.h>
#include <thread>

using wissbi::MsgBuf;
using wissbi::io_policy::TCP;
using namespace std;

class TCPIOPolicyTest : public ::testing::Test, public TCP {
    protected:
    virtual void SetUp() {
        sock_ = -1;
        serv_ = socket(AF_INET, SOCK_STREAM, 0);
        ASSERT_NE(-1, serv_);

        serv_addr_.sin_family = AF_INET;
        serv_addr_.sin_port = htons(0);
        serv_addr_.sin_addr.s_addr = inet_addr("127.0.0.1");
        memset(&(serv_addr_.sin_zero), '\0', 8);

        ASSERT_NE(-1, ::bind(serv_, reinterpret_cast<sockaddr*>(&serv_addr_), sizeof(serv_addr_)));
        ASSERT_NE(-1, listen(serv_, 10));
        socklen_t len = sizeof(serv_addr_);
        ASSERT_NE(-1, getsockname(serv_, reinterpret_cast<sockaddr*>(&serv_addr_), &len));

        thread([this]{
            sockaddr_in client_addr;
            socklen_t len = sizeof(sockaddr_in);
            sock_ = accept(serv_, reinterpret_cast<sockaddr*>(&client_addr), &len);
            struct timeval tv;
            tv.tv_sec = 1;
            tv.tv_usec = 0;
            setsockopt(sock_, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
        }).detach();
    }

    virtual void TearDown() {
        close(serv_);
        close(sock_);
    }

    bool accepted() {
        int retry_count = 0;
        while(sock_ == -1 && retry_count++ < 100) {
            this_thread::sleep_for(chrono::milliseconds(10));
        }
        return sock_ != -1;
    }

    int serv_;
    int sock_;
    sockaddr_in serv_addr_;
};

TEST_F(TCPIOPolicyTest, PutEmpty) {
    MsgBuf msg_buf;
    msg_buf.len = 0;
    Connect(reinterpret_cast<sockaddr*>(&serv_addr_));
    EXPECT_TRUE(accepted());
    EXPECT_FALSE(Put(msg_buf));
}

TEST_F(TCPIOPolicyTest, PutOne) {
    string msg("Hello World");
    MsgBuf msg_buf;
    strncpy(msg_buf.buf, msg.c_str(), msg.length());
    msg_buf.len = msg.length();

    MsgBuf new_buf;
    Connect(reinterpret_cast<sockaddr*>(&serv_addr_));
    EXPECT_TRUE(accepted());
    EXPECT_TRUE(Put(msg_buf));

    uint32_t len_header;
    ReadAll(sock_, reinterpret_cast<char*>(&len_header), 4);
    new_buf.len = ntohl(len_header);
    EXPECT_EQ(msg.length(), new_buf.len);
    ReadAll(sock_, new_buf.buf, new_buf.len);
    EXPECT_STREQ(msg.c_str(), string(new_buf.buf, new_buf.len).c_str());
}

TEST_F(TCPIOPolicyTest, GetOne) {
    string msg("Hello World");
    MsgBuf msg_buf;
    strncpy(msg_buf.buf, msg.c_str(), msg.length());
    msg_buf.len = msg.length();

    MsgBuf new_buf;
    Connect(reinterpret_cast<sockaddr*>(&serv_addr_));
    EXPECT_TRUE(accepted());

    uint32_t len_header = htonl(msg_buf.len);
    EXPECT_TRUE(WriteAll(sock_, reinterpret_cast<char*>(&len_header), 4));
    EXPECT_TRUE(WriteAll(sock_, msg_buf.buf, msg_buf.len));
    EXPECT_TRUE(Get(&new_buf));
    EXPECT_STREQ(msg.c_str(), string(new_buf.buf, new_buf.len).c_str());
}

TEST_F(TCPIOPolicyTest, AttachConnectedSock) {
    string msg("Hello World");
    MsgBuf msg_buf;
    strncpy(msg_buf.buf, msg.c_str(), msg.length());
    msg_buf.len = msg.length();

    MsgBuf new_buf;
    int new_sock = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_NE(-1, connect(new_sock, reinterpret_cast<sockaddr*>(&serv_addr_), sizeof(serv_addr_)));
    EXPECT_TRUE(accepted());
    AttachConnectedSock(new_sock);

    uint32_t len_header = htonl(msg_buf.len);
    EXPECT_TRUE(WriteAll(sock_, reinterpret_cast<char*>(&len_header), 4));
    EXPECT_TRUE(WriteAll(sock_, msg_buf.buf, msg_buf.len));
    EXPECT_TRUE(Get(&new_buf));
    EXPECT_STREQ(msg.c_str(), string(new_buf.buf, new_buf.len).c_str());
}

