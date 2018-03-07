
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


#include "par_edit_ctrls.h"
#include <wx/imaglist.h>

BEGIN_EVENT_TABLE(AFNumeric,wxTextCtrl)
    EVT_TEXT_ENTER(-1, AFNumeric::OnTextEnter)
    EVT_KILL_FOCUS(AFNumeric::OnLoseFocus)
    EVT_SET_FOCUS(AFNumeric::OnSetFocus)
END_EVENT_TABLE()

AFNumeric::AFNumeric( wxWindow *parent, int id, double val, bool intonly,
                     const wxPoint &pos, const wxSize &size)
    : wxTextCtrl(parent, id, wxString::Format("%f", val), pos, size, 
            wxTE_PROCESS_ENTER|wxTE_RIGHT)
{
    mIVal = 0;
    mDVal = 0.0;
    bIntegersOnly = intonly;
    bUseStdlibFormat = false;

    SetupValidator();

    if (bIntegersOnly)
    {
        mFormat = "%d";
        SetInt( (int) val );
    }
    else
    {
        mFormat = "%lg";
        SetDouble(val);
    }
}

void AFNumeric::OnTextEnter(wxCommandEvent &evt)
{
    if (mFocusVal != GetValue())
    {
        mFocusVal = GetValue();
        Translate();
        this->SetSelection(0,this->GetValue().Len());
        evt.Skip();
    }
}

void AFNumeric::OnSetFocus(wxFocusEvent &evt)
{
    mFocusVal = GetValue();
    this->SetSelection(0,mFocusVal.Len());
    evt.Skip();
}

void AFNumeric::OnLoseFocus(wxFocusEvent &evt)
{
    if (mFocusVal != GetValue())
    {
        wxCommandEvent enterpress(wxEVT_COMMAND_TEXT_ENTER, this->GetId() );
        enterpress.SetEventObject(this);
        enterpress.SetString(GetValue());
        GetEventHandler()->ProcessEvent(enterpress);
    }
    evt.Skip();
}

void AFNumeric::SetupValidator()
{
    wxArrayStr excludes;

    excludes.Add(",");
    if (bIntegersOnly)
    {
        excludes.Add("+");
        excludes.Add("e");
        excludes.Add("E");
        excludes.Add(".");
    }

    wxTextValidator val(wxFILTER_NUMERIC|wxFILTER_EXCLUDE_CHAR_LIST);
    val.SetExcludes( excludes );

    this->SetValidator( val );
}

static bool is_valid_char(bool intonly, char c, bool additional, char c1)
{
    if (intonly)
        return isdigit(c) || c == '-' || c=='+' || (additional && c == c1);
    else
        return isdigit(c) || c == '-' || c=='+' || c == '.' || c == 'e' || c == 'E' || (additional && c == c1);
}

void AFNumeric::Translate()
{
    wxString buf;
    wxString strval = GetValue();
    int len = strval.Len();
    int i;

    // find start of number (all numbers start like integers or a dot)
    i=0;
    while(i<len && !is_valid_char(true, strval[i], true, '.'))
        i++;

    // get all valid number characters
    while(i<len && is_valid_char( bIntegersOnly, strval[i], true, ',' ))
    {
        if ( strval[i]!=',' ) buf += strval[i];
        i++;
    }

    if (bIntegersOnly)
        SetInt( atoi( buf.c_str() ) );
    else
        SetDouble( atof( buf.c_str() ) );
}

void AFNumeric::DoFormat()
{
    if (bIntegersOnly)
        SetValue( wxString::Format(mFormat.c_str(), mIVal) );
    else
        SetValue( wxString::Format(mFormat.c_str(), mDVal) );
}

void AFNumeric::UseStdlibFormat(bool b)
{
    bUseStdlibFormat = b;
}

void AFNumeric::SetInt(int val)
{
    mIVal = val;
    DoFormat();
}

void AFNumeric::SetDouble(double val)
{
    mDVal = val;
    DoFormat();
}

