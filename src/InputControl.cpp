#include "InputControl.h"

#include "wx/textctrl.h"
#include "wx/checkbox.h"
#include "wx/stattext.h"
#include "wx/panel.h"
#include "wx/tooltip.h"
#include "wx/dialog.h"

#include <algorithm>
#include <sstream>

wxString InputControl::getUnits(){return _varobj->units;}
//wxString InputControl::getVarname(){return _varname;}
wxString InputControl::getValue(){return _value; }
string InputControl::getVarPath(){return _varobj->name;}
bool InputControl::needUpdate(){return _need_update;}
vector<string> InputControl::getComboChoices(){return _varobj->combo_get_choices();}
bool InputControl::isCombo(){if(_varobj->ctype == "combo"){return true;} else {return false;}}
bool InputControl::isCheckbox(){if(_varobj->ctype== "checkbox"){return true;} else {return false;}}
bool InputControl::isFilePath(){if(_varobj->ctype == "path") return true; else return false; }
bool InputControl::isOverride(){return _disabled;}


InputControl::InputControl(wxWindow *parent, wxWindowID winid, spbase &var, const wxPoint pos, const wxSize size, long style, const wxString &name, bool textonly, bool hide)
		: wxPanel(parent, winid, pos, size, style, name)
{
    _varobj = &var;
	_parent = parent;
	_disabled = hide || var.is_disabled;
	
    if(_varobj->units != "" && _varobj->units != "none"){
		_units = "["+_varobj->units+"]";
	}
	else{
		_units = "";
	}

	//Check for a sizing override
	if(size != wxDefaultSize ){
		//Is the width less than the default control size (375,21) ?
		_size = wxSize(size.GetWidth(), max(size.GetHeight(), 21));
		
	}
	else{
		_size = wxSize(375, 21);
	}
			
	_text_only = textonly;
	Build();			
};

InputControl::~InputControl()
{
    //is this control leaking?

    return;
}

string InputControl::getSelectedText(){
	if(_varobj->ctype == "combo") { 
		string k= (string)cb->GetValue(); 
		return k;
	} 
	else if(_varobj->ctype == "checkbox"){
		string k = chk->GetValue() ? "true" : "false";
		return k;
	}
	else {
		return "";
	}
};

int InputControl::getSelection()
{
    //Returns the index of the current selection. If a checkbox, returns 0 for false and 1 for true

	if(_varobj->ctype == "combo") 
        return cb->GetSelection();
	else if(_varobj->ctype == "checkbox")
        return chk->GetValue() ? 1 : 0;
    else
        throw spexception("Attempting to get the selection index from a non-choice control");

};

void InputControl::setPanelObject(string choice, wxPanel &panel){ 
	/*
	This method takes an instantiated panel and sets the pointer in the cmap structure equal to that value. 
	The process is as follows:
	1) Instantiate a wxPanel in the main GUI methods
	2) Associate that panel object with a particular selection in a combobox or checkbox using this function
	3) When the selection changes, this control automatically handles panel hiding/enabling
	*/

    if( _varobj->ctype == "combo" )
    {
        //_varobj->combo_select( choice );
        choicedat.at( choice ).has_panel = true;
        choicedat.at( choice ).panel = &panel;
    }
    else if( _varobj->ctype == "checkbox" )
    {
        if( choice != "true" && choice != "false" )
            throw spexception("Attempting to add a panel to a checkbox option that is neither \"true\" nor \"false\" and cannot proceed.");

        choicedat.at( choice ).has_panel = true;
        choicedat.at( choice ).panel = &panel;
    }
    else
        throw spexception("Attempting to add a choice panel to a non-choice control");

};

void InputControl::updateInputDisplay()
{
	//Hide and show panels according to the selection
    for( unordered_map<string, choice_helper>::iterator it = choicedat.begin(); it != choicedat.end(); it++)
        if( it->second.has_panel )
            it->second.panel->Hide(); //first hide all the panels

	//If the control is permanently disabled, return here
	if(_disabled) return;

    if( choicedat.at(_value).has_panel )
        choicedat.at(_value).panel->Show(true);

}

void InputControl::addDisabledSiblings(string sel, int n, wxWindow* p[] )
{
    
    for(int i=0; i<n; i++)
    {
        choicedat.at(sel).dis_sibs.push_back( p[i] );
        _all_dis_objs.push_back( p[i]);     //also add to vector of all disable objects
    }
}
void InputControl::addDisabledSiblings(string sel, wxWindow *p)
{
    choicedat.at(sel).dis_sibs.push_back( p );
    _all_dis_objs.push_back( p );
}

