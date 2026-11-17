#include "mc.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <stdexcept>

namespace{
    struct RunningStats{
        std::size_t count = 0;
        double mean = 0.0;
        double m2 = 0.0;

        void add(double x){
            ++count;
            const double delta = x - mean;
            mean += delta / static_cast<double>(count);
            m2 += delta * (x - mean);
        }

        MCResult result() const{
            const double var = m2 / static_cast<double>(count - 1);
            const double se = std::sqrt(var / static_cast<double>(count));
            return {mean, se, mean - 1.96 * se, mean + 1.96 * se};
        }
    };

    std::size_t independent_samples(std::size_t n, bool antithetic){
        const std::size_t m = antithetic ? n / 2 : n;
        if(m < 2){
            throw std::invalid_argument("Need at least 2 independent samples");
        }
        return m;
    }

    double vanilla_payoff(double S, double K, bool is_call){
        return is_call ? std::max(S - K, 0.0) : std::max(K - S, 0.0);
    }
}

MCResult mc_european(const Params& p, OptionType type, std::size_t n, bool antithetic, std::uint64_t seed){
    const std::size_t m = independent_samples(n, antithetic);

    std::mt19937_64 rng(seed);
    std::normal_distribution<double> N01(0.0, 1.0);

    const double drift = (p.r - 0.5 * p.sigma * p.sigma) * p.T;
    const double vol = p.sigma * std::sqrt(p.T);
    const double disc = std::exp(-p.r * p.T);
    const bool is_call = (type == OptionType::Call);

    auto payoff = [&](double z) {
        const double ST = p.S0 * std::exp(drift + vol * z);
        return disc * vanilla_payoff(ST, p.K, is_call);
    };

    RunningStats stats;
    for (std::size_t i = 0; i < m; i++){
        const double z = N01(rng);
        stats.add(antithetic ? 0.5 * (payoff(z) + payoff(-z)) : payoff(z));
    }
    return stats.result();
}

MCResult mc_asian(const Params& p, OptionType type, std::size_t n, std::size_t steps, bool antithetic, std::uint64_t seed){
    if(steps == 0){
        throw std::invalid_argument("Steps must be positive");
    }
    const std::size_t m = independent_samples(n, antithetic);

    std::mt19937_64 rng(seed);
    std::normal_distribution<double> N01(0.0, 1.0);

    const double dt = p.T / static_cast<double>(steps);
    const double drift = (p.r - 0.5 * p.sigma * p.sigma) * dt;
    const double vol = p.sigma * std::sqrt(dt);
    const double disc = std::exp(-p.r * p.T);
    const bool is_call = (type == OptionType::Call);

    RunningStats stats;
    for (std::size_t i = 0; i < m; i++){
        double S = p.S0, S_anti = p.S0;
        double sum = 0.0, sum_anti = 0.0;
        for (std::size_t k = 0; k < steps; k++){
            const double z = N01(rng);
            S *= std::exp(drift + vol * z);
            sum += S;
            if(antithetic){
                S_anti *= std::exp(drift - vol * z);
                sum_anti += S_anti;
            }
        }
        const double x = disc * vanilla_payoff(sum / static_cast<double>(steps), p.K, is_call);
        if (antithetic){
            const double x_anti = disc * vanilla_payoff(sum_anti / static_cast<double>(steps), p.K, is_call);
            stats.add(0.5 * (x + x_anti));
        }
        else {
            stats.add(x);
        }
    }
    return stats.result();
}