#include "gtest/gtest.h"
#include "util.hpp"

TEST(UtilTest, TestConnectStringToSockaddr1) {
    sockaddr_in in_addr;
    wissbi::util::ConnectStringToSockaddr("192.168.0.1:12345", &in_addr);
    EXPECT_EQ(12345, ntohs(in_addr.sin_port));
    EXPECT_STREQ("192.168.0.1", inet_ntoa(in_addr.sin_addr));
    EXPECT_EQ(AF_INET, in_addr.sin_family);
}

TEST(UtilTest, TestGetHostIP1) {
    FILE *f = popen("hostname -i", "r");
    char buf[20];
    size_t n = fread(buf, 1, 20, f);
    buf[n-1] = '\0';
    pclose(f);
    EXPECT_STREQ(buf, wissbi::util::GetHostIP().c_str());
}

TEST(UtilTest, TestEscapeSubFolderPath1) {
    std::string unescaped("foo/bar/test");
    EXPECT_STREQ("foo#bar#test", wissbi::util::EscapeSubFolderPath(unescaped).c_str());
}

TEST(UtilTest, TestUnescapeSubFolderPath1) {
    std::string escaped("foo#bar#test");
    EXPECT_STREQ("foo/bar/test", wissbi::util::UnescapeSubFolderPath(escaped).c_str());
}
