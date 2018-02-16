#include <fstream>
#include <ostream>
#include <shared/lib_weatherfile.h>
#include "GUI_main.h"

void SPFrame::CreateClimatePage(wxScrolledWindow *parent)
{
    //Create the master heliostat page
#if _USE_WINDOW_BG 
    parent->SetBackgroundColour(_background_colour);
#endif
    //Climate list
    wxStaticBox *sb_0 = new wxStaticBox(parent, wxID_ANY, "Choose Climate/Location");
    wxStaticBoxSizer *sbs_0 = new wxStaticBoxSizer(sb_0, wxVERTICAL);

    wxStaticText *climate_lab = new wxStaticText(parent, wxID_ANY, "Filter locations by name:");
    _climate_search = new wxTextCtrl(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(200,_default_input_size.GetHeight()));
    _climate_ctrl = new wxListCtrl(parent, wxID_ANY, wxDefaultPosition, wxSize(650,150), wxLC_REPORT|wxLC_SINGLE_SEL);
    
    wxBoxSizer *climate_but_sizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton *climate_refresh = new wxButton(parent, wxID_ANY, "Refresh list" );
    wxButton *open_climate_folder = new wxButton(parent, wxID_ANY, "Open folder location" );
    climate_but_sizer->Add(climate_refresh, 0, wxTOP|wxRIGHT, 5);
    climate_but_sizer->Add(open_climate_folder, 0, wxTOP|wxLEFT, 5);

    wxBoxSizer *sz = new wxBoxSizer(wxHORIZONTAL);
    sz->Add(climate_lab, 0, wxTOP|wxRIGHT, 5);
    sz->Add(_climate_search);
    
    sbs_0->Add(sz, 0, wxALL, 5);
    sbs_0->Add(_climate_ctrl);
    sbs_0->Add(climate_but_sizer);

    //Location information
    wxStaticBox *sb_1 = new wxStaticBox(parent, wxID_ANY, "Location Information");
    wxStaticBoxSizer *sbs_1 = new wxStaticBoxSizer(sb_1, wxVERTICAL);
    wxGridSizer *sbs_1_gs = new wxGridSizer(2, 0, 0);
    wxSize osize = wxSize(215,_default_input_size.GetHeight());

    _loc_city = new OutputControl(parent, wxID_ANY, _variables.amb.loc_city);
    _loc_state = new OutputControl(parent, wxID_ANY, _variables.amb.loc_state);
    _loc_timezone = new OutputControl(parent, wxID_ANY, _variables.amb.time_zone);
    _loc_elevation = new OutputControl(parent, wxID_ANY, _variables.amb.elevation);
    _loc_latitude = new OutputControl(parent, wxID_ANY, _variables.amb.latitude);
    _loc_longitude = new OutputControl(parent, wxID_ANY, _variables.amb.longitude);

    //Add each item to the grid sizer
    sbs_1_gs->Add(_loc_city);
    sbs_1_gs->Add(_loc_state);
    sbs_1_gs->Add(_loc_timezone);
    sbs_1_gs->Add(_loc_elevation);
    sbs_1_gs->Add(_loc_latitude);
    sbs_1_gs->Add(_loc_longitude);

    sbs_1->Add(sbs_1_gs, 0, wxALL, 5);
    
    //Controls for the atmospheric and DNI models
    wxStaticBox *sb_2 = new wxStaticBox(parent, wxID_ANY, "Atmospheric conditions");
    wxStaticBoxSizer *sbs_2 = new wxStaticBoxSizer(sb_2, wxVERTICAL);
    InputControl *sun_type = new InputControl(parent, wxID_ANY,  _variables.amb.sun_type);
    //inputs for gaussian and pillbox sun require angular extent
    wxPanel 
        *input_sun_panel1 = new wxPanel(parent),
        *input_sun_panel2 = new wxPanel(parent);
    wxBoxSizer 
        *input_sun_panels1 = new wxBoxSizer(wxVERTICAL),
        *input_sun_panels2 = new wxBoxSizer(wxVERTICAL);
    InputControl 
        *sun_rad_limit = new InputControl(input_sun_panel1, wxID_ANY,  _variables.amb.sun_rad_limit),
        *sun_csr = new InputControl(input_sun_panel2, wxID_ANY,  _variables.amb.sun_csr);
    input_sun_panels1->Add(sun_rad_limit);
    input_sun_panel1->SetSizer(input_sun_panels1);
    sun_type->setPanelObject("Pillbox sun", *input_sun_panel1);
    sun_type->setPanelObject("Gaussian sun", *input_sun_panel1);

    input_sun_panels2->Add(sun_csr);
    input_sun_panel2->SetSizer(input_sun_panels2);
    sun_type->setPanelObject("Buie CSR", *input_sun_panel2);
    
    //grid for user sun
    wxPanel *user_sun_panel = new wxPanel(parent);
    wxBoxSizer *user_sun_sizer_top = new wxBoxSizer(wxHORIZONTAL);
    wxStaticBox *sb_2a = new wxStaticBox(user_sun_panel, wxID_ANY, "User-defined sun shape");
    wxStaticBoxSizer *sbs_2a = new wxStaticBoxSizer(sb_2a, wxVERTICAL);
    wxButton *user_sun_import = new wxButton(user_sun_panel, wxID_ANY, "Import", wxDefaultPosition, _grid_but_size );
    wxButton *user_sun_export = new wxButton(user_sun_panel, wxID_ANY, "Export", wxDefaultPosition, _grid_but_size );
    _user_sun_ct = new wxSpinCtrl(user_sun_panel, wxID_ANY, "5", wxDefaultPosition, _spin_ctrl_size, wxSP_ARROW_KEYS | wxALIGN_RIGHT, 1, 1000);
    wxStaticText *user_sun_ct_lab = new wxStaticText(user_sun_panel, wxID_ANY, "Rows");
    _user_sun_grid = new wxGrid(user_sun_panel, wxID_ANY, wxDefaultPosition, wxSize(300,200));
    _user_sun_grid->CreateGrid(5,2);
    user_sun_sizer_top->Add(_user_sun_ct, 0, wxLEFT|wxRIGHT, 5);
    user_sun_sizer_top->Add(user_sun_ct_lab, 0, wxLEFT|wxTOP, 5);
    user_sun_sizer_top->Add(user_sun_import, 0, wxLEFT, 15);
    user_sun_sizer_top->Add(user_sun_export, 0, wxLEFT, 10);
    sbs_2a->Add(user_sun_sizer_top);
    sbs_2a->Add(_user_sun_grid, 0, wxALL, 5);
    sun_type->setPanelObject("User sun", *user_sun_panel);
    user_sun_panel->SetSizer(sbs_2a);
    sun_type->updateInputDisplay();

    //bindings for user sun
    user_sun_import->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnSunImport ), NULL, this);
    user_sun_export->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnSunExport ), NULL, this);
    _user_sun_ct->Connect(wxEVT_COMMAND_SPINCTRL_UPDATED, wxCommandEventHandler( SPFrame::OnSunCount ), NULL, this);
    _user_sun_grid->Connect(wxEVT_GRID_CELL_CHANGED, wxCommandEventHandler( SPFrame::OnSunEdit ), NULL, this);

    //DNI model
    wxPanel *dni_panel = new wxPanel(parent);
    wxPanel *const_panel = new wxPanel(parent);
    InputControl
        *insol_type = new InputControl(parent, wxID_ANY,  _variables.amb.insol_type),
        *del_h2o = new InputControl(dni_panel, wxID_ANY,  _variables.amb.del_h2o),
        *dpres = new InputControl(dni_panel, wxID_ANY,  _variables.amb.dpres),
        *dni_layout = new InputControl(const_panel, wxID_ANY,  _variables.amb.dni_layout);
    wxBoxSizer *dni_sizer = new wxBoxSizer(wxVERTICAL);
    dni_sizer->Add(del_h2o);
    dni_sizer->Add(dpres);
    dni_panel->SetSizer(dni_sizer);

    insol_type->setPanelObject("Allen model", *dni_panel);
    insol_type->setPanelObject("Moon model", *dni_panel);
    wxBoxSizer *const_sizer = new wxBoxSizer(wxVERTICAL);
    const_sizer->Add(dni_layout);
    const_panel->SetSizer(const_sizer);
    insol_type->setPanelObject("Constant value", *const_panel);
    //Weather file data=-1;Meinel model=0;Hottel model=1;Constant value=2;Allen model=3;Moon model=4
    insol_type->setDisabledSiblings("Moon model", del_h2o);
    insol_type->updateInputDisplay();


    //Atmospheric model
    InputControl
        *atm_model = new InputControl(parent, wxID_ANY,  _variables.amb.atm_model);
    
    _atm_panel = new wxPanel(parent);
    wxBoxSizer *atm_panel_s1 = new wxBoxSizer(wxVERTICAL);
    wxString msg = "Note:\nThe polynomial expresses loss fraction per kilometer\nof distance from the receiver.";
    wxStaticText *_atm_helptext = new wxStaticText(_atm_panel, wxID_ANY, msg);
    _atm_helptext->SetForegroundColour(_helptext_colour);
    wxFlexGridSizer *atm_panel_gs = new wxFlexGridSizer(3,1,5);

    //set the variable map value equal to the first set
    int ncoefs = _variables.amb.atm_coefs.val.ncols();
    int atm_sel = _variables.amb.atm_model.mapval();
    wxString nsuf = "thstndrdththththththththth";
    vector<wxStaticText*> atm_coef_labs(ncoefs);
    vector<wxStaticText*> atm_coef_units(ncoefs);
    _atm_coefs.resize(ncoefs);
    wxString lab, units = "[-]";
    for(int i=0; i<ncoefs; i++)
    {
        lab.sprintf("%d%c%c order coefficient",i,nsuf[i*2],nsuf[i*2+1]);
        if(i>0) units.sprintf("[1/km^%d]",i);
        atm_coef_labs.at(i) = new wxStaticText(_atm_panel, wxID_ANY, lab, wxDefaultPosition, wxSize(190,_default_input_size.GetHeight()), wxTE_RIGHT );
        _atm_coefs.at(i) = new wxTextCtrl(_atm_panel, wxID_ANY, my_to_string(_variables.amb.atm_coefs.val.at(atm_sel,i)), wxDefaultPosition, wxSize(100, _default_input_size.GetHeight()) );
        _atm_coefs.at(i)->Disable();
        atm_coef_units.at(i) = new wxStaticText(_atm_panel, wxID_ANY, units);
        atm_panel_gs->Add(atm_coef_labs.at(i), 0, wxRIGHT, 5);
        atm_panel_gs->Add(_atm_coefs.at(i));
        atm_panel_gs->Add(atm_coef_units.at(i), wxLEFT|wxTOP,5);
    }
    
    OutputControl *atm_atten_est = new OutputControl(_atm_panel, wxID_ANY, _variables.amb.atm_atten_est);

    atm_panel_s1->Add(_atm_helptext, 0, wxALL, 5);
    atm_panel_s1->Add(atm_panel_gs, 0, wxALL, 5);
    atm_panel_s1->Add(atm_atten_est);

    _atm_panel->SetSizer(atm_panel_s1);
    
    atm_model->Connect(wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( SPFrame::OnAtmUpdate ), NULL, this);
    for(int i=0; i<ncoefs; i++)
    {
        //Bind each text ctrl to check on leaving whether we need to update the variable map
        _atm_coefs.at(i)->Connect(wxEVT_KILL_FOCUS, wxFocusEventHandler( SPFrame::OnAttenFocus ), NULL, this);
    }

    //Register inputs
    _input_map[&_variables.amb.sun_type] = sun_type;
    _input_map[&_variables.amb.sun_csr] = sun_csr;
    _input_map[&_variables.amb.sun_rad_limit] = sun_rad_limit;
    _input_map[&_variables.amb.atm_model] = atm_model;
    _input_map[&_variables.amb.insol_type] = insol_type;
    _input_map[&_variables.amb.del_h2o] = del_h2o;
    _input_map[&_variables.amb.dpres] = dpres;
    _input_map[&_variables.amb.dni_layout] = dni_layout;
    _output_map[&_variables.amb.atm_atten_est] = atm_atten_est;

    sbs_2->Add(sun_type);
    sbs_2->Add(input_sun_panel1);
    sbs_2->Add(input_sun_panel2);
    sbs_2->Add(insol_type);
    sbs_2->Add(dni_panel);
    sbs_2->Add(const_panel);
    sbs_2->Add(atm_model);
    sbs_2->Add(_atm_panel);

    wxBoxSizer *bottom_sizer = new wxBoxSizer(wxHORIZONTAL);
    bottom_sizer->Add(sbs_2);
    bottom_sizer->Add(user_sun_panel, 0, wxLEFT, 10);

    wxBoxSizer *main_sizer = new wxBoxSizer(wxVERTICAL);
    main_sizer->Add(sbs_0, 0, wxALL, 5);
    main_sizer->Add(sbs_1, 0, wxALL, 5);
    main_sizer->Add(bottom_sizer, 0, wxALL, 5);

    parent->SetSizer(main_sizer);

    //Bind 
    _climate_ctrl->Connect(wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( SPFrame::OnChangeClimateFile ), NULL, this );
    climate_refresh->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnRefreshClimateList), NULL, this );
    _climate_search->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler( SPFrame::OnClimateSearchText), NULL, this );
    open_climate_folder->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnOpenClimateFolder ), NULL, this );

    //set the climate list
    LoadClimateFiles();
    //Format the user sun table
    UpdateUserSunGrid();

    parent->SetScrollbars(10, 10, parent->GetSize().GetWidth()/10, parent->GetSize().GetWidth()/10);
    return;
}

