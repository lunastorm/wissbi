#include "gtest/gtest.h"
#include "util.hpp"
#include <stdlib.h>
#include <fstream>

TEST(UtilTest, TestConnectStringToSockaddr) {
    sockaddr_in in_addr;
    wissbi::util::ConnectStringToSockaddr("192.168.0.1:12345", &in_addr);
    EXPECT_EQ(12345, ntohs(in_addr.sin_port));
    EXPECT_STREQ("192.168.0.1", inet_ntoa(in_addr.sin_addr));
    EXPECT_EQ(AF_INET, in_addr.sin_family);
}

TEST(UtilTest, TestGetHostIP) {
    char tmpstr[20] = "/tmp/temp.XXXXXX";
    char *tmpfile = mktemp(tmpstr);
    assert(tmpfile != NULL);
    system(("ping -c 1 `hostname` | grep PING | sed -e 's/.*(\\([0-9]*\\.[0-9]*\\.[0-9]*\\.[0-9]*\\)).*/\\1/' > " + std::string(tmpfile)).c_str());

    std::ifstream ifs(tmpfile);
    std::string ip;
    std::getline(ifs, ip);
    unlink(tmpfile);
    EXPECT_STREQ(ip.c_str(), wissbi::util::GetHostIP().c_str());
}

TEST(UtilTest, TestEscapeSubFolderPath) {
    std::string unescaped("foo/bar/test");
    EXPECT_STREQ("foo#bar#test", wissbi::util::EscapeSubFolderPath(unescaped).c_str());
}

TEST(UtilTest, TestIgnoreTrailingSlash) {
    std::string unescaped("foo/bar/test/");
    EXPECT_STREQ("foo#bar#test", wissbi::util::EscapeSubFolderPath(unescaped).c_str());
}

TEST(UtilTest, TestUnescapeSubFolderPath) {
    std::string escaped("foo#bar#test");
    EXPECT_STREQ("foo/bar/test", wissbi::util::UnescapeSubFolderPath(escaped).c_str());
}
