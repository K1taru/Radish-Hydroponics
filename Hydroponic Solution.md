
# RAMGO HydroGro (A/B) nutrient notes for radish hydroponics

This document captures practical guidance for using a 2-part inorganic hydroponic nutrient (Bottle A + Bottle B) in a recirculating system.

Important: exact RAMGO HydroGro dosage (ml/L) can vary by product batch and crop chart. If you can share a clear photo of the label/mixing chart for Bottle A and Bottle B, I can convert it into exact per-liter dosing and Arduino setpoints.

## What Bottle A and Bottle B are for

Most hydroponic “A/B” fertilizers split incompatible salts so they stay dissolved in the concentrate:

- **Bottle A** typically contains **calcium nitrate** (Ca + nitrate-N) and often **chelated iron**.
- **Bottle B** typically contains **phosphates** (P), **sulfates** (S), **magnesium sulfate** (Mg), potassium salts (K), plus **trace elements**.

They are separated because concentrated calcium will precipitate with concentrated phosphate/sulfate (forming solids), which can:

- clog tubing/pumps
- reduce available nutrients
- make EC/TDS readings misleading

## Safe mixing procedure (works for nearly all A/B nutrients)

1. Fill reservoir with water.
2. Add **Bottle A**, mix thoroughly.
3. Add **Bottle B**, mix thoroughly.
4. Measure **EC (preferred) or TDS/PPM**, then adjust by adding small increments of A then B (same ratio) until target.
5. Measure pH and adjust last.

Never mix A and B concentrates together before they are diluted in the reservoir.

## Targets for radishes (hydroponics)

Radish is a fast crop; it generally prefers a mildly acidic nutrient solution and a “moderate” strength.

### Optimal pH

- Practical target band: **pH 5.8–6.5**
- Common setpoint: **pH 6.0–6.3**

Your guess (5.5–6.5) is close; going much below ~5.6 can increase risk of nutrient imbalance for some setups, and above ~6.6 can reduce availability of iron and some micronutrients.

### Optimal nutrient strength (EC + PPM/TDS)

Use **EC** when possible, because “PPM” depends on the meter’s conversion scale.

- Typical EC range for radish: **1.2–1.8 mS/cm**
	- Seedlings/first true leaves: **0.6–1.0 mS/cm**
	- Main growth: **1.2–1.8 mS/cm**
	- If leaf tips burn: back off toward **1.0–1.2 mS/cm**

Approximate PPM equivalents (depends on your TDS meter scale):

- If your meter uses the **500 scale**: 1.2–1.8 mS/cm ≈ **600–900 ppm**
- If your meter uses the **700 scale**: 1.2–1.8 mS/cm ≈ **840–1260 ppm**

If you don’t know your scale, look for “0.5” or “0.7” in the meter specs, or compare your meter’s ppm reading against a known EC calibration solution.

## Dosage guidance (until we confirm the RAMGO label)

Because RAMGO HydroGro’s concentrate strength isn’t documented in this repo, the most reliable approach is:

- Dose **A and B equally** (same ml/L) and use the **EC target** above.
- Add in small steps (example: +0.5 ml/L A, mix; +0.5 ml/L B, mix) until EC is in range.

### Fill from the RAMGO label (needed for exact ml/L)

Paste/attach the values printed on your bottles here:

- Bottle A: ____ ml per ____ L (for leafy/fruit/root crops) or a chart value for radish
- Bottle B: ____ ml per ____ L
- Any stated target EC/PPM/pH: __________________

Once you provide those numbers, we can compute:

- exact ml per reservoir volume
- peristaltic pump run-time per dosing event (given your pump’s ml/min)
- stable automation rules (hysteresis) for EC and pH control

## Practical tips for stable readings (Arduino automation)

- Temperature affects EC/TDS; if your TDS module doesn’t do temperature compensation, keep readings consistent (same time of day, after mixing).
- Always let the reservoir circulate for a few minutes after dosing before trusting pH/TDS.
- pH tends to drift upward in many systems; adjust slowly to avoid overshoot.

