#include "mc.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <stdexcept>

MCResult mc_european(const Params& p, OptionType type, std::size_t n, bool antithetic, std::uint64_t seed){
    const std::size_t m = antithetic ? n / 2 : n;
    if (m < 2){
        throw std::invalid_argument("Need at least 2 independent samples!");
    }

    std::mt19937_64 rng(seed);
    std::normal_distribution<double> N01(0.0, 1.0);

    const double drift = (p.r - 0.5 * p.sigma * p.sigma) * p.T;
    const double vol = p.sigma * std::sqrt(p.T);
    const double disc = std::exp(-p.r * p.T);
    const bool is_call = (type == OptionType::Call);

    auto payoff = [&](double z) {
        const double ST = p.S0 * std::exp(drift + vol * z);
        return disc * (is_call ? std::max(ST - p.K,0.0) : std::max(p.K - ST, 0.0));
    };

    double mean = 0.0, m2 = 0.0;
    for (std::size_t k = 1; k <= m; ++k){
        const double z = N01(rng);
        const double x = antithetic ? 0.5 * (payoff(z) + payoff(-z)) : payoff(z);
        const double delta = x - mean;
        mean += delta / static_cast<double>(k);
        m2 += delta * (x - mean);
    }

    const double var = m2 / static_cast<double>(m - 1);
    const double se = std::sqrt(var / static_cast<double>(m));
    return {mean, se, mean - 1.96 * se, mean + 1.96 * se};
}