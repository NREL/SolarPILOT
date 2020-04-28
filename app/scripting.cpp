
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


#include <lk/env.h>
#include <fstream>
#include <algorithm> 
#include "rapidxml.hpp"

#include "AutoPilot_API.h"
#include "scripting.h"
#include "GUI_main.h"
#include "IOUtil.h"
#include "param_dialog.h"

using namespace std;

static void EditorOutput(const char *msg)
{
    SPFrame::Instance().ScriptMessageOutput(msg);
}

static bool LKInfoCallback(simulation_info *siminfo, void *data)
{
    SolarPILOTScriptWindow *frame = static_cast<SolarPILOTScriptWindow*>( data );
    if(frame != NULL && frame->IsReportingEnabled()) 
    {
        int current = siminfo->getCurrentSimulation();
        int total = siminfo->getTotalSimulationCount();
        std::string notices = *siminfo->getSimulationNotices();

        if( current > 0 ||! notices.empty() )
        {
            std::string msg = "";
            if( current > 0 )
            {
                msg.append( my_to_string(current+1) + "/" + my_to_string(total) + ": " );
                if( !notices.empty() )
                    msg.append(": ");
                else
                    msg.append("\n");
            }
            if( ! notices.empty() )
                msg.append(notices + "\n");

            frame->AddOutput(msg);
            wxYieldIfNeeded();
        }
    }
    return true;
};


static void _reset_sf( lk::invoke_t &cxt )
{
    LK_DOC("reset_geometry", "Reset the system geometry to the SolarPILOT default values, clearing any changes or variable settings.", "(void):null");

    SPFrame::Instance().GetSolarFieldObject()->getVarMap()->reset();
}

static void _add_receiver( lk::invoke_t &cxt )
{
    LK_DOC("add_receiver", "Add a new receiver, returning the unique index", "(string:name[, boolean:make selection]):integer");   
    
    SPFrame &F = SPFrame::Instance();
    var_map *V = F.GetSolarFieldObject()->getVarMap();


    string tname = cxt.arg(0).as_string();
    // check to make sure this isn't a duplicate. Each item needs a unique name
    bool dupe = false;
    for(unsigned int i=0; i<V->recs.size(); i++)
    { 
        if(tname == V->recs.at(i).rec_name.val) 
            dupe = true; 
    }
    if(dupe)
    {
        EditorOutput("Please enter a unique name for this geometry.");
        return;
    }

    //Add a receiver
    int ind = V->recs.size();

    V->add_receiver(ind);
    V->recs[ind].rec_name.val = tname;
                
    //Re-create the solar field object
    F.GetSolarFieldObject()->Create(*V);

    F.UpdateReceiverUITemplates();
            
    //update the input display
    F.UpdateCalculatedGUIValues();

    cxt.result().assign( (double)SPFrame::Instance().GetVariablesObject()->recs.back().id.val );

    return;
}

static void _drop_receiver( lk::invoke_t &cxt )
{
    LK_DOC("drop_receiver", "Drop a receiver from the current solar field", "(integer:index)");

    SPFrame &F = SPFrame::Instance();
    SolarField *SF = F.GetSolarFieldObject();
    var_map *V = SF->getVarMap();

    std::string tname = lower_case( cxt.arg(0).as_string().ToStdString() );

    for(size_t i=0; i<V->hels.size(); i++)
    {
        if( tname == lower_case( V->recs.at(i).rec_name.val ) )
        {
            //delete the item
            V->drop_receiver( V->recs.at(i).id.val );
            SF->Create(*V);
            cxt.result().assign( 1. );
            return;
        }
    }

    cxt.result().assign( 0. );

    return;
}

static void _add_heliostat_template( lk::invoke_t &cxt )
{
    LK_DOC("add_heliostat_template", "Add a new heliostat template that can be used in the layout.", "(string:template name):integer");

    //Add a heliostat
    SPFrame &F = SPFrame::Instance();
    var_map *V = F.GetSolarFieldObject()->getVarMap();

    string tname = cxt.arg(0).as_string();
    bool dupe = false;
    for(unsigned int i=0; i<V->hels.size(); i++)
    { 
        if(tname == V->hels.at(i).helio_name.val) 
            dupe = true; 
    }
    if(dupe)
    {
        EditorOutput("Please enter a unique name for this heliostat template.");
        return;
    }

    int ind = V->hels.size();
    V->add_heliostat(ind);
    V->hels.back().helio_name.val = tname;
    //Re-create the solar field object
    F.GetSolarFieldObject()->Create(*V);

    cxt.result().assign( (double)SPFrame::Instance().GetVariablesObject()->hels.back().id.val );

}

static void _drop_heliostat_template( lk::invoke_t &cxt )
{
    LK_DOC("drop_heliostat_template", "Delete (drop) the specified heliostat template from the current setup. Returns true if successful.", "(string:template name):bool");

    SPFrame &F = SPFrame::Instance();
    SolarField *SF = F.GetSolarFieldObject();
    var_map *V = SF->getVarMap();

    std::string tname = lower_case( cxt.arg(0).as_string().ToStdString() );

    for(size_t i=0; i<V->hels.size(); i++)
    {
        if( tname == lower_case( V->hels.at(i).helio_name.val ) )
        {
            //delete the item
            V->drop_heliostat( V->hels.at(i).id.val );
            SF->Create(*V);
            cxt.result().assign( 1. );
            return;
        }
    }

    cxt.result().assign( 0. );

}

//static void _add_heliostat( lk::invoke_t &cxt )
//{
//    LK_DOC("add_heliostat", "Add a new heliostat, returning the unique index", "(string:name[, boolean:make selection]):integer");   
//
//};
//
//
//static void _drop_heliostat( lk::invoke_t &cxt )
//{
//    LK_DOC("drop_heliostat", "Drop a heliostat from the current layout", "(integer:index)");
//
//    return;
//}


