
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


#include <algorithm>
#include <wx/dc.h>
#include <wx/dcgraph.h>
#include <wx/dcbuffer.h>
#include <sstream>
#include <iomanip>

#include "FieldPlot.h"
#include "gui_util.h"
#include "OpticalMesh.h"
#include "SolarField.h"
#include "plot_select_dialog.h"

#include "plot_base.h"      //include this for the color maps. We don't directly use this class here, yet (but should)

using namespace std;

void FieldPlot::SetPlotOption(int option)
{
    _option = option;
}

void FieldPlot::SetFontSize(int hpixel)
{
    _fontsize = hpixel;
}

int FieldPlot::GetPPI()
{
    return _ppi;
}

wxBitmap *FieldPlot::GetBitmap()
{
    return &_pbit;
}

FieldPlot::FieldPlot(wxPanel *parent, SolarField &SF, const int plot_option,
                     const wxWindowID id, const wxPoint pos, const wxSize size,
                     long style)
        : wxScrolledWindow(parent, id, pos, size, style)
{
    _SF = &SF;    //Point to the solar field object
    _option = plot_option;
    _is_data_ready = false;    //on initialization, the data for the solar field hasn't been prepared
    _fontsize = 8;
    _ppi = 96;
    _ppiold = 96;
    _zoom_fact = 0.95;
    SetBackgroundStyle(wxBG_STYLE_CUSTOM);
    _origin_offset[0] = 0;
    _origin_offset[1] = 0;
    _is_data_visible = false;
    _is_zoom_rectangle = false;
    _meters_per_pixel = 0.;
    _origin_pixels[0] = -999;
    _origin_pixels[0] = -999;
    _ctrl_down = false;
    _helios_annot.clear();
    _solarfield_annot.clear();

    _plot_choices.clear();
    /*
    0 | Land boundaries
    1 | Field layout
    2 | Total efficiency
    3 | Cosine efficiency
    4 | Attenuation efficiency
    5 | Blocking efficiency
    6 | Shading efficiency
    7 | Image intercept efficiency
    8 | Heliostat reflectivity
    9 | Delivered power
    10| Ranking metric
    11| receiver map
    12| Optical mesh
    13| Cloudiness efficiency
    */
    _plot_choices = {
        "Land boundaries",
        "Field layout",
        "Total efficiency",
        "Cosine efficiency",
        "Attenuation efficiency",
        "Blocking efficiency",
        "Shading efficiency",
        "Image intercept efficiency",
        "Reflectivity",
        "Delivered power",
        "Ranking metric",
        "Annual efficiency",
        "Annual energy",
        "Receiver map",
        "Optical mesh",
        "Cloudiness efficiency",
    };

}

bool FieldPlot::IsDataReady()
{
    return _is_data_ready;
}

void FieldPlot::SetPlotZoomFact(double fact)
{
    _origin_offset[0] *= fact/_zoom_fact;
    _origin_offset[1] *= fact/_zoom_fact;
    _zoom_fact = fact;
}

void FieldPlot::SetOriginOffset(int xy[2])
{
    _origin_offset[0] = xy[0];
    _origin_offset[1] = xy[1];
}

void FieldPlot::AddToOriginOffset(int xy[2])
{
    _origin_offset[0] += xy[0];
    _origin_offset[1] += xy[1];
}

void FieldPlot::SetDataVisible(bool is_visible)
{
    _is_data_visible = is_visible;
}

void FieldPlot::SetZoomRectangle(int xyxy[4])
{
    for(int i=0; i<4; i++)
        _zoom_rectangle[i] = xyxy[i];
}

void FieldPlot::EnableZoomRectangle(bool is_enabled)
{
    _is_zoom_rectangle = is_enabled;
}

std::vector< std::string > FieldPlot::GetPlotChoices()
{
    return _plot_choices;
}

st_hash_tree *FieldPlot::GetKDHashTree()
{
    return &_helio_hash;
}

void FieldPlot::SetPlotData(SolarField &SF, int plot_option)
{
    //assign
    _SF = &SF;
    _option = plot_option;
    _is_data_ready = plot_option != 0;
    _helios_select.clear();
    _helios_annot.clear();
    _solarfield_annot.clear();

    //update the calculation of the heliostat KD tree for quick mouse interaction
	Hvector *helios = _SF->getHeliostats();

    double extents[2];
	_SF->getLandObject()->getExtents(*_SF->getVarMap(), extents);

	KDLayoutData ld;
	ld.xlim[0] = -extents[1];
	ld.xlim[1] = extents[1];
	ld.ylim[0] = -extents[1];
	ld.ylim[1] = extents[1];
	ld.min_unit_dx = ld.min_unit_dy = _SF->getHeliostatTemplates()->at(0)->getCollisionRadius()*2.;

    _helio_hash.reset();        //make sure the heliostat hash tree is cleaned out (Was causing crashes without this)
	_helio_hash.create_mesh(ld);
	for(size_t i=0; i<helios->size(); i++)
	{
		sp_point *loc = helios->at(i)->getLocation();
		_helio_hash.add_object((void*)helios->at(i), loc->x, loc->y );
	}

	_helio_hash.add_neighborhood_data();

    return;
}

void FieldPlot::SetPPI(int ppi)
{
    _fontsize = (int)(_fontsize * ((float)ppi/(float)_ppi));
    _ppi = ppi;
}

void FieldPlot::ResetPPIOnPaintEvent(int oldppi)
{
    _reset_ppi = true;
    if( oldppi > 0)
    {
        _ppiold = oldppi;
    }
}

void FieldPlot::OnPaint(wxPaintEvent &event)
{
    //Use a memory DC with a bitmap, and catch the wxEraseBackground event to prevent flicker

    wxPaintDC _pdc(this);
    DoPaint(_pdc);
    event.Skip();
}

