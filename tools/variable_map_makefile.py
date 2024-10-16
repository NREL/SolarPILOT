from math import pi
import datetime
import pylightxl as xl

td = datetime.date.today()

sam_license = \
"""
/*
BSD 3-Clause License

Copyright (c) Alliance for Sustainable Energy, LLC. See also https://github.com/NREL/ssc/blob/develop/LICENSE
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
"""

stamp = "//File automatically generated by variable_map_makefile.py {}/{}/{}\n\n{}\n".format(td.year, td.month, td.day, sam_license)

with open("variable_map.xlsx", 'rb') as f:
    db = xl.readxl(f)
    flines = [row for row in db.ws('Sheet1').rows]
    flines.pop(0)   # Remove header

# Copy the contents of the variables to a text file to support diff between versions
with open('variable_definitions.txt', 'w') as fout:
    fout.write( '\n'.join(['\t'.join([str(v) for v in line]) for line in flines]) )

tmap = {"MATRIX_T":"matrix_t<double>",
        "BOOL":"bool",
        "STRING":"std::string",
        "INT":"int",
        "DOUBLE":"double",
        "WEATHERDATA":"WeatherData",
        "VEC_DOUBLE":"std::vector< double >",
        "VEC_INTEGER":"std::vector< int >",
        "DVEC_POINT":"std::vector<std::vector<sp_point> >",
        "VOIDPTR":"void*"
        }

dmap = {}

#Group lines by domain
for line in flines:
    try:
        dmap[line[0]].append( line )
    except:
        dmap[line[0]] = [line]   
        
#header file preamble
#defpath = "../../ssc_dev/solarpilot/"
# defpath = "../../ssc-solarpilot/solarpilot/"
defpath = "../../ssc/solarpilot/"
deffileh = defpath + "definitions.h"
deffilec = defpath + "definitions.cpp"

fdh = open(deffileh, 'w')
fdh.write(stamp)

fdh.write( \
"""
#ifndef _VARDEFS_
#define _VARDEFS_ 1

#include "mod_base.h"
#include "Toolbox.h"

//Enumeration of data columns in the variable map file


//Sandbox mode
#define _SANDBOX 0
//Include Coretrace (relevant to fieldcore only! Disabling this option will cause SolarPILOT compilation to fail.).
#ifdef SP_STANDALONE
	#define SP_USE_SOLTRACE
	//Compile without threading functionality? Comment out to remove.
	#define SP_USE_THREADS
	//create local make-dir functions
	#ifdef _WIN32 
	    #define SP_USE_MKDIR
	#endif
#endif

#ifndef PI
    #define PI %.20f
    #define R2D %.20f
    #define D2R %.20f
#endif

struct _vartable_base
{
public:
	unordered_map<std::string, spbase*> _local_varptrs;
	virtual void addptrs(unordered_map<std::string, spbase*>&) = 0;
};

"""%(pi, 180./pi, pi/180.)
)

domains = list(dmap.keys())
domains.sort()

for domain in domains:
    fdh.write("\n\n")
    
    fdh.write("struct var_"+domain + " : public _vartable_base\n{\n")
    
    #sort each domain by inout type then variable name
    dmap[domain].sort(key = lambda x: x[1])    
    dmap[domain].sort(key = lambda x: x[2])
    
    for var in dmap[domain]:
        tp = "spvar" if var[2] == "IN" else "spout"
        fdh.write( "\t%s< "%tp + tmap[var[3]] + " > " + var[1] + "; \t\t//[" + var[5] + "] " + var[11] + "\n" )
        if var[7] == "combo":
            copts = var[8].split(";")
            enstr = "struct "+var[1].upper()+"{ enum EN{%s}; };\n"
            entries = []
            for copt in copts:
                if copt == "":
                    continue
                ename, ev = copt.split("=")
                escrub = ''.join(e for e in ename if (e.isalnum() or e==' '))
                ename = escrub.replace(" ","_").upper()
                if ename[0].isdigit():
                    ename = "N" + ename
                entries.append(ename + "=" + ev)
            fdh.write("\t" + enstr%(",".join(entries)))
                
    fdh.write(\
    """
    void addptrs(unordered_map<std::string, spbase*> &pmap);\n"""
    )
    
    fdh.write("};\n") 
    
    
