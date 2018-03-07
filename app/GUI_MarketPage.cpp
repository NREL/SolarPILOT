
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

using namespace std;


void SPFrame::CreateMarketPage(wxScrolledWindow *parent)
{
    //Create the master receiver page and return a pointer
#if _USE_WINDOW_BG 
    parent->SetBackgroundColour(_background_colour);
#endif    //Create objects

    InputControl 
        *is_pmt_factors = new InputControl(parent, wxID_ANY, _variables.fin.is_pmt_factors);
    _input_map[ is_pmt_factors->getVarObject() ] = is_pmt_factors;
    
    //Schedules
    wxStaticBox *sbwd = new wxStaticBox(parent, wxID_ANY, wxT("Weekday Schedule"));
    wxStaticBoxSizer *sbwd_s = new wxStaticBoxSizer(sbwd, wxVERTICAL);
    wxStaticBox *sbwe = new wxStaticBox(parent, wxID_ANY, wxT("Weekend Schedule"));
    wxStaticBoxSizer *sbwe_s = new wxStaticBoxSizer(sbwe, wxVERTICAL);

    wxDiurnalPeriodCtrl *sched_wd = new wxDiurnalPeriodCtrl(parent, wxID_ANY);
    sched_wd->SetupTOUGrid();
    sched_wd->Schedule( _variables.fin.weekday_sched.val ); 
    wxDiurnalPeriodCtrl *sched_we = new wxDiurnalPeriodCtrl(parent, wxID_ANY);
    sched_we->SetupTOUGrid();
    sched_we->Schedule( _variables.fin.weekend_sched.val ); 
    sched_wd->SetName( "weekday" );
    sched_we->SetName( "weekend" );    //Set the names to keep track of which is which
    
    sbwd_s->Add(sched_wd, 0, wxALL, 5);
    sbwe_s->Add(sched_we, 0, wxALL, 5);

    sched_wd->Connect(wxEVT_DIURNALPERIODCTRL_CHANGE, wxCommandEventHandler( SPFrame::OnDispatchGrid ), NULL, this);
    sched_we->Connect(wxEVT_DIURNALPERIODCTRL_CHANGE, wxCommandEventHandler( SPFrame::OnDispatchGrid ), NULL, this);

    //Dispatch factors
    wxStaticBox *sb1 = new wxStaticBox(parent, wxID_ANY, "Payment allocation factors");
    wxStaticBoxSizer *sbs1 = new wxStaticBoxSizer(sb1, wxVERTICAL);
    sbs1->Add(is_pmt_factors);
    
    string sint = "1";
    int npf = _variables.fin.pmt_factors.val.size();

    wxString varpath;
    string allvars;
    for(int i=0; i<npf; i++)
    {

        wxStaticText *label =  new wxStaticText(parent, wxID_ANY, "Payment allocation factor - period " + my_to_string(i+1), wxDefaultPosition, wxSize(190, _default_input_size.GetHeight()), wxTE_RIGHT);
        wxTextCtrl *input = new wxTextCtrl(parent, wxID_ANY, my_to_string(_variables.fin.pmt_factors.val.at(i) ), wxDefaultPosition, wxSize(90,_default_input_size.GetHeight())  );

        wxBoxSizer *sbs1_h = new wxBoxSizer(wxHORIZONTAL);
        sbs1_h->Add( label, 0, wxALL, 5);
        sbs1_h->Add( input, 0, wxALL, 5);

        sbs1->Add(sbs1_h);

        is_pmt_factors->addDisabledSiblings("false",  input);

    }

    wxBoxSizer 
        *col1 = new wxBoxSizer(wxVERTICAL),
        *col2 = new wxBoxSizer(wxVERTICAL);
    col1->Add(sbwd_s, 0, wxALL, 5);
    col1->Add(sbwe_s, 0, wxALL, 5);
    col2->Add(sbs1, 0, wxALL, 5);
    
    wxBoxSizer *main_sizer = new wxBoxSizer(wxHORIZONTAL);
    main_sizer->Add(col1);
    main_sizer->Add(col2);

    parent->SetSizer(main_sizer);
    
    //Set properties
    parent->SetScrollbars(10, 10, parent->GetSize().GetWidth()/10, parent->GetSize().GetWidth()/10);
    return;
}

void SPFrame::OnDispatchGrid( wxCommandEvent &event)
{
    /* One of the dispatch grids has been edited */
    wxDiurnalPeriodCtrl *obj = (wxDiurnalPeriodCtrl*)event.GetEventObject();
    wxString name = obj->GetName();
    wxString sched = obj->Schedule();

    //Update the local variables
    if(name != "weekend")
    {
        _variables.fin.weekday_sched.val = sched;

    }
    else
    {
        _variables.fin.weekend_sched.val = sched;
    }

    _variables.fin.pricing_array.Val().clear();

}