void SPFrame::UpdateClimateListValues()
{
    
    _climate_ctrl->DeleteAllItems();
    _climate_ctrl->DeleteAllColumns();

    int nwf = _climate_files.size();
    wxListItem col0;
    col0.SetId(0);
    col0.SetText("Location");
    col0.SetWidth( _climate_ctrl->GetSize().GetWidth()-25 );
    _climate_ctrl->InsertColumn(0, col0);

    //Insert all of the weather files
    for(int i=0; i<nwf; i++)
    {
        wxListItem item;
        item.SetId(i);
        item.SetText( _climate_files.at(i).c_str() );
        _climate_ctrl->InsertItem( item );
    }
    
}

void SPFrame::LoadClimateFiles()
{
    /*
    Load climate files into the list control
    */
    
    //Get a list of all files in the directory.
    std::string cpath = _climate_dir.GetPath();
    gui_util::list_files(cpath, _all_climate_files, "csv tm2 tm3 smw epw");

    int nwf = _all_climate_files.size();

    //Keep track of all of the climate files for later sorting
    _climate_files.clear();
    for(int i=0; i<nwf; i++)
    { 
        _climate_files.push_back( _all_climate_files.at(i) );
    }
    
    UpdateClimateListValues();

    //Try setting the selected item according to the weather file in the variable map
    int isave = 0;
    string wf_saved = ioutil::name_only( _variables.amb.weather_file.val );
    for(int i=0; i<nwf; i++)
    {
        if(wf_saved == _climate_files.at(i))
            isave=i; 
    }
    wxString tstr = _climate_dir.GetPath(true) + _climate_files.at(isave);
    UpdateClimateFile( tstr, _variables );
}

