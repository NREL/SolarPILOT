
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


#ifndef _INPUTCONTROL_
#define _INPUTCONTROL_ 1

#include <stdarg.h>

#include "wx/wx.h"
#include "gui_util.h"
#include "definitions.h"
#include "mod_base.h"    //From the performance code.. need the variable map structure

/*
This is a custom input control that provides a single object for input, units labels, range enforcement, and variable naming.
This control is built to interface with the performance code variable array.
*/



class InputControl : public wxPanel /*, public mod_base*/
{
    
private:
    //Declare constituent components
    spbase *_varobj;
    std::string 
        _units,        //The units to display in the interface
        _value,        //Value to set in the interface
        _oldtext;    //Previous value for difference checking
    wxWindow *_parent;
    wxSize _size;
    wxToolTip *tip;
    bool _need_update;
    bool _text_only;
    bool _disabled;
    wxColour _hcolor;    //highlight color
    std::vector<wxWindow*> 
        _all_dis_objs;    //a list of all associated disabled objects
    //Objects
    wxTextCtrl *tc;
    wxComboBox *cb;
    wxCheckBox *chk;
    wxStaticText *st_units, *st_label;


    struct choice_helper
    {
        wxPanel *panel;
        bool has_panel;
        std::vector<wxWindow*> dis_sibs;
    };

public:
    bool is_binding_set;    //flag to track whether this control has already had its bindings set

    unordered_map<std::string, choice_helper> choicedat;    //vector panel information corresponding to each combo choice in _varobj, if applicable

     InputControl(wxWindow *parent,
                 wxWindowID winid,
                 spbase &var,
                 const wxPoint pos = wxDefaultPosition,
                 const wxSize size = wxDefaultSize, // wxSize(320, 21),
                 long style = 0,
                 const wxString &name = "none",
                 bool textonly = false,
                 bool hide = false);
    ~InputControl();

    //Accessors
    wxString getUnits();
    wxString getVarname();
    wxString getValue();
    std::string getVarPath();
    std::string getSelectedText();
    int getSelection();
    bool needUpdate();
    void setPanelObject(std::string choice, wxPanel &panel);
    std::vector<std::string> getComboChoices();
    spbase *getVarObject();
    bool isCombo();
    bool isCheckbox();
    bool isFilePath();
    bool isOverride();

    /*
    Objects can be associated for enable/disable control either using their string tag or by directly
    passing a pointer to the object. 

    addDisabledSiblingObj()        Track pointers to wxWindow-derived controls for enabling/disabling

    setDisabledSiblings()        Track the var_set name path to the corresponding object
    */
    void updateInputDisplay();
    void addDisabledSiblings(std::string sel, int n, wxWindow *p[]);
    void addDisabledSiblings(std::string sel, wxWindow *p);
    void setDisabledSiblings(std::string sel, int n, wxWindow* p[]);
    void setDisabledSiblings(std::string sel, wxWindow* p);
    std::vector<wxWindow*> *getDisabledSiblings(std::string &choice);
    std::vector<wxWindow*> *getAllDisabledSiblings();
    
    void setColour(wxColour c=wxColour(255,255,255));
    void setValue(bool val);
    void setValue(int val);
    void setValue(std::string &val);
    void setValue(double &val, std::string fmt = "%.1f");
    void selectAllText();
    void setVarObject(spbase* varobj);
    void setTextOnlyMode(bool mode);
    void updateComboChoices();
    
protected:
    void OnText( wxCommandEvent &WXUNUSED(event));
    void OnCombo( wxCommandEvent &event);
    void OnCheck( wxCommandEvent &event);
    void OnFile( wxCommandEvent &event);
    void OnFocus( wxFocusEvent &WXUNUSED(event));
    void OnLeaving( wxFocusEvent &event);
    void OnBGPaintEvent( wxEraseEvent &event);
    void OnPaint( wxPaintEvent &event);
    void OnMouseWheelSkip(wxCommandEvent &event); // catch mouse scroll events on combobox's here
    void Build();
};


#endif