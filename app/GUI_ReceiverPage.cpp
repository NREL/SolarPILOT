
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
#include "IOUtil.h"
#include <fstream>
#include <wx/colordlg.h>
#include <wx/dc.h>
#include <wx/dcgraph.h>
#include <wx/dcbuffer.h>

using namespace std;

void SPFrame::CreateMasterReceiverPage(wxScrolledWindow *parent)
{
    //Create the master receiver page and return a pointer
#if _USE_WINDOW_BG 
    parent->SetBackgroundColour(_background_colour);
#endif

    
    wxStaticBox *sb = new wxStaticBox(parent, wxID_ANY, wxT("Receiver geometries"));
    wxStaticBoxSizer *sbs = new wxStaticBoxSizer(sb, wxVERTICAL);

    _rec_add = new wxButton(parent, wxID_ANY, wxT("Add new"));
    _rec_del = new wxButton(parent, wxID_ANY, wxT("Delete"));
    _rec_duplicate = new wxButton(parent, wxID_ANY, wxT("Copy As.."));
    _rec_duplicate->Disable();
    _rec_state = new wxButton(parent, wxID_ANY, wxT("Disable"));
    _rec_state->Disable();
    _rec_rename = new wxButton(parent, wxID_ANY, wxT("Rename"));
    _rec_rename->Disable();
    wxButton *rec_color = new wxButton(parent, wxID_ANY, "Color..");

    
    wxBoxSizer *but_sizer = new wxBoxSizer(wxHORIZONTAL);
    but_sizer->Add(_rec_add, 0, wxRIGHT, 5);
    but_sizer->Add(_rec_del, 0, wxRIGHT, 5);
    but_sizer->Add(_rec_duplicate, 0, wxRIGHT, 5);
    but_sizer->Add(_rec_state, 0, wxRIGHT, 5);
    but_sizer->Add(_rec_rename, 0, wxRIGHT, 5);
    but_sizer->Add(rec_color);

    _rec_config = new wxListCtrl(parent, wxID_ANY, wxDefaultPosition, wxSize(400,150), wxLC_REPORT|wxLC_SINGLE_SEL);
    //set up the columns in the listctrl
    wxListItem col0, col1, col2;
    col0.SetText(wxT("Name"));
    col0.SetId(2);
    col0.SetWidth(150);
    _rec_config->InsertColumn(0, col0);
    col1.SetText(wxT("State"));
    col1.SetId(1);
    col1.SetWidth(65);
    _rec_config->InsertColumn(1, col1);
    col2.SetText("Color");
    col2.SetId(3);
    col2.SetWidth(100);
    _rec_config->InsertColumn(2, col2);
            
    sbs->Add(but_sizer, 0, wxALL, 5);
    sbs->Add(_rec_config, 0, wxALL, 5);
    
    //sbs->Add(new wxColourPickerCtrl(parent, wxID_ANY));

    wxString msg = wxT("Note: Each enabled receiver template will be used in the simulation.");
    wxStaticText *htext = new wxStaticText(parent, wxID_ANY, msg);
    htext->SetForegroundColour(_helptext_colour);
    sbs->Add(htext, 0, wxALL, 5);

    wxStaticBox *sbopt = new wxStaticBox(parent, wxID_ANY, wxT("Multi-receiver aimpoint and layout optimization"));
    wxStaticBoxSizer *sbsopt = new wxStaticBoxSizer(sbopt, wxVERTICAL);
    InputControl *multirec_opt_timeout = new InputControl(parent, wxID_ANY, _variables.opt.multirec_opt_timeout);
    InputControl *multirec_screen_mult = new InputControl(parent, wxID_ANY, _variables.opt.multirec_screen_mult);
    InputControl *multi_rec_aim_rand = new InputControl(parent, wxID_ANY, _variables.flux.multi_rec_aim_rand);

    sbsopt->Add(multirec_opt_timeout);
    sbsopt->Add(multirec_screen_mult);
    sbsopt->Add(multi_rec_aim_rand);

    _input_map[multirec_opt_timeout->getVarObject()] = multirec_opt_timeout;
    _input_map[multirec_screen_mult->getVarObject()] = multirec_screen_mult;
    _input_map[multi_rec_aim_rand->getVarObject()] = multi_rec_aim_rand;

    //create objects for specifying the receiver power fractions
    _rec_power_fractions = new wxGrid(parent, wxID_ANY, wxDefaultPosition);
    _rec_power_fractions->CreateGrid(1, 1);
    
    InputControl *is_multirec_powfrac = new InputControl(parent, wxID_ANY, _variables.sf.is_multirec_powfrac);
    is_multirec_powfrac->addDisabledSiblings("false", _rec_power_fractions);
    _input_map[is_multirec_powfrac->getVarObject()] = is_multirec_powfrac;
    
    _msg_rec_power_fractions = new wxStaticText(parent, wxID_ANY, 
        "Note: the specified power fractions will be scaled in proportion to\n"
        "their values such that the total design power equals the field design power.");
    _msg_rec_power_fractions->SetForegroundColour(_helptext_colour);

    wxBoxSizer *power_sizer = new wxBoxSizer(wxVERTICAL);
    power_sizer->Add(is_multirec_powfrac);
    power_sizer->Add(_rec_power_fractions, 1, wxALL, 5);
    power_sizer->Add(_msg_rec_power_fractions, 0, wxALL, 5);
    power_sizer->SetSizeHints(parent);

    UpdateReceiverPowerGrid();

    wxBoxSizer *main_sizer = new wxBoxSizer(wxVERTICAL);
    main_sizer->Add(sbs, 0, wxALL, 5);
    main_sizer->Add(sbsopt, 0, wxALL, 5);
    main_sizer->Add(power_sizer, 0, wxALL, 5);
    parent->SetSizer(main_sizer);

    //Bindings
    _rec_add->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnReceiverAdd), NULL, this);
    _rec_del->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnReceiverDel), NULL, this);
    _rec_state->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnReceiverState), NULL, this);
    _rec_duplicate->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnReceiverAdd), NULL, this);
    _rec_rename->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnReceiverRename), NULL, this);
    rec_color->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnReceiverColor), NULL, this);
    _rec_config->Connect(wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( SPFrame::OnReceiverSelect), NULL, this);
    _rec_config->Connect(wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler( SPFrame::OnReceiverDeselect), NULL, this);
    _rec_power_fractions->Connect(wxEVT_GRID_CELL_CHANGED, wxGridEventHandler( SPFrame::OnReceiverPowerGridEdit), NULL, this);

    parent->SetScrollbars(10, 10, parent->GetSize().GetWidth()/10, parent->GetSize().GetWidth()/10);
    
    return;
}

