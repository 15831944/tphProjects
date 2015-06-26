#pragma once

#include "ComparativeProject.h"
#include "../Engine/terminal.h"

class CCmpReport
{
public:
	CCmpReport(void);
	virtual ~CCmpReport(void);
public:
	BOOL CheckData();
	BOOL GetModifyFlag();
	void SetModifyFlag(BOOL bModified);
	CString GetHostName(const CString& strFolder);
	Terminal* GetTerminal();
	BOOL Run(HWND hwnd, CCompRepLogBar* pWndStatus,void (CALLBACK * _ShowCopyInfo)(LPCTSTR));
	BOOL LoadProject(const CString& strName, const CString& strDesc);
	BOOL ProjExists(const CString& strName);
	BOOL SaveProject();
	CComparativeProject* GetComparativeProject();
	bool InitReport(const CString& strName);
protected:
	BOOL m_bModified;
	CComparativeProject* m_compProject;
};