void FieldPlot::DoPaint(wxDC &_pdc)
{

    //set up the canvas
    wxMemoryDC _dc;

    //scaling multipliers
    double ppimult = (double)_ppi/(double)_ppiold;
    int ssize_w = (int)this->GetSize().GetWidth()*ppimult;
    int ssize_h = (int)this->GetSize().GetHeight()*ppimult;

    //validation
    ssize_w = ssize_w < 100 ? 1024 : ssize_w;
    ssize_h = ssize_h < 100 ? 711 : ssize_h;

    //assign a bitmap to the DC
    _pbit.Create(ssize_w, ssize_h);
    _dc.SelectObject( _pbit );

    wxGCDC gdc(_dc);

    //Draw the bounding box
    _dc.SetPen( *wxWHITE_PEN );
    _dc.SetBrush( *wxWHITE_BRUSH );
    wxRect windowRect(wxPoint(0,0), wxSize(ssize_w, ssize_h)); //GetClientSize());
    _dc.DrawRectangle( windowRect );

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
    --------------------------------------------
    */

    //Define colors
    wxColour
        gray("#d4d4d4"),
        white("#ffffff"),
        black("#000000"),
        shadow("#D4D4D4"),
        helio_border("#96B9D6"),
        grad_high("#FF0000"),
        grad_low("#0040FF"),
        helio_region("#6B9CB5");

    //The pixel size of the drawing canvas
    double canvsize[2];
    canvsize[0] = ssize_w;
    canvsize[1] = ssize_h;
    double plot_scale = _zoom_fact;

    var_map *V = _SF->getVarMap();

    if(_option == FIELD_PLOT::LAND || !_is_data_ready)
    {    //Land boundaries

        //Create polygons of the land inclusions and exclusions

        double pbounds[2];
        Land::getExtents( *V, pbounds );
        double fieldsize[] = {pbounds[1]*2., pbounds[1]*2.};

        //we need 40pix on each side for x label, y label, gradient bar
        _dc.SetFont( wxFont(_fontsize, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL) );
        string ets = my_to_string(-int(pbounds[1]));    //create a string that is approximately the largest extent on the plot
        wxSize etss = _dc.GetTextExtent( ets );
        int
            left_buffer = 15*ppimult+etss.GetWidth()+etss.GetHeight(),
            right_buffer = 40*ppimult+etss.GetWidth(),
            top_buffer = 0,
            bottom_buffer = etss.GetHeight()*2+15*ppimult;

        //Origin location - relative to the canvas
        double o[2];
        o[0] = left_buffer + (canvsize[0]-left_buffer-right_buffer)/2.;
        o[1] = top_buffer+(canvsize[1]-top_buffer-bottom_buffer)/2.;    //origin

        o[0] += _origin_offset[0];
        o[1] += _origin_offset[1];

        _origin_pixels[0] = o[0];
        _origin_pixels[1] = o[1];    //copy to member

        double plotsize[2];
        plotsize[0] = canvsize[0] - (left_buffer+right_buffer);
        plotsize[1] = canvsize[1] - (top_buffer+bottom_buffer);

        //Determine the pixels per meter
        double ppm = fmin(plotsize[0]/fieldsize[0]*plot_scale, plotsize[1]/fieldsize[1]*plot_scale);

        _meters_per_pixel = 1./ppm;

        //get the radial extents
        double radvals[2];    //[min, max]
        Land::getRadialExtents(*V, radvals, V->sf.tht.val);


        //Get the solar field anglular limits
        double sfangles[2];
        sfangles[1] = V->sf.accept_max.val*D2R;
        sfangles[0] = V->sf.accept_min.val*D2R;

        //Set the brush/pen for inclusions
        _dc.SetPen( wxPen( white, 0, wxPENSTYLE_TRANSPARENT) );
        _dc.SetBrush( wxBrush( gray ) );

        //Draw the radial extents
        //draw the radial geometry
        if(radvals[0] > 0.)
        {
            if(fabs(sfangles[0]) == PI && fabs(sfangles[1]) == PI)
            {
                _dc.DrawCircle(o[0], o[1], radvals[1]*ppm);
            }
            else
            {
                //DrawArc draws from 0 to 1, counterclockwise
                double
                    x0 = radvals[1]*sin(sfangles[0])*ppm + o[0],
                    y0 = -radvals[1]*cos(sfangles[0])*ppm + o[1],
                    x1 = radvals[1]*sin(sfangles[1])*ppm + o[0],
                    y1 = -radvals[1]*cos(sfangles[1])*ppm + o[1];

                _dc.DrawArc(x1,y1,x0,y0,o[0],o[1]);
            }
        }

        //Get the inclusions
        //bounds_array *incs = _SF->getLandObject()->getInclusions();
        vector<vector<sp_point> > *incs = &_SF->getVarMap()->land.inclusions.val;
        //Get the exclusions
        //bounds_array *excs = _SF->getLandObject()->getExclusions();
        vector<vector<sp_point> > *excs = &_SF->getVarMap()->land.exclusions.val;

        //Convert the point vectors into double arrays
        int
            ninc_polys = incs->size(),
            nexc_polys = excs->size();

        //For each polygon region in the inclusions:
        _dc.SetBrush( wxBrush( gray ) );
        for(int i=0; i<ninc_polys; i++)
        {
            int ninc = incs->at(i).size();

            double
                *xincs = new double[ninc],
                *yincs = new double[ninc];

            for(int j=0; j<ninc; j++)
            {
                xincs[j] = incs->at(i).at(j).x - _SF->getVarMap()->land.tower_offset_x.val;
                yincs[j] = incs->at(i).at(j).y - _SF->getVarMap()->land.tower_offset_y.val;
            }

            DrawScaledPolygon(_dc, ppm, o, xincs, yincs, ninc);

            delete[] xincs;
            delete[] yincs;
        }

        _dc.SetBrush( wxBrush( white ) );
        //Draw the exclusions
        for(int i=0; i<nexc_polys; i++)
        {
            int nexc = excs->at(i).size();

            double
                *xexcs = new double[nexc],
                *yexcs = new double[nexc];

            for(int j=0; j<nexc; j++)
            {
                xexcs[j] = excs->at(i).at(j).x;
                yexcs[j] = excs->at(i).at(j).y;
            }

            DrawScaledPolygon(_dc, ppm, o, xexcs, yexcs, nexc);
        }

        //exclude the center region
        _dc.SetBrush( wxBrush( white ) );
        if(radvals[1] > 0.)
        {
            _dc.DrawEllipse(o[0]-radvals[0]*ppm, o[1]-radvals[0]*ppm, radvals[0]*ppm*2., radvals[0]*ppm*2.);
        }

        //-----draw axis items that are to be clipped
        //x axis
        int x_axis_loc = top_buffer + plotsize[1];
        _dc.SetPen(wxPen(black, 1 * ppimult, wxPENSTYLE_SOLID));
        _dc.DrawLine(o[0], x_axis_loc, o[0], x_axis_loc + 10 * ppimult);
        //y axis
        _dc.DrawLine(left_buffer - 10 * ppimult, o[1], left_buffer, o[1]);
        //Draw the origin
        _dc.SetPen(wxPen(wxT("black"), 1 * ppimult, wxPENSTYLE_SOLID));
        _dc.SetBrush(*wxRED_BRUSH);
        _dc.DrawCircle(o[0], o[1], 5 * ppimult);

        //Draw the clipping rectangles
        _dc.SetPen( wxPen( white, 1, wxPENSTYLE_TRANSPARENT ) );
        _dc.SetBrush( *wxWHITE_BRUSH );

        _dc.DrawRectangle(0, 0, left_buffer, canvsize[1]);
        _dc.DrawRectangle(0, plotsize[1], canvsize[0], bottom_buffer);
        _dc.DrawRectangle(canvsize[0]-right_buffer, 0, right_buffer, canvsize[1]);
        _dc.DrawRectangle(0, 0, canvsize[0], 1);

        //Draw the plot boundary
        _dc.SetPen( wxPen( black, 1*ppimult, wxPENSTYLE_SOLID) );
        _dc.SetBrush( *wxTRANSPARENT_BRUSH );
        _dc.DrawRectangle( left_buffer, 1*ppimult+top_buffer, plotsize[0], plotsize[1]);

        //Draw the x axis
        _dc.SetPen(wxPen(black, 1 * ppimult, wxPENSTYLE_SOLID));
        ets = "0";
        etss = _dc.GetTextExtent( ets );
        _dc.DrawText(ets, o[0]-etss.GetWidth()/2, x_axis_loc + 9*ppimult);
        ets = "Field position (East+) [m]";
        etss = _dc.GetTextExtent( ets );
        _dc.DrawText(ets, o[0] - etss.GetWidth()/2, x_axis_loc + (15*ppimult+etss.GetHeight()));

        //estimate the number of divisions
        ets = my_to_string( int(-fieldsize[0]/2.));
        etss = _dc.GetTextExtent( ets );
        int ndiv = fmin(int(plotsize[0]*_zoom_fact/ (etss.GetWidth()*1.5 )), (int)(_zoom_fact*20));
        double xscale = calcScale(fieldsize[0], ndiv);
        double
            xtickloc = xscale,
            xtickloc_ppm = xscale*ppm;
        while(xtickloc < fieldsize[0]/2.)
        {
            //To the right
            _dc.DrawLine(o[0]+xtickloc_ppm, x_axis_loc, o[0]+xtickloc_ppm, x_axis_loc+7*ppimult);
            string xts = my_to_string(int(xtickloc+.000001));
            wxSize xte = _dc.GetTextExtent(xts);
            _dc.DrawText( xts , o[0]+xtickloc_ppm-xte.GetWidth()/2, x_axis_loc+9*ppimult);
            //To the left
            _dc.DrawLine(o[0]-xtickloc_ppm, x_axis_loc, o[0]-xtickloc_ppm, x_axis_loc+7*ppimult);
            xts = my_to_string(int(-xtickloc-.000001));
            xte = _dc.GetTextExtent( xts );
            _dc.DrawText( xts , o[0]-xtickloc_ppm-xte.GetWidth()/2, x_axis_loc+9*ppimult);
            xtickloc += xscale;
            xtickloc_ppm += xscale*ppm;
        }

        //Draw the y axis
        string etsy = "0"; wxSize etssy = _dc.GetTextExtent( etsy );
        _dc.DrawText(etsy, left_buffer-(etssy.GetWidth()+10), o[1]-etssy.GetHeight()/2);

        //Label
        etsy = "Field position (North+) [m]";
        etssy = _dc.GetTextExtent(etsy);
        _dc.DrawRotatedText(etsy, 2, (plotsize[1]+etssy.GetWidth())/2., 90.);

        //estimate the number of divisions
        etsy = my_to_string( int(-fieldsize[1]/2.));
        etssy = _dc.GetTextExtent( etsy );
        int ndivy = min(int(plotsize[0]*_zoom_fact/ (etssy.GetHeight()*2. )), (int)(_zoom_fact*20));
        double yscale = calcScale(fieldsize[1], ndivy);

        double
            ytickloc = yscale,
            ytickloc_ppm = yscale*ppm;
        while(ytickloc < fieldsize[1]/2. )
        {
            //To the bottom
            _dc.DrawLine(left_buffer-7, o[1]+ytickloc_ppm, left_buffer, o[1]+ytickloc_ppm);
            string yts =my_to_string(int(-ytickloc-.00001));
            wxSize yte = _dc.GetTextExtent(yts);
            _dc.DrawText( yts , left_buffer-10*ppimult-yte.GetWidth(), o[1]+ytickloc_ppm-yte.GetHeight()/2);

            //To the top
            _dc.DrawLine(left_buffer-7*ppimult, o[1]-ytickloc_ppm, left_buffer, o[1]-ytickloc_ppm);
            yts = my_to_string(int(ytickloc+.000001));
            yte = _dc.GetTextExtent(yts);
            _dc.DrawText( yts , left_buffer-10*ppimult-yte.GetWidth(), o[1]-ytickloc_ppm-yte.GetHeight()/2);

            ytickloc += yscale;
            ytickloc_ppm += yscale*ppm;
        }

        //Draw the x and y axis lines
        _dc.SetPen( wxPen( black, 1*ppimult, wxPENSTYLE_DOT ) );
        _dc.DrawLine(left_buffer, o[1], canvsize[0]-right_buffer, o[1]);    //x line
        _dc.DrawLine(o[0], top_buffer, o[0], x_axis_loc);    //y line

    }
    else if(_option == FIELD_PLOT::MESH)
    {

        vector<opt_element*> mesh = _SF->getOpticalHashTree()->get_terminal_nodes();

        //if the mesh has no data, quit here and just create a plot of the available land
        if( mesh.size() == 0)
        {
            SetPlotOption(FIELD_PLOT::LAND);
            wxPaintEvent evt(this->GetId());
            ProcessEvent( evt );
            return;
        }

        //Plot the zones of the optical mesh

        double pbounds[2];
        Land::getExtents( *V, pbounds );
        double fieldsize[2];
        fieldsize[0] = pbounds[1]*2.;
        fieldsize[1] = pbounds[1]*2.;

        //we need 40pix on each side for x label, y label, gradient bar
        _dc.SetFont( wxFont(_fontsize, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL) );
        string ets = my_to_string(-int(pbounds[1]));    //create a string that is approximately the largest extent on the plot
        wxSize etss = _dc.GetTextExtent( ets );
        int
            left_buffer = 15+etss.GetWidth()+etss.GetHeight(),
            right_buffer = 40+etss.GetWidth(),
            top_buffer = 0,
            bottom_buffer = etss.GetHeight()*2+15*ppimult;

        //Origin location - relative to the canvas
        double o[2];
        o[0] = left_buffer + (canvsize[0]-left_buffer-right_buffer)/2.;
        o[1] = top_buffer+(canvsize[1]-top_buffer-bottom_buffer)/2.;    //origin

        o[0] += _origin_offset[0];
        o[1] += _origin_offset[1];

        _origin_pixels[0] = o[0];
        _origin_pixels[1] = o[1];    //copy to member

        double plotsize[2];
        plotsize[0] = canvsize[0] - (left_buffer+right_buffer);
        plotsize[1] = canvsize[1] - (top_buffer+bottom_buffer);

        //Determine the pixels per meter
        double ppm = min(plotsize[0]/fieldsize[0]*plot_scale, plotsize[1]/fieldsize[1]*plot_scale);

        _meters_per_pixel = 1./ppm;

        //get the radial extents
        double radvals[2];    //[min, max]
        Land::getRadialExtents(*V, radvals, V->sf.tht.val);


        //Draw the plot boundary
        _dc.SetPen( wxPen( black, 1*ppimult, wxPENSTYLE_SOLID) );
        _dc.DrawRectangle( left_buffer, 1*ppimult+top_buffer, plotsize[0], plotsize[1]);

        //Draw the elements
        _dc.SetPen( wxPen( black, 1*ppimult, wxPENSTYLE_SOLID ) );
        _dc.SetBrush( wxBrush( white, wxBRUSHSTYLE_TRANSPARENT ));

        for(int i=0; i<(int)mesh.size(); i++)
        {
            double *rr = mesh.at(i)->get_xr();
            double *azr = mesh.at(i)->get_yr();
            double azr0 = azr[0];
            double azr1 = azr[1];
            //translate cylindrical into cartesian
            double
                sina1 = sin(azr1),
                sina0 = sin(azr0),
                cosa1 = cos(azr1),
                cosa0 = cos(azr0);
            double
                x00 = floor(rr[0]*sina1*ppm + o[0]),
                y00 = floor(-rr[0]*cosa1*ppm + o[1]),
                x01 = floor(rr[0]*sina0*ppm + o[0]),
                y01 = floor(-rr[0]*cosa0*ppm + o[1]),
                x10 = floor(rr[1]*sina1*ppm + o[0]),
                y10 = floor(-rr[1]*cosa1*ppm + o[1]),
                x11 = floor(rr[1]*sina0*ppm + o[0]),
                y11 = floor(-rr[1]*cosa0*ppm + o[1]);

            _dc.DrawArc(x00,y00,x01,y01,o[0],o[1]);
            _dc.DrawArc(x10,y10,x11,y11,o[0],o[1]);
            _dc.DrawLine(x00,y00,x10,y10);
            _dc.DrawLine(x01,y01,x11,y11);
        }

        //Draw the clipping rectangles
        _dc.SetPen(wxPen(white, 1, wxPENSTYLE_TRANSPARENT));
        _dc.SetBrush(*wxWHITE_BRUSH);

        _dc.DrawRectangle(0, 0, left_buffer, canvsize[1]);
        _dc.DrawRectangle(0, plotsize[1]+top_buffer+1*ppimult, canvsize[0], bottom_buffer);
        _dc.DrawRectangle(canvsize[0] - right_buffer, 0, right_buffer, canvsize[1]);
        _dc.DrawRectangle(0, 0, canvsize[0], 1);

        //Draw the x axis
        int x_axis_loc = top_buffer + plotsize[1];
        _dc.SetPen(wxPen(black, 1 * ppimult, wxPENSTYLE_SOLID));
        _dc.DrawLine(o[0], x_axis_loc, o[0], x_axis_loc + 10 * ppimult);
        ets = "0";
        etss = _dc.GetTextExtent(ets);
        _dc.DrawText(ets, o[0] - etss.GetWidth() / 2, x_axis_loc + 9 * ppimult);
        ets = "Field position (East+) [m]";
        etss = _dc.GetTextExtent(ets);
        _dc.DrawText(ets, o[0] - etss.GetWidth() / 2, x_axis_loc + (15 * ppimult + etss.GetHeight()));

        //estimate the number of divisions
        ets = my_to_string(int(-fieldsize[0] / 2.));
        etss = _dc.GetTextExtent(ets);
        int ndiv = min(int(plotsize[0] * _zoom_fact / (etss.GetWidth()*1.5)), (int)(_zoom_fact * 20));
        double xscale = calcScale(fieldsize[0], ndiv);
        double
            xtickloc = xscale,
            xtickloc_ppm = xscale*ppm;
        while (xtickloc < fieldsize[0] / 2.) {
            //To the right
            _dc.DrawLine(o[0] + xtickloc_ppm, x_axis_loc, o[0] + xtickloc_ppm, x_axis_loc + 7 * ppimult);
            string xts = my_to_string(int(xtickloc + .000001));
            wxSize xte = _dc.GetTextExtent(xts);
            _dc.DrawText(xts, o[0] + xtickloc_ppm - xte.GetWidth() / 2, x_axis_loc + 9 * ppimult);
            //To the left
            _dc.DrawLine(o[0] - xtickloc_ppm, x_axis_loc, o[0] - xtickloc_ppm, x_axis_loc + 7 * ppimult);
            xts = my_to_string(int(-xtickloc - .000001));
            xte = _dc.GetTextExtent(xts);
            _dc.DrawText(xts, o[0] - xtickloc_ppm - xte.GetWidth() / 2, x_axis_loc + 9 * ppimult);
            xtickloc += xscale;
            xtickloc_ppm += xscale*ppm;
        }

        //Draw the y axis
        _dc.DrawLine(left_buffer - 10 * ppimult, o[1], left_buffer, o[1]);
        string etsy = "0"; wxSize etssy = _dc.GetTextExtent(etsy);
        _dc.DrawText(etsy, left_buffer - (etssy.GetWidth() + 10 * ppimult), o[1] - etssy.GetHeight() / 2);

        //Label
        etsy = "Field position (North+) [m]";
        etssy = _dc.GetTextExtent(etsy);
        _dc.DrawRotatedText(etsy, 2, (plotsize[1] + etssy.GetWidth()) / 2., 90.);

        //estimate the number of divisions
        etsy = my_to_string(int(-fieldsize[1] / 2.));
        etssy = _dc.GetTextExtent(etsy);
        int ndivy = min(int(plotsize[0] * _zoom_fact / (etssy.GetHeight()*2.)), (int)(_zoom_fact * 20));
        double yscale = calcScale(fieldsize[1], ndivy);

        double
            ytickloc = yscale,
            ytickloc_ppm = yscale*ppm;
        while (ytickloc < fieldsize[1] / 2.) {
            //To the bottom
            _dc.DrawLine(left_buffer - 7 * ppimult, o[1] + ytickloc_ppm, left_buffer, o[1] + ytickloc_ppm);
            string yts = my_to_string(int(-ytickloc - .00001));
            wxSize yte = _dc.GetTextExtent(yts);
            _dc.DrawText(yts, left_buffer - 10 * ppimult - yte.GetWidth(), o[1] + ytickloc_ppm - yte.GetHeight() / 2);

            //To the top
            _dc.DrawLine(left_buffer - 7 * ppimult, o[1] - ytickloc_ppm, left_buffer, o[1] - ytickloc_ppm);
            yts = my_to_string(int(ytickloc + .000001));
            yte = _dc.GetTextExtent(yts);
            _dc.DrawText(yts, left_buffer - 10 * ppimult - yte.GetWidth(), o[1] - ytickloc_ppm - yte.GetHeight() / 2);

            ytickloc += yscale;
            ytickloc_ppm += yscale*ppm;
        }

        //Draw the x and y axis lines
        _dc.SetPen( wxPen( black, 1*ppimult, wxPENSTYLE_DOT ) );
        _dc.DrawLine(left_buffer, o[1], canvsize[0]-right_buffer, o[1]);    //x line
        _dc.DrawLine(o[0], top_buffer, o[0], x_axis_loc);    //y line

        //Draw the origin
        _dc.SetPen( wxPen( wxT("black"), 1, wxPENSTYLE_SOLID) );
        _dc.SetBrush( *wxRED_BRUSH );
        _dc.DrawCircle( o[0], o[1], 5*ppimult);

    }
    else if(_option != FIELD_PLOT::LAND && _option != FIELD_PLOT::MESH)
    {    //Field layout geometry
        //Get the plot extents size = [4], {xmax, xmin, ymax, ymin}
        double
            *extents = _SF->getPlotBounds(),
            xsize = extents[0] - extents[1],    //Physical size of the plotted area in the X direction
            ysize = extents[2] - extents[3];    //Physical size of the plotted area in the Y direction
        double fieldsize[] = {xsize, ysize};
        if(xsize == 0 || ysize == 0) return;    //No data

        //Get information about the heliostats
        Hvector *heliostats = _SF->getHeliostats();
        int npos = heliostats->size();
        Heliostat *H;

        //For each heliostat, get the plotted value of interest
        double
            valmin = 1.e23,
            valmax = 0.,
            valave = 0.;
        double *plot_vals = new double[npos];

        std::vector< int > rec_count(_SF->getActiveReceiverCount(), 0);

        if(_option != FIELD_PLOT::LAYOUT)
        {
            for(int i=0; i<npos; i++)
            {
                H = heliostats->at(i);

				if (!(H->IsEnabled() && H->IsInLayout()))
					continue;

                //Get the value of interest for the heliostat
                if(_option == FIELD_PLOT::EFF_TOT)
                {    //Total efficiency
                    plot_vals[i] = H->getEfficiencyTotal();
                }
                else if(_option == FIELD_PLOT::EFF_COS)
                {    //Cosine efficiency
                    plot_vals[i] = H->getEfficiencyCosine();
                }
                else if(_option == FIELD_PLOT::EFF_ATT)
                {    //Attenuation efficiency
                    plot_vals[i] = H->getEfficiencyAtten();
                }
                else if(_option == FIELD_PLOT::EFF_BLOCK)
                {    //Blocking efficiency
                    plot_vals[i] = H->getEfficiencyBlock();
                }
                else if(_option == FIELD_PLOT::EFF_SHAD)
                {    //Shading efficiency
                    plot_vals[i] = H->getEfficiencyShading();
                }
                else if(_option == FIELD_PLOT::EFF_INT)
                {    //Image intercept efficiency
                    plot_vals[i] = H->getEfficiencyIntercept();
                }
                else if(_option == FIELD_PLOT::EFF_REFLECT)
                {  //Heliostat reflectivity
                    plot_vals[i] = H->getTotalReflectivity();
                }
                else if(_option == FIELD_PLOT::POWER)
                {    // Delivered power
                    plot_vals[i] = H->getPowerToReceiver()/1000.;
                }
                else if(_option == FIELD_PLOT::RANK)
                {  //Color by layout ranking metric
                    plot_vals[i] = H->getRankingMetricValue();
                }
                else if (_option == FIELD_PLOT::EFF_ANNUAL)
                {  //Color by annual average efficiency
                    plot_vals[i] = H->getAnnualEfficiency();
                }
                else if (_option == FIELD_PLOT::ENERGY)
                {  //Color by annual power output
                    plot_vals[i] = H->getAnnualEnergy();
                }
                else if(_option == FIELD_PLOT::RECEIVER)
                {  //color by mapped receiver
                    int r=0;
                    for(r=0; r<(int)_SF->getReceivers()->size(); r++)
                    {
                        if (H->getWhichReceiver() == _SF->getReceivers()->at(r))
                        {
                            rec_count.at(r)++;
                            break;
                        }
                    }
                    plot_vals[i] = (double)(r+1);
                }
                else if(_option == FIELD_PLOT::EFF_CLOUD)
                { //Cloudiness efficiency
                    plot_vals[i] = H->getEfficiencyCloudiness();
                }
                if(plot_vals[i] < valmin) valmin = plot_vals[i];
                if(plot_vals[i] > valmax) valmax = plot_vals[i];
                valave += plot_vals[i];
            }
            valave *= 1./double(npos);
        }

        //Specify the plot region size
        const wxFont base_font = wxFont(_fontsize, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
        _dc.SetFont( base_font );

        string ets = my_to_string(-int(xsize));    //create a string that is approximately the largest extent on the plot
        wxSize etss = _dc.GetTextExtent( ets );
        string maxlab, avelab, minlab;    //Create the gradient bar labels
        wxString labfmt;
        double vmult;
        string vunits;

        switch (_option)
        {
        case FIELD_PLOT::EFF_TOT:
        case FIELD_PLOT::EFF_COS:
        case FIELD_PLOT::EFF_ATT:
        case FIELD_PLOT::EFF_BLOCK:
        case FIELD_PLOT::EFF_SHAD:
        case FIELD_PLOT::EFF_INT:
        case FIELD_PLOT::EFF_REFLECT:
        case FIELD_PLOT::EFF_CLOUD:
        case FIELD_PLOT::EFF_ANNUAL:
            vmult = 100.;
            vunits = "%";
            break;
        case FIELD_PLOT::POWER:
            vmult = 1.e-3;
            vunits = "kWt";
            break;
        case FIELD_PLOT::ENERGY:
            vmult = 1.e-6;
            vunits = "MWht";
            break;
        case FIELD_PLOT::RANK:
        case FIELD_PLOT::RECEIVER:
        case FIELD_PLOT::LAND:
        case FIELD_PLOT::LAYOUT:
        case FIELD_PLOT::MESH:
        default:
            vmult = 1.;
            vunits = "";
            break;
        }


        labfmt.Printf("%s.%df", "%", gui_util::CalcBestSigFigs(valmax*vmult));
        maxlab = to_string(valmax*vmult, labfmt.c_str()) + vunits;
        avelab = to_string(valave*vmult, labfmt.c_str()) + vunits;
        minlab = to_string(valmin*vmult, labfmt.c_str()) + vunits;
        wxSize glabs;
        //which is biggest in X?
        int glab_x;
        glabs = _dc.GetTextExtent(maxlab);
        glab_x = glabs.GetWidth();
        if( _dc.GetTextExtent(avelab).GetWidth() > glab_x )
        {
            glabs = _dc.GetTextExtent(avelab);
            glab_x = glabs.GetWidth();
        }
        if( _dc.GetTextExtent(minlab).GetWidth() > glab_x)
        {
            glabs = _dc.GetTextExtent(minlab);
            glab_x = glabs.GetWidth();
        }
        if(_option ==FIELD_PLOT::LAYOUT )
        {
            //layout only, no bar
            glabs.Set(1,1);
            glab_x = 1;
        }


        int
            left_buffer = 15*ppimult+etss.GetWidth()+etss.GetHeight(),
            right_buffer = 25*ppimult+glab_x+2*ppimult,    //gradient bar width is 20
            top_buffer = 0,
            bottom_buffer = etss.GetHeight()*2+15*ppimult;

        double plotsize[] = {canvsize[0] - (left_buffer+right_buffer), canvsize[1] - (top_buffer+bottom_buffer)};

        //Determine the pixels per meter
        double ppm = min(plotsize[0]/fieldsize[0]*plot_scale, plotsize[1]/fieldsize[1]*plot_scale);

        //Determine the origin, center the plot on the canvas
        double o[] = {
            left_buffer + plotsize[0]/2. - (xsize/2. + extents[1])*ppm,
            top_buffer + plotsize[1]/2. + (ysize/2. + extents[3])*ppm};
        o[0] += _origin_offset[0];
        o[1] += _origin_offset[1];

        _origin_pixels[0] = o[0];
        _origin_pixels[1] = o[1];    //copy to member
        _meters_per_pixel = 1./ppm;

        //Draw the x axis
        int x_axis_loc = top_buffer+plotsize[1];

        //Only plot shadows for rectangular heliostats right now...
        if( heliostats->at(0)->getVarMap()->is_round.mapval() == var_heliostat::IS_ROUND::RECTANGULAR)
        {
            for(int i=0; i<npos; i++)
            {

                //Get the shadow coordinates for each heliostat and plot
                H = heliostats->at(i);

                double rcoll = H->getCollisionRadius()*ppm;
                //check to see if this heliostat needs drawing
                sp_point *loc = H->getLocation();
                double xlocm = o[0] + loc->x*ppm;
                double ylocm = o[1] - loc->y*ppm;

                if((xlocm - rcoll)> canvsize[0] || (xlocm + rcoll) < 0 || (ylocm - rcoll)> canvsize[1] || (ylocm + rcoll) < 0)
                    continue;

                //if the heliostat is omitted, don't draw a shadow
                if(! H->IsInLayout() )
                    continue;

                vector<sp_point> *shad = H->getShadowCoords();
                int ns = shad->size();
                double
                    *xs = new double[ns],
                    *ys = new double[ns];
                for(int j=0; j<ns; j++)
                {
                    xs[j] = shad->at(j).x;
                    ys[j] = shad->at(j).y;
                }
                _dc.SetPen( wxPen( shadow , 1, wxPENSTYLE_SOLID) );
                _dc.SetBrush( wxBrush( shadow, wxBRUSHSTYLE_SOLID) );
                DrawScaledPolygon(_dc, ppm, o, xs, ys, ns);

                delete[] xs;
                delete[] ys;
            }
        }

        unordered_map<std::string, wxColour> receiver_color_map;

        //Handle round and rectangular heliostats differently
        if( heliostats->at(0)->getVarMap()->is_round.mapval() == var_heliostat::IS_ROUND::ROUND)
        {
            double Hw = heliostats->at(0)->getVarMap()->width.val;
            for(int i=0; i<npos; i++)
            {
                //Get the location and size of each heliostat
                H = heliostats->at(i);

                double
                    x = H->getLocation()->x,
                    y = H->getLocation()->y;
                if(_option == FIELD_PLOT::LAYOUT)
                {
                    _dc.SetPen( wxPen( black, 1, wxPENSTYLE_SOLID) );
                    _dc.SetBrush( wxBrush( helio_region, wxBRUSHSTYLE_SOLID) );
                }
                else
                {
                    wxColour gc;
                    ColorGradientHotCold( gc, (plot_vals[i]-valmin)/(valmax - valmin) );
                    _dc.SetPen( wxPen( gc, 1, wxPENSTYLE_SOLID) );
                    _dc.SetBrush( wxBrush( gc, wxBRUSHSTYLE_SOLID ) );
                }
                DrawScaledEllipse(_dc, ppm, o, x, y, Hw);

            }
        }
        else
        {    //rectangular

            //if showing data and we're zoomed in, scale the label font size
            if (_is_data_visible)
            {
                //create a sample label
                wxSize ext = _dc.GetTextExtent(maxlab);
                //font scaling factor
                double label_scale = heliostats->front()->getCollisionRadius()*ppm / ext.GetWidth() * 1.41;
                int fontsize_scaled = max( (int)(label_scale * _fontsize), 4 );

                //check a sample label size
                _dc.SetFont(wxFont(fontsize_scaled, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

            }

            for(int i=0; i<npos; i++)
            {
                H = heliostats->at(i);

                double rcoll = H->getCollisionRadius()*ppm;
                //check to see if this heliostat needs drawing
                sp_point *loc = H->getLocation();
                double xlocm = o[0] + loc->x*ppm;
                double ylocm = o[1] - loc->y*ppm;

                if((xlocm - rcoll)> canvsize[0] || (xlocm + rcoll) < 0 || (ylocm - rcoll)> canvsize[1] || (ylocm + rcoll) < 0)
                    continue;

                //if the heliostat is omitted, just put a dot and move on
                if(! H->IsInLayout() )
                {
                    // _dc.SetPen( wxPen( wxColour(0,0,0), 1, wxPENSTYLE_SOLID) );
                    // _dc.DrawCircle(H->getLocation()->x*ppm + o[0], -H->getLocation()->y*ppm + o[1], (int)(std::ceil(ppm/2.+.00001)));
                    continue;
                } 

                //get the corner coordinates for each heliostat and plot
                vector<sp_point> *corners = H->getCornerCoords();
                int nc = corners->size();
                double
                    *xc = new double[nc],
                    *yc = new double[nc];

                for(int j=0; j<nc; j++)
                {
                    xc[j] = corners->at(j).x;
                    yc[j] = corners->at(j).y;
                }

                //Plot
                if(_option == FIELD_PLOT::LAYOUT)
                {
                    _dc.SetPen( wxPen( black, 1, wxPENSTYLE_SOLID) );
                    _dc.SetBrush( wxBrush( helio_region, wxBRUSHSTYLE_SOLID) );
                }
                else
                {

                    //if the heliostat is selected, use a different outline
                    wxColour gc;
                    if (_option == FIELD_PLOT::RECEIVER)
                    {
                        PlotBase::ColorGradientRainbow(gc, (plot_vals[i] - valmin) / (valmax+1 - valmin));
                        //keep track of which receiver is assigned to which color
                        receiver_color_map[H->getWhichReceiver()->getVarMap()->rec_name.val] = gc;
                    }
                    else
                        ColorGradientHotCold( gc, (plot_vals[i]-valmin)/(valmax - valmin) );

                    bool is_selected = std::find(_helios_select.begin(), _helios_select.end(), H ) != _helios_select.end();
                    if( is_selected )
                    {
                        wxColour emph;
                        emph.Set(0,255,0);
                        _dc.SetPen( wxPen( emph, 4, wxPENSTYLE_SOLID) );
                    }
                    else
                    {
                        _dc.SetPen( wxPen( gc, 1, wxPENSTYLE_SOLID) );
                    }
                    
                    _dc.SetBrush( wxBrush( gc, wxBRUSHSTYLE_SOLID ) );
                }
                DrawScaledPolygon(_dc, ppm, o, xc, yc, nc);

                //if data is visible and sizes are okay, show text
                if( _is_data_visible && rcoll > 7. && _option != FIELD_PLOT::LAYOUT)
                {
                    wxString labval = to_string(vmult*plot_vals[i], labfmt.c_str());
                    wxSize ext = _dc.GetTextExtent(labval);

                    _dc.SetTextForeground( *wxLIGHT_GREY );
                    _dc.DrawText( labval, xlocm-ext.GetWidth()/2, ylocm-ext.GetHeight()/2 );
                }

                delete [] xc;
                delete [] yc;
            }
        }
        _dc.SetFont( base_font );
        _dc.SetTextForeground( *wxBLACK );
        //Draw the clipping rectangles
        _dc.SetPen( wxPen( white, 1, wxPENSTYLE_TRANSPARENT) );
        _dc.SetBrush( *wxWHITE_BRUSH );

        _dc.DrawRectangle(0, 0, left_buffer, canvsize[1]);
        _dc.DrawRectangle(0, plotsize[1], canvsize[0], bottom_buffer);
        _dc.DrawRectangle(canvsize[0]-right_buffer, 0, right_buffer, canvsize[1]);
        _dc.DrawRectangle(0, 0, canvsize[0], 1);

        //if annotating, draw here
        if( _helios_annot.size() + _solarfield_annot.size() > 0 )
        {
			int annot_text_linesep = 2; //line separation/buffer
			int annot_col_buffer = 20; //column separation
			int annot_box_linewidth = 3; //bounding box line width
			int annot_box_frame_buffer = 4; //bounding box external buffer (offset)
			int annot_box_text_buffer = 5; //buffer between annot box and internal text
			int annot_text_id_offset = 15; //offset between left of base text and ID lines

			//parse the annotation. solarfield annotation overrides heliostat annotation
			std::vector<std::string> lines = split( _solarfield_annot.size() > 0 ? _solarfield_annot : _helios_annot, ";");

			//figure out the widest line
			int abox_text_height = 0, col0width = 0, col1width = 0, i_id = -1;
			for (size_t i = 0; i < lines.size(); i++)
			{
				//skip the ID line
				if (lines.at(i).find("ID") != std::string::npos)
				{
					i_id = i;
					continue;
				}

				//process other lines
				std::vector<std::string> oneline = split(lines.at(i), ",");
				wxSize ts_i0 = _dc.GetTextExtent(oneline.at(0));
				wxSize ts_i1 = _dc.GetTextExtent(oneline.at(1));

				if (i > 0)
				{
					if (ts_i0.GetWidth() > col0width)
						col0width = ts_i0.GetWidth();
					if (ts_i1.GetWidth() > col1width)
						col1width = ts_i1.GetWidth();
				}
				else
				{
					col0width = ts_i0.GetWidth();
					col1width = ts_i1.GetWidth();
				}

				//add the larger height of the label and value
				abox_text_height += std::fmax(ts_i0.GetHeight(), ts_i1.GetHeight()) + annot_text_linesep;
			}

			//calculate the upper right of the plot frame
			int Ux = canvsize[0] - right_buffer;
			int Uy = top_buffer;

			//box width and annot box x coord
			int abox_w = col0width + annot_col_buffer + col1width + 2* annot_box_text_buffer + annot_box_linewidth;
			int Ax = Ux - annot_box_frame_buffer - abox_w - annot_box_linewidth;

			//adjust bounding box for the ID's line
			
			std::vector<std::string> idlines; //vector of lines to save

            if (_solarfield_annot.size() > 0)
                i_id = -1; //don't do the individual ID stuff if we're in solarfield mode

			wxString idlab = "Selected heliostat ID's:";
			if (i_id > 0)
			{

				std::vector<std::string> idline = split(lines.at(i_id), ",");
				std::vector<std::string> idvals = split(idline.at(1), ".");

				idlines.push_back("");

				abox_text_height += _dc.GetTextExtent(idlab).GetHeight() + annot_text_linesep;
				int nline = 0;
				int cwidth = 0;
				for (size_t i = 0; i < idvals.size(); i++)
				{
					wxSize id_ts = _dc.GetTextExtent(idvals.at(i)+", ");

					//add height of the first line
					if (i == 0)
						abox_text_height += _dc.GetTextExtent(idvals.at(i)).GetHeight() + annot_text_linesep;
					
					//if the id list width spills over, start a new line
					if (id_ts.GetWidth() + cwidth > abox_w - annot_text_id_offset - annot_box_linewidth - 2*annot_box_text_buffer) //offset by 20 px, mind buffer
					{
						cwidth = id_ts.GetWidth();
						abox_text_height += id_ts.GetHeight() + annot_text_linesep;
						nline++;
						idlines.push_back(idvals.at(i) + (i < idvals.size() - 1 ? ", " : ""));
					}
					else
					{
						cwidth += id_ts.GetWidth();
						idlines.at(nline).append(idvals.at(i) + (i < idvals.size() - 1 ? ", " : ""));
					}
				}
			}

			//final calc of box height
			int abox_h = abox_text_height + 2 * annot_box_text_buffer + annot_box_linewidth; //buffer
			int Ay = Uy + annot_box_frame_buffer + annot_box_linewidth;
			//don't let abox spill off the bottom
			abox_h = std::fmin(abox_h, plotsize[1] - 2 * annot_box_frame_buffer - annot_box_linewidth);

			//calculate the x,y position of the text
			int T0 = annot_box_linewidth / 2 + annot_box_text_buffer;
			int Tx = Ax + T0;
			int Ty = Ay + T0;

			//draw the bounding rectangle

            wxPen cpen = _dc.GetPen();
            _dc.SetPen( wxPen( wxColour(220,220,220), annot_box_linewidth, wxPENSTYLE_SOLID ) );
            _dc.DrawRectangle(Ax, Ay, abox_w, abox_h );
            _dc.SetPen( cpen );

			//draw each line
			int current_y = Ty;

			//max drawable y
			int max_y_lim = top_buffer + plotsize[1] - bottom_buffer - annot_box_frame_buffer - annot_box_linewidth - annot_box_text_buffer - annot_text_linesep;
            if (idlines.size() > 0)
                max_y_lim -= _dc.GetTextExtent(idlines.back()).GetHeight();

			for (size_t i = 0; i < lines.size(); i++)
			{
				std::vector<std::string> oneline = split(lines.at(i), ",");

				if (i == i_id)
				{
					//handle ID's line separately
					_dc.DrawText(idlab, Tx, current_y); //label
					current_y += _dc.GetTextExtent(idlab).GetHeight() + annot_text_linesep;

					for (size_t j = 0; j < idlines.size(); j++)
					{
						_dc.DrawText(idlines.at(j), Tx + annot_text_id_offset, current_y);
						current_y += _dc.GetTextExtent(idlines.at(j)).GetHeight() + annot_text_linesep;
						
						if (current_y > max_y_lim)
						{
							_dc.DrawText("...", Tx + annot_text_id_offset, current_y);
							break;
						}
					}

				}
				else
				{
					//regular items
					_dc.DrawText(oneline.at(0), Tx, current_y ); //label
					_dc.DrawText(oneline.at(1), Tx + col0width + annot_col_buffer, current_y); 

					current_y += _dc.GetTextExtent(lines.at(i)).GetHeight() + annot_text_linesep;
				}
			}

        }

        if (_option != FIELD_PLOT::RECEIVER)
        {
            //Draw the gradient bar
            if (_option > FIELD_PLOT::LAYOUT && _option != FIELD_PLOT::RECEIVER )
                _dc.GradientFillLinear(wxRect(wxPoint(canvsize[0] - right_buffer + 5 * ppimult, 50 * ppimult), wxPoint(canvsize[0] - right_buffer + 25 * ppimult, x_axis_loc - 50 * ppimult)), grad_low, grad_high, wxNORTH);
            double lineave;
            if (fabs(valmax - valmin) > 1.e-6)
                lineave = 50 * ppimult + (plotsize[1] - 100.)*(1. - (valave - valmin) / (valmax - valmin));    //where should the average line indicator be in Y?
            else
                lineave = 50 * ppimult + (plotsize[1] - 100.)*.5;
            //Draw the average line
            _dc.SetPen(wxPen(white, 1, wxPENSTYLE_SOLID));
            _dc.DrawLine(canvsize[0] - right_buffer + 5 * ppimult, lineave, canvsize[0] - right_buffer + 25 * ppimult, lineave);
            //Label the gradient bar
            if (_option != FIELD_PLOT::LAYOUT && _option != FIELD_PLOT::RECEIVER )
            {
                _dc.DrawText(maxlab, canvsize[0] - right_buffer + 2 * ppimult, 51 * ppimult - glabs.GetHeight());
                _dc.DrawText(minlab, canvsize[0] - right_buffer + 2 * ppimult, x_axis_loc - 48 * ppimult);
                _dc.DrawText(avelab, canvsize[0] - right_buffer + 27 * ppimult, lineave - glabs.GetHeight() / 2);
            }
        }
        else
        {
            //draw the receiver colormap legend
            int cboxpos_x = left_buffer + 10;
            int cboxpos_y = top_buffer + 5;

            std::vector< std::string > r_in_map;
            for (unordered_map<std::string, wxColour>::iterator rc = receiver_color_map.begin(); rc != receiver_color_map.end(); rc++)
                r_in_map.push_back(rc->first);
            std::sort(r_in_map.begin(), r_in_map.end());

            for (int rc = 0; rc < r_in_map.size(); rc++)
            {
                int xstartpos = cboxpos_x;

                _dc.SetBrush(receiver_color_map[ r_in_map[rc] ]);
                wxSize labsize = _dc.GetTextExtent(r_in_map[rc]);
                int th = labsize.GetHeight();
                _dc.DrawRectangle(cboxpos_x, cboxpos_y, 1.3*th, th);
                cboxpos_x += (int)(1.3*th) + 4;
                
                _dc.DrawText(r_in_map[rc], cboxpos_x, cboxpos_y);
                cboxpos_x += labsize.GetWidth()+th/2;

                if (2*cboxpos_x-xstartpos > canvsize[0]*0.9 + left_buffer)
                {
                    cboxpos_x = left_buffer + 10;
                    cboxpos_y += 1.3*th;
                }
            }
        }

        delete [] plot_vals;


        //Draw the plot boundary
        _dc.SetPen( wxPen( black, 1*ppimult, wxPENSTYLE_SOLID) );
        _dc.SetBrush( *wxTRANSPARENT_BRUSH );
        _dc.DrawRectangle( left_buffer, 1*ppimult+top_buffer, plotsize[0], plotsize[1]);

        //Draw the x axis
        _dc.SetPen( wxPen( black, 1, wxPENSTYLE_SOLID) );
        _dc.DrawLine(o[0], x_axis_loc, o[0], x_axis_loc + 10*ppimult);
        ets = "0";
        etss = _dc.GetTextExtent( ets );
        if(o[0] > left_buffer && o[0] < left_buffer + plotsize[0])
            _dc.DrawText(ets, o[0]-etss.GetWidth()/2, x_axis_loc+9*ppimult);
        ets = "Field position (East+) [m]";
        etss = _dc.GetTextExtent( ets );
        _dc.DrawText(ets, left_buffer + plotsize[0]/2 - etss.GetWidth()/2, x_axis_loc+(15*ppimult+etss.GetHeight()));
        //estimate the number of divisions
        ets = my_to_string( int(-fieldsize[0]/2.));
        etss = _dc.GetTextExtent( ets );
        int ndiv = min(int(plotsize[0]*_zoom_fact/ (etss.GetWidth()*1.5 )), (int)(_zoom_fact*20));
        double xscale = calcScale(fieldsize[0], ndiv);

        //to the right
        double xtickloc = xscale;
        double xtickloc_ppm = xscale*ppm;

        while(true)
        {
            int xpos = o[0] + xtickloc_ppm;

            if(xpos > left_buffer + plotsize[0]) break;
            if(xtickloc_ppm != xtickloc_ppm) break;

            if(xpos > left_buffer)
            {
                _dc.DrawLine(xpos, x_axis_loc, xpos, x_axis_loc+7*ppimult);
                string xts = my_to_string(int(xtickloc+.000001));
                wxSize xte = _dc.GetTextExtent(xts);
                _dc.DrawText( xts , xpos-xte.GetWidth()/2, x_axis_loc+9*ppimult);
            }

            xtickloc += xscale;
            xtickloc_ppm += xscale*ppm;
        }

        //to the left
        xtickloc = xscale;
        xtickloc_ppm = xscale*ppm;

        while(true)
        {
            //To the left
            int xpos = o[0]-xtickloc_ppm;

            if(xpos < left_buffer) break;
            if(xtickloc_ppm != xtickloc_ppm) break;

            if(xpos < left_buffer + plotsize[0])
            {
                _dc.DrawLine(xpos, x_axis_loc, xpos, x_axis_loc+7*ppimult);
                string xts = my_to_string(int(-xtickloc-.000001));
                wxSize xte = _dc.GetTextExtent( xts );
                _dc.DrawText( xts , xpos-xte.GetWidth()/2, x_axis_loc+9*ppimult);
            }

            xtickloc += xscale;
            xtickloc_ppm += xscale*ppm;
        }


        //Draw the y axis
        _dc.DrawLine(left_buffer-10*ppimult, o[1], left_buffer, o[1]);
        string etsy = "0"; wxSize etssy = _dc.GetTextExtent( etsy );
        if(o[1] < plotsize[1] && o[1] > 0)
            _dc.DrawText(etsy, left_buffer-(etssy.GetWidth()+8*ppimult), o[1]-etssy.GetHeight()/2);
        //Label
        etsy = "Field position (North+) [m]";
        etssy = _dc.GetTextExtent(etsy);
        _dc.DrawRotatedText(etsy, 2, (plotsize[1]+etssy.GetWidth())/2., 90.);
        //estimate the number of divisions
        etsy = my_to_string( int(-fieldsize[1]/2.));
        etssy = _dc.GetTextExtent( etsy );
        int ndivy = min(int(plotsize[0]*_zoom_fact/ (etssy.GetHeight() )), (int)(_zoom_fact*20));
        double yscale = calcScale(fieldsize[1], ndivy);

        //To the top
        double ytickloc = yscale;
        double ytickloc_ppm = yscale*ppm;

        while(true)
        {
            int ypos = o[1] - ytickloc_ppm;

            if(ypos < top_buffer) break;
            if(ytickloc_ppm != ytickloc_ppm) break;

            if(ypos < top_buffer + plotsize[1])
            {
                _dc.DrawLine(left_buffer-7*ppimult, o[1]-ytickloc_ppm, left_buffer, o[1]-ytickloc_ppm);
                string yts = my_to_string(int(ytickloc+.00001));
                wxSize yte = _dc.GetTextExtent(yts);
                _dc.DrawText( yts , left_buffer-10*ppimult-yte.GetWidth(), o[1]-(ytickloc_ppm+yte.GetHeight()/2));
            }

            ytickloc += yscale;
            ytickloc_ppm += yscale*ppm;
        }

        //To the bottom
        ytickloc = yscale;
        ytickloc_ppm = yscale*ppm;
        while(true)
        {
            int ypos = o[1]+ytickloc_ppm;

            if(ypos > top_buffer + plotsize[1]) break;
            if(ytickloc_ppm != ytickloc_ppm) break;

            if(ypos > top_buffer)
            {
                _dc.DrawLine(left_buffer-7*ppimult, o[1]+ytickloc_ppm, left_buffer, o[1]+ytickloc_ppm);
                string yts = my_to_string(int(-ytickloc-.000001));
                wxSize yte = _dc.GetTextExtent(yts);
                _dc.DrawText( yts , left_buffer-10*ppimult-yte.GetWidth(), o[1]+ytickloc_ppm-yte.GetHeight()/2);
            }

            ytickloc += yscale;
            ytickloc_ppm += yscale*ppm;
        }

        //Draw the origin
        _dc.SetPen( wxPen( black, 1, wxPENSTYLE_SOLID) );
        _dc.SetBrush( *wxRED_BRUSH );
        _dc.DrawCircle( o[0], o[1], 5*ppimult);
    }

    //Draw the zoom rectangle
    if(_is_zoom_rectangle)
    {
        _dc.SetPen( wxPen( wxColor(80,80,225), 2, wxPENSTYLE_SOLID) );
        _dc.SetBrush( wxBrush(wxColor(200,200,225), wxBRUSHSTYLE_CROSS_HATCH) );
        _dc.DrawRectangle( _zoom_rectangle[0], _zoom_rectangle[1], _zoom_rectangle[2] - _zoom_rectangle[0], _zoom_rectangle[3] - _zoom_rectangle[1] );
    }

    _dc.SelectObject( wxNullBitmap );
    _pdc.DrawBitmap( _pbit, 0, 0);
    if(_reset_ppi)
    {
        _fontsize = (int)(_fontsize * (double)_ppiold/(double)_ppi);
        _ppi = _ppiold;
        _reset_ppi = false;
    }

}

void FieldPlot::DrawScaledPolygon( wxDC &dc, double ppm, double origin[2], double x[], double y[], int n_points)
{
    /*
    Take a polygon with n_points values (not closed) and indices of x[] and y[] of length n_points,
    and draw the polygon on the device. The X,Y coordinates will be scaled according to the maximum
    extents w and h.

    ppm            Pixels per meter of drawing
    origin[] {x-pos, y-pos} Location in pixels of the plot origin

    Note that the DC coordinates are (0,0) at the upper left corner, positive X to the right,
    and negative Y downward.

    */

    //Create an array of points of the items
    wxPoint *points;
    points = new wxPoint[n_points];

    for(int i=0; i<n_points; i++)
    {
        points[i].x = x[i]*ppm + origin[0];
        points[i].y = -y[i]*ppm + origin[1];
    }

    dc.DrawPolygon(n_points, points);

    delete [] points;
}

void FieldPlot::DrawScaledEllipse( wxDC &dc, double ppm, double origin[2], double x, double y, double diam)
{
    wxCoord
        xs, ys, ws, hs;
    xs = int(x*ppm + origin[0]);
    ys = int(y*ppm + origin[1]);
    ws = int(diam*ppm);
    hs = ws;

    dc.DrawEllipse(xs, ys, ws, hs);
}

void FieldPlot::ColorGradientHotCold(wxColour &col, double index)
{

    /*
    Index from 0-1 indicating the position on the scale from COLD to HOT

    Returns value indicating RRRGGGBBB on a scale of 0-255 for each integer triplet
    */
    double dind = index;
    if(index<0.) dind = 0.;
    if(index>1.) dind = 1.;

    col.Set(int(dind*255.), 0, int((1.-dind)*255.));

}

double FieldPlot::calcScale(double span, int segments)
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

void FieldPlot::ClearSelectedHeliostats()
{
    _helios_select.clear();
    _helios_annot.clear();
}

std::vector<Heliostat*> *FieldPlot::GetSelectedHeliostats()
{
    return &_helios_select;
}

void FieldPlot::HeliostatAnnotation(Heliostat *H)
{
    /*
    Produce an annotated text box that describes the selected heliostat(s)
    */

    //validation
    if( ! H )
        return;

    //--manage adding and removing based on selection state
    ptrdiff_t pos = std::find(_helios_select.begin(), _helios_select.end(), H) - _helios_select.begin();

    if( pos >= _helios_select.size() ) 
        //Add the heliostat only if it's not already in the stored vector
        _helios_select.push_back(H);
    else
        //If a selected heliostat was clicked again, delete it from the selected vector
        _helios_select.erase(_helios_select.begin()+pos);

    //clear the annotation string
    _helios_annot.clear();  

    //declare and init reporting values
    double h_avg_tot = 0.;
    double h_avg_atten = 0.;
    double h_avg_cosine = 0.;
    double h_avg_blocking = 0.;
    double h_avg_shading = 0.;
    double h_avg_spillage = 0.;
    double h_avg_cloud = 0.;
    double h_avg_power = 0.;
    double h_tot_power = 0.;
    double h_tot_area = 0.;
    double h_avg_annual = 0.;
    double h_e_annual = 0.;

    wxArrayStr ids;

    size_t nh = _helios_select.size();

    for(size_t i=0; i<nh; i++)
    {
        Heliostat *hi = _helios_select.at(i);
        h_avg_tot += hi->getEfficiencyTotal();
        h_avg_atten += hi->getEfficiencyAtten();
        h_avg_cosine += hi->getEfficiencyCosine();
        h_avg_blocking += hi->getEfficiencyBlock();
        h_avg_shading += hi->getEfficiencyShading();
        h_avg_spillage += hi->getEfficiencyIntercept();
        h_avg_cloud += hi->getEfficiencyCloudiness();
        h_avg_power += hi->getPowerToReceiver();
        h_tot_power += hi->getPowerToReceiver();
        h_tot_area += hi->getArea();
        h_avg_annual += hi->getAnnualEfficiency();
        h_e_annual += hi->getAnnualEnergy();

        ids.push_back( my_to_string(hi->getId()) );
    }

    if( nh > 1 )
    {
        h_avg_atten /= (double)nh;
        h_avg_tot /= (double)nh;
        h_avg_cosine /= (double)nh;
        h_avg_blocking /= (double)nh;
        h_avg_shading /= (double)nh;
        h_avg_spillage /= (double)nh;
        h_avg_cloud /= (double)nh;
        h_avg_power /= (double)nh;
        h_avg_annual /= (double)nh;
        h_e_annual /= (double)nh;
    }

	std::stringstream annot;
	annot << "Total efficiency" << "," << wxString::Format("%.1f %%", h_avg_tot*100.) << ";";
	annot << "Attenuation" << "," << wxString::Format("%.1f %%", h_avg_atten*100.) << ";";
	annot << "Cosine" << "," << wxString::Format("%.1f %%", h_avg_cosine*100.) << ";";
	annot << "Blocking" << "," << wxString::Format("%.1f %%", h_avg_blocking*100.) << ";";
	annot << "Shading" << "," << wxString::Format("%.1f %%", h_avg_shading*100.) << ";";
	annot << "Spillage" << "," << wxString::Format("%.1f %%", h_avg_spillage*100.) << ";";
	annot << "Cloudiness" << "," << wxString::Format("%.1f %%", h_avg_cloud*100.) << ";";
	annot << "Average power" << "," << wxString::Format("%.2f kW", h_avg_power / 1000.) << ";";
	annot << "Total power" << "," << wxString::Format("%.2f kW", h_tot_power / 1000.) << ";";
	annot << "Total area" << "," << wxString::Format("%.2f m2", h_tot_area) << ";";
    annot << "Annual efficiency" << "," << wxString::Format("%.1f %%", h_avg_annual*100.) << ";";
    annot << "Annual energy" << "," << wxString::Format("%2.f MW", h_e_annual / 1e6) << ";";
	annot << "ID" << ",";

	for (size_t i = 0; i < nh; i++)
		annot << _helios_select.at(i)->getId() << (i<nh-1 ? "." : "");

	_helios_annot = annot.str();

    return;
}

void FieldPlot::SolarFieldAnnotation(SolarField *SF, sim_result *R, std::vector<int> &options)
{
    /*
    Produce an annotated text box that describes the solar field
    */
    
    _solarfield_annot.clear();
    
    //validation
    if (!SF || !R || options.size() == 0)
        return;

    std::stringstream annot;

    for (std::vector<int>::iterator opt = options.begin(); opt != options.end(); opt++)
    {
        switch (*opt)
        {
        case PlotSelectDialog::DATE_TIME:
            annot << "Date/time," << R->time_date_stamp << ";";
            break;
        case PlotSelectDialog::SUN_POSITION:
            annot << "Sun position," << wxString::Format("%d az %d el (deg)", (int)(R->solar_az*R2D), (int)(90. - R->solar_zen*R2D)) << ";";
            break;
        case PlotSelectDialog::DNI:
            annot << "DNI," << (int)(R->dni*1000.) << " W/m2;";
            break;
        case PlotSelectDialog::TOTAL_EFF:
            annot << "Total efficiency," << (int)(1000.*(R->eff_total_sf.wtmean / R->eff_absorption.wtmean))/10. << "%;";
            break;
        case PlotSelectDialog::COSINE_EFF:
            annot << "Cosine efficiency," << std::setw(4) << (int)(1000.*R->eff_cosine.wtmean) / 10. << "%;";
            break;
        case PlotSelectDialog::BLOCK_EFF:
            annot << "Block/Shade efficiency," << std::setw(4) << (int)(1000.*R->eff_blocking.wtmean*R->eff_shading.wtmean) / 10. << "%;";
            break;
        case PlotSelectDialog::ATT_EFF:
            annot << "Attenuation efficiency," << std::setw(4) << (int)(1000.*R->eff_attenuation.wtmean) / 10. << "%;";
            break;
        case PlotSelectDialog::INT_EFF:
            annot << "Intercept efficiency," << std::setw(4) << (int)(1000.* R->eff_intercept.wtmean) / 10. << "%;";
            break;
        case PlotSelectDialog::TOT_POWER:
            annot << "Total power," << std::setw(4) << R->power_absorbed/1000. << " MW;";
            break;
        case PlotSelectDialog::AIM_METHOD:
            annot << "Aim method," << R->aim_method << ";";
            break;
        }
    }

    _solarfield_annot = annot.str();

    return;
}


//**********EVENTS***********************
BEGIN_EVENT_TABLE(FieldPlot, wxScrolledWindow)
    EVT_PAINT(FieldPlot::OnPaint)
    EVT_ERASE_BACKGROUND(FieldPlot::OnEraseBackground)
END_EVENT_TABLE()
