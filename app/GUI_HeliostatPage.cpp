
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


void SPFrame::CreateMasterHeliostatPage(wxScrolledWindow *parent)
{
    //Create the master heliostat page and return a pointer
#if _USE_WINDOW_BG 
    parent->SetBackgroundColour(_background_colour);
#endif
    wxStaticBox *sb = new wxStaticBox(parent, wxID_ANY, wxT("Heliostat templates"));
    wxStaticBoxSizer *sbs = new wxStaticBoxSizer(sb, wxVERTICAL);

    _heliotemp_add = new wxButton(parent, wxID_ANY, wxT("Add new"));
    _heliotemp_del = new wxButton(parent, wxID_ANY, wxT("Delete"));
    _heliotemp_state = new wxButton(parent, wxID_ANY, wxT("Disable"));
    _heliotemp_state->Disable();
    _heliotemp_rename = new wxButton(parent, wxID_ANY, wxT("Rename"));
    _heliotemp_rename->Disable();
    
    wxBoxSizer *but_sizer = new wxBoxSizer(wxHORIZONTAL);
    but_sizer->Add(_heliotemp_add, 0, wxRIGHT, 5);
    but_sizer->Add(_heliotemp_del, 0, wxRIGHT, 5);
    but_sizer->Add(_heliotemp_state, 0, wxRIGHT, 5);
    but_sizer->Add(_heliotemp_rename, 0, 0, 0);

    _heliostat_config = new wxListCtrl(parent, wxID_ANY, wxDefaultPosition, wxSize(300,150), wxLC_REPORT|wxLC_SINGLE_SEL);
    //set up the columns in the listctrl
    wxListItem col0, col1, col2;
    col0.SetText(wxT("Name"));
    col0.SetId(2);
    col0.SetWidth(150);
    _heliostat_config->InsertColumn(0, col0);
    col1.SetText(wxT("State"));
    col1.SetId(1);
    col1.SetWidth(65);
    _heliostat_config->InsertColumn(1, col1);
    
    //Arrow buttons
    wxBitmap bmp_up, bmp_down;
    bmp_up.LoadFile( _image_dir.GetPath(true) + "uparrow.png", wxBITMAP_TYPE_PNG);
    bmp_down.LoadFile( _image_dir.GetPath(true) + "downarrow.png", wxBITMAP_TYPE_PNG);
    wxBitmapButton
        *item_up = new wxBitmapButton(parent, wxID_ANY, bmp_up, wxDefaultPosition, wxSize(32,32)),
        *item_down = new wxBitmapButton(parent, wxID_ANY, bmp_down, wxDefaultPosition, wxSize(32,32));
    wxStaticText *arrow_lab = new wxStaticText(parent, wxID_ANY, wxT("Move item"));
    item_up->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnHeliotempOrderUp ), NULL, this);
    item_down->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnHeliotempOrderDown ), NULL, this);

    wxBoxSizer *arrow_sizer = new wxBoxSizer(wxVERTICAL);
    arrow_sizer->Add(item_up, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5);
    arrow_sizer->Add(arrow_lab, 0, wxALIGN_CENTER_HORIZONTAL, 0);
    arrow_sizer->Add(item_down, 0, wxALL|wxALIGN_CENTER_HORIZONTAL, 5);

    wxBoxSizer *config_sizer = new wxBoxSizer(wxHORIZONTAL);
    config_sizer->Add(_heliostat_config, 0, wxALL, 5);
    config_sizer->Add(arrow_sizer, 0, wxALIGN_CENTER_VERTICAL, 0);

    sbs->Add(but_sizer, 0, wxALL, 5);
    sbs->Add(config_sizer, 0, wxALL, 5);

    //template distribution group
    wxStaticBox *sb1 = new wxStaticBox(parent, wxID_ANY, wxT("Heliostat type use") );
    wxStaticBoxSizer *sbs1 = new wxStaticBoxSizer(sb1, wxVERTICAL);
    InputControl *template_rule = new InputControl(parent, wxID_ANY, _variables.sf.template_rule);
    sbs1->Add(template_rule);

    wxPanel *tpan_1 = new wxPanel(parent);
    wxBoxSizer *tpan_1s = new wxBoxSizer(wxVERTICAL);

    //collect the combo choices in the variable structure
    _variables.sf.temp_which.combo_clear();
    for(int i=0; i<(int)_variables.hels.size(); i++)
    {
        std::string is = my_to_string(i);
        _variables.sf.temp_which.combo_add_choice( _variables.hels.at(i).helio_name.val, is ); 
    }
    std::string cval = _variables.sf.temp_which.as_string(); 
    vector<string> cchoices = _variables.sf.temp_which.combo_get_choices(); 
    if( std::find(cchoices.begin(), cchoices.end(), cval ) != cchoices.end() )
        _variables.sf.temp_which.combo_select( cval );
    else
        _variables.sf.temp_which.combo_select_by_choice_index(0);

    InputControl *temp_which = new InputControl( tpan_1, wxID_ANY, _variables.sf.temp_which);
    tpan_1s->Add(temp_which);
    tpan_1->SetSizer(tpan_1s);
    template_rule->setPanelObject("Use single template", *tpan_1);
    sbs1->Add(tpan_1);

    _input_map[ template_rule->getVarObject() ] = template_rule;
    _input_map[ temp_which->getVarObject() ] = temp_which;

    /* Specify template bounds using a grid */
    wxPanel *tpan_2 = new wxPanel(parent);
    wxBoxSizer *tpan_2s = new wxBoxSizer(wxVERTICAL);
    _temp_bounds = new wxGrid(tpan_2, wxID_ANY, wxDefaultPosition, wxSize(500,150) );
    _temp_bounds->CreateGrid(1, 4);
    _temp_bounds->SetRowLabelSize(150);
    _temp_bounds->SetColLabelValue(0, wxT("Min radius\n[m]"));
    _temp_bounds->SetColLabelValue(1, wxT("Max radius\n[m]"));
    _temp_bounds->SetColLabelValue(2, wxT("Min extent\n[deg]"));
    _temp_bounds->SetColLabelValue(3, wxT("Max extent\n[deg]"));
    _temp_bounds->SetRowLabelValue(0, wxT("Template 1"));
    wxString msg1 = "Notes:\n"
                    "1) Heliostats not contained within any specified range will use the first template in the list.\n"
                    "2) Heliostats contained in multiple ranges will use the first suitable template.";
    wxStaticText *htext1 = new wxStaticText(tpan_2, wxID_ANY, msg1);
    htext1->SetForegroundColour(_helptext_colour);
    tpan_2s->Add(htext1, 0, wxTOP|wxBOTTOM, 5);
    tpan_2s->Add(_temp_bounds);

    tpan_2->SetSizer(tpan_2s);
    //Use single template=0;Specified range=1;Even radial distribution=2
    template_rule->setPanelObject("Specified range", *tpan_2);
    sbs1->Add(tpan_2);
    //bind this grid to update stored values
    _temp_bounds->Connect(wxEVT_GRID_CELL_CHANGED, wxCommandEventHandler( SPFrame::OnTempBoundsEdit ), NULL, this);
    
    /* Evenly distribute radially */
    //put in some text to describe what's going on here
    wxPanel *tpan_3 = new wxPanel(parent);
    template_rule->setPanelObject("Even radial distribution", *tpan_3);
    wxBoxSizer *tpan_3s = new wxBoxSizer(wxVERTICAL);
    wxString msg = wxT("Each of the heliostat geometries that are enabled will be distributed radially\n")
                    wxT("away from the tower so that each occupies an equal radial space in the solar field.\n")
                    wxT("The geometries will be applied in the order they appear in the list with the first\n")
                    wxT("being closest to the receiver.");
    wxStaticText *htext = new wxStaticText(tpan_3, wxID_ANY, msg);
    htext->SetForegroundColour(_helptext_colour);
    tpan_3s->Add(htext);
    tpan_3->SetSizer(tpan_3s);
    sbs1->Add(tpan_3);
    template_rule->updateInputDisplay();

    wxBoxSizer *main_sizer = new wxBoxSizer(wxVERTICAL);
    main_sizer->Add(sbs, 0, wxALL, 5);
    main_sizer->Add(sbs1, 0, wxALL, 5);
    parent->SetSizer(main_sizer);


    //Bindings
    _heliotemp_add->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnHeliotempAdd), NULL, this);
    _heliotemp_del->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnHeliotempDel), NULL, this);
    _heliotemp_state->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnHeliotempState), NULL, this);
    _heliotemp_rename->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnHeliotempRename), NULL, this);
    _heliostat_config->Connect(wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( SPFrame::OnHeliotempSelect), NULL, this);
    _heliostat_config->Connect(wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler( SPFrame::OnHeliotempDeselect), NULL, this);
    
    parent->SetScrollbars(10, 10, parent->GetSize().GetWidth()/10, parent->GetSize().GetWidth()/10);
    
    return;
}

