// ReportParameter2.cpp: implementation of the ReportParameter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ReportParameter2.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


ReportParameter::ReportParameter()
{
	m_nInterval = 0;
	m_nReportType = 0;
	m_nThreshold = 0;
	m_nPassagerType = 0;
}

ReportParameter::~ReportParameter()
{

}

void ReportParameter::SetReportParamter( const ElapsedTime& tStart, const ElapsedTime& tEnd, 
									     int nInterval, int nReportType, 
									     int nThreshold, int nPassagerType )
{
	m_tStart = tStart;
	m_tEnd = tEnd;
	m_nInterval = nInterval;
	m_nReportType = nReportType;
	m_nThreshold = nThreshold;
	m_nPassagerType = nPassagerType;
}

void ReportParameter::GetReportParamter( ElapsedTime& tStart, ElapsedTime& tEnd, 
		  							     int& nInterval, int& nReportType, 
									     int& nThreshold, int& nPassagerType ) const
{
	tStart = m_tStart;
	tEnd = m_tEnd;
	nInterval = m_nInterval;
	nReportType = m_nReportType;
	nThreshold = m_nThreshold;
	nPassagerType = m_nPassagerType;
}

BOOL ReportParameter::LoadData(const CString& strPath)
{
	if(strPath == "")
		return FALSE;

	return TRUE;
}
