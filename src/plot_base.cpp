#include "plot_base.h"
#include "gui_util.h"
#include <algorithm>
#include <wx/dcbuffer.h>
#include <wx/dcgraph.h>

PlotBase::PlotBase()
{
    Create();
}

int PlotBase::GetFontSize()
{
    return _hfont;
}
int PlotBase::GetResolutionMultiplier()
{
    return _resmult;
}
int PlotBase::GetColormap()
{
    return _color_map;
}
    
void PlotBase::Create()
{
    //Set default values
    _xaxmax = 1.;
    _xaxmin = 0.;
    _yaxmax = 1.;
    _yaxmin = 0.;
    _zaxmin = 0.;
    _zaxmax = 0.;
    _z_autoscale = true;
    _hfont = 9;
    _resmult = 5;
    _color_map = COLORMAP::PARULA;
    _xlab = "";
    _ylab = "";
    _units = "";
    _wrap_values = false;
    _x_reversed = false;
    //Define colors
    _gray.Set("#d4d4d4");
    _white.Set("#ffffff");
    _black.Set("#000000");
    _grad_high.Set("#FF0000");
    _grad_low.Set("#0040FF");    
}

void PlotBase::DrawScaledPolygon( wxDC &dc, double ppx, double ppy, double origin[2], double x[], double y[], int n_points)
{
    /*
    Take a polygon with n_points values (not closed) and indices of x[] and y[] of length n_points,
    and draw the polygon on the device. The X,Y coordinates will be scaled according to the maximum 
    extents w and h.

    ppx            Pixels per unit on the X axis
    ppy            Pixels per unit on the Y axis
    origin[] {x-pos, y-pos} Location in pixels of the plot origin

    Note that the DC coordinates are (0,0) at the upper left corner, positive X to the right, 
    and negative Y downward.
    
    */
    
    //Create an array of points of the items
    wxPoint *points;
    points = new wxPoint[n_points];

    for(int i=0; i<n_points; i++)
    {
        points[i].x = origin[0] - x[i]*ppx;
        points[i].y = origin[1] - y[i]*ppy;
    }

    dc.DrawPolygon(n_points, points);

    delete [] points;
}

void PlotBase::ColorGradientHotCold(wxColour &col, double index)
{
    /* 
    Index from 0-1 indicating the position on the scale from COLD to HOT

    Returns value indicating RRRGGGBBB on a scale of 0-255 for each integer triplet
    */
    double index_use = index < 1. ? (index < 0. ? 0. : index) : (index > 1. ? 1. : index);
    col.Set(int(index_use*255.), 0, int((1.- index_use)*255.));
}

void PlotBase::ColorGradientJet(wxColour &col, double index)
{
    double index_use = index < 1. ? (index < 0. ? 0. : index) : (index > 1. ? 1. : index);

    if(index_use > .75)
    {
        col.Set(255, 255-int((index_use -.75)/.25*255), 0);
        return;
    }
    if(index_use > 0.5)
    {
        col.Set(int((index_use -.5)/.25*255) , 255, 0);
        return;
    }
    if(index_use > 0.25)
    {
        col.Set(0, 255, 255 - int( (index_use -.25)/.25 * 255));
        return;
    }

    col.Set(0, int(index_use /.25*255), 255);
    return;    
}

void PlotBase::ColorGradientGrayscale(wxColour &col, double index)
{
    
    double index_use = index < 1. ? (index < 0. ? 0. : index) : (index > 1. ? 1. : index);
    col.Set(255.*index_use, 255.*index_use, 255.*index_use);
    return;
}

