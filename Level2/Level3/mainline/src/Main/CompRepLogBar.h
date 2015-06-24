#if !defined(AFX_COMPREPLOGBAR_H__8509581D_6B9F_42FF_8075_5079F72B66B3__INCLUDED_)
#define AFX_COMPREPLOGBAR_H__8509581D_6B9F_42FF_8075_5079F72B66B3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CompRepLogBar.h : header file
//

#define WM_COMPARE_RUN WM_USER + 256

/////////////////////////////////////////////////////////////////////////////
// CCompRepLogBar window
#include "scbarg.h"
#include "ShapesListBox.h"
#include "XPStyle/GfxOutBarCtrl.h"
#include "common/ProgressBar.h"

class CComparativeProject;

class CCompRepLogBar : public CSizingControlBarG
{
// Construction
public:
	CCompRepLogBar();
	CComparativeProject*	m_pProj;

// Form Data
public:
	//{{AFX_DATA(CCompRepLogBar)
	CString	m_strLogText;
	CEdit m_edtRunLogs;
	CButton m_btnExport;
	CButton m_btnPrint;
	CButton m_btnControl;
	CButton m_btnCancel;
	//CProgressCtrl m_prgProgress;
	CProgressCtrl	m_prgProgress;
	//}}AFX_DATA

// Attributes
public:
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCompRepLogBar)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

// Implementation
public:
	void DeleteLogText();
	const std::vector<CString>& GetLogText() const
	{
		return m_vLogText;
	}
	void SetLogText(const CString& strText);
	void AddLogText(const CString& strText, UINT nFlag = 0);
	void SetProgressRange(int nStart, int nEnd);
	void SetProgressPos(int nPos);
	int GetProgressPos();
	virtual ~CCompRepLogBar();

	void SetParentIndex(UINT nIndex)
	{
		m_nIndex = nIndex;
	}

	// Generated message map functions
protected:
	//{{AFX_MSG(CCompRepLogBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLogExport();
	afx_msg void OnLogPrint();
	afx_msg void OnLogControl();
	afx_msg void OnLogCancel();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnWindowPosChanged(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnClose();
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnNcLButtonUp(UINT nHitTest, CPoint point);
	//}}AFX_MSG
	afx_msg LRESULT OnComparativeRun(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

	UINT m_nIndex;
protected:

	enum 
	{ 
		IDC_LOG_RUNLOGS = 1, 
	    IDC_LOG_EXPORT, 
		IDC_LOG_PRINT,
		IDC_LOG_CONTROL,
		IDC_LOG_CANCEL,
		IDC_LOG_PROGRESS
	};

	std::vector<CString> m_vLogText;

	CFont m_Font;

	void ShowLogText();

	BOOL m_bLButtonDown;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMPREPLOGBAR_H__8509581D_6B9F_42FF_8075_5079F72B66B3__INCLUDED_)
