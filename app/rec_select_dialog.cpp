
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
