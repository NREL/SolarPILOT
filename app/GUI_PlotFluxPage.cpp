#include "GUI_main.h"
#include "wx/tglbtn.h"
#include "STObject.h"

using namespace std;


void SPFrame::CreateFluxPlotPage(wxScrolledWindow *parent)
{
/* 
    Create a plot of the receiver flux map
    */
#if _USE_WINDOW_BG 
    parent->SetBackgroundColour(_background_colour);
#endif
    wxBoxSizer *main_sizer = new wxBoxSizer(wxVERTICAL);
    
    
    wxBitmap
        font_inc_bit, font_dec_bit, bit_save_img, bit_save_tab;
    font_inc_bit.LoadFile(_image_dir.GetPath(true) + "format-font-size-more.png", wxBITMAP_TYPE_PNG);
    font_dec_bit.LoadFile(_image_dir.GetPath(true) + "format-font-size-less.png", wxBITMAP_TYPE_PNG);
    bit_save_img.LoadFile(_image_dir.GetPath(true) + "save-image.png", wxBITMAP_TYPE_PNG);
    bit_save_tab.LoadFile(_image_dir.GetPath(true) + "save-table.png", wxBITMAP_TYPE_PNG);

    wxBitmapButton *font_inc = new wxBitmapButton(parent, wxID_ANY, font_inc_bit);
    font_inc->SetName(wxT("flux"));
    font_inc->SetToolTip("Increase plot font size");
    wxBitmapButton *font_dec = new wxBitmapButton(parent, wxID_ANY, font_dec_bit);
    font_dec->SetName(wxT("flux"));
    font_dec->SetToolTip("Decrease font plot size");
    wxBitmapButton *save_bitmap = new wxBitmapButton(parent, wxID_ANY, bit_save_img);
    save_bitmap->SetToolTip("Save the plot image to a file");
    wxBitmapButton *save_table = new wxBitmapButton(parent, wxID_ANY, bit_save_tab);
    save_table->SetToolTip("Save the plot data to a file");
    font_inc->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnPlotFontIncrease ), NULL, this);
    font_dec->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnPlotFontDecrease ), NULL, this);
    
    //inputs for z-axis scaling
    InputControl
        *is_autoscale = new InputControl(parent, wxID_ANY, _variables.flux.is_autoscale, wxDefaultPosition, wxSize(200,21)),
        *plot_zmin = new InputControl(parent, wxID_ANY, _variables.flux.plot_zmin, wxDefaultPosition, wxSize(50,21), 0L, "none", true),
        *plot_zmax = new InputControl(parent, wxID_ANY, _variables.flux.plot_zmax, wxDefaultPosition,  wxSize(50,21), 0L, "none", true);

    {
    wxWindow* dsibs[] = {plot_zmin, plot_zmax};
    is_autoscale->setDisabledSiblings("true", 2, dsibs);
    }

    //add inputs
    _input_map[ is_autoscale->getVarObject() ] = is_autoscale;
    _input_map[ plot_zmax->getVarObject() ] = plot_zmax;
    _input_map[ plot_zmin->getVarObject() ] = plot_zmin;
    //we have to do the bindings here! The flux plot page is recreated on the fly and bindings can be lost
    is_autoscale->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler( SPFrame::OnComboEvent ), NULL, this);
    plot_zmax->Connect(wxEVT_KILL_FOCUS, wxFocusEventHandler( SPFrame::OnFocusEvent ), NULL, this); 
    plot_zmin->Connect(wxEVT_KILL_FOCUS, wxFocusEventHandler( SPFrame::OnFocusEvent ), NULL, this); 
    //--
    if(_variables.flux.is_autoscale.val )
    {
    plot_zmax->Enable(false);
        plot_zmin->Enable(false);
    }

    wxStaticText *select_lab = new wxStaticText(parent, wxID_ANY, wxT("Plot display"));
    wxBitmap
        res_inc_bit, res_dec_bit, res_orig_bit;
    res_inc_bit.LoadFile(_image_dir.GetPath(true) + "resolution_up.png", wxBITMAP_TYPE_PNG);
    res_dec_bit.LoadFile(_image_dir.GetPath(true) + "resolution_down.png", wxBITMAP_TYPE_PNG);
    res_orig_bit.LoadFile(_image_dir.GetPath(true) + "resolution_original.png", wxBITMAP_TYPE_PNG);
    wxBitmap
        scatter_bit_on, scatter_bit_off;
    scatter_bit_on.LoadFile(_image_dir.GetPath(true) + "scatter-on.png", wxBITMAP_TYPE_PNG);
    scatter_bit_off.LoadFile(_image_dir.GetPath(true) + "scatter-off.png", wxBITMAP_TYPE_PNG);
        
    _scatter_btn = new MyToggleButton(parent, wxID_ANY, scatter_bit_on, scatter_bit_off);
    
    wxBitmapButton *res_inc = new wxBitmapButton(parent, wxID_ANY, res_inc_bit);
    wxBitmapButton *res_dec = new wxBitmapButton(parent, wxID_ANY, res_dec_bit);
    wxBitmapButton *res_orig = new wxBitmapButton(parent, wxID_ANY, res_orig_bit);
    _scatter_btn->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(SPFrame::OnScatterToggle), NULL, this);

    res_inc->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnPlotResIncrease ), NULL, this);
    res_dec->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnPlotResDecrease ), NULL, this);
    res_orig->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnPlotResReset ), NULL, this);
    save_bitmap->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnFluxBitmapSave ), NULL, this);
    save_table->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnFluxTableSave ), NULL, this);
    //Tool tips
    _scatter_btn->SetToolTip("Toggle view to/from aimpoint plot");
	res_inc->SetToolTip( "Increase plot resolution" );
    res_dec->SetToolTip( "Decrease plot resolution" );
    res_orig->SetToolTip( "Display at data resolution" );

    //Selection for the receiver
    wxArrayStr receivers;
    std::vector<Receiver*> *active_recs = _SF.getReceivers();
    for(unsigned int i=0; i<active_recs->size(); i++)
        receivers.push_back( active_recs->at(i)->getVarMap()->rec_name.val );
    
    _rec_select = new wxComboBox(parent, wxID_ANY, receivers[0], wxDefaultPosition, wxSize(wxDefaultSize.GetWidth(), _default_input_size.GetHeight()), receivers, wxCB_DROPDOWN|wxCB_READONLY);
    _rec_select->Connect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( SPFrame::OnReceiverFluxSelect ), NULL, this);

    wxArrayString cmap_choices;
    cmap_choices.Add("Jet");
    cmap_choices.Add("Grayscale");
    cmap_choices.Add("Hot-cold");
    cmap_choices.Add("Parula");
    wxComboBox *cmap = new wxComboBox(parent, wxID_ANY, "Parula", wxDefaultPosition, wxDefaultSize, cmap_choices, wxCB_DROPDOWN|wxCB_READONLY);
    cmap->Connect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( SPFrame::OnColormapSelect ), NULL, this);

    Hvector helios;
    GetSelectedHeliostats(helios, _flux_lc_check->GetValue());
    _flux_frame = new FluxPlot(parent, _SF, helios, 0, wxID_ANY, wxDefaultPosition, wxSize(600,500) );
    _flux_frame->SetPlotType(0); // flux

    _flux_frame->SetZRange(
        _variables.flux.plot_zmin.val,
        _variables.flux.plot_zmax.val,
        _variables.flux.is_autoscale.val
        );

    wxBoxSizer *top_sizer = new wxBoxSizer(wxHORIZONTAL);
    top_sizer->Add(select_lab, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    top_sizer->Add(_rec_select, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    top_sizer->AddSpacer(10);
    top_sizer->Add(cmap, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    top_sizer->Add((1,30), 0, 0, 0);
    top_sizer->Add(new wxStaticLine(parent, wxID_ANY, wxDefaultPosition, wxSize(1,1), wxLI_VERTICAL), 0, wxEXPAND|wxLEFT|wxRIGHT, 20);
    top_sizer->Add(font_inc, 0, wxRIGHT|wxALIGN_CENTER_VERTICAL, 2);
    top_sizer->Add(font_dec, 0, wxALIGN_CENTER_VERTICAL, 0);
    top_sizer->Add(new wxStaticLine(parent, wxID_ANY, wxDefaultPosition, wxSize(1,1), wxLI_VERTICAL), 0, wxEXPAND|wxLEFT|wxRIGHT, 20);
    top_sizer->Add(save_bitmap, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5);
    top_sizer->Add(save_table, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 5);
    top_sizer->Add(new wxStaticLine(parent, wxID_ANY, wxDefaultPosition, wxSize(1,1), wxLI_VERTICAL), 0, wxEXPAND|wxLEFT|wxRIGHT, 20);
    top_sizer->Add(res_inc, 0, wxRIGHT|wxALIGN_CENTER_VERTICAL, 2);
    top_sizer->Add(res_dec, 0, wxRight|wxALIGN_CENTER_VERTICAL, 2);
    top_sizer->Add(res_orig, 0, wxRight|wxALIGN_CENTER_VERTICAL, 2);
    top_sizer->Add(new wxStaticLine(parent, wxID_ANY, wxDefaultPosition, wxSize(1,1), wxLI_VERTICAL), 0, wxEXPAND|wxLEFT|wxRIGHT, 20);
    top_sizer->Add(_scatter_btn, 0, wxALIGN_CENTER_VERTICAL|wxLEFT, 10);
    top_sizer->Add(new wxStaticLine(parent, wxID_ANY, wxDefaultPosition, wxSize(1,1), wxLI_VERTICAL), 0, wxEXPAND|wxLEFT|wxRIGHT, 20);
    
    wxBoxSizer *scale_sizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *scale_lower = new wxBoxSizer(wxHORIZONTAL);

    scale_lower->Add(plot_zmin);
    scale_lower->Add(plot_zmax);
    scale_sizer->Add(is_autoscale);
    scale_sizer->Add(scale_lower);
    top_sizer->Add(scale_sizer);

    main_sizer->Add(top_sizer, 0, wxALL, 5);
    
    main_sizer->Add(_flux_frame, 1, wxEXPAND|wxALL, 5);

    parent->SetSizer(main_sizer);
    parent->SetScrollbars(10, 10, parent->GetSize().GetWidth()/10, parent->GetSize().GetWidth()/10);
    
    return;
}

void SPFrame::OnPlotResIncrease( wxCommandEvent &WXUNUSED(event))
{
_flux_frame->SetResolutionMultiplier( _flux_frame->GetResolution()+1 );
    this->Update();
    this->Refresh();
}

void SPFrame::OnPlotResDecrease( wxCommandEvent &WXUNUSED(event))
{
int res0 = _flux_frame->GetResolution();
    if(res0 > 1) _flux_frame->SetResolutionMultiplier( res0-1);

    this->Update();
    this->Refresh();
}

void SPFrame::OnPlotResReset( wxCommandEvent &WXUNUSED(event))
{
_flux_frame->SetResolutionMultiplier(1);
    this->Update();
    this->Refresh();
}

void SPFrame::OnFluxBitmapSave( wxCommandEvent &WXUNUSED(event))
{
    try
    {
        
        wxFileDialog *dlg = new wxFileDialog(this, wxT("Export image to a file"), wxEmptyString, "flux-plot.png", 
            wxT("Image files (*.jpg;*.png;*.bmp)|*.jpg;*.png;*.bmp"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
        if(dlg->ShowModal()==wxID_OK)
        {
            //Save the bitmap plot to a file.
            wxBitmap *bitmap = _flux_frame->GetBitmap();
            wxImage image = bitmap->ConvertToImage();

            wxString fpath = dlg->GetPath();
            vector<string> seps = split(fpath.ToStdString(), ".");
            string ftype = lower_case(seps.at(seps.size()-1));
        
            if(ftype == "jpg" || ftype == "jpeg")
            { // jpeg
                image.SaveFile( fpath, wxBITMAP_TYPE_JPEG );
            }
            else if(ftype == "png")
            { //png
                image.SaveFile( fpath, wxBITMAP_TYPE_PNG );
            }
            else if(ftype == "bmp")
            { //bmp
                image.SaveFile( fpath, wxBITMAP_TYPE_BMP );
            }
            else
            {return;}
                
        }
    }
    catch(std::exception &e)
    {
        PopMessage(e.what(), "OnFluxBitmapSave Error", wxICON_ERROR|wxOK);
    }
    catch(...)
    {
        PopMessage("Caught unspecified error. Simulation is terminating.", "OnFluxBitmapSave Error", wxICON_ERROR|wxOK);
        return;
    }

}

void SPFrame::OnFluxTableSave( wxCommandEvent &WXUNUSED(event))
{
    /* 
    Save a file containing the flux map data for the currently plotted flux map
    */
    try
    {

        //Where should it go?
        wxFileDialog *dlg = new wxFileDialog(this, "Export table to a file", wxEmptyString, "flux-table.csv",
            "Delimited file (*.csv;*.txt)|*.csv;*.txt", wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
        if(dlg->ShowModal()==wxID_OK)
        {
            //get the path
            wxFileName path = dlg->GetPath();
            //Get the selected file extension and required delimiter
            wxString delim = path.GetExt() == "csv" ? "," : "\t";

            wxString tmp = path.GetFullPath();
            if(! _flux_frame->SaveDataTable(tmp, delim) )
                PopMessage(wxT("The file could not be opened. Please ensure that the file is not open or write-protected."), wxEmptyString, wxOK);
        }
    }
    catch(std::exception &e)
    {
        PopMessage(e.what(), "OnFluxTableSave Error", wxICON_ERROR|wxOK);
    }
    catch(...)
    {
        PopMessage("Caught unspecified error. Simulation is terminating.", "OnFluxTableSave Error", wxICON_ERROR|wxOK);
        return;
    }
}

void SPFrame::OnReceiverFluxSelect( wxCommandEvent &WXUNUSED(event) )
{
    //A new receiver has been selected, redraw the flux map
    int rselect = _rec_select->GetSelection();
    _flux_frame->SetWhichReceiver( rselect );
    this->Update(); 
    this->Refresh();
}

void SPFrame::OnScatterToggle( wxCommandEvent &WXUNUSED(event) )
{
    bool state = _scatter_btn->Toggle();
    _flux_frame->SetPlotType( state ? 1 : 0 );
    this->Update();
    this->Refresh();
}

void SPFrame::OnColormapSelect( wxCommandEvent &event )
{
    int sel = ((wxComboBox*)(event.GetEventObject()))->GetSelection();
    _flux_frame->SetColormap(sel);
    this->Update();
    this->Refresh();
}

void SPFrame::OnFluxPlotBinChangeX( wxFocusEvent &event )
{
    OnFluxPlotBinChange( 0 );
    event.Skip();
}

void SPFrame::OnFluxPlotBinChangeY( wxFocusEvent &event )
{
    OnFluxPlotBinChange( 1 );
    event.Skip();
}

void SPFrame::OnFluxPlotBinChange( int axis )
{
    if( _variables.flux.flux_model.mapval() != var_fluxsim::FLUX_MODEL::SOLTRACE )  //is this a soltrace sim?
        return;

    //Has a soltrace simulation been created?
    if( _STSim == 0 )
        return;

    //Does the soltrace simulation have existing intersection data?
    if( _STSim->IntData.nint == 0 )
        return;

    //did a value change?
    if( axis == 0 )
    {   
            //    new value                         old value
        if( _variables.flux.x_res.val == _SF.getReceivers()->front()->getFluxSurfaces()->front().getFluxNX() )
            return;
    }
    else
    {
        if( _variables.flux.y_res.val == _SF.getReceivers()->front()->getFluxSurfaces()->front().getFluxNY() )
            return;
    }


    //Regenerate the flux map plot data
    vector<Receiver*> *rec_to_sim = _SF.getReceivers();
    //Shape the flux surface files to match
    for(unsigned int i=0; i<rec_to_sim->size(); i++)
    {
        rec_to_sim->at(i)->DefineReceiverGeometry(
            _variables.flux.x_res.val, 
            _variables.flux.y_res.val
            );    //Flux map should match spec
    }
    SolTraceFluxBinning(_SF);
    Hvector helios;
    GetSelectedHeliostats(helios, _flux_lc_check->GetValue());

    _flux_frame->SetPlotData(_SF, helios, 0);

    //Process the results
    double azzen[] = { _results.back().solar_az, _results.back().solar_zen};  //re-use last position
    _results.back().process_raytrace_simulation(_SF, 2, azzen, helios, _STSim->IntData.q_ray, _STSim->IntData.emap, _STSim->IntData.smap,
        _STSim->IntData.rnum, _STSim->IntData.nint, _STSim->IntData.bounds);    
    return;
}