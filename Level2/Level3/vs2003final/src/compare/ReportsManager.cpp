// ReportsManager.cpp: implementation of the CReportsManager class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ReportsManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CReportsManager::CReportsManager()
{
	m_vReportToCompare.clear();
}

CReportsManager::~CReportsManager()
{

}

BOOL CReportsManager::LoadData( const CString& strPath )
{
	if(strPath == "")
		return FALSE;
	
	return TRUE;
}

BOOL CReportsManager::AddReport( const CReportToCompare& report)
{
	m_vReportToCompare.push_back(report);
	
	return TRUE;
}

void CReportsManager::RemoveReport( int nIndex )
{
	ASSERT( nIndex >= 0 && (unsigned)nIndex < m_vReportToCompare.size() );
	
	m_vReportToCompare.erase( m_vReportToCompare.begin() + nIndex );
}

CReportToCompare& CReportsManager::GetReport( int nIndex )
{
	ASSERT( nIndex >= 0 && (unsigned)nIndex< m_vReportToCompare.size() );
	
	return m_vReportToCompare[nIndex];
}

void CReportsManager::SetReports(const std::vector<CReportToCompare>& vReports)
{
	m_vReportToCompare = vReports;
}

void CReportsManager::DeleteModelParameter(const CString& strModelUniqueName)
{
	std::vector<CReportToCompare>::size_type i = 0;

	for (;i < m_vReportToCompare.size(); ++i)
	{
		m_vReportToCompare.at(i).DeleteModelParameter(strModelUniqueName);
	}
}