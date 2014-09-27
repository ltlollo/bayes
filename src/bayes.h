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

constexpr Bad neg(Good) { return Bad(); }
constexpr Good neg(Bad) { return Good(); }

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

class Counts {
    Count goods{0}, bads{0};

public:
    constexpr Counts(Good, Count goods) : goods{goods}, bads{0} {}
    constexpr Counts(Bad, Count bads) : goods{0}, bads{bads} {}
    constexpr Counts(Count goods, Count bads) : goods{goods}, bads{bads} {}
    constexpr Counts operator+(const Counts& rhs) const noexcept  {
        return Counts{goods + rhs.goods, bads + rhs.bads};
    }
    double sum() const noexcept;
    template<typename T> Count get(T kind) const noexcept;
};

struct Data {
    Pred pred;
    Counts counts;
};

template<typename T>
double population(const Counts& stat, T pred) noexcept {
    return stat.get(pred)/stat.sum();
}

template<typename T>
double population(const Counts& stat, T pred, double pdef) noexcept {
    return stat.get(pred) ? stat.get(pred)/stat.sum() : pdef;
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

    bool randfreq(double f);
    double influence(const Counts& counts) const noexcept;
    bool influencing(const Counts& counts) const noexcept;
    bool interesting(const Text& text) const noexcept;
    template<typename T> double pcond(const Text& text, T prop) const noexcept;

public:
    Bayes();
    template<typename T> double pcond(T kind, const Text& text) const noexcept;
    template<typename T> void train(T kind, const Text& text);
    template<typename T> void biased_train(T prop, const Text& text);
    template<typename T> void aged_train(T, const Text& text);
    template<typename T> void biased_aged_train(T, const Text& text);
    double opinionated() const noexcept;
};

#include "bayes_impl.h"

} // end of namespace bayes

#endif // BAYES_H