void PlotBase::ColorGradientParula(wxColour &col, double index)
{
    static double parula [] = {
       53.0655,   42.4065,  134.9460,
       53.6930,   46.2160,  142.7768,
       54.0971,   50.0550,  150.7135,
       54.1504,   53.9487,  158.7236,
       53.7378,   57.9306,  166.8357,
       52.6714,   62.0253,  175.0026,
       50.7125,   66.2258,  183.2500,
       47.5583,   70.5345,  191.5266,
       42.7195,   75.1292,  199.9092,
       35.7765,   80.2601,  208.2724,
       26.1383,   86.0610,  215.9549,
       15.0790,   91.7575,  221.4250,
        5.7004,   96.5384,  224.2261,
        1.8715,  100.3981,  225.1627,
        1.3971,  103.7556,  225.1812,
        2.7076,  106.7685,  224.6295,
        4.9030,  109.5658,  223.7780,
        7.5758,  112.2386,  222.6869,
       10.4457,  114.7964,  221.4605,
       13.0498,  117.2923,  220.1469,
       15.2351,  119.7495,  218.7436,
       17.0363,  122.1798,  217.3365,
       18.4280,  124.6100,  215.9085,
       19.4797,  127.0719,  214.5354,
       20.0870,  129.5933,  213.2426,
       20.2524,  132.2214,  212.1013,
       19.8467,  134.9885,  211.1500,
       18.7170,  137.9457,  210.5442,
       16.9351,  141.0930,  210.1841,
       14.6246,  144.3771,  209.9902,
       12.0708,  147.6543,  209.7792,
        9.6251,  150.8255,  209.3604,
        7.8563,  153.7712,  208.5807,
        6.7575,  156.4935,  207.4425,
        6.2506,  158.9855,  205.9535,
        6.0358,  161.2620,  204.1391,
        5.9044,  163.3669,  202.0805,
        5.8240,  165.3111,  199.7948,
        5.9005,  167.1455,  197.3600,
        6.4700,  168.8819,  194.7528,
        7.8277,  170.5625,  192.0335,
       10.2070,  172.1644,  189.1930,
       13.4269,  173.7106,  186.2682,
       17.2565,  175.2213,  183.2082,
       21.4965,  176.6725,  180.0725,
       26.1166,  178.0943,  176.8054,
       31.0544,  179.4736,  173.4456,
       36.2842,  180.8004,  169.9776,
       41.8130,  182.0978,  166.4292,
       47.6309,  183.3458,  162.7642,
       53.7501,  184.5481,  159.0002,
       60.1800,  185.6701,  155.1814,
       66.9298,  186.7373,  151.2807,
       73.9662,  187.7109,  147.3413,
       81.2731,  188.5748,  143.4259,
       88.7825,  189.3205,  139.5530,
       96.4186,  189.9225,  135.7983,
      104.0840,  190.3807,  132.2129,
      111.6521,  190.7137,  128.8237,
      119.0502,  190.9170,  125.5991,
      126.2482,  191.0205,  122.5646,
      133.2244,  191.0429,  119.6661,
      139.9540,  190.9772,  116.8850,
      146.4905,  190.8466,  114.2238,
      152.8586,  190.6496,  111.6838,
      159.0296,  190.4162,  109.1987,
      165.0615,  190.1280,  106.7940,
      170.9582,  189.7996,  104.4441,
      176.7413,  189.4418,  102.1453,
      182.3969,  189.0477,   99.8905,
      187.9721,  188.6204,   97.6573,
      193.4415,  188.1606,   95.4496,
      198.8560,  187.7009,   93.2674,
      204.1893,  187.2411,   91.0852,
      209.4694,  186.7813,   88.8775,
      214.6938,  186.3215,   86.6444,
      219.8826,  185.8811,   84.3725,
      225.0205,  185.4955,   82.0335,
      230.1352,  185.1903,   79.5739,
      235.2205,  185.0280,   76.9497,
      240.2510,  185.1122,   74.0543,
      245.1269,  185.6562,   70.7277,
      249.5252,  186.9699,   66.8208,
      252.7946,  189.2687,   62.5059,
      254.4320,  192.3164,   58.3579,
      254.7465,  195.6337,   54.6790,
      254.2304,  199.0090,   51.4536,
      253.2475,  202.3564,   48.4801,
      251.9400,  205.6830,   45.7385,
      250.4718,  209.0073,   43.1089,
      248.9611,  212.3570,   40.5473,
      247.4705,  215.7779,   37.9757,
      246.1909,  219.3340,   35.3484,
      245.2196,  223.0647,   32.5774,
      244.6060,  227.0172,   29.7214,
      244.4608,  231.2340,   26.7735,
      244.8510,  235.7313,   23.7451,
      245.7798,  240.5067,   20.6365,
      247.2380,  245.5163,   17.3640,
      248.9565,  250.6905,   13.7190
      };

    double index_use = index < 1. ? (index < 0. ? 0. : index) : (index > 1. ? 1. : index);
    int nc = sizeof(parula)/sizeof(double)/3;

    int hi = (int)(index_use*nc)+1;
    hi = hi < 1 ? 1 : hi;
    hi = hi > nc-1 ? nc-1 : hi;

    int lo = hi-1 < 0 ? 0 : hi-1;
    double f = index_use*nc - lo;

    double 
        rh = parula[hi*3],
        gh = parula[hi*3+1],
        bh = parula[hi*3+2],
        rl = parula[lo*3],
        gl = parula[lo*3+1],
        bl = parula[lo*3+2];

    int
        r = rl + (rh-rl)*f,
        g = gl + (gh-gl)*f,
        b = bl + (bh-bl)*f;

    col.Set(r,g,b);

    return;
}


