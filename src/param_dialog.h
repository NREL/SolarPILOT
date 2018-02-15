#ifndef _PARAM_DIALOG_
#define _PARAM_DIALOG_ 1
#include <wx/wx.h>
#include "mod_base.h"
#include "wx/treectrl.h"
#include "par_edit_ctrls.h"
#include "gui_util.h"
#include <vector>
using namespace std;

class AFTreeView;
class AFSearchListBox;


class par_variables_dialog : public wxDialog
{
    /* 
    Create the dialog for selecting which values will be included in the parametric simulation.
    */
    
    AFTreeView *tree;
    AFTextCtrl *txtSearch;
    wxTreeItemId m_root;
    var_map *_V;
    bool _value_mode;
    wxComboBox *_layout_combo;

    struct item_info
    {
        wxString name;
        wxString label;
        wxString context;
        wxTreeItemId tree_id;
        bool shown;
        bool checked;
    };

    vector<item_info> m_items;
    
    void OnUncheckAll(wxCommandEvent &evt);
    void OnExpandAll(wxCommandEvent &evt);
    void OnSearch( wxCommandEvent & );
    void OnTree(wxTreeEvent &evt);
    
    DECLARE_EVENT_TABLE()
public:
    par_variables_dialog()
    {};
        par_variables_dialog(wxWindow *parent, wxWindowID winid, wxString imagedir, bool value_mode, wxString label, 
        wxPoint pos = wxDefaultPosition, wxSize size = wxDefaultSize, 
        long style=wxDEFAULT_DIALOG_STYLE|wxSTAY_ON_TOP|wxRESIZE_BORDER);
    
    void UpdateTree();
    void SetItems(var_map *V, int type_filter = -1);
    void SetItems(const wxArrayStr &names, const wxArrayStr &labels, const wxArrayStr &contexts);
    wxArrayStr GetCheckedNames();
    int GetVarOptimizationType();
    void SetVarOptimizationType(int index);
    void ShowAllItems();
    void SetCheckedNames(const wxArrayStr &list);
};

class par_values_dialog 
{
    /* 
    Create the dialog where the user specifies the values of parameterized variables.
    */
    wxString _image_dir;
    wxWindow *_parent;
    wxWindowID _winid;
    wxString _label;
    wxPoint _pos;
    wxSize _size;
    long _style;
    bool _layout_required;

public:
    par_values_dialog()
    {};
        par_values_dialog(wxWindow *parent, wxWindowID winid, wxString imagedir, wxString label, 
        wxPoint pos = wxDefaultPosition, wxSize size = wxDefaultSize, 
        long style=wxDEFAULT_DIALOG_STYLE|wxSTAY_ON_TOP|wxRESIZE_BORDER);

    bool ShowEditValuesDialog(const wxString &title, wxArrayStr &values, par_variable *v);
    bool ShowNumericValuesDialog(const wxString &title, wxArrayStr &values, bool int_only);
    bool ShowFixedDomainDialog(const wxString &title, const wxArrayStr &items, const wxArrayStr &contexts, wxArrayStr &sel);
    bool ShowSelectVariableDialog(const wxString &title, const wxArrayStr &names, const wxArrayStr &labels, const wxArrayStr &contexts, wxArrayStr &list, bool expand_all=false);
    bool IsLayoutRequired();
};



#endif