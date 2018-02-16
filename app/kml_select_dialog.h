#ifndef _KMLDIALOG_
#define _KMLDIALOG_ 1

#include <wx/wx.h>
#include <wx/dialog.h>
#include <vector>
using namespace std;

class kml_select_dialog : public wxDialog
{
    vector<wxTextCtrl*> latctrls, lonctrls;
    wxComboBox *inex_combo, *append_combo, *EW_combo, *NS_combo;
public:

    kml_select_dialog();
    kml_select_dialog(
        wxWindow *parent, 
        wxWindowID winid, 
        wxString label,
        wxPoint pos = wxDefaultPosition,
        wxSize size = wxDefaultSize,
        long style=wxDEFAULT_DIALOG_STYLE|wxSTAY_ON_TOP|wxRESIZE_BORDER);
    

    void getLocation(double &lat, double &lon);


    void setLocation(double &lat, double &lon);

    bool isAppend();

    bool isInclusion();

    void OnApply(wxCommandEvent &WXUNUSED(event));

    void OnCancel(wxCommandEvent &WXUNUSED(event));

    void OnText( wxKeyEvent &event);

    void OnTowerFile( wxCommandEvent &WXUNUSED(event) );
    
};


#endif