#include <cmath>
#include <iostream>
#include <stdexcept>
#include "bs.hpp"
#include "mc.hpp"

static int failures = 0;

static void check(bool cond, const char* name){
    std::cout << (cond ? "[PASS]" : "[FAIL]") << name << "\n";
    if (!cond){
        ++failures;
    }
}

int main(){
    const Params p{100.0, 100.0, 0.05, 0.2, 1.0};
    const double call = bs_price(p, OptionType::Call);
    const double put = bs_price(p, OptionType::Put);

    // --- Black-Scholes ---
    check(std::abs(call - 10.4506) < 1e-4, "BS call matches known value");
    check(std::abs(put - 5.5735) < 1e-4, "BS put matches known value");
    check(std::abs((call - put) - (p.S0 - p.K * std::exp(-p.r * p.T))) < 1e-10, "put-call parity");

    // --- Monte Carlo vs BS ---
    const MCResult mc_call = mc_european(p, OptionType::Call, 1'000'000);
    const MCResult mc_put = mc_european(p, OptionType::Put, 1'000'000);
    const MCResult mc_anti = mc_european(p, OptionType::Call, 1'000'000, true);

    check(std::abs(mc_call.price - call) < 3 * mc_call.se, "MC call within 3 SE of BS");
    check(std::abs(mc_put.price - put) < 3 * mc_put.se, "MC put within 3 SE of BS");
    check(std::abs(mc_anti.price - call) < 3 * mc_anti.se, "antithetic call within 3 SE of BS");

    // --- variance reduction ---
    check(mc_anti.se < mc_call.se, "antithetic reduces SE at equal n");

    // --- 1/sqrt(N) convergence ---
    const MCResult small = mc_european(p, OptionType::Call, 100'000);
    const MCResult large = mc_european(p, OptionType::Call, 400'000);
    const double ratio = large.se / small.se;
    check(ratio > 0.45 && ratio < 0.55, "SE halves when N quadruples");

    // --- input validation ---
    bool threw = false;
    try {
        mc_european(p, OptionType::Call, 1);
    }catch (const std::invalid_argument&){
        threw = true;
    }
    check(threw, "rejects n < 2");

    std::cout << (failures == 0 ? "\nAll tests passed\n" : "\nSome tests FAILED\n");
    return failures == 0 ? 0 : 1;
}