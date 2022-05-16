%% Matlab test script
% If current folder is .\source\solarpilot_dev\solarpilot\deploy\api then
% the only path needed is the location of the TMY and can be local.
TMY_file="..\climate_files\USA CA Daggett (TMY2).csv";
cp = py.copylot.CoPylot();

% If current folder is any other folder then you have to provide the path 
% where copylot.py and solarpilot.dll are placed, and the TMY needs the full path.
% Uncomment the following lines in that case. 
% CoPylot_path='C:\Users\adelacal\source\solarpilot_dev\solarpilot\deploy\api';
% TMY_file="C:\Users\adelacal\source\solarpilot_dev\solarpilot\deploy\climate_files\USA CA Daggett (TMY2).csv";
% if count(py.sys.path,CoPylot_path) == 0
%     insert(py.sys.path,int32(0),CoPylot_path);
% end
% cp = py.copylot.CoPylot(path=CoPylot_path);

plot_results = true ;
test_solTrace_sim = true;

%%   Minimum working example -> Must update path to weather file
disp('Minimum working example')
r = cp.data_create();
cp.api_callback_create(r);
cp.data_set_string(r,"ambient.0.weather_file",TMY_file); %Must provide a TMY file
cp.generate_layout(r);
field = cp.get_layout_info(r);
cp.simulate(r);
flux = cp.get_fluxmap(r);
cp.data_free(r);
cp.api_disable_callback(r) %#tested
disp('Minimum working example - Pass')

%% Plotting (default) solar field and flux map
if plot_results
      field.to_csv("field.csv");
      field_table=readtable("field.csv");
      figure()
      colors=[0.0,0.4,0.7];
      scatter(field_table.x_location, field_table.y_location,...
          "Marker","square","MarkerEdgeColor",colors,"MarkerFaceColor",colors,...
          "SizeData",3);
      fluxm=double(py.numpy.array(flux));
      figure()
      imagesc(fluxm);
      colorbar();
end



%% Testing set and get number - Pass
r = cp.data_create();  %# New SolarPILOT instance
py.print(cp.version(r));    %# Version number
%cp.api_callback_create(r)   %# callback
%cp.api_disable_callback(r) %#tested
t1(1)=cp.data_set_number(r, "solarfield.0.dni_des", 1111.0);
t1(2)=isequal(cp.data_get_number(r, "solarfield.0.dni_des"),1111.0);
if all(t1)
    disp('Testing set and get number - Pass')
else
    disp('Testing set and get number - No Pass')
end
%% Testing set and get boolean - Pass
t2(1)=cp.data_set_number(r, "solarfield.0.is_sliprow_skipped", true);         %# Works with True and False
t2(2)=cp.data_get_number(r, "solarfield.0.is_sliprow_skipped");
t2(3)=cp.data_set_number(r, "solarfield.0.is_sliprow_skipped", 0.);         %# Works with 1 and 0 
t2(4)=isequal(cp.data_get_number(r, "solarfield.0.is_sliprow_skipped"),false);

if all(t2)
    disp('Testing set and get boolean - Pass')
else
    disp('Testing set and get boolean - No Pass')
end

%% Testing set and get string - Pass
cp.data_set_string(r, "ambient.0.loc_state", "Mind");  %# - Pass
t3(1)=isequal(string(cp.data_get_string(r, "ambient.0.loc_state")),"Mind"); % # - Pass
t3(2)=not(cp.data_set_string(r, "ambient.0.sun_csr", "0.05"));% # - Not a string variable - Pass
if all(t3)
    disp('Testing set and get string - Pass')
else
    disp('Testing set and get string - No Pass')
end

%% Testing setting and getting Combo variables - Through string - Pass
t4(1)=isequal(string(cp.data_get_string(r, "ambient.0.sun_type")),'Limb-darkened sun');  %# Default value
t4(2)=cp.data_set_string(r, "ambient.0.sun_type", "Gaussian sun");  %# Combo - acceptable option - Pass
t4(3)=isequal(string(cp.data_get_string(r, "ambient.0.sun_type")),"Gaussian sun");
t4(4)=not(isequal(string(cp.data_get_string(r, "ambient.0.sun_type")),"aussian sun")); %# Combo - not acceptable option - Pass
t4(5)=isequal(string(cp.data_get_string(r, "ambient.0.sun_type")),"Gaussian sun");%  # Value should not change
if all(t4)
    disp('Testing setting and getting Combo variables - Through string - Pass')
else
    disp('Testing setting and getting Combo variables - Through string - No Pass')
end