static void _sp_var( lk::invoke_t &cxt )
{
    LK_DOC2( "var", "Sets or gets a variable value in the SolarPILOT data set.", 
        "Set a variable value.", "(string:name, variant:value):none", 
        "Get a variable value", "(string:name):variant" );
    
    var_map *vmap = SPFrame::Instance().GetSolarFieldObject()->getVarMap();
    
    wxString name = cxt.arg(0).as_string();
    if (cxt.arg_count() == 1)  //Get a variable
    {
        if( vmap->_varptrs.find( name.ToStdString() ) == vmap->_varptrs.end() ) 
        {
            cxt.result().assign(0.);
            return;
        }
        else
        {

            spbase *var = vmap->_varptrs[name.ToStdString()];
            std::string varstr = var->as_string();
            
            switch (var->dattype)
            {
            case SP_INT:
            {
                if (var->is_output)
                {
                    spout<int>* v = static_cast<spout<int>*>(var);
                    cxt.result().assign((double)v->Val());
                }
                else
                {
                    spvar<int> *v = static_cast< spvar<int>* >( var );
                    cxt.result().assign( (double)v->val );
                }
                return;
            }
            case SP_DOUBLE:
            {
                if (var->is_output)
                {
                    spout<double>* v = static_cast<spout<double>*>(var);
                    cxt.result().assign(v->Val());
                }
                else
                {
                    spvar<double> *v = static_cast< spvar<double>* >( var );
                    cxt.result().assign( v->val );
                }
                return;
            }
            case SP_STRING:
                cxt.result().assign( var->as_string() );
                return;
            case SP_BOOL:
            {
                if (var->is_output)
                {
                    spout<bool>* v = static_cast<spout<bool>*>(var);
                    cxt.result().assign(v->Val() ? 1. : 0.);
                }
                else
                {
                    spvar<bool> *v = static_cast< spvar<bool>* >( var );
                    cxt.result().assign( v->val ? 1. : 0. );
                }
                return;
            }
            case SP_MATRIX_T:
            {
                cxt.result().empty_vector();

                matrix_t<double> vec;
                spbase::_setv(varstr, vec);

                cxt.result().vec()->reserve( vec.nrows() );
                for(int i=0; i<vec.nrows(); i++)
                {
                    cxt.result().vec()->push_back( lk::vardata_t() );
                    cxt.result().vec()->at(i).empty_vector();

                    for( int j=0; j<vec.ncols(); j++)
                    {
                        cxt.result().vec()->at(i).vec_append( vec.at(i,j) );
                    }
                }

                return;
            }
            case SP_DVEC_POINT:
            {
                std::vector< std::vector< sp_point > > Vp;
                spbase::_setv( varstr, Vp );

                cxt.result().empty_vector();
                cxt.result().vec()->reserve( Vp.size() );
                int ni = Vp.size();

                for(int i=0; i<ni; i++)
                {
                    for(int j=0; j<Vp.at(i).size(); i++)
                    {
                        cxt.result().vec()->push_back( lk::vardata_t() );
                        cxt.result().vec()->at(i*ni + j).vec_append( (double)j );
                        cxt.result().vec()->at(i*ni + j).vec_append(Vp.at(i).at(j).x);
                        cxt.result().vec()->at(i*ni + j).vec_append(Vp.at(i).at(j).y);
                        cxt.result().vec()->at(i*ni + j).vec_append(Vp.at(i).at(j).z);
                    }
                }

                return;
            }
            case SP_VEC_DOUBLE:
            case SP_VEC_INTEGER:
            {
                std::vector<double> Vd;
                spbase::_setv( varstr, Vd );
                cxt.result().empty_vector();
                cxt.result().vec()->reserve( Vd.size() );

                for(int i=0; i<Vd.size(); i++)
                    cxt.result().vec_append( Vd.at(i) );

                return;
            }
            case SP_WEATHERDATA:
            case SP_VOIDPTR:
            default:
                break;
            }

        }
     
    }
    else if (cxt.arg_count() == 2)      //set variable
    {
    
        std::string arg = cxt.arg(1).as_string();
        std::string sname = (std::string)name;
            
        //make sure the specified variable exists
        if( vmap->_varptrs.find( sname ) == vmap->_varptrs.end() )
        {
            EditorOutput( ("Invalid variable name: " + sname).c_str() );
            cxt.result().assign(0.);
            return;
        }
        else
        {
            //if it's a combo, make sure the specified combo choice exists
            if( vmap->_varptrs.at( sname )->ctype == "combo" )
            {
                std::vector< std::string > cbchoices = vmap->_varptrs.at( sname )->combo_get_choices();
                if( std::find( cbchoices.begin(), cbchoices.end(), arg ) != cbchoices.end() )
                {
                    //valid variable and selection
                    vmap->_varptrs.at(sname)->set_from_string( arg.c_str() );
                    cxt.result().assign( arg );
                }
                else
                {
                    EditorOutput( ("Invalid variable choice for \"" + sname + "\": \"" + arg + "\" is not a valid option.").c_str() );
                    cxt.result().assign(0.);
                    return;
                }
            }
            else
            {
                //no problems, just set the variable
                vmap->_varptrs.at(sname)->set_from_string( arg.c_str() );
                cxt.result().assign( arg );
            }
        }

    }
}

static void _update_geometry(lk::invoke_t &cxt)
{
    LK_DOC("update_geometry",
        "Refresh the solar field, receiver, or ambient condition settings based on the current parameter settings.",
        "(void):boolean");

    SPFrame &F = SPFrame::Instance();
    SolarField *SF = F.GetSolarFieldObject();
    if (SF->getHeliostats()->size() == 0)
    {
        //no layout exists, so we should be calling the 'run_layout' method instead
        EditorOutput("No layout exists, so the 'update_geometry' function cannot be executed. Please first create or import a layout using 'run_layout'.");
        cxt.result().assign(0.);
        return;
    }

    var_map *V = SF->getVarMap();

    wxString weatherfile = V->amb.weather_file.val;
    F.UpdateClimateFile(weatherfile, *V, true);

    //Update the design method box.. this actually updates both the map values and GUI. Probably should fix this sometime..
    F.UpdateDesignSelect(V->sf.des_sim_detail.mapval(), *V);

    //Set up the solar field
    SF->Clean();
    SF->Create(*V);

    try
    {
        SolarField::PrepareFieldLayout(*SF, 0, true);    
        
        if (SF->ErrCheck())
        {
            EditorOutput("An error occurred when preparing the updated field geometry in the call 'update_geometry'.");
            cxt.result().assign(0.);
            return;
        }

        SF->calcHeliostatArea();
        SF->updateAllCalculatedParameters(*V);

        double azzen[2];
        SF->CalcDesignPtSunPosition(V->sf.sun_loc_des.mapval(), azzen[0], azzen[1]);
        Vect sun = Ambient::calcSunVectorFromAzZen(azzen[0] * D2R, azzen[1] * D2R);

        SF->updateAllTrackVectors(sun);    
        
        if (SF->ErrCheck())
        {
            EditorOutput("An error occurred when preparing the updated field geometry in the call 'update_geometry'.");
            cxt.result().assign(0.);
            return;
        }
    }
    catch (std::exception &e)
    {
        EditorOutput("An error occurred when preparing the updated field geometry in the call 'update_geometry'. Error:\n");
        EditorOutput(e.what());
        cxt.result().assign(0.);
        return;
    }
    catch (...)
    {
        EditorOutput("Unknown error when executing 'update_geometry'.");
        cxt.result().assign(0.);
        return;
    }

    cxt.result().assign(1.);
    return;
}

static void _generate_layout( lk::invoke_t &cxt )
{
    
    LK_DOC2("run_layout", 
        "Create a solar field layout. Options include 'nthreads':integer (default All),'save_detail':boolean (default True), 'show_progress':boolean (default True)", 
        //run layout without specified positions
        "SolarPILOT generates layout positions", "([table:options]):boolean",
        //run layout with specified positions
        "User specifies layout positions in the following format, where first 4 columns are required:\n"
        "0				1			2			3			4			5				6				7			   8		9		10		11		12	     13\n"
        "< template (int) > <enabled> <in layout> <location X> <location Y> <location Z> <x focal length> <y focal length> <cant i> <cant j> <cant k> <aim X> <aim Y> <aim Z>"
        //"<template (int)> <location X> <location Y> <location Z> <x focal length> <y focal length> <cant i> <cant j> <cant k> <aim X> <aim Y> <aim Z>"
        , "(array:positions):boolean"
        );
        

    SPFrame &F = SPFrame::Instance();
    SolarField *SF = F.GetSolarFieldObject();
    var_map *V = SF->getVarMap();

    bool current_script_reporting_status = F.GetScriptWindowPointer()->IsReportingEnabled();

    if( cxt.arg_count() > 0 )
    {
        lk::vardata_t *options=0;


        if( cxt.arg(0).type() == lk::vardata_t::VECTOR )
        {
            //user table
            if( cxt.arg_count() > 1 )
                options = &cxt.arg(1);

            //assign layout positions
            V->sf.layout_data.val.clear();
            size_t npos = cxt.arg(0).vec()->size();
            for(size_t i=0; i<npos; i++)
            {
                std::string helstr = "";
                std::vector< lk::vardata_t > *hp = cxt.arg(0).vec()->at(i).vec();

                for(size_t j=0; j<hp->size(); j++)
                    helstr += hp->at(j).as_string() + (j < hp->size()-1 ? "," : "");
                
                V->sf.layout_data.val += helstr + (i < npos-1 ? ";" : "");
            }
            
            //user specified layout
            V->sf.layout_method.combo_select_by_mapval( var_solarfield::LAYOUT_METHOD::USERDEFINED );
        }
        else
            options = &cxt.arg(0);

        if (options)
        {
            if( options->hash()->find( "nthreads" ) != options->hash()->end() )
                F.SetThreadCount( options->hash()->at( "nthreads" )->as_integer() );
            if (options->hash()->find("show_progress") != options->hash()->end())
                F.GetScriptWindowPointer()->EnableScriptWindowReporting(options->hash()->at("show_progress")->as_boolean());
        }
    }

    wxString v=(wxString)V->amb.weather_file.val;
    F.UpdateClimateFile(v, *V, true); 
    F.UpdateDesignSelect( V->sf.des_sim_detail.mapval(), *V );
    SF->Clean();
    SF->Create(*V);
    bool ok = F.DoManagedLayout(*SF, *V);        //Returns TRUE if successful

    sim_results& results = *F.GetResultsObject();
    //Process the simulation results
    results.clear();
    results.resize(1);
    double azzen[2];
    SF->CalcDesignPtSunPosition(V->sf.sun_loc_des.mapval(), azzen[0], azzen[1]);

    sim_params P;
    P.dni = V->sf.dni_des.val;

    results.at(0).process_analytical_simulation(*SF, P, 0, azzen);

    //Load the results in the grid
    F.UpdateLayoutGrid();


    //Redraw the plots
    F.GetFieldPlotObject()->SetPlotData(*SF, FIELD_PLOT::EFF_TOT);
    //update the selection combo
    F.UpdateFieldPlotSelections();
    //update the receiver flux map selection combo
    F.UpdateFluxPlotSelections();

    F.DoResultsPage();

    F.UpdateCalculatedGUIValues();
    F.SetGeomState(false);
    //F._inputs_modified = true;    //Any time the layout is run, flag for save on exit

    //clear the flux heliostat list control
    /*_flux_lc->ClearAll();
    _flcsort.clear();*/
    //UpdateFluxLC(-1);

    cxt.result().assign( ok );

    //F.UpdateLayoutGrid();
    F.GetFieldPlotObject()->SetPlotData( *SF, FIELD_PLOT::EFF_TOT ); 
    F.GetFieldPlotObject()->Update();

    F.GetScriptWindowPointer()->EnableScriptWindowReporting(current_script_reporting_status);

    return;
}

