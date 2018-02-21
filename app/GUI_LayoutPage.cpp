#include "GUI_main.h"
#include "IOUtil.h"

using namespace std;


void SPFrame::CreateLayoutPage(wxScrolledWindow *parent)
{
    unsigned int i;
    //Create the master receiver page and return a pointer
#if _USE_WINDOW_BG 
    parent->SetBackgroundColour(_background_colour);
#endif    //-------Create objects

    wxBoxSizer *main_sizer = new wxBoxSizer(wxVERTICAL);    //The main sizer for the parent
    wxBoxSizer *top_sizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *c1_sizer = new wxBoxSizer(wxVERTICAL);    //First column sizer
    wxBoxSizer *c2_sizer = new wxBoxSizer(wxVERTICAL);    //Second column sizer
    
    //************Design values group************************
    
    wxStaticBox *sb_0 = new wxStaticBox(parent, -1, "Design values");
    wxStaticBoxSizer *sbs_0 = new wxStaticBoxSizer(sb_0, wxVERTICAL);
    wxPanel *sun_loc_p = new wxPanel(parent);
    wxBoxSizer *sun_loc_s = new wxBoxSizer(wxVERTICAL);
    InputControl
        *q_des = new InputControl(parent, wxID_ANY,  _variables.sf.q_des),
        *dni_des = new InputControl(parent, wxID_ANY,  _variables.sf.dni_des),
        *sun_loc_des = new InputControl(parent, wxID_ANY,  _variables.sf.sun_loc_des),
        *sun_az_des_user = new InputControl(sun_loc_p, wxID_ANY,  _variables.sf.sun_az_des_user),
        *sun_el_des_user = new InputControl(sun_loc_p, wxID_ANY,  _variables.sf.sun_el_des_user);

    sun_loc_s->Add(sun_az_des_user);
    sun_loc_s->Add(sun_el_des_user);
    sun_loc_p->SetSizer(sun_loc_s);
    //Summer solstice=0;Equinox=1;Winter solstice=2;Zenith=3;Other..=4
    sun_loc_des->setPanelObject("Other..", *sun_loc_p);
    
    sbs_0->Add(q_des);
    sbs_0->Add(dni_des);
    sbs_0->Add(sun_loc_des);
    sbs_0->Add(sun_loc_p, 0, wxALL, 5);
    sun_loc_des->updateInputDisplay();

    wxPanel *design_p1 = new wxPanel(parent);
    sbs_0->Add(design_p1, 0, 0, 0);
    
    //*******************************************************


    //************Field configuration group*********************
    wxStaticBox *sb_1 = new wxStaticBox(parent, -1, "Field configuration");
    wxPanel *design_p2 = new wxPanel(parent);
    InputControl
        *tht = new InputControl(parent, wxID_ANY,  _variables.sf.tht);

    InputControl
        *layout_method = new InputControl(parent, wxID_ANY,  _variables.sf.layout_method);
    
    //add relevant inputs for the layout methods
    //Radial Stagger=1;Cornfield=2;User-defined=3
    wxPanel *layout_rad_stagger_panel = new wxPanel(parent);
    layout_method->setPanelObject("Radial Stagger",*layout_rad_stagger_panel);
    wxPanel *layout_cornfield_panel = new wxPanel(parent);
    layout_method->setPanelObject("Cornfield", *layout_cornfield_panel);
    
    InputControl
        *rad_spacing_method = new InputControl(layout_rad_stagger_panel, wxID_ANY,  _variables.sf.rad_spacing_method),
        *az_spacing = new InputControl(layout_rad_stagger_panel, wxID_ANY,  _variables.sf.az_spacing),
        *spacing_reset = new InputControl(layout_rad_stagger_panel, wxID_ANY,  _variables.sf.spacing_reset),
        *trans_limit_fact = new InputControl(layout_rad_stagger_panel, wxID_ANY,  _variables.sf.trans_limit_fact),
        *is_sliprow_skipped = new InputControl(layout_rad_stagger_panel, wxID_ANY, _variables.sf.is_sliprow_skipped),
        *slip_plane_blocking = new InputControl(layout_rad_stagger_panel, wxID_ANY, _variables.sf.slip_plane_blocking),
        *row_spacing_x = new InputControl(layout_cornfield_panel, wxID_ANY,  _variables.sf.row_spacing_x),
        *row_spacing_y = new InputControl(layout_cornfield_panel, wxID_ANY,  _variables.sf.row_spacing_y),
        *xy_field_shape = new InputControl(layout_cornfield_panel, wxID_ANY,  _variables.sf.xy_field_shape),
        *xy_rect_aspect = new InputControl(layout_cornfield_panel, wxID_ANY,  _variables.sf.xy_rect_aspect);
        
    xy_field_shape->setDisabledSiblings("Hexagon", xy_rect_aspect);
    xy_field_shape->setDisabledSiblings("Undefined", xy_rect_aspect );
    layout_method->setDisabledSiblings("Cornfield", rad_spacing_method );
    rad_spacing_method->setDisabledSiblings("DELSOL Empirical Fit", az_spacing );
    rad_spacing_method->addDisabledSiblings("DELSOL Empirical Fit", is_sliprow_skipped );
    rad_spacing_method->addDisabledSiblings("DELSOL Empirical Fit", slip_plane_blocking );
    rad_spacing_method->setDisabledSiblings("Eliminate blocking", trans_limit_fact );
    
    wxBoxSizer *layout_rad_stagger_s = new wxBoxSizer(wxVERTICAL);
    layout_rad_stagger_s->Add(rad_spacing_method);
    layout_rad_stagger_s->Add(az_spacing);
    layout_rad_stagger_s->Add(spacing_reset);
    layout_rad_stagger_s->Add(trans_limit_fact);
    layout_rad_stagger_s->Add(is_sliprow_skipped);
    layout_rad_stagger_s->Add(slip_plane_blocking);
    layout_rad_stagger_panel->SetSizer(layout_rad_stagger_s);
    wxBoxSizer *layout_cornfield_s = new wxBoxSizer(wxVERTICAL);
    layout_cornfield_s->Add(row_spacing_x);
    layout_cornfield_s->Add(row_spacing_y);
    layout_cornfield_s->Add(xy_field_shape);
    layout_cornfield_s->Add(xy_rect_aspect);
    layout_cornfield_panel->SetSizer(layout_cornfield_s);

    layout_method->updateInputDisplay();

    wxStaticBoxSizer *sbs_1 = new wxStaticBoxSizer(sb_1, wxVERTICAL);
    sbs_1->Add(tht);
    
    wxBoxSizer *design_s2 = new wxBoxSizer(wxVERTICAL);
    design_p2->SetSizer(design_s2);
    sbs_1->Add(design_p2, 0, 0, 0);

    sbs_1->Add(layout_method);
    sbs_1->Add(layout_rad_stagger_panel, 0, 0, 0);
    sbs_1->Add(layout_cornfield_panel, 0, 0, 0);
    
    //************ end field configuration group *****************

    //************ The field boundaries group *****************
    wxStaticBox *sb_2 = new wxStaticBox(parent, -1, "Field Boundaries");
    wxStaticBoxSizer *sbs_2 = new wxStaticBoxSizer(sb_2, wxVERTICAL);
    InputControl
        *accept_min = new InputControl(parent, wxID_ANY,  _variables.sf.accept_min),
        *accept_max = new InputControl(parent, wxID_ANY,  _variables.sf.accept_max);
    sbs_2->Add(accept_min);
    sbs_2->Add(accept_max);

    OutputControl 
        *radmin_m = new OutputControl(parent, wxID_ANY, _variables.land.radmin_m), 
        *radmax_m = new OutputControl(parent, wxID_ANY, _variables.land.radmax_m);
    sbs_2->Add(radmin_m, 0, 0, 0);
    sbs_2->Add(radmax_m, 0, 0, 0);

    InputControl
        *is_bounds_scaled = new InputControl(parent, wxID_ANY,  _variables.land.is_bounds_scaled),
        *max_scaled_rad = new InputControl(parent, wxID_ANY,  _variables.land.max_scaled_rad),
        *min_scaled_rad = new InputControl(parent, wxID_ANY,  _variables.land.min_scaled_rad),
        *is_bounds_fixed = new InputControl(parent, wxID_ANY,  _variables.land.is_bounds_fixed),
        *max_fixed_rad = new InputControl(parent, wxID_ANY,  _variables.land.max_fixed_rad),
        *min_fixed_rad = new InputControl(parent, wxID_ANY,  _variables.land.min_fixed_rad);
    
    {
        wxWindow* dsibs[] = {max_scaled_rad, min_scaled_rad};
        is_bounds_scaled->setDisabledSiblings("false", 2, dsibs);
    }
    {
        wxWindow* dsibs[] = {max_fixed_rad, min_fixed_rad};
        is_bounds_fixed->setDisabledSiblings("false", 2, dsibs);
    }
    
    sbs_2->Add(is_bounds_scaled);
    sbs_2->Add(max_scaled_rad);
    sbs_2->Add(min_scaled_rad);
    sbs_2->Add(is_bounds_fixed);
    sbs_2->Add(max_fixed_rad);
    sbs_2->Add(min_fixed_rad);

    //add the bounds array part
    InputControl
        *is_bounds_array = new InputControl(parent, wxID_ANY,  _variables.land.is_bounds_array),
        *is_exclusions_relative = new InputControl(parent, wxID_ANY, _variables.land.is_exclusions_relative),
        *tower_offset_x = new InputControl(parent, wxID_ANY, _variables.land.tower_offset_x),
        *tower_offset_y = new InputControl(parent, wxID_ANY, _variables.land.tower_offset_y);

    sbs_2->Add(is_bounds_array);
    sbs_2->Add(is_exclusions_relative);
    sbs_2->Add( tower_offset_x );
    sbs_2->Add( tower_offset_y );
    
    wxButton *b_import_1 = new wxButton(parent, wxID_ANY, "Import", wxDefaultPosition, _grid_but_size);
    wxButton *b_export_1 = new wxButton(parent, wxID_ANY, "Export", wxDefaultPosition, _grid_but_size);
    wxBoxSizer *bounds_sizer = new wxBoxSizer(wxVERTICAL);
    //add the buttons in a horizontal sizer
    wxBoxSizer *b_sizer_1 = new wxBoxSizer(wxHORIZONTAL);
    b_sizer_1->Add(b_import_1, 0, wxALL, 5);
    b_sizer_1->Add(b_export_1, 0, wxALL, 5);
    _bounds_ct = new wxSpinCtrl(parent, wxID_ANY, "4", wxDefaultPosition, _spin_ctrl_size, wxSP_ARROW_KEYS | wxALIGN_RIGHT, 1, 100000);
    wxStaticText *bounds_ct_lab = new wxStaticText(parent, wxID_ANY, "Rows");
    b_sizer_1->Add((15,15), 0, 0, 0);    //space
    b_sizer_1->Add(bounds_ct_lab, 0, wxTOP, 10);
    b_sizer_1->Add(_bounds_ct, 0, wxTOP|wxLEFT, 5);
    bounds_sizer->Add(b_sizer_1, 0, wxALIGN_CENTER, 0);
    //create the land boundary grid
    _bounds_grid = new wxGrid(parent, wxID_ANY, wxDefaultPosition, wxSize(320,150));
    _bounds_grid->CreateGrid(4,4);
    _bounds_grid->SetRowLabelSize(25);
    _bounds_grid->SetDefaultColSize(100, true);
    for(i=0; i<2; i++)
        _bounds_grid->SetColSize(i, 35);
    _bounds_grid->SetMargins(0,0);
    _bounds_grid->SetColLabelValue(0,wxT("Type"));
    _bounds_grid->SetColLabelValue(1,wxT("No."));
    _bounds_grid->SetColLabelValue(2,wxT("X"));
    _bounds_grid->SetColLabelValue(3,wxT("Y"));
    //display any data from the variable map in the bounds grid
    UpdateLandBoundsGrid(_variables);
    bounds_sizer->Add(_bounds_grid, 0, wxALIGN_CENTER|wxALL, 5);
    
    //disable the controls when "is_bounds_array" is false
    {
    wxWindow* dsibs[] = {b_import_1, b_export_1, _bounds_ct, bounds_ct_lab, _bounds_grid, tower_offset_x, tower_offset_y, is_exclusions_relative};
    is_bounds_array->addDisabledSiblings("false", 8, dsibs);
    }


    sbs_2->Add(bounds_sizer, 0, wxEXPAND, 0);
    //Bind the buttons to their events
    b_import_1->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnBoundsImport ), NULL, this);
    b_export_1->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnBoundsExport ), NULL, this);
    _bounds_ct->Connect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxCommandEventHandler( SPFrame::OnBoundsCount ), NULL, this);
    _bounds_grid->Connect(wxEVT_GRID_CELL_CHANGED, wxGridEventHandler( SPFrame::OnBoundsEdit ), NULL, this);

    //***************** end of the field boundaries group *****************

    //************* Field layout simulation steps group ***********************
    wxStaticBox *sb_3 = new wxStaticBox(parent, wxID_ANY, wxT("Design point definition"));
    wxStaticBoxSizer *sbs_3 = new wxStaticBoxSizer(sb_3, wxVERTICAL);
    
    InputControl *hsort_method = new InputControl(parent, wxID_ANY,  _variables.sf.hsort_method);
    _input_map[ hsort_method->getVarObject() ] = hsort_method;
    
    //create the combo. This is a bit too customized too use the InputControl system
    //First get the options from the variable map
    _design_combo = new InputControl(parent, wxID_ANY,  _variables.sf.des_sim_detail); 
    _input_map[ _design_combo->getVarObject() ] = _design_combo;
    
    //depending on the selection of des_sim_detail, different display options will be enabled
    //First option: Subset of days and hours
    _design_combo_panel = new wxPanel(parent);
    _design_grid = new wxGrid(_design_combo_panel, wxID_ANY, wxDefaultPosition, wxSize(750, 125) );
    _design_grid->CreateGrid(1,1);
    _design_grid->Connect(wxEVT_GRID_CELL_CHANGED, wxGridEventHandler( SPFrame::OnDesignEdit ), NULL, this);
    _design_ct = new wxSpinCtrl(_design_combo_panel, wxID_ANY, "4", wxDefaultPosition, _spin_ctrl_size);
    _design_ct->SetRange(1,100);
    _design_simallhrs = new wxCheckBox(_design_combo_panel, wxID_ANY, wxT("Simulate all daytime hours for specified days") );
    _design_simallhrs->SetValue(false); 
    
    wxString msg1 = wxT("Note: The sun position will be calculated at the\ninstantaneous time specified in the table.");
    _design_steptext = new wxStaticText(_design_combo_panel, wxID_ANY, msg1);
    _design_steptext->SetForegroundColour(_helptext_colour);
    
    wxStaticText *design_lab = new wxStaticText(_design_combo_panel, wxID_ANY, wxT("Rows") );
    wxBoxSizer *design_panel_s1 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *design_top_sizer = new wxBoxSizer(wxHORIZONTAL);
    design_top_sizer->Add(_design_ct, 0, wxALL, 5);
    design_top_sizer->Add(design_lab, 0, wxTOP, 10);
    design_top_sizer->Add((45,20), 0, 0, 0);
    design_top_sizer->Add(_design_simallhrs, 0, wxTOP, 10);
    design_top_sizer->Add(_design_steptext, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 30);
    design_panel_s1->Add(design_top_sizer, 0, 0, 0);
    design_panel_s1->Add(_design_grid, 0, wxALL, 5);
    _design_combo_panel->SetSizer(design_panel_s1);
    
    //Second option: Single design point
    /* use same objects as first option, but reduce the table down to a single entry */

    //Third option: Do not filter any heliostats
    /* this will hide all objects*/

    //Fourth option: Perform an annual simulation
    /* This will hide all objects. Show the warning message */
    wxString msg = wxT("Note:\nAnnual simulation of an hourly weather file can take up to an hour, depending on processor speed.")
                        wxT("\nTo reduce the number of simulations, use the Limited Annual Simulation option or specify the desired")
                        wxT("\nsimulation time steps using the Subset of Days and Hours option");
    _design_helptext = new wxStaticText(parent, wxID_ANY, msg);
    _design_helptext->SetForegroundColour(_helptext_colour);
    
    //Fifth option: Perform a limited annual simulation
    //how many days will the simulation entail?
    _design_ndays = new wxSpinCtrl(parent, wxID_ANY, _variables.sf.des_sim_ndays.as_string(), wxDefaultPosition, _spin_ctrl_size);
    _design_ndays->SetRange(1,365);
    _design_ndays_lab = new wxStaticText(parent, wxID_ANY, _variables.sf.des_sim_ndays.short_desc );
    
    _design_nhours = new wxSpinCtrl(parent, wxID_ANY, _variables.sf.des_sim_nhours.as_string(), wxDefaultPosition, _spin_ctrl_size);
    _design_nhours->SetRange(1,5);
    _design_nhours_lab = new wxStaticText(parent, wxID_ANY, _variables.sf.des_sim_nhours.short_desc );
    
    _design_days_display = new wxListCtrl(parent, wxID_ANY, wxDefaultPosition, wxSize(600,125), wxLC_REPORT);    //A listctrl that displays the days that will be simulated
    
    wxBoxSizer *design_limann_sizer = new wxBoxSizer(wxHORIZONTAL);
    design_limann_sizer->Add(_design_ndays, 0, wxALL, 5);
    design_limann_sizer->Add(_design_ndays_lab, 0, wxTOP, 10);
    design_limann_sizer->Add(_design_nhours, 0, wxALL, 5);
    design_limann_sizer->Add(_design_nhours_lab, 0, wxTOP, 10);
    sbs_3->Add(hsort_method);
    sbs_3->Add(_design_combo);
    sbs_3->Add(_design_helptext, 0, wxALL, 5);
    sbs_3->Add(_design_combo_panel, 0, 0, 0);
    sbs_3->Add(design_limann_sizer, 0, 0, 0);
    sbs_3->Add(_design_days_display, 0, wxALL, 5);
    
    //bind the selection
    _design_combo->Connect(wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( SPFrame::OnDesignSelect ), NULL, this);
    _design_simallhrs->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( SPFrame::OnDesignSelect ), NULL, this);
    _design_ct->Connect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxCommandEventHandler( SPFrame::OnDesignSimhrsChanged ), NULL, this);
    _design_ndays->Connect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxCommandEventHandler( SPFrame::OnDesignNdaysChanged ), NULL, this);
    _design_nhours->Connect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxCommandEventHandler( SPFrame::OnDesignNhrsChanged ), NULL, this);
    //*********************************************************************

    //************* Advanced layout options group ********************************************************
    InputControl
        *is_opt_zoning = new InputControl(parent, wxID_ANY,  _variables.sf.is_opt_zoning),
        *min_zone_size_rad = new InputControl(parent, wxID_ANY,  _variables.sf.min_zone_size_rad),
        *max_zone_size_rad = new InputControl(parent, wxID_ANY,  _variables.sf.max_zone_size_rad),
        *min_zone_size_az = new InputControl(parent, wxID_ANY,  _variables.sf.min_zone_size_az),
        *max_zone_size_az = new InputControl(parent, wxID_ANY,  _variables.sf.max_zone_size_az),
        *zone_div_tol = new InputControl(parent, wxID_ANY,  _variables.sf.zone_div_tol),
        *is_prox_filter = new InputControl(parent, wxID_ANY,  _variables.sf.is_prox_filter),
        *prox_filter_frac = new InputControl(parent, wxID_ANY,  _variables.sf.prox_filter_frac);
    {
    wxWindow* dsibs[] = {max_zone_size_rad, max_zone_size_az, min_zone_size_rad, min_zone_size_az, zone_div_tol};
    is_opt_zoning->setDisabledSiblings("false",    5, dsibs);
    }
    is_prox_filter->setDisabledSiblings("false", prox_filter_frac);

    wxStaticBox *sb_4 = new wxStaticBox(parent, wxID_ANY, wxT("Advanced layout options"));
    wxStaticBoxSizer *sbs_4 = new wxStaticBoxSizer(sb_4, wxVERTICAL);
    sbs_4->Add(is_opt_zoning);
    sbs_4->Add(min_zone_size_rad);
    sbs_4->Add(max_zone_size_rad);
    sbs_4->Add(min_zone_size_az);
    sbs_4->Add(max_zone_size_az);
    sbs_4->Add(zone_div_tol);
    sbs_4->Add(new wxStaticLine(parent, wxID_ANY, wxDefaultPosition,wxSize(1,1), wxLI_HORIZONTAL), 0, wxEXPAND|wxLEFT|wxRIGHT, 5);
    sbs_4->Add(is_prox_filter);
    sbs_4->Add(prox_filter_frac);
    //*********************************************************************

    //Add the group ot the first column sizer
    c1_sizer->Add(sbs_0, 0, wxALL, 5);
    c1_sizer->Add(sbs_1, 0, wxALL, 5);
    c1_sizer->Add(sbs_4, 0, 0, 0);
    c2_sizer->Add(sbs_2, 0, wxALL, 5);
    
    //*******Add the page columns
    main_sizer->Add(sbs_3, 0, 0, 0);
    top_sizer->Add(c1_sizer, 0, wxEXPAND, 0);
    top_sizer->Add(c2_sizer, 0, wxEXPAND, 0);
    main_sizer->Add(top_sizer, 0, wxEXPAND, 0);
    parent->SetSizer(main_sizer);
    
    //**********Add all of the inputs and outputs to the map*************
    InputControl *inputs[] = {dni_des, q_des, tht, layout_method, az_spacing, spacing_reset,  rad_spacing_method, accept_min, accept_max, is_bounds_scaled, 
        max_scaled_rad, min_scaled_rad, is_bounds_fixed, max_fixed_rad, min_fixed_rad, is_bounds_array, sun_loc_des, is_exclusions_relative,
        sun_el_des_user, sun_az_des_user, row_spacing_x, row_spacing_y, xy_field_shape, xy_rect_aspect, is_prox_filter, 
        prox_filter_frac, is_opt_zoning, max_zone_size_rad, max_zone_size_az, min_zone_size_rad, min_zone_size_az, 
        zone_div_tol, trans_limit_fact, is_sliprow_skipped, slip_plane_blocking, tower_offset_x, tower_offset_y, NULL};
    OutputControl *outputs[] = {radmax_m, radmin_m , NULL};

    i=0;
    while(inputs[i] != NULL)
    {
        _input_map[ inputs[i]->getVarObject() ] = inputs[i];
        i++;
    }
    i=0;
    while(outputs[i] != NULL)
    {
        _output_map[ outputs[i]->getVarObject() ] = outputs[i];
        i++;
    }

    parent->SetScrollbars(10, 10, parent->GetSize().GetWidth()/10, parent->GetSize().GetWidth()/10);
    UpdateDesignSelect(_variables.sf.des_sim_detail.mapval(), _variables);

    return;
}

