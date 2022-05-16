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
cp.data_set_string(r, "ambient.0.weather_file", TMY_file);
cp.generate_layout(r);
field = cp.get_layout_info(r);
cp.simulate(r);
flux = cp.get_fluxmap(r);

tic
cp.calculate_optical_efficiency_table(r, py.int(12), py.int(11));
%cp.calculate_optical_efficiency_table(r, py.int(2), py.int(2))  %# For debugging
toc

eff_filename = "./efficiency_table.csv";
cp.save_optical_efficiency_table(r, eff_filename), %# Testing simple table saving (consistent with get_optical_efficiency_table)
cp.save_optical_efficiency_table(r, "./modelica_efficiency_table.txt", "sf_eff_table");  %# Testing Modelica specific table output

sf_eff = cp.get_optical_efficiency_table(r);
cp.data_free(r);

% Read in efficiency table
eff_table=readtable(eff_filename);
ele= eff_table{1,2:end};
azi= eff_table{2:end,1};
eff_data = eff_table{2:end,2:end};

% Check data is consistent between two methods
t1(1)=isequal(azi',round(double(sf_eff.get('azimuth')),4));
t1(2)=isequal(ele,round(double(sf_eff.get('elevation')),4));
t1(3)=isequal(eff_data,round(double(py.numpy.array(sf_eff.get('eff_data'))),6));

FontSize=14;
LineWidth=1.5;
figure()
legInfoAzi=cell(length(azi),1);
hold on
for i=1:length(azi)
    plot(ele,eff_data(i,:),'LineWidth',LineWidth);
    legInfoAzi{i}=num2str(azi(i));
end
hold off
xlabel('Elevation Angle [deg]')
ylabel('Solarfield Efficiency [-]')
set(gca,'FontSize',FontSize)
leg=legend(legInfoAzi,'NumColumns',2,'Location','Best');
title(leg,'Azimuth [deg]')
box on 
grid on
xlim([0,90])

figure()
legInfoEle=cell(length(ele),1);
hold on
for i=1:length(ele)
    plot(azi,eff_data(:,i),'LineWidth',LineWidth);
    legInfoEle{i}=num2str(ele(i));
end
hold off
xlabel('Azimuth Angle [deg]')
ylabel('Solarfield Efficiency [-]')
set(gca,'FontSize',FontSize)
leg=legend(legInfoEle,'NumColumns',2,'Location','Best');
title(leg,'Elevation [deg]')
xlim([0,360])
box on 
grid on

