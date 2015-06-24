// ReportsManager.h: interface for the CReportsManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REPORTSMANAGER_H__9F793DC9_3061_4A7A_9177_55341FEC1512__INCLUDED_)
#define AFX_REPORTSMANAGER_H__9F793DC9_3061_4A7A_9177_55341FEC1512__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "ReportToCompare.h"

class CReportsManager  
{
public:
	CReportsManager();
	CReportsManager(const CReportsManager& _other) { *this=_other;}

	virtual ~CReportsManager();

	BOOL LoadData( const CString& strPath );
	BOOL AddReport( const CReportToCompare& report);
	void RemoveReport( int nIndex );
	CReportToCompare& GetReport( int nIndex );

	std::vector<CReportToCompare>& GetReportsList()
	{
		return m_vReportToCompare;
	}
	
	CReportsManager& operator = ( const CReportsManager& _rhs )
	{
		m_vReportToCompare = _rhs.m_vReportToCompare;
		return *this;
	}

	void SetReports(const std::vector<CReportToCompare>& vReports);

	void DeleteModelParameter(const CString& strModelUniqueName);

private:
	std::vector<CReportToCompare> m_vReportToCompare;
};

#endif // !defined(AFX_REPORTSMANAGER_H__9F793DC9_3061_4A7A_9177_55341FEC1512__INCLUDED_)
