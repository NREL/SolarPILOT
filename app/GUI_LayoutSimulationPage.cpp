
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
#include "IOUtil.h"
#include <algorithm>

using namespace std;

void SPFrame::CreateSimulationsLayoutTab( wxScrolledWindow *layout)
{
    //-----------------Layout page----------------------------
    wxBoxSizer *layout_sizer = new wxBoxSizer(wxVERTICAL);
    
    //simulation button
    wxStaticBox *sb0 = new wxStaticBox(layout, wxID_ANY, wxEmptyString);
    wxStaticBoxSizer *sbs0 = new wxStaticBoxSizer(sb0, wxVERTICAL);
    _layout_button = new wxBitmapButton(layout, wxID_ANY, _simimages[0]);
    _layout_button->SetBitmapHover(_simimages[1]);
    _layout_button->SetBitmapSelected(_simimages[2]);
    _layout_button->SetToolTip( wxT("Run") );
    
    wxBoxSizer *sim_sizer = new wxBoxSizer(wxHORIZONTAL);
    sim_sizer->Add(_layout_button, 0, wxALIGN_CENTER_VERTICAL, 0);
    wxStaticText *sim_lab = new wxStaticText(layout, wxID_ANY, wxT("Generate\nNew Layout") );
    sim_lab->SetFont( wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL) );
    sim_sizer->Add(sim_lab, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 10);
    wxStaticLine *lay_sl_0 = new wxStaticLine(layout, wxID_ANY, wxDefaultPosition, wxSize(1,1), wxLI_VERTICAL);
    sim_sizer->Add(lay_sl_0, 0, wxEXPAND|wxALL, 5);
    
    //Regenerate existing layout button
    wxStaticLine *lay_sl_3 = new wxStaticLine(layout, wxID_ANY, wxDefaultPosition, wxSize(1,1), wxLI_VERTICAL);
    wxBitmap bit; bit.LoadFile(_image_dir.GetPath(true) + "refresh.png", wxBITMAP_TYPE_PNG);
    _layout_refresh = new wxBitmapButton(layout, wxID_ANY, bit );
    wxStaticText *ref_label = new wxStaticText(layout, wxID_ANY, wxT("Regenerate\nCurrent Layout"));
    _refresh_warning = new ImageCanvas(layout, wxID_ANY, _image_dir.GetPath(true).ToStdString() + "warning.png", wxBITMAP_TYPE_PNG, wxDefaultPosition, wxSize(32,32));
    _refresh_warning->SetToolTip(wxT("The inputs have changed. Regenerate the geometry to be consistent with the specified inputs."));
    
    sim_sizer->Add(_refresh_warning, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    sim_sizer->Add(_layout_refresh, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    sim_sizer->Add(ref_label, 0, wxLEFT|wxALIGN_CENTER_VERTICAL, 5);
    sim_sizer->Add((1,15),0,0,0);
    sim_sizer->Add(lay_sl_3, 0, wxEXPAND|wxALL, 5);
    sim_sizer->Add((1,15),0,0,0);
    
    
    //Progress bar
    wxBoxSizer *prog_sizer = new wxBoxSizer(wxVERTICAL);
    _layout_gauge = new wxGauge(layout, wxID_ANY, 100, wxDefaultPosition, wxSize(200,25), wxGA_HORIZONTAL|wxGA_SMOOTH );
    _layout_gauge->SetRange(1000);
    _layout_elapsed = new wxStaticText(layout, wxID_ANY, wxT("Elapsed Time: 0:00") ),
    _layout_remain = new wxStaticText(layout, wxID_ANY, wxT("Simulation (0/0)") );
    prog_sizer->Add(_layout_gauge, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
    prog_sizer->Add(_layout_elapsed, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);
    prog_sizer->Add(_layout_remain, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    sim_sizer->Add(prog_sizer);
    wxStaticLine *lay_sl_1 = new wxStaticLine(layout, wxID_ANY, wxDefaultPosition, wxSize(1,1), wxLI_VERTICAL);
    sim_sizer->Add(lay_sl_1, 0, wxEXPAND|wxALL, 5);
    //connect the button
    _layout_button->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnDoLayout ), NULL, this);
    
    //Simulation log
    _layout_log = new wxTextCtrl(layout, wxID_ANY, wxT("Layout simulation log:"), wxDefaultPosition, wxSize(100,45), wxTE_MULTILINE|wxTE_READONLY);
    sim_sizer->Add(_layout_log, 1, wxEXPAND|wxALL, 5); 
    sbs0->Add(sim_sizer, 1, wxEXPAND, 0);
    layout_sizer->Add(sbs0, 0, wxALL|wxEXPAND, 5);
    
    // ** Create the heliostat grid **
    wxStaticBox *sb2 = new wxStaticBox(layout, wxID_ANY, wxT("Field Layout"));
    wxStaticBoxSizer *sbs2 = new wxStaticBoxSizer(sb2, wxVERTICAL);

    wxStaticText *layout_ct_lab = new wxStaticText(layout, wxID_ANY, wxT("Heliostats"));
    _layout_ct = new wxSpinCtrl(layout, wxID_ANY, wxT("1"), wxDefaultPosition, wxSize(85,_spin_ctrl_size.GetHeight()) );
    _layout_ct->SetRange(1,9e7);
    _layout_import = new wxButton(layout, wxID_ANY, wxT("Import"), wxDefaultPosition, _grid_but_size);
    _layout_export = new wxButton(layout, wxID_ANY, wxT("Export"), wxDefaultPosition, _grid_but_size);
    
    _layout_grid = new wxGrid(layout, wxID_ANY, wxDefaultPosition, wxSize(700,400) );
    _layout_grid->CreateGrid(1,1);
    UpdateLayoutGrid();
    _layout_import->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnLayoutImport ), NULL, this);
    _layout_export->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnLayoutExport ), NULL, this);
    _layout_refresh->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnLayoutRefresh ), NULL, this);
    _layout_ct->Connect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxCommandEventHandler( SPFrame::OnLayoutCount ), NULL, this);
    _layout_grid->Connect(wxEVT_GRID_CELL_CHANGED, wxGridEventHandler( SPFrame::OnLayoutEdit ), NULL, this);

    wxBoxSizer *lay_but_sizer = new wxBoxSizer(wxHORIZONTAL);
    lay_but_sizer->Add(layout_ct_lab, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    lay_but_sizer->Add(_layout_ct, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    lay_but_sizer->Add(_layout_import, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    lay_but_sizer->Add(_layout_export, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    sbs2->Add(lay_but_sizer, 0, wxALL, 5);
    sbs2->Add(_layout_grid, 1, wxEXPAND|wxALL, 5);

    layout_sizer->Add(sbs2, 1, wxALL|wxEXPAND, 5);

    //log any input/output controls

    layout->SetSizer(layout_sizer);
    layout->SetScrollbars(10, 10, layout->GetSize().GetWidth()/10, layout->GetSize().GetWidth()/10);
    return;
}

void SPFrame::UpdateLayoutGrid()
{
    /* Fills the contents of the heliostat layout grid to match data in memory */

    //Create all of the columns
    int ncol = 15;
    if(_layout_grid->GetNumberCols() != ncol) {
        _layout_grid->DeleteCols(0, _layout_grid->GetNumberCols());
        _layout_grid->AppendCols(ncol,false);
    }
    string cols[] = {"Template", "Loc. X\n[m]", "Loc. Y\n[m]", "Loc. Z\n[m]","Slant\n[m]","Rad. pos\n[m]","Az. pos\n[deg]",
                     "Focal X\n[m]","Focal Y\n[m]","Cant vect.\n<i>","Cant vect.\n<j>","Cant vect.\n<k>",
                     "Aim pt. X\n[m]","Aim pt. Y\n[m]","Aim pt. Z\n[m]"};
    int dw=58;
    int widths[] = {120,dw,dw,dw,dw,dw,dw,dw,dw,dw,dw,dw,dw,dw,dw,dw};
    
    _layout_grid->SetRowLabelSize(40);
    for(int i=0; i<ncol; i++)
    {
        _layout_grid->SetColLabelValue(i, cols[i]);
        _layout_grid->SetColSize(i, widths[i]);
    }

    //Fill in the data
    Hvector *heliostats = _SF.getHeliostats();
    int npos = heliostats->size();
    Heliostat *H;

    _layout_ct->SetValue( npos );
    GridCount(_layout_ct, _layout_grid);
    
    for(int i=0; i<npos; i++)
    {
        /* Fill in the data for each heliostat in the template */


        H = heliostats->at(i);    //shorthand the pointer
        if(! H->IsInLayout()) continue;

        sp_point *loc = H->getLocation();

        _layout_grid->SetCellValue(i, 0, *H->getMasterTemplate()->getHeliostatName() );
        _layout_grid->SetCellValue(i, 1, to_string(loc->x, "%.1f") );
        _layout_grid->SetCellValue(i, 2, to_string(loc->y, "%.1f") );
        _layout_grid->SetCellValue(i, 3, to_string(loc->z, "%.1f") );
        _layout_grid->SetCellValue(i, 4, to_string(H->getSlantRange(), "%.1f") );
        _layout_grid->SetCellValue(i, 5, to_string(H->getRadialPos(), "%.1f") );
        _layout_grid->SetCellValue(i, 6, to_string( atan2(H->getLocation()->x,H->getLocation()->y)*R2D, "%.2f") );
        _layout_grid->SetCellValue(i, 7, to_string(H->getFocalX(), "%.1f") );
        _layout_grid->SetCellValue(i, 8, to_string(H->getFocalY(), "%.1f") );
        Vect *cant = H->getCantVector();
        _layout_grid->SetCellValue(i, 9, to_string(cant->i, "%.3f"));
        _layout_grid->SetCellValue(i, 10, to_string(cant->j, "%.3f"));
        _layout_grid->SetCellValue(i, 11, to_string(cant->k, "%.3f"));
        sp_point *aim = H->getAimPoint();
        _layout_grid->SetCellValue(i, 12, to_string(aim->x, "%.2f"));
        _layout_grid->SetCellValue(i, 13, to_string(aim->y, "%.2f"));
        _layout_grid->SetCellValue(i, 14, to_string(aim->z, "%.2f"));
            
    }
    

    //update the land bounding area value determined during simulation

    int icols[] = {0, 4,5,6};
        
    for(int i=0; i<4; i++)
    {
        wxGridCellAttr *attr = new wxGridCellAttr();
        attr->SetBackgroundColour(_colorTextBG);
        attr->SetTextColour(_colorTextFG);
        attr->SetReadOnly(true);
        _layout_grid->SetColAttr(icols[i], attr);
    }
    
    return;
    
}

void SPFrame::UpdateLayoutLog(string msg, bool clear)
{
    /* 
    Update the layout log text control. If the flag "clear" is true, the log will reset to the original message and
    then append the string "msg"
    */
    if(clear)
    {
        _layout_log->SetValue( wxT("Layout simulation log:\n\t")+msg );
    }
    else
    {
        wxString ctext = _layout_log->GetValue();
        ctext.append(wxT("\n\t")+msg);
        _layout_log->SetValue( ctext );
    }

}

void SPFrame::SimProgressUpdate(simulation_info* siminfo)
{
    /* 
    This method is called to update the GUI simulation progress display according
    to the status of the simulation info object.

    This is implemented as a callback from the simulation code.
    */

    if(! siminfo->isEnabled() ) return;    //Don't do anything if the simulation object and callback haven't been enabled
    
    int ntot, csim;
    double simprog;
    
    siminfo->getSimulationInfo(csim, ntot, simprog);
    string *notice = siminfo->getSimulationNotices();
    if(*notice != "") notice->insert(0, wxString::Format("%d ms | ", _sim_watch.Time()));
    //Choose which base to update
    if(_in_layout_simulation)
    {
        SimProgressBase(csim, ntot, notice, chooseProgressGauge(), chooseProgressText());
    }
    else
    {    
        wxString msg;
        if (siminfo->getSimulationNotices()->empty())
            msg = "Calculation";
        else
        {
            msg =  *siminfo->getSimulationNotices();
            msg.Replace("\n", " | ");
        }

        FluxProgressBase(csim, ntot, chooseProgressGauge(), chooseProgressText(), msg);
    }
        
    siminfo->clearSimulationNotices();
    
}

void SPFrame::SimProgressUpdateMT(int n_complete, int n_tot)
{
    //Multithreaded version of SimProgressUpdate
    string notices = "";
    SimProgressBase(n_complete, n_tot, &notices, chooseProgressGauge(), chooseProgressText());

}

void SPFrame::SimErrorHandler(simulation_error *simerror)
{
    /* 
    Processes simulation warnings, errors, notices and provides feedback to the user.

    This is a callback.

    The simulation code is bound to this method to call it when an error occurs.
    */
    
    //Add the message to the simulation log
    string *errtext = simerror->getSimulationErrors();
    _sim_log_data.push_back( *errtext );

    //Handle depending on the type of error
    if( simerror->isFatal() )
    {
        //Fatal error
        PopMessage(*errtext, "Fatal error", wxICON_ERROR|wxOK);
        simerror->setTerminateStatus(true);
    }
    else if( simerror->isDisplayNow() )
    {
        wxString msg;
        msg = *errtext;
        msg.append(wxT("\n\nContinue simulation?"));
        int id = PopMessage(msg, wxT("Simulation warning"), wxICON_WARNING|wxYES|wxNO);
        if(id == wxID_NO)
        {
            simerror->setTerminateStatus(true);
        }
        else
        {
            simerror->Reset();
        }
    }
    simerror->clearErrorLog();    //Always clear the messages to avoid double-display
    
}

void SPFrame::StartSimTimer()
{
    _SF.getSimInfoObject()->Reset();
    _sim_timer->Start(50);
    _sim_watch.Start(0);    //Timer to keep track of elapsed time
}

void SPFrame::StopSimTimer()
{
    _sim_timer->Stop();
    _sim_watch.Pause();    //Value will reset on StartSimTimer() call

}

void SPFrame::OnDoLayout( wxCommandEvent &event)
{
    /* 
    The simulate button has been clicked on the layout/optimization page 
    
    This button is a little tricky.. it also serves as the 'cancel simulation' button 
    when a simulation is running. Therefore it can be called in cancel mode while 
    it is also running a field layout. 

    In normal mode:
    1) The code steps through normally, creating and laying out the field, and filling in
    the results of the layout in the layout grid control.

    In cancel mode:
    1) _SF.CancelSimulation() will be called and the method should return without 
    doing anything else. 
    2) The other instance of the method will exit out of FieldLayout() or its current simulation
    location with an error and go to the sim_error_flag. This will clean up, handle error display
    and reset the error object.
    
    */
    
	wxBitmapButton* bb = (wxBitmapButton*)event.GetEventObject();
    try
    {
        //check whether a flux simulation is currently running. Both simulations cannot occur at the same time.
        if(_in_flux_simulation || _in_param_simulation || _in_user_param_simulation || _in_optimize_simulation)
        { 
            //Notify user
            PopMessage("A simulation is currently underway. Please cancel the simulation or wait"
                "for completion before generating a new layout", "Error");
            event.Skip(); return;
        }

        

        //If this method is already running and the button is clicked again, cancel the simulation
        if(! _in_layout_simulation)
        { 
            SetSimulationStatus(true, _in_layout_simulation, bb); 
        }
        else
        { 
            if(_is_mt_simulation)
            {
                //For a multithreaded simulation, cancel all of the threads
                for(int i=0; i<_n_threads_active; i++)
                {
                    _simthread[i].CancelSimulation();
                }
                PopMessage(wxT("Simulation cancelled by user."), wxT("Notice"), wxICON_INFORMATION|wxOK);
            }
            else
            {    
                _SF.CancelSimulation(); 
            }
            return;    //Don't do any clean up here, let the first call do the clean up when it exits
        }

        StartSimTimer();
        int simtime=0, proctime=0;

        try
        {

            //Run the layout simulation method
            _SF.Create(_variables);        
            if(_SF.ErrCheck())
                goto sim_error_flag;

            if(! DoManagedLayout(_SF, _variables) ) 
                goto sim_error_flag;

            simtime = _sim_watch.Time();
            //Process the simulation results
            _results.clear();
            _results.resize(1);
            double azzen[2];
            _SF.CalcDesignPtSunPosition(_variables.sf.sun_loc_des.mapval(), azzen[0], azzen[1]);

            sim_params P;
            P.dni = _variables.sf.dni_des.val;

            _results.at(0).process_analytical_simulation(_SF, P, 0, azzen);

            //Load the results in the grid
            UpdateLayoutGrid();
    
    
            //Redraw the plots
            _plot_frame->SetPlotData( _SF, FIELD_PLOT::EFF_TOT );
            //update the selection combo
            UpdateFieldPlotSelections();
            //update the receiver flux map selection combo
            UpdateFluxPlotSelections();
    
            DoResultsPage();
            proctime = _sim_watch.Time() - simtime;
        }
        catch(int ierr)
        {
            PopMessage(gui_util::error_list(ierr));
        }
    
        //Show the layout results
        _page_panel->SetActivePage( pageNames.results_layout );
        this->GetSizer()->Layout();

        UpdateCalculatedGUIValues();
        SetGeomState(false);
        _inputs_modified = true;    //Any time the layout is run, flag for save on exit

        //clear the flux heliostat list control
        _flux_lc->ClearAll();
        _flcsort.clear();
        UpdateFluxLC(-1);

        _layout_log->AppendText(wxString::Format("\nComplete! \n\tTotal computation time: %d ms\n\tProcessing time: %d ms\n", simtime, proctime));
        goto sim_ok_flag;    //Skip to the last flag to clean up the simulation

sim_error_flag:    //the code will jump here if there was a problem
        _layout_log->AppendText("\nSimulation failed\n");
        _layout_gauge->SetValue(0);
        _SF.getSimErrorObject()->Reset();
sim_ok_flag:
        StopSimTimer();
        _layout_gauge->SetValue(0);
        SetSimulationStatus(false, _in_layout_simulation, bb);
        //set other simulation flags
        _is_mt_simulation = false;    //False by default
        _n_threads_active = _n_threads;
        return;
    }
    catch(std::exception &e)
    {
        StopSimTimer();
        PopMessage(e.what(), "OnDoLayout Error", wxICON_ERROR|wxOK);
        _layout_log->AppendText("\nSimulation failed\n");
        _layout_gauge->SetValue(0);
        _SF.getSimErrorObject()->Reset();
        SetSimulationStatus(false, _in_layout_simulation, bb);
        //set other simulation flags
        _is_mt_simulation = false;    //False by default
        _n_threads_active = _n_threads;
    }
    catch(...)
    {
        StopSimTimer();
        PopMessage("Caught unspecified error. Simulation is terminating.", "OnDoLayout Error", wxICON_ERROR|wxOK);
        _layout_log->AppendText("\nSimulation failed\n");
        _layout_gauge->SetValue(0);
        _SF.getSimErrorObject()->Reset();
        SetSimulationStatus(false, _in_layout_simulation, bb);
        //set other simulation flags
        _is_mt_simulation = false;    //False by default
        _n_threads_active = _n_threads;
    }
}

void SPFrame::OnSimTimerCount( wxTimerEvent &WXUNUSED(event))
{
    /* 
    This event is called from the simulation timer to periodically 
    update the GUI with the elapsed time of the ongoing simulation.
    */

    //get the elapsed time
    int timems = _sim_watch.Time();
    double
        timesecd = timems/1000.;
    int timemin, timehr, timesec;
    wxString elstr;
    timehr = int(floor(timesecd/3600.));
    timemin = int(floor( (timesecd/3600.-timehr)*60. ));
    timesec = int(timems/1000.)%60;
    elstr.sprintf("Elapsed Time: %02d:%02d:%02d", timehr, timemin, timesec);

    //Update the gui
    _layout_elapsed->SetLabel( elstr );
}

void SPFrame::OnLayoutCount( wxCommandEvent &WXUNUSED(event))
{
    /* The number of grid cells has been changed by the user */
    GridCount(_layout_ct, _layout_grid);
    //FormatLayoutGridCells();
}

void SPFrame::OnLayoutImport( wxCommandEvent &WXUNUSED(event))
{
    /* The import button has been clicked for the layout grid */
    try
    {
        //read the contents of a file in to the variable map
        //prompt for a file path
        wxFileDialog filedlg(this, wxT("Select a file for import..."), wxEmptyString, wxEmptyString, wxT("Data files (*.txt;*.dat;*.csv)|*.txt;*.dat;*.csv") );
        filedlg.CentreOnParent();

        //Show and process
        if(filedlg.ShowModal() == wxID_OK)
        {
            wxString info = filedlg.GetPath().c_str();
            //Try opening the file
            if(ioutil::file_exists( info.c_str() ) )
            {

                //read in the contents of the file, check for formatting
                string fdat, eol;
                ioutil::read_file(info.ToStdString(), fdat, eol);
                vector<string> linecheck = split(fdat, eol);
                string delim;
                bool format_error = false;
                if(linecheck.size() < 1)
                {
                    format_error = true;
                }
                else
                {    //Ok with multiple lines
                    //check for the right number of values
                    delim = ioutil::getDelimiter(linecheck.at(min((int)linecheck.size()-1, 5))); //go a few lines down to avoid confusion with the header
                    vector<string> valcheck = split(linecheck.at(0), delim);
                    if(valcheck.size() < 4) format_error = true;
                }
                if(format_error)
                {
                    wxString msg = "The selected file contains a formatting error. Please refer to the help menu for formatting requirements."
                                   "The first line of the file is:\n" + linecheck.at(0);
                    PopMessage(msg, "Format error", wxOK|wxICON_ERROR);
                    return;
                }

                //If there's a header, remove the header lines
                int nhdrlines = 0;
                bool is_hdr = false;
                for(int i=0; i<(int)linecheck.size(); i++)
                {
                    //Remove any newline characters
                    wxString newline = linecheck.at(i);
                    newline.Replace("\n","");
                    newline.Replace("\r","");
                    
                    vector<string> valcheck = split(newline.ToStdString(), delim);
                    is_hdr = false;
                    for(int j=0; j<(int)valcheck.size(); j++)
                    {
                        
                        double x;
                        bool ok = to_double(valcheck.at(j), &x);

                        //allow "NULL" to appear in the data table
                        
                        //If there was an error converting the entry to a value, then assume it's a text entry and part of a header line
                        if(! ok && lower_case(valcheck.at(j)) != "null")
                        {
                            nhdrlines++;
                            is_hdr = true;
                            break;
                        }
                        
                    }
                    //At this point, all entries are numeric, so no header line exist. Break and continue with the method.
                    if(! is_hdr)
                        break;
                }

                //Reconstruct without header lines
                wxString fdat2;
                for(int i=nhdrlines; i<(int)linecheck.size(); i++)
                    fdat2.Append(linecheck.at(i) + "\n");
                                

                //No format problems, set the variable map value
                _variables.sf.layout_method.combo_select_by_mapval( var_solarfield::LAYOUT_METHOD::USERDEFINED );
                _variables.sf.layout_data.set_from_string( fdat2.c_str() );
                _variables.sf.des_sim_detail.combo_select_by_mapval( var_solarfield::DES_SIM_DETAIL::DO_NOT_FILTER_HELIOSTATS ); 
                UpdateInputValues();
            
                UpdateLayoutLog("Initializing solar field...", true);
                _SF.Create(_variables);
                _layout_gauge->SetValue(100);
                _layout_gauge->GetParent()->Refresh();

                UpdateLayoutLog("Generating heliostat geometry...");
                bool simok = _SF.FieldLayout();    
                if(_SF.ErrCheck() || !simok) return;
                //Calculate the corner coordinates of the heliostats
                double azzen[2];
                _SF.CalcDesignPtSunPosition(_variables.sf.sun_loc_des.mapval(), azzen[0], azzen[1]);
                Vect sun = Ambient::calcSunVectorFromAzZen(azzen[0]*D2R, azzen[1]*D2R);
                _SF.updateAllTrackVectors(sun);

                _layout_gauge->SetValue(900);
                UpdateLayoutLog("Updating layout geometry grid...");
                UpdateLayoutGrid();
                _layout_gauge->SetValue(0);
            
                UpdateCalculatedGUIValues();  
                SetGeomState(false);
                _inputs_modified = true;    //prompt for save later
            
                //Create the layout plot
                _plot_frame->SetPlotData( _SF, FIELD_PLOT::LAYOUT );
                //update the selection combo
                _plot_select->SetSelection( FIELD_PLOT::LAYOUT );

            }
            else
            {
                PopMessage("Error - File not found!", "File error", wxOK|wxICON_ERROR);
                return; //error the file wasn't found
            }
        }
    }
    catch(std::exception &e)
    {
        PopMessage(e.what(), "OnLayoutImport Error", wxICON_ERROR|wxOK);
    }
    catch(...)
    {
        PopMessage("An unhandled error has occurred and the action could not be completed.", "OnLayoutImport Error", wxICON_ERROR|wxOK);
        return;
    }
}

void SPFrame::OnLayoutExport( wxCommandEvent &WXUNUSED(event))
{
    /* The export button has been clicked for the layout grid */
    try
    {
        string *var = &_variables.sf.layout_data.val;
        
        //Get the file
        wxFileDialog filedlg(this, wxT("Select a file for export..."), wxEmptyString, wxEmptyString, wxT("Data files (*.txt;*.dat;*.csv)|*.txt;*.dat;*.csv"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT );
        filedlg.CentreOnParent();

        //Show and process
        if(filedlg.ShowModal() == wxID_OK)
        {
            wxString info = filedlg.GetPath().c_str();
        
            FILE *file;
            file = fopen(info.c_str(), "w");
            if( file == 0 )
            {
                PopMessage(wxT("The specified file could not be opened for writing. Please ensure that the file is not open or write-protected."), wxT("Error"), wxICON_ERROR|wxOK);
                return;
            }
            
            //Note -- removed the header since the import function no longer expects this
            
            //write the header
            string labs[] = {"Template", "Loc. X", "Loc. Y", "Loc. Z","X focus","Y focus",
                             "Cant vect.","Cant vect.","Cant vect.", "Aim pt. X","Aim pt. Y","Aim pt. Z"};
            string units[] = {"", "[m]","[m]","[m]","[m]","[m]","<i>","<j>","<k>","[m]","[m]","[m]"};
            for(int i=0; i<12; i++)
            {
                fprintf(file,"%s,",labs[i].c_str());
            }
            fprintf(file,"\n");
            for(int i=0; i<12; i++)
            {
                fprintf(file, "%s,", units[i].c_str());
            }
            fprintf(file, "\n");
            //write the data
            vector<string> lines = split(*var, ";");
            for(unsigned int i=0; i<lines.size(); i++)
            {
                fprintf(file, "%s\n", lines.at(i).c_str());
            }
            fclose(file);

            //notify the user of a successful export
            wxString msg;
            msg.sprintf("%s\nFile successfully created.",info.c_str());
            PopMessage(msg, "Success");
        }

        return;
    }
    catch(std::exception &e)
    {
        PopMessage(e.what(), "OnLayoutExport Error", wxICON_ERROR|wxOK);
    }
    catch(...)
    {
        PopMessage("An unhandled error has occurred and the action could not be completed.", "OnLayoutExport Error", wxICON_ERROR|wxOK);
        return;
    }



}

void SPFrame::OnLayoutEdit( wxGridEvent &WXUNUSED(event))
{
    /* The user has changed the contents of a cell in the heliostat field layout group */

}

void SPFrame::OnLayoutRefresh( wxCommandEvent &WXUNUSED(event))
{
    /* Update the values displayed in the layout grid */
    try
    {
        //For each heliostat, update the focal length, canting vector, and aim point
        _SF.Create(_variables);
        if(! _variables.sf.layout_data.val.empty() )  //If there's no existing layout to refresh, skip
        {
            _SF.PrepareFieldLayout(_SF, 0, true);    if(_SF.ErrCheck()) return;
            
            double azzen[2];
            _SF.CalcDesignPtSunPosition(_variables.sf.sun_loc_des.mapval(), azzen[0], azzen[1]);
            Vect sun = Ambient::calcSunVectorFromAzZen(azzen[0]*D2R, azzen[1]*D2R);
                
            _SF.updateAllTrackVectors(sun);    if(_SF.ErrCheck()) return;
            UpdateLayoutGrid();
            SetGeomState(false);
            _inputs_modified = true;
        }
        //redraw the plots
        this->Refresh();
        this->Update();
    }
    catch(std::exception &e)
    {
        PopMessage(e.what(), "OnLayoutRefresh Error", wxICON_ERROR|wxOK);
    }
    catch(...)
    {
        PopMessage("An unhandled error has occurred and the action could not be completed.", "OnLayoutRefresh Error", wxICON_ERROR|wxOK);
        return;
    }
}
