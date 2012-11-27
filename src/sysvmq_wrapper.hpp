#ifndef WISSBI_SYSVMQ_WRAPPER_HPP_
#define WISSBI_SYSVMQ_WRAPPER_HPP_

#include "msg_buf.hpp"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

namespace wissbi {

class SysVMqWrapper {
    public:
    SysVMqWrapper() {
        mqid_ = msgget(IPC_PRIVATE, S_IRUSR|S_IWUSR);
        if(mqid_ < 0) {
            throw "cannot acquire mq: " + std::string(strerror(errno));
        }
    }

    ~SysVMqWrapper() {
        struct msqid_ds ds;
        msgctl(mqid_, IPC_RMID, &ds);
    }

    bool Put(const MsgBuf &msg_buf) {
        const_cast<MsgBuf&>(msg_buf).mtype = 1;
        return 0 == msgsnd(mqid_, &msg_buf, msg_buf.len, 0);
    }

    bool Get(MsgBuf *msg_buf_ptr) {
        ssize_t n = msgrcv(mqid_, msg_buf_ptr, wissbi::MAX_MSG_SIZE, 0, 0);
        if(n < 0) {
            msg_buf_ptr->len = 0;
            return false;
        }
        msg_buf_ptr->len = n;
        return true;
    }

    int mqid() {
        return mqid_;
    }

    private:
    int mqid_;
};

}

#endif  // WISSBI_SYSVMQ_WRAPPER_HPP_
