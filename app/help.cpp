#include "help.h"
//http://docs.wxwidgets.org/trunk/overview_html.html#overview_html_helpformats

using namespace std;

HelpFrame::HelpFrame(const wxString &path, const wxString &title, const wxSize size)
	: wxFrame((wxFrame *)NULL, wxID_ANY, title, wxDefaultPosition, size)
	, m_embeddedHtmlHelp(wxHF_EMBEDDED|wxHF_DEFAULT_STYLE)
{
	m_embeddedHelpWindow = new wxHtmlHelpWindow;
    // m_embeddedHtmlHelp.UseConfig(config, rootPath); // Can set your own config object here
    m_embeddedHtmlHelp.SetHelpWindow(m_embeddedHelpWindow);

    m_embeddedHelpWindow->Create(this,
        wxID_ANY, wxDefaultPosition, GetClientSize(), wxTAB_TRAVERSAL|wxNO_BORDER, wxHF_DEFAULT_STYLE);

    m_embeddedHtmlHelp.AddBook(wxFileName(path+wxT("doc.zip")));
    m_embeddedHtmlHelp.Display(wxT("Introduction"));


}