static void _get_layout_info( lk::invoke_t &cxt )
{
    LK_DOC("get_layout", "Get information regarding the heliostat field layout. Returns matrix with each row corresponding to a heliostat." 
                         "Information includes: [index, position-x, position-y, position-z, template, ranking metric value]", "(void):table");


    SolarField *SF = SPFrame::Instance().GetSolarFieldObject();

    Hvector *hels = SF->getHeliostats();

    lk::vardata_t &r = cxt.result();
    r.empty_vector();
    r.vec()->reserve( hels->size() );

    for(int i=0; i<(int)hels->size(); i++)
    {
        r.vec()->push_back( lk::vardata_t() );
        r.vec()->at(i).empty_vector();

        r.vec()->at(i).vec_append( hels->at(i)->getId() );

        sp_point *loc = hels->at(i)->getLocation();
        r.vec()->at(i).vec_append( loc->x );
        r.vec()->at(i).vec_append( loc->y );
        r.vec()->at(i).vec_append( loc->z );

        r.vec()->at(i).vec_append( *hels->at(i)->getMasterTemplate()->getHeliostatName() );

        r.vec()->at(i).vec_append( hels->at(i)->getRankingMetricValue() );
    }

    return;
}

static void _simulate( lk::invoke_t &cxt )
{
    LK_DOC("run_performance", "Calculate heliostat field performance. Options include 'nthreads':integer (default All), "
                              "'save_detail':boolean (default True), 'update_aimpoints':boolean (default True), 'show_progress':boolean (default True)", 
                              "([table:options]):boolean");


    SPFrame &F = SPFrame::Instance();
    SolarField *SF = F.GetSolarFieldObject();
    var_map *V = SF->getVarMap();

    bool current_script_reporting_status = F.GetScriptWindowPointer()->IsReportingEnabled();

    if( cxt.arg_count() > 0 )
    {
        lk::vardata_t &v = cxt.arg(0);
        if( v.hash()->find( "nthreads" ) != v.hash()->end() )
            F.SetThreadCount( v.hash()->at( "nthreads" )->as_integer() );

        if( v.hash()->find( "update_aimpoints" ) != v.hash()->end() )
            if( !v.hash()->at( "update_aimpoints" )->as_boolean() )
                V->flux.aim_method.combo_select( "Keep existing" );
        if (v.hash()->find("show_progress") != v.hash()->end())
            F.GetScriptWindowPointer()->EnableScriptWindowReporting(v.hash()->at("show_progress")->as_boolean());
    }

    //Which type of simulation is this?
    int simtype = V->flux.flux_model.mapval();    //0=Delsol, 1=Soltrace
    
    //Set up field, update aimpoints, and simulate at the performance sun position

    Hvector *helios = SF->getHeliostats();

    if(! interop::PerformanceSimulationPrep(*SF, *helios, simtype) ) 
    {
        cxt.result().assign( 0. );
        return;
    }
    
    Vect sun = Ambient::calcSunVectorFromAzZen( SF->getVarMap()->flux.flux_solar_az.Val() * D2R, (90. - SF->getVarMap()->flux.flux_solar_el.Val())*D2R );
    
    SF->calcHeliostatShadows(sun);    
    if(SF->ErrCheck())
    {
        cxt.result().assign( 0. );
        return;
    }
    
    F.GetResultsObject()->clear();
    F.GetResultsObject()->resize(1);

    F.StartSimTimer();

    //Which type of simulation?
    bool ok;
    switch(simtype)
    {
    case var_fluxsim::FLUX_MODEL::HERMITE_ANALYTICAL:
        ok = F.HermiteFluxSimulationHandler(*SF, *helios);
        break;
    case var_fluxsim::FLUX_MODEL::SOLTRACE:
        ok = F.SolTraceFluxSimulation(*SF, *V, *helios);
        break;
    default:
        ok = false;
        break;
    }

    F.StopSimTimer();
    SF->getSimInfoObject()->Reset();

    F.GetFluxPlotObject()->SetPlotData(*SF, *helios, 0 );
    F.GetFieldPlotObject()->SetPlotData( *SF, FIELD_PLOT::EFF_TOT ); 
    
    cxt.result().assign( ok ? 1. : 0. );

    F.GetScriptWindowPointer()->EnableScriptWindowReporting(current_script_reporting_status);

    return;
}

static void _summary_results( lk::invoke_t &cxt )
{
    LK_DOC("get_summary_results", "Return an array of tables with summary results from each simulation. "
                                  "The array length is greater than 1 for multiple-receiver simulations.", "(void):array");

    SPFrame &F = SPFrame::Instance();

    grid_emulator table;
    sim_results *results = F.GetResultsObject();

    if( results->size() < 1 )
    {
        EditorOutput("No simulation summary results exist. Please simulate first.");
        return;
    }


    lk::vardata_t &rt = cxt.result();
    rt.empty_vector();
    rt.vec()->resize(results->size());

    for (size_t i = 0; i < results->size(); i++)
    {
        lk::vardata_t &r = rt.vec()->at(i);

        F.CreateResultsTable(results->at(i), table);

        r.empty_hash();

        for (int i = 0; i < table.GetNumberRows(); i++)
            r.hash_item(table.GetRowLabelValue(i), table.GetCellValue(i, 1));

        //add a few more summary results
        bool is_soltrace = r.hash()->find("Shadowing and Cosine efficiency") != r.hash()->end();

        double Qwf;
        double Qin = Qwf = r.hash()->at("Power incident on field")->as_number();

        if (is_soltrace)
        {
            /*
            soltrace
            for this option, the "Shadowing and Cosine efficiency" is already calculated by the
            program. Just make sure the Shading and Cosine efficiencies aren't double counted.
            */
            r.hash_item("Shading efficiency", 100.);
            r.hash_item("Cosine efficiency", 100.);
            r.hash_item("Shading loss", 0.);
            r.hash_item("Cosine loss", 0.);

            double eta_sc = r.hash()->at("Shadowing and Cosine efficiency")->as_number() / 100.;
            Qwf *= eta_sc;
        }
        else
        {
            //hermite
            r.hash_item("Shadowing and Cosine efficiency",
                r.hash()->at("Shading efficiency")->as_number()
                *r.hash()->at("Cosine efficiency")->as_number() / 100.);

            double eta_s = r.hash()->at("Shading efficiency")->as_number() / 100.;
            Qwf *= eta_s;
            r.hash_item("Shading loss", Qin*(1. - eta_s));
            double eta_c = r.hash()->at("Cosine efficiency")->as_number() / 100.;
            r.hash_item("Cosine loss", Qwf*(1 - eta_c));
            Qwf *= eta_c;
        }
        r.hash_item("Shadowing and Cosine loss", Qin - Qwf);

        double eta_r = r.hash()->at("Reflection efficiency")->as_number() / 100.;
        r.hash_item("Reflection loss", Qwf * (1. - eta_r));
        Qwf *= eta_r;
        double eta_b = r.hash()->at("Blocking efficiency")->as_number() / 100.;
        r.hash_item("Blocking loss", Qwf*(1. - eta_b));
        Qwf *= eta_b;
        double eta_i = r.hash()->at("Image intercept efficiency")->as_number() / 100.;
        r.hash_item("Image intercept loss", Qwf*(1. - eta_i));
        Qwf *= eta_i;
        double eta_a = r.hash()->at("Absorption efficiency")->as_number() / 100.;
        r.hash_item("Absorption loss", Qwf*(1. - eta_a));

        r.hash_item("Receiver name", i == 0 ? "All receivers" : results->at(i).receiver_names.front());
    }

    return;
}

