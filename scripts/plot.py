import csv
import os
import sys

import matplotlib.pyplot as plt

BLUE = "#2a78d6"
ORANGE = "#eb6834"
INK = "#3d3d3a"
MUTED = "#8a8980"

src = sys.argv[1] if len(sys.argv) > 1 else "convergence.csv"
out = sys.argv[2] if len(sys.argv) > 2 else "convergence.png"

with open(src, newline="") as f:
    rows = list(csv.DictReader(f))
n = [int(r["n"]) for r in rows]
price = [float(r["price"]) for r in rows]
se = [float(r["se"]) for r in rows]
price_a = [float(r["price_anti"]) for r in rows]
se_a = [float(r["se_anti"]) for r in rows]
bs = float(rows[0]["bs"])

plt.rcParams.update({
    "font.size": 10, "axes.edgecolor": MUTED, "axes.labelcolor": INK,
    "xtick.color": INK, "ytick.color": INK, "axes.spines.top": False,
    "axes.spines.right": False,
})
fig, (ax1, ax2) = plt.subplots(1,2, figsize=(11, 4.2))

lo = [p - 1.96 * s for p, s in zip(price, se)]
hi = [p + 1.96 * s for p, s in zip(price, se)]
ax1.fill_between(n, lo, hi, color=BLUE, alpha = 0.15, linewidth=0)
ax1.plot(n, price, color=BLUE, linewidth=2, marker="o", markersize=4, label="MC estimate (95% CI)")
ax1.axhline(bs, color=MUTED, linewidth=1.5, linestyle="--", label=f"Black-Scholes = {bs:.4f}")
ax1.set_xscale("log")
ax1.set_xlabel("number of paths n")
ax1.set_ylabel("European call price")
ax1.set_title("Price estimate converges to Black-Scholes", color = INK, loc="left")
ax1.grid(True, which="major", color="#e6e5e0", linewidth=0.8)
ax1.legend(frameon=False)

ref = [se[0] * (n[0] / k) ** 0.5 for k in n]
ax2.plot(n, ref, color=MUTED, linewidth=1.5, linestyle="--", label=r"$\propto 1/\sqrt{n}$")
ax2.plot(n, se, color=BLUE, linewidth=2, marker="o", markersize=4, label="plain")
ax2.plot(n, se_a, color=ORANGE, linewidth=2, marker="o", markersize=4, label="antithetic")
ax2.set_xscale("log")
ax2.set_yscale("log")
ax2.set_xlabel("number of paths n")
ax2.set_ylabel("standard error")
ax2.set_title("Standard error decays as 1/√n", color=INK, loc="left")
ax2.grid(True, which="major", color="#e6e5e0", linewidth=0.8)
ax2.legend(frameon=False)

fig.tight_layout()
os.makedirs(os.path.dirname(out) or ".", exist_ok=True)
fig.savefig(out, dpi=150)
print(f"saved {out}")