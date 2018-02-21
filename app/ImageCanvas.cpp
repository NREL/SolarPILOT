#include "ImageCanvas.h"
#include <wx/dc.h>

ImageCanvas::ImageCanvas(wxWindow *parent, wxWindowID id, std::string imagepath, wxBitmapType imagetype, const wxPoint pos, const wxSize size)
    : wxScrolledWindow( parent, id, pos, size, wxNO_BORDER)
{
    image_path = imagepath;
    image_size = size;
    image_type = imagetype;
    wxInitAllImageHandlers();
        
        
}


void ImageCanvas::OnPaint( wxPaintEvent &WXUNUSED(event) )
{
    wxPaintDC dc( this );
    PrepareDC( dc );
        
    //load the file
    wxBitmap bit;
    bit.LoadFile(image_path, image_type);


    dc.DrawBitmap( bit, 0, 0 );
        
}

BEGIN_EVENT_TABLE(ImageCanvas, wxScrolledWindow)
    EVT_PAINT(ImageCanvas::OnPaint)
END_EVENT_TABLE()
