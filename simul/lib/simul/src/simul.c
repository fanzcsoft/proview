/* 
 * Proview   Open Source Process Control.
 * Copyright (C) 2005-2017 SSAB EMEA AB.
 *
 * This file is part of Proview.
 *
 * This program is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License as 
 * published by the Free Software Foundation, either version 2 of 
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with Proview. If not, see <http://www.gnu.org/licenses/>
 *
 * Linking Proview statically or dynamically with other modules is
 * making a combined work based on Proview. Thus, the terms and 
 * conditions of the GNU General Public License cover the whole 
 * combination.
 *
 * In addition, as a special exception, the copyright holders of
 * Proview give you permission to, from the build function in the
 * Proview Configurator, combine Proview with modules generated by the
 * Proview PLC Editor to a PLC program, regardless of the license
 * terms of these modules. You may copy and distribute the resulting
 * combined work under the terms of your choice, provided that every 
 * copy of the combined work is accompanied by a complete copy of 
 * the source code of Proview (the version used to produce the 
 * combined work), being distributed under the terms of the GNU 
 * General Public License plus this exception.
 */


/*  rt_simul.c */ 

/*  2016 - 03 - 02	Bruno: V0.1 */
/*  2016 - 04 - 11	Bruno: V0.2 */


#include <stdio.h>
#include <math.h>

#include "pwr.h"
#include "simul.h"

// This is for further development:
// #include <gsl/gsl_linalg.h>
// to link you may append -lgsl -lgslcblas


#define false 0
#define true 1
#define TwoPI (2*M_PI)
#define SMIN_T 0.00001
#define IMIN_T -0.00001
#define Clamp(x, min, max) x = ((x)<(min)) ? (min) : (((x)>(max)) ? (max) : (x))   
#define Te tp->ActualScanTime
#define MAXCELLS 100


/*_*
  Sim_SigGen
  function:	signal generator for square, sine and triangle waveforms.

  @aref sim_siggen Sim_SigGen

  2015 - 12 - 5		Bruno: initial object.
  2016 - 04 - 11	Bruno: noise & offset & set output range setting parameters added.
*/

void Sim_SigGen_exec( plc_sThread *tp, pwr_sClass_Sim_SigGen *plc_obj)
{
	pwr_tFloat32 incre = TwoPI * Te / *plc_obj->PeriodP;
	pwr_tFloat32 ka = 1/(1 + *plc_obj->NFilterTP / Te);
	pwr_tFloat32 ef = rand() / (pwr_tFloat32) RAND_MAX - 0.5;
	pwr_tFloat32 Out;

	plc_obj->sf = (1-ka)*plc_obj->sf + ka*ef ;
	
	Clamp(plc_obj->wavei, 0, M_PI); 

	plc_obj->wavei += (plc_obj->sign) ? incre : -incre;
	plc_obj->sign = (plc_obj->sign && plc_obj->wavei >= M_PI) ? false : (!plc_obj->sign && plc_obj->wavei <= 0) ? true : plc_obj->sign; 

	switch (plc_obj->Wave){
		case pwr_eSim_GenType_Triangle:	Out= plc_obj->wavei/M_PI-0.5;
								break;
		case pwr_eSim_GenType_Square:	Out=(plc_obj->sign) ? 0.5: -0.5;
								break;
		case pwr_eSim_GenType_Sine:		Out=sin(((plc_obj->sign) ? plc_obj->wavei : -plc_obj->wavei)+1)/2.0;
								break;
		case pwr_eSim_GenType_None:	Out=0.0;
								break;
		default:					Out=0.0;
	}
	plc_obj->Out = *plc_obj->OffsetP + *plc_obj->RangeP *(Out + *plc_obj->NGainP * plc_obj->sf); // Adding noise & offset & set output range
}


/*_*
  Sim_Integrator
  @aref sim_integrator Sim_Integrator

  function:	performs a discrete time integration with dynamic gain. In Laplace transform:

			 K
	F(s)=	---
		 	 s

  2016 - 03 - 02	Bruno: initial object.
  2016 - 04 - 11	Bruno: cleaning.
*/

void Sim_Integrator_exec( plc_sThread *tp, pwr_sClass_Sim_Integrator *plc_obj)
{
	if (*plc_obj->DynGainP <= 0.0) {plc_obj->Out = *plc_obj->InP;return;}
	pwr_tFloat32 Out;

	Out = plc_obj->Out + Te * *plc_obj->InP * *plc_obj->DynGainP;
	Clamp (Out, plc_obj->LL_OP, plc_obj->HL_OP);
	plc_obj->Out = Out;	
}

