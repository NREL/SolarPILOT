
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


#ifndef _OUTPUTCONTROL_
#define _OUTPUTCONTROL_ 

#include "wx/wx.h"
#include "gui_util.h"


/*
This is a custom output control that provides a single object for calculations, values, units labels, and variable naming.
This control is built to interface with the performance code variable array.

The format tags follow this prototype:
%[flags][width][.precision][length]specifier 
Where specifier is the most significant one and defines the type and the interpretation of the value of the coresponding argument:
specifier    Output                                                        Example
-----------------------------------------------------------------------------------
c            Character                                                    a
d or i        Signed decimal integer                                        392
e            Scientific notation (mantissa/exponent) using e character    3.9265e+2
E            Scientific notation (mantissa/exponent) using E character    3.9265E+2
f            Decimal floating point                                        392.65
g            Use the shorter of %e or %f                                    392.65
G            Use the shorter of %E or %f                                    392.65
o            Unsigned octal                                                610
s            String of characters                                        sample
u            Unsigned decimal integer                                    7235
x            Unsigned hexadecimal integer                                7fa
X            Unsigned hexadecimal integer (capital letters)                7FA
p            Pointer address                                                B800:0000
n            Nothing printed. The argument must be a pointer to a signed int, where 
            the number of characters written so far is stored.    
%            A % followed by another % character will write % to stdout.    %
*/

class OutputControl : public wxPanel
{
    
private:
    //Declare constituent components
    bool _hide_labels;
    spbase *_varobj;
    wxTextCtrl *tc;
    wxStaticText *st_units, *st_label;
    std::string 
        _label,        //The label to display in the interface
        _units,        //The units to display in the interface
        _value,        //Value to set in the interface
        _format;    //format string for display
    wxWindow *_parent;
    wxSize _size;
    wxToolTip *tip;
    //---Create color objects
    wxColour 
        *colorTextBG,
        *colorTextFG;

    
public:
    
    OutputControl(wxWindow *parent,
                 wxWindowID winid,
                 spbase &var,
                 const wxString &format = "%.1f",
                 const wxPoint pos = wxDefaultPosition,
                 const wxSize size = wxDefaultSize,
                 bool hide_labels = false);

    //Accessors
    bool isOverride();
    wxString getUnits();
    wxString getValue();
    spbase *getVarObject();
    void setVarObject(spbase* varobj);
    void setValue(std::string value);
    void setValue(wxString value);
    void setValue(double value); 
    void setValue(int value);

protected:
    void Build();
};

#endif
