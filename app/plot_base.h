#ifndef _PLOT_BASE_
#define _PLOT_BASE_ 1

#include <wx/wx.h>
#include <wx/dc.h>

struct COLORMAP { enum A {JET, GRAYSCALE, HOTCOLD, PARULA}; };

class PlotBase
{

public:
    wxBitmap _bitmap;
    
    double
        _xaxmax,    //maximum x axis value in axis units
        _xaxmin,    //minimum x axis value in axis units
        _yaxmax,    //maximum y axis value in axis units
        _yaxmin,    //minimum y axis value in axis units
        _zaxmin,    //minimum z axis value in axis units
        _zaxmax;    //maximum z axis value in axis units

    double 
        _ppx,    //Pixels per x axis unit 
        _ppy;    //Pixels per y axis unit
    double 
        _origin[2],        //location of the origin in pixels relative to drawctrl origin
        _drawsize[2];    //Size of the chartable area in pixels
    int _hfont,        //Font size
        _resmult,    
        _right_buffer,    //Buffer between the right side of chart area and device boundary
        _left_buffer,    //Buffer between the left side of chart area and device boundary
        _top_buffer,    //Buffer between the top side of chart area and device boundary
        _bottom_buffer,    //Buffer between the bottom side of chart area and device boundary
        _color_map;    //int member of enum COLORMAP
    
    wxString 
        _xlab,
        _ylab,
        _units,
        _zfmt;  //Format for number being plotted
    wxSize _plotsize;
    bool _wrap_values, _x_reversed, _z_autoscale;
    //Define colors
    wxColour
        _gray,
        _white,
        _black,
        _grad_high,
        _grad_low;    

    void DrawScaledPolygon( wxDC &dc, double ppx, double ppy, double origin[2], double x[], double y[], int n_points);
    //colormaps
    void ColorGradientHotCold(wxColour &col, double index);
    void ColorGradientJet(wxColour &col, double index);
    void ColorGradientGrayscale(wxColour &col, double index);
    void ColorGradientParula(wxColour &col, double index);
    //---
    double calcScale(double span, int segments);
    double bilinearInterp(double xf, double yf, double *Z);
    //---------------------------
    PlotBase();
    void Create();
    void SetFontSize(int hfont);
    void SetXAxisRange(double xmin, double xmax);
    void SetYAxisRange(double ymin, double ymax);
    void SetZRange(double zmin, double zmax, bool is_autoscale);
    void SetXLabel(wxString &xlab);
    void SetYLabel(wxString &ylab);
    void SetUnits(wxString &units);
    void SetXLabel(const char *xlab);
    void SetYLabel(const char *ylab);
    void SetUnits(const char *units);
    void SetPlotSize(wxSize &psize);
    void SetColormap(int cmap);    
    void SetResolutionMultiplier(int resmult);
    void SetGradientType(int gmap);
    void SetDataWrapping(bool do_wrap);
    void SetXAxisReversed(bool do_reverse);

    void AxesSetup(wxMemoryDC &dc, double minval=0., double maxval=0.);
    void DrawColorbar(wxMemoryDC &dc, double minval, double maxval, double aveval);
    void DrawText(wxMemoryDC &dc, std::string message, double x, double y);

    int GetFontSize();
    int GetResolutionMultiplier();
    int GetColormap();
    wxBitmap *GetBitmap();

};



#endif