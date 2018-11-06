
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


#ifndef _FIELDPLOT_
#define _FIELDPLOT_ 1

#include <wx/wx.h>
//#include <wx/dc.h>
//#include <wx/dcgraph.h>
//#include <wx/dcbuffer.h>
#include "treemesh.h"

class SolarField;
class Heliostat;

    /* 
    Options for values to plot are:
    0 | Land boundaries
    1 | Field layout
    2 | Total efficiency
    3 | Cosine efficiency
    4 | Attenuation efficiency
    5 | Blocking efficiency
    6 | Shading efficiency
    7 | Image intercept efficiency
    8 | Delivered power
    9 | Layout ranking metric
    10| Receiver map
    11| Mesh zones
    12| Cloudiness efficiency
    --------------------------------------------
    */
enum FIELD_PLOT
{
    LAND=0,
    LAYOUT,
    EFF_TOT,
    EFF_COS,
    EFF_ATT,
    EFF_BLOCK,
    EFF_SHAD,
    EFF_INT,
    EFF_REFLECT,
    POWER,
    RANK,
    EFF_ANNUAL,
    ENERGY,
    RECEIVER,
    MESH,
    EFF_CLOUD,
    _END
};


class FieldPlot : public wxScrolledWindow
{
private:
    bool _is_data_ready;
    bool _is_data_visible;
    SolarField *_SF;    //Pointer to the solar field object
    int _option;    //Plot option
    int _fontsize;    //Vertical font size in pixels
    int _ppi;    //pixels per inch
    int _ppiold;
    bool _reset_ppi;
    wxBitmap _pbit; //bitmap containing the current plot
    double _zoom_fact;  //zoom factor
    int _origin_offset[2];  //offset in pixels for origin
    int _origin_pixels[2]; //location of the origin in pixels. Set by DoPaint
    double _meters_per_pixel; //real-life distance covered by single pixel
    int _zoom_rectangle[4]; //x0,y0,x1,y1
    bool _is_zoom_rectangle;
    bool _ctrl_down;
    std::vector< std::string > _plot_choices;
    st_hash_tree _helio_hash;
    std::string _helios_annot; //annotation for selected heliostats
    std::vector< Heliostat *> _helios_select;   //currently selected heliostats

    DECLARE_EVENT_TABLE()


public:
    FieldPlot( wxPanel *parent, 
               SolarField &SF,
               const int plot_option = 0,
               const wxWindowID id = wxID_ANY, 
               const wxPoint pos = wxDefaultPosition, 
               const wxSize size = wxDefaultSize,
               long style = wxHSCROLL | wxVSCROLL| wxFULL_REPAINT_ON_RESIZE | wxBG_STYLE_CUSTOM);
    void SetPlotZoomFact(double fact);
    void SetPlotData(SolarField &SF, int plot_option);
    void DrawScaledPolygon( wxDC &dc, double ppm, double origin[2], double x[], double y[], int n_points);
    void DrawScaledEllipse( wxDC &dc, double ppm, double origin[2], double x, double y, double diam);
    void SetPlotOption(int option);
    void SetFontSize(int hpixel);
    void SetOriginOffset(int xy[2]);
    void AddToOriginOffset(int xy[2]);
    void SetDataVisible(bool is_visible);
    void SetZoomRectangle(int xy0_xy1[4]);
    void EnableZoomRectangle(bool is_enabled=true);
    st_hash_tree *GetKDHashTree();
    void HeliostatAnnotation(Heliostat *H);
    void ClearSelectedHeliostats();
    std::vector< Heliostat* > *GetSelectedHeliostats();

    std::vector< std::string > GetPlotChoices();
    bool IsDataReady();

    double GetPlotZoomFact()
    {
        return _zoom_fact;
    }
    
    int GetFontSize()
    {
        return _fontsize;
    }

    int *GetOriginOffset()
    {
        return _origin_offset;
    };
        
    int *GetOriginPixels()
    {
        return _origin_pixels;
    };

    double GetMetersPerPixel()
    {
        return _meters_per_pixel;
    };

    void SetPPI(int ppi);
    int GetPPI();
    void ResetPPIOnPaintEvent(int oldppi = -1);
    wxBitmap *GetBitmap();
    void DoPaint(wxDC &_pdc);
    
protected:
    void OnPaint(wxPaintEvent &event);
    void OnEraseBackground( wxEraseEvent &WXUNUSED(event)) {}
    void ColorGradientHotCold(wxColour &col, double index);
    double calcScale(double span, int segments);
};




#endif