void SPFrame::OnBoundsCount( wxCommandEvent &event)
{
    GridCount( (wxSpinCtrl*)event.GetEventObject(), _bounds_grid );
}

void SPFrame::UpdateLandBoundsGrid( var_map &V) 
{
    /*
    Take strings "incs" and "excs" and fill the _bounds_grid array with the data in each. "data" should be of the
    format:
    [POLY][P]x1,y1,z1[P]x2,y2,z3... [POLY][P]...
    */

    std::vector<std::vector<sp_point> > *incs = &V.land.inclusions.val;
    std::vector<std::vector<sp_point> > *excs = &V.land.exclusions.val;

    //If the strings are empty, return
    if( incs->size() == 0 && excs->size() == 0 ) return;

    //vector<string>
    //    i_polys = split(incs, "[POLY]"),
    //    e_polys = split(excs, "[POLY]"),
    //    poly,
    //    pt;
    
    //Clear the bounds grid
    if(_bounds_grid->GetNumberRows() > 0 )
        _bounds_grid->DeleteRows(0,_bounds_grid->GetNumberRows());

    //Add each row of data
    int ict=0, ect=0, k, row=0;
    //First the inclusions
    for(unsigned int i=0; i<incs->size(); i++)
    {    //For each polygon in the list
        
        //Check to make sure the polygon doesn't have a closing point equal to the starting point
        int np = incs->at(i).size();
        if(incs->at(i).at(0) == incs->at(i).back())
            np += -1;

        for(int j=0; j<np; j++)
        {    //For each point in the polygon
            _bounds_grid->AppendRows(1);
            _bounds_grid->SetCellValue(row, 0, "I");    //Set the inclusion/exclusion flag
            _bounds_grid->SetCellValue(row, 1, my_to_string(ict));    //Set the polygon number
            sp_point *pt = &incs->at(i).at(j);
            for(k=0; k<2; k++)
            {
                _bounds_grid->SetCellValue( row, k+2, my_to_string( (*pt)[k] ));
            }
            row++;
        }
        ict++;
    }
    //Now the exclusions
    for(unsigned int i=0; i<excs->size(); i++)
    {    //For each polygon in the list
        
        //Check to make sure the polygon doesn't have a closing point equal to the starting point
        int np = excs->at(i).size();
        if(excs->at(i).at(0) == excs->at(i).back())
            np += -1;

        for(int j=0; j<np; j++)
        {    //For each point in the polygon
            _bounds_grid->AppendRows(1);
            _bounds_grid->SetCellValue(row, 0, "E");    //Set the inclusion/exclusion flag
            _bounds_grid->SetCellValue(row, 1, my_to_string(ect));    //Set the polygon number
            sp_point *pt = &excs->at(i).at(j);
            for(k=0; k<2; k++)
            {
                _bounds_grid->SetCellValue(row, k+2, my_to_string( (*pt)[k] ));
            }
            row++;
        }
        ect++;
    }
    //Update the display count in the spinctrl
    _bounds_ct->SetValue(row);
    
}

