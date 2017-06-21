#include "settings_dialog.h"

settings_dialog::settings_dialog( wxWindow *parent, wxWindowID winID, wxString label, gui_settings &gs, wxSize size)
		: wxDialog(parent, winID, label, wxDefaultPosition, size)
{


	//The tools->settings menu has been selected. Construct and display a settings dialog
	int n_settings = gs.data_size();

	//wxPanel *set_dlg = new wxPanel(this, wxID_ANY);
	wxFlexGridSizer *grids = new wxFlexGridSizer(2, 5, 10);

	setting *sets = gs.getFirst();
	tcs.clear();
	for(int i=0; i<n_settings; i++){
		tcs.push_back( new wxTextCtrl(this, wxID_ANY, sets->value, wxDefaultPosition, wxSize(60, 21)) );
		grids->Add(tcs.at(i));
		wxStaticText *st = new wxStaticText(this, wxID_ANY, sets->label, wxDefaultPosition, wxSize(size.GetWidth()-60, 21));
		grids->Add(st);
		
		//map the text control to the setting object
		set_map[ tcs.at(i) ] = sets;

		sets = gs.getNext();
	}

	wxButton
		*ok_button = new wxButton(this, wxID_ANY, wxT("Apply"), wxDefaultPosition, wxSize(120, 25)),
		*cancel_button = new wxButton(this, wxID_ANY, wxT("Cancel"), wxDefaultPosition, wxSize(120,25));
	wxBoxSizer *bsizer = new wxBoxSizer(wxHORIZONTAL);
	bsizer->Add(ok_button, 0, wxALL, 10);
	bsizer->Add(cancel_button, 0, wxALL, 10);

	ok_button->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( settings_dialog::OnOkButton ), NULL, this);
	cancel_button->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( settings_dialog::OnCancelButton), NULL, this);

	wxBoxSizer
		*main_sizer = new wxBoxSizer(wxVERTICAL);
	main_sizer->Add(grids, 0, wxALL, 10);
	main_sizer->Add(bsizer, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 10);

	this->SetSizerAndFit(main_sizer);
	this->CenterOnParent();

}

void settings_dialog::OnOkButton(wxCommandEvent &WXUNUSED(event)){
	for(unordered_map<wxTextCtrl*, setting*>::iterator it = set_map.begin(); it != set_map.end(); it++){
		it->second->value = it->first->GetValue();	//Update the setting value with the textctrl value
	}

	this->EndModal(wxID_OK);
}

void settings_dialog::OnCancelButton(wxCommandEvent &WXUNUSED(event)){
	this->EndModal(wxID_CANCEL);
}


