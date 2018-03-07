
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


#include "OutputControl.h"
#include "wx/textctrl.h"
#include "wx/stattext.h"
#include "wx/panel.h"
#include "wx/tooltip.h"
#include <algorithm>

using namespace std;

bool OutputControl::isOverride()
{
    return _varobj->is_disabled;
}

OutputControl::OutputControl(wxWindow *parent, wxWindowID winid, spbase &var,
                 const wxString &format, const wxPoint pos, const wxSize size, bool hide_labels) 
                 : wxPanel(parent, winid, pos, size) 
{
    _hide_labels = hide_labels;
    _varobj = &var;
    _parent = parent;
    _value = var.as_string(); 
    _label = var.short_desc; 
    _units = var.units; 
    if(_units == "none") _units = "";
    if(_units != "") _units = "["+_units+"]";
    //Check for a sizing override
    if(size != wxDefaultSize )
    {
        //Is the width less than the default control size (375,21) ?
        _size = wxSize(size.GetWidth(), max(size.GetHeight(), 21));
        
    }
    else
    {
        _size = wxSize(375, 21);
    }
    _format = format;


    Build();
}

void OutputControl::setValue(string value)
{
    setValue((wxString)value);
}

void OutputControl::setValue(wxString value)
{
    if(value.Find('.') == wxNOT_FOUND)
    {
        tc->SetValue(value);
    }
    else
    {
        //handle as a double
        double dval;
        to_double(value.ToStdString(), &dval);
        setValue(dval);
    }
}


void OutputControl::setValue(double value)
{ 
    string fmt;
    if(_units.find("$") != string::npos)
    {
        string fval = gui_util::FormatAsCurrency(value);
        tc->SetValue( wxT( "$ " + fval ) );
    }
    else
    {
        //Format using default
        wxString s;
        s.sprintf(_format.c_str(), value); 
        tc->SetValue( s );
    }
        
}

void OutputControl::Build()
{
    //Create colors
    colorTextBG = new wxColour(220, 220, 240);
    colorTextFG = new wxColour(25, 0, 255);

    //Calculate sizes
    wxSize tc_size, lab_size, unit_size;
    int ysize = _size.GetHeight();
    if( _label + _units == "")
    {
        unit_size = wxSize(0,ysize);
        tc_size = _size;
        lab_size = wxSize(0,ysize);
    }
    else
    {
        unit_size = wxSize(60,ysize);
        if(_hide_labels)
            tc_size = _size;
        else
            tc_size = wxSize(100,ysize);
        int lab_width = _size.GetWidth() - tc_size.GetWidth() - unit_size.GetWidth();
        lab_width = lab_width < 0 ? 0 : lab_width;
        lab_size = wxSize( lab_width, ysize);
    }
        
    //Initialize
    tc = new wxTextCtrl(this, wxID_ANY, _value, wxDefaultPosition,tc_size, wxBORDER_DOUBLE);        //Text control
    if(_units.find("$") != string::npos)
    {
        double dval; to_double(_value, &dval);
        setValue( dval );
    }
    
    if(_varobj->long_desc != "")
    {
        tip = new wxToolTip( _varobj->long_desc );
        tc->SetToolTip( tip );
        tc->GetToolTip()->Enable(true);
    }
        
    wxBoxSizer *bs = new wxBoxSizer(wxHORIZONTAL);
    
    if(! _hide_labels) 
    {
        st_label = new wxStaticText(this, -1, _label, wxDefaultPosition, lab_size, wxTE_RIGHT);    //Static label
        bs->Add(st_label, 0, wxEXPAND|wxRIGHT|wxTOP, 5);
    }
    
    bs->Add(tc, 0, wxALIGN_CENTRE_VERTICAL, 0);
    
    if(! _hide_labels)
    {
        st_units = new wxStaticText(this, -1, _units, wxDefaultPosition, unit_size, wxTE_LEFT);    //Static units
        bs->Add(st_units, 0, wxEXPAND|wxLEFT|wxTOP, 5);
    }
        
    this->Connect(wxEVT_PAINT, wxPaintEventHandler( OutputControl::OnPaint ), NULL, this);
    this->SetSizerAndFit(bs);
    this->Layout();
    
    //Set other properties
    tc->SetEditable(false);
    tc->SetForegroundColour(*colorTextFG);
    tc->SetBackgroundColour(*colorTextBG);

}

//Accessors
wxString OutputControl::getUnits()
{
    return _units;
}
wxString OutputControl::getValue()
{
    return _value;
} 
spbase* OutputControl::getVarObject()
{
    return _varobj;
}
void OutputControl::setVarObject(spbase* varobj)
{
    _varobj = varobj;
}

void OutputControl::setValue(int value)
{
    tc->SetValue(my_to_string( value ));
}

void OutputControl::OnPaint( wxPaintEvent &event)
{ 
    this->Layout();
        
    Refresh(false);
    event.Skip();
}
