import os, csv
import pandas as pd
import matplotlib.pyplot as plt

from copylot import CoPylot

cp = CoPylot()
print("Process ID: ", os.getpid())

if False:
    ## Minimum working example -> Must update path to weather file
    r = cp.data_create()
    cp.data_set_string(
            r,
            "ambient.0.weather_file",
            "C:/Users/WHamilt2/Documents/solarPILOT_build/SolarPILOT/deploy/climate_files/USA CA Daggett (TMY2).csv",
        )       # TODO: requires weather file to be specified, Should fix this before the release???
    print(cp.generate_layout(r))
    field = cp.get_layout_info(r)
    print(cp.simulate(r))  
    flux = cp.get_fluxmap(r)
    cp.data_free(r)

    # Plotting (default) solar field and flux map
    if False:
        # Solar Field
        plt.scatter(field['x_location'], field['y_location'], s=1.5)
        plt.tight_layout()
        plt.show()

        # flux
        im = plt.imshow(flux)
        plt.colorbar(im)
        plt.tight_layout()
        plt.show()

#### New SolarPILOT instances
r = cp.data_create()

print(cp.version(r))    # Version number
cp.api_callback_create(r)   # callback
#cp.api_disable_callback(r) - tested

if False:
    ## Testing set and get number - Pass
    cp.data_set_number(r, "solarfield.0.dni_des", 1111.0)
    print(cp.data_get_number(r, "solarfield.0.dni_des"))
    ## Testing set and get boolean - Pass
    print(cp.data_get_number(r, "solarfield.0.is_sliprow_skipped"))
    cp.data_set_number(r, "solarfield.0.is_sliprow_skipped", True)         # Works with True and False
    print(cp.data_get_number(r, "solarfield.0.is_sliprow_skipped"))
    cp.data_set_number(r, "solarfield.0.is_sliprow_skipped", 0.)         # Works with 1 and 0 
    print(cp.data_get_number(r, "solarfield.0.is_sliprow_skipped"))

if False:
    ## Testing set and get string - Pass
    cp.data_set_string(r, "ambient.0.loc_state", "Mind")  # - Pass
    print(cp.data_get_string(r, "ambient.0.loc_state"))  # - Pass
    cp.data_set_string(r, "ambient.0.sun_csr", "0.05")  # - Not a string variable - Pass

if False:
    ## Testing setting and getting Combo variables - Through string - Pass
    print(cp.data_get_string(r, "ambient.0.sun_type"))
    cp.data_set_string(r, "ambient.0.sun_type", "Gaussian sun")  # Combo - acceptable option - Pass
    print(cp.data_get_string(r, "ambient.0.sun_type"))
    cp.data_set_string(r, "ambient.0.sun_type", "ussian s")  # Combo - not acceptable option - Pass
    print(cp.data_get_string(r, "ambient.0.sun_type"))
    ## Testing setting and getting Combo variables - Through number - Pass
    print(cp.data_get_string(r, "ambient.0.insol_type"))    # get string of combo choice
    cp.data_set_number(r, "ambient.0.insol_type", 1)  # Combo - acceptable option - Pass
    print(cp.data_get_number(r, "ambient.0.insol_type"))    # get number of combo choice
    cp.data_set_number(r, "ambient.0.insol_type", 5)  # Combo - not acceptable option - Pass
    print(cp.data_get_string(r, "ambient.0.insol_type"))

if False:
    ## Testing set and get array - Pass
    cp.data_set_array_from_csv(r, "financial.0.pmt_factors", "test_array.csv")
    print(cp.data_get_array(r, "financial.0.pmt_factors"))
    cp.data_set_array(r, "financial.0.pmt_factors", [1.,2.,3.,4.,5.,6.,7.,8.,9.])
    print(cp.data_get_array(r, "financial.0.pmt_factors"))
    print(cp.data_get_array(r, "financial.0.schedule_array"))
    cp.data_set_array(r, "financial.0.schedule_array", [1.,2.,3.,4.,5.,6.,7.,8.,9.]) # testing set integer array - Pass
    print(cp.data_get_array(r, "financial.0.schedule_array"))
    

