
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


#include "plot_contourf.h"
#include "gui_util.h"
#include <math.h>
#include <wx/textfile.h>
#include <algorithm>
#include <wx/dc.h>
#include <wx/dcbuffer.h>
#include <wx/dcgraph.h>
#include "plot_base.h"

using namespace std;

bool PlotContourf::Plot(PlotBase &base, vector<vector<double> > &data)
{
    
    //Use a memory DC with a bitmap, and catch the wxEraseBackground event to prevent flicker

    int pw = base._plotsize.GetWidth();
    int ph = base._plotsize.GetHeight();

    pw = pw < 100 ? 1024 : pw;
    ph = ph < 100 ? 711 : ph;

    base._bitmap.Create(pw,ph);
    wxMemoryDC _dc;
    _dc.SelectObject( base._bitmap );

    
    double maxval=-9.e9, minval=9.e9;
    for(int i=0; i<(int)data.size(); i++)
    {
        for(int j=0; j<(int)data.at(i).size(); j++)
        {
            double d = data.at(i).at(j);
            if(d > maxval) maxval = d;
            if(d < minval) minval = d;
        }
    }
    base.AxesSetup(_dc, minval, maxval);
    
    // -----------------------------------------------------
    bool has_data = true;

    _dc.SetPen( wxPen( base._gray, 1, wxPENSTYLE_SOLID ) );
    _dc.SetBrush( *wxTRANSPARENT_BRUSH );
    //Draw the boundaries of the surface
    double zvaltot=0., zvalave=0., fval;
    
    //---- Draw the intensity ----
    
    //Calculate the element size
    int 
        fnx = data.size(),
        fny = data.at(0).size();
    double
        xaxspan = base._xaxmax - base._xaxmin,
        yaxspan = base._yaxmax - base._yaxmin;
    double 
        fdx = xaxspan/double( fnx ),
        fdy = yaxspan/double( fny );
    //Calculate statistics of interest for the flux
    for(int i=0; i<fnx; i++)
    {
        for(int j=0; j<fny; j++)
        {
            fval = data.at(i).at(j);
            if(fval < minval) minval = fval;
            if(fval > maxval) maxval = fval;
            zvaltot += fval;
        }
    }
    zvalave = zvaltot / double(fnx*fny);

    //if no data is provided, return here
    if(minval == maxval && maxval==0) has_data = false;

    //Impose user-specified min and max z values
    if( base._zaxmax > base._zaxmin && !base._z_autoscale)
    {
        minval = base._zaxmin;
        maxval = base._zaxmax;
    }
    
    //Plot each node
    if(has_data)
    {
        //Construct a grid of only flux values that we can interpolate from
        vector<vector<double> > fvals(fnx+2, vector<double>(fny+2, 0.) );
        
        for(int i=0; i<fnx; i++)
        {
            for(int j=0; j<fny; j++)
            {
                fvals.at(i+1).at(j+1) = data.at(i).at(j);
            }
        }
            
        //Set the fringe values
        if(base._wrap_values)
        {    //For the closed cylinder, wrap around in X
            for(int j=1; j<fny+1; j++)
            {
                fvals.at(0).at(j) = fvals.at(fnx).at(j);
                fvals.at(fnx+1).at(j) = fvals.at(1).at(j);
            }    
        }
        else
        {
            for(int j=1; j<fny+1; j++)
            {
                fvals.at(0).at(j) = fvals.at(1).at(j);
                fvals.at(fnx+1).at(j) = fvals.at(fnx).at(j);
            }
        }

        for(int i=1; i<fnx+1; i++)
        { 
            fvals.at(i).at(0) = fvals.at(i).at(1);
            fvals.at(i).at(fny+1) = fvals.at(i).at(fny);
        }
            
        //Set the corners
        fvals.at(0).at(0) = fvals.at(0).at(1);
        fvals.at(0).at(fny+1) = fvals.at(0).at(fny);
        fvals.at(fnx+1).at(0) = fvals.at(fnx).at(0);
        fvals.at(fnx+1).at(fny+1) = fvals.at(fnx+1).at(fny);
            
        //Take the grid and create a linearly interpolated grid. This will go into the bilinear algorithm.
        vector<vector<double> > zvals(fnx+1, vector<double>(fny+1, 0.) );

        for(int i=0; i<fnx+1; i++)
        {
            for(int j=0; j<fny+1; j++)
            {
                zvals.at(i).at(j) = (fvals.at(i).at(j)+fvals.at(i+1).at(j)+fvals.at(i).at(j+1)+fvals.at(i+1).at(j+1)) * 0.25;
            }
        }
        //Calculate the size of the higher resolution interpolated zones
        double
            ddx = fdx/double(base._resmult),
            ddy = fdy/double(base._resmult);
        wxColour fcolor;
                            
        //plot the flux
        for(int i=1; i<fnx+1; i++)
        {
            for(int j=0; j<fny; j++)
            {
                
                //Get the lower and upper bounds in the X & Y directions for the
                //flux node
                double 
                    x0 = base._xaxmax - fdx*i, 
                    y0 = base._yaxmin + fdy*j,
                    z00 = zvals.at(i).at(j),
                    z01 = zvals.at(i).at(j+1),
                    z10 = zvals.at(i-1).at(j),
                    z11 = zvals.at(i-1).at(j+1),
                    Z[] = {z00, z10, z01, z11};
                for(int k=0; k<base._resmult; k++)
                {
                    double 
                        xf = (0.5 + k)*ddx/fdx,
                        xx = x0 + (0.5 + k)*ddx;
                    for(int m=0; m<base._resmult; m++)
                    {
                        double 
                            yy = y0 + (0.5 + m)*ddy,
                            yf = (0.5 + m)*ddy/fdy;
                        double zz = base.bilinearInterp(xf, yf, Z);
                        double z = (zz - minval)/(maxval - minval);
                        
                        switch (base.GetColormap())
                        {
                        case COLORMAP::JET:
                            base.ColorGradientJet(fcolor, z);
                            break;
                        case COLORMAP::GRAYSCALE:
                            base.ColorGradientGrayscale(fcolor, z);
                            break;
                        case COLORMAP::HOTCOLD:
                            base.ColorGradientHotCold(fcolor, z);
                            break;
                        case COLORMAP::PARULA:
                            base.ColorGradientParula(fcolor, z);
                        default:
                            break;
                        }
                        
                        _dc.SetPen( wxPen(fcolor, 1, wxPENSTYLE_SOLID) );
                        _dc.SetBrush( wxBrush(fcolor, wxBRUSHSTYLE_SOLID) );

                        //Construct the poly
                        double
                            xxl = xx - ddx/2.,
                            xxu = xxl + ddx,
                            yyl = yy - ddy/2.,
                            yyu = yyl + ddy,
                            XX[] = {xxl, xxu, xxu, xxl},
                            YY[] = {yyu, yyu, yyl, yyl};

                        base.DrawScaledPolygon(_dc, base._ppx, base._ppy, base._origin, XX, YY, 4);

                    }
                }
                    
            }
        }
    }
    _dc.SetPen( wxPen( base._gray, 2, wxPENSTYLE_SOLID ) );
    _dc.SetBrush( *wxTRANSPARENT_BRUSH );
    double 
        XX[] = {base._xaxmin, base._xaxmax, base._xaxmax, base._xaxmin},
        YY[] = {base._yaxmax, base._yaxmax, base._yaxmin, base._yaxmin};
    base.DrawScaledPolygon(_dc, base._ppx, base._ppy, base._origin, XX, YY, 4);

    

    //Draw the gradient bar
    if(has_data) base.DrawColorbar(_dc, minval, maxval, zvalave);

    _dc.SelectObject( wxNullBitmap );
    
    return true;
}