int AFNumeric::GetInt()
{
    return mIVal;
}

double AFNumeric::GetDouble()
{
    return mDVal;
}

void AFNumeric::SetIntegersOnly(bool b)
{
    bIntegersOnly = b;
    SetupValidator();
}

bool AFNumeric::IntegersOnly()
{
    return bIntegersOnly;
}

void AFNumeric::SetFormat(const wxString &fmt)
{
    mFormat = fmt;
}

wxString AFNumeric::GetFormat()
{
    return mFormat;
}


BEGIN_EVENT_TABLE(AFLabel,wxWindow)
    EVT_PAINT( AFLabel::OnPaint )
    EVT_SIZE( AFLabel::OnResize )
END_EVENT_TABLE()

AFLabel::AFLabel( wxWindow *parent, int id, const wxString &caption, const wxPoint &pos, const wxSize &size)
    : wxWindow(parent, id, pos, size )
{
    mColour = *wxBLACK;
    mCaption = caption;
    bTop = false;
    bRight = false;
    mRelSize = 0;
    mBold = false;
    bWordWrap = false;
}

wxSize AFLabel::DoGetBestSize()
{
    wxClientDC dc(this);
    dc.SetFont( GetFont() );
    return dc.GetTextExtent(mCaption);
}

void AFLabel::AlignTop(bool b)
{
    bTop = b;
}

void AFLabel::AlignRight(bool b)
{
    bRight = b;
}

void AFLabel::SetCaption(const wxString &txt)
{
    mCaption = txt;
    Refresh();
}

wxString AFLabel::GetCaption()
{
    return mCaption;
}

void AFLabel::SetColour(const wxColour &c)
{
    mColour = c;
}

wxColour AFLabel::GetColour()
{
    return mColour;
}

void AFLabel::SetBold(bool b)
{
    mBold = b;
}

bool AFLabel::IsBold()
{
    return mBold;
}

void AFLabel::SetWordWrap(bool b)
{
    bWordWrap = b;
}

bool AFLabel::IsWordWrapped()
{
    return bWordWrap;
}

void AFLabel::SetRelativeSize(int sz)
{
    mRelSize = sz;
}

int AFLabel::GetRelativeSize()
{
    return mRelSize;
}

void AFLabel::OnPaint(wxPaintEvent &WXUNUSED(event))
{
    wxPaintDC pdc(this);
    PrepareDC(pdc);
    pdc.SetDeviceClippingRegion( this->GetUpdateRegion() );
    
    pdc.SetTextForeground(mColour);

    wxFont f = this->GetFont();

    if (mBold)
        f.SetWeight( wxFONTWEIGHT_BOLD );
    else if (!mBold && f.GetWeight() != wxFONTWEIGHT_NORMAL )
        f.SetWeight( wxFONTWEIGHT_NORMAL );

    if (mRelSize != 0)
        f.SetPointSize( f.GetPointSize() + mRelSize );

    pdc.SetFont( f );

    int w,h;
    GetClientSize(&w,&h);
    wxRect geom(0, 0, w, h);

    wxCoord width, height;
    pdc.GetTextExtent(mCaption, &width, &height);
    wxRect tbounds(geom.x+2, geom.y+geom.height/2-height/2-1,
                    width, height+3);

    if (bTop)
        tbounds.y = geom.y + 1;

    if (bRight)
        tbounds.x = geom.x + geom.width - width - 2;    


    if (bWordWrap && !bRight && bTop)
        Painter::WordWrap(pdc, mCaption, w-4, true, 2, 1);
    else
        pdc.DrawText(mCaption, tbounds.x, tbounds.y);

    pdc.DestroyClippingRegion();
}

void AFLabel::OnResize(wxSizeEvent &WXUNUSED(event))
{
    Refresh();
    Update();
}


//******* AFTreeView control *************

BEGIN_EVENT_TABLE(AFTreeView, wxTreeCtrl)
    EVT_LEFT_DOWN(AFTreeView::OnLClick)
