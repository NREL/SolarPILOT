
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


#include "GUI_main.h"

using namespace std;


void SPFrame::CreateFieldPlotPage(wxScrolledWindow *parent, wxArrayStr &choices, int selection)
{
    /* 
	Create the master receiver page and return a pointer
	
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
	9 | Ranking metric
	10| receiver map
	11| Optical mesh
	12| Cloudiness efficiency
	--------------------------------------------
	
	
	*/
#if _USE_WINDOW_BG 
	parent->SetBackgroundColour(_background_colour);
#endif
	wxBoxSizer *main_sizer = new wxBoxSizer(wxVERTICAL);
	
	//
	_plot_select = new wxComboBox(parent, wxID_ANY, choices[selection], wxDefaultPosition, wxSize(wxDefaultSize.GetWidth(), _default_input_size.GetHeight()), choices, wxCB_DROPDOWN|wxCB_READONLY);
	wxStaticText *select_lab = new wxStaticText(parent, wxID_ANY, wxT("Plot display"));
	_plot_select->Connect(wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( SPFrame::OnPlotSelectCombo ), NULL, this);

    wxBitmap 
        show_data_bit, show_data_bit_on;
    show_data_bit.LoadFile(_image_dir.GetPath(true) + "show-data.png", wxBITMAP_TYPE_PNG);
    show_data_bit_on.LoadFile(_image_dir.GetPath(true) + "show-data-on.png", wxBITMAP_TYPE_PNG);
    _show_field_data = new MyToggleButton(parent, wxID_ANY, show_data_bit_on, show_data_bit);
    _show_field_data->SetToolTip("Show heliostat data points in zoom mode");
    _show_field_data->SetState(false);
    _show_field_data->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnPlotDataToggle ), NULL, this);

	wxBitmap
		font_inc_bit, font_dec_bit, bitsave, file_exp_bit;
	font_inc_bit.LoadFile(_image_dir.GetPath(true) + "format-font-size-more.png", wxBITMAP_TYPE_PNG);
	font_dec_bit.LoadFile(_image_dir.GetPath(true) + "format-font-size-less.png", wxBITMAP_TYPE_PNG);
	bitsave.LoadFile(_image_dir.GetPath(true) + "save-image.png", wxBITMAP_TYPE_PNG);
	file_exp_bit.LoadFile(_image_dir.GetPath(true) + "save-table.png", wxBITMAP_TYPE_PNG);

	wxBitmapButton *font_inc = new wxBitmapButton(parent, wxID_ANY, font_inc_bit);
	font_inc->SetName(wxT("layout"));
	wxBitmapButton *font_dec = new wxBitmapButton(parent, wxID_ANY, font_dec_bit);
	font_dec->SetName(wxT("layout"));
	wxBitmapButton *save_bitmap = new wxBitmapButton(parent, wxID_ANY, bitsave);
	wxBitmapButton *file_export = new wxBitmapButton(parent, wxID_ANY, file_exp_bit);
	file_export->SetToolTip( wxT("Export the layout simulation results to a text file") );
	int tt; to_integer(_gui_settings["tt_delay"].value, &tt);
	//file_export->GetToolTip()->SetDelay( tt );
	font_inc->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnPlotFontIncrease ), NULL, this);
	font_dec->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnPlotFontDecrease ), NULL, this);
	save_bitmap->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnBitmapSave ), NULL, this);
	file_export->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnLayoutSimulationExport ), NULL, this);


    wxBitmap zoom_in_bit, zoom_out_bit, zoom_original_bit, zoom_rect_bit, zoom_rect_bit_on, zoom_pan_bit, zoom_pan_bit_on;
    
    zoom_in_bit.LoadFile(_image_dir.GetPath(true) + "zoom-in.png", wxBITMAP_TYPE_PNG);
    zoom_out_bit.LoadFile(_image_dir.GetPath(true) + "zoom-out.png", wxBITMAP_TYPE_PNG);
    zoom_original_bit.LoadFile(_image_dir.GetPath(true) + "zoom-original.png", wxBITMAP_TYPE_PNG);
    zoom_rect_bit.LoadFile(_image_dir.GetPath(true) + "zoom-rect.png", wxBITMAP_TYPE_PNG);
    zoom_rect_bit_on.LoadFile(_image_dir.GetPath(true) + "zoom-rect-on.png", wxBITMAP_TYPE_PNG);
    zoom_pan_bit.LoadFile(_image_dir.GetPath(true) + "zoom-pan.png", wxBITMAP_TYPE_PNG);
    zoom_pan_bit_on.LoadFile(_image_dir.GetPath(true) + "zoom-pan-on.png", wxBITMAP_TYPE_PNG);

    wxBitmapButton *zoom_in = new wxBitmapButton(parent, wxID_ANY, zoom_in_bit);
    wxBitmapButton *zoom_out = new wxBitmapButton(parent, wxID_ANY, zoom_out_bit);
    wxBitmapButton *zoom_original = new wxBitmapButton(parent, wxID_ANY, zoom_original_bit);
    _zoom_rect = new MyToggleButton(parent, wxID_ANY, zoom_rect_bit_on, zoom_rect_bit);
    _zoom_pan = new MyToggleButton(parent, wxID_ANY, zoom_pan_bit_on, zoom_pan_bit);

    zoom_in->SetToolTip("Increase zoom");
    zoom_out->SetToolTip("Decrease zoom");
    zoom_original->SetToolTip("Reset view");
    _zoom_rect->SetToolTip("Select zoom area");
    _zoom_pan->SetToolTip("Pan view region");


	_plot_frame = new FieldPlot(parent, _SF,  selection, wxID_ANY, wxDefaultPosition, wxSize(600,500));
	
	wxBoxSizer *top_sizer = new wxBoxSizer(wxHORIZONTAL);
	top_sizer->Add(select_lab, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
	top_sizer->Add(_plot_select, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    top_sizer->Add(_show_field_data, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5);
	top_sizer->Add((1,30), 0, 0, 0);
	top_sizer->Add(new wxStaticLine(parent, wxID_ANY, wxDefaultPosition, wxSize(1,1), wxLI_VERTICAL), 0, wxEXPAND|wxLEFT|wxRIGHT, 20);
	top_sizer->Add(font_inc, 0, wxRIGHT|wxALIGN_CENTER_VERTICAL, 2);
	top_sizer->Add(font_dec, 0, wxALIGN_CENTER_VERTICAL, 0);
	top_sizer->Add(new wxStaticLine(parent, wxID_ANY, wxDefaultPosition, wxSize(1,1), wxLI_VERTICAL), 0, wxEXPAND|wxLEFT|wxRIGHT, 20);
	top_sizer->Add(save_bitmap, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5);
	top_sizer->Add(file_export, 0, wxALIGN_CENTER_VERTICAL, 0);
    top_sizer->Add(new wxStaticLine(parent, wxID_ANY, wxDefaultPosition, wxSize(1,1), wxLI_VERTICAL), 0, wxEXPAND|wxLEFT|wxRIGHT, 20);
    top_sizer->Add(zoom_in, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5);
    top_sizer->Add(zoom_out, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5);
    top_sizer->Add(zoom_original, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5);
    top_sizer->Add(_zoom_rect, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5);
    top_sizer->Add(_zoom_pan, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5);

    zoom_in->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnFieldPlotZoomIn ), NULL, this);
    zoom_out->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnFieldPlotZoomOut ), NULL, this);
    zoom_original->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnFieldPlotZoomOriginal ), NULL, this);
    _zoom_rect->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnFieldPlotZoomRect ), NULL, this);
    _zoom_pan->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnFieldPlotZoomPan ), NULL, this);

    _plot_frame->Connect(wxEVT_LEFT_DOWN, wxMouseEventHandler( SPFrame::OnFieldPlotMouseLeftDown ), NULL, this);
    _plot_frame->Connect(wxEVT_LEFT_UP, wxMouseEventHandler( SPFrame::OnFieldPlotMouseLeftUp ), NULL, this);
	_plot_frame->Connect(wxEVT_RIGHT_UP, wxMouseEventHandler( SPFrame::OnFieldPlotMouseRight ), NULL, this);
    _plot_frame->Connect(wxEVT_MOTION, wxMouseEventHandler( SPFrame::OnFieldPlotMotion ), NULL, this );
    _plot_frame->Connect(wxEVT_MOUSEWHEEL, wxMouseEventHandler( SPFrame::OnFieldPlotMouseWheel ), NULL, this);
    _plot_frame->Connect(wxEVT_MIDDLE_DOWN, wxMouseEventHandler( SPFrame::OnFieldPlotMouseCenterDown ), NULL, this);
    _plot_frame->Connect(wxEVT_MIDDLE_UP, wxMouseEventHandler( SPFrame::OnFieldPlotMouseCenterUp ), NULL, this);

	main_sizer->Add(top_sizer);

	main_sizer->Add(_plot_frame,1,wxALL|wxEXPAND,5);

	parent->SetSizer(main_sizer);
	parent->SetScrollbars(10, 10, parent->GetSize().GetWidth()/10, parent->GetSize().GetWidth()/10);
	return;
}


