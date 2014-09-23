#ifndef PROTO_H
#define PROTO_H

namespace msg {
constexpr unsigned msglen{1023}, maxlen{1048000}, maxmsgs{2500};
constexpr unsigned maxsize{maxmsgs*msglen};

static_assert(msglen > 0, "message length: too small");
static_assert(maxmsgs > 0, "number of messages limit: too small");
static_assert(maxlen > 0, "message length limit: too small");
}

#endif // PROTO_H
