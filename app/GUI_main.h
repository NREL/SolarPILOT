
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


#ifndef _P_GUI_
#define _P_GUI_ 1

#define _USE_WINDOW_BG    0    //Flag to indicate whether we should use the window BG. The wxNotebook has some trouble with color inheritance

#include <map>
#include <wx/wx.h>
#include <wx/statline.h>
#include <wx/imaglist.h>
#include <wx/artprov.h>
#include <wx/spinctrl.h>
#include <wx/grid.h>
#include <wx/defs.h>
#include <wx/notebook.h>
#include <wx/listctrl.h>
#include <wx/bitmap.h>
#include <wx/splash.h>
#include <wx/gauge.h>
#include <wx/stdpaths.h>
#include <wx/clipbrd.h>
#include <wx/filepicker.h>
#include <wx/textfile.h>
#include <wx/filename.h>

#include "string_util.h"
#include "interop.h"
#include "gui_util.h"
#include "parse_kml.h"
#include "solpos00.h"
#include "sort_method.h"
#include "Toolbox.h"
#include "mod_base.h"    //Use for variable structures
#include "InputControl.h"
#include "OutputControl.h"
#include "rename_dialog.h"
#include "rec_select_dialog.h"
#include "layout_sim_export_dialog.h"
#include "FieldPlot.h"
#include "FluxPlot.h"
#include "gui_about_dialog.h"
#include "kml_select_dialog.h"
#include "ImageCanvas.h"
#include "settings_dialog.h"
#include "param_dialog.h"
#include "pagepanel.h"
#include <wex/diurnal.h>

#include "definitions.h"

#include <wex/lkscript.h>

//menu id's
enum{
    ID_FILE_NEW=5000,
    ID_FILE_OPEN,
    ID_FILE_SCRIPT,
    ID_FILE_SCRIPT_OPEN,
    ID_FILE_RECENT,
    ID_FILE_RECENT_LOAD=5050,
    ID_FILE_SAVE,
    ID_FILE_SAVEAS,
    ID_FILE_EXIT,

    ID_TOOLS_SETTINGS,

    ID_HELP_ABOUT,
    ID_HELP_CONTENTS,
    ID_HELP_LICENSE,

    ID_RUN_LAYOUT,
    ID_RUN_REFRESH,
    ID_RUN_SIMULATE,
    ID_RUN_PARAMETRIC,
    ID_RUN_USERPARAMETRIC,
    ID_RUN_OPTIMIZE,

    ID_SIM_TIMER,
    ID_SPFRAME_WINDOW
};

//Icon id's
enum{
    //Sidebar icons
    ID_ICON_MAIN = 0,
    ID_ICON_CLIMATE,
    ID_ICON_FINANCE,
    ID_ICON_COST,
    ID_ICON_LAYOUTCONFIG,
    ID_ICON_LAYOUT,
    ID_ICON_LAYOUTCALC,
    ID_ICON_HELIOSTAT_FOLDER,
    ID_ICON_HELIOSTAT,
    ID_ICON_RECEIVER_FOLDER,
    ID_ICON_RECEIVER,
    ID_ICON_RESULTS,
    ID_ICON_SOLTRACE,
    ID_ICON_FLUX,
    ID_ICON_TABLE,
    ID_ICON_GEARS,
    ID_ICON_SIMULATIONS,
    ID_ICON_SIMULATIONS32,
    ID_ICON_OPTIMIZE,

    //other icons - correspond to image list position, not the wxART_<> values
    ID_ICON_INFORMATION,
    ID_ICON_QUESTION,
    ID_ICON_WARNING,
    ID_ICON_ERROR
};

class AutoPilot_S;
class AutoPilot_MT;
class LayoutSimThread;
class STSimThread;
class STSimThread;
class SolarPILOTScriptWindow;

struct ST_FluxObj;
struct ST_System;

typedef std::map<spbase*, InputControl*> input_map;
typedef std::map<spbase*, OutputControl*> output_map;

std::string to_string(wxString &wxs);
std::string to_varpath(std::string &group, int &ID, std::string &name);
std::string to_varpath(std::string &group, std::string &ID, std::string &name);
std::string to_varpath(char group[], int &ID, char name[]);
std::string to_varpath(char group[], char ID[], char name[]);
std::string to_varpath(char group[], std::string &ID, char name[]);


