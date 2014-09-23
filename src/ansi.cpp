#include "ansi.h"

namespace ansi {

int sun_connect(const int fd, const sockaddr_un* addr) noexcept {
    return connect(fd, (sockaddr*)addr, SUN_LEN(addr));
}

int sun_bind(const int fd, const sockaddr_un* addr) {
    return bind(fd, (sockaddr*)addr, sizeof(*addr));
}
}