END_EVENT_TABLE()

AFTreeView::AFTreeView( wxWindow *parent, int id, wxString imagedir, const wxPoint &pos, const wxSize &size)
    : wxTreeCtrl(parent, id, pos, size, wxTR_HAS_BUTTONS|wxTR_NO_LINES|wxTR_SINGLE)
{
    bCheckMode = true;
    wxImageList *images = new wxImageList( 16, 16 );
    images->Add(wxBitmap(imagedir+"checkbox_false_16.png", wxBITMAP_TYPE_PNG));
    images->Add(wxBitmap(imagedir+"checkbox_true_16.png", wxBITMAP_TYPE_PNG));
    AssignImageList( images );

}

void AFTreeView::Check(const wxTreeItemId &item, bool b)
{
    SetItemImage(item, b ? ICON_CHECK_TRUE : ICON_CHECK_FALSE);
}

bool AFTreeView::IsChecked(const wxTreeItemId &item)
{
    return (GetItemImage(item)==ICON_CHECK_TRUE);
}

void AFTreeView::EnableCheckMode(bool b)
{
    bCheckMode = b;
}

bool AFTreeView::IsCheckMode()
{
    return bCheckMode;
}

void AFTreeView::OnLClick(wxMouseEvent &evt)
{
    int flags=0;
    wxTreeItemId item = HitTest(evt.GetPosition(), flags);
    if (!item.IsOk()||!bCheckMode)
    {
        evt.Skip();
        return;
    }

    int state = GetItemImage(item);
    if (state == ICON_CHECK_TRUE || state == ICON_CHECK_FALSE)
    {
        SetItemImage(item, 1-state);
        wxTreeEvent tree_evt( ::wxEVT_COMMAND_TREE_ITEM_ACTIVATED , this, item );
        tree_evt.SetPoint( evt.GetPosition() );
        tree_evt.SetLabel( GetItemText(item) );
        ProcessEvent( tree_evt );
    }
    else
        evt.Skip();
}

//---------------------------------

enum{ IDSLB_LIST = 1598, IDSLB_FILTER };

BEGIN_EVENT_TABLE( AFSearchListBox, wxPanel )
    EVT_LISTBOX( IDSLB_LIST, AFSearchListBox::OnSelect )
    EVT_TEXT( IDSLB_FILTER, AFSearchListBox::OnFilter )
END_EVENT_TABLE()

AFSearchListBox::AFSearchListBox(wxWindow *parent, int id, const wxPoint &pos, const wxSize &size )
    : wxPanel( parent, id, pos, size, wxCLIP_CHILDREN )
{
    m_label = new wxStaticText( this, -1, " Filter:  ");

    m_notifyLabel = new wxStaticText( this, -1, wxEmptyString );
    m_notifyLabel->SetForegroundColour( *wxRED );

    m_txtFilter = new wxTextCtrl( this, IDSLB_FILTER );
    wxSize szbest = m_txtFilter->GetBestSize();
    szbest.SetWidth( 2*szbest.GetWidth() );
    m_txtFilter->SetInitialSize( szbest );

    m_list = new wxListBox( this, IDSLB_LIST, wxDefaultPosition, wxDefaultSize,
            0, 0,
            wxLB_SINGLE|wxLB_HSCROLL|wxLB_ALWAYS_SB );
    
    wxBoxSizer *szh = new wxBoxSizer(wxHORIZONTAL);
    szh->Add( m_label, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5  );
    szh->Add( m_txtFilter, 0, wxALL|wxEXPAND, 2 );
    szh->Add( m_notifyLabel, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );

    wxBoxSizer *szmain = new wxBoxSizer(wxVERTICAL);
    szmain->Add( szh, 0, wxALL|wxEXPAND, 2 );
    szmain->Add( m_list, 1, wxALL|wxEXPAND, 2);
    SetSizer(szmain);
}