struct PageNames
{
    wxString
        climate,
        markets,
        costs,
        layout_control,
        heliostat_master,
        receiver_master,
        simulations,
        simulations_layout,
        simulations_performance,
        simulations_parametrics,
        simulations_optimization,
        results,
        results_layout,
        results_flux,
        results_summary;
};


//---------------------------------------
class SPFrame : public wxFrame
{

private:
    gui_settings _gui_settings;
    PageNames pageNames;
    //Default themes and sizings
    int _tool_tip_delay, _n_threads, _n_threads_active, _nrecent_max, _trial_days;
    wxDateTime _trial_start;

	int 
		_version_major,
		_version_minor,
		_version_patch;
	std::string
		_software_version;

    wxColour
        _helptext_colour,
        _background_colour,
        _colorTextBG,
        _colorTextFG;
    wxSize
        _default_input_size,
        _spin_ctrl_size,
        _grid_but_size;
    bool 
        _geom_modified,    //Flag indicating that the geometry displayed in the layout grid has not been updated
        _inputs_modified,    //Flag indicating whether any (relevant) input value has changed, requiring a File->Save prompt
        _in_layout_simulation,    //Flag indicating whether a layout simulation is currently running
        _in_flux_simulation,    //Flag indicating whether a flux simulation is currently running
        _in_optimize_simulation,    //Flag indicating whether an optimization simulation is currently running
        _in_param_simulation,    //Flag indicating whether a parametric simulation is currently running
        _in_user_param_simulation, //Flag indicating whether a user-parametric simulation is currently running
        _is_mt_simulation,    //Is the current simulation multithreaded?
        _cancel_simulation; //Flag indicating whether the user wants to cancel the simulation
    std::string 
        _contact_info;    //String containing contact info for bugs, etc. Used in crash messages.
        
    wxFileName
        _recent_file_list,    //File containing the list of recent files
        _settings_file,
        _working_dir, //The working directory
        _climate_dir,    //Directory with the climate files
        _install_dir, //the head installation directory
        _image_dir, //the directory containing resource images
        _local_dir,    //The location to store edited app files
        _help_dir,    //the location of the help directory
        _open_file,    //The currently opened file
        _backup_file, //name of the backup file
        _sim_log_file;    //Log file with notices, errors, etc

    std::vector<std::string> _sim_log_data;    //All data to be written to the log file at the end of the simulation

    PagePanel *_page_panel;

    var_map _variables;
    input_map _input_map;
    output_map _output_map;
    SolarField _SF;
    wxImageList *_imageList;
    wxGrid *_bounds_grid;
    wxSpinCtrl *_bounds_ct;

    wxTimer *_sim_timer;
    wxStopWatch _sim_watch;

    //menus
    wxMenu
        *recentMenu,
        *menuFile,
        *menuHelp,
        *menuTools,
        *menuRun;

    //overall
    wxBoxSizer *_main_sizer;

    //Climate page
    wxListCtrl *_climate_ctrl;
    wxTextCtrl *_climate_search;
    OutputControl 
        *_loc_city,
        *_loc_state,
        *_loc_timezone,
        *_loc_elevation,
        *_loc_latitude,
        *_loc_longitude;
    ArrayString
        _all_climate_files,
        _climate_files,
        _local_wfdat;    //A local variable to store the broken out weather file data
                        //"[P]day,hour,month,DNI,Tdb,Pamb,Vwind[P]..."
    wxSpinCtrl *_user_sun_ct;
    wxGrid *_user_sun_grid;
    std::vector<wxTextCtrl*> _atm_coefs;
    wxPanel *_atm_panel;

    //Objects for the master heliostat page
    wxButton *_heliotemp_add;
    wxButton *_heliotemp_del;
    wxButton *_heliotemp_state;
    wxButton *_heliotemp_rename;
    wxListCtrl *_heliostat_config;
    wxGrid *_temp_bounds;

