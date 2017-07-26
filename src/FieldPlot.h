#ifndef _FIELDPLOT_
#define _FIELDPLOT_ 1

#include <wx/wx.h>
#include <wx/dc.h>
#include <wx/dcgraph.h>
#include <wx/dcbuffer.h>
#include "SolarField.h"

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
	POWER,
	RANK,
	RECEIVER,
	MESH,
	EFF_CLOUD
};


class FieldPlot : public wxScrolledWindow
{
private:
	bool _is_data_ready;
    bool _is_data_visible;
	SolarField *_SF;	//Pointer to the solar field object
	int _option;	//Plot option
	int _fontsize;	//Vertical font size in pixels
	int _ppi;	//pixels per inch
	int _ppiold;
	bool _reset_ppi;
	wxBitmap _pbit; //bitmap containing the current plot
	double _zoom_fact;  //zoom factor
    int _origin_offset[2];  //offset in pixels for origin
    int _zoom_rectangle[4]; //x0,y0,x1,y1
    bool _is_zoom_rectangle;
    std::vector< std::string > _plot_choices;
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

    std::vector< std::string > GetPlotChoices();
    bool IsDataReady();

    double GetPlotZoomFact(){return _zoom_fact;}
	int GetFontSize(){return _fontsize;}
    int *GetOriginOffset(){return _origin_offset;};
	void SetPPI(int ppi);
	int GetPPI();
	void ResetPPIOnPaintEvent(int oldppi = -1);
	wxBitmap *GetBitmap();
	void DoPaint(wxDC &_pdc);
    
protected:
	void OnPaint(wxPaintEvent &event);
	void OnEraseBackground( wxEraseEvent &WXUNUSED(event)){}
	void ColorGradientHotCold(wxColour &col, double index);
	double calcScale(double span, int segments);
};




#endif