void SPFrame::LayoutSimulationExport(SolarField &SF, wxString &fname, vector<bool> &options, wxString &header, wxString &delim, bool quiet)
{
    wxTextFile fobj(fname);

	if(! (fobj.Exists() ? fobj.Open() : fobj.Create() ) )
	{
	    if(! quiet)
			PopMessage(wxT("The file could not be opened. Please ensure that the file is not open or write-protected."), wxEmptyString, wxOK);
		
		return;
	}

	fobj.Clear();
	wxChar eol = *fobj.GetEOL();
	//additional header ingo
	fobj.AddLine(header);

    double azzen[2];
    SF.CalcDesignPtSunPosition(SF.getVarMap()->sf.sun_loc_des.mapval(), azzen[0], azzen[1]);

	//Header lines with simulation info
	wxString hdr;
	hdr.Printf(wxT("Solar azimuth,%.2f,deg%cSolar zenith,%.2f,deg%cDNI,%.2f,W/m2%c"),
		azzen[0], eol,
		azzen[1], eol,
		SF.getVarMap()->sf.dni_des.val, eol
		);
	fobj.AddLine(hdr);

	wxString hdat;	//header line listing data entries
	int ind=0;
	hdat.Append( options.at(ind++) ? wxT("Heliostat ID,") : wxEmptyString );		//0  | Heliostat ID Number
	hdat.Append( options.at(ind++) ? wxT("Pos-x,Pos-y,Pos-z,") : wxEmptyString );		//1  | Heliostat location (x,y,z)
	hdat.Append( options.at(ind++) ? wxT("Aim-x,Aim-y,Aim-z,") : wxEmptyString );		//2  | Heliostat aim point on the receiver (x,y,z)
	hdat.Append( options.at(ind++) ? wxT("Track-x,Track-y,Track-z,") : wxEmptyString );		//3  | Tracking vector (i,j,k)
	hdat.Append( options.at(ind++) ? wxT("Reflectivity,") : wxEmptyString );		//4  | Heliostat reflectivity
	hdat.Append( options.at(ind++) ? wxT("Soiling,") : wxEmptyString );		//5  | Heliostat soiling factor
	hdat.Append( options.at(ind++) ? wxT("Total eff,") : wxEmptyString );		//6  | Total heliostat efficiency
	hdat.Append( options.at(ind++) ? wxT("Cosine eff,") : wxEmptyString );		//7  | Heliostat cosine efficiency
	hdat.Append( options.at(ind++) ? wxT("Attenuation,") : wxEmptyString );		//8  | Heliostat attenuation efficiency
	hdat.Append( options.at(ind++) ? wxT("Blocking,") : wxEmptyString );		//9 | Heliostat blocking efficiency
	hdat.Append( options.at(ind++) ? wxT("Shading,") : wxEmptyString );		//10 | Heliostat shadowing efficiency
	hdat.Append( options.at(ind++) ? wxT("Intercept eff.,") : wxEmptyString );		//11 | Heliostat intercept efficiency
	hdat.Append( options.at(ind++) ? wxT("Power [kW],") : wxEmptyString );		//12 | Heliostat delivered power
	hdat.Append( options.at(ind++) ? wxT("Ranking metric,") : wxEmptyString );		//13 | Heliostat ranking metric
	hdat.Append( options.at(ind++) ? wxT("Shadow coords-") : wxEmptyString );		//14  | Heliostat shadow coordinates

	if(options.at(14))
	{
	    for(unsigned int i=0; i<SF.getHeliostats()->at(0)->getCornerCoords()->size(); i++)
	    {
	        wxString s;
			s.Printf(wxT("x%d,y%d,z%d,"), i, i, i);
			hdat.Append(s);
		}
	}
	hdat.Append( options.at(ind++) ? wxT("Cloudiness eff,") : wxEmptyString ); //15 | heliostat cloudiness loss

	hdat.Replace(wxT(","), delim, true);
	fobj.AddLine(hdat);

	//Create the format strings
	ind = 0;
	vector<wxString> fmt;
	fmt.push_back( options.at(ind++) ? wxT("%d,") : wxEmptyString );	//0  | Heliostat ID Number
	fmt.push_back( options.at(ind++) ? wxT("%.2f,%.2f,%.2f,") : wxEmptyString );	//1  | Heliostat location (x,y,z)
	fmt.push_back( options.at(ind++) ? wxT("%.2f,%.2f,%.2f,") : wxEmptyString );	//2  | Heliostat aim point on the receiver (x,y,z)
	fmt.push_back( options.at(ind++) ? wxT("%.3f,%.3f,%.3f,") : wxEmptyString );	//3  | Tracking vector (i,j,k)
	fmt.push_back( options.at(ind++) ? wxT("%.4f,") : wxEmptyString );		//4  | Heliostat reflectivity
	fmt.push_back( options.at(ind++) ? wxT("%.4f,") : wxEmptyString );		//5  | Heliostat soiling factor
	fmt.push_back( options.at(ind++) ? wxT("%.4f,") : wxEmptyString );		//6  | Total heliostat efficiency
	fmt.push_back( options.at(ind++) ? wxT("%.4f,") : wxEmptyString );		//7  | Heliostat cosine efficiency
	fmt.push_back( options.at(ind++) ? wxT("%.4f,") : wxEmptyString );		//8  | Heliostat attenuation efficiency
	fmt.push_back( options.at(ind++) ? wxT("%.4f,") : wxEmptyString );		//9 | Heliostat blocking efficiency
	fmt.push_back( options.at(ind++) ? wxT("%.4f,") : wxEmptyString );		//10 | Heliostat shadowing efficiency
	fmt.push_back( options.at(ind++) ? wxT("%.4f,") : wxEmptyString );		//11 | Heliostat intercept efficiency
	fmt.push_back( options.at(ind++) ? wxT("%.2f,") : wxEmptyString );		//12 | Heliostat delivered power
	fmt.push_back( options.at(ind++) ? wxT("%f,") : wxEmptyString );		//13 | Heliostat ranking metric
	fmt.push_back( options.at(ind++) ? wxT("%.2f,%.2f,%.2f,") : wxEmptyString );	//14  | Heliostat shadow coordinates

	for(unsigned int i=0; i<fmt.size(); i++)
	{
	    fmt[i].Replace(wxT(","), delim, true);
	}
	fmt.push_back( options.at(ind++) ? wxT("%.4f,") : wxEmptyString );	//15 | Cloudiness
		
	wxString line;
	//Loop through each heliostat and create the input
		
	int npos = SF.getHeliostats()->size();
	Heliostat *H; 
	sp_point *loc, *aim;
	Vect *track;
	vector<double> stats;
	vector<sp_point>* corners;


	for(int i=0; i<npos; i++)
	{
	    H = SF.getHeliostats()->at(i);
		corners = H->getCornerCoords();
		loc = H->getLocation();
		aim = H->getAimPoint();
		track = H->getTrackVector();
		H->getSummaryResults( stats );
			
		line.Clear();
		wxString s;
		//0  | Heliostat ID Number

		if( options.at(0) )
		{
		    s.Printf(fmt.at(0), H->getId());
			line.Append(s);
		}
		//1  | Heliostat location (x,y,z)

		if( options.at(1) )
		{
		    s.Printf(fmt.at(1), loc->x, loc->y, loc->z);
			line.Append(s);
		}
		//2  | Heliostat aim point on the receiver (x,y,z)

		if( options.at(2) )
		{
		    s.Printf(fmt.at(2), aim->x, aim->y, aim->z);
			line.Append(s);
		}
		//3  | Tracking vector (i,j,k)

		if( options.at(3) )
		{
		    s.Printf(fmt.at(3), track->i, track->j, track->k);
			line.Append(s);
		}
		//4  | Heliostat reflectivity

		if( options.at(4) )
		{
		    s.Printf(fmt.at(4), stats.at(helio_perf_data::PERF_VALUES::REFLECTIVITY));
			line.Append(s);
		}
		//5  | Heliostat soiling factor

		if( options.at(5) )
		{
		    s.Printf(fmt.at(5), stats.at(helio_perf_data::PERF_VALUES::SOILING));
			line.Append(s);
		}
		//6  | Total heliostat efficiency

		if( options.at(6) )
		{
		    s.Printf(fmt.at(6), stats.at(helio_perf_data::PERF_VALUES::ETA_TOT));
			line.Append(s);
		}
		//7  | Heliostat cosine efficiency

		if( options.at(7) )
		{
		    s.Printf(fmt.at(7), stats.at(helio_perf_data::PERF_VALUES::ETA_COS));
			line.Append(s);
		}
		//8  | Heliostat attenuation efficiency

		if( options.at(8) )
		{
		    s.Printf(fmt.at(8), stats.at(helio_perf_data::PERF_VALUES::ETA_ATT));
			line.Append(s);
		}
		//9 | Heliostat blocking efficiency

		if( options.at(9) )
		{
		    s.Printf(fmt.at(9), stats.at(helio_perf_data::PERF_VALUES::ETA_BLOCK));
			line.Append(s);
		}
		//10 | Heliostat shadowing efficiency

		if( options.at(10) )
		{
		    s.Printf(fmt.at(10), stats.at(helio_perf_data::PERF_VALUES::ETA_SHADOW));
			line.Append(s);
		}
		//11 | Heliostat intercept efficiency

		if( options.at(11) )
		{
		    s.Printf(fmt.at(11), stats.at(helio_perf_data::PERF_VALUES::ETA_INT));
			line.Append(s);
		}
		//12 | Heliostat delivered power

		if( options.at(12) )
		{
		    s.Printf(fmt.at(12), stats.at(helio_perf_data::PERF_VALUES::POWER_TO_REC));
			line.Append(s);
		}
		//13 | Heliostat ranking metric

		if( options.at(13) )
		{
		    s.Printf(fmt.at(13), stats.at(helio_perf_data::PERF_VALUES::RANK_METRIC));
			line.Append(s);
		}
		//14  | Heliostat shadow coordinates

		if( options.at(14) )
		{
		    for(unsigned int j=0; j<corners->size(); j++)
		    {
		        s.Printf(fmt.at(14), corners->at(j).x, corners->at(j).y, corners->at(j).z);
				line.Append(s);
			}
		}
		//15  | Cloudiness

		if( options.at(15) )
		{
		    s.Printf(fmt.at(15), stats.at(helio_perf_data::PERF_VALUES::ETA_CLOUD));
			line.Append(s);
		}
			
		fobj.AddLine(line);

	}
		
	fobj.Write();
	fobj.Close();

	if(!quiet) PopMessage(wxT("File successfully created."));
}