static void _detail_results( lk::invoke_t &cxt )
{
    /* 
    
    returns a vector with hash entries for each heliostat

    id(integer), location (array), aimpoint (array), tracking_vector (array), layout_metric (double), power_to_receiver (double), 
        efficiency (double), cosine (double), intercept (double), reflectance (double), attenuation (double),
        blocking (double), shading (double), clouds (double)

    */

    LK_DOC("get_detail_results", 
        "[Only valid for Hermite (analytical) simulation engine.]\n"
		"Return an array with detailed heliostat-by-heliostat results from a simulation. "
        "Each entry in the array is a table with entries as follows:\n"
        "{ id(integer), location (array), aimpoint (array), tracking_vector (array), "
        "layout_metric (double), "
        "power_to_receiver (double), "
        "power_reflected (double), "
        "efficiency (double), "
        "cosine (double), "
        "intercept (double), "
        "reflectance (double), "
        "attenuation (double), "
        "blocking (double), "
        "shading (double), "
        "clouds (double) }", 
        "([array:selected heliostat indices]):array");
    
    SPFrame &F = SPFrame::Instance();
    SolarField *SF = F.GetSolarFieldObject();


    if( SF->getHeliostats()->size() > 0 )
    {
        Hvector helio_select;

        if( cxt.arg_count() > 0 ) //use selected heliostats
        {
            //expecting an array of heliostat id's
            if( cxt.arg(0).type() != lk::vardata_t::VECTOR )
                return;

            lk::vardata_t &v = cxt.arg(0);

            std::vector<int> ids;
            ids.reserve( v.vec()->size() );
            for(size_t i=0; i<v.vec()->size(); i++)
                ids.push_back(v.vec()->at(i).as_integer());

            helio_select.reserve( v.vec()->size() );

            unordered_map<int, Heliostat*> *heliosid = SF->getHeliostatsByID();

            for(size_t i=0; i<ids.size(); i++)
                helio_select.push_back( heliosid->at( ids.at(i) ) );

        }
        else    //use all heliostats
        {
            helio_select.assign( SF->getHeliostats()->begin(), SF->getHeliostats()->end() );
        }

        //loop through selected heliostats, gathering information
        cxt.result().empty_vector();
        cxt.result().vec()->reserve( helio_select.size() );

        for( size_t i=0; i<helio_select.size(); i++)
        {
            cxt.result().vec()->push_back( lk::vardata_t() );
            lk::vardata_t &p = cxt.result().vec()->back();
            Heliostat* H = helio_select.at(i);
            
            p.empty_hash();

            p.hash_item( "id", H->getId() );
            p.hash_item( "location", lk::vardata_t() );
            p.hash()->at("location")->empty_vector();
            p.hash()->at("location")->vec_append(H->getLocation()->x );
            p.hash()->at("location")->vec_append(H->getLocation()->y );
            p.hash()->at("location")->vec_append(H->getLocation()->z );
            p.hash_item( "aimpoint", lk::vardata_t() );
            p.hash()->at("aimpoint")->empty_vector();
            p.hash()->at("aimpoint")->vec_append(H->getAimPoint()->x );
            p.hash()->at("aimpoint")->vec_append(H->getAimPoint()->y );
            p.hash()->at("aimpoint")->vec_append(H->getAimPoint()->z );
            p.hash_item( "tracking_vector", lk::vardata_t() );
            p.hash()->at("tracking_vector")->empty_vector();
            p.hash()->at("tracking_vector")->vec_append(H->getTrackVector()->i );
            p.hash()->at("tracking_vector")->vec_append(H->getTrackVector()->j );
            p.hash()->at("tracking_vector")->vec_append(H->getTrackVector()->k );
            p.hash_item( "layout_metric", H->getRankingMetricValue() );
            p.hash_item( "power_to_receiver", H->getPowerToReceiver()/1000. );  //kW
            p.hash_item( "power_reflected", H->getArea()
                                            *H->getEfficiencyCosine()
                                            *H->getTotalReflectivity()
                                            *H->getEfficiencyBlock()
                                            *H->getEfficiencyShading()
                                            *H->getEfficiencyCloudiness() 
                                            *SF->getVarMap()->flux.flux_dni.val/1000. //kW
                        );
            p.hash_item( "energy", H->getEnergyValue() ); //kWh -- energy delivered over the simulation time period
            p.hash_item( "efficiency_annual", H->getAnnualEfficiency() );
            p.hash_item( "efficiency", H->getEfficiencyTotal() );
            p.hash_item( "cosine", H->getEfficiencyCosine() );
            p.hash_item( "intercept", H->getEfficiencyIntercept() );
            p.hash_item( "reflectance", H->getTotalReflectivity() );
            p.hash_item( "attenuation", H->getEfficiencyAtten() );
            p.hash_item( "blocking", H->getEfficiencyBlock() );
            p.hash_item( "shading", H->getEfficiencyShading() );
            p.hash_item( "clouds", H->getEfficiencyCloudiness() );

        }

    }
    
    return;
}


static void _get_fluxmap( lk::invoke_t &cxt )
{

    LK_DOC("get_fluxmap", "Retrieve the receiver fluxmap, optionally specifying the receiver ID to retrieve.", "([integer:receiver id]):array");

    SPFrame &F = SPFrame::Instance();
    SolarField *SF = F.GetSolarFieldObject();

    Receiver *rec;

    if( cxt.arg_count() == 1 )
    {
        int id = cxt.arg(0).as_integer();

        if( id > SF->getReceivers()->size() -1 )
            return;

        rec = SF->getReceivers()->at(id);
    }
    else
    {
        rec = SF->getReceivers()->front();
    }
    
    FluxGrid *fg = rec->getFluxSurfaces()->front().getFluxMap();

    cxt.result().empty_vector();
    cxt.result().vec()->reserve( fg->size() );

    for(size_t i=0; i<fg->front().size(); i++)
    {
        cxt.result().vec()->push_back( lk::vardata_t() );
        lk::vardata_t &p = cxt.result().vec()->back();

        p.empty_vector();
        
        for(size_t j=0; j<fg->size(); j++)
        {
            p.vec_append( fg->at(j).at(i).flux );
        }
    }

}

