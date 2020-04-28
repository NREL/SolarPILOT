
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


/* 
Conventions:
Solar Zenith: 90=Horizon, 0=Vertical
Azimuth: 0=North, 90=East, +/-180=South, -90=West
    - Avoid 0..360

Installation requirements:
wxWidgets

*/

//All of the required include files

#include <stdio.h>
#include <vector>
#include <ctime>
#include <math.h>
#include <map>
#include <algorithm>
#include <thread>
#include <chrono>
#include <fstream>
#include <sstream>

#include <wx/dir.h>
#include <wx/progdlg.h>
#include <wx/webview.h>

#include <lk/env.h>

#include "LayoutSimulateThread.h"
#include "STSimulateThread.h"
#include "STObject.h"
#include "IOUtil.h"
#include "plot_select_dialog.h"

using namespace std;

#include "GUI_main.h"
#include "AutoPilot_API.h"
#include "scripting.h"


string to_string(wxString &wxs)
{
    string s = (string)wxs; 
    return s;
}

string to_varpath(string &group, int &ID, string &name)
{
    return group + "." + my_to_string(ID) + "." + name;
}

string to_varpath(string &group, string &ID, string &name)
{
    return group + "." + ID + "." + name;
}

string to_varpath(char group[], int &ID, char name[])
{
    string 
        sg = group, 
        sn = name;
    return sg + "." + my_to_string(ID) + "." + sn;
}

string to_varpath(char group[], char ID[], char name[])
{
    string
        sg = group,
        si = ID,
        sn = name;
    return sg + "." + si + "." + sn;
}

string to_varpath(char group[], string &ID, char name[])
{
    string
        sg = group,
        sn = name;
    return sg + "." + ID + "." + sn;
}

class spApp : public wxApp
{
private:
    bool
        _is_ctrl,
        _is_alt,
        _is_shift;
    SPFrame *app_frame;
public:

    bool OnInit()
    {
    
        wxInitAllImageHandlers();
        wxLKScriptWindow::SetFactory( new SolarPILOTScriptWindowFactory );
        //Use the standardpaths object to set up 
    
        wxString install_dir = wxPathOnly( wxStandardPaths::Get().GetExecutablePath() )+"/../";
        wxString userdir = wxPathOnly(wxStandardPaths::Get().GetUserLocalDataDir()) + "/SolarPILOT/";

        /* Splash screen */
        //Add an image handler for PNG's
    #if _WINDOWS
        wxSplashScreen* splash = 0;
    
    #ifndef _DEBUG 
        wxImage::AddHandler(new wxPNGHandler );
        wxBitmap bitmap;
        string spdir = install_dir+"exelib/images/Splash.png";

        if(bitmap.LoadFile(spdir, wxBITMAP_TYPE_PNG))
        {
            splash = new wxSplashScreen(bitmap,
                wxSPLASH_CENTRE_ON_SCREEN|wxSPLASH_TIMEOUT,
                3000, NULL, -1, wxDefaultPosition, wxDefaultSize,
                wxSIMPLE_BORDER);
        }
        wxYieldIfNeeded();
    #endif
    #endif
    
        //Initialize keyboard event states
        _is_ctrl = false;
        _is_alt = false;
        _is_shift = false;

        //Get the screen size to make sure the window fits
        int xw = wxSystemSettings::GetMetric( wxSYS_SCREEN_X );
        int yw = wxSystemSettings::GetMetric( wxSYS_SCREEN_Y );

        int px = min(1300, (int)((double)xw*0.9));
        int py = min(1000, (int)((double)yw*0.9));

    
        app_frame = new SPFrame(0, ID_SPFRAME_WINDOW, wxT("SolarPILOT"), wxPoint((xw-px)/2, (yw-py)/2), wxSize(px,py));
    

        if( this->argv.GetArguments().size()>1)
        {
            app_frame->Open(this->argv.GetArguments()[1], true);
        }

        //Check whether there's a crash file
        ArrayString dirfiles, ffiles;
        string suserdir = (string)userdir;
        gui_util::list_files(suserdir, dirfiles,"spt~");

        for(int i=0; i<(int)dirfiles.size(); i++)
            ffiles.push_back(dirfiles.at(i));

        if((int)dirfiles.size() > 0)
        {
        
            wxYieldIfNeeded();
            wxMessageDialog *dlg = new wxMessageDialog((wxWindow*)NULL, "SolarPILOT has detected that the last session terminated abnormally. Would you like to restore your previous session?", "Recover file", wxYES_NO|wxSTAY_ON_TOP);
            if(dlg->ShowModal() == wxID_YES)
            {
                app_frame->Open(userdir.ToStdString()+ffiles.at(0), true);
                //prompt the user to save somewhere else
                wxMessageDialog *dlg2 = new wxMessageDialog((wxWindow*)NULL, "A temporary copy has been opened. Would you like to save the file now?", "Save file", wxYES_NO);
                if(dlg2->ShowModal() == wxID_YES)
                {
                    app_frame->Save(1);
                }
            }
            ::wxRemoveFile(userdir.ToStdString()+ffiles.at(0));
        }

    #ifdef _WINDOWS
        if(splash != (wxWindow*)NULL) splash->Destroy();
    #endif
    
        app_frame->Show(true);

        return true;
    };

    virtual int OnExit()
    {
        return wxApp::OnExit();
    };

};


IMPLEMENT_APP( spApp );

void SPFrame::TempEscape(wxKeyEvent &evt)
{
    switch (evt.GetKeyCode())
    {
    case WXK_ESCAPE:
        ( (wxFrame*)evt.GetEventObject() )->GetParent()->Close();
    default:
        break;
    }
}


static SPFrame *g_mainWindow = 0;

SPFrame::SPFrame(wxWindow* parent, int id, const wxString& title, const wxPoint& pos, const wxSize& size, long style):
    wxFrame(parent, id, title, pos, size, style)
{
    
    g_mainWindow = this;

    //Set the version tag
	_version_major = 1;
	_version_minor = 3;
	_version_patch = 91;

    _software_version = my_to_string(_version_major) + "." + my_to_string(_version_minor) + "." + my_to_string(_version_patch);
    _contact_info = "solarpilot.support@nrel.gov";

    Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler( SPFrame::OnClose ), NULL, this );

    //Set up directories
    _working_dir.SetPath( ::wxGetCwd() ); 
    _install_dir.SetPath( wxPathOnly( wxStandardPaths::Get().GetExecutablePath()) ); // + "/../";
    _install_dir.RemoveLastDir();

    _image_dir.SetPath( _install_dir.GetPath() ); 
    _image_dir.AppendDir("exelib");
    _image_dir.AppendDir("images");
    _climate_dir.SetPath( _install_dir.GetPath() );
    _climate_dir.AppendDir("climate_files");
    _help_dir.SetPath( _install_dir.GetPath() );
    _help_dir.AppendDir("exelib");
    _help_dir.AppendDir("help");
    _local_dir.SetPath( wxPathOnly(wxStandardPaths::Get().GetUserLocalDataDir()) );
    _local_dir.AppendDir("SolarPILOT");

    if(! wxDir::Exists(_local_dir.GetPath()))
    {
        wxDir::Make(_local_dir.GetPath());
    }

    //Parse the settings file
    _settings_file.SetPath( _local_dir.GetPath() );
    _settings_file.SetName( "settings.dat" );

    //check for file existence. If no local settings file has been created, copy the base file into the new directory
    if(! wxFileExists(_settings_file.GetFullPath() ))
    {
        wxCopyFile(_install_dir.GetPath()+"/exelib/var/settings.dat", _settings_file.GetFullPath());
    }

    gui_util::parseSettingsFile( (std::string)_settings_file.GetFullPath() , _gui_settings);
    
    //Set tool tip delay [ms]
    to_integer(_gui_settings["tt_delay"].value, &_tool_tip_delay);
    //Set the number of CPU's
    to_integer(_gui_settings["ncpu"].value, &_n_threads);
    int nmaxthreads = wxThread::GetCPUCount();
    if(_n_threads > nmaxthreads || _n_threads < 0 ) _n_threads = nmaxthreads;
    //Set the recents file list count
    to_integer(_gui_settings["nrecent_max"].value, &_nrecent_max);

    //Default flag values
    _in_layout_simulation = false;
    _in_flux_simulation = false;
    _in_param_simulation = false;
    _in_user_param_simulation = false;
    _cancel_simulation = false;
    _is_mt_simulation = false;
    _in_optimize_simulation = false;
    _enable_detail_update_gauge = true;

    //Define colours and themes
    _helptext_colour = wxColour(255,120,0);
    _background_colour = wxColour(255,255,255);
    //Create colors
    _colorTextBG = wxColour(220, 220, 240);
    _colorTextFG = wxColour(25, 0, 255);
    _default_input_size = wxSize(375,21);
    _spin_ctrl_size = wxSize(50,25);
    _grid_but_size = wxSize(100,25);
    
    _variables.reset();
#ifdef _DEBUG
	//if in debug mode, make the default field smaller so that processes run more quickly
	_variables.sf.q_des.val = 75.;
	_variables.sf.tht.val = 90.;
	_variables.sf.des_sim_ndays.val = 2;
#endif


    //Create the simulation progress timer and connect it to a timer event
    _sim_timer = new wxTimer(this, wxID_ANY);
    _sim_timer->SetOwner(this);
    Connect(_sim_timer->GetId(), wxEVT_TIMER, wxTimerEventHandler( SPFrame::OnSimTimerCount ));
    
    //create a function pointer to the update call
    _SF.getSimInfoObject()->setCallbackFunction(SolarFieldInfoCallback, this);
    _SF.getSimInfoObject()->isEnabled(false);    //Disable updates during initialization

    //Create a function pointer to the ERROR handler
    _SF.getSimErrorObject()->setCallbackFunction(SolarFieldErrorCallback, this);
    _sim_log_data.clear();
    
    //Initialize the solar field geometry objects
    _SF.Create(_variables);
    
    //initialize pointers
    _SFopt_MT = 0;
    _SFopt_S = 0;
    _STSim = 0;
    _active_script_window = 0;

    //----Create the menu items------
    //recent items submenu
    recentMenu = new wxMenu;
    _recent_file_list.SetPath( _local_dir.GetPath() );
    _recent_file_list.SetName("recfile.dat");
    if(! wxFileExists(_recent_file_list.GetFullPath()))
    {
        wxTextFile fnew(_recent_file_list.GetFullPath());
        fnew.Create();
        fnew.Close();
    }

    Connect(ID_FILE_RECENT, ID_FILE_RECENT+10, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SPFrame::OnFileRecent) );
    UpdateRecents();
    Connect(ID_FILE_RECENT_LOAD, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(SPFrame::OnFileRecentLoad) ); 
    
    // file menu
    menuFile = new wxMenu;
    menuFile->Append(ID_FILE_NEW, wxT("&New Project\tCtrl+N"));
    menuFile->Append(ID_FILE_OPEN, wxT("&Open\tCtrl+O"));
    menuFile->Append(ID_FILE_SCRIPT, wxT("New Scrip&t\tCtrl+T"));
    menuFile->Append(ID_FILE_SCRIPT_OPEN, wxT("Open Scri&pt\tCtrl+Alt+T"));
    menuFile->AppendSubMenu(recentMenu, wxT("Open &Recent..."));
    menuFile->Append(ID_FILE_RECENT_LOAD, wxT("Load &Previous File\tCtrl+Alt+F") );
    menuFile->Append(ID_FILE_SAVE, wxT("&Save\tCtrl+S"));
    menuFile->Append(ID_FILE_SAVEAS, wxT("Save &As\tCtrl+Shift+S"));
    menuFile->AppendSeparator();
    menuFile->Append(ID_FILE_EXIT, wxT("&Exit\tAlt+F4"));

    // tools menu
    menuTools = new wxMenu;
    menuTools->Append(ID_TOOLS_SETTINGS, wxT("&Settings"));

    // help menu
    menuHelp = new wxMenu;
    menuHelp->Append(ID_HELP_ABOUT, wxT("&About\tF9"));
    menuHelp->Append(ID_HELP_CONTENTS, wxT("&Contents\tF1"));
    menuHelp->Append(ID_HELP_LICENSE, wxT("&License"));

    // run menu
    menuRun = new wxMenu;
    menuRun->Append(ID_RUN_LAYOUT, wxT("Field &Layout\tF5"));
    menuRun->Append(ID_RUN_REFRESH, wxT("&Refresh geometry\tAlt+F5"));
    menuRun->Append(ID_RUN_SIMULATE, wxT("&Performance simulation\tF6"));
    menuRun->Append(ID_RUN_PARAMETRIC, wxT("&Parametric simulation\tF7"));
    menuRun->Append(ID_RUN_USERPARAMETRIC, wxT("&User parametric simulation\tAlt+F7"));
    menuRun->Append(ID_RUN_OPTIMIZE, wxT("&Optimization\tF8"));

    //Create the menubar
    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, wxT("&File"));
    menuBar->Append(menuTools, wxT("&Tools"));
    menuBar->Append(menuHelp, wxT("&Help"));
    menuBar->Append(menuRun, wxT("&Run"));
    SetMenuBar(menuBar);

    //Bind the menu bar events
    Connect(ID_FILE_NEW, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SPFrame::OnFileNew ) );
    Connect(ID_FILE_SCRIPT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SPFrame::OnFileScript ) );
    Connect(ID_FILE_SCRIPT_OPEN, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SPFrame::OnFileScriptOpen ) );
    Connect(ID_FILE_OPEN, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SPFrame::OnFileOpen ) );
    Connect(ID_FILE_SAVEAS, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SPFrame::OnFileSaveAs ) );
    Connect(ID_FILE_SAVE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SPFrame::OnFileSave ) );
    Connect(ID_FILE_EXIT, wxEVT_COMMAND_MENU_SELECTED, wxCloseEventHandler( SPFrame::OnClose) ); //OnQuit ) );
    Connect(ID_TOOLS_SETTINGS, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SPFrame::OnToolsSettings ) );
    Connect(ID_HELP_ABOUT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SPFrame::OnHelpAbout ) );
    Connect(ID_HELP_CONTENTS, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SPFrame::OnHelpContents ) );
    Connect(ID_HELP_LICENSE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SPFrame::OnHelpLicense ) );
    Connect(ID_RUN_LAYOUT, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SPFrame::OnMenuRunLayout ) );
    Connect(ID_RUN_REFRESH, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SPFrame::OnMenuRunRefresh ) );
    Connect(ID_RUN_SIMULATE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SPFrame::OnMenuRunSimulation ) );
    Connect(ID_RUN_PARAMETRIC, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SPFrame::OnMenuRunParametric ) );
    Connect(ID_RUN_USERPARAMETRIC, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SPFrame::OnMenuRunUserParametric ) );
    Connect(ID_RUN_OPTIMIZE, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( SPFrame::OnMenuRunOptimize ) );

    //Image list
    wxImage::AddHandler(new wxPNGHandler );
    wxImage::AddHandler(new wxICOHandler );
    const wxSize imageSize(32, 32);
    _imageList = new wxImageList(imageSize.GetWidth(), imageSize.GetHeight());
    std::string impath = _image_dir.GetPath(true);
    _imageList->Add( wxIcon((string)(impath+"Main.ico"), wxBITMAP_TYPE_ICO) );
    _imageList->Add( wxIcon((string)(impath+"Climate.ico"), wxBITMAP_TYPE_ICO) );
    _imageList->Add( wxIcon((string)(impath+"Financing.ico"), wxBITMAP_TYPE_ICO) );
    _imageList->Add( wxIcon((string)(impath+"Costs.ico"), wxBITMAP_TYPE_ICO) );
    _imageList->Add( wxIcon((string)(impath+"configure-2.ico"), wxBITMAP_TYPE_ICO) );
    _imageList->Add( wxIcon((string)(impath+"Layout.ico"), wxBITMAP_TYPE_ICO) );
    _imageList->Add( wxIcon((string)(impath+"calculator-3.ico"), wxBITMAP_TYPE_ICO) );
    _imageList->Add( wxIcon((string)(impath+"HeliostatFolder.ico"), wxBITMAP_TYPE_ICO) );
    _imageList->Add( wxIcon((string)(impath+"Heliostat-32x32.ico"), wxBITMAP_TYPE_ICO) );
    _imageList->Add( wxIcon((string)(impath+"ReceiverFolder.ico"), wxBITMAP_TYPE_ICO) );
    _imageList->Add( wxIcon((string)(impath+"Receiver.ico"), wxBITMAP_TYPE_ICO) );
    _imageList->Add( wxIcon((string)(impath+"Results.png"), wxBITMAP_TYPE_PNG) );
    _imageList->Add( wxIcon((string)(impath+"soltrace.ico"), wxBITMAP_TYPE_ICO) );
    _imageList->Add( wxIcon((string)(impath+"flux.ico"), wxBITMAP_TYPE_ICO) );
    _imageList->Add( wxIcon((string)(impath+"table.ico"), wxBITMAP_TYPE_ICO) );
    _imageList->Add( wxIcon((string)(impath+"system-run-5.ico"), wxBITMAP_TYPE_ICO) );
    _imageList->Add( wxIcon((string)(impath+"next_normal.png"), wxBITMAP_TYPE_PNG) );
    _imageList->Add( wxIcon((string)(impath+"next_normal32.png"), wxBITMAP_TYPE_PNG) );
    _imageList->Add( wxIcon((string)(impath+"MaximumParaboloid.ico"), wxBITMAP_TYPE_ICO) );
    _imageList->Add(wxArtProvider::GetIcon(wxART_INFORMATION, wxART_OTHER, imageSize));
    _imageList->Add(wxArtProvider::GetIcon(wxART_QUESTION, wxART_OTHER, imageSize));
    _imageList->Add(wxArtProvider::GetIcon(wxART_WARNING, wxART_OTHER, imageSize));
    _imageList->Add(wxArtProvider::GetIcon(wxART_ERROR, wxART_OTHER, imageSize));
    
    //Simulate button images
    _simimages[0].LoadFile(impath+"next_normal.png", wxBITMAP_TYPE_PNG);
    _simimages[1].LoadFile(impath+"next_hover.png", wxBITMAP_TYPE_PNG);
    _simimages[2].LoadFile(impath+"next_click.png", wxBITMAP_TYPE_PNG);
    _simimages[3].LoadFile(impath+"Cancel.png", wxBITMAP_TYPE_PNG);
    
    //display license on first run
    wxTextFile runinit(_local_dir.GetPath(true) + "runinit");
    bool showlic = false;
    if(runinit.Exists() )
    {
        runinit.Open();
        showlic = runinit.GetFirstLine().Find(_software_version) == wxString::npos;
    }
    else
    {
        showlic = true;
        runinit.Create();
    }
    runinit.Clear();
    runinit.AddLine(_software_version);
    runinit.Close();
    if(showlic)
    {
        wxIcon icopy = _imageList->GetIcon(ID_ICON_MAIN);
        gui_license *pop = new gui_license(NULL, wxID_ANY, icopy );
        pop->Show();
        runinit.Clear();
        runinit.AddLine(_software_version);
        runinit.Write();
    }
    

    //Declare the page names
    pageNames.climate = "Climate";
    pageNames.markets = "Markets";
    pageNames.costs = "Costs";
    pageNames.layout_control = "Layout Setup";
    pageNames.heliostat_master = "Heliostats";
    pageNames.receiver_master =  "Receivers";
    pageNames.simulations =         "Simulations";
    pageNames.simulations_layout = "Field Layout";
    pageNames.simulations_performance = "Performance Simulation";
    pageNames.simulations_parametrics = "Parametrics";
    pageNames.simulations_optimization = "Optimization";
    pageNames.results =             "Results";
    pageNames.results_layout = "Layout Results";
    pageNames.results_flux = "Receiver Flux Profile";
    pageNames.results_summary = "System Summary";
    
    
    //--------------------------------------------------
    _main_sizer = new wxBoxSizer(wxHORIZONTAL);

    _page_panel = new PagePanel(this);
    _page_panel->SetImageList( _imageList );
    _page_panel->SetMinSize(wxSize(250, 200));    
    
    _main_sizer->Add(_page_panel, 0, wxEXPAND, 0 );
        
    CreateInputPages(this, _page_panel);
    
    _page_panel->SetActivePage(0);

    for(int i=0; i<_page_panel->GetPageCount(); i++)
    {
        //don't add the receiver or heliostat templates. They're handled separately
        wxString owner = _page_panel->GetPageGroupOwner(i);
        if( owner == pageNames.receiver_master ||
            owner == pageNames.heliostat_master /*||
            (_page_panel->GetPageName(i) != pageNames.results_layout
            && owner == pageNames.results)*/
            )
            continue;

        //Add other pages
        wxWindow *page = _page_panel->GetPage(i);
        _main_sizer->Add(page, 1, wxEXPAND, 0);
    }

    //Layout of main frame
    this->SetSizer(_main_sizer);
    this->Layout();
     
    int dwidth,dheight;
    _page_panel->GetPageDrawingSize(dheight, dwidth);

    _page_panel->SetScrollbars(10, 10, dwidth/10, dheight/10);
    
    //Call property definitions and object layout
    SetCaseName();
    SetIcon( _imageList->GetIcon( ID_ICON_MAIN ) );
    
    UpdateCalculatedGUIValues();
    _inputs_modified = false;    //At this point, no changes have been made in the GUI
    SetGeomState(false);    //geometry is consistent

    _SF.getSimInfoObject()->isEnabled(true);    //Updates are now possible for the GUI progress stuff

