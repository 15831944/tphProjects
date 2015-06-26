#pragma once

#include "ComparativeProject.h"
#include "../Engine/terminal.h"

class CCmpReport
{
public:
	CCmpReport(void);
	virtual ~CCmpReport(void);
public:
	void SetFocusReportName(const CString& strFocusReport);
	CString GetFocusReportName();
	CString GetHostName(const CString& strFolder);
	Terminal* GetTerminal();
	BOOL Run(HWND hwnd, CCompRepLogBar* pWndStatus,void (CALLBACK * _ShowCopyInfo)(int, LPCTSTR));
	BOOL LoadProject(const CString& strName, const CString& strDesc);
	BOOL ProjExists(const CString& strName);
	BOOL SaveProject();
	CComparativeProject* GetComparativeProject();

	bool InitReport(const CString& strName);
protected:
	CString m_strFocusRepName;

	CComparativeProject* m_compProject;
};
