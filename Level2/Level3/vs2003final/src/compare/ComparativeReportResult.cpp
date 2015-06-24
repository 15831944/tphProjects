// ComparativeReportResult.cpp: implementation of the CComparativeReportResult class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ComparativeReportResult.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CComparativeReportResult::CComparativeReportResult()
{
	m_vSampleRepPaths.clear();
}

CComparativeReportResult::~CComparativeReportResult()
{
	m_vSampleRepPaths.clear();
}

void CComparativeReportResult::AddSamplePath(const std::vector<std::string>& _vSamplePaths)
{
	for(std::vector<std::string>::const_iterator iter = _vSamplePaths.begin(); iter!=_vSamplePaths.end(); iter++)
	{
		m_vSampleRepPaths.push_back( *iter );
	}
}