bool PlotContourf::Export(PlotBase &base, wxString filename, wxBitmapType type)
{
    
    wxImage image = base._bitmap.ConvertToImage();
    return image.SaveFile(filename, type);
}

bool PlotContourf::ExportDataTable(PlotBase &base, wxString &path, wxString &delim, vector<vector<double> > &data)
{
    /* 
    Write a data table with flux information to 'path' using data separated by 'delim'
    */

    //Calculate min/max/ave values
    double maxval=-9.e9, minval=9.e9;
    int ndx = (int)data.size();
    int ndy = (int)data.front().size();
    double qtot = 0.;
    for(int i=0; i<ndx; i++)
    {
        for(int j=0; j<ndy; j++)
        {
            double d = data.at(i).at(j);
            qtot += d;
            if(d > maxval) maxval = d;
            if(d < minval) minval = d;
        }
    }
    double qave = qtot/(double)(ndx*ndy);

    //Check to make sure the table has data
    if(minval == maxval)
    {
        wxMessageBox("The receiver fluxmap is empty! Could not export data.", "Export error", wxOK);
        return true;
    }
    //try opening the file
    wxTextFile fout(path);
    if(! (fout.Exists() ? fout.Open() : fout.Create()) )
    {
        return false;
    }

    fout.Clear();

    //Write header
    fout.AddLine("Flux intensity plot (units kW/m2)");
    

    //write summary stats
    fout.AddLine(wxString::Format("Max. flux%s%f%skW/m2", delim, maxval, delim));
    fout.AddLine(wxString::Format("Min. flux%s%f%skW/m2", delim, minval, delim));
    fout.AddLine(wxString::Format("Ave. flux%s%f%skW/m2", delim, qave, delim));

    //determine the best number formatting for the x axis labels
    wxString xfmt;
    int nxdec = gui_util::CalcBestSigFigs( fmax(fabs(base._xaxmax),fabs(base._xaxmin)) );
    xfmt.sprintf("%s%df", "%.", max(nxdec,0));
    
    //determine the best number formatting for the y axis labels
    wxString yfmt;
    int nydec = gui_util::CalcBestSigFigs( fmax(fabs(base._yaxmax),fabs(base._yaxmin)) );
    yfmt.sprintf("%s%df", "%.", nydec);

    //write x axis values
    fout.AddLine("");
    wxString xlab =base._xlab;
    xlab.Replace(delim, ";");
    fout.AddLine(wxString::Format("%s%s -->", delim, xlab));
    wxString ylab = base._ylab;
    ylab.Replace(delim, ";");
    wxString line = wxString::Format("%s%s", ylab, delim);
    wxString tmp;
    for(int i=0; i<ndx; i++)
        line.Append(wxString::Format(xfmt, base._xaxmax - (base._xaxmax-base._xaxmin)/(double)(ndx)*(double)i ) + delim);
    fout.AddLine(line);
    line.Clear();

    for(int j=0; j<ndy; j++)
    {
        //add y axis value - proceed top down
        line = wxString::Format(yfmt, base._yaxmax - (base._yaxmax - base._yaxmin)*(double)j/(double)(ndy) - 0.5 ) + ",";
        for(int i=0; i<ndx; i++)
        {
            line.Append( wxString::Format("%f%s", data.at(ndx - 1 - i).at(ndy - 1 - j), delim) );
        }
        fout.AddLine(line);
    }

    fout.Write();
    fout.Close();

    return true;


}
