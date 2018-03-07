#include "GUI_main.h"
#include "AutoPilot_API.h"

using namespace std;

void SPFrame::CreateOptimizationPage(wxScrolledWindow *parent)
{
    try
    {

        //------------- simulation and progress bar
        //Run button
        _optimize_button = new wxBitmapButton(parent, wxID_ANY, _simimages[0]);
        _optimize_button->SetBitmapHover(_simimages[1]);
        _optimize_button->SetBitmapSelected(_simimages[2]);
        _optimize_button->SetToolTip( wxT("Run") );

        _optimize_summary_gauge = new wxGauge(parent, wxID_ANY, 1000, wxDefaultPosition, wxSize(200,25), wxGA_HORIZONTAL|wxGA_SMOOTH|wxGA_PROGRESS );
        _optimize_summary_progress = new wxStaticText(parent, wxID_ANY, "Subproblem progress (-/-)");

        _optimize_elapsed = new wxStaticText(parent, wxID_ANY, wxT("Elapsed Time: 0:00") );
        _optimize_detail_gauge = new wxGauge(parent, wxID_ANY, 1000, wxDefaultPosition, wxSize(200,25), wxGA_HORIZONTAL|wxGA_PROGRESS); 
        _optimize_detail_progress = new wxStaticText(parent, wxID_ANY, "Simulation progress (-/-)");
        _optimize_button->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnDoOptimizationSimulation ), NULL, this);
        
        wxCheckBox *enable_detail = new wxCheckBox(parent, wxID_ANY, "");
        enable_detail->SetValue(true);
        enable_detail->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( SPFrame::OnDisableDetailUpdate ), NULL, this);

        //------------------- Log and controls
        _optimize_log = new wxTextCtrl(parent, wxID_ANY, "", wxDefaultPosition, wxSize(250,400), wxTE_READONLY|wxTE_MULTILINE);
        _optimize_log->SetFont( wxFont(9, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL ) );
        wxButton *clear_log = new wxButton(parent, wxID_ANY, "Clear Log");
        wxButton *save_log = new wxButton(parent, wxID_ANY, "Save Log");
        wxButton *apply_opt = new wxButton(parent, wxID_ANY, "Apply to inputs");

        clear_log->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnClearOptimizationLog ), NULL, this);
        save_log->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnSaveOptimizationLog ), NULL, this);
        apply_opt->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnApplyOptimizationResult ), NULL, this);

        
        //----------- settings
        InputControl
            *max_step = new InputControl(parent, wxID_ANY, _variables.opt.max_step),
            *max_iter = new InputControl(parent, wxID_ANY, _variables.opt.max_iter),
            *converge_tol = new InputControl(parent, wxID_ANY, _variables.opt.converge_tol),
            *max_desc_iter = new InputControl(parent, wxID_ANY, _variables.opt.max_desc_iter),
            *max_gs_iter = new InputControl(parent, wxID_ANY, _variables.opt.max_gs_iter),
            *flux_penalty = new InputControl(parent, wxID_ANY, _variables.opt.flux_penalty),
            *power_penalty = new InputControl(parent, wxID_ANY, _variables.opt.power_penalty),
            *algorithm = new InputControl(parent, wxID_ANY, _variables.opt.algorithm);
        OutputControl
            *gs_refine_ratio = new OutputControl(parent, wxID_ANY, _variables.opt.gs_refine_ratio, "%.3f");

        

        // ------------ variable box
        wxButton *opt_add_var = new wxButton(parent, wxID_ANY, wxT("Add"));
        wxButton *opt_remove_var = new wxButton(parent, wxID_ANY, wxT("Remove"));
        opt_add_var->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnOptimizeAddVar ), NULL, this);
        opt_remove_var->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnOptimizeRemoveVar), NULL, this);
    
        _opt_var_grid = new wxGrid(parent, wxID_ANY, wxDefaultPosition, wxSize(400, 200));
        _opt_var_grid->CreateGrid(0, 4);
        
        _opt_var_grid->SetRowLabelSize(250);
        _opt_var_grid->SetColLabelValue(0, "Variable");
        _opt_var_grid->SetColLabelValue(1, "Lower\nBound");
        _opt_var_grid->SetColLabelValue(2, "Upper\nBound");
        _opt_var_grid->SetColLabelValue(3, "Initial\nStep");

        _opt_var_grid->Connect(wxEVT_GRID_CELL_CHANGED, wxGridEventHandler( SPFrame::OnOptimizeGridChange ), NULL, this);


        wxStaticBox *sb_2 = new wxStaticBox(parent, -1, "Settings");
        wxStaticBoxSizer *sbs_2 = new wxStaticBoxSizer(sb_2, wxVERTICAL);

        sbs_2->Add(algorithm);
        sbs_2->Add(max_step);
        sbs_2->Add(max_iter);
        sbs_2->Add(converge_tol);
        sbs_2->Add(max_desc_iter);
        sbs_2->Add(max_gs_iter);
        sbs_2->Add(gs_refine_ratio);
        sbs_2->Add(flux_penalty);
        sbs_2->Add(power_penalty);

        wxBoxSizer *main_sizer = new wxBoxSizer(wxVERTICAL);
        
        wxBoxSizer *head_sizer = new wxBoxSizer(wxHORIZONTAL);
        head_sizer->Add(_optimize_button, 0, wxALIGN_CENTER_VERTICAL, 0);
        wxStaticText *opt_lab = new wxStaticText(parent, wxID_ANY, wxT("Run Optimization") );
        opt_lab->SetFont( wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL) );
        head_sizer->Add(opt_lab, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 10);
        head_sizer->Add( new wxStaticLine(parent, wxID_ANY, wxDefaultPosition, wxSize(1,1), wxLI_VERTICAL), 0, wxEXPAND|wxALL, 5);

        //Progress bar
        wxBoxSizer *prog_sizer1 = new wxBoxSizer(wxVERTICAL);
        prog_sizer1->Add(_optimize_summary_gauge, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
        prog_sizer1->Add(_optimize_summary_progress, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
        prog_sizer1->Add(_optimize_elapsed, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
        wxBoxSizer *prog_sizer2 = new wxBoxSizer(wxVERTICAL);
        prog_sizer2->Add(_optimize_detail_gauge, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
        prog_sizer2->Add(_optimize_detail_progress, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

        head_sizer->Add(prog_sizer1);
        head_sizer->Add(prog_sizer2);
        head_sizer->Add(enable_detail, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);


        wxBoxSizer *control_sizer = new wxBoxSizer(wxHORIZONTAL);
        control_sizer->Add(sbs_2);

        wxBoxSizer *opt_but_sizer = new wxBoxSizer(wxHORIZONTAL);
        opt_but_sizer->Add(opt_add_var, 0, wxALL, 5);
        opt_but_sizer->Add(opt_remove_var, 0, wxALL, 5);

        wxBoxSizer *opt_col_sizer = new wxBoxSizer(wxVERTICAL);
        opt_col_sizer->Add(opt_but_sizer);
        opt_col_sizer->Add(_opt_var_grid, 1, wxEXPAND|wxALL, 5);
        control_sizer->Add(opt_col_sizer, 1, wxEXPAND, 0);
        
        wxBoxSizer *butsizer = new wxBoxSizer(wxHORIZONTAL);
        butsizer->Add(clear_log, 0, wxALL, 5);
        butsizer->Add(save_log, 0, wxALL, 5);
        butsizer->Add(apply_opt, 0, wxALL, 5);
        
        main_sizer->Add(head_sizer, 0, wxEXPAND, 0);
        main_sizer->Add(control_sizer, 0, wxEXPAND, 0);
        main_sizer->Add(butsizer);
        main_sizer->Add(_optimize_log, 1, wxEXPAND|wxALL,10);

        //Add any input or output controls
        InputControl *inputs[] = {max_step, max_iter, converge_tol, max_desc_iter, max_gs_iter, flux_penalty,
            power_penalty, algorithm, NULL};
        int i=0;
        while(inputs[i] != NULL)
        {
            _input_map[ inputs[i]->getVarObject() ] = inputs[i];
            i++;
        }
        i=0;
        OutputControl *outputs[] = {gs_refine_ratio, /*aspect_display,*/  NULL};
        while(outputs[i] != NULL)
        {
            _output_map[ outputs[i]->getVarObject() ] = outputs[i];
            i++;
        }

        parent->SetSizer(main_sizer);
        parent->SetScrollbars(10, 10, parent->GetSize().GetWidth()/10, parent->GetSize().GetWidth()/10);
    }
    catch(std::exception &ex)
    {
        PopMessage("Caught exception while building the optimization simulation page. " + (string)ex.what());
    }
    catch(...)
    {
        PopMessage("Caught exception while building the optimization simulation page.");
    }
    return;
}




void SPFrame::OnDoOptimizationSimulation( wxCommandEvent &event )
{

    try
    {
        //check whether a layout simulation is currently running. Both simulations cannot occur at the same time.
        if(_in_layout_simulation || 
            _in_param_simulation || 
            _in_user_param_simulation || 
            _in_flux_simulation )
        { 
            //Notify user
            PopMessage("A simulation is currently underway. Please cancel the simulation or wait"
                "for completion before generating a new layout", "Error");
            event.Skip(); 
            return;
        }

        //If a flux simulation is underway, this instance means the user wants to cancel the simulation
        if(_in_optimize_simulation)
        {
            if( _variables.flux.flux_model.mapval() == var_fluxsim::FLUX_MODEL::SOLTRACE )
            {
                //Cancel the SolTrace simulation
                _cancel_simulation = true; 
            }
            else
            {
                //Cancel the analytical simulation
                if(_is_mt_simulation)
                {
                    _SFopt_MT->CancelSimulation();                    
                }
                else
                {
                    _SFopt_S->CancelSimulation();
                }
                _optimize_summary_gauge->SetValue(0);
            }
            PopMessage("Simulation cancelled by user.", "Notice", wxICON_INFORMATION|wxOK);
            return;
        }
        wxYieldIfNeeded();    //prevent GUI from freezing
        
        _optimize_detail_gauge->SetRange(1000);
        _optimize_summary_gauge->SetRange(1000);
        
        
        SetSimulationStatus(true, _in_optimize_simulation, (wxBitmapButton*)event.GetEventObject());
        StartSimTimer();

        //Create a copy of the variables structure
        var_map V(_variables);

        //set up variables
        int nv = _opt_data.size();
        vector<double*> optvars(nv);
        vector<double> upper(nv);
        vector<double> lower(nv);
        vector<double> stepsize(nv);
        vector<string> names(nv);

        for(int i=0; i<_opt_data.size(); i++)
        {
            spvar<double> *varptr = static_cast<spvar<double>*>( V._varptrs.at( _opt_data.at(i).varname ) );
            optvars.at(i) = &varptr->val;
            vector<string> namedat = split(_opt_data.at(i).varname, ".");
            names.at(i) = namedat.back();
            //bounds
            if( _opt_data.at(i).selections.at(0) == "none" )
                lower.at(i) = -HUGE_VAL;
            else
            {
                double lval;
                ::to_double(_opt_data.at(i).selections.at(0), &lval);
                lower.at(i) = lval;
            }
            if( _opt_data.at(i).selections.at(1) == "none" )
                upper.at(i) = HUGE_VAL;
            else
            {
                double uval;
                ::to_double(_opt_data.at(i).selections.at(1), &uval);
                upper.at(i) = uval;
            }
            
            //step size
            double vdef;
            ::to_double(_opt_data.at(i).selections.at(2), &vdef);
            stepsize.at(i) = vdef;
        }

        if(_n_threads > 1)
        {
            _n_threads_active = _n_threads;
            _is_mt_simulation = true;

            _SFopt_MT = new AutoPilot_MT();
            
            _SFopt_MT->SetSummaryCallback( SolarFieldOptimizeSummaryCallback, this);
            _SFopt_MT->SetDetailCallback( SolarFieldOptimizeDetailCallback, this);
            
            //set up the weather data for simulation
            vector<string> wdata;
            for(int i=0; i<_local_wfdat.size(); i++)
                wdata.push_back( _local_wfdat.at(i) );
            _SFopt_MT->GenerateDesignPointSimulations( V, wdata );
            
            //Do the expert setup
            _SFopt_MT->Setup(V, true);
            
            //if the detail gauge has been disabled, flag that here
            if(! _enable_detail_update_gauge)
                _SFopt_MT->SetDetailCallbackStatus(false);

            //run the optimization
            _SFopt_MT->Optimize(V.opt.algorithm.mapval(), optvars, upper, lower, stepsize, &names);


            _n_threads_active = 0;
            _is_mt_simulation = false;

            try
            {
                delete _SFopt_MT;
            }
            catch(...)
            {
            }
            _SFopt_MT = 0;
        }
        else
        {
            _is_mt_simulation = false;

            _SFopt_S = new AutoPilot_S();
            _SFopt_S->SetSummaryCallback( SolarFieldOptimizeSummaryCallback, this);
            _SFopt_S->SetDetailCallback( SolarFieldOptimizeDetailCallback, this);
            

            //set up the weather data for simulation
            vector<string> wdata;
            for(int i=0; i<_local_wfdat.size(); i++)
                wdata.push_back( _local_wfdat.at(i) );
            _SFopt_S->GenerateDesignPointSimulations( V, wdata );
            
            //Do the expert setup
            _SFopt_S->Setup(V, true);
            
            //if the detail gauge has been disabled, flag that here
            if(! _enable_detail_update_gauge)
                _SFopt_S->SetDetailCallbackStatus(false);

            //run the optimization
            _SFopt_S->Optimize(V.opt.algorithm.mapval(), optvars, upper, lower, stepsize, &names);


            try
            {
                delete _SFopt_S;
            }
            catch(...)
            {
            }
            _SFopt_S = 0;
        }
        
        //Create an entry in the local variable structure to store the optimal result
        _best_opt_result.clear();
        for(int i=0; i<optvars.size(); i++)
            _best_opt_result.push_back( *optvars.at(i) );
        
        _optimize_log->AppendText( "\n" + _optimize_elapsed->GetLabel() );

        StopSimTimer();
        SetSimulationStatus(false, _in_optimize_simulation, (wxBitmapButton*)event.GetEventObject());
        _optimize_detail_gauge->SetRange(1000);
        _optimize_summary_gauge->SetRange(1000);
        _optimize_detail_gauge->SetValue(0);
        _optimize_summary_gauge->SetValue(0);
        _optimize_summary_progress->SetLabel("Subproblem progress (-/-)");
        _optimize_detail_progress->SetLabel("Simulation progress (-/-)");
        _optimize_elapsed->SetLabel("Elapsed Time: 0:00");

    }
    catch(std::exception &e)
    {
        SetSimulationStatus(false, _in_optimize_simulation, (wxBitmapButton*)event.GetEventObject());
        PopMessage(e.what(), "OnDoOptimizationSimulation Error", wxICON_ERROR|wxOK);
    }
    catch(...)
    {
        SetSimulationStatus(false, _in_optimize_simulation, (wxBitmapButton*)event.GetEventObject());
        PopMessage("Caught unspecified error. Simulation is terminating.", "OnDoOptimizationSimulation Error", wxICON_ERROR|wxOK);
        return;
    }

}

void SPFrame::OptimizeProgressSummaryUpdate(simulation_info *siminfo)
{
    /* 
    Update the progress of the optimization simulation
    */

    int cursim = siminfo->getCurrentSimulation();
    int totct = siminfo->getTotalSimulationCount();
    totct = totct < 1 ? 1 : totct;
    _optimize_summary_gauge->SetValue( (int)((double)cursim / (double)totct *1000) );
    int elapsed = _sim_watch.Time()/1000;
    int mins = (int)( floor( (double)elapsed/60. ) );
    int secs = (int)(elapsed - mins*60);
    _optimize_summary_progress->SetLabel( wxString::Format("Subproblem progress (%d/%d)", cursim, totct) );
    _optimize_elapsed->SetLabel( wxString::Format("Elapsed Time: %0d:%02d", mins, secs) );
    
    string msg = *siminfo->getSimulationNotices();
    _optimize_log->AppendText( msg + (msg.size() > 0 ? "\n" : "") );
    siminfo->clearSimulationNotices();

    wxYieldIfNeeded();
}

void SPFrame::OptimizeProgressDetailUpdate(simulation_info *siminfo)
{

    int cursim = siminfo->getCurrentSimulation();
    int totct = siminfo->getTotalSimulationCount();
    totct = totct < 1 ? 1 : totct;
    _optimize_detail_gauge->SetValue( (int)((double)cursim / (double)totct *1000) );
    _optimize_detail_progress->SetLabel( wxString::Format("Simulation progress (%d/%d)", cursim, totct) );

    int elapsed = _sim_watch.Time()/1000;
    int mins = (int)( floor( (double)elapsed/60. ) );
    int secs = (int)(elapsed - mins*60);
    _optimize_elapsed->SetLabel( wxString::Format("Elapsed Time: %0d:%02d", mins, secs) );
    
    wxYieldIfNeeded();
}

void SPFrame::OnClearOptimizationLog( wxCommandEvent &WXUNUSED(evt))
{
    _optimize_log->Clear();
}

void SPFrame::OnSaveOptimizationLog( wxCommandEvent &WXUNUSED(evt))
{
    //Export the contents of the optimize log to a text file
    try
    {
        //Prompt for file location
        wxFileDialog *fdlg = new wxFileDialog(this, "Select file destination", wxEmptyString, wxEmptyString, "Text files (*.txt)|*.txt", wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
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
            //Write the contents to the file
            fout.AddLine( _optimize_log->GetValue() );

            fout.Write();
            fout.Close();
        }
    }
    catch(std::exception &e)
    {
        PopMessage(e.what(), "OnSaveOptimizationLog Error", wxICON_ERROR|wxOK);
    }
    catch(...)
    {
        PopMessage("Caught unspecified error. Simulation is terminating.", "OnSaveOptimizationLog Error", wxICON_ERROR|wxOK);
        return;
    }
}

void SPFrame::OnApplyOptimizationResult( wxCommandEvent &WXUNUSED(evt))
{
    try
    {

        //check to see if the optimized value has been created
        if( _best_opt_result.size() > 0 )
        {

            //map all of the optimized values back to the inputs
            for(int i=0; i<_best_opt_result.size(); i++)
            {
                string strval = my_to_string(_best_opt_result.at(i));
                _variables._varptrs.at( _opt_data.at(i).varname )->set_from_string( strval.c_str() );
            }

            _SF.updateAllCalculatedParameters(_variables);
            UpdateInputValues();
            UpdateCalculatedGUIValues();
        }
        else
        {
            //notify the user that no optimal solution has been created
            PopMessage("SolarPILOT can't find an optimized solution. Please make sure you have "
                "run the optimization algorithm before passing results to the input values.");
            return;
        }
        
    }
    catch(std::exception &e)
    {
        PopMessage(e.what(), "OnApplyOptimizationResult Error", wxICON_ERROR|wxOK);
    }
    catch(...)
    {
        PopMessage("Caught unspecified error. Simulation is terminating.", "OnApplyOptimizationResult Error", wxICON_ERROR|wxOK);
        return;
    }

}

void SPFrame::OnDisableDetailUpdate( wxCommandEvent &evt)
{
    _enable_detail_update_gauge = ((wxCheckBox*)(evt.GetEventObject()))->GetValue();
    if(_SFopt_MT != 0)
        _SFopt_MT->SetDetailCallbackStatus(_enable_detail_update_gauge);
    if(_SFopt_S != 0)
        _SFopt_S->SetDetailCallbackStatus(_enable_detail_update_gauge);
    
}

void SPFrame::OnOptimizeAddVar( wxCommandEvent &WXUNUSED(event))
{
    try
    {
        //Call the new parametric variable dialog
        par_variables_dialog *dlg = new par_variables_dialog(this, wxID_ANY, _image_dir.GetPath(true), false, wxT("Add optimization variables"));
        dlg->SetItems(&_variables, SP_DATTYPE::SP_DOUBLE); 
        //Set the current selections
        wxArrayStr checked;
        for(int i=0; i<_opt_data.size(); i++)
            checked.Add( _opt_data[i].varname );
        dlg->SetCheckedNames( checked );
        dlg->SetSize(450,550);
    
        if(dlg->ShowModal() == wxID_OK)
        {
            wxArrayStr vals = dlg->GetCheckedNames();

            int nnew=0;
            for(int i=0; i<(int)vals.size(); i++)
            {
                if(_opt_data.Index( vals[i].ToStdString() ) > -1) continue;    //If the item already exists, dont mess with it.
                _opt_data.addVar( _variables._varptrs.at(vals[i].ToStdString() ) ); 
                nnew++;
            }

            if(nnew>0) 
                OptReloadVariableList();
        }
    }
    catch(std::exception &e)
    {
        PopMessage(e.what(), "OnOptimizeAddVar Error", wxICON_ERROR|wxOK);
    }
    catch(...)
    {
        PopMessage("Caught unspecified error. Simulation is terminating.", "OnOptimizeAddVar Error", wxICON_ERROR|wxOK);
        return;
    }
}

int compare_int(int *a, int *b)
{
  if (*a > *b) return 1;
  else if (*a < *b) return -1;
  else return 0;
}

void SPFrame::OnOptimizeRemoveVar( wxCommandEvent &WXUNUSED(event))
{
    try
    {
        if( _opt_var_grid->GetNumberRows() == 0 )
            return;
        wxArrayInt rows = _opt_var_grid->GetSelectedRows();

        rows.Sort( compare_int );

        int nsel = rows.GetCount();
        
        if( nsel == 0 ) return;

        int irem = nsel-1;

        do
        {

            int idx = rows[irem--];
            if (idx < 0)
                wxMessageBox("No variable selected!");
            else
            {
                _opt_data.remove(idx); 
            }
        }
        while( irem > -1 );

        OptReloadVariableList();
    
    }
    catch(std::exception &e)
    {
        PopMessage(e.what(), "OnOptimizeRemoveVar Error", wxICON_ERROR|wxOK);
    }
    catch(...)
    {
        PopMessage("Caught unspecified error. Simulation is terminating.", "OnOptimizeRemoveVar Error", wxICON_ERROR|wxOK);
        return;
    }
}

void SPFrame::CheckOptGridRange()
{
    for(int row=0; row<_opt_var_grid->GetNumberRows(); row++)
    {
        for(int col=1; col<_opt_var_grid->GetNumberCols(); col++)
        {
            //highlight errors
            double b,v;
            wxColour white("white");
            wxColour red("red");
            spbase *var = _variables._varptrs[ _opt_data.at(row).varname ];
            _opt_var_grid->SetCellBackgroundColour(row, col, white);
            if(_opt_data.at(row).selections.at(col-1) != "none")
            {
                ::to_double(_opt_data.at(row).selections.at(col-1), &b);
                string vs = var->as_string();
                ::to_double(vs, &v);
                if( (col==1 && v <= b) || (col==2 && v >= b) )
                    _opt_var_grid->SetCellBackgroundColour(row, col, red);
            }
        }
    }
}

void SPFrame::OnOptimizeGridChange( wxGridEvent &event)
{
    int row = event.GetRow();
    int col = event.GetCol();
    if( col > 0 )
    {
        string newval = _opt_var_grid->GetCellValue(row, col);
        if(lower_case(newval) != "none" )
        {
            //try to convert it as a double
            double vtest;
            if( ! ::to_double(newval, &vtest) )
            {
                _opt_var_grid->SetCellValue(row, col, _opt_data.at(row).selections.at(col-1));
                PopMessage("Invalid grid entry! Value must either be double precision or \"none\".");
            }
            else
            {
                _opt_data.at(row).selections.at(col-1) = newval;
                CheckOptGridRange();
            }
        }
        else
            _opt_data.at(row).selections.at(col-1) = "none";
    }
    else
    {
        wxString newvar = _opt_var_grid->GetCellValue(row, col);
        unordered_map<string, spbase*>::iterator vfind = _variables._varptrs.find( (string)newvar );
        if( vfind != _variables._varptrs.end() )
        {
            if( _opt_data.Index( (string)newvar ) == -1 ) //not in current table
            {
                _opt_data.remove(row);
                _opt_data.addVar( vfind->second );
                OptReloadVariableList();
            }
        }
        else
        {
            _opt_var_grid->SetCellValue(row, col, _opt_data.at(row).varname);
            PopMessage("The variable name you entered was not found in the variables list");
        }
    }

    return;
}


void SPFrame::OptReloadVariableList()
{
    
    if( _opt_var_grid->GetNumberRows() > 0 )
        _opt_var_grid->DeleteRows(0, _opt_var_grid->GetNumberRows());
    _opt_var_grid->AppendRows(_opt_data.size());
    for(int i=0; i<_opt_data.size(); i++)
    {
        spbase* var = _variables._varptrs.at( _opt_data.at(i).varname );
        _opt_var_grid->SetRowLabelValue(i, var->short_desc);
        _opt_var_grid->SetCellValue(i,0, var->name);
        if( _opt_data.at(i).selections.size() == 3 )
        {
            _opt_var_grid->SetCellValue(i,1, _opt_data.at(i).selections.at(0) );     //lower bound
            _opt_var_grid->SetCellValue(i,2, _opt_data.at(i).selections.at(1) );     //upper bound
            _opt_var_grid->SetCellValue(i,3, _opt_data.at(i).selections.at(2) );     //step size
        }
        else
        {
            double defval;
            string val = var->as_string();
            ::to_double(val, &defval);
            defval *= _variables.opt.max_step.val;

            _opt_var_grid->SetCellValue(i,1,"none");
            _opt_var_grid->SetCellValue(i,2,"none");
            _opt_var_grid->SetCellValue(i, 3, my_to_string(defval) );

            _opt_data.at(i).selections.resize(3);
            _opt_data.at(i).selections.at(0) = "none";
            _opt_data.at(i).selections.at(1) = "none";
            _opt_data.at(i).selections.at(2) = _opt_var_grid->GetCellValue(i, 3);
        }
    }

    CheckOptGridRange();
}
