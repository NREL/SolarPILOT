#ifndef _PLOT_CONTOURF_ 
#define _PLOT_CONTOURF_ 1

#include <wx/wx.h>
#include <vector>

class PlotBase;

class PlotContourf 
{
public:
    void SetZAxisScale(PlotBase &base, bool is_autoscale, double zax_min, double zax_max);
    bool Plot(PlotBase &base, std::vector<std::vector<double> > &data);
    bool Export(PlotBase &base, wxString filename, wxBitmapType type=wxBITMAP_TYPE_PNG);
    bool ExportDataTable(PlotBase &base, wxString &path, wxString &delim, std::vector<std::vector<double> > &data);

};


#endif