void SPFrame::UpdateDesignSelect( int sel, var_map &vset )
{
    /* 
    A new design point option was selected from the combobox.. Update the design point options on the
    field layout page.

    OPTIONS
    1) Subset of days/hours=2;
    2) Single design point=1;
    3) Do not filter heliostats=0;
    4) Annual simulation=3;
    5) Limited annual simulation=4

    The goal is to construct the weather data array. 

    */

    //Call the method to fill in weather data and time step
    interop::GenerateSimulationWeatherData(vset, sel, _local_wfdat);

    if(sel == var_solarfield::DES_SIM_DETAIL::SUBSET_OF_DAYSHOURS)
    {    //1) Subset of days/hours=2;
        if(! _design_combo_panel->IsShown() ) _design_combo_panel->Show();
        if(_design_days_display->IsShown() ) _design_days_display->Hide();
        if(_design_ndays->IsShown() ) {_design_ndays->Hide(); _design_ndays_lab->Hide();}
        if(_design_nhours->IsShown() )
        {_design_nhours->Hide(); _design_nhours_lab->Hide(); }
            if(_design_helptext->IsShown()) _design_helptext->Hide();
        

        //clear the grid
        _design_grid->DeleteRows(0, _design_grid->GetNumberRows());
        _design_grid->DeleteCols(0, _design_grid->GetNumberCols());
        if(! _design_ct->IsEnabled()) _design_ct->Enable();
        _design_ct->SetValue(5);
        /*
        The grid should have the format:
        day of month | hour of day | month | DNI | T dry bulb | pressure | wind speed

        --or for the case of all daytime hours, use weather or DNI model--

        day of month | month
        */
        if(! _design_simallhrs->GetValue())
        { //full grid
            _design_grid->AppendCols(7);
            _design_grid->SetDefaultColSize(75, true);
            _design_grid->AppendRows(5);

            _design_grid->SetColLabelValue(0, wxT("Day of\nMonth"));
            _design_grid->SetColLabelValue(1, wxT("Hour of\nDay"));
            _design_grid->SetColLabelValue(2, wxT("Month"));
            _design_grid->SetColLabelValue(3, wxT("DNI\n[W/m2]"));
            _design_grid->SetColLabelValue(4, wxT("Drybulb T.\n[Deg C]"));
            _design_grid->SetColLabelValue(5, wxT("Pressure\n[Atm]"));
            _design_grid->SetColLabelValue(6, wxT("Wind speed\n[m/s]"));
        }
        else
        {
            _design_grid->AppendCols(2,true);
            _design_grid->SetDefaultColSize(75, true);
            _design_grid->AppendRows(5);
            _design_grid->SetColLabelValue(0, wxT("Day of\nMonth"));
            _design_grid->SetColLabelValue(1, wxT("Month"));
        }
        _design_combo_panel->GetParent()->Layout();

    }
    else if(sel == var_solarfield::DES_SIM_DETAIL::SINGLE_SIMULATION_POINT)
    {    //2) Single design point=1;
        /* Use a single design point. Reduce the number of entries in the table to one */
        if(! _design_combo_panel->IsShown() ) _design_combo_panel->Show();
        if(_design_days_display->IsShown() ) _design_days_display->Hide();
        if(_design_ndays->IsShown() ) {_design_ndays->Hide(); _design_ndays_lab->Hide();}
        if(_design_nhours->IsShown() )
        {_design_nhours->Hide(); _design_nhours_lab->Hide(); }
            if(_design_helptext->IsShown()) _design_helptext->Hide();
        
        //clear the grid
        _design_grid->DeleteRows(0, _design_grid->GetNumberRows());
        _design_grid->DeleteCols(0, _design_grid->GetNumberCols());
        _design_ct->SetValue(1);
        if(! _design_ct->IsEnabled()) _design_ct->Enable();
        
        //Disable the ability to control the number of rows
        _design_ct->Disable();

        if(! _design_simallhrs->GetValue())
        { //full grid
            _design_grid->AppendCols(7);
            _design_grid->SetDefaultColSize(75, true);
            _design_grid->AppendRows(1);

            _design_grid->SetColLabelValue(0, wxT("Day of\nMonth"));
            _design_grid->SetColLabelValue(1, wxT("Hour of\nDay"));
            _design_grid->SetColLabelValue(2, wxT("Month"));
            _design_grid->SetColLabelValue(3, wxT("DNI\n[W/m2]"));
            _design_grid->SetColLabelValue(4, wxT("Drybulb T.\n[Deg C]"));
            _design_grid->SetColLabelValue(5, wxT("Pressure\n[Atm]"));
            _design_grid->SetColLabelValue(6, wxT("Wind speed\n[m/s]"));

        }
        else
        {
            _design_grid->AppendCols(2,true);
            _design_grid->SetDefaultColSize(75, true);
            _design_grid->AppendRows(5);
            _design_grid->SetColLabelValue(0, wxT("Day of\nMonth"));
            _design_grid->SetColLabelValue(1, wxT("Month"));

        }
        _design_combo_panel->GetParent()->Layout();

        double step[7];

        //dom, hod, mo, dni, tdry, p, vwind
        vset.sf.sim_step_data.Val().getStep(0, step[0], step[1], step[2], step[3], step[3], step[4], step[5], step[6]);

        vector<string> vals;
        for(int i=0; i<7; i++)
            vals.push_back( my_to_string( step[i] ) );

        //Autofill with the default simulation step
        for(int i=0; i<7; i++)
        {
            _design_grid->SetCellValue(0,i,vals[i]);
        }

    }
    else if(sel == var_solarfield::DES_SIM_DETAIL::DO_NOT_FILTER_HELIOSTATS)
    {    //3) Do not filter heliostats=0;
        if( _design_combo_panel->IsShown() ) 
            _design_combo_panel->Hide();
        
        if( _design_ndays->IsShown() ) 
        {
            _design_ndays->Hide(); 
            _design_ndays_lab->Hide();
        }
        
        if(_design_nhours->IsShown() )
        {
            _design_nhours->Hide(); 
            _design_nhours_lab->Hide(); 
        }
        
        if(_design_days_display->IsShown() ) 
            _design_days_display->Hide();

        if(_design_helptext->IsShown()) 
            _design_helptext->Hide();
        
    }
    else if(sel == var_solarfield::DES_SIM_DETAIL::ANNUAL_SIMULATION)
    {    //4) Annual simulation=3;
        if( _design_combo_panel->IsShown() ) 
            _design_combo_panel->Hide();
        if( _design_ndays->IsShown() ) 
        {
            _design_ndays->Hide(); 
            _design_ndays_lab->Hide();
        }

        if(_design_nhours->IsShown() )
        {
            _design_nhours->Hide(); 
            _design_nhours_lab->Hide(); 
        }

        if(_design_days_display->IsShown() ) 
            _design_days_display->Hide();

        _design_helptext->SetLabel( 
            wxT("Note:\nAnnual simulation of an hourly weather file can take up to an hour, depending on processor speed.")
            wxT("\nTo reduce the number of simulations, use the Limited Annual Simulation option or specify the desired")
            wxT("\nsimulation time steps using the Subset of Days and Hours option") 
            );
        if(! _design_helptext->IsShown()) _design_helptext->Show();
        
        _design_helptext->GetParent()->Layout();
    }
    else if(sel == var_solarfield::DES_SIM_DETAIL::LIMITED_ANNUAL_SIMULATION || sel == var_solarfield::DES_SIM_DETAIL::REPRESENTATIVE_PROFILES)
    {    //5) Limited annual simulation=4 || Representative profiles=5
        
        if( _design_combo_panel->IsShown() ) 
            _design_combo_panel->Hide();
        
        /*Show the input for the number of days to use*/
        if(! _design_ndays->IsShown() ) 
        {
            _design_ndays->Show(); 
            _design_ndays_lab->Show();
        }

        if(! _design_nhours->IsShown() )
        {
            _design_nhours->Show(); 
            _design_nhours_lab->Show(); 
        }

        if(! _design_days_display->IsShown() ) 
            _design_days_display->Show();

        if(_design_helptext->IsShown()) 
            _design_helptext->Hide();
        
        //Clear the display if it has any rows or columns
        if(_design_days_display->GetColumnCount() > 0) 
            _design_days_display->DeleteAllColumns();

        if(_design_days_display->GetItemCount() > 0) 
            _design_days_display->DeleteAllItems();

        wxListItem col0, col1, col2, col3, col4, col5;
        col0.SetId(0); 
        col0.SetText( wxT("Day No.") );
        col0.SetWidth(60);
        _design_days_display->InsertColumn(0,col0);
        col1.SetId(1); 
        col1.SetText( wxT("Month") );
        col1.SetWidth(80);
        _design_days_display->InsertColumn(1,col1);
        col2.SetId(2); 
        col2.SetText( wxT("Day") );
        col2.SetWidth(50);
        _design_days_display->InsertColumn(2,col2);
        col3.SetId(3); 
        col3.SetText( wxT("Peak DNI W/m2") );
        col3.SetWidth(110);
        _design_days_display->InsertColumn(3,col3);
        col4.SetId(4); 
        col4.SetText( wxT("Total DNI kWhr/m2") );
        col4.SetWidth(120);
        _design_days_display->InsertColumn(4,col4);
        col5.SetId(5); 
        col5.SetText( wxT("No. hours") );
        col5.SetWidth(70);
        _design_days_display->InsertColumn(5,col5);


        //Calculate the month and day for each item
        WeatherData *tsdat = &vset.sf.sim_step_data.Val();
        int nsim = (int)tsdat->size();
        
        //Determine the number of days
        int nday = 0;
        vector<int> ranges;
        for(int i=1; i<nsim; i++)
        {
            //if the combination of day-of-month + month changes, increment days
            if( tsdat->Month.at(i) != tsdat->Month.at(i-1) || tsdat->Day.at(i) != tsdat->Day.at(i-1) ) 
            {
                nday++;
                ranges.push_back(i);    //Track the indices where the days change
            }
        }
        nday++;
        ranges.push_back(nsim);

        
        for(int i=0; i<nday; i++)
        {
            
            //Calculate max and average flux
            double dni_peak = 0.;
            double dni_tot = 0.;
            double dni;
            int j;
            if(i==0) j=0;
            else j=ranges.at(i-1);
            int hrcount=0;
            while(j<ranges.at(i))
            {
                dni = tsdat->DNI.at(j);
                if( dni > dni_peak ) dni_peak = dni;
                dni_tot += dni/1000.;
                hrcount++;
                j++;
            }
            
            int dom = tsdat->Day.at( ranges.at(i) - 1 );
            int month = tsdat->Month.at( ranges.at(i) - 1 );
            int doy = DateTime::CalculateDayOfYear(2011, month, dom);

            wxListItem item;
            item.SetId(i);
            wxString tstr =  my_to_string(doy) ;
            item.SetText( tstr );
            _design_days_display->InsertItem(item);
            _design_days_display->SetItem(i, 1, DateTime::GetMonthName((int)month) );
            _design_days_display->SetItem(i, 2, my_to_string((int)dom)  );
            _design_days_display->SetItem(i, 3, to_string(dni_peak, "%.1f")  );
            _design_days_display->SetItem(i, 4, to_string(dni_tot, "%.1f")  );
            _design_days_display->SetItem(i, 5, my_to_string(hrcount+1)  );
        }
        
        _design_ndays->GetParent()->Layout();
    }
    else if(sel == var_solarfield::DES_SIM_DETAIL::EFFICIENCY_MAP__ANNUAL)
    {
        if( _design_combo_panel->IsShown() ) 
            _design_combo_panel->Hide();

        if(! _design_ndays->IsShown() ) 
        {
            _design_ndays->Show(); 
            _design_ndays_lab->Show();
        }

        if(! _design_nhours->IsShown() )
        {
            _design_nhours->Show(); 
            _design_nhours_lab->Show(); 
        }

        _design_helptext->SetLabel( 
            wxT("Specify the number of days to use in the creation of an efficiency map. Each heliostat will be evaluated\n")
            wxT("using an efficiency map over the entire specified weather file.") 
            );

        if(! _design_days_display->IsShown() ) 
            _design_days_display->Show();

        if(! _design_days_display->IsShown() ) 
            _design_days_display->Show();

        //Calculate which days/hours to use for the efficiency map and display
        if(_design_days_display->GetColumnCount() > 0) _design_days_display->DeleteAllColumns();
        if(_design_days_display->GetItemCount() > 0) _design_days_display->DeleteAllItems();
        wxListItem col0, col1, col2, col3;
        col0.SetId(0); 
        col0.SetText( wxT("Day No.") );
        col0.SetWidth(60);
        _design_days_display->InsertColumn(0,col0);
        col1.SetId(1); 
        col1.SetText( wxT("Month") );
        col1.SetWidth(80);
        _design_days_display->InsertColumn(1,col1);
        col2.SetId(2); 
        col2.SetText( wxT("Day") );
        col2.SetWidth(50);
        _design_days_display->InsertColumn(2,col2);
        col3.SetId(3); 
        col3.SetText( wxT("No. hours") );
        col3.SetWidth(70);
        _design_days_display->InsertColumn(3,col3);
        
        WeatherData *tsdat = &vset.sf.sim_step_data.Val();
        int nsim = (int)tsdat->size();

        //Determine the number of days
        int nday = 0;
        vector<int> ranges;
        for(int i=1; i<nsim; i++)
        {
            //if the combination of day-of-month + month changes, increment days
            if( tsdat->Month.at(i) != tsdat->Month.at(i-1) || tsdat->Day.at(i) != tsdat->Day.at(i-1) ) 
            {
                nday++;
                ranges.push_back(i);    //Track the indices where the days change
            }
        }
        nday++;
        ranges.push_back(nsim);

        for(int i=0; i<nday; i++)
        {
            
            //Calculate max and average flux
            int j;
            if(i==0) j=0;
            else j=ranges.at(i-1);
            int hrcount=0;
            while(j<ranges.at(i))
            {
                hrcount++;
                j++;
            }
            
            int dom = tsdat->Day.at( ranges.at(i) - 1 );
            int month = tsdat->Month.at( ranges.at(i) - 1 );
            int doy = DateTime::CalculateDayOfYear(2011, month, dom);

            wxListItem item;
            item.SetId(i);
            wxString tstr =  my_to_string(doy) ;
            item.SetText( tstr );
            _design_days_display->InsertItem(item);
            _design_days_display->SetItem(i, 1, DateTime::GetMonthName((int)month) );
            _design_days_display->SetItem(i, 2, my_to_string((int)dom)  );
            _design_days_display->SetItem(i, 3, my_to_string(hrcount+1)  );
        }
        
        _design_ndays->GetParent()->Layout();

    }
    
}