fdh.write(\
"""

struct var_map
{
    var_ambient amb;
    var_financial fin;
    var_fluxsim flux;
    var_land land;
    var_optimize opt;
    var_parametric par;
    var_solarfield sf;
    
    std::vector< var_heliostat > hels;
    std::vector< var_receiver > recs;
   
    var_map();
    var_map( var_map &vc );   
    void copy( var_map &vc );
    void reset();

    unordered_map<std::string, spbase*> _varptrs;
    void add_receiver(int id);
    void add_heliostat(int id);
    void drop_receiver(int id);
    void drop_heliostat(int id);
};
""")

fdh.write("\n#endif\n")
fdh.close()

#---------------------------------------------------------------------
#  Write cpp file
#---------------------------------------------------------------------
imap = {\
    "ambient":"amb",
    "financial":"fin",
    "fluxsim":"flux",
    "heliostat":"hels.back()",
    "land":"land",
    "optimize":"opt",
    "parametric":"par",
    "receiver":"recs.back()",
    "solarfield":"sf"
}

fdc = open(deffilec,'w')

fdc.write(stamp)

fdc.write(\
"""
#include "definitions.h"
#include "mod_base.h"
#include "string_util.h"

var_map::var_map()
{
    reset();
}

var_map::var_map( var_map &vc )
{
    reset();
    copy( vc );
}

void var_map::copy( var_map &vc )
{
    //to copy, we need to make sure the template structures for heliostats and receivers are
    //parallel between both varmaps first. Then copy by iterating over the list of strings in
    //the varmap and set the corresponding object values in each map.

    //receiver templates
    for( size_t i=0; i<recs.size(); i++ )
        drop_receiver((int)i);
    for( size_t i=0; i<vc.recs.size(); i++ )
        add_receiver( vc.recs.at(i).id.val );

    //heliostat templates
    for( size_t i=0; i<hels.size(); i++ )
        drop_heliostat((int)i);
    for( size_t i=0; i<vc.hels.size(); i++ )
        add_heliostat( vc.hels.at(i).id.val );

    //now add by string->obj
    for( unordered_map< std::string, spbase* >::iterator var=_varptrs.begin(); var!=_varptrs.end(); var++ )
        var->second->set_from_string( vc._varptrs.at( var->first )->as_string().c_str() );
    
    return;
}

void var_map::reset()
{
    //first clear the receiver and heliostat objects
    hels.clear();
    recs.clear();
    //clear out pointer map
    _varptrs.clear();
    
    //reserve size for the templates. otherwise, the pointer addresses get confused. This is the max limit of templates.
    hels.reserve(100);
    recs.reserve(100);
    
"""
)

sc = "\""

dmain = [s for s in domains]
dmain.remove("heliostat")
dmain.remove("receiver")

"""
std::string Address, 
SP_DATTYPE Dtype, 
std::string Value, 
std::string Units, 
bool Is_param, 
std::string Ctrl, 
std::string Special,
bool UI_disable,
std::string Label,
std::string Description
"""

"""
0	Domain
1	String name
2	INOUT
3	Type
4	Value
5	Units
6	Parameterizable?
7	Control
8	Special
9	UI Disable
10	Label
11	Description
"""

def wrtvar(domain, var, sep):
    if var[2] == "IN":
        var_value = ""
        if var[3]=='INT':
            var_value = sc+str(int(var[4]))+sc
        elif var[3]=='DOUBLE':
            var_value = sc+str(float(var[4]))+sc
        else:
            var_value = sc+str(var[4])+sc

        fdc.write("\t%-33s.set(%-35s, %-25s, %20s, %12s, %8s, %10s, %5s, %8s, %40s, %s);\n"%\
            tuple([
                    imap[domain]+"."+var[1],  #instance name. variable name
                    #-------------------
                    sc+var[0]+"."+sep+"."+var[1]+sc,  #address
                    "SP_DATTYPE::SP_"+var[3],  #data type
                    var_value, #value
                    sc+var[5]+sc, #units
#                        sc+var[6]+sc, #range
                    'true' if bool(var[6]) else 'false',  #parameterizable
                    sc+var[7]+sc, #control
                    sc+var[8]+sc, #special args
                    "true" if var[9].lower() == 'x' else "false",  #UI disable
                    sc+var[10]+sc,  #label
                    sc+var[11]+sc  #description
                    ])        
                )
    else:
        fdc.write("\t%-33s.setup(%-35s, %-25s, %13s %20s, %8s, %10s, %5s, %8s, %40s, %s);\n"%\
            tuple([
                    imap[domain]+"."+var[1],  #instance name. variable name
                    #-------------------
                    sc+var[0]+"."+sep+"."+var[1]+sc,  #address
                    "SP_DATTYPE::SP_"+var[3],  #data type
                    " ",
                    sc+var[5]+sc, #units
#                        sc+var[6]+sc, #range
                    'true' if bool(var[6]) else 'false',  #parameterizable
                    sc+var[7]+sc, #control
                    sc+var[8]+sc, #special args
                    "true" if var[9].lower() == 'x' else "false",  #UI disable
                    sc+var[10]+sc,  #label
                    sc+var[11]+sc  #description
                    ])        
                )




