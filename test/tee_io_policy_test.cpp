#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "io_policy/tee.hpp"

using wissbi::MsgBuf;
using wissbi::io_policy::Tee;
using ::testing::_;
using ::testing::Return;
using namespace std;

class MockIOPolicy {
    public:
    MOCK_METHOD1(Get, bool(MsgBuf *msg_buf));
    MOCK_METHOD1(Put, bool(const MsgBuf &msg_buf));
};

class TeeIOPolicyTest : public ::testing::Test {
    protected:
    virtual void SetUp() {
    }

    virtual void TearDown() {
    }
};

TEST_F(TeeIOPolicyTest, Put) {
    shared_ptr<MockIOPolicy> out1_ptr(new MockIOPolicy());
    shared_ptr<MockIOPolicy> out2_ptr(new MockIOPolicy());
    shared_ptr<MockIOPolicy> out3_ptr(new MockIOPolicy());

    Tee<MockIOPolicy> tee;
    tee.AddBranch("1", out1_ptr);
    tee.AddBranch("2", out2_ptr);
    tee.AddBranch("3", out3_ptr);

    EXPECT_CALL(*out1_ptr, Put(_)).WillOnce(Return(true));
    EXPECT_CALL(*out2_ptr, Put(_)).WillOnce(Return(true));
    EXPECT_CALL(*out3_ptr, Put(_)).WillOnce(Return(true));
    MsgBuf msg;
    tee.Put(msg);
}

TEST_F(TeeIOPolicyTest, BranchRemoved) {
    shared_ptr<MockIOPolicy> out1_ptr(new MockIOPolicy());
    shared_ptr<MockIOPolicy> out2_ptr(new MockIOPolicy());
    shared_ptr<MockIOPolicy> out3_ptr(new MockIOPolicy());

    Tee<MockIOPolicy> tee;
    tee.AddBranch("1", out1_ptr);
    tee.AddBranch("2", out2_ptr);

    EXPECT_CALL(*out1_ptr, Put(_)).WillOnce(Return(true));
    EXPECT_CALL(*out2_ptr, Put(_)).WillOnce(Return(true));
    EXPECT_CALL(*out3_ptr, Put(_)).WillOnce(Return(true));
    MsgBuf msg;
    tee.Put(msg);

    tee.RemoveBranch("1");
    tee.RemoveBranch("2");
    tee.AddBranch("3", out3_ptr);
    tee.Put(msg);
}

