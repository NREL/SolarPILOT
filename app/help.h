#ifndef _HELP_
#define _HELP_ 1
#include <wx/wx.h>
#include <wx/html/helpctrl.h>
#include <wx/cshelp.h>
#include <wx/filesys.h>
#include <wx/fs_zip.h>
#ifdef _MSC_VER
#include <wx/msw/helpchm.h>
#endif

//Id's for 

class HelpFrame : public wxFrame
{
	wxHelpController m_help;
	wxHtmlHelpController     m_advancedHtmlHelp;
	wxHtmlHelpController     m_embeddedHtmlHelp;
	wxHtmlHelpWindow*        m_embeddedHelpWindow;
public:
	HelpFrame(const wxString &path, const wxString &title, const wxSize size=wxDefaultSize);
	wxHelpControllerBase &GetHelpController(){ return m_help; }
	wxHtmlHelpController& GetAdvancedHtmlHelpController() { return m_advancedHtmlHelp; }

	int OnExit(){
		delete wxHelpProvider::Set(NULL);
		return 0;
	}

};



#endif