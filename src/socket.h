#ifndef SOCKET_H
#define SOCKET_H

#include "func.h"
#include "proto.h"
#include "ansi.h"

namespace file {

class socket {
    int m_fd;
    ansi::sockaddr_un m_addr;

public:
    socket();
    socket(const socket& rhs);
    socket(socket&& rhs);
    socket& operator=(const socket& rhs);
    socket& operator=(socket&& rhs);
    ~socket() noexcept;
    socket clone() const;
    void close();
    int fd() const noexcept;
    void addr(const ansi::sockaddr_un& addr);
    void operator>>(std::string& s);
    template<typename T> void operator>>(std::vector<T>& msg);
    template<typename T> void operator>>(T& msg);
    template<typename T> T recv(const size_t len);
    template<typename T> T recv();
    template<typename T> const socket& send(const T& msg);
    template<typename T> const socket& send(const std::vector<T>& msg);
    template<unsigned N> void connect(const char(&path)[N]);
    void listen(unsigned max = ansi::max_conn) const;
    socket& time(unsigned sec, unsigned usec = 0);
};

template<typename T>
void socket::operator>>(std::vector<T>& msg) {
    auto vec = ansi::recvvec<msg::msglen, T>(this->fd());
    msg.insert(std::end(msg), std::make_move_iterator(std::begin(vec)),
             std::make_move_iterator(std::end(vec)));
}

template<typename T>
void socket::operator>>(T& msg) {
    static_assert(sizeof(T) <= msg::maxlen, "max socksize exceded");
    fun::donotfail("recv", ansi::recv, m_fd, &msg, sizeof(T), 0);
}

template<typename T>
T socket::recv(const size_t msgsize) {
    T res;
    if (!msgsize) {
        return res;
    }
    res.reserve(msgsize);
    for (size_t i{0}; i < msgsize/msg::msglen; ++i) {
        *this >> res;
    } if (msgsize%msg::msglen) {
        *this >> res;
        res.resize(msgsize);
    }
    return res;
}

template<typename T> T socket::recv() {
    T res;
    *this >> res;
    return res;
}

template<typename T>
const socket& socket::send(const std::vector<T>& msg) {
    auto it = std::begin(msg);
    for (size_t i{0}; i < msg.size()/msg::msglen; ++i, it+=msg::msglen) {
        *this << std::vector<T>(it, it+msg::msglen);
    } if (msg.size()%msg::msglen) {
        *this << std::vector<T>(it, std::end(msg));
    }
    return *this;
}

template<typename T>
const socket& socket::send(const T& msg) {
    return *this << msg;
}

template<unsigned N>
void socket::connect(const char(&path)[N]) {
    static_assert(N<=107, "socket path too big");
    m_addr = ansi::sockaddr_un{ansi::unix, {0}};
    ansi::strcpy(m_addr.sun_path, path);
    fun::donotfail("connect", ansi::sun_connect, m_fd, &m_addr);
}

template<typename T>
socket& operator<<(socket& s, const std::vector<T>& msg) {
    ansi::sendvec<msg::msglen, T>(s.fd(), msg);
    return s;
}

socket& operator<<(socket& s, const size_t msg);

socket& operator<<(socket& s, const double msg);

socket& operator<<(socket& s, const std::string& msg);

class bind {
    ansi::sockaddr_un m_addr;
public:
    template<unsigned N> bind(const char (&path)[N], socket& s);
    ~bind() {
        ansi::unlink(m_addr.sun_path);
    }
    ansi::sockaddr_un addr() const noexcept { return m_addr; }
};

template<unsigned N>
bind::bind(const char (&path)[N], socket& s) : m_addr{ansi::unix, {0}} {
    static_assert(N <= 107, "socket path too big");
    ansi::strcpy(m_addr.sun_path, path);
    fun::donotfail("bind", ansi::sun_bind, s.fd(), &m_addr);
    s.addr(m_addr);
}

}

#endif // SOCKET_H
