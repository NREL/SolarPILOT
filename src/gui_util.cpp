
#include "gui_util.h"
#include "mod_base.h"	//from the PILOT code
#include <string>
#include <cstdio>
#include <wx/dir.h>
#include <wx/textfile.h>
#include "rapidxml.hpp"
#include <algorithm>

#ifdef _WIN32
#include <direct.h>
#include <Windows.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif

// ----------------- gui settings ----------------
gui_settings::gui_settings(){
	i_next = 0;
	data.clear();
	order.clear();
};

void gui_settings::clear(){
	data.clear();
	order.clear();
	i_next = 0;
};
void gui_settings::add( vector<string> &line ){
	add( line.at(0), line.at(1), line.at(2) );
};
void gui_settings::add( string &varname, string &value, string &label){
	data[varname].varname = varname;
	data[varname].value = value;
	data[varname].label = label;
	order.push_back(varname);
};
setting &gui_settings::operator[](string varname){
	return data[varname];
};
setting &gui_settings::get(string varname){
	return data[varname];
};
setting *gui_settings::getFirst(){
	i_next=1;
	return &data[order.at(0)];
		
}
setting *gui_settings::getNext(){
	if(i_next < (int)data.size())
		return &data[order.at(i_next++)];
	else
		return (setting*)NULL;
}
int gui_settings::data_size(){
	return (int)order.size();
};

// ------------------ array string
wxArrayStr::wxArrayStr(){};

wxArrayStr::wxArrayStr( ArrayString &array ){
	Clear();
	for(int i=0; i<array.size(); i++)
		push_back( array[i] );
};
wxArrayStr &wxArrayStr::operator=(const wxArrayString &rhs){
	Clear();
	for(int i=0; i<(int)rhs.size(); i++){
		push_back(rhs[i]);
	}
	return *this;
};









MyToggleButton::MyToggleButton(wxWindow* parent, wxWindowID id, const wxBitmap &biton, const wxBitmap &bitoff, const wxPoint &pos, const wxSize &size, long style) :
	wxBitmapButton(parent, id, bitoff, pos, size, style)
{
	//Set locals
	_onbit = biton;
	_offbit = bitoff;

	_state = false;

	if(size == wxDefaultSize)
		this->SetSize(bitoff.GetSize());

	//Bind button event
	this->Connect(wxEVT_BUTTON, wxCommandEventHandler( MyToggleButton::OnButtonEvent ), NULL, this);

}

bool MyToggleButton::Toggle(){
	return SetState(! _state); //returns the new state
}

bool MyToggleButton::SetState(bool state){
	_state = state;
	if(state){
		SetBitmap(_onbit);
	}
	else{
		SetBitmap(_offbit);
	}
	return _state;
}

bool MyToggleButton::GetState()
{
    return _state;
}

void MyToggleButton::OnButtonEvent( wxCommandEvent &evt){
	Toggle();
	evt.Skip();	//pass up the chain
}

int gui_util::CalcBestSigFigs(double val){
	//Calculate and return the best number of decimal sig figs for the given value. Always show all integer values.
	return max( 3 - int(ceil( log10( val ) ) ), 0);
}

string gui_util::FormatWithCommas(int val){
	string
		sval = to_string(val, "%d");
	int ns = sval.size();
	int nct = 1;
	int i=ns-1; 
	while(i > 1){
		if(nct == 3){
			sval.insert(sval.begin()+i, ',');
			nct = 0;
		}
		nct++;
		i--;
	}
	return sval;
}

string gui_util::FormatWithCommas(double val, string fmt){
	string sval = to_string(val, fmt.c_str());
	int ns = sval.size();
	int nct = -1;
	int i=ns-1;
	while(i > 1){
		if(sval.at(i) == '.') nct = 0;	//Don't start inserting commas until the decimal is hit
		if(nct < 0){
			i--;
			continue;
		}

		if(nct == 3){
			sval.insert(sval.begin()+i, ',');
			nct=0;
		}
		nct++;
		i--;
	}
	return sval;

}

