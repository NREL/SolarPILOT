
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


#include "kml_select_dialog.h"
#include <wx/wx.h>
#include <wx/filedlg.h>
#include <wx/dialog.h>
#include "gui_util.h"
#include "IOUtil.h"
#include <rapidxml.hpp>

using namespace std;
using namespace rapidxml;


kml_select_dialog::kml_select_dialog()
{};
    
kml_select_dialog::kml_select_dialog(
    wxWindow *parent, wxWindowID winid, wxString label, wxPoint pos, wxSize size, long style)
{
    Create(parent, winid, label, pos, size, style);

    wxStaticText *msg = new wxStaticText(this, wxID_ANY, wxT("Specify the tower location:"));

    wxButton *fromfile = new wxButton(this, wxID_ANY, "From File...");
    fromfile->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( kml_select_dialog::OnTowerFile ), NULL, this);

    wxFlexGridSizer *gs = new wxFlexGridSizer(5, 5, 10);

    wxStaticText 
        *latlab = new wxStaticText(this, wxID_ANY, wxT("Latitude")),
        *lonlab = new wxStaticText(this, wxID_ANY, wxT("Longitude")),
        *deglab = new wxStaticText(this, wxID_ANY, wxT("Deg")),
        *minlab = new wxStaticText(this, wxID_ANY, wxT("Min")),
        *seclab = new wxStaticText(this, wxID_ANY, wxT("Sec"));

    for(int i=0; i<3; i++)
    {
        latctrls.push_back( new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60,21)) );
    }
    for(int i=0; i<3; i++)
    {
        lonctrls.push_back( new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(60,21)) );
    }
    wxArrayStr 
        EW_choices, 
        NS_choices;
    EW_choices.push_back("W");
    EW_choices.push_back("E");
    NS_choices.push_back("N");
    NS_choices.push_back("S");

    EW_combo = new wxComboBox(this, wxID_ANY, EW_choices[0], wxDefaultPosition, wxDefaultSize, EW_choices, wxCB_DROPDOWN|wxCB_READONLY);
    NS_combo = new wxComboBox(this, wxID_ANY, NS_choices[0], wxDefaultPosition, wxDefaultSize, NS_choices, wxCB_DROPDOWN|wxCB_READONLY);

    gs->AddSpacer(0);
    gs->Add(deglab);
    gs->Add(minlab);
    gs->Add(seclab);
    gs->AddSpacer(0);
    gs->Add(latlab, 0, wxALIGN_CENTER_VERTICAL, 0);
    for(int i=0; i<3; i++)
    {
        gs->Add(latctrls.at(i), 0, wxALIGN_CENTER_VERTICAL, 0);
    }
    gs->Add(NS_combo, 0, wxALIGN_CENTER_VERTICAL, 0);
    gs->Add(lonlab, 0, wxALIGN_CENTER_VERTICAL, 0);
    for(int i=0; i<3; i++)
    {
        gs->Add(lonctrls.at(i), 0, wxALIGN_CENTER_VERTICAL, 0);
    }
    gs->Add(EW_combo, 0, wxALIGN_CENTER_VERTICAL, 0);

    wxStaticText *inex_lab = new wxStaticText(this, wxID_ANY, wxT("Select region type"));
    wxArrayStr inex_choices;
    inex_choices.push_back(wxT("Inclusion"));
    inex_choices.push_back(wxT("Exclusion"));
    inex_combo = new wxComboBox(this, wxID_ANY, inex_choices[0], wxDefaultPosition, wxDefaultSize, inex_choices, wxCB_DROPDOWN|wxCB_READONLY);

    wxStaticText *append_lab = new wxStaticText(this, wxID_ANY, wxT("Existing land boundary data"));
    wxArrayStr append_choices;
    append_choices.push_back(wxT("Append to"));
    append_choices.push_back(wxT("Replace"));
    append_combo = new wxComboBox(this, wxID_ANY, append_choices[0], wxDefaultPosition, wxDefaultSize, append_choices, wxCB_DROPDOWN|wxCB_READONLY);

    wxFlexGridSizer *gscb = new wxFlexGridSizer(2, 5, 5);
    gscb->Add(inex_combo, 0, wxALIGN_CENTER_VERTICAL, 0);
    gscb->Add(inex_lab, 0, wxALIGN_CENTER_VERTICAL, 0);
    gscb->Add(append_combo, 0, wxALIGN_CENTER_VERTICAL, 0);
    gscb->Add(append_lab, 0, wxALIGN_CENTER_VERTICAL, 0);

    wxButton
        *apply = new wxButton(this, wxID_ANY, wxT("Ok")),
        *cancel = new wxButton(this, wxID_ANY, wxT("Cancel"));

    wxBoxSizer *main_sizer = new wxBoxSizer(wxVERTICAL);
    wxBoxSizer *but_sizer = new wxBoxSizer(wxHORIZONTAL);
    but_sizer->Add(apply, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    but_sizer->Add(cancel, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5);

    wxBoxSizer *top_sizer = new wxBoxSizer(wxHORIZONTAL);
    top_sizer->Add(msg, 0, wxTOP|wxLEFT, 15);
    top_sizer->AddStretchSpacer();
    top_sizer->Add(fromfile, 0, wxTOP|wxLEFT, 15);
    top_sizer->AddSpacer(5);

    main_sizer->Add(top_sizer, 0, wxEXPAND, 0);
    main_sizer->Add(gs, 0, wxALL, 10);
    main_sizer->Add(gscb, 0, wxALL, 10);
    main_sizer->Add(but_sizer, 0, wxALIGN_CENTER_HORIZONTAL|wxBOTTOM, 15);

    apply->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( kml_select_dialog::OnApply ), NULL, this);
    cancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( kml_select_dialog::OnCancel ), NULL, this);
    this->Connect( wxEVT_KEY_UP, wxKeyEventHandler( kml_select_dialog::OnText ), NULL, this);

    this->SetSizerAndFit(main_sizer);
    this->CenterOnParent();

};