%% Testing setting and getting Combo variables - Through number - Pass
t5(1)=isequal(string(cp.data_get_string(r, "ambient.0.insol_type")),'Weather file data');    %# get string of combo choice
t5(2)=cp.data_set_number(r, "ambient.0.insol_type", 1); % # Combo - acceptable option - Pass
t5(3)=isequal(cp.data_get_number(r, "ambient.0.insol_type"),1); %   # get number of combo choice
t5(4)=isequal(string(cp.data_get_string(r, "ambient.0.insol_type")),'Hottel model');    %# get string of combo choice
t5(5)=not(cp.data_set_number(r, "ambient.0.insol_type", 5));  %# Combo - not acceptable option - Pass
t5(6)=isequal(string(cp.data_get_string(r, "ambient.0.insol_type")),'Hottel model');  %# Value should not change
if all(t5)
    disp('Testing set and get boolean - Pass')
else
    disp('Testing set and get boolean - No Pass')
end
%% Testing set and get array - Pass
t6(1)=cp.data_set_array_from_csv(r, "financial.0.pmt_factors", "test_array.csv");
t6(2)=isequal(length(double(cp.data_get_array(r, "financial.0.pmt_factors"))),9);
set_vector = [1.,2.,3.,4.,5.];
t6(3)=cp.data_set_array(r, "financial.0.pmt_factors", set_vector);
ret_vector = double(cp.data_get_array(r, "financial.0.pmt_factors"));
t6(4)=isequal(set_vector,ret_vector);

t6(5)=isequal(length(double(cp.data_get_array(r, "financial.0.schedule_array"))),8760);
t6(6)=cp.data_set_array(r, "financial.0.schedule_array", set_vector); %# testing set integer array - Pass
ret_vector = double(cp.data_get_array(r, "financial.0.schedule_array"));
t6(7)=isequal(set_vector,ret_vector);

if all(t6)
    disp('Testing set and get array - Pass')
else
    disp('Testing set and get array - No Pass')
end

%% Testing set and get matrix - Pass
set_matrix = [1, 1, 1, 1, 1; 2, 2, 2, 2, 2; 3, 3, 3, 3, 3; 4, 4, 4, 4, 4]; %matlab matrix

%Matlab matrix to a python matrix
cstr = cell(1, size(set_matrix, 1));
for row = 1:size(set_matrix, 1)
    cstr(row) = {set_matrix(row, :)};
end
set_matrix_python = py.numpy.array(cstr);

t7(1)=cp.data_set_matrix(r, "ambient.0.atm_coefs", set_matrix_python);
ret_matrix = double(py.numpy.array(cp.data_get_matrix(r, "ambient.0.atm_coefs")));
t7(2)=isequal(set_matrix,ret_matrix); %pass

csv_matrix=table2array(readtable('test_matrix.csv'));     
t7(3)=cp.data_set_matrix_from_csv(r, "ambient.0.atm_coefs", "test_matrix.csv");
ret_matrix = double(py.numpy.array(cp.data_get_matrix(r, "ambient.0.atm_coefs")));
t7(4)=isequal(csv_matrix,ret_matrix); %pass

if all(t7)
    disp('Testing set and get matrix - Pass')
else
    disp('Testing set and get matrix - No Pass')
end

%% Testing variable reset - Pass
cp.reset_vars(r);
t8(1)=isequal(cp.data_get_number(r, "solarfield.0.dni_des"),950);

if all(t8)
    disp('Testing variable reset - Pass')
else
    disp('Testing variable reset - No Pass')
end

%% Testing add and drop receiver - Pass
t9(1)=isequal(double(cp.add_receiver(r, "test_rec")),1); % # Second Receiver 
t9(2)=isequal(double(cp.add_receiver(r, "test_rec")),-1);  %# can't add another receiver with the same name
t9(3)=cp.drop_receiver(r, "test_rec");
t9(4)=not(cp.drop_receiver(r, "test_rec")); %# can't drop receiver because is alredy dropped

if all(t8)
    disp('Testing add and drop receiver - Pass')
else
    disp('Testing add and drop receiver - No Pass')
end

%% Testing add and drop heliostat template - Pass
t10(1)=isequal(double(cp.add_heliostat_template(r, "test_helio")),1); %second heliostat template
t10(2)=isequal(double(cp.add_heliostat_template(r, "test_helio")),-1); %can't add another heliostat template with the same name
t10(3)=cp.drop_heliostat_template(r, "test_helio");
t10(4)=not(cp.drop_heliostat_template(r, "test_heli")); %# can't drop heliostat templante with wrong name

if all(t10)
    disp('Testing add and drop heliostat template - Pass')
else
    disp('Testing add and drop heliostat template - No Pass')
end
%% Testing generate and assign layout - Pass
cp.reset_vars(r);
cp.data_set_number(r, "solarfield.0.q_des", 100) ;     % # Small field for debugging
cp.data_set_string(r, "receiver.0.rec_type", "Flat plate");
cp.data_set_string(r,"ambient.0.weather_file",TMY_file);
 
