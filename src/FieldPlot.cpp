#include "FieldPlot.h"
#include "gui_util.h"
#include "OpticalMesh.h"
#include "SolarField.h"
#include <algorithm>

void FieldPlot::SetPlotOption(int option){ _option = option; }
void FieldPlot::SetFontSize(int hpixel){_fontsize = hpixel;}
int FieldPlot::GetPPI(){return _ppi;}
wxBitmap *FieldPlot::GetBitmap(){return &_pbit;}
	

FieldPlot::FieldPlot(wxPanel *parent, SolarField &SF, const int plot_option, 
					 const wxWindowID id, const wxPoint pos, const wxSize size, 
					 long style)
        : wxScrolledWindow(parent, id, pos, size, style){
	_SF = &SF;	//Point to the solar field object
	_option = plot_option;
	_is_data_ready = false;	//on initialization, the data for the solar field hasn't been prepared
	_fontsize = 8;
	_ppi = 96;
	_ppiold = 96;
    _zoom_fact = 0.95;
	SetBackgroundStyle(wxBG_STYLE_CUSTOM);
    _origin_offset[0] = 0;
    _origin_offset[1] = 0;
    _is_data_visible = false;
    _is_zoom_rectangle = false;

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

void FieldPlot::SetOriginOffset(int *xy)
{
    _origin_offset[0] = xy[0];
    _origin_offset[1] = xy[1];
}

void FieldPlot::AddToOriginOffset(int *xy)
{
    _origin_offset[0] += xy[0];
    _origin_offset[1] += xy[1];
}

void FieldPlot::SetDataVisible(bool is_visible)
{
    _is_data_visible = is_visible;
}

void FieldPlot::SetZoomRectangle(int *xyxy)
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

void FieldPlot::SetPlotData(SolarField &SF, int plot_option){
	_SF = &SF;
	_option = plot_option;
	_is_data_ready = plot_option != 0;
}

void FieldPlot::SetPPI(int ppi){
	_fontsize = (int)(_fontsize * ((float)ppi/(float)_ppi));
	_ppi = ppi;
}

void FieldPlot::ResetPPIOnPaintEvent(int oldppi){
	_reset_ppi = true;
	if( oldppi > 0){
		_ppiold = oldppi;
	}
}

void FieldPlot::OnPaint(wxPaintEvent &event){
	//Use a memory DC with a bitmap, and catch the wxEraseBackground event to prevent flicker
	 
	wxPaintDC _pdc(this);
	DoPaint(_pdc);
	event.Skip();
}

void FieldPlot::DoPaint(wxDC &_pdc){
	//wxGCDC gdc(_pdc);
	wxMemoryDC _dc;
	
	double ppimult = (double)_ppi/(double)_ppiold;
	int ssize_w = (int)this->GetSize().GetWidth()*ppimult;
	int ssize_h = (int)this->GetSize().GetHeight()*ppimult;

    //validation
    ssize_w = ssize_w < 100 ? 1024 : ssize_w;
    ssize_h = ssize_h < 100 ? 711 : ssize_h;

	_pbit.Create(ssize_w, ssize_h);
	_dc.SelectObject( _pbit );
	
	wxGCDC gdc(_dc);

	/*
	wxAutoBufferedPaintDC pdc(this);
	wxGCDC _dc(pdc);
	PrepareDC(_dc);
	*/
	
	//Draw the bounding box
	_dc.SetPen( *wxWHITE_PEN );
	_dc.SetBrush( *wxWHITE_BRUSH );
	wxRect windowRect(wxPoint(0,0), wxSize(ssize_w, ssize_h)); //GetClientSize());
	_dc.DrawRectangle( windowRect );
	
	//_dc.SetAxisOrientation(true, true);
	
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

	if(_option == FIELD_PLOT::LAND || !_is_data_ready){	//Land boundaries
		
		//Create polygons of the land inclusions and exclusions

		double pbounds[2];
        Land::getExtents( *V, pbounds );
		double fieldsize[] = {pbounds[1]*2., pbounds[1]*2.};
		
		//we need 40pix on each side for x label, y label, gradient bar
		_dc.SetFont( wxFont(_fontsize, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL) );
		string ets = my_to_string(-int(pbounds[1]));	//create a string that is approximately the largest extent on the plot
		wxSize etss = _dc.GetTextExtent( ets );
		int 
			left_buffer = 15*ppimult+etss.GetWidth()+etss.GetHeight(),
			right_buffer = 40*ppimult+etss.GetWidth(),
			top_buffer = 0,
			bottom_buffer = etss.GetHeight()*2+15*ppimult;
		
		//Origin location - relative to the canvas
		double o[2]; 
		o[0] = left_buffer + (canvsize[0]-left_buffer-right_buffer)/2.;
		o[1] = top_buffer+(canvsize[1]-top_buffer-bottom_buffer)/2.;	//origin
		
        o[0] += _origin_offset[0];
        o[1] += _origin_offset[1];

		double plotsize[2];
		plotsize[0] = canvsize[0] - (left_buffer+right_buffer); 
		plotsize[1] = canvsize[1] - (top_buffer+bottom_buffer);

		//Determine the pixels per meter 
		double ppm = fmin(plotsize[0]/fieldsize[0]*plot_scale, plotsize[1]/fieldsize[1]*plot_scale);

		//get the radial extents
		double radvals[2];	//[min, max]
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
		if(radvals[0] > 0.){
			if(fabs(sfangles[0]) == PI && fabs(sfangles[1]) == PI){
				_dc.DrawCircle(o[0], o[1], radvals[1]*ppm);
			}
			else{
				//DrawArc draws from 0 to 1, counterclockwise
				double
					x0 = radvals[1]*sin(sfangles[0])*ppm + o[0],
					y0 = -radvals[1]*cos(sfangles[0])*ppm + o[1],
					x1 = radvals[1]*sin(sfangles[1])*ppm + o[0],
					y1 = -radvals[1]*cos(sfangles[1])*ppm + o[1];

				_dc.DrawArc(x0,y0,x1,y1,o[0],o[1]);
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
		for(int i=0; i<ninc_polys; i++){
			int ninc = incs->at(i).size();

			double 
				*xincs = new double[ninc],
				*yincs = new double[ninc];

			for(int j=0; j<ninc; j++){
				xincs[j] = incs->at(i).at(j).x - _SF->getVarMap()->land.tower_offset_x.val;
				yincs[j] = incs->at(i).at(j).y - _SF->getVarMap()->land.tower_offset_y.val;
			}
			
			DrawScaledPolygon(_dc, ppm, o, xincs, yincs, ninc);

			delete[] xincs;
			delete[] yincs;
		}

		_dc.SetBrush( wxBrush( white ) );
		//Draw the exclusions
		for(int i=0; i<nexc_polys; i++){
			int nexc = excs->at(i).size();

			double
				*xexcs = new double[nexc],
				*yexcs = new double[nexc];

			for(int j=0; j<nexc; j++){
				xexcs[j] = excs->at(i).at(j).x;
				yexcs[j] = excs->at(i).at(j).y;
			}

			DrawScaledPolygon(_dc, ppm, o, xexcs, yexcs, nexc);
		}

		//exclude the center region
		_dc.SetBrush( wxBrush( white ) );
		if(radvals[1] > 0.){
			_dc.DrawEllipse(o[0]-radvals[0]*ppm, o[1]-radvals[0]*ppm, radvals[0]*ppm*2., radvals[0]*ppm*2.);
		}


        //Draw the clipping rectangles
        _dc.SetPen( wxPen( white, 1, wxPENSTYLE_TRANSPARENT ) );
        _dc.SetBrush( *wxWHITE_BRUSH );

        _dc.DrawRectangle(0, 0, left_buffer, canvsize[1]);
        _dc.DrawRectangle(0, plotsize[1], canvsize[0]-left_buffer, bottom_buffer);
        _dc.DrawRectangle(canvsize[0]-right_buffer, 0, right_buffer, canvsize[1]);
        _dc.DrawRectangle(0, 0, canvsize[0], 1);

		//Draw the plot boundary
		_dc.SetPen( wxPen( black, 1*ppimult, wxPENSTYLE_SOLID) );
        _dc.SetBrush( *wxTRANSPARENT_BRUSH );
		_dc.DrawRectangle( left_buffer, 1*ppimult+top_buffer, plotsize[0], plotsize[1]);
		
		//Draw the x axis
		int x_axis_loc = top_buffer+plotsize[1];
		_dc.SetPen( wxPen( black, 1*ppimult, wxPENSTYLE_SOLID) );
		_dc.DrawLine(o[0], x_axis_loc, o[0], x_axis_loc + 10*ppimult);
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
		while(xtickloc < fieldsize[0]/2.){
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
		_dc.DrawLine(left_buffer-10*ppimult, o[1], left_buffer, o[1]);
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
		while(ytickloc < fieldsize[1]/2. ){
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
		
		// --- was here
		
		//Draw the x and y axis lines
		_dc.SetPen( wxPen( black, 1*ppimult, wxPENSTYLE_DOT ) );
		_dc.DrawLine(left_buffer, o[1], canvsize[0]-right_buffer, o[1]);	//x line
		_dc.DrawLine(o[0], top_buffer, o[0], x_axis_loc);	//y line


		//Draw the origin
		_dc.SetPen( wxPen( wxT("black"), 1*ppimult, wxPENSTYLE_SOLID) );
		_dc.SetBrush( *wxRED_BRUSH );
		_dc.DrawCircle( o[0], o[1], 5*ppimult);
		
	}
	else if(_option == FIELD_PLOT::MESH){

		vector<opt_element*> mesh = _SF->getOpticalHashTree()->get_terminal_nodes();

		//if the mesh has no data, quit here and just create a plot of the available land
		if( mesh.size() == 0){
			SetPlotOption(FIELD_PLOT::LAND);
			//event.Skip();
			wxPaintEvent evt(this->GetId());
			ProcessEvent( evt );
			return;
		}

		//double pi=acos(-1.);
		
		//Plot the zones of the optical mesh
		
		double pbounds[2];
        Land::getExtents( *V, pbounds );
		double fieldsize[2];
		fieldsize[0] = pbounds[1]*2.;
		fieldsize[1] = pbounds[1]*2.;
		
		//we need 40pix on each side for x label, y label, gradient bar
		_dc.SetFont( wxFont(_fontsize, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL) );
		string ets = my_to_string(-int(pbounds[1]));	//create a string that is approximately the largest extent on the plot
		wxSize etss = _dc.GetTextExtent( ets );
		int 
			left_buffer = 15+etss.GetWidth()+etss.GetHeight(),
			right_buffer = 40+etss.GetWidth(),
			top_buffer = 0,
			bottom_buffer = etss.GetHeight()*2+15*ppimult;
		
		//Origin location - relative to the canvas
		double o[2];
		o[0] = left_buffer + (canvsize[0]-left_buffer-right_buffer)/2.;
		o[1] = top_buffer+(canvsize[1]-top_buffer-bottom_buffer)/2.;	//origin
		
        o[0] += _origin_offset[0];
        o[1] += _origin_offset[1];

		double plotsize[2];
		plotsize[0] = canvsize[0] - (left_buffer+right_buffer);
		plotsize[1] = canvsize[1] - (top_buffer+bottom_buffer);

		//Determine the pixels per meter 
		double ppm = min(plotsize[0]/fieldsize[0]*plot_scale, plotsize[1]/fieldsize[1]*plot_scale);

		//get the radial extents
		double radvals[2];	//[min, max]
        Land::getRadialExtents(*V, radvals, V->sf.tht.val);
		
        //Draw the clipping rectangles
        _dc.SetPen( wxPen( white, 1, wxPENSTYLE_TRANSPARENT ) );
        _dc.SetBrush( *wxWHITE_BRUSH );

        _dc.DrawRectangle(0, 0, left_buffer, canvsize[1]);
        _dc.DrawRectangle(0, plotsize[1], canvsize[0]-left_buffer, bottom_buffer);
        _dc.DrawRectangle(canvsize[0]-right_buffer, 0, right_buffer, canvsize[1]);
        _dc.DrawRectangle(0, 0, canvsize[0], 1);

		//Draw the plot boundary
		_dc.SetPen( wxPen( black, 1*ppimult, wxPENSTYLE_SOLID) );
		_dc.DrawRectangle( left_buffer, 1*ppimult+top_buffer, plotsize[0], plotsize[1]);
		
		//Draw the x axis
		int x_axis_loc = top_buffer+plotsize[1];
		_dc.SetPen( wxPen( black, 1*ppimult, wxPENSTYLE_SOLID) );
		_dc.DrawLine(o[0], x_axis_loc, o[0], x_axis_loc + 10*ppimult);
		ets = "0";
		etss = _dc.GetTextExtent( ets );
		_dc.DrawText(ets, o[0]-etss.GetWidth()/2, x_axis_loc + 9*ppimult);
		ets = "Field position (East+) [m]";
		etss = _dc.GetTextExtent( ets );
		_dc.DrawText(ets, o[0] - etss.GetWidth()/2, x_axis_loc + (15*ppimult+etss.GetHeight()));
		
		//estimate the number of divisions
		ets = my_to_string( int(-fieldsize[0]/2.));
		etss = _dc.GetTextExtent( ets );
		int ndiv = min(int(plotsize[0]*_zoom_fact/ (etss.GetWidth()*1.5 )), (int)(_zoom_fact*20));
		double xscale = calcScale(fieldsize[0], ndiv);
		double 
			xtickloc = xscale,
			xtickloc_ppm = xscale*ppm;
		while(xtickloc < fieldsize[0]/2.){
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
		_dc.DrawLine(left_buffer-10*ppimult, o[1], left_buffer, o[1]);
		string etsy = "0"; wxSize etssy = _dc.GetTextExtent( etsy );
		_dc.DrawText(etsy, left_buffer-(etssy.GetWidth()+10*ppimult), o[1]-etssy.GetHeight()/2);
		
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
		while(ytickloc < fieldsize[1]/2. ){
			//To the bottom
			_dc.DrawLine(left_buffer-7*ppimult, o[1]+ytickloc_ppm, left_buffer, o[1]+ytickloc_ppm);
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
		
		//Draw the elements
		_dc.SetPen( wxPen( black, 1*ppimult, wxPENSTYLE_SOLID ) );
		_dc.SetBrush( wxBrush( white, wxBRUSHSTYLE_TRANSPARENT ));
		
		//_dc.SetFont( wxFont(6, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL) );
		//wxString gt;

		for(int i=0; i<(int)mesh.size(); i++){
			double *rr = mesh.at(i)->get_xr();
			double *azr = mesh.at(i)->get_yr();
			double azr0 = azr[0]; //*pi/180.;
			double azr1 = azr[1]; //*pi/180.;
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

			/*gt = wxString::Format("%d",mesh.at(i)->get_array()->size());
			wxSize gte = _dc.GetTextExtent( gt );
			double rave = (rr[0]+rr[1])*.5;
			double tx = rave*sin((azr1+azr0)*.5)*ppm + o[0] - gte.GetWidth()/2;
			double ty = -rave*cos((azr1+azr0)*.5)*ppm + o[1] - gte.GetHeight()/2;
			_dc.DrawText(gt, tx, ty);*/
		}
		//_dc.SetFont( wxFont(_fontsize, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL) );
		
		
		//Draw the x and y axis lines
		_dc.SetPen( wxPen( black, 1*ppimult, wxPENSTYLE_DOT ) );
		_dc.DrawLine(left_buffer, o[1], canvsize[0]-right_buffer, o[1]);	//x line
		_dc.DrawLine(o[0], top_buffer, o[0], x_axis_loc);	//y line


		//Draw the origin
		_dc.SetPen( wxPen( wxT("black"), 1, wxPENSTYLE_SOLID) );
		_dc.SetBrush( *wxRED_BRUSH );
		_dc.DrawCircle( o[0], o[1], 5*ppimult);




	}
	else if(_option != FIELD_PLOT::LAND && _option != FIELD_PLOT::MESH){	//Field layout geometry
		//Get the plot extents size = [4], {xmax, xmin, ymax, ymin}
		double 
			*extents = _SF->getPlotBounds(),
			xsize = extents[0] - extents[1],	//Physical size of the plotted area in the X direction
			ysize = extents[2] - extents[3];	//Physical size of the plotted area in the Y direction
		double fieldsize[] = {xsize, ysize};
		if(xsize == 0 || ysize == 0) return;	//No data
		
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

		if(_option != FIELD_PLOT::LAYOUT){
			for(int i=0; i<npos; i++){
				H = heliostats->at(i);
				//Get the value of interest for the heliostat
				if(_option == FIELD_PLOT::EFF_TOT){	//Total efficiency
					plot_vals[i] = H->getEfficiencyTotal();
				}
				else if(_option == FIELD_PLOT::EFF_COS){	//Cosine efficiency
					plot_vals[i] = H->getEfficiencyCosine();
				}
				else if(_option == FIELD_PLOT::EFF_ATT){	//Attenuation efficiency
					plot_vals[i] = H->getEfficiencyAtten();
				}
				else if(_option == FIELD_PLOT::EFF_BLOCK){	//Blocking efficiency
					plot_vals[i] = H->getEfficiencyBlock();
				}
				else if(_option == FIELD_PLOT::EFF_SHAD){	//Shading efficiency
					plot_vals[i] = H->getEfficiencyShading();
				}
				else if(_option == FIELD_PLOT::EFF_INT){	//Image intercept efficiency
					plot_vals[i] = H->getEfficiencyIntercept();
				}
                else if(_option == FIELD_PLOT::EFF_REFLECT){  //Heliostat reflectivity
                    plot_vals[i] = H->getTotalReflectivity();
                }
				else if(_option == FIELD_PLOT::POWER){	// Delivered power
					plot_vals[i] = H->getPowerToReceiver();
				}
				else if(_option == FIELD_PLOT::RANK){  //Color by layout ranking metric
					plot_vals[i] = H->getRankingMetricValue();
				}
				else if(_option == FIELD_PLOT::RECEIVER){  //color by mapped receiver
					int r=0;
					for(r=0; r<(int)_SF->getReceivers()->size(); r++){
						if(H->getWhichReceiver() == _SF->getReceivers()->at(r)) break;
					}
					plot_vals[i] = (double)(r+1);
				}
				else if(_option == FIELD_PLOT::EFF_CLOUD){ //Cloudiness efficiency
					plot_vals[i] = H->getEfficiencyCloudiness();
				}
				if(plot_vals[i] < valmin) valmin = plot_vals[i];
				if(plot_vals[i] > valmax) valmax = plot_vals[i];
				valave += plot_vals[i];
			}
			valave *= 1./double(npos);
		}

		//Specify the plot region size
		_dc.SetFont( wxFont(_fontsize, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL) );
		string ets = my_to_string(-int(xsize));	//create a string that is approximately the largest extent on the plot
		wxSize etss = _dc.GetTextExtent( ets );
		string maxlab, avelab, minlab;	//Create the gradient bar labels
		wxString labfmt;
		double vmult;
		vmult = (_option==FIELD_PLOT::POWER || _option == FIELD_PLOT::RANK || _option==FIELD_PLOT::RECEIVER) ? 1. : 100.;
		string vunits;
		if(_option==FIELD_PLOT::POWER){
			vunits = "[kWt]";
		}
		else if(_option == FIELD_PLOT::RECEIVER || _option == FIELD_PLOT::RANK){
			vunits = "";
		}
		else{
			vunits = "%";
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
		if( _dc.GetTextExtent(avelab).GetWidth() > glab_x ){
			glabs = _dc.GetTextExtent(avelab);
			glab_x = glabs.GetWidth();
		}
		if( _dc.GetTextExtent(minlab).GetWidth() > glab_x){
			glabs = _dc.GetTextExtent(minlab);
			glab_x = glabs.GetWidth();
		}
		if(_option ==FIELD_PLOT::LAYOUT ){
			//layout only, no bar
			glabs.Set(1,1);
			glab_x = 1;
		}


		int 
			left_buffer = 15*ppimult+etss.GetWidth()+etss.GetHeight(),
			right_buffer = 25*ppimult+glab_x+2*ppimult,	//gradient bar width is 20
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

		//Draw the x axis
		int x_axis_loc = top_buffer+plotsize[1];

		//Only plot shadows for rectangular heliostats right now...
		if( heliostats->at(0)->getVarMap()->is_round.mapval() == var_heliostat::IS_ROUND::RECTANGULAR)
        {
			for(int i=0; i<npos; i++){

				//Get the shadow coordinates for each heliostat and plot
				H = heliostats->at(i);

                double rcoll = H->getCollisionRadius()*ppm;
                //check to see if this heliostat needs drawing
                sp_point *loc = H->getLocation();
                double xlocm = o[0] + loc->x*ppm;
                double ylocm = o[1] - loc->y*ppm;

                if((xlocm - rcoll)> canvsize[0] || (xlocm + rcoll) < 0 || (ylocm - rcoll)> canvsize[1] || (ylocm + rcoll) < 0)
                    continue;

				vector<sp_point> *shad = H->getShadowCoords();
				int ns = shad->size();
				double
					*xs = new double[ns],
					*ys = new double[ns];
				for(int j=0; j<ns; j++){
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

		//Handle round and rectangular heliostats differently
		if( heliostats->at(0)->getVarMap()->is_round.mapval() == var_heliostat::IS_ROUND::ROUND)
        {
			double Hw = heliostats->at(0)->getVarMap()->width.val;
			for(int i=0; i<npos; i++){
				//Get the location and size of each heliostat
				H = heliostats->at(i);

				double
					x = H->getLocation()->x,
					y = H->getLocation()->y;
				if(_option == FIELD_PLOT::LAYOUT){
					_dc.SetPen( wxPen( black, 1, wxPENSTYLE_SOLID) );
					_dc.SetBrush( wxBrush( helio_region, wxBRUSHSTYLE_SOLID) );
				}
				else{
					wxColour gc;
					ColorGradientHotCold( gc, (plot_vals[i]-valmin)/(valmax - valmin) );
					_dc.SetPen( wxPen( gc, 1, wxPENSTYLE_SOLID) );
					_dc.SetBrush( wxBrush( gc, wxBRUSHSTYLE_SOLID ) );
				}
				DrawScaledEllipse(_dc, ppm, o, x, y, Hw);

			}
		}
		else{	//rectangular
            //int iexc =0;
			for(int i=0; i<npos; i++){
				H = heliostats->at(i);
                double rcoll = H->getCollisionRadius()*ppm;
                //check to see if this heliostat needs drawing
                sp_point *loc = H->getLocation();
                double xlocm = o[0] + loc->x*ppm;
                double ylocm = o[1] - loc->y*ppm;

                if((xlocm - rcoll)> canvsize[0] || (xlocm + rcoll) < 0 || (ylocm - rcoll)> canvsize[1] || (ylocm + rcoll) < 0)
                    continue;

				//get the corner coordinates for each heliostat and plot
				vector<sp_point> *corners = H->getCornerCoords();
				int nc = corners->size();
				double 
					*xc = new double[nc],
					*yc = new double[nc];

				for(int j=0; j<nc; j++){
					xc[j] = corners->at(j).x;
					yc[j] = corners->at(j).y;
				}

				//Plot
				//_dc.SetPen( wxPen( helio_border, 1, wxSOLID) );
				if(_option == FIELD_PLOT::LAYOUT){
					_dc.SetPen( wxPen( black, 1, wxPENSTYLE_SOLID) );
					_dc.SetBrush( wxBrush( helio_region, wxBRUSHSTYLE_SOLID) );
				}
				else{
					wxColour gc;
					ColorGradientHotCold( gc, (plot_vals[i]-valmin)/(valmax - valmin) );
					_dc.SetPen( wxPen( gc, 1, wxPENSTYLE_SOLID) );
					_dc.SetBrush( wxBrush( gc, wxBRUSHSTYLE_SOLID ) );
				}
				DrawScaledPolygon(_dc, ppm, o, xc, yc, nc);

                //if data is visible and sizes are okay, show text
                if( _is_data_visible && rcoll > 7. && _option != FIELD_PLOT::LAYOUT)
                {
                    wxString labval = to_string(100.*plot_vals[i], labfmt.c_str());
                    wxSize ext = _dc.GetTextExtent(labval);

                    _dc.SetTextForeground( *wxLIGHT_GREY );
                    _dc.DrawText( labval, xlocm-ext.GetWidth()/2, ylocm-ext.GetHeight()/2 );
                }

				delete [] xc;
				delete [] yc;
			}
		}
        _dc.SetTextForeground( *wxBLACK );
        //Draw the clipping rectangles
        _dc.SetPen( wxPen( white, 1, wxPENSTYLE_TRANSPARENT) );
        _dc.SetBrush( *wxWHITE_BRUSH );

        _dc.DrawRectangle(0, 0, left_buffer, canvsize[1]);
        _dc.DrawRectangle(0, plotsize[1], canvsize[0]-left_buffer, bottom_buffer);
        _dc.DrawRectangle(canvsize[0]-right_buffer, 0, right_buffer, canvsize[1]);
        _dc.DrawRectangle(0, 0, canvsize[0], 1);

        
        //Draw the gradient bar
		if(_option > 1)
			_dc.GradientFillLinear( wxRect(wxPoint(canvsize[0]-right_buffer+5*ppimult, 50*ppimult), wxPoint(canvsize[0]-right_buffer+25*ppimult, x_axis_loc-50*ppimult)), grad_low, grad_high, wxNORTH);
		double lineave;
		if( fabs(valmax-valmin) > 1.e-6 )	
			lineave = 50*ppimult+(plotsize[1]-100.)*(1.-(valave-valmin)/(valmax-valmin));	//where should the average line indicator be in Y?
		else
			lineave = 50*ppimult+(plotsize[1]-100.)*.5;
		//Draw the average line
		_dc.SetPen( wxPen( white, 1, wxPENSTYLE_SOLID) );
		_dc.DrawLine(canvsize[0]-right_buffer+5*ppimult, lineave, canvsize[0]-right_buffer+25*ppimult, lineave);
		//Label the gradient bar
		if(_option != FIELD_PLOT::LAYOUT){
			_dc.DrawText( maxlab, canvsize[0]-right_buffer+2*ppimult, 51*ppimult-glabs.GetHeight() );
			_dc.DrawText( minlab, canvsize[0]-right_buffer+2*ppimult, x_axis_loc-48*ppimult);
			_dc.DrawText( avelab, canvsize[0]-right_buffer+27*ppimult, lineave-glabs.GetHeight()/2 );
		}
		delete [] plot_vals;

		
		//Draw the plot boundary
		_dc.SetPen( wxPen( black, 1*ppimult, wxPENSTYLE_SOLID) );
		_dc.SetBrush( *wxTRANSPARENT_BRUSH );
		_dc.DrawRectangle( left_buffer, 1*ppimult+top_buffer, plotsize[0], plotsize[1]);

		//Draw the x axis
		//int x_axis_loc = top_buffer+plotsize[1];
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
        
		while(true){ //xtickloc < fieldsize[0]/2.){
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
        
        while(true){ //xtickloc < fieldsize[0]/2.){
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

		while(true){    //ytickloc < max(fabs(extents[2]),fabs(extents[3])) ){
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
        while(true){    //ytickloc < max(fabs(extents[2]),fabs(extents[3])) ){
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
	if(_reset_ppi){
		_fontsize = (int)(_fontsize * (double)_ppiold/(double)_ppi);
		_ppi = _ppiold;
		_reset_ppi = false;
	}
	//event.Skip();

}

void FieldPlot::DrawScaledPolygon( wxDC &dc, double ppm, double origin[2], double x[], double y[], int n_points){
	/*
	Take a polygon with n_points values (not closed) and indices of x[] and y[] of length n_points,
	and draw the polygon on the device. The X,Y coordinates will be scaled according to the maximum 
	extents w and h.

	ppm			Pixels per meter of drawing
	origin[] {x-pos, y-pos} Location in pixels of the plot origin

	Note that the DC coordinates are (0,0) at the upper left corner, positive X to the right, 
	and negative Y downward.
	
	*/
	
	//Create an array of points of the items
	wxPoint *points;
	points = new wxPoint[n_points];

	for(int i=0; i<n_points; i++){
		points[i].x = x[i]*ppm + origin[0];
		points[i].y = -y[i]*ppm + origin[1];
	}

	dc.DrawPolygon(n_points, points);

	delete [] points;
}

void FieldPlot::DrawScaledEllipse( wxDC &dc, double ppm, double origin[2], double x, double y, double diam){
	wxCoord
		xs, ys, ws, hs;
	xs = int(x*ppm + origin[0]);
	ys = int(y*ppm + origin[1]);
	ws = int(diam*ppm);
	hs = ws;

	dc.DrawEllipse(xs, ys, ws, hs);
}

void FieldPlot::ColorGradientHotCold(wxColour &col, double index){

	/* 
	Index from 0-1 indicating the position on the scale from COLD to HOT

	Returns value indicating RRRGGGBBB on a scale of 0-255 for each integer triplet
	*/
	double dind = index;
	if(index<0.) dind = 0.;
	if(index>1.) dind = 1.;
	
	col.Set(int(dind*255.), 0, int((1.-dind)*255.));
	
}

double FieldPlot::calcScale(double span, int segments){
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
	
	if(id==0){
		//round up
		return ceil(basis)*pflog;
	}
	else if(id==1){
		//round to the middle
		return (floor(basis)+.5)*pflog;
	}
	else{
		//round down
		return floor(basis)*pflog;
	}
	
}


//**********EVENTS***********************
BEGIN_EVENT_TABLE(FieldPlot, wxScrolledWindow)
	EVT_PAINT(FieldPlot::OnPaint)
	EVT_ERASE_BACKGROUND(FieldPlot::OnEraseBackground)
END_EVENT_TABLE()
