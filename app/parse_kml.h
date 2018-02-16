#ifndef _KML_PARSE_ 
#define _KML_PARSE_ 1

/* 
Library for parsing KML files (Google Earth format), based on XML.
code.google.com/p/libkml/
https://developers.google.com/kml/documentation/kmlreference
*/

#include <wx/string.h>
#include <rapidxml.hpp>
#include "gui_util.h"

using namespace rapidxml;

static void ParseKML(wxString &file, double &tower_lat, double &tower_lon, std::vector<sp_point> &poly)
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


};





#endif