% generate - Pass
t11(1)=cp.data_set_string(r, "solarfield.0.des_sim_detail", "Single simulation point"); % # to speed up solution time for testing
t11(2)=cp.data_set_number(r, "solarfield.0.is_sliprow_skipped", true);
t11(3)=cp.data_set_number(r, "solarfield.0.slip_plane_blocking", 0.0);

%## If you add a another template and do not choose a temp_which -> ERROR
%cp.add_heliostat_template(r, "test_helio")
% ## Requires setting heliostat template to use for field generation
% cp.data_set_string(r, "solarfield.0.temp_which", "Template 1")
% %# Test - Use single template - Pass
% %# Test - Specified range - Pass
% cp.add_heliostat_template(r, "Template 2")
% cp.data_set_number(r, "heliostat.0.height", 6.)
% cp.data_set_number(r, "heliostat.0.width", 6.)
% cp.data_set_number(r, "heliostat.0.temp_rad_max", 500)
% cp.data_set_number(r, "heliostat.1.temp_rad_min", 500)
% cp.data_set_number(r, "heliostat.1.temp_rad_max", 2000)
% cp.data_set_string(r, "solarfield.0.template_rule", "Specified range")
% %# Test - Even radial distribution - Pass
% cp.add_heliostat_template(r, "Template 2")
% cp.data_set_number(r, "heliostat.0.height", 6.)
% cp.data_set_number(r, "heliostat.0.width", 6.)
% cp.data_set_string(r, "solarfield.0.template_rule", "Even radial distribution")
% cp.generate_layout(r, nthreads=py.int(4))  %# Testing - Pass


%assign
%pulling in heliostat locations
csvDataFile=readtable("test_field.csv");
helio_data=py.list();
for i=1:height(csvDataFile)
    helio_data.append(csvDataFile{i,:})
%     helio_data.append(py.numpy.array(csvDataFile{i,:}))
end
t11(4)=cp.assign_layout(r, helio_data); % # Testing - Pass
%     
%     # Changing flux map resolution 
t11(5)=cp.generate_layout(r);
t11(6)=cp.data_set_number(r, "fluxsim.0.x_res", 45);
t11(7)=cp.data_set_number(r, "fluxsim.0.y_res", 30);
t11(8)=cp.simulate(r);
flux_hermite = cp.get_fluxmap(r);  %# Testing - Pass
     
%Plotting flux map
if plot_results
    flux_hermite_matlab=double(py.numpy.array(flux_hermite));
    figure()
    imagesc(flux_hermite_matlab);
    colorbar();
    title('Hermite Results')
end

if all(t11)
    disp('Testing generate and assign layout - Pass')
else
    disp('Testing generate and assign layout - No Pass')
end


%% SolTrace simulation
if test_solTrace_sim 
    t12(1)=cp.data_set_string(r, "fluxsim.0.flux_model", "SolTrace"); %# Tested
    t12(2)=cp.data_set_string(r, "fluxsim.0.aim_method", "Keep existing");
    t12(3)=cp.data_set_number(r, "fluxsim.0.max_rays", 100000000);
    t12(4)=cp.data_set_number(r, "fluxsim.0.min_rays", 1000000);
    t12(5)=cp.simulate(r);  %# Testing - Pass
    flux_ST = cp.get_fluxmap(r);  %# Testing - Pass
    
    %         # Change back for rest of tests
    t12(6)=cp.data_set_string(r, "fluxsim.0.flux_model", "Hermite (analytical)");
    t12(7)=cp.data_set_string(r, "fluxsim.0.aim_method", "Image size priority");
% 
%     	# Plotting flux map
    if plot_results
        flux_ST_matlab=double(py.numpy.array(flux_ST));
        figure()
        imagesc(flux_ST_matlab);
        colorbar();
        title('SolTrace Results');
    end
    if all(t12)
        disp('Testing SolTrace simulation - Pass')
    else
        disp('Testing SolTrace simulation - No Pass')
    end
end 
%% Testing update layout - Pass 
%         ##check = cp.add_land(r, b'exclus', [[1000, 1000],[500,1000], [500,500], [1000, 500]]) # - Testing - Pass


res = cp.detail_results(r);
res.to_csv('res.csv');
res_table=readtable('res.csv');
delete('res.csv');
helio_dict = py.dict(pyargs(...
    "id", [res_table.id(1),res_table.id(2)], ...
    "location-x", [1500, -1500],...
    "location-y", [1500, 1500],...
    "soiling" , [0.5, 0.3],...
    "reflectivity", [0.2, 0.8],...
    "enabled", [1, 0]));
t13(1)=cp.modify_heliostats(r, helio_dict);

% find max flux of original geometry
t13(2)=cp.simulate(r);  %# Testing - Pass
flux = cp.get_fluxmap(r);
flux_matlab=double(py.numpy.array(flux));
maxflux_befc = max(flux_matlab,[],'all');

