
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


void SPFrame::CreateCostPage(wxScrolledWindow *parent)
{
    //Create the master receiver page and return a pointer
#if _USE_WINDOW_BG 
    parent->SetBackgroundColour(_background_colour);
#endif    //Create objects

    //Direct costs grou
    wxStaticBox *sb0 = new wxStaticBox(parent, wxID_ANY, "Direct Capital Costs" );
    wxStaticBoxSizer *sbs0 = new wxStaticBoxSizer(sb0, wxVERTICAL);

    InputControl
        *tower_fixed_cost = new InputControl(parent, wxID_ANY,  _variables.fin.tower_fixed_cost),
        *tower_exp = new InputControl(parent, wxID_ANY,  _variables.fin.tower_exp),
        *rec_ref_cost = new InputControl(parent, wxID_ANY,  _variables.fin.rec_ref_cost),
        *rec_ref_area = new InputControl(parent, wxID_ANY,  _variables.fin.rec_ref_area),
        *rec_cost_exp = new InputControl(parent, wxID_ANY,  _variables.fin.rec_cost_exp),
        *site_spec_cost = new InputControl(parent, wxID_ANY,  _variables.fin.site_spec_cost),
        *heliostat_spec_cost = new InputControl(parent, wxID_ANY,  _variables.fin.heliostat_spec_cost);
    InputControl
        *wiring_user_spec = new InputControl(parent, wxID_ANY,  _variables.fin.wiring_user_spec),
        *fixed_cost = new InputControl(parent, wxID_ANY,  _variables.fin.fixed_cost);
    
    OutputControl
        *tower_cost = new OutputControl(parent, wxID_ANY, _variables.fin.tower_cost),
        *rec_cost = new OutputControl(parent, wxID_ANY, _variables.fin.rec_cost),
        *site_cost = new OutputControl(parent, wxID_ANY, _variables.fin.site_cost),
        *heliostat_area = new OutputControl(parent, wxID_ANY, _variables.sf.sf_area),
        *heliostat_cost = new OutputControl(parent, wxID_ANY, _variables.fin.heliostat_cost),
        *wiring_cost = new OutputControl(parent, wxID_ANY, _variables.fin.wiring_cost),
        *total_direct_cost = new OutputControl(parent, wxID_ANY, _variables.fin.total_direct_cost);

    wxFlexGridSizer *sbs0gs = new wxFlexGridSizer(2,0,0);
    sbs0gs->Add(tower_fixed_cost);
    sbs0gs->Add((1,1),0,0,0);
    sbs0gs->Add(tower_exp);
    sbs0gs->Add(tower_cost);
    sbs0gs->Add(rec_ref_cost);
    sbs0gs->Add((1,1),0,0,0);
    sbs0gs->Add(rec_ref_area);
    sbs0gs->Add((1,1),0,0,0);
    sbs0gs->Add(rec_cost_exp);
    sbs0gs->Add(rec_cost);
    sbs0gs->Add(heliostat_area);
    sbs0gs->Add((1,1),0,0,0);
    sbs0gs->Add(site_spec_cost);
    sbs0gs->Add(site_cost);
    sbs0gs->Add(heliostat_spec_cost);
    sbs0gs->Add(heliostat_cost);
    sbs0gs->Add(wiring_user_spec);
    sbs0gs->Add(wiring_cost, 0, wxALIGN_BOTTOM, 0);
    sbs0gs->Add((1,1),0,0,0);
    sbs0gs->Add(fixed_cost);
    sbs0->Add(sbs0gs);
    sbs0->Add(total_direct_cost, 0, wxALIGN_RIGHT|wxTOP, 20);
    

    //Indirect costs group
    wxStaticBox *sb1 = new wxStaticBox(parent, wxID_ANY, "Indirect Capital Costs" );
    wxStaticBoxSizer *sbs1 = new wxStaticBoxSizer(sb1, wxVERTICAL);

    InputControl
        *land_const = new InputControl(parent, wxID_ANY,  _variables.land.land_const),
        *land_mult = new InputControl(parent, wxID_ANY,  _variables.land.land_mult),
        *land_spec_cost = new InputControl(parent, wxID_ANY,  _variables.fin.land_spec_cost),
        *contingency_rate = new InputControl(parent, wxID_ANY,  _variables.fin.contingency_rate),
        *sales_tax_rate = new InputControl(parent, wxID_ANY,  _variables.fin.sales_tax_rate),
        *sales_tax_frac = new InputControl(parent, wxID_ANY,  _variables.fin.sales_tax_frac);
    OutputControl
        *bound_area = new OutputControl(parent, wxID_ANY, _variables.land.bound_area),
        *land_area_tot = new OutputControl(parent, wxID_ANY, _variables.land.land_area),
        *land_cost = new OutputControl(parent, wxID_ANY, _variables.fin.land_cost),
        *contingency_cost = new OutputControl(parent, wxID_ANY, _variables.fin.contingency_cost),
        *sales_tax_cost = new OutputControl(parent, wxID_ANY, _variables.fin.sales_tax_cost),
        *total_indirect_cost = new OutputControl(parent, wxID_ANY, _variables.fin.total_indirect_cost);

    wxFlexGridSizer *sbs1gs = new wxFlexGridSizer(2, 0, 0);
    sbs1gs->Add(land_spec_cost);
    sbs1gs->Add((1,1), 0, 0, 0);
    sbs1gs->Add(land_const);
    sbs1gs->Add(land_mult);
    sbs1gs->Add(bound_area);
    sbs1gs->Add((1,1), 0, 0, 0);
    sbs1gs->Add(land_area_tot);
    //sbs1gs->Add(land_area);
    sbs1gs->Add(land_cost);
    sbs1gs->Add(contingency_rate);
    sbs1gs->Add(contingency_cost);
    sbs1gs->Add(sales_tax_rate);
    sbs1gs->Add((1,1), 0, 0, 0);
    sbs1gs->Add(sales_tax_frac);
    sbs1gs->Add(sales_tax_cost);
    sbs1->Add(sbs1gs);
    sbs1->Add(total_indirect_cost, 0, wxALIGN_RIGHT|wxTOP, 20);

    //Total costs group
    wxStaticBox *sb2 = new wxStaticBox(parent, wxID_ANY, "Total Installed Costs" );
    wxStaticBoxSizer *sbs2 = new wxStaticBoxSizer(sb2, wxVERTICAL);

    OutputControl *total_installed_cost = new OutputControl(parent, wxID_ANY, _variables.fin.total_installed_cost);
    string msg = "The total installed cost does not include financing or OM costs.";
    wxStaticText *st = new wxStaticText(parent, wxID_ANY, msg, wxDefaultPosition, _default_input_size);
    st->SetForegroundColour(_helptext_colour);
    wxFlexGridSizer *sbs2gs = new wxFlexGridSizer(2, 0, 0);
    sbs2gs->Add(st);
    sbs2gs->Add(total_installed_cost);
    sbs2->Add(sbs2gs);

    //Layout
    wxBoxSizer *main_sizer = new wxBoxSizer(wxVERTICAL);
    main_sizer->Add(sbs0, 0, wxALL, 5);
    main_sizer->Add(sbs1, 0, wxALL, 5);
    main_sizer->Add(sbs2, 0, wxALL, 5);
    
    parent->SetSizer(main_sizer);

    //Add to input and output maps
    InputControl *inputs[] = {tower_fixed_cost, tower_exp, rec_ref_cost, rec_ref_area, rec_cost_exp, site_spec_cost,
                              heliostat_spec_cost, wiring_user_spec, fixed_cost, 
                              land_mult, land_const, land_spec_cost, contingency_rate, sales_tax_rate, sales_tax_frac, NULL};
    int i=0;
    while(inputs[i] != NULL)
    { _input_map[ inputs[i]->getVarObject() ] = inputs[i]; i++; }
        i=0;
    OutputControl *outputs[] = {tower_cost, rec_cost, site_cost, heliostat_cost, wiring_cost, bound_area, 
                                land_area_tot, land_cost, contingency_cost, sales_tax_cost, total_direct_cost, total_indirect_cost, 
                                total_installed_cost, heliostat_area, NULL};
    while(outputs[i] != NULL)
    { _output_map[ outputs[i]->getVarObject() ] = outputs[i]; i++; }
        
    //Set properties
    parent->SetScrollbars(10, 10, parent->GetSize().GetWidth()/10, parent->GetSize().GetWidth()/10);
    return;
}