if False:
    ## Testing set and get matrix - Pass
    cp.data_set_matrix(r, "ambient.0.atm_coefs", [[1, 1, 1, 1], [2, 2, 2, 2], [3, 3, 3, 3]])
    print(cp.data_get_matrix(r, "ambient.0.atm_coefs"))
    cp.data_set_matrix_from_csv(r, "ambient.0.atm_coefs", "test_matrix.csv")
    print(cp.data_get_matrix(r, "ambient.0.atm_coefs"))

if False:
    ## Testing variable reset - Pass
    cp.reset_vars(r)
    print(cp.data_get_number(r, "solarfield.0.dni_des"))
    print(cp.data_get_array(r, "financial.0.pmt_factors"))
    print(cp.data_get_matrix(r, "ambient.0.atm_coefs"))

if False:
    ## Testing add and drop receiver - Pass
    print(cp.add_receiver(r, "test_rec"))
    print(cp.add_receiver(r, "test_rec"))
    print(cp.drop_receiver(r, "test_rec"))
    print(cp.drop_receiver(r, "test_re"))

if False:
    ## Testing add and drop heliostat template - Pass
    print(cp.add_heliostat_template(r, "test_helio"))
    print(cp.add_heliostat_template(r, "test_helio"))
    print(cp.drop_heliostat_template(r, "test_helio"))
    print(cp.drop_heliostat_template(r, "test_heli"))

