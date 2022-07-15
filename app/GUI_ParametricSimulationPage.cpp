
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
#include "LayoutSimulateThread.h"
#include "plot_select_dialog.h"
#include "IOUtil.h"
#include "interop.h"

using namespace std;


void SPFrame::CreateSimulationsParametricsTab( wxScrolledWindow *param)
{
    //-----------------Parametrics page----------------------------
    wxBoxSizer *param_sizer = new wxBoxSizer(wxVERTICAL);


    //----- standard parametric -------------
    _toggle_collapse.LoadFile(_image_dir.GetPath(true) + "collapse-list.png", wxBITMAP_TYPE_PNG);
    _toggle_expand.LoadFile(_image_dir.GetPath(true) + "expand-list.png", wxBITMAP_TYPE_PNG);
    _par_panel = new wxPanel(param, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL|wxBORDER_THEME);
    _par_toggle_shown = true;
    _par_toggle_button = new wxBitmapButton(param, wxID_ANY, _toggle_collapse);
    _par_toggle_button->SetName("par");
    _par_toggle_button->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnToggleParametric), NULL, this);

    //Run button
    _par_button = new wxBitmapButton(_par_panel, wxID_ANY, _simimages[0]);
    _par_button->SetBitmapHover(_simimages[1]);
    _par_button->SetBitmapSelected(_simimages[2]);
    _par_button->SetToolTip( wxT("Run parametric") );
    _par_button->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnParametricSimulate ), NULL, this);
    wxStaticText *sim_lab = new wxStaticText(_par_panel, wxID_ANY, wxT("  Run  \nParametric"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
    sim_lab->SetFont( wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL) );
    

    wxStaticText 
        *par_panel_lab = new wxStaticText(param, wxID_ANY, wxT("Parametric Simulation Setup")),
        *par_var_lab = new wxStaticText(_par_panel, wxID_ANY, wxT("Variables:")),
        *par_sel_lab = new wxStaticText(_par_panel, wxID_ANY, wxT("Selected Variable Values:"));
    _par_add_var = new wxButton(_par_panel, wxID_ANY, wxT("Add"));
    _par_remove_var = new wxButton(_par_panel, wxID_ANY, wxT("Remove"));
    _par_edit_var = new wxButton(_par_panel, wxID_ANY, wxT("Edit"));
    _par_edit_linkages = new wxButton(_par_panel, wxID_ANY, wxT("Edit Linkages"));

    _par_add_var->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnParAddVar ), NULL, this);
    _par_remove_var->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnParRemoveVar), NULL, this);
    _par_edit_var->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnParEditVar), NULL, this);
    _par_edit_linkages->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnParEditLinkages), NULL, this);
    
    wxFlexGridSizer *par_panel_sizer = new wxFlexGridSizer(2, 5, 5);
    //top buttons
    wxBoxSizer
        *top_left_but_sizer = new wxBoxSizer(wxHORIZONTAL),
        *top_right_but_sizer = new wxBoxSizer(wxHORIZONTAL);

    top_left_but_sizer->Add(par_var_lab, 0, 0, 0);
    top_left_but_sizer->AddStretchSpacer();
    top_left_but_sizer->Add(_par_add_var, 0, wxRIGHT, 5);
    top_left_but_sizer->Add(_par_remove_var, 0, wxRIGHT, 5);
    par_panel_sizer->Add(top_left_but_sizer, 0, wxEXPAND, 0);

    top_right_but_sizer->Add(par_sel_lab, 0, 0, 0);
    top_right_but_sizer->AddStretchSpacer();
    top_right_but_sizer->Add(_par_edit_var);
    par_panel_sizer->Add(top_right_but_sizer, 0, wxEXPAND, 0);

    //list controls
    wxArrayStr empty;
    _par_variables = new wxListBox(_par_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, empty, wxLB_SINGLE);
    _par_variables->SetMinSize(wxSize(300,150));
    _par_values = new wxListBox(_par_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, empty, wxLB_SINGLE);
    _par_values->SetMinSize(wxSize(300,150));
    par_panel_sizer->Add(_par_variables, 1, 0, 0);
    par_panel_sizer->Add(_par_values, 1, 0, 0);
    _par_variables->Connect(wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler( SPFrame::OnParVarlistSelected), NULL, this);
    _par_variables->Connect(wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( SPFrame::OnParVarDoubleClick), NULL, this);
    _par_values->Connect(wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler( SPFrame::OnParVarDoubleClick), NULL, this);
    
    //bottom buttons
    par_panel_sizer->Add(_par_edit_linkages);
    par_panel_sizer->AddStretchSpacer();

    //Progress gauges
    _par_single_gauge = new wxGauge(_par_panel, wxID_ANY, 1000, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL|wxGA_SMOOTH);
    _par_all_gauge = new wxGauge(_par_panel, wxID_ANY, 1000, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL|wxGA_SMOOTH);
    _par_single_text = new wxStaticText(_par_panel, wxID_ANY, "Simulation Progress (0/0)");
    _par_all_text = new wxStaticText(_par_panel, wxID_ANY, "Parametric Progress (0/0)");
    wxFlexGridSizer *prog_sizer = new wxFlexGridSizer(2, 5, 5);
    
    //run button and progress gauges
    wxBoxSizer *run_sizer = new wxBoxSizer(wxHORIZONTAL);
    run_sizer->Add(_par_button, 0, wxALL, 5);
    run_sizer->Add(sim_lab, 0, wxALIGN_CENTER_VERTICAL, 0);
    prog_sizer->Add(run_sizer, 0, 0, 0);
    prog_sizer->AddStretchSpacer();
    prog_sizer->Add(_par_single_gauge, 0, wxALIGN_CENTER_VERTICAL, 0);
    prog_sizer->Add(_par_single_text, 0, wxALIGN_CENTER_VERTICAL, 0);
    prog_sizer->Add(_par_all_gauge, 0, wxALIGN_CENTER_VERTICAL, 0);
    prog_sizer->Add(_par_all_text, 0, wxALIGN_CENTER_VERTICAL, 0);
    
    //Add sizer
    par_panel_sizer->Add(prog_sizer, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    //Data options
    InputControl
        *par_save_helio = new InputControl(_par_panel, wxID_ANY, _variables.par.par_save_helio),
        *par_save_summary = new InputControl(_par_panel, wxID_ANY, _variables.par.par_save_summary),
        *par_save_field_img = new InputControl(_par_panel, wxID_ANY, _variables.par.par_save_field_img),
        *par_save_flux_img = new InputControl(_par_panel, wxID_ANY, _variables.par.par_save_flux_img),
        *par_save_flux_dat = new InputControl(_par_panel, wxID_ANY, _variables.par.par_save_flux_dat);

    wxBoxSizer *par_save_sizer = new wxBoxSizer(wxVERTICAL);
    par_save_sizer->Add(par_save_helio);
    par_save_sizer->Add(par_save_summary);
    
    wxBoxSizer *par_save_field_img_sizer = new wxBoxSizer(wxHORIZONTAL);
    par_save_field_img_sizer->Add(par_save_field_img);
    wxButton *morebut = new wxButton(_par_panel, wxID_ANY, "...", wxDefaultPosition, wxSize(25,21));
    par_save_field_img_sizer->Add( morebut );
    morebut->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SPFrame::OnParFieldSaveChoicePanel), NULL, this);
    par_save_sizer->Add(par_save_field_img_sizer);

    par_save_sizer->Add(par_save_flux_img);
    par_save_sizer->Add(par_save_flux_dat);
    par_panel_sizer->Add(par_save_sizer, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    _input_map[ par_save_helio->getVarObject() ] = par_save_helio;
    _input_map[ par_save_summary->getVarObject() ] = par_save_summary;
    _input_map[ par_save_field_img->getVarObject() ] = par_save_field_img;
    _input_map[ par_save_flux_img->getVarObject() ] = par_save_flux_img;
    _input_map[ par_save_flux_dat->getVarObject() ] = par_save_flux_dat;
    wxBoxSizer *bufsizer = new wxBoxSizer(wxVERTICAL);
    bufsizer->Add(par_panel_sizer, 0, wxALL, 10);
    _par_panel->SetSizer(bufsizer);
    //-------------
    

    // ----- User-specified parametric table -----------
    wxStaticText *user_par_toggle_label = new wxStaticText(param, wxID_ANY, wxT("Simulation Variable Table"));
    _user_par_panel = new wxPanel(param, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL|wxBORDER_THEME);
    _user_par_toggle_shown = true;
    _user_par_toggle_button = new wxBitmapButton(param, wxID_ANY, _toggle_collapse);
    _user_par_toggle_button->SetName("user");
    _user_par_toggle_button->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnToggleParametric), NULL, this);

    //create objects
    _user_var_ct = new wxSpinCtrl(_user_par_panel, wxID_ANY, wxT("1"), wxDefaultPosition, _spin_ctrl_size);
    _user_var_ct->SetRange(1,100);
    _user_var_ct->Connect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxCommandEventHandler( SPFrame::OnUserVarCount ), NULL, this);
    _user_sim_ct = new wxSpinCtrl(_user_par_panel, wxID_ANY, wxT("1"), wxDefaultPosition, _spin_ctrl_size);
    _user_sim_ct->SetRange(1,100);
    _user_sim_ct->Connect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxCommandEventHandler( SPFrame::OnUserSimCount), NULL, this);

    wxStaticText 
        *user_ct_rows = new wxStaticText(_user_par_panel, wxID_ANY, wxT("Vars.")),
        *user_ct_cols = new wxStaticText(_user_par_panel, wxID_ANY, wxT("Runs"));

    wxButton
        *user_load_button = new wxButton(_user_par_panel, wxID_ANY, "Import table"),
        *user_save_button = new wxButton(_user_par_panel, wxID_ANY, "Export"),
        *user_vars_button = new wxButton(_user_par_panel, wxID_ANY, "Variables...");
    user_load_button->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnUserParImport ), NULL, this);
    user_save_button->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnUserParExport ), NULL, this);
    user_vars_button->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnUserParVariables ), NULL, this);

    //Run button
    _user_par_button = new wxBitmapButton(_user_par_panel, wxID_ANY, _simimages[0]);
    _user_par_button->SetBitmapHover(_simimages[1]);
    _user_par_button->SetBitmapSelected(_simimages[2]);
    _user_par_button->SetToolTip( wxT("Run user-specified parametric table") );
    //_user_par_button->GetToolTip()->SetDelay(_tool_tip_delay);
    _user_par_button->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnUserParSimulate ), NULL, this);
    wxStaticText *user_par_lab = new wxStaticText(_user_par_panel, wxID_ANY, wxT("Run Table"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
    user_par_lab->SetFont( wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL) );

    //grid
    _user_par_grid = new wxGrid(_user_par_panel, wxID_ANY, wxDefaultPosition, wxSize(750,250));
    _user_par_grid->CreateGrid(2,2);
    _user_par_grid->SetColLabelValue(0, "Variable");
    _user_par_grid->SetColLabelValue(1, "Sim. 1");
    _user_par_grid->SetCellValue(0,0, "Regenerate layout");
    _user_par_grid->SetReadOnly(0,0,true);
    _user_par_grid->SetColSize(0, 120);
    _user_par_grid->Connect(wxEVT_GRID_CELL_CHANGED, wxGridEventHandler( SPFrame::OnUserParEditTable ), NULL, this);
    
    //Data options
    InputControl
        *upar_save_helio = new InputControl(_user_par_panel, wxID_ANY, _variables.par.upar_save_helio),
        *upar_save_summary = new InputControl(_user_par_panel, wxID_ANY, _variables.par.upar_save_summary),
        *upar_save_field_img = new InputControl(_user_par_panel, wxID_ANY, _variables.par.upar_save_field_img),
        *upar_save_flux_img = new InputControl(_user_par_panel, wxID_ANY, _variables.par.upar_save_flux_img),
        *upar_save_flux_dat = new InputControl(_user_par_panel, wxID_ANY, _variables.par.upar_save_flux_dat);
    _input_map[ upar_save_helio->getVarObject() ] = upar_save_helio;
    _input_map[ upar_save_summary->getVarObject() ] = upar_save_summary;
    _input_map[ upar_save_field_img->getVarObject() ] = upar_save_field_img;
    _input_map[ upar_save_flux_img->getVarObject() ] = upar_save_flux_img;
    _input_map[ upar_save_flux_dat->getVarObject() ] = upar_save_flux_dat;
    

    wxBoxSizer *upar_save_field_img_sizer = new wxBoxSizer(wxHORIZONTAL);
    upar_save_field_img_sizer->Add(upar_save_field_img);
    wxButton *umorebut = new wxButton(_user_par_panel, wxID_ANY, "...", wxDefaultPosition, wxSize(25, 21));
    upar_save_field_img_sizer->Add(umorebut);
    umorebut->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SPFrame::OnParFieldSaveChoicePanel), NULL, this);
        
    //Progress gauges
    _user_single_gauge = new wxGauge(_user_par_panel, wxID_ANY, 1000, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL|wxGA_SMOOTH);
    _user_all_gauge = new wxGauge(_user_par_panel, wxID_ANY, 1000, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL|wxGA_SMOOTH);
    _user_single_text = new wxStaticText(_user_par_panel, wxID_ANY, "Simulation Progress (0/0)");
    _user_all_text = new wxStaticText(_user_par_panel, wxID_ANY, "Parametric Progress (0/0)");
    
        
    //Layout
    wxBoxSizer 
        *user_sizer = new wxBoxSizer(wxVERTICAL),
        *user_but_sizer = new wxBoxSizer(wxHORIZONTAL),
        *user_sim_sizer = new wxBoxSizer(wxHORIZONTAL),
        *user_bottom_sizer = new wxBoxSizer(wxHORIZONTAL),
        *user_bottom_left_sizer = new wxBoxSizer(wxVERTICAL);
    user_but_sizer->Add(_user_var_ct, 0, wxALL, 5);
    user_but_sizer->Add(user_ct_rows, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    user_but_sizer->Add(_user_sim_ct, 0, wxALL, 5);
    user_but_sizer->Add(user_ct_cols, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    user_but_sizer->Add(user_load_button, 0, wxALL, 5);
    user_but_sizer->Add(user_save_button, 0, wxALL, 5);
    user_but_sizer->Add(user_vars_button, 0, wxALL, 5);
    user_sizer->Add(user_but_sizer, wxEXPAND, 0);
    user_sizer->Add(_user_par_grid, 0, wxALL, 5);
    user_sim_sizer->Add(_user_par_button, 0, wxALL, 5);
    user_sim_sizer->Add(user_par_lab, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    user_bottom_left_sizer->Add(user_sim_sizer);
    //progress gauges
    wxFlexGridSizer *user_prog_sizer = new wxFlexGridSizer(2,5,5);
    user_prog_sizer->Add(_user_single_gauge, 0, wxALIGN_CENTER_VERTICAL, 0);
    user_prog_sizer->Add(_user_single_text, 0, wxALIGN_CENTER_VERTICAL, 0);
    user_prog_sizer->Add(_user_all_gauge, 0, wxALIGN_CENTER_VERTICAL, 0);
    user_prog_sizer->Add(_user_all_text, 0, wxALIGN_CENTER_VERTICAL, 0);
    user_bottom_left_sizer->Add(user_prog_sizer, 0, wxALL, 5);
    user_bottom_sizer->Add(user_bottom_left_sizer, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    wxBoxSizer *user_save_sizer = new wxBoxSizer(wxVERTICAL);
    user_save_sizer->Add(upar_save_helio);
    user_save_sizer->Add(upar_save_summary);
    //user_save_sizer->Add(upar_save_field_img);
    user_save_sizer->Add(upar_save_field_img_sizer);
    user_save_sizer->Add(upar_save_flux_img);
    user_save_sizer->Add(upar_save_flux_dat);
    user_bottom_sizer->Add(user_save_sizer, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
    user_sizer->Add(user_bottom_sizer, 0, wxALL, 5);
    _user_par_panel->SetSizerAndFit(user_sizer);
    _user_par_toggle_shown = false;
    _user_par_panel->Hide();
    _user_par_toggle_button->SetBitmap(_toggle_expand);

    //-------end user table ----------------------------

    //---- SAM batch parametric ----------
    wxStaticText *sam_par_label = new wxStaticText(param, wxID_ANY, wxT("SAM Batch Simulation"));
    _sam_param_panel = new wxPanel(param, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL|wxBORDER_THEME);
    wxBoxSizer *sam_param_sizer = new wxBoxSizer(wxVERTICAL);
        
    wxBitmap sam_bit;
    sam_bit.LoadFile(_image_dir.GetPath(true) + "Sun-64x64.png", wxBITMAP_TYPE_PNG);
    wxBitmapButton *sam_button = new wxBitmapButton(_sam_param_panel, wxID_ANY, sam_bit);
    sam_button->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnSAMInputButton ), NULL, this);
    wxStaticText *sam_label = new wxStaticText(_sam_param_panel, wxID_ANY, wxT("Generate SAM input data"));

    InputControl
        *eff_file_name = new InputControl(_sam_param_panel, wxID_ANY, _variables.par.eff_file_name),
        *flux_file_name = new InputControl(_sam_param_panel, wxID_ANY, _variables.par.flux_file_name),
        *sam_grid_format = new InputControl(_sam_param_panel, wxID_ANY, _variables.par.sam_grid_format),
        *fluxmap_format = new InputControl(_sam_param_panel, wxID_ANY, _variables.par.fluxmap_format),
        *is_fluxmap_norm = new InputControl(_sam_param_panel, wxID_ANY, _variables.par.is_fluxmap_norm);

    _input_map[ eff_file_name->getVarObject() ] = eff_file_name;
    _input_map[ flux_file_name->getVarObject() ] = flux_file_name;
    _input_map[ fluxmap_format->getVarObject() ] = fluxmap_format;
    _input_map[ is_fluxmap_norm->getVarObject() ] = is_fluxmap_norm;
    _input_map[ sam_grid_format->getVarObject() ] = sam_grid_format;
    string odir = _variables.par.sam_out_dir.val;
    if(odir.empty()) odir = _working_dir.GetPath(true);
    _dir_ctrl = new wxDirPickerCtrl(_sam_param_panel, wxID_ANY, odir);
    _dir_ctrl->SetMinSize( wxSize(400,_default_input_size.GetHeight()));
    _dir_ctrl->Connect(wxEVT_COMMAND_DIRPICKER_CHANGED, wxCommandEventHandler( SPFrame::OnSAMDirSelect ), NULL, this);
    
    wxBoxSizer *sam_sizer = new wxBoxSizer(wxHORIZONTAL);
    sam_sizer->Add(sam_button, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    sam_sizer->Add(sam_label, 1, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
    sam_param_sizer->Add(sam_sizer, 0, 0, 0);
    sam_param_sizer->Add(_dir_ctrl, 0, wxALL, 5);
    
    sam_param_sizer->Add(eff_file_name);
    sam_param_sizer->Add(flux_file_name);
    sam_param_sizer->Add(sam_grid_format);
    sam_param_sizer->Add(fluxmap_format);
    sam_param_sizer->Add(is_fluxmap_norm);
     
    _sam_param_panel->SetSizer(sam_param_sizer);

    //_sam_toggle_shown = true;
    _sam_toggle_button = new wxBitmapButton(param, wxID_ANY, _toggle_collapse);
    _sam_toggle_button->SetName("sam");
    _sam_toggle_button->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnToggleParametric), NULL, this);
    _sam_toggle_shown = false;
    _sam_param_panel->Hide();
    _sam_toggle_button->SetBitmap(_toggle_expand);
    //----- end SAM batch parametric ------------

    wxFlexGridSizer *main_grid = new wxFlexGridSizer(2, 5, 5);
    main_grid->Add(_par_toggle_button, 0, wxALIGN_CENTER_VERTICAL, 0);
    main_grid->Add(par_panel_lab, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    main_grid->AddSpacer(1);
    main_grid->Add(_par_panel, 0, wxALL, 5);

    main_grid->Add(_user_par_toggle_button, 0, wxALIGN_CENTER_VERTICAL, 0);
    main_grid->Add(user_par_toggle_label, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    main_grid->AddSpacer(1);
    main_grid->Add(_user_par_panel, 0, wxALL, 5);

    main_grid->Add(_sam_toggle_button, 0, wxALIGN_CENTER_VERTICAL, 0);
    main_grid->Add(sam_par_label, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    main_grid->AddSpacer(1);
    main_grid->Add(_sam_param_panel, 0, wxALL, 5);

    param_sizer->Add(main_grid, 0, 0, 0);
    
    //
    param->SetSizer(param_sizer);
    param->SetScrollbars(10, 10, param->GetSize().GetWidth()/10, param->GetSize().GetWidth()/10);
    return;
}

void SPFrame::ParProgressUpdate(int n_complete, int n_tot)
{
    string notices = "";
    SimProgressBase(n_complete, n_tot, &notices, _par_all_gauge, _par_all_text, "Parametric Progress");
}

void SPFrame::UserParProgressUpdate(int n_complete, int n_tot)
{
    string notices = "";
    SimProgressBase(n_complete, n_tot, &notices, _user_all_gauge, _user_all_text, "Parametric Progress");
}

void SPFrame::ParRefreshValuesList()
{
    int idx = _par_variables->GetSelection();
    _par_values->Clear();
    if(idx<0 || _par_variables->GetCount() == 0) return;

    int nitems = _par_data.at(idx).selections.size();
    wxArrayStr initems;
    if(_par_data.at(idx).varname == "ambient.0.weather_file")
    {
        for(int i=0; i<(int)_par_data.at(idx).selections.size(); i++)
            initems.push_back( ioutil::name_only( _par_data.at(idx).selections[i] ) );
    }
    else
    {
        initems = _par_data.at(idx).selections;
    }

    if(nitems>0) _par_values->InsertItems(initems, 0);
}

void SPFrame::ParReloadVariableList()
{
    wxArrayStr var_items;
    for(int i=0; i<_par_data.size(); i++)
        var_items.push_back( _par_data.at(i).display_text 
            + (_par_data.at(i).linked ?  " [linked]" :  "") 
            + (_par_data.at(i).layout_required ? " [layout]" : ""));
    

    //Add values to the variable list control
    _par_variables->Clear();
    if((int)var_items.Count()>0) _par_variables->InsertItems(var_items, 0);
}

void SPFrame::OnSAMInputButton( wxCommandEvent &WXUNUSED(event))
{
    /* The SAM input generator button on the parametrics page has been clicked. 
    Call the general method */
    try
    {
        SAMInputParametric();
    }
    catch(std::exception &e)
    {
        PopMessage(e.what(), "SAM Input Parametric Error", wxICON_ERROR|wxOK);
    }
    catch(...)
    {
        PopMessage("Caught unspecified error. Simulation is terminating.", "SAM Input Parametric Error", wxICON_ERROR|wxOK);
        return;
    }
}

void SPFrame::OnSAMDirSelect( wxCommandEvent &WXUNUSED(event))
{
    //Update the local variable
    _variables.par.sam_out_dir.val = _dir_ctrl->GetPath();
}

void SPFrame::OnToggleParametric( wxCommandEvent &event)
{
    /* 
    Collapse/expand the relevant parametric panel.
    */

    wxBitmapButton *evtbutton = (wxBitmapButton*)event.GetEventObject();
    if(evtbutton->GetName() == "sam")
    {
        if(_sam_toggle_shown)
        {
            //Hide
            _sam_toggle_shown = false;
            _sam_param_panel->Hide();
            _sam_toggle_button->SetBitmap(_toggle_expand);
        }
        else
        {
            //show
            _sam_toggle_shown = true;
            _sam_param_panel->Show();
            _sam_toggle_button->SetBitmap(_toggle_collapse);
            //Hide
            _par_toggle_shown = false;
            _par_panel->Hide();
            _par_toggle_button->SetBitmap(_toggle_expand);
            //Hide
            _user_par_toggle_shown = false;
            _user_par_panel->Hide();
            _user_par_toggle_button->SetBitmap(_toggle_expand);
        }
        _sam_param_panel->GetParent()->Layout();
        _sam_param_panel->GetParent()->Refresh();
        return;
    }
    if(evtbutton->GetName() == "par")
    {
        if(_par_toggle_shown)
        {
            //Hide
            _par_toggle_shown = false;
            _par_panel->Hide();
            _par_toggle_button->SetBitmap(_toggle_expand);
        }
        else
        {
            //show
            _par_toggle_shown = true;
            _par_panel->Show();
            _par_toggle_button->SetBitmap(_toggle_collapse);
            //Hide
            _sam_toggle_shown = false;
            _sam_param_panel->Hide();
            _sam_toggle_button->SetBitmap(_toggle_expand);
            //Hide
            _user_par_toggle_shown = false;
            _user_par_panel->Hide();
            _user_par_toggle_button->SetBitmap(_toggle_expand);
        }
        _par_panel->GetParent()->Layout();
        _par_panel->GetParent()->Refresh();
        return;
    }
    if(evtbutton->GetName() == "user")
    {
        if(_user_par_toggle_shown)
        {
            //Hide
            _user_par_toggle_shown = false;
            _user_par_panel->Hide();
            _user_par_toggle_button->SetBitmap(_toggle_expand);
        }
        else
        {
            //show
            _user_par_toggle_shown = true;
            _user_par_panel->Show();
            _user_par_toggle_button->SetBitmap(_toggle_collapse);
            //Hide
            _sam_toggle_shown = false;
            _sam_param_panel->Hide();
            _sam_toggle_button->SetBitmap(_toggle_expand);
            //Hide
            _par_toggle_shown = false;
            _par_panel->Hide();
            _par_toggle_button->SetBitmap(_toggle_expand);
        }
        _user_par_panel->GetParent()->Layout();
        _user_par_panel->GetParent()->Refresh();
        return;
    }



}

void SPFrame::OnParAddVar( wxCommandEvent &WXUNUSED(event))
{
    try
    {
        //Call the new parametric variable dialog
        par_variables_dialog *dlg = new par_variables_dialog(this, wxID_ANY, _image_dir.GetPath(true), false, wxT("Add parametric variables"));
        dlg->SetItems(&_variables);
        //Set the current selections
        wxArrayStr checked;
        for(int i=0; i<_par_data.size(); i++)
            checked.Add( _par_data[i].varname );
        dlg->SetCheckedNames( checked );
        dlg->SetSize(450,550);
    
        if(dlg->ShowModal() == wxID_OK)
        {
            wxArrayStr vals = dlg->GetCheckedNames();

            //Add the weather file locations
            ArrayString wfiles;
            std::string cpath = _climate_dir.GetPath(true).ToStdString();
            gui_util::list_files( cpath, wfiles, "eps csv tm2 tm3 tmy");
            _par_data.SetWeatherFileList(wfiles);

            int nnew=0;
            for(int i=0; i<(int)vals.size(); i++)
            {
                if(_par_data.Index( vals[i].ToStdString() ) > -1) continue;    //If the item already exists, dont mess with it.
                _par_data.addVar( _variables._varptrs.at(vals[i].ToStdString() ) ); 
                nnew++;
            }

            if(nnew>0) ParReloadVariableList();
            ParRefreshValuesList();
        }
    }
    catch(std::exception &e)
    {
        PopMessage(e.what(), "OnParAddVar Error", wxICON_ERROR|wxOK);
    }
    catch(...)
    {
        PopMessage("Caught unspecified error. Simulation is terminating.", "OnParAddVar Error", wxICON_ERROR|wxOK);
        return;
    }
}

void SPFrame::OnParRemoveVar( wxCommandEvent &WXUNUSED(event))
{
        /*if (!mCaseWin || !mCase || !mPSimInf)
        return;*/
    try
    {
        
        int idx = _par_variables->GetSelection();
        if (idx < 0)
            wxMessageBox("No variable selected!");
        else
        {
            _par_data.remove(idx); 
            ParReloadVariableList();
        }

    
        if (_par_variables->GetCount() > 0)
            _par_variables->Select(idx-1 >= 0 ? idx-1 : idx );
        ParRefreshValuesList();
    }
    catch(std::exception &e)
    {
        PopMessage(e.what(), "OnParRemoveVar Error", wxICON_ERROR|wxOK);
    }
    catch(...)
    {
        PopMessage("Caught unspecified error. Simulation is terminating.", "OnParRemoveVar Error", wxICON_ERROR|wxOK);
        return;
    }
}

void SPFrame::OnParEditVar( wxCommandEvent &WXUNUSED(event))
{
    try
    {
    
        int idx = _par_variables->GetSelection();
        if (idx < 0)
            wxMessageBox("No variable selected!");
        else
        {
            par_values_dialog *dlg = new par_values_dialog((wxWindow*)NULL, wxID_ANY, _image_dir.GetPath(true), wxT("Edit parametric values"));
            wxArrayStr values = _par_data.at(idx).selections;    //Initialize
            if( dlg->ShowEditValuesDialog(wxT("Edit parametric values"), values, &_par_data.at(idx)) )
            {    //modifies the 'values' array

                //get the variable values and display in the value list
                if(_par_data.at(idx).varname == "ambient.0.weather_file")
                {
                    _par_data.at(idx).selections.Clear();
                    for(int i=0; i<(int)values.size(); i++)
                        _par_data.at(idx).selections.push_back( _climate_dir.GetPath(true).ToStdString()+ioutil::name_only(values[i].ToStdString()) );
                }
                else
                {
                    _par_data.at(idx).selections.Clear();
                    for(int i=0; i<(int)values.size(); i++)
                        _par_data.at(idx).selections.push_back(values[i].ToStdString());
                }
                _par_data.at(idx).layout_required = dlg->IsLayoutRequired();
                ParRefreshValuesList();
                ParReloadVariableList();
            }

        
        }
    }
    catch(std::exception &e)
    {
        PopMessage(e.what(), "OnParEditVar Error", wxICON_ERROR|wxOK);
    }
    catch(...)
    {
        PopMessage("Caught unspecified error. Simulation is terminating.", "OnParEditVar Error", wxICON_ERROR|wxOK);
        return;
    }
}

void SPFrame::OnParEditLinkages( wxCommandEvent &WXUNUSED(event))
{
    
    try
    {
        wxArrayStr names;
        wxArrayStr labels;
        wxArrayStr cur_values;
        wxArrayStr contexts;
        for (int i=0;i<_par_data.size(); i++)
        {
            names.Add( _par_data.at(i).varname);
            labels.Add( _par_data.at(i).display_text );
            if(_par_data.at(i).linked) cur_values.Add( labels.back() );
            contexts.Add( wxT("Linked Parametric Variables") );
        
        }
        
        par_variables_dialog *dlg = new par_variables_dialog(this, wxID_ANY, _image_dir.GetPath(true), false, wxT("Choose Linked Parametric Variables"), wxDefaultPosition, wxSize(400,400));
        dlg->SetItems(names, labels, contexts);
        dlg->SetCheckedNames(cur_values);
        if(dlg->ShowModal() != wxID_OK) return;

        cur_values.Clear();
        cur_values = dlg->GetCheckedNames();

        //mark the selected variables as linked
        for(int i=0; i<(int)_par_data.size(); i++) _par_data.at(i).linked = false;
        for(int i=0; i<(int)cur_values.Count(); i++)
        {
            int idx = names.Index(cur_values[i]);
            if(idx == wxNOT_FOUND) continue;
            _par_data.at(idx).linked = true;
        }
        //make sure all of the linkages have the same number of values
        bool count_error = false;
        int lcount0=-1;
        for(int i=0; i<_par_data.size(); i++)
        {
            if(_par_data.at(i).linked)
            {
                if(lcount0<0)
                {
                    lcount0 = _par_data.at(i).selections.size();
                }
                else
                {
                    count_error = count_error || (int)_par_data.at(i).selections.size() != lcount0;
                }
            }
        }
        if(count_error)
            throw spexception("Linked variables must have the same number of values! Please modify the linked variable values before simulating.");

        ParReloadVariableList();
        ParRefreshValuesList();
    }
    catch(std::exception &e)
    {
        PopMessage(e.what(), "OnParEditLinkages Error", wxICON_ERROR|wxOK);
    }
    catch(...)
    {
        PopMessage("Caught unspecified error. Simulation is terminating.", "OnParEditLinkages Error", wxICON_ERROR|wxOK);
        return;
    }
}

void SPFrame::OnParFieldSaveChoicePanel(wxCommandEvent &)
{
    std::vector< std::string > choices = _plot_frame->GetPlotChoices();

    PlotSelectDialog dlg(this, wxID_ANY, "Select saved figures", choices);
    
    dlg.SetSelections(&_plot_export_selections);
    dlg.SetAnnotations(&_plot_annot_selections);

    if (dlg.ShowModal() == wxID_APPLY)
    {
        _plot_export_selections = dlg.GetSelectionIds();
        _plot_annot_selections = dlg.GetAnnotationIds();
    }
    else
    {}
}

void SPFrame::OnParametricSimulate( wxCommandEvent &WXUNUSED(event))
{
    /* The parametric simulate button has been pressed */
    try
    {    

        if(_in_flux_simulation || _in_layout_simulation || _in_user_param_simulation || _in_optimize_simulation)
        { 
            //Notify user
            PopMessage("A simulation is currently underway. Please cancel the simulation or wait"
                "for completion before beginning a new analysis.", "Error");
            return;
        }
        if(! _in_param_simulation)
        {
            SetSimulationStatus(true, _in_param_simulation, _par_button);
        }
        else
        {
            //cancel the method if necessary
            _sim_control._cancel_simulation = true;
            if(_sim_control._is_mt_simulation && _simthread != 0)
            {
                //For a multithreaded simulation, cancel all of the threads
                for(int i=0; i< _sim_control._n_threads_active; i++)
                {
                    _simthread[i].CancelSimulation();
                }
            }
            else
            {    
                _par_SF.CancelSimulation(); 
            }
            PopMessage(wxT("Simulation cancelled by user."), wxT("Notice"), wxICON_INFORMATION|wxOK);
            return;    //Don't do any clean up here, let the first call do the clean up when it exits
        }

        //validate
        if(! ParCheckSetup(_par_data, _variables) )
        {
            StopSimTimer();
            SetSimulationStatus(false, _in_param_simulation, _par_button);
            _sim_control._is_mt_simulation = false;    //False by default
            _sim_control._n_threads_active = _sim_control._n_threads;
            return;
        }

        StartSimTimer();
        //initialize subset of key status variables for par_sf
        _par_SF.Clean();

        ParametricSimulate(_par_data);
    
        DoResultsPage();
            
        //Show the simulation results
        _page_panel->SetActivePage( pageNames.results_summary );
        this->GetSizer()->Layout();

    }
    catch(std::exception &e)
    {
        PopMessage(e.what(), "OnParametricSimulate error", wxICON_ERROR|wxOK);
    }
    catch(...)
    {
        PopMessage("An unspecified error occured during parametric simulation.", "OnParametricSimulate error", wxICON_ERROR|wxOK);
    }

    StopSimTimer();
    SetSimulationStatus(false, _in_param_simulation, _par_button);
    _sim_control._is_mt_simulation = false;    //False by default
    _sim_control._n_threads_active = _sim_control._n_threads;

}

void SPFrame::OnParVarlistSelected( wxCommandEvent &WXUNUSED(event))
{
    try
    {
        ParRefreshValuesList();
    }
    catch(std::exception &e)
    {
        PopMessage(e.what(), "OnParVarlistSelected Error", wxICON_ERROR|wxOK);
    }
    catch(...)
    {
        PopMessage("Caught unspecified error. Simulation is terminating.", "OnParVarlistSelected Error", wxICON_ERROR|wxOK);
        return;
    }

}

void SPFrame::OnParVarDoubleClick( wxCommandEvent &event)
{

    try
    {
        OnParEditVar(event);
    }
    catch(std::exception &e)
    {
        PopMessage(e.what(), "OnParVarDoubleClick Error", wxICON_ERROR|wxOK);
    }
    catch(...)
    {
        PopMessage("Caught unspecified error. Simulation is terminating.", "OnParVarDoubleClick Error", wxICON_ERROR|wxOK);
        return;
    }
}

void SPFrame::OnUserVarCount( wxCommandEvent &WXUNUSED(event))
{
    try
    {
        int nnew = _user_var_ct->GetValue();
        int nold = _user_par_grid->GetNumberRows()-1;
        int ndiff = nnew - nold;
        if(ndiff < 0)
        {
            _user_par_grid->DeleteRows(nold+ndiff+1, -ndiff, false);
        }
        else if(ndiff > 0)
        {
            _user_par_grid->AppendRows(ndiff, false);
        }
    }
    catch(std::exception &e)
    {
        PopMessage(e.what(), "OnUserVarCount Error", wxICON_ERROR|wxOK);
    }
    catch(...)
    {
        PopMessage("Caught unspecified error. Simulation is terminating.", "OnUserVarCount Error", wxICON_ERROR|wxOK);
        return;
    }
}

void SPFrame::OnUserSimCount( wxCommandEvent &WXUNUSED(event))
{
    try
    {
        int nnew = _user_sim_ct->GetValue();
        int nold = _user_par_grid->GetNumberCols()-1;
        int ndiff = nnew - nold;
        if(ndiff < 0)
        {
            _user_par_grid->DeleteCols(nold+ndiff+1, -ndiff, false);
        }
        else if(ndiff > 0)
        {
            _user_par_grid->AppendCols(ndiff);
        }
        for(int i=1; i<nnew+1; i++)
            _user_par_grid->SetColLabelValue(i, wxString::Format("Sim. %d", i) );
        _user_par_grid->SetColLabelValue(0, "Variable");
    }
    catch(std::exception &e)
    {
        PopMessage(e.what(), "OnUserSimCount Error", wxICON_ERROR|wxOK);
    }
    catch(...)
    {
        PopMessage("Caught unspecified error. Simulation is terminating.", "OnUserSimCount Error", wxICON_ERROR|wxOK);
        return;
    }
}

void SPFrame::OnUserParImport( wxCommandEvent &WXUNUSED(event) )
{
    try
    {
        UserParImport();
    }
    catch(std::exception &e)
    {
        PopMessage(e.what(), "OnUserParImport Error", wxICON_ERROR|wxOK);
    }
    catch(...)
    {
        PopMessage("Caught unspecified error. Simulation is terminating.", "OnUserParImport Error", wxICON_ERROR|wxOK);
        return;
    }
}

void SPFrame::OnUserParExport( wxCommandEvent &WXUNUSED(event))
{
    //Export the contents of the parametric to a text file
    try
    {
        //Prompt for file location
        wxFileDialog *fdlg = new wxFileDialog(this, "Select file destination", wxEmptyString, wxEmptyString, "CSV files (*.csv)|*.csv", wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
        if(fdlg->ShowModal() == wxID_OK)
        {
            //Get the path
            wxString fpath = fdlg->GetPath();
            wxTextFile fout(fpath);
            if(! (fout.Exists() ? fout.Open() : fout.Create()) )
            {
                PopMessage("The file could not be openend for writing. Please make sure the specified path is correct and that the file is not open or write-protected.");
                return;
            }
            fout.Clear();
            //Write the cell contents to the file, CSV style
            int 
                nrow = _user_par_grid->GetNumberRows(),
                ncol = _user_par_grid->GetNumberCols();
            wxString line;
            for(int i=0; i<nrow; i++)
            {
                line.Clear();
                for(int j=0; j<ncol; j++)
                {
                    line.Append( _user_par_grid->GetCellValue(i, j) + ",");
                }
                fout.AddLine(line);
            }
            fout.Write();
            fout.Close();
        }
    }
    catch(std::exception &e)
    {
        PopMessage(e.what(), "OnUserParExport Error", wxICON_ERROR|wxOK);
    }
    catch(...)
    {
        PopMessage("Caught unspecified error. Simulation is terminating.", "OnUserParExport Error", wxICON_ERROR|wxOK);
        return;
    }
}

void SPFrame::OnUserParVariables( wxCommandEvent &WXUNUSED(event))
{
    //Print a list of available parametric variables to a CSV file
    try
    {
        //file path
        wxFileDialog *fdlg = new wxFileDialog(this, "Select file destination", wxEmptyString, wxEmptyString, "Comma-separated files (*.csv)|*.csv", wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
        if(fdlg->ShowModal() == wxID_OK)
        {
            //Get the path
            wxString fpath = fdlg->GetPath();
            wxTextFile fout(fpath);
            if(! (fout.Exists() ? fout.Open() : fout.Create()) )
            {
                PopMessage("The file could not be openend for writing. Please make sure the specified path is correct and that the file is not open or write-protected.");
                return;
            }
            fout.Clear();
            //Write all of the parameterizable variables
            vector<string> vars;
            for( unordered_map<string, spbase*>::iterator var=_variables._varptrs.begin(); var != _variables._varptrs.end(); var++)
            {
                string svar = var->second->as_string();
                if( (int)svar.size() > 25 ) continue;       //don't write long variable values
                if( ! var->second->is_param ) continue; //only write parameterizable variables
                if( var->first.find(".") == string::npos ) continue;  //only write properly formatted variables

                string sopts = "";

                //if this is a combo, append all of the choices
                if( var->second->ctype == "combo" )
                {
                    if( var->second->dattype == SP_DATTYPE::SP_STRING )
                    {
                        sopts.append(" [");
                        sopts.append( join( static_cast< spvar<string>* >(var->second)->combo_get_choices(), ";" ) );
                        sopts.append("]");
                    }
                }

                vars.push_back( (string)wxString::Format("%s,%s,%s,%s,%s", var->first.c_str(), var->second->as_string().c_str(), var->second->units.c_str(), var->second->short_desc.c_str(), sopts) ); 
            }

            quicksort(vars);

            for(int i=0; i<vars.size(); i++)
                fout.AddLine( vars.at(i) );

            fout.Write();
            fout.Close();

            //open the file
            wxPlatformInfo P = wxPlatformInfo::Get();
            switch(P.GetOperatingSystemId())
            {
            case wxOperatingSystemId::wxOS_WINDOWS:
            case wxOperatingSystemId::wxOS_WINDOWS_NT:
            case wxOperatingSystemId::wxOS_WINDOWS_CE:
            {
                wxExecute("explorer "+wxPathOnly(fpath), wxEXEC_ASYNC, NULL);    //Windows
                break;
            }
            case wxOperatingSystemId::wxOS_MAC:
                wxExecute("open "+wxPathOnly(fpath), wxEXEC_ASYNC, NULL);    //Mac
                break;
            case wxOperatingSystemId::wxOS_UNIX_LINUX:
                wxExecute("xdg-open "+wxPathOnly(fpath), wxEXEC_ASYNC, NULL);
                break;
            }

        
        }



    }
    catch(std::exception &e)
    {
        PopMessage(e.what(), "OnUserParVariables Error", wxICON_ERROR|wxOK);
    }
    catch(...)
    {
        PopMessage("Caught unspecified error. Simulation is terminating.", "OnUserParVariables Error", wxICON_ERROR|wxOK);
        return;
    }
}

void SPFrame::OnUserParSimulate( wxCommandEvent &WXUNUSED(event))
{
    /* 
    Run a parametric simulation based on a table of variable values provided by the user on the
    parametrics page.

    Regeneration of the field layout is specified with a variable line in the table.
    */
    try
    {
        if(_in_flux_simulation || _in_layout_simulation || _in_param_simulation || _in_optimize_simulation)
        { 
            //Notify user
            PopMessage("A simulation is currently underway. Please cancel the simulation or wait"
                "for completion before beginning a new analysis.", "Error");
            return;
        }
        if(! _in_user_param_simulation)
        {
            SetSimulationStatus(true, _in_user_param_simulation, _user_par_button);
        }
        else
        {
            //cancel the method if necessary
            _sim_control._cancel_simulation = true;
            if(_sim_control._is_mt_simulation && _simthread != 0)
            {
                //For a multithreaded simulation, cancel all of the threads
                for(int i=0; i< _sim_control._n_threads_active; i++)
                    _simthread[i].CancelSimulation();
            }
            else
            {    
                _par_SF.CancelSimulation(); 
            }
            PopMessage(wxT("Simulation cancelled by user."), wxT("Notice"), wxICON_INFORMATION|wxOK);
            return;    //Don't do any clean up here, let the first call do the clean up when it exits
        }

        //get the information output settings
        bool save_summary = _variables.par.upar_save_summary.val;
        bool save_field = _variables.par.upar_save_helio.val; 
        bool save_field_img = _variables.par.upar_save_field_img.val;
        bool save_flux_img = _variables.par.upar_save_flux_img.val;
        bool save_flux_dat = _variables.par.upar_save_flux_dat.val;

        //check all of the entries in _user_sim_table to make sure they're actually variables
        ArrayString all_vars;
        for( unordered_map<string, spbase*>::iterator var=_variables._varptrs.begin(); var != _variables._varptrs.end(); var++)
        {
            //Only include variables that meet the parametric variable inclusion requirements
            if(! var->second->is_param ) continue;  //only write parameterizable variables
            if( var->first.find(".") == string::npos) continue;  //Only write properly formatted variables
            
            all_vars.push_back(var->first);
        }

        ArrayString simvars, any_not_found;
        bool has_regen_layout = false;
        _user_sim_table.getKeys(simvars);
        for(int i=0; i<(int)simvars.size(); i++)
        {
            if( lower_case(simvars[i]) == "regenerate layout")
            { 
                has_regen_layout = true;    //This row is required, so check for it
                continue;
            }
            bool not_found = all_vars.Index( simvars.at(i) ) == wxNOT_FOUND;
            if(not_found) any_not_found.push_back(simvars.at(i));
        }

        //Was the "Regenerate layout" row included in the table? If not, prompt the user for what to do.
        if(! has_regen_layout)
        {
            PopMessage("The user parametric table requires a row labeled \"Regenerate layout\" specifying whether each run requires a new field layout. "
                "Please add this row to the table to continue.", "Parametric setup warning", wxICON_WARNING|wxOK);
            StopSimTimer();
            SetSimulationStatus(false, _in_user_param_simulation, _user_par_button);
            return;
        }
        //validate
        if(! ParCheckSetup(simvars, _variables) )
        {
            StopSimTimer();
            SetSimulationStatus(false, _in_user_param_simulation, _user_par_button);
            return;
        }

        //If any weren't found, notify the user and quit
        if(any_not_found.size() > 0)
        {
            wxString varlist = "Simulation Error: The following user-specified variables were not found in the variable library:\n";
            for(int i=0; i<(int)any_not_found.size(); i++)
                varlist.Append( any_not_found.at(i) + "\n" );
            PopMessage(varlist);
            StopSimTimer();
            SetSimulationStatus(false, _in_user_param_simulation, _user_par_button);
            return;
        }

        //if the user wants to save the field, ask which options they would like
        //Prompt the user to specify what information they would like to export
        vector<bool> options;
        wxString delim;
        if(save_field)
        {
            layout_export_dialog *ld = new layout_export_dialog(this, wxID_ANY, wxT("Data export options"));
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
        }
        wxString save_field_dir;
        if(save_field || save_summary || save_field_img || save_flux_img)
        {
        //If the current file is not associated with a directory, prompt the user to select a directory
        if(!_open_file.FileExists())
        {
            wxDirDialog *ddlg = new wxDirDialog(this, "Please select a location for exported performance data...", _working_dir.GetPath(), wxDD_DEFAULT_STYLE|wxDD_DIR_MUST_EXIST);
            if( ddlg->ShowModal() != wxID_OK ) return;
            save_field_dir = ddlg->GetPath();
            ddlg->Destroy();
        }
        else
        {
            save_field_dir = _open_file.GetPath();
        }
    }

        //If none of the simulations require a new layout, enable the option to simulate selected heliostats
        bool any_need_layout = false;
        for(int i=0; i<_user_sim_table.nsim(); i++)
        {
            string sval = lower_case( _user_sim_table["Regenerate layout"].at(i) );
            if( sval.find("y") != string::npos || sval.find("t") != string::npos )
            {
                any_need_layout = true;
                break;
            }
        }

    
        Hvector perf_helios;
        bool perf_helios_filtered = false;
        if( (!any_need_layout) && _flux_lc_check->GetValue() )
        {
            perf_helios_filtered = true;
            if(! GetSelectedHeliostats(perf_helios, true)) return;
        }
    
        //At this point, all of the variables should be valid
        StartSimTimer();

        SetSimulationStatus(true, _in_user_param_simulation, _user_par_button);
    
        //determine the number of simulations
        int  nsim = _user_sim_table.nsim();

        //clear previous results
        _results.clear();

        for(int i=0; i<nsim; i++)
        {
            UserParProgressUpdate(i, nsim);
        
            //Create a new variable map
            var_map vset(_variables);

            //Set all of the values for this iteration
            string layout_str = lower_case( _user_sim_table["Regenerate layout"].at(i) );
            bool full_layout = 
                    layout_str.find("t") != string::npos 
                ||  layout_str.find("y") != string::npos
                ||  layout_str.find("1") != string::npos;

            //iterate over all variable keys
            std::vector< std::string > variable_record;
            for(int j=0; j<(int)simvars.size(); j++)
            {
                if( lower_case(simvars[j]) == "regenerate layout") continue;

                spbase *vdat = vset._varptrs.at(simvars[j]); //getVarByString(vset, keys[j]);

                //set the variable in the map
                std::string vval = _user_sim_table[simvars[j]].at(i);
                vdat->set_from_string( vval.c_str() );

                //keep track of variable name and value for reporting
                variable_record.push_back(
                    wxString::Format("%s,%s;", vdat->short_desc.c_str(), vval.c_str()).ToStdString()
                );
            }

            //Update the climate file if needed
            if(_variables.amb.weather_file.val != vset.amb.weather_file.val)
            {
                wxString val = vset.amb.weather_file.val;
                UpdateClimateFile(val, vset, true);
            }

            //Update the design method box
            UpdateDesignSelect( vset.sf.des_sim_detail.mapval(), vset);

            //Set up the solar field
            _par_SF.Clean();
            _par_SF.Create(vset);
            _par_SF.getSimInfoObject()->setCallbackFunction( SolarFieldInfoCallback, this);
        
            bool sim_cancelled = false;

            if(full_layout)
            {
                sim_cancelled = sim_cancelled || !interop::DoManagedLayout(_sim_control, _par_SF, vset, _simthread);
            }
            else
            {
                //just update the geometry
                if(! vset.sf.layout_data.val.empty() )
                {
                    _par_SF.PrepareFieldLayout(_par_SF, 0, true);    if(_par_SF.ErrCheck()) return;

                    double azzen[2];
                    _par_SF.CalcDesignPtSunPosition(vset.sf.sun_loc_des.mapval(), azzen[0], azzen[1]);
                    Vect sun = Ambient::calcSunVectorFromAzZen(azzen[0]*D2R, azzen[1]*D2R);
                    _par_SF.updateAllTrackVectors(sun);  if(_par_SF.ErrCheck()) return;
                }

                //Check to make sure a field geometry exists
                if( _par_SF.getHeliostats()->size() == 0 )
                {
                    PopMessage("The simulation cannot run without an existing field layout, and none has been created. Please generate a field layout before running this simulation.","Parametric setup error",wxOK|wxICON_WARNING);
                    UserParProgressUpdate(0, 0);
                    StopSimTimer();
                    SetSimulationStatus(false, _in_user_param_simulation, _user_par_button);
                    _user_single_gauge->SetValue(0);
                    _user_single_text->SetLabel("Simulation progress (0/0)");
                    return;
                }


            }

            //run performance simulation
            Hvector helios;
            if(perf_helios_filtered)
            {
                //Map by ID to new hvector
                for(int ih=0; ih<(int)perf_helios.size(); ih++)
                {
                    helios.push_back( (*_par_SF.getHeliostatsByID())[ perf_helios.at(ih)->getId() ] );
                }
            }
            else
            {
                //Get all of the heliostats
                helios = *_par_SF.getHeliostats();
            }
            int n_old_result = _results.size();
            _results.push_back(sim_result());
            //if the simulation is SolTrace, we need to automatically generate new aim points according to the specified method
            if( vset.flux.flux_model.mapval() == var_fluxsim::FLUX_MODEL::SOLTRACE )
            {
                vset.flux.flux_model.combo_select_by_mapval( var_fluxsim::FLUX_MODEL::HERMITE_ANALYTICAL); //Delsol
                if(!sim_cancelled)    sim_cancelled = sim_cancelled || !DoPerformanceSimulation(_par_SF, vset, helios);
                vset.flux.flux_model.combo_select_by_mapval( var_fluxsim::FLUX_MODEL::SOLTRACE); //Delsol
            }
        
            if(!sim_cancelled) sim_cancelled = sim_cancelled || !DoPerformanceSimulation(_par_SF, vset, helios);
            for(int r= n_old_result; r<_results.size(); r++)
                _results.at(r).sim_type = 3;
            int n_new_result = _results.size() - n_old_result;

            //------------------------------------------------------------------
            //export performance data
            if(save_summary && !sim_cancelled)
            {
                wxString fname;
                fname.Printf("%s/userparam_summary_%d.csv",_working_dir.GetPath().c_str(),i+1);
                
                //open and write the file
                wxTextFile fout(fname);
                fout.Clear();
                if (!(fout.Exists() ? fout.Open() : fout.Create()))
                {
                    PopMessage("Error opening the parametric output file \"" + fname + "\". Terminating the simulation");
                    fout.Close();
                    return;
                }

                for (int r = 0; r < n_new_result; r++)
                { 
                    grid_emulator gridtable;
                    interop::CreateResultsTable(_results.at(n_old_result + r), gridtable);

                    //Write the table
                    wxArrayStr textresults;
                    gridtable.GetPrintableTable(textresults, "");
                    
                    if (n_new_result > 1)
                        fout.AddLine(wxString::Format("Performance for:, %s", r == 0 ? "All receivers" : _results.at(n_old_result + r).receiver_names.front()));
                    
                    for(int j=0; j<(int)textresults.size(); j++)
                        fout.AddLine(textresults[j]);
                }
                fout.Write();
                fout.Close();
            }
            //export layout data
            if(save_field && !sim_cancelled)
            {
                wxString fname;
                fname.Printf("%s/userparam_field-detail_%d.csv", save_field_dir.c_str(), i+1);
                wxString header;
                header.Append("Variable name, Variable ID, Value, Units\n");

                for(int j=0; j<(int)simvars.size(); j++)
                {
                    string key = simvars.at(j);
                    if( _variables._varptrs.find( key ) != _variables._varptrs.end() )
                    {
                        wxString tstring;
                        tstring.Printf("%s, %s, %s, %s\n", 
                            _variables._varptrs[ (string)key ]->short_desc.c_str(),
                            key.c_str(),
                            _user_sim_table.at( key ).at( i ).c_str(),
                            _variables._varptrs[ (string)key ]->units.c_str() );
                        header.Append(tstring);
                    }
                }
                header.Append("\n\n");
                LayoutSimulationExport(_par_SF, fname, options, header, delim, true);
            }
            //save field image
            if(save_field_img && !sim_cancelled)
            {
                for (std::vector<int>::iterator pn = _plot_export_selections.begin(); pn != _plot_export_selections.end(); pn++)
                {
                    wxString fname;
                    fname.Printf("%s/userparam_field-plot_%d_%d.png", save_field_dir.c_str(), i+1, *pn);
                    wxClientDC pdc(this);
                    _plot_frame->SetPlotData(_par_SF, *pn );
                    _plot_frame->SolarFieldAnnotation(&_par_SF, &_results.at(n_old_result + 1), _plot_annot_selections);
                    std::string *annot = _plot_frame->getSolarFieldAnnotationObject();
                    if (std::find(_plot_annot_selections.begin(), _plot_annot_selections.end(), PlotSelectDialog::VARIABLES) != _plot_annot_selections.end())
                    {
                        for (int j = 0; j < (int)variable_record.size(); j++)
                        {
                            annot->append(variable_record.at(j));
                        }
                    }

                    _plot_frame->DoPaint(pdc);
                    wxBitmap *bitmap = _plot_frame->GetBitmap();
                    wxImage image = bitmap->ConvertToImage();
                    image.SaveFile( fname, wxBITMAP_TYPE_PNG );
                }

            }
            //save flux image
            if(save_flux_img && !sim_cancelled)
            {
                wxString fname;
                fname.Printf("%s/userparam_flux-plot_%d.png", save_field_dir.c_str(), i+1);
                wxClientDC pdc(this);
                _flux_frame->SetPlotData(_par_SF, *_par_SF.getHeliostats(), 0);
                _flux_frame->DoPaint(pdc);
                wxBitmap *bitmap = _flux_frame->GetBitmap();
                wxImage image = bitmap->ConvertToImage();
                image.SaveFile( fname, wxBITMAP_TYPE_PNG );

            }
            //save flux data
            if(save_flux_dat && !sim_cancelled)
            {
                wxString fname;
                fname.Printf("%s/userparam_flux-data_%d.csv", save_field_dir.c_str(), i+1);
                wxString cdelim = ",";
                _flux_frame->SaveDataTable(fname, cdelim);
            }
            if(_par_SF.CheckCancelStatus() || sim_cancelled) break; //break if the simulation has been cancelled

            //------------------------------------------------------------------
        
            if(_par_SF.CheckCancelStatus() || _sim_control._cancel_simulation) break;
        }
        UserParProgressUpdate(0, 0);
        _user_single_gauge->SetValue(0);
        _user_single_text->SetLabel("Simulation progress (0/0)");

        StopSimTimer();
        SetSimulationStatus(false, _in_user_param_simulation, _user_par_button);
        DoResultsPage();

        //Show the simulation results
        _page_panel->SetActivePage( pageNames.results_summary );
        this->GetSizer()->Layout();
    }
    catch(std::exception &e)
    {
        PopMessage(e.what(), "OnUserParSimulate Error", wxICON_ERROR|wxOK);
        StopSimTimer();
        SetSimulationStatus(false, _in_user_param_simulation, _user_par_button);
    }
    catch(...)
    {
        PopMessage("Caught unspecified error. Simulation is terminating.", "OnUserParSimulate Error", wxICON_ERROR|wxOK);
        StopSimTimer();
        SetSimulationStatus(false, _in_user_param_simulation, _user_par_button);
        return;
    }
}

void SPFrame::OnUserParEditTable( wxGridEvent &WXUNUSED(event))
{
    try
    {
        SyncParValuesTable();
    }
    catch(std::exception &e)
    {
        PopMessage(e.what(), "OnUserParEditTable Error", wxICON_ERROR|wxOK);
    }
    catch(...)
    {
        PopMessage("Caught unspecified error. Simulation is terminating.", "OnUserParEditTable Error", wxICON_ERROR|wxOK);
        return;
    }
}

void SPFrame::SyncParValuesTable()
{
    /* 
    Data is stored in the variable map under 
    _variables["parametric"][0]["user_par_values"]
    
    Data is of the form:
    [var]sample.0.varname,val1,val2,val3,val4[var]sample.0.varname2;val1,val2,val3,val4...

    Also update the local _user_sim_table structure
    */

    int 
        nrow = _user_par_grid->GetNumberRows(),
        ncol = _user_par_grid->GetNumberCols();
    string *uv = &_variables.par.user_par_values.val;

    uv->clear();
    _user_sim_table.ClearAll();

    for(int i=0; i<nrow; i++)
    {
        uv->append("[var]");
        string var;
        ArrayString dats;
        for(int j=0; j<ncol; j++)
        {
            string val = (string)_user_par_grid->GetCellValue(i,j);
            uv->append( val + "," );  
            if(j==0) var = val;
            else dats.push_back(val);
        }
        //Add to the value table
        _user_sim_table[var] = dats;
    }
}

void SPFrame::UpdateParValueTable()
{
    //Update the values displayed in the user parametric table to match the stored data

    string *svals = &_variables.par.user_par_values.val;
    if( svals->find("[var]") == string::npos ) return;

    vector<string> vars = split(*svals, "[var]");

    vector<string> dat = split(vars.at(0), ",");

    //How many lines/cols are there?
    int 
        nrow = (int)vars.size(),
        ncol = (int)dat.size();

    //Make sure the grid is the appropriate size
    _user_par_grid->DeleteRows(0, _user_par_grid->GetNumberRows());
    _user_par_grid->DeleteCols(0, _user_par_grid->GetNumberCols());
    _user_par_grid->InsertCols(0, ncol);
    _user_par_grid->InsertRows(0, nrow);
    _user_sim_ct->SetValue(ncol-1);
    _user_var_ct->SetValue(nrow-1);
    
    for(int i=0; i<(int)vars.size(); i++)
    {
        dat = split(vars.at(i), ",");

        for(int j=0; j<(int)dat.size(); j++)
        {
            _user_par_grid->SetCellValue(i, j, dat.at(j));
        }
    }

    //Update column labels
    _user_par_grid->SetColLabelValue(0, "Variable");
    for(int i=1; i<ncol; i++)
        _user_par_grid->SetColLabelValue(i, wxString::Format("Sim. %d", i) );
    //read only 
    _user_par_grid->SetReadOnly(0, 0, true);
}

void SPFrame::UserParImport()
{
    //Prompt for file
    wxFileDialog *fdlg = new wxFileDialog(this, wxT("Select data table"), wxEmptyString, wxEmptyString, "Text file (*.csv;*.txt)|*.csv;*.txt", wxFD_OPEN);
    if(fdlg->ShowModal() == wxID_OK)
    {
        //Get the file
        wxString fpath = fdlg->GetPath();

        //open the file
        wxTextFile fdat(fpath);
        if(! fdat.Open()) return;

        //Which delimiter is it?
        int nc, nt;
        wxString line = fdat.GetFirstLine();
        nc = line.Find(",");
        nt = line.Find("\t");
        string delim = nc > nt ? "," : "\t";

        //Process all of the lines
        vector<string> line_dat;
        int ilayout=-1;
        vector<vector<string> > all_dat;
        for(line = fdat.GetFirstLine(); !fdat.Eof(); line = fdat.GetNextLine())
        {
            line_dat = split(line.ToStdString(), delim);
            if( line_dat.size() == 0 ) break;
            if( line_dat.front() == "" ) break;
            if(lower_case(line_dat.front()) == "regenerate layout") ilayout = (int)all_dat.size();
            all_dat.push_back(line_dat);
        }

        //Resize the grid and update the ticker values
        int
            nsim = all_dat.at(0).size(),
            nvar = all_dat.size() - (ilayout>0 ? 1 : 0);
        _user_par_grid->DeleteRows(0, _user_par_grid->GetNumberRows());
        _user_par_grid->DeleteCols(0, _user_par_grid->GetNumberCols());
        _user_par_grid->InsertCols(0, nsim);
        _user_par_grid->InsertRows(0, nvar);
        _user_par_grid->SetColLabelValue(0, "Variable");
        for(int i=1; i<nsim+1; i++)
            _user_par_grid->SetColLabelValue(i, wxString::Format("Sim. %d", i) );
        _user_par_grid->SetCellValue(0,0, "Regenerate layout");
        _user_par_grid->SetReadOnly(0,0, true);
        _user_par_grid->SetColSize(0, 120);
        _user_var_ct->SetValue(_user_par_grid->GetNumberRows()-1);
        _user_sim_ct->SetValue(_user_par_grid->GetNumberCols()-1);
        for(int i=0; i<nvar; i++)
        {
            //If the line is for the 'keep layout' option, put it on the first line of the table
            int iuse;
            if(i==ilayout)
                iuse = 0;
            else
                iuse = i;

            for(int j=0; j<nsim; j++)
            {
                _user_par_grid->SetCellValue(iuse, j, all_dat.at(iuse).at(j));
            }
        }
        
        fdat.Close();

        SyncParValuesTable();
    }
}

bool SPFrame::ParCheckSetup(ArrayString &par_vars, var_map &vset)
{
    /* 
    Check the parametric specifications to see whether there might be obvious issues with the case setup.

    Return False if there's a problem, true otherwise
    */

    if( 
            par_vars.Index( "fluxsim.0.flux_solar_az_in" ) != wxNOT_FOUND 
        ||  par_vars.Index( "fluxsim.0.flux_solar_el_in" ) != wxNOT_FOUND
       )
    {
        if( vset.flux.flux_time_type.val != "Sun position" )
        {
            PopMessage("When specifying the solar position for performance simulation, you must enable the"
                       " \"Sun position\" option for the Simulation time specification method on the performance simulation page.");
            return false;
        }
    }

    return true;
}

bool SPFrame::ParCheckSetup(parametric &par_data, var_map &vset)
{
    /* 
    overload
    */

    ArrayString par_vars;
    for(int i=0; i<par_data.size(); i++)
        par_vars.Add(par_data.at(i).varname);

    return ParCheckSetup(par_vars, vset);
}