void InputControl::setDisabledSiblings(string sel, int n, wxWindow *p[])
{
    choicedat.at(sel).dis_sibs.clear();

    addDisabledSiblings(sel, n, p);
}
void InputControl::setDisabledSiblings(string sel, wxWindow *p)
{
    choicedat.at(sel).dis_sibs.clear();
    addDisabledSiblings(sel, p);
}

void InputControl::setColour(wxColour c){
	if(_varobj->ctype == "combo" || _varobj->ctype == "checkbox"){ return;}
	tc->SetBackgroundColour(c);
}

void InputControl::setValue(bool val)
{
	setValue(val ? 1 : 0);
}

void InputControl::setValue(int val)
{
	if(_varobj->ctype=="combo")
    { 
        cb->SetValue( _varobj->as_string() );
        return;
    }
	if(_varobj->ctype=="checkbox")
    { 
        chk->SetValue(val == 1); 
        return;
    }
	//Don't do anything for the regular text control here
}

void InputControl::setValue(string &val){
	/* 
	This version handles iteraction with the variable map, where all information is stored in 
	text format. The combobox setting corresponds to the mapped integer (NOT the selection 
	index), and the checkbox value will be either "true" or "false". Textcontrol data will be
	input as-is.
	*/

	if(_varobj->ctype == "combo"){	
		int ival;
		to_integer(val, &ival);
        setValue(ival);
	}
	else if(_varobj->ctype == "checkbox"){
		//convert the text "true" or "false" into a bool value
		chk->SetValue( lower_case( val ) == "true" );
	}
	else{
        if( _varobj->dattype == SP_DATTYPE::SP_DOUBLE )
        {
            double x;
            to_double(val, &x);
            stringstream fv;

            fv << x;

		    tc->SetValue( fv.str() );
        }
	}
}

void InputControl::setValue(double &val, string fmt){
	if(_varobj->ctype == "combo" || _varobj->ctype == "checkbox" || _varobj->ctype == "path") return;
	wxString sval;
	sval.sprintf(fmt.c_str(), val);
	tc->SetValue( sval );
}

void InputControl::selectAllText(){ 
	if(_varobj->ctype == "combo" || _varobj->ctype == "checkbox") return; 
	tc->SetFocus(); 
	tc->SetSelection(-1,-1); 
}

void InputControl::setVarObject(spbase *varobj)
{
    _varobj = varobj;
}

void InputControl::setTextOnlyMode(bool mode){
	_text_only = mode;
}

vector<wxWindow*> *InputControl::getAllDisabledSiblings()
{
    return &_all_dis_objs;
}

vector<wxWindow*> *InputControl::getDisabledSiblings(string &choice)
{
    return &choicedat.at(choice).dis_sibs;
}

void InputControl::updateComboChoices()
{
    if( ! isCombo() )
        return;
     

    vector<string> choice_keys = _varobj->combo_get_choices();
        int n = _varobj->combo_get_count();
    //initialize
    wxArrayString choices_temp;
    choicedat.clear();
    for(int i=0; i<n; i++)
    {
        //create
        choicedat[choice_keys.at(i)];

        //initialize
        choicedat.at(choice_keys.at(i)).has_panel = false;
        choices_temp.Add( choice_keys.at(i) );
	}

    cb->Clear();
    cb->Append( choices_temp );
    
    cb->SetValue(_varobj->as_string());
    return;
}

void InputControl::OnText( wxCommandEvent &WXUNUSED(event)){_need_update = false;}

void InputControl::OnBGPaintEvent( wxEraseEvent &event){
	event.Skip();
}
void InputControl::OnPaint( wxPaintEvent &event){ 
	this->Layout();
		
	Refresh(false);
	event.Skip();
}


void InputControl::OnCombo( wxCommandEvent &event){
	//handle combobox selection events
	_need_update = true;
    _value = cb->GetValue();
	_varobj->combo_select_by_choice_index( cb->GetSelection() );	//cselect is the position index of the selected item in the list

	updateInputDisplay();
		
	event.Skip();
}

void InputControl::OnCheck( wxCommandEvent &event){
	//Handle checkbox events
	_need_update = true;
    string state = getSelection() == 1 ? "true" : "false"; //0 or 1
    _varobj->set_from_string( state.c_str() );
	_value = state;	//Update the local value to be true of false
	
    updateInputDisplay();

	event.Skip();

}

void InputControl::OnFile( wxCommandEvent &WXUNUSED(event)){
	wxFileDialog *fdlg = new wxFileDialog(this, "Select file path", wxEmptyString, _value);
	if( fdlg->ShowModal() == wxID_OK ){
		_need_update = true;
        _value = (string)fdlg->GetPath();
		_varobj->set_from_string( _value.c_str() );
	}
}

void InputControl::OnFocus( wxFocusEvent &event){
	//On entering the control, keep track of the original value
	_oldtext = tc->GetValue();
	event.Skip();
}