    //Objects for the master receiver page
    wxButton
        *_rec_add,
        *_rec_del,
        *_rec_duplicate,
        *_rec_state,
        *_rec_rename;
    wxListCtrl *_rec_config;
    std::vector<wxPanel*> _rec_pages;    //pointers to all of the receiver pages
    InputControl *_rec_which;    //Combo list of available receivers
    std::map<std::string, std::vector<wxTextCtrl*> > _hll_coefs, _hlw_coefs;
    wxGrid *_rec_power_fractions;
    wxStaticText *_msg_rec_power_fractions;
    struct s_user_flux_objects
    {
        wxSpinCtrl* nxptr;
        wxSpinCtrl* nyptr;
        wxGrid* gridptr;
        s_user_flux_objects() {};
        s_user_flux_objects(wxGrid* g, wxSpinCtrl* y, wxSpinCtrl* x)
        {
            nxptr = x;
            nyptr = y;
            gridptr = g;
        };
    };
    std::map< int, s_user_flux_objects > _user_flux_objects;

    //Objects for Layout page
    wxGrid *_design_grid;
    wxSpinCtrl *_design_ct;
    InputControl *_design_combo;
    wxPanel *_design_combo_panel;
    wxCheckBox *_design_simallhrs;
    wxSpinCtrl 
        *_design_ndays,
        *_design_nhours; 
    wxStaticText 
        *_design_ndays_lab,
        *_design_nhours_lab;
    wxListCtrl *_design_days_display;
    wxStaticText 
        *_design_helptext,
        *_design_steptext;
    wxBitmap _simimages[4];    //images for simulation button {normal,hover,click,cancel}
    wxBitmapButton *_layout_button;

    //objects for the simulations pages
    wxNotebook *_simbook;
    wxCheckBox *_do_positions;
    wxCheckBox *_do_optimize;
    wxTextCtrl *_layout_log;
    wxStaticText 
        *_layout_elapsed,
        *_layout_remain;
    wxGauge *_layout_gauge;
    wxGrid *_layout_grid;
    wxSpinCtrl *_layout_ct;
    wxButton
        *_layout_import,
        *_layout_export,
        *_layout_delete,
        *_layout_insert;
    wxBitmapButton
        *_layout_refresh;
    ImageCanvas
        *_refresh_warning;
    wxDirPickerCtrl
        *_dir_ctrl;
    LayoutSimThread *_simthread;
    STSimThread *_stthread;

    //Flux page
    wxComboBox
        *_rec_select;
    FluxPlot
        *_flux_frame;
    MyToggleButton
        *_scatter_btn;

    wxListCtrl
        *_flux_lc;
    wxCheckBox
        *_flux_lc_check;
    Hvector
        _flcsort;
    wxGauge *_flux_gauge;
    wxStaticText
        *_flux_progress;
    wxBitmapButton *_flux_button;

    //optimize page
    wxGauge *_optimize_summary_gauge;
    wxGauge *_optimize_detail_gauge;
    wxTextCtrl *_optimize_log;
    wxStaticText
        *_optimize_summary_progress,
        *_optimize_detail_progress,
        *_optimize_elapsed;
    AutoPilot_MT
        *_SFopt_MT;
    AutoPilot_S
        *_SFopt_S;
    bool _enable_detail_update_gauge;
    wxBitmapButton *_optimize_button;
    std::vector<double> _best_opt_result;
    wxGrid *_opt_var_grid;
    optimization _opt_data;    //use the parametric structure because it's already been coded

    //Parametrics page
    wxPanel 
        *_sam_param_panel;
    wxBitmapButton
        *_sam_toggle_button,
        *_par_toggle_button;
    bool
        _sam_toggle_shown,
        _par_toggle_shown,
        _user_par_toggle_shown;
    wxBitmap 
        _toggle_expand,
        _toggle_collapse;
    //
    wxPanel
        *_par_panel;
    std::vector<int> _plot_export_selections;
    std::vector<int> _plot_annot_selections;
    wxButton
        *_par_add_var,
        *_par_remove_var,
        *_par_edit_var,
        *_par_edit_linkages;
    wxListBox
        *_par_variables,
        *_par_values;
    parametric
        _par_data;
    wxBitmapButton
        *_par_button;
    SolarField _par_SF;
    wxGauge
        *_par_single_gauge,
        *_par_all_gauge;
    wxStaticText
        *_par_single_text,
        *_par_all_text;
    //
    wxPanel 
        *_user_par_panel;
    wxBitmapButton
        *_user_par_button,
        *_user_par_toggle_button;
    wxGrid
        *_user_par_grid;
    wxSpinCtrl 
        *_user_var_ct,
        *_user_sim_ct;

