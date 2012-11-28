#ifndef WISSBI_IO_POLICY_SYSVMQ_HPP_
#define WISSBI_IO_POLICY_SYSVMQ_HPP_

#include "msg_buf.hpp"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>

namespace wissbi {
namespace io_policy {

class SysvMq {
    public:
    SysvMq() {
        std::ostringstream oss;
        oss << "/tmp/wissbi.sysvmq." << getpid();
        key_file_ = oss.str();
        int res = creat(key_file_.c_str(), O_RDONLY | S_IRUSR | S_IWUSR);
        close(res);
        key_ = ftok(key_file_.c_str(), 0);
        mqid_ = msgget(key_, IPC_CREAT | S_IRUSR | S_IWUSR);
        if(mqid_ < 0) {
            throw "cannot acquire mq: " + std::string(strerror(errno));
        }
    }

    ~SysvMq() {
        struct msqid_ds ds;
        msgctl(mqid_, IPC_RMID, &ds);
        assert(0 != key_file_.length());
        unlink(key_file_.c_str());
    }

    bool Put(const MsgBuf &msg) {
        const_cast<MsgBuf&>(msg).mtype = 1;
        return 0 == msgsnd(mqid_, &msg, msg.len, 0);
    }

    bool Get(MsgBuf *msg_ptr) {
        ssize_t n = msgrcv(mqid_, msg_ptr, wissbi::MAX_MSG_SIZE, 0, 0);
        if(n < 0) {
            msg_ptr->len = 0;
            return false;
        }
        msg_ptr->len = n;
        return true;
    }

    int mqid() {
        return mqid_;
    }

    key_t key() {
        return key_;
    }

    std::string key_file() {
        return key_file_;
    }

    private:
    int mqid_;
    key_t key_;
    std::string key_file_;
};

}
}

#endif  // WISSBI_IO_POLICY_SYSVMQ_HPP_
