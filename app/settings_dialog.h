#ifndef _SETDLG_
#define _SETDLG_ 1

#include <wx/wx.h>
#include "gui_util.h"

class settings_dialog : public wxDialog
{
    std::vector<wxTextCtrl*> tcs;
    unordered_map<wxTextCtrl*, setting*> set_map;
public:
    settings_dialog( wxWindow *parent, wxWindowID winID, wxString label, gui_settings &gs, wxSize size);

protected:
    void OnOkButton(wxCommandEvent &event);

    void OnCancelButton(wxCommandEvent &event);

};


#endif