static void _optimize( lk::invoke_t &cxt )
{
    LK_DOC("run_optimization","Execute an optimization run, returning the optimized result and iteration information. "
        "Variables to be optimized are passed in a vector, with each row containing a table specifying "
        "{variable, step, upbound, lowbound, inital}. The table must include the variable key, others are optional. "
        "The return table includes the following: 'result':table of variable names and associated optimized values, "
        "'objective':number, 'flux':number, 'iterations':array of evaluation point, objective, flux. "
        "Optional arguments include maxiterations/tolerance/defaultstep/powerpenalty/nthreads.",
        "(vector:variable tables[, table:options]):table");

    SPFrame &F = SPFrame::Instance();
    SolarField *SF = F.GetSolarFieldObject();
    var_map *V = SF->getVarMap();

    //get the variable table
    if( cxt.arg_count() < 1 || cxt.arg(0).type() != lk::vardata_t::VECTOR )
        return;

    lk::vardata_t &vartab = cxt.arg(0);

    //set up options, if provided
    if( cxt.arg_count() == 2 )
    {
        //maxiterations/tolerance/defaultstep/powerpenalty/nthreads

        lk::varhash_t *opthash = cxt.arg(1).hash();

        if( opthash->find( "nthreads" ) != opthash->end() )
            F.SetThreadCount( opthash->at( "nthreads" )->as_integer() );
        
        if( opthash->find( "maxiterations" ) != opthash->end() )
            V->opt.max_iter.val = opthash->at("maxiterations")->as_integer(); 
        
        if( opthash->find( "tolerance" ) != opthash->end() )
            V->opt.converge_tol.val = opthash->at("tolerance")->as_number(); 
        
        if( opthash->find( "defaultstep" ) != opthash->end() )
            V->opt.max_step.val = opthash->at("defaultstep")->as_number();
        
        if( opthash->find( "powerpenalty" ) != opthash->end() )
            V->opt.power_penalty.val = opthash->at("powerpenalty")->as_number();
    }

    //set up variables
    int nv = vartab.vec()->size();
    vector<double*> optvars(nv);
    vector<double> upper(nv);
    vector<double> lower(nv);
    vector<double> stepsize(nv);
    vector<string> names(nv);

    for(size_t i=0; i<nv; i++ )
    {
        //check that the specified variable names exist
        std::string varname = vartab.vec()->at(i).hash()->at("variable")->as_string();

        if( V->_varptrs.find(varname) == V->_varptrs.end() )
            throw lk::error_t("Specified variable does not exist: " + varname);
        
        //handle the variable
        if( V->_varptrs.at( varname )->dattype != SP_DATTYPE::SP_DOUBLE )
            throw lk::error_t("Optimized variable must be of type 'double'; discrete or boolean variables are not supported. Variable: " + varname);

        spvar<double> *varptr = static_cast<spvar<double>*>( V->_varptrs.at( varname ) );
        optvars.at(i) = &varptr->val;
        vector<string> namedat = split(varname, ".");
        names.at(i) = namedat.back();

        lk::varhash_t *varhash = vartab.vec()->at(i).hash();

        //bounds
        if( varhash->find("lowbound") == varhash->end() )
            lower.at(i) = -HUGE_VAL;
        else
            lower.at(i) = varhash->at("lowbound")->as_number();

        if( varhash->find("upbound") == varhash->end() )
            upper.at(i) = HUGE_VAL;
        else
            upper.at(i) = varhash->at("upbound")->as_number();

        if( varhash->find("initial") != varhash->end() )
            varptr->val = varhash->at("initial")->as_number();

        if( varhash->find("step") == varhash->end() )
            stepsize.at(i) = V->opt.max_step.val * varptr->val;
        else
            stepsize.at(i) = varhash->at("step")->as_number();

    }

    int n_threads = F.GetThreadCount();
    ArrayString *local_wfdat = F.GetLocalWeatherDataObject();
    lk::vardata_t iter_vec;
    std::vector< double > obj_vals;
    std::vector< std::vector<double> > flux_vals;
    std::vector< std::vector< double > > eval_points;

    if(n_threads > 1)
    {
        AutoPilot_MT *SFopt_MT = new AutoPilot_MT();
            
        SFopt_MT->SetSummaryCallback( LKInfoCallback, SF->getSimInfoObject()->getCallbackData());
            
        //set up the weather data for simulation
        vector<string> wdata;
        for(int i=0; i<local_wfdat->size(); i++)
            wdata.push_back( local_wfdat->at(i) );
        SFopt_MT->GenerateDesignPointSimulations( *V, wdata );
            
        //Do the expert setup
        SFopt_MT->Setup(*V, true);
            
        //run the optimization
        SFopt_MT->Optimize(optvars, upper, lower, stepsize, &names);

        //get resulting info
        SFopt_MT->GetOptimizationObject()->getOptimizationSimulationHistory( eval_points, obj_vals, flux_vals );

        try
        {
            delete SFopt_MT;
        }
        catch(...)
        {}
    }
    else
    {

        AutoPilot_S *SFopt_S = new AutoPilot_S();
        SFopt_S->SetSummaryCallback( LKInfoCallback, SF->getSimInfoObject()->getCallbackData());

        //set up the weather data for simulation
        vector<string> wdata;
        for(int i=0; i<local_wfdat->size(); i++)
            wdata.push_back( local_wfdat->at(i) );
        SFopt_S->GenerateDesignPointSimulations( *V, wdata );
            
        //Do the expert setup
        SFopt_S->Setup(*V, true);
            
        //run the optimization
        SFopt_S->Optimize(optvars, upper, lower, stepsize, &names);

        //get resulting info
        SFopt_S->GetOptimizationObject()->getOptimizationSimulationHistory( eval_points, obj_vals, flux_vals );


        try
        {
            delete SFopt_S;
        }
        catch(...)
        {}
            }
        

    //set up return structure
    //result/objective/flux/iterations
    cxt.result().empty_hash();

    lk::vardata_t res_hash;
    res_hash.empty_hash();

    for(size_t i=0; i<optvars.size(); i++)
    {
        std::string varname = vartab.vec()->at(i).hash()->at("variable")->as_string();
        spvar<double> *varptr = static_cast<spvar<double>*>( V->_varptrs.at( varname ) );
        res_hash.hash_item( varname, varptr->val );
    }

    cxt.result().hash_item("result", res_hash);

    iter_vec.empty_vector();

    for(size_t i=0; i<flux_vals.size(); i++)
    {
        iter_vec.vec()->push_back( lk::vardata_t() );
        iter_vec.vec()->at(i).empty_vector();
        for(size_t j=0; j<eval_points.front().size(); j++)
        {
            iter_vec.vec()->at(i).vec_append( eval_points.at(i).at(j) );
        }
        iter_vec.vec()->at(i).vec_append( obj_vals.at(i) );
        for(size_t j=0; j<flux_vals.front().size(); j++)
            iter_vec.vec()->at(i).vec_append( flux_vals.at(i).at(j) );
    }

    lk::vardata_t fluxresult;
    fluxresult.empty_vector();
    for (size_t j = 0; j < flux_vals.back().size(); j++)
        fluxresult.vec_append(flux_vals.back().at(j));

    cxt.result().hash_item( "objective", obj_vals.back() );
    cxt.result().hash_item( "flux", fluxresult);
    cxt.result().hash_item( "iterations", iter_vec );

}

static void _clear_land( lk::invoke_t &cxt )
{
    LK_DOC("clear_land", "Reset the land boundary polygons, clearing any data. Optionally specify 'type' as 'inclusion' or 'exclusion'.", "([string:type]):void");

    bool clear_inclusions = true;
    bool clear_exclusions = true;

    if( cxt.arg_count() == 1 )
    {
        std::string arg = cxt.arg(0).as_string();
        arg = lower_case(arg);

        if( arg.find( "inclusion" ) != std::string::npos )
            clear_exclusions = false;
        else if( arg.find( "exclusion" ) != std::string::npos )
            clear_inclusions = false;
    }

    var_map *V = SPFrame::Instance().GetSolarFieldObject()->getVarMap();

    if( clear_inclusions )
        V->land.inclusions.val.clear();
    if( clear_exclusions )
        V->land.exclusions.val.clear();

    if( clear_inclusions && clear_exclusions )
        V->land.is_bounds_array.val = false;

}

static void _add_land( lk::invoke_t &cxt )
{
    LK_DOC("add_land_area", 
            "Add land inclusion or a land exclusion region within a specified polygon. "
            "Specify the type as 'inclusion' or 'exclusion', and optionally append (true) or overwrite (false) the existing regions.",
            "(array:polygon, string:type[, boolean:append=true]):boolean");

    var_land &L = SPFrame::Instance().GetSolarFieldObject()->getVarMap()->land;

    bool is_append = true;

    if( cxt.arg_count() == 3 )
        is_append = cxt.arg(2).as_boolean();

    std::string type = lower_case( cxt.arg(1).as_string().ToStdString() );

    if( type.find("incl") != std::string::npos )
        type = "inclusion";
    else if( type.find("excl") != std::string::npos )
        type = "exclusion";
    else
    {
        //invalid argument
        cxt.result().assign(0.);
        return;
    }

    //convert the polygon into the required string format
    std::vector< std::string > pt, poly;
    for(size_t i=0; i<cxt.arg(0).vec()->size(); i++)
    {
        pt.clear();
        for(size_t j=0; j<cxt.arg(0).vec()->at(i).vec()->size(); j++)
            pt.push_back( cxt.arg(0).vec()->at(i).vec()->at(j).as_string().ToStdString() );

        poly.push_back( join( pt, "," ) );
    }

    std::string spoly = "[POLY]" + join( poly, "[P]" );
    
    if( type == "inclusion" )
    {
        if(! is_append ) 
            L.inclusions.val.clear();

        L.inclusions.set_from_string( spoly.c_str() );
    }
    else
    {
        if(! is_append )
            L.exclusions.val.clear();

        L.exclusions.set_from_string( spoly.c_str() );
    }

    L.is_bounds_array.val = true;
    
    cxt.result().assign(1.);

}

