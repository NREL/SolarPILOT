
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
#include "STSimulateThread.h"
#include "STObject.h"

using namespace std;


void SPFrame::CreateSimulationsFluxTab( wxScrolledWindow *flux)
{
    try
    {
        //Create the different panels that will be used to sort the options
    wxPanel
        *azel_panel = new wxPanel(flux),
        *time_panel = new wxPanel(flux),
        *soltrace_panel = new wxPanel(flux),
        *sigma_panel = new wxPanel(flux),
        *dist_panel = new wxPanel(sigma_panel),
        *cloud_panel = new wxPanel(flux);
    wxBoxSizer
        *azel_sizer = new wxBoxSizer(wxVERTICAL),
        *time_sizer = new wxBoxSizer(wxVERTICAL),
        *soltrace_sizer = new wxBoxSizer(wxVERTICAL),
        *sigma_sizer = new wxBoxSizer(wxVERTICAL),
        *dist_sizer = new wxBoxSizer(wxVERTICAL);

    InputControl
        *flux_model = new InputControl(flux, wxID_ANY, _variables.flux.flux_model ),
        *flux_time_type = new InputControl(flux, wxID_ANY, _variables.flux.flux_time_type ),
        *flux_dni = new InputControl(flux, wxID_ANY, _variables.flux.flux_dni ),
        *aim_method = new InputControl(flux, wxID_ANY, _variables.flux.aim_method ),
        *sigma_limit_x = new InputControl(sigma_panel, wxID_ANY, _variables.flux.sigma_limit_x ),
        *sigma_limit_y = new InputControl(sigma_panel, wxID_ANY, _variables.flux.sigma_limit_y ),
        *norm_dist_sigma = new InputControl(dist_panel, wxID_ANY, _variables.flux.norm_dist_sigma ),
        *flux_dist = new InputControl(sigma_panel, wxID_ANY, _variables.flux.flux_dist ),
        *flux_month = new InputControl(time_panel, wxID_ANY, _variables.flux.flux_month ),
        *flux_day = new InputControl(time_panel, wxID_ANY, _variables.flux.flux_day ),
        *flux_hour = new InputControl(time_panel, wxID_ANY, _variables.flux.flux_hour ),
        *flux_solar_az_in = new InputControl(azel_panel, wxID_ANY, _variables.flux.flux_solar_az_in ),
        *flux_solar_el_in = new InputControl(azel_panel, wxID_ANY, _variables.flux.flux_solar_el_in ),
        *x_res = new InputControl(flux, wxID_ANY, _variables.flux.x_res ),
        *y_res = new InputControl(flux, wxID_ANY, _variables.flux.y_res ),
        *min_rays = new InputControl(soltrace_panel, wxID_ANY, _variables.flux.min_rays ),
        *max_rays = new InputControl(soltrace_panel, wxID_ANY, _variables.flux.max_rays ),
        *seed = new InputControl(soltrace_panel, wxID_ANY, _variables.flux.seed ),
        *save_data = new InputControl(soltrace_panel, wxID_ANY, _variables.flux.save_data ),
        *is_save_raydata = new InputControl(soltrace_panel, wxID_ANY, _variables.flux.is_save_raydata ),
        *is_load_raydata = new InputControl(soltrace_panel, wxID_ANY, _variables.flux.is_load_raydata ),
        *raydata_file = new InputControl(soltrace_panel, wxID_ANY, _variables.flux.raydata_file ),
        *is_sunshape_err = new InputControl(soltrace_panel, wxID_ANY, _variables.flux.is_sunshape_err ),
        *is_optical_err = new InputControl(soltrace_panel, wxID_ANY, _variables.flux.is_optical_err ),
        *is_cloudy = new InputControl(flux, wxID_ANY, _variables.flux.is_cloudy ),
        *cloud_shape = new InputControl(cloud_panel, wxID_ANY, _variables.flux.cloud_shape ),
        *cloud_width = new InputControl(cloud_panel, wxID_ANY, _variables.flux.cloud_width ),
        *cloud_depth = new InputControl(cloud_panel, wxID_ANY, _variables.flux.cloud_depth ),
        *cloud_opacity = new InputControl(cloud_panel, wxID_ANY, _variables.flux.cloud_opacity ),
        *cloud_skew = new InputControl(cloud_panel, wxID_ANY, _variables.flux.cloud_skew ),
        *cloud_loc_x = new InputControl(cloud_panel, wxID_ANY, _variables.flux.cloud_loc_x ),
        *cloud_loc_y = new InputControl(cloud_panel, wxID_ANY, _variables.flux.cloud_loc_y ),
        *is_cloud_pattern = new InputControl(cloud_panel, wxID_ANY, _variables.flux.is_cloud_pattern ),
        *is_cloud_symd = new InputControl(cloud_panel, wxID_ANY, _variables.flux.is_cloud_symd ),
        *is_cloud_symw = new InputControl(cloud_panel, wxID_ANY, _variables.flux.is_cloud_symw ),
        *cloud_sep_width = new InputControl(cloud_panel, wxID_ANY, _variables.flux.cloud_sep_width ),
        *cloud_sep_depth = new InputControl(cloud_panel, wxID_ANY, _variables.flux.cloud_sep_depth );

    OutputControl
        *flux_solar_az = new OutputControl(time_panel, wxID_ANY, _variables.flux.flux_solar_az),
        *flux_solar_el = new OutputControl(time_panel, wxID_ANY, _variables.flux.flux_solar_el);
    {
    wxWindow* dsibs[] = {aim_method, sigma_limit_x, sigma_limit_y};
    flux_model->setDisabledSiblings("SolTrace", 3, dsibs);
    }    //preserve aim points for SolTrace runs
    _input_map[ &_variables.recs[0].rec_type ]->setDisabledSiblings("External cylindrical", sigma_limit_x);

    //save ray data button
    wxBitmap savebit;
    savebit.LoadFile(_image_dir.GetPath(true) + "document-open-2.png", wxBITMAP_TYPE_PNG);
    wxBitmapButton *save_data_loc = new wxBitmapButton(soltrace_panel, wxID_ANY, savebit);
    save_data_loc->SetToolTip( _variables.flux.save_data_loc.long_desc );

    //Run button
    _flux_button = new wxBitmapButton(flux, wxID_ANY, _simimages[0]);
    _flux_button->SetBitmapHover(_simimages[1]);
    _flux_button->SetBitmapSelected(_simimages[2]);
    _flux_button->SetToolTip( "Run" );
    _flux_gauge = new wxGauge(flux, wxID_ANY, 100, wxDefaultPosition, wxSize(200,25), wxGA_HORIZONTAL|wxGA_SMOOTH );
    _flux_gauge->SetRange(1000);
    _flux_progress = new wxStaticText(flux, wxID_ANY, "Progress (Stage 0/0): 0%" );
    
    wxBoxSizer *top_sizer = new wxBoxSizer(wxHORIZONTAL);
    wxStaticText *sim_lab = new wxStaticText(flux, wxID_ANY, "Simulate\nperformance" );
    sim_lab->SetFont( wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL) );
    
    wxButton *st_export = new wxButton(soltrace_panel, wxID_ANY, "Export SolTrace .stinput file");
    
    _flux_button->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnDoPerformanceSimulation ), NULL, this);
    st_export->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnStinputExport ), NULL, this);
    save_data_loc->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnSTRayDataButton ), NULL, this);

    //special catches for rebinning soltrace data
    x_res->Connect(wxEVT_KILL_FOCUS, wxFocusEventHandler( SPFrame::OnFluxPlotBinChangeX ), NULL, this);
    y_res->Connect(wxEVT_KILL_FOCUS, wxFocusEventHandler( SPFrame::OnFluxPlotBinChangeY ), NULL, this);

    //Do layout
    wxBoxSizer *flux_sizer = new wxBoxSizer(wxVERTICAL);
    
    top_sizer->Add(_flux_button, 0, wxALL, 5);
    top_sizer->Add(sim_lab, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 10);
    
    wxStaticLine *sl1 = new wxStaticLine(flux, wxID_ANY, wxDefaultPosition, wxSize(1,1), wxSL_VERTICAL);
    top_sizer->Add((25,1), 0, 0, 0);
    top_sizer->Add(sl1, 0, wxEXPAND|wxALL, 5);
    top_sizer->Add((25,1), 0, 0, 0);
    wxBoxSizer *prog_sizer = new wxBoxSizer(wxVERTICAL);
    prog_sizer->Add(_flux_gauge, 0, wxALL, 5);
    prog_sizer->Add(_flux_progress, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5);
    top_sizer->Add(prog_sizer, 0, wxALIGN_CENTER_VERTICAL, 0);

    wxStaticBox *sb0 = new wxStaticBox(flux, wxID_ANY, "Sun position");
    wxStaticBoxSizer *sbs0 = new wxStaticBoxSizer(sb0, wxVERTICAL);

    azel_sizer->Add(flux_solar_az_in);
    azel_sizer->Add(flux_solar_el_in);
    azel_panel->SetSizer(azel_sizer);
    
    time_sizer->Add(flux_month);
    time_sizer->Add(flux_day);
    time_sizer->Add(flux_hour);
    time_sizer->Add(flux_solar_az);
    time_sizer->Add(flux_solar_el);
    time_panel->SetSizer(time_sizer);
    
    sbs0->Add(flux_time_type);
    sbs0->Add(flux_dni);
    sbs0->Add(azel_panel, 0, wxALL, 5);
    sbs0->Add(time_panel, 0, wxALL, 5);

    wxStaticBox *sb1 = new wxStaticBox(flux, wxID_ANY, "Simulation parameters");
    wxStaticBoxSizer *sbs1 = new wxStaticBoxSizer(sb1, wxVERTICAL);
    
    sigma_sizer->Add(sigma_limit_y);
    sigma_sizer->Add(sigma_limit_x);
    sigma_sizer->Add(flux_dist);
    dist_sizer->Add(norm_dist_sigma);
    dist_panel->SetSizer(dist_sizer);
    sigma_sizer->Add(dist_panel, 0, wxALL, 5);
    sigma_panel->SetSizer(sigma_sizer);
    sbs1->Add(aim_method);
    sbs1->Add(sigma_panel, 0, wxALL, 5);
    
    sbs1->Add(flux_model);
    
    wxString msg1 = "Specify the maximum number of flux nodes in the horizontal and vertical\ndirections. This will apply to each receiver flux surface.";
    wxStaticText *st1 = new wxStaticText(flux, wxID_ANY, msg1);
    st1->SetForegroundColour(_helptext_colour);
    sbs1->Add(st1, 0, wxALL, 5);
    sbs1->Add(x_res);
    sbs1->Add(y_res);
    
    wxString msg2 = "The simulation will run until either the minimum ray intersection count or the\nmaximum generated rays is met.";
    wxStaticText *st2 = new wxStaticText(soltrace_panel, wxID_ANY, msg2);
    st2->SetForegroundColour(_helptext_colour);
    soltrace_sizer->Add(st2, 0, wxALL, 5);
    soltrace_sizer->Add(min_rays);
    soltrace_sizer->Add(max_rays);
    soltrace_sizer->Add(seed);
    
    wxStaticBox *sb_s1 = new wxStaticBox(soltrace_panel, wxID_ANY, "Optical errors");
    wxStaticBoxSizer *sbs_s1 = new wxStaticBoxSizer(sb_s1, wxVERTICAL);
    sbs_s1->Add(is_sunshape_err);
    sbs_s1->Add(is_optical_err);
    soltrace_sizer->Add(sbs_s1, 0, wxALL, 5);

    wxBoxSizer *st_sub_sizer =  new wxBoxSizer(wxHORIZONTAL);
    st_sub_sizer->Add(save_data_loc, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    st_sub_sizer->Add(save_data, 0, wxALIGN_CENTER_VERTICAL, 0);

    soltrace_sizer->Add(st_sub_sizer);
    soltrace_sizer->Add(st_export, 0, wxALL, 5);
    soltrace_sizer->Add(is_save_raydata);
    soltrace_sizer->Add(is_load_raydata);
    soltrace_sizer->Add(raydata_file);
    soltrace_panel->SetSizer(soltrace_sizer);

    sbs1->Add(soltrace_panel);

    //---- cloud static box
    wxStaticBox *sb3 = new wxStaticBox(flux, wxID_ANY, "Clouds");
    wxStaticBoxSizer *sbs3 = new wxStaticBoxSizer(sb3, wxVERTICAL);
    sbs3->Add(is_cloudy);
    wxBoxSizer *cloud_s = new wxBoxSizer(wxVERTICAL);
    cloud_s->Add(cloud_shape);
    cloud_s->Add(cloud_width);
    cloud_s->Add(cloud_depth);
    cloud_s->Add(cloud_opacity);
    cloud_s->Add(cloud_skew);
    cloud_s->Add(cloud_loc_x);
    cloud_s->Add(cloud_loc_y);
    cloud_s->Add(is_cloud_pattern);
    cloud_s->Add(is_cloud_symd);
    cloud_s->Add(is_cloud_symw);
    cloud_s->Add(cloud_sep_width);
    cloud_s->Add(cloud_sep_depth);
    cloud_panel->SetSizerAndFit(cloud_s);
    sbs3->Add(cloud_panel, 0, wxALL, 5);
    is_cloudy->setPanelObject("true", *cloud_panel);
    {
    wxWindow* dsibs[] = {cloud_sep_depth,cloud_sep_width,is_cloud_symd,is_cloud_symw};
    is_cloud_pattern->setDisabledSiblings("false", 4, dsibs);
    }
    {
    wxWindow* dsibs[] = {is_cloud_pattern, cloud_sep_width, cloud_sep_depth, is_cloud_symd, is_cloud_symw};
    cloud_shape->setDisabledSiblings("Front", 5, dsibs);
    }
    is_cloudy->updateInputDisplay();


    //Associate panels with choices
    //Hermite (analytical)=0;SolTrace=1
    flux_model->setPanelObject("SolTrace", *soltrace_panel);    //"SolTrace"
    flux_model->updateInputDisplay();
    //Sun position=0;Hour/Day=1
    flux_time_type->setPanelObject("Sun position", *azel_panel);    //"Sun position"
    flux_time_type->setPanelObject("Hour/Day", *time_panel);    //"Hour/Day"
    flux_time_type->updateInputDisplay();
    //Simple aim points=0;Sigma aiming=1;Probability shift=2;Image size priority=3;Keep existing=4;Freeze tracking=5
    aim_method->setPanelObject("Sigma aiming", *sigma_panel);    //"Sigma aiming"
    aim_method->setPanelObject("Probability shift", *sigma_panel);    //probability shift, also uses sigma
    aim_method->setPanelObject("Image size priority", *sigma_panel);    //priority aiming
    aim_method->setDisabledSiblings("Sigma aiming", flux_dist);
    {
    wxWindow* dsibs[] = {flux_dist, norm_dist_sigma};
    aim_method->setDisabledSiblings("Image size priority", 2, dsibs);
    }
    aim_method->updateInputDisplay();
    //Triangular=0;Normal=1;Uniform=2
    flux_dist->setPanelObject("Normal", *dist_panel);    //normal distribution
    flux_dist->updateInputDisplay();

    
    //Create list control 
    wxArrayStr choices;
    choices.push_back("Slant range");
    choices.push_back("Radial distance");
    choices.push_back("Azimuthal position");
    choices.push_back("X position");
    choices.push_back("Y position");
    choices.push_back("X aim point");
    choices.push_back("Y aim point");
    choices.push_back("Z aim point");
    wxComboBox *lcsort = new wxComboBox(flux, wxID_ANY, choices[0], wxDefaultPosition, wxSize(wxDefaultSize.GetWidth(), _default_input_size.GetHeight()), choices, wxCB_DROPDOWN|wxCB_READONLY);
    wxStaticText *lclab = new wxStaticText(flux, wxID_ANY, "Sort heliostats by: ");
    _flux_lc_check = new wxCheckBox(flux, wxID_ANY, "Simulate only selected heliostats" );
    _flux_lc_check->SetValue(false);
    _flux_lc = new wxListCtrl(flux, wxID_ANY, wxDefaultPosition, wxSize(500,300), wxLC_REPORT);
     lcsort->Connect(wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( SPFrame::OnFluxLCSort ), NULL, this);
     UpdateFluxLC();    //just sets the column labels

    wxBoxSizer
        *col_sizer = new wxBoxSizer(wxHORIZONTAL),
        *col1 = new wxBoxSizer(wxVERTICAL),
        *col2 = new wxBoxSizer(wxVERTICAL);
    col1->Add(sbs0, 0, wxALL, 5);
    col1->Add(sbs1, 0, wxALL, 5);
    col1->Add(sbs3, 0, wxALL, 5);
    col_sizer->Add(col1, 0, wxRIGHT, 5);
    wxBoxSizer *top_sizer_2 = new wxBoxSizer(wxHORIZONTAL);
    col2->Add((30,1),0,0,0);
    top_sizer_2->Add(lclab, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    top_sizer_2->Add(lcsort, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    top_sizer_2->Add(_flux_lc_check, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 25);
    col2->Add(top_sizer_2, 0, 0, 0);
    col2->Add(_flux_lc, 1, wxEXPAND|wxALL, 5);
    col_sizer->Add(col2, 1, wxEXPAND, 0);
    flux_sizer->Add(top_sizer);
    flux_sizer->Add(col_sizer, 1, wxEXPAND, 0);
    //Add any input or output controls
    InputControl *inputs[] = {flux_model, flux_time_type, aim_method, sigma_limit_x, sigma_limit_y, flux_month, flux_day, flux_hour, 
                              flux_dist, norm_dist_sigma, flux_solar_az_in, flux_solar_el_in, x_res, y_res, min_rays, 
                              max_rays, seed, save_data, is_sunshape_err, is_optical_err, is_cloudy, cloud_shape, 
                              cloud_width, cloud_depth, cloud_opacity, cloud_skew, is_cloud_pattern, cloud_sep_width, 
                              cloud_sep_depth, cloud_loc_x, cloud_loc_y, is_cloud_symd, is_cloud_symw, flux_dni, 
                              is_load_raydata, is_save_raydata, raydata_file, NULL};
    int i=0;
    while(inputs[i] != NULL)
    {
        _input_map[ inputs[i]->getVarObject() ] = inputs[i];
        i++;
    }
    i=0;
    OutputControl *outputs[] = {flux_solar_az, flux_solar_el, NULL};
    while(outputs[i] != NULL)
    {
        _output_map[ outputs[i]->getVarObject() ] = outputs[i];
        i++;
    }


    flux->SetSizer(flux_sizer);
    flux->SetScrollbars(10, 10, flux->GetSize().GetWidth()/10, flux->GetSize().GetWidth()/10);
    }
    catch(std::exception &ex)
    {
        PopMessage("Caught exception while building the performance simulation page. " + (string)ex.what());
    }
    catch(...)
    {
        PopMessage("Caught exception while building the performance simulation page.");
    }
    return;
}

void SPFrame::UpdateFluxLC(int sortkey)
{
/* 
    Update the sorting of the listcontrol on the flux simluations page 
    
    Sorting choices are:
    0    Slant range
    1    Radial distance
    2    Azimuthal position
    3    X position
    4    Y position
    5    X aim point
    6    Y aim point
    7    Z aim point
    
    if the sortkey is less than 0, just reset the columns

    */

    //Set up the header columns
    _flux_lc->DeleteAllItems();
    _flux_lc->DeleteAllColumns();
    _flux_lc->InsertColumn(0, "#", 0, 30);
    _flux_lc->InsertColumn(1, "Geometry", 0, 80);
    _flux_lc->InsertColumn(2, "Location", 0, 120);
    _flux_lc->InsertColumn(3, "Radial pos.", 0, 80);
    _flux_lc->InsertColumn(4, "Azim. pos.", 0, 80);
    _flux_lc->InsertColumn(5, "Aim point", 0, 120);

    
    //get the heliostats
    Hvector *helios = _SF.getHeliostats();
    int nh = helios->size();
    if(nh == 0 || sortkey < 0) return;

    Heliostat *H;
    sp_point *loc, *aim;
    double az;
    

    /*    Collect heliostat information 
        This will display will be formattes as:
        
        |    Name       |        Location        |    Radial position    |    Azimuthal pos.    |        Aim point
        <Template name> <X pos>,<Y pos>,<Z pos>        <Radial dist>        <az. pos>          <X aim>,<Y aim>,<Z aim>

        The _flux_lc_map will contain entries in the following order:
              0          1         2          3       4       5       6       7       8      9 
        <temp. name> <slant> <radial pos> <az pos> <x pos> <y pos> <z pos> <x aim> <y aim> <z aim>
        
    */

    //Create a vector of heliostat pointers and a vector of the data we'd like to sort by
    _flcsort.resize(nh);
    vector<double> cdata(nh);

    for(int i=0; i<nh; i++)
    {
        
        _flcsort.at(i) = helios->at(i);
        
        if(sortkey == 0) 
			cdata.at(i) =  helios->at(i)->getSlantRange() ; 
        else if(sortkey == 1) 
			cdata.at(i) = helios->at(i)->getRadialPos(); 
        else if(sortkey == 2) 
			cdata.at(i) = helios->at(i)->getAzimuthalPos()*R2D; 
        else if(sortkey == 3) 
			cdata.at(i) = helios->at(i)->getLocation()->x; 
        else if(sortkey == 4) 
			cdata.at(i) = helios->at(i)->getLocation()->y; 
        else if(sortkey == 5) 
			cdata.at(i) = helios->at(i)->getAimPoint()->x; 
        else if(sortkey == 6) 
			cdata.at(i) = helios->at(i)->getAimPoint()->y; 
        else if(sortkey == 7) 
			cdata.at(i) = helios->at(i)->getAimPoint()->z; 
        else
			return;
    }

    //Sort
    quicksort(cdata, _flcsort, 0, nh-1);
    

    //Now fill out the list control based on the sorted heliostats
    wxString sloc, saim, sslant, srad, saz;
    wxFormatString 
        fmtpt = "%.1f, %.1f, %.1f",
        fmtd = "%.1f %s";
    
    for(int i=0; i<nh; i++)
    {    //for each heliostat

        //Get the info
        H = _flcsort.at(i);

        loc = H->getLocation();
        aim = H->getAimPoint();
        az = atan2(loc->x, loc->y)*R2D;
        
        double data[] = {
            H->getSlantRange(),
            H->getRadialPos(),
            az,
            loc->x, loc->y, loc->z,
            aim->x, aim->y, aim->z};

        sslant.sprintf(fmtd, data[0], "[m]");
        srad.sprintf(fmtd, data[1], "[m]");
        saz.sprintf(fmtd, az, "[deg]");
        sloc.sprintf(fmtpt, data[3], data[4], data[5]);
        saim.sprintf(fmtpt, data[6], data[7], data[8]);

        //get the sortable item
        cdata.push_back( data[ sortkey ] );

        wxListItem item;
        item.SetId(i);
        item.SetText( my_to_string(i+1) );
        _flux_lc->InsertItem(item);
        _flux_lc->SetItem(i, 1, *H->getMasterTemplate()->getHeliostatName() );
        _flux_lc->SetItem(i, 2, sloc);    //location
        _flux_lc->SetItem(i, 3, srad);    //radial pos    
        _flux_lc->SetItem(i, 4, saz);    //az pos
        _flux_lc->SetItem(i, 5, saim);    //aim

    }


}

void SPFrame::SolTraceFileExport(string fname)
{

/* 
    Method for exporting the SolTrace simulation setup to an .stinput file to be read directly by SolTrace.
    This is based on the formatting for SolTrace inputs as of version 2012.7.6.

    For reference on formatting, see the Soltrace source file "sysdata.cpp". This method also has many analogous 
    structures and procedures to the SPFrame::SolTraceFluxSimulation() method above.

    Note that SolTrace .stinput files do not have an entry for enabling/disabling the "use sun shape" and "use optical
    errors" checkboxes on the trace configuration page.
    */
    ST_System STSim;
    Hvector helios;
    if(! GetSelectedHeliostats(helios, _flux_lc_check->GetValue() ) ) return;
    Vect sun = Ambient::calcSunVectorFromAzZen( _variables.flux.flux_solar_az.Val() * D2R, (90. - _variables.flux.flux_solar_el.Val())*D2R );
    STSim.CreateSTSystem(_SF, helios, sun);
    
    FILE *fout = fopen(fname.c_str(), "w");
    if(! fout )
    {
        PopMessage("Error opening the SolTrace simulation output file. Please make sure the file is closed and the directory is not write-protected.", "File error", wxICON_ERROR|wxOK);
        return;
    }

    //Write header line
    DateTime dt;
    DTobj *now = dt.Now();
    fprintf(fout, "# SOLTRACE VERSION 2012.7.6 INPUT FILE -- GENERATED BY SOLARPILOT v%s on %d/%d/%d at %02d:%02d:%02d\n", 
        _software_version.c_str(), now->_mday, now->_month, now->_year, now->_hour, now->_min, now->_sec);
    //Write all of the output data to a string
    STSim.Write(fout);
    
    fclose(fout);
    return;
    
}

bool SPFrame::GetSelectedHeliostats(Hvector &helios, bool select_enabled)
{

//If a subset of heliostats are specified for flux simulation, collect those heliostat ids
    int *ids = 0, nh;
    if(select_enabled)
    {

        int ns = _flux_lc->GetSelectedItemCount();

        if(ns == 0)
        {

                int id = PopMessage("The \"Simulate only selected heliostats\" option is enabled on the Performance Simulation page, "
                "but no heliostats are selected. \n\n Continue simulation with all heliostats?", "Simulation warning", wxYES_NO|wxCANCEL);
            if(id != wxID_YES)
                return false;
        }
        else
        {
            ids = new int[ns+1];
            ids[0] = ns;

            int j=0;
            nh = _flux_lc->GetItemCount();
            long ind=-1;
            for(int i=0; i<nh; i++)
            {
                ind = _flux_lc->GetNextItem(ind, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
                if(ind != -1)
                {
                    ids[j+1] = _flcsort.at(ind)->getId();
                    j++;
                }
                if(j==ns) break;
            }
        }
    }
    
    //Collect the heliostats into a structure for passing
    if(ids != 0)
    {
        //Find heliostats
        for(int i=0; i<ids[0]; i++)
        {
            helios.push_back( (*_SF.getHeliostatsByID())[ids[i+1]] );
        }
    }
    else
    {
        //All heliostats
        int k=0;
        unordered_map<int,Heliostat*>::iterator it = _SF.getHeliostatsByID()->begin();
        while( it != _SF.getHeliostatsByID()->end() )
        {
            helios.push_back( it->second );
            it++; k++;
        }
        nh = k;
    }
    delete [] ids;    
    return true;
}

void SPFrame::OnDoPerformanceSimulation( wxCommandEvent &event)
{
    /* 
    The flux simulation button has been clicked. 

    This button serves as both the 'run' and 'cancel' button, depending on whether a flux
    simulation is currently running. If a 
    */
    try
    {
        //check whether a layout simulation is currently running. Both simulations cannot occur at the same time.
        if(_in_layout_simulation || _in_param_simulation || _in_user_param_simulation || _in_optimize_simulation)
        {
            //Notify user
            PopMessage("A simulation is currently underway. Please cancel the simulation or wait"
                "for completion before generating a new layout", "Error");
            event.Skip(); 
            return;
        }

        //If a flux simulation is underway, this instance means the user wants to cancel the simulation
        if(_in_flux_simulation)
        {
            if(_variables.flux.flux_model.mapval() == var_fluxsim::FLUX_MODEL::SOLTRACE) 
            {
                //Cancel the SolTrace simulation
                _cancel_simulation = true; 
            }
            else
            {
                //Cancel the analytical simulation
                _SF.CancelSimulation(); 
            }
            PopMessage("Simulation cancelled by user.", "Notice", wxICON_INFORMATION|wxOK);
            return;
        }
        // wxYieldIfNeeded();    //prevent GUI from freezing
    
        //Check if a layout is available for simulation
        if(_SF.getHeliostats()->size() == 0)
            // no field layout
            throw spexception("Performance simulation requires an existing layout. Please create a system geometry before simulating performance.");

        wxBitmapButton* evt_button = (wxBitmapButton*)event.GetEventObject();
        
        SetSimulationStatus(true, _in_flux_simulation, evt_button);
        StartSimTimer();

        bool simok = false;
        {
            Hvector helios;
            if (GetSelectedHeliostats(helios, _flux_lc_check->GetValue())) {
                _results.clear();
                _results.resize(1);
                simok = DoPerformanceSimulation(_SF, _variables, helios);
            }
        }

        StopSimTimer();
        SetSimulationStatus(false, _in_flux_simulation, evt_button);

        if(simok)
        {
            
            //Create the output pages
            Hvector helios;
            GetSelectedHeliostats(helios, _flux_lc_check->GetValue());
            _flux_frame->SetPlotType(0); // flux
            _flux_frame->SetPlotData( _SF, helios, 0 );

            //also update the layout plot
            _plot_frame->SetPlotData( _SF, FIELD_PLOT::EFF_TOT ); 
            //update the selection combo
            UpdateFieldPlotSelections();
            UpdateFluxPlotSelections();

            DoResultsPage(_results);
            
            UpdateLayoutGrid();    //update to show new aim points
            //Go to the new flux plot
            _page_panel->SetActivePage( pageNames.results_flux );
            this->GetSizer()->Layout();


        }
    }
    catch(std::exception &e)
    {
        SetSimulationStatus(false, _in_flux_simulation, (wxBitmapButton*)event.GetEventObject());
        PopMessage(e.what(), "OnDoPerformanceSimulation Error", wxICON_ERROR|wxOK);
    }
    catch(...)
    {
        SetSimulationStatus(false, _in_flux_simulation, (wxBitmapButton*)event.GetEventObject());
        PopMessage("Caught unspecified error. Simulation is terminating.", "OnDoPerformanceSimulation Error", wxICON_ERROR|wxOK);
        return;
    }
}

void SPFrame::OnFluxLCSort( wxCommandEvent &event )
{
    /* 
    The combobox for sorting the flux map heliostat list control has been changed. 
    Get the selected item number and call the list control update method.
    */
    try
    {
        int sel = ( (wxComboBox*)event.GetEventObject() )->GetSelection();
        UpdateFluxLC(sel);
    }
    catch(std::exception &e)
    {
        PopMessage(e.what(), "OnFluxLCSort Error", wxICON_ERROR|wxOK);
    }
    catch(...)
    {
        PopMessage("An unhandled error has occurred and the action could not be completed.", "OnFluxLCSort Error", wxICON_ERROR|wxOK);
        return;
    }
}

void SPFrame::OnStinputExport( wxCommandEvent &WXUNUSED(event))
{
    /* The button for exporting a SolTrace input file has been clicked */
    try
    {
        //Choose a file location
        wxFileDialog filedlg(this, "Select a file for export...", wxEmptyString, wxEmptyString, "SolTrace file (*.stinput;*.txt)|*.stinput;*.txt", wxFD_SAVE|wxFD_OVERWRITE_PROMPT );
        filedlg.CentreOnParent();

        //Show and process
        if(filedlg.ShowModal() == wxID_OK)
        {
            wxString info = filedlg.GetPath();
        
            SolTraceFileExport(info.ToStdString());
        
            //notify the user of a successful export
            wxString msg;
            msg.sprintf("%s\nFile successfully created.", info.c_str());
            PopMessage(msg, "Success");
        }
    }
    catch(std::exception &e)
    {
        PopMessage(e.what(), "OnStinputExport Error", wxICON_ERROR|wxOK);
    }
    catch(...)
    {
        PopMessage("An unhandled error has occurred and the action could not be completed.", "OnStinputExport Error", wxICON_ERROR|wxOK);
        return;
    }
}

void SPFrame::OnSTRayDataButton( wxCommandEvent &WXUNUSED(event))
{
    /* 
    Choose the location where the ray data will be exported.
    */
    try
    {
        //Get the file name to save as
        wxFileDialog filedlg(this, "Choose file location...", wxEmptyString, wxEmptyString, "CSV files (*.csv)|*.csv", wxFD_SAVE|wxFD_OVERWRITE_PROMPT );
        filedlg.CentreOnParent();

        //Show and process
        if(filedlg.ShowModal() == wxID_OK)
        {
            _variables.flux.save_data_loc.val = filedlg.GetPath().c_str();
        }
        else
        {
            return;
        }
    }
    catch(std::exception &e)
    {
        PopMessage(e.what(), "OnSTRayDataButton Error", wxICON_ERROR|wxOK);
    }
    catch(...)
    {
        PopMessage("An unhandled error has occurred and the action could not be completed.", "OnSTRayDataButton Error", wxICON_ERROR|wxOK);
        return;
    }
}
