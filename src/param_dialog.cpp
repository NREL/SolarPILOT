#include "param_dialog.h"
#include "numericvareditform.h"
#include <wx/treectrl.h>

enum {
  ID_txtSearch,
  ID_tree,
  ID_btnUncheckAll,
  ID_btnExpandAll };

BEGIN_EVENT_TABLE( par_variables_dialog, wxDialog )
	EVT_BUTTON(ID_btnUncheckAll, par_variables_dialog::OnUncheckAll)
	EVT_BUTTON(ID_btnExpandAll, par_variables_dialog::OnExpandAll)
	EVT_TEXT(ID_txtSearch, par_variables_dialog::OnSearch )
	EVT_TREE_ITEM_ACTIVATED(ID_tree, par_variables_dialog::OnTree)
END_EVENT_TABLE()

par_variables_dialog::par_variables_dialog(wxWindow *parent, wxWindowID winid, wxString imagedir, bool value_mode, wxString label,wxPoint pos,wxSize size,long style)
		: wxDialog(parent, winid, label, pos, size, style)
{
	//setup
	_value_mode = value_mode;

	wxBoxSizer *search_sizer = new wxBoxSizer( wxHORIZONTAL );
	search_sizer->Add( new wxStaticText(this, wxID_ANY, "  Search: "), 0, wxALL|wxALIGN_CENTER_VERTICAL, 4);
	txtSearch = new AFTextCtrl(this, ID_txtSearch);
	search_sizer->Add( txtSearch, 1, wxALL|wxEXPAND, 4 );
	
	wxBoxSizer *button_sizer = new wxBoxSizer(wxHORIZONTAL);
	button_sizer->Add(  new wxButton(this, ID_btnExpandAll, "Expand All"), 0, wxALL|wxEXPAND, 4 );
	button_sizer->Add(  new wxButton(this, ID_btnUncheckAll, "Uncheck All"), 0, wxALL|wxEXPAND, 4 );
	button_sizer->AddStretchSpacer();
	button_sizer->Add( new wxButton(this, wxID_OK), 0, wxALL|wxEXPAND, 4  );
	button_sizer->Add( new wxButton(this, wxID_CANCEL), 0, wxALL|wxEXPAND, 4  );

	tree = new AFTreeView(this, ID_tree, imagedir);
	wxStaticText *layout_lab;
	if(_value_mode){
		wxArrayStr val_choices;
		val_choices.Add(wxT("Keep existing layout"));
		val_choices.Add(wxT("Regenerate field layout"));
		layout_lab = new wxStaticText(this, wxID_ANY, wxT("Field layout requirement:"));
		//layout_lab->SetForegroundColour(wxColor(255, 0, 0));
		_layout_combo = new wxComboBox(this, wxID_ANY, val_choices[0], wxDefaultPosition, wxDefaultSize, val_choices, wxCB_DROPDOWN|wxCB_READONLY);
	}
	
	wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
	sizer->Add( search_sizer, 0, wxALL|wxEXPAND, 0 );
	sizer->Add( tree, 1, wxALL|wxEXPAND, 4 );
	if(_value_mode){
		wxBoxSizer *layout_sizer = new wxBoxSizer(wxHORIZONTAL);
		layout_sizer->Add(layout_lab, 0, wxRIGHT|wxALIGN_CENTER_VERTICAL, 10);
		layout_sizer->Add(_layout_combo, 0, wxEXPAND, 0);
		sizer->Add( layout_sizer, 0, wxALL|wxALIGN_RIGHT, 4);
	}
	sizer->Add( button_sizer, 0, wxALL|wxEXPAND, 0 );

	SetSizer( sizer );
	SetEscapeId( wxID_CANCEL );
	
	return;
}

void par_variables_dialog::SetVarOptimizationType(int index){if(_layout_combo) _layout_combo->SetSelection(index); }

