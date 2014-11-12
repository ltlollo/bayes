#include "bayes.h"
#include "extra/socket.h"
#include <iostream>

void job(file::Socket&& ms, bst::Bayes& bayes) {
    try {
        file::Socket sock{std::move(ms)};
        sock.time(100);
        auto action = sock.recv<std::size_t>();
        auto size = sock.recv<std::size_t>();
        err::doreturn("exceded size", size > msg::maxsize);
        auto str = sock.recv<std::string>(size);
        double res{0.};
        switch (action) {
        case 0: // train good
            bayes.biased_aged_train(bst::Good(), bst::parse(str));
            break;
        case 1: // train bad
            bayes.biased_aged_train(bst::Bad(), bst::parse(str));
            break;
        case 2: // evaluate
            res = bayes.pcond(bst::Good(), bst::parse(str));
            break;
        default:
            res = -1;
            break;
        }
        sock.send(res);
    } catch (std::exception& e) {
        std::cerr << e.what();
    }
}

class Server {
    bst::Bayes bayes;
    file::Socket sock;
    file::Bind sockbind;
public:
    Server() : sock{}, sockbind("/tmp/.bayes-sock", sock) {
        err::donotfail_errno("sigaction", ansi::signal, ansi::sigint,
                             [](int, siginfo_t*, void*){
            ansi::unlink("/tmp/.bayes-sock");
            ansi::exit(0);
        });
    }
    [[ noreturn ]] void run() {
        sock.listen();
        fun::loop([&](){
            job(sock.clone(), bayes);
        });
    }
};

int main(int argc, char *argv[]) {
    const auto print_help = [&]() {
        std::cerr << "USAGE:\t" << argv[0]
                  << "\n"
                     "SCOPE:\n"
                  << std::endl;
    };
    if (argc > 1) {
        print_help();
        return 1;
    }
    try {
        Server().run();
    } catch (std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    } catch (...) {
        return 1;
    }
}