static void _heliostats_by_region( lk::invoke_t &cxt )
{
    LK_DOC("get_heliostats", "Returns heliostats that fall within a region. Options are:\n"
                             ">> all (no additional arguments),\n"
                             ">> cylindrical (provide [rmin,rmax,azmin,azmax radians]),\n"
                             ">> cartesian (provide [xmin, xmax, ymin, ymax[, zmin, zmax]]),\n"
                             ">> polygon (provide [[x1,y1],[x2,y2],...]),\n"
                             ">> svg (provide string with 'scale-x scale-y;offset-x offset-y;<svg path 1>;<svg path 2>;...',\n"
                             ">> svgfile (provide string filename, optional table {'offset'=array, 'scale'=array}).\n\n"
                             "Returns an array of included heliostat ID's or locations.", 
                             "(string:system, variant:region info[, string:return info - id/location]):array");

    SPFrame &F = SPFrame::Instance(); 
    SolarField *SF = F.GetSolarFieldObject();

    Hvector *helios = SF->getHeliostats();

    //which coordinate system?
    std::string system = cxt.arg(0).as_string();

    //which return data type?
    bool is_returnloc = false;
    if( cxt.arg_count() == 3 )
        is_returnloc = lower_case( cxt.arg(2).as_string().ToStdString() ) == "location";
    
    cxt.result().empty_vector();
    
    if( helios->size() < 1 )
        return;

    double delta = 0.0001;

    if( lower_case(system) == "all" )
    {
        for(size_t i=0; i<helios->size(); i++)
        {
            if( is_returnloc )
            {
                lk::vardata_t pv;
                pv.empty_vector();
                pv.vec_append( helios->at(i)->getLocation()->x );
                pv.vec_append( helios->at(i)->getLocation()->y );
                pv.vec_append( helios->at(i)->getLocation()->z );

                cxt.result().vec()->push_back( pv );
            }
            else
                cxt.result().vec_append( (double)helios->at(i)->getId() );
        }
    }
    else if( lower_case(system) == "cylindrical" )
    {
        double rmin,rmax,azmin,azmax;
        rmin = cxt.arg(1).vec()->at(0).as_number()-delta;
        rmax = cxt.arg(1).vec()->at(1).as_number()+delta;
        azmin = cxt.arg(1).vec()->at(2).as_number()-delta;
        azmax = cxt.arg(1).vec()->at(3).as_number()+delta;

        for(size_t i=0; i<helios->size(); i++)
        {
            double rpos = helios->at(i)->getRadialPos();
            double apos = helios->at(i)->getAzimuthalPos();

            if( rpos > rmin )
                if( rpos < rmax )
                    if( apos > azmin )
                        if( apos < azmax )
                            if( is_returnloc )
                            {
                                lk::vardata_t pv;
                                pv.empty_vector();
                                pv.vec_append( helios->at(i)->getLocation()->x );
                                pv.vec_append( helios->at(i)->getLocation()->y );
                                pv.vec_append( helios->at(i)->getLocation()->z );

                                cxt.result().vec()->push_back( pv );
                            }
                            else
                                cxt.result().vec_append( (double)helios->at(i)->getId() );
        }

    }
    else if( lower_case(system) == "cartesian" )
    {
        double xmin,xmax,ymin,ymax,zmin,zmax;
        xmin = cxt.arg(1).vec()->at(0).as_number()-delta;
        xmax = cxt.arg(1).vec()->at(1).as_number()+delta;
        ymin = cxt.arg(1).vec()->at(2).as_number()-delta;
        ymax = cxt.arg(1).vec()->at(3).as_number()+delta;
        if( cxt.arg(1).vec()->size() == 6 )
        {
            zmin = cxt.arg(1).vec()->at(4).as_number()-delta;
            zmax = cxt.arg(1).vec()->at(5).as_number()+delta;
        }
        else
        {
            zmin = -9e9;
            zmax = 9e9;
        }


        for(size_t i=0; i<helios->size(); i++)
        {
            sp_point *loc = helios->at(i)->getLocation();

            if(loc->x > xmin)
                if(loc->x < xmax)
                    if(loc->y > ymin)
                        if(loc->y < ymax)
                            if(loc->z > zmin)
                                if(loc->z < zmax)
                                    if( is_returnloc )
                                    {
                                        lk::vardata_t pv;
                                        pv.empty_vector();
                                        pv.vec_append( helios->at(i)->getLocation()->x );
                                        pv.vec_append( helios->at(i)->getLocation()->y );
                                        pv.vec_append( helios->at(i)->getLocation()->z );

                                        cxt.result().vec()->push_back( pv );
                                    }
                                    else
                                        cxt.result().vec_append( (double)helios->at(i)->getId() );
        }
    }
    else if( lower_case(system) == "polygon" )
    {
        //construct a polygon from the listed points
        std::vector< sp_point > polygon;
        for(size_t i=0; i<cxt.arg(1).vec()->size(); i++)
        {
            lk::vardata_t *pt = &cxt.arg(1).vec()->at(i);
            polygon.push_back( sp_point( pt->vec()->at(0).as_number(), pt->vec()->at(1).as_number(), 0. ) );
        }

        for(size_t i=0; i<helios->size(); i++)
        {
            if( Toolbox::pointInPolygon( polygon,  *helios->at(i)->getLocation() ) )
                if( is_returnloc )
                {
                    lk::vardata_t pv;
                    pv.empty_vector();
                    pv.vec_append( helios->at(i)->getLocation()->x );
                    pv.vec_append( helios->at(i)->getLocation()->y );
                    pv.vec_append( helios->at(i)->getLocation()->z );

                    cxt.result().vec()->push_back( pv );
                }
                else
                    cxt.result().vec_append( (double)helios->at(i)->getId() );
        }

    }
    else if( (lower_case(system) == "svg") || (lower_case(system) == "svgfile") )
    {
        /* 
        undocumented feature
        
        Provide string. Coordinates are space separated, points are comma separated, shapes are semicolon separated.
        First entry is x-scale y-scale, Second entry is x-offset y-offset.
        
        */

        std::vector< std::string > entries;
        std::vector< std::string > scale_s;
        std::vector< std::string > offset_s;


        if( lower_case(system) == "svgfile" )
        {

            std::string fname = cxt.arg(1).as_string();

            if( ! ioutil::file_exists(fname.c_str()) )
                throw lk::error_t("Invalid SVG file - not found.");

            if( cxt.arg_count() == 3 )
            {
                //options table provided
                if( cxt.arg(2).hash()->find("scale") != cxt.arg(2).hash()->end() )
                {
                    scale_s.push_back( cxt.arg(2).hash()->at("scale")->vec()->at(0).as_string().ToStdString() );
                    scale_s.push_back( cxt.arg(2).hash()->at("scale")->vec()->at(1).as_string().ToStdString() );
                }
                else
                {
                    scale_s.push_back("1.");
                    scale_s.push_back("1.");
                }
            
                if( cxt.arg(2).hash()->find("offset") != cxt.arg(2).hash()->end() )
                {
                    offset_s.push_back( cxt.arg(2).hash()->at("offset")->vec()->at(0).as_string().ToStdString() );
                    offset_s.push_back( cxt.arg(2).hash()->at("offset")->vec()->at(1).as_string().ToStdString() );
                }
                else
                {
                    offset_s.push_back("0.");
                    offset_s.push_back("0.");
                }
            }
            else
            {
                offset_s.push_back("0.");
                offset_s.push_back("0.");
                scale_s.push_back("1.");
                scale_s.push_back("1.");
            }

            //load the svg file and parse it as an xml document
            using namespace rapidxml;
            //Read in the file to a string
            string file;        //contents of the file
            string eol;
            ioutil::read_file(fname, file, eol);
    
            char *fstr = new char[file.size()+1];
            strncpy(fstr, (const char*)file.c_str(), file.size());
            fstr[file.size()] = 0;    //Null terminator

            xml_document<> doc;
            doc.parse<0>(fstr);
            xml_node<> *top_node = doc.first_node();    //<data>

            xml_node<> *node = top_node->first_node("g");
            xml_node<> *tnode = node->first_node("g");
            if(tnode)
                node = tnode;
            node = node->first_node("path"); 
            
            //assume that this is consistent with SVG files created by inkscape.. I don't know whether other SVG creators have a consistent XML structure. 
            entries.clear();
            while( node )
            {
                entries.push_back( node->first_attribute("d")->value() );
                node = node->next_sibling("path");
            }
        }
        else
        {
            //get the string data and break it up into units
            std::string data = cxt.arg(1).as_string();
            entries = split(data, ";");
            scale_s = split(entries.front(), " ");
            offset_s = split(entries.at(1), " ");

            entries.erase(entries.begin(),entries.begin()+2);
        }
                
        

        //get the scale and offset vectors
        double scale_x, scale_y, offset_x, offset_y;

        to_double( scale_s.at(0), &scale_x );
        to_double( scale_s.at(1), &scale_y );
        to_double( offset_s.at(0), &offset_x );
        to_double( offset_s.at(1), &offset_y );

        //allocate the main polygons structure
        std::vector< std::vector< sp_point > > polygons;

        for(size_t i=0; i<entries.size(); i++)
        {
            polygons.push_back( std::vector< sp_point >() );
            std::vector< sp_point > *P = &polygons.back();

            Toolbox::poly_from_svg( entries.at(i), *P, true);

            for(size_t j=0; j<P->size(); j++)
            {
                P->at(j).x = P->at(j).x * scale_x + offset_x;
                P->at(j).y = P->at(j).y * scale_y + offset_y;
            }
        }

        //check each heliostat to see if it's in any polygon
        for(size_t i=0; i<helios->size(); i++)
        {
            for(size_t j=0; j<polygons.size(); j++)
            {
                std::vector< sp_point > *polygon = &polygons.at(j);
                sp_point *loc = helios->at(i)->getLocation();

                if( Toolbox::pointInPolygon( *polygon,  *loc ) )
                {
                        if( is_returnloc )
                        {
                            lk::vardata_t pv;
                            pv.empty_vector();
                            pv.vec_append( loc->x );
                            pv.vec_append( loc->y );
                            pv.vec_append( loc->z );

                            cxt.result().vec()->push_back( pv );
                        }
                        else
                            cxt.result().vec_append( (double)helios->at(i)->getId() );

                        //if included, don't need to check other polygons
                        break;
                }
            }
        }


    }
    else
    {
        throw lk::error_t("invalid region type specified. Expecting one of [cylindrical, cartesian, polygon]");
    }
}

static void _modify_heliostats( lk::invoke_t &cxt )
{

    LK_DOC("modify_heliostats", "Modify attributes of a subset of heliostats in the current layout. Modifiable attributes include "
                                "location/aimpoint/soiling/reflectivity/enabled, "
                                "and are specified in the input table by variable name and array pairs. \n\n"
                                "The length of each variable array must match the length of the ID array. For example, \n"
                                "modify_heliostats([1,2,3], { 'location'=[[1.,10.],[2.,11.],[3.,12.]] } );", "(array:heliostat IDs, table:variables):none");

    SPFrame &F = SPFrame::Instance(); 
    SolarField *SF = F.GetSolarFieldObject();
    if( SF->getHeliostats()->size() < 1 )
        return;

    //collect all relevant heliostat ID's
    std::vector< int > hids;

    //pull the ID's from the provided array
    std::vector< lk::vardata_t > *vec = cxt.arg(0).vec();
    for(size_t i=0; i<vec->size(); i++)
        hids.push_back( vec->at(i).as_integer() );
    
    //consolidate all heliostat's into a vector by ID
    unordered_map< int, Heliostat* > *hmap = SF->getHeliostatsByID();
    Hvector helios;

    for(size_t i=0; i<hids.size(); i++)
    {
        try
        {
            helios.push_back( hmap->at( hids.at(i) ) );
        }
        catch(...)
        {}
            }
    
    //get the variable table
    lk::varhash_t *vars = cxt.arg(1).hash();

    //these are the supported options
    std::vector< std::string > attrs = {
    "location",
    "aimpoint",
    "soiling",
    "reflectivity",
    "enabled"
    };
    
    //for each provided option
    for( lk::varhash_t::iterator var=vars->begin(); var != vars->end(); var++)
    {
        std::string varname = var->first;
        lk::vardata_t val = *var->second;

        //first make sure this is a valid attribute
        if( std::find( attrs.begin(), attrs.end(), varname) == attrs.end() )
            throw lk::error_t("Invalid attribute specified: " + varname );

        if( varname == "location" )
        {
            //locations need to be modified through the layout shell object
            layout_shell *layout = SF->getLayoutShellObject();
            
            //make sure the heliostat ID's array is the same length as the location array
            std::vector< lk::vardata_t > *locvec = cxt.arg(1).hash()->at( "location" )->vec();
            if( locvec->size() != helios.size() )
                throw lk::error_t( "The number of locations provided does not match the number of heliostat ID's provided." );
            //assign location(s)
            layout->clear();

            for(size_t i=0; i<helios.size(); i++)
            {
                //update the layout object
                layout->push_back( layout_obj() );
                layout_obj& lobj = layout->back();

                lobj.aim = *helios.at(i)->getAimPoint();
                lobj.cant = *helios.at(i)->getCantVector();
                lobj.focal_x = helios.at(i)->getFocalX();
                lobj.focal_y = helios.at(i)->getFocalY();
                lobj.helio_type = helios.at(i)->getMasterTemplate()->getId();
                
                //update location
                lobj.location.x = locvec->at(i).vec()->at(0).as_number();
                lobj.location.y = locvec->at(i).vec()->at(1).as_number();
                lobj.location.z = 0.;

                //update enabled/in layout statuses
                lobj.is_enabled = helios.at(i)->IsEnabled();
                lobj.is_in_layout = helios.at(i)->IsInLayout();

                if( locvec->at(i).vec()->size() > 2 )
                    lobj.location.z = locvec->at(i).vec()->at(2).as_number();

            }
            SF->PrepareFieldLayout(*SF, 0, true);
        }
        else if( varname == "aimpoint" )
        {
            //make sure the heliostat ID's array is the same length as the aimpoint array
            std::vector< lk::vardata_t > *aimvec = cxt.arg(1).hash()->at( "aimpoint" )->vec();
            if( aimvec->size() != helios.size() )
                throw lk::error_t( "The number of aimpoints provided does not match the number of heliostat ID's provided." );
            //assign aimpoint(s)
            for(size_t i=0; i<helios.size(); i++)
            {
                double ii = aimvec->at(i).vec()->at(0).as_number();
                double j = aimvec->at(i).vec()->at(1).as_number();
                double k = aimvec->at(i).vec()->at(2).as_number();

                helios.at(i)->setAimPoint( ii, j, k );
            }

        }
        else if( varname == "soiling" )
        {
            //make sure the heliostat ID's array is the same length as the soiling array
            std::vector< lk::vardata_t > *svec = cxt.arg(1).hash()->at( "soiling" )->vec();
            if( svec->size() != helios.size() )
                throw lk::error_t( "The number of soiling values provided does not match the number of heliostat ID's provided." );
            //assign soiling(s)
            for(size_t i=0; i<helios.size(); i++)
                helios.at(i)->getEfficiencyObject()->soiling = svec->at(i).as_number();

        }
        else if( varname == "reflectivity" )
        {
            //make sure the heliostat ID's array is the same length as the soiling array
            std::vector< lk::vardata_t > *svec = cxt.arg(1).hash()->at( "reflectivity" )->vec();
            if( svec->size() != helios.size() )
                throw lk::error_t( "The number of reflectivity values provided does not match the number of heliostat ID's provided." );
            //assign reflectivities
            for(size_t i=0; i<helios.size(); i++)
                helios.at(i)->getEfficiencyObject()->reflectivity = svec->at(i).as_number();

        }
        else if( varname == "enabled" )
        {
            //make sure the heliostat ID's array is the same length as the soiling array
            std::vector< lk::vardata_t > *svec = cxt.arg(1).hash()->at( "enabled" )->vec();
            if( svec->size() != helios.size() )
                throw lk::error_t( "The number of 'enabled' values provided does not match the number of heliostat ID's provided." );
            //assign reflectivities
            for(size_t i=0; i<helios.size(); i++)
                helios.at(i)->IsEnabled( svec->at(i).as_boolean() ? true : false );

        }

    }

}

static void _save_from_script( lk::invoke_t &cxt )
{
    LK_DOC("save", "Save the current case as a SolarPILOT .spt file. Returns true if successful.", "(string:path):boolean");

    SPFrame &F = SPFrame::Instance();
    SolarField *SF = F.GetSolarFieldObject();
    var_map *V = SF->getVarMap();

    std::string fname = cxt.arg(0).as_string();
    if(! ioutil::dir_exists( ioutil::path_only(fname).c_str() ) )
    {
        cxt.result().assign( 0. );
        return;
    }

    try
    {
        ioutil::saveXMLInputFile(fname, *V,  *F.GetParametricDataObject(), *F.GetOptimizationDataObject(), F.GetVersionInfo()+" (lk script)");

        cxt.result().assign( 1. );
    }
    catch(...)
    {
        cxt.result().assign( 0. );
        return;
    }
}

static void _dump_varmap( lk::invoke_t &cxt )
{
    LK_DOC("dump_variables", "Dump the variable structure to a text csv file. Returns true if successful.", "(string:path):boolean");
    
    //valid path?
    std::string fname = cxt.arg(0).as_string();
    if(! ioutil::dir_exists( ioutil::path_only(fname).c_str() ) )
    {
        cxt.result().assign( 0. );
        return;
    }
    
    var_map *V = SPFrame::Instance().GetSolarFieldObject()->getVarMap();

    try
    {
        //create a list of all variable keys
        std::vector< std::string > names;
        names.reserve( V->_varptrs.size() );

        for(unordered_map< std::string, spbase* >::iterator it=V->_varptrs.begin(); it != V->_varptrs.end(); it++ )
            names.push_back( it->first );

        sort(names.begin(), names.end());   //output the variables alphabetically

        ofstream of( fname );
        std::string sep = " ,";

        if( of.is_open() )      //check whether the file is accessible
        {
            for( size_t i=0; i<names.size(); i++)
            {
                spbase *var = V->_varptrs.at(names.at(i));

                std::string val = var->as_string();

                if( val.size() > 30 )      //tuncate very long values
                {
                    val.erase(val.begin()+20, val.end());
                    val += "... (truncated)";
                }

                //replace all commas
                std::string::size_type n = 0;
                while( (n = val.find(",", n) ) != std::string::npos )
                {
                    val.replace( n, 1, "'");
                    n += 1;
                }

                std::string units = var->units;
                //replace all commas
                n = 0;
                while( (n = units.find(",", n) ) != std::string::npos )
                {
                    units.replace( n, 1, "'");
                    n += 1;
                }

                of << names.at(i) << sep << val << sep << units << sep << var->short_desc << "\n";
            }
    
            of.close();
            cxt.result().assign( 1. );  //success
        }

    }
    catch(exception &e)
    {
        cxt.error().assign( e.what() );
    }
    
    cxt.result().assign( 0. );  //failed

}

static void _open_from_script( lk::invoke_t &cxt )
{
    LK_DOC("open_project", "Open a SolarPILOT .spt case file. Returns true if successful. Updates the interface.", "(string:path):boolean");

    std::string fname = cxt.arg(0).as_string();
    if(! ioutil::dir_exists( ioutil::path_only(fname).c_str() ) )
    {
        cxt.result().assign( 0. );
        return;
    }

    SPFrame &F = SPFrame::Instance();

    try
    {
        F.Open( fname, true );
        cxt.result().assign( 1. );
        return;
    }
    catch(exception &e)
    {
        cxt.error().assign( e.what() );
    }

    cxt.result().assign( 0. );

}


static void _update_interface( lk::invoke_t &cxt )
{
    LK_DOC("update_interface", "Synchronize the user interface input and calculated values to match changes introduced by the script.", "(void):void" );

    SPFrame &F = SPFrame::Instance();

    F.UpdateCalculatedGUIValues();
    F.UpdateInputValues();
    F.UpdateFieldPlotSelections();
    F.UpdateFluxPlotSelections();
    
}

static lk::fcall_t *solarpilot_functions()
{
    static lk::fcall_t st[] = {
        _generate_layout,
        _update_geometry,
        
        _add_receiver,
        _drop_receiver,

        _sp_var,
        _simulate,
        _summary_results,
        _detail_results,
        _get_layout_info,
        _get_fluxmap,
        _optimize,
        _heliostats_by_region,
        _modify_heliostats,
        _add_heliostat_template,
        _drop_heliostat_template,
        _reset_sf,
        _save_from_script,
        _dump_varmap,
        _open_from_script,
        _update_interface,
        _add_land,
        _clear_land,
        0 };

    return (lk::fcall_t*)st;
}


//--------------------- scripting --------------------------------------
SolarPILOTScriptWindow::SolarPILOTScriptWindow( wxWindow *parent, int id )
    : wxLKScriptWindow( parent, id )
{
    GetEditor()->RegisterLibrary( solarpilot_functions(), "SolarPILOT Functions");
	//when creating a new script window, we'll usurp the callback function and then reset it if the window is closed later.
	simulation_info *sim = SPFrame::Instance().GetSolarFieldObject()->getSimInfoObject();
	
	_ui_default_callback = static_cast<bool (*)(simulation_info*, void*)>( sim->getCallbackFunction() );
	_ui_default_data = sim->getCallbackData();

    sim->setCallbackFunction(LKInfoCallback, (void*)this);

    _reporting_enabled = true;

    this->AddOutput("\nTip: Use the 'Help' button to look up and add variables to the script.");
}

SolarPILOTScriptWindow::~SolarPILOTScriptWindow()
{
    if (!SPFrame::Destroyed())
    {
        SPFrame::Instance().SetScriptWindowPointer(0);

	    SPFrame::Instance().GetSolarFieldObject()->getSimInfoObject()->setCallbackFunction(_ui_default_callback, _ui_default_data);
    }
}

void SolarPILOTScriptWindow::ScriptOutput(const char *msg)
{
    wxString fmsg = msg;
    fmsg.Append(wxT("\n"));

    this->AddOutput(msg);
}

void SolarPILOTScriptWindow::EnableScriptWindowReporting(bool enabled)
{
    _reporting_enabled = enabled;
}

bool SolarPILOTScriptWindow::IsReportingEnabled()
{
    return _reporting_enabled;
}

static std::string join(std::vector<std::string> &items, std::string sep)
{
    std::string result = items.front();
    for (size_t i = 1; i < items.size(); i++)
        result.append(sep + items.at(i));
    return result;
}

void SolarPILOTScriptWindow::OnHelp( )
{
    var_map *V = SPFrame::Instance().GetVariablesObject();
    wxFileName fn = SPFrame::Instance().GetImageDir();

    par_variables_dialog *dlg = new par_variables_dialog(this, wxID_ANY, fn.GetPath(true), false, wxT("Variable lookup"));
    dlg->SetItems(V, -1, true); 
    
    dlg->SetSize(450, 550);

    if (dlg->ShowModal() == wxID_OK)
    {
        wxArrayStr vals = dlg->GetCheckedNames();

        std::string names;

        for (int i = 0; i < (int)vals.size(); i++)
        {
            spbase* v = V->_varptrs.at(vals[i].ToStdString());
            names.append( "var(\"" + v->name + "\")" );
            if (v->ctype == "combo")
                names.append("   /*" + join(v->combo_get_choices(), ",") + "*/");
            names.append("\n");
        }
        this->GetEditor()->InsertText(this->GetEditor()->GetCurrentPos(), names);
    }
}

void SolarPILOTScriptWindow::OnScriptStarted()
{
}

void SolarPILOTScriptWindow::OnScriptStopped()
{
}

SolarPILOTScriptWindowFactory::SolarPILOTScriptWindowFactory()
{
    // nothing to do
}

SolarPILOTScriptWindowFactory::~SolarPILOTScriptWindowFactory()
{
    // nothing to do
}

wxLKScriptWindow *SolarPILOTScriptWindowFactory::Create()
{
    wxLKScriptWindow *sw = new SolarPILOTScriptWindow( &SPFrame::Instance(), wxID_ANY );
    SPFrame::Instance().SetScriptWindowPointer( (SolarPILOTScriptWindow*)(sw) );

#ifdef __WXMSW__
    sw->SetIcon( wxICON( appicon ) );
#endif    
    return sw;
}