void par_variables_dialog::UpdateTree(){
	//tree->Freeze();
	tree->DeleteAllItems();

	m_root = tree->AddRoot("Available Variables",
		AFTreeView::ICON_REMOVE,AFTreeView::ICON_REMOVE);
	tree->SetItemBold(m_root);
	wxTreeItemId cur_parent;
	wxString cur_context = wxEmptyString;

	for (int i=0;i < (int)m_items.size();i++)
	{
		m_items.at(i).tree_id.Unset();

		if ( !m_items.at(i).shown && !m_items.at(i).checked ) continue;

		wxString cxt = m_items.at(i).context;
		wxString lbl = m_items.at(i).label;

		if (cur_context != cxt)
		{
			cur_context = cxt;
			cur_parent = tree->AppendItem(m_root, cur_context);
			tree->SetItemBold(cur_parent);
		}
		
		
		if (lbl.IsEmpty())
			lbl = m_items.at(i).label;

		if (cur_parent.IsOk())
			m_items.at(i).tree_id = tree->AppendItem( cur_parent, lbl,AFTreeView::ICON_CHECK_FALSE,-1 );
		else
			m_items.at(i).tree_id = tree->AppendItem( m_root, lbl, AFTreeView::ICON_CHECK_FALSE, -1 );

		if ( m_items.at(i).checked )
			tree->Check( m_items.at(i).tree_id, true );
	}

	tree->Expand(m_root);
	tree->UnselectAll();
	//tree->Thaw();

}

void par_variables_dialog::SetItems(var_map *V, int type_filter){

	m_items.clear();

    unordered_map<string, string> class_map;
    class_map["ambient"] = V->amb.class_name.val;
#if _CUSTOM_REC
    class_map["custom_rec"] = V->crec.class_name.val;
#endif
    class_map["financial"] = V->fin.class_name.val;
    class_map["fluxsim"] = V->flux.class_name.val;
    class_map["heliostat"] = V->hels[0].class_name.val;
    class_map["land"] = V->land.class_name.val;
    class_map["optimize"] = V->opt.class_name.val;
    class_map["parametric"] = V->par.class_name.val;
    class_map["plant"] = V->plt.class_name.val;
    class_map["receiver"] = V->recs[0].class_name.val;
    class_map["solarfield"] = V->sf.class_name.val;

    //Collect class names into a list
    vector<string> class_names;
    vector<string> class_labs;
    for( unordered_map<string, string>::iterator it=class_map.begin(); it!=class_map.end(); it++)
    {
        class_names.push_back( it->first );
        class_labs.push_back( it->second );
    }
    quicksort(class_labs, class_names);

    //create sortable lists for each class name individually
    unordered_map<string, vector<string> > class_list_vars;
    unordered_map<string, vector<string> > class_list_labs;
    for( int i=0; i<class_names.size(); i++)
    {
        class_list_vars[ class_names.at(i) ] = vector<string>();
        class_list_labs[ class_names.at(i) ] = vector<string>();
    }
    
    //populate lists
    for( unordered_map<string, spbase*>::iterator var=V->_varptrs.begin(); var != V->_varptrs.end(); var++)
    {
		if( ! var->second->is_param || var->second->is_disabled || var->second->short_desc.empty() )
            continue;
        string cname = (split(var->first, ".")).at(0);
        
        class_list_vars[ cname ].push_back( var->first );
        class_list_labs[ cname ].push_back( var->second->short_desc );
    }

    //sort each class list
    for( int i=0; i< class_names.size(); i++)
    {
        quicksort(class_list_labs[class_names[i]], class_list_vars[ class_names[i] ]);
    }

    //get all names in order
    vector<string> all_names;
    for( int i=0; i<class_names.size(); i++)
        for( int j=0; j<class_list_vars[class_names[i]].size(); j++)
            all_names.push_back(class_list_vars[class_names[i]][j]);

    //for( unordered_map<string, spbase*>::iterator var=V->_varptrs.begin(); var != V->_varptrs.end(); var++)
    for( int i=0; i<all_names.size(); i++)
    {
        spbase* var = V->_varptrs[all_names[i]];

        if( ! var->is_param || var->short_desc.empty() || var->is_disabled)
            continue;

        if( type_filter != -1 )
            if( type_filter != var->dattype )
                continue;

        m_items.push_back(item_info());
		m_items.back().tree_id = 0;
		m_items.back().name = all_names[i];
		m_items.back().label = var->short_desc;
		m_items.back().shown = true;
		m_items.back().context = class_map[ (split(all_names[i], ".")).at(0) ];
		m_items.back().checked = false;
    }

	/*for(var_set::iterator cls=V->begin(); cls != V->end(); cls++){
		for(std::map<int,var_map>::iterator inst = cls->second.begin(); inst != cls->second.end(); inst++){
			for(var_map::iterator var = inst->second.begin(); var != inst->second.end(); var++){
				if(!var->second.is_param || var->second.short_desc == "") continue;
				m_items.push_back(item_info());
				m_items.back().tree_id = 0;
				m_items.back().name = var->second.varpath;
				m_items.back().label = var->second.short_desc;
				m_items.back().shown = true;
				m_items.back().context = cls->second[0]["class_name"].value;
				m_items.back().checked = false;
			}
		}
	}*/
	UpdateTree();
	txtSearch->SetFocus();

}

