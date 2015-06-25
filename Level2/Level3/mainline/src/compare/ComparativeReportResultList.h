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

class CCmpReportParameter;

typedef std::vector<CCmpBaseReport*> CmpReportResultVector;
class CCmpReportManager  
{
public:
	CCmpReportManager();
	virtual ~CCmpReportManager();
protected:
	CmpReportResultVector m_vReport;
public:
	void LoadData(CCmpReportParameter *pParam, const CString& strProjPath);
	void Save();
	const CmpReportResultVector& GetReportResult() const;
	const CCmpBaseReport* GetReportResult(int _nIdx) const;
	void GetReportResult( ComparativeReportType repType, std::vector<const CCmpBaseReport*>& _vResult) const;

	void AddReportResult(CCmpBaseReport* pResult);

	void ClearReportResult();
};

#endif // !defined(AFX_COMPARATIVEREPORTRESULTLIST_H__FE77B92F_6A45_4A04_BD0D_040BEC998113__INCLUDED_)