void SPFrame::CreateReceiverPage(wxScrolledWindow *parent, int id)
{
    //Create the receiver page and return a pointer
#if _USE_WINDOW_BG 
    parent->SetBackgroundColour(_background_colour);
#endif    
    //---------------- receiver geometry group ------------------------------
    wxStaticBox *sb0 = new wxStaticBox(parent, wxID_ANY, wxT("Receiver geometry"));
    wxStaticBoxSizer *sbs0 = new wxStaticBoxSizer(sb0, wxVERTICAL);

    wxPanel
        *panel_rec_ext = new wxPanel(parent),
        *panel_rec_cav = new wxPanel(parent),
        *panel_rec_flat = new wxPanel(parent),
        *panel_rec_fall = new wxPanel(parent);
    wxBoxSizer
        *panel_rec_ext_s = new wxBoxSizer(wxVERTICAL),
        *panel_rec_cav_s = new wxBoxSizer(wxVERTICAL),
        *panel_rec_flat_s = new wxBoxSizer(wxVERTICAL),
        *panel_rec_fall_s = new wxBoxSizer(wxVERTICAL);
    
    // controls shown for all receiver types
    InputControl* rec_type = new InputControl(parent, wxID_ANY, _variables.recs[id].rec_type);
    InputControl* rec_height = new InputControl(parent, wxID_ANY, _variables.recs[id].rec_height);
    
    // controls for external receiver
    InputControl* rec_diameter = new InputControl(panel_rec_ext, wxID_ANY, _variables.recs[id].rec_diameter);

    //controls for flat receiver
    InputControl* rec_width = new InputControl(panel_rec_flat, wxID_ANY, _variables.recs[id].rec_width);
    
    // controls for cavity receiver
    InputControl* n_panels = new InputControl(panel_rec_cav, wxID_ANY, _variables.recs[id].n_panels);
    InputControl* rec_cav_rad = new InputControl(panel_rec_cav, wxID_ANY, _variables.recs[id].rec_cav_rad);
    InputControl* rec_cav_cdepth = new InputControl(panel_rec_cav, wxID_ANY, _variables.recs[id].rec_cav_cdepth);
    InputControl* rec_cav_apwfrac = new InputControl(panel_rec_cav, wxID_ANY, _variables.recs[id].rec_cav_apwfrac);
    InputControl* rec_cav_tlip = new InputControl(panel_rec_cav, wxID_ANY, _variables.recs[id].rec_cav_tlip);
    InputControl* rec_cav_blip = new InputControl(panel_rec_cav, wxID_ANY, _variables.recs[id].rec_cav_blip);

    OutputControl* rec_cav_apw = new OutputControl(panel_rec_cav, wxID_ANY, _variables.recs[id].rec_cav_apw, wxT("%.2f"));
    OutputControl* rec_cav_aph = new OutputControl(panel_rec_cav, wxID_ANY, _variables.recs[id].rec_cav_aph, wxT("%.2f"));

    // controls for falling particle receiver
    InputControl* rec_width_fall = new InputControl(panel_rec_fall, wxID_ANY, _variables.recs[id].rec_width);
    InputControl* norm_curtain_width = new InputControl(panel_rec_fall, wxID_ANY, _variables.recs[id].norm_curtain_width);
    InputControl* norm_curtain_height = new InputControl(panel_rec_fall, wxID_ANY, _variables.recs[id].norm_curtain_height);
    InputControl* max_curtain_depth = new InputControl(panel_rec_fall, wxID_ANY, _variables.recs[id].max_curtain_depth);
    InputControl* curtain_type = new InputControl(panel_rec_fall, wxID_ANY, _variables.recs[id].curtain_type);
    InputControl* curtain_radius = new InputControl(panel_rec_fall, wxID_ANY, _variables.recs[id].curtain_radius);
    InputControl* is_snout = new InputControl(panel_rec_fall, wxID_ANY, _variables.recs[id].is_snout);
    InputControl* snout_depth = new InputControl(panel_rec_fall, wxID_ANY, _variables.recs[id].snout_depth);
    InputControl* snout_vert_top_angle = new InputControl(panel_rec_fall, wxID_ANY, _variables.recs[id].snout_vert_top_angle);
    InputControl* snout_vert_bot_angle = new InputControl(panel_rec_fall, wxID_ANY, _variables.recs[id].snout_vert_bot_angle);
    InputControl* snout_horiz_angle = new InputControl(panel_rec_fall, wxID_ANY, _variables.recs[id].snout_horiz_angle);

    //----- Troughs location panel -------------
    string sid = my_to_string(id);

    wxButton* import_troughs_locs = new wxButton(panel_rec_fall, wxID_ANY, "Import", wxDefaultPosition, wxDefaultSize, 0L, wxDefaultValidator, sid);
    import_troughs_locs->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SPFrame::OnTroughsLocImport), NULL, this);
    wxButton* export_troughs_locs = new wxButton(panel_rec_fall, wxID_ANY, "Export", wxDefaultPosition, wxDefaultSize, 0L, wxDefaultValidator, sid);
    export_troughs_locs->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SPFrame::OnTroughsLocExport), NULL, this);
    wxSpinCtrl* num_troughs_spin = new wxSpinCtrl(panel_rec_fall, wxID_ANY, "0", wxDefaultPosition, _spin_ctrl_size, wxSP_ARROW_KEYS | wxALIGN_RIGHT, 0, 10, 0, sid);
    num_troughs_spin->Connect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxCommandEventHandler(SPFrame::OnNumTroughsSpin), NULL, this);

    wxBoxSizer* troughs_locs_button_sizer = new wxBoxSizer(wxHORIZONTAL);
    troughs_locs_button_sizer->Add(import_troughs_locs, 0, wxALL, 5);
    troughs_locs_button_sizer->Add(export_troughs_locs, 0, wxALL, 5);
    troughs_locs_button_sizer->AddSpacer(10);
    troughs_locs_button_sizer->Add(new wxStaticText(panel_rec_fall, wxID_ANY, "Number of troughs"), 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
    troughs_locs_button_sizer->Add(num_troughs_spin, 0, wxALL, 5);

    wxGrid* trough_locs_grid = new wxGrid(panel_rec_fall, wxID_ANY, wxDefaultPosition, wxSize(250, 250), 262144L, sid);
    _troughs_loc_objects[id] = s_troughs_loc_objects(trough_locs_grid, num_troughs_spin);

    trough_locs_grid->CreateGrid(0, 0);
    UpdateTroughsLocGrid(id);
    trough_locs_grid->Connect(wxEVT_GRID_CELL_CHANGED, wxGridEventHandler(SPFrame::OnTroughLocChange), NULL, this);
    //----------------------------------------
    OutputControl* curtain_height = new OutputControl(panel_rec_fall, wxID_ANY, _variables.recs[id].curtain_total_height, wxT("%.2f"));
    OutputControl* max_curtain_width = new OutputControl(panel_rec_fall, wxID_ANY, _variables.recs[id].max_curtain_width, wxT("%.2f"));

    // additional controls for all receivers
    InputControl* accept_ang_type = new InputControl(parent, wxID_ANY, _variables.recs[id].accept_ang_type);
    InputControl* accept_ang_x = new InputControl(parent, wxID_ANY, _variables.recs[id].accept_ang_x);
    InputControl* accept_ang_y = new InputControl(parent, wxID_ANY, _variables.recs[id].accept_ang_y);
    InputControl* rec_azimuth = new InputControl(parent, wxID_ANY, _variables.recs[id].rec_azimuth);
    InputControl* rec_elevation = new InputControl(parent, wxID_ANY, _variables.recs[id].rec_elevation);
    
    // hidden controls
    InputControl* panel_rotation = new InputControl(parent, wxID_ANY, _variables.recs[id].panel_rotation);
    InputControl* is_polygon = new InputControl(parent, wxID_ANY, _variables.recs[id].is_polygon);
    InputControl* aperture_type = new InputControl(parent, wxID_ANY, _variables.recs[id].aperture_type);
    
    // output controls    
    OutputControl* rec_aspect = new OutputControl(parent, wxID_ANY, _variables.recs[id].rec_aspect, wxT("%.2f"));
    OutputControl* absorber_area = new OutputControl(parent, wxID_ANY, _variables.recs[id].absorber_area, wxT("%.1f"));
    OutputControl* aperture_area = new OutputControl(parent, wxID_ANY, _variables.recs[id].aperture_area, wxT("%.1f"));

    string msg = "For non-planar receivers, the receiver will be oriented such that the\n"
        "primary parent with the specified azimuth angle will be tilted at the\n"
        "specified elevation angle. Other panels will be oriented along the arc of\n"
        "specified radius lying within the rotated receiver midline plane.";
    wxStaticText* az_info = new wxStaticText(panel_rec_flat, wxID_ANY, msg);
    az_info->SetForegroundColour(_helptext_colour);

    //External cylindrical=0;Cavity=1;Flat plate=2;Falling particle=3
    rec_type->setPanelObject("External cylindrical", *panel_rec_ext);
    rec_type->setPanelObject("Cavity", *panel_rec_cav);
    rec_type->setPanelObject("Flat plate", *panel_rec_flat);
    rec_type->setPanelObject("Falling particle", *panel_rec_fall);

    panel_rec_ext_s->Add(rec_diameter);
    
    panel_rec_flat_s->Add(rec_width);
    panel_rec_flat_s->Add(az_info, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    panel_rec_cav_s->Add(n_panels);
    panel_rec_cav_s->Add(rec_cav_rad);
    panel_rec_cav_s->Add(rec_cav_cdepth);
    panel_rec_cav_s->Add(rec_cav_apwfrac);
    panel_rec_cav_s->Add(rec_cav_tlip);
    panel_rec_cav_s->Add(rec_cav_blip);
    panel_rec_cav_s->Add(rec_cav_aph);
    panel_rec_cav_s->Add(rec_cav_apw);

    panel_rec_fall_s->Add(rec_width_fall);
    panel_rec_fall_s->Add(norm_curtain_height);
    panel_rec_fall_s->Add(curtain_height);
    panel_rec_fall_s->Add(norm_curtain_width);
    panel_rec_fall_s->Add(max_curtain_width);
    panel_rec_fall_s->Add(max_curtain_depth);
    panel_rec_fall_s->Add(troughs_locs_button_sizer);
    panel_rec_fall_s->Add(trough_locs_grid, 1, wxEXPAND | wxALL, 5);
    panel_rec_fall_s->Add(curtain_type);
    panel_rec_fall_s->Add(curtain_radius);
    panel_rec_fall_s->Add(is_snout);
    panel_rec_fall_s->Add(snout_depth);
    panel_rec_fall_s->Add(snout_vert_top_angle);
    panel_rec_fall_s->Add(snout_vert_bot_angle);
    panel_rec_fall_s->Add(snout_horiz_angle);

    sbs0->Add(rec_type);
    sbs0->Add(rec_height);
    sbs0->Add(accept_ang_type);
    sbs0->Add(accept_ang_x);
    sbs0->Add(accept_ang_y);
    sbs0->Add(rec_azimuth);
    sbs0->Add(rec_elevation);
    
    //panel_rec_flat_s->Add(new wxStaticLine(panel_rec_flat, wxID_ANY, wxDefaultPosition, wxSize(1,1), wxHORIZONTAL), 0, wxEXPAND|wxALL, 5);

    sbs0->Add(panel_rotation);
    sbs0->Add(is_polygon);
    sbs0->Add(aperture_type);

    panel_rec_ext->SetSizer(panel_rec_ext_s);
    panel_rec_cav->SetSizer(panel_rec_cav_s);
    panel_rec_flat->SetSizer(panel_rec_flat_s);
    panel_rec_fall->SetSizer(panel_rec_fall_s);

    //add receiver type panels
    sbs0->Add(panel_rec_ext);
    sbs0->Add(panel_rec_cav);
    sbs0->Add(panel_rec_flat);
    sbs0->Add(panel_rec_fall);

    //outputs
    sbs0->Add(rec_aspect);
    sbs0->Add(absorber_area);
    sbs0->Add(aperture_area);

    {
        wxWindow* dsibs[] = {panel_rotation};
        is_polygon->setDisabledSiblings("false", 1, dsibs);
    }
    {
		wxWindow* dsibs[] = {accept_ang_type, accept_ang_y, rec_elevation};
		rec_type->setDisabledSiblings("External cylindrical", 3, dsibs);
    }
    {
		wxWindow* dsibs[] = {is_polygon};
		rec_type->setDisabledSiblings("Flat plate", 1, dsibs);
    }
    {
        wxWindow* dsibs[] = {snout_depth, snout_vert_top_angle, snout_vert_bot_angle, snout_horiz_angle};
        is_snout->setDisabledSiblings("false", 4, dsibs);
    }
    {
        wxWindow* dsibs[] = { rec_elevation };
        rec_type->setDisabledSiblings("Falling particle", 1, dsibs);
    }
    {
        wxWindow* dsibs[] = {curtain_radius};
        curtain_type->setDisabledSiblings("Flat", 1, dsibs);
    }

    //---------------- end receiver geometry group ------------------------------

    //Receiver positioning group
    wxStaticBox *sb1 = new wxStaticBox(parent, wxID_ANY, wxT("Receiver position"));
    wxStaticBoxSizer *sbs1 = new wxStaticBoxSizer(sb1, wxVERTICAL);

    InputControl
        *rec_offset_x = new InputControl(parent, wxID_ANY, _variables.recs[id].rec_offset_x),
        *rec_offset_y = new InputControl(parent, wxID_ANY, _variables.recs[id].rec_offset_y),
        *rec_offset_z = new InputControl(parent, wxID_ANY, _variables.recs[id].rec_offset_z),
        *rec_offset_reference = new InputControl(parent, wxID_ANY, _variables.recs[id].rec_offset_reference),
        *is_open_geom = new InputControl(parent, wxID_ANY, _variables.recs[id].is_open_geom);
        
    OutputControl
        *rec_offset_x_global = new OutputControl(parent, wxID_ANY, _variables.recs[id].rec_offset_x_global),
        *rec_offset_y_global = new OutputControl(parent, wxID_ANY, _variables.recs[id].rec_offset_y_global),
        *rec_offset_z_global = new OutputControl(parent, wxID_ANY, _variables.recs[id].rec_offset_z_global),
        *optical_height = new OutputControl(parent, wxID_ANY, _variables.recs[id].optical_height); 

    //informational text
    msg = "Receiver position offset is relative to the specified object.";
    wxStaticText *rmsg = new wxStaticText(parent, wxID_ANY, msg);
    rmsg->SetForegroundColour(_helptext_colour);

    sbs1->Add(rmsg);
    sbs1->Add(rec_offset_reference);
    sbs1->Add(rec_offset_x);
    sbs1->Add(rec_offset_y);
    sbs1->Add(rec_offset_z);
    sbs1->Add(optical_height);
    sbs1->Add(rec_offset_x_global);
    sbs1->Add(rec_offset_y_global);
    sbs1->Add(rec_offset_z_global);
    sbs1->Add(is_open_geom);

    //Optical properties
    wxStaticBox *sb2 = new wxStaticBox(parent, wxID_ANY, wxT("Optical properties"));
    wxStaticBoxSizer *sbs2 = new wxStaticBoxSizer(sb2, wxVERTICAL);
    InputControl
        *absorptance = new InputControl(parent, wxID_ANY, _variables.recs[id].absorptance),
        *peak_flux = new InputControl(parent, wxID_ANY, _variables.recs[id].peak_flux),
        *flux_profile_type = new InputControl(parent, wxID_ANY, _variables.recs[id].flux_profile_type);
    //string sid = my_to_string(id);    
    
    //----------------------------------------
    wxPanel *user_flux_panel = new wxPanel(parent);

    wxButton *import_user_flux = new wxButton(user_flux_panel, wxID_ANY, "Import", wxDefaultPosition, wxDefaultSize, 0L, wxDefaultValidator, sid);
    import_user_flux->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SPFrame::OnUserFluxImport), NULL, this);
    wxButton *export_user_flux = new wxButton(user_flux_panel, wxID_ANY, "Export", wxDefaultPosition, wxDefaultSize, 0L, wxDefaultValidator, sid);
    export_user_flux->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SPFrame::OnUserFluxExport), NULL, this);
    wxSpinCtrl *n_flux_x = new wxSpinCtrl(user_flux_panel, wxID_ANY, "5", wxDefaultPosition, _spin_ctrl_size, wxSP_ARROW_KEYS | wxALIGN_RIGHT, 2, 1000, 5, sid);
    n_flux_x->Connect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxCommandEventHandler(SPFrame::OnUserFluxNx), NULL, this);
    wxSpinCtrl *n_flux_y = new wxSpinCtrl(user_flux_panel, wxID_ANY, "5", wxDefaultPosition, _spin_ctrl_size, wxSP_ARROW_KEYS | wxALIGN_RIGHT, 2, 1000, 5, sid);
    n_flux_y->Connect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxCommandEventHandler(SPFrame::OnUserFluxNy), NULL, this);

    wxBoxSizer *flux_button_sizer = new wxBoxSizer(wxHORIZONTAL);
    flux_button_sizer->Add(import_user_flux, 0, wxALL, 5);
    flux_button_sizer->Add(export_user_flux, 0, wxALL, 5);
    flux_button_sizer->AddSpacer(10);
    flux_button_sizer->Add(new wxStaticText(user_flux_panel, wxID_ANY, "X-axis bins"), 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
    flux_button_sizer->Add(n_flux_x, 0, wxALL, 5);
    flux_button_sizer->AddSpacer(10);
    flux_button_sizer->Add(new wxStaticText(user_flux_panel, wxID_ANY, "Y-axis bins"), 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);
    flux_button_sizer->Add(n_flux_y, 0, wxALL, 5);

    wxBoxSizer *user_flux_sizer = new wxBoxSizer(wxVERTICAL);
    user_flux_sizer->Add(flux_button_sizer);

    wxGrid *user_flux_grid = new wxGrid(user_flux_panel, wxID_ANY, wxDefaultPosition, wxSize(250, 250), 262144L, sid);
    _user_flux_objects[id] = s_user_flux_objects(user_flux_grid, n_flux_y, n_flux_x);

    user_flux_grid->CreateGrid(5, 5);
    UpdateUserFluxGrid(id);
    user_flux_grid->Connect(wxEVT_GRID_CELL_CHANGED, wxGridEventHandler(SPFrame::OnUserFluxGridChange), NULL, this);

    user_flux_sizer->Add(user_flux_grid, 1, wxEXPAND | wxALL, 5);

    user_flux_panel->SetSizer(user_flux_sizer);

    flux_profile_type->setPanelObject("User", *user_flux_panel);
    
    {
        wxWindow* dsibs[] = { user_flux_panel, flux_profile_type };
        rec_type->addDisabledSiblings("External cylindrical", 2, dsibs);
        rec_type->addDisabledSiblings("Falling particle", 2, dsibs);

    }
    rec_type->updateInputDisplay();

    //----------------------------------------

    sbs2->Add(absorptance);
    sbs2->Add(peak_flux);
    sbs2->Add(flux_profile_type);
    sbs2->Add(user_flux_panel);

    flux_profile_type->updateInputDisplay();

    //-----Thermal losses group
    wxStaticBox *sb3 = new wxStaticBox(parent, wxID_ANY, wxT("Thermal losses"));
    wxStaticBoxSizer *sbs3 = new wxStaticBoxSizer(sb3, wxVERTICAL);

    wxStaticText *hll_lab = new wxStaticText(parent, wxID_ANY, _variables.recs[id].therm_loss_load.short_desc );
    wxFlexGridSizer *hll_gs = new wxFlexGridSizer(3,1,5);
    
	//set the variable map value equal to the first set
    int ncoefs = _variables.recs[id].therm_loss_load.val.ncells();
    wxString nsuf = wxT("thstndrdththththththththth");
    vector<wxStaticText*> hlt_coef_labs(ncoefs);
    vector<wxStaticText*> hlt_coef_units(ncoefs);
    _hll_coefs[sid] = vector<wxTextCtrl*>(1);    //initialize this page's coefficient vector
    _hll_coefs[sid].resize(ncoefs);
    wxString lab, units = wxT("[kWt/m^2]");
    for(int i=0; i<ncoefs; i++)
    {
        lab.sprintf("%d%c%c order coefficient",i,nsuf[i*2],nsuf[i*2+1]);
        if(i>0) units = "";
        hlt_coef_labs.at(i) = new wxStaticText(parent, wxID_ANY, lab, wxDefaultPosition, wxSize(190,_default_input_size.GetHeight()), wxTE_RIGHT );
        _hll_coefs[sid].at(i) = new wxTextCtrl(parent, wxID_ANY, my_to_string(_variables.recs[id].therm_loss_load.val.at(i)), 
                                                wxDefaultPosition, wxSize(100, _default_input_size.GetHeight()) );
        hlt_coef_units.at(i) = new wxStaticText(parent, wxID_ANY, units);
        hll_gs->Add(hlt_coef_labs.at(i), 0, wxRIGHT, 5);
        hll_gs->Add(_hll_coefs[sid].at(i), 0, 0, 0);
        hll_gs->Add(hlt_coef_units.at(i), wxLEFT|wxTOP,5);
    }
    wxStaticText *hlw_lab = new wxStaticText(parent, wxID_ANY, _variables.recs[id].therm_loss_wind.short_desc );
    wxFlexGridSizer *hlw_gs = new wxFlexGridSizer(3,1,5);
    //vector<string> hlw_coef_defs = split(_variables.recs[id].therm_loss_wind.val, ",");
    //set the variable map value equal to the first set
    ncoefs = _variables.recs[id].therm_loss_wind.val.ncells();
    vector<wxStaticText*> hlw_coef_labs(ncoefs);
    vector<wxStaticText*> hlw_coef_units(ncoefs);
    _hlw_coefs[sid] = vector<wxTextCtrl*>(1);    //initialize this page's coefficient vector
    _hlw_coefs[sid].resize(ncoefs);
    wxString wlab, wunits = wxT("[-]");
    for(int i=0; i<ncoefs; i++)
    {
        wlab.sprintf("%d%c%c order coefficient",i,nsuf[i*2],nsuf[i*2+1]);
        if(i>0) wunits.sprintf("[1/(m/s)^%d]",i);
        hlw_coef_labs.at(i) = new wxStaticText(parent, wxID_ANY, wlab, wxDefaultPosition, wxSize(190,_default_input_size.GetHeight()), wxTE_RIGHT );
        _hlw_coefs[sid].at(i) = new wxTextCtrl(parent, wxID_ANY, my_to_string(_variables.recs[id].therm_loss_wind.val.at(i)), 
                                                wxDefaultPosition, wxSize(100, _default_input_size.GetHeight()) );
        hlw_coef_units.at(i) = new wxStaticText(parent, wxID_ANY, wunits);
        hlw_gs->Add(hlw_coef_labs.at(i), 0, wxRIGHT, 5);
        hlw_gs->Add(_hlw_coefs[sid].at(i), 0, 0, 0);
        hlw_gs->Add(hlw_coef_units.at(i), wxLEFT|wxTOP,5);
    }
    
    InputControl
        *therm_loss_base = new InputControl(parent, wxID_ANY, _variables.recs[id].therm_loss_base),
        *piping_loss_coef = new InputControl(parent, wxID_ANY, _variables.recs[id].piping_loss_coef),
        *piping_loss_const = new InputControl(parent, wxID_ANY, _variables.recs[id].piping_loss_const);

    OutputControl
        *therm_loss = new OutputControl(parent, wxID_ANY, _variables.recs[id].therm_loss),
        *piping_loss = new OutputControl(parent, wxID_ANY, _variables.recs[id].piping_loss);

    sbs3->Add(therm_loss_base);
    sbs3->Add(hll_lab, 0, wxALL, 5);
    sbs3->Add(hll_gs, 0, wxALL, 5);
    wxStaticLine *sl = new wxStaticLine(parent, wxID_ANY, wxDefaultPosition, wxSize(1,1), wxHORIZONTAL);
    sbs3->Add(sl, 0, wxEXPAND|wxALL, 5);
    sbs3->Add(hlw_lab, 0, wxALL, 5);
    sbs3->Add(hlw_gs, 0, wxALL, 5);
    sbs3->Add(therm_loss);
    wxStaticLine *sl3 = new wxStaticLine(parent, wxID_ANY, wxDefaultPosition, wxSize(1,1), wxHORIZONTAL    );
    sbs3->Add(sl3, 0, wxEXPAND|wxALL, 5);
    sbs3->Add(piping_loss_coef);
    sbs3->Add(piping_loss_const);
    sbs3->Add(piping_loss);

    for(int i=0; i<ncoefs; i++)
    {
        //Bind each text ctrl to check on leaving whether we need to update the variable map
        _hll_coefs[sid].at(i)->Connect(wxEVT_KILL_FOCUS, wxFocusEventHandler( SPFrame::OnHeatLossLoadFocus ), NULL, this);
        _hll_coefs[sid].at(i)->SetName(sid);
        _hlw_coefs[sid].at(i)->Connect(wxEVT_KILL_FOCUS, wxFocusEventHandler( SPFrame::OnHeatLossWindFocus ), NULL, this);
        _hlw_coefs[sid].at(i)->SetName(sid);
    }
    
    //-------------------------

    //Main parent layout
    wxBoxSizer 
        *main_sizer = new wxBoxSizer(wxHORIZONTAL),
        *col_sizer_1 = new wxBoxSizer(wxVERTICAL),
        *col_sizer_2 = new wxBoxSizer(wxVERTICAL);

    col_sizer_1->Add(sbs0, 0, wxALL, 5);
    col_sizer_1->Add(sbs1, 0, wxALL, 5);
    col_sizer_2->Add(sbs2, 0, wxALL, 5);
    col_sizer_2->Add(sbs3, 0, wxALL, 5);

    main_sizer->Add(col_sizer_1);
    main_sizer->Add(col_sizer_2);

    parent->SetSizer(main_sizer);
    


    InputControl *inputs[] = {rec_type, is_polygon, n_panels, panel_rotation, rec_height, rec_diameter, rec_width, aperture_type, rec_azimuth, rec_elevation, 
                              rec_cav_rad, rec_cav_cdepth, rec_offset_reference, rec_offset_x, rec_offset_y, rec_offset_z, rec_cav_apwfrac, rec_cav_tlip, rec_cav_blip, 
                              rec_width_fall, norm_curtain_width, norm_curtain_height, max_curtain_depth, curtain_type, 
                              curtain_radius, is_snout, snout_depth, snout_vert_top_angle, snout_vert_bot_angle, snout_horiz_angle, therm_loss_base, piping_loss_coef, piping_loss_const, peak_flux,
                              flux_profile_type, absorptance, accept_ang_type, is_open_geom, accept_ang_x, accept_ang_y, NULL};
    int i=0;
    while(inputs[i] != NULL)
    { 
        _input_map[ inputs[i]->getVarObject() ] = inputs[i]; 
        i++; 
    }
    
    i=0;
    OutputControl *outputs[] = {rec_aspect, optical_height, rec_offset_x_global, rec_offset_y_global, rec_offset_z_global, rec_cav_apw, rec_cav_aph, aperture_area,
                                absorber_area, curtain_height, max_curtain_width, therm_loss, piping_loss, NULL};
    while(outputs[i] != NULL)
    { 
        _output_map[ outputs[i]->getVarObject() ] = outputs[i]; 
        i++; 
    }

    parent->SetScrollbars(10, 10, parent->GetSize().GetWidth()/10, parent->GetSize().GetWidth()/10);
    
    return;
}

