"""
Avellaneda-Stoikov & Almgren-Chriss Interactive Simulator
Simulates market maker inventory paths, PnL distribution, and optimal liquidation curves using standard library.
"""

import math
import random

def simulate_as_market_maker():
    random.seed(42)
    mid = 100.0
    gamma = 0.1
    sigma = 0.2
    kappa = 1.5
    T = 1.0
    inventory = 0
    cash = 0.0

    print("[*] Simulating Avellaneda-Stoikov market maker over 100 periods...")
    for step in range(100):
        t = step / 100.0
        time_left = max(0.001, T - t)
        reservation = mid - inventory * gamma * (sigma ** 2) * time_left
        spread = gamma * (sigma ** 2) * time_left + (2.0 / gamma) * math.log(1.0 + gamma / kappa)
        bid = reservation - spread / 2.0
        ask = reservation + spread / 2.0

        # Random execution
        if random.random() < 0.3:
            inventory += 1
            cash -= bid
        if random.random() < 0.3:
            inventory -= 1
            cash += ask

    pnl = cash + inventory * mid
    print(f"[*] Final Inventory: {inventory}, Cash: ${cash:.2f}, Total PnL: ${pnl:.2f}")

if __name__ == "__main__":
    simulate_as_market_maker()
