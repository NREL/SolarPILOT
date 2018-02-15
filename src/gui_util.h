#ifndef _GUI_UTIL_
#define _GUI_UTIL_ 1

#include "string_util.h"
#include "mod_base.h"
#include "SolarField.h"
#include "sort_method.h"
#include <cstdio>
#include <string>
#include <vector>
#include <sstream>

#include "wx/wx.h"
#include "wx/string.h"
#include "wx/grid.h"

#include "interop.h"
#include "IOUtil.h"

//struct layout_shell; // Heliostat;

//template<typename T> static std::string my_to_string( T value )
//{
//    std::ostringstream os;
//    os << value;
//    return os.str();
//};

struct setting
{
    string
        label,
        value,
        varname;
};

class gui_settings
{
    unordered_map<string, setting>
        data;
    vector<string>
        order;
    int i_next;
public:
    gui_settings();

    void clear();
    void add( vector<string> &line );
    void add( string &varname, string &value, string &label);
    setting &operator[](string varname);
    setting &get(string varname);
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
    vector<vector<wxString> > data;
    vector<wxString> rowlabs;
    vector<wxString> collabs;

    int _nrow;
    int _ncol;

public:
    void CreateGrid(int nrow, int ncol);
    bool SetColLabelValue(int col, wxString value);
    bool SetRowLabelValue(int row, wxString value);
    bool SetCellValue(int row, int col, wxString value);
    bool SetCellValue(wxString value, int row, int col);
    void AddRow(int row, wxString label, wxString units, double value, int sigfigs=-1, double min=std::numeric_limits<double>::quiet_NaN(), double max=std::numeric_limits<double>::quiet_NaN(), double stdev=std::numeric_limits<double>::quiet_NaN());
    //void AddRow(int row, wxString label, wxString units, vector<double> &values, int sigfigs=1);
    int GetNumberRows()
    {
        return _nrow;
    }
    int GetNumberCols()
    {
        return _ncol;
    }
    wxString GetRowLabelValue(int row)
    {
        return rowlabs.at(row);
    }
    wxString GetColLabelValue(int col)
    {
        return collabs.at(col);
    }
    wxString GetCellValue(int row, int col)
    {
        return data.at(row).at(col);
    }

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
    MyToggleButton()
    {};
    MyToggleButton(wxWindow* parent, wxWindowID id, const wxBitmap &biton, const wxBitmap &bitoff, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, long style=wxBU_AUTODRAW);
    bool Toggle();
    bool SetState(bool state);
    bool GetState();
    void OnButtonEvent( wxCommandEvent &evt);
};

namespace gui_util
{
    void parseSettingsFile(string fname, gui_settings &gs); 
    bool writeSettingsFile(string &fname, gui_settings &gs);
    bool list_files( std::string &dir,ArrayString &files, std::string filters = "" );
    
    int CalcBestSigFigs(double val);
    string FormatWithCommas(int val);
    string FormatWithCommas(double val, string fmt = "%.2f");
    string FormatAsCurrency(double val);

    wxDateTime::Month intTowxMonth(int month);
};

//--------- error list -------------------
static wxString error_list(int err_ind)
{
    /* 
    Return an error message according to the specified error index.
    */
    switch(err_ind)
    {
    case 0:
        return "Invalid settings error. One or more simulation settings are not currently supported.";
    case 1:
        return "Range error: Index out of range!";
    case 2:
        return "File error: Requested file could not be opened.";
    default:
        return "Unhandled SolarPILOT error. Please contact mike.wagner@nrel.gov for debugging help.";
    };
}; 

#endif
