#include "market_maker/avellaneda_stoikov.hpp"
#include "market_maker/almgren_chriss.hpp"
#include <cassert>
#include <iostream>

void test_as_inventory_skew() {
    market_maker::ASParameters params{.gamma = 0.1, .sigma = 0.2, .kappa = 1.5, .T = 1.0, .max_inventory = 10};
    market_maker::AvellanedaStoikovModel model(params);

    double mid = 100.0;
    auto flat_quotes = model.compute_quotes(mid, 0, 0.5);
    auto long_quotes = model.compute_quotes(mid, 5, 0.5);
    auto short_quotes = model.compute_quotes(mid, -5, 0.5);

    // Reservation price must decrease when long inventory (discouraging buys, encouraging sells)
    assert(long_quotes.reservation_price < flat_quotes.reservation_price);
    // Reservation price must increase when short inventory (encouraging buys, discouraging sells)
    assert(short_quotes.reservation_price > flat_quotes.reservation_price);

    // Spread must remain positive
    assert(flat_quotes.ask_price > flat_quotes.bid_price);
    std::cout << "[PASS] Avellaneda-Stoikov inventory skew verified\n";
}

void test_almgren_chriss_conservation() {
    double X = 5000.0;
    auto sched = market_maker::AlmgrenChrissExecutor::compute_optimal_schedule(
        X, 1.0, 10, 0.25, 1e-4, 1e-5, 1e-4);

    double total_traded = 0.0;
    for (double n : sched.trade_sizes) {
        total_traded += n;
    }

    // Sum of trades must equal total initial inventory X
    assert(std::abs(total_traded - X) < 1e-6);
    assert(std::abs(sched.holdings_trajectory.back()) < 1e-6);
    std::cout << "[PASS] Almgren-Chriss inventory conservation verified\n";
}

int main() {
    std::cout << "Running Market Making & Optimal Execution Unit Tests...\n";
    test_as_inventory_skew();
    test_almgren_chriss_conservation();
    std::cout << "\nAll Unit Tests Passed Successfully!\n";
    return 0;
}
