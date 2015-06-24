#if !defined(AFX_PROCESSORTAGWND_H__576A1BDF_41A5_4DF4_94EC_D3AC99346E1D__INCLUDED_)
#define AFX_PROCESSORTAGWND_H__576A1BDF_41A5_4DF4_94EC_D3AC99346E1D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProcessorTagWnd.h : header file
//

class CTermPlanDoc;
/////////////////////////////////////////////////////////////////////////////
// CProcessorTagWnd dialog

class CProcessorTagWnd : public CWnd
{
// Construction
public:
	CProcessorTagWnd(UINT idx, CTermPlanDoc* pDoc);

	//{{AFX_DATA(CProcessorTagWnd)
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProcessorTagWnd)
	protected:
	//}}AFX_VIRTUAL

// Implementation
protected:
	UINT m_nProcIdx;
	CTermPlanDoc* m_pDoc;

	CString m_sTagNames;
	CString m_sTagValues;
	CString m_sTitle;
	CSize m_szTagNames;
	CSize m_szTagValues;

	CString m_sWidestLineValue;

	void DrawTitle(const CRect& r);
	CTermPlanDoc* GetDocument() { return m_pDoc; }

	void BuildProcTagStrings(CString& _sTagNames, CString& _sTagValues, int& _nMaxWidthNames, int& _nMaxWidthValues, int& _nNumLines);
	CSize BuildProcTagStrings(CDC* pDC, CString& _sTagNames, CString& _sTagValues);

	// Generated message map functions
	//{{AFX_MSG(CProcessorTagWnd)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROCESSORTAGWND_H__576A1BDF_41A5_4DF4_94EC_D3AC99346E1D__INCLUDED_)
