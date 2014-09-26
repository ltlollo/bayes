#include "bayes.h"

namespace bst {

double Counts::sum() const noexcept {
    return goods + bads;
}

template<>
Count Counts::get(Good) const noexcept {
    return goods;
}

template<>
Count Counts::get(Bad) const noexcept {
    return bads;
}

Counts Counts::operator+(const Counts& rhs) const noexcept {
    return Counts{goods+rhs.goods, bads+rhs.bads};
}

Bayes::Bayes() : gen(rd()) {}

double Bayes::influence(const Counts& counts) const noexcept {
    return std::abs(population(counts, Good()) - population(total, Good()));
}

bool Bayes::influencing(const Counts& counts) const noexcept {
    return influence(counts) > delta_influence;
}

double Bayes::pcond(Good, const Text& text) const noexcept {
    auto pg = pcond(text, Good());
    auto pb = pcond(text, Bad());
    auto pgs = population(total, Good(), 0.2);
    return pg*pgs/(pg*pgs + pb*(1.0 - pgs));
}

double Bayes::pcond(Bad, const Text& text) const noexcept {
    return 1.0 - pcond(Good(), text);
}

double Bayes::opinionated() const noexcept {
    Count res{0};
    for (const auto& stat: stats) {
        if (!influencing(stat.counts)) {
            break;
        }
        res += stat.counts.sum();
    }
    return res ? res/total.sum() : 0.0;
}

bool Bayes::interesting(const Text &text) const noexcept {
    Count interest{0}, unknown{0};
    for (const auto& w: text) {
        auto stat = std::find_if(stats.begin(), stats.end(),
                              [&](const Data& data) {
            return data.pred == w.first;
        });
        if (stat != stats.end()) {
            if (influencing(stat->counts)) {
                ++interest;
            }
        } else {
            ++unknown;
        }
    }
    return (static_cast<double>(interest)/text.size() > initerest_trsh &&
            static_cast<double>(unknown)/text.size() > unknown_trsh) ?
                true : false;
}

Text parse(const std::string& orig) {
    Text text;
    if (orig.empty()) {
        return text;
    }
    std::istringstream iss(orig);
    Pred pred;
    while (iss >> pred) {
        if (text.find(pred) == text.end()) {
            text[pred] = 1;
        } else {
            text[pred]++;
        }
    }
    return text;
}

}
