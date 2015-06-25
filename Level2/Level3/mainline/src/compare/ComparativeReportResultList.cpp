// ComparativeReportResultList.cpp: implementation of the CComparativeReportResultList class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ComparativeReportResultList.h"
#include "ReportToCompare.h"
#include "ReportsManager.h"
#include "InputParameter.h"
#include "ComparativeQLengthReport.h"
#include "ComparativeQTimeReport.h"
#include "ComparativeThroughputReport.h"
#include "ComparativeSpaceDensityReport.h"
#include "ComparativePaxCountReport.h"
#include "ComparativeAcOperationReport.h"
#include "ComparativeTimeTerminalReport.h"
#include "ComparativeDistanceTravelReport.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCmpReportManager::CCmpReportManager()
{

}

CCmpReportManager::~CCmpReportManager()
{
	for(std::vector<CCmpBaseReport*>::iterator iter = m_vReport.begin(); iter!=m_vReport.end(); iter++)
	{
		delete (*iter);
	}
	m_vReport.clear();
}

void CCmpReportManager::LoadData( CCmpReportParameter *pParam, const CString& strProjPath )
{
	CSingleReportsManager* pSingleReportManager = pParam->GetReportsManager();
	
	for (int nReport = 0; nReport < pSingleReportManager->getCount(); nReport++)
	{
		const CReportToCompare& report2Cmp = pSingleReportManager->getReport(nReport);

		CCmpBaseReport* pResult = NULL;


		switch (report2Cmp.GetCategory())
		{
		case ENUM_QUEUELENGTH_REP:
			pResult = new CComparativeQLengthReport;
			break;

		case ENUM_QUEUETIME_REP:
			pResult = new CComparativeQTimeReport;
			break;

		case ENUM_THROUGHPUT_REP:
			pResult = new CComparativeThroughputReport;
			break;

		case ENUM_PAXDENS_REP:
			pResult = new CComparativeSpaceDensityReport;
			break;

		case ENUM_PAXCOUNT_REP:
			pResult = new CComparativePaxCountReport;
			break;
		case ENUM_ACOPERATION_REP:
			pResult = new CComparativeAcOperationReport;
			break;
		case ENUM_DURATION_REP:
			pResult = new CComparativeTimeTerminalReport;
			break;
		case ENUM_DISTANCE_REP:
			pResult = new CComparativeDistanceTravelReport;
			break;

		}
		if (pResult != NULL)
		{
			CString strReportPath = strProjPath + _T("\\Results\\") + report2Cmp.GetName() + _T(".txt");
			pResult->LoadReport(strReportPath.GetBuffer(0));

			AddReportResult(pResult);
		}
	}

}

void CCmpReportManager::Save()
{

}

const CmpReportResultVector& CCmpReportManager::GetReportResult() const
{
	return m_vReport;
}

void CCmpReportManager::GetReportResult( ComparativeReportType repType, std::vector<const CCmpBaseReport*>& _vResult) const
{
	for(CmpReportResultVector::const_iterator iter=m_vReport.begin(); iter!=m_vReport.end(); iter++)
	{
		if( (*iter)->GetReportType() == repType)_vResult.push_back( *iter );
	}
}

const CCmpBaseReport* CCmpReportManager::GetReportResult(int _nIdx) const
{
	if( _nIdx<0 || (unsigned)_nIdx >= m_vReport.size()) return NULL;
	return  m_vReport[ _nIdx ] ;
}

void CCmpReportManager::ClearReportResult()
{
	for(std::vector<CCmpBaseReport*>::iterator iter = m_vReport.begin(); iter!=m_vReport.end(); iter++)
	{
		delete (*iter);
	}
	m_vReport.clear();
}

void CCmpReportManager::AddReportResult( CCmpBaseReport* pResult )
{
	if(pResult)
	{
		m_vReport.push_back(pResult);
	}
}
