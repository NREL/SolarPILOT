#ifndef _GUI_ABOUT_
#define _GUI_ABOUT_ 1

#include "GUI_main.h"
using namespace std;


/* 
Create an about dialog
*/


class gui_about : public wxFrame
{
    wxBitmap _bitmap;
    wxStaticBitmap *_sbit;
    string _version;
    wxPanel *_panel;

public:
    gui_about();
    gui_about(wxWindow *parent, wxWindowID id, string &bg_name, string &version, wxIcon icon );
    
protected:
    void OnPaint(wxPaintEvent &event);
private:
    DECLARE_EVENT_TABLE()

};


class gui_license : public wxFrame
{
    wxString _lic_msg;
    void OnOkButton(wxCommandEvent &event);
public:
    gui_license(wxWindow *parent, wxWindowID id, wxIcon icon);
};


#endif