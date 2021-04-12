
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



#include "gui_util.h"
#include "mod_base.h"    //from the PILOT code
#include "IOUtil.h"
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

using namespace std;

// ----------------- gui settings ----------------
gui_settings::gui_settings()
{
    i_next = 0;
    data.clear();
    order.clear();
};

void gui_settings::clear()
{
    data.clear();
    order.clear();
    i_next = 0;
};
void gui_settings::add( vector<string> &line )
{
    add( line.at(0), line.at(1), line.at(2) );
};
void gui_settings::add( string &varname, string &value, string &label)
{
    data[varname].varname = varname;
    data[varname].value = value;
    data[varname].label = label;
    order.push_back(varname);
};
setting &gui_settings::operator[](string varname)
{
    return data[varname];
};
setting &gui_settings::get(string varname)
{
    return data[varname];
};
setting *gui_settings::getFirst()
{
    i_next=1;
    return &data[order.at(0)];
        
}
setting *gui_settings::getNext()
{
    if(i_next < (int)data.size())
        return &data[order.at(i_next++)];
    else
        return (setting*)NULL;
}
int gui_settings::data_size()
{
    return (int)order.size();
};

// ------------------ array string
wxArrayStr::wxArrayStr()
{};
    
wxArrayStr::wxArrayStr( ArrayString &array )
{
    Clear();
    for(int i=0; i<array.size(); i++)
        push_back( array[i] );
};
wxArrayStr &wxArrayStr::operator=(const wxArrayString &rhs)
{
    Clear();
    for(int i=0; i<(int)rhs.size(); i++)
    {
        push_back(rhs[i]);
    }
    return *this;
};


MyToggleButton::MyToggleButton() {};


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

bool MyToggleButton::Toggle()
{
    return SetState(! _state); //returns the new state
}

bool MyToggleButton::SetState(bool state)
{
    _state = state;
    if(state)
    {
        SetBitmap(_onbit);
    }
    else
    {
        SetBitmap(_offbit);
    }
    return _state;
}

bool MyToggleButton::GetState()
{
    return _state;
}

void MyToggleButton::OnButtonEvent( wxCommandEvent &evt)
{
    Toggle();
    evt.Skip();    //pass up the chain
}

int gui_util::CalcBestSigFigs(double val)
{
    //Calculate and return the best number of decimal sig figs for the given value. Always show all integer values.
    return max( 3 - int(ceil( log10( val ) ) ), 1);
}

string gui_util::FormatWithCommas(int val)
{
    string
        sval = to_string(val, "%d");
    int ns = sval.size();
    int nct = 1;
    int i=ns-1; 
    while(i > 1)
    {
        if(nct == 3)
        {
            sval.insert(sval.begin()+i, ',');
            nct = 0;
        }
        nct++;
        i--;
    }
    return sval;
}

string gui_util::FormatWithCommas(double val, string fmt)
{
    string sval = to_string(val, fmt.c_str());
    int ns = sval.size();
    int nct = -1;
    int i=ns-1;
    while(i > 1)
    {
        if(sval.at(i) == '.') nct = 0;    //Don't start inserting commas until the decimal is hit
        if(nct < 0)
        {
            i--;
            continue;
        }

        if(nct == 3)
        {
            sval.insert(sval.begin()+i, ',');
            nct=0;
        }
        nct++;
        i--;
    }
    return sval;

}

string gui_util::FormatAsCurrency(double val)
{
    //format for currency
    string sval = to_string(val, "%.2f");
    vector<string> sint = split(sval, ".");
    int nsint = sint.at(0).size();
    int ncomma = int(floor(double(nsint-1)/3.));
    int cloc = nsint-ncomma*3;
    string fval="";
    int i=0;
    while(i < nsint)
    {
        char c = sval.at(i);
        fval += c;
        if(i==cloc-1 && i < nsint-1)
        {
            fval.append(",");
            cloc += 3;
        }
        i++;
    }

    fval.append("."+sint.at(1));
    return fval;
}

