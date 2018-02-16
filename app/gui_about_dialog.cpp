#include "gui_about_dialog.h"
#include <wx/clipbrd.h>

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

BEGIN_EVENT_TABLE(gui_about, wxFrame)
    EVT_PAINT( gui_about::OnPaint )
END_EVENT_TABLE()


//---------------------------------------------------------------------------------------

gui_license::gui_license(wxWindow *parent, wxWindowID id, wxIcon icon)
: wxFrame(parent, id, "SolarPILOT License", wxDefaultPosition, wxSize(400,350), wxDEFAULT_FRAME_STYLE|wxSTAY_ON_TOP)
{
    SetIcon( icon );

    wxPanel *main_panel = new wxPanel(this);
    _lic_msg = "Copyright © 2013 the Alliance for Sustainable Energy, LLC, All Rights Reserved\n\n"
        "This computer software is prepared by the Alliance for Sustainable Energy, LLC, (hereinafter the \"Contractor\"), "
        "under Contract DE-AC36-08G028308 (\"Contract\") with the Department of Energy (\"DOE\"). "
        "The United States Government has been granted for itself and others acting on its behalf a paid-up, "
        "non-exclusive, irrevocable, worldwide license in the Software to reproduce, prepare derivative works, "
        "and perform publicly and display publicly. Beginning five (5) years after the date permission to assert "
        "copyright is obtained from DOE, and subject to any subsequent five (5) year renewals, the United States "
        "Government is granted for itself and others acting on its behalf a paid-up, non-exclusive, irrevocable, "
        "worldwide license in the Software to reproduce, prepare derivative works, distribute copies to the public, "
        "perform publicly and display publicly, and to permit others to do so. If the Contractor ceases to make "
        "this computer software available, it may be obtained from DOE's Office of Scientific and Technical "
        "Information's Energy Science and Technology Software Center (ESTSC) at PO Box 1020, Oak Ridge, "
        "TN 37831-1020. THIS SOFTWARE IS PROVIDED BY THE CONTRACTOR \"AS IS\" AND ANY EXPRESS OR IMPLIED "
        "WARRANTIES, INCLUDING BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS "
        "FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE CONTRACTOR, DOE, OR THE U.S GOVERNMENT "
        "BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER, INCLUDING BUT "
        "NOT LIMITED TO CLAIMS ASSOCIATED WITH THE LOSS OF DATA OR PROFITS, WHICH MAY RESULT FROM AN ACTION IN "
        "CONTRACT, NEGLIGENCE OR OTHER TORTIOUS CLAIM THAT ARISES OUT OF OR IN CONNECTION WITH THE ACCESS, "
        "USE OR PERFORMANCE OF THIS SOFTWARE.\n\n"
        "The Software is being provided for internal purposes only and may not be re-distributed. "
        "Use of this software implies acceptance of this license agreement.";

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
