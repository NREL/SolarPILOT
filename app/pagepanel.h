#ifndef _PAGEPANEL
#define _PAGEPANEL 1

#include <wx/wx.h>
#include <wx/imaglist.h>
#include <vector>
#include <unordered_map>

using std::unordered_map;

class PagePanel : public wxScrolledWindow
{
    int m_active_page;
    int m_pageheight;
    int m_leveloffset;
    wxImageList *_image_list;
    wxWindow *m_parent;

    struct Page
    {
        wxBitmap icon;
        wxString label;
        wxWindow *page;
        int page_level;
        int page_position;
        wxString group_owner;
    };

    std::vector<Page> m_pages;
    unordered_map< std::string, int > m_pages_by_name;
    
    DECLARE_EVENT_TABLE()
public:
    PagePanel(wxWindow *parent, wxWindowID id=wxID_ANY, const wxPoint pos = wxDefaultPosition, const wxSize size = wxDefaultSize);
    void OnClick(wxMouseEvent &evt);
    void SetActivePage(int pagenum);
    void SetActivePage( wxString pagename );
    void RenamePage( wxString page_old, wxString page_new );
    wxWindow *GetActivePage();
    std::string GetActivePageName();
    void DestroyPages();
    void DestroyPage(wxString pagename);
    void SetImageList( wxImageList *images );
    void AddPage( wxWindow *input_page, wxString pagename, int icon_id, int page_level=0, wxString page_group=wxEmptyString);
    void InsertPage( int pagepos, wxWindow *input_page, wxString pagename, int icon_id, int page_level=0, wxString page_group=wxEmptyString);
    wxWindow* GetPage( wxString pagename );
    wxWindow* GetPage( int order_index );
    int GetPagePosition( wxString pagename );
    int GetPageCount();
    void GetPageDrawingSize(int &height, int &width);
    std::string GetPageGroupOwner( int order_index );
    std::string GetPageName( int order_index );
    void MovePage(wxString pagename, int positions_up);
    void OnPaint( wxPaintEvent &event );
};



#endif
