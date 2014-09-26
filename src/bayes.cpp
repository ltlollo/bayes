#include "bayes.h"

namespace bst {

void Counts::operator+=(const Counts& rhs) noexcept {
    goods += rhs.goods;
    bads += rhs.bads;
}

double Counts::sum() const noexcept {
    return goods + bads;
}

Count Counts::get(Good) const noexcept {
    return goods;
}

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
        total.bads += it.second;
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
        total.goods += it.second;
    }
    std::sort(std::begin(stats), std::end(stats),
              [&](const Data& fst, const Data& snd){
        return influence(fst.counts) > influence(snd.counts);
    });
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
