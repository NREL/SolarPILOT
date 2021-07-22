
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
#include "interop.h"

using namespace std;

void SPFrame::CreateResultsSummaryPage(wxScrolledWindow *parent)
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
        if(_results.size() == 0) //goto empty_sim_results_flag;
            throw empty_sim_results;  //empty sim results

        wxButton 
            *export_button,
            *copy_button;
    
        int sim_type = _results.at(0).sim_type;    //Use the first simulation to figure out what type of simulation it is
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

            wxComboBox *rec_select_combo=0;
            if (_results.front().receiver_names.size() > 1)
            {
                wxArrayString recnames;
                recnames.push_back("All receivers");
                for (int i = 0; i < _results.front().receiver_names.size(); i++)
                    recnames.push_back(_results.front().receiver_names.at(i));
                rec_select_combo = new wxComboBox(parent, wxID_ANY, recnames.front(), wxDefaultPosition, wxDefaultSize, recnames, wxCB_DROPDOWN | wxCB_READONLY);
                rec_select_combo->Connect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( SPFrame::OnResultsReceiverSelect ), NULL, this);
            }

            wxStaticBox *grid_sb = new wxStaticBox(parent, wxID_ANY, wxT("Flux simulation results summary"));
            wxStaticBoxSizer *grid_sbs = new wxStaticBoxSizer(grid_sb, wxVERTICAL);
        
            _results_grid = new wxGrid(parent, wxID_ANY, wxDefaultPosition, wxSize(350,600));
            grid_emulator textgrid;
            interop::CreateResultsTable(_results.front(), textgrid);
            _results_grid->CreateGrid(textgrid.GetNumberRows(),textgrid.GetNumberCols());
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
                        _results_grid->SetCellBackgroundColour(j,i,wxColour(100,100,100,255));
                    else if(i > 1)
                        _results_grid->SetCellBackgroundColour(j,i,wxColour(225,225,225,255));
                }
            }

            wxBoxSizer *bsizer = new wxBoxSizer(wxHORIZONTAL);
            bsizer->Add(export_button, 0, wxALL, 5);
            bsizer->Add(copy_button, 0, wxALL, 5);
            if (rec_select_combo)
            {
                bsizer->AddSpacer(25);
                bsizer->Add( new wxStaticText(parent, wxID_ANY, "Results display selection"), 0, wxALL, 5 );
                bsizer->Add(rec_select_combo, 0, wxALL, 5);
            }

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
            CreateParametricsTable(_par_data, _results, textgrid);
            _results_grid->CreateGrid(textgrid.GetNumberRows(), textgrid.GetNumberCols());
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

void SPFrame::OnResultsReceiverSelect(wxCommandEvent &evt)
{
    wxString selection = static_cast<wxComboBox*>(evt.GetEventObject())->GetValue();
    //which result is this?
    sim_result* result = 0;
    
    if (selection == "All receivers")
    {
        result = &_results.front();
    }
    else
    {
        for (sim_results::iterator sr = _results.begin(); sr != _results.end(); sr++)
            if (selection == sr->receiver_names.front())
                result = &*sr;
        if (!result)
            return;
    }

    grid_emulator textgrid;
    _results_grid->ClearGrid();
    interop::CreateResultsTable(*result, textgrid);
    
    textgrid.MapToWXGrid(_results_grid);

    _results_grid->GetParent()->Update();
    _results_grid->GetParent()->Refresh();
}