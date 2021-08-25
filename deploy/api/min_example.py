import matplotlib.pyplot as plt
from copylot import CoPylot

## Minimum working example -> Must update path to weather file
cp = CoPylot()
r = cp.data_create()
assert cp.data_set_string(
        r,
        "ambient.0.weather_file",
        "../climate_files/USA CA Daggett (TMY2).csv",
    )
assert cp.generate_layout(r)
field = cp.get_layout_info(r)
assert cp.simulate(r)
flux = cp.get_fluxmap(r)
assert cp.data_free(r)

# Plotting (default) solar field and flux map
# Solar Field
plt.scatter(field['x_location'], field['y_location'], s=1.5)
plt.tight_layout()
plt.show()

# flux
im = plt.imshow(flux)
plt.colorbar(im)
plt.tight_layout()
plt.show()




