// ComparativeReportManager.h: interface for the CComparativeReportManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMPARATIVEREPORTMANAGER_H__46AB8C1F_DE52_4371_8C4B_3B0A34ABFE67__INCLUDED_)
#define AFX_COMPARATIVEREPORTMANAGER_H__46AB8C1F_DE52_4371_8C4B_3B0A34ABFE67__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "ComparativeProject.h"

class CComparativeReportManager  
{
public:
	CComparativeReportManager();
	virtual ~CComparativeReportManager();

public:
	BOOL	LoadData(const CString& strPath, OUT std::vector<CComparativeProject>& vProjects);
};

#endif // !defined(AFX_COMPARATIVEREPORTMANAGER_H__46AB8C1F_DE52_4371_8C4B_3B0A34ABFE67__INCLUDED_)
