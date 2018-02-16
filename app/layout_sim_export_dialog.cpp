#include "layout_sim_export_dialog.h"

layout_export_dialog::layout_export_dialog(
    wxWindow *parent, wxWindowID winid, wxString label,wxPoint pos, wxSize size, long style)
    : wxDialog(parent, winid, label, pos, size, style)
{
    //Create objects and do layout
    wxStaticText *msgbox = new wxStaticText(this, wxID_ANY, 
        wxT("Please select the data you would like to export:"));
            
    wxGridSizer *check_sizer = new wxGridSizer(2, 5, 10);

    wxArrayStr labels;
    labels.push_back(wxT("Heliostat ID Number"));
    labels.push_back(wxT("Heliostat location (x,y,z)"));
    labels.push_back(wxT("Heliostat aim point on the receiver (x,y,z)"));
    labels.push_back(wxT("Tracking vector (i,j,k)"));
    labels.push_back(wxT("Heliostat reflectivity"));
    labels.push_back(wxT("Heliostat soiling factor"));
    labels.push_back(wxT("Total heliostat efficiency"));
    labels.push_back(wxT("Heliostat cosine efficiency"));
    labels.push_back(wxT("Heliostat attenuation efficiency"));
    labels.push_back(wxT("Heliostat blocking efficiency"));
    labels.push_back(wxT("Heliostat shadowing efficiency"));
    labels.push_back(wxT("Heliostat intercept efficiency"));
    labels.push_back(wxT("Heliostat delivered power"));
    labels.push_back(wxT("Heliostat ranking metric"));
    labels.push_back(wxT("Heliostat shadow coordinates"));
    labels.push_back(wxT("Heliostat cloudiness loss"));
        
    nchecks = (int)labels.size();    //Match the number of setting options enumerated below

    //Set the the default value for each option
    bool defaults[] = {true, true, true, false, true, true, true, true, true, true, true, true, true, false, false, false};

    //Set up each object
    checkboxes.clear();
    states.clear();
    for(int i=0; i<nchecks; i++)
    {
        checkboxes.push_back( new wxCheckBox(this, wxID_ANY, labels[i]) );
        checkboxes.at(i)->SetValue(defaults[i]);
        check_sizer->Add(checkboxes.at(i));
        states.push_back(defaults[i]);
    }
        
    sep_options.Clear();
    sep_options.push_back(wxT("Tab"));
    sep_options.push_back(wxT("Comma"));
    sep_options.push_back(wxT("Semi-colon"));
    sep_options.push_back(wxT("Space"));
    sep_actuals.Clear();
    sep_actuals.push_back(wxT("\t"));
    sep_actuals.push_back(wxT(","));
    sep_actuals.push_back(wxT(";"));
    sep_actuals.push_back(wxT(" "));
        
    sep_combo = new wxComboBox(this, wxID_ANY, sep_options[1], wxDefaultPosition, wxDefaultSize, sep_options, wxCB_READONLY|wxCB_DROPDOWN);
    wxStaticText *sep_label = new wxStaticText(this, wxID_ANY, wxT("File delimiter"));
    wxBoxSizer *cb_sizer = new wxBoxSizer(wxHORIZONTAL);
    cb_sizer->Add(sep_combo, 0, wxRIGHT|wxALIGN_CENTER_VERTICAL, 5);
    cb_sizer->Add(sep_label, 0, wxALIGN_CENTER_VERTICAL, 0);
    check_sizer->Add(cb_sizer);

    wxButton
        *ok_button = new wxButton(this, wxID_ANY, wxT("Ok")),
        *cancel_button = new wxButton(this, wxID_ANY, wxT("Cancel"));
    ok_button->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( layout_export_dialog::OnOkButton ), NULL, this);
    cancel_button->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( layout_export_dialog::OnCancelButton), NULL, this);
    this->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler( layout_export_dialog::OnText ), NULL, this);

    wxBoxSizer *button_sizer = new wxBoxSizer(wxHORIZONTAL);
    button_sizer->Add(ok_button, 0, wxALL, 10);
    button_sizer->Add(cancel_button, 0, wxALL, 10);

    wxBoxSizer *main_sizer = new wxBoxSizer(wxVERTICAL);
    main_sizer->Add(msgbox, 0, wxALL, 5);
    main_sizer->Add(check_sizer, 0, wxALL, 10);
    main_sizer->Add(button_sizer, 0, wxALL, 5);
    this->SetSizerAndFit(main_sizer);
    this->CenterOnParent();

};

void layout_export_dialog::getSelections(vector<bool> &States)
{
    States.clear();
    for(int i=0; i<nchecks; i++)
    {
        States.push_back(states.at(i));
    }
};

wxString layout_export_dialog::getDelimiter()
{
    return sep_actuals[sep_combo->GetSelection()];
}

void layout_export_dialog::OnOkButton(wxCommandEvent &WXUNUSED(event))
{
    //Update the states vector
    for(int i=0; i<nchecks; i++)
    {
        states.at(i) = checkboxes.at(i)->GetValue();
    }
                
    this->EndModal(wxID_OK);
}

void layout_export_dialog::OnCancelButton(wxCommandEvent &WXUNUSED(event))
{
    this->EndModal(wxID_CANCEL);
}

void layout_export_dialog::OnText( wxKeyEvent &event)
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

