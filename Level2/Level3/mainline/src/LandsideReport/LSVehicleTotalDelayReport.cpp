#include "StdAfx.h"
#include ".\lsvehicletotaldelayreport.h"
#include "LSVehicleTotalDelayParam.h"
#include "LandsideBaseParam.h"
#include "LandsideBaseResult.h"
#include "LSVehicleTotalDelayDetailResult.h"
#include "LSVehicleTotalDelaySummaryResult.h"

LSVehicleTotalDelayReport::LSVehicleTotalDelayReport(void)
{
	m_pParemeter = new LSVehicleTotalDelayParam;
}

LSVehicleTotalDelayReport::~LSVehicleTotalDelayReport(void)
{
}

void LSVehicleTotalDelayReport::GenerateReport( CBGetLogFilePath pFunc, InputLandside *pLandside )
{
	if(m_pParemeter == NULL)
		return;
	delete	m_pResult;
	m_pResult = NULL;


	//only detail report

	if(m_pParemeter->getReportType() == LandsideReport_Detail)
	{
		m_pResult = new LSVehicleTotalDelayDetailResult;

	}
	else if(m_pParemeter->getReportType() == LandsideReport_Summary )
	{
		m_pResult = new LSVehicleTotalDelaySummaryResult;		
	}
	else//should not be here, it must be something wrong
	{
		ASSERT(0);
	}



	if(m_pResult == NULL)
		return;

	m_pResult->GenerateResult(pFunc, m_pParemeter,pLandside);
}

int LSVehicleTotalDelayReport::GetReportType() const
{
	return LANDSIDE_VEHICLE_TOTALDELAY;
}

CString LSVehicleTotalDelayReport::GetReportFileName()
{
	return _T("\\LandsideVehicleTotalDelay\\Report.rep");
}

void LSVehicleTotalDelayReport::RefreshReport()
{

}

BOOL LSVehicleTotalDelayReport::WriteReportData( ArctermFile& _file )
{
	ASSERT(m_pParemeter != NULL);
	if(m_pParemeter == NULL)
		return FALSE;


	//header
	CString strColumn = _T("Vehicle ID, Vehicle Type, Entry Time, Entry Resource,Exit Time, Exit Resource,Total Delay Time,Ideal Arrive Time,Actual Time Spent,Ideal Time Spent,Vehicle Complete Type");
	_file.writeField(strColumn);
	_file.writeLine();

	//type
	_file.writeInt((int)m_pParemeter->getReportType());
	_file.writeLine();

	if(m_pResult)
		m_pResult->WriteReportData(_file);

	return TRUE;
}

BOOL LSVehicleTotalDelayReport::ReadReportData( ArctermFile& _file )
{
	//clear result data
	delete m_pResult;
	m_pResult = NULL;

	int nDetailOrSummary = 0;
	_file.getInteger(nDetailOrSummary);
	_file.getLine();
	if(nDetailOrSummary == LandsideReport_Detail)
	{  //detail report
		m_pResult = new LSVehicleTotalDelayDetailResult;
	}
	else if(nDetailOrSummary == LandsideReport_Summary)
	{
		m_pResult = new LSVehicleTotalDelaySummaryResult;		
	}

	if(m_pResult)
	{
		return m_pResult->ReadReportData(_file);
	}


	m_pResult->ReadReportData(_file);
	return TRUE;
}
