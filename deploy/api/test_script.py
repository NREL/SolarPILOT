import os, csv
import matplotlib.pyplot as plt

from copylot import CoPylot

plot_results = True
test_solTrace_sim = True
cp = CoPylot()
print("Process ID: ", os.getpid())

if True:
    ## Minimum working example -> Must update path to weather file
    r = cp.data_create()
    cp.api_callback_create(r) 
    assert cp.data_set_string(
            r,
            "ambient.0.weather_file",
            "../climate_files/USA CA Daggett (TMY2).csv")
    assert cp.generate_layout(r)
    field = cp.get_layout_info(r)
    assert cp.simulate(r)  
    flux = cp.get_fluxmap(r)
    assert cp.data_free(r)

    # Plotting (default) solar field and flux map
    if plot_results:
        # Solar Field
        plt.scatter(field['x_location'], field['y_location'], s=1.5)
        plt.tight_layout()
        plt.show()

        # flux
        im = plt.imshow(flux)
        plt.colorbar(im)
        plt.tight_layout()
        plt.show()

r = cp.data_create()  # New SolarPILOT instance

print(cp.version(r))    # Version number
cp.api_callback_create(r)   # callback
#cp.api_disable_callback(r) - tested

if True:
    ## Testing set and get number - Pass
    assert cp.data_set_number(r, "solarfield.0.dni_des", 1111.0)
    assert cp.data_get_number(r, "solarfield.0.dni_des") == 1111.0
    ## Testing set and get boolean - Pass
    assert cp.data_set_number(r, "solarfield.0.is_sliprow_skipped", True)         # Works with True and False
    assert cp.data_get_number(r, "solarfield.0.is_sliprow_skipped")
    assert cp.data_set_number(r, "solarfield.0.is_sliprow_skipped", 0.)         # Works with 1 and 0 
    assert not cp.data_get_number(r, "solarfield.0.is_sliprow_skipped")

if True:
    ## Testing set and get string - Pass
    assert cp.data_set_string(r, "ambient.0.loc_state", "Mind")  # - Pass
    assert cp.data_get_string(r, "ambient.0.loc_state") == "Mind"  # - Pass
    assert not cp.data_set_string(r, "ambient.0.sun_csr", "0.05") # - Not a string variable - Pass

if True:
    ## Testing setting and getting Combo variables - Through string - Pass
    assert cp.data_get_string(r, "ambient.0.sun_type") == 'Limb-darkened sun'  # Default value
    assert cp.data_set_string(r, "ambient.0.sun_type", "Gaussian sun")  # Combo - acceptable option - Pass
    assert cp.data_get_string(r, "ambient.0.sun_type") == "Gaussian sun"
    assert not cp.data_set_string(r, "ambient.0.sun_type", "ussian s")  # Combo - not acceptable option - Pass
    assert cp.data_get_string(r, "ambient.0.sun_type") == "Gaussian sun"  # Value should not change
    ## Testing setting and getting Combo variables - Through number - Pass
    assert cp.data_get_string(r, "ambient.0.insol_type") == 'Weather file data'    # get string of combo choice
    assert cp.data_set_number(r, "ambient.0.insol_type", 1)  # Combo - acceptable option - Pass
    assert cp.data_get_number(r, "ambient.0.insol_type") == 1    # get number of combo choice
    assert cp.data_get_string(r, "ambient.0.insol_type") == 'Hottel model'    # get string of combo choice
    assert not cp.data_set_number(r, "ambient.0.insol_type", 5)  # Combo - not acceptable option - Pass
    assert cp.data_get_string(r, "ambient.0.insol_type") == 'Hottel model'  # Value should not change

