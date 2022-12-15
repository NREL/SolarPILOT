import os, csv
from copylot import CoPylot
import time
import numpy as np
import matplotlib.pyplot as plt

plt.rcParams['font.size'] = 14
plt.rcParams['font.sans-serif'] = 'Arial'
plt.rcParams['axes.linewidth'] = 2.5
plt.rcParams['xtick.major.width'] = 1.5
plt.rcParams['ytick.major.width'] = 1.5

eff_filename = "efficiency_table.csv"

cp = CoPylot(debug = False)
print("Process ID: ", os.getpid())
r = cp.data_create()
assert cp.data_set_string(
        r,
        "ambient.0.weather_file",
        "../climate_files/USA CA Daggett (TMY2).csv",
    )
assert cp.generate_layout(r)
# field = cp.get_layout_info(r)
# assert cp.simulate(r)
# flux = cp.get_fluxmap(r)

start = time.time()
assert cp.calculate_optical_efficiency_table(r, 12, 11)
# assert cp.calculate_optical_efficiency_table(r, 2, 2)  # For debugging
end = time.time()
print("Efficiency computation required {:.2f} seconds".format(end-start))

# Testing getting optical efficiency table after calculation
cwd = os.getcwd()
assert cp.save_optical_efficiency_table(r, os.path.join(cwd, eff_filename)) # Testing simple table saving (consistent with get_optical_efficiency_table)
assert cp.save_optical_efficiency_table(r, os.path.join(cwd, "modelica_efficiency_table.txt"), "sf_eff_table")  # Testing Modelica specific table output
sf_eff = cp.get_optical_efficiency_table(r)

start = time.time()
direct = cp.calculate_get_optical_efficiency_table(r, 12, 11) # calculate and get
end = time.time()
print("Direct Calculate and get efficiency required {:.2f} seconds".format(end-start))

assert cp.data_free(r)

# Read in efficiency table
eff_data = []
el = []
azi = []
with open(eff_filename) as f:
    csvReader = csv.reader(f)
    for row in csvReader:
        if el == []:
            el.extend([float(i) for i in row])
            el.pop(0) # pop off the first element
        else:
            azi.append(float(row[0]))
            row.pop(0)
            eff_data.append([float(i) for i in row])

# Check data is consistent between two methods
for a in range(len(sf_eff['azimuth'])):
    assert azi[a] == round(sf_eff['azimuth'][a], 4)
    assert direct['azimuth'][a] == sf_eff['azimuth'][a], 4

for e in range(len(sf_eff['elevation'])):
    assert el[e] == round(sf_eff['elevation'][e], 4)
    assert direct['elevation'][e] == sf_eff['elevation'][e]

for a in range(len(sf_eff['azimuth'])):
    for e in range(len(sf_eff['elevation'])):
        assert eff_data[a][e] == round(sf_eff['eff_data'][a][e], 6)
        assert direct['eff_data'][a][e] == sf_eff['eff_data'][a][e]

# Transpose data for plotting
sf_eff['eff_data_T'] = np.array(sf_eff['eff_data']).T.tolist()

# Plot efficiency data
fig, (ax1, ax2) = plt.subplots(1, 2)
for a in range(len(sf_eff['azimuth'])):
    ax1.plot(sf_eff['elevation'], sf_eff['eff_data'][a], label=str(sf_eff['azimuth'][a]))
ax1.legend(title='Azimuth Angle [deg]')
ax1.set_xlabel('Elevation Angle [deg]')
ax1.set_ylabel('Solarfield Efficiency [-]')

for e in range(len(sf_eff['elevation'])):
    ax2.plot(sf_eff['azimuth'], sf_eff['eff_data_T'][e], label=str(sf_eff['elevation'][e]))
ax2.legend(title='Elevation Angle [deg]')
ax2.set_xlabel('Azimuth Angle [deg]')
ax1.set_ylabel('Solarfield Efficiency [-]')

plt.tight_layout()
plt.show()

