#include "StdAfx.h"
#include ".\lsresourcequeuereport.h"
#include "../Common/Termfile.h"

#include "LSResourceQueueParam.h"
#include "LSResourceQueueDetailResult.h"
#include "LSResourceQueueSummaryResult.h"



LSResourceQueueReport::LSResourceQueueReport(void)
{
	m_pParemeter = new LSResourceQueueParam;
}

LSResourceQueueReport::~LSResourceQueueReport(void)
{
}
void LSResourceQueueReport::GenerateReport( CBGetLogFilePath pFunc, InputLandside *pLandside)
{
	if(m_pParemeter == NULL)
		return;
	delete	m_pResult;
	m_pResult = NULL;


	//only detail report

	if(m_pParemeter->getReportType() == LandsideReport_Detail)
	{
		m_pResult = new LSResourceQueueDetailResult;

	}
	else if(m_pParemeter->getReportType() == LandsideReport_Summary )
	{
		m_pResult = new LSResourceQueueSummaryResult;		
	}
	else//should not be here, it must be something wrong
	{
		ASSERT(0);
	}



	if(m_pResult == NULL)
		return;

	m_pResult->GenerateResult(pFunc, m_pParemeter,pLandside);
}

int LSResourceQueueReport::GetReportType() const
{
	return LANDSIDE_RESOURCE_QUEUELEN;
}

CString LSResourceQueueReport::GetReportFileName()
{
	return _T("\\LandsideResourceQueue\\Report.rep");
}

void LSResourceQueueReport::RefreshReport()
{

}

BOOL LSResourceQueueReport::WriteReportData( ArctermFile& _file )
{
	ASSERT(m_pParemeter != NULL);
	if(m_pParemeter == NULL)
		return FALSE;


	//header
	CString strColumn = _T("Resource ID, Resource Name,ID, Type, Time, Operation");
	_file.writeField(strColumn);
	_file.writeLine();

	//type
	_file.writeInt((int)m_pParemeter->getReportType());
	_file.writeLine();

	if(m_pResult)
		m_pResult->WriteReportData(_file);

	return TRUE;
}

BOOL LSResourceQueueReport::ReadReportData( ArctermFile& _file )
{
	//clear result data
	delete m_pResult;
	m_pResult = NULL;

	int nDetailOrSummary = 0;
	_file.getInteger(nDetailOrSummary);
	_file.getLine();
	if(nDetailOrSummary == LandsideReport_Detail)
	{  //detail report
		m_pResult = new LSResourceQueueDetailResult;
	}
	else if(nDetailOrSummary == LandsideReport_Summary)
	{
		m_pResult = new LSResourceQueueSummaryResult;		
	}

	if(m_pResult)
	{
		return m_pResult->ReadReportData(_file);
	}


	m_pResult->ReadReportData(_file);
	return TRUE;
}