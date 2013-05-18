#ifndef WISSBI_UTIL_HPP_
#define WISSBI_UTIL_HPP_

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <string>
#include <sstream>
#include <algorithm>

namespace wissbi {
namespace util {

static void ConnectStringToSockaddr(const std::string& conn_str, sockaddr_in* addr_ptr) {
    memset(addr_ptr, 0, sizeof(*addr_ptr));
    auto idx = conn_str.find(":");
    addr_ptr->sin_addr.s_addr = inet_addr(conn_str.substr(0, idx).c_str());

    std::istringstream iss(conn_str.substr(idx + 1, conn_str.length()));
    int port;
    iss >> port;
    addr_ptr->sin_port = htons(port);

    addr_ptr->sin_family = AF_INET;
}

static std::string SockaddrToConnectString(const sockaddr_in& addr) {
    std::ostringstream oss;
    oss << inet_ntoa(addr.sin_addr);
    oss << ":";
    oss << ntohs(addr.sin_port);
    return oss.str();
}

static std::string GetHostIP() {
    char buf[256];
    gethostname(buf, 256);
    struct addrinfo addr_hint;
    memset(&addr_hint, 0, sizeof(struct addrinfo));
    addr_hint.ai_family = AF_INET;
    addr_hint.ai_socktype = SOCK_STREAM;
    struct addrinfo *addr_head;
    getaddrinfo(buf, NULL, &addr_hint, &addr_head);
    return inet_ntoa(reinterpret_cast<struct sockaddr_in*>(addr_head->ai_addr)->sin_addr);
}

static std::string EscapeSubFolderPath(const std::string& unescaped) {
    std::string res(unescaped);
    std::replace(res.begin(), res.end(), '/', '#');
    res.erase(std::remove(res.end() - 1, res.end(), '#'), res.end());
    return res;
}

static std::string UnescapeSubFolderPath(const std::string& escaped) {
    std::string res(escaped);
    std::replace(res.begin(), res.end(), '#', '/');
    return res;
}

}
}

#endif  // WISSBI_UTIL_HPP_
