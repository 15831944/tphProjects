#if !defined(AFX_PRINTABLETREECTRL_H__68DAE38E_071D_4600_B78B_F4B9CF3B8BC0__INCLUDED_)
#define AFX_PRINTABLETREECTRL_H__68DAE38E_071D_4600_B78B_F4B9CF3B8BC0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PrintableTreeCtrl.h : header file
//
#include "ColorTreeCtrl.h"

#define LEFT_MARGIN 4
#define RIGHT_MARGIN 4
#define TOP_MARGIN 4
#define BOTTOM_MARGIN 4

/////////////////////////////////////////////////////////////////////////////
// CPrintableTreeCtrl window

class CPrintableTreeCtrl : public CColorTreeCtrl
{
// Construction
public:
	CPrintableTreeCtrl();

// Attributes
public:

// Operations
public:
void PrintTree( CString _sHeader, CString _sFoot );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrintableTreeCtrl)
	
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);

	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);

	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPrintableTreeCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPrintableTreeCtrl)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
		
private:
	CString m_sHeader, m_sFoot;
	BOOL  m_bNewPrint;

private:
	CRect rcBounds;
	int m_nCharWidth;
	int m_nRowHeight;
	int m_nRowsPerPage;
	HANDLE hDIB;
	WINDOWPLACEMENT WndPlace;
	// Operations
public:
	//Print each page's Head and Foot
	void PrintHeadFoot(CDC *pDC, CPrintInfo *pInfo);
	//change Bitmap's Format from device-dependent to device-independent
	HANDLE DDBToDIB( CBitmap& bitmap, DWORD dwCompression, CPalette* pPal );

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRINTABLETREECTRL_H__68DAE38E_071D_4600_B78B_F4B9CF3B8BC0__INCLUDED_)