#if _SANDBOX == 1
    if(! Sandbox() ) return;
#endif

}

SPFrame &SPFrame::Instance()
{
    if ( !g_mainWindow )
    {
        wxMessageBox("internal error - no mainwindow instance defined" );
        ::exit( -1 );
    }

    return *g_mainWindow;
}

bool SPFrame::Destroyed()
{
    return !g_mainWindow;
}

SPFrame::~SPFrame()
{
    g_mainWindow = 0;
}

bool SPFrame::CreateRestorePoint()
{
    //save a backup of the current session
    Save(2);
    return true;
}

void SPFrame::PreSimulation()
{
    CreateRestorePoint();
}

void SPFrame::PostSimulation()
{
    ::wxRemoveFile(_backup_file.GetFullPath());
}

void SPFrame::CreateInputPages(wxWindow *parent, PagePanel *pagepanel)
{
    /* 
    This method creates all of the pages and subpages for the main notebook based on the 
    contents of the _variables array. If this is called within the program, the previous 
    instance of the _nb_main object should be destroyed prior to calling this method.

    Returns a pointer to the main notebook (wxTreebook)
    */
    
    //clear data helper arrays
    _rec_pages.clear();
    _hll_coefs.clear(); 
    _hlw_coefs.clear();
    _input_map.clear();
    _output_map.clear();
    //----
    wxYieldIfNeeded();

    //Climate page
    wxScrolledWindow *climate_page = new wxScrolledWindow(parent);
    CreateClimatePage(climate_page);
    pagepanel->AddPage( climate_page , pageNames.climate, ID_ICON_CLIMATE) ;
    //Finance page
    wxScrolledWindow *market_page = new wxScrolledWindow(parent); 
    CreateMarketPage(market_page);
    pagepanel->AddPage( market_page, pageNames.markets, ID_ICON_FINANCE );
    //Costs page
    wxScrolledWindow *costs_page = new wxScrolledWindow(parent);
    CreateCostPage( costs_page );
    pagepanel->AddPage( costs_page, pageNames.costs, ID_ICON_COST );
    //Solar field layout page
    wxScrolledWindow *layout_page = new wxScrolledWindow(parent);
    CreateLayoutPage( layout_page );
    pagepanel->AddPage( layout_page, pageNames.layout_control, ID_ICON_LAYOUTCONFIG );
    //Master heliostat page
    wxScrolledWindow *m_heliostat_page = new wxScrolledWindow(parent);
    CreateMasterHeliostatPage( m_heliostat_page );
    pagepanel->AddPage( m_heliostat_page, pageNames.heliostat_master, ID_ICON_HELIOSTAT_FOLDER );
    //Create heliostat template pages
    UpdateHelioUITemplates();
    //Master receiver page
    wxScrolledWindow *m_receiver_page = new wxScrolledWindow(parent);
    CreateMasterReceiverPage( m_receiver_page );
    pagepanel->AddPage( m_receiver_page, pageNames.receiver_master, ID_ICON_RECEIVER_FOLDER );
    UpdateReceiverUITemplates();

    //Simulations page
    wxScrolledWindow *sim_page = new wxScrolledWindow(parent);
    CreateSimulationsPage( sim_page );
    pagepanel->AddPage( sim_page, pageNames.simulations, ID_ICON_SIMULATIONS32);
    //-----Simulations subpages-----
    //layout tab
    wxScrolledWindow *layout = new wxScrolledWindow(parent);
    CreateSimulationsLayoutTab( layout );
    pagepanel->AddPage(layout, pageNames.simulations_layout, ID_ICON_LAYOUTCALC, 1);
    //Flux page
    wxScrolledWindow *flux = new wxScrolledWindow(parent);
    CreateSimulationsFluxTab( flux );
    pagepanel->AddPage(flux, pageNames.simulations_performance, ID_ICON_SOLTRACE, 1);
    //Parametrics page
    wxScrolledWindow *parametrics = new wxScrolledWindow(parent);
    CreateSimulationsParametricsTab( parametrics );
    pagepanel->AddPage(parametrics, pageNames.simulations_parametrics, ID_ICON_GEARS, 1);
    //Optimization page
    wxScrolledWindow *optimization = new wxScrolledWindow(parent);
    CreateOptimizationPage( optimization );
    pagepanel->AddPage(optimization, pageNames.simulations_optimization, ID_ICON_OPTIMIZE, 1);

    //Results page - blank
    wxPanel *results = new wxPanel(parent);
    pagepanel->AddPage( results, pageNames.results, ID_ICON_RESULTS);


    wxArrayStr choices;
    choices.Add("Land boundaries");
    
    wxScrolledWindow *layoutresults = new wxScrolledWindow(this);
    CreateFieldPlotPage(layoutresults, choices, 0);
    pagepanel->AddPage( layoutresults, pageNames.results_layout, ID_ICON_LAYOUT, 1, pageNames.results);

    //Display a blank flux page
    wxScrolledWindow *fluxresults = new wxScrolledWindow(this);
    CreateFluxPlotPage(fluxresults);
    _page_panel->InsertPage( _page_panel->GetPagePosition( pageNames.results_layout )+1, fluxresults, pageNames.results_flux, ID_ICON_FLUX, 1, pageNames.results );

    //Display a black results page
    this->_results.clear();
    wxScrolledWindow *page = new wxScrolledWindow(this);
    CreateResultsSummaryPage(page);
    _page_panel->InsertPage( _page_panel->GetPagePosition( pageNames.results_flux )+1, page, pageNames.results_summary, ID_ICON_TABLE, 1, pageNames.results );

    //Bind all of the input objects to handle changes in values
    bindControls();
}


/* 
--------------------------------------------------------------------------------------

                                Menu bar methods

--------------------------------------------------------------------------------------
*/



//--------------------- scripting --------------------------------------

void SPFrame::OnFileOpen( wxCommandEvent &WXUNUSED(event))
{
    try
    {
        Open();
    }
    catch(std::exception &e)
    {
        PopMessage(e.what(), "OnFileOpen Error", wxICON_ERROR|wxOK);
    }
    catch(...)
    {
        PopMessage("An unhandled error has occurred and the action could not be completed.", "OnFileOpen Error", wxICON_ERROR|wxOK);
        return;
    }    
}

void SPFrame::OnFileRecent( wxCommandEvent &event)
{
    try
    {
        wxString fname = recentMenu->GetLabelText( event.GetId() );
        wxString frep=fname;
        frep.Replace(wxT("\r"), wxT(""));
        Open(frep);
    }
    catch(std::exception &e)
    {
        PopMessage(e.what(), "OnFileRecent Error", wxICON_ERROR|wxOK);
    }
    catch(...)
    {
        PopMessage("An unhandled error has occurred and the action could not be completed.", "OnFileRecent Error", wxICON_ERROR|wxOK);
        return;
    }    
}

void SPFrame::OnFileScript( wxCommandEvent &WXUNUSED(event))
{
    try
    {
        SolarPILOTScriptWindow::CreateNewWindow();
    }
    catch(std::exception &e)
    {
        PopMessage(e.what(), "OnFileScript Error", wxICON_ERROR|wxOK);
    }
    catch(...)
    {
        PopMessage("An unhandled error has occurred and the action could not be completed.", "OnFileScript Error", wxICON_ERROR|wxOK);
        return;
    }    
}

void SPFrame::OnFileScriptOpen( wxCommandEvent &WXUNUSED(event))
{
    try
    {
        SolarPILOTScriptWindow::OpenFiles();
    }
    catch(std::exception &e)
    {
        PopMessage(e.what(), "OnFileScriptOpen Error", wxICON_ERROR|wxOK);
    }
    catch(...)
    {
        PopMessage("An unhandled error has occurred and the action could not be completed.", "OnFileScriptOpen Error", wxICON_ERROR|wxOK);
        return;
    }    
}

void SPFrame::OnFileRecentLoad( wxCommandEvent &WXUNUSED(evt) )
{
    try
    {
        wxString fname = recentMenu->GetLabelText( ID_FILE_RECENT );    //the most recent file on the list
        wxString frep=fname;
        frep.Replace(wxT("\r"), wxT(""));
        if(wxFileExists( frep ))
            Open(frep);
        else
            PopMessage("The requested file (" + frep + ") was not found.");
    }
    catch(std::exception &e)
    {
        PopMessage(e.what(), "OnFileRecentLoad Error", wxICON_ERROR|wxOK);
    }
    catch(...)
    {
        PopMessage("An unhandled error has occurred and the action could not be completed.", "OnFileRecentLoad Error", wxICON_ERROR|wxOK);
        return;
    }    
}

void SPFrame::OnFileSave( wxCommandEvent &WXUNUSED(event))
{
    try
    {
        /* Handle the save event from the menu*/
        Save(0);
        AddRecentFile( _open_file );
    }
    catch(std::exception &e)
    {
        PopMessage(e.what(), "OnFileSave Error", wxICON_ERROR|wxOK);
    }
    catch(...)
    {
        PopMessage("An unhandled error has occurred and the action could not be completed.", "OnFileSave Error", wxICON_ERROR|wxOK);
        return;
    }
}

void SPFrame::OnFileSaveAs( wxCommandEvent &WXUNUSED(event))
{
    try
    {
        /* Save the current file under a new name */
        Save(1);
        AddRecentFile( _open_file );
    }
    catch(std::exception &e)
    {
        PopMessage(e.what(), "OnFileSaveAs Error", wxICON_ERROR|wxOK);
    }
    catch(...)
    {
        PopMessage("An unhandled error has occurred and the action could not be completed.", "OnFileSaveAs Error", wxICON_ERROR|wxOK);
        return;
    }    
}

void SPFrame::OnFileNew( wxCommandEvent &WXUNUSED(event))
{
    try
    {
        /* Load the default values */
        NewFile();    
    }
    catch(std::exception &e)
    {
        PopMessage(e.what(), "OnFileNew Error", wxICON_ERROR|wxOK);
    }
    catch(...)
    {
        PopMessage("An unhandled error has occurred and the action could not be completed.", "OnFileNew Error", wxICON_ERROR|wxOK);
        return;
    }
}

wxSize SPFrame::getDefaultInputSize()
{
	return _default_input_size;
};

bool SPFrame::CloseProject()
{    
    if(! ModCheckPrompt() ) 
        return false;    //prompt to save. if the user cancels, return.

    _SF.Clean();

    _inputs_modified = false;

    return true;
}

void SPFrame::OnClose( wxCloseEvent &evt )
{
    Raise();
    if ( !CloseProject() )
    {
        evt.Veto();
        return;
    }
    
    //workaround for linux
    _page_panel->SetActivePage( pageNames.simulations_layout );

    // destroy the window
    wxGetApp().ScheduleForDestruction( this );
}

