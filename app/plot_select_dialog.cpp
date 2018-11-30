#include "plot_select_dialog.h"



PlotSelectDialog::PlotSelectDialog(wxWindow* parent, wxWindowID id, wxString label,
    std::vector<std::string> options, wxPoint pos, wxSize size, long style)
{
    Create(parent, id, label, pos, size, style);

    wxStaticBox *sbplot = new wxStaticBox(this, wxID_ANY, "Plot selections");
    wxStaticBoxSizer *sbsplot = new wxStaticBoxSizer(sbplot, wxVERTICAL);

    wxFlexGridSizer *fgs = new wxFlexGridSizer(2, wxSize(5, 5));
    _options.clear();
    for (size_t i = 0; i < options.size(); i++)
    {
        _options.push_back(new wxCheckBox(this, wxID_ANY, options.at(i)));
        fgs->Add(_options.back());
    }

    sbsplot->Add(fgs, 0, wxALL, 5);
    wxBoxSizer *main = new wxBoxSizer(wxVERTICAL);

    wxButton *optall = new wxButton(this, wxID_ANY, "All");
    optall->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PlotSelectDialog::OnAllOptButton), NULL, this);
    wxButton *optnone = new wxButton(this, wxID_ANY, "None");
    optnone->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PlotSelectDialog::OnNoneOptButton), NULL, this);

    wxBoxSizer *optbsizer = new wxBoxSizer(wxHORIZONTAL);
    optbsizer->Add(optall, 0, wxALL, 5);
    optbsizer->Add(optnone, 0, wxALL, 5);
    sbsplot->Add(optbsizer, 0, wxALL, 5);
    main->Add(sbsplot);

    //main->Add(new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL), 0, wxEXPAND | wxALL, 5);

    wxStaticBox *sbannot = new wxStaticBox(this, wxID_ANY, "Annotations");
    wxStaticBoxSizer *sbsannot = new wxStaticBoxSizer(sbannot, wxVERTICAL);

    std::vector< std::string > labs =
    {
        "Date and time", "Sun position", "DNI", "Total efficiency", "Cosine efficiency",
        "Blocking/shading efficiency", "Attenuation efficiency", "Intercept efficiency",
        "Total power", "Aiming method", "Variables"
    };

    _annotations.clear();
    for (std::vector<std::string>::iterator sit = labs.begin(); sit != labs.end(); sit++)
    {
        _annotations.push_back(new wxCheckBox(this, wxID_ANY, *sit));
        sbsannot->Add(_annotations.back());
    }

    wxButton *annotall = new wxButton(this, wxID_ANY, "All");
    annotall->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PlotSelectDialog::OnAllAnnotButton), NULL, this);
    wxButton *annotnone = new wxButton(this, wxID_ANY, "None");
    annotnone->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PlotSelectDialog::OnNoneAnnotButton), NULL, this);

    wxBoxSizer *annotbsizer = new wxBoxSizer(wxHORIZONTAL);
    annotbsizer->Add(annotall, 0, wxALL, 5);
    annotbsizer->Add(annotnone, 0, wxALL, 5);
    sbsannot->Add(annotbsizer, 0, wxALL, 5);

    main->Add(sbsannot, 0, wxALL, 5);

    wxButton *apply = new wxButton(this, wxID_ANY, "Apply");
    apply->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PlotSelectDialog::OnApplyButton), NULL, this);
    wxButton *cancel = new wxButton(this, wxID_ANY, "Cancel");
    cancel->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(PlotSelectDialog::OnCancelButton), NULL, this);

    wxBoxSizer *bsizer = new wxBoxSizer(wxHORIZONTAL);
    bsizer->Add(apply, 0, wxALL, 5);
    bsizer->Add(cancel, 0, wxALL, 5);
    main->Add(bsizer, 0, wxALL, 5);

    this->SetSizer(main);
    main->SetSizeHints(this);
    this->CenterOnParent();
};

void PlotSelectDialog::OnApplyButton(wxCommandEvent &)
{
    this->EndModal(wxID_APPLY);
};

void PlotSelectDialog::OnCancelButton(wxCommandEvent&)
{
    this->EndModal(wxID_CANCEL);
};

void PlotSelectDialog::OnAllOptButton(wxCommandEvent &)
{
    std::vector<int> all;
    for (int i = 0; i < (int)_options.size(); i++)
        all.push_back(i);
    SetSelections(&all);
};

void PlotSelectDialog::OnNoneOptButton(wxCommandEvent &)
{
    SetSelections();
};

void PlotSelectDialog::OnAllAnnotButton(wxCommandEvent &)
{
    std::vector<int> all;
    for (int i = 0; i < (int)_annotations.size(); i++)
        all.push_back(i);
    SetAnnotations(&all);
};

void PlotSelectDialog::OnNoneAnnotButton(wxCommandEvent &)
{
    SetAnnotations();
};

std::vector<int> PlotSelectDialog::GetSelectionIds()
{
    std::vector<int> sel;
    for (size_t i = 0; i < _options.size(); i++)
        if (_options.at(i)->IsChecked())
            sel.push_back(i);
    return sel;
};

std::vector<int> PlotSelectDialog::GetAnnotationIds()
{
    std::vector<int> sel;
    for (size_t i = 0; i < _annotations.size(); i++)
        if (_annotations.at(i)->IsChecked())
            sel.push_back(i);
    return sel;
};

void PlotSelectDialog::SetSelections(std::vector<int> *indices)
{
    if (indices)
    {
        SetSelections();    //clear
        for (size_t i = 0; i < indices->size(); i++)
            if (indices->at(i) < _options.size())
                _options.at(indices->at(i))->SetValue(true);
    }
    else
    {
        for (size_t i = 0; i < _options.size(); i++)
            _options.at(i)->SetValue(false);
    }
    this->Update();
    this->Refresh();
};

void PlotSelectDialog::SetAnnotations(std::vector<int> *indices)
{
    if (indices)
    {
        SetAnnotations();    //clear
        for (size_t i = 0; i < indices->size(); i++)
            if (indices->at(i) < _annotations.size())
                _annotations.at(indices->at(i))->SetValue(true);
    }
    else
    {
        for (size_t i = 0; i < _annotations.size(); i++)
            _annotations.at(i)->SetValue(false);
    }
    this->Update();
    this->Refresh();
}