void SPFrame::UpdateClimateFile( wxString &file,  var_map &V, bool mapval_only )
{
    
    //Update the weather file display information 
    weatherfile wf;
    if(! wf.open(file.ToStdString()) ) return; //error
    
    weather_header hdr;
    wf.header(&hdr);
    
    _loc_city->setValue(hdr.city);
    _loc_state->setValue( hdr.state);
    _loc_timezone->setValue( hdr.tz );
    _loc_elevation->setValue( hdr.elev );
    _loc_latitude->setValue( hdr.lat );
    _loc_longitude->setValue( hdr.lon );

    //Set the variable map weather file
    V.amb.weather_file.val = file;
    V.amb.latitude.val = (hdr.lat);
    V.amb.longitude.val = (hdr.lon);
    V.amb.elevation.val = (hdr.elev);
    V.amb.time_zone.val = (hdr.tz); 

    //Update the weather data
    string linef = "%d,%d,%d,%.2f,%.1f,%.1f,%.1f";
    wxString line;
    int nrec = (int)wf.nrecords();
    V.amb.wf_data.val.clear(); 
    if(!mapval_only) _local_wfdat.resize(nrec);

    weather_record wrec;
    for (int i=0;i<nrec;i++)
    {
        //int year, month, day, hour;
        wf.read(&wrec);
        
        line.sprintf("%d,%d,%d,%.2f,%.1f,%.1f,%.1f", wrec.day, wrec.hour, wrec.month, wrec.dn, wrec.tdry, wrec.pres/1000., wrec.wspd);
        V.amb.wf_data.val.append(wrec.day, wrec.hour, wrec.month, wrec.dn, wrec.tdry, wrec.pres/1000., wrec.wspd, 1.);
        
        if(!mapval_only) _local_wfdat.at(i) = line;    //Keep track of data broken out for quick GUI reference
    }

    //wf.close();
        
}