    wxGauge
        *_user_single_gauge,
        *_user_all_gauge;
    wxStaticText
        *_user_single_text,
        *_user_all_text;
    simulation_table
        _user_sim_table;

    //Plotting and output pages
    wxComboBox
        *_plot_select;
    FieldPlot 
        *_plot_frame;
    MyToggleButton *_zoom_pan, *_zoom_rect, *_show_field_data;
    int _field_left_mouse_start[2];
    
    sim_results
        _results;
    wxGrid *_results_grid;
    ST_System *_STSim;

    SolarPILOTScriptWindow *_active_script_window;

    void CreateInputPages(wxWindow *parent, PagePanel *pagepanel);
    void SetCaseName(wxString case_name = _T("New Case"));
    void bindControls();
    void SetSimulationStatus(bool in_sim, bool &sim_type_flag, wxBitmapButton *button);
    void CreateClimatePage(wxScrolledWindow *page);
    void CreateMasterHeliostatPage(wxScrolledWindow *parent);
    void CreateHeliostatPage(wxScrolledWindow *parent, int id);
    void CreateMasterReceiverPage(wxScrolledWindow *parent);
    void CreateReceiverPage(wxScrolledWindow *parent, int id);
    void CreateMarketPage(wxScrolledWindow *parent);
    void CreateCostPage(wxScrolledWindow *parent);
    void CreateLayoutPage(wxScrolledWindow *parent);
    void CreateFieldPlotPage(wxScrolledWindow *parent, wxArrayStr &choices, int selection = 0);
    void CreateFluxPlotPage(wxScrolledWindow *parent);
    void CreateSimulationsPage(wxScrolledWindow *parent);
    void CreateSimulationsLayoutTab(wxScrolledWindow *parent);
    void CreateSimulationsFluxTab(wxScrolledWindow *parent);
    void CreateSimulationsParametricsTab(wxScrolledWindow *parent);
    void CreateResultsSummaryPage(wxScrolledWindow *parent);
    void CreateOptimizationPage(wxScrolledWindow *parent);
#if _SANDBOX == 1
    bool Sandbox();
#endif

protected:
    /* Event handlers should be protected, others can be public */
    void TempEscape( wxKeyEvent &event);
    void OnFileOpen( wxCommandEvent &event);
    void OnFileScript( wxCommandEvent &event);
    void OnFileScriptOpen( wxCommandEvent &event);
    void OnFileRecent( wxCommandEvent &event);
    void OnFileRecentLoad( wxCommandEvent &event);
    void OnFileSaveAs( wxCommandEvent &event);
    void OnFileSave( wxCommandEvent &event);
    void OnFileNew( wxCommandEvent &event);
    void OnClose( wxCloseEvent &event );
    void OnToolsSettings( wxCommandEvent &event);
    void OnHelpAbout( wxCommandEvent &event);
    void OnHelpContents( wxCommandEvent &event);
    void OnHelpLicense( wxCommandEvent &event);
    void OnMenuRunLayout( wxCommandEvent &event);
    void OnMenuRunSimulation( wxCommandEvent &event);
    void OnMenuRunRefresh( wxCommandEvent &event);
    void OnMenuRunParametric( wxCommandEvent &event);
    void OnMenuRunUserParametric( wxCommandEvent &event);
    void OnMenuRunOptimize( wxCommandEvent &event);

    void OnFocusEvent( wxFocusEvent &event);
    void OnComboEvent( wxCommandEvent &event);
    void OnSimTimerCount( wxTimerEvent &event);

    void OnDispatchGrid( wxCommandEvent &event);

    void OnBoundsImport( wxCommandEvent &event);
    void OnBoundsExport( wxCommandEvent &event);
    void OnBoundsCount( wxCommandEvent &event);
    void OnBoundsEdit( wxGridEvent &event);
    void OnDesignEdit( wxGridEvent &event);
    void OnDesignSelect( wxCommandEvent &event);
    void OnDesignSimhrsChanged( wxCommandEvent &event);
    void OnDesignNdaysChanged( wxCommandEvent &event);
    void OnDesignNhrsChanged( wxCommandEvent &event);
    
