# -----------------------------------------------------------------
# Example to run solarpilot case and transfer it to soltrace directly
# through each program's Python API. This requires that both the 
# coretrace_api.* and solarpilot.* libraries be present in the run
# directory or specified in the python path. 
# -----------------------------------------------------------------

import os
from pysoltrace import PySolTrace
from copylot import CoPylot

import plotly.graph_objects as go

print("Process ID: ", os.getpid())

cp = CoPylot(debug=False)

## Minimum working example -> Must update path to weather file
spcxt = cp.data_create()        #creates data context. pass this parameter to each solarpilot api call
cp.api_callback_create(spcxt) 
cp.data_set_string(spcxt, "ambient.0.weather_file", "../climate_files/USA CA Daggett (TMY2).csv")
cp.data_set_number(spcxt, 'heliostat.0.is_faceted', 0)

# Generate a new heliostat field layout
assert cp.generate_layout(spcxt)
# field = cp.get_layout_info(spcxt)

# Select a region of heliostats in order to modify them
hmod = cp.heliostats_by_region(spcxt, 'cylindrical', arguments=[0, 2000, 0, 3.14])  #select the entire west side of the field
# Modify the reflectivity of the heliostat to act like a cloud
cp.modify_heliostats(spcxt, {'id':hmod.id.values, 'reflectivity':[0. for i in range(len(hmod.id))]})

# Must run a simulation to get aim points set
assert cp.simulate(spcxt)  
# flux = cp.get_fluxmap(spcxt)
# weather = cp.generate_simulation_days(spcxt)

# ------------------------------------------------------------------

# Create a pysoltrace instance
pscxt = PySolTrace()
# Write the solarpilot layout to a soltrace case file
cp.export_soltrace(spcxt, "test-soltrace.stinput")

# OR
# Load the solarpilot case geometry directly into a new soltrace API context
cp.load_soltrace_context(spcxt, pscxt._p_data)

# set simulation parameters
pscxt.num_ray_hits = 1e5
pscxt.max_rays_traced = pscxt.num_ray_hits*100
pscxt.is_sunshape = True 
pscxt.is_surface_errors = True
# Run the soltrace simulation
pscxt.run(-1, True)

# Get a pandas dataframe with all of the ray data
df = pscxt.get_ray_dataframe()

pscxt.clear_context()   #free the memory associated with soltrace

# -----------
# Collect and plot ray data

# Data for a three-dimensional line
loc_x = df.loc_x.values
loc_y = df.loc_y.values
loc_z = df.loc_z.values

# Create plotly figure for ray hits
fig = go.Figure(data=go.Scatter3d(x=loc_x, y=loc_y, z=loc_z, mode='markers', marker=dict( size=1, color=df.stage, colorscale='bluered', opacity=0.8, ) ) )

# Show traces for a range of rays, specified in range() function
for i in range(50,250):
    dfr = df[df.number == i]
    ray_x = dfr.loc_x 
    ray_y = dfr.loc_y
    ray_z = dfr.loc_z
    raynum = dfr.number
    fig.add_trace(go.Scatter3d(x=ray_x, y=ray_y, z=ray_z, mode='lines', line=dict(color='black', width=0.5)))

fig.update_layout(showlegend=False, scene_aspectmode='data')

fig.show()

# Free memory associated with solarpilot case
assert cp.data_free(spcxt)