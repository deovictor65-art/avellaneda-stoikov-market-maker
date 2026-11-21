#pragma once

#include <vector>
#include <cmath>

namespace market_maker {

struct ExecutionSchedule {
    std::vector<double> holdings_trajectory; // x_j
    std::vector<double> trade_sizes;         // n_j
    double expected_cost{0.0};
    double cost_variance{0.0};
};

class AlmgrenChrissExecutor {
public:
    // X: total volume to liquidate
    // T: execution horizon (e.g., 1.0 day or 3600 seconds)
    // N: number of discrete execution intervals
    // sigma: asset annual volatility
    // eta: temporary price impact coefficient (eta * v)
    // gamma: permanent price impact coefficient (gamma * X)
    // lambda: trader risk aversion
    static ExecutionSchedule compute_optimal_schedule(
        double X, double T, size_t N, double sigma, double eta, double gamma, double lambda) {

        double tau = T / static_cast<double>(N);
        // Half-decay factor: kappa
        double kappa_sq = (lambda * sigma * sigma) / eta;
        double kappa = std::sqrt(kappa_sq);

        std::vector<double> x(N + 1, 0.0);
        std::vector<double> n(N, 0.0);

        x[0] = X;
        double sinh_kappa_T = std::sinh(kappa * T);

        for (size_t j = 1; j <= N; ++j) {
            double t_j = j * tau;
            if (std::abs(sinh_kappa_T) < 1e-12) {
                // Linear TWAP fallback if kappa == 0
                x[j] = X * (1.0 - static_cast<double>(j) / N);
            } else {
                x[j] = X * (std::sinh(kappa * (T - t_j)) / sinh_kappa_T);
            }
            n[j - 1] = x[j - 1] - x[j];
        }

        // Expected transaction cost E[x] = 0.5 * gamma * X^2 + eta * sum(n_j^2 / tau)
        double permanent_cost = 0.5 * gamma * X * X;
        double temporary_cost = 0.0;
        for (double trade : n) {
            temporary_cost += eta * (trade * trade / tau);
        }
        double expected_cost = permanent_cost + temporary_cost;

        // Cost variance V[x] = 0.5 * sigma^2 * sum(tau * (x_j)^2)
        double variance_sum = 0.0;
        for (size_t j = 0; j < N; ++j) {
            variance_sum += tau * x[j] * x[j];
        }
        double cost_variance = sigma * sigma * variance_sum;

        return ExecutionSchedule{
            .holdings_trajectory = std::move(x),
            .trade_sizes = std::move(n),
            .expected_cost = expected_cost,
            .cost_variance = cost_variance
        };
    }
};

} // namespace market_maker
