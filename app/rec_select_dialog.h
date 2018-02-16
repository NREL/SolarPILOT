#ifndef _RSDIALOG_
#define _RSDIALOG_ 1

#include "GUI_main.h"

class rec_select_dialog : public wxDialog
{
private:
    unordered_map<wxWindow*, Receiver*> rmap;
    std::vector<wxCheckBox*> checkboxes;
    int nrec;

public:

    rec_select_dialog();
    rec_select_dialog(
        wxWindow *parent, 
        wxWindowID winid, 
        wxString label,
        SolarField &SF,
        wxPoint pos = wxDefaultPosition,
        wxSize size = wxDefaultSize,
        long style=wxDEFAULT_DIALOG_STYLE|wxSTAY_ON_TOP|wxRESIZE_BORDER);

    void getSelectedReceivers(std::vector<Receiver*> &rec_ptrs);

    void OnOkButton(wxCommandEvent &WXUNUSED(event));

    void OnCancelButton(wxCommandEvent &WXUNUSED(event));

    void OnText( wxKeyEvent &event);

};






#endif