void SPFrame::Save(int save_config)
{
    /* 
    Save method ... Called from menu or from CTRL+S 
    
    "saveconfig" options:
    0 = Save current file using existing name (save) 
    1 = Save current file using new name (save as)
    2 = Save open session backup file (~filename.spt)
    */

    wxFileName file_use;
    bool is_prompt = true;
    if(save_config == 0)
    {    //Use the currently opened file.
        //is there a current file open?
        if(_open_file.Exists())
        {
            file_use = _open_file;
            is_prompt = false;
        }
    }
    else if(save_config == 2)
    {
        is_prompt = false;
        if(_open_file.Exists())
        {
            file_use = _local_dir;
            file_use.SetName( _open_file.GetName()+"~" );
        }
        else
        {
            file_use = _local_dir;
            file_use.SetName( "unsaved-case.spt~");
        }
        _backup_file = file_use;
    }

    
    if(is_prompt)
    {
        //Get the file name to save as
        wxFileDialog filedlg(this, wxT("Save a SolarPILOT file..."), wxEmptyString, wxEmptyString, wxT("SolarPILOT files (*.spt)|*.spt"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT );
        filedlg.CentreOnParent();

        //Show and process
        if(filedlg.ShowModal() == wxID_OK)
        {
            file_use = filedlg.GetPath().c_str();
        }
		else
		{
			return;	
		}
        
    }
    if(! ioutil::saveXMLInputFile((std::string)file_use.GetFullPath(), _variables, _par_data, _opt_data, _software_version) )
    {
        PopMessage(wxT("Error opening the file. Please ensure the file is not already open or write-protected."), wxT("Error"), wxICON_ERROR|wxOK);
        return;
    }
    


    //Update the name of the currently opened file
    if(save_config != 2)
    {
        _open_file = file_use;
        _working_dir = _open_file.GetPath();
        SetCaseName( file_use.GetFullPath() );
        //Variables are now consistent with saved file
        _inputs_modified = false;
    }

    
}

void SPFrame::Open(wxString file_in, bool quiet)
{
    /* 
    Open a file. 

    If the argument passed for "file" is equal to wxEmptyString, then prompt the user to open a particular file
    */

    quiet = true;
    //Check for modifications
    if(! ModCheckPrompt()) return;
    wxFileName file;
    if(file_in == wxEmptyString)
    {

        wxFileDialog filedlg(this, wxT("Open a SolarPILOT file..."), wxEmptyString, wxEmptyString, wxT("SolarPILOT files (*.spt)|*.spt"), wxFD_OPEN );
        filedlg.CentreOnParent();

        //Show and process
        if(filedlg.ShowModal() == wxID_OK)
        {
            file.Assign( filedlg.GetPath() );
            //Try opening the file
            if(! file.FileExists() )
            {
                PopMessage("Error opening the file. File does not exist!", "File error", wxICON_ERROR|wxOK);
                return;
            }
            
        }
        else
        {
            return;
        }
    }
    else
    {
        file = file_in;
    }
    _working_dir.SetPath( file.GetPath() );

    //Check whether this is an XML file or the older version
    FILE *ftemp;
    ftemp = fopen(file.GetFullPath().c_str(), "r");
    string firstline;
    bool isxml = false;
    ioutil::read_line(ftemp, firstline);
    isxml = firstline.find("<data>") != string::npos;
    fclose(ftemp);

    //Parse the input file
    if(isxml)
    {
        ioutil::parseXMLInputFile(file.GetFullPath().ToStdString(), _variables, _par_data, _opt_data);
    }
    else
    {
        //provided for backward compatibility
        throw spexception("The old solarpilot (non-xml) format input file is no longer supported. Could not open file.");
    }
    SetCaseName( file.GetFullPath() );    //Update the displayed case name 

    //is there data for a solar field layout?
    
    //Create a progress dialog
    wxProgressDialog *pdlg=0; //initialize null
    if(!quiet) pdlg = new wxProgressDialog(wxT("File import progress..."), wxEmptyString, 1000, this, wxPD_SMOOTH|wxPD_AUTO_HIDE);
    if(!quiet) pdlg->Show();
    wxYieldIfNeeded();
    //Initialize the solar field geometry
    if(!quiet) pdlg->Update(100, "Initializing solar field");
    _SF.Create(_variables);
    if(!quiet) _layout_gauge->SetValue(100);
    if(!quiet) _layout_gauge->GetParent()->Refresh();

    if(!_variables.sf.layout_data.val.empty())
    {
        if(!quiet) pdlg->Update(400, "Generating heliostat geometry");
        _SF.PrepareFieldLayout(_SF, 0, true);    //Run the layout method in refresh_only mode

        double azzen[2];
        _SF.CalcDesignPtSunPosition(_variables.sf.sun_loc_des.mapval(), azzen[0], azzen[1]);
        Vect sun = Ambient::calcSunVectorFromAzZen(azzen[0]*D2R, azzen[1]*D2R);
        _SF.calcHeliostatShadows(sun);

        if(!quiet) pdlg->Update(1000);
    }
        
    if(!quiet) pdlg->Destroy();
    if(!quiet) _layout_gauge->SetValue(0);

    //Destroy the notebook and recreate
    //this needs to come after we've created the solar field stuff
    _page_panel->DestroyPages();
    CreateInputPages(this, _page_panel);
    this->GetSizer()->Clear();
    this->GetSizer()->Add(_page_panel, 0, wxEXPAND, 0);

    _page_panel->SetActivePage(0);

    for(int i=0; i<_page_panel->GetPageCount(); i++)
    {
        wxWindow *page = _page_panel->GetPage(i);
        this->GetSizer()->Add(page, 1, wxEXPAND, 0);
    }

    this->GetSizer()->Layout();
    
    //Update the layout grid
    UpdateLayoutGrid();
    //Show the land boundaries
    //DoLayoutPlot(true);
    _plot_frame->SetPlotData( _SF, FIELD_PLOT::LAND );
    //update the selection combo
    _plot_select->SetSelection( FIELD_PLOT::LAND );
    
    //Update the GUI values
    UpdateInputValues();
    UpdateCalculatedGUIValues();

    //Update the parametrics values
    ParReloadVariableList();
    ParRefreshValuesList();
    SyncParValuesTable();
    //update optimization values
    OptReloadVariableList();
    
    //Add the file name to the list of recently opened files
    AddRecentFile( file );
    UpdateRecents();
    
    //Update the value of the current file
    _open_file = file;

    //Set the change flag to false
    SetGeomState(false);
    _inputs_modified = false;

}

void SPFrame::NewFile()
{
    //Check for modifications
    if(! ModCheckPrompt()) return;

    SetCaseName( "New file" );    //Update the displayed case name 

    //Use the variable definitions file to set up variable structure
    _variables.reset();
    _par_data.clear();
    _opt_data.clear();

    //Initialize the solar field geometry objects
    _SF.Create(_variables);
    
    //Destroy the notebook and recreate
    _page_panel->DestroyPages();
    CreateInputPages(this, _page_panel);

    this->GetSizer()->Clear();
    this->GetSizer()->Add(_page_panel, 0, wxEXPAND, 0);

    _page_panel->SetActivePage(0);

    for(int i=0; i<_page_panel->GetPageCount(); i++)
    {
        wxWindow *page = _page_panel->GetPage(i);
        this->GetSizer()->Add(page, 1, wxEXPAND, 0);
    }

    this->GetSizer()->Layout();

    //Show the land boundaries
    _plot_frame->SetPlotData( _SF, FIELD_PLOT::LAND );
    //update the selection combo
    _plot_select->SetSelection( FIELD_PLOT::LAND );

    //Update the GUI values
    UpdateInputValues();
    UpdateCalculatedGUIValues();
    
    //Add the file name to the list of recently opened files
    if(_open_file != "")
    {
        AddRecentFile( _open_file );
        UpdateRecents();
    }

    //Update the value of the current file
    _open_file = "";

    //Set the change flag to false
    SetGeomState(false);
    _inputs_modified = false;

    //Notify the user
    PopMessage("New file created");
}

void SPFrame::UpdateRecents()
{
    int nmenu = recentMenu->GetMenuItemCount();
    //clear all of the items from the menu
    for(int i=0; i<nmenu; i++)
    {
        recentMenu->Destroy( ID_FILE_RECENT + i );
    }
    
    string fnames; 
    bool load_ok = false;
    if(ioutil::file_exists(_recent_file_list.GetFullPath().c_str()))
    { 
        string eol;
        ioutil::read_file(_recent_file_list.GetFullPath().ToStdString(), fnames, eol);
        vector<string> files = split(fnames, eol);
        int fsize = files.size();
        if(fsize>0)
        {
            int nloaded = 0;
            for(int i=0; i<(fsize<_nrecent_max ? fsize : _nrecent_max); i++)
            {
                if( ioutil::file_exists(files.at(i).c_str()) ) recentMenu->Append(ID_FILE_RECENT+nloaded,  files.at(i) );
                else continue;
                if(! recentMenu->IsEnabled(ID_FILE_RECENT+nloaded)) recentMenu->Enable(ID_FILE_RECENT+nloaded, true);    //Make sure the item is enabled
                nloaded ++;
            }
            load_ok = true;
        }
    }
    if(! load_ok)
    {
        recentMenu->Append(ID_FILE_RECENT, wxT("< No recent files >"));
        recentMenu->Enable(ID_FILE_RECENT, false);
    }
}

void SPFrame::AddRecentFile(wxFileName &fname)
{
    //Add the file name to the list of recently opened files
    string ftexts, eols;
    ioutil::read_file(_recent_file_list.GetFullPath().ToStdString(), ftexts, eols);
    wxString ftext(ftexts), eol(eols);
    //split into entries
    vector<string> fvals = split(ftext.ToStdString(), eol.ToStdString());
    //check if the file is already in the recent list
    fvals.insert(fvals.begin(), fname.GetFullPath().ToStdString());
    int nvals = fvals.size();
    for(int i=1; i<nvals; i++)
    {
        if( lower_case(fname.GetFullPath().ToStdString()) == lower_case(fvals.at(i)) )
        { 
            fvals.erase(fvals.begin()+i); 
            nvals--;
        }
    }
    //update the list of recently opened files
    wxTextFile fobj(_recent_file_list.GetFullPath());
    if(! (fobj.Exists() ? fobj.Open() : fobj.Create()) )
    {
        wxString msg;
        msg.Printf("Error updating the File->Recent Files list. Please report this bug to %s.", _contact_info.c_str());
        PopMessage(msg);
        return;
    }

    fobj.Clear();
    int fsize = fvals.size();
    for(int i=0; i<min(_nrecent_max, fsize); i++)
    {
        fobj.AddLine(fvals.at(i));
    }
    fobj.Write();
    fobj.Close();
    
}

bool SPFrame::GetInputModState()
{
	return _inputs_modified;
};

bool SPFrame::ModCheckPrompt()
{
    /* 
    If any variables have been modified, prompt the user to save before continuing
    with the operation.

    User may save 
    (YES) : Saves file, returns TRUE and can continue operation
    (NO) : Does not save file, returns TRUE and can continue operation
    (CANCEL) : Does not save file, returns FALSE and operation should terminate
    */
    if(_inputs_modified)
    {
        int id = PopMessage("Would you like to save your changes?","Save changes", wxYES_NO|wxCANCEL);
        if(id == wxID_YES)
        {
            Save(0);
        }
        else if(id == wxID_NO)
        {
            /* do nothing */
        }
        else
        {
            return false;        // cancel
        }
    }
    return true;
}

//------------- Tools -------------------

void SPFrame::OnToolsSettings( wxCommandEvent &WXUNUSED(event))
{
    settings_dialog *dlg = new settings_dialog(this, wxID_ANY, wxT("Program settings"), _gui_settings, wxSize(400,500));
    if( dlg->ShowModal() == wxID_OK)
    {
        //Update the stored values
        std::string sf = _settings_file.GetFullPath();
        gui_util::writeSettingsFile(sf, _gui_settings);
        to_integer(_gui_settings["ncpu"].value, &_n_threads);
        int nmaxthreads = wxThread::GetCPUCount();
        if(_n_threads > nmaxthreads || _n_threads < 0) _n_threads = nmaxthreads;

        to_integer(_gui_settings["nrecent_max"].value, &_nrecent_max);
    }
    else
    {    
        //Do nothing, cancelled
    }
    dlg->Destroy();
}


//------------- help --------------------

void SPFrame::OnHelpContents( wxCommandEvent &WXUNUSED(event) )
{
    ShowContextHelp();
}

void SPFrame::OnHelpAbout( wxCommandEvent &WXUNUSED(event) )
{
    /* Show the splash screen again with information on the version and authors */
    string dir = _image_dir.GetPath(true) + "Splash_about.png";
    gui_about *pop = new gui_about(this, wxID_ANY, dir, _software_version, _imageList->GetIcon( ID_ICON_MAIN));
    pop->Show();

}

void SPFrame::OnHelpLicense( wxCommandEvent &WXUNUSED(event) )
{
    gui_license *pop = new gui_license(this, wxID_ANY, _imageList->GetIcon( ID_ICON_MAIN ) );
    pop->Show();
}

void SPFrame::ShowContextHelp()
{

    //which page is currently shown?
    wxString page = _page_panel->GetActivePageName();

    wxString topic_map;

    if(page == pageNames.climate)
        topic_map = "Climate";

    else if(page == pageNames.markets)
        topic_map = "Markets";

    else if(page == pageNames.costs)
        topic_map = "Costs";

    else if(page == pageNames.layout_control)
        topic_map = "LayoutSetup";

    else if(page == pageNames.heliostat_master)
        topic_map = "HelioTempManagement";

    else if(page == pageNames.receiver_master)
        topic_map = "RecTempManagement";

    else if(page == pageNames.simulations)
        topic_map = "Simulations";

    else if(page == pageNames.simulations_layout)
        topic_map = "FieldLayout";

    else if(page == pageNames.simulations_performance)
        topic_map = "PerformanceSimulation";

    else if(page == pageNames.simulations_parametrics)
        topic_map = "Parametrics";

    else if(page == pageNames.simulations_optimization)
        topic_map = "Optimization";
    
    else if(page == pageNames.results)
        topic_map =    "Results";
    
    else if(page == pageNames.results_layout)
        topic_map = "LayoutResults";
    
    else if(page == pageNames.results_flux)
        topic_map = "ReceiverFluxProfile";
    
    else if(page == pageNames.results_summary)
        topic_map = "SystemSummary";
    
    else
    {
        //handle heliostats and receivers
        bool is_topic = false;
        for(int i=0; i<_variables.recs.size(); i++)
        {
            if(_variables.recs.at(i).rec_name.val == page)
            {
                is_topic = true;
                break;
            }
        }

        if(is_topic)
        {
            //show the receiver help topic.
            topic_map = "ReceiverTemplate";
        }
        else
        {
            //check heliostats
            for(int i=0; i<_variables.hels.size(); i++)
            {
                if(_variables.hels.at(i).helio_name.val == page)
                {
                    is_topic = true;
                    break;
                }
            }
            if(is_topic)
            {
                topic_map = "HeliostatTemplate";
            }
            else
            {
                topic_map = "Pagereference";
            }

        }
    }

    wxFileName hp = _help_dir;
    hp.SetName(topic_map + ".html");
    wxPlatformInfo P = wxPlatformInfo::Get();
    switch(P.GetOperatingSystemId())
    {
    case wxOperatingSystemId::wxOS_WINDOWS:
    case wxOperatingSystemId::wxOS_WINDOWS_NT:
    case wxOperatingSystemId::wxOS_WINDOWS_CE:
    {
        //show the heliostat help topic
        wxString help = wxString::Format("HH.EXE \"ms-its:%s/SolarPILOT.chm::/%s.htm\"", _help_dir.GetPath().ToStdString(), topic_map);
        wxExecute(help);

        break;
    }
    case wxOperatingSystemId::wxOS_MAC:
        wxExecute("open "+hp.GetFullPath(), wxEXEC_ASYNC, NULL);    //Mac
        break;
    case wxOperatingSystemId::wxOS_UNIX_LINUX:
        wxExecute("xdg-open "+hp.GetFullPath(), wxEXEC_ASYNC, NULL);
        break;
    }


}

SolarField *SPFrame::GetSolarFieldObject()
{
    return &_SF;
}


var_map *SPFrame::GetVariablesObject()
{
    return &_variables;
}

sim_results *SPFrame::GetResultsObject()
{
    return &_results;
}

void SPFrame::SetThreadCount(int nthread)
{
    _n_threads = max( min(wxThread::GetCPUCount(), nthread), 1 );
}

int SPFrame::GetThreadCount()
{
    return _n_threads;
}

ArrayString *SPFrame::GetLocalWeatherDataObject()
{
    return &_local_wfdat;
}

FluxPlot *SPFrame::GetFluxPlotObject()
{
    return _flux_frame;
}

FieldPlot *SPFrame::GetFieldPlotObject()
{
    return _plot_frame;
}

parametric *SPFrame::GetParametricDataObject()
{
    return &_par_data;
}

optimization *SPFrame::GetOptimizationDataObject()
{
    return &_opt_data;
}

void SPFrame::SetScriptWindowPointer(SolarPILOTScriptWindow *p)
{
    _active_script_window = p;
}

SolarPILOTScriptWindow* SPFrame::GetScriptWindowPointer()
{
    return _active_script_window;
}

std::string SPFrame::GetVersionInfo()
{
    return _software_version;
}

wxFileName SPFrame::GetImageDir()
{
    return _image_dir;
}

void SPFrame::OnMenuRunLayout( wxCommandEvent &WXUNUSED(evt) )
{
    //switch to the layout page
    _page_panel->SetActivePage( pageNames.simulations_layout );
    this->GetSizer()->Layout();

    //Trigger the layout event as if the button were clicked
    wxCommandEvent newevent( wxEVT_COMMAND_BUTTON_CLICKED );
    newevent.SetId( _layout_button->GetId() );
    newevent.SetEventObject( _layout_button );
    _layout_button->GetEventHandler()->ProcessEvent(newevent);


}

void SPFrame::OnMenuRunSimulation( wxCommandEvent &WXUNUSED(evt) )
{
    try
    {
        _page_panel->SetActivePage( pageNames.simulations_performance );
        this->GetSizer()->Layout();

        //Trigger the simulate event as if the button were clicked
        wxCommandEvent newevent( wxEVT_COMMAND_BUTTON_CLICKED );
        newevent.SetId( _flux_button->GetId() );
        newevent.SetEventObject( _flux_button );
        _flux_button->GetEventHandler()->ProcessEvent(newevent);

    }
    catch(std::exception &e)
    {
        PopMessage(e.what());
    }
    catch(...)
    {
        PopMessage("Caught error in OnMenuRunSimulation");
    }
}

void SPFrame::OnMenuRunRefresh( wxCommandEvent &WXUNUSED(evt) )
{
    _page_panel->SetActivePage( pageNames.simulations_layout );
    this->GetSizer()->Layout();
    
    wxCommandEvent dummyevent;
    OnLayoutRefresh( dummyevent );
}

void SPFrame::OnMenuRunOptimize( wxCommandEvent &WXUNUSED(evt) )
{
    _page_panel->SetActivePage( pageNames.simulations_optimization );
    this->GetSizer()->Layout();

    wxCommandEvent newevent;
    newevent.SetEventObject( _optimize_button );
    OnDoOptimizationSimulation( newevent );

}

void SPFrame::OnMenuRunParametric( wxCommandEvent &WXUNUSED(evt) )
{
    _page_panel->SetActivePage( pageNames.simulations_parametrics );
    this->GetSizer()->Layout();

    wxCommandEvent dummyevent;
    OnParametricSimulate( dummyevent );
}

void SPFrame::OnMenuRunUserParametric( wxCommandEvent &WXUNUSED(evt) )
{
    _page_panel->SetActivePage( pageNames.simulations_parametrics );
    this->GetSizer()->Layout();

    wxCommandEvent dummyevent;
    OnUserParSimulate( dummyevent );
}

/* 
--------------------------------------------------------------------------------------

                            Supplemental GUI methods

--------------------------------------------------------------------------------------
*/

void SPFrame::SetCaseName(wxString case_name)
{
    SetTitle( "SolarPILOT " + _software_version + "  |  " + ( case_name.IsEmpty() ? "New file" : case_name) );
}

void SPFrame::bindControls()
{
    /* Update all of the input controls to bind events */
    
    //Bind all of the input objects to handle changes in values
    input_map::iterator iti;
    for( iti=_input_map.begin(); iti != _input_map.end(); iti++ )
    {
        if(iti->second->is_binding_set) continue;
        iti->second->is_binding_set = true;

        if(iti->second->isCombo())
            iti->second->Connect(wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( SPFrame::OnComboEvent ), NULL, this);
    
        else if(iti->second->isCheckbox())
            iti->second->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( SPFrame::OnComboEvent ), NULL, this);
        
        else
            iti->second->Connect(wxEVT_KILL_FOCUS, wxFocusEventHandler( SPFrame::OnFocusEvent ), NULL, this); 
    }
}

void SPFrame::UpdateInputValues()
{
    /* Update all of the input values to be consistent with the variable map */
    for(input_map::iterator it=_input_map.begin(); it != _input_map.end(); it++)
    {
        std::string ifas = it->first->as_string();
        it->second->setValue( ifas );
        if(it->second->isCombo()) 
            it->second->updateInputDisplay();
    }

    //Other inputs not in the _input_map
    _design_ndays->SetValue( _variables.sf.des_sim_ndays.val );

    UpdateDesignSelect( _variables.sf.des_sim_detail.mapval(), _variables);

    //Bounds table
    UpdateLandBoundsGrid(_variables);
    //User sun grid
    UpdateUserSunGrid();
    //emittance grids
    
    //update heliostat template grid
    setHelioTemplateGrid();

    //Climate file
    wxString wname = (wxString)_variables.amb.weather_file.val;
    UpdateClimateFile( wname, _variables );

    //user parametric table
    UpdateParValueTable();

}

void SPFrame::UpdateCalculatedGUIValues()
{
    /*
    Update the GUI calculated values. First call the UpdateCalculatedMapValues() method to 
    provide calculations required for the simulation, then proceed to update the values in 
    the GUI controls here. Additional calculations within this method are for GUI reference
    only and are not used by the simulation.
    */
    
    //First update the map values that are used in setting the GUI values
    _SF.updateAllCalculatedParameters(_variables);

    //Anything that triggers this subroutine would also prompt for saved changes
    _inputs_modified = true;
    SetGeomState(true);

    //First enable and disable controls depending on the option selections in the GUI
    vector<wxWindow*> 
        *alldisobjs;    //pointer to sibling objects for each control
    std::map<wxWindow*, bool> odisplay; //Map from the object pointer to the final display state

    output_map::iterator ot;
    for(ot = _output_map.begin(); ot != _output_map.end(); ot++)
    {
        odisplay[ ot->second ] = true;
    }

    input_map::iterator it;
    vector<wxWindow*> overrides;
    for(it = _input_map.begin(); it != _input_map.end(); it++)
    { 
        //Get a list of all overridden variables to hide.
        if( it->second->isOverride() )
            overrides.push_back(it->second);

        //setup for processing
        odisplay[ it->second ] = true;    //Enable all of the controls
        it->second->setColour(); //unhighlight all of the controls
        //Enable other associated controls
        alldisobjs = it->second->getAllDisabledSiblings();
        if(alldisobjs->size()>0)
        {
            for(unsigned int i=0; i<alldisobjs->size(); i++)
            { 
                odisplay[ alldisobjs->at(i) ] = true;
            }
        }
    }
    
    vector<wxWindow*> *disobj;
    for(it = _input_map.begin(); it != _input_map.end(); it++)
    {
        //Disable according to each configuration control
        if(it->second->isCombo() || it->second->isCheckbox())
        {
            std::string stname = (string)it->second->getValue();
            disobj = it->second->getDisabledSiblings( stname );

            if(disobj->size() > 0)
            {
                //If the control is a combobox or checkbox and there are associated disabled siblings..
                for(unsigned int i=0; i<disobj->size(); i++)
                        odisplay[ disobj->at(i) ] = false;
            }
        }
    }
    //Check outputcontrols for override status
    for(output_map::iterator oit = _output_map.begin(); oit != _output_map.end(); oit++)
    { 
        if( oit->second->isOverride() )
            overrides.push_back(oit->second);
    }

    //Now apply the display status to each object
    int ii=-1;
    for(std::map<wxWindow*, bool>::iterator ito=odisplay.begin(); ito != odisplay.end(); ito++)
    {
        ii++;
        if(ito->first->IsBeingDeleted()) continue;
        if(ito->second) 
        {
            ito->first->Enable(); 
        }
        else
        {
            ito->first->Disable(); 
        }
    }
    for(int i=0; i<(int)overrides.size(); i++)
    {
        if( overrides.at(i)->IsBeingDeleted() ) continue;
        overrides.at(i)->Hide();
        overrides.at(i)->GetParent()->Layout();
        try
        {
            overrides.at(i)->GetGrandParent()->Layout();
        }
        catch(...)
        {
        }
    }


    //update all UI output controls based on mapped variables
    for( output_map::iterator out = _output_map.begin(); out != _output_map.end(); out++)
    {
        out->second->setValue( out->first->as_string() );
    }

    //need to update calculated values in the receiver power fractions grid too
    UpdateReceiverPowerGrid();

    _page_panel->GetActivePage()->Layout();
    _page_panel->GetActivePage()->Refresh();
    
    _flux_frame->SetZRange(
        _variables.flux.plot_zmin.val,
        _variables.flux.plot_zmax.val,
        _variables.flux.is_autoscale.val);

}