void par_variables_dialog::SetItems(const wxArrayStr &names, const wxArrayStr &labels, const wxArrayStr &contexts){
	if ( names.Count() != labels.Count() || names.Count() != contexts.Count()) return;

	m_items.resize( names.Count() );
	for (size_t i=0;i<names.Count();i++)
	{
		m_items[i].name = names[i];
		m_items[i].label = labels[i];
		m_items[i].tree_id = 0;
		m_items[i].shown = true;
		m_items[i].checked = false;
		m_items[i].context = contexts[i];
	}

	UpdateTree();
	txtSearch->SetFocus();
}

void par_variables_dialog::ShowAllItems()
{
	tree->ExpandAll();
	tree->UnselectAll();
	tree->ScrollTo(this->m_root);
}

void par_variables_dialog::OnSearch( wxCommandEvent & WXUNUSED(evt))
{
	wxString filter = txtSearch->GetValue().Lower();

	if (filter.IsEmpty())
	{
		for (int i=0;i<(int)m_items.size();i++)
			m_items.at(i).shown = true;
	}
	else
	{
		for (int i=0;i<(int)m_items.size();i++)
		{
			if (filter.Len() <= 2 && m_items.at(i).label.Left( filter.Len() ).Lower() == filter)
				m_items.at(i).shown = true;
			else if (m_items.at(i).label.Lower().Find( filter ) >= 0)
				m_items.at(i).shown = true;
			else if (m_items.at(i).name.Lower().Find( filter ) == 0)
				m_items.at(i).shown = true;
			else
				m_items.at(i).shown = false;
		}
	}

	UpdateTree();

	if ( !filter.IsEmpty() )
	{
		//tree->Freeze();
		tree->ExpandAll();
		tree->EnsureVisible( m_root );
		//tree->Thaw();
	}
	else
	{
		//tree->Freeze();
		for (int i=0;i<(int)m_items.size();i++)
		{
			if ( m_items.at(i).tree_id.IsOk() && m_items.at(i).checked )
				tree->Expand( tree->GetItemParent( m_items.at(i).tree_id ));
		}
		tree->EnsureVisible( m_root );
		//tree->Thaw();
	}
}

void par_variables_dialog::OnTree(wxTreeEvent &evt)
{
	wxTreeItemId item = evt.GetItem();	
	for (int i=0;i<(int)m_items.size();i++)
		if (m_items.at(i).tree_id == item)
			m_items.at(i).checked = tree->IsChecked( m_items.at(i).tree_id );

	evt.Skip();
}

void par_variables_dialog::SetCheckedNames(const wxArrayStr &list)
{
	for (int i=0;i<(int)m_items.size();i++)
	{
		m_items.at(i).checked = (list.Index( m_items.at(i).name ) >= 0);

		if (m_items.at(i).tree_id.IsOk())
		{
			tree->Check( m_items.at(i).tree_id, m_items.at(i).checked );
			if ( m_items.at(i).checked )
				tree->EnsureVisible( m_items.at(i).tree_id );
		}
	}

	if (m_root.IsOk())
		tree->EnsureVisible(m_root);
}

wxArrayStr par_variables_dialog::GetCheckedNames()
{
	wxArrayStr list;
	for (int i=0;i<(int)m_items.size();i++)
		if ( m_items.at(i).checked )
			list.Add( m_items.at(i).name );

	return list;
}

int par_variables_dialog::GetVarOptimizationType(){
	//only in var_value mode. [0=Keep existing, 1=Regenerate layout, -1=Error]
	if(!_value_mode) return -1;
	return _layout_combo->GetSelection();
}

void par_variables_dialog::OnExpandAll(wxCommandEvent &WXUNUSED(evt))
{
	tree->ExpandAll();
	if (m_root.IsOk())
		tree->EnsureVisible(m_root);
}

