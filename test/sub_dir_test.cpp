
#include <stdlib.h>
#include <algorithm>
/*
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include <string>
*/
#include "gtest/gtest.h"
#include "sub_dir.hpp"

class SubDirTest : public ::testing::Test {
    protected:
    virtual void SetUp() {
        const char *kTmpDirTemplate = "/tmp/wissbi-test.XXXXXX";
        strncpy(meta_dir_, kTmpDirTemplate, strlen(kTmpDirTemplate) + 1);
        assert(mkdtemp(meta_dir_) != NULL);
        assert(system((std::string("mkdir ") + meta_dir_ + "/sub").c_str()) == 0);
    }

    virtual void TearDown() {
        system((std::string("rm -rf ") + meta_dir_).c_str());
    }

    char meta_dir_[50];
};

TEST_F(SubDirTest, NoQueue) {
    wissbi::SubDir sub_dir(meta_dir_, "foo");
    EXPECT_EQ(0, sub_dir.GetSubList().size());
}

TEST_F(SubDirTest, NoSubInQueue) {
    system((std::string("mkdir ") + meta_dir_ + "/sub/foo").c_str());
    wissbi::SubDir sub_dir(meta_dir_, "foo");
    EXPECT_EQ(0, sub_dir.GetSubList().size());
}

TEST_F(SubDirTest, SomeSubInQueue) {
    system((std::string("mkdir ") + meta_dir_ + "/sub/foo").c_str());
    system((std::string("touch ") + meta_dir_ + "/sub/foo/192.168.0.1:12345").c_str());
    system((std::string("touch ") + meta_dir_ + "/sub/foo/192.168.0.2:12345").c_str());
    system((std::string("touch ") + meta_dir_ + "/sub/foo/192.168.0.3:12345").c_str());
    
    wissbi::SubDir sub_dir(meta_dir_, "foo");
    auto list = sub_dir.GetSubList();
    EXPECT_EQ(3, list.size());
    EXPECT_NE(list.end(), std::find(list.begin(), list.end(), "192.168.0.1:12345"));
    EXPECT_NE(list.end(), std::find(list.begin(), list.end(), "192.168.0.2:12345"));
    EXPECT_NE(list.end(), std::find(list.begin(), list.end(), "192.168.0.3:12345"));
}

TEST_F(SubDirTest, HasSubQueue) {
    system((std::string("mkdir ") + meta_dir_ + "/sub/foo").c_str());
    system((std::string("mkdir ") + meta_dir_ + "/sub/foo/bar").c_str());
    system((std::string("touch ") + meta_dir_ + "/sub/foo/192.168.0.1:12345").c_str());
    system((std::string("touch ") + meta_dir_ + "/sub/foo/192.168.0.2:12345").c_str());
    system((std::string("touch ") + meta_dir_ + "/sub/foo/192.168.0.3:12345").c_str());
    
    wissbi::SubDir sub_dir(meta_dir_, "foo");
    auto list = sub_dir.GetSubList();
    EXPECT_EQ(3, list.size());
    EXPECT_NE(list.end(), std::find(list.begin(), list.end(), "192.168.0.1:12345"));
    EXPECT_NE(list.end(), std::find(list.begin(), list.end(), "192.168.0.2:12345"));
    EXPECT_NE(list.end(), std::find(list.begin(), list.end(), "192.168.0.3:12345"));
}