    void OnChangeClimateFile( wxListEvent &event);
    void OnRefreshClimateList( wxCommandEvent &event);
    void OnOpenClimateFolder( wxCommandEvent &event);
    void OnClimateSearchText( wxCommandEvent &event);
    void OnSunImport( wxCommandEvent &event);
    void OnSunExport( wxCommandEvent &event);
    void OnSunCount( wxCommandEvent &event);
    void OnSunEdit( wxCommandEvent &event);
    void OnAtmUpdate( wxCommandEvent &event);
    void OnAttenFocus( wxFocusEvent &event);
    
    void OnHeliotempAdd( wxCommandEvent &event);
    void OnHeliotempDel( wxCommandEvent &event);
    void OnHeliotempState( wxCommandEvent &event);
    void OnHeliotempRename( wxCommandEvent &event);
    void OnHeliotempSelect( wxListEvent &event);
    void OnHeliotempDeselect( wxListEvent &event);
    void OnTempBoundsEdit( wxCommandEvent &event);
    void OnHeliotempOrderUp( wxCommandEvent &event);
    void OnHeliotempOrderDown( wxCommandEvent &event);
    
    void OnReceiverAdd( wxCommandEvent &event);
    void OnReceiverDel( wxCommandEvent &event);
    void OnReceiverState( wxCommandEvent &event);
    void OnReceiverRename( wxCommandEvent &event);
    void OnReceiverColor( wxCommandEvent &event);
    void OnReceiverSelect( wxListEvent &event);
    void OnReceiverDeselect( wxListEvent &event);
    void OnReceiverPowerGridEdit( wxGridEvent &event );
    
    void OnHeatLossLoadFocus( wxFocusEvent &event);
    void OnHeatLossWindFocus( wxFocusEvent &event);
    void OnUserFluxImport(wxCommandEvent &event);
    void OnUserFluxExport(wxCommandEvent &event);
    void OnUserFluxNx(wxCommandEvent &event);
    void OnUserFluxNy(wxCommandEvent &event);
    void OnUserFluxGridChange(wxGridEvent &event);

    void OnDoLayout( wxCommandEvent &event);            
    void OnLayoutImport( wxCommandEvent &event);            
    void OnLayoutExport( wxCommandEvent &event);            
    void OnLayoutCount( wxCommandEvent &event);
    void OnLayoutEdit( wxGridEvent &event);
    void OnLayoutRefresh( wxCommandEvent &event);
    void OnPlotSelectCombo( wxCommandEvent &event );
    void OnPlotDataToggle( wxCommandEvent &event );
    void OnPlotFontIncrease( wxCommandEvent &event);
    void OnPlotFontDecrease( wxCommandEvent &event);
    void OnBitmapSave( wxCommandEvent &event);
    void OnFieldPlotZoomIn( wxCommandEvent &event);
    void OnFieldPlotZoomOut( wxCommandEvent &event);
    void OnFieldPlotZoomRect( wxCommandEvent &event);
    void OnFieldPlotZoomPan( wxCommandEvent &event);
    void OnFieldPlotZoomOriginal( wxCommandEvent &event);
    void OnDeleteSelectedHeliostats( wxCommandEvent &event );
    void OnRestoreSelectedHeliostats( wxCommandEvent &event );
    void OnFieldPlotMouseLeftDown( wxMouseEvent &event);
    void OnFieldPlotMouseLeftUp( wxMouseEvent &event);
    void OnFieldPlotMouseRight( wxMouseEvent &event );
    void OnFieldPlotMouseWheel( wxMouseEvent &event);
    void OnFieldPlotMouseCenterUp( wxMouseEvent &event);
    void OnFieldPlotMouseCenterDown( wxMouseEvent &event);
    void OnFieldPlotMotion( wxMouseEvent &event);