double PlotBase::calcScale(double span, int segments)
{
    /* 
    Given a total scale span and a desired number of segments, return a 
    convenient scale segment length.
    */

    double
        f = span/double(segments),
        flog = log10(f),
        pflog = pow(10,floor(flog)),
        basis = f/pflog,
        tdiff = ceil(basis)-basis,
        bdiff = basis - floor(basis),
        mdiff = fabs(basis - (ceil(basis)+floor(basis))/2.);
    double diffmin = tdiff;
    int id=0;
    if(bdiff<diffmin) {diffmin = bdiff; id = 2;}
    if(mdiff<diffmin) {diffmin = mdiff; id = 1;}
    
    if(id==0)
    {
        //round up
        return ceil(basis)*pflog;
    }
    else if(id==1)
    {
        //round to the middle
        return (floor(basis)+.5)*pflog;
    }
    else
    {
        //round down
        return floor(basis)*pflog;
    }
}

double PlotBase::bilinearInterp(double xf, double yf, double *Z)
{
    /*
    Take two fractional distances "xf" and "yf" that represent the fractional distance between the 
    bounds of the square (with lower left -> 0,0 and upper right -> 1,1), and take the array of
    corner point magnitudes "Z" of size 4 and calculate the bilinearly interpolated z value. 

    Z is:
    {f(0,0), f(1,0), f(0,1), f(1,1)}
    */

    return ( Z[0] + (Z[1] - Z[0])*xf + (Z[2] - Z[0])*yf + (Z[0] - Z[1] - Z[2] + Z[3])*yf*xf );
}
    
void PlotBase::SetFontSize(int hfont)
{_hfont = hfont;}
    
void PlotBase::SetXAxisRange(double xmin, double xmax)
{
    _xaxmin = xmin;
    _xaxmax = xmax;
}

void PlotBase::SetYAxisRange(double ymin, double ymax)
{
    _yaxmin = ymin;
    _yaxmax = ymax;
}

void PlotBase::SetZRange(double zmin, double zmax, bool is_autoscale)
{
    _z_autoscale = is_autoscale;
    _zaxmin = zmin;
    _zaxmax = zmax;
}

void PlotBase::SetXLabel(wxString &xlab)
{
    _xlab = xlab;
}

void PlotBase::SetXLabel(const char *xlab)
{
    _xlab = (wxString)xlab;
}

void PlotBase::SetYLabel(wxString &ylab)
{
    _ylab = ylab;
}

void PlotBase::SetYLabel(const char *ylab)
{
    _ylab = (wxString)ylab;
}

void PlotBase::SetUnits(wxString &units)
{
    _units = units;
}

void PlotBase::SetUnits(const char *units)
{
    _units = (wxString)units;
}

void PlotBase::SetPlotSize(wxSize &psize)
{
    _plotsize = psize;
}

void PlotBase::SetResolutionMultiplier(int resmult)
{
    _resmult = resmult;
}

void PlotBase::SetColormap(int cmap)
{
    _color_map = cmap;
}

void PlotBase::SetDataWrapping(bool do_wrap)
{
    _wrap_values = do_wrap;
}

