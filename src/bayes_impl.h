template<typename T>
void Bayes::train(T kind, const Text& text) {
    if (text.empty()) {
        return;
    }
    for (const auto& it: text) {
        auto counts = Counts(kind, it.second);
        auto pos = std::find_if(std::begin(stats), std::end(stats),
                                [&](const Data& data) {
            return data.pred == it.first;
        });
        if (pos != std::end(stats)) {
            pos->counts = pos->counts + counts;
        } else {
            stats.push_back(Data{it.first, counts});
        }
        total = total + counts;
    }
    std::sort(std::begin(stats), std::end(stats),
              [&](const Data& fst, const Data& snd){
        return influence(fst.counts) > influence(snd.counts);
    });
}


template<typename T>
void Bayes::biased_train(T kind, const Text& text) {
    auto rnd = std::generate_canonical<double, 1>(gen);
    if (rnd <= 0.2) {
        train(kind, text);
    } else {
        double bias = couriosity(kind)*interesting(text);
        if (rnd <= 0.2 + bias) {
            train(kind, text);
        }
    }
}

template<typename T>
void Bayes::aged_train(T kind, const Text& text) {
    if (randfreq(std::exp(-total.sum()/ageing_rate))) {
        train(kind, text);
    }
}

template<typename T>
void Bayes::biased_aged_train(T kind, const Text& text) {
    auto rnd = std::generate_canonical<double, 1>(gen);
    auto ageing = std::exp(-total.sum()/ageing_rate);
    if (rnd <= ageing) {
        train(kind, text);
    } else {
        double bias = couriosity(kind)*interesting(text);
        if (rnd <= ageing*std::exp(total.sum()*bias/ageing_rate)) {
            train(kind, text);
        }
    }
}

template<typename T>
double Bayes::pcond(const Text& text, T kind) const noexcept {
    double prob {1.0};
    for (const auto& stat : stats) {
        if (!influencing(stat.counts)) {
            break;
        }
        if (text.find(stat.pred) != text.end()) {
            prob *= psmoothed(stat.counts.get(kind), total.get(kind));
        } else {
            prob *= (1.0 - psmoothed(stat.counts.get(kind), total.get(kind)));
        }
    }
    return prob;
}
