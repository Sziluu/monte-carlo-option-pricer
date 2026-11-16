#pragma once

enum class OptionType { Call, Put};

struct Params{
    double S0, K, r, sigma, T;
};

double norm_cdf(double x);
double bs_price(const Params& p, OptionType type);