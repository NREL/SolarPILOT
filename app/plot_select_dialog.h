#ifndef _PLOT_SELECT_DLG_
#define _PLOT_SELECT_DLG_

#include <string>
#include <vector>
#include <wx/wx.h>

class PlotSelectDialog : public wxDialog
{
    std::vector< wxCheckBox* > _options;
    std::vector< wxCheckBox* > _annotations;

public:

    enum ANNOT { DATE_TIME = 0, SUN_POSITION, DNI, TOTAL_EFF, COSINE_EFF, BLOCK_EFF, ATT_EFF, INT_EFF, TOT_POWER, AIM_METHOD, VARIABLES };

    PlotSelectDialog(wxWindow* parent, wxWindowID id, wxString label,
        std::vector<std::string> options, wxPoint pos = wxDefaultPosition, wxSize size = wxDefaultSize,
        long style = wxDEFAULT_DIALOG_STYLE | wxSTAY_ON_TOP | wxRESIZE_BORDER);

    void OnApplyButton(wxCommandEvent &);
    void OnCancelButton(wxCommandEvent&);
    void OnAllOptButton(wxCommandEvent &);
    void OnNoneOptButton(wxCommandEvent &);
    void OnAllAnnotButton(wxCommandEvent &);
    void OnNoneAnnotButton(wxCommandEvent &);
    std::vector<int> GetSelectionIds();
    std::vector<int> GetAnnotationIds();
    void SetSelections(std::vector<int> *indices = 0);
    void SetAnnotations(std::vector<int> *indices = 0);

};


#endif