string gui_util::FormatAsCurrency(double val){
	//format for currency
	string sval = to_string(val, "%.2f");
	vector<string> sint = split(sval, ".");
	int nsint = sint.at(0).size();
	int ncomma = int(floor(double(nsint-1)/3.));
	int cloc = nsint-ncomma*3;
	string fval="";
	int i=0;
	while(i < nsint){
		char c = sval.at(i);
		fval += c;
		if(i==cloc-1 && i < nsint-1){
			fval.append(",");
			cloc += 3;
		}
		i++;
	}

	fval.append("."+sint.at(1));
	return fval;
}

wxDateTime::Month gui_util::intTowxMonth(int month){
	
	switch (month)
	{
	case 1: return wxDateTime::Jan;
	case 2: return wxDateTime::Feb;
	case 3: return wxDateTime::Mar;
	case 4: return wxDateTime::Apr;
	case 5: return wxDateTime::May;
	case 6: return wxDateTime::Jun;
	case 7: return wxDateTime::Jul;
	case 8: return wxDateTime::Aug;
	case 9: return wxDateTime::Sep;
	case 10: return wxDateTime::Oct;
	case 11: return wxDateTime::Nov;
	case 12: return wxDateTime::Dec;
	default:
		break;
	}
	return wxDateTime::Month::Inv_Month;
}


//--------grid emulator--------------------
void grid_emulator::CreateGrid(int nrow, int ncol){
	_nrow = nrow;
	_ncol = ncol;
	data.clear();
	data.resize(nrow);
	for(int i=0; i<nrow; i++)
		data.at(i).resize(ncol);
	rowlabs.resize(nrow);
	collabs.resize(ncol);
}

bool grid_emulator::SetColLabelValue(int col, wxString value){
	collabs.at(col) = value;
	return true;
}

bool grid_emulator::SetRowLabelValue(int row, wxString value){
	rowlabs.at(row) = value;
	return true;
}

bool grid_emulator::SetCellValue(int row, int col, wxString value){
	data.at(row).at(col) = value;
	return true;
}

bool grid_emulator::SetCellValue(wxString value, int row, int col){
	return SetCellValue(row, col, value);
}

void grid_emulator::GetPrintableTable(wxArrayStr &printable, wxString eol){
	printable.Clear();
	wxString hdr;
	for(int i=0; i<_ncol; i++)
		hdr.Append(", "+collabs.at(i));
	printable.Add(hdr+eol);

	for(int i=0; i<_nrow; i++){
		wxString line = rowlabs.at(i);
		for(int j=0; j<_ncol; j++){
			wxString tval = GetCellValue(i,j);
			tval.Replace(",","");
			line.Append(", "+tval);	//Remove any commas from cell values - the file is comma-delimited
		}
		printable.Add(line+eol);
	}
	
}

void grid_emulator::MapToWXGrid(wxGrid *grid){
	grid->CreateGrid(_nrow, _ncol);
	
	for(int i=0; i<_nrow; i++)
		grid->SetRowLabelValue(i, rowlabs.at(i));
	for(int i=0; i<_ncol; i++)
		grid->SetColLabelValue(i, collabs.at(i));
	for(int i=0; i<_nrow; i++)
		for(int j=0; j<_ncol; j++)
			grid->SetCellValue(i, j, GetCellValue(i,j));
}

void grid_emulator::AddRow(int row, wxString label, wxString units, double value, int sigfigs, double min, double max, double stdev){
	//Row adding method for simple performance runs

	if((GetNumberCols() < 5) || (GetNumberRows() < row+1))
		throw spexception("Sorry! Results table incorrectly formatted. Please contact solarpilot.support@nrel.gov for help.");
	
	bool is_currency = false;
	if( units.Find("$") != wxString::npos ) is_currency = true;

    //calculate a good precision
    if( sigfigs < 0 )
    {
        int prec = 5-(int)log10f(value);
        sigfigs = prec < 0 ? 0 : prec;
    }

	wxString infmt = wxString::Format("%s.%df", "%", sigfigs);
	wxString stfmt = wxString::Format("%s.%df", "%", sigfigs+2);

	SetRowLabelValue(row, label);
	SetCellValue(row, 0, units);
	SetCellValue(row, 1, is_currency ? gui_util::FormatAsCurrency(value) : to_string(value, infmt.c_str()) );
	SetCellValue(row, 2, (min==min ? to_string(min, infmt.c_str()) : "") );
	SetCellValue(row, 3, (max == max ? to_string(max, infmt.c_str()) : ""));
	SetCellValue(row, 4, (stdev == stdev ? to_string(stdev, stfmt.c_str()) : ""));

}

