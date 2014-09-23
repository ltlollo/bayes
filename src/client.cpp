#include "socket.h"
#include <iterator>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        return 1;
    }
    size_t action{99};
    std::string actionstr(argv[1]);
    if (actionstr == "good") {
        action = 0;
    } else if (actionstr == "bad") {
        action = 1;
    } else if (actionstr == "eval") {
        action = 2;
    } else {
        return 1;
    }
    std::cin >> std::noskipws;
    std::istream_iterator<char> begin(std::cin), end;
    auto input = std::string(begin, end);

    file::socket sock;
    sock.connect("/tmp/.bayes-sock");
    sock.send(action);
    sock.send(input.size());
    sock.send(input);
    auto res = sock.recv<double>();
    std::cout << res << std::endl;
    return 0;
}
