import os, csv
import pandas as pd
import matplotlib.pyplot as plt

from copylot import CoPylot

cp = CoPylot()

print('Version: ', cp.version() )

print('Process ID: ', os.getpid())

r = cp.data_create()
cp.api_callback_create(r)

## Testing set and get number - DONE
if False:
    cp.data_set_number(r, b"solarfield.0.dni_des", 1111.)
    print ( cp.data_get_number(r, b"solarfield.0.dni_des"))

## Testing set and get string - DONE
if False:
    cp.data_set_string(r, b"ambient.0.loc_state", b"Mind")  #- Working
    print ( cp.data_get_string(r, b"ambient.0.loc_state")) #- Working

## Testing set and get array - DONE
if False:
    cp.data_set_array_from_csv(r, b"financial.0.pmt_factors", "test_array.csv")
    #cp.data_set_array(r, b"financial.0.pmt_factors", [1.,2.,3.,4.,5.,6.,7.,8.,9.])
    print( cp.data_get_array(r, b"financial.0.pmt_factors"))

## Testing set and get matrix - DONE
if False:
    cp.data_set_matrix(r, b"ambient.0.atm_coefs", [[1,1,1,1],[2,2,2,2],[3,3,3,3]])
    #cp.data_set_matrix_from_csv(r, b"ambient.0.atm_coefs", "test_matrix.csv")
    print( cp.data_get_matrix(r, b"ambient.0.atm_coefs"))

## Testing variable reset - DONE
if False:
    cp.reset_vars(r)
    print ( cp.data_get_number(r, b"solarfield.0.dni_des"))
    print( cp.data_get_array(r, b"financial.0.pmt_factors"))
    print( cp.data_get_matrix(r, b"ambient.0.atm_coefs"))

## Testing add and drop receiver - DONE
if False:
    print( cp.add_receiver(r, b"test_rec"))
    print( cp.add_receiver(r, b"test_rec"))
    print( cp.drop_receiver(r, b"test_rec"))
    print( cp.drop_receiver(r, b"test_re"))

## Testing add and drop heliostat template - DONE
if False:
    print( cp.add_heliostat_template(r, b"test_helio"))
    print( cp.add_heliostat_template(r, b"test_helio"))
    print( cp.drop_heliostat_template(r, b"test_helio"))
    print( cp.drop_heliostat_template(r, b"test_heli"))

## Testing setting and getting Combo variables - DONE
if False:
    print (cp.data_get_string(r, b"ambient.0.sun_type"))
    cp.data_set_string(r, b"ambient.0.sun_type" , b'Gaussian sun') # - Working now - Combo
    print (cp.data_get_string(r, b"ambient.0.sun_type"))

