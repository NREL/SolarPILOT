
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

#include "FluxPlot.h"
#include "gui_util.h"
#include "SolarField.h"
#include "plot_contourf.h"
#include "plot_scatter.h"

#include <wx/dcgraph.h>
#include <wx/dcbuffer.h>

using namespace std;

FluxPlot::FluxPlot(wxPanel *parent, SolarField &SF, Hvector helios, int receiver, const wxWindowID id, 
                   const wxPoint pos, const wxSize size, long style)
        : wxScrolledWindow(parent, id, pos, size, style)
        {
            
    _zax_autoscale = true;
    _SF = &SF;    //Point to the solar field object
    _receiver = receiver;    //Index of the receiver to use
    _helios = helios;
    _plotobj.Create();

}

void FluxPlot::SetZRange(double zmin, double zmax, bool is_autoscale)
{
    _zax_autoscale = is_autoscale; 
    _zax_max = zmax; 
    _zax_min = zmin;
}

void FluxPlot::SetPlotData(SolarField &SF, Hvector helios, int receiver)
{
    _SF = &SF;
    _receiver = receiver;
    _helios = helios;
    _plotobj.Create();
}

bool FluxPlot::SaveDataTable(wxString &path, wxString &delim)
{
    Receiver *rec = _SF->getReceivers()->at(_receiver);    //This is the receiver to use
    FluxSurfaces *fs = rec->getFluxSurfaces();    
    FluxSurface *ffs = &fs->at(0);
    FluxGrid *fg = ffs->getFluxMap();

    //Calculate the element size
    int 
        fnx = ffs->getFluxNX(),
        fny = ffs->getFluxNY();
        
    //Copy the data into a double vector
    vector<vector<double> > fdata(fnx, vector<double>(fny));

    //need to manually reverse data for flux table to be consistent with plot
    for(int i=0; i<fnx; i++)
        for(int j=0; j<fny; j++)
            fdata.at(i).at(j) = fg->at(fnx-1-i).at(j).flux;
    PlotContourf tplot;
    
    return tplot.ExportDataTable(_plotobj, path, delim, fdata);
}

void FluxPlot::OnPaint(wxPaintEvent &event)
{
    //Use a memory DC with a bitmap, and catch the wxEraseBackground event to prevent flicker
     
    wxPaintDC _pdc(this);
    DoPaint(_pdc);
    event.Skip();
}

void FluxPlot::OnEraseBackground(wxEraseEvent &WXUNUSED(event)) {}

