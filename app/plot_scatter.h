#ifndef _PLOT_SCATTER_ 
#define _PLOT_SCATTER_ 1

#include <wx/wx.h>
#include <vector>


using namespace std;

class PlotBase;

class PlotScatter
{
public:
    void SetZAxisScale(PlotBase &base, bool is_autoscale, double zax_min, double zax_max);
    bool Plot(PlotBase &base, vector<vector<double> > &data);
    bool Export(PlotBase &base, wxString filename, wxBitmapType type=wxBITMAP_TYPE_PNG);
    bool ExportDataTable(PlotBase &base, wxString &path, wxString &delim, vector<vector<double> > &data);

};


#endif