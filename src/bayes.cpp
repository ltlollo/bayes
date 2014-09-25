#include "bayes.h"

namespace bst {

Bayes::Bayes() : gen(rd()) {
}

double Bayes::influence(const Counts& counts) const noexcept {
    return std::abs(pgoods(counts.goods, counts.bads) -
                    pgoods(goods, bads));
}

bool Bayes::influencing(const Counts& counts) const noexcept {
    return influence(counts) > delta_influence;
}

double Bayes::pcond(const Text& text, Good) const noexcept {
    double prob {1.};
    for (const auto& stat : stats) {
        if (!influencing(stat.counts)) {
            break;
        }
        if (text.find(stat.pred) != text.end()) {
            prob *= psmoothed(stat.counts.goods, goods);
        } else {
            prob *= (1.0 - psmoothed(stat.counts.goods, goods));
        }
    }
    return prob;
}

double Bayes::pcond(const Text& text, Bad) const noexcept {
    double prob {1.};
    for (const auto& stat : stats) {
        if (!influencing(stat.counts)) {
            break;
        }
        if (text.find(stat.pred) != text.end()) {
            prob *= psmoothed(stat.counts.bads, bads);
        } else {
            prob *= (1.0 - psmoothed(stat.counts.bads, bads));
        }
    }
    return prob;
}

bool Bayes::randfreq(double f) {
    return std::generate_canonical<double, 1>(gen) <= f;
}

void Bayes::train(Bad, const Text& text) {
    if (text.empty()) {
        return;
    }
    for (const auto& it: text) {
        auto pos = std::find_if(std::begin(stats), std::end(stats),
                                [&](const Data& data) {
            return data.pred == it.first;
        });
        if (pos != std::end(stats)) {
            pos->counts += Counts{0, it.second};
        } else {
            stats.push_back(Data{it.first, Counts{0, it.second}});
        }
        bads += it.second;
    }
    std::sort(std::begin(stats), std::end(stats),
              [&](const Data& fst, const Data& snd){
        return influence(fst.counts) > influence(snd.counts);
    });
}

void Bayes::train(Good, const Text& text) {
    if (text.empty()) {
        return;
    }
    for (const auto& it: text) {
        auto pos = std::find_if(std::begin(stats), std::end(stats),
                                [&](const Data& data) {
            return data.pred == it.first;
        });
        if (pos != std::end(stats)) {
            pos->counts += Counts{it.second, 0};
        } else {
            stats.push_back(Data{it.first, Counts{it.second, 0}});
        }
        goods += it.second;
    }
    std::sort(std::begin(stats), std::end(stats),
              [&](const Data& fst, const Data& snd){
        return influence(fst.counts) > influence(snd.counts);
    });
}

double Bayes::pcond(Good, const Text& text) const noexcept {
    auto pg = pcond(text, Good());
    auto pb = pcond(text, Bad());
    auto pgs = pgoods(goods+laplace_eps, bads+4.0*laplace_eps);
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
        res += (stat.counts.goods + stat.counts.bads);
    }
    return res ? static_cast<double>(res)/(goods+bads) : 0.0;
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
            static_cast<double>(unknown)/text.size() > unknown_trsh)
            ? true : false;
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