void PlotBase::SetXAxisReversed(bool do_reversed)
{
    _x_reversed = do_reversed;
}

wxBitmap *PlotBase::GetBitmap()
{
    return &_bitmap;
}

void PlotBase::AxesSetup(wxMemoryDC &dc, double minval, double maxval)
{
    //Draw the bounding box
    dc.SetPen( *wxWHITE_PEN );
    dc.SetBrush( *wxWHITE_BRUSH );
    wxRect windowRect(wxPoint(0,0), _plotsize);
    dc.DrawRectangle( windowRect );
    
    //The pixel size of the drawing canvas
    double canvsize[2];
    canvsize[0] = _plotsize.GetWidth();
    canvsize[1] = _plotsize.GetHeight();
    
    double plot_scale = 0.95;
        
    //The total span of each axis in their respective units
    double
        xaxspan = _xaxmax - _xaxmin,
        yaxspan = _yaxmax - _yaxmin;

    //Set buffer areas for the axes
    dc.SetFont( wxFont(_hfont, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL) );
    
    //determine the best number formatting for the x axis labels
    wxString xfmt;
    int nxdec = gui_util::CalcBestSigFigs( max(fabs(_xaxmax),fabs(_xaxmin)) );
    xfmt.sprintf("%s%df", "%.", max(nxdec,0));
    
    //determine the best number formatting for the y axis labels
    wxString yfmt;
    int nydec = gui_util::CalcBestSigFigs( max(fabs(_yaxmax),fabs(_yaxmin)) );
    yfmt.sprintf("%s%df", "%.", nydec);
    
    string ets = to_string(-yaxspan, yfmt.c_str());    //create a string that is approximately the largest extent on the plot
    wxSize etss = dc.GetTextExtent( ets );
    _left_buffer = 15+etss.GetWidth()+etss.GetHeight();
    _top_buffer = 0;
    _bottom_buffer = etss.GetHeight()*2+15;
    int nzdec = gui_util::CalcBestSigFigs( max(fabs(maxval),fabs(minval)) );
    _zfmt.sprintf("%s%df %s", "%.", nzdec, _units.c_str());
    ets = to_string(max(fabs(maxval),fabs(minval)), _zfmt.c_str());
    etss = dc.GetTextExtent( ets);
    _right_buffer = 40+etss.GetWidth();

    //Plot area (excluding area for axes) in pixels
    _drawsize[0] = canvsize[0] - (_left_buffer+_right_buffer);
    _drawsize[1] = canvsize[1] - (_top_buffer+_bottom_buffer);
    
    //Determine the pixels per unit in each axis
    _ppx = _drawsize[0]/xaxspan*plot_scale;
    _ppy = _drawsize[1]/yaxspan*plot_scale;

    //Origin location - relative to the canvas
    _origin[0] = _left_buffer + _drawsize[0]/2. - (xaxspan/2. + _xaxmin)*_ppx;
    _origin[1] = _top_buffer + _drawsize[1]/2. + (yaxspan/2. + _yaxmin)*_ppy;

    //Draw the plot boundary
    dc.SetPen( wxPen( _black, 1, wxPENSTYLE_SOLID) );
    dc.SetBrush( *wxBLACK_BRUSH );
    dc.DrawRectangle( _left_buffer, _top_buffer, _drawsize[0], _drawsize[1]);
    
    dc.SetBrush( *wxWHITE_BRUSH);
    //Draw the x axis
    dc.SetPen( wxPen( _black, 1, wxPENSTYLE_SOLID) );
    dc.SetBrush( *wxWHITE_BRUSH );
    dc.DrawLine(_origin[0], _drawsize[1], _origin[0], _drawsize[1]+10);
    ets = "0";
    etss = dc.GetTextExtent( ets );
    dc.DrawText(ets, _origin[0]-etss.GetWidth()/2, _drawsize[1]+9);
    
    ets = (string)_xlab;
    
    etss = dc.GetTextExtent( ets );
    dc.DrawText(ets, _left_buffer+_drawsize[0]/2-etss.GetWidth()/2, _drawsize[1]+(15+etss.GetHeight()));
    //estimate the number of divisions
    ets = to_string( -xaxspan/2., xfmt.c_str());    //approximation of the xaxis text size
    etss = dc.GetTextExtent( ets );
    int ndiv = min(int(_drawsize[0]/ (etss.GetWidth()*1.35 )), 20);
    double xscale = calcScale(xaxspan, ndiv);
    double 
        xtickloc = xscale,
        xtickloc_ppm = xscale*_ppx;
    wxString xts;
    double xx = fmax(fabs(_xaxmin), fabs(_xaxmax));
    while(xtickloc < xx)
    {
        //To the right
        wxSize xte;
        if(xtickloc <= _xaxmax)
        {
            dc.DrawLine(_origin[0]+xtickloc_ppm, _drawsize[1], _origin[0]+xtickloc_ppm, _drawsize[1]+7);
            xts.Printf(xfmt, (_x_reversed ? -1. : 1)*(xtickloc+1.e-6));
            xte = dc.GetTextExtent(xts);
            dc.DrawText( xts , _origin[0]+xtickloc_ppm-xte.GetWidth()/2, _drawsize[1]+9);
        }
        //To the left
        if(-xtickloc >= _xaxmin)
        {
            dc.DrawLine(_origin[0]-xtickloc_ppm, _drawsize[1], _origin[0]-xtickloc_ppm, _drawsize[1]+7);
            xts.Printf(xfmt, (_x_reversed ? 1. : -1)*(xtickloc+1.e-6));
            xte = dc.GetTextExtent( xts );
            dc.DrawText( xts , _origin[0]-xtickloc_ppm-xte.GetWidth()/2, _drawsize[1]+9);
        }
        xtickloc += xscale;
        xtickloc_ppm += xscale*_ppx;
    }
    
    //Draw the y axis
    dc.DrawLine(_left_buffer-10, _origin[1], _left_buffer, _origin[1]);
    string etsy = "0"; wxSize etssy = dc.GetTextExtent( etsy );
    dc.DrawText(etsy, _left_buffer-(etssy.GetWidth()+8), _origin[1]-etssy.GetHeight()/2);
    //Label
    etsy = (string)_ylab;
    etssy = dc.GetTextExtent(etsy);
    dc.DrawRotatedText(etsy, 2, (_drawsize[1]-etssy.GetWidth())/2., 270.);
    //estimate the number of divisions
    etsy = my_to_string( int(-yaxspan/2.));
    etssy = dc.GetTextExtent( etsy );
    int ndivy = min(int(_drawsize[0]/ (etssy.GetHeight() )), 20);
    double yscale = calcScale(yaxspan, ndivy);
        
    double
        ytickloc = yscale,
        ytickloc_ppm = yscale*_ppy;
    wxString yts;
    while(ytickloc < fmax(fabs(_yaxmax), fabs(_yaxmin)) )
    {
        //To the top
        if(ytickloc < fabs(_yaxmax))
        {
            dc.DrawLine(_left_buffer-7, _origin[1]-ytickloc_ppm, _left_buffer, _origin[1]-ytickloc_ppm);
            yts.Printf(yfmt, ytickloc);
            wxSize yte = dc.GetTextExtent(yts);
            dc.DrawText( yts , _left_buffer-10-yte.GetWidth(), _origin[1]-ytickloc_ppm-yte.GetHeight()/2);
        }
        //To the bottom
        if(ytickloc < fabs(_yaxmin))
        {
            dc.DrawLine(_left_buffer-7, _origin[1]+ytickloc_ppm, _left_buffer, _origin[1]+ytickloc_ppm);
            yts.Printf(yfmt, -ytickloc);
            wxSize yte = dc.GetTextExtent(yts);
            dc.DrawText( yts , _left_buffer-10-yte.GetWidth(), _origin[1]+ytickloc_ppm-yte.GetHeight()/2);
        }
        
        ytickloc += yscale;
        ytickloc_ppm += yscale*_ppy;
    }
    
}

