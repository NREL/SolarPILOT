
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


#include "pagepanel.h"



PagePanel::PagePanel(wxWindow *parent, wxWindowID id, const wxPoint pos, const wxSize size)
:   wxScrolledWindow(parent, id, pos, size)
{
    /* 
    Build frame and do top-level layout. 

    This doesn't draw the controls on the canvas, that's handled in OnDraw()
    */

    m_pageheight = 34;
    m_active_page = 0;
    m_leveloffset = 20;
    m_parent = parent;

    DestroyPages();

    return; 
}


void PagePanel::OnClick(wxMouseEvent &evt)
{
    int ypos = evt.GetPosition().y;
    int px, py;
    this->GetScrollPixelsPerUnit(&px,&py);

    int yscroll = this->GetScrollPos(wxVERTICAL)*py;

    int npage = (int)( (double)(ypos + yscroll) / (double)m_pageheight );
    if( npage < m_pages.size() )
        SetActivePage( npage );
    
    
    m_parent->Layout();
    m_parent->Update();
    m_parent->Refresh();

    wxYield();
}

void PagePanel::SetActivePage(int pagenum)
{
    m_active_page = pagenum;
    for(int i=0; i<m_pages.size(); i++)
    {
        if(m_pages.at(i).page->IsShown() != (i==pagenum) )
            m_pages.at(i).page->Show( i==pagenum );
    }
    this->Update();
    this->Refresh();
}


void PagePanel::SetActivePage(wxString pagename)
{
    for(int i=0; i<m_pages.size(); i++)
    {
        bool comptest = m_pages.at(i).label == pagename;

        if(m_pages.at(i).page->IsShown() != comptest )
            m_pages.at(i).page->Show( comptest );

        if(comptest)
            m_active_page = i;
    }
    this->Update();
    this->Refresh();
}

void PagePanel::RenamePage(wxString page_old, wxString page_new )
{
    int page_loc = m_pages_by_name[ (std::string)page_old ];
    Page *page = &m_pages.at( page_loc );
    page->label = page_new;

    m_pages_by_name.erase( m_pages_by_name.find( (std::string)page_old) );
    m_pages_by_name[ (std::string)page_new ] = page_loc;

    this->Update();
    this->Refresh();
}

wxWindow *PagePanel::GetActivePage()
{
    return m_pages.at(m_active_page).page;
}

std::string PagePanel::GetActivePageName()
{
    return (std::string)m_pages.at( m_active_page ).label;
}

void PagePanel::DestroyPages()
{
    //call destructors on any existing input pages
    for(int i=0; i<(int)m_pages.size(); i++)
    {
        m_pages.at(i).page->Destroy();
    }
    
    //clean up the pages vectors
    m_pages.clear();
    m_pages_by_name.clear();

}

void PagePanel::DestroyPage(wxString pagename)
{
    if( m_pages_by_name.find( (std::string)pagename ) == m_pages_by_name.end() )
        return;
    
    int idestr = m_pages.at( m_pages_by_name[ (std::string)pagename ] ).page_position;
    
    m_pages.at( m_pages_by_name[ (std::string)pagename ] ).page->Destroy();
    m_pages.erase( m_pages.begin() + idestr );
    
    m_pages_by_name.erase( (std::string)pagename );
    
    for(int i=idestr; i<(int)m_pages.size(); i++)
    {
        m_pages.at(i).page_position = i;    //reset to current index
    }

    //update the map
    m_pages_by_name.clear();
    for(int i=0; i<m_pages.size(); i++)
    {
        m_pages_by_name[ (std::string)m_pages.at(i).label ] = i;
    }

    this->Update();
    this->Refresh();
}

void PagePanel::SetImageList( wxImageList *images )
{
    _image_list = images;
}

void PagePanel::AddPage( wxWindow *input_page, wxString pagename, int icon_id, int page_level, wxString page_group )
{
    InsertPage( m_pages.size(), input_page, pagename, icon_id, page_level, page_group);
}

void PagePanel::InsertPage(int pagepos, wxWindow *input_page, wxString pagename, int icon_id, int page_level, wxString page_group)
{
    Page newpage;
    newpage.icon = _image_list->GetBitmap( icon_id );
    newpage.label = pagename;
    newpage.page = input_page;
    newpage.page_level = page_level;
    newpage.page_position = pagepos;
    newpage.group_owner = page_group;

    if(pagepos < m_pages.size())
    {
        m_pages.insert( m_pages.begin()+pagepos, newpage );
    }
    else
    {
        m_pages.push_back( newpage );
    }

    //update the map
    m_pages_by_name.clear();
    for(int i=0; i<m_pages.size(); i++)
    {
        m_pages_by_name[ (std::string)m_pages.at(i).label ] = i;
    }

    int dwidth,dheight;
    GetPageDrawingSize(dheight, dwidth);

    SetScrollbars(10, 10, dwidth/10, dheight/10);

}



wxWindow* PagePanel::GetPage( wxString pagename )
{
    return m_pages.at( m_pages_by_name[ (std::string)pagename ] ).page;
}

wxWindow* PagePanel::GetPage( int order_index )
{
    return m_pages.at(order_index).page;
}

