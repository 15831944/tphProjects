#pragma once

#include "ComparativeProject.h"
#include "../Engine/terminal.h"

class CCmpReport
{
public:
	CCmpReport(void);
	virtual ~CCmpReport(void);
public:
	void SetCurReport(const CString& strCurReport){ m_strCurReport = strCurReport; }
	CString GetCurReport(){ return m_strCurReport; }
	BOOL CheckData();
	BOOL GetModifyFlag();
	void SetModifyFlag(BOOL bModified);
	CString GetHostName(const CString& strFolder);
//	BOOL InitTerminal();
	Terminal* GetTerminal();
	BOOL Run(HWND hwnd, CCompRepLogBar* pWndStatus,void (CALLBACK * _ShowCopyInfo)(LPCTSTR));
	BOOL LoadProject(const CString& strName, const CString& strDesc);
	BOOL ProjExists(const CString& strName);
	BOOL SaveProject();
	CComparativeProject* GetComparativeProject();

	bool InitReport(const CString& strName);
protected:
	BOOL m_bModified;
	CString m_strCurReport;
//	Terminal m_terminalForReportParam;

	CComparativeProject* m_compProject;


};
