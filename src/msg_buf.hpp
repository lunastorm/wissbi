#ifndef WISSBI_MSG_BUF_HPP_
#define WISSBI_MSG_BUF_HPP_

#include <string>

namespace wissbi {

const size_t MAX_MSG_SIZE = 4096;

typedef struct msg_buf {
    long mtype;
    char buf[MAX_MSG_SIZE];
    size_t len;
    std::string dest;
} MsgBuf;

}

#endif  // WISSBI_MSG_BUF_HPP_