void SPFrame::CreateHeliostatPage(wxScrolledWindow *parent, int id)
{
    //Create a page that defines a single heliostat type
#if _USE_WINDOW_BG 
    parent->SetBackgroundColour(_background_colour);
#endif
    //make a string copy of the ID
    string sid = my_to_string(id);

    //--Heliostat geometry group--
    wxStaticBox *sb0 = new wxStaticBox(parent, wxID_ANY, wxT("Heliostat geometry"));
    wxStaticBoxSizer *sbs0 = new wxStaticBoxSizer(sb0, wxVERTICAL);
    
    InputControl *width = new InputControl(parent, wxID_ANY,  _variables.hels[id].width);
    InputControl *height = new InputControl(parent, wxID_ANY,  _variables.hels[id].height);
    OutputControl *r_collision = new OutputControl(parent, wxID_ANY, _variables.hels[id].r_collision);
    
    wxBoxSizer *geom_panel_rect_s = new wxBoxSizer(wxVERTICAL);
    geom_panel_rect_s->Add(width, 0, 0, 0);
    geom_panel_rect_s->Add(height, 0, 0, 0);
    geom_panel_rect_s->Add(r_collision, 0, 0, 0);

    //-----Canting group for Rectangular ONLY--
    /* 
    No canting=0
    On-axis at slant=-1
    On-axis, user-defined=1
    Off-axis, day and hour=3
    User-defined vector=4 
    */
    InputControl *is_faceted = new InputControl(parent, wxID_ANY,  _variables.hels[id].is_faceted);
    InputControl *n_cant_x = new InputControl(parent, wxID_ANY,  _variables.hels[id].n_cant_x);
    InputControl *n_cant_y = new InputControl(parent, wxID_ANY,  _variables.hels[id].n_cant_y);
    InputControl *x_gap = new InputControl(parent, wxID_ANY,  _variables.hels[id].x_gap);
    InputControl *y_gap = new InputControl(parent, wxID_ANY,  _variables.hels[id].y_gap);
    geom_panel_rect_s->Add(is_faceted, 0, 0, 0);
    geom_panel_rect_s->Add(n_cant_x, 0, 0, 0);
    geom_panel_rect_s->Add(n_cant_y, 0, 0, 0);
    geom_panel_rect_s->Add(x_gap, 0, 0, 0);
    geom_panel_rect_s->Add(y_gap, 0, 0, 0);
    
    InputControl *cant_method = new InputControl(parent, wxID_ANY,  _variables.hels[id].cant_method);
    geom_panel_rect_s->Add(cant_method, 0, 0, 0);
    vector<wxPanel*> cpanels;
    string opt[5] = {"No canting","On-axis at slant","On-axis, user-defined","Off-axis, day and hour","User-defined vector"};

    for(int i=0; i<5; i++)
    { 
        cpanels.push_back( new wxPanel(parent) ); 
        cant_method->setPanelObject(opt[i], *cpanels.at(i));
    }
    
    //On-axis at slant
    wxPanel *par = cpanels.at(1);
    wxString msg1 = wxT("The \"slant range\" is defined as the hypotenuse of the triangle formed\n")
                        wxT("by the radial distance of the heliostat to the tower and the tower height.\n")
                        wxT("This value is automatically calculated for each heliostat and represents\n")
                        wxT("the ideal case.");
    wxStaticText *htext1 = new wxStaticText(par, wxID_ANY, msg1);
    htext1->SetForegroundColour(_helptext_colour);
    wxBoxSizer *cpan_sizer_1 = new wxBoxSizer(wxVERTICAL);
    cpan_sizer_1->Add(htext1);
    par->SetSizer(cpan_sizer_1);
    geom_panel_rect_s->Add(par);
    

    //On-axis, user defined
    par = cpanels.at(2);
    InputControl *cant_rad_scaled = new InputControl(par, wxID_ANY,  _variables.hels[id].cant_rad_scaled);
    InputControl *is_cant_rad_scaled = new InputControl(par, wxID_ANY,  _variables.hels[id].is_cant_rad_scaled);
    OutputControl *cant_radius = new OutputControl(par, wxID_ANY, _variables.hels[id].cant_radius);
    wxBoxSizer *cpan_sizer_2 = new wxBoxSizer(wxVERTICAL);
    cpan_sizer_2->Add(cant_rad_scaled, 0, 0, 0);
    cpan_sizer_2->Add(is_cant_rad_scaled, 0, 0, 0);
    cpan_sizer_2->Add(cant_radius, 0, 0, 0);
    par->SetSizer(cpan_sizer_2);
    geom_panel_rect_s->Add(par, 0, 0, 0);

    //Off-axis, day and hour
    par = cpanels.at(3);
    InputControl 
        *cant_hour = new InputControl(par, wxID_ANY,  _variables.hels[id].cant_hour),
        *cant_day = new InputControl(par, wxID_ANY,  _variables.hels[id].cant_day);
    OutputControl
        *cant_sun_el = new OutputControl(par, wxID_ANY, _variables.hels[id].cant_sun_el),
        *cant_sun_az = new OutputControl(par, wxID_ANY, _variables.hels[id].cant_sun_az); 
    wxBoxSizer *cpan_sizer_3 = new wxBoxSizer(wxVERTICAL);
    cpan_sizer_3->Add(cant_hour);
    cpan_sizer_3->Add(cant_day);
    cpan_sizer_3->Add(cant_sun_el);
    cpan_sizer_3->Add(cant_sun_az);
    par->SetSizer(cpan_sizer_3);
    geom_panel_rect_s->Add(par);
    
    wxString es = wxEmptyString;
    //user-defined vector
    par = cpanels.at(4);
    InputControl 
        *cant_vect_i = new InputControl(par, wxID_ANY,  _variables.hels[id].cant_vect_i),
        *cant_vect_j = new InputControl(par, wxID_ANY,  _variables.hels[id].cant_vect_j),
        *cant_vect_k = new InputControl(par, wxID_ANY,  _variables.hels[id].cant_vect_k);
    wxSize smallsize = wxSize(75, _default_input_size.GetHeight());
    OutputControl
        *cant_norm_i = new OutputControl(par, wxID_ANY, _variables.hels[id].cant_norm_i, "%.3f", wxDefaultPosition, smallsize, true),
        *cant_norm_j = new OutputControl(par, wxID_ANY, _variables.hels[id].cant_norm_j, "%.3f", wxDefaultPosition, smallsize, true),
        *cant_norm_k = new OutputControl(par, wxID_ANY, _variables.hels[id].cant_norm_k, "%.3f", wxDefaultPosition, smallsize, true),
        *cant_mag_i = new OutputControl(par, wxID_ANY, _variables.hels[id].cant_mag_i, "%.1f", wxDefaultPosition, smallsize, true),
        *cant_mag_j = new OutputControl(par, wxID_ANY, _variables.hels[id].cant_mag_j, "%.1f", wxDefaultPosition, smallsize, true),
        *cant_mag_k = new OutputControl(par, wxID_ANY, _variables.hels[id].cant_mag_k, "%.1f", wxDefaultPosition, smallsize, true);

    wxFlexGridSizer *cp4_gs = new wxFlexGridSizer(3,0,0);
    wxStaticText 
        *lab1 = new wxStaticText(par, wxID_ANY, wxT("Unit")), 
        *lab2 = new wxStaticText(par, wxID_ANY, wxT("Magnitude"));
    cp4_gs->Add((1,1), 0, 0, 0);
    cp4_gs->Add(lab1, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM, 5);
    cp4_gs->Add(lab2, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM, 5);
    cp4_gs->Add(cant_vect_i);
    cp4_gs->Add(cant_norm_i);
    cp4_gs->Add(cant_mag_i);
    cp4_gs->Add(cant_vect_j);
    cp4_gs->Add(cant_norm_j);
    cp4_gs->Add(cant_mag_j);
    cp4_gs->Add(cant_vect_k);
    cp4_gs->Add(cant_norm_k);
    cp4_gs->Add(cant_mag_k);
    InputControl *cant_vect_scale = new InputControl(par, wxID_ANY,  _variables.hels[id].cant_vect_scale);
    InputControl *is_cant_vect_slant = new InputControl(par, wxID_ANY,  _variables.hels[id].is_cant_vect_slant);
    wxBoxSizer *cpan_sizer_4 = new wxBoxSizer(wxVERTICAL);
    cpan_sizer_4->Add(cp4_gs);
    cpan_sizer_4->Add(cant_vect_scale);
    cpan_sizer_4->Add(is_cant_vect_slant);
    par->SetSizer(cpan_sizer_4);
    geom_panel_rect_s->Add(par);

    //Hide all of the non-default panels
    cant_method->updateInputDisplay();
    
    sbs0->Add(geom_panel_rect_s, 0, 0, 0);
    //---------------------------------
    
     
    
    //--Focusing group--
    wxStaticBox *sb1 = new wxStaticBox(parent, wxID_ANY, wxT("Focus parameters") );
    wxStaticBoxSizer *sbs1 = new wxStaticBoxSizer(sb1, wxVERTICAL);
    InputControl *focus_method = new InputControl(parent, wxID_ANY,  _variables.hels[id].focus_method);
    wxPanel *focp_3 = new wxPanel(parent);
    //Flat=0;At slant=1;Group average=2;User-defined=3
    focus_method->setPanelObject("User-defined", *focp_3);
    InputControl
        *is_focal_equal = new InputControl(focp_3, wxID_ANY,  _variables.hels[id].is_focal_equal),
        *is_xfocus = new InputControl(focp_3, wxID_ANY,  _variables.hels[id].is_xfocus),
        *is_yfocus = new InputControl(focp_3, wxID_ANY,  _variables.hels[id].is_yfocus),
        *x_focal_length = new InputControl(focp_3, wxID_ANY,  _variables.hels[id].x_focal_length),
        *y_focal_length = new InputControl(focp_3, wxID_ANY,  _variables.hels[id].y_focal_length);
    {
        wxWindow* dsibs[] = {is_yfocus, y_focal_length};
        is_focal_equal->setDisabledSiblings("true", 2, dsibs);
    }
    is_xfocus->setDisabledSiblings("false", x_focal_length);
    is_yfocus->setDisabledSiblings("false", y_focal_length);

    wxBoxSizer *focp_3s = new wxBoxSizer(wxVERTICAL);
    focp_3s->Add(is_focal_equal);
    focp_3s->Add(is_xfocus);
    focp_3s->Add(is_yfocus);
    focp_3s->Add(x_focal_length);
    focp_3s->Add(y_focal_length);
    focp_3->SetSizer(focp_3s);
    
    sbs1->Add(focus_method);
    sbs1->Add(focp_3);
    focus_method->updateInputDisplay();

    //------------------

    //--Optical error group--
    wxStaticBox *sb2 = new wxStaticBox(parent, wxID_ANY, wxT("Optical error parameters") );
    wxStaticBoxSizer *sbs2 = new wxStaticBoxSizer(sb2, wxVERTICAL);
    InputControl
        *err_elevation = new InputControl(parent, wxID_ANY,  _variables.hels[id].err_elevation),
        *err_azimuth = new InputControl(parent, wxID_ANY,  _variables.hels[id].err_azimuth),
        *err_surface_x = new InputControl(parent, wxID_ANY,  _variables.hels[id].err_surface_x),
        *err_surface_y = new InputControl(parent, wxID_ANY,  _variables.hels[id].err_surface_y),
        *err_reflect_x = new InputControl(parent, wxID_ANY,  _variables.hels[id].err_reflect_x),
        *err_reflect_y = new InputControl(parent, wxID_ANY,  _variables.hels[id].err_reflect_y),
        *track_method = new InputControl(parent, wxID_ANY,  _variables.hels[id].track_method),
        *track_period = new InputControl(parent, wxID_ANY,  _variables.hels[id].track_period),
        *st_err_type = new InputControl(parent, wxID_ANY, _variables.hels[id].st_err_type);

    track_method->setDisabledSiblings("Continuous", track_period );

    OutputControl
        *err_total = new OutputControl(parent, wxID_ANY, _variables.hels[id].err_total, "%.5f");

    wxWindow * errs[] = { err_elevation, err_azimuth, err_surface_x, err_surface_y, err_reflect_x, err_reflect_y, NULL};
    
    {
        int i=0;
        while( errs[i] != NULL )
        { sbs2->Add( errs[i] ); i++; }
            }
    sbs2->Add( err_total );
    sbs2->Add( st_err_type );
    sbs2->Add( track_method );
    sbs2->Add( track_period );
    //-----------------------

    //--Performance group--
    wxStaticBox *sb3 = new wxStaticBox(parent, wxID_ANY, wxT("Mirror performance parameters") );
    wxStaticBoxSizer *sbs3 = new wxStaticBoxSizer(sb3, wxVERTICAL);
    InputControl
        *reflect_ratio = new InputControl(parent, wxID_ANY,  _variables.hels[id].reflect_ratio),
        *reflectivity = new InputControl(parent, wxID_ANY,  _variables.hels[id].reflectivity),
        *soiling = new InputControl(parent, wxID_ANY,  _variables.hels[id].soiling);
    OutputControl
        *ref_total = new OutputControl(par, wxID_ANY, _variables.hels[id].ref_total, "%.3f" ),
        *a_total = new OutputControl(par, wxID_ANY, _variables.hels[id].area, "%.2f" );

    sbs3->Add(reflect_ratio);
    sbs3->Add(reflectivity);
    sbs3->Add(soiling);
    sbs3->Add(ref_total);
    sbs3->Add(a_total);

    //---------------------


    //Panel layout
    wxBoxSizer *main_sizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *top_sizer = new wxBoxSizer(wxHORIZONTAL);
    wxBoxSizer *col_sizer_1 = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *col_sizer_2 = new wxBoxSizer(wxVERTICAL);

    col_sizer_1->Add(sbs0, 0, wxALL, 5);
    col_sizer_1->Add(sbs1, 0, wxALL, 5);
    col_sizer_2->Add(sbs2, 0, wxALL, 5);
    col_sizer_2->Add(sbs3, 0, wxALL, 5);

    top_sizer->Add(col_sizer_1, 0, 0, 0);
    top_sizer->Add(col_sizer_2, 0, 0, 0);
    main_sizer->Add(top_sizer, 0, 0, 0);
    parent->SetSizer(main_sizer);
    {
    wxWindow* dsibs[] = {n_cant_x, 
        n_cant_y,
        x_gap,
        y_gap,
        cant_method, 
        cant_rad_scaled,
        is_cant_rad_scaled,
        cant_radius,
        cant_hour,
        cant_day,
        cant_sun_el,
        cant_sun_az,
        cant_method,
        cant_vect_i,
        cant_vect_j,
        cant_vect_k,
        cant_norm_i,
        cant_norm_j,
        cant_norm_k,
        cant_mag_i,
        cant_mag_j,
        cant_mag_k,
        cant_vect_scale,
        is_cant_vect_slant};

    is_faceted->setDisabledSiblings("false", 24, dsibs);
    }


    //Add all of the input items to the input control list
    InputControl* inputs[] = {width, height, is_faceted, n_cant_x, n_cant_y, cant_method, cant_rad_scaled, 
                              is_cant_rad_scaled, cant_day, cant_hour, cant_vect_i, cant_vect_j, cant_vect_k, cant_vect_scale,
                              is_cant_vect_slant, focus_method, is_focal_equal, is_xfocus, is_yfocus, x_focal_length, y_focal_length, 
                              err_elevation, err_azimuth, err_surface_x, err_surface_y, err_reflect_x, err_reflect_y, st_err_type, reflect_ratio,
                              reflectivity, soiling, y_gap, x_gap, track_period, track_method, NULL};
    OutputControl* outputs[] = {r_collision, cant_radius, cant_sun_az, cant_sun_el, cant_norm_i, cant_norm_j, cant_norm_k, 
                              cant_mag_i, cant_mag_j, cant_mag_k, err_total, ref_total, a_total, NULL};
    {
        int i=0;
        while( inputs[i] != NULL )
        {
            _input_map[ inputs[i]->getVarObject() ] = inputs[i];
            i++;
        }
    }
    {
        int i=0;
        while( outputs[i] != NULL )
        {
            _output_map[ outputs[i]->getVarObject() ] = outputs[i];
            i++;
        }
    }

    parent->SetScrollbars(10, 10, parent->GetSize().GetWidth()/10, parent->GetSize().GetWidth()/10);
    
    return;
}

