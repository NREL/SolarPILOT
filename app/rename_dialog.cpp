
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


#include "rename_dialog.h"

/* 
This control is a simple popup window for naming/renaming a heliostat template item.
*/

rename_dlg::rename_dlg() {};

rename_dlg::rename_dlg( wxWindow *parent, wxWindowID winid, wxString label, wxString defvalue,
    wxPoint pos, wxSize size, long style)
{
    Create(parent, winid, label, pos, size, style);

    wxBoxSizer *dlg_sizer = new wxBoxSizer(wxVERTICAL);
    _dlg_text = new wxTextCtrl(this, wxID_ANY, defvalue, wxDefaultPosition, wxSize(150,21),wxTE_PROCESS_ENTER);
    wxStaticText *dlg_lab = new wxStaticText(this, wxID_ANY, label);
    wxButton 
        *dlg_ok = new wxButton(this, wxID_ANY, wxT("Apply")),
        *dlg_cancel = new wxButton(this, wxID_ANY, wxT("Cancel"));
    wxBoxSizer 
        *dlg_top_s = new wxBoxSizer(wxHORIZONTAL),
        *dlg_bot_s = new wxBoxSizer(wxHORIZONTAL);
    dlg_top_s->Add(_dlg_text, 0, wxALL, 5);
    dlg_top_s->Add(dlg_lab, 0, wxALL, 5);
    dlg_bot_s->Add(dlg_ok, 0, wxALL, 5);
    dlg_bot_s->Add(dlg_cancel, 0, wxALL, 5);
    dlg_sizer->Add(dlg_top_s, 0, wxALIGN_CENTER_HORIZONTAL|wxTOP, 15);
    dlg_sizer->Add(dlg_bot_s, 0, wxALIGN_CENTER_HORIZONTAL, 0);
    this->SetSizer( dlg_sizer );
    this->CenterOnParent();

    dlg_ok->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( rename_dlg::OnButtonApply ), NULL, this);
    dlg_cancel->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( rename_dlg::OnButtonCancel ), NULL, this);
    _dlg_text->Connect(wxEVT_KEY_DOWN, wxKeyEventHandler( rename_dlg::OnText ), NULL, this);
        
};

wxString rename_dlg::getTextValue()
{
    return _name;
}
    
int rename_dlg::getRetCode()
{
    return _retcode;
}

void rename_dlg::OnButtonApply( wxCommandEvent &WXUNUSED(event))
{
    _name = _dlg_text->GetValue();
    _retcode = wxID_OK;
    this->EndModal(_retcode);
};

void rename_dlg::OnButtonCancel( wxCommandEvent &WXUNUSED(event))
{
    _name = wxT("");
    _retcode = wxID_CANCEL;
    this->EndModal(_retcode);
};

void rename_dlg::OnText( wxKeyEvent &event)
{
    /* Check for escape char or return char, otherwise continue */
        
    //get the key pressed
    int key = event.GetKeyCode();
    if( key == WXK_ESCAPE )
    {
        _retcode = wxID_CANCEL;
        this->EndModal(_retcode);
    }
    if( key == WXK_RETURN )
    {
        _retcode = wxID_OK;
        _name = _dlg_text->GetValue();
        this->EndModal(_retcode);
    }

    event.Skip();
};