void SPFrame::UpdateUserSunGrid()
{
    
    int nlines = _variables.amb.user_sun.val.nrows();

    _user_sun_ct->SetValue(nlines);

    _user_sun_grid->DeleteRows(0, _user_sun_grid->GetNumberRows());
    _user_sun_grid->DeleteCols(0, 2);
    _user_sun_grid->SetDefaultColSize(75,true);
    _user_sun_grid->AppendCols(2);
    _user_sun_grid->SetColLabelValue(0, "Radial Pos.\n[mrad]");
    _user_sun_grid->SetColLabelValue(1, "Intensity");
    _user_sun_grid->AppendRows(nlines,true);
    
    for(int i=0; i<nlines; i++)
    {
        _user_sun_grid->SetCellValue(i,0, my_to_string(_variables.amb.user_sun.val.at(i,0)) ); 
        _user_sun_grid->SetCellValue(i,1, my_to_string(_variables.amb.user_sun.val.at(i,1)) );
    }
}

void SPFrame::OnChangeClimateFile( wxListEvent &event) 
{
    /*
    The selection in the climate files list was changed. 
    */
    //Which item was selected?
    int sel = event.m_itemIndex;
    wxString file = _climate_dir.GetPath(true) + _climate_files.at(sel);
    UpdateClimateFile( file, _variables );

    //Update the design point simulation table since it's values possibly depend on the climate file
    UpdateDesignSelect(_variables.sf.des_sim_detail.mapval(), _variables); 
    
    _inputs_modified = true;
}