void SPFrame::OnBoundsImport( wxCommandEvent &WXUNUSED(event))
{
    try
    {
        BoundsImport();
    }
    catch(std::exception &e)
    {
        PopMessage(e.what(), "OnBoundsImport Error", wxICON_ERROR|wxOK);
    }
    catch(...)
    {
        PopMessage("An unhandled error has occurred and the action could not be completed.", "OnBoundsImport Error", wxICON_ERROR|wxOK);
        return;
    }
}

void SPFrame::BoundsImport()
{

    /*
    Import a file containing the land bounds. The format of the file should be:
    <type I or E><separator><Belongs to Poly #><separator><x value 1><separator><y value 1><...ignore rest of line>
    ...
    
    Definitions:
    <Type I or E>        | Is this polygon an Inclusion or Exclusion from the solar field?
    <Belongs to Poly #>    | Integer indicating which polygon this point belongs to. Restart numbering with Inclusions/Exclusions
    <x>                    | Value for X location. East is positive.
    <y>                    | Value for Y location. North is positive.
    <separator>            | Text separator, one of ","  " "  "\t"  ";"

    ** DO NOT CLOSE THE POLYGON - i.e. point 1 and point N in the file should be distinct **
    ** The polygon should be described by N points in order clockwise azimuthally **

    This algorithm sets the "land.0.bounds_array" variable in the variable map. The format 
    for this array is:
    [P]x1,y1,z1[P]x2,y2,z2...
    
    */

    //prompt for a file path
    wxFileDialog filedlg(this, wxT("Select a file for import..."), wxEmptyString, wxEmptyString, wxT("Data files (*.txt;*.dat;*.csv;.kml)|*.txt;*.dat;*.csv;*.kml") );
    filedlg.CentreOnParent();

    //Show and process
    if(filedlg.ShowModal() == wxID_OK)
    {
        wxString info = filedlg.GetPath().c_str();
        //Try opening the file
        if(ioutil::file_exists( info.c_str() ) )
        {
            string 
                files,
                fname = (std::string)info,
                eol;    
            ioutil::read_file(fname, files, eol);
            wxString file(files);

            //Pointers to the _variables for inclusions and exclusions
            vector<vector<sp_point> > *incs = &_variables.land.inclusions.val;
            vector<vector<sp_point> > *excs = &_variables.land.exclusions.val;

            string extension = lower_case( ioutil::ext_only( info.ToStdString() ) );

            //If this is a KML file (Google Earth format), call the KML parser
            if( extension == "kml")
            {
                //specify the tower location
                kml_select_dialog *tdlg = new kml_select_dialog(this, wxID_ANY, wxT("Land import settings"));
                //Has the tower lat/lon location already been set?
                if( _variables.land.import_tower_set.val )
                {
                    //Set the current location as the default when opening
                    double
                        tlat, tlon;
                    tlat = _variables.land.import_tower_lat.val;
                    tlon = _variables.land.import_tower_lon.val;
                    tdlg->setLocation(tlat, tlon);
                }


                double
                    tlat, tlon;
                if(tdlg->ShowModal() == wxID_OK)
                {
                    //get the tower location, lat and long
                    tdlg->getLocation(tlat, tlon);
                    _variables.land.import_tower_set.val = true;
                    _variables.land.import_tower_lat.val = tlat;
                    _variables.land.import_tower_lon.val = tlon;
                }
                else
                {
                    return;
                }

                vector<sp_point> poly;
                ParseKML(file, tlat, tlon, poly);
                
                if(tdlg->isInclusion())
                {
                    //Inclusion
					if (!tdlg->isAppend())
					{
						incs->clear();
						excs->clear();
					}

                    incs->push_back(poly);
                }
                else
                {
                    //Exclusion
					if (!tdlg->isAppend())
					{
						incs->clear();
						excs->clear();
					}

                    excs->push_back(poly);
                }
                //otherwise, it was cancelled

            }
            else if(extension == "csv" || extension == "dat" || extension == "txt") 
            {
                //throw spexception("Invalid file format, expecting .KML extension");

                //break the file into inclusions and exclusions
                vector<string> entries = split(file.ToStdString(), eol);
                int nlines = entries.size();
                string incstxt;
                string excstxt;
                bool is_incs=true;
                for(int i=0; i<nlines; i++)
                {
                    if( entries.at(i) == "*exclusions*" )
                    {
                        is_incs = false;
                        continue;
                    }
                    if( is_incs )
                        incstxt.append( entries.at(i) );
                    else
                        excstxt.append( entries.at(i) );
                }

                _variables.land.inclusions.set_from_string( incstxt.c_str() );
                _variables.land.exclusions.set_from_string( excstxt.c_str() );

            }

            //Call to the function that sets the bounds_grid data
            UpdateLandBoundsGrid(_variables);

            //Update the calculated values
            UpdateCalculatedGUIValues();

        }
        else
        {
            //Error: the file was not found
            throw spexception("The specified file was not found.");
        }
    }


    return;

}

