// ReportsManager.cpp: implementation of the CReportsManager class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ReportsManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSingleReportsManager::CSingleReportsManager()
{
	m_vReportToCompare.clear();
}

CSingleReportsManager::~CSingleReportsManager()
{

}

BOOL CSingleReportsManager::LoadData( const CString& strPath, CModelsManager *pModelsManager )
{
	if(strPath == "")
		return FALSE;
	
	m_vReportToCompare.clear();
	
	CReportToCompareDataSet dsReport;

	dsReport.SetInputParam(pModelsManager);
	dsReport.loadDataSet(strPath);
	if (dsReport.GetReports(m_vReportToCompare))
		return TRUE;
	else
		return FALSE;
}

BOOL CSingleReportsManager::AddReport( const CReportToCompare& report)
{
	m_vReportToCompare.push_back(report);
	
	return TRUE;
}

void CSingleReportsManager::RemoveReport( int nIndex )
{
	ASSERT( nIndex >= 0 && (unsigned)nIndex < m_vReportToCompare.size() );
	
	m_vReportToCompare.erase( m_vReportToCompare.begin() + nIndex );
}

CReportToCompare& CSingleReportsManager::getReport( int nIndex )
{
	ASSERT( nIndex >= 0 && (unsigned)nIndex< m_vReportToCompare.size() );
	
	return m_vReportToCompare[nIndex];
}


void CSingleReportsManager::DeleteModelParameter(const CString& strModelUniqueName)
{
	std::vector<CReportToCompare>::size_type i = 0;

	for (;i < m_vReportToCompare.size(); ++i)
	{
		m_vReportToCompare.at(i).DeleteModelParameter(strModelUniqueName);
	}
}

bool CSingleReportsManager::IsNameAvailable( const CString& strName ) const
{
	std::vector<CReportToCompare>::size_type nReport;
	for(nReport = 0; nReport < m_vReportToCompare.size(); nReport++)
	{
		if(strName.CompareNoCase(m_vReportToCompare[nReport].GetName()) == 0)
		{
			return false;
		}
	}
	return true;
}

bool CSingleReportsManager::IsCategoryAvailable( int nReportType ) const
{
	for(int i = 0; i < (int)m_vReportToCompare.size(); i++)
	{
		if(m_vReportToCompare.at(i).GetCategory() == nReportType)
		{
			return false;
		}
	}
	return true;
}

void CSingleReportsManager::SaveData( const CString& strPath )
{
	CReportToCompareDataSet dsReport;
	dsReport.SetReports(m_vReportToCompare);
	dsReport.saveDataSet(strPath, false);
}

BOOL CSingleReportsManager::DeleteReport( const CString& strReportName )
{
	std::vector<CReportToCompare>::iterator iter;
	CString strTemp;
	for (iter = m_vReportToCompare.begin(); iter != m_vReportToCompare.end(); iter++)
	{
		strTemp = iter->GetName();
		if (strTemp.CompareNoCase(strReportName) == 0)
		{
			m_vReportToCompare.erase(iter);
			return TRUE;
		}
	}
	return FALSE;
}

int CSingleReportsManager::getCount() const
{
	return static_cast<int>(m_vReportToCompare.size());
}

CReportToCompare* CSingleReportsManager::GetReportByName( const CString& strName )
{
	std::vector<CReportToCompare>::iterator iter;
	for (iter = m_vReportToCompare.begin(); iter != m_vReportToCompare.end(); iter++)
	{
		if (strName.CompareNoCase(iter->GetName()) == 0)
		{
			return &(*iter);
		}
	}
	return NULL;
}

void CSingleReportsManager::UpdateReport(const CString& strOldName, const CReportToCompare& report)
{
	std::vector<CReportToCompare>::iterator iter;
	for (iter = m_vReportToCompare.begin(); iter != m_vReportToCompare.end(); iter++)
	{
		if (strOldName.CompareNoCase(iter->GetName()) == 0)
		{
			*iter = report;
			break;
		}
	}
}
