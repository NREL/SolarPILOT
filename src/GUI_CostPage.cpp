#include "GUI_main.h"


void SPFrame::CreateCostPage(wxScrolledWindow *parent){
	//Create the master receiver page and return a pointer
#if _USE_WINDOW_BG 
	parent->SetBackgroundColour(_background_colour);
#endif	//Create objects

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
		*plant_spec_cost = new InputControl(parent, wxID_ANY,  _variables.fin.plant_spec_cost),
		*tes_spec_cost = new InputControl(parent, wxID_ANY,  _variables.fin.tes_spec_cost),
		*fixed_cost = new InputControl(parent, wxID_ANY,  _variables.fin.fixed_cost);
	
	OutputControl
		*tower_cost = new OutputControl(parent, wxID_ANY, _variables.fin.tower_cost),
		*rec_cost = new OutputControl(parent, wxID_ANY, _variables.fin.rec_cost),
		*site_cost = new OutputControl(parent, wxID_ANY, _variables.fin.site_cost),
        *heliostat_area = new OutputControl(parent, wxID_ANY, _variables.sf.sf_area),
		*heliostat_cost = new OutputControl(parent, wxID_ANY, _variables.fin.heliostat_cost),
		*wiring_cost = new OutputControl(parent, wxID_ANY, _variables.fin.wiring_cost),
		*plant_cost = new OutputControl(parent, wxID_ANY, _variables.fin.plant_cost),
		*tes_cost = new OutputControl(parent, wxID_ANY, _variables.fin.tes_cost),
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
	sbs0gs->Add(plant_spec_cost);
	sbs0gs->Add(plant_cost);
	sbs0gs->Add(tes_spec_cost);
	sbs0gs->Add(tes_cost);
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

	OutputControl
		*total_installed_cost = new OutputControl(parent, wxID_ANY, _variables.fin.total_installed_cost),
		*cost_per_capacity = new OutputControl(parent, wxID_ANY, _variables.fin.cost_per_capacity );
	string msg = "The total installed cost does not include financing or OM costs.";
	wxStaticText *st = new wxStaticText(parent, wxID_ANY, msg, wxDefaultPosition, _default_input_size);
	st->SetForegroundColour(_helptext_colour);
	wxFlexGridSizer *sbs2gs = new wxFlexGridSizer(2, 0, 0);
	sbs2gs->Add(st);
	sbs2gs->Add(total_installed_cost);
	sbs2gs->Add((1,1), 0, 0, 0);
	sbs2gs->Add(cost_per_capacity);
	sbs2->Add(sbs2gs);

	//Layout
	wxBoxSizer *main_sizer = new wxBoxSizer(wxVERTICAL);
	main_sizer->Add(sbs0, 0, wxALL, 5);
	main_sizer->Add(sbs1, 0, wxALL, 5);
	main_sizer->Add(sbs2, 0, wxALL, 5);
	
	parent->SetSizer(main_sizer);

	//Add to input and output maps
	InputControl *inputs[] = {tower_fixed_cost, tower_exp, rec_ref_cost, rec_ref_area, rec_cost_exp, site_spec_cost,
							  heliostat_spec_cost, wiring_user_spec, plant_spec_cost, tes_spec_cost, fixed_cost, 
							  land_mult, land_const, land_spec_cost, contingency_rate, sales_tax_rate, sales_tax_frac, NULL};
	int i=0;
	while(inputs[i] != NULL){ _input_map[ inputs[i]->getVarObject() ] = inputs[i]; i++; }
	i=0;
	OutputControl *outputs[] = {tower_cost, rec_cost, site_cost, heliostat_cost, wiring_cost, plant_cost, tes_cost, bound_area, 
								land_area_tot, land_cost, contingency_cost, sales_tax_cost, total_direct_cost, total_indirect_cost, 
								total_installed_cost, cost_per_capacity, heliostat_area, NULL};
	while(outputs[i] != NULL){ _output_map[ outputs[i]->getVarObject() ] = outputs[i]; i++; }

	//Set properties
	parent->SetScrollbars(10, 10, parent->GetSize().GetWidth()/10, parent->GetSize().GetWidth()/10);
	return;
}
