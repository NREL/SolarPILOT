
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


#ifndef _PAR_EDIT_CTRLS_
#define _PAR_EDIT_CTRLS_ 1

#include <wx/wx.h>
#include <wx/treectrl.h>
#include <vector>
#include "gui_util.h"


#define EVT_NUMERIC(id, func) EVT_TEXT_ENTER(id, func)

class AFNumeric : public wxTextCtrl
{
public:
    AFNumeric( wxWindow *parent, int id, double val=0, 
        bool intonly=false,
        const wxPoint &pos = wxDefaultPosition,
        const wxSize &size = wxDefaultSize);

    void SetInt(int val);
    void SetDouble(double val);
    int GetInt();
    double GetDouble();

    void SetIntegersOnly(bool b=true);
    bool IntegersOnly();

    void SetFormat(const wxString &fmt);
    wxString GetFormat();
    void UseStdlibFormat(bool b);

private:
    void DoFormat();
    void Translate();
    void OnTextEnter(wxCommandEvent &evt);
    void OnSetFocus(wxFocusEvent &evt);
    void OnLoseFocus(wxFocusEvent &evt);
    void SetupValidator();
    wxString mFocusVal;
    wxString mFormat;
    bool bIntegersOnly;
    bool bUseStdlibFormat;
    int mIVal;
    double mDVal;

    DECLARE_EVENT_TABLE()
};

class AFLabel : public wxWindow
{
public:
    AFLabel( wxWindow *parent, int id, const wxString &caption = "", const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize);

    virtual wxSize DoGetBestSize();

    void AlignRight( bool b = true );
    void AlignTop( bool b = true );
    void AlignCenter( bool b = true );

    void SetCaption(const wxString &txt);
    wxString GetCaption();

    void SetColour(const wxColour &c);
    wxColour GetColour();
    
    void SetBold(bool b);
    bool IsBold();

    void SetWordWrap(bool b);
    bool IsWordWrapped();

    void SetRelativeSize(int sz=0);
    int GetRelativeSize();

private:
    bool mBold;
    int mRelSize;
    bool bTop;
    bool bRight;
    bool bWordWrap;
    wxString mCaption;
    wxColour mColour;
    
    void OnPaint(wxPaintEvent &evt);
    void OnResize(wxSizeEvent &evt);

    DECLARE_EVENT_TABLE()
    
};

// emits ITEM_ACTIVATED for item check change
class AFTreeView : public wxTreeCtrl
{
public:

    enum {ICON_CHECK_FALSE, ICON_CHECK_TRUE, ICON_JUMPTO, ICON_ADD, ICON_REMOVE, ICON_RARROW, ICON_JUSTIFY, ICON_FOLDER, ICON_FILE, ICON_BROKEN_LINK};

    AFTreeView( wxWindow *parent, int id, wxString imagedir, const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize);

    void EnableCheckMode(bool b);
    bool IsCheckMode();
    void Check(const wxTreeItemId &item, bool b);
    bool IsChecked(const wxTreeItemId &item);

private:
    void OnLClick(wxMouseEvent &evt);
    bool bCheckMode;

    DECLARE_EVENT_TABLE();
};

class AFSearchListBox : public wxPanel
{
public:
    AFSearchListBox( wxWindow *parent, int id, const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize );
    void Append( const wxString &s );
    void Append( const wxArrayStr &s );
    int GetSelection();
    int Count();
    wxString GetItem( int i );
    void Clear();
    wxString GetStringSelection();
    void SetSelection( int i );
    void SetStringSelection(const wxString &s);

    void SetPromptText( const wxString & );

private:
    wxTextCtrl *m_txtFilter;
    wxStaticText *m_label;
    wxStaticText *m_notifyLabel;
    wxListBox *m_list;

    struct item {
        item() : str(wxEmptyString), shown(false) {  }
        item( const wxString &s, bool sh ) : str(s), shown(sh) {  }
        wxString str;
        bool shown;
    };
	std::vector<item> m_items;
    
    void UpdateView();
    void SendEvent();
    void OnFilter( wxCommandEvent & );
    void OnSelect( wxCommandEvent & );
    DECLARE_EVENT_TABLE();
};

class AFTextCtrl : public wxTextCtrl
{
public:
    AFTextCtrl( wxWindow *parent, int id, const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, long style = 0);

    void SendEventOnLoseFocus(bool b) { bSendOnFocus = b; }

protected:
    void OnLoseFocus(wxFocusEvent &evt);
    void OnSetFocus(wxFocusEvent &evt);

private:
    bool bSendOnFocus;
    wxString m_origVal;
    DECLARE_EVENT_TABLE()
};

class Painter
{
public:
    enum ArrowType { UP, DOWN, LEFT, RIGHT };

    //Replacement WordWrap function:
    static int WordWrap(wxDC& dc, const wxString &str, int width, bool draw=false, int x=0, int y=0, wxArrayStr *lines=NULL);
    
    //Replacement:
    static void DrawRaisedPanel(wxDC &dc, int x, int y, int width, int height);
    static void DrawSunkenPanel(wxDC &dc, int x, int y, int width, int height);
    static void DrawEngravedPanel(wxDC &dc, int x, int y, int width, int height, bool fill);
    static void DrawScrollBar(wxDC &dc, bool vertical, int x, int y, int width, int height);
    static void DrawArrowButton(wxDC &dc, ArrowType type, int x, int y, int width, int height);
    static void DrawArrow(wxDC &dc, ArrowType type, int x, int y, int width, int height);

    
    static wxFont GetNormalFont(bool b=true, int sz=-1);
    static wxFont GetArialFont(bool b=true, int sz=-1); //Keep this around.
};
#endif