void SPFrame::OnRefreshClimateList( wxCommandEvent &WXUNUSED(event))
{
    /* Refresh button is clicked */
    LoadClimateFiles();
}

void SPFrame::OnOpenClimateFolder( wxCommandEvent &WXUNUSED(event))
{
    /* Open the climate folder. Different commands depending on operating system */
    wxPlatformInfo P = wxPlatformInfo::Get();
    switch(P.GetOperatingSystemId())
    {
    case wxOperatingSystemId::wxOS_WINDOWS:
    case wxOperatingSystemId::wxOS_WINDOWS_NT:
        wxExecute("explorer "+_climate_dir.GetPath(), wxEXEC_ASYNC, NULL);    //Windows
        break;
    case wxOperatingSystemId::wxOS_MAC:
        wxExecute("open "+_climate_dir.GetPath(), wxEXEC_ASYNC, NULL);    //Mac
        break;
    case wxOperatingSystemId::wxOS_UNIX_LINUX:
        wxExecute("xdg-open "+_climate_dir.GetPath(), wxEXEC_ASYNC, NULL);
        break;
    }
}

void SPFrame::OnClimateSearchText( wxCommandEvent &event)
{
    /*
    When text is entered in the climate file search box, refresh the list to only
    include those items that contain this text.    
    */

    //Get the text to search for
    wxString filter = ((wxTextCtrl*)event.GetEventObject())->GetValue();
    filter.LowerCase();
    string tfile;
    string::size_type inx;

    _climate_files.clear();
    if(filter == wxEmptyString )
    {
        for(int i=0; i<_all_climate_files.size(); i++)
        {
            _climate_files.push_back( _all_climate_files.at(i) );
        }
    }
    else
    {
        //update the climate file list
        for(int i=0; i<_all_climate_files.size(); i++)
        {
            tfile = lower_case( _all_climate_files.at(i) );
            inx = tfile.find( filter.c_str() );
            if(inx != string::npos) { 
                _climate_files.push_back( _all_climate_files.at(i) ); 
            }
        }
    }
    UpdateClimateListValues();

}