    void OnPlotResIncrease( wxCommandEvent &event);
    void OnPlotResDecrease( wxCommandEvent &event);
    void OnPlotResReset( wxCommandEvent &event);
    void OnFluxBitmapSave( wxCommandEvent &event);            
    void OnFluxTableSave( wxCommandEvent &event);            
    void OnLayoutSimulationExport( wxCommandEvent &event);            
    void OnFluxLCSort( wxCommandEvent &event);
    void OnSTRayDataButton( wxCommandEvent &event);
    void OnReceiverFluxSelect( wxCommandEvent &event);
    void OnScatterToggle( wxCommandEvent &event);
    void OnColormapSelect( wxCommandEvent &event);
    void OnFluxPlotBinChangeX( wxFocusEvent &event);
    void OnFluxPlotBinChangeY( wxFocusEvent &event);
    void OnFluxPlotBinChange( int axis );

    
    void OnDoPerformanceSimulation( wxCommandEvent &event);            
    void OnStinputExport( wxCommandEvent &event);            
    void OnSAMInputButton( wxCommandEvent &event);
    void OnSAMDirSelect( wxCommandEvent &event);
    void OnToggleParametric( wxCommandEvent &event);

    void OnParAddVar( wxCommandEvent &event);
    void OnParRemoveVar( wxCommandEvent &event);
    void OnParEditVar( wxCommandEvent &event);
    void OnParEditLinkages( wxCommandEvent &event);
    void OnParVarlistSelected( wxCommandEvent &event);
    void OnParVarDoubleClick( wxCommandEvent &event);
    void OnParametricSimulate( wxCommandEvent &event);            
    void OnParFieldSaveChoicePanel(wxCommandEvent &event);
    void OnUserVarCount( wxCommandEvent &event);
    void OnUserSimCount( wxCommandEvent &event);
    void OnUserParImport( wxCommandEvent &event);
    void OnUserParExport( wxCommandEvent &event);            
    void OnUserParVariables( wxCommandEvent &event);
    void OnUserParSimulate( wxCommandEvent &event);            
    void OnUserParEditTable( wxGridEvent &event);

    void OnDoOptimizationSimulation( wxCommandEvent &event);    
    void OnClearOptimizationLog( wxCommandEvent &event);
    void OnSaveOptimizationLog( wxCommandEvent &event);
    void OnApplyOptimizationResult( wxCommandEvent &event);
    void OnDisableDetailUpdate( wxCommandEvent &event);

    void OnOptimizeAddVar( wxCommandEvent &event);
    void OnOptimizeRemoveVar( wxCommandEvent &event);
    void OnOptimizeGridChange( wxGridEvent &event);


    void OnSimulationResultsExport( wxCommandEvent &event);            
    void OnSimulationResultsCopy( wxCommandEvent &event);  
    void OnResultsReceiverSelect( wxCommandEvent &event);

public:
    SPFrame(wxWindow* parent, int id, const wxString& title, const wxPoint& pos=wxDefaultPosition, const wxSize& size=wxDefaultSize, long style=wxDEFAULT_FRAME_STYLE);
    ~SPFrame();
    static SPFrame &Instance();
    static bool Destroyed();
    
	wxSize getDefaultInputSize();

    bool CloseProject();
    void ShowContextHelp();

    SolarField *GetSolarFieldObject();
    var_map *GetVariablesObject();
    sim_results *GetResultsObject();
    void SetThreadCount(int nthread);
    int GetThreadCount();
    ArrayString *GetLocalWeatherDataObject();
    FluxPlot *GetFluxPlotObject();
    FieldPlot *GetFieldPlotObject();
    parametric *GetParametricDataObject();
    optimization *GetOptimizationDataObject();
    void  SetScriptWindowPointer(SolarPILOTScriptWindow *p);
    SolarPILOTScriptWindow* GetScriptWindowPointer();
    std::string GetVersionInfo();
    wxFileName GetImageDir();

    void UpdateHelioUITemplates();
    void UpdateReceiverUITemplates();
    void UpdateReceiverPowerGrid();
    void UpdateInputValues();
    void UpdateCalculatedGUIValues();
    void UpdateFieldPlotSelections();
    void UpdateFluxPlotSelections();
    
    bool CreateRestorePoint();
    void PreSimulation();
    void PostSimulation();
    wxGauge *chooseProgressGauge();
    wxStaticText *chooseProgressText();

    void Save(int save_config);
    void Open(wxString file = wxEmptyString, bool quiet=false);
    void NewFile();
    void UpdateRecents();
    void AddRecentFile(wxFileName &fname);
	bool GetInputModState();
    bool ModCheckPrompt();
    
    void UpdateLayoutGrid();
    void UpdateLayoutLog(std::string msg, bool clear = false);
    
    void PlotFontIncrease( wxString type);
    void PlotFontDecrease( wxString type);
    