## Testing generate - DONE, assign - DONE, and update layout
if True:
    cp.reset_vars(r)
    cp.data_set_number(r, b"solarfield.0.q_des", 100)
    cp.data_set_string(r, b"receiver.0.rec_type", b"Flat plate")
    cp.data_set_string(r, b"ambient.0.weather_file", b"C:/Users/WHamilt2/Documents/solarPILOT_build/SolarPILOT/deploy/climate_files/USA CA Daggett (TMY2).csv")

    # generate - DONE
    if True:
        cp.data_set_string(r, b"solarfield.0.des_sim_detail", b"Single simulation point") # to speed up solution time for testing
                ## TODO: Should we set this variable value within API?  How can we do it on the fly? NEED to check the approach with Mike...
        ## If you add a another template and do not choose a temp_which -> ERROR
        # print( cp.add_heliostat_template(r, b"test_helio"))
        ## Requires setting heliostat template to use for field generation
        #cp.data_set_string(r, b"solarfield.0.temp_which", b"Template 1") 
        # Test - Use single template - DONE
        # Test - Specified range - DONE
            # print( cp.add_heliostat_template(r, b"Template 2") )
            # cp.data_set_number(r, b"heliostat.0.height", 6.)
            # cp.data_set_number(r, b"heliostat.0.width", 6.)
            # cp.data_set_number(r, b"heliostat.0.temp_rad_max", 500)
            # cp.data_set_number(r, b"heliostat.1.temp_rad_min", 500)
            # cp.data_set_number(r, b"heliostat.1.temp_rad_max", 2000)
            # cp.data_set_string(r, b"solarfield.0.template_rule", b"Specified range")
        # Test - Even radial distribution - DONE
            # cp.add_heliostat_template(r, b"Template 2")
            # cp.data_set_number(r, b"heliostat.0.height", 6.)
            # cp.data_set_number(r, b"heliostat.0.width", 6.)
            # cp.data_set_string(r, b"solarfield.0.template_rule", b"Even radial distribution")
        print( cp.generate_layout(r, nthreads = 4) ) # Testing - DONE

    # assign
    if False:
        # pulling in heliostat locations
        helio_data = []
        with open('test_field.csv') as csvDataFile:
            csvReader = csv.reader(csvDataFile)
            for row in csvReader:
                helio_data.append([float(i) for i in row])
        print( cp.assign_layout(r, helio_data)) # Testing - DONE

    if True:
        ## Trying soltrace simulation
        #cp.data_set_string(r, b"fluxsim.0.flux_model", b"SolTrace") # Tested
        cp.data_set_number(r, b'fluxsim.0.x_res', 45)
        cp.data_set_number(r, b'fluxsim.0.y_res', 30)
        print( cp.simulate(r)) # Testing - DONE

    # Update field
    if False:
        ##check = cp.add_land(r, b'exclus', [[1000, 1000],[500,1000], [500,500], [1000, 500]]) # - Testing - DONE
        if True:
            helio_dict = {}
            helio_dict['id'] = [4166, 4000]
            helio_dict['location-x'] = [1500, -1500]
            helio_dict['location-y'] = [1500, 1500]
            helio_dict['soiling'] = [ 0.5, 0.3]
            helio_dict['reflectivity'] = [0.2, 0.8]
            helio_dict['enabled'] = [1, 0]
            check = cp.modify_heliostats(r, helio_dict) # TODO: doesn't seem to modify the field - Need to discuss with Mike
            ## This seems to set field and doesn't preserve previous field-> res2 below includes only these two heliostats??
        
        print( cp.update_geometry(r)) #TODO: When should this function be used?

    if True:
        res1 = cp.summary_results(r, save_dict=True)                # Testing - DONE
        #res, header = cp.detail_results(r, restype = 'matrix')     # returns (matrix, header) - DONE
        #res = cp.detail_results(r, restype = 'matrix')             # returns (matrix, header) - DONE
        #res = cp.detail_results(r, restype = 'dictionary')         # returns dictionary - DONE
        res2 = cp.detail_results(r)                                 # returns dataframe - DONE
        res3 = cp.heliostats_by_region(r, coor_sys=b'polygon', arguments = [[500,500], [1000,1000], [500,1000]]) # tested all, cylindrical, cartesian, and polygon - DONE

    # Pulling field data
    field = cp.get_layout_info(r) # Testing - DONE

    #cp.clear_land(r, clear_type=b'inclusion') # Testing - DONE

    # TODO: Is there a way to create a function to create a field map plot and save it to a image file? 
    # Plotting solarfield layout
    if True:
        x = []
        y = []
        for i in range(len(field)):
            x.append(field[i][1])
            y.append(field[i][2])
        plt.scatter(x,y,s=1.5)
        plt.tight_layout()
        plt.show()

    flux = cp.get_fluxmap(r) # Testing - DONE
    # Plotting flux map
    if True:
        im = plt.imshow(flux)
        plt.colorbar(im)
        plt.tight_layout()
        plt.show()
        
# Testing dump_varmap and save_from script - DONE
if False:
    cwd = os.getcwd()
    print( cp.dump_varmap_tofile(r, b"C:/Users/WHamilt2/Documents/solarPILOT_build/SolarPILOT/deploy/api/varmap_dump.csv") )
    #print( cp.dump_varmap_tofile(r, b"varmap_dump_v2.csv") ) # This does not work - must provide full path
    print( cp.dump_varmap_tofile(r, (cwd + '\\varmap_dump_v3.csv').encode()) ) # This works

    print( cp.save_from_script(r, b"C:/Users/WHamilt2/Documents/solarPILOT_build/SolarPILOT/deploy/api/case_study.spt") )
    print( cp.save_from_script(r, (cwd + '\\case_study_v2.spt').encode()) ) # This works