for domain in dmain:
    
    for var in dmap[domain]:
        wrtvar(domain, var, "0")
    fdc.write("\n")

for domain in dmain:
    fdc.write("\t"+imap[domain]+".addptrs( _varptrs );\n")
    
fdc.write(\
    """
    add_heliostat(0);
    add_receiver(0);
    \n""")

fdc.write("}\n\n")


for domain in ['heliostat','receiver']:
    fdc.write("void var_map::add_{}(int id)\n{}\n\tstd::string sid=my_to_string(id);\n".format(domain,"{") )
    fdc.write("\t{}.push_back(var_{}());\n\n".format('hels' if domain == 'heliostat' else 'recs', domain))
    for var in dmap[domain]:
        wrtvar(domain, var, sc+"+sid+"+sc)
    fdc.write("\n\t"+imap[domain]+".id.val = id;")
    fdc.write("\n\t"+imap[domain]+".addptrs( _varptrs );\n")
    fdc.write("}\n\n")



for domain in domains:
    
    fdc.write("void var_{}::addptrs(unordered_map<std::string, spbase*> &pmap)\n{}\n".format(domain,"{"))
    if domain in ['heliostat','receiver']:
        fdc.write("\tstd::string sid = my_to_string(id.val);\n")
        sep = ".\"+sid+\"."
    else:
        sep = ".0."
        
    for var in dmap[domain]:
#        if var[2] == "IN":
        fdc.write("\t_local_varptrs[\"{}{}{}\"] = &{};\n".format(var[0], sep, var[1], var[1] ))
    
    fdc.write(\
"""
	for (unordered_map<std::string, spbase*>::iterator it = _local_varptrs.begin(); it != _local_varptrs.end(); it++)
		pmap[it->first] = it->second;
}\n\n
""")

fdc.write(\
"""
void var_map::drop_receiver(int id)
{
    //find the receiver in the list
    int id_ind=-1;
    for( int i=0; i<(int)recs.size(); i++)
    {
        if( recs.at(i).id.val == id )
        {
            id_ind = i;
            break;
        }
    }
    if( id_ind < 0 )
        throw spexception("Attempting to remove a receiver ID that doesn't exist!");

    //remove var ptrs
    std::string pat = "receiver.";

    unordered_map<std::string, spbase*>::iterator it = _varptrs.begin();
    while( it != _varptrs.end() )
    {
        if( it->first.find( pat ) != std::string::npos )
        {
            it = _varptrs.erase( it );
        }
        else
        {
            it++;
        }
    }
    
    //remove the variable structure
    recs.erase( recs.begin() + id_ind );

    //add back all receiver pointers
    for(int i=0; i<(int)recs.size(); i++)
    {
        recs.at(i).addptrs(_varptrs);
    }    
}

void var_map::drop_heliostat(int id)
{
    //find the heliostat in the list
    int id_ind=-1;
    for( int i=0; i<(int)hels.size(); i++)
    {
        if( hels.at(i).id.val == id )
        {
            id_ind = i;
            break;
        }
    }
    if( id_ind < 0 )
        throw spexception("Attempting to remove a heliostat ID that doesn't exist!");

    //remove var ptrs
    std::string pat = "heliostat.";

    unordered_map<std::string, spbase*>::iterator it = _varptrs.begin();
    while( it != _varptrs.end() )
    {
        if( it->first.find( pat ) != std::string::npos )
        {
            it = _varptrs.erase( it );
        }
        else
        {
            it++;
        }
    }
    
    //remove the variable structure
    hels.erase( hels.begin() + id_ind );

    //add back all heliostat pointers
    for(int i=0; i<(int)hels.size(); i++)
    {
        hels.at(i).addptrs(_varptrs);
    }
}
"""
)

fdc.close()