void InputControl::OnLeaving( wxFocusEvent &event){
	/*
	Whenever the text control loses focus, we need to update the GUI with the new value. Return 
	*/
	
	if(_varobj->get_data_type() == SP_DATTYPE::SP_DOUBLE || _varobj->get_data_type() == SP_DATTYPE::SP_INT) // _dattype == "double" || _dattype == "int"){
    {

		//check for illegal characters
		double val;
		if( ! tc->GetValue().ToDouble(&val) ){
			/* This isn't a valid number. Reset to the stored value */
			//tc->SetValue( (*_V)[_vargroup][_inst][_varname].value);
            string v; 
            _varobj->as_string(v);
            tc->SetValue( v );
			return;
		}
		//Do a range check
		//int btype;
		//if(! checkRange( _range, val, &btype) ){
		//	/* If the range check returns false, notify the user and reset to the stored value */
		//	wxFormatString fmt = wxT("The value entered (%s) for \"%s\" is outside of the valid range. The allowable value range for this input is %s.");
		//	wxString msg;
		//	msg.sprintf(fmt, tc->GetValue().c_str(), _label.c_str(), _range.c_str());
		//	wxMessageDialog *dlg = new wxMessageDialog(this, msg, wxT("Range error"), wxOK|wxICON_ERROR);
		//	dlg->ShowModal();

		//}
	}
	
    string v; 
    _varobj->as_string(v);
	if(tc->GetValue() == v){
		_need_update = false; 
	} 
	else {
		_need_update = true;
		string val = (string)tc->GetValue();
		_varobj->set_from_string( val.c_str() );	//Set the value in the variable map
		_value = val;	//Also set the value locally
	}

	//Send the event up the chain
	event.SetEventObject( this );
	ProcessEvent( event );
}

