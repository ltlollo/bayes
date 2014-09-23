#ifndef FUNC_H
#define FUNC_H

#include <stdexcept>
#include <type_traits>
#include <utility>
#include <iostream>

namespace fun {

template<bool relaxed, typename T>
[[noreturn]] inline std::enable_if_t<!relaxed> dbg(T&& msg) {
    throw std::runtime_error(std::forward<T>(msg));
}

template<bool relaxed, typename T>
inline std::enable_if_t<relaxed> dbg(T&& msg) {
    std::cerr << msg;
}

template<typename P, typename... Ts>
inline constexpr auto fun(P&& f, Ts&&... args) {
    return f(std::forward<Ts>(args)...);
}

template<typename P, typename T, typename... Ts>
inline void dotry(T& err, P&& f, Ts&&... args) {
    try {
        f(std::forward<Ts>(args)...);
    } catch(std::runtime_error& e) {
        err << e.what();
    }
}

template<bool relax, typename T, typename... Ts>
inline void dbgfail(T&& msg, Ts&&... args) {
    if (fun(std::forward<Ts>(args)...) < 0) {
        dbg<relax>(std::forward<T>(msg));
    }
}

template<bool relax, typename T, typename U>
inline std::enable_if_t<std::is_integral<U>::value>
dbgfail(T&& msg, U i) {
    if (i < 0) {
        dbg<relax>(std::forward<T>(msg));
    }
}

template<bool relax, typename T, typename... Ts>
inline void dbgreturn(T&& msg, Ts&&... args) {
    if (fun(std::forward<Ts>(args)...)) {
        dbg<relax>(std::forward<T>(msg));
    }
}

template<bool relax, typename T, typename U>
inline std::enable_if_t<std::is_integral<U>::value> dbgreturn(T&& msg, U i) {
    if (i) {
        dbg<relax>(std::forward<T>(msg));
    }
}

template<typename T, typename... Ts>
inline void donotfail(T&& msg, Ts&&... args) {
    dbgfail<false>(std::forward<T>(msg), std::forward<Ts>(args)...);
}

template<typename T, typename... Ts>
inline void doreturn(T&& msg, Ts&&... args) {
    dbgreturn<false>(std::forward<T>(msg), std::forward<Ts>(args)...);
}

template<typename T, typename... Ts>
inline void logfail(T&& msg, Ts&&... args) {
    dbgfail<true>(std::forward<T>(msg), std::forward<Ts>(args)...);
}

template<typename T, typename... Ts>
inline void logreturn(T&& msg, Ts&&... args) {
    dbgreturn<true>(std::forward<T>(msg), std::forward<Ts>(args)...);
}

template<typename P, typename... Ts>
[[noreturn]] inline void loop(P&& f, Ts&&... args) {
    for(;;) {
        f(std::forward<Ts>(args)...);
    }
}

template<typename T, typename U, typename... Ts>
void print_args(T&& padd, U&& arg, Ts&&... args) {
    std::cerr << padd << arg << '\n';
    print_args(std::forward<Ts>(args)...);
}

template<typename T, typename U>
void print_args(T&& padd, U&& arg) {
    std::cerr << padd << arg << '\n';
}

template<typename P, typename... Ts>
inline void functest(P f, Ts&&... args) {
    try {
        f(std::forward<Ts>(args)...);
        std::cerr << "returned successfully\n\twith args:\n";
        print_args("\t", std::forward<Ts>(args)...);
    } catch(std::exception& e) {
        std::cerr << "execption caught: " << e.what() << "\n\twith args:\n";
        print_args('\t', std::forward<Ts>(args)...);
    }
}

}

#endif // FUNC_H
