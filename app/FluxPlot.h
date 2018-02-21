#ifndef _FLUXPLOT_
#define _FLUXPLOT_ 1

#include <wx/wx.h>
#include <wx/dc.h>
#include "SolarField.h"
#include "plot_base.h"


class FluxPlot : public wxScrolledWindow 
{
private:
    int _receiver;
    int _colormap;
    SolarField *_SF;    //Pointer to the solar field object
    PlotBase _plotobj; 
    int _plot_type;        //0=flux, 1=scatter
    Hvector _helios;
    wxBitmap _pbit; //bitmap containing the current plot
    double _zax_max, _zax_min;
    bool _zax_autoscale;

    DECLARE_EVENT_TABLE()


public:
    FluxPlot( wxPanel *parent, 
               SolarField &SF,
               Hvector heliostats,
               int receiver = 0,
               const wxWindowID id = wxID_ANY, 
               const wxPoint pos = wxDefaultPosition, 
               const wxSize size = wxDefaultSize,
               long style = wxHSCROLL | wxVSCROLL| wxFULL_REPAINT_ON_RESIZE | wxBG_STYLE_CUSTOM);

    void SetPlotData(SolarField &SF, Hvector heliostats, int receiver);
    void DoPaint(wxDC &_pdc);
	void SetFontSize(int hpixel);
	void SetResolutionMultiplier(int resmult);
	int GetFontSize();
	wxBitmap *GetBitmap();
    bool SaveDataTable(wxString &path, wxString &delim);
	int GetResolution();
	void SetWhichReceiver(int rec);
	void SetPlotType(int type);
	void SetColormap(int cmap);
    void SetZRange(double zmin, double zmax, bool is_autoscale); 

protected:
    void OnPaint(wxPaintEvent &event);
	void OnEraseBackground(wxEraseEvent &WXUNUSED(event));
        
};




#endif