    void GridCount(int nrow, int ncol, wxGrid *grid);
    void GridCount( wxSpinCtrl *sc, wxGrid *grid );
    
    void UpdateUserSunGrid();
    void setHelioTemplateGrid();
    void UpdateDesignSelect(int sel, var_map &vset);

    void LoadClimateFiles();
    void UpdateClimateListValues();
    void UpdateClimateFile( wxString &file, var_map &V, bool mapval_only=false);

    void UpdateLandBoundsGrid(var_map &V); //string &incs, string &excs);    
    void BoundsImport();
    void BoundsExport();

    void SetGeomState(bool state);
    bool GetGeomState();
    void UpdateUserFluxGrid(int id);
    void UpdateUserFluxData(int id);

    void DoResultsPage();  //create from _results
    void SAMInputParametric();
    void SAMInputParametric2();
    void LayoutSimulationExport(SolarField &SF, wxString &fname, std::vector<bool> &options, wxString &header, wxString &delim, bool quiet = false);
    void CreateResultsTable(sim_result &result, grid_emulator &table);
    void CreateParametricsTable(parametric &par, sim_results &results, grid_emulator &table);
    
    void StartSimTimer();
    void StopSimTimer();
    void SimProgressUpdate(simulation_info* siminfo);
    void SimProgressUpdateMT(int n_complete, int n_tot);
    void SimProgressBase(int n_complete, int n_tot, std::string *notices, wxGauge *active_gauge, wxStaticText *active_label, wxString label_text = "Simulation");
    void SimErrorHandler(simulation_error *simerror);
    void OptimizeProgressSummaryUpdate(simulation_info *siminfo);
    void OptimizeProgressDetailUpdate(simulation_info *siminfo);
    void OptimizeLogMessage(const char* message);
    void OptimizeLogMessage(std::string message);
    void OptReloadVariableList();
    void CheckOptGridRange();

    bool DoManagedLayout(SolarField &SF, var_map &vset);
    bool DoPerformanceSimulation(SolarField &SF, var_map &vset, Hvector &helios);
    bool SolTraceFluxSimulation(SolarField &SF, var_map &vset, Hvector &helios);
    bool SolTraceFluxBinning(SolarField &SF);
    bool HermiteFluxSimulationHandler(SolarField &SF, Hvector &helios);
    void UpdateFluxLC(int sortkey=0);
    int SolTraceProgressUpdate(st_uint_t ntracedtotal, st_uint_t ntraced, st_uint_t ntotrace, st_uint_t curstage, st_uint_t nstages, void *data);
    void SolTraceFileExport(std::string fname);
    void FluxProgressBase(int n_complete, int n_tot, wxGauge *active_gauge, wxStaticText *active_label, wxString label_text = "Calculation");
    bool GetSelectedHeliostats(Hvector &helios, bool select_enabled);

    void ParReloadVariableList();
    void ParRefreshValuesList();
    void ParametricSimulate(parametric &P);
    void ParProgressUpdate(int n_complete, int n_tot);
    void UserParProgressUpdate(int n_complete, int n_tot);
    void UpdateParValueTable();
    void SyncParValuesTable();
    void UserParImport();
    bool ParCheckSetup(parametric &par_data, var_map &vset);
    bool ParCheckSetup(ArrayString &par_vars, var_map &vset);

    void WriteVariablesToFile(wxTextFile &tfile, var_map &vset);
    void LogFileVariableDump(var_map &vset);
    void LogFileVariableDump(var_map &vset, wxArrayStr &messages);
    int PopMessage(wxString message, wxString title = wxEmptyString, long style = wxOK|wxICON_INFORMATION);
    void ScriptMessageOutput(const char *msg);
    
};

//--------------- Callback methods for the simulation engines --------------------------
extern int STCallback(st_uint_t ntracedtotal, st_uint_t ntraced, st_uint_t ntotrace, st_uint_t curstage, st_uint_t nstages, void *data);

extern bool SolarFieldInfoCallback(simulation_info *siminfo, void *data);

extern void SolarFieldErrorCallback(simulation_error *simerror, void *data);

extern bool SolarFieldOptimizeSummaryCallback(simulation_info *siminfo, void *data);

extern bool SolarFieldOptimizeDetailCallback(simulation_info *siminfo, void *data);

#endif
