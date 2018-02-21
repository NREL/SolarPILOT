#ifndef _LSDIALOG_
#define _LSDIALOG_ 1

#include <wx/wx.h>
#include "gui_util.h"

class layout_export_dialog : public wxDialog
{
private:
    std::vector<wxCheckBox*> checkboxes;
    wxComboBox *sep_combo;
    std::vector<bool> states;
    int nchecks;
    wxArrayStr sep_options;
    wxArrayStr sep_actuals;
    
public:

    layout_export_dialog(
        wxWindow *parent, 
        wxWindowID winid, 
        wxString label,
        wxPoint pos = wxDefaultPosition,
        wxSize size = wxDefaultSize,
        long style=wxDEFAULT_DIALOG_STYLE|wxSTAY_ON_TOP|wxRESIZE_BORDER);

    void getSelections(std::vector<bool> &States);

    wxString getDelimiter();

    void OnOkButton(wxCommandEvent &WXUNUSED(event));

    void OnCancelButton(wxCommandEvent &WXUNUSED(event));

    void OnText( wxKeyEvent &event);

};



#endif