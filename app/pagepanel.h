
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
