#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include <string>

#include "gtest/gtest.h"
#include "sub_entry.hpp"

class SubEntryTest : public ::testing::Test {
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

TEST_F(SubEntryTest, MetaDirNotExist) {
    system((std::string("rm -rf ") + meta_dir_).c_str());
    bool exception = false;
    try {
        wissbi::SubEntry sub_entry(meta_dir_, "foo", "192.168.0.1:12345");
    }
    catch(...){
        exception = true;
    }
    EXPECT_TRUE(exception);
    EXPECT_NE(0, system((std::string("ls ") + meta_dir_).c_str()));
}

TEST_F(SubEntryTest, SubDirNotExist) {
    system((std::string("rm -rf ") + meta_dir_ + "/sub").c_str());
    bool exception = false;
    try {
        wissbi::SubEntry sub_entry(meta_dir_, "foo", "192.168.0.1:12345");
    }
    catch(...){
        exception = true;
    }
    EXPECT_TRUE(exception);
    EXPECT_NE(0, system((std::string("ls ") + meta_dir_ + "/sub").c_str()));
}

TEST_F(SubEntryTest, CreatedQueue) {
    system((std::string("mkdir ") + meta_dir_ + "/sub/foo").c_str());
    wissbi::SubEntry sub_entry(meta_dir_, "foo", "192.168.0.1:12345");
    EXPECT_EQ(0, system((std::string("ls ") + meta_dir_ + "/sub/foo/192.168.0.1:12345,foo").c_str()));
}

TEST_F(SubEntryTest, AutoCreateQueue) {
    wissbi::SubEntry sub_entry(meta_dir_, "foo", "192.168.0.1:12345");
    EXPECT_EQ(0, system((std::string("ls ") + meta_dir_ + "/sub/foo/192.168.0.1:12345,foo").c_str()));
}

TEST_F(SubEntryTest, EntryExists) {
    system((std::string("mkdir ") + meta_dir_ + "/sub/foo").c_str());
    system((std::string("touch --date 1970-01-01T00:00:00Z ") + meta_dir_ + "/sub/foo/" + "192.168.0.1:12345,foo").c_str());

    wissbi::SubEntry sub_entry(meta_dir_, "foo", "192.168.0.1:12345");
    struct stat stat_buf;
    EXPECT_EQ(0, stat((std::string(meta_dir_) + "/sub/foo/192.168.0.1:12345,foo").c_str(), &stat_buf));
    EXPECT_GT(stat_buf.st_mtime, 0);
}

TEST_F(SubEntryTest, EntryCleanup) {
    {
        wissbi::SubEntry sub_entry(meta_dir_, "foo", "192.168.0.1:12345");
        EXPECT_EQ(0, system((std::string("ls ") + meta_dir_ + "/sub/foo/192.168.0.1:12345,foo").c_str()));
    }
    EXPECT_NE(0, system((std::string("ls ") + meta_dir_ + "/sub/foo/192.168.0.1:12345,foo").c_str()));
}

TEST_F(SubEntryTest, EntryRenewExisting) {
    wissbi::SubEntry sub_entry(meta_dir_, "foo", "192.168.0.1:12345");
    system((std::string("touch --date 1970-01-01T00:00:00Z ") + meta_dir_ + "/sub/foo/" + "192.168.0.1:12345,foo").c_str());

    sub_entry.renew();
    struct stat stat_buf;
    EXPECT_EQ(0, stat((std::string(meta_dir_) + "/sub/foo/192.168.0.1:12345,foo").c_str(), &stat_buf));
    EXPECT_GT(stat_buf.st_mtime, 0);
}

TEST_F(SubEntryTest, EntryRenewRecover) {
    wissbi::SubEntry sub_entry(meta_dir_, "foo", "192.168.0.1:12345");
    system((std::string("rm ") + meta_dir_ + "/sub/foo/" + "192.168.0.1:12345,foo").c_str());

    sub_entry.renew();
    struct stat stat_buf;
    EXPECT_EQ(0, stat((std::string(meta_dir_) + "/sub/foo/192.168.0.1:12345,foo").c_str(), &stat_buf));
    EXPECT_GT(stat_buf.st_mtime, 0);
}

TEST_F(SubEntryTest, SubQueue) {
    system((std::string("mkdir -p ") + meta_dir_ + "/sub/foo/bar").c_str());
    wissbi::SubEntry sub_entry(meta_dir_, "foo/bar/test", "192.168.0.1:12345");
    EXPECT_EQ(0, system((std::string("ls ") + meta_dir_ + "/sub/foo/bar/test/192.168.0.1:12345,foo#bar#test").c_str()));
}

TEST_F(SubEntryTest, TopicMode) {
    system((std::string("mkdir -p ") + meta_dir_ + "/sub/foo/bar").c_str());
    wissbi::SubEntry sub_entry(meta_dir_, "foo/bar/test", "192.168.0.1:12345", true);
    EXPECT_EQ(0, system((std::string("ls ") + meta_dir_ + "/sub/foo/bar/test/192.168.0.1:12345,foo#bar#test.192.168.0.1:12345").c_str()));
}