void SPFrame::OnSunCount( wxCommandEvent &event)
{
    GridCount( (wxSpinCtrl*)event.GetEventObject(), _user_sun_grid);
}

void SPFrame::OnSunImport( wxCommandEvent &WXUNUSED(event))
{
    /*
    Import a file containing the sun shape array. The format of the file should be:
    <radial 0><separator><relative intens 0>
    <radial 1><separator><relative intens 1>
    ...
    
    Definitions:
    <radial R>            | Radial position from centroid of sun shape in mrad
    <relative intens>    | Relative intensity of the sun at position R
    <separator>            | Text separator, one of ","  " "  "\t"  ";"

    This algorithm sets the "ambient.0.user_sun" variable in the variable map. The format 
    for this array is:
    x1,y1\nx2,y2\n...
    
    */

    //prompt for a file path
    wxFileDialog filedlg(this, "Select a file for import...", wxEmptyString, wxEmptyString, 
        "Sun shape files (*.sun;*.txt;*.dat;*.csv)|*.sun;*.txt;*.dat;*.csv" );
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
                fnames = (std::string)info,
                eol;
            ioutil::read_file(fnames, files, eol);
            wxString file(files), fname(fnames);
            vector<string> entries = split(file.ToStdString(), eol);
            int nlines = entries.size();

            /*
            Don't allow a large number of data points - causes problems in DELSOL's algorithm for sunshape coefficients. 
            Limit should be < 100 points. Recommend 
            */
            if(nlines > 100)
            {
                PopMessage(
                    "Sunshape profiles are limited to less than 100 data points. \n\n"
                    "A highly-resolved sunshape profile may cause problems with the analytical algorithms. Consider limiting "
                    "the profile to a range of 0-25 mrad (typical DNI measurement device half-angle) or reducing the resolution.",
                    "Sunshape resolution error", wxICON_ERROR|wxOK);
                return;
            }
            
            //determine the delimiter
            vector<string> data;
                        
            //Find the type of delimiter
            vector<string> delims;
            delims.push_back(",");
            delims.push_back(" ");
            delims.push_back("\t");
            delims.push_back(";");
            string delim = "\t";    //initialize
            unsigned int  ns=0;
            for(unsigned int i=0; i<delims.size(); i++)
            {
                data = split(entries.at(0), delims.at(i));
                if(data.size()>ns)
                { delim = delims.at(i); ns = data.size(); }    //pick the delimiter that returns the most entries
                    }
            data.clear();

            wxFormatString fmt = "%f,%f;";
            wxString stemp;
            matrix_t<double> *vval = &_variables.amb.user_sun.val;

            vval->clear();    //Clear
            vval->resize(nlines, 2);

            double fdat[2];

            
            //Process all of the entries
            for(int i=0; i<nlines; i++)
            {
                data = split(entries.at(i), delim);
                
                //Process the data and add it to the saved strings
                for(int j=0; j<2; j++) fdat[j] = 0.; //reset to zeros
                for(unsigned int j=0; j<2; j++)
                {
                    to_double(data.at(j), &fdat[j]);
                }
                
                //Set the points
                vval->at(i,0) = fdat[0];
                vval->at(i,1) = fdat[1];
                
            }
            //Call to the function that sets the user_sun_grid data
            UpdateUserSunGrid();

            //Update the calculated values
            UpdateCalculatedGUIValues();

        }
        else
        {
            //Error: the file was not found
        }
    }


    return;

}

