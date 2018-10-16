
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


#include "layout_sim_export_dialog.h"

using namespace std;

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
    labels.push_back(wxT("Heliostat annual power production"));
    labels.push_back(wxT("Heliostat annual total efficiency"));
        
    nchecks = (int)labels.size();    //Match the number of setting options enumerated below

    //Set the the default value for each option
    bool defaults[] = {true, true, true, false, true, true, true, true, true, true, true, true, true, false, false, false, true, true};

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

