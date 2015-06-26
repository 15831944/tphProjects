#include "StdAfx.h"
#include "FlightStandOperationChartResult.h"



CFlightStandOperationChartResult::CFlightStandOperationChartResult()
{

}

CFlightStandOperationChartResult::~CFlightStandOperationChartResult()
{

}

void CFlightStandOperationChartResult::SetUnuseActualStandCount( int iCount )
{
	m_nUnuseActualStandCount = iCount;
}

void CFlightStandOperationChartResult::SetUnuseScheduleStandCount( int iCount )
{
	m_nUnuseScheduleStandCount = iCount;
}


