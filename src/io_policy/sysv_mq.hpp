#ifndef WISSBI_IO_POLICY_SYSVMQ_HPP_
#define WISSBI_IO_POLICY_SYSVMQ_HPP_

#include "msg_buf.hpp"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

namespace wissbi {
namespace io_policy {

class SysvMq {
    public:
    SysvMq() {
        mqid_ = msgget(IPC_PRIVATE, S_IRUSR|S_IWUSR);
        if(mqid_ < 0) {
            throw "cannot acquire mq: " + std::string(strerror(errno));
        }
    }

    ~SysvMq() {
        struct msqid_ds ds;
        msgctl(mqid_, IPC_RMID, &ds);
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

    private:
    int mqid_;
};

}
}

#endif  // WISSBI_IO_POLICY_SYSVMQ_HPP_
