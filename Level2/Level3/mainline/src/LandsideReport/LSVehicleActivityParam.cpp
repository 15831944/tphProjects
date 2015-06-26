#include "StdAfx.h"
#include ".\lsvehicleactivityparam.h"

LSVehicleActivityParam::LSVehicleActivityParam(void)
{
	//10 mins
	m_lInterval = 60 * 60 * TimePrecision;
}

LSVehicleActivityParam::~LSVehicleActivityParam(void)
{
}

CString LSVehicleActivityParam::GetReportParamName()
{
	return  _T("\\LandsideVehicleActivity\\Param.pac");
}