void FluxPlot::DoPaint(wxDC &_pdc)
{
    
    //Use a memory DC with a bitmap, and catch the wxEraseBackground event to prevent flicker
    wxMemoryDC _dc;
    int ssize_w = (int)this->GetSize().GetWidth();
    int ssize_h = (int)this->GetSize().GetHeight();

    //validation
    ssize_w = ssize_w < 100 ? 1024 : ssize_w;
    ssize_h = ssize_h < 100 ? 711 : ssize_h;

    _pbit.Create(ssize_w, ssize_h);
    _dc.SelectObject( _pbit );
    wxGCDC gdc(_dc);
    
    Receiver *rec = _SF->getReceivers()->at(_receiver);    //This is the receiver to use
    var_receiver *vrec = rec->getVarMap();
    int rgeom = rec->getGeometryType();
    
    wxSize parsize = this->GetClientSize();
    parsize.x = parsize.x < 100 ? 1024 : parsize.x;
    parsize.y = parsize.y < 100 ? 711 : parsize.y;

    _plotobj.SetPlotSize(parsize);
    
    /* 
    0    |    Continuous closed cylinder - external
    1    |    Continuous open cylinder - external    
    2    |    Continuous open cylinder - internal cavity
    3    |    Planar rectangle
    4    |    Planar ellipse
    5    |    Discrete closed N-polygon - external    
    6    |    Discrete open N-polygon - external
    7    |    Discrete open N-polygon - internal cavity
    */

    //set the z-axis scale if needed
    _plotobj.SetZRange(_zax_min, _zax_max, _zax_autoscale);

    double rw = rec->getReceiverWidth(*vrec);
    double rh = vrec->rec_height.val; 

    /* 
    Depending on the surface, define the x-axis differently.

    If the surface is continuous, express in circumferential angle.
    If the surface is flat or polygonal, express in length from the CCW bound.

    NOTE
    Unlike the global coordinates where the minimum receiver span indicates the boundary
    on the CCW extent, the flux map is reversed to have the CCW bound as the maximum 
    bound in the X direction on the plot. This is because we are looking at the flux map 
    from an external perspective and the CCW angle from the receiver's perspective will
    be on our right side.

    */
    double xaxmin=NULL, xaxmax=NULL, xaxcent;
    bool is_angle = false;

    //Get the flux surfaces
    FluxSurfaces *fs = rec->getFluxSurfaces();
    int nfs = fs->size();

    switch (rgeom)
    {
    case Receiver::REC_GEOM_TYPE::CYLINDRICAL_CLOSED:
    case Receiver::REC_GEOM_TYPE::CYLINDRICAL_OPEN:
    case Receiver::REC_GEOM_TYPE::CYLINDRICAL_CAV:
        xaxmin = -180.;
        xaxmax = 180.;
        xaxcent = 0.;
        is_angle = true;
        break;
    case Receiver::REC_GEOM_TYPE::PLANE_RECT:
    case Receiver::REC_GEOM_TYPE::PLANE_ELLIPSE:
        //Extents will be the geometrical limits of the receiver plane
        xaxmin = -rw/2.;
        xaxmax = rw/2.;
        break;
    case Receiver::REC_GEOM_TYPE::POLYGON_CLOSED:
    case Receiver::REC_GEOM_TYPE::POLYGON_OPEN:
    case Receiver::REC_GEOM_TYPE::POLYGON_CAV:
    {    //Polygonal surfaces

        //angular definitions for the reciever
        double span = PI + 2. * asin(vrec->rec_cav_cdepth.val);
        double angcent = vrec->panel_rotation.val;
        double angmin = angcent - span / 2.;
        double angmax = angcent + span / 2.;
        
        //Convert to degrees
        angmin *= R2D;
        angmax *= R2D;
        angcent *= R2D;

        //Loop through the receiver surfaces and add up the width of each
        double rwtot=0.;
        for(int i=0; i<nfs; i++)
        {
            rwtot += fs->at(i).getSurfaceWidth();
        }

        //Calculate from the receiver azimuth angle what the corresponding zero point on the
        //axis would be
        xaxcent = (angcent - angmin)/(angmax - angmin)*rwtot;
        //Calculate the extents of the receiver in distance
        //Reverse perspective.
        xaxmax = xaxcent * rwtot;
        xaxmin = -(1.-xaxcent)*rwtot;
        xaxcent = 0.;
        break;
    }
    default:
        break;
    }

    
    //The Y axis limits will be 0 at the bottom of the surface and H at the top
    double
        yaxmin = 0.,
        yaxmax = rh;
    
    _plotobj.SetXAxisRange(xaxmin, xaxmax);
    _plotobj.SetYAxisRange(yaxmin, yaxmax);
    
    if(is_angle) _plotobj.SetXLabel("Receiver circumferential position (0=North, +East) [deg]");
    else _plotobj.SetXLabel("Receiver horizontal position[m]");
    
    //Label
    _plotobj.SetYLabel("Receiver vertical position [m]");
    _plotobj.SetXAxisReversed(true);
    //Draw the boundaries of the receiver surfaces
    //0    |    Continuous closed cylinder - external
    //1    |    Continuous open cylinder - external    
    //2    |    Continuous open cylinder - internal cavity
    //3    |    Planar rectangle
    switch (rgeom)
    {
    case Receiver::REC_GEOM_TYPE::CYLINDRICAL_CLOSED:
    case Receiver::REC_GEOM_TYPE::CYLINDRICAL_OPEN:
    case Receiver::REC_GEOM_TYPE::CYLINDRICAL_CAV:
    case Receiver::REC_GEOM_TYPE::PLANE_RECT:
    case Receiver::REC_GEOM_TYPE::POLYGON_CLOSED:
    {
        //Set the fringe values
        if(rgeom == Receiver::REC_GEOM_TYPE::CYLINDRICAL_CLOSED)    //For the closed cylinder, wrap around in X
            _plotobj.SetDataWrapping(true);

        if(_plot_type == 0)
        {    //Flux map
            _plotobj.SetUnits(" kW/m2");
    
            //---- Draw the flux intensity ----
            FluxSurface *ffs = &fs->at(0);
            FluxGrid *fg = ffs->getFluxMap();

            //Calculate the element size
            int 
                fnx = ffs->getFluxNX(),
                fny = ffs->getFluxNY();
        
            //Copy the data into a double vector
            vector<vector<double> > fdata(fnx, vector<double>(fny));

            {
                bool datok = false;
                if( (int)fg->size() == fnx )
                {
                    if( (int)fg->front().size() == fny )
                    {
                        datok = true;

                        //Load data in. 
                        for(int i=0; i<fnx; i++)
                            for(int j=0; j<fny; j++)
                                fdata.at(i).at(j) = fg->at(i).at(j).flux;
                    }
                }
                if(!datok)
                {
                    //make something pretty
                    int nx = 40;
                    int ny = nx;
                    vector<vector<double> > ddat(nx, vector<double>(ny,0.));


                    double cf = 0.4;
                    double cfp = cf-0.2;

                    double xc = int(cf*nx);
                    double yc = int(cf*ny);

                    double xcp = int(cfp*nx);
                    double ycp = int(cfp*ny);

                    double rl = 0.4;

                    for(int i=0; i<nx; i++)
                    {
                        for(int j=0; j<ny; j++)
                        {
                            double dxc = (i-xc)/float(nx);
                            double dyc = (ny-1-j-yc)/float(ny);
        
                            double dxcp = (i-xcp)/float(nx);
                            double dycp = (ny-1-j-ycp)/float(ny);
        
                            double rc = sqrt(dxc*dxc + dyc*dyc);
        
                            double rcp = sqrt(dxcp*dxcp + dycp*dycp);

                            if(rc < rl && rcp > rl)
                                ddat.at(i).at(j) = 1.;
                        }
                    }
                    PlotContourf newplt;
                    int oldcmap = _plotobj.GetColormap();
                    _plotobj.SetColormap(COLORMAP::GRAYSCALE);
                    newplt.Plot(_plotobj, ddat);
                    _plotobj.SetColormap(oldcmap);
                    _plotobj.DrawText(_dc, "No data found", 0.5, 0.5);
                }
                else
                {
                    PlotContourf newplt;
                    newplt.Plot(_plotobj, fdata);
                }
            }


        }
        else if(_plot_type == 1)
        {    //Aim point scatter plot
            _plotobj.SetUnits(" m");
    
            vector<vector<double> > fdata;
            
            //Get receiver angles
            double raz = vrec->rec_azimuth.val*D2R;
            double rel = vrec->rec_elevation.val*D2R;
            double tht = vrec->optical_height.Val();
            double offx = vrec->rec_offset_x_global.Val();
            double offy = vrec->rec_offset_y_global.Val();
            //double offz = vrec->rec_offset_z_global.Val();
            double rdiam = rec->getReceiverWidth(*vrec);
            double rheight = vrec->rec_height.val;

            //Also Process to determine how many heliostats share each aim point and the average 
            //radial position of heliostats on the point
            //Calculate the element size
            int 
                fnx = fs->at(0).getFluxNX(),
                fny = fs->at(0).getFluxNY();
            vector<vector<int> > bins(fnx,vector<int>(fny,0));
            vector<vector<double> > lens(fnx,vector<double>(fny,0.));
            //----

            for(int i=0; i<(int)_helios.size(); i++)
            {
                if( _helios.at(i)->getWhichReceiver() != rec) continue;
                
                
                sp_point *aim = _helios.at(i)->getAimPoint();
                sp_point aimc(*aim);
                aimc.Add(-offx, -offy, -tht);
                double radialpos = _helios.at(i)->getRadialPos();
                
                int binx, biny;
                //is the receiver cylindrical? if so, adjust the aimpoint relative to the 'unwrapped' cylinder
                if(rgeom<3)
                {
                    //calculate aimpoint azimuth
                    double aimaz = atan2(aimc.x, aimc.y);
                    aimc.x = (aimaz+PI)*180./PI;
                    aimc.y = aimc.z + rheight/2.;    
                    aimc.z = 0.;
                    //now the coordinates are in the XY plane
                    binx = (int)(floor(max(0.,min(0.9999,(PI + aimaz)/(2.*PI)))*fnx));
                }
                else
                {
                    //Transform the aim point into receiver coordinates
                    Toolbox::rotation(PI-raz, 2, aimc);
                    Toolbox::rotation(PI/2-rel, 0, aimc);
                    //Adjust to center the data on the receiver
                    aimc.Add(rdiam/2., rheight/2., 0.);
                    binx = fnx - 1 - (int)(floor(aimc.x/rdiam*fnx));
                }
                biny = (int)(floor(max(0.,min(0.9999,aimc.y/rheight))*fny));

                //Add data to array
                bins.at(binx).at(biny) ++;
                lens.at(binx).at(biny) += radialpos;
            }

            double dx, xlow;
            if(rgeom<3)
            {
                dx = 360./(double)(fnx);
                xlow = dx/2.;
            }
            else
            {
                dx = rdiam/(double)(fnx);
                xlow = dx/2.;
            }
            double
                dy = rheight/(double)(fny),
                ylow = dy/2.;
            //Create data array for scatter plot
            for(int i=0; i<fnx; i++)
            {
                double xp = i*dx + xlow;
                for(int j=0; j<fny; j++)
                {
                    double yp = j*dy + ylow;
                    
                    if(bins.at(i).at(j) > 0)
                    {
                        vector<double> dat;
                        dat.push_back(xp);
                        dat.push_back(yp);
                        dat.push_back(lens.at(i).at(j)/(double)bins.at(i).at(j));
                        dat.push_back((double)bins.at(i).at(j));

                        fdata.push_back(dat);
                    }
                }
            }
                        

            PlotScatter newplt;
            newplt.Plot(_plotobj, fdata);
        }
        
        
        break;

    }
    case Receiver::REC_GEOM_TYPE::PLANE_ELLIPSE:
        //TODO
        break;
    case Receiver::REC_GEOM_TYPE::POLYGON_OPEN:
    case Receiver::REC_GEOM_TYPE::POLYGON_CAV:
    {

    //5    |    Discrete closed N-polygon - external    
    //6    |    Discrete open N-polygon - external
    //7    |    Discrete open N-polygon - internal cavity
    
        /*
        double xpos0 = xaxmin;
        for(int i=0; i<nfs; i++)
        {
            double
                sh = fs->at(i).getSurfaceHeight(),
                sw = fs->at(i).getSurfaceWidth();

            double
                X[] = {xpos0, xpos0+sw, xpos0+sw, xpos0},
                Y[] = {sh, sh, 0, 0};
            
            DrawScaledPolygon(_dc, ppx, ppy, o, X, Y, 4);

            xpos0 += sw;
        }*/
        break;
    }
    default:
        break;
    }    


    _dc.SelectObject( wxNullBitmap );
    _pdc.DrawBitmap( *_plotobj.GetBitmap(), 0, 0);

}

void FluxPlot::SetFontSize(int hpixel)
{
	_plotobj.SetFontSize(hpixel);
}

void FluxPlot::SetResolutionMultiplier(int resmult)
{
	_plotobj.SetResolutionMultiplier(resmult);
}

int FluxPlot::GetFontSize()
{
	return _plotobj.GetFontSize();
}

wxBitmap *FluxPlot::GetBitmap()
{
	return _plotobj.GetBitmap();
}

int FluxPlot::GetResolution()
{
	return _plotobj.GetResolutionMultiplier();
}

void FluxPlot::SetWhichReceiver(int rec)
{
	_receiver = rec;
}

void FluxPlot::SetPlotType(int type)
{
	_plot_type = type;
}

void FluxPlot::SetColormap(int cmap)
{
	_plotobj.SetColormap(cmap);
};

//**********EVENTS***********************
BEGIN_EVENT_TABLE(FluxPlot, wxScrolledWindow)
    EVT_PAINT(FluxPlot::OnPaint)
    EVT_ERASE_BACKGROUND(FluxPlot::OnEraseBackground)
END_EVENT_TABLE()