void AFSearchListBox::SetPromptText( const wxString &s )
{
    m_label->SetLabel( s );
    Layout();
}

void AFSearchListBox::Append( const wxString &s )
{
    m_items.push_back( item(s,true) );
    
    UpdateView();
}

void AFSearchListBox::Append( const wxArrayStr &s )
{
    for (size_t i=0;i<s.Count();i++)
        m_items.push_back( item(s[i],true) );

    UpdateView();
}

int AFSearchListBox::GetSelection()
{
    wxString sel = m_list->GetStringSelection();
    if (!sel.IsEmpty())
    {
        for (int i=0;i<(int)m_items.size();i++)
            if (m_items.at(i).str == sel)
                return i;
    }
    return -1;
}

int AFSearchListBox::Count()
{
    return (int)m_items.size();
}

wxString AFSearchListBox::GetItem( int i )
{
    if (i >= 0 && i < (int)m_items.size())
        return m_items.at(i).str;
    else
        return wxEmptyString;
}

void AFSearchListBox::Clear()
{
    m_list->Clear();
    m_items.clear();
}

wxString AFSearchListBox::GetStringSelection()
{
    return m_list->GetStringSelection();
}

void AFSearchListBox::SetSelection( int i )
{
    if (i >= 0 && i < (int)m_items.size())
    {
        if ( !m_items.at(i).shown )
        {
            m_items.at(i).shown = true;
            UpdateView();
        }
    
        m_list->SetStringSelection( m_items.at(i).str );
    }
}

void AFSearchListBox::SetStringSelection(const wxString &s)
{
    for (int i=0;i<(int)m_items.size();i++)
    {
        if (m_items.at(i).str == s)
        {
            SetSelection(i);
            return;
        }
    }
}

void AFSearchListBox::OnFilter( wxCommandEvent & )
{
    wxString filter = m_txtFilter->GetValue().Lower();
    wxString sel = m_list->GetStringSelection();
    
    m_notifyLabel->SetLabel( wxEmptyString );

    if (filter.IsEmpty())
    {
        for (int i=0;i<(int)m_items.size();i++)
            m_items.at(i).shown = true;
    }
    else
    {
        int count = 0;
        for (int i=0;i<(int)m_items.size();i++)
        {
            if (filter.Len() <= 2 && m_items.at(i).str.Left( filter.Len() ).Lower() == filter)
            {
                m_items.at(i).shown = true;
                count++;
            }
            else if (m_items.at(i).str.Lower().Find( filter ) >= 0)
            {
                m_items.at(i).shown = true;
                count++;
            }
            else if (m_items.at(i).str.Lower().Find( filter ) == 0)
            {
                m_items.at(i).shown = true;
                count++;
            }
            else if (m_items.at(i).str == sel)
                m_items.at(i).shown = true;
            else
                m_items.at(i).shown = false;
        }

        if (count == 0)
            m_notifyLabel->SetLabel( "No matches. (selected item shown)" );
    }

    UpdateView();
    m_list->SetStringSelection( sel );
}

void AFSearchListBox::OnSelect( wxCommandEvent &WXUNUSED(e) )
{
    wxCommandEvent edup( wxEVT_COMMAND_LISTBOX_SELECTED, this->GetId() );
    edup.SetEventObject( this );
    edup.SetString( m_list->GetStringSelection() );
    edup.SetInt( GetSelection() );

    ProcessEvent( edup );
}

void AFSearchListBox::UpdateView()
{
    m_list->Freeze();
    m_list->Clear();
    for (int i=0;i<(int)m_items.size();i++)
        if (m_items.at(i).shown)
            m_list->Append( m_items.at(i).str );

    m_list->Thaw();
}

/* ******** AFTextCtrl ************** */


BEGIN_EVENT_TABLE(AFTextCtrl,wxTextCtrl)
    EVT_KILL_FOCUS(AFTextCtrl::OnLoseFocus)
    EVT_SET_FOCUS(AFTextCtrl::OnSetFocus)
