
template<typename T>
void Bayes::biased_train(T prop, const Text& text) {
    auto rnd = std::generate_canonical<double, 1>(gen);
    if (rnd <= 0.2) {
        train(prop, text);
    } else {
        double bias = couriosity(prop)*interesting(text);
        if (rnd <= 0.2 + bias) {
            train(prop, text);
        }
    }
}

template<typename T>
void Bayes::aged_train(T prop, const Text& text) {
    if (randfreq(std::exp(-total.sum()/ageing_rate))) {
        train(prop, text);
    }
}

template<typename T>
void Bayes::biased_aged_train(T prop, const Text& text) {
    auto rnd = std::generate_canonical<double, 1>(gen);
    auto ageing = std::exp(-total.sum()/ageing_rate);
    if (rnd <= ageing) {
        train(prop, text);
    } else {
        double bias = couriosity(prop)*interesting(text);
        if (rnd <= ageing*std::exp(total.sum()*bias/ageing_rate)) {
            train(prop, text);
        }
    }
}

template<typename T>
double Bayes::pcond(const Text& text, T prop) const noexcept {
    double prob {1.0};
    for (const auto& stat : stats) {
        if (!influencing(stat.counts)) {
            break;
        }
        if (text.find(stat.pred) != text.end()) {
            prob *= psmoothed(stat.counts.get(prop), total.get(prop));
        } else {
            prob *= (1.0 - psmoothed(stat.counts.get(prop), total.get(prop)));
        }
    }
    return prob;
}
