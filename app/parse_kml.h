#ifndef _KML_PARSE_ 
#define _KML_PARSE_ 1

/* 
Library for parsing KML files (Google Earth format), based on XML.
code.google.com/p/libkml/
https://developers.google.com/kml/documentation/kmlreference
*/

#include "gui_util.h"

extern void ParseKML(wxString &file, double &tower_lat, double &tower_lon, std::vector<sp_point> &poly);


#endif