#include "bs.hpp"
#include <cmath>

double norm_cdf(double x){
    return 0.5 * std::erfc(-x / std::sqrt(2.0));
}

double bs_price(const Params& p, OptionType type){
    const double sqrtT = std::sqrt(p.T);
    const double d1 = (std::log(p.S0 / p.K) + (p.r + 0.5 * p.sigma * p.sigma) * p.T) 
                      / (p.sigma * sqrtT);
    const double d2 = d1 - p.sigma * sqrtT;
    const double disc = std::exp(-p.r * p.T);
    
    if (type == OptionType::Call){
        return p.S0 * norm_cdf(d1) - p.K * disc * norm_cdf(d2);
    }
    else{
        return p.K * disc * norm_cdf(-d2) - p.S0 * norm_cdf(-d1);
    }
}