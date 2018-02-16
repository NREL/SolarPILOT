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
    void setValue(string value);
    void setValue(wxString value);
    void setValue(double value); 
    void setValue(int value);

protected:
    void Build();
    void OnPaint( wxPaintEvent &event);
};

#endif
