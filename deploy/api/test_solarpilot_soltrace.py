# -----------------------------------------------------------------
# Example to run solarpilot case and transfer it to soltrace directly
# through each program's Python API. This requires that both the 
# coretrace_api.* and solarpilot.* libraries be present in the run
# directory or specified in the python path. 
# -----------------------------------------------------------------
if __name__ == "__main__":  #<< guard needed to run soltrace multi-threaded
    import numpy as np
    import matplotlib.pyplot as plt
    from copylot import CoPylot
    import math
    import pysoltrace

    cp = CoPylot()

    spcxt = cp.data_create()        #creates data context. pass this parameter to each solarpilot api call
    cp.api_callback_create(spcxt) 

    cp.data_set_string(spcxt, "ambient.0.weather_file", "USA CA Daggett (TMY2).csv")
    cp.data_set_number(spcxt, "solarfield.0.q_des", 100)
    cp.data_set_number(spcxt, "solarfield.0.tht", 90)
    cp.data_set_number(spcxt, "heliostat.0.is_faceted", 0)
    cp.data_set_number(spcxt, "fluxsim.0.y_res", 20)
    cp.data_set_number(spcxt, "receiver.0.rec_height", 15)
    cp.data_set_number(spcxt, "receiver.0.rec_diameter", 10)

    cp.generate_layout(spcxt)

    cp.data_set_number(spcxt, "fluxsim.0.flux_time_type", 0)
    cp.data_set_number(spcxt, "fluxsim.0.flux_solar_az_in", 270)
    cp.data_set_number(spcxt, "fluxsim.0.flux_solar_el_in", 70)

    # run analytical simluation
    cp.simulate(spcxt)
    # weather = cp.generate_simulation_days(spcxt)

    flux = cp.get_fluxmap(spcxt)

    h_rec = cp.data_get_number(spcxt, "receiver.0.rec_height")
    d_rec = cp.data_get_number(spcxt, "receiver.0.rec_diameter")

    ny = len(flux)
    nx = len(flux[0])

    y_rec = np.arange(0, h_rec, h_rec/ny)
    x_rec = np.arange(0, np.pi*d_rec, np.pi*d_rec/nx)

    Xr,Yr = np.meshgrid(x_rec, y_rec)

    plt.title("Flux simulation from the analytical engine")
    plt.contourf(Xr, Yr, flux, levels=25)
    plt.colorbar()
    plt.title(f"max flux {max(max(flux)):.0f} kW/m2, mean flux {sum(sum(l) for l in flux)/(len(flux)*len(flux[0])):.1f}")
    plt.show()
    # ------------------------------------------
    # Export to soltrace using built-in solarpilot function
    cp.export_soltrace(spcxt, "test-soltrace.stinput")
    # 
    st = cp.load_soltrace_structure(spcxt)

    # here, modify the receiver 
    # element = st.stages[1].add_element()
    # element.surface = 'l'
    # element.surface_params[2] = cp.data_get_number(spcxt, "receiver.0.rec_height")
    # etc...



    st.num_ray_hits = 1e5
    st.max_rays_traced = st.num_ray_hits*100
    st.write_soltrace_input_file("test-soltrace-pysoltrace.stinput")
    st.run(-1, True, 10)

    # Get a pandas dataframe with all of the ray data
    df = st.raydata

    dfr = df[df.stage==2]
    dfr = dfr[dfr.element==-1]  #absorbed rays

    tht = cp.data_get_number(spcxt, "solarfield.0.tht")
    dfr['zpos'] = dfr.loc_z - tht + h_rec/2.
    dfr['cpos'] = (np.arctan2(dfr.loc_x, dfr.loc_y)+math.pi)*d_rec/2.

    flux_st = np.zeros((ny,nx))
    dx = d_rec*np.pi / nx 
    dy = h_rec / ny
    anode = dx*dy
    ppr = st.powerperray / anode *1e-3

    for ind,ray in dfr.iterrows():

        j = int(ray.cpos/dx)
        i = int(ray.zpos/dy)

        flux_st[i,j] += ppr
    print(dfr.describe())
    plt.figure()
    plt.title("Flux simulation from the SolTrace engine")
    plt.contourf(Xr, Yr, flux_st, levels=25)
    plt.colorbar()
    plt.title(f"max flux {flux_st.max():.0f} kW/m2, mean flux {flux_st.mean():.1f}")
    plt.show()

    assert cp.data_free(spcxt)

    # --------------------------------------------------------------------
    try:
        import plotly.graph_objects as go

        # Must run a simulation to get aim points set

        # -----------
        # Collect and plot ray data
        # Free memory associated with solarpilot case

        # Data for a three-dimensional line
        import random
        maxrays = int(1e5)
        inds = list(range(len(df)))
        if maxrays < len(inds):
            ind_use = []
            nileft = len(inds)
            for i in range(maxrays):
                ii = inds.pop(random.randint(0,len(inds)-1))
                ind_use.append( ii )
                nileft = len(inds)
                if nileft == 0:
                    break
            ind_use.sort()
        else:
            ind_use = inds
        dfs = df.iloc[ind_use]

        loc_x = dfs.loc_x.values
        loc_y = dfs.loc_y.values
        loc_z = dfs.loc_z.values

        # Create plotly figure for ray hits
        fig = go.Figure(data=go.Scatter3d(x=loc_x, y=loc_y, z=loc_z, mode='markers', marker=dict( size=0.5, color=df.stage, colorscale='bluered', opacity=0.8, ) ) )

        # Show traces for a range of rays, specified in range() function
        for i in range(50,250):
            dfi = dfs[dfs.number == i]
            ray_x = dfi.loc_x 
            ray_y = dfi.loc_y
            ray_z = dfi.loc_z
            raynum = dfi.number
            fig.add_trace(go.Scatter3d(x=ray_x, y=ray_y, z=ray_z, mode='lines', line=dict(color='orange', width=0.5)))

        fig.update_layout(showlegend=False, scene_aspectmode='data')

        fig.show()
    except ImportError:
        print("Cannot generate spatial ray hit map: plotly not installed")