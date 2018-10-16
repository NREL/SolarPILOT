
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
#include "treemesh.h"

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

	wxBitmap del_select_bit, undel_select_bit;
	del_select_bit.LoadFile(_image_dir.GetPath(true) + "db_remove.png", wxBITMAP_TYPE_PNG);
	undel_select_bit.LoadFile(_image_dir.GetPath(true) + "edit-undelete.png", wxBITMAP_TYPE_PNG);
	wxBitmapButton *del_select = new wxBitmapButton(parent, wxID_ANY, del_select_bit);
	wxBitmapButton *undel_select = new wxBitmapButton(parent, wxID_ANY, undel_select_bit);

    zoom_in->SetToolTip("Increase zoom");
    zoom_out->SetToolTip("Decrease zoom");
    zoom_original->SetToolTip("Reset view");
    _zoom_rect->SetToolTip("Select zoom area");
    _zoom_pan->SetToolTip("Pan view region");
	del_select->SetToolTip("Delete selected heliostats");
	undel_select->SetToolTip("Restore deleted heliostats");
	
	_plot_frame = new FieldPlot(parent, _SF,  selection, wxID_ANY, wxDefaultPosition, wxSize(600,500));
	
	wxBoxSizer *top_sizer = new wxBoxSizer(wxHORIZONTAL);
	top_sizer->Add(select_lab, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
	top_sizer->Add(_plot_select, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    top_sizer->Add(_show_field_data, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5);
	top_sizer->Add((1,30), 0, 0, 0);
	top_sizer->Add(new wxStaticLine(parent, wxID_ANY, wxDefaultPosition, wxSize(1,1), wxLI_VERTICAL), 0, wxEXPAND|wxLEFT|wxRIGHT, 10);
	top_sizer->Add(font_inc, 0, wxRIGHT|wxALIGN_CENTER_VERTICAL, 2);
	top_sizer->Add(font_dec, 0, wxALIGN_CENTER_VERTICAL, 0);
	top_sizer->Add(new wxStaticLine(parent, wxID_ANY, wxDefaultPosition, wxSize(1,1), wxLI_VERTICAL), 0, wxEXPAND|wxLEFT|wxRIGHT, 10);
	top_sizer->Add(save_bitmap, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5);
	top_sizer->Add(file_export, 0, wxALIGN_CENTER_VERTICAL, 0);
    top_sizer->Add(new wxStaticLine(parent, wxID_ANY, wxDefaultPosition, wxSize(1,1), wxLI_VERTICAL), 0, wxEXPAND|wxLEFT|wxRIGHT, 10);
    top_sizer->Add(zoom_in, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5);
    top_sizer->Add(zoom_out, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5);
    top_sizer->Add(zoom_original, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5);
    top_sizer->Add(_zoom_rect, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5);
    top_sizer->Add(_zoom_pan, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5);
    top_sizer->Add(new wxStaticLine(parent, wxID_ANY, wxDefaultPosition, wxSize(1,1), wxLI_VERTICAL), 0, wxEXPAND|wxLEFT|wxRIGHT, 10);
    top_sizer->Add(del_select, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5);
    top_sizer->Add(undel_select, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5);

    zoom_in->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnFieldPlotZoomIn ), NULL, this);
    zoom_out->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnFieldPlotZoomOut ), NULL, this);
    zoom_original->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnFieldPlotZoomOriginal ), NULL, this);
    _zoom_rect->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnFieldPlotZoomRect ), NULL, this);
    _zoom_pan->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnFieldPlotZoomPan ), NULL, this);
	del_select->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnDeleteSelectedHeliostats ), NULL, this);
	undel_select->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnRestoreSelectedHeliostats ), NULL, this);

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
    /*
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
    16 | Annual energy delivery
    17 | Annual total efficiency
    */

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
    //0 | Heliostat ID Number
	hdat.Append( options.at(ind++) ? wxT("Heliostat ID,") : wxEmptyString );		
    //1 | Heliostat location(x, y, z)
	hdat.Append( options.at(ind++) ? wxT("Pos-x,Pos-y,Pos-z,") : wxEmptyString );		
    //2 | Heliostat aim point on the receiver(x, y, z)
	hdat.Append( options.at(ind++) ? wxT("Aim-x,Aim-y,Aim-z,") : wxEmptyString );
    //3 | Tracking vector(i, j, k)
	hdat.Append( options.at(ind++) ? wxT("Track-x,Track-y,Track-z,") : wxEmptyString );
    //4 | Heliostat reflectivity
	hdat.Append( options.at(ind++) ? wxT("Reflectivity,") : wxEmptyString );
    //5 | Heliostat soiling factor
	hdat.Append( options.at(ind++) ? wxT("Soiling,") : wxEmptyString );
    //6 | Total heliostat efficiency
	hdat.Append( options.at(ind++) ? wxT("Total eff,") : wxEmptyString );
    //7 | Heliostat cosine efficiency
	hdat.Append( options.at(ind++) ? wxT("Cosine eff,") : wxEmptyString );
    //8 | Heliostat attenuation efficiency
	hdat.Append( options.at(ind++) ? wxT("Attenuation,") : wxEmptyString );
    //9 | Heliostat blocking efficiency
	hdat.Append( options.at(ind++) ? wxT("Blocking,") : wxEmptyString );
    //10 | Heliostat shadowing efficiency
	hdat.Append( options.at(ind++) ? wxT("Shading,") : wxEmptyString );
    //11 | Heliostat intercept efficiency
	hdat.Append( options.at(ind++) ? wxT("Intercept eff.,") : wxEmptyString );
    //12 | Heliostat delivered power
	hdat.Append( options.at(ind++) ? wxT("Power [kW],") : wxEmptyString );
    //13 | Heliostat ranking metric
	hdat.Append( options.at(ind++) ? wxT("Ranking metric,") : wxEmptyString );
    //14 | Heliostat shadow coordinates
	hdat.Append( options.at(ind) ? wxT("Shadow coords-") : wxEmptyString );
	if(options.at(ind++))
	{
	    for(unsigned int i=0; i<SF.getHeliostats()->at(0)->getCornerCoords()->size(); i++)
	    {
	        wxString s;
			s.Printf(wxT("x%d,y%d,z%d,"), i, i, i);
			hdat.Append(s);
		}
	}
    //15 | Cloudiness efficiency
	hdat.Append( options.at(ind++) ? wxT("Cloudiness eff,") : wxEmptyString );
    //16 | Annual energy delivery
    hdat.Append(options.at(ind++) ? wxT("Annual energy,") : wxEmptyString);
    //17 | Annual total efficiency
    hdat.Append(options.at(ind++) ? wxT("Annual efficiency,") : wxEmptyString);

    hdat.Replace(wxT(","), delim, true);
	fobj.AddLine(hdat);

    std::vector< wxString > fmt = 
    {
        "%.0f,",        //0 | Heliostat ID Number
        "%.2f,",        //1 | Heliostat location(x, y, z)
        "%.2f,",        //2 | Heliostat aim point on the receiver(x, y, z)
        "%.3f,",        //3 | Tracking vector(i, j, k)
        "%.4f,",        //4 | Heliostat reflectivity
        "%.4f,",        //5 | Heliostat soiling factor
        "%.4f,",        //6 | Total heliostat efficiency
        "%.4f,",        //7 | Heliostat cosine efficiency
        "%.4f,",        //8 | Heliostat attenuation efficiency
        "%.4f,",        //9 | Heliostat blocking efficiency
        "%.4f,",        //10 | Heliostat shadowing efficiency
        "%.4f,",        //11 | Heliostat intercept efficiency
        "%.2f,",        //12 | Heliostat delivered power
        "%f,",          //13 | Heliostat ranking metric
        "%.2f,",        //14 | Heliostat shadow coordinates
        "%.4f,",        //15 | Cloudiness efficiency
        "%.1f,",        //16 | Annual energy delivery
        "%.4f,",        //17 | Annual total efficiency
    };

	//Create the format strings
	for(unsigned int i=0; i<fmt.size(); i++)
	    fmt[i].Replace(wxT(","), delim, true);
		
	//Loop through each heliostat and create the input
	for(int i=0; i<SF.getHeliostats()->size(); i++)
	{
	    Heliostat *H = SF.getHeliostats()->at(i);
		std::vector<sp_point> *corners = H->getCornerCoords();
		sp_point* loc = H->getLocation();
        sp_point* aim = H->getAimPoint();
		Vect* track = H->getTrackVector();
        std::vector<double> stats;
		H->getSummaryResults( stats );

        std::vector< std::vector<double> > all_data;
        all_data.push_back( {(double)H->getId()} );                                       //0 | Heliostat ID Number
        all_data.push_back( {loc->x, loc->y, loc->z} );                                    //1 | Heliostat location(x,y,z)
        all_data.push_back( {aim->x, aim->y, aim->z} );                                    //2 | Heliostat aim point on the receiver(x,y,z)
        all_data.push_back( {track->i, track->j, track->k} );                              //3 | Tracking vector(i,j,k)
        all_data.push_back( {stats.at(helio_perf_data::PERF_VALUES::REFLECTIVITY)} );      //4 | Heliostat reflectivity
        all_data.push_back( {stats.at(helio_perf_data::PERF_VALUES::SOILING)} );           //5 | Heliostat soiling factor
        all_data.push_back( {stats.at(helio_perf_data::PERF_VALUES::ETA_TOT)} );           //6 | Total heliostat efficiency
        all_data.push_back( {stats.at(helio_perf_data::PERF_VALUES::ETA_COS)} );           //7 | Heliostat cosine efficiency
        all_data.push_back( {stats.at(helio_perf_data::PERF_VALUES::ETA_ATT)} );           //8 | Heliostat attenuation efficiency
        all_data.push_back( {stats.at(helio_perf_data::PERF_VALUES::ETA_BLOCK)} );         //9 | Heliostat blocking efficiency
        all_data.push_back( {stats.at(helio_perf_data::PERF_VALUES::ETA_SHADOW)} );        //10 | Heliostat shadowing efficiency
        all_data.push_back( {stats.at(helio_perf_data::PERF_VALUES::ETA_INT)} );           //11 | Heliostat intercept efficiency
        all_data.push_back( {stats.at(helio_perf_data::PERF_VALUES::POWER_TO_REC)} );      //12 | Heliostat delivered power
        all_data.push_back( {stats.at(helio_perf_data::PERF_VALUES::RANK_METRIC)} );       //13 | Heliostat ranking metric
        all_data.push_back( std::vector<double>() );      //fill this in below...                             //14 | Heliostat shadow coordinates
        for (unsigned int j = 0; j < corners->size(); j++)
        {
            sp_point *p = &corners->at(j);
            all_data.back().push_back(p->x);
            all_data.back().push_back(p->y);
            all_data.back().push_back(p->z);
        }
        all_data.push_back({stats.at(helio_perf_data::PERF_VALUES::ETA_CLOUD)} );         //15 | Cloudiness efficiency
        all_data.push_back({stats.at(helio_perf_data::PERF_VALUES::ANNUAL_POWER)} );      //16 | Annual energy delivery
        all_data.push_back({stats.at(helio_perf_data::PERF_VALUES::ANNUAL_EFFICIENCY)} ); //17 | Annual total efficiency
        
                                                                                            //heliotat shadow coords

        wxString line;
        for (int j = 0; j < (int)all_data.size(); j++)
            if (options.at(j))
                for (int k = 0; k < (int)all_data.at(j).size(); k++)
                    line.Append(wxString::Format(fmt.at(j), all_data.at(j).at(k)) );

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
        16 | Annual energy delivery
        17 | Annual total efficiency

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

void SPFrame::OnDeleteSelectedHeliostats( wxCommandEvent &WXUNUSED(evt))
{
	std::vector<Heliostat*> *hsel = _plot_frame->GetSelectedHeliostats();
	for(size_t i=0; i<hsel->size(); i++)
	{
		hsel->at(i)->setInLayout(false);
		hsel->at(i)->IsEnabled(false);
	}

	_SF.UpdateLayoutAfterChange();
	UpdateCalculatedGUIValues();

	_plot_frame->ClearSelectedHeliostats();

	_plot_frame->Update();
	_plot_frame->Refresh();
	return;
}

void SPFrame::OnRestoreSelectedHeliostats( wxCommandEvent &WXUNUSED(evt))
{
	//restore the layout by re-enabling all of the disabled heliostats in the field
	Hvector *helios = _SF.getHeliostats();
	for(size_t i=0; i<helios->size(); i++)
	{
		helios->at(i)->setInLayout(true);
		helios->at(i)->IsEnabled(true);
	}

	_plot_frame->ClearSelectedHeliostats();

	_SF.UpdateLayoutAfterChange();
	UpdateCalculatedGUIValues();

	_plot_frame->Update();
	_plot_frame->Refresh();
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
	
	std::vector<void*> hitelements;
	bool has_elements = _plot_frame->GetKDHashTree()->get_all_data_at_loc( hitelements, xcoord, ycoord );

	//brute search for closest
	if(has_elements)
	{
		int imin=0;
		double r0;
		for(size_t i=0; i<hitelements.size(); i++)
		{
			sp_point *hloc = ((Heliostat*)(hitelements.at(i)))->getLocation();
			double r = (hloc->x - xcoord)*(hloc->x - xcoord) + (hloc->y - ycoord)*(hloc->y - ycoord);
			r = sqrtf(r);
			
			if( i==0 )
			{
				r0 = r;
				continue;
			}
			if( r < r0 )
			{
				r0 = r;
				imin = i;
			}
		}

		//closest is heliostat 'imin'
		Heliostat *H = (Heliostat*)hitelements.at(imin);
		
		//if a single heliostat is selected (no CTRL press), clear the existing list of selected heliostats
		bool isctrl = wxGetKeyState(wxKeyCode::WXK_CONTROL); 
		if(! isctrl )
			_plot_frame->ClearSelectedHeliostats();
		
		_plot_frame->HeliostatAnnotation(H);

	}
	else
	{
		//no elements. clear the annotation, if needed
		_plot_frame->ClearSelectedHeliostats();
	}

	//prompt redraw
	_plot_frame->Update();
	_plot_frame->Refresh();

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