void InputControl::Build(){
	is_binding_set = false;	//initialize flag to indicate that no bindings have been set

	//Decide whether we  want a combo box or a textctrl and do associated setup
	if(_varobj->ctype == "combo")
	{
		/* 
		For combobox's, the default selection is indicated by the choice name
		*/

        vector<string> choice_keys = _varobj->combo_get_choices();
        int n = _varobj->combo_get_count();
        
        _value = _varobj->as_string(); // choice_keys.at( _varobj->Cselect() );


        //initialize
        wxArrayString choices_temp;
        choicedat.clear();
        for(int i=0; i<n; i++)
        {
            //create
            choicedat[choice_keys.at(i)];

            //initialize
            choicedat.at(choice_keys.at(i)).has_panel = false;
            choices_temp.Add( choice_keys.at(i) );
		}

		//Create the combobox
		cb = new wxComboBox(this, wxID_ANY, _value, wxDefaultPosition, wxSize(150,max(25,_size.GetHeight())), choices_temp, wxCB_DROPDOWN|wxCB_READONLY);
		st_label = new wxStaticText(this, -1, _varobj->short_desc, wxDefaultPosition, wxSize(_size.GetWidth()-170,_size.GetHeight()), wxTE_RIGHT); //static label
		
		//Connect the control to the update method if the combo selection is changed
		cb->Connect(wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler(InputControl::OnCombo), NULL, this);
		cb->Connect(wxEVT_MOUSEWHEEL, wxCommandEventHandler(InputControl::OnMouseWheelSkip), NULL, this);
		//Do the layout
		wxBoxSizer *bs = new wxBoxSizer(wxHORIZONTAL);
		bs->Add(st_label, 0, wxEXPAND|wxRIGHT|wxTOP, 10);
		bs->Add(cb, 0, wxEXPAND|wxALL, 5);

		this->SetSizerAndFit(bs);
			
	}
	else if(_varobj->ctype == "checkbox")
    {
		//create a map of true/false and values to return
        choicedat.clear();

        //create
        choicedat["true"];
        choicedat["false"];

        //initialize
        choicedat.at("true").has_panel = false;  //"false"
        choicedat.at("false").has_panel = false;  //"true"
        
		//Create the checkbox
		chk = new wxCheckBox(this, wxID_ANY, "");
		st_label = new wxStaticText(this, -1, _varobj->short_desc, wxDefaultPosition, wxSize(_size.GetWidth()-115, _size.GetHeight()), wxALIGN_LEFT); //static label

		//Connect the control to the update method if the check box state has changed
		chk->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(InputControl::OnCheck), NULL, this);

		//Do the layout
		wxBoxSizer *bs = new wxBoxSizer(wxHORIZONTAL);
		bs->Add(chk, 0, wxALL, 5);
		bs->Add(st_label, 0, wxEXPAND|wxTOP, 5);
		if(_varobj->long_desc != ""){
			tip = new wxToolTip( _varobj->long_desc );
			chk->SetToolTip( tip );
			chk->GetToolTip()->Enable(true);
			//chk->GetToolTip()->SetDelay(750);
		}

		//Set the default state
        _value = _varobj->as_string();
		if(lower_case(_value) == "true"){ chk->SetValue( true ); }

		this->SetSizerAndFit(bs);
		
	}
	else if(_varobj->ctype == "path")
    {
        
        _value = _varobj->as_string();

		tc = new wxTextCtrl(this, wxID_ANY, _value, wxDefaultPosition, wxSize(150, _size.GetHeight()));
		wxButton *fbut = new wxButton(this, wxID_ANY, "...", wxDefaultPosition, wxSize(20, _size.GetHeight()));
		if(! _text_only){
			st_label = new wxStaticText(this, -1, _varobj->short_desc, wxDefaultPosition, wxSize(max(215,_size.GetWidth()-175),_size.GetHeight()), wxTE_RIGHT);	//Static label
		}
		if(_varobj->long_desc != ""){
			tip = new wxToolTip( _varobj->long_desc );
			tc->SetToolTip( tip );
			tc->GetToolTip()->Enable(true);
			//tc->GetToolTip()->SetDelay(750);
		}

		tc->Connect(wxEVT_KILL_FOCUS, wxFocusEventHandler(InputControl::OnLeaving), NULL, this );
		tc->Connect(wxEVT_SET_FOCUS, wxFocusEventHandler(InputControl::OnFocus), NULL, this );
		tc->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(InputControl::OnText), NULL, this);
		fbut->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(InputControl::OnFile), NULL, this);

		wxBoxSizer *bs = new wxBoxSizer(wxHORIZONTAL);
		if(! _text_only) bs->Add( st_label, 0, wxEXPAND|wxRIGHT|wxTOP, 5);
		bs->Add(tc, 0, wxALIGN_CENTRE_VERTICAL, 0);
		bs->Add(fbut, 0, wxALIGN_CENTER_VERTICAL, 0);
		this->SetSizerAndFit(bs);

	}
	else {		//Text control
        _value = _varobj->as_string();
        if( _varobj->dattype == SP_DATTYPE::SP_DOUBLE )
        {
            double x;
            to_double(_value, &x);
            stringstream fv;

            fv << x;

            _value = fv.str();
        }
		//Initialize
		tc = new wxTextCtrl(this, wxID_ANY, _value, wxDefaultPosition, wxSize(_text_only ? _size.GetWidth() : 100,_size.GetHeight()));		//Text control
		if(! _text_only){
			st_label = new wxStaticText(this, -1, _varobj->short_desc, wxDefaultPosition, wxSize(max(215,_size.GetWidth()-175),_size.GetHeight()), wxTE_RIGHT);	//Static label
			st_units = new wxStaticText(this, -1, _units, wxDefaultPosition, wxSize(60,_size.GetHeight()), wxTE_LEFT);	//Static units
		}
		if(_varobj->long_desc != ""){
			tip = new wxToolTip( _varobj->long_desc );
			tc->SetToolTip( tip );
			tc->GetToolTip()->Enable(true);
			//tc->GetToolTip()->SetDelay(750);
		}
		
		//Bind the focus event directly to the textctrl
		// Note that the "NULL" and "this" parameters **MUST** be included in the argument, otherwise any
		// access functions in the handler will cause memory errors!
		tc->Connect(wxEVT_KILL_FOCUS, wxFocusEventHandler(InputControl::OnLeaving), NULL, this );
		tc->Connect(wxEVT_SET_FOCUS, wxFocusEventHandler(InputControl::OnFocus), NULL, this );
		tc->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(InputControl::OnText), NULL, this);

		wxBoxSizer *bs = new wxBoxSizer(wxHORIZONTAL);
		if(!_text_only) bs->Add(st_label, 0, wxEXPAND|wxRIGHT|wxTOP, 5);
		bs->Add(tc, 0, wxALIGN_CENTRE_VERTICAL, 0);
		if(!_text_only) bs->Add(st_units, 0, wxEXPAND|wxLEFT|wxTOP, 5);
		this->SetSizerAndFit(bs);
	}
	this->Connect(wxEVT_ERASE_BACKGROUND, wxEraseEventHandler( InputControl::OnBGPaintEvent ), NULL, this);
	this->Connect(wxEVT_PAINT, wxPaintEventHandler( InputControl::OnPaint ), NULL, this);
	this->Layout();
}

void InputControl::OnMouseWheelSkip(wxCommandEvent &event){
	GetParent()->GetEventHandler()->ProcessEvent(event);
}

spbase *InputControl::getVarObject()
{
    return _varobj;
}