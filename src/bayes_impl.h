template<typename T> void Bayes::filtered_train(T prop, const Text& text) {
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

template<typename T> void Bayes::aged_train(T prop, const Text& text) {
    if (randfreq(std::exp(-(goods+bads)/ageing_rate))) {
        train(prop, text);
    }
}

template<typename T> void Bayes::agc_train(T prop, const Text& text) {
    auto rnd = std::generate_canonical<double, 1>(gen);
    auto ageing = std::exp(-(goods+bads)/ageing_rate);
    if (rnd <= ageing) {
        train(prop, text);
    } else {
        double bias = couriosity(prop)*interesting(text);
        if (rnd <= ageing*std::exp((goods+bads)*bias/ageing_rate)) {
            train(prop, text);
        }
    }
}
