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