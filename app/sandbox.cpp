
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


#if _SANDBOX == 1
#include "GUI_main.h"
#include "SolarField.h"
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <algorithm>

#include "AutoPIlot_API.h"

using namespace std;


static bool DoAnUpdate(simulation_info *siminfo, void *data)
{
    wxProgressDialog *pdlg = static_cast< wxProgressDialog* >( data );
    int ns = siminfo->getCurrentSimulation();
    int nt = siminfo->getTotalSimulationCount();
    string *msg = siminfo->getSimulationNotices();

    int prg = (int)( (double)ns/(double)nt * 1000. );
    
    bool status = pdlg->Update(min(999,prg), wxString::Format("%d/%d %s", ns, nt, (*msg).c_str()) );
    
    wxYield();
    return status;
}


bool SPFrame::Sandbox()
{
    //playground
    

    //----- example of using the single-thread API ---------
    //AutoPIlot_S saPI;
    AutoPIlot_MT saPI;
    saPI.SetMaxThreadCount(4);

    sp_optimize opt;
    sp_ambient amb;
    sp_cost cost;
    sp_heliostats helios;
    sp_receivers recs;
    sp_layout layout;
    
    var_set V;
    //ioutil::parseDefinitionArray(V);
    //
    ////define stuff
    //opt.LoadDefaults(V);
    //amb.LoadDefaults(V);
    //cost.LoadDefaults(V);
    //helios.resize(1);
    //helios.front().LoadDefaults(V);
    //recs.resize(1);
    //recs.front().LoadDefaults(V);
    //layout.LoadDefaults(V);

    saPI.LoadAllDefaultValues(amb, cost, layout, helios, recs, opt, &V);

    //set up the weather data for simulation
    vector<string> wdata;
    for(int i=0; i<_local_wfdat.size(); i++)
        wdata.push_back( _local_wfdat.at(i) );
    saPI.GenerateDesignPointSimulations(amb, V, wdata);
    

    wxProgressDialog *pdlg1 = new wxProgressDialog("Progress", "", 1000, (wxWindow*)0, wxPD_SMOOTH|wxPD_CAN_ABORT);
    pdlg1->Show();
    saPI.SetSummaryCallback( &DoAnUpdate, (void*)pdlg1);

    saPI.Setup(amb, cost, layout, helios, recs);

    saPI.CreateLayout();
    //saPI.Optimize(opt, recs, layout);
    


    vector<vector<double> > 
        flux, sunpos;
    vector<double> eff;

    saPI.CalculateFluxMaps(sunpos, flux, eff, 12, 1);

    

    pdlg1->Destroy();
    //------- end of example ------------

    return false;

}



//#include "Toolbox.cpp"

//bool SPFrame::Sandbox()
//{
//
//    polyclip::Poly A = {{ 50.,150.,0.}, {200., 50.,0.}, {350.,150.,0.},
//                    {350.,300.,0.}, {250.,300.,0.}, {200.,250.,0.},
//                    {150.,350.,0.}, {100.,250.,0.}, {100.,200.,0.} };
//
//    polyclip::Poly B = {{100.,100.,0.}, {300.,100.,0.}, {300.,300.,0.}, {100.,300.,0.}};
//
//    polyclip P;
//    
//    polyclip::Poly R = P.clip(A, B);
//
//    return false;
//};




//bool SPFrame::Sandbox()
//{
//
//        /* Copy and paste following code to solve example of finding minimum inner diameter: */
//        int n_tube_nodes = 10;
//        double d_out = 0.012;            //[m]
//        double T_fluid_in = 470.0;        //[C]
//        double T_fluid_out = 650.0;        //[C]
//        double P_fluid_in = 25.0;        //[MPa]
//        double e_roughness = 4.5E-5;    //[m] Absolute tube roughness
//        double L_tube = 4.1;            //[m] Length of tube
//
//        N_sco2_rec::C_calc_tube_min_th      calc_min_th;
//        
//        double q_abs_total_input = 300000.0;
//        vector<double> max_flux_in(n_tube_nodes);
//        for( int i = 0; i < n_tube_nodes; i++ )
//            max_flux_in[i] = q_abs_total_input - 0.1*q_abs_total_input*(i);        //[W/m2]
//
//        bool is_tube_feasible1= calc_min_th.calc_th_1Dmaxflux_Tout(max_flux_in, L_tube, d_out, T_fluid_in, T_fluid_out, P_fluid_in);
//        double d_in_min = calc_min_th.get_min_d_in();
//        double m_dot_class = calc_min_th.get_m_dot_tube_kgsec();
//        double th_des = (d_out - d_in_min)/2.;
//        double max_damage = calc_min_th.get_max_damage();
//        double pres_drop = calc_min_th.get_deltaP_kPa()/P_fluid_in*0.001;
//        
//
//        bool is_tube_feasible = calc_min_th.calc_th_1Dmaxflux_mdot(max_flux_in, L_tube, d_out, T_fluid_in, P_fluid_in, m_dot_class);
//        d_in_min = calc_min_th.get_min_d_in();
//        double T_fluid_out_calc = calc_min_th.get_T_out_C();
//        th_des = (d_out - d_in_min)/2.;
//        max_damage = calc_min_th.get_max_damage();
//        pres_drop = calc_min_th.get_deltaP_kPa()/P_fluid_in*0.001;
//        
//        bool still_feasible = calc_min_th.calc_perf_1Dmaxflux_mdot(max_flux_in, L_tube, d_out, th_des, T_fluid_in, P_fluid_in, m_dot_class);
//        d_in_min = calc_min_th.get_min_d_in();
//        T_fluid_out_calc = calc_min_th.get_T_out_C();
//        th_des = (d_out - d_in_min)/2.;
//        pres_drop = calc_min_th.get_deltaP_kPa()/P_fluid_in*0.001;
//
//        vector<vector<double> > damage;
//        calc_min_th.get_damage_matrix(damage);
//        max_damage = calc_min_th.get_max_damage();
//
//        return false;
//};

#endif



