#include "GUI_main.h"


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
