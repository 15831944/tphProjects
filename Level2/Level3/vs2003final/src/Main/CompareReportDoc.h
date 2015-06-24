#if !defined(AFX_COMPAREREPORTDOC_H__4977E8E0_CA40_4220_8A11_BB85045390A4__INCLUDED_)
#define AFX_COMPAREREPORTDOC_H__4977E8E0_CA40_4220_8A11_BB85045390A4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CompareReportDoc.h : header file
//

#include "../compare/ComparativeProject.h"
#include "../Engine/terminal.h"
#include "..\Compare\CmpReport.h"

/////////////////////////////////////////////////////////////////////////////
// CCompareReportDoc document

class CCompareReportDoc : public CDocument
{
protected:
	CCompareReportDoc();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CCompareReportDoc)



// Attributes
public:
	CString GetOriginProjName(){return m_strProjName;}
	virtual BOOL DoSave(LPCTSTR lpszPathName, BOOL bReplace = TRUE);

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCompareReportDoc)
	public:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
	protected:
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL CheckData();
	BOOL GetModifyFlag();
	void SetModifyFlag(BOOL bModified);
	CString GetHostName(const CString& strFolder);
	BOOL InitTerminal();
	Terminal& GetTerminal();
	BOOL Run(HWND hwnd, CCompRepLogBar* pWndStatus);
	BOOL LoadProject(const CString& strName, const CString& strDesc);
	BOOL SaveProject();
	CComparativeProject* GetComparativeProject();
	CCmpReport* GetCmpReport(){ return &this->m_cmpReport; }
	virtual ~CCompareReportDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	BOOL m_bModified;
	CString m_strProjName;
	CString m_strPrevModelName;
	Terminal m_terminalForReportParam;
	
	CComparativeProject* m_compProject;
	CCmpReport m_cmpReport;
	//{{AFX_MSG(CCompareReportDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMPAREREPORTDOC_H__4977E8E0_CA40_4220_8A11_BB85045390A4__INCLUDED_)
