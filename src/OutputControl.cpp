#include "OutputControl.h"
#include "wx/textctrl.h"
#include "wx/stattext.h"
#include "wx/panel.h"
#include "wx/tooltip.h"
#include <algorithm>

bool OutputControl::isOverride(){return _varobj->is_disabled;}

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
	if(size != wxDefaultSize ){
		//Is the width less than the default control size (375,21) ?
		_size = wxSize(size.GetWidth(), max(size.GetHeight(), 21));
		
	}
	else{
		_size = wxSize(375, 21);
	}
	_format = format;


	Build();
}

void OutputControl::setValue(string value){
	setValue((wxString)value);
}

void OutputControl::setValue(wxString value){
	if(value.Find('.') == wxNOT_FOUND){
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


void OutputControl::setValue(double value){ 
	string fmt;
	if(_units.find("$") != string::npos){
		string fval = gui_util::FormatAsCurrency(value);
		tc->SetValue( wxT( "$ " + fval ) );
	}
	else{
		//Format using default
		wxString s;
		s.sprintf(_format.c_str(), value); 
		tc->SetValue( s );
	}
		
}

void OutputControl::Build(){
	//Create colors
	colorTextBG = new wxColour(220, 220, 240);
	colorTextFG = new wxColour(25, 0, 255);

	//Calculate sizes
	wxSize tc_size, lab_size, unit_size;
	int ysize = _size.GetHeight();
	if( _label + _units == ""){
		unit_size = wxSize(0,ysize);
		tc_size = _size;
		lab_size = wxSize(0,ysize);
	}
	else{
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
	tc = new wxTextCtrl(this, wxID_ANY, _value, wxDefaultPosition,tc_size, wxBORDER_DOUBLE);		//Text control
	if(_units.find("$") != string::npos){
		double dval; to_double(_value, &dval);
		setValue( dval );
	}
	
	if(_varobj->long_desc != ""){
		tip = new wxToolTip( _varobj->long_desc );
		tc->SetToolTip( tip );
		tc->GetToolTip()->Enable(true);
		//tc->GetToolTip()->SetDelay(1500);
	}
		
    wxBoxSizer *bs = new wxBoxSizer(wxHORIZONTAL);
	
    if(! _hide_labels) 
    {
        st_label = new wxStaticText(this, -1, _label, wxDefaultPosition, lab_size, wxTE_RIGHT);	//Static label
	    bs->Add(st_label, 0, wxEXPAND|wxRIGHT|wxTOP, 5);
    }
	
    bs->Add(tc, 0, wxALIGN_CENTRE_VERTICAL, 0);
	
    if(! _hide_labels)
    {
	    st_units = new wxStaticText(this, -1, _units, wxDefaultPosition, unit_size, wxTE_LEFT);	//Static units
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
wxString OutputControl::getUnits(){return _units;}
wxString OutputControl::getValue(){return _value;} 
spbase* OutputControl::getVarObject(){return _varobj;}
void OutputControl::setVarObject(spbase* varobj)
{
    _varobj = varobj;
}

void OutputControl::setValue(int value){tc->SetValue(my_to_string( value ));}

void OutputControl::OnPaint( wxPaintEvent &event){ 
	this->Layout();
		
	Refresh(false);
	event.Skip();
}