END_EVENT_TABLE()

AFTextCtrl::AFTextCtrl( wxWindow *parent, int id, const wxPoint &pos, const wxSize &size, long style)
    : wxTextCtrl(parent, id, "", pos, size, wxTE_PROCESS_ENTER|style)
{
    /* nothing to do */
    bSendOnFocus = true;
}

void AFTextCtrl::OnLoseFocus(wxFocusEvent &evt)
{
    if (bSendOnFocus && m_origVal != GetValue())
    {
        wxCommandEvent enterpress(wxEVT_COMMAND_TEXT_ENTER, this->GetId() );
        enterpress.SetEventObject(this);
        enterpress.SetString(GetValue());
        GetEventHandler()->ProcessEvent(enterpress);
    }
    evt.Skip();
}

void AFTextCtrl::OnSetFocus(wxFocusEvent &evt)
{
    m_origVal = GetValue();
    evt.Skip();
}

//**************** painter ***************************
wxFont Painter::GetNormalFont(bool b, int sz)
{
    wxFont f = *wxNORMAL_FONT;
    if (b)
        f.SetWeight( wxFONTWEIGHT_BOLD );
    if (sz >= 6)
        f.SetPointSize( sz );
    return f;
}

wxFont Painter::GetArialFont(bool b, int sz)
{
    return wxFont(sz, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, 
        b?wxFONTWEIGHT_BOLD:wxFONTWEIGHT_NORMAL, false, "arial" ); 
}

int Painter::WordWrap(wxDC& dc, const wxString &str, int width, bool draw, int x, int y, wxArrayStr *lines)
{
    int line = 0;
    int line_height = dc.GetCharHeight();
    wxString remaining = str;

    while ( !remaining.IsEmpty() )
    {
        wxString line_text = remaining;
        wxCoord line_width;
        dc.GetTextExtent(line_text, &line_width, NULL);
        while(line_width > 5 && line_width >= width-3 && line_text.Len() > 0)
        {
            int pos = line_text.Find(' ', true);
            if (pos < 0)
                line_text.Truncate( line_text.Len()-1 );
            else
                line_text.Truncate(pos);

            dc.GetTextExtent(line_text, &line_width, NULL);
        }

        if (line_text.IsEmpty() || line_width < 5)
            break;

        if (lines) lines->Add( line_text );
        
        if (draw)
            dc.DrawText(line_text, x, y+line*line_height);

        line++;

        remaining = remaining.Mid(line_text.Len());
        remaining.Trim(false).Trim();
    }

    return line*line_height;
}

void Painter::DrawRaisedPanel(wxDC &dc, int x, int y, int width, int height)
{    
    dc.DrawRectangle(x, y, width, height);
    
    wxPen savedPen = dc.GetPen();
    dc.SetPen(*wxWHITE_PEN);

	dc.DrawLine(x, y+1, x+width-1, y+1);
	dc.DrawLine(x, y+1, x, y+height-1);
	dc.DrawLine(x+1, y+1, x+width-2, y+1);

	dc.SetPen(*wxLIGHT_GREY_PEN);
	dc.DrawLine(x+1, y+height-2, x+width-2, y+height-2);
	dc.DrawLine(x+width-2, y+2, x+width-2, y+height-2);

	dc.SetPen(*wxBLACK_PEN);
	dc.DrawLine(x, y+height-1, x+width-1, y+height-1);
	dc.DrawLine(x+width-1, y, x+width-1, y+height);
    
    dc.SetPen(savedPen);
}

void Painter::DrawSunkenPanel(wxDC &dc, int x, int y, int width, int height)
{
    dc.DrawRectangle(x, y, width, height);

    wxPen savedPen = dc.GetPen();
    wxBrush savedBrush = dc.GetBrush();
    
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.SetPen(*wxBLACK_PEN);
    dc.DrawRectangle(x, y, width-1, height-1);
    
    dc.SetPen(*wxGREY_PEN);
    dc.DrawRectangle(x+1, y+1, width-2, height-2);

    dc.SetBrush(savedBrush);
    dc.SetPen(savedPen);
}