void kml_select_dialog::getLocation(double &lat, double &lon)
{
    //Get the values from the textctrls
    double lats[3], longs[3];
    for(int i=0; i<3; i++)
    {
        to_double(latctrls.at(i)->GetValue().ToStdString(), &lats[i]);
        to_double(lonctrls.at(i)->GetValue().ToStdString(), &longs[i]);
    }

    //Convert to decimal form - degrees
    double
        latsign = NS_combo->GetValue() == "N" ? 1. : -1.,
        lonsign = EW_combo->GetValue() == "E" ? 1. : -1.;
    lon = lonsign*(fabs(longs[0]) + fabs(longs[1])/60. + fabs(longs[2])/3600.);
    lat = latsign*(fabs(lats[0]) + fabs(lats[1])/60. + fabs(lats[2])/3600.);

}


void kml_select_dialog::setLocation(double &lat, double &lon)
{
    double
        latabs = abs(lat),
        latsign = lat/latabs,
        latdeg = floor(latabs),
        latmin = floor( (latabs-latdeg)*60. ),
        latsec = (latabs - latdeg - latmin/60.)*3600.,

        lonabs = abs(lon),
        lonsign = lon/lonabs,
        londeg = floor(lonabs),
        lonmin = floor( (lonabs-londeg)*60. ),
        lonsec = (lonabs - londeg - lonmin/60.)*3600.;

    latctrls.at(0)->SetValue( my_to_string((int)(latdeg*latsign)) );
    latctrls.at(1)->SetValue( my_to_string((int)latmin) ); 
    latctrls.at(2)->SetValue( to_string(latsec, "%.4f") );

    lonctrls.at(0)->SetValue( my_to_string((int)(londeg*lonsign)) );
    lonctrls.at(1)->SetValue( my_to_string((int)lonmin) ); 
    lonctrls.at(2)->SetValue( to_string(lonsec, "%.4f") );

    EW_combo->SetValue( lonsign < 0. ? "W" : "E" );
    NS_combo->SetValue( latsign < 0. ? "S" : "N" );
}

bool kml_select_dialog::isAppend()
{
    return append_combo->GetValue() == wxT("Append to"); 
}

bool kml_select_dialog::isInclusion()
{
    return inex_combo->GetValue() == wxT("Inclusion"); 
}

void kml_select_dialog::OnApply(wxCommandEvent &WXUNUSED(event))
{
    //Convert all to double and set values
    this->EndModal(wxID_OK);
}

void kml_select_dialog::OnCancel(wxCommandEvent &WXUNUSED(event))
{
    this->EndModal(wxID_CANCEL);
}

void kml_select_dialog::OnText( wxKeyEvent &event)
{
    /* Check for escape char or return char, otherwise continue */
        
    //get the key pressed
    int key = event.GetKeyCode();
    if( key == WXK_ESCAPE )
    {
        this->EndModal(0);
    }
    if( key == WXK_RETURN )
    {
        this->EndModal(1);
    }

    event.Skip();
};

void kml_select_dialog::OnTowerFile( wxCommandEvent &WXUNUSED(event) )
{
    //select the tower position from a kml file
    //prompt for a file path
    wxFileDialog filedlg(this, wxT("Select tower location file..."), wxEmptyString, wxEmptyString, wxT("KML file (.kml)|*.kml") );
    filedlg.CentreOnParent();

    //Show and process
    if(filedlg.ShowModal() == wxID_OK)
    {
        wxString info = filedlg.GetPath().c_str();
        //Try opening the file
        if(ioutil::file_exists( info.c_str() ) )
        {
            string 
                files,
                fname = (string)info,
                eol;    
            ioutil::read_file(fname, files, eol);
            wxString file(files);

            //Parse the KML file and find the tower coordinates
            char *fstr = new char[file.size()+1];
            strncpy(fstr, (const char*)file.mb_str(), file.size());
            fstr[file.size()] = 0;    //Null terminator

            xml_document<> doc;
            doc.parse<0>(fstr);
            xml_node<> *node = doc.first_node()->first_node("Document")->first_node("Placemark");


            wxString coords = (string)node->
                first_node("Point")->
                first_node("coordinates")->value();

            coords.Trim(false);    //Remove leading tabs and newlines
            coords.Trim();    //Remove trailing junk
            //Construct the point
            vector<string> pp = split(coords.ToStdString(), ",");
            
            double tlat, tlon;
            to_double(pp.at(1), &tlat);
            to_double(pp.at(0), &tlon);
            this->setLocation(tlat, tlon);
            
            delete [] fstr;
        }
    }
}
