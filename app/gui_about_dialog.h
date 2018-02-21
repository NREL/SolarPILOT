#ifndef _GUI_ABOUT_
#define _GUI_ABOUT_ 1

#include <wx/wx.h>

/* 
Create an about dialog
*/


class gui_about : public wxFrame
{
    wxBitmap _bitmap;
    wxStaticBitmap *_sbit;
    std::string _version;
    wxPanel *_panel;

public:
    gui_about();
    gui_about(wxWindow *parent, wxWindowID id, std::string &bg_name, std::string &version, wxIcon icon );
    
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