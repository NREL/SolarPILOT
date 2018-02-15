#include "GUI_main.h"


void SPFrame::CreateResultsSummaryPage(wxScrolledWindow *parent, sim_results &results)
{
    /* 
    Create page with a summary of simulation results.
    */
#if _USE_WINDOW_BG 
    parent->SetBackgroundColour(_background_colour);
#endif
    wxBoxSizer *main_sizer = new wxBoxSizer(wxVERTICAL);
    
    try
    {
        
        int empty_sim_results = -1234;
        if(results.size() == 0) //goto empty_sim_results_flag;
            throw empty_sim_results;  //empty sim results

        wxButton 
            *export_button,
            *copy_button;
    
        int sim_type = results.at(0).sim_type;    //Use the first simulation to figure out what type of simulation it is
        if(sim_type == 0)
        {    //Layout
            
        }
        else if(sim_type == 1)
        { //Optimization
            
        }
        else if(sim_type == 2)
        {    //Flux simulation
            export_button = new wxButton(parent, wxID_ANY, wxT("Export to CSV"));
            export_button->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnSimulationResultsExport ), NULL, this);
            copy_button = new wxButton(parent, wxID_ANY, wxT("Copy to clipboard"));
            copy_button->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnSimulationResultsCopy ), NULL, this);

            wxStaticBox *grid_sb = new wxStaticBox(parent, wxID_ANY, wxT("Flux simulation results summary"));
            wxStaticBoxSizer *grid_sbs = new wxStaticBoxSizer(grid_sb, wxVERTICAL);
        
            sim_result *res = &results.at(0);

            _results_grid = new wxGrid(parent, wxID_ANY, wxDefaultPosition, wxSize(350,600));
            grid_emulator textgrid;
            CreateResultsTable(*res, textgrid);
            textgrid.MapToWXGrid(_results_grid);
            _results_grid->SetRowLabelSize(200);

        
            int nrow=_results_grid->GetNumberRows(),
                ncol=_results_grid->GetNumberCols();
            for(int i=0; i<ncol; i++)
            {
                for(int j=0; j<nrow; j++)
                {
                    _results_grid->SetReadOnly(j,i,true);
                    if(_results_grid->GetCellValue(j,i).empty())
                        _results_grid->SetCellBackgroundColour(j,i,wxColour(150,150,150,255));
                }
            }

            wxBoxSizer *bsizer = new wxBoxSizer(wxHORIZONTAL);
            bsizer->Add(export_button, 0, wxALL, 5);
            bsizer->Add(copy_button, 0, wxALL, 5);
            grid_sbs->Add(bsizer, 0, 0, 0);
            grid_sbs->Add(_results_grid, 0, wxALL|wxEXPAND, 5);
                
            main_sizer->Add(grid_sbs, 1, wxALL|wxEXPAND, 5);
        }
        else if(sim_type == 3)
        {    //Parametric
            export_button = new wxButton(parent, wxID_ANY, wxT("Export to CSV"));
            export_button->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnSimulationResultsExport ), NULL, this);
            copy_button = new wxButton(parent, wxID_ANY, wxT("Copy to clipboard"));
            copy_button->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SPFrame::OnSimulationResultsCopy ), NULL, this);

            wxStaticBox *grid_sb = new wxStaticBox(parent, wxID_ANY, wxT("Parametric simulation results summary"));
            wxStaticBoxSizer *grid_sbs = new wxStaticBoxSizer(grid_sb, wxVERTICAL);
        
            _results_grid = new wxGrid(parent, wxID_ANY, wxDefaultPosition, wxSize(350,600));
            grid_emulator textgrid;
            CreateParametricsTable(_par_data, results, textgrid);
            textgrid.MapToWXGrid(_results_grid);
        
            _results_grid->SetRowLabelSize(200);

        
            int nrow=_results_grid->GetNumberRows(),
                ncol=_results_grid->GetNumberCols();
            for(int i=0; i<ncol; i++)
            {
                for(int j=0; j<nrow; j++)
                {
                    _results_grid->SetReadOnly(j,i,true);
                }
            }

            wxBoxSizer *bsizer = new wxBoxSizer(wxHORIZONTAL);
            bsizer->Add(export_button, 0, wxALL, 5);
            bsizer->Add(copy_button, 0, wxALL, 5);
            grid_sbs->Add(bsizer, 0, 0, 0);
            grid_sbs->Add(_results_grid, 0, wxALL|wxEXPAND, 5);
                
            main_sizer->Add(grid_sbs, 1, wxALL|wxEXPAND, 5);
        }
        else
        {
            throw empty_sim_results;
        }

    }
    catch( int throwflag )
    {
        //do nothing
        throwflag;
    }
    
    parent->SetSizer(main_sizer);
    parent->SetScrollbars(10, 10, parent->GetSize().GetWidth()/10, parent->GetSize().GetWidth()/10);
    return;
}