void SPFrame::OnReceiverAdd( wxCommandEvent& evt )
{
    try
    {

        /* The user has prompted to add a new receiver */
        int nitems = _rec_config->GetItemCount();
        if( nitems >= _variables.recs.capacity() )
        {
            PopMessage("The maximum number of receiver templates is " + my_to_string(_variables.recs.capacity()) + ". Cannot add any more receiver templates.");
            return;
        }

        int ind = _variables.recs.size();

        //which name to use?
        //Create a popup dialog
        string name = my_to_string(nitems+1);
        name = "Receiver " + name;
    
        bool no_errs = false;
        bool cancelled = false;
        while( ! (no_errs || cancelled) )
        {
            
            rename_dlg *dlg = new rename_dlg(this, wxID_ANY, "Receiver geometry name", name, wxDefaultPosition, wxSize(350,100));
    
            if( dlg->ShowModal() == wxID_OK )
            {
                string tname = (string)dlg->getTextValue();
                // check to make sure this isn't a duplicate. Each item needs a unique name
                bool dupe = false;
                for(unsigned int i=0; i<_variables.recs.size(); i++)
                { 
                    if(tname == _variables.recs.at(i).rec_name.val) 
                        dupe = true; 
                }
                if(dupe)
                {
                    PopMessage("Please enter a unique name for this geometry.", "Warning", wxOK|wxICON_WARNING);
                    continue;
                }
                else
                {
                    no_errs = true;
                }

                //Add a receiver
                _variables.add_receiver(ind);

                if (static_cast<wxButton*>(evt.GetEventObject())->GetLabelText().Find("Add") < 0)
                {
                    //this is a copy event
                    int sel = -1;
                    sel = _rec_config->GetNextItem(sel, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
                    if (sel < 0) return;

					//assign all values by copying from reference
					for (unordered_map<std::string, spbase*>::iterator
						vit = _variables.recs.back()._local_varptrs.begin();
						vit != _variables.recs.back()._local_varptrs.end();
						vit++)
					{
						std::stringstream varname;
						varname << "receiver." << sel <<"." << split(vit->first, ".").back();
						
						vit->second->set_from_string( _variables.recs[sel]._local_varptrs[varname.str()]->as_string().c_str() );
					}
                }

                _variables.recs[ind].rec_name.val = tname;
                
            }
            else
            {
                cancelled = true;
            }
            dlg->Destroy();
        }
        if(no_errs && !cancelled)
        {
            //Re-create the solar field object
            _SF.Create(_variables);

            UpdateReceiverUITemplates();
            
            //update the input display
            UpdateCalculatedGUIValues();
        }
    }
    catch(std::exception &e)
    {
        PopMessage(e.what(), "OnReceivertempAdd Error", wxICON_ERROR|wxOK);
    }
    catch(...)
    {
        PopMessage("An unhandled error has occurred and the action could not be completed.", "OnReceivertempAdd Error", wxICON_ERROR|wxOK);
    }
    return;
}

void SPFrame::UpdateReceiverUITemplates()
{
    /* 
    Update all display entities and map values based on the current information in _variables.recs object.

    All data in _variables.recs must be up-to-date and in order!
    */

    //order pages, creating new as needed
    int ntemp = (int)_variables.recs.size();

    //Delete existing pages
    vector<std::string> existing;
    for(int i=0; i<_page_panel->GetPageCount(); i++)
    {
        if( _page_panel->GetPageGroupOwner( i ) == pageNames.receiver_master )
            existing.push_back(_page_panel->GetPageName(i));
    }

    //How many pages are already built?
    int nexisting = (int)existing.size();
        
    //Check to see if any pages need to be deleted
    if(nexisting > ntemp )
    {
        for(int i=0; i<nexisting; i++)
        {
            bool in_rtemp = false;
            for(int j=0; j<ntemp; j++)
            {
                if( existing.at(i) == _variables.recs.at(j).rec_name.val )
                {
                    in_rtemp = true;
                    continue;
                }
            }

            if(! in_rtemp )
                _page_panel->DestroyPage( existing.at(i) );
        }
    }

    //Create new pages
    bool new_bind = false;
    for(int i=nexisting; i<ntemp; i++)       
    {

        //Create a new page
        wxScrolledWindow *rtui = new wxScrolledWindow(this);
        _variables.recs.at(i).cbdata.val = (void*)rtui;
            
        CreateReceiverPage( rtui, i);
        _page_panel->InsertPage(
            _page_panel->GetPagePosition( pageNames.receiver_master )+i+1, 
            rtui, 
            _variables.recs.at(i).rec_name.val, 
            ID_ICON_RECEIVER, 1, pageNames.receiver_master);
        new_bind = true;

        //Add to the sizer
        rtui->Show(false);
        _main_sizer->Add(rtui, 1, wxEXPAND, 0);
        
    }
    
    for(int i=0; i<ntemp; i++)
    {
        //enable or disable according to state
        ((wxScrolledWindow*)_variables.recs.at(i).cbdata.val)->Enable( _variables.recs.at(i).is_enabled.val );

        //update the combo choices for the offset reference dropdown
        _input_map[ static_cast<spbase*>( &_variables.recs.at(i).rec_offset_reference ) ]->updateComboChoices();
    }

    //Update page values and bindings
    if(new_bind)
        bindControls(); //Update bindings for new page(s)

    //Update the display in the listctrl and in the selection combo
    _rec_config->DeleteAllItems();
    
    for(int i=0; i<ntemp; i++)
    {
        wxListItem row;
        row.SetId(i);
        _rec_config->InsertItem(i, row);
        _rec_config->SetItem(i, 0, _variables.recs.at(i).rec_name.val );
        _rec_config->SetItem(i, 1, _variables.recs.at(i).is_enabled.val ? "Enabled" : "Disabled" );
        _rec_config->SetItem(i, 2, _variables.recs.at(i).map_color.val);
    }

    UpdateReceiverPowerGrid();

    _page_panel->Update();
    _page_panel->Refresh();
    
    this->Layout();
    this->Update();
    this->Refresh();
}

void SPFrame::UpdateReceiverPowerGrid()
{
    /*
    Update the power limit grid on the receiver templates page
    */
    _SF.updateCalculatedReceiverPower(_variables);

    int ncol = 5;
    if (_rec_power_fractions->GetNumberCols() != ncol) {
        _rec_power_fractions->DeleteCols(0, _rec_power_fractions->GetNumberCols());
        _rec_power_fractions->AppendCols(ncol, false);
    }

    string cols[] = { "Template", "Dimensions [m,m]", "Area [m2]", "Power fraction", "Power [MWt]" };
    int dw = 80;
    int widths[] = { 120,dw,dw,dw,dw };

    _rec_power_fractions->SetRowLabelSize(40);
    for (int i = 0; i < ncol; i++)
    {
        _rec_power_fractions->SetColLabelValue(i, cols[i]);
        _rec_power_fractions->SetColSize(i, widths[i]);
    }

    //count how many receivers should go in the table. Only active ones appear
    int nrec_active = 0;
    for (int i = 0; i < _variables.recs.size(); i++)
        if (_variables.recs[i].is_enabled.val)
            nrec_active++;
    
    //set the correct number of rows in the table
    if (_rec_power_fractions->GetNumberRows() > 0)
        _rec_power_fractions->DeleteRows(0, _rec_power_fractions->GetNumberRows());

    //Fill in the data
    int cur_row = 0;
    for (int i = 0; i < (int)_variables.recs.size(); i++)
    {
        if (!_variables.recs[i].is_enabled.val)
            continue;
        //add 1 row
        _rec_power_fractions->AppendRows();

        bool is_external = _variables.recs[i].rec_type.mapval() == var_receiver::REC_TYPE::EXTERNAL_CYLINDRICAL;

        //0  name        
        _rec_power_fractions->SetCellValue(cur_row, 0, _variables.recs[i].rec_name.val);
        //1  dimensions
        _rec_power_fractions->SetCellValue(cur_row, 1,
            wxString::Format("%s (%s) x %s (%s)", 
                (is_external ? _variables.recs[i].rec_diameter.as_string() : _variables.recs[i].rec_width.as_string()).c_str(),
                is_external ? "D" : "W",
                _variables.recs[i].rec_height.as_string().c_str(),
                "H"
                )   
            );
        //2  area
        _rec_power_fractions->SetCellValue(cur_row, 2, _variables.recs[i].absorber_area.as_string());
        //3  power fraction (input)
        _rec_power_fractions->SetCellValue(cur_row, 3, _variables.recs[i].power_fraction.as_string());
        //4  power
        wxString pfs = _rec_power_fractions->GetCellValue(cur_row, 3);
        double pf;
        if ( !pfs.ToDouble(&pf) )
            pf = 1.;
        wxString newpower = wxString::Format("%.2f", _variables.recs.at(i).q_rec_des.Val() );
        _rec_power_fractions->SetCellValue(cur_row, 4, newpower);

        cur_row++;
    }

    //update the land bounding area value determined during simulation
    int icols[] = { 0, 1, 2, 4 };
    for (int i = 0; i < 4; i++)
    {
        wxGridCellAttr *attr = new wxGridCellAttr();
        attr->SetBackgroundColour(_colorTextBG);
        attr->SetTextColour(_colorTextFG);
        attr->SetReadOnly(true);
        _rec_power_fractions->SetColAttr(icols[i], attr);
    }

    _rec_power_fractions->AutoSizeColumns();

    //if the power fraction is not unity, warn the user
    double pow_frac_total = 0.;
    for (int i = 0; i < (int)_variables.recs.size(); i++)
        if (_variables.recs[i].is_enabled.val)
            pow_frac_total += _variables.recs[i].power_fraction.val;
    
    if (std::abs(pow_frac_total - 1.) > 1.e-3)
        _msg_rec_power_fractions->Show(true);
    else
        _msg_rec_power_fractions->Show(false);
}

void SPFrame::OnReceiverDel( wxCommandEvent &WXUNUSED(event))
{
    /* The user has prompted to delete an existing receiver geometry */
    try
    {
        int nitems = _rec_config->GetItemCount();
        if(nitems==1)
        {
            PopMessage("Cannot delete the only receiver.", "Cannot delete", wxICON_ERROR|wxOK);
            return;
        }
        int sel=-1;

        sel = _rec_config->GetNextItem(sel, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        if(sel<0) return;
        wxString msg = wxT("Deleting \"")+_variables.recs.at(sel).rec_name.val+wxT("\" cannot be undone. Continue?");
        int did = PopMessage(msg, "Delete heliostat template", wxOK|wxCANCEL|wxICON_EXCLAMATION);
        if(did == wxID_OK)
        {
            //Get the name of the page to delete
            string 
                pname = _variables.recs.at(sel).rec_name.val; //,
        
            //clean up the input and output map
            unordered_map<std::string, InputControl*> inswap_map; 
            {
                input_map::iterator it = _input_map.begin();
                while( it != _input_map.end() )
                {
                    std::string thisname = it->first->name;

                    if( thisname.find( "receiver." ) != std::string::npos )
                    {
                        if( thisname.find("."+my_to_string(_variables.recs.at(sel).id.val)+".") == std::string::npos )
                        {
                            //this group is not being removed, so save
                            inswap_map[ thisname ] = it->second;
                        }
                        //remove all heliostat entries from input map
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
                    if( thisname.find( "receiver." ) != std::string::npos )
                    {
                        if( thisname.find("."+my_to_string(_variables.recs.at(sel).id.val)+".") == std::string::npos )
                        {
                            //this group is not being removed, so save
                            outswap_map[ thisname ] = it->second;
                        }
                        //remove all heliostat entries from input map
                        //it = _output_map.erase( it );                    
                        _output_map.erase(it++);
                        }
                    else
                    {
                        it++;
                    }
                }
            }

            //delete the item
            _variables.drop_receiver( _variables.recs.at(sel).id.val );

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
        
            UpdateReceiverUITemplates();
        }
    }
    catch(std::exception &e)
    {
        PopMessage(e.what(), "OnRectempDel Error", wxICON_ERROR|wxOK);
    }
    catch(...)
    {
        PopMessage("An unhandled error has occurred and the action could not be completed.", "OnRectempDel Error", wxICON_ERROR|wxOK);
    }
    return;
}

void SPFrame::OnReceiverState( wxCommandEvent &WXUNUSED(event))
{
    /* The user has changed the state of the template */
    try
    {

        //get the current selected item
        int sel=-1;
    
        sel = _rec_config->GetNextItem(sel, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        if(sel<0) return;
        
        //Check to make sure not all templates are disabled
        bool any_enabled = false;
        
        for(int i=0; i<(int)_variables.recs.size(); i++)
        {
            if( i != sel )
                any_enabled = any_enabled || _variables.recs.at(i).is_enabled.val;
            else
                any_enabled = any_enabled || ! _variables.recs.at(i).is_enabled.val;
        }
        if( ! any_enabled )
            throw spexception( "At least one template must be enabled. Please enable the desired template before disabling others." );
        
        var_receiver* rv = &_variables.recs.at(sel);

        //Modify the state of the selected item
        if( rv->is_enabled.val ) {
            rv->is_enabled.val = false;
            wxListItem col0;
            col0.SetId(sel);
            _rec_config->DeleteItem(sel);
            _rec_config->InsertItem(col0);
            _rec_config->SetItem(sel, 0, rv->rec_name.val);
            _rec_config->SetItem(sel, 1, wxT("Disabled"));
        }
        else
        {
            rv->is_enabled.val = true;
            wxListItem col0;
            col0.SetId(sel);
            _rec_config->DeleteItem(sel);
            _rec_config->InsertItem(col0);
            _rec_config->SetItem(sel, 0, rv->rec_name.val);
            _rec_config->SetItem(sel, 1, wxT("Enabled"));
        }

        UpdateReceiverUITemplates();
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

void SPFrame::OnReceiverRename( wxCommandEvent &WXUNUSED(event))
{
    /* Rename the selected receiver */

    int sel=-1;

    sel = _rec_config->GetNextItem(sel, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if(sel<0) return;

    //Get the current name
    string name = _variables.recs.at(sel).rec_name.val;

    //Create a popup dialog
    rename_dlg *dlg = new rename_dlg(this, wxID_ANY, "Rename Heliostat Template", name, wxDefaultPosition, wxSize(350,100));
    
    if( dlg->ShowModal() == wxID_OK )
    {
        string tname = (string)dlg->getTextValue();
        _variables.recs.at(sel).rec_name.val = tname;
        //Rename the treebook item
        _page_panel->RenamePage( name, tname );
    }
    dlg->Destroy();

    UpdateReceiverUITemplates();
}

void SPFrame::OnReceiverColor(wxCommandEvent&)
{
    int sel = -1;
    sel = _rec_config->GetNextItem(sel, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (sel < 0) return;
    
    var_receiver* rv = &_variables.recs.at(sel);

    wxColourData cdat;
    if (rv->map_color.val == "random")
        cdat.SetColour(wxColour("BLACK"));
    else
        cdat.SetColour(wxColour(rv->map_color.val));
    
    cdat.SetChooseFull(true);

    wxColourDialog* dlg = new wxColourDialog(this, &cdat);
    if (dlg->ShowModal() == wxID_OK)
    {
        std::string cname = dlg->GetColourData().GetColour().GetAsString(wxC2S_HTML_SYNTAX).ToStdString();
        if (cname == "#FFFFFF")
            cname = "random";
        rv->map_color.set_from_string( cname.c_str() );
        UpdateReceiverUITemplates();
    }
}

void SPFrame::OnReceiverSelect( wxListEvent &event)
{
    /* 
    The user has selected a heliostat template in the master listctrl
    Check the current status, and switch the display of teh heliostat state button to enable or disable as needed
    */
    int sel = event.m_itemIndex;
    if(! _rec_state->IsEnabled() ) {
        _rec_state->Enable();
        _rec_rename->Enable();
        _rec_duplicate->Enable();
    }
    
    //check the data array to see the state of the item, and update the state button accordingly
    if( _variables.recs.at(sel).is_enabled.val )
    {
        _rec_state->SetLabel( wxT("Disable") );
    }
    else {
        _rec_state->SetLabel( wxT("Enable") );
    }
    
    return;
}

void SPFrame::OnReceiverDeselect( wxListEvent &event)
{
    /* Check if any of the items are selected, otherwise disable the state button */

    if(_rec_config->GetSelectedItemCount() == 0) {
        _rec_state->Disable();
        _rec_rename->Disable();
        _rec_duplicate->Disable();
    }
    event.Skip();

}

void SPFrame::OnReceiverPowerGridEdit( wxGridEvent &event )
{
    /*
    If the power fractions grid has been edited, handle here
    */

    int
        row = event.GetRow(),
        col = event.GetCol();
    
    //which receiver is this?
    int which_rec = 0;
    {
        int i = 0;
        while (true)
        {
            if (which_rec == row)
            {
                which_rec = i;
                break;
            }
            else if( _variables.recs[i].is_enabled.val)
            {
                which_rec++;
            }
            i++;
        }
    }
    
    wxString newtext = _rec_power_fractions->GetCellValue(row, col);
    double newnum;
    //validate
    if( newtext.ToDouble(&newnum) )
    {
        if (newnum >= 0.)
        {
            _variables.recs[which_rec].power_fraction.val = newnum;
            _rec_power_fractions->SetEvtHandlerEnabled(false);
            UpdateReceiverPowerGrid();
            _rec_power_fractions->SetEvtHandlerEnabled(true);
            return;     //if getting this far, then the action is successful. update and return
        }
    }

    //something went wrong. Notify user.
    PopMessage("Please enter a valid non-negative number.", "Input error");
    return;
}


void SPFrame::OnHeatLossLoadFocus( wxFocusEvent &event)
{
    /* 
    One of the textctrls in the heat loss group has lost focus. Update the
    value stored in the variable map and refresh the gui
    */
    string sid = (string)( (wxTextCtrl*)event.GetEventObject() )->GetName();
    int id; to_integer(sid, &id);

    matrix_t<double> *var = &_variables.recs[id].therm_loss_load.val;
    int nc = _hll_coefs[sid].size();
    for(int i=0; i<nc; i++)
    {
        wxString val = _hll_coefs[sid].at(i)->GetValue();
        double dval; to_double((string)val, &dval);
        var->at(i) = dval;
    }
    
    UpdateCalculatedGUIValues();
    event.Skip();
}

void SPFrame::OnHeatLossWindFocus( wxFocusEvent &event)
{
    /* 
    One of the textctrls in the heat loss group has lost focus. Update the
    value stored in the variable map and refresh the gui
    */
    string sid = (string)( (wxTextCtrl*)event.GetEventObject() )->GetName();
    int id; to_integer(sid, &id);

    matrix_t<double> *var = &_variables.recs[id].therm_loss_wind.val;
    int nc = _hlw_coefs[sid].size();
    for(int i=0; i<nc; i++)
    {
        wxString val = _hlw_coefs[sid].at(i)->GetValue();
        double dval; to_double((string)val, &dval);
        var->at(i) = dval;
    }
    
    UpdateCalculatedGUIValues();
    event.Skip();
}


void SPFrame::OnUserFluxImport(wxCommandEvent &evt)
{
    int recid;
    to_integer( static_cast<wxButton*>(evt.GetEventObject())->GetName().ToStdString() , &recid );

    wxFileDialog filedlg(this, "Select a file for import...", wxEmptyString, wxEmptyString,
        "Data files (*.txt;*.dat;*.csv)|*.txt;*.dat;*.csv");
    filedlg.CentreOnParent();

    //Show and process
    if (filedlg.ShowModal() == wxID_OK)
    {
        wxString info = filedlg.GetPath().c_str();
        //Try opening the file
        if (ioutil::file_exists(info.c_str()))
        {
			try
			{
				std::string
					files,
					fnames = (std::string)info,
					eol;
				ioutil::read_file(fnames, files, eol);
				wxString file(files), fname(fnames);
				std::vector<std::string> entries = split(file.ToStdString(), eol);
				int nlines = entries.size();

				//determine the delimiter
				std::vector<std::string> data;

				//Find the type of delimiter
				std::vector<std::string> delims;
				delims.push_back(",");
				delims.push_back(" ");
				delims.push_back("\t");
				delims.push_back(";");
				std::string delim = "\t";    //initialize
				unsigned int  ns = 0;
				for (unsigned int i = 0; i < delims.size(); i++)
				{
					data = split(entries.at(0), delims.at(i));
					if (data.size() > ns)
					{
						delim = delims.at(i); ns = data.size();
					}    //pick the delimiter that returns the most entries
				}
				data.clear();

				wxFormatString fmt = "%f,%f;";
				wxString stemp;
				matrix_t<double> *vval = &_variables.recs[recid].user_flux_profile.val;

				vval->clear();    //Clear
				vval->resize(nlines, ns);

				//Process all of the entries
				for (int i = 0; i < nlines; i++)
				{
					data = split(entries.at(i), delim);

					for (unsigned int j = 0; j < ns; j++)
						to_double(data.at(j), &vval->at(i, j));

				}
				//Call to the function that sets the data
				UpdateUserFluxGrid(recid);
			}
			catch (std::exception &e)
			{
				std::stringstream ss;
				ss << "The specified file could not be loaded. " << e.what();
				PopMessage(ss.str(), "Error");
			}
			catch (...)
			{
				PopMessage("The specified file could not be loaded: Unknown error.", "Error");
			}
        }
        else
        {
            //Error: the file was not found
            PopMessage("The specified file could not be loaded: File not found.", "Error");
        }
    }

}

void SPFrame::OnUserFluxExport(wxCommandEvent &evt) 
{
    int recid;
    to_integer(static_cast<wxButton*>(evt.GetEventObject())->GetName().ToStdString(), &recid);

    //Get the file
    wxFileDialog filedlg(this, "Select a path for export...", wxEmptyString, wxEmptyString,
        "Comma separated file (*.csv)|*.txt;*.csv", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    filedlg.CentreOnParent();

    //Show and process
    if (filedlg.ShowModal() == wxID_OK)
    {
        wxString info = filedlg.GetPath().c_str();

        matrix_t<double> *vals = &_variables.recs[recid].user_flux_profile.val;

        ofstream file;
        file.open((string)info);
        if (!file.is_open())
        {
            PopMessage("The specified file could not be opened for writing. Please ensure that the file is not open or write-protected.", "Error", wxICON_ERROR | wxOK);
            return;
        }

        int nc = (int)vals->ncols();

        for (int i = 0; i < (int)vals->nrows(); i++)
            for (int j = 0; j < nc; j++)
                file << vals->at(i, j) << (j < nc - 1 ? "," : "\n");

        file.close();

        //notify the user of a successful export
        wxString msg;
        msg.sprintf("%s\nFile successfully created.", info.c_str());
        PopMessage(msg, "Success");
    }
}

void SPFrame::OnUserFluxNx(wxCommandEvent &evt)
{
    int recid;
    wxSpinCtrl* spin = static_cast<wxSpinCtrl*>(evt.GetEventObject());
    to_integer(spin->GetName().ToStdString(), &recid);

    wxGrid* grid = _user_flux_objects[recid].gridptr;

    int nspin = spin->GetValue();
    int ngrid = grid->GetNumberCols();

    if (nspin < ngrid)
        grid->DeleteCols(nspin, ngrid-nspin);
    else if (nspin > ngrid)
    {
        grid->AppendCols(nspin-ngrid);

        //set the new column values to equal the last previously existing column
        for (size_t j = ngrid; j < nspin; j++)
            for (size_t i = 0; i < grid->GetNumberRows(); i++)
                grid->SetCellValue(i, j, grid->GetCellValue(i, j - 1));

        //update the col labels
        for (size_t j = ngrid; j < nspin; j++)
        {
            _user_flux_objects[recid].gridptr->SetColLabelValue(j, std::to_string(j + 1));
            _user_flux_objects[recid].gridptr->SetColSize(j, 50);
        }
    }
    else
        return;

    UpdateUserFluxData(recid);
}

void SPFrame::OnUserFluxNy(wxCommandEvent &evt)
{
    int recid;
    wxSpinCtrl* spin = static_cast<wxSpinCtrl*>(evt.GetEventObject());
    to_integer(spin->GetName().ToStdString(), &recid);
    
    if (_user_flux_objects.find(recid) == _user_flux_objects.end())
        return;     //invalid receiver ID

    wxGrid* grid = _user_flux_objects[recid].gridptr;

    int nspin = spin->GetValue();
    int ngrid = grid->GetNumberRows();

    if (nspin < ngrid)
        grid->DeleteRows(nspin, ngrid-nspin);
    else if (nspin > ngrid)
    {
        grid->AppendRows(nspin-ngrid);

        //set the new column values to equal the last previously existing column
        for (size_t j = ngrid; j < nspin; j++)
            for (size_t i = 0; i < grid->GetNumberCols(); i++)
                grid->SetCellValue(j, i, grid->GetCellValue(j-1, i));
        
    }
    else
        return;

    UpdateUserFluxData(recid);
}

void SPFrame::OnUserFluxGridChange(wxGridEvent &evt)
{
    int recid;
    to_integer(static_cast<wxGrid*>(evt.GetEventObject())->GetName().ToStdString(), &recid);
    UpdateUserFluxData(recid);
}

void SPFrame::UpdateUserFluxGrid(int id)
{
    /*
    Take the current data in the variable map string and update the interface grid.

    format:
    x1y1,x2y1,x3y1...;x1y2,x2,y2,...;...

    */

    if (id > _variables.recs.size() - 1)
        return;     //out of bounds

    matrix_t<double>* table = &_variables.recs[id].user_flux_profile.val;

    //resize the grid
    _user_flux_objects[id].nxptr->SetValue(table->ncols());
    _user_flux_objects[id].nyptr->SetValue(table->nrows());
    GridCount(table->nrows(), table->ncols(), _user_flux_objects[id].gridptr);

    //update the grid
    for (size_t i = 0; i < table->nrows(); i++)
        for (size_t j = 0; j < table->ncols(); j++)
            _user_flux_objects[id].gridptr->SetCellValue(i, j, wxString::Format("%f", table->at(i, j)));

    //update the col labels
    for (size_t j = 0; j < table->ncols(); j++)
    {
        _user_flux_objects[id].gridptr->SetColLabelValue(j, std::to_string(j + 1));
        _user_flux_objects[id].gridptr->SetColSize(j, 50);
    }

    return;
}

void SPFrame::UpdateUserFluxData(int id)
{
    /*
    Take the current data in the user flux grid and format it for storage as a string variable
    in the variable map.

    format:
    x1y1,x2y1,x3y1...;x1y2,x2,y2,...;...

    */
       
    wxGrid* grid = _user_flux_objects[id].gridptr;
    matrix_t<double>* table = &_variables.recs[id].user_flux_profile.val;

    int
        nr = grid->GetNumberRows(),
        nc = grid->GetNumberCols();

    table->resize(nr, nc);

    for (size_t i = 0; i < (size_t)nr; i++)
        for (size_t j = 0; j < (size_t)nc; j++)
            to_double(grid->GetCellValue(i, j).ToStdString(), &table->at(i, j));

    //update scaling
    Receiver::updateUserFluxNormalization(_variables.recs[id]);

    return;
}

void SPFrame::OnTroughsLocImport(wxCommandEvent& evt)
{
    int recid;
    to_integer(static_cast<wxButton*>(evt.GetEventObject())->GetName().ToStdString(), &recid);

    wxFileDialog filedlg(this, "Select a file for import...", wxEmptyString, wxEmptyString,
        "Data files (*.txt;*.dat;*.csv)|*.txt;*.dat;*.csv");
    filedlg.CentreOnParent();

    //Show and process
    if (filedlg.ShowModal() == wxID_OK)
    {
        wxString info = filedlg.GetPath().c_str();
        //Try opening the file
        if (ioutil::file_exists(info.c_str()))
        {
            try
            {
                std::string
                    files,
                    fnames = (std::string)info,
                    eol;
                ioutil::read_file(fnames, files, eol);
                wxString file(files), fname(fnames);
                std::vector<std::string> entries = split(file.ToStdString(), eol);
                int nlines = entries.size();

                //determine the delimiter
                std::vector<std::string> data;

                //Find the type of delimiter
                std::vector<std::string> delims;
                delims.push_back(",");
                delims.push_back(" ");
                delims.push_back("\t");
                delims.push_back(";");
                std::string delim = "\t";    //initialize
                unsigned int  ns = 0;
                for (unsigned int i = 0; i < delims.size(); i++)
                {
                    data = split(entries.at(0), delims.at(i));
                    if (data.size() > ns)
                    {
                        delim = delims.at(i); ns = data.size();
                    }    //pick the delimiter that returns the most entries
                }
                data.clear();

                if (ns != 2)
                {
                    std::stringstream ss;
                    ss << "Particle troughs location data must have only two columns, i.e., normalized height and normalized depth.";
                    PopMessage(ss.str(), "Error");
                    return;
                }
                if (nlines > 10)
                {
                    std::stringstream ss;
                    ss << "Maximum number of particle troughs is 10.";
                    PopMessage(ss.str(), "Error");
                    return;
                }
                wxFormatString fmt = "%f,%f;";
                wxString stemp;
                matrix_t<double>* vval = &_variables.recs[recid].norm_heights_depths.val;

                vval->clear();    //Clear
                vval->resize(nlines, ns);

                //Process all of the entries
                for (int i = 0; i < nlines; i++)
                {
                    data = split(entries.at(i), delim);

                    for (unsigned int j = 0; j < ns; j++)
                        to_double(data.at(j), &vval->at(i, j));

                }
                //Call to the function that sets the data
                UpdateTroughsLocGrid(recid);
            }
            catch (std::exception& e)
            {
                std::stringstream ss;
                ss << "The specified file could not be loaded. " << e.what();
                PopMessage(ss.str(), "Error");
            }
            catch (...)
            {
                PopMessage("The specified file could not be loaded: Unknown error.", "Error");
            }
        }
        else
        {
            //Error: the file was not found
            PopMessage("The specified file could not be loaded: File not found.", "Error");
        }
    }

}

void SPFrame::OnTroughsLocExport(wxCommandEvent& evt)
{
    int recid;
    to_integer(static_cast<wxButton*>(evt.GetEventObject())->GetName().ToStdString(), &recid);

    //Get the file
    wxFileDialog filedlg(this, "Select a path for export...", wxEmptyString, wxEmptyString,
        "Comma separated file (*.csv)|*.txt;*.csv", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
    filedlg.CentreOnParent();

    //Show and process
    if (filedlg.ShowModal() == wxID_OK)
    {
        wxString info = filedlg.GetPath().c_str();
        matrix_t<double>* vals = &_variables.recs[recid].norm_heights_depths.val;

        ofstream file;
        file.open((string)info);
        if (!file.is_open())
        {
            PopMessage("The specified file could not be opened for writing. Please ensure that the file is not open or write-protected.", "Error", wxICON_ERROR | wxOK);
            return;
        }

        int nc = (int)vals->ncols();

        for (int i = 0; i < (int)vals->nrows(); i++)
            for (int j = 0; j < nc; j++)
                file << vals->at(i, j) << (j < nc - 1 ? "," : "\n");

        file.close();

        //notify the user of a successful export
        wxString msg;
        msg.sprintf("%s\nFile successfully created.", info.c_str());
        PopMessage(msg, "Success");
    }
}

void SPFrame::OnNumTroughsSpin(wxCommandEvent& evt)
{
    int recid;
    wxSpinCtrl* spin = static_cast<wxSpinCtrl*>(evt.GetEventObject());
    to_integer(spin->GetName().ToStdString(), &recid);

    if (_troughs_loc_objects.find(recid) == _troughs_loc_objects.end())
        return;     //invalid receiver ID

    wxGrid* grid = _troughs_loc_objects[recid].gridptr;

    int nspin = spin->GetValue();
    int ngrid = grid->GetNumberRows();

    if (nspin < ngrid)
    {
        grid->DeleteRows(nspin, ngrid - nspin);
    }
    else if (nspin > ngrid)
    {
        grid->AppendRows(nspin - ngrid);
        //set the new column values to equal the last previously existing column
        for (size_t j = ngrid; j < nspin; j++)
            for (size_t i = 0; i < grid->GetNumberCols(); i++)
                if (ngrid != 0)
                    grid->SetCellValue(j, i, grid->GetCellValue(j - 1, i));
                else
                    grid->SetCellValue(j, i, "0.0");
    }
    else
        return;
    UpdateTroughLocData(recid);
}

void SPFrame::OnTroughLocChange(wxGridEvent& evt)
{
    int recid;
    to_integer(static_cast<wxGrid*>(evt.GetEventObject())->GetName().ToStdString(), &recid);
    UpdateTroughLocData(recid);
}

void SPFrame::UpdateTroughsLocGrid(int id)
{
    /*
    Take the current data in the variable map string and update the interface grid.

    format:
    x1y1,x2y1,x3y1...;x1y2,x2,y2,...;...

    */

    if (id > _variables.recs.size() - 1)
        return;     //out of bounds

    matrix_t<double>* table = &_variables.recs[id].norm_heights_depths.val;
    //resize the grid
    _troughs_loc_objects[id].num_troughs_ptr->SetValue(table->nrows());
    GridCount(table->nrows(), 2, _troughs_loc_objects[id].gridptr);

    //update the grid
    for (size_t i = 0; i < table->nrows(); i++)
        for (size_t j = 0; j < 2; j++)
            _troughs_loc_objects[id].gridptr->SetCellValue(i, j, wxString::Format("%f", table->at(i, j)));

    //update the col labels
    _troughs_loc_objects[id].gridptr->SetColLabelValue(0, "Normalized Height");
    _troughs_loc_objects[id].gridptr->SetColLabelValue(1, "Normalized Depth");
    for (size_t j = 0; j < 2; j++)
    {
        _troughs_loc_objects[id].gridptr->SetColSize(j, 130);
    }
    return;
}

void SPFrame::UpdateTroughLocData(int id)
{
    /*
    Take the current data in the trough location grid and format it for storage as a string variable
    in the variable map.

    format:
    x1y1,x2y1,x3y1...;x1y2,x2,y2,...;...

    */

    wxGrid* grid = _troughs_loc_objects[id].gridptr;
    matrix_t<double>* table = &_variables.recs[id].norm_heights_depths.val;

    int
        nr = grid->GetNumberRows(),
        nc = grid->GetNumberCols();

    table->resize(nr, nc);

    for (size_t i = 0; i < (size_t)nr; i++)
        for (size_t j = 0; j < (size_t)nc; j++)
            to_double(grid->GetCellValue(i, j).ToStdString(), &table->at(i, j));

    return;
}

