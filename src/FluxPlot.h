#ifndef _FLUXPLOT_
#define _FLUXPLOT_ 1

#include <wx/wx.h>
#include <wx/dc.h>
#include <wx/dcgraph.h>
#include <wx/dcbuffer.h>
#include "SolarField.h"
#include "plot_base.h"
#include "plot_contourf.h"
#include "plot_scatter.h"


class FluxPlot : public wxScrolledWindow /*, public mod_base*/
{
private:
	int _receiver;
	int _colormap;
	SolarField *_SF;	//Pointer to the solar field object
	PlotBase _plotobj; 
	int _plot_type;		//0=flux, 1=scatter
	Hvector _helios;
	wxBitmap _pbit; //bitmap containing the current plot
	double _zax_max, _zax_min;
    bool _zax_autoscale;

	//PlotScatter _plotscatter;
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
	//void DrawScaledEllipse( wxDC &dc, double ppx, double ppy, double origin[2], double x, double y, double width, double height);
	void SetFontSize(int hpixel){_plotobj.SetFontSize(hpixel);}
	void SetResolutionMultiplier(int resmult){_plotobj.SetResolutionMultiplier(resmult);}
	int GetFontSize(){return _plotobj.GetFontSize();}
	wxBitmap *GetBitmap(){return _plotobj.GetBitmap();}
	bool SaveDataTable(wxString &path, wxString &delim);
	int GetResolution(){return _plotobj.GetResolutionMultiplier();}
	void SetWhichReceiver(int rec){_receiver = rec;}
	void SetPlotType(int type){_plot_type = type;}
	void SetColormap(int cmap){_plotobj.SetColormap(cmap);};
    void SetZRange(double zmin, double zmax, bool is_autoscale); 

protected:
	void OnPaint(wxPaintEvent &event);
	void OnEraseBackground( wxEraseEvent &WXUNUSED(event)){}
	
};




#endif