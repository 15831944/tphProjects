#if !defined(AFX_SPLITTERWNDEX_H__0417751B_60C5_4BEA_89E9_C639EF9322E8__INCLUDED_)
#define AFX_SPLITTERWNDEX_H__0417751B_60C5_4BEA_89E9_C639EF9322E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SplitterWndEx.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSplitterWndEx window

class CSplitterWndEx : public CSplitterWnd
{
public:
	CSplitterWndEx();
	virtual ~CSplitterWndEx();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSplitterWndEx)
	virtual void OnDrawSplitter( CDC* pDC, ESplitType nType, const CRect& rect );
	//}}AFX_VIRTUAL


	void RefreshSplitBars(void);

	// Generated message map functions
protected:
	//{{AFX_MSG(CSplitterWndEx)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


class CSplitterWnd2X2 : public CSplitterWndEx
{
public:
	BOOL Create(CWnd* pParentWnd, CCreateContext* pContext);

	// safe split, move splitter if already split
	virtual BOOL SplitRow(int cyBefore);
	virtual BOOL SplitColumn(int cxBefore);

};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPLITTERWNDEX_H__0417751B_60C5_4BEA_89E9_C639EF9322E8__INCLUDED_)
