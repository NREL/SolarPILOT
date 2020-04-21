
/*******************************************************************************************************
*  Copyright 2018 Alliance for Sustainable Energy, LLC
*
*  NOTICE: This software was developed at least in part by Alliance for Sustainable Energy, LLC
*  ("Alliance") under Contract No. DE-AC36-08GO28308 with the U.S. Department of Energy and the U.S.
*  The Government retains for itself and others acting on its behalf a nonexclusive, paid-up,
*  irrevocable worldwide license in the software to reproduce, prepare derivative works, distribute
*  copies to the public, perform publicly and display publicly, and to permit others to do so.
*
*  Redistribution and use in source and binary forms, with or without modification, are permitted
*  provided that the following conditions are met:
*
*  1. Redistributions of source code must retain the above copyright notice, the above government
*  rights notice, this list of conditions and the following disclaimer.
*
*  2. Redistributions in binary form must reproduce the above copyright notice, the above government
*  rights notice, this list of conditions and the following disclaimer in the documentation and/or
*  other materials provided with the distribution.
*
*  3. The entire corresponding source code of any redistribution, with or without modification, by a
*  research entity, including but not limited to any contracting manager/operator of a United States
*  National Laboratory, any institution of higher learning, and any non-profit organization, must be
*  made publicly available under this license for as long as the redistribution is made available by
*  the research entity.
*
*  4. Redistribution of this software, without modification, must refer to the software by the same
*  designation. Redistribution of a modified version of this software (i) may not refer to the modified
*  version by the same designation, or by any confusingly similar designation, and (ii) must refer to
*  the underlying software originally provided by Alliance as "Solar Power tower Integrated Layout and 
*  Optimization Tool" or "SolarPILOT". Except to comply with the foregoing, the terms "Solar Power 
*  tower Integrated Layout and Optimization Tool", "SolarPILOT", or any confusingly similar
*  designation may not be used to refer to any modified version of this software or any modified
*  version of the underlying software originally provided by Alliance without the prior written consent
*  of Alliance.
*
*  5. The name of the copyright holder, contributors, the United States Government, the United States
*  Department of Energy, or any of their employees may not be used to endorse or promote products
*  derived from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
*  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
*  FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER,
*  CONTRIBUTORS, UNITED STATES GOVERNMENT OR UNITED STATES DEPARTMENT OF ENERGY, NOR ANY OF THEIR
*  EMPLOYEES, BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
*  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
*  IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
*  THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************************************/


#ifndef _GUI_UTIL_
#define _GUI_UTIL_ 1

#include <string>
#include <vector>

#include <wx/wx.h>
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

class grid_emulator : public grid_emulator_base
{
 /*   std::vector<std::vector<wxString> > data;
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
        double mean=std::numeric_limits<double>::quiet_NaN(),
		double min=std::numeric_limits<double>::quiet_NaN(), 
		double max=std::numeric_limits<double>::quiet_NaN(), 
		double stdev=std::numeric_limits<double>::quiet_NaN());

	int GetNumberRows();
	int GetNumberCols();
	wxString GetRowLabelValue(int row);
	wxString GetColLabelValue(int col);
	wxString GetCellValue(int row, int col);
*/
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