void SPFrame::OnSimulationResultsExport( wxCommandEvent &WXUNUSED(event))
{
    if(! CheckLicense() ) return;
    /* Export the simulation results summary table to a CSV file. Prompt for file location. */
    wxFileDialog filedlg(this, wxT("Export results..."), wxEmptyString, wxEmptyString, wxT("Comma-separated files (*.csv)|*.csv"), wxFD_SAVE|wxFD_OVERWRITE_PROMPT );
    filedlg.CentreOnParent();

    //Show and process
    wxString file_use;
    if(filedlg.ShowModal() == wxID_OK)
    {
        file_use = filedlg.GetPath();
    }
    else
    {
        return;
    }

    //Collect the data from the results table as it appears
    int
        nrow = _results_grid->GetNumberRows(),
        ncol = _results_grid->GetNumberCols();
    vector<vector<wxString> > table(nrow+1, vector<wxString>(ncol+1, wxEmptyString));
    

    //Get the row labels
    for(int i=0; i<nrow; i++)
    {
        table.at(i+1).at(0) = _results_grid->GetRowLabelValue(i);
    }
    //Get the column labels
    for(int i=0; i<ncol; i++)
    {
        table.at(0).at(i+1) = _results_grid->GetColLabelValue(i);
    }
    //Get the cell values
    for(int i=0; i<nrow; i++)
    {
        for(int j=0; j<ncol; j++)
        {
            wxString val = _results_grid->GetCellValue(i, j);
            val.Replace(",","");
            table.at(i+1).at(j+1) = val;
        }
    }

    //Save the results to the file
    wxTextFile fout(file_use);

    if(! (fout.Exists() ? fout.Open() : fout.Create() ) )
    {
        PopMessage("The specified file could not be opened for writing. Please ensure that the file is not open or write-protected.", "File error");
        return;
    }

    fout.Clear();
    
    for(int i=0; i<nrow+1; i++)
    {
        wxString line;
        for(int j=0; j<ncol+1; j++)
        {
            line.Append( wxString::Format("%s,", table.at(i).at(j) ) );
        }
        fout.AddLine( line );
    }
    fout.Write();

    fout.Close();

}

void SPFrame::OnSimulationResultsCopy( wxCommandEvent &WXUNUSED(event))
{
    if(! CheckLicense() ) return;
    /* The "copy to clipboard" button has been clicked for the simulation results. */

    //Open the clipboard 
    if(! wxTheClipboard->Open()) return;
    
    //Collect the data from the results table as it appears
    int
        nrow = _results_grid->GetNumberRows(),
        ncol = _results_grid->GetNumberCols();
    vector<vector<wxString> > table(nrow+1, vector<wxString>(ncol+1, wxEmptyString));
    

    //Get the row labels
    for(int i=0; i<nrow; i++)
    {
        table.at(i+1).at(0) = _results_grid->GetRowLabelValue(i);
    }
    //Get the column labels
    for(int i=0; i<ncol; i++)
    {
        table.at(0).at(i+1) = _results_grid->GetColLabelValue(i);
    }
    //Get the cell values
    for(int i=0; i<nrow; i++)
    {
        for(int j=0; j<ncol; j++)
        {
            wxString val = _results_grid->GetCellValue(i,j);
            val.Replace(",","");
            table.at(i+1).at(j+1) = val;
        }
    }

    //Create a text string containing tab-separated values. Null terminate.
    wxString data;
    for(int i=0; i<nrow+1; i++)
    {
        for(int j=0; j<ncol+1; j++)
        {
            data.append( table.at(i).at(j) );
            if(j==ncol)
            {
                data.append("\n");
            }
            else
            {
                data.append("\t");
            }
        }
    }
    wxTheClipboard->SetData( new wxTextDataObject(data.c_str()) );

    //Close the clipboard
    wxTheClipboard->Close();

    //Notify
    PopMessage("Successfully copied to the clipboard.");
}
