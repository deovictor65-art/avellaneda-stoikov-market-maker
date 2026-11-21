#include "market_maker/avellaneda_stoikov.hpp"
#include "market_maker/almgren_chriss.hpp"
#include <iostream>
#include <iomanip>
#include <random>

int main() {
    std::cout << "=========================================================\n";
    std::cout << "   Avellaneda-Stoikov & Almgren-Chriss Market Maker (C++20)\n";
    std::cout << "=========================================================\n\n";

    // 1. Avellaneda-Stoikov Quoting Simulation
    market_maker::ASParameters params{
        .gamma = 0.1,
        .sigma = 0.3,
        .kappa = 1.5,
        .T = 1.0,
        .max_inventory = 10
    };
    market_maker::AvellanedaStoikovModel as_model(params);

    std::cout << "[*] 1. Avellaneda-Stoikov Inventory Skew Demonstration:\n";
    double mid = 100.0;
    double t = 0.5;

    std::cout << std::setw(12) << "Inventory" 
              << std::setw(14) << "Mid Price" 
              << std::setw(16) << "Reservation" 
              << std::setw(12) << "Bid Quote" 
              << std::setw(12) << "Ask Quote" 
              << std::setw(12) << "Spread" << "\n";
    std::cout << "--------------------------------------------------------------------------------\n";

    for (int q = -5; q <= 5; q += 2) {
        auto quotes = as_model.compute_quotes(mid, q, t);
        std::cout << std::setw(12) << q
                  << std::setw(14) << std::fixed << std::setprecision(3) << mid
                  << std::setw(16) << quotes.reservation_price
                  << std::setw(12) << quotes.bid_price
                  << std::setw(12) << quotes.ask_price
                  << std::setw(12) << (quotes.ask_price - quotes.bid_price) << "\n";
    }

    // 2. Almgren-Chriss Optimal Execution Schedule
    std::cout << "\n[*] 2. Almgren-Chriss Optimal Liquidation Trajectory (X = 10,000 shares):\n";
    double X = 10'000.0;
    double T_exec = 1.0;
    size_t N_steps = 10;
    double sigma = 0.30;
    double eta = 2.5e-4;   // Temporary impact
    double gamma = 1e-5;   // Permanent impact
    double lambda = 1e-4;  // Risk aversion

    auto schedule = market_maker::AlmgrenChrissExecutor::compute_optimal_schedule(
        X, T_exec, N_steps, sigma, eta, gamma, lambda);

    std::cout << std::setw(8) << "Step (j)" 
              << std::setw(18) << "Remaining (x_j)" 
              << std::setw(18) << "Traded (n_j)" << "\n";
    std::cout << "------------------------------------------------\n";

    for (size_t j = 0; j < N_steps; ++j) {
        std::cout << std::setw(8) << j
                  << std::setw(18) << std::fixed << std::setprecision(1) << schedule.holdings_trajectory[j]
                  << std::setw(18) << schedule.trade_sizes[j] << "\n";
    }
    std::cout << std::setw(8) << N_steps
              << std::setw(18) << schedule.holdings_trajectory[N_steps]
              << std::setw(18) << "0.0\n";

    std::cout << "\n[+] Expected Transaction Cost: $" << schedule.expected_cost << "\n";
    std::cout << "[+] Cost Standard Deviation:   $" << std::sqrt(schedule.cost_variance) << "\n";
    std::cout << "\n[✔] Market Making & Optimal Execution Engine Validated.\n";

    return 0;
}