% Change geometry, simulate, find max flux
t13(3)=cp.data_set_number(r, "heliostat.0.height", 11.0);
t13(4)=cp.simulate(r);
flux = cp.get_fluxmap(r);
flux_matlab=double(py.numpy.array(flux));
maxflux_aftc_noUP = max(flux_matlab,[],'all');

%Update, simulate, and find max flux
t13(5)=cp.update_geometry(r);  %# Testing - Pass
t13(6)=cp.simulate(r);
flux = cp.get_fluxmap(r);
flux_matlab=double(py.numpy.array(flux));
maxflux_aftu = max(flux_matlab,[],'all');

t13(7)=isequal(length(unique([maxflux_befc, maxflux_aftc_noUP,maxflux_aftu])),3);

if all(t13)
    disp('Testing update layout - Pass ')
else
    disp('Testing update layout - No Pass')
end
          
%% Testing get results - pass:
res1 = cp.summary_results(r, save_dict=true);  %# Testing - Pass
Nhel=res1.get('Simulated heliostat count');
t14(1)=isequal(Nhel,1172);

%[res, header] = cp.detail_results(r, restype = 'matrix')     %# returns (matrix, header) - Pass
% res = cp.detail_results(r, restype = 'matrix')            % # returns (matrix, header) - Pass
% res = cp.detail_results(r, restype = 'dictionary')         %# returns dictionary - Pass

res2 = cp.detail_results(r, get_corners=true);  %# returns dataframe - Pass

%Dataframe to table throught csv
res2.to_csv("res2.csv");
res2_table=readtable("res2.csv");
delete("res2.csv");
t14(2)=isequal(height(res2_table),Nhel);

set_matrix = [500, 500; 1000, 1000; 500, 1000]; %matlab matrix
%Matlab matrix to a python matrix
cstr = cell(1, size(set_matrix, 1));
for row = 1:size(set_matrix, 1)
    cstr(row) = {set_matrix(row, :)};
end
set_matrix_python = py.numpy.array(cstr);
res3 = cp.heliostats_by_region(r, coor_sys="polygon", arguments=set_matrix_python);  %# tested all, cylindrical, cartesian, and polygon - Pass
% res3.to_csv("res3.csv");
% res3_table=readtable("res3.csv");
% delete("res3.csv");

if all(t14)
    disp('Testing get results - Pass ')
else
    disp('Testing get results - No Pass')
end

%% Testing modify_heliostats by disabling half the field and re-simulating - Pass

cp.simulate(r);  %# Testing - Pass
flux = cp.get_fluxmap(r);
flux_matlab=double(py.numpy.array(flux));
maxflux_original=max(flux_matlab,[],'all');
Nhel_half=round((Nhel/2),0);
helio_dict = py.dict(pyargs(...
            "id", [res2_table.id(Nhel_half:end)], ...
            "enabled", [zeros(length(res2_table.id(Nhel_half:end)),1)]));
t15(1)=cp.modify_heliostats(r, helio_dict ); %# Testing - Pass
t15(2)=cp.simulate(r);  %# Testing - Pass
flux = cp.get_fluxmap(r);
flux_matlab=double(py.numpy.array(flux));
maxflux_updated=max(flux_matlab,[],'all');
if plot_results
    figure()
    imagesc(flux_matlab);
    colorbar();
    title('Half field Results');
end
t15(3)=not(isequal(maxflux_original,maxflux_updated));
if all(t15)
    disp('Testing modify_heliostats by disabling half the field and re-simulating - Pass ')
else
    disp('Testing modify_heliostats by disabling half the field and re-simulating - No Pass')
end
% 
%     # Pulling field data
%field,header = cp.get_layout_info(r, get_corners=true, restype="matrix");  %# Testing - No Pass
%cp.clear_land(r, clear_type=b'inclusion') %# Testing - Pass
%cp.clear_land(r, clear_type='inclusion'); %# Testing - Pass

%% Testing dump_varmap and save_from script - Pass
cp.dump_varmap_tofile(r, "varmap_dump_v2.csv"); %# This does not work - must provide full path
t16(1)=cp.dump_varmap_tofile(r, "./varmap_dump_v2.csv"); %works with the ./ 
cp.data_set_number(r, "solarfield.0.dni_des", 1111);
t16(2)=cp.save_from_script(r, "./case_study_v2.spt");
cp.data_free(r); 

r = cp.data_create();
t16(3)=cp.load_from_script(r, "case_study_v2.spt");
t16(4)=isequal(cp.data_get_number(r, "solarfield.0.dni_des"),1111);  %# Testing if variable value loads correctly
cp.data_free(r); % Works - free memory

if all(t16)
    disp('Testing dump_varmap and save_from script - Pass ')
else
    disp('Testing dump_varmap and save_from script - No Pass')
end
