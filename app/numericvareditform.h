#ifndef __NumericVarEditForm_h
#define __NumericVarEditForm_h 1

#include <wx/wx.h>
#include "par_edit_ctrls.h"

class NumericVarEditForm : public wxPanel
{
public:
    NumericVarEditForm(wxWindow *parent, int id=-1);
    virtual ~NumericVarEditForm();

    wxStaticBox *GroupBox1;
    wxStaticBox *GroupBox2;
    wxButton *cmdCancel;
    wxButton *cmdOk;
    wxButton *cmdUpdateValues;
    AFLabel *lblNotification;
    wxButton *cmdMoveDown;
    wxListBox *lstValues;
    wxButton *cmdRemove;
    wxButton *cmdAddBefore;
    wxButton *cmdAddAfter;
    wxButton *cmdMoveUp;
    AFLabel *Label3;
    AFLabel *Label2;
    AFLabel *Label1;
    AFNumeric *numIncr;
    AFNumeric *numEnd;
    AFNumeric *numStart;
    wxComboBox *layoutCombo;

    void SetValues(const wxArrayStr &values, bool int_only=false);
    void OnCommand(wxCommandEvent &evt);
    void GenerateValues();
    bool CheckRanges();
    int GetVarOptimizationType();
    void SetVarOptimizationType(int index);
    bool bIntOnly;

    DECLARE_EVENT_TABLE()
};

class NumericVarEditFormDialog : public wxDialog
{
public:
    NumericVarEditFormDialog(wxWindow *parent, const wxString &title, void *data = NULL);

    NumericVarEditForm *GetPanel();
    void OnCommand(wxCommandEvent &evt);
    void OnClose(wxCloseEvent &evt);

private:
    NumericVarEditForm *mPanel;
    DECLARE_EVENT_TABLE()
};

#endif