void SPFrame::OnBoundsExport( wxCommandEvent &WXUNUSED(event))
{
    try
    {
        BoundsExport();
    }
    catch(std::exception &e)
    {
        PopMessage(e.what(), "OnBoundsExport Error", wxICON_ERROR|wxOK);
    }
    catch(...)
    {
        PopMessage("An unhandled error has occurred and the action could not be completed.", "OnBoundsExport Error", wxICON_ERROR|wxOK);
        return;
    }
}

void SPFrame::BoundsExport()
{
    /*
    Export the contents of the field bounds grid to a file of the user's choosing.
    */
    
    //Get the file
    wxFileDialog filedlg(this, wxT("Select a file for import..."), wxEmptyString, wxEmptyString, wxT("Text files (*.txt;*.dat;*.csv)|*.txt;*.dat;*.csv"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT );
    filedlg.CentreOnParent();

    //Show and process
    if(filedlg.ShowModal() == wxID_OK)
    {
        wxString info = filedlg.GetPath().c_str();
        
        FILE *file = fopen(info.c_str(), "w");
        if(! file )
        {
            PopMessage(wxT("The specified file could not be opened for writing. Please ensure that the file is not open or write-protected."), wxT("Error"), wxICON_ERROR|wxOK);
            return;
        }
            
        fprintf(file, "%s", _variables.land.inclusions.as_string().c_str() );
        fprintf(file, "%s", "\n*exclusions*\n");
        fprintf(file, "%s", _variables.land.exclusions.as_string().c_str() );

        fclose(file);

        //notify the user of a successful export
        wxString msg;
        msg.sprintf("%s\nFile successfully created.",info.c_str());
        PopMessage(msg, "Success");
    }

    return;
}

void SPFrame::OnBoundsEdit( wxGridEvent &WXUNUSED(evt) )
{
    /*
    Process a bounds grid edit event. 

    This method will update the contents of the variable map.
    */
    try
    {
        vector<vector< sp_point > > *incs = &_variables.land.inclusions.val;
        vector<vector< sp_point > > *excs = &_variables.land.exclusions.val;

        //Clear existing data and reconstruct
        incs->clear();
        excs->clear();

        string valx, valy;
        int nrows = _bounds_grid->GetNumberRows();
        string ptype, ptype0, pnum, pnum0;

        ptype0 = ""; pnum0 = "";
        for(int i=0; i<nrows; i++)
        {
            ptype = _bounds_grid->GetCellValue(i, 0);
            ptype = lower_case( ptype );
            if(! (ptype == "i" || ptype == "e") ) continue;

            pnum = _bounds_grid->GetCellValue(i,1);
            if(pnum == "") continue;

            valx = _bounds_grid->GetCellValue(i,2);    //x
            valy = _bounds_grid->GetCellValue(i,3); //y
            if(valx == "" || valy == "") continue;
            
            if(ptype != ptype0 || pnum != pnum0)
            {
                if(ptype == "i") 
                    incs->push_back( vector< sp_point >() );
                else 
                    excs->push_back( vector< sp_point >() );
                ptype0 = ptype;
                pnum0 = pnum;
            }

            double vx, vy;
            to_double(valx, &vx);
            to_double(valy, &vy);

            if( ptype == "i" )
                incs->back().push_back( sp_point(vx, vy, 0.) );
            else
                excs->back().push_back( sp_point(vx, vy, 0.) );

        }
    }
    catch(std::exception &e)
    {
        PopMessage(e.what(), "OnBoundsEdit Error", wxICON_ERROR|wxOK);
    }
    catch(...)
    {
        PopMessage("An unhandled error has occurred and the action could not be completed.", "OnBoundsEdit Error", wxICON_ERROR|wxOK);
        return;
    }
}

void SPFrame::OnDesignSimhrsChanged( wxCommandEvent &event)
{
    /* The number of simulation hours has changed. Resize the grid */
    try
    {
        int nprev = _design_grid->GetNumberRows();
        int nnew = ((wxSpinCtrl*)event.GetEventObject())->GetValue();

        if(nnew>0)
        {
            if(nnew>nprev)
            {
                _design_grid->AppendRows(nnew-nprev);
            }
            else
            {
                _design_grid->DeleteRows(nnew, nprev-nnew);
            }
            _design_grid->GetParent()->Layout();
        }
        else
        {
            ((wxSpinCtrl*)event.GetEventObject())->SetValue(1);
        }
    }
    catch(std::exception &e)
    {
        PopMessage(e.what(), "OnDesignSimhrsChanged Error", wxICON_ERROR|wxOK);
    }
    catch(...)
    {
        PopMessage("An unhandled error has occurred and the action could not be completed.", "OnDesignSimhrsChanged Error", wxICON_ERROR|wxOK);
        return;
    }    

}

void SPFrame::OnDesignNdaysChanged( wxCommandEvent &WXUNUSED(event))
{
    try
    {
        int sel = _variables.sf.des_sim_detail.mapval();
        int ndays = _design_ndays->GetValue();
        //Set the variable map value
        _variables.sf.des_sim_ndays.val = ndays;
        if(ndays>0)
        {
            UpdateDesignSelect(sel, _variables);
        }
        else
        {
            _design_ndays->SetValue(1);
        }
    }
    catch(std::exception &e)
    {
        PopMessage(e.what(), "OnDesignNdaysChanged Error", wxICON_ERROR|wxOK);
    }
    catch(...)
    {
        PopMessage("An unhandled error has occurred and the action could not be completed.", "OnDesignNdaysChanged Error", wxICON_ERROR|wxOK);
        return;
    }
}

void SPFrame::OnDesignNhrsChanged( wxCommandEvent &WXUNUSED(event))
{
    try
    {
        int sel = _variables.sf.des_sim_detail.mapval();
        int nhrs = _design_nhours->GetValue();
        //Set the variable map value
        _variables.sf.des_sim_nhours.val = nhrs;
        if(nhrs>-1)
        {
            UpdateDesignSelect(sel, _variables);
        }
        else
        {
            _design_nhours->SetValue(0);
        }
    }
    catch(std::exception &e)
    {
        PopMessage(e.what(), "OnDesignNhrsChanged Error", wxICON_ERROR|wxOK);
    }
    catch(...)
    {
        PopMessage("An unhandled error has occurred and the action could not be completed.", "OnDesignNhrsChanged Error", wxICON_ERROR|wxOK);
        return;
    }
}

void SPFrame::OnDesignEdit( wxGridEvent &WXUNUSED(event))
{
    /* 
    The design-point table data has been changed. Update the stored values 
    
    Format is:
    "[P]day,hour,month,DNI,Tdb,Pamb,Vwind[P]..."

    Store the data in:
    _variables.sf.sim_step_data
            
    */
    try
    {
        int nrow = _design_grid->GetNumberRows();
        int ncol = 7;

        string data;
        
        for(int i=0; i<nrow; i++)
        {
            data.append("[P]");
            for(int j=0; j<ncol; j++)
            {
                wxString val = _design_grid->GetCellValue(i, j);
                if(val == "") val = "0.";
                data.append( val.c_str() );
                if(j<ncol-1) data.append( "," );
            }
        }

        _variables.sf.sim_step_data.set_from_string(data.c_str());

    }
    catch(std::exception &e)
    {
        PopMessage(e.what(), "OnDesignEdit Error", wxICON_ERROR|wxOK);
    }
    catch(...)
    {
        PopMessage("An unhandled error has occurred and the action could not be completed.", "OnDesignEdit Error", wxICON_ERROR|wxOK);
        return;
    }

}

void SPFrame::OnDesignSelect( wxCommandEvent &WXUNUSED(event))
{
    try
    {
        //which item is selected?
        int sel = _variables.sf.des_sim_detail.mapval();

        UpdateDesignSelect(sel, _variables);
    }
    catch(std::exception &e)
    {
        PopMessage(e.what(), "OnDesignSelect Error", wxICON_ERROR|wxOK);
    }
    catch(...)
    {
        PopMessage("An unhandled error has occurred and the action could not be completed.", "OnDesignSelect Error", wxICON_ERROR|wxOK);
        return;
    }
}
