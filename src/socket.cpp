#include "socket.h"

using namespace fun;
using namespace msg;
using namespace ansi;
using std::swap;

socklen_t socklen{sizeof(sockaddr_un)};

namespace file {

socket& operator<<(socket& s, const size_t msg) { // double?
    safesend(s.fd(), msg);
    return s;
}

socket& operator<<(socket& s, const double msg) {
    safesend(s.fd(), msg);
    return s;
}

socket& operator<<(socket& s, const std::string& msg) {
    sendstr<msg::msglen>(s.fd(), msg);
    return s;
}

socket socket::clone() const {
    return socket(*this);
}

void socket::listen(unsigned max) const {
    fun::donotfail("listen", ansi::listen, m_fd, max);
}

void socket::addr(const sockaddr_un& addr) {
    m_addr = addr;
}

socket& socket::time(unsigned sec, unsigned usec)  {
   timeval t{sec, usec};
   fun::donotfail("time", setsockopt, m_fd, ss, rt, &t, sizeof(timeval));
   return *this;
}

socket::socket() : m_fd{ansi::socket(unix, stream, 0)}, m_addr{unix, {0}} {
    fun::donotfail("get file descriptor", m_fd);
}

socket::~socket() noexcept {
    if (m_fd > 0) {
        ansi::close(m_fd);
    }
}

socket::socket(const socket& rhs)
    : m_fd{accept(rhs.m_fd, (sockaddr*)&m_addr, &socklen)} {
    fun::donotfail("accept", m_fd);
}


socket::socket(socket&& rhs) : m_fd{rhs.m_fd} {
    rhs.m_fd = -1;
}

int socket::fd() const noexcept {
    return m_fd;
}

socket& socket::operator=(const socket& rhs) {
    // self assign is a non-blocking move
    if (&rhs != this) {
        this->close();
        m_fd = accept(rhs.m_fd, (sockaddr*)&m_addr, &socklen);
        fun::donotfail("accept", m_fd);
    }
    return *this;
}

socket& socket::operator=(socket&& rhs) {
    if(&rhs != this) {
        this->close();
        swap(m_fd, rhs.m_fd);
        swap(m_addr, rhs.m_addr);
    }
    return *this;
}

void socket::close() {
    if (m_fd < 0) {
        return;
    }
    int res{ansi::close(m_fd)};
    m_fd = -1;
    fun::doreturn("close", res);
}

void socket::operator>>(std::string& s) {
    s += recvstr<msg::msglen>(this->fd());
}

template<>
const socket& socket::send(const std::string& msg) {
    auto it = std::begin(msg);
    for (size_t i{0}; i < msg.size()/msg::msglen; ++i, it+=msg::msglen) {
        *this << std::string(it, it+msg::msglen);
    } if (msg.size()%msg::msglen) {
        *this << std::string(it, std::end(msg));
    }
    return *this;
}

} // file namespace
