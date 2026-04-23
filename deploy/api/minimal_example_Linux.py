"""
minimal_example.py
==================
Minimal working example for the SolarPILOT CoPylot Python API on Linux.

This script demonstrates:
  1. Basic field generation and flux map calculation.
  2. Dynamic aiming point modification and re-simulation — the core use
     case for time-varying heliostat control studies.

Requirements
------------
- solarpilot.so compiled and placed in ~/spt_dev/api/  (see README)
- copylot.py and pysoltrace.py in ~/spt_dev/api/
- A TMY weather file (see WEATHER_FILE variable below)
- Python packages: pysolar, numpy, matplotlib
    pip install pysolar numpy matplotlib --break-system-packages

Usage
-----
    python3 minimal_example.py
"""

import sys
import os
import platform
import numpy as np
import matplotlib.pyplot as plt

# ---------------------------------------------------------------------------
# 1. Add the SolarPILOT API directory to the Python path
# ---------------------------------------------------------------------------
if platform.system() == "Windows":
    SOLARPILOT_API_PATH = os.path.join(
        "C:", os.sep, "Program Files", "SolarPILOT", "1.5.2", "api"
    )
else:  # Linux / macOS
    SOLARPILOT_API_PATH = os.path.join(os.path.expanduser("~"), "spt_dev", "api")

sys.path.append(SOLARPILOT_API_PATH)
os.chdir(SOLARPILOT_API_PATH)

from copylot import CoPylot

# ---------------------------------------------------------------------------
# 2. Configuration — update WEATHER_FILE to point to your TMY file
# ---------------------------------------------------------------------------
# SolarPILOT includes example climate files in:
#   ~/spt_dev/SolarPILOT/deploy/climate_files/
# Example: "USA CA Daggett (TMY2).csv"
WEATHER_FILE = os.path.join(
    os.path.expanduser("~"),
    "spt_dev", "SolarPILOT", "deploy", "climate_files",
    "USA CA Daggett (TMY2).csv",
)

if not os.path.exists(WEATHER_FILE):
    raise FileNotFoundError(
        f"Weather file not found: {WEATHER_FILE}\n"
        "Update the WEATHER_FILE variable to point to a valid TMY file."
    )

# ---------------------------------------------------------------------------
# 3. Initialize CoPylot and create a SolarPILOT instance
# ---------------------------------------------------------------------------
cp = CoPylot()
r = cp.data_create()
cp.api_callback_create(r)

assert cp.data_set_string(r, "ambient.0.weather_file", WEATHER_FILE), \
    "Failed to set weather file."

# ---------------------------------------------------------------------------
# 4. Configure a small field for demonstration
# ---------------------------------------------------------------------------
cp.data_set_number(r, "solarfield.0.q_des", 100.)          # 100 MWt design power
cp.data_set_string(r, "receiver.0.rec_type", "External cylindrical")
cp.data_set_string(r, "fluxsim.0.flux_model", "Hermite (analytical)")

# Use a single simulation point to speed up the example
cp.data_set_string(r, "solarfield.0.des_sim_detail", "Single simulation point")

# ---------------------------------------------------------------------------
# 5. Generate heliostat field layout
# ---------------------------------------------------------------------------
print("Generating heliostat field layout...")
assert cp.generate_layout(r, nthreads=4), "Layout generation failed."

field = cp.get_layout_info(r)
print(f"  Field contains {len(field)} heliostats.")

# ---------------------------------------------------------------------------
# 6. Run baseline simulation and get flux map
# ---------------------------------------------------------------------------
print("Running baseline simulation...")
assert cp.simulate(r), "Simulation failed."

flux_baseline = cp.get_fluxmap(r)
print(f"  Baseline peak flux: {max(max(row) for row in flux_baseline):.2f} kW/m²")

# ---------------------------------------------------------------------------
# 7. Modify aiming points dynamically and re-simulate
#    This is the core pattern for time-varying heliostat control studies.
# ---------------------------------------------------------------------------
print("\nModifying aiming points for a subset of heliostats...")

# Get current heliostat details
res = cp.detail_results(r)
tower_height = cp.data_get_number(r, "solarfield.0.tht")

# Select first 20 heliostats and shift their aiming points upward by 2 m
n_modified = 20
helio_dict = {}
helio_dict["id"]         = list(res["id"].iloc[:n_modified])
helio_dict["aimpoint-x"] = [0.0] * n_modified           # centered horizontally
helio_dict["aimpoint-z"] = [tower_height + 2.0] * n_modified  # 2 m above nominal

# IMPORTANT: aim method must be "Keep existing" when modifying aiming points
assert cp.data_set_string(r, "fluxsim.0.aim_method", "Keep existing"), \
    "Failed to set aim method."
assert cp.modify_heliostats(r, helio_dict), \
    "Failed to modify heliostat aiming points."

print(f"  Modified {n_modified} heliostats — aimpoint-z shifted to {tower_height + 2.0:.1f} m")

# Re-simulate with new aiming points
print("Re-simulating with modified aiming points...")
assert cp.simulate(r), "Re-simulation failed."

flux_modified = cp.get_fluxmap(r)
print(f"  Modified peak flux: {max(max(row) for row in flux_modified):.2f} kW/m²")

# ---------------------------------------------------------------------------
# 8. Plot results
# ---------------------------------------------------------------------------
fig, axes = plt.subplots(1, 3, figsize=(14, 4))

# Solar field layout
axes[0].scatter(field["x_location"], field["y_location"], s=1.5)
axes[0].set_title("Heliostat Field Layout")
axes[0].set_xlabel("x (m)")
axes[0].set_ylabel("y (m)")
axes[0].set_aspect("equal")

# Baseline flux map
im1 = axes[1].imshow(flux_baseline, origin="upper")
axes[1].set_title("Baseline Flux Map")
plt.colorbar(im1, ax=axes[1], label="kW/m²")

# Modified flux map
im2 = axes[2].imshow(flux_modified, origin="upper")
axes[2].set_title("Modified Aiming Points Flux Map")
plt.colorbar(im2, ax=axes[2], label="kW/m²")

plt.tight_layout()
plt.savefig("solarpilot_example_output.png", dpi=150)
print("\nPlot saved to: solarpilot_example_output.png")
plt.show()

# ---------------------------------------------------------------------------
# 9. Free memory
# ---------------------------------------------------------------------------
assert cp.data_free(r), "Failed to free SolarPILOT data."
print("\nDone.")