void SPFrame::OnSunExport( wxCommandEvent &WXUNUSED(event))
{
    /*
    Export the contents of the field bounds grid to a file of the user's choosing.
    */
    
    //Get the file
    wxFileDialog filedlg(this, "Select a file for export...", wxEmptyString, wxEmptyString, 
        "Sun shape files (*.sun;*.txt;*.dat;*.csv)|*.sun;*.txt;*.dat;*.csv", wxFD_SAVE|wxFD_OVERWRITE_PROMPT );
    filedlg.CentreOnParent();

    //Show and process
    if(filedlg.ShowModal() == wxID_OK)
    {
        wxString info = filedlg.GetPath().c_str();
        
        matrix_t<double> *vals = &_variables.amb.user_sun.val;
        
        ofstream file;
        file.open((string)info);
        if(! file.is_open() )
        {
            PopMessage("The specified file could not be opened for writing. Please ensure that the file is not open or write-protected.", "Error", wxICON_ERROR|wxOK);
            return;
        }

        for(int j=0; j<(int)vals->nrows(); j++)
        {
            file << vals->at(0) << "<" << vals->at(1) << "\n";
        }
        
        file.close();

        //notify the user of a successful export
        wxString msg;
        msg.sprintf("%s\nFile successfully created.",info.c_str());
        PopMessage(msg, "Success");
    }

    return;
}

void SPFrame::OnSunEdit( wxCommandEvent &WXUNUSED(event))
{
    /* The sun shape grid has been edited */

    string valr, vali;
    int nrows = _user_sun_grid->GetNumberRows();
    _variables.amb.user_sun.val.clear();
    _variables.amb.user_sun.val.resize(nrows,2);

    for(int i=0; i<nrows; i++)
    {
        valr = _user_sun_grid->GetCellValue(i,0);    //radial pos
        vali = _user_sun_grid->GetCellValue(i,1);   //intensity
        if(valr == "" || vali == "") continue;
        
        _variables.amb.user_sun.val.at(i, 0) = std::stod( valr );
        _variables.amb.user_sun.val.at(i, 1) = std::stod( vali );
    }
    
}

void SPFrame::OnAtmUpdate( wxCommandEvent &WXUNUSED(event))
{
    /* 
    Modify the displayed atmosphere stuff based on the selection. The options programmed assume:

    DELSOL3 clear day
    DELSOL3 hazy day
    User-defined
    Sengupta-Wagner
    */

    //int nselect = _atm_combo->GetSelection();
    int nselect = _input_map[&_variables.amb.atm_model]->getSelection();
    int nord = _atm_coefs.size();

    if(nselect < 3)        //DELSOL3 clear day, DELSOL3 hazy day, User-defined
    {
        if(! _atm_panel->IsShown()) _atm_panel->Show();
        //get the defaults for this option
        
        for(int i=0; i<nord; i++)
        {
            _atm_coefs.at(i)->SetValue( my_to_string(_variables.amb.atm_coefs.val.at(nselect,i) ) );
            if(nselect<2) {
                if(_atm_coefs.at(i)->IsEnabled()) _atm_coefs.at(i)->Disable();
            }
            else
            {
                if(! _atm_coefs.at(i)->IsEnabled()) _atm_coefs.at(i)->Enable();
            }
        }
    }
    else if(nselect==3)
    {
        /* Sengupta model. Nothing here yet*/
        if(_atm_panel->IsShown()) _atm_panel->Hide();

    }
    UpdateCalculatedGUIValues();
    _atm_panel->GetParent()->Layout();
}

void SPFrame::OnAttenFocus( wxFocusEvent &WXUNUSED(event))
{
    /* 
    One of the textctrls in the atmospheric attenuation group has lost focus. Update the
    value stored in the variable map and refresh the gui
    */

    _variables.amb.atm_coefs.val.clear();
    int nc = _atm_coefs.size();
    _variables.amb.atm_coefs.val.resize(nc,2);
    for(int i=0; i<nc; i++)
    {
        double val;
        to_double((string)_atm_coefs.at(i)->GetValue(), &val);

        _variables.amb.atm_coefs.val.at(2, i) = val;  //user coefficients are always in row 2
    }
    //Also update the variable map choices for the user coefficients
    UpdateCalculatedGUIValues();
}
