#pragma once
#include <cstddef>
#include <cstdint>
#include "bs.hpp"

struct MCResult{
    double price;
    double se;
    double ci_low;
    double ci_high;
};

MCResult mc_european(const Params& p, OptionType type, std::size_t n, bool antithetic = false, std::uint64_t seed = 42);
MCResult mc_asian(const Params& p, OptionType type, std::size_t n, std::size_t steps, bool antithetic = false, std::uint64_t seed = 42);