void par_variables_dialog::OnUncheckAll(wxCommandEvent &WXUNUSED(evt))
{
	for (int i=0;i<(int)m_items.size();i++)
		m_items.at(i).checked = false;
	UpdateTree();
}

//*************** parametric values dialog *****************
par_values_dialog::par_values_dialog(wxWindow *parent, wxWindowID winid, wxString imagedir, wxString label, wxPoint pos, wxSize size, long style)
	//: wxDialog(parent, winid, label, pos, size, style)
{
	_image_dir = imagedir;
	_parent = parent;
	_winid = winid;
	_label = label;
	_pos = pos;
	_size = size;
	_style = style;
	_layout_required = false;
}

bool par_values_dialog::IsLayoutRequired(){return _layout_required;}

bool par_values_dialog::ShowEditValuesDialog(const wxString &title, wxArrayStr &values, par_variable *v){
		if (!v)
		return false;

	int i;
	string vtype = v->data_type;
	_layout_required = v->layout_required;

	if ( vtype == "combo"
		|| vtype == "checkbox"
		|| vtype == "string"
		|| vtype == "location")
	{
		// fixed domain selection (combo box, list, checkbox, location)
		wxArrayStr fixed_items = v->choices; //Split(v->GetExpression(), ",");
		wxArrayStr cur_items, contexts;
		for (i=0;i<(int)values.Count();i++)
		{
			//int item_i = atoi( values[i].c_str() );
			//if (item_i >= 0 && item_i < (int) fixed_items.Count())
			cur_items.Add( values[i]); //fixed_items[i] );
		}
		for(i=0; i<(int)fixed_items.Count(); i++){
			contexts.Add( v->display_text );
		}

		if (ShowFixedDomainDialog(title, fixed_items, contexts, cur_items))
		{
			// translate back to integer values
			values.Clear();
			for (int i=0;i<(int)cur_items.Count();i++)
				//values.Add( wxString::Format("%d", fixed_items.Index( cur_items[i] ) ) );
				values.Add(cur_items[i]);

			return true;
		}
		else
			return false;
	}
	else if ( vtype == "int" || vtype == "double") 
	{
		return ShowNumericValuesDialog(title, values, vtype == "int");
	}
	

	wxMessageBox("Could not edit values for \"" + v->display_text + "\" (domain type error)");
	return false;

}

bool par_values_dialog::ShowNumericValuesDialog(const wxString &title, wxArrayStr &values, bool int_only)
{
	NumericVarEditFormDialog dlg(_parent, title);
	NumericVarEditForm *frm = dlg.GetPanel();
	frm->SetValues( values, int_only );
	frm->SetVarOptimizationType( _layout_required ? 1 : 0 );

	if (dlg.ShowModal() == wxID_OK)
	{
		values = frm->lstValues->GetStrings();
		_layout_required = frm->GetVarOptimizationType() == 1;
		return true;
	}
	else
		return false;
}

bool par_values_dialog::ShowFixedDomainDialog(const wxString &title, const wxArrayStr &items, const wxArrayStr &contexts, wxArrayStr &sel)
{
	return ShowSelectVariableDialog(title, items, items, contexts, sel, true);
}

bool par_values_dialog::ShowSelectVariableDialog(const wxString &title, const wxArrayStr &names, const wxArrayStr &labels, const wxArrayStr &contexts, wxArrayStr &list, bool expand_all)
{
	par_variables_dialog dlg(_parent, wxID_ANY, _image_dir, true, wxT("Select variable values"));
	dlg.SetSize(450,550);
	dlg.SetItems( names, labels, contexts );
	dlg.SetCheckedNames( list );
	dlg.SetVarOptimizationType( _layout_required ? 1 : 0 );
	//dlg.SetCheckedNames( list );
	if (expand_all)
		dlg.ShowAllItems();

	if (dlg.ShowModal() == wxID_OK)
	{
		wxArrayStr names = dlg.GetCheckedNames();
		
		// remove any from list
		int i=0;
		while (i<(int)list.Count())
		{
			if (names.Index( list[i] ) < 0)
				list.RemoveAt(i);
			else
				i++;
		}

		// append any new ones
		for (i=0;i<(int)names.Count();i++)
		{
			if (list.Index( names[i] ) < 0)
				list.Add( names[i] );
		}

		//check layout selection
		_layout_required = dlg.GetVarOptimizationType() == 1;

		return true;
	}
	else
		return false;
}