/*
  COPYRIGHT (C) 2003  Lorenzo Dozio <dozio@aero.polimi.it>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
*/


#define S_FUNCTION_NAME		sfun_rtai_meter
#define S_FUNCTION_LEVEL	2

#ifdef MATLAB_MEX_FILE
#include "mex.h"
#endif
#include "simstruc.h"

#define NUMBER_OF_PARAMS	(1)
#define SAMPLE_TIME_PARAM	ssGetSFcnParam(S,0)
#define SAMPLE_TIME             ((real_T) mxGetPr(SAMPLE_TIME_PARAM)[0])

#ifndef MATLAB_MEX_FILE

#include <rtai_netrpc.h>
#include <rtai_msg.h>
#include <rtai_mbx.h>

#define MAX_RTAI_METERS               1000
#define MBX_RTAI_METER_SIZE           5000
extern SimStruct *rtaiMeter[];
extern char *TargetMeterMbxID;

#endif

static void mdlInitializeSizes(SimStruct *S)
{
	uint_T i;

	ssSetNumSFcnParams(S, NUMBER_OF_PARAMS);
	if (ssGetNumSFcnParams(S) != ssGetSFcnParamsCount(S)) {
		return;
	}
	for (i = 0; i < NUMBER_OF_PARAMS; i++) {
		ssSetSFcnParamNotTunable(S, i);
	}
	ssSetNumInputPorts(S, 1);
	ssSetNumOutputPorts(S, 0);
	ssSetInputPortWidth(S, 0, 1);
	ssSetInputPortDirectFeedThrough(S, 0, 1);
	ssSetNumContStates(S, 0);
	ssSetNumDiscStates(S, 0);
	ssSetNumSampleTimes(S, 1);
	ssSetNumPWork(S, 1);
}

static void mdlInitializeSampleTimes(SimStruct *S)
{
	ssSetSampleTime(S, 0, SAMPLE_TIME);
	ssSetOffsetTime(S, 0, 0.0);
}

#define MDL_START
static void mdlStart(SimStruct *S)
{
#ifndef MATLAB_MEX_FILE
	char name[7];
	MBX *mbx;
	int i;

	for (i = 0; i < MAX_RTAI_METERS; i++) {
		sprintf(name, "%s%d", TargetMeterMbxID, i);
		if (!rt_get_adr(nam2num(name))) break;
	}
	rtaiMeter[i] = S;
	if (!(mbx = (MBX *)rt_mbx_init(nam2num(name), (MBX_RTAI_METER_SIZE/(sizeof(float))*(sizeof(float)))))) {
		printf("Cannot init mailbox\n");
		exit(1);
	}
	ssGetPWork(S)[0]= (void *)mbx;
#endif
}

static void mdlOutputs(SimStruct *S, int_T tid)
{
	InputRealPtrsType uPtrs = ssGetInputPortRealSignalPtrs(S,0);
	float data;

#ifndef MATLAB_MEX_FILE
	MBX *mbx = (MBX *)ssGetPWorkValue(S,0);
	data = (float)*uPtrs[0];
	rt_mbx_send_if(mbx, &data, sizeof(data));
#endif
}

static void mdlTerminate(SimStruct *S)
{
#ifndef MATLAB_MEX_FILE
	MBX *mbx = (MBX *)ssGetPWorkValue(S,0);
	rt_mbx_delete(mbx);
#endif
}

#ifdef  MATLAB_MEX_FILE
#include "simulink.c"
#else
#include "cg_sfun.h"
#endif
