#pragma once

#include <cmath>
#include <algorithm>

namespace market_maker {

struct MMQuotes {
    double reservation_price{0.0};
    double bid_price{0.0};
    double ask_price{0.0};
    double half_spread{0.0};
};

struct ASParameters {
    double gamma{0.1};   // Risk aversion parameter
    double sigma{0.2};   // Volatility of asset
    double kappa{1.5};   // Order book liquidity density (intensity decay)
    double T{1.0};       // Trading session horizon (e.g. 1 day = 1.0)
    int max_inventory{10}; // Hard inventory constraint
};

class AvellanedaStoikovModel {
public:
    explicit AvellanedaStoikovModel(ASParameters params = {}) : params_(params) {}

    MMQuotes compute_quotes(double mid_price, int current_inventory, double current_time) const noexcept {
        double time_left = std::max(0.0001, params_.T - current_time);

        // Reservation price: r(s, q, t) = s - q * gamma * sigma^2 * (T - t)
        double inventory_penalty = current_inventory * params_.gamma * (params_.sigma * params_.sigma) * time_left;
        double reservation_price = mid_price - inventory_penalty;

        // Optimal total spread: s = gamma * sigma^2 * (T - t) + (2 / gamma) * ln(1 + gamma / kappa)
        double spread = params_.gamma * (params_.sigma * params_.sigma) * time_left +
                        (2.0 / params_.gamma) * std::log(1.0 + params_.gamma / params_.kappa);
        double half_spread = spread * 0.5;

        // Quotes centered around reservation price
        double bid = reservation_price - half_spread;
        double ask = reservation_price + half_spread;

        // Inventory constraints: stop quoting side if max inventory breached
        if (current_inventory >= params_.max_inventory) {
            bid = -1.0; // Don't buy more
        }
        if (current_inventory <= -params_.max_inventory) {
            ask = -1.0; // Don't sell more
        }

        return MMQuotes{
            .reservation_price = reservation_price,
            .bid_price = bid,
            .ask_price = ask,
            .half_spread = half_spread
        };
    }

private:
    ASParameters params_;
};

} // namespace market_maker
