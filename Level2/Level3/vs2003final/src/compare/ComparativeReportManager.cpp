// ComparativeReportManager.cpp: implementation of the CComparativeReportManager class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "ComparativeReportManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CComparativeReportManager::CComparativeReportManager()
{
	
}

CComparativeReportManager::~CComparativeReportManager()
{

}

BOOL CComparativeReportManager::LoadData(const CString& strPath, OUT std::vector<CComparativeProject>& vProjects)
{
	CComparativeProject proj;
	proj.SetName(_T("Demo1"));
	proj.SetDescription(_T("Must be modified"));

	vProjects.push_back(proj);

	return TRUE;
}