void SPFrame::OnLayoutSimulationExport( wxCommandEvent &WXUNUSED(event))
{
	//Save the simulation results to a text file
	
	//Check to see whether we have any useful data to export

	if(_SF.getHeliostats()->size() == 0)
	{
	    PopMessage(wxT("No simulation data to export! Please run a simulation first."), wxEmptyString, wxOK);
		return;
	}
	
	//Prompt the user to specify what information they would like to export
	layout_export_dialog *ld = new layout_export_dialog(this, wxID_ANY, wxT("Data export options"));
	vector<bool> options;
	wxString delim;

	if( ld->ShowModal() == wxID_OK )
	{
	    /*
		get the setting options. by index, these are:
		0  | Heliostat ID Number
		1  | Heliostat location (x,y,z)
		2  | Heliostat aim point on the receiver (x,y,z)
		3  | Tracking vector (i,j,k)
		4  | Heliostat reflectivity
		5  | Heliostat soiling factor
		6  | Total heliostat efficiency
		7  | Heliostat cosine efficiency
		8  | Heliostat attenuation efficiency
		9  | Heliostat blocking efficiency
		10 | Heliostat shadowing efficiency
		11 | Heliostat intercept efficiency
		12 | Heliostat delivered power
		13 | Heliostat ranking metric
		14 | Heliostat shadow coordinates
		15 | Cloudiness efficiency
		*/

		ld->getSelections(options);
		delim = ld->getDelimiter();
		ld->Destroy();
	}
	else
	{	
	    ld->Destroy();
		return;
	}


	wxFileDialog *dlg = new wxFileDialog(this, wxT("Export data to a file"), wxEmptyString, wxT("simulation_results.csv"),
		wxT("Data file (*.csv)|*.csv"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT);

	if(dlg->ShowModal()==wxID_OK)
	{
	    
		//Set up the text file
		wxString fname = dlg->GetPath();
		wxString hdr = wxEmptyString;
		LayoutSimulationExport(_SF, fname, options, hdr, delim, false);
		
	}
	dlg->Destroy();

}


void SPFrame::OnPlotSelectCombo( wxCommandEvent &event)
{
    /* The selection for the plot to display on the field plot page has been changed */
	wxString tsel = ( (wxComboBox*)event.GetEventObject() )->GetStringSelection();

    std::vector< std::string > choices = _plot_frame->GetPlotChoices();

    int sel = std::find( choices.begin(), choices.end(), tsel ) - choices.begin();

	_plot_frame->SetPlotOption(sel);

	this->Refresh();
	this->Update();
	
}

/* 
------------ DPI panel object and global function -------------------
*/
class FieldPlotSavePanelCtrls : public wxPanel
{
	wxTextCtrl *dpi_input;
	int _dpi;
public:

	FieldPlotSavePanelCtrls( wxWindow *parent )
		: wxPanel(parent)
	{
		_dpi = 96;
		dpi_input = new wxTextCtrl(this, wxID_ANY, "96", wxDefaultPosition, wxSize(80,21));
		dpi_input->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( FieldPlotSavePanelCtrls::OnDPIText ), NULL, this);
		wxStaticText *dpi_label = new wxStaticText(this, wxID_ANY, "DPI setting:", wxDefaultPosition, wxSize(65,21));
		wxBoxSizer *dpi_sizer = new wxBoxSizer(wxHORIZONTAL);
		dpi_sizer->AddSpacer(150);
		dpi_sizer->Add(dpi_label, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
		dpi_sizer->Add(dpi_input, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
		this->SetSizerAndFit(dpi_sizer);
	};

	int GetPPI()
	{ 
	    return _dpi; 
	}
	

	void OnDPIText( wxCommandEvent &WXUNUSED(evt))
	{
	    try
	    {
	        string val = (string)dpi_input->GetValue();
			int newdpi;
			to_integer(val, &newdpi);
			_dpi = newdpi;
		}
		catch(...)
		{
		}
	}
};

static wxWindow* createFieldPlotSavePanelCtrls(wxWindow *parent)
{
    return new FieldPlotSavePanelCtrls(parent);
}
/* 
---------------------------------------------------------------------
*/


void SPFrame::OnBitmapSave( wxCommandEvent &WXUNUSED(event))
{
    wxFileDialog dlg(this, wxT("Export to a file"), wxEmptyString, "plot.png", 
		wxT("Image files (*.jpg;*.png;*.bmp;*.tif)|*.jpg;*.png;*.bmp;*.tif"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT);

	//custom control window for DPI setting

	dlg.SetExtraControlCreator(&createFieldPlotSavePanelCtrls);


	if(dlg.ShowModal()==wxID_OK)
	{
	    //Save the bitmap plot to a file.
		int oldppi = _plot_frame->GetPPI();
		wxWindow * const extra = dlg.GetExtraControl();
		int ppi = static_cast<FieldPlotSavePanelCtrls*>(extra)->GetPPI();
		_plot_frame->SetPPI(ppi);
		_plot_frame->ResetPPIOnPaintEvent(oldppi);
		wxClientDC pdc(this);
		_plot_frame->DoPaint(pdc);
		
		wxBitmap *bitmap = _plot_frame->GetBitmap();
		wxImage image = bitmap->ConvertToImage();

		wxString fpath = dlg.GetPath();
		vector<string> seps = split(fpath.ToStdString(), ".");
		string ftype = lower_case(seps.at(seps.size()-1));
		

		if(ftype == "jpg" || ftype == "jpeg")
		{ // jpeg
		    image.SaveFile( fpath, wxBITMAP_TYPE_JPEG );
		}
		else if(ftype == "png")
		{ //png
		    image.SaveFile( fpath, wxBITMAP_TYPE_PNG );
		}
		else if(ftype == "bmp")
		{ //bmp
		    image.SaveFile( fpath, wxBITMAP_TYPE_BMP );
		}
		else if(ftype == "tif")
		{
		    image.SaveFile( fpath, wxBITMAP_TYPE_TIF );
		}
		else
		{
		    PopMessage("Invalid image file type! Please choose from JPG, PNG, BMP, or TIFF.");
			return;
		}

	}
}

/* 
-------------- zoom and view functionality ---------------------
*/

void SPFrame::OnFieldPlotZoomIn( wxCommandEvent &WXUNUSED(evt))
{
    double zoom = _plot_frame->GetPlotZoomFact();
    _plot_frame->SetPlotZoomFact( zoom * 1.6 );

	_plot_frame->Refresh();
	_plot_frame->Update();
}

void SPFrame::OnFieldPlotZoomOut( wxCommandEvent &WXUNUSED(evt))
{
    double zoom = _plot_frame->GetPlotZoomFact();
    _plot_frame->SetPlotZoomFact( zoom / 1.6 );

	_plot_frame->Refresh();
	_plot_frame->Update();
}

void SPFrame::OnFieldPlotZoomRect( wxCommandEvent &WXUNUSED(evt))
{
    bool state = _zoom_rect->Toggle();
    if(state)
        _zoom_pan->SetState(false);
}

void SPFrame::OnFieldPlotZoomPan( wxCommandEvent &WXUNUSED(evt))
{
    bool state = _zoom_pan->Toggle();
    if(state)
        _zoom_rect->SetState(false);
}

void SPFrame::OnFieldPlotZoomOriginal( wxCommandEvent &WXUNUSED(evt))
{
    _plot_frame->SetPlotZoomFact( 0.95 );
    int xy[] = {0,0};
    _plot_frame->SetOriginOffset(xy);

    _zoom_pan->SetState(false);
    _zoom_rect->SetState(false);

    _plot_frame->Refresh();
    _plot_frame->Update();
}

void SPFrame::OnFieldPlotMouseLeftDown( wxMouseEvent &evt)
{
    _plot_frame->SetFocus();

    _field_left_mouse_start[0] = evt.GetPosition().x;
    _field_left_mouse_start[1] = evt.GetPosition().y;
}

void SPFrame::OnFieldPlotMouseLeftUp( wxMouseEvent &evt)
{

    int *state0 = _plot_frame->GetOriginOffset();
    
    int path[2];
    path[0] = evt.GetPosition().x - _field_left_mouse_start[0] + state0[0];
    path[1] = evt.GetPosition().y - _field_left_mouse_start[1] + state0[1];

    if(_zoom_pan->GetState() )
        _plot_frame->SetOriginOffset( path );

    if(_zoom_rect->GetState() )
    {
        wxSize csize = _plot_frame->GetSize();
        int boxsizex = abs(_field_left_mouse_start[0] - evt.GetPosition().x);
        int boxsizey = abs(_field_left_mouse_start[1] - evt.GetPosition().y);

        double newscale = fmin( (double)csize.GetWidth()/(double)boxsizex, (double)csize.GetHeight()/(double)boxsizey);

        int boxorx = (_field_left_mouse_start[0] + evt.GetPosition().x)/2 - csize.GetWidth()/2;
        int boxory = csize.GetHeight()/2 - (_field_left_mouse_start[1] + evt.GetPosition().y)/2;

        int xy[] = {state0[0] - boxorx, state0[1] + boxory};
        _plot_frame->SetOriginOffset( xy );
        _plot_frame->SetPlotZoomFact( _plot_frame->GetPlotZoomFact() * newscale );

        _plot_frame->EnableZoomRectangle(false);
    }

    _plot_frame->Update();
    _plot_frame->Refresh();
}

void SPFrame::OnFieldPlotMouseRight( wxMouseEvent &evt )
{
	/* 
	Handle right mouse clicks on the field plot page. The goal is to provide additional
	information via right click on selected heliostat (or other) plot elements.
	*/

	//Get the click position in pixels
	long x,y;
	evt.GetPosition( &x, &y );

	//Get the current position of the solar field origin in pixels
	int *origin = _plot_frame->GetOriginPixels();
	double scale = _plot_frame->GetMetersPerPixel();
	
	//Translate into dimensional click location
	double xcoord = (x-origin[0])*scale;
	double ycoord = (origin[1]-y)*scale;

	//Find the nearest heliostat
	Hvector *helios = _SF.getHeliostats();
	
	//use the search tree method.. is this data stored after layout? Each heliostat should be tagged with it's tree address.


	return;
}

void SPFrame::OnFieldPlotMouseWheel( wxMouseEvent &evt) 
{
    int rot = (int)( (float)evt.GetWheelRotation()/120. );
    double zoom = pow(1.17, rot);
    _plot_frame->SetPlotZoomFact( _plot_frame->GetPlotZoomFact() * zoom );

    _plot_frame->Update();
    _plot_frame->Refresh();
}

void SPFrame::OnFieldPlotMouseCenterDown( wxMouseEvent &evt )
{
    _plot_frame->SetFocus();

    _field_left_mouse_start[0] = evt.GetPosition().x;
    _field_left_mouse_start[1] = evt.GetPosition().y;
}

void SPFrame::OnFieldPlotMouseCenterUp( wxMouseEvent &evt )
{

    int *state0 = _plot_frame->GetOriginOffset();
    
    int path[2];
    path[0] = evt.GetPosition().x - _field_left_mouse_start[0] + state0[0];
    path[1] = evt.GetPosition().y - _field_left_mouse_start[1] + state0[1];

    _plot_frame->SetOriginOffset( path );

    _plot_frame->Update();
    _plot_frame->Refresh();
}

void SPFrame::OnFieldPlotMotion( wxMouseEvent &evt )
{
    if(! evt.Dragging() ) return;
    if( _zoom_rect->GetState() ) 
    {
        if(evt.LeftIsDown())
        {
            int xyxy[] = {_field_left_mouse_start[0], _field_left_mouse_start[1], evt.GetPosition().x, evt.GetPosition().y};
            _plot_frame->SetZoomRectangle(xyxy);
            _plot_frame->EnableZoomRectangle(true);

            _plot_frame->Update();
            _plot_frame->Refresh();

            return;
        }
        else
            return;
    }
    if( evt.LeftIsDown() )
    {
        int delta[2];
        delta[0] = evt.GetPosition().x - _field_left_mouse_start[0];
        delta[1] = evt.GetPosition().y - _field_left_mouse_start[1];

        _field_left_mouse_start[0] += delta[0];
        _field_left_mouse_start[1] += delta[1];

        _plot_frame->AddToOriginOffset( delta );
        
        _plot_frame->Update();
        _plot_frame->Refresh();
    }
}

void SPFrame::OnPlotDataToggle( wxCommandEvent &WXUNUSED(evt) )
{
    bool state = _show_field_data->Toggle();

    _plot_frame->SetDataVisible( state );

    _plot_frame->Update();
    _plot_frame->Refresh();
}