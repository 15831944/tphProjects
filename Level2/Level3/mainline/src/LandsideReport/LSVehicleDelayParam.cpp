#include "StdAfx.h"
#include ".\lsvehicledelayparam.h"

LSVehicleDelayParam::LSVehicleDelayParam(void)
{
	//10 mins
	m_lInterval = 10 * 60 * TimePrecision;
}

LSVehicleDelayParam::~LSVehicleDelayParam(void)
{
}

CString LSVehicleDelayParam::GetReportParamName()
{
	return  _T("\\LandsideVehicleDelay\\Param.pac");
}
