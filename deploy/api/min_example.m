% If current folder is .\source\solarpilot_dev\solarpilot\deploy\api then
% the only path needed is the location of the TMY and can be local.
TMY_file="..\climate_files\USA CA Daggett (TMY2).csv";
cp = py.copylot.CoPylot();

% If current folder is any other folder then you have to provide the path 
% where copylot.py and solarpilot.dll are placed, and the TMY needs the full path.
% Uncomment the following lines in that case. 
% CoPylot_path='C:\Users\adelacal\source\solarpilot_dev\solarpilot\deploy\api';
% TMY_file="C:\Users\adelacal\source\solarpilot_dev\solarpilot\deploy\climate_files\USA CA Daggett (TMY2).csv";
% 
% if count(py.sys.path,CoPylot_path) == 0
%     insert(py.sys.path,int32(0),CoPylot_path);
% end
% cp = py.copylot.CoPylot(path=CoPylot_path);

r = cp.data_create();
cp.data_set_string(r,"ambient.0.weather_file",TMY_file);
cp.generate_layout(r);
field = cp.get_layout_info(r);
cp.simulate(r);
flux = cp.get_fluxmap(r);
res= cp.summary_results(r);
eff=res.get('Solar field optical efficiency')
cp.data_free(r);

plot_results=true;
if plot_results
      field.to_csv("field.csv");
      field_table=readtable("field.csv");
      delete("field.csv");
      colors=[0.0,0.4,0.7];
      figure()
      scatter(field_table.x_location, field_table.y_location,...
          "Marker","square","MarkerEdgeColor",colors,"MarkerFaceColor",colors,...
          "SizeData",3);
      box on
      grid on

      fluxm=double(py.numpy.array(flux));
      figure()
      imagesc(fluxm);
      colorbar();
end





