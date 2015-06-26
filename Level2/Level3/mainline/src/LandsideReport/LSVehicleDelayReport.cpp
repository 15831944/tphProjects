#include "StdAfx.h"
#include ".\lsvehicledelayreport.h"
#include "LSVehicleDelayParam.h"
#include "../Common/Termfile.h"
#include "LSVehicleDelayResult.h"
#include "LSVehicleDelayDetailResult.h"
#include "LSVehicleDelaySummaryResult.h"



LSVehicleDelayReport::LSVehicleDelayReport(void)
{
	m_pParemeter = new LSVehicleDelayParam;
}

LSVehicleDelayReport::~LSVehicleDelayReport(void)
{
}
void LSVehicleDelayReport::GenerateReport( CBGetLogFilePath pFunc, InputLandside *pLandside)
{
	if(m_pParemeter == NULL)
		return;
	delete	m_pResult;
	m_pResult = NULL;


	//only detail report
	
	if(m_pParemeter->getReportType() == LandsideReport_Detail)
	{
		m_pResult = new LSVehicleDelayDetailResult;

	}
	else if(m_pParemeter->getReportType() == LandsideReport_Summary )
	{
		m_pResult = new LSVehicleDelaySummaryResult;		
	}
	else//should not be here, it must be something wrong
	{
		ASSERT(0);
	}



	if(m_pResult == NULL)
		return;

	m_pResult->GenerateResult(pFunc, m_pParemeter,pLandside);
}

int LSVehicleDelayReport::GetReportType() const
{
	return LANDSIDE_VEHICLE_DELAY;
}

CString LSVehicleDelayReport::GetReportFileName()
{
	return _T("\\LandsideVehicleDelay\\Report.rep");
}

void LSVehicleDelayReport::RefreshReport()
{

}

BOOL LSVehicleDelayReport::WriteReportData( ArctermFile& _file )
{
	ASSERT(m_pParemeter != NULL);
	if(m_pParemeter == NULL)
		return FALSE;


	//header
	CString strColumn = _T("ID, Type, Start Time, End Time, Delay Time, Resource ID, Resource Name, Reason, Operation, Vehicle Complete Type");
	_file.writeField(strColumn);
	_file.writeLine();

	//type
	_file.writeInt((int)m_pParemeter->getReportType());
	_file.writeLine();

	if(m_pResult)
		m_pResult->WriteReportData(_file);

	return TRUE;
}

BOOL LSVehicleDelayReport::ReadReportData( ArctermFile& _file )
{
	//clear result data
	delete m_pResult;
	m_pResult = NULL;

	int nDetailOrSummary = 0;
	_file.getInteger(nDetailOrSummary);
	_file.getLine();
	if(nDetailOrSummary == LandsideReport_Detail)
	{  //detail report
		m_pResult = new LSVehicleDelayDetailResult;
	}
	else if(nDetailOrSummary == LandsideReport_Summary)
	{
		m_pResult = new LSVehicleDelaySummaryResult;		
	}

	if(m_pResult)
	{
		return m_pResult->ReadReportData(_file);
	}


	m_pResult->ReadReportData(_file);
	return TRUE;
}