void SPFrame::UpdateFieldPlotSelections()
{
    /* 
    Update the field plot selection combo based on the system being simulated
    */
    _plot_select->Clear();

    std::vector< std::string > choices = _plot_frame->GetPlotChoices();
    
    _plot_select->Append( choices.at( FIELD_PLOT::LAND ) );
    _plot_select->SetValue( choices.front() );

    if( _plot_frame->IsDataReady() )
    {
        _plot_select->Append( choices.at(  FIELD_PLOT::LAYOUT ) );
        _plot_select->Append( choices.at(  FIELD_PLOT::EFF_TOT ) );
            _plot_select->SetValue( choices.at( FIELD_PLOT::EFF_TOT ) );
        _plot_select->Append( choices.at(  FIELD_PLOT::EFF_COS ) );
        _plot_select->Append( choices.at(  FIELD_PLOT::EFF_ATT ) );
        _plot_select->Append( choices.at(  FIELD_PLOT::EFF_BLOCK ) );
        _plot_select->Append( choices.at(  FIELD_PLOT::EFF_SHAD ) );
        _plot_select->Append( choices.at(  FIELD_PLOT::EFF_INT ) );
        _plot_select->Append( choices.at(  FIELD_PLOT::EFF_REFLECT ) );
        _plot_select->Append( choices.at(  FIELD_PLOT::POWER ) );
        _plot_select->Append( choices.at(  FIELD_PLOT::RANK ) );
        _plot_select->Append(choices.at(   FIELD_PLOT::EFF_ANNUAL ) );
        _plot_select->Append(choices.at(   FIELD_PLOT::ENERGY ) );
    }

    if( _SF.getVarMap()->recs.size() > 1 )
        _plot_select->Append( choices.at(  FIELD_PLOT::RECEIVER ) );

    if( _SF.getVarMap()->sf.is_opt_zoning.val )
        _plot_select->Append( choices.at(  FIELD_PLOT::MESH ) );

    if( _SF.getVarMap()->flux.is_cloudy.val )
        _plot_select->Append( choices.at(  FIELD_PLOT::EFF_CLOUD ) );


}

void SPFrame::UpdateFluxPlotSelections()
{
    _rec_select->Clear();
    Rvector *active_recs = _SF.getReceivers();
    for (int i = 0; i < active_recs->size(); i++)
    {
        std::string rname = active_recs->at(i)->getVarMap()->rec_name.val;
        _rec_select->Append(rname);
        if (i == 0)
            _rec_select->SetValue(rname);
    }

}

void SPFrame::GridCount(wxSpinCtrl *sc, wxGrid *grid)
{
    int nnew = sc->GetValue();
    GridCount(nnew, grid->GetNumberCols(), grid);
}

void SPFrame::GridCount(int nrow, int ncol, wxGrid *grid)
{
    int nprevc = grid->GetNumberCols();
    int nprevr = grid->GetNumberRows();
    
    //Add the correct number of rows
    if(nrow > nprevr)
    {
        grid->AppendRows(nrow-nprevr);
    }
    else if(nrow < nprevr)
    {
        grid->DeleteRows(nrow, nprevr-nrow);
    }
    else
    { /* do nothing */ }
    
    //Add the correct number of columns
    if (ncol > nprevc)
    {
        grid->AppendCols(ncol - nprevc);
    }
    else if (ncol < nprevc)
    {
        grid->DeleteCols(ncol, nprevc - ncol);
    }
    else
    { /* do nothing */
    }
}

void SPFrame::SetGeomState(bool state)
{
    /* 
    If TRUE, the solar field geometry is not consistent with what is displayed in the layout data.
    if FALSE, the geometry is up to date. Hide the notification icon.
    */
    
    _geom_modified = state;
    
    //show or hide the heliostat grid notice icon depending on the state
    if(_geom_modified)
    {
        if(! _refresh_warning->IsShown()) 
        {
            _refresh_warning->Show();
            _refresh_warning->GetParent()->Layout();
        }
        if(! _layout_refresh->IsEnabled()) _layout_refresh->Enable(true);
    }
    else
    {
        if(_refresh_warning->IsShown())
        {
            _refresh_warning->Hide();
            _refresh_warning->GetParent()->Layout();
        }
        if(_layout_refresh->IsEnabled()) _layout_refresh->Disable();
    }
    
}

bool SPFrame::GetGeomState()
{ 
    return _geom_modified; 
}

void SPFrame::DoResultsPage()
{
    //Delete old pages
    wxWindow *page = _page_panel->GetPage(pageNames.results_summary );
    wxScrolledWindow *swpage = static_cast<wxScrolledWindow*>( page );
    
    page->DestroyChildren();

    CreateResultsSummaryPage(swpage);

}

void SPFrame::SimProgressBase(int n_complete, int n_tot, string *notices, wxGauge *active_gauge, wxStaticText *active_label, wxString label_text)
{
    //Update the gui
    double simprog;
    active_label->SetLabel( wxString::Format("%s (%d/%d)", label_text, n_complete+1, n_tot) );

    if(*notices != "")
    {
        _layout_log->AppendText("\n");
        _layout_log->AppendText(*notices);
    }
    if(n_tot>0) simprog = (float)(n_complete+1)/(float)n_tot;
    else simprog = 0.;
    simprog = max(min(simprog, 1.), 0.);
    active_gauge->SetValue(int(simprog*1000.));
    wxYieldIfNeeded();    //prevent GUI from freezing
}

void SPFrame::FluxProgressBase(int n_complete, int n_tot, wxGauge *active_gauge, wxStaticText *active_label, wxString label_text)
{
    //Update the gui
    double simprog;

    active_label->SetLabel( wxString::Format("%s (%d/%d)", label_text, n_complete, n_tot) );

    if(n_tot>0) simprog = (float)n_complete/(float)n_tot;
    else simprog = 0.;
    simprog = max(min(simprog,1.), 0.);
    active_gauge->SetValue(int(simprog*1000.));

    //Refresh (mark as dirty) and force update. Note that wxYield* variants cause the window to hang on linux here.
    active_gauge->Refresh();
    active_gauge->Update();
#ifdef _WIN32
    wxYield();
#endif

}

int SPFrame::SolTraceProgressUpdate(st_uint_t ntracedtotal, st_uint_t ntraced, st_uint_t ntotrace, st_uint_t curstage, st_uint_t nstages, void *data)
{
    (void)data;
    (void)ntracedtotal; //these are currently unreferenced.
    /* 
    UI callback for SolTrace simulation progress 
    
    Variables are:
    st_uint_t ntracedtotal    |    Number of total rays traced
    st_uint_t ntraced        |    Number of rays traced this stage
    st_uint_t ntotrace        |    Number or rays remaining to be traced
    st_uint_t curstage        |    Current stage
    st_uint_t nstages        |    Total number of stages to trace
    void *data                |    Contains pointer to SPFrame class
    
    This method is called to update the GUI simulation progress display according
    to the status of the simulation info object.

    This is implemented as a callback from the simulation code.
    */
    
    //Check if the simulation has been cancelled
    if(_cancel_simulation) 
        return 0;    //Code for coretrace to quit
    
    //Update the gui
    double pfrac = (double)ntraced/max((double)ntotrace, 1.)*100.;
    int fprog;
    
    if(pfrac>100. || pfrac < 1.e-6) 
        fprog = 0;
    else 
        fprog = (int)pfrac;

    //provide an estimate of the remaining time
    int ctime = (int)_sim_watch.Time();
    int endtime = (int)( (ctime * (double)ntotrace/(double)(ntraced > 0 ? ntraced : 1) - ctime)/1000. );
    
    wxString tremmsg;
    if(pfrac < 10.)
    {
        if(endtime > 150)
            tremmsg.Printf( "%d min left", endtime/60 );
        else
            tremmsg.Printf( "%d s left", endtime );
    }
    else
    {
        int min = endtime/60;
        int sec = endtime - min*60;
        tremmsg.Printf( "%d:%02d left", min, sec );
    }

    wxGauge *active_gauge = chooseProgressGauge();
    wxStaticText *active_text = chooseProgressText();
    active_text->SetLabel( wxString::Format("Progress (Stage %d/%d): %.2f%s, %s", (int)curstage, (int)nstages, pfrac,"%", tremmsg.c_str()) );

    active_gauge->SetValue( max(int(fprog*10.),0) );
    wxYieldIfNeeded();    //prevent GUI from freezing
    active_gauge->Update();
    active_gauge->Refresh();
    return 1;
}

bool SPFrame::SolTraceFluxSimulation(SolarField &SF, var_map &vset, Hvector &helios)
{
    /* 
    Send geometry to Soltrace and get back simulation results. 
    From Soltrace library, reference "runthreads.cpp" and "sysdata.cpp"

    Note that the SolTrace coordinate system for defining the sun position is typically:
    X -> positive west
    Y -> positive toward zenith
    Z -> positive north
    .. However ..
    We will use a coordinate system consistent with SolarPILOT geometry where:
    X -> positive east
    Y -> positive north
    Z -> positive zenith

    As long as the sun position vector passed to SolTrace is consistent with the field geometry
    that we're using, the results will be correct.

    */

    bool is_load_raydata = vset.flux.is_load_raydata.val;
    bool is_save_raydata = vset.flux.is_save_raydata.val;
    //raydata_file
    wxFileName raydata_file( vset.flux.raydata_file.val );

    //check that the file exists
    vector<vector<double> > raydat_st0;
    vector<vector<double> > raydat_st1;
    int nsunrays_loadst=0;
    if(is_load_raydata)
    {
        if(! ioutil::file_exists( raydata_file.GetFullPath().c_str() ) )
            throw spexception("Specified ray data file does not exist. Looking for file: " + raydata_file.GetFullPath());

        //Load the ray data from a file
        ifstream fdat( raydata_file.GetFullPath().ToStdString() );

        if( fdat.is_open() )
        {
            
            string str;
            str.reserve(96);

            char line[96];
            bool nextloop = false;
            bool firstline = true;
            while( fdat.getline( line, 96 ) )
            {
                //first line is number of sun rays
                if(firstline)
                {
                    for(int i=0; i<16; i++)
                    {
                        if(line[i] == '\n' || i==15)
                        {
                            to_integer(str, &nsunrays_loadst);
                            str.clear();
                            str.reserve(96);
                            break;
                        }
                        else
                        {
                            str.push_back(line[i]);
                        }
                    }
                    firstline = false;
                    continue;
                }


                vector<double> dat(8);

                
                int ilast = 0;
    
                for(int i=0; i<96; i++)
                {
                    //check for the transition character
                    if( line[i] == '#' ) 
                    {
                        nextloop = true;
                        break;
                    }

                    if( line[i] == ',' )
                    {
                        to_double(str, &dat[ilast++]);
                        
                        //clear the string
                        str.clear();
                        str.reserve(96);
                        //if this was the 8th entry, go to next line
                        if( ilast > 7 ) 
                            break;
                    }
                    else
                    {
                        str.push_back( line[i] );
                    }
                }
                ilast = 0;

                if( nextloop ) break;

                raydat_st0.push_back(dat);
            }

            //next loop to get stage 1 input rays
            while( fdat.getline( line, 96 ) )
            {
                vector<double> dat(7);
                
                int ilast = 0;
                //int istr = 0;
                for(int i=0; i<96; i++)
                {
                    if( line[i] == ',' )
                    {
                        to_double(str, &dat[ilast++]);
                        
                        //clear the string
                        str.clear();
                        str.reserve(96);
                        //if this was the 8th entry, go to next line
                        if( ilast > 6 ) 
                            break;
                    }
                    else
                    {
                        str.push_back( line[i] );
                    }
                }
                ilast = 0;

                raydat_st1.push_back(dat);
            }

            fdat.close();
        }

        //set the number of traced rays based on the length of the supplied data
        int nray = (int)raydat_st0.size() ;
        
        vset.flux.min_rays.val =  nray ;
    }
    //for saving, check that the specified directory exists. If none specified or if it doesn't exist, prepend the working directory.
    if(is_save_raydata)
    {
        if(! raydata_file.DirExists() )
            raydata_file = _working_dir.GetPath(true) + raydata_file.GetName(); 
    }

    bool err_maxray = false;
    int minrays, maxrays;
    vector<st_context_t> contexts;

    _stthread = 0;    //initialize to null

    //get sun position and create a vector
    double el = vset.flux.flux_solar_el.Val()*D2R;
    double az = vset.flux.flux_solar_az.Val()*D2R;
    Vect sun = Ambient::calcSunVectorFromAzZen(az, PI/2.-el);

    _STSim = new ST_System;
    
    _STSim->CreateSTSystem(SF, helios, sun);

    minrays = _STSim->sim_raycount;
    maxrays = _STSim->sim_raymax;

    vector< vector<vector< double > >* > st0datawrap;
    vector< vector<vector< double > >* > st1datawrap;

    if(_n_threads > 1)
    {
        //Multithreading support
        _stthread = new STSimThread[_n_threads];
        _is_mt_simulation = true;

        int rays_alloc=0;
        int rays_alloc1=0;
        for(int i=0; i<_n_threads; i++)
        {
            //declare soltrace context
            st_context_t pcxt = st_create_context();
            //load soltrace data structure into context
            ST_System::LoadIntoContext(_STSim, pcxt);
            //get random seed
            int seed = SF.getFluxObject()->getRandomObject()->integer();
            //setup the thread
            _stthread[i].Setup(pcxt, i, seed, is_load_raydata, is_save_raydata);

            //Decide how many rays to trace for each thread. Evenly divide and allocate remainder to thread 0
            int rays_this_thread = _STSim->sim_raycount/_n_threads;
            if (i==0) rays_this_thread += (_STSim->sim_raycount%_n_threads);
            //when loading ray data externally, we need to divide up receiver stage hits
            int rays_this_thread1 = raydat_st1.size() / _n_threads;     //for receiver stage input rays
            if (i==0) rays_this_thread1 += (raydat_st1.size()%_n_threads);

            //if loading ray data, add by thread here
            if(is_load_raydata)
            {
                _stthread[i].CopyStageRayData( raydat_st0, 0, rays_alloc, rays_alloc + rays_this_thread );
                _stthread[i].CopyStageRayData( raydat_st1, 1, rays_alloc1, rays_alloc1 + rays_this_thread1 );   //for receiver stage input rays
            }
            rays_alloc += rays_this_thread;
            rays_alloc1 += rays_this_thread1;

            st_sim_params( pcxt, rays_this_thread, _STSim->sim_raymax/_n_threads );

        }

        for(int i=0; i<_n_threads; i++)
        {
            thread( &STSimThread::StartThread, std::ref( _stthread[i] ) ).detach();
        }
        //wxLogMessage((wxString)("Running threads"));
        int ntotal=0, ntraced=0, ntotrace=0, stagenum=0, nstages=0;

        // every now and then query the threads and update the UI;
        while (1)
        {
            int num_finished = 0;
            for (int i=0;i<_n_threads;i++)
                if (_stthread[i].IsFinished())
                    num_finished++;

            if (num_finished == _n_threads)
                break;

            // threads still running so update interface
            int ntotaltraces = 0;
            for (int i=0;i<_n_threads;i++)
            {
                _stthread[i].GetStatus(&ntotal, &ntraced, &ntotrace, &stagenum, &nstages);
                ntotaltraces += ntotal;
            }

            SolTraceProgressUpdate(ntotal, ntraced, ntotrace, stagenum, nstages, (void*)NULL);
            
            // if dialog's cancel button was pressed, send cancel signal to all threads
            if (_cancel_simulation)
            {
                for (int i=0;i<_n_threads;i++)
                    _stthread[i].CancelTrace();
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(75));            
        }
        
        // determine if any errors occurred
        bool errors_found = false;
        contexts.clear();
        for (int i=0;i<_n_threads;i++)
        {
            contexts.push_back(_stthread[i].GetContextId());
            int code = _stthread[i].GetResultCode();

            if (code < 0) 
            {
                errors_found = true;
                err_maxray = true;
                PopMessage(wxString::Format("Error occured in trace core thread %d, code=%d.\n\n", i+1, code));
                break;  //Don't keep displaying if there's an error
            }
        }
        
        //Consolidate the stage 0 ray data if needed
        if(is_save_raydata && !errors_found)
        {
            for(int i=0; i<_n_threads; i++)
            {
                st0datawrap.push_back( _stthread[i].GetStage0RayDataObject() );
                st1datawrap.push_back( _stthread[i].GetStage1RayDataObject() );
            }
        }

    }
    else
    {
        //Create context
        st_context_t cxt = st_create_context();
        int seed = SF.getFluxObject()->getRandomObject()->integer();
        ST_System::LoadIntoContext(_STSim, cxt);
        if( interop::SolTraceFluxSimulation_ST(cxt, seed, *_STSim, STCallback, this, &raydat_st0, &raydat_st1, is_save_raydata, is_load_raydata) )
            contexts.push_back(cxt);
        else
            err_maxray = true;  //hit max ray limit if function returns false

        if(is_save_raydata && !err_maxray)
        {
            st0datawrap.push_back( &raydat_st0 );
            st1datawrap.push_back( &raydat_st1 );
        }
    }

    //reset the progress gauge
    _is_mt_simulation = false;
    _flux_gauge->SetValue(0);
    //Did the simulation terminate after reaching the max ray count?
    if(err_maxray)
    { 
        PopMessage("The simulation has reached the maximum number of rays ("+my_to_string(maxrays)+
            ") without achieving the required number of ray hits ("+my_to_string(minrays)+")."+
            " Consider increasing the 'Maximum number of generated rays' or decreasing the "+
            " 'Desired number of ray intersections'.");
        return false;
    }
    //Was the simulation cancelled during st_sim_run()?
    if(_cancel_simulation)
    {
        _cancel_simulation = false; //reset
        return false;
    }
    
    //Process the results
    int nint = 0;
    int nc = contexts.size();
    vector<int> csizes;
    for(int i=0; i<nc; i++)
    {
        csizes.push_back( st_num_intersections(contexts.at(i)) );
        nint += csizes.at(i);
    }

    double bounds[5]; //xmin, xmax, ymin, ymax, empty
    _STSim->IntData.nsunrays=0;

    _STSim->IntData.AllocateArrays(nint);

    //Collect all of the results
    int ind=0;
    for(int i=0; i<nc; i++)
    {
        int cs = csizes.at(i);

        st_locations(contexts.at(i), &_STSim->IntData.hitx[ind], &_STSim->IntData.hity[ind], &_STSim->IntData.hitz[ind]);
        st_cosines(contexts.at(i), &_STSim->IntData.cosx[ind], &_STSim->IntData.cosy[ind], &_STSim->IntData.cosz[ind]);
        st_elementmap(contexts.at(i), &_STSim->IntData.emap[ind]);
        st_stagemap(contexts.at(i), &_STSim->IntData.smap[ind]);
        st_raynumbers(contexts.at(i), &_STSim->IntData.rnum[ind]);

        int nsr;
        st_sun_stats(contexts.at(i), &bounds[0], &bounds[1], &bounds[2], &bounds[3], &nsr);    //Bounds should always be the same
        _STSim->IntData.nsunrays += nsr;
        ind += cs;

    }

    //DNI
    double dni = vset.sf.dni_des.val/1000.;    //[kw/m2]
    
    //if the heliostat field ray data is loaded from a file, just specify the number of sun rays based on this value
    if(is_load_raydata)
        _STSim->IntData.nsunrays = nsunrays_loadst;

    //Get bounding box and sun ray information to calculate power per ray
    _STSim->IntData.q_ray = (bounds[1]-bounds[0])*(bounds[3]-bounds[2])/float(_STSim->IntData.nsunrays)*dni;
    

    bool skip_receiver = false;

    if(! skip_receiver)
    {

        bounds[4] = (float)_STSim->IntData.nsunrays;
        
        for(int i=0; i<5; i++)
            _STSim->IntData.bounds[i] = bounds[i];
        SolTraceFluxBinning(SF);


        //Process the results
        sim_params P;
        P.dni = dni;
        double azzen[2] = {az, PI/2.-el};
        _results.back().process_raytrace_simulation(SF, P, 2, azzen, helios, _STSim->IntData.q_ray, _STSim->IntData.emap, _STSim->IntData.smap, _STSim->IntData.rnum, nint, bounds);    
    }

    //If the user wants to save stage0 ray data, do so here
    if(is_save_raydata)
    {
        ofstream fout(raydata_file.GetFullPath().ToStdString());
        fout.clear();
        //first line is number of sun rays
        fout << _STSim->IntData.nsunrays << "\n";
        //write heliostat IN stage
        for(int i=0; i<(int)st0datawrap.size(); i++)
        {
            for( int j=0; j<(int)st0datawrap.at(i)->size(); j++)
            {
                for( int k=0; k<8; k++ )
                    fout << st0datawrap.at(i)->at(j).at(k) << ",";
                fout << "\n";
            }
        }
        //special separator
        fout << "#\n";
        //write receiver IN stage
        for(int i=0; i<(int)st1datawrap.size(); i++)
        {
            for( int j=0; j<(int)st1datawrap.at(i)->size(); j++)
            {
                for( int k=0; k<7; k++ )
                    fout << st1datawrap.at(i)->at(j).at(k) << ",";
                fout << "\n";
            }
        }

        fout.close();
    }

    //If the user wants to save the ray data, do so here
    if( vset.flux.save_data.val )
    {
        string fname = vset.flux.save_data_loc.val;
        if(fname == "")
        {
            PopMessage("Ray data was not saved. No file was specified.", "Notice", wxICON_EXCLAMATION|wxOK);
        }
        else
        {
            FILE *file = fopen(fname.c_str(), "w");
            if(! file )
            {
                PopMessage(wxT("Error opening the flux simulation output file. Please make sure the file is closed and the directory is not write-protected."), wxT("File error"), wxICON_ERROR|wxOK);
                return false;
            }
            fprintf(file, "Pos X, Pos Y, Pos Z, Cos X, Cos Y, Cos Z, Element Map, Stage Map, Ray Number\n");
            for(int i=0; i<nint; i++)
            {
                fprintf(file, "%.3f, %.3f, %.3f, %.7f, %.7f, %.7f, %d, %d, %d\n", 
                    _STSim->IntData.hitx[i], _STSim->IntData.hity[i], _STSim->IntData.hitz[i], 
                    _STSim->IntData.cosx[i], _STSim->IntData.cosy[i], _STSim->IntData.cosz[i], 
                    _STSim->IntData.emap[i], _STSim->IntData.smap[i], _STSim->IntData.rnum[i]);
            }
            fclose(file);
        }

    }

    //Clean up
    _STSim->IntData.DeallocateArrays();
    if(_stthread != 0) delete[] _stthread;

    return true;
    
}

