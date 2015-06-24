// ComparativeReportResultList.h: interface for the CComparativeReportResultList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMPARATIVEREPORTRESULTLIST_H__FE77B92F_6A45_4A04_BD0D_040BEC998113__INCLUDED_)
#define AFX_COMPARATIVEREPORTRESULTLIST_H__FE77B92F_6A45_4A04_BD0D_040BEC998113__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "ComparativeReportResult.h"

#define UM_COMPREPORT_ADDNEW	WM_USER + 30
#define UM_COMPREPORT_REMOVE	WM_USER + 31

typedef std::vector<CComparativeReportResult*> CmpReportResultVector;
class CComparativeReportResultList  
{
public:
	CComparativeReportResultList();
	virtual ~CComparativeReportResultList();
protected:
	CmpReportResultVector m_vReport;
public:
	void LoadData();
	void Save();
	const CmpReportResultVector& GetReportResult() const;
	const CComparativeReportResult* GetReportResult(int _nIdx) const;
	void GetReportResult( ComparativeReportType repType, std::vector<const CComparativeReportResult*>& _vResult) const;

	void AddReportResult(CComparativeReportResult* pResult)
	{
		if(pResult)
		{
			m_vReport.push_back(pResult);
		}
	}

	void ClearReportResult();
};

#endif // !defined(AFX_COMPARATIVEREPORTRESULTLIST_H__FE77B92F_6A45_4A04_BD0D_040BEC998113__INCLUDED_)
