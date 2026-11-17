#include <iostream>
#include <iomanip>
#include <cmath>
#include <chrono>
#include "bs.hpp"
#include "mc.hpp"

int main(){
    Params p{100.0, 100.0, 0.05, 0.2, 1.0};
    double c = bs_price(p, OptionType::Call);
    double put = bs_price(p, OptionType::Put);
    std::cout << std::fixed << std::setprecision(4)
              << "Call: " << c << "\nPut: " << put
              << "\nParity: " << c - put << " vs "
              << p.S0 - p.K * std::exp(-p.r * p.T) << "\n";

    const double bs_call = bs_price(p, OptionType::Call);
    std::cout << "\n" << std::setw(8) << "N" << std::setw(12) << "MC plain" 
              << std::setw(8) << "SE" << std::setw(10) << "MC anti" 
              << std::setw(10) << "SE anti" << "\n";
    for (std::size_t n : {1'000, 10'000, 100'000, 1'000'000}){
        MCResult a = mc_european(p, OptionType::Call, n, false);
        MCResult b = mc_european(p, OptionType::Call, n, true);
        std::cout << std::setw(8) << n
                  << std::setw(12) << a.price << std::setw(8) << a.se
                  << std::setw(10) << b.price << std::setw(10) << b.se << "\n";
    }
    std::cout << "BS call: " << bs_call << "\n";

    const std::size_t n_bench = 10'000'000;

    auto t0 = std::chrono::steady_clock::now();
    MCResult r = mc_european(p, OptionType::Call, n_bench);
    auto t1 = std::chrono::steady_clock::now();

    double sec = std::chrono::duration<double>(t1-t0).count();
    std::cout << "\n" << n_bench << " paths in " << sec << " s (" << n_bench / sec / 1e6 << " M paths/s), price = " << r.price << "\n";
}