#include "extra/socket.h"
#include <iterator>
#include <iostream>

int main(int argc, char *argv[]) {
    const auto print_help = [&]() {
        std::cerr << "USAGE:\t" << argv[0]
                  << " action\n"
                     "\taction<string>: eval|good|bad:\n"
                     "\t\teval: \n"
                     "\t\tgood: \n"
                     "\t\tbad: \n"
                     "SCOPE:\n"
                  << std::endl;
    };
    if (argc < 2) {
        print_help();
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
        print_help();
        return 1;
    }
    std::cin >> std::noskipws;
    std::istream_iterator<char> begin(std::cin), end;
    auto input = std::string(begin, end);

    file::Socket sock;
    sock.connect("/tmp/.bayes-sock");
    sock.send(action);
    sock.send(input.size());
    sock.send(input);
    auto res = sock.recv<double>();
    std::cout << res << std::endl;
    return 0;
}
