#if !defined(AFX_CHARTWND_H__97887060_2604_4AA0_AAD1_87337D7863F5__INCLUDED_)
#define AFX_CHARTWND_H__97887060_2604_4AA0_AAD1_87337D7863F5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChartWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CChartWnd form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include "RowWnd.h"
#include "GannCommon.h"

class CChartWnd : public CFormView
{
protected:
public:
	CChartWnd();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CChartWnd)

// Form Data
public:
	//{{AFX_DATA(CChartWnd)
	enum { IDD = IDD_FORMVIEW };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Attributes
public:
	int iWidthPerHour;
	CRowWnd* m_pRowWnd[MAX_NUM_ROWS_ONSCREEN];
// Operations
public:
	void CreateRowWindows();
	void Clear();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChartWnd)
	public:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	protected:
	virtual void OnDraw(CDC* pDC);
	virtual void CalcWindowRect(LPRECT lpClientRect, UINT nAdjustType = adjustBorder);
	//}}AFX_VIRTUAL

// Implementation
protected:
public:
	void SetData( int _nStartIndex );
	virtual ~CChartWnd();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CChartWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHARTWND_H__97887060_2604_4AA0_AAD1_87337D7863F5__INCLUDED_)
