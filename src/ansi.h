#ifndef ANSI_H
#define ANSI_H

#include <string>
#include <vector>

#include "func.h"
#include "proto.h"

namespace ansi {

#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

constexpr unsigned max_conn{5};
constexpr int ss{SOL_SOCKET}, lw{SO_RCVLOWAT}, unix{AF_UNIX},
stream{SOCK_STREAM}, rt{SO_RCVTIMEO}, stdin{STDIN_FILENO},
stdout{STDOUT_FILENO}, stderr{STDERR_FILENO}, sigint{SIGINT}, sigpipe{SIGPIPE};

int sun_connect(const int fd, const sockaddr_un* addr) noexcept;

int sun_bind(const int fd, const sockaddr_un* addr);

template<unsigned protolen>
inline void sendstr(const int fd, const std::string& msg) {
    static_assert(protolen <= msg::maxlen, "max socksize exceded");
    fun::doreturn("message too big", msg.size() > protolen);
    std::string fullmsg{msg};
    fullmsg.resize(protolen, '\0');
    fun::donotfail("send", send, fd, fullmsg.c_str(), fullmsg.size(), 0);
}

template<unsigned protolen, typename T>
inline void sendvec(const int fd, const std::vector<T>& msg) {
    static_assert(protolen*sizeof(T) <= msg::maxlen, "max socksize exceded");
    fun::doreturn("message too big", msg.size() > protolen);
    std::vector<T> fullmsg{msg};
    fullmsg.resize(protolen, static_cast<T>(0));
    fun::donotfail("send", send, fd, &fullmsg[0], protolen*sizeof(T), 0);
}

template<unsigned protolen>
inline std::string recvstr(const int fd) {
    static_assert(protolen <= msg::maxlen, "max socksize exceded");
    char data[protolen+1];
    auto l = protolen;
    fun::doreturn("sockopt", setsockopt, fd, ss, lw, &l, sizeof(l));
    fun::donotfail("recv", recv, fd, data, protolen, 0);
    return std::string(data);
}

template<unsigned protolen, typename T>
inline std::vector<T> recvvec(const int fd) {
    static_assert(protolen <= msg::maxlen, "max socksize exceded");
    std::vector<T> data(protolen, 0);
    auto l = protolen*sizeof(T);
    fun::doreturn("sockopt", setsockopt, fd, ss, lw, &l, sizeof(l));
    fun::donotfail("recv", recv, fd, &data[0], protolen*sizeof(T), 0);
    return data;
}

template<typename T>
inline void safesend(const int fd, const T& msg) {
    static_assert(sizeof(T) <= msg::maxlen, "max socksize exceded");
    fun::donotfail("send", send, fd, &msg, sizeof(T), 0);
}
} // ansi namespace

#endif // ANSI_H