wxDateTime::Month gui_util::intTowxMonth(int month)
{
    
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
void grid_emulator::GetPrintableTable(wxArrayStr& printable, wxString eol)
{
    printable.Clear();
    wxString hdr;
    for (int i = 0; i < _ncol; i++)
        hdr.Append(", " + collabs.at(i));
    printable.Add(hdr + eol);

    for (int i = 0; i < _nrow; i++)
    {
        wxString line = rowlabs.at(i);
        for (int j = 0; j < _ncol; j++)
        {
            wxString tval = GetCellValue(i, j);
            tval.Replace(",", "");
            line.Append(", " + tval);    //Remove any commas from cell values - the file is comma-delimited
        }
        printable.Add(line + eol);
    }

}

void grid_emulator::MapToWXGrid(wxGrid* grid)
{

    if (grid->GetNumberRows() != GetNumberRows())
    {
        grid->DeleteRows(0, grid->GetNumberRows());
        grid->AppendRows(GetNumberRows());
    }
    if (grid->GetNumberCols() != GetNumberCols())
    {
        grid->DeleteCols(0, grid->GetNumberCols());
        grid->AppendCols(GetNumberCols());
    }

    for (int i = 0; i < _nrow; i++)
        grid->SetRowLabelValue(i, rowlabs.at(i));
    for (int i = 0; i < _ncol; i++)
        grid->SetColLabelValue(i, collabs.at(i));
    for (int i = 0; i < _nrow; i++)
        for (int j = 0; j < _ncol; j++)
            grid->SetCellValue(i, j, GetCellValue(i, j));

    int nrow = grid->GetNumberRows(),
        ncol = grid->GetNumberCols();
    for (int i = 0; i < ncol; i++)
    {
        for (int j = 0; j < nrow; j++)
        {
            grid->SetReadOnly(j, i, true);
            if (grid->GetCellValue(j, i).empty())
                grid->SetCellBackgroundColour(j, i, wxColour(100, 100, 100, 255));
            else if (i > 1)
                grid->SetCellBackgroundColour(j, i, wxColour(225, 225, 225, 255));
        }
    }
}

bool gui_util::list_files( std::string &directory, ArrayString &files, std::string filters )
{
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
    else
    { 
        types = split(filters, " ");
    }

    files.clear();
    bool cont = dir.GetFirst(&filename, "", wxDIR_FILES);
    while(cont)
    {
        //only add if one of the extensions matches the file type
        sname = split((string)filename.c_str(), ".");
        for(unsigned int i=0; i<types.size(); i++)
        {
            if(lower_case(sname.back()) == types.at(i))
            { 
                files.push_back((string)filename.c_str());
                break;
            }
        }
        cont = dir.GetNext(&filename);
    }
    
    return true;

}

void gui_util::parseSettingsFile(string fname, gui_settings &gs)
{
    string files, eols;
    ioutil::read_file(fname, files, eols);
    wxString file(files), eol(eols);
    
    std::vector<std::string> lines, line;
    lines = split(file.ToStdString(), eol.ToStdString());
    gs.clear();
    for(unsigned int i=0; i<lines.size(); i++)
    {
        line = split(lines.at(i), "\t");
        gs.add(line);
    }

}

bool gui_util::writeSettingsFile(string &fname, gui_settings &gs)
{
    
    wxTextFile fobj(fname);
    fobj.Open();
    if( ! fobj.IsOpened() ) return false;
    
    fobj.Clear();
    wxFormatString fmt = wxT("%s\t%s\t%s");
    setting *set = gs.getFirst();
    for(int i=0; i<gs.data_size(); i++)
    {
        wxString line;
        line.sprintf(fmt, set->varname.c_str(), set->value.c_str(), set->label.c_str());
        fobj.AddLine(line);
        set = gs.getNext();
    }
    bool writeok = fobj.Write();
    fobj.Close();
    return writeok;
}

wxString gui_util::error_list(int err_ind)
{
	/*
	Return an error message according to the specified error index.
	*/
	switch (err_ind)
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
}