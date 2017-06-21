#include <stdlib.h>
#include <fstream>
#include <string>
#include <iostream>
#include <direct.h>

int main()
{
	using namespace std;

	ofstream outfile;

	const int n_cols = 20;				//[-] Number of circumferential nodes around tube at any axial position
	const int n_rows = 20;

	double abs_flux[n_cols][n_rows];
	
	for( int k = 0; k < n_rows; k++ )
	{
		for( int j = 0; j < n_cols; j++ )
		{
			abs_flux[j][k] = 150 - (k*4.5) + 100*((double)j/(double)(n_cols-1));
		}
	}
	
	string EES_sub_dir = "EES_Thermal\\Find_Tube_Thickness.emf";
	char buffer[200];
	_getcwd( buffer, sizeof(buffer) );
	string buffer_str = (string) buffer;
	unsigned found = buffer_str.find_last_of("/\\");
	string base_dir = buffer_str.substr( 0, found );

	string command = "C:\\EES32\\ees.exe \"" + base_dir + "\\" + EES_sub_dir + "\"";


	string EES_output_dir = "'" + base_dir + "'";
	string Abs_Energy = "..\\EES_Thermal\\AbsorbedEnergy.txt";
	string Abs_Energy_EES = "'" + Abs_Energy + "'";
	outfile.open(Abs_Energy);
	outfile << -1 << " " << n_cols << endl;
	
	for( int k = 0; k < n_rows; k++ )
	{
		for( int j = 0; j < n_cols; j++ )
		{
			outfile << abs_flux[j][k] << " ";
		}
		outfile << endl;
	}
	
	outfile.close();

	ofstream out_tube_props;
	out_tube_props.open( "..\\EES_thermal\\tube_props.txt" );
	
	out_tube_props << "'Haynes 230'" << "," << 470.0 << "," << 650.0 << "," << 25.0 << "," << 1.5 << "," << 3.0 << "," << 3.E-6  << "," << Abs_Energy_EES << "," << EES_output_dir << endl; 

	out_tube_props.close();	

	//string EES_emf = "C:\\Users\\tneises\\Documents\\Projects\\SolarPILOT\\custom\\EES_Thermal\\Find_Tube_Thickness.emf";
	system(command.c_str());

	//sprintf( command, "C:\\EES32\\ees.exe \"" "%
	//sprintf( buffer, "%s", EES_emf );
	//string command1 = "C:\\EES32\\ees.exe \"" + EES_emf + "\"";
	//system(command1.c_str());

	return 0;
}
