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