## Testing generate - Pass, assign - Pass, and update layout
if True:
    cp.reset_vars(r)
    cp.data_set_number(r, "solarfield.0.q_des", 100)       # Small field for debugging
    cp.data_set_string(r, "receiver.0.rec_type", "Flat plate")
    cp.data_set_string(
        r,
        "ambient.0.weather_file",
        "C:/Users/WHamilt2/Documents/solarPILOT_build/SolarPILOT/deploy/climate_files/USA CA Daggett (TMY2).csv",
    )

    # generate - Pass
    if True:
        cp.data_set_string(r, "solarfield.0.des_sim_detail", "Single simulation point")  # to speed up solution time for testing
        cp.data_set_number(r, "solarfield.0.is_sliprow_skipped", True)
        cp.data_set_number(r, "solarfield.0.slip_plane_blocking", 0.0)
        ## If you add a another template and do not choose a temp_which -> ERROR
        # print( cp.add_heliostat_template(r, "test_helio"))
        ## Requires setting heliostat template to use for field generation
        # cp.data_set_string(r, "solarfield.0.temp_which", "Template 1")
        # Test - Use single template - Pass
        # Test - Specified range - Pass
        # print( cp.add_heliostat_template(r, "Template 2") )
        # cp.data_set_number(r, "heliostat.0.height", 6.)
        # cp.data_set_number(r, "heliostat.0.width", 6.)
        # cp.data_set_number(r, "heliostat.0.temp_rad_max", 500)
        # cp.data_set_number(r, "heliostat.1.temp_rad_min", 500)
        # cp.data_set_number(r, "heliostat.1.temp_rad_max", 2000)
        # cp.data_set_string(r, "solarfield.0.template_rule", "Specified range")
        # Test - Even radial distribution - Pass
        # cp.add_heliostat_template(r, "Template 2")
        # cp.data_set_number(r, "heliostat.0.height", 6.)
        # cp.data_set_number(r, "heliostat.0.width", 6.)
        # cp.data_set_string(r, "solarfield.0.template_rule", "Even radial distribution")
        print(cp.generate_layout(r, nthreads=4))  # Testing - Pass

    # assign
    if True:
        # pulling in heliostat locations
        helio_data = []
        with open("test_field.csv") as csvDataFile:
            csvReader = csv.reader(csvDataFile)
            for row in csvReader:
                helio_data.append([float(i) for i in row])
        print(cp.assign_layout(r, helio_data))  # Testing - Pass

    if True:
        ## Trying soltrace simulation
        # cp.data_set_string(r, "fluxsim.0.flux_model", "SolTrace") # Tested
        cp.data_set_number(r, "fluxsim.0.x_res", 45)
        cp.data_set_number(r, "fluxsim.0.y_res", 30)
        
        print(cp.simulate(r))  # Testing - Pass
        flux = cp.get_fluxmap(r)  # Testing - Pass

    # Plotting flux map
    if True:
        im = plt.imshow(flux)
        plt.colorbar(im)
        plt.tight_layout()
        plt.show()

    # Update field
    if True:
        ##check = cp.add_land(r, b'exclus', [[1000, 1000],[500,1000], [500,500], [1000, 500]]) # - Testing - Pass
        if True:
            helio_dict = {}
            helio_dict["id"] = [4166, 4000]
            helio_dict["location-x"] = [1500, -1500]
            helio_dict["location-y"] = [1500, 1500]
            helio_dict["soiling"] = [0.5, 0.3]
            helio_dict["reflectivity"] = [0.2, 0.8]
            helio_dict["enabled"] = [1, 0]
            print( cp.modify_heliostats(r, helio_dict ))  # TODO: Need to discuss with Mike the solution
            ## This needs to be fixed for both general api and lk api

        if True:
            # find max flux of original geometry
            print(cp.simulate(r))  # Testing - Pass
            flux = cp.get_fluxmap(r)
            maxflux_befc = max(max(flux))

            # Change geometry, simulate, find max flux
            cp.data_set_number(r, "heliostat.0.height", 11.0)
            print(cp.simulate(r))
            flux = cp.get_fluxmap(r)
            maxflux_aftc_noUP = max(max(flux))

            # Update, simulate, and find max flux
            print(cp.update_geometry(r))  # Testing - Pass
            print(cp.simulate(r))
            flux = cp.get_fluxmap(r)
            maxflux_aftu = max(max(flux))
            
    if True:
        res1 = cp.summary_results(r, save_dict=True)  # Testing - Pass
        # res, header = cp.detail_results(r, restype = 'matrix')     # returns (matrix, header) - Pass
        # res = cp.detail_results(r, restype = 'matrix')             # returns (matrix, header) - Pass
        # res = cp.detail_results(r, restype = 'dictionary')         # returns dictionary - Pass
        res2 = cp.detail_results(r, get_corners=True)  # returns dataframe - Pass
        res3 = cp.heliostats_by_region(r, coor_sys="polygon", arguments=[[500, 500], [1000, 1000], [500, 1000]])  # tested all, cylindrical, cartesian, and polygon - Pass

        if True:
            ## Testing modify_heliostats by disabling half the field and re-simulating - Pass
            helio_dict = {}
            #helio_dict["id"] = list(res2.id.iloc[0:int(len(res2)*2/3)])
            ## disabling all but 5 heliostats
            helio_dict["id"] = list(res2.id.iloc[0:300])
            helio_dict["id"].extend(list(res2.id.iloc[305:len(res2)]))
            helio_dict["enabled"] = [0]*len(helio_dict["id"])
            print( cp.modify_heliostats(r, helio_dict )) # Testing - Pass
            print(cp.simulate(r))  # Testing - Pass

    # Pulling field data
    field,header = cp.get_layout_info(r, get_corners=True, restype="matrix")  # Testing - Pass

    # cp.clear_land(r, clear_type=b'inclusion') # Testing - Pass

    # Is there a way to create a function to create a field map plot and save it to a image file?
    # - No, currently this functionality lives in UI
    # - We would have to duplicate functionality in python using matplotlib
    # Plotting solarfield layout
    if True:
        x = []
        y = []
        for i in range(len(field)):
            x.append(field[i][1])
            y.append(field[i][2])
        plt.scatter(x, y, s=1.5)
        plt.tight_layout()
        plt.show()

    flux = cp.get_fluxmap(r)  # Testing - Pass
    # Plotting flux map
    if True:
        im = plt.imshow(flux)
        plt.colorbar(im)
        plt.tight_layout()
        plt.show()

# Testing dump_varmap and save_from script - Pass
if True:
    cwd = os.getcwd()
    print(cp.dump_varmap_tofile(r, "C:/Users/WHamilt2/Documents/solarPILOT_build/SolarPILOT/deploy/api/varmap_dump.csv"))
    # print( cp.dump_varmap_tofile(r, "varmap_dump_v2.csv") ) # This does not work - must provide full path
    print(cp.dump_varmap_tofile(r, cwd + "\\varmap_dump_v3.csv"))  # This works
    print(cp.save_from_script( r, "C:/Users/WHamilt2/Documents/solarPILOT_build/SolarPILOT/deploy/api/case_study.spt"))
    print(cp.save_from_script(r, cwd + "\\case_study_v2.spt"))  # This works

cp.data_free(r) # Works - free memory