if True:
    ## Testing set and get array - Pass
    assert cp.data_set_array_from_csv(r, "financial.0.pmt_factors", "test_array.csv")
    # print(cp.data_get_array(r, "financial.0.pmt_factors"))  # Visual check values
    assert len(cp.data_get_array(r, "financial.0.pmt_factors")) == 9
    set_vector = [1.,2.,3.,4.,5.]
    assert cp.data_set_array(r, "financial.0.pmt_factors", set_vector)
    ret_vector = cp.data_get_array(r, "financial.0.pmt_factors")
    for i, val in enumerate(set_vector):
        assert val == ret_vector[i]
    assert len(cp.data_get_array(r, "financial.0.schedule_array")) == 8760
    assert cp.data_set_array(r, "financial.0.schedule_array", set_vector) # testing set integer array - Pass
    ret_vector = cp.data_get_array(r, "financial.0.schedule_array")
    for i, val in enumerate(set_vector):
        assert val == ret_vector[i]

if True:
    ## Testing set and get matrix - Pass
    set_maxtrix = [[1, 1, 1, 1, 1], [2, 2, 2, 2, 2], [3, 3, 3, 3, 3], [4, 4, 4, 4, 4]]
    assert cp.data_set_matrix(r, "ambient.0.atm_coefs", set_maxtrix)
    ret_matrix = cp.data_get_matrix(r, "ambient.0.atm_coefs")
    for i, _ in enumerate(set_maxtrix):
        for j, val in enumerate(set_maxtrix[i]):
            assert val == ret_matrix[i][j]
    
    assert cp.data_set_matrix_from_csv(r, "ambient.0.atm_coefs", "test_matrix.csv")
    ret_matrix = cp.data_get_matrix(r, "ambient.0.atm_coefs")
    assert len(ret_matrix) == 3
    assert len(ret_matrix[0]) == 3
    #print(cp.data_get_matrix(r, "ambient.0.atm_coefs"))  # Visual check values

if True:
    ## Testing variable reset - Pass
    cp.reset_vars(r)
    assert cp.data_get_number(r, "solarfield.0.dni_des") == 950

if True:
    ## Testing add and drop receiver - Pass
    assert cp.add_receiver(r, "test_rec") == 1  # Second Receiver 
    assert cp.add_receiver(r, "test_rec") < 0  # can't add another receiver with the same name
    assert cp.drop_receiver(r, "test_rec")
    assert not cp.drop_receiver(r, "test_re")

if True:
    ## Testing add and drop heliostat template - Pass
    assert cp.add_heliostat_template(r, "test_helio") == 1  # second heliostat template
    assert cp.add_heliostat_template(r, "test_helio") < 0   # can't add another heliostat template with the same name
    assert cp.drop_heliostat_template(r, "test_helio")
    assert not cp.drop_heliostat_template(r, "test_heli")

