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

class CSingleReportsManager  
{
public:
	CSingleReportsManager();
	CSingleReportsManager(const CSingleReportsManager& _other) { *this=_other;}

	virtual ~CSingleReportsManager();

	BOOL LoadData( const CString& strPath, CModelsManager *pModelsManager );
	void SaveData(const CString& strPath);

	BOOL AddReport( const CReportToCompare& report);
	//delete old report, add new one
	void UpdateReport(const CString& strOldName, const CReportToCompare& report);

	int getCount()const;
	CReportToCompare& getReport( int nIndex );
	CReportToCompare* GetReportByName( const CString& strName);

	//stop using this method if you have to
	//consider to use getCount(), getReport()
	std::vector<CReportToCompare>& GetReportsList()
	{
		return m_vReportToCompare;
	}
	
	CSingleReportsManager& operator = ( const CSingleReportsManager& _rhs )
	{
		m_vReportToCompare = _rhs.m_vReportToCompare;
		return *this;
	}

	void DeleteModelParameter(const CString& strModelUniqueName);

	bool IsNameAvailable(const CString& strName) const;
	
	//
	bool IsCategoryAvailable(int nReportType) const;

	BOOL DeleteReport( const CString& strReportName );
	void RemoveReport( int nIndex );

private:
	std::vector<CReportToCompare> m_vReportToCompare;
};

#endif // !defined(AFX_REPORTSMANAGER_H__9F793DC9_3061_4A7A_9177_55341FEC1512__INCLUDED_)
