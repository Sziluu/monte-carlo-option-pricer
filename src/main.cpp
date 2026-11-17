#include <iostream>
#include <iomanip>
#include <cmath>
#include <chrono>
#include <fstream>
#include "bs.hpp"
#include "mc.hpp"

int main(){
    Params p{100.0, 100.0, 0.05, 0.2, 1.0};
    std::cout << std::fixed << std::setprecision(4);

    // --- Black-Scholes ---
    double call = bs_price(p, OptionType::Call);
    double put = bs_price(p, OptionType::Put);
    std::cout << "Call: " << call << "\nPut: " << put
              << "\nParity: " << call - put << " vs "
              << p.S0 - p.K * std::exp(-p.r * p.T) << "\n";

    // --- European MC vs BS ---
    std::cout << "\n" << std::setw(8) << "N" << std::setw(12) << "MC plain" 
              << std::setw(8) << "SE" << std::setw(10) << "MC anti" 
              << std::setw(10) << "SE anti" << "\n";
    for (std::size_t n : {1'000, 10'000, 100'000, 1'000'000}){
        const MCResult a = mc_european(p, OptionType::Call, n, false);
        const MCResult b = mc_european(p, OptionType::Call, n, true);
        std::cout << std::setw(8) << n
                  << std::setw(12) << a.price << std::setw(8) << a.se
                  << std::setw(10) << b.price << std::setw(10) << b.se << "\n";
    }
    std::cout << "BS call: " << call << "\n";

    // --- Asian option (no closed form) ---
    const std::size_t steps = 252;
    const MCResult asian = mc_asian(p, OptionType::Call, 100'000, steps, false);
    const MCResult asian_anti = mc_asian(p, OptionType::Call, 100'000, steps, true);
    std::cout << "\nAsian call (" << steps << " steps, 100000 paths)\n"
              << " plain:       " << asian.price << " SE " << asian.se
              << " 95% CI [" << asian.ci_low << ", " << asian.ci_high << "]\n"
              << " antithetic: " << asian_anti.price << " SE" << asian_anti.se
              << " 95% CI [" << asian_anti.ci_low << ", " << asian_anti.ci_high << "]\n";   

    std::ofstream csv("convergence.csv");
    csv << "n,price,se,price_anti,se_anti,bs\n" << std::setprecision(8);
    for (std::size_t n  = 100; n <= 1'000'000; n *= 2){
        const MCResult a = mc_european(p, OptionType::Call, n, false, 7);
        const MCResult b = mc_european(p, OptionType::Call, n, true, 7);
        csv << n << "," << a.price << "," << a.se << "," << b.price << "," << b.se << "," << call << "\n";
    }
    std::cout << "\nConvergence data written to convergence.csv\n";

    const std::size_t n_bench = 10'000'000;
    auto t0 = std::chrono::steady_clock::now();
    MCResult r = mc_european(p, OptionType::Call, n_bench);
    auto t1 = std::chrono::steady_clock::now();
    double sec = std::chrono::duration<double>(t1-t0).count();
    std::cout << "\n" << n_bench << " paths in " << sec << " s (" << n_bench / sec / 1e6 << " M paths/s), price = " << r.price << "\n";
}