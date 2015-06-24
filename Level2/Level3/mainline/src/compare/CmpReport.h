#pragma once

#include "ComparativeProject.h"
#include "../Engine/terminal.h"

class CCmpReport
{
public:
	CCmpReport(void);
	virtual ~CCmpReport(void);
public:
	void SetProjName(const CString& strName){ m_strProjName = strName; }
	void SetCurReport(const CString& strCurReport){ m_strCurReport = strCurReport; }
	CString GetCurReport(){ return m_strCurReport; }
	CString GetOriginProjName(){return m_strProjName;}
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
protected:
	BOOL m_bModified;
	CString m_strProjName;
	CString m_strPrevModelName;
	CString m_strCurReport;
//	Terminal m_terminalForReportParam;

	CComparativeProject* m_compProject;
};
