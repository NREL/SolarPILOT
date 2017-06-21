#include "GUI_main.h"


void SPFrame::CreatePlantPage(wxScrolledWindow *parent){
	//Create the master receiver page and return a pointer
#if _USE_WINDOW_BG 
	parent->SetBackgroundColour(_background_colour);
#endif
	wxStaticBox *sb0 = new wxStaticBox(parent, wxID_ANY, wxT("Plant sizing"));
	wxStaticBoxSizer *sbs0 = new wxStaticBoxSizer(sb0, wxVERTICAL);

	OutputControl
		*q_sf_des = new OutputControl(parent, wxID_ANY, _variables.sf.q_des),
		*power_net = new OutputControl(parent, wxID_ANY, _variables.plt.power_net),
		*power_gross = new OutputControl(parent, wxID_ANY, _variables.plt.power_gross);
	InputControl
		*par_factor = new InputControl(parent, wxID_ANY,  _variables.plt.par_factor),
		*eta_cycle = new InputControl(parent, wxID_ANY,  _variables.plt.eta_cycle),
		*solar_mult = new InputControl(parent, wxID_ANY,  _variables.plt.solar_mult),
		*hours_tes = new InputControl(parent, wxID_ANY,  _variables.plt.hours_tes);
	sbs0->Add(q_sf_des);
	sbs0->Add(solar_mult);
	sbs0->Add(eta_cycle);
	//sbs0->Add(q_pb_des);
	sbs0->Add(power_gross);
	sbs0->Add(par_factor);
	sbs0->Add(power_net);
	sbs0->Add(hours_tes);


	InputControl *inputs[] = {par_factor, eta_cycle, solar_mult, hours_tes, NULL};
	OutputControl *outputs[] = {power_gross, power_net, q_sf_des, NULL}; //, q_pb_des
	int i=0;
	while(inputs[i] != NULL){ _input_map[ inputs[i]->getVarObject() ] = inputs[i]; i++; }
	i=0;
	while(outputs[i] != NULL){ _output_map[ outputs[i]->getVarObject() ] = outputs[i]; i++; }

	wxBoxSizer
		*main_sizer = new wxBoxSizer(wxHORIZONTAL),
		*col_sizer_1 = new wxBoxSizer(wxVERTICAL),
		*col_sizer_2 = new wxBoxSizer(wxVERTICAL);

	col_sizer_1->Add(sbs0, 0, wxALL, 5);
	
	main_sizer->Add(col_sizer_1);
	main_sizer->Add(col_sizer_2);
	parent->SetSizer(main_sizer);

	parent->SetScrollbars(10, 10, parent->GetSize().GetWidth()/10, parent->GetSize().GetWidth()/10);
	return;
}
