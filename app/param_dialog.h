
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


#ifndef _PARAM_DIALOG_
#define _PARAM_DIALOG_ 1

#include <vector>

#include <wx/wx.h>
#include "wx/treectrl.h"

#include "mod_base.h"
#include "par_edit_ctrls.h"
#include "gui_util.h"

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

    std::vector<item_info> m_items;
    
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
    void SetItems(var_map *V, int type_filter = -1, bool with_outputs = false);
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