## Testing generate - Pass, assign - Pass, and update layout
if True:
    cp.reset_vars(r)
    assert cp.data_set_number(r, "solarfield.0.q_des", 100)       # Small field for debugging
    assert cp.data_set_string(r, "receiver.0.rec_type", "Flat plate")
    assert cp.data_set_string(
        r,
        "ambient.0.weather_file",
        "../climate_files/USA CA Daggett (TMY2).csv",
    )

    # generate - Pass
    if True:
        assert cp.data_set_string(r, "solarfield.0.des_sim_detail", "Single simulation point")  # to speed up solution time for testing
        assert cp.data_set_number(r, "solarfield.0.is_sliprow_skipped", True)
        assert cp.data_set_number(r, "solarfield.0.slip_plane_blocking", 0.0)
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
        assert cp.generate_layout(r, nthreads=4)  # Testing - Pass

    # assign
    if True:
        # pulling in heliostat locations
        helio_data = []
        with open("test_field.csv") as csvDataFile:
            csvReader = csv.reader(csvDataFile)
            for row in csvReader:
                helio_data.append([float(i) for i in row])
        assert cp.assign_layout(r, helio_data)  # Testing - Pass
    
    # Changing flux map resolution    
    assert cp.data_set_number(r, "fluxsim.0.x_res", 45)
    assert cp.data_set_number(r, "fluxsim.0.y_res", 30)

    assert cp.simulate(r)
    flux_hermite = cp.get_fluxmap(r)  # Testing - Pass
    
    # Plotting flux map
    if plot_results:
        im = plt.imshow(flux_hermite)
        plt.title('Hermite Results')
        plt.colorbar(im)
        plt.tight_layout()
        plt.show()

    ## SolTrace simulation
    if test_solTrace_sim: 
        print("="*10 + " Testing SolTrace " + "="*10)
        assert cp.data_set_string(r, "fluxsim.0.flux_model", "SolTrace") # Tested
        assert cp.data_set_string(r, "fluxsim.0.aim_method", "Keep existing")
        assert cp.data_set_number(r, "fluxsim.0.max_rays", 100000000)
        assert cp.data_set_number(r, "fluxsim.0.min_rays", 1000000)
        assert cp.simulate(r)  # Testing - Pass
        flux_ST = cp.get_fluxmap(r)  # Testing - Pass
        # Change back for rest of tests
        assert cp.data_set_string(r, "fluxsim.0.flux_model", "Hermite (analytical)")
        assert cp.data_set_string(r, "fluxsim.0.aim_method", "Image size priority")

    	# Plotting flux map
        if plot_results:
            im = plt.imshow(flux_ST)
            plt.title('SolTrace Results')
            plt.colorbar(im)
            plt.tight_layout()
            plt.show()

    # Update field
    if True:
        ##check = cp.add_land(r, b'exclus', [[1000, 1000],[500,1000], [500,500], [1000, 500]]) # - Testing - Pass
        if True:
            res = cp.detail_results(r)
            helio_dict = {}
            helio_dict["id"] = [res.id[0], res.id[1]]
            helio_dict["location-x"] = [1500, -1500]
            helio_dict["location-y"] = [1500, 1500]
            helio_dict["soiling"] = [0.5, 0.3]
            helio_dict["reflectivity"] = [0.2, 0.8]
            helio_dict["enabled"] = [1, 0]
            assert cp.modify_heliostats(r, helio_dict)

        if True:

            # find max flux of original geometry
            assert cp.simulate(r)  # Testing - Pass
            flux = cp.get_fluxmap(r)
            maxflux_befc = max(max(flux))

            # Change geometry, simulate, find max flux
            assert cp.data_set_number(r, "heliostat.0.height", 11.0)
            assert cp.simulate(r)
            flux = cp.get_fluxmap(r)
            maxflux_aftc_noUP = max(max(flux))

            # Update, simulate, and find max flux
            assert cp.update_geometry(r)  # Testing - Pass
            assert cp.simulate(r)
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
            assert cp.modify_heliostats(r, helio_dict ) # Testing - Pass
            assert cp.simulate(r)  # Testing - Pass

    # Pulling field data
    field,header = cp.get_layout_info(r, get_corners=True, restype="matrix")  # Testing - Pass

    # cp.clear_land(r, clear_type=b'inclusion') # Testing - Pass

    # Is there a way to create a function to create a field map plot and save it to a image file?
    # - No, currently this functionality lives in UI
    # - We would have to duplicate functionality in python
    # Plotting solarfield layout
    if plot_results:
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
    if plot_results:
        im = plt.imshow(flux)
        plt.colorbar(im)
        plt.tight_layout()
        plt.show()

# Testing dump_varmap and save_from script - Pass
if True:
    cwd = os.getcwd()
    assert not cp.dump_varmap_tofile(r, "varmap_dump_v2.csv") # This does not work - must provide full path
    assert cp.data_set_number(r, "solarfield.0.dni_des", 1111.0)
    assert cp.dump_varmap_tofile(r, os.path.join(cwd, "varmap.csv"))
    assert cp.save_from_script(r, os.path.join(cwd, "case_study_v2.spt"))
    assert cp.data_free(r) 

    cp = CoPylot()
    r = cp.data_create()
    assert cp.load_from_script(r, os.path.join(cwd, "case_study_v2.spt"))
    assert cp.data_get_number(r, "solarfield.0.dni_des") == 1111.0  # Testing if variable value loads correctly

assert cp.data_free(r) # Works - free memory