void SPFrame::OnHeliotempAdd( wxCommandEvent &WXUNUSED(event))
{
    /* The user has prompted to add a new heliostat template */
    try
    {
        int nitems = _heliostat_config->GetItemCount();

        if( nitems >= _variables.hels.capacity() )
        {
            PopMessage("The maximum number of heliostat templates is " + my_to_string(_variables.hels.capacity()) + ". Cannot add any more heliostat templates.");
            return;
        }
    
        int ind = _variables.hels.back().id.val + 1; // Add to the last id value to ensure it is unique

        //which name to use?
        //Create a popup dialog
        string name = my_to_string(ind + 1);
        name = "Template " + name;
    
        bool no_errs = false;
        while( ! no_errs)
        {
            
            rename_dlg *dlg = new rename_dlg(this, wxID_ANY, "Heliostat Template Name", name, wxDefaultPosition, wxSize(350,100));
    
            if( dlg->ShowModal() == wxID_OK )
            {
                string tname = (string)dlg->getTextValue();
                // check to make sure this isn't a duplicate. Each item needs a unique name
                bool dupe = false;
                for(unsigned int i=0; i<_variables.hels.size(); i++)
                { 
                    if(tname == _variables.hels.at(i).helio_name.val) 
                        dupe = true; 
                }
                if(dupe)
                {
                    PopMessage("Please enter a unique name for this template.", "Warning", wxOK|wxICON_WARNING);
                    continue;
                }
                else
                {
                    no_errs = true;
                }

                //Add a heliostat
                _variables.add_heliostat(ind);
                _variables.hels.back().helio_name.val = tname;

            }
            else
            {
                no_errs = true;  //cancelled
            }

            dlg->Destroy();
        }
        if(no_errs)
        {
            //Re-create the solar field object
            _SF.Create(_variables);

            UpdateHelioUITemplates();
            
            //update the input display
            UpdateCalculatedGUIValues();
        }
    }
    catch(std::exception &e)
    {
        PopMessage(e.what(), "OnHeliotempAdd Error", wxICON_ERROR|wxOK);
    }
    catch(...)
    {
        PopMessage("An unhandled error has occurred and the action could not be completed.", "OnHeliotempAdd Error", wxICON_ERROR|wxOK);
    }
    return;
}

