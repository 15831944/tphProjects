#include "StdAfx.h"
#include ".\lsvehicletotaldelayparam.h"

LSVehicleTotalDelayParam::LSVehicleTotalDelayParam(void)
{
	//10 mins
	m_lInterval = 10 * 60 * TimePrecision;
}

LSVehicleTotalDelayParam::~LSVehicleTotalDelayParam(void)
{
}

CString LSVehicleTotalDelayParam::GetReportParamName()
{
	return  _T("\\LandsideVehicleTotalDelay\\Param.pac");
}
