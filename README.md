# High-Frequency Market Making & Optimal Execution (`avellaneda-stoikov-market-maker`)

A modern C++20 quantitative execution and market-making engine implementing the continuous-time stochastic control framework of **Avellaneda & Stoikov (2008)** alongside the **Almgren & Chriss (2000)** optimal execution liquidation model.

## Features

- **Avellaneda-Stoikov Dynamic Reservation Price**:
  $$r(s, q, t) = s - q \gamma \sigma^2 (T - t)$$
  Dynamically skews bid/ask quotes based on inventory holding $q$, risk aversion $\gamma$, and volatility $\sigma$.
- **Endogenous Optimal Spread Quoting**:
  $$s(t) = \gamma \sigma^2 (T - t) + \frac{2}{\gamma} \ln\left(1 + \frac{\gamma}{\kappa}\right)$$
  Calculates liquidity-aware bid/ask levels incorporating book order arrival intensity decay $\kappa$.
- **Almgren-Chriss Optimal Liquidation Trajectory**:
  Minimizes expected transaction cost plus variance of execution costs under temporary and permanent price impact.
- **Microsecond Execution**: Header-only C++20 design with zero heap allocations in the critical path.

## Quick Build & Run

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
ctest --test-dir build --output-on-failure
./build/market_maker_sim
```

## License

MIT License. Authored by deovictor65-art.