bool SPFrame::SolTraceFluxBinning(SolarField &SF)
{
   //Collect all of the rays that hit the receiver(s) into the flux profile
    int rstage1 = 2;

    for(int r = 0; r<(int)SF.getReceivers()->size(); r++)
    {
        Receiver *Rec = SF.getReceivers()->at(r);
        if(! Rec->isReceiverEnabled() )
            continue;
        var_receiver *RV = Rec->getVarMap();

        //.. for each receiver, 
        int recgeom = Rec->getGeometryType(); 
        
        //Pre-declare all relevant variables
        FluxSurface *fs;
        FluxGrid *fg;
        int e_ind, nfx, nfy, 
            ibin, jbin;    //indices of the flux grid bin that the current ray will go into
        double rel, raz, rh, rw, paz, ph, pw, Arec, dqspec;
        Vect rayhit;
        //----------

            
        switch(recgeom)
        {
        case Receiver::REC_GEOM_TYPE::CYLINDRICAL_CLOSED:        //0    |    Continuous closed cylinder - external
        {
            //There will be only one flux surface and flux grid. Get both objects.
            fs = &Rec->getFluxSurfaces()->at(0);
            fs->ClearFluxGrid();
            fg = fs->getFluxMap();
            e_ind = r+1;    //element index for this receiver

            rel = RV->rec_elevation.val*D2R;
            raz = RV->rec_azimuth.val*D2R;
            rh = RV->rec_height.val;
            
            sp_point offset(RV->rec_offset_x_global.Val(), RV->rec_offset_y_global.Val(), RV->optical_height.Val() );   //optical height includes z offset

            //The number of points in the flux grid 
            //(x-axis is angular around the circumference, y axis is receiver height)
            nfx = fs->getFluxNX();
            nfy = fs->getFluxNY();
                        
            Arec = Rec->getAbsorberArea();
            dqspec = _STSim->IntData.q_ray/Arec*(float)(nfx*nfy);

            for(int j=0; j<_STSim->IntData.nint; j++)
            {    //loop through each intersection

                if(_STSim->IntData.smap[j] != rstage1 || abs(_STSim->IntData.emap[j]) != e_ind) continue;    //only consider rays that interact with this element

                //Where did the ray hit relative to the location of the receiver?
                rayhit.Set(_STSim->IntData.hitx[j] - offset.x, _STSim->IntData.hity[j] - offset.y, _STSim->IntData.hitz[j] - offset.z);

                //Do any required transform to get the ray intersection into receiver coordinates
                Toolbox::rotation(-raz, 2, rayhit);
                Toolbox::rotation(-rel, 0, rayhit);

                //Calculate the point location in relative cylindrical coorinates
                paz = 0.5 - atan2(rayhit.i, rayhit.j)/(2.*PI);    //0 for the flux grid begins at <S>, progresses CCW to 1
                ph = 0.5+rayhit.k/rh;    //0 for flux grid at bottom of the panel, 1 at top

                //Calculate which bin to add this ray to in the flux grid
                ibin = int(floor(paz*nfx));
                jbin = int(floor(ph*nfy));

                //Add the magnitude of the flux to the correct bin
                fg->at(ibin).at(jbin).flux += dqspec;

            }

            break;
        }
        case Receiver::REC_GEOM_TYPE::CYLINDRICAL_OPEN:
        case Receiver::REC_GEOM_TYPE::CYLINDRICAL_CAV:
            break;
        case Receiver::REC_GEOM_TYPE::PLANE_RECT:    //3    |    Planar rectangle
        {
            //Only one flux surface and grid per receiver instance
            fs = &Rec->getFluxSurfaces()->at(0);
            fs->ClearFluxGrid();
            fg = fs->getFluxMap();
            e_ind = r+1;    //element index for this receiver

            rel = RV->rec_elevation.val*D2R;
            raz = RV->rec_azimuth.val*D2R;
            rh = RV->rec_height.val;
            rw = Rec->getReceiverWidth(*RV); 
            
            sp_point offset(RV->rec_offset_x_global.Val(), RV->rec_offset_y_global.Val(), RV->optical_height.Val() );   //optical height includes z offset

            //The number of points in the flux grid 
            //(x-axis receiver width, y axis is receiver height)
            nfx = fs->getFluxNX();
            nfy = fs->getFluxNY();
                                    
            Arec = Rec->getAbsorberArea();
            dqspec = _STSim->IntData.q_ray/Arec*(float)(nfx*nfy);

            
            for(int j=0; j<_STSim->IntData.nint; j++)
            {    //loop through each intersection

                if(_STSim->IntData.smap[j] != rstage1 || abs(_STSim->IntData.emap[j]) != e_ind) continue;    //only consider rays that interact with this element

                //Where did the ray hit relative to the location of the receiver?
                rayhit.Set(_STSim->IntData.hitx[j] - offset.x, _STSim->IntData.hity[j] - offset.y, _STSim->IntData.hitz[j] - offset.z);

                //Do any required transform to get the ray intersection into receiver coordinates
                Toolbox::rotation(PI-raz, 2, rayhit);
                Toolbox::rotation(-rel, 0, rayhit);

                //Calculate the point location in relative cylindrical coorinates
                pw = 0.5 + rayhit.i / rw;    //0 at "starboard" side, increase towards "port"
                ph = 0.5 + rayhit.k / rh;    //0 for flux grid at bottom of the panel, 1 at top

                //Calculate which bin to add this ray to in the flux grid
                ibin = int(floor(pw*nfx));
                jbin = int(floor(ph*nfy));

                //Add the magnitude of the flux to the correct bin
                fg->at(ibin).at(jbin).flux += dqspec;

            }
            break;
        }
        case Receiver::REC_GEOM_TYPE::PLANE_ELLIPSE:
        case Receiver::REC_GEOM_TYPE::POLYGON_CLOSED:
        case Receiver::REC_GEOM_TYPE::POLYGON_OPEN:
        case Receiver::REC_GEOM_TYPE::POLYGON_CAV:
        default:
            return false;
            break;
        }
            

    }
    return true;
}

bool SPFrame::HermiteFluxSimulationHandler(SolarField &SF, Hvector &helios)
{
    /* 
    Call the hermite flux evaluation algorithm and process.
    */
    SF.HermiteFluxSimulation(helios, 
        _variables.flux.aim_method.mapval() == var_fluxsim::AIM_METHOD::IMAGE_SIZE_PRIORITY    //to not re-simulate, aim strategy must be "IMAGE_SIZE"...
        && helios.size() == SF.getHeliostats()->size());                                        //and all heliostats must be included.
    
    //Process the results
    double azzen[2];
    azzen[0] = D2R* SF.getVarMap()->flux.flux_solar_az.Val();
    azzen[1] = D2R* (90. - SF.getVarMap()->flux.flux_solar_el.Val() );
    
    sim_params P;
    P.dni = SF.getVarMap()->flux.flux_dni.val;

    _results.back().process_analytical_simulation(SF, P, 2, azzen, &helios);

    //if we have more than 1 receiver, create performance summaries for each and append to the results vector
    if (SF.getActiveReceiverCount() > 1)
    {
        //which heliostats are aiming at which receiver?
        unordered_map<Receiver*, Hvector> aim_map;
        for (Hvector::iterator h = helios.begin(); h != helios.end(); h++)
            aim_map[(*h)->getWhichReceiver()].push_back( *h );

        for (Rvector::iterator rec = SF.getReceivers()->begin(); rec != SF.getReceivers()->end(); rec++)
        {
            _results.push_back( sim_result() );
            Rvector recs = { *rec };
            _results.back().process_analytical_simulation(SF, P, 2, azzen, &aim_map[*rec], &recs);
        }
    }
    
    return true;
}

void SPFrame::SetSimulationStatus(bool in_sim, bool &sim_type_flag, wxBitmapButton *button)
{
    /* 
    Set the flag indicating whether we're in a simulation. Update the 'button' to 
    display the run or cancel icons depending on the state. 

    The "&sim_type_flag" should be the address of the simulation flag that we wish to set.
    Options are "_in_layout_simulation" , "_in_flux_simulation" , "_in_param_simulation", "_in_optimize_simulation"
    */

    if(in_sim)
    {
        PreSimulation();
        //Set the image to the cancel option
        button->SetBitmapLabel(_simimages[3]);
        button->SetBitmapHover(_simimages[3]);
        button->SetBitmapFocus(_simimages[3]);
        button->SetBitmapSelected(_simimages[3]);
        button->SetToolTip("Cancel"); 

        sim_type_flag = true;
        _cancel_simulation = false;
        
    }
    else
    {
        //Set the image to the simulate options
        button->SetBitmapLabel(_simimages[0]);
        button->SetBitmapFocus(_simimages[0]);
        button->SetBitmapHover(_simimages[1]);
        button->SetBitmapSelected(_simimages[2]);
        button->SetToolTip("Run"); 

        sim_type_flag = false;
        _cancel_simulation = false;
        _is_mt_simulation = false;    //Reset
        PostSimulation();
    }
}

int SPFrame::PopMessage(wxString message, wxString title, long style)
{
    /* 
    Method for setting up and displaying a notification to the user. Returns the 
    wx return code based on the dialog options.
    */

    wxMessageDialog *dlg = new wxMessageDialog(this, message, title, style);
    dlg->CenterOnParent();
    int ret_code = dlg->ShowModal();
    dlg->Destroy();
    return ret_code;

}

void SPFrame::ScriptMessageOutput(const char *msg)
{
    if (_active_script_window != 0)
    {
        _active_script_window->ScriptOutput(msg);
    }
}

static bool SAMParametricCallback(simulation_info *siminfo, void *data)
{
    wxProgressDialog *dlg = static_cast<wxProgressDialog*>( data );

    int cursim = siminfo->getCurrentSimulation();
    int totsim = siminfo->getTotalSimulationCount(); 

    if( dlg != 0 )
    {
        if(! dlg->Update( (int)( min( (double)(cursim+1)/(double)totsim*1000., 1000.) ), "Field Optical Performance: Simulation "+my_to_string(cursim)+" of "+my_to_string(totsim) ) )
            return false;
    }
    return true;
};

void SPFrame::SAMInputParametric()
{
    //check whether a layout simulation is currently running. Both simulations cannot occur at the same time.
    if(_in_layout_simulation || _in_flux_simulation || _in_user_param_simulation || _in_optimize_simulation)
    { 
        //Notify user
        PopMessage("A simulation is currently underway. SolarPILOT cannot begin a new "
            "parametric simulation until the current task has completed.", "Error");
        return;
    }
    //Get the file locations and names
    string
        filedir = (string)_dir_ctrl->GetPath(),
        eff_file = filedir + "//" + _variables.par.eff_file_name.val,
        flux_file = filedir + "//" + _variables.par.flux_file_name.val;
    //Check the files for the correct extension
    if(ioutil::ext_only(eff_file) != "csv") eff_file.append(".csv");
    if(ioutil::ext_only(flux_file) != "csv") flux_file.append(".csv");

    wxProgressDialog *pdlg = new wxProgressDialog("Progress...", "", 1000, this, wxPD_SMOOTH|wxPD_AUTO_HIDE|wxPD_CAN_ABORT|wxPD_ELAPSED_TIME);
    pdlg->Show();

    AutoPilot_MT m_sapi;

    m_sapi.SetSummaryCallback( SAMParametricCallback, pdlg );

    sp_flux_table fluxtab;
    fluxtab.is_user_spacing = true;
    fluxtab.n_flux_days = 8;
    fluxtab.delta_flux_hrs = 2.;
    
    int nfx, nfy;
    switch(_variables.par.fluxmap_format.mapval() )
    {
    case var_parametric::FLUXMAP_FORMAT::SAM_FORMAT: //12x1
        nfx = 12;
        nfy = 1;
        break;
    case var_parametric::FLUXMAP_FORMAT::N12X10_ARRAY:
        nfx = 12;
        nfy = 10;
        break;
    case var_parametric::FLUXMAP_FORMAT::SPECIFIED_DIMENSIONS:
        nfx = _variables.flux.x_res.val;
        nfy = _variables.flux.y_res.val;
        break;
    default:
        throw spexception("Invalid fluxmap format flag in SAMInputParametric");
    };

    m_sapi.Setup(_variables);
    m_sapi.CalculateFluxMaps(fluxtab, nfx, nfy, _variables.par.is_fluxmap_norm.val);

    pdlg->Destroy();

    {
        std::ofstream file(eff_file, std::ofstream::out);
        if( !file.is_open() )
        {
            PopMessage("The field efficiency output file \""+eff_file+"\" failed to open. Simulation terminated.");
            return;
        }
        file.clear();

        for(int i=0; i<(int)fluxtab.efficiency.size(); i++)
        {
            file << (fluxtab.azimuths.at(i)*R2D - 180.) << "," << fluxtab.zeniths.at(i)*R2D << "," << fluxtab.efficiency.at(i) << "\n";
        }

        file.close();
    }

    {
        //flux

        std::ofstream file(flux_file, std::ofstream::out);
        if( !file.is_open() )
        {
            PopMessage("The receiver flux output file \""+flux_file+"\" failed to open. Simulation terminated.");
            return;
        }
        file.clear();

        block_t<double> *flux = &fluxtab.flux_surfaces.at(0).flux_data;
        int nc = flux->ncols();
        
        for(int i=0; i<(int)fluxtab.efficiency.size(); i++)
        {
            file << (fluxtab.azimuths.at(i)*R2D - 180.) << "," << fluxtab.zeniths.at(i)*R2D << ",";
            for(int j=0; j<(int)flux->nrows(); j++)
            {
                if( j > 0 )
                    file << ",,";

                for(int k=0; k<nc; k++)
                {
                    file << flux->at(j, k, i) << (k<nc-1 ? "," : "");
                }
                file << "\n";
            }
        }

        file.close();
    }


}

