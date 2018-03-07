
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


#include "settings_dialog.h"

settings_dialog::settings_dialog( wxWindow *parent, wxWindowID winID, wxString label, gui_settings &gs, wxSize size)
        : wxDialog(parent, winID, label, wxDefaultPosition, size)
{


    //The tools->settings menu has been selected. Construct and display a settings dialog
    int n_settings = gs.data_size();

    wxFlexGridSizer *grids = new wxFlexGridSizer(2, 5, 10);

    setting *sets = gs.getFirst();
    tcs.clear();
    for(int i=0; i<n_settings; i++)
    {
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

void settings_dialog::OnOkButton(wxCommandEvent &WXUNUSED(event))
{
    for(unordered_map<wxTextCtrl*, setting*>::iterator it = set_map.begin(); it != set_map.end(); it++)
    {
        it->second->value = it->first->GetValue();    //Update the setting value with the textctrl value
    }

    this->EndModal(wxID_OK);
}

void settings_dialog::OnCancelButton(wxCommandEvent &WXUNUSED(event))
{
    this->EndModal(wxID_CANCEL);
}


