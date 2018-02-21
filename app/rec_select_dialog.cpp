#include "rec_select_dialog.h"
#include "SolarField.h"

using namespace std;

rec_select_dialog::rec_select_dialog()
{};
    
rec_select_dialog::rec_select_dialog(
    wxWindow *parent, 
    wxWindowID winid, 
    wxString label,
    SolarField *SF,
    wxPoint pos,
    wxSize size,
    long style)
{
    Create(parent, winid, label, pos, size, style);

    //Create objects and do layout
    nrec = SF->getReceivers()->size();
        
    wxStaticText *msgbox = new wxStaticText(this, wxID_ANY, 
        "Multiple receivers are enabled. Please select the receiver(s)\n"
        "for which you would like to generate flux maps...");

    wxGridSizer *check_sizer = new wxGridSizer(1, 5, 0);

    vector<Receiver*> *Recs = SF->getReceivers();

    checkboxes.clear();
    rmap.clear();
    for(int i=0; i<nrec; i++)
    {
        if(! Recs->at(i)->isReceiverEnabled() ) continue;
        wxString name = Recs->at(i)->getVarMap()->rec_name.val;
        checkboxes.push_back( new wxCheckBox(this, wxID_ANY, name) );
        rmap[ checkboxes.at(i) ] = Recs->at(i);
        check_sizer->Add(checkboxes.at(i));
    }

    wxButton
        *ok_button = new wxButton(this, wxID_ANY, wxT("Ok")),
        *cancel_button = new wxButton(this, wxID_ANY, wxT("Cancel"));
    ok_button->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( rec_select_dialog::OnOkButton ), NULL, this);
    cancel_button->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( rec_select_dialog::OnCancelButton), NULL, this);
    this->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler( rec_select_dialog::OnText ), NULL, this);

    wxBoxSizer *button_sizer = new wxBoxSizer(wxHORIZONTAL);
    button_sizer->Add(ok_button, 0, wxALL, 10);
    button_sizer->Add(cancel_button, 0, wxALL, 10);

    wxBoxSizer *main_sizer = new wxBoxSizer(wxVERTICAL);
    main_sizer->Add(msgbox, 0, wxALL, 5);
    main_sizer->Add(check_sizer, 0, wxALL, 10);
    main_sizer->Add(button_sizer, 0, wxALL, 5);
    this->SetSizer(main_sizer);
    this->CenterOnParent();

};

void rec_select_dialog::getSelectedReceivers(vector<Receiver*> &rec_ptrs)
{
    
    //Cycle through the checkbox map and set rec_ptrs to include pointers to all of the selected
    //receivers.
    rec_ptrs.clear();
    for( unordered_map<wxWindow*, Receiver*>::iterator it = rmap.begin(); it != rmap.end(); it++)
    {
        if( ((wxCheckBox*)(it->first))->GetValue() )
            rec_ptrs.push_back(it->second);
    }

};

void rec_select_dialog::OnOkButton(wxCommandEvent &WXUNUSED(event))
{
    this->EndModal(wxID_OK);
}

void rec_select_dialog::OnCancelButton(wxCommandEvent &WXUNUSED(event))
{
    this->EndModal(wxID_CANCEL);
}

void rec_select_dialog::OnText( wxKeyEvent &event)
{
    /* Check for escape char or return char, otherwise continue */
        
    //get the key pressed
    int key = event.GetKeyCode();
    if( key == WXK_ESCAPE )
    {
        this->EndModal(wxID_CANCEL);
    }
    if( key == WXK_RETURN )
    {
        this->EndModal(wxID_OK);
    }

    event.Skip();
};