void SPFrame::SAMInputParametric2()
{
    /* 
    Generate the input required for SAM. Flux map and efficiency array.
    */

    //check whether a layout simulation is currently running. Both simulations cannot occur at the same time.
    if(_in_layout_simulation || _in_flux_simulation || _in_user_param_simulation || _in_optimize_simulation)
    { 
        //Notify user
        PopMessage("A simulation is currently underway. SolarPILOT cannot begin a new "
            "parametric simulation until the current task has completed.", "Error");
        return;
    }

    //Get the file locations and names
    string
        filedir = (string)_dir_ctrl->GetPath(),
        eff_file = filedir + "//" + _variables.par.eff_file_name.val,
        flux_file = filedir + "//" + _variables.par.flux_file_name.val;
    //Check the files for the correct extension
    if(ioutil::ext_only(eff_file) != "dat") eff_file.append(".dat");
    if(ioutil::ext_only(flux_file) != "csv") flux_file.append(".csv");
    
    //define simulation parameters.. Note this same structure is re-used for the flux simulations
    sim_params P;
    P.dni = _variables.sf.dni_des.val;
    P.is_layout = false;
    P.Patm = 1.;
    P.Simweight = 1.;
    P.Tamb = 25.;
    P.TOUweight = 1.;
    P.Vwind = 0.;
        
    bool is_sam_grid_auto = _SF.getVarMap()->par.sam_grid_format.val == "Auto spacing";
    if( ! is_sam_grid_auto )
    {
        //------ First run the efficiency file --------
        wxProgressDialog *pdlg = new wxProgressDialog("Progress...", "", 1000, this, wxPD_SMOOTH|wxPD_AUTO_HIDE|wxPD_CAN_ABORT|wxPD_ELAPSED_TIME);
        pdlg->Show();

        
        double *azen_tab;
        int nazen_tab;
        //set the solar positions to calculate
        double eff_az[] = {0.,  30.,  60.,  90., 120., 150., 180., 210., 240., 270., 300., 330.};
        double eff_zen[] = {0.50,   7.,  15.,  30.,  45.,  60.,  75.,  85.};
        int neff_az = 12;
        int neff_zen = 8;

        nazen_tab = neff_az * neff_zen;
        azen_tab = new double[ nazen_tab * 2];
        int k=0;
        for(int i=0; i<neff_az; i++)
        {
            for(int j=0; j<neff_zen; j++)
            {
                azen_tab[k++] = (eff_az[i]-180.)*D2R;
                azen_tab[k++] = eff_zen[j]*D2R;
            }
        }

    
        _results.clear();
        _results.resize(nazen_tab);
        string neff_tot_str = my_to_string(nazen_tab);
        k=0;
        for(int i=0; i<nazen_tab; i++)
        {

            _SF.Simulate(azen_tab[i*2], azen_tab[i*2+1], P);

            _results.at(k++).process_analytical_simulation(_SF, P, 2, &azen_tab[i*2]);    //TO DO: #2 is flux simulation. Add parametric-specific method
            
            string kstr = my_to_string(k+1);
            
            if(! pdlg->Update((int)( min( (double)(k+1)/(double)nazen_tab*1000., 1000.) ), 
                "Efficiency matrix: Simulation "+kstr+" of "+neff_tot_str) )
            {
                pdlg->Destroy();
                PopMessage("Simulation cancelled");
                return;
            }
            wxYieldIfNeeded();
        }
        delete [] azen_tab;

        //Write the results to the output file
        wxTextFile file(eff_file);
        if(! (file.Exists() ? file.Open() : file.Create() ) )
        {
            PopMessage("The field efficiency output file \""+eff_file+"\" failed to open. Simulation terminated.");
            pdlg->Destroy();
            return;
        }
        file.Clear();
        /*
        The efficiency array file contains an extra column for 360 degrees that duplicates the 0 degrees values. 
        This allows for easy interpolation by the heliostat field component. Implement that behavior here.
        */
        //Write angle header lines
        wxString pline;
        for(int i=0; i<neff_az; i++)
        { 
            pline.Append( wxString::Format("%7.2f,", eff_az[i]) ); 
        }
        pline.Append( wxString::Format("%7.2f,", 360.));
        file.AddLine( pline );
        pline.Clear();
        for(int i=0; i<neff_zen; i++)
        { 
            pline.Append( wxString::Format("%7.2f,", eff_zen[i])); 
        }
        file.AddLine( pline );
        //keep track of the efficiency values for easier writing in the array_view file
        matrix_t<double> eff_view(neff_zen, neff_az);
        //Write values
        k=0;
        for(int i=0; i<neff_az; i++)
        {
            for(int j=0; j<neff_zen; j++)
            {
                eff_view.at(j, i) = _results.at(k++).eff_total_sf.ave;
                file.AddLine( wxString::Format("%.5f", eff_view.at(j, i) ) );
            }
        }
        for(int j=0; j<neff_zen; j++)
            file.AddLine( wxString::Format("%.5f", eff_view.at(j, 0) ) );

        file.Write();
        file.Close();
        pdlg->Destroy();
        _results.clear();

        //We also want to write the "array view" file which provides an easier way to view the efficiency matrix
        wxTextFile afile(filedir + "/array_view.csv");
        if(! (afile.Exists() ? afile.Open() : afile.Create() ))
        {
            PopMessage("The field efficiency output file \""+afile.GetName()+"\" failed to open. Simulation terminated.");
            pdlg->Destroy();
            return;
        }
        pline.Clear();
        afile.Clear();
        //azimuth angles
        pline.Append( "," );
        for(int i=0; i<neff_az; i++)
            pline.Append( wxString::Format( "%.2f,", eff_az[i] ) );
        afile.AddLine(pline);
        for(int i=0; i<neff_zen; i++)
        {
            pline.Clear();
            pline.Append( wxString::Format( "%.2f,", eff_zen[i] ) );
            for(int j=0; j<neff_az; j++)
                pline.Append( wxString::Format( "%.5f,", eff_view.at(i, j) ) );
            afile.AddLine( pline );
        }
        afile.Write();
        afile.Close();

        //------ End of efficiency file ----------------------------------------
    }

    //--------- Generate the flux map file ---------------------------------
    //Check if there are multiple receivers. If so, prompt the user to select 
    //which receiver they would like to simulate
    Rvector rec_to_sim;
    if(_SF.getActiveReceiverCount() > 1)
    {
        rec_select_dialog *rdlg = new rec_select_dialog(this, wxID_ANY, wxT("Select receivers"), &_SF);
        int id = rdlg->ShowModal();
        if(id == wxID_OK)
        {
            rdlg->getSelectedReceivers(rec_to_sim);
        }
        else
        {
            return;    //Cancelled
        }
    }
    else
    {
        rec_to_sim.push_back(_SF.getReceivers()->at(0));    //Only receiver    
    }
    
    wxProgressDialog *fdlg = new wxProgressDialog("Progress..", "", 1000, this, wxPD_SMOOTH|wxPD_AUTO_HIDE|wxPD_CAN_ABORT|wxPD_ELAPSED_TIME);
    fdlg->SetMinSize(wxSize(300,250));
    fdlg->Show();

    //Get flags and settings
    int fluxmap_format = _variables.par.fluxmap_format.mapval();
    bool is_fluxmap_norm = _variables.par.is_fluxmap_norm.val; 
    //Shape the flux surface files to match
    if(fluxmap_format == 0 || fluxmap_format == 1)
    {    //SAM format || 12 x 10 (same thing)
        for(unsigned int i=0; i<rec_to_sim.size(); i++)
        {
            switch(rec_to_sim.at(i)->getGeometryType() )
            {
            case Receiver::REC_GEOM_TYPE::CYLINDRICAL_CLOSED:        //External cylindrical receiver
            case Receiver::REC_GEOM_TYPE::POLYGON_CLOSED:
                rec_to_sim.at(i)->DefineReceiverGeometry(12, 1);    //Flux map should by 12 x 1
                break;
            default:    //Flat plate receiver etc.
                rec_to_sim.at(i)->DefineReceiverGeometry(12, 10);
            };
        }
    }
    
    vector<int> uday;
    vector<vector<double> > utime;
    Ambient::calcSpacedDaysHours(_variables.amb.latitude.val, _variables.amb.longitude.val, _variables.amb.time_zone.val, 8, 2., utime, uday);

    int nflux_sim = 0;
    for(int i=0; i<(int)utime.size(); i++)
        nflux_sim += utime.at(i).size();

    //Arrays to keep track of input values
    vector<int>
        all_days,
        all_hours;
    vector<double>
        all_az,
        all_zen;

    
    _results.resize(nflux_sim);
    //From the day and time array, produce an azimuth/zenith array
    string nflux_tot_str = my_to_string(nflux_sim);
    int k=0;
    int nday = (int)uday.size();
    for(int i=0; i<nday; i++)
    {
        int nhour_day = (int)utime.at(i).size();
        for(int j=0; j<nhour_day; j++)
        {
            DateTime DT((double)uday.at(i), utime.at(i).at(j)+12.);
            double azzen[2];
            Ambient::calcSunPosition(_variables, DT, &azzen[0], &azzen[1]);
            
            //--- keep track of input values
            all_days.push_back(uday[i]);
            all_hours.push_back((int)Toolbox::round(utime.at(i).at(j)) );
            all_az.push_back(azzen[0] * R2D);
            all_zen.push_back(azzen[1] * R2D);

            _SF.Simulate(azzen[0]*R2D, azzen[1]*R2D, P);  //re-use P from above
            _SF.HermiteFluxSimulation( *_SF.getHeliostats() );
            _results.at(k).process_analytical_simulation(_SF, P, 2, azzen);    //TO DO: #2
            
            //Collect flux results here
            _results.at(k).process_flux( &_SF, is_fluxmap_norm);
            
            //
            k++;
            string kstr = my_to_string(k+1);
            if(! fdlg->Update((int)( min( (double)(k+1)/(double)nflux_sim*1000., 1000.) ), 
                "Flux maps: Simulation "+kstr+" of "+nflux_tot_str) )
            {
                fdlg->Destroy();
                PopMessage("Simulation cancelled");
                return;
            }
            wxYieldIfNeeded();
        }
    }

    //Write the results to the output file
    wxTextFile ffile(flux_file);
    if(! (ffile.Exists() ? ffile.Open() : ffile.Create() ) )
    {
        PopMessage(wxT("The flux map output file \""+flux_file+"\" failed to open. Please ensure that the file is not open or write-protected."),
            wxEmptyString, wxOK);
        return;
    }

    ffile.Clear();

    //Write header lines
    ffile.AddLine( wxString::Format("This file contains the receiver flux data. Generated with SolarPILOT v%s.\n",_software_version.c_str()) );
    ffile.AddLine( wxString::Format("Plant lat.:, %.1f,deg\n", _variables.amb.latitude.val) );
    int nall = _results.size();
    for(int i=0; i<nall; i++)
    {
        ffile.AddLine( wxString::Format("%5.1f,%5.1f,%5.1f,%5.1f,", (float)all_days.at(i), (float)all_hours.at(i), all_az.at(i), all_zen.at(i)));
    }
    //Write the flux maps
    string annot;
    if(is_fluxmap_norm)
    {
        annot = " Flux absorption factor [kWt absorbed per kWt delivered to receiver]";
    }
    else
    {
        annot = " Flux absorption [kWt/m2 delivered to the receiver]";
    }
    for(int i=0; i<nall; i++)
    {    //For each result..
        int nfs = _results.at(i).flux_surfaces.size();    //Number of receivers
        for(int infs=0; infs<nfs; infs++)
        {    //For each receiver..
            int nmap = _results.at(i).flux_surfaces.at(infs).size();        //Number of flux surfaces on this receiver
            for(int inmap=0; inmap<nmap; inmap++)
            {        //For each flux surface on this receiver..
                wxString maplab;
                maplab.Printf(wxT("Receiver name [%s] | Flux surface [%d]"), _results.at(i).receiver_names.at(infs).c_str(), inmap+1 );
                
                ffile.AddLine( wxString::Format(" Azimuth,Zenith,%s\n%5.1f,%5.1f\n%s", maplab.c_str(), all_az.at(i), all_zen.at(i), annot.c_str()));
                FluxSurface *fs = &_results.at(i).flux_surfaces.at(infs).at(inmap);
                int 
                    nflux_x = fs->getFluxNX(),
                    nflux_y = fs->getFluxNY();
                FluxGrid *fmap = _results.at(i).flux_surfaces.at(infs).at(inmap).getFluxMap(); 
                for(int kk=0; kk<nflux_y; kk++)
                {
                    wxString line;
                    line.Clear();
                    for(int j=0; j<nflux_x; j++)
                    {
                        line.Append( wxString::Format("%12.8f,", fmap->at(j).at(nflux_y-kk-1).flux) );
                    }
                    ffile.AddLine(line);
                }
                ffile.AddLine("\n");
            }
        }
    }
    ffile.Write();
    ffile.Close();

    if( is_sam_grid_auto )
    {
        //Write the results to the output file
        std::ofstream file(eff_file, std::ofstream::out);
        if( !file.is_open() )
        {
            PopMessage("The field efficiency output file \""+eff_file+"\" failed to open. Simulation terminated.");
            return;
        }
        file.clear();

        for(int i=0; i<nflux_sim; i++)
            file << all_az.at(i) << "," << all_zen.at(i) << "," << _results.at(i).eff_total_sf.ave << "\n";

        file.close();
        
    }

    _results.clear();
    fdlg->Destroy();

    PopMessage("Simulation complete!");
}

