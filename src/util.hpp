#ifndef WISSBI_UTIL_HPP_
#define WISSBI_UTIL_HPP_

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <sstream>

namespace wissbi {
namespace util {

void ConnectStringToSockaddr(const std::string& conn_str, sockaddr_in* addr_ptr) {
    memset(addr_ptr, 0, sizeof(*addr_ptr));
    auto idx = conn_str.find(":");
    addr_ptr->sin_addr.s_addr = inet_addr(conn_str.substr(0, idx).c_str());

    std::istringstream iss(conn_str.substr(idx + 1, conn_str.length()));
    int port;
    iss >> port;
    addr_ptr->sin_port = htons(port);
}

}
}

#endif  // WISSBI_UTIL_HPP_