void Painter::DrawEngravedPanel(wxDC &dc, int x, int y, int width, int height, bool fill)
{
    wxBrush savedBrush = dc.GetBrush();
    wxPen savedPen = dc.GetPen();
    
    if (fill)
    {
        dc.DrawRectangle(x, y, width, height);
    }

    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.SetPen(*wxGREY_PEN);
    dc.DrawRectangle(x, y, width-2, height-2);
    
    dc.SetPen(*wxWHITE_PEN);
    dc.DrawRectangle(x+1, y+1, width-2, height-2);
    dc.SetBrush(savedBrush);
    dc.SetPen(savedPen);
}

void Painter::DrawScrollBar(wxDC &dc, bool vertical, int x, int y, int width, int height)
{
    wxPen savedPen = dc.GetPen();
    wxBrush savedBrush = dc.GetBrush();
    
    dc.SetPen(*wxLIGHT_GREY_PEN);
    dc.SetBrush(*wxLIGHT_GREY_BRUSH);
    dc.DrawRectangle(x, y, width, height);    

    dc.SetBrush(savedBrush);
    dc.SetPen(savedPen);
    if (vertical)
    {
        Painter::DrawArrowButton(dc, UP, x, y, width, width);
        if (height > 2.5*width)
            Painter::DrawRaisedPanel(dc, x, y+width+1, width, 0.3*height);

        Painter::DrawArrowButton(dc, DOWN, x, y+height-width, width, width);
    }
    else
    {
        Painter::DrawArrowButton(dc, LEFT, x, y, height, height);
        if (width > 2.5*height)
            Painter::DrawRaisedPanel(dc, x+height+1, y, 0.3*width, height);

        Painter::DrawArrowButton(dc, RIGHT, x+width-height, y, height, height);
    }
    dc.SetBrush(savedBrush);
    dc.SetPen(savedPen);
}

void Painter::DrawArrowButton(wxDC &dc, ArrowType type, int x, int y, int width, int height)
{
    int asize = width < height ? width/2 : height/2;
    
    wxBrush savedBrush = dc.GetBrush();
    wxPen savedPen = dc.GetPen();
    Painter::DrawRaisedPanel(dc, x, y, width, height);
    dc.SetBrush(*wxBLACK_BRUSH);
    dc.SetPen(*wxBLACK_PEN);
    
    switch(type)
    {
    case UP:
    case DOWN:
        Painter::DrawArrow(dc, type, x+(width-asize)/2, y+(height-asize)/2, asize, asize);
        break;
    default:
        Painter::DrawArrow(dc, type, x+(width-asize)/2, y+(height-asize)/2, asize, asize);
    }
    
    dc.SetBrush(savedBrush);
    dc.SetPen(savedPen);
}

void Painter::DrawArrow(wxDC &dc, ArrowType type, int x, int y, int width, int height)
{
    wxPoint pts[3];
    switch(type)
    {
    case RIGHT:
      pts[0] = wxPoint(x,y);
      pts[1] = wxPoint(x, y+height);
      pts[2] = wxPoint(x+width, y+height/2);
      break;
   case LEFT:
      pts[0] = wxPoint(x+width,y);
      pts[1] = wxPoint(x+width, y+height);
      pts[2] = wxPoint(x, y+height/2);
      break;
   case UP:
      pts[0] = wxPoint(x,y+height);
      pts[1] = wxPoint(x+width, y+height);
      pts[2] = wxPoint(x+width/2, y);
      break;
   case DOWN:
      pts[0] = wxPoint(x,y);
      pts[1] = wxPoint(x+width, y);
      pts[2] = wxPoint(x+width/2, y+height);
      break;
   default:
       return;
      }

    dc.DrawPolygon(3, pts);
}