void SPFrame::OnHeliotempDel( wxCommandEvent &WXUNUSED(event))
{
    /* The user has prompted to delete an existing heliostat template */
    try
    {
        int nitems = _heliostat_config->GetItemCount();
        if(nitems==1) throw spexception("Cannot delete the only template.");

        int sel=-1;

        sel = _heliostat_config->GetNextItem(sel, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        if(sel<0) return;
        wxString msg = wxT("Deleting \"")+_variables.hels.at(sel).helio_name.val+wxT("\" cannot be undone. Continue?");
        int rid = PopMessage(msg, "Delete heliostat template", wxOK|wxCANCEL|wxICON_EXCLAMATION);
        if(rid == wxID_OK)
        {
            //Get the name of the page to delete
            string 
                pname = _variables.hels.at(sel).helio_name.val;

            //clean up the input and output map
            unordered_map<std::string, InputControl*> inswap_map; 
            {
                input_map::iterator it = _input_map.begin();
                while( it != _input_map.end() )
                {
                    std::string thisname = it->first->name;

                    if( thisname.find( "heliostat." ) != std::string::npos )
                    {
                        if( thisname.find("."+my_to_string(_variables.hels.at(sel).id.val)+".") == std::string::npos )
                        {
                            //this group is not being removed, so save
                            inswap_map[ thisname ] = it->second;
                        }
                        //remove all heliostat entries from input map
                        //it = _input_map.erase( it );
                        _input_map.erase(it++);
                    }
                    else
                    {
                        it++;
                    }
                }
            }
            unordered_map<std::string, OutputControl*> outswap_map; 
            {
                output_map::iterator it = _output_map.begin();
                while( it != _output_map.end() )
                {
                    std::string thisname = it->first->name;
                    if( thisname.find( "heliostat." ) != std::string::npos )
                    {
                        if( thisname.find("."+my_to_string(_variables.hels.at(sel).id.val)+".") == std::string::npos )
                        {
                            //this group is not being removed, so save
                            outswap_map[ thisname ] = it->second;
                        }
                        //remove all heliostat entries from input map
                        _output_map.erase(it++);
                    }
                    else
                    {
                        it++;
                    }
                }
            }

            //delete the item
            _variables.drop_heliostat( _variables.hels.at(sel).id.val );

            //re-map the input_map and ouput_map
            for( unordered_map<std::string, InputControl*>::iterator it=inswap_map.begin(); it != inswap_map.end(); it++)
            {
                _input_map[ _variables._varptrs[ it->first ] ] = it->second;
                it->second->setVarObject( _variables._varptrs[ it->first ] );    //reassign the variable pointer in the inputcontrol
            }

            for( unordered_map<std::string, OutputControl*>::iterator it=outswap_map.begin(); it != outswap_map.end(); it++)
            {
                _output_map[ _variables._varptrs[ it->first ] ] = it->second;
                it->second->setVarObject( _variables._varptrs[ it->first] );
            }

            //Re-create the solar field object
            _SF.Create(_variables);
        
            UpdateHelioUITemplates();
        }
    }
    catch(std::exception &e)
    {
        PopMessage(e.what(), "OnHeliotempDel Error", wxICON_ERROR|wxOK);
    }
    catch(...)
    {
        PopMessage("An unhandled error has occurred and the action could not be completed.", "OnHeliotempDel Error", wxICON_ERROR|wxOK);
    }
    return;
}

void SPFrame::OnHeliotempState( wxCommandEvent &WXUNUSED(event))
{
    /* The user has changed the state of the template */
    try
    {

        //get the current selected item
        int sel=-1;
    
        sel = _heliostat_config->GetNextItem(sel, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        if(sel<0) return;

        //Check to make sure not all templates are disabled
        bool any_enabled = false;

        for(int i=0; i<(int)_variables.hels.size(); i++)
        {
            if( i != sel )
                any_enabled = any_enabled || _variables.hels.at(i).is_enabled.val;
            else
                any_enabled = any_enabled || ! _variables.hels.at(i).is_enabled.val;
        }
        if( ! any_enabled )
            throw spexception( "At least one template must be enabled. Please enable the desired template before disabling others." );


        //Modify the state of the selected item
        if( _variables.hels.at(sel).is_enabled.val ) {
            _variables.hels.at(sel).is_enabled.val = false;
            wxListItem col0;
            col0.SetId(sel);
            _heliostat_config->DeleteItem(sel);
            _heliostat_config->InsertItem(col0);
            _heliostat_config->SetItem(sel, 0, _variables.hels.at(sel).helio_name.val);
            _heliostat_config->SetItem(sel, 1, wxT("Disabled"));
        
        }
        else
        {
            _variables.hels.at(sel).is_enabled.val = true;
            wxListItem col0;
            col0.SetId(sel);
            _heliostat_config->DeleteItem(sel);
            _heliostat_config->InsertItem(col0);
            _heliostat_config->SetItem(sel, 0, _variables.hels.at(sel).helio_name.val);
            _heliostat_config->SetItem(sel, 1, wxT("Enabled"));
        
        }
        UpdateHelioUITemplates();
    }
    catch(std::exception &e)
    {
        PopMessage(e.what(), "OnHeliotempState Error", wxICON_ERROR|wxOK);
    }
    catch(...)
    {
        PopMessage("Caught unspecified error. Action could not be completed.", "OnHeliotempState Error", wxICON_ERROR|wxOK);
    }

}

void SPFrame::OnHeliotempSelect( wxListEvent &event)
{
    /* 
    The user has selected a heliostat template in the master listctrl
    Check the current status, and switch the display of teh heliostat state button to enable or disable as needed
    */
    int sel = event.m_itemIndex;
    if(! _heliotemp_state->IsEnabled() ) {
        _heliotemp_state->Enable();
        _heliotemp_rename->Enable();
    }
    
    //check the data array to see the state of the item, and update the state button accordingly
    if( _variables.hels.at(sel).is_enabled.val )
    {
        _heliotemp_state->SetLabel( wxT("Disable") );
    }
    else {
        _heliotemp_state->SetLabel( wxT("Enable") );
    }
    
    return;
}

void SPFrame::OnHeliotempDeselect( wxListEvent &event)
{
    /* Check if any of the items are selected, otherwise disable the state button */

    if(_heliostat_config->GetSelectedItemCount() == 0) {
        _heliotemp_state->Disable();
        _heliotemp_rename->Disable();
    }
    event.Skip();

}

void SPFrame::OnHeliotempRename( wxCommandEvent &WXUNUSED(event))
{
    /* Rename the selected template */

    int sel=-1;

    sel = _heliostat_config->GetNextItem(sel, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if(sel<0) return;

    //Get the current name
    string name = _variables.hels.at(sel).helio_name.val;

    //Create a popup dialog
    rename_dlg *dlg = new rename_dlg(this, wxID_ANY, "Rename Heliostat Template", name, wxDefaultPosition, wxSize(350,100));
    
    if( dlg->ShowModal() == wxID_OK )
    {
        string tname = (string)dlg->getTextValue();
        _variables.hels.at(sel).helio_name.val = tname;
        _page_panel->RenamePage( name, (wxString)tname );
    }
    dlg->Destroy();


    UpdateHelioUITemplates();
}

void SPFrame::OnTempBoundsEdit( wxCommandEvent &WXUNUSED(event))
{
    /* 
    The contents of the heliostat template bounds grid have been changed. Update the values stored in 
    the variable map.

    Contents of the grid are:
    min radius | max radius | min angular extent | max angular extent
    r1
    r2
    ...
    */
    int nr = _temp_bounds->GetNumberRows();
    int nc = _temp_bounds->GetNumberCols();
    
    for(int i=0; i<nr; i++)
    {
        ArrayString stemp;

        for(int j=0; j<nc; j++)
            stemp.push_back(_temp_bounds->GetCellValue(i, j).ToStdString());

        _variables.hels[i].temp_rad_min.set_from_string( stemp[0].c_str() ); 
        _variables.hels[i].temp_rad_max.set_from_string( stemp[1].c_str() ); 
        _variables.hels[i].temp_az_min.set_from_string( stemp[2].c_str() ); 
        _variables.hels[i].temp_az_max.set_from_string( stemp[3].c_str() ); 

    }
}

void SPFrame::OnHeliotempOrderUp( wxCommandEvent &WXUNUSED(event))
{
    /* Switch the order of all relevant information and displays */

    //get the selected item
    int sel=-1;
    
    sel = _heliostat_config->GetNextItem(sel, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if(sel<1) return;

    //move the item up in the info object
    iter_swap( _variables.hels.begin() + sel, _variables.hels.begin() + sel - 1);

    //Select the moved item in the listctrl
    _heliostat_config->SetItemState(sel - 1, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);

    _page_panel->MovePage( _variables.hels.at(sel).helio_name.val, 1 );

    UpdateHelioUITemplates();
    
}

void SPFrame::OnHeliotempOrderDown( wxCommandEvent &WXUNUSED(event))
{
    /* Switch the order of all relevant information and displays */

    //get the selected item
    int nitems = _heliostat_config->GetItemCount();
    int sel=-1;
    
    sel = _heliostat_config->GetNextItem(sel, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if(sel<0) return;
    if(sel==nitems-1) return;

    //Move the item down in the info object
    iter_swap( _variables.hels.begin() + sel, _variables.hels.begin() + sel + 1);

     //Select the moved item in the listctrl
    _heliostat_config->SetItemState(sel + 1, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);

    _page_panel->MovePage( _variables.hels.at(sel).helio_name.val, -1 );

    UpdateHelioUITemplates();
   
}

void SPFrame::setHelioTemplateGrid()
{
    int nht = (int)_variables.hels.size();

    _temp_bounds->DeleteRows(0, _temp_bounds->GetNumberRows(), false);
    int ct=0;
    for(int i=0; i<nht; i++)
    {
        if(! _variables.hels.at(i).is_enabled.val) continue;
        _temp_bounds->AppendRows(1, false);
        _temp_bounds->SetRowLabelValue(ct, _variables.hels.at(i).helio_name.val);
        _temp_bounds->SetCellValue(ct, 0, to_string(_variables.hels.at(i).temp_rad_min.val,"%.1f") );
        _temp_bounds->SetCellValue(ct, 1, to_string(_variables.hels.at(i).temp_rad_max.val,"%.1f") );
        _temp_bounds->SetCellValue(ct, 2, to_string(_variables.hels.at(i).temp_az_min.val,"%.1f") );
        _temp_bounds->SetCellValue(ct, 3, to_string(_variables.hels.at(i).temp_az_max.val,"%.1f") );
        ct++;
    }

}

void SPFrame::UpdateHelioUITemplates()
{
    /* 
    Update all display entities and map values based on the current information in _variables.hels object.

    All data in _variables.hels must be up-to-date and in order!
    */

    //order pages, creating new as needed
    int ntemp = (int)_variables.hels.size();

    //Delete existing pages
    vector<std::string> existing;
    for(int i=0; i<_page_panel->GetPageCount(); i++)
    {
        if( _page_panel->GetPageGroupOwner( i ) == pageNames.heliostat_master )
            existing.push_back(_page_panel->GetPageName(i));
    }

    //How many pages are already built?
    int nexisting = (int)existing.size();
        
    //Check to see if any pages need to be deleted
    if(nexisting > ntemp )
    {
        for(int i=0; i<nexisting; i++)
        {
            bool in_htemp = false;
            for(int j=0; j<ntemp; j++)
            {
                if( existing.at(i) == _variables.hels.at(j).helio_name.val )
                {
                    in_htemp = true;
                    continue;
                }
            }

            if(! in_htemp )
                _page_panel->DestroyPage( existing.at(i) );
        }
    }

    //Create new pages
    bool new_bind = false;
    for(int i=nexisting; i<ntemp; i++)       
    {

        //Create a new page
        wxScrolledWindow *htui = new wxScrolledWindow(this);
        _variables.hels.at(i).cbdata.val = (void*)htui;
            
        CreateHeliostatPage( htui, i);
        _page_panel->InsertPage(
            _page_panel->GetPagePosition( pageNames.heliostat_master )+i+1, 
            htui, 
            _variables.hels.at(i).helio_name.val, 
            ID_ICON_HELIOSTAT, 1, pageNames.heliostat_master);
        new_bind = true;

        //Add to the sizer
        htui->Show(false);
        _main_sizer->Add(htui, 1, wxEXPAND, 0);
        
    }
    
    for(int i=0; i<ntemp; i++)
    {
        //enable or disable according to state
        ((wxScrolledWindow*)_variables.hels.at(i).cbdata.val)->Enable( _variables.hels.at(i).is_enabled.val );
    }

    //Update page values and bindings
    if(new_bind)
        bindControls(); //Update bindings for new page(s)

    _heliostat_config->DeleteAllItems();
    for(int i=0; i<ntemp; i++)
    {
        wxListItem row;
        row.SetId(i);
        _heliostat_config->InsertItem(i, row);
        _heliostat_config->SetItem(i, 0, _variables.hels.at(i).helio_name.val );
        _heliostat_config->SetItem(i, 1, _variables.hels.at(i).is_enabled.val ? "Enabled" : "Disabled" );
    }

    //Update the template selection combo
    _input_map[ &_variables.sf.temp_which ]->updateComboChoices();
    
    //Update the template bounds table
    setHelioTemplateGrid();

    _page_panel->Update();
    _page_panel->Refresh();
    
    this->Layout();
    this->Update();
    this->Refresh();
}
