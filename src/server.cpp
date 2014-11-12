#include "bayes.h"
#include "extra/server.h"

void job(file::Socket&& ms, bst::Bayes& bayes) {
    try {
        file::Socket sock{std::move(ms)};
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
        printf("%s\n", e.what());
    }
}

class Server {
    bst::Bayes bayes;
    srv::Server srv;
public:
    Server() : srv("/tmp/.bayes-sock") {
        srv.run(job, std::ref(bayes));
    }
};

int main(int argc, char *argv[]) {
    const auto print_help = [&]() {
        printf("USAGE: %s\n", argv[0]);
    };
    if (argc > 1) {
        print_help();
        return 1;
    }
    try {
        Server();
    } catch (srv::Clean) {
        printf("cleaning...\n");
        return 1;
    } catch (std::runtime_error& e) {
        printf("%s\n", e.what());
        return 1;
    }
}
