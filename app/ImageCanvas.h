#ifndef _IMAGE_CANVAS_
#define _IMAGE_CANVAS_ 1

//#include "GUI_main.h"
#include <wx/wx.h>
#include <wx/bitmap.h>
#include <wx/dc.h>

/* 
This class creates a simple canvas for displaying an image
*/


class ImageCanvas : public wxScrolledWindow
{
private:
    std::string image_path;
    wxSize image_size;
    wxBitmapType image_type;
    DECLARE_EVENT_TABLE()

public:
    ImageCanvas(wxWindow *parent, wxWindowID id, std::string imagepath, wxBitmapType imagetype, const wxPoint pos, const wxSize size);
    
    void OnPaint( wxPaintEvent &event );

};


#endif