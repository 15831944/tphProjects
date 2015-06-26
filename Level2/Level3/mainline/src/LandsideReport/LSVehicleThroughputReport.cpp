#include "StdAfx.h"
#include ".\lsvehiclethroughputreport.h"
#include "Common\TERMFILE.H"
#include "Landside\InputLandside.h"
#include "LSVehicleThroughputParam.h"
#include "LSThroughputDetailResult.h"
#include "LSThroughputSummaryResult.h"

LSVehicleThroughputReport::LSVehicleThroughputReport(void)
{
	m_pParemeter = new LSVehicleThroughputParam;
}

LSVehicleThroughputReport::~LSVehicleThroughputReport(void)
{
	if (m_pParemeter)
	{
		delete m_pParemeter;
		m_pParemeter = NULL;
	}

	if (m_pResult)
	{
		delete m_pResult;
		m_pResult = NULL;
	}
}

void LSVehicleThroughputReport::GenerateReport( CBGetLogFilePath pFunc, InputLandside *pLandside )
{
	if(m_pParemeter == NULL)
		return;
	delete	m_pResult;
	m_pResult = NULL;


	//only detail report

	if(m_pParemeter->getReportType() == LandsideReport_Detail)
	{
		m_pResult = new LSThroughputDetailResult;

	}
	else if(m_pParemeter->getReportType() == LandsideReport_Summary )
	{
		m_pResult = new LSThroughputSummaryResult;		
	}
	else//should not be here, it must be something wrong
	{
		ASSERT(0);
	}



	if(m_pResult == NULL)
		return;

	m_pResult->GenerateResult(pFunc, m_pParemeter,pLandside);
}

int LSVehicleThroughputReport::GetReportType() const
{
	return LANDSIDE_RESOURCE_THROUGHPUT;
}

CString LSVehicleThroughputReport::GetReportFileName()
{
	return _T("LandsideVehicleThroughput\\Report.rep");
}

void LSVehicleThroughputReport::RefreshReport()
{

}

BOOL LSVehicleThroughputReport::WriteReportData( ArctermFile& _file )
{
	ASSERT(m_pParemeter != NULL);
	if(m_pParemeter == NULL)
		return FALSE;


	//header
	_file.writeLine();

	//type
	_file.writeInt((int)m_pParemeter->getReportType());
	_file.writeLine();

	if(m_pResult)
		m_pResult->WriteReportData(_file);

	return TRUE;
}

BOOL LSVehicleThroughputReport::ReadReportData( ArctermFile& _file )
{
	//clear result data
	delete m_pResult;
	m_pResult = NULL;

	_file.getLine();
	int nDetailOrSummary = 0;
	_file.getInteger(nDetailOrSummary);
	_file.getLine();
	if(nDetailOrSummary == LandsideReport_Detail)
	{  //detail report
		m_pResult = new LSThroughputDetailResult;
	}
	else if(nDetailOrSummary == LandsideReport_Summary)
	{
		m_pResult = new LSThroughputSummaryResult;		
	}

	if(m_pResult)
	{
		return m_pResult->ReadReportData(_file);
	}

	return TRUE;
}
