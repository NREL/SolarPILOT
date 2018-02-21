#ifndef _GUI_UTIL_
#define _GUI_UTIL_ 1

#include <string>
#include <vector>

#include <wx/wx.h>
#include <wx/defs.h>
#include <wx/grid.h>

#include "interop.h"

struct setting
{
    std::string
        label,
        value,
        varname;
};

class gui_settings
{
    unordered_map<std::string, setting> data;
    std::vector<std::string> order;
    int i_next;
public:
    gui_settings();

    void clear();
    void add( std::vector<std::string> &line );
    void add( std::string &varname, std::string &value, std::string &label);
    setting &operator[](std::string varname);
    setting &get(std::string varname);
    setting *getFirst();
    setting *getNext();
    int data_size();
};

class wxArrayStr : public wxArrayString
{
public:
    //Extends the wxArrayStr class with a conversion method from ArrayString
    wxArrayStr();
    wxArrayStr( ArrayString &array );
    wxArrayStr &operator=(const wxArrayString &rhs);

};

class grid_emulator
{
    std::vector<std::vector<wxString> > data;
    std::vector<wxString> rowlabs;
    std::vector<wxString> collabs;

    int _nrow;
    int _ncol;

public:
    void CreateGrid(int nrow, int ncol);
    bool SetColLabelValue(int col, wxString value);
    bool SetRowLabelValue(int row, wxString value);
    bool SetCellValue(int row, int col, wxString value);
    bool SetCellValue(wxString value, int row, int col);
    void AddRow(int row, wxString label, wxString units, double value, int sigfigs=-1, 
		double min=std::numeric_limits<double>::quiet_NaN(), 
		double max=std::numeric_limits<double>::quiet_NaN(), 
		double stdev=std::numeric_limits<double>::quiet_NaN());

	int GetNumberRows();
	int GetNumberCols();
	wxString GetRowLabelValue(int row);
	wxString GetColLabelValue(int col);
	wxString GetCellValue(int row, int col);

    void GetPrintableTable(wxArrayStr &printable, wxString eol = "\n");
    void MapToWXGrid(wxGrid *wxgrid);
};

class MyToggleButton : public wxBitmapButton
{
    wxBitmap
        _onbit,
        _offbit;

    bool _state;

public:
	MyToggleButton();
    MyToggleButton(wxWindow* parent, wxWindowID id, const wxBitmap &biton, const wxBitmap &bitoff, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, long style=wxBU_AUTODRAW);
    bool Toggle();
    bool SetState(bool state);
    bool GetState();
    void OnButtonEvent( wxCommandEvent &evt);
};

namespace gui_util
{
    void parseSettingsFile(std::string fname, gui_settings &gs); 
    bool writeSettingsFile(std::string &fname, gui_settings &gs);
    bool list_files( std::string &dir,ArrayString &files, std::string filters = "" );
    
    int CalcBestSigFigs(double val);
    std::string FormatWithCommas(int val);
    std::string FormatWithCommas(double val, std::string fmt = "%.2f");
    std::string FormatAsCurrency(double val);

    wxDateTime::Month intTowxMonth(int month);

	//--------- error list -------------------
	wxString error_list(int err_ind);
};



#endif
