
/*******************************************************************************************************
*  Copyright 2018 Alliance for Sustainable Energy, LLC
*
*  NOTICE: This software was developed at least in part by Alliance for Sustainable Energy, LLC
*  ("Alliance") under Contract No. DE-AC36-08GO28308 with the U.S. Department of Energy and the U.S.
*  The Government retains for itself and others acting on its behalf a nonexclusive, paid-up,
*  irrevocable worldwide license in the software to reproduce, prepare derivative works, distribute
*  copies to the public, perform publicly and display publicly, and to permit others to do so.
*
*  Redistribution and use in source and binary forms, with or without modification, are permitted
*  provided that the following conditions are met:
*
*  1. Redistributions of source code must retain the above copyright notice, the above government
*  rights notice, this list of conditions and the following disclaimer.
*
*  2. Redistributions in binary form must reproduce the above copyright notice, the above government
*  rights notice, this list of conditions and the following disclaimer in the documentation and/or
*  other materials provided with the distribution.
*
*  3. The entire corresponding source code of any redistribution, with or without modification, by a
*  research entity, including but not limited to any contracting manager/operator of a United States
*  National Laboratory, any institution of higher learning, and any non-profit organization, must be
*  made publicly available under this license for as long as the redistribution is made available by
*  the research entity.
*
*  4. Redistribution of this software, without modification, must refer to the software by the same
*  designation. Redistribution of a modified version of this software (i) may not refer to the modified
*  version by the same designation, or by any confusingly similar designation, and (ii) must refer to
*  the underlying software originally provided by Alliance as "Solar Power tower Integrated Layout and 
*  Optimization Tool" or "SolarPILOT". Except to comply with the foregoing, the terms "Solar Power 
*  tower Integrated Layout and Optimization Tool", "SolarPILOT", or any confusingly similar
*  designation may not be used to refer to any modified version of this software or any modified
*  version of the underlying software originally provided by Alliance without the prior written consent
*  of Alliance.
*
*  5. The name of the copyright holder, contributors, the United States Government, the United States
*  Department of Energy, or any of their employees may not be used to endorse or promote products
*  derived from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
*  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
*  FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER,
*  CONTRIBUTORS, UNITED STATES GOVERNMENT OR UNITED STATES DEPARTMENT OF ENERGY, NOR ANY OF THEIR
*  EMPLOYEES, BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
*  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
*  IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
*  THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************************************/


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
    static void ColorGradientHotCold(wxColour &col, double index);
    static void ColorGradientJet(wxColour &col, double index);
    static void ColorGradientGrayscale(wxColour &col, double index);
    static void ColorGradientLookup(wxColour &col, double index, double* table, int nrow);
    static void ColorGradientParula(wxColour &col, double index);
    static void ColorGradientRainbow(wxColour &col, double index);
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