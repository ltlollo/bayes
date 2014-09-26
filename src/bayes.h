#ifndef BAYES_H
#define BAYES_H

#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <random>

namespace bst {

struct Good {};
struct Bad {};

constexpr double laplace_eps{0.01}, delta_influence{0.1}, ageing_rate{1e4},
initerest_trsh{0.5}, unknown_trsh{0.1};
constexpr double couriosity(Good) { return 0.02; }
constexpr double couriosity(Bad) { return 0.01; }

static_assert(laplace_eps > 0.0 && laplace_eps < 1.0, "invalid epsilon");
static_assert(delta_influence > 0.0 && delta_influence < 1.0, "invalid delta");
static_assert(delta_influence > 0.0, "invalid speed");

using Count = std::size_t;
using Pred = std::string;
using Text = std::map<Pred, Count>;

struct Counts {
    Count goods, bads;
    void operator+=(const Counts& rhs) noexcept {
        goods += rhs.goods;
        bads += rhs.bads;
    }
    double sum() const noexcept {
        return goods + bads;
    }
    Count get(Good) const noexcept {
        return goods;
    }
    Count get(Bad) const noexcept {
        return bads;
    }
};

struct Data {
    Pred pred;
    Counts counts;
};

static inline double pgoods(Count goods, Count bads) noexcept {
    return goods/(goods + bads);
}

static inline double pgoods(double goods, double bads) noexcept {
    return goods/(goods + bads);
}

static inline double pbads(Count goods, Count bads) noexcept {
    return 1.0 - pgoods(goods, bads);
}

static inline double psmoothed(double w, double c) noexcept {
    return (w + laplace_eps)/(c + 2*laplace_eps);
}

Text parse(const std::string& orig);

class Bayes {
    std::vector<Data> stats;
    Counts total{0, 0};
    std::random_device rd;
    std::mt19937 gen;

    inline double influence(const Counts& counts) const noexcept;
    inline bool influencing(const Counts& counts) const noexcept;
    template<typename T>double pcond(const Text& text, T prop) const noexcept;
    bool randfreq(double f);
    bool interesting(const Text& text) const noexcept;
public:
    Bayes();
    double pcond(Good, const Text& text) const noexcept;
    double pcond(Bad, const Text& text) const noexcept;
    void train(Good, const Text& text);
    void train(Bad, const Text& text);
    template<typename T> void biased_train(T prop, const Text& text);
    template<typename T> void aged_train(T, const Text& text);
    template<typename T> void biased_aged_train(T, const Text& text);
    double opinionated() const noexcept;
};

#include "bayes_impl.h"

} // end of namespace bayes

#endif // BAYES_H
