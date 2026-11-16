#include <iostream>
#include <iomanip>
#include <cmath>
#include "bs.hpp"

int main(){
    Params p{100.0, 100.0, 0.05, 0.2, 1.0};
    double c = bs_price(p, OptionType::Call);
    double put = bs_price(p, OptionType::Put);
    std::cout << std::fixed << std::setprecision(4)
              << "Call: " << c << "\nPut: " << put
              << "\nParity: " << c - put << " vs "
              << p.S0 - p.K * std::exp(-p.r * p.T) << "\n";
}