int PagePanel::GetPagePosition( wxString pagename )
{
    return m_pages.at( m_pages_by_name[ (std::string)pagename ] ).page_position;
}

int PagePanel::GetPageCount()
{
    return (int)m_pages.size();
}

void PagePanel::GetPageDrawingSize(int &height, int &width)
{
    width = this->GetSize().GetWidth()-20;
    height = m_pageheight * m_pages.size() + 10;
}

std::string PagePanel::GetPageGroupOwner( int order_index )
{
    return (std::string)m_pages.at(order_index).group_owner;
}

void PagePanel::MovePage(wxString pagename, int positions_up )
{
    /* Move the page 'pagename' up (or down) the specified number of positions. Up is positive, down is negative. */

    int pos_start = m_pages_by_name[ (std::string)pagename ];
    int pos_final = pos_start + positions_up;
    if(pos_final < 0) pos_final = 0;
    if(pos_final > m_pages.size()-1) pos_final = m_pages.size()-1;

    Page p = m_pages.at( pos_start );   //copy
    m_pages.erase( m_pages.begin() + pos_start );   //erase existing
    m_pages.insert( m_pages.begin() + pos_final, p );   //Insert new guy

    //update the map
    m_pages_by_name.clear();
    for(int i=0; i<m_pages.size(); i++)
    {
        m_pages.at(i).page_position = i;
        m_pages_by_name[ (std::string)m_pages.at(i).label ] = i;
    }

}

std::string PagePanel::GetPageName( int order_index )
{
    return (std::string)m_pages.at(order_index).label;
}

void PagePanel::OnPaint( wxPaintEvent &WXUNUSED(event) )
{
    wxPaintDC dc( this );
    PrepareDC( dc );
    
    //draw the background
    dc.SetPen( wxPen( wxColour(0,0,100,0), 1, wxPENSTYLE_SOLID ) );
    dc.SetBrush( wxBrush( wxColour(255,255,255,1), wxBRUSHSTYLE_SOLID ) );

    int boxwidth = GetSize().GetWidth()-20;

    dc.DrawRectangle(0, 0, boxwidth, m_pages.size()*m_pageheight + 10 );

    int border = 1; //pt

    for(int i=0; i<m_pages.size(); i++)
    {
        //ensure brush and pen settings are correct each time
        dc.SetPen( wxPen(wxColour(0,0,0,0.6), 4, wxPenStyle::wxPENSTYLE_SOLID) );
        dc.SetFont( wxFont(10, wxFontFamily::wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL) );
        dc.SetBrush( wxBrush(wxColour(255,255,255,1.), wxBRUSHSTYLE_SOLID) );

        //load the file
        dc.DrawBitmap( m_pages.at(i).icon, m_leveloffset*m_pages.at(i).page_level + border+1, i*m_pageheight + border+1);

        //Add the text
        dc.DrawText( m_pages.at(i).label, m_pageheight+5 + m_leveloffset*m_pages.at(i).page_level+border+1, i*m_pageheight+6 + border+1);

        //offset shading
        if( m_pages.at(i).page_level > 0) 
        {
            int L = m_pages.at(i).page_level;

            int offbar_width = (int)(0.3*m_leveloffset);
            dc.SetPen( wxPen(wxColour(200, 200, 200, 1), 1, wxPenStyle::wxPENSTYLE_TRANSPARENT) );
            dc.SetBrush( wxBrush(wxColour(200, 200, 200, 1), wxBRUSHSTYLE_SOLID) );
            dc.DrawRectangle( m_leveloffset*L + border - offbar_width, i*m_pageheight + border+1, offbar_width, m_pageheight );

            //nice little bullets over here
            dc.SetPen( wxPen(wxColour(0, 0, 150, 1), 1, wxPENSTYLE_SOLID) );
            dc.SetBrush( wxBrush(wxColour(0, 0, 150, 1), wxBRUSHSTYLE_SOLID) );
            dc.DrawCircle( m_leveloffset*L + border - offbar_width/2, i*m_pageheight + border + 1 + m_pageheight/2, offbar_width/2-1);
        }

        //Active highlight
        if(i == m_active_page)
        { 
            dc.SetPen( wxPen(wxColour(0, 0, 150, 1), 1, wxPenStyle::wxPENSTYLE_SOLID) );
            dc.SetBrush( wxBrush(wxColour(0, 0, 150, 1), wxBRUSHSTYLE_SOLID) );
            dc.DrawRectangle( boxwidth-15, i*m_pageheight+ border+1, 15, m_pageheight );

            dc.SetPen( wxPen(wxColour(0, 0, 222, 1), 1, wxPenStyle::wxPENSTYLE_SOLID) );
            dc.SetBrush( wxBrush(wxColour(0, 0, 0, 1), wxBRUSHSTYLE_TRANSPARENT) );
            dc.DrawRectangle( 0, i*m_pageheight+ border+1, boxwidth, m_pageheight );

        }
    }

}

BEGIN_EVENT_TABLE(PagePanel, wxScrolledWindow)
    EVT_PAINT(PagePanel::OnPaint)
    EVT_LEFT_DOWN(PagePanel::OnClick)
END_EVENT_TABLE()
