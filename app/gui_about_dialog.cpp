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


#include "gui_about_dialog.h"
#include <wx/clipbrd.h>
#include <wx/dcbuffer.h>

using namespace std;

gui_about::gui_about()
{}
    
gui_about::gui_about(wxWindow *parent, wxWindowID id, string &bg_name, string &version, wxIcon icon )
{
    
    _bitmap.LoadFile(bg_name, wxBITMAP_TYPE_PNG);
    wxPoint pos = wxPoint(100,100);
    wxSize imgsize = wxSize(_bitmap.GetWidth(), _bitmap.GetHeight());

    _version = version;
    Create(parent, id, wxT("SolarPILOT | version "+version), wxDefaultPosition, imgsize, wxDEFAULT_FRAME_STYLE & ~ (wxRESIZE_BORDER | wxMAXIMIZE_BOX));
    CenterOnParent();
    SetIcon( icon );

    //Need to have a panel as the DC background
    _panel = new wxPanel(this);
    _panel->SetBackgroundStyle(wxBG_STYLE_PAINT);
    wxBoxSizer *main_sizer = new wxBoxSizer(wxVERTICAL);
    main_sizer->Add(_panel,1,wxEXPAND,0);

	_panel->Bind(wxEVT_PAINT, &gui_about::OnPaint, this);
        
    this->SetSizer(main_sizer);
    this->SetSizeHints( imgsize );
    this->Layout();
        
}
    
void gui_about::OnPaint(wxPaintEvent &event)
{
    
	wxAutoBufferedPaintDC dc(_panel);

    dc.DrawBitmap( _bitmap, 0, 0);
    dc.SetFont( wxFont(20, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD) );
    dc.SetPen( *wxBLACK_PEN );
    string msg = "Software version "+_version;
    wxSize mext = dc.GetTextExtent(msg);
    dc.DrawText(msg, _bitmap.GetWidth()/2 - mext.GetWidth()/2, 280);
    event.Skip();
}


//---------------------------------------------------------------------------------------

gui_license::gui_license(wxWindow *parent, wxWindowID id, wxIcon icon)
: wxFrame(parent, id, "SolarPILOT License", wxDefaultPosition, wxSize(400,350), wxDEFAULT_FRAME_STYLE|wxSTAY_ON_TOP)
{
    SetIcon( icon );

    wxPanel *main_panel = new wxPanel(this);
    _lic_msg = 
		"Copyright 2018 Alliance for Sustainable Energy, LLC "
		"\n\n"
		"NOTICE: This software was developed at least in part by Alliance for Sustainable Energy, LLC "
		"(\"Alliance\") under Contract No. DE-AC36-08GO28308 with the U.S. Department of Energy and the U.S. "
		"The Government retains for itself and others acting on its behalf a nonexclusive, paid-up, "
		"irrevocable worldwide license in the software to reproduce, prepare derivative works, distribute "
		"copies to the public, perform publicly and display publicly, and to permit others to do so. "
		"\n\n"
		"Redistribution and use in source and binary forms, with or without modification, are permitted "
		"provided that the following conditions are met: "
		"\n\n"
		"1. Redistributions of source code must retain the above copyright notice, the above government "
		"rights notice, this list of conditions and the following disclaimer. "
		"\n\n"
		"2. Redistributions in binary form must reproduce the above copyright notice, the above government "
		"rights notice, this list of conditions and the following disclaimer in the documentation and/or "
		"other materials provided with the distribution. "
		"\n\n"
		"3. The entire corresponding source code of any redistribution, with or without modification, by a "
		"research entity, including but not limited to any contracting manager/operator of a United States "
		"National Laboratory, any institution of higher learning, and any non-profit organization, must be "
		"made publicly available under this license for as long as the redistribution is made available by "
		"the research entity. "
		"\n\n"
		"4. Redistribution of this software, without modification, must refer to the software by the same "
		"designation. Redistribution of a modified version of this software (i) may not refer to the modified "
		"version by the same designation, or by any confusingly similar designation, and (ii) must refer to "
		"the underlying software originally provided by Alliance as \"Solar Power tower Integrated Layout and "
		"Optimization Tool\" or \"SolarPILOT\".Except to comply with the foregoing, the terms \"Solar Power "
		"tower Integrated Layout and Optimization Tool\", \"SolarPILOT\", or any confusingly similar "
		"designation may not be used to refer to any modified version of this software or any modified "
		"version of the underlying software originally provided by Alliance without the prior written consent "
		"of Alliance. "
		"\n\n"
		"5. The name of the copyright holder, contributors, the United States Government, the United States "
		"Department of Energy, or any of their employees may not be used to endorse or promote products "
		"derived from this software without specific prior written permission. "
		"\n\n"
		"THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\" AND ANY EXPRESS OR "
		"IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND "
		"FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER, "
		"CONTRIBUTORS, UNITED STATES GOVERNMENT OR UNITED STATES DEPARTMENT OF ENERGY, NOR ANY OF THEIR "
		"EMPLOYEES, BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL "
		"DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, "
		"DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER "
		"IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF "
		"THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
		;

    wxTextCtrl *tcmsg = new wxTextCtrl(main_panel, wxID_ANY, _lic_msg, wxDefaultPosition, wxSize(700,350), wxTE_MULTILINE|wxTE_READONLY|wxTE_RICH2);
    wxButton *okbut = new wxButton(main_panel, wxID_ANY, "Done");

    okbut->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( gui_license::OnOkButton ), NULL, this);

    wxBoxSizer *main_sizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *but_sizer = new wxBoxSizer(wxHORIZONTAL);

    but_sizer->Add(okbut, 0, wxALL, 5);

    main_sizer->Add(tcmsg, 1, wxEXPAND|wxALL, 5);
    main_sizer->Add(but_sizer, 0, wxALIGN_CENTER_HORIZONTAL, 0);

    wxBoxSizer *win_sizer = new wxBoxSizer(wxVERTICAL);
    win_sizer->Add(main_panel, 1, wxEXPAND, 0);
    main_panel->SetSizer(main_sizer);
    this->SetSizerAndFit(win_sizer);

}

void gui_license::OnOkButton( wxCommandEvent &WXUNUSED(event))
{
    this->Destroy();
}
