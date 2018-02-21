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