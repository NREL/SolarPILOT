
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


/* 
Library for parsing KML files (Google Earth format), based on XML.
code.google.com/p/libkml/
https://developers.google.com/kml/documentation/kmlreference
*/

#include "parse_kml.h"

#include <wx/string.h>
#include <rapidxml.hpp>

#include "gui_util.h"

using namespace rapidxml;

void ParseKML(wxString &file, double &tower_lat, double &tower_lon, std::vector<sp_point> &poly)
{
    /* 
    Parse the KML polygon and create a formatted polygon to be used by the bounds grid.

    The format for the bounds grid is:
    [POLY][P]x1,y1,z1[P]x2,y2,z2...
    
    The "poly" vector is NOT CLEARED by this method. Be sure to clear it before calling, if needed.
    */
    
    //We have to copy the file onto the heap as a char* since the xml parser points to permanent 
    //memory locations. The wxString buffers are temporary.

    char *fstr = new char[file.size()+1];
    strncpy(fstr, (const char*)file.mb_str(), file.size());
    fstr[file.size()] = 0;    //Null terminator

    xml_document<> doc;
    doc.parse<0>(fstr);
    xml_node<> *node = doc.first_node()->first_node("Document")->first_node("Placemark");
    
    double 
        tlatr = tower_lat*D2R,
        tlonr = tower_lon*D2R,
        r_earth = 6371009.;    //Radius of the earth in meters

    while(node != 0)
    {
        wxString coords = (std::string)node->
            first_node("Polygon")->
            first_node("outerBoundaryIs")->
            first_node("LinearRing")->
            first_node("coordinates")->value();
    
        coords.Trim(false);    //Remove leading tabs and newlines
        coords.Trim();    //Remove trailing junk

        //Construct the polygon
        std::vector<std::string> pp = split(coords.ToStdString(), " ");
        
		for(unsigned int i=0; i<pp.size(); i++)
        {
            
            //Convert point values into doubles
            std::vector<std::string> ps = split(pp.at(i),",");
            double
                plat, plon;
            to_double(ps.at(1), &plat);
            to_double(ps.at(0), &plon);
            plat *= D2R;
            plon *= D2R;
            //Convert degrees into meters
            double dist = r_earth*acos( sin(tlatr)*sin(plat) + cos(tlatr)*cos(plat)*cos(tlonr - plon) );
            double dy = (plat < tlatr ? -1. : 1.)*r_earth*acos( sin(tlatr)*sin(plat) + cos(tlatr)*cos(plat) );
            double dx = (plon < tlonr ? -1. : 1.)*sqrt( pow(dist, 2) - pow(dy, 2) );

            poly.push_back(sp_point(dx, dy, 0.));
        }
        
        //Update to the new sibling node
        node = node->next_sibling();
    }

    delete [] fstr;


}