void PlotBase::DrawText(wxMemoryDC &dc, std::string message, double x, double y)
{
    
    dc.SelectObject( _bitmap );

    wxPen oldpen = dc.GetPen();
    dc.SetTextForeground(wxColour("orange"));
    dc.SetFont(wxFont(15,wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL) );
    double dx,dy;
    dx = _plotsize.GetWidth();
    dy = _plotsize.GetHeight();

    dc.DrawText(message,(int)(x*dx), (int)(y*dy)); 
    dc.SetPen(oldpen);

}

void PlotBase::DrawColorbar(wxMemoryDC &dc, double minval, double maxval, double aveval)
{
    //Draw the gradient bar, in 4 sections
    double canvsize[2];
    canvsize[0] = _plotsize.GetWidth();
    canvsize[1] = _plotsize.GetHeight();
    int
        gtop = 50,
        gbot = _drawsize[1]-50,
        gvspan = -gtop + gbot,
        gleft = canvsize[0]-_right_buffer+5,
        gright = canvsize[0]-_right_buffer+25,
        ghspan = gright-gleft;
    wxColour col1, col2, col3, col4, col5;

    switch (_color_map)
    {
    case COLORMAP::JET:
        ColorGradientJet(col1, 1.);
        ColorGradientJet(col2, 0.75);
        ColorGradientJet(col3, 0.5);
        ColorGradientJet(col4, 0.25);
        ColorGradientJet(col5, 0.);
        break;
    case COLORMAP::GRAYSCALE:
        ColorGradientGrayscale(col1, 1.);
        ColorGradientGrayscale(col2, 0.75);
        ColorGradientGrayscale(col3, 0.5);
        ColorGradientGrayscale(col4, 0.25);
        ColorGradientGrayscale(col5, 0.);
        break;
    case COLORMAP::HOTCOLD:
        ColorGradientHotCold(col1, 1.);
        ColorGradientHotCold(col2, 0.75);
        ColorGradientHotCold(col3, 0.5);
        ColorGradientHotCold(col4, 0.25);
        ColorGradientHotCold(col5, 0.);
        break;
    case COLORMAP::PARULA:
        ColorGradientParula(col1, 1.);
        ColorGradientParula(col2, 0.75);
        ColorGradientParula(col3, 0.5);
        ColorGradientParula(col4, 0.25);
        ColorGradientParula(col5, 0.);
        break;
    default:
        break;
    }


    dc.GradientFillLinear( wxRect(wxPoint(gleft, gtop+gvspan*0.00), wxPoint(gright, gtop+gvspan*0.25)), col1, col2, wxSOUTH);
    dc.GradientFillLinear( wxRect(wxPoint(gleft, gtop+gvspan*0.25), wxPoint(gright, gtop+gvspan*0.50)), col2, col3, wxSOUTH);
    dc.GradientFillLinear( wxRect(wxPoint(gleft, gtop+gvspan*0.50), wxPoint(gright, gtop+gvspan*0.75)), col3, col4, wxSOUTH);
    dc.GradientFillLinear( wxRect(wxPoint(gleft, gtop+gvspan*0.75), wxPoint(gright, gtop+gvspan*1.00)), col4, col5, wxSOUTH);

    dc.SetPen(*wxBLACK_PEN);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.DrawRectangle(gleft, gtop, ghspan+2, gvspan+2);

    double lineave = 50+(_drawsize[1]-100.)*(1.-(aveval-minval)/(maxval-minval));    //where should the average line indicator be in Y?
    //Draw the average line
    dc.SetPen( wxPen( _white, 1, wxPENSTYLE_SOLID) );
    dc.DrawLine(canvsize[0]-_right_buffer+5, lineave, canvsize[0]-_right_buffer+25, lineave);
    //Label the gradient bar
    wxString tlab, blab, alab;
    tlab.sprintf(_zfmt, maxval);
    blab.sprintf(_zfmt, minval);
    alab.sprintf(_zfmt, aveval);
        
    dc.DrawText( tlab , canvsize[0]-_right_buffer+5, gtop - dc.GetTextExtent( tlab).GetHeight()-2);
    dc.DrawText( blab , canvsize[0]-_right_buffer+5, gbot + 2);
    dc.DrawText( alab , canvsize[0]-_right_buffer+28, lineave-dc.GetTextExtent( alab).GetHeight()/2);
    

}