//void grid_emulator::AddRow(int row, wxString label, wxString units, vector<double> &values, int sigfigs){
//	//Row adding method for parametrics
//	int nsim = (int)values.size();
//	if((GetNumberCols() < nsim + 1) || (row > GetNumberRows()-1)){
//		wxMessageBox("Sorry! Parametric results table is incorrectly formatted. Please contact mike.wagner@nrel.gov for help.", "SolarPILOT Error");
//		throw;
//		return;
//	}
//
//	bool is_currency = false;
//	if( units.Find("$") != wxString::npos ) is_currency = true;
//	
//	wxString infmt = wxString::Format("%s.%df", "%", sigfigs);
//	wxString stfmt = wxString::Format("%s.%df", "%", sigfigs+2);
//
//	SetRowLabelValue(row, label);
//	SetCellValue(row, 0, units);
//
//	for(int i=0; i<nsim; i++){
//		SetCellValue(row, i+2, is_currency ? gui_util::FormatAsCurrency(values.at(i)) : to_string(values.at(i), infmt) );
//	}
//
//
//}

bool gui_util::list_files( std::string &directory, ArrayString &files, std::string filters ){
	/*
	Get the list of files in the directory "dir" that contain the extensions in "filters".
	Filters should be just the name of the extensions separated by spaces, e.g. 
	"csv txt dat"
	*/
	wxString dirname(directory);
	wxDir dir(dirname);
	wxString filename;
	
	vector<string> types;
	vector<string> sname;

	if(filters == "") {
		types.resize(1); 
		types.at(0) = "";
	}
	else{ 
		types = split(filters, " ");
	}

	files.clear();
	bool cont = dir.GetFirst(&filename, "", wxDIR_FILES);
	while(cont)
	{
		//only add if one of the extensions matches the file type
		sname = split((string)filename.c_str(), ".");
		for(unsigned int i=0; i<types.size(); i++){
			if(lower_case(sname.back()) == types.at(i)){ 
				files.push_back((string)filename.c_str());
				break;
			}
		}
		cont = dir.GetNext(&filename);
	}
	


	return true;

}

void gui_util::parseSettingsFile(string fname, gui_settings &gs){
	string files, eols;
	ioutil::read_file(fname, files, eols);
	wxString file(files), eol(eols);
	
	std::vector<std::string> lines, line;
	lines = split(file.ToStdString(), eol.ToStdString());
	gs.clear();
	for(unsigned int i=0; i<lines.size(); i++){
		line = split(lines.at(i), "\t");
		gs.add(line);
	}

}

bool gui_util::writeSettingsFile(string &fname, gui_settings &gs){
	
	wxTextFile fobj(fname);
	fobj.Open();
	if( ! fobj.IsOpened() ) return false;
	
	fobj.Clear();
	wxFormatString fmt = wxT("%s\t%s\t%s");
	setting *set = gs.getFirst();
	for(int i=0; i<gs.data_size(); i++){
		wxString line;
		line.sprintf(fmt, set->varname.c_str(), set->value.c_str(), set->label.c_str());
		fobj.AddLine(line);
		set = gs.getNext();
	}
	bool writeok = fobj.Write();
	fobj.Close();
	return writeok;
/*
	FILE *file;
	if( fopen_s(&file, fname.c_str(), "w") != 0){
		return false;
	}
	else{
		setting *set = gs.getFirst();
		for(unsigned int i=0; i<gs.data_size(); i++){
			fprintf(file, "%s\t%s\t%s\n", set->varname.c_str(), set->value.c_str(), set->label.c_str() );
			set = gs.getNext();
		}
		fclose(file);
		return true;
	}*/
}