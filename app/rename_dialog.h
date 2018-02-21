#ifndef _HDIALOG_
#define _HDIALOG_ 1

#include <wx/wx.h>

/* 
This control is a simple popup window for naming/renaming a heliostat template item.
*/

class rename_dlg : public wxDialog
{
private:
    wxString _name;
    int _retcode;
    wxTextCtrl *_dlg_text;

public:
    rename_dlg();
	rename_dlg(
		wxWindow *parent,
		wxWindowID winid,
		wxString label,
		wxString defvalue,
		wxPoint pos,
		wxSize size,
		long style = wxCLOSE_BOX | wxSTAY_ON_TOP);
    
	wxString getTextValue();
	int getRetCode();
    
protected:
	void OnButtonApply(wxCommandEvent &WXUNUSED(event));
	void OnButtonCancel(wxCommandEvent &WXUNUSED(event));
	void OnText(wxKeyEvent &event);
};

#endif