void SPFrame::ParametricSimulate( parametric &P )
{
    /* 
    Run a parametric simulation based on the contents of 'parametric' P.

    Regeneration of field layout is determined by a user setting.
    */
    
    //check whether any variables are marked for the layout option
    bool any_need_layout = false;
    for(int i=0; i<P.size(); i++)
    {
        if(P.at(i).layout_required)
        {
            any_need_layout = true;
            break;
        }
    }

    //Collect the linked variables
    vector<par_variable*> linked, unlinked;

    for(int i=0; i<P.size(); i++)
    {
        if(P.at(i).linked) 
            linked.push_back(&P.at(i));
        else 
            unlinked.push_back(&P.at(i));
    }

    //Do a final check on the linked parametric value count
    int nc0=-1;
    for(int i=0; i<(int)linked.size(); i++)
    {
        if(nc0<0)
        {
            nc0 = linked.at(i)->selections.size();
        }
        else
        {
            if(nc0 != (int)linked.at(i)->selections.size())
            {
                throw spexception("Linked parametric value count mismatch. Please ensure all linked variables have the same number of values to simulate.");
            }
        }
    }

    //get the information output settings
    bool save_summary = _variables.par.par_save_summary.val; //_par_save_system_summary->GetValue();
    bool save_field = _variables.par.par_save_helio.val; //_par_save_field_data->GetValue();
    bool save_field_img = _variables.par.par_save_field_img.val;
    bool save_flux_img = _variables.par.par_save_flux_img.val;
    bool save_flux_dat = _variables.par.par_save_flux_dat.val;

    bool any_linked = (int)linked.size() > 0;
    int link_offset = any_linked ? 1 : 0;

    //if the user wants to save the field, ask which options they would like
        //Prompt the user to specify what information they would like to export
    vector<bool> options;
    wxString delim, save_field_dir;
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
            15 | Annual power delivery
            16 | Annual total efficiency
        
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
    if(save_field || save_summary || save_field_img || save_flux_img || save_flux_dat)
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
    Hvector perf_helios;
    bool perf_helios_filtered = false;
    if( (!any_need_layout) && _flux_lc_check->GetValue() )
    {
        perf_helios_filtered = true;
        if(! GetSelectedHeliostats(perf_helios, true) ) return;
    }


    int nsim=0;
    //Linked parametric
    wxString simname;

    //create an index array to keep track of the index of all of the nested parametric loops
    int nvar = (int)unlinked.size()+link_offset;
    int *parlengths = new int[nvar];
    int *parind = new int[nvar];
    bool *parchanged = new bool[nvar];    //array indicating which variables have changed for this simulation


    //fill in the number of simulations required per parametric variable
    if(any_linked) parlengths[0] = (int)linked.at(0)->selections.size();
    int nsim_tot = 0;
    for(int i=0; i<nvar-link_offset; i++)
    {
        parlengths[i+link_offset] = (int)unlinked.at(i)->selections.size();
    }

    //Calculate the number of total simulations
    nsim_tot = parlengths[0];
    for(int i=1; i<nvar; i++)
    {
        nsim_tot *= parlengths[i];
    }

    //Notify the user if they are attempting a large number of runs
    if(nsim_tot > 500)
    {
        string sval = gui_util::FormatWithCommas(nsim_tot);
        int id = PopMessage( wxString::Format("SolarPILOT is attempting a large number of parametric runs (N = %s ). Continue?", sval.c_str()), "", wxYES_NO|wxICON_WARNING );
        if(id != wxID_YES) return;
    }
    else if(nsim_tot < 1)
    {
        throw spexception( "No data for the parametric simulation.\n\nPlease specify at least one variable for parameterization on the Parametric Simulation page." );
    }


    //loop through all linked
    _results.clear();
    try
    {
        _results.reserve(nsim_tot);
    }
    catch(...)
    {
        throw spexception( wxString::Format(
            "Memory error while allocating space for parametric results. This may be caused by too many parametric simulation runs.\n\n"
            "Contact %s for debugging help.", _contact_info) );
    }

    for(int i=0; i<(int)P.size(); i++)
        P.at(i).sim_values.Clear();


    interop::ticker_initialize(parind, nvar);
    bool sim_complete = false;
    wxYieldIfNeeded();

    //create a new variable map
    var_map varpar( _variables );
    
    while(! sim_complete)
    {
        //update the parametric gauge
        ParProgressUpdate(nsim, nsim_tot);

        //reset the simulation timer
        _sim_watch.Start();
        
        //set the linked values first
        for(int i=0; i<(int)linked.size(); i++)
        {
            std::string value = linked.at(i)->selections[parind[0]];

            varpar._varptrs.at( linked.at(i)->varname )->set_from_string( value.c_str() );
            linked.at(i)->sim_values.push_back(value);    //Keep track of the values used in each simulation
        }

        //set the unlinked values
        for(int i=0; i<(int)unlinked.size(); i++)
        {
            std::string value = unlinked.at(i)->selections[ parind[i+link_offset] ];
            
            varpar._varptrs.at( unlinked.at(i)->varname )->set_from_string( value.c_str() );
            unlinked.at(i)->sim_values.push_back(value);
        }

        //based on which parameters have changed, determine whether we need to layout the field or just regen geometry
        bool full_layout = false;    //flag indicating whether a full layout is required on this simulation set
        if(any_linked && parchanged[0])
        {    //linked values
            for(int i=0; i<(int)linked.size(); i++)
            {
                //any in the linked that require full layout?
                if(linked.at(i)->layout_required)
                {
                    full_layout = true;
                    break;
                }
            }
        }
        if(! full_layout)
        {
            //now check other parameters
            for(int i=0; i<(int)unlinked.size(); i++)
            {
                if(parchanged[i+link_offset])
                {
                    if(unlinked.at(i)->layout_required)
                    {
                        full_layout = true;
                        break;
                    }
                }
            }
        }
        
        _par_SF.getSimInfoObject()->setCallbackFunction( SolarFieldInfoCallback, this);

        bool sim_cancelled = false;
        
        try
        {

            //Run a check to see whether the sun position is above the horizon
            bool sunpos_ok = true;
            if (varpar.flux.flux_time_type.mapval() == var_fluxsim::FLUX_TIME_TYPE::SUN_POSITION) {	//Sun position specified
                if (varpar.flux.flux_solar_el.Val() < 0.)
                    sunpos_ok = false;
            }
            else {
                //day and time specified

                int flux_day = varpar.flux.flux_day.val;
                double flux_hour = varpar.flux.flux_hour.val;
                int flux_month = varpar.flux.flux_month.val;
                DateTime DT;
                Ambient::setDateTime(DT, flux_hour, DT.GetDayOfYear(2011, flux_month, flux_day));
                double az, zen;
                Ambient::calcSunPosition(varpar, DT, &az, &zen);
                if (zen > 90.)
                    sunpos_ok = false;
            }
            
            if(!sunpos_ok)
            {
            
                //push back an empty simulation result
                _results.push_back(sim_result());
                _results.back().zero();
        
                //reset the sim type to parametric
                _results.back().sim_type = 3;
            }
            else
            {        
                try
                {
                    if(_variables.amb.weather_file.val != varpar.amb.weather_file.val)
                    {
                        wxString v=(wxString)varpar.amb.weather_file.val;
                        UpdateClimateFile(v, varpar, true); 
                    }

                    //Update the design method box.. this actually updates both the map values and GUI. Probably should fix this sometime..
                    UpdateDesignSelect( varpar.sf.des_sim_detail.mapval(), varpar );

                    //Check the cancel status before starting up
                    if( _par_SF.CheckCancelStatus() ) break;

                    //Set up the solar field
                    _par_SF.Clean();
                    _par_SF.Create(varpar);
        
                    _par_SF.getSimInfoObject()->isEnabled(false);
                }
                catch(std::exception &e)
                {
                    throw spexception("Fatal error during field data structure setup: " + (string)e.what());
                }
                catch(...)
                {
                    throw spexception("Fatal error during field data structure setup.");
                }
        
                if(full_layout)
                {
                    /* 
                    Call the layout simulate method. This will automatically handle field geometry creation
                    and multi-threading.
                    */
                    try
                    {
                        sim_cancelled = sim_cancelled || !DoManagedLayout(_par_SF, varpar);        //Returns TRUE if successful
                    }
                    catch(std::exception &e)
                    {
                        throw spexception("Fatal error during field layout generation: " + (string)e.what());
                    }
                    catch(...)
                    {
                        throw spexception("Fatal error during field layout generation.");
                    }
                
                }
                else
                {
                    //just update the geometry
                    if(! varpar.sf.layout_data.val.empty() )
                    {
                        try
                        {
                            SolarField::PrepareFieldLayout(_par_SF, 0, true);    if(_par_SF.ErrCheck()) return;
                            _par_SF.calcHeliostatArea();
                            _par_SF.updateAllCalculatedParameters( varpar );

                            double azzen[2];
                            _par_SF.CalcDesignPtSunPosition(varpar.sf.sun_loc_des.mapval(), azzen[0], azzen[1]);
                            Vect sun = Ambient::calcSunVectorFromAzZen(azzen[0]*D2R, azzen[1]*D2R);

                            _par_SF.updateAllTrackVectors(sun);    if(_par_SF.ErrCheck()) return;
                        }
                        catch(std::exception &e)
                        {
                            throw spexception("Fatal error during field geometry updating: " + (string)e.what());
                        }
                        catch(...)
                        {
                            throw spexception("Fatal error during field geometry updating.");
                        }
                    }

                }

                //run performance simulation
                Hvector helios;
                if(perf_helios_filtered)
                {
                    //Map by ID to new hvector
                    for(int i=0; i<(int)perf_helios.size(); i++)
                    {
                        helios.push_back( (*_par_SF.getHeliostatsByID() )[ perf_helios.at(i)->getId() ] );
                    }
                }
                else
                {
                    //Get all of the heliostats
                    helios = *_par_SF.getHeliostats();
                }

                //check to see if the heliostat vector is empty
                if(helios.size() == 0)
                    throw spexception("Empty field layout - can't perform parametric performance simulation!");

                int n_old_result = _results.size();
                try
                {
                    _results.push_back(sim_result());
                }
                catch(...)
                {
                    throw spexception("Memory error creating results storage array");
                }
                try
                {
                    if(!sim_cancelled) sim_cancelled = sim_cancelled || !DoPerformanceSimulation(_par_SF, varpar, helios); //Returns TRUE if successful
                }
                catch(std::exception &e)
                {
                    throw spexception("Fatal error during performance simulation: " + (string)e.what());
                }
                catch(...)
                {
                    throw spexception("Fatal error during performance simulation.");
                }
        
                //reset the sim type to parametric
                int n_new_result = _results.size() - n_old_result;
                for(int i=n_old_result; i<_results.size(); i++)
                    _results.at(i).sim_type = 3;

                //update the layout data stored in the variable map
                try
                {
                    interop::UpdateMapLayoutData(varpar, &helios);
                }
                catch(std::exception &e)
                {
                    throw spexception("Fatal error during field layout logging: " + (string)e.what());
                }
                catch(...)
                {
                    throw spexception("Fatal error during field layout logging.");
                }

                //export performance data
                if(save_summary && !sim_cancelled)
                {
                    wxString fname;
                    fname.Printf("%s/param_perf-summary_%d.csv",save_field_dir.c_str(),nsim+1);
                    //open and write the file
                    wxTextFile fout(fname);
                    if(! (fout.Exists() ? fout.Open() : fout.Create()) )
                    {
                        wxString emsg = "Error opening the parametric output file \""+fname+"\". Terminating the simulation";
                        throw spexception(emsg.ToStdString());
                    }
                    try
                    {
                        fout.Clear();
                        for (int r = 0; r < n_new_result; r++)
                        {
                            grid_emulator gridtable;
                            CreateResultsTable(_results.at(n_old_result + r), gridtable);

                            //Write the table
                            wxArrayStr textresults;
                            gridtable.GetPrintableTable(textresults, "");

                            if (n_new_result > 1)
                                fout.AddLine(wxString::Format("Performance for:, %s", r==0 ? "All receivers" : _results.at(n_old_result+r).receiver_names.front()));

                            for (int j = 0; j < (int)unlinked.size(); j++)
                            {
                                fout.AddLine(wxString::Format("%s, %s", unlinked.at(j)->display_text.c_str(), unlinked.at(j)->sim_values.back().c_str()));
                            }
                            for (int j = 0; j < (int)linked.size(); j++)
                            {
                                fout.AddLine(wxString::Format("%s, %s", linked.at(j)->display_text.c_str(), linked.at(j)->sim_values.back().c_str()));
                            }
                            fout.AddLine(wxString::Format("Layout?, %s", full_layout ? "True" : "False"));

                            for (int j = 0; j < (int)textresults.size(); j++)
                                fout.AddLine(textresults[j]);
                        }
                    }
                    catch(spexception &err)
                    {
                        fout.AddLine(err.what());
                    }
                    fout.Write();
                    fout.Close();
                }
                //export layout data
                if(save_field && !sim_cancelled)
                {
                    wxString fname;
                    fname.Printf("%s/param_field-detail_%d.csv", save_field_dir.c_str(), nsim+1);
                    wxString header;
                    header.Append("Variable name, Variable ID, Value, Units\n");
                    for(int j=0; j<(int)linked.size(); j++)
                    {
                        wxString tstring;
                        tstring.Printf("%s, %s, %s, %s\n", 
                            linked.at(j)->display_text.c_str(), 
                            linked.at(j)->varname.c_str(),
                            linked.at(j)->selections[ parind[0] ].c_str(),
                            linked.at(j)->units.c_str());
                        header.Append(tstring);
                    }
                    for(int j=0; j<(int)unlinked.size(); j++)
                    {
                        wxString tstring;
                        tstring.Printf("%s, %s, %s, %s\n", 
                            unlinked.at(j)->display_text.c_str(), 
                            unlinked.at(j)->varname.c_str(),
                            unlinked.at(j)->selections[ parind[j+link_offset] ].c_str(),
                            unlinked.at(j)->units.c_str());
                        header.Append(tstring);
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
                        fname.Printf("%s/param_field-plot_%d_%d.png", save_field_dir.c_str(), nsim+1, *pn);
                        wxClientDC pdc(this); 
                        _plot_frame->SetPlotData(_par_SF, *pn);
                        _plot_frame->SolarFieldAnnotation(&_par_SF, &_results.at(n_old_result), _plot_annot_selections);
                        std::string *annot = _plot_frame->getSolarFieldAnnotationObject();
                        if (std::find(_plot_annot_selections.begin(), _plot_annot_selections.end(), PlotSelectDialog::VARIABLES) != _plot_annot_selections.end())
                        {
                            for (int j = 0; j < (int)unlinked.size(); j++)
                            {
                                annot->append(wxString::Format("%s,%s;", unlinked.at(j)->display_text.c_str(), unlinked.at(j)->sim_values.back().c_str()));
                            }
                            for (int j = 0; j < (int)linked.size(); j++)
                            {
                                annot->append(wxString::Format("%s,%s;", linked.at(j)->display_text.c_str(), linked.at(j)->sim_values.back().c_str()));
                            }
                        }

                        _plot_frame->DoPaint(pdc);
                        wxBitmap *bitmap = _plot_frame->GetBitmap();
                        wxImage image = bitmap->ConvertToImage();
                        image.SaveFile( fname, wxBITMAP_TYPE_PNG );
                    }

                }
                //save flux image and data
                if((save_flux_img || save_flux_dat ) && !sim_cancelled)
                {
                    for (size_t i = 0; i < _rec_select->GetCount(); i++)
                    {
                        wxString recname = _rec_select->GetString(i);
                        wxClientDC pdc(this);
                        _flux_frame->SetPlotData(_par_SF, *_par_SF.getHeliostats(), i);
                        if (save_flux_img)
                        {
                            wxString fname;
                            fname.Printf("%s/param_flux-plot_%s_%d.png", save_field_dir.c_str(), recname.ToStdString().c_str(), nsim + 1);
                            _flux_frame->DoPaint(pdc);
                            wxBitmap *bitmap = _flux_frame->GetBitmap();
                            wxImage image = bitmap->ConvertToImage();
                            image.SaveFile(fname, wxBITMAP_TYPE_PNG);
                        }
                        if (save_flux_dat)
                        {
                            wxString fname;
                            fname.Printf("%s/param_flux-data_%s_%d.csv", save_field_dir.c_str(), recname.ToStdString().c_str(), nsim+1);
                            wxString cdelim = ",";
                            _flux_frame->SaveDataTable(fname, cdelim);
                        }
                    }
                }
                
                if(_par_SF.CheckCancelStatus() || sim_cancelled) break; //break if the simulation has been cancelled
            }
        }
        catch(std::exception &e)
        {
            wxArrayStr msgs;
            msgs.Add(e.what());
            //LogFileVariableDump(vset, msgs);
        }
        catch(...)
        {
            //LogFileVariableDump(vset);
        }

        nsim++;
        
        sim_complete = interop::ticker_increment(parlengths, parind, parchanged, nvar);
        
    }
    ParProgressUpdate(0,0);
    _par_single_gauge->SetValue(0);
    _par_single_text->SetLabel("Simulation Progress (0/0)");
    
    delete [] parchanged;
    delete [] parind;
    delete [] parlengths;
    
    

}

void SPFrame::CreateResultsTable(sim_result &result, grid_emulator &table)
{
    try
    {
		//table.CreateGrid(result.is_soltrace ? 18 : 19, 6);
		table.CreateGrid(18, 6);
        
        table.SetColLabelValue(0, "Units");
        table.SetColLabelValue(1, "Value");
        table.SetColLabelValue(2, "Mean");
        table.SetColLabelValue(3, "Minimum");
        table.SetColLabelValue(4, "Maximum");
        table.SetColLabelValue(5, "Std. dev");
    
        int id=0;
        table.AddRow(id++, "Total plant cost", "$", result.total_installed_cost, 0);
        //table.AddRow(id++, "Cost/Energy metric", "-", result.coe_metric);
        table.AddRow(id++, "Simulated heliostat area", "m^2", result.total_heliostat_area);
        table.AddRow(id++, "Simulated heliostat count", "-", result.num_heliostats_used, 0);
        table.AddRow(id++, "Power incident on field", "kW", result.power_on_field);
        table.AddRow(id++, "Power absorbed by the receiver", "kW", result.power_absorbed);
        table.AddRow(id++, "Power absorbed by HTF", "kW", result.power_to_htf);

        double nan = std::numeric_limits<double>::quiet_NaN();
        //------------------------------------
        if (result.is_soltrace)
        {
            table.AddRow(id++, "Cloudiness efficiency", "%", 100.*result.eff_cloud.wtmean, 2);
            table.AddRow(id++, "Shadowing and Cosine efficiency", "%", 100.*result.eff_cosine.wtmean, 2);
            table.AddRow(id++, "Reflection efficiency", "%", 100.*result.eff_reflect.wtmean, 2);
            table.AddRow(id++, "Blocking efficiency", "%", 100.*result.eff_blocking.wtmean, 2);
            table.AddRow(id++, "Image intercept efficiency", "%", 100.*result.eff_intercept.wtmean, 2);
            table.AddRow(id++, "Absorption efficiency", "%", 100.*result.eff_absorption.wtmean, 2);
            table.AddRow(id++, "Solar field optical efficiency", "%", 100.*result.eff_total_sf.wtmean / result.eff_absorption.wtmean, 2);
            table.AddRow(id++, "Optical efficiency incl. receiver", "%", 100.*result.eff_total_sf.wtmean, 2);
            table.AddRow(id++, "Incident flux", "kW/m2", result.flux_density.ave, -1, result.flux_density.min, result.flux_density.max, result.flux_density.stdev);
            table.AddRow(id++, "No. rays traced", "-", result.num_ray_traced, 0);
            table.AddRow(id++, "No. heliostat ray intersections", "-", result.num_ray_heliostat, 0);
            table.AddRow(id++, "No. receiver ray intersections", "-", result.num_ray_receiver, 0);
        }
        else
        {   //results table for hermite simulation
            table.AddRow(id++, "Cloudiness efficiency", "%", 
                    100.*result.eff_cloud.wtmean, 2,
                    100.*result.eff_cloud.ave, 
                    100.*result.eff_cloud.min, 
                    100.*result.eff_cloud.max, 
                    100.*result.eff_cloud.stdev);
            table.AddRow(id++, "Shading efficiency", "%",
                    100.*result.eff_shading.wtmean, 2,
                    100.*result.eff_shading.ave,
                    100.*result.eff_shading.min,
                    100.*result.eff_shading.max,
                    100.*result.eff_shading.stdev);
            table.AddRow(id++, "Cosine efficiency", "%",
                    100.*result.eff_cosine.wtmean, 2,
                    100.*result.eff_cosine.ave,
                    100.*result.eff_cosine.min,
                    100.*result.eff_cosine.max,
                    100.*result.eff_cosine.stdev);
			table.AddRow(id++, "Reflection efficiency", "%",
                    100.*result.eff_reflect.wtmean, 2,
                    100.*result.eff_reflect.ave, 
                    100.*result.eff_reflect.min, 
                    100.*result.eff_reflect.max, 
                    100.*result.eff_reflect.stdev);
            table.AddRow(id++, "Blocking efficiency", "%",
                    100.*result.eff_blocking.wtmean, 2,
                    100.*result.eff_blocking.ave,
                    100.*result.eff_blocking.min,
                    100.*result.eff_blocking.max,
                    100.*result.eff_blocking.stdev);
            table.AddRow(id++, "Attenuation efficiency", "%",
                    100.*result.eff_attenuation.wtmean, 2,
                    100.*result.eff_attenuation.ave,
                    100.*result.eff_attenuation.min,
                    100.*result.eff_attenuation.max,
                    100.*result.eff_attenuation.stdev);
            table.AddRow(id++, "Image intercept efficiency", "%",
                    100.*result.eff_intercept.wtmean, 2,
                    100.*result.eff_intercept.ave,
                    100.*result.eff_intercept.min,
                    100.*result.eff_intercept.max,
                    100.*result.eff_intercept.stdev);
            table.AddRow(id++, "Absorption efficiency", "%", 100.*result.eff_absorption.wtmean, 2);
            table.AddRow(id++, "Solar field optical efficiency", "%",
                    100.*result.eff_total_sf.wtmean /result.eff_absorption.wtmean, 2,
                    nan,
                    100.*result.eff_total_sf.min/result.eff_absorption.wtmean,
                    100.*result.eff_total_sf.max/result.eff_absorption.wtmean,
                    100.*result.eff_total_sf.stdev/result.eff_absorption.wtmean);
            table.AddRow(id++, "Optical efficiency incl. receiver", "%",
                    100.*result.eff_total_sf.wtmean, 2,
                    nan,
                    100.*result.eff_total_sf.min,
                    100.*result.eff_total_sf.max,
                    100.*result.eff_total_sf.stdev);
            table.AddRow(id++, "Annualized heliostat efficiency", "%",
                    100.*result.eff_annual.wtmean, 2,
                    nan,
                    100.*result.eff_annual.min,
                    100.*result.eff_annual.max,
                    100.*result.eff_annual.stdev);
            table.AddRow(id++, "Incident flux", "kW/m2",
                    result.flux_density.ave, -1,
                    nan, 
                    result.flux_density.min, 
                    result.flux_density.max, 
                    result.flux_density.stdev);
        }
    }
    catch(...)
    {
        throw spexception("An error occurred while trying to process the simulation results for display.");
    }
    
}

void SPFrame::CreateParametricsTable(parametric &par, sim_results &results, grid_emulator &table)
{
    //how many simulations are there?
    int nsim = results.size();
    int nrec = std::max( (int)results.front().receiver_names.size(), 1);
    //how many variables were parameterized?
    int nvar = par.size();

    table.CreateGrid(18+nvar+(nrec>1 ? 1 : 0), 1+nsim);
    table.SetColLabelValue(0, "Units");
    for(int i=0; i<nsim; i++)
    {
        wxString collab;
        collab.Printf("Simulation %d", i/(nrec+1)+1);
        table.SetColLabelValue(i+1, collab);
    }

    int id=0;
    for(int i=0; i<nvar; i++)
        table.SetRowLabelValue(id++, par.at(i).display_text);
    if(nrec > 1)
        table.SetRowLabelValue(id++, "Which receiver");
    table.SetRowLabelValue(id++, "Total plant cost");
    table.SetRowLabelValue(id++, "Cost/Energy metric");
    table.SetRowLabelValue(id++, "Simulated heliostat area");
    table.SetRowLabelValue(id++, "Simulated heliostat count");
    table.SetRowLabelValue(id++, "Power incident on field");
    table.SetRowLabelValue(id++, "Power absorbed by the receiver");
    table.SetRowLabelValue(id++, "Power absorbed by the HTF");
    table.SetRowLabelValue(id++, "Cloudiness efficiency");
    table.SetRowLabelValue(id++, "Shading efficiency");
    table.SetRowLabelValue(id++, "Cosine efficiency");
    table.SetRowLabelValue(id++, "Reflection efficiency");
    table.SetRowLabelValue(id++, "Blocking efficiency");
    table.SetRowLabelValue(id++, "Attenuation efficiency");
    table.SetRowLabelValue(id++, "Image intercept efficiency");
    table.SetRowLabelValue(id++, "Absorption efficiency");
    table.SetRowLabelValue(id++, "Solar field optical efficiency");
    table.SetRowLabelValue(id++, "Average incident flux");
    table.SetRowLabelValue(id++, "Peak incident flux");

    //Units
    id=0;
    for(int i=0; i<nvar; i++)
        table.SetCellValue(par.at(i).units, id++, 0);
    if (nrec > 1)
        table.SetCellValue("-", id++, 0);
    table.SetCellValue("$", id++, 0);
    table.SetCellValue("-", id++, 0);
    table.SetCellValue("m^2", id++, 0);
    table.SetCellValue(" - ", id++, 0);
    table.SetCellValue("kW", id++, 0);
    table.SetCellValue("kW", id++, 0);
    table.SetCellValue("kW", id++, 0);
    table.SetCellValue("%", id++, 0);
    table.SetCellValue("%", id++, 0);
    table.SetCellValue("%", id++, 0);
    table.SetCellValue("%", id++, 0);
    table.SetCellValue("%", id++, 0);
    table.SetCellValue("%", id++, 0);
    table.SetCellValue("%", id++, 0);
    table.SetCellValue("%", id++, 0);
    table.SetCellValue("%", id++, 0);
    table.SetCellValue("kW/m2", id++, 0);
    table.SetCellValue("kW/m2", id++, 0);
    
    //Values
    for(int i=0; i<nsim; i++)
    {
        sim_result *result = &results.at(i);
        id=0;

        for(int j=0; j<nvar; j++)
        {
            int iadj = i;
            if(nrec > 1 ) 
                iadj /= (nrec + 1);

            if(par.at(j).data_type == "location")
            {
                table.SetCellValue(ioutil::name_only( par.at(j).sim_values[iadj] ), id++, i+1);
            }
            else
            { 
                table.SetCellValue(par.at(j).sim_values[iadj], id++, i+1);
            }
        }
        if (nrec > 1)
            table.SetCellValue(i%(nrec+1)==0 ? "All receivers" : result->receiver_names.front(), id++, i + 1);
        table.SetCellValue(gui_util::FormatAsCurrency( result->total_installed_cost ), id++, i+1);
        table.SetCellValue(to_string(result->coe_metric, "%.3f"), id++, i+1);
        table.SetCellValue(to_string(result->total_heliostat_area, "%.1f"), id++, i+1);
        table.SetCellValue(to_string(result->num_heliostats_used, "%d"), id++, i+1);
        table.SetCellValue(to_string(result->power_on_field, "%.1f"), id++, i+1);
        table.SetCellValue(to_string(result->power_absorbed, "%.1f"), id++, i+1);
        table.SetCellValue(to_string(result->power_to_htf, "%.1f"), id++, i+1);
        table.SetCellValue(to_string(100.*result->eff_cloud.wtmean, "%.1f"), id++, i+1);
        table.SetCellValue(to_string(100.*(result->is_soltrace ? 1. : result->eff_shading.wtmean), "%.1f"), id++, i+1);
        table.SetCellValue(to_string(100.*result->eff_cosine.wtmean, "%.1f"), id++, i+1);
        table.SetCellValue(to_string(100.*result->eff_reflect.wtmean, "%.1f"), id++, i+1);
        table.SetCellValue(to_string(100.*(result->is_soltrace ? 1. : result->eff_blocking.wtmean), "%.1f"), id++, i+1);
        table.SetCellValue(to_string(100.*result->eff_attenuation.wtmean, "%.1f"), id++, i+1);
        table.SetCellValue(to_string(100.*result->eff_intercept.wtmean, "%.1f"), id++, i+1);
        table.SetCellValue(to_string(100.*result->eff_absorption.wtmean, "%.1f"), id++, i+1);
        table.SetCellValue(to_string(100.*result->eff_total_sf.wtmean, "%.1f"), id++, i+1);
        table.SetCellValue(to_string(result->flux_density.ave, "%.1f"), id++, i+1);
        table.SetCellValue(to_string(result->flux_density.max, "%.1f"), id++, i+1);
    }
}

bool SPFrame::DoPerformanceSimulation( SolarField &SF, var_map &vset, Hvector &helios)
{
    
    //Which type of simulation is this?
    int simtype = vset.flux.flux_model.mapval();    //0=Delsol, 1=Soltrace
    
    //Set up field, update aimpoints, and simulate at the performance sun position
    if(! interop::PerformanceSimulationPrep(SF, helios, simtype) ) 
        return false;
    
    Vect sun = Ambient::calcSunVectorFromAzZen( SF.getVarMap()->flux.flux_solar_az.Val() * D2R, (90. - SF.getVarMap()->flux.flux_solar_el.Val())*D2R );
    
    SF.calcHeliostatShadows(sun);    
    if(SF.ErrCheck())
        return false;
    
    //Which type of simulation?
    switch(simtype)
    {
    case var_fluxsim::FLUX_MODEL::HERMITE_ANALYTICAL:
        return HermiteFluxSimulationHandler(SF, helios);
    case var_fluxsim::FLUX_MODEL::SOLTRACE:
        return SolTraceFluxSimulation(SF, vset, helios);
    default:
        return false;
    }
    
}

bool SPFrame::DoManagedLayout(SolarField &SF, var_map &V)
{
    /* 
    This method is called to create a field layout. The method automatically handles
    multithreading of hourly simulations.

    Call
    SF.Create()
    before passing the solar field to this method.
    
    */
    
    //Make sure the solar field has been created
    if( SF.getVarMap() == 0 )
    {
        PopMessage("The solar field Create() method must be called before generating the field layout.","Error");        
        return false;
    }
    
    //Is it possible to run a multithreaded simulation?
    int nsim_req = SF.calcNumRequiredSimulations();
    if(_n_threads > 1 && nsim_req > 1)
    {
        //More than 1 thread and more than 1 simulation to run

        //Prepare the master solar field object for layout simulation
        WeatherData wdata;
        bool full_sim = SF.PrepareFieldLayout(SF, &wdata);
        
        //If full simulation is required...
        if(full_sim)
        {

            int nthreads = min(nsim_req, _n_threads);

            //Duplicate SF objects in memory
            
            wxString msg;
            msg.Printf("Preparing %d threads for simulation", _n_threads);
            _layout_log->AppendText(msg);
            SolarField **SFarr;
            SFarr = new SolarField*[nthreads];
            for(int i=0; i<nthreads; i++)
            {
                SFarr[i] = new SolarField(SF);
            }
            
            //Create sufficient results arrays in memory
            sim_results results;
            results.resize(nsim_req);
                        
            //Calculate the number of simulations per thread
            int npert = (int)ceil((float)nsim_req/(float)nthreads);

            //Create thread objects
            _simthread = new LayoutSimThread[nthreads];
            _n_threads_active = nthreads;    //Keep track of how many threads are active
            _is_mt_simulation = true;
            
            int
                sim_first = 0,
                sim_last = npert;
            for(int i=0; i<nthreads; i++)
            {
                std::string si = my_to_string(i+1);
                _simthread[i].Setup(si, SFarr[i], &results, &wdata, sim_first, sim_last, false, false);
                sim_first = sim_last;
                sim_last = min(sim_last+npert, nsim_req);
            }
            
            msg.Printf("\n%d ms | Simulating %d design hours", _sim_watch.Time(), nsim_req);
            _layout_log->AppendText(msg);
            //Run
            for(int i=0; i<nthreads; i++)
                thread( &LayoutSimThread::StartThread, std::ref( _simthread[i] ) ).detach();
            

            //Wait loop
            while(true)
            {
                int nsim_done = 0, nsim_remain=0, nthread_done=0;
                for(int i=0; i<nthreads; i++)
                {
                    if( _simthread[i].IsFinished() )
                        nthread_done ++;
                    
                    int ns, nr;
                    _simthread[i].GetStatus(&ns, &nr);
                    nsim_done += ns;
                    nsim_remain += nr;
                    
                    
                }
                SimProgressUpdateMT(nsim_done, nsim_req);
                if(nthread_done == nthreads) break;
                std::this_thread::sleep_for(std::chrono::milliseconds(75));

            }

            //Check to see whether the simulation was cancelled
            bool cancelled = false;
            for(int i=0; i<nthreads; i++)
            {
                cancelled = cancelled || _simthread[i].IsSimulationCancelled();
            }
            
            //check to see whether simulation errored out
            bool errored_out = false;
            for(int i=0; i<_n_threads; i++)
            {
                errored_out = errored_out || _simthread[i].IsFinishedWithErrors();
            }
            if( errored_out )
            {
                //make sure each thread is cancelled
                for(int i=0; i<_n_threads; i++)
                    _simthread[i].CancelSimulation();

                //Get the error messages, if any
                string errmsgs;
                for(int i=0; i<_n_threads; i++)
                {
                    for(int j=0; j<(int)_simthread[i].GetSimMessages()->size(); j++)
                        errmsgs.append( _simthread[i].GetSimMessages()->at(j) + "\n");
                }
                //Display error messages
                if(! errmsgs.empty() )
                    PopMessage( errmsgs, "SolarPILOT Simulation Error", wxOK|wxICON_ERROR );
            
            }

            //Clean up dynamic memory
            for(int i=0; i<nthreads; i++)
            {
                delete SFarr[i];
            }
            delete [] SFarr;
            delete [] _simthread;
            _simthread = 0;

            //If the simulation was cancelled per the check above, exit out
            if(cancelled || errored_out)
            {
                return false;
            }
            
            //For the map-to-annual case, run a simulation here
            if(V.sf.des_sim_detail.mapval() == var_solarfield::DES_SIM_DETAIL::EFFICIENCY_MAP__ANNUAL)                
                SolarField::AnnualEfficiencySimulation(V.amb.weather_file.val, &SF, results); 

            //Process the results
            SF.ProcessLayoutResults(&results, nsim_req);

        }
    }
    else
    {
        _n_threads_active = 1;
        _is_mt_simulation = false;
        bool simok = SF.FieldLayout();            
        if(SF.ErrCheck() || !simok) return false;
    }

    //follow-on stuff
    Vect sun = Ambient::calcSunVectorFromAzZen( SF.getVarMap()->sf.sun_az_des.Val()*D2R, (90. - SF.getVarMap()->sf.sun_el_des.Val())*D2R );   
    SF.calcHeliostatShadows(sun);    if(SF.ErrCheck()) return false;
    V.land.bound_area.Setval( SF.getLandObject()->getLandBoundArea() ); 
        
    return true;

}

wxGauge *SPFrame::chooseProgressGauge()
{
    
    //Choose which gauge to update
    if(_in_param_simulation)
    {
        return _par_single_gauge;
    }
    else if(_in_user_param_simulation)
    {
        return _user_single_gauge;
    }
    else
    {
        if(_in_layout_simulation)
        {
            return _layout_gauge;
        }
        else
        {    
            return _flux_gauge;
        }
    }
    
}

wxStaticText *SPFrame::chooseProgressText()
{
    //Choose which text label to update
    if(_in_param_simulation)
    {
        return _par_single_text;
    }
    else if(_in_user_param_simulation)
    {
        return _user_single_text;
    }
    else
    {
        if(_in_layout_simulation)
        {
            return _layout_remain;
        }
        else
        {    
            return _flux_progress;
        }
    }
}

/* 
--------------------------------------------------------------------------------------

                            Event handlers

--------------------------------------------------------------------------------------
*/

void SPFrame::OnFocusEvent( wxFocusEvent &event)
{
    try
    {
        InputControl* eobj = (InputControl*)event.GetEventObject();
        if( eobj->needUpdate() )
        { 
            UpdateCalculatedGUIValues();
        }
        //-- Special handlers -- 
    
        //If the minrays value has changed on the flux simulations page, auto-update the max rays value
        if( eobj->getVarPath() == "fluxsim.0.min_rays")
        {
            int min_rays; 
            to_integer( eobj->getValue().ToStdString(), &min_rays);
            std::string srays = my_to_string(min_rays * 100);
            _input_map[ &_variables.flux.max_rays ]->setValue( srays );
            _variables.flux.max_rays.set_from_string( srays.c_str() );
        }

        event.Skip();
    }
    catch( std::exception &e )
    {
        PopMessage(e.what(), "OnFocusEvent Error", wxICON_ERROR|wxOK);
    }
    catch(...)
    {
        PopMessage("An unhandled error has occurred and the action could not be completed.", "OnFocusEvent Error", wxICON_ERROR|wxOK);
        return;
    }
}

void SPFrame::OnComboEvent( wxCommandEvent &event)
{
    try
    {
        //A combobox has been changed, update the interface
        UpdateCalculatedGUIValues();
        event.Skip();
    }
    catch( std::exception &e )
    {
        PopMessage(e.what(), "OnComboEvent Error", wxICON_ERROR|wxOK);
    }
    catch(...)
    {
        PopMessage("An unhandled error has occurred and the action could not be completed.", "OnComboEvent Error", wxICON_ERROR|wxOK);
        return;
    }
}

void SPFrame::OnPlotFontIncrease( wxCommandEvent &event)
{
    wxString name = ((wxFrame*)(event.GetEventObject()))->GetName();
    PlotFontIncrease( name );
}

void SPFrame::PlotFontIncrease( wxString type )
{
    
    if(type == "layout")
    {
        //Increase the font size of the plot
        int fsize0 = _plot_frame->GetFontSize();
        if(fsize0 > 47)
        {
            _plot_frame->SetFontSize(48);
            return;
        }
        int fsize = fsize0;
        if(fsize0 < 12)
        {
            fsize += 1;
        }
        else if(fsize0 < 28)
        {
            fsize += 2;
        }
        else
        {
            fsize += 6;
        }

        _plot_frame->SetFontSize(fsize);
    }
    else
    {
        //Increase the font size of the plot
        int fsize0 = _flux_frame->GetFontSize();
        if(fsize0 > 47)
        {
            _flux_frame->SetFontSize(48);
            return;
        }
        int fsize = fsize0;
        if(fsize0 < 12)
        {
            fsize += 1;
        }
        else if(fsize0 < 28)
        {
            fsize += 2;
        }
        else
        {
            fsize += 6;
        }

        _flux_frame->SetFontSize(fsize);
    }
    this->Update();
    this->Refresh();

}

void SPFrame::OnPlotFontDecrease( wxCommandEvent &event)
{
    wxString name = ((wxWindow*)(event.GetEventObject()))->GetName();
    PlotFontDecrease( name );
}

void SPFrame::PlotFontDecrease( wxString type )
{
    //Decrease the font size of the plot
    if(type == "layout")
    {
        int fsize0 = _plot_frame->GetFontSize();
        if(fsize0 < 3)
        {
            _plot_frame->SetFontSize(3);
            return;
        }
        int fsize = fsize0;
        if(fsize0 < 13)
        {
            fsize += -1;
        }
        else if(fsize0 < 29)
        {
            fsize += -2;
        }
        else
        {
            fsize += -6;
        }

        _plot_frame->SetFontSize(fsize);
    }
    else
    {
        int fsize0 = _flux_frame->GetFontSize();
        if(fsize0 < 3)
        {
            _flux_frame->SetFontSize(3);
            return;
        }
        int fsize = fsize0;
        if(fsize0 < 13)
        {
            fsize += -1;
        }
        else if(fsize0 < 29)
        {
            fsize += -2;
        }
        else
        {
            fsize += -6;
        }

        _flux_frame->SetFontSize(fsize);
    }
    this->Update();
    this->Refresh();
}

void SPFrame::LogFileVariableDump(var_map &vset)
{
    wxArrayStr empty;
    LogFileVariableDump(vset, empty);
}

void SPFrame::LogFileVariableDump(var_map &vset, wxArrayStr &messages)
{
    wxTextFile flog(_working_dir.GetPath(true) + "variable_log.csv");
    flog.Exists() ? flog.Open() : flog.Create();
    //flog.Clear();
    for(int i=0; i<(int)messages.size(); i++)
        flog.AddLine(messages[i]);

    WriteVariablesToFile(flog, vset);
    flog.AddLine("-------------------------------------------------------------------------");
    flog.Write();
    flog.Close();
}

void SPFrame::WriteVariablesToFile(wxTextFile &tfile, var_map &vset)
{
        
    //Report input variables for simulation
    tfile.AddLine("\nInput Variable Summary\nClass (inst),Variable,Units,Value");
    tfile.AddLine(wxString::Format( "Number of rays,,%d", vset.flux.min_rays.val) );

    for( unordered_map<string, spbase*>::iterator var=vset._varptrs.begin(); var != vset._varptrs.end(); var++)
    {
        string svar = var->second->as_string();
        if( (int)svar.size() > 25 ) continue;       //don't write long variable values
        if( svar.empty() ) continue;

        tfile.AddLine( wxString::Format("%s,%s,%s,%s", var->first.c_str(), var->second->short_desc.c_str(), var->second->units.c_str(), svar.c_str()) );
    }
}

/* ----------------------------------

Callbacks

-------------------------------------*/

int STCallback(st_uint_t ntracedtotal, st_uint_t ntraced, st_uint_t ntotrace, st_uint_t curstage, st_uint_t nstages, void *data)
{
	SPFrame *frame = static_cast<SPFrame*>(data);
	if (frame != NULL) return frame->SolTraceProgressUpdate(ntracedtotal, ntraced, ntotrace, curstage, nstages, data);
	else return 0;
};

bool SolarFieldInfoCallback(simulation_info *siminfo, void *data)
{
	SPFrame *frame = static_cast<SPFrame*>(data);
	if (frame != NULL) frame->SimProgressUpdate(siminfo);
	return true;
};

void SolarFieldErrorCallback(simulation_error *simerror, void *data)
{
	SPFrame *frame = static_cast<SPFrame*>(data);
	if (frame != NULL) frame->SimErrorHandler(simerror);
};

bool SolarFieldOptimizeSummaryCallback(simulation_info *siminfo, void *data)
{
	SPFrame *frame = static_cast<SPFrame*>(data);
	if (frame != NULL) frame->OptimizeProgressSummaryUpdate(siminfo);
	return true;
};

bool SolarFieldOptimizeDetailCallback(simulation_info *siminfo, void *data)
{
	SPFrame *frame = static_cast<SPFrame*>(data);
	if (frame != NULL) frame->OptimizeProgressDetailUpdate(siminfo);
	return true;
};