/*_*
  Sim_LagFilter
  @aref sim_lagfilter Sim_LagFilter

  function:	performs a discrete time lag filtering. In Laplace transform:

			    1
	F(s)=	----------
		 	1 + Tlag.s

  2015 - 12 - 12	Bruno: initial object.
  2016 - 04 - 11	Bruno: cleaning.
*/

void Sim_LagFilter_exec( plc_sThread *tp, 
                         pwr_sClass_Sim_LagFilter *plc_obj)
{
	if (*plc_obj->LagTimeP <= 0.0) {plc_obj->Out = *plc_obj->InP;return;};
	pwr_tFloat32 ka = 1/(1 + *plc_obj->LagTimeP / Te);
	pwr_tFloat32 Out;

	Out = (1.0-ka) * plc_obj->Out + ka * *plc_obj->InP * *plc_obj->GainP;
	Clamp (Out, plc_obj->LL_OP, plc_obj->HL_OP);
	plc_obj->Out = Out;
}

/*_*
  Sim_LeadLagFilter
  @aref sim_leadlagfilter Sim_LeadLagFilter

  function:	performs a discrete time lead / lag filtering. In Laplace transform:

			1 + Tlead.s
	F(s)=	-----------
			1 + Tlag.s

  2015 - 12 - 12	Bruno: initial object.
  2016 - 04 - 11	Bruno: cleaning.
*/

void Sim_LeadLagFilter_exec( plc_sThread *tp, 
                         pwr_sClass_Sim_LeadLagFilter *plc_obj)
{
	if (*plc_obj->LagTimeP <= 0.0) {plc_obj->Out = *plc_obj->InP;return;}
	pwr_tFloat32 kc = *plc_obj->LeadTimeP/Te;
	pwr_tFloat32 kd = *plc_obj->LagTimeP/Te;
	pwr_tFloat32 ka = 1.0+kd;
	pwr_tFloat32 kb = 1.0+kc;	
	pwr_tFloat32 Out;

	Out = kd/ka * plc_obj->Out + *plc_obj->GainP * ( kb/ka * *plc_obj->InP - kc/ka * plc_obj->PrevIn);
	Clamp (Out, plc_obj->LL_OP, plc_obj->HL_OP);
	plc_obj->Out = Out;
	plc_obj->PrevIn=*plc_obj->InP;
}


/*_*
  Sim_SouFilter
  @aref sim_soufilter Sim_SouFilter

  function:	performs a discrete time second order underdamped filtering. In Laplace transform:

			             1
	F(s)=	-----------------------------
			1/(w0^2).s^2 + 2.ksi/w0.s + 1

	ksi: damping factor
	w0: pseudo resonance pulsation

  2015 - 12 - 19	Bruno: initial object.
  2016 - 04 - 11	Bruno: cleaning.
*/

void Sim_SouFilter_exec(plc_sThread *tp, 
                         pwr_sClass_Sim_SouFilter *plc_obj)
{
	if (*plc_obj->w0P <= 0.0) {plc_obj->Out = *plc_obj->InP;return;}
	pwr_tFloat32 a0 = Te * Te * *plc_obj->w0P * *plc_obj->w0P + 2.0 * *plc_obj->ksiP * Te * *plc_obj->w0P + 1.0;
	pwr_tFloat32 a1 = 2.0 * (*plc_obj->ksiP * Te * *plc_obj->w0P + 1.0);
	pwr_tFloat32 a2 = -1.0;
	pwr_tFloat32 b0 = Te * Te * *plc_obj->w0P * *plc_obj->w0P;
	pwr_tFloat32 Out;

	plc_obj->Outm2 = plc_obj->Outm1;
	plc_obj->Outm1 = plc_obj->Out;
	Out = a1/a0 * plc_obj->Out + a2/a0 * plc_obj->Outm2 + b0/a0 * *plc_obj->InP * *plc_obj->GainP;
	Clamp (Out, plc_obj->LL_OP, plc_obj->HL_OP);
	plc_obj->Out = Out;
}

/*_*
  Sim_SouTOoFilter
  @aref sim_soutoofilter Sim_SouTOoFilter

  function:	performs a discrete time second order uderdamped To overdamped filtering. In Laplace transform:

			1/(w0^2).s^2 + 2.ksi/w0.s + 1
	F(s)=	-----------------------------
			(1 + Tlag1.s) . (1 + Tlag2.s)

	ksi: damping factor
	w0: pseudo resonance pulsation

  2015 - 12 - 19	Bruno: initial object.
  2016 - 04 - 11	Bruno: cleaning.
*/

void Sim_SouTOoFilter_exec(plc_sThread *tp, 
                         pwr_sClass_Sim_SouTOoFilter *plc_obj)
{
	if(*plc_obj->w0P<=0.0) { plc_obj->Out = *plc_obj->InP; return; } 
	pwr_tFloat32 b0 = Te * Te * *plc_obj->w0P * *plc_obj->w0P + 2.0 * *plc_obj->ksiP * Te * *plc_obj->w0P + 1.0;
	pwr_tFloat32 b1 = -2.0 * (*plc_obj->ksiP * Te * *plc_obj->w0P + 1.0);
	pwr_tFloat32 b2 = 1.0;
	pwr_tFloat32 a0;
	pwr_tFloat32 a1 = *plc_obj->w0P * *plc_obj->w0P * (2 * *plc_obj->LagTime1P * *plc_obj->LagTime2P+ Te * (*plc_obj->LagTime1P + *plc_obj->LagTime2P));
	pwr_tFloat32 a2 = - *plc_obj->w0P * *plc_obj->w0P * *plc_obj->LagTime1P * *plc_obj->LagTime2P;
	pwr_tFloat32 Out;
	a0 = (*plc_obj->w0P>0.0) ? *plc_obj->w0P * *plc_obj->w0P * (*plc_obj->LagTime1P+Te)*(*plc_obj->LagTime2P+Te) : 1.0;

	plc_obj->Outm2 = plc_obj->Outm1;
	plc_obj->Outm1 = plc_obj->Out;
	Out = a1/a0 * plc_obj->Out + a2/a0 * plc_obj->Outm2 
		+ b0/a0 * *plc_obj->InP * *plc_obj->GainP 
		+ b1/a0 * plc_obj->Inm1 * *plc_obj->GainP
		+ b2/a0 * plc_obj->Inm2 * *plc_obj->GainP;
	Clamp (Out, plc_obj->LL_OP, plc_obj->HL_OP);
	plc_obj->Inm2 = plc_obj->Inm1;
	plc_obj->Inm1 = *plc_obj->InP;
	plc_obj->Out = Out;
}

/*_*
  Sim_Delay
  @aref sim_delay Sim_Delay

  function:	performs a discrete time delay on a 100 float numbers cells shift register. In Laplace transform:

	F(s)=	exp(-T.s)
			
  2015 - 12 - 12	Bruno: initial object.
  2016 - 04 - 11	Bruno: cleaning.
*/

void Sim_Delay_exec(plc_sThread *tp, 
                         pwr_sClass_Sim_Delay *plc_obj)
	{
		pwr_tFloat32 OP=0.0;
		int i, Ntime, Nscan;	

		if ((int) *plc_obj->DelayP < Te) {plc_obj->Out=*plc_obj->InP; return; }	
		if (plc_obj->PrevDelay != *plc_obj->DelayP) for(i=MAXCELLS-1; i>=0; i--) plc_obj->D[i]=*plc_obj->InP;
		plc_obj->PrevDelay = *plc_obj->DelayP;
		Nscan = (int) (0.5+*plc_obj->DelayP/Te);
		if (Nscan > MAXCELLS)
		{
			Ntime = (int) (0.5+*plc_obj->DelayP/(Te * (pwr_tFloat32) MAXCELLS));	
			if (plc_obj->DtCtr==0) for(i=0; i<=MAXCELLS; i++) plc_obj->D[i]=*plc_obj->InP;
			OP = plc_obj->D[MAXCELLS-1];
			if (plc_obj->DtCtr >= Ntime)
			{
				plc_obj->DtCtr=0;
				for(i=MAXCELLS-1; i>0; i--) plc_obj->D[i]=plc_obj->D[i-1];
				plc_obj->D[0]=*plc_obj->InP;
			}
			plc_obj->DtCtr++;
		}
		else
		{
			OP = plc_obj->D[Nscan-1];
			for(i=Nscan; i>0; i--) plc_obj->D[i]=plc_obj->D[i-1];
			plc_obj->D[0]=*plc_obj->InP;
		}	
		Clamp (OP, plc_obj->LL_OP, plc_obj->HL_OP);
		plc_obj->Out=OP;
	}

/*_*
  Sim_SlewRateFilter
  @aref sim_slewratelimiter Sim_SlewRateLimiter

  function:	performs a slew rate limitation

  2015 - 12 - 5	Bruno: initial object.
  2016 - 04 - 11	Bruno: cleaning.
*/

void Sim_SlewRateLimiter_exec(plc_sThread *tp, 
                         pwr_sClass_Sim_SlewRateLimiter *plc_obj)
{
	pwr_tFloat32 ka = *plc_obj->SlopeP*Te;
	pwr_tFloat32 In=*plc_obj->InP, Out = plc_obj->Out;
	Clamp (In, plc_obj->LL, plc_obj->HL);

	if(abs(Out-In)<ka) Out=In;
	if(In>Out) Out+=ka; 
	if(In<Out) Out-=ka;

	Clamp (Out, plc_obj->LL, plc_obj->HL);
	plc_obj->Out = Out;
}



