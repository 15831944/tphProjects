#if !defined(AFX_GANNTCHARONELINEWND_H__97EEDD5C_2B3D_4A16_9CB1_2B73BA7057EE__INCLUDED_)
#define AFX_GANNTCHARONELINEWND_H__97EEDD5C_2B3D_4A16_9CB1_2B73BA7057EE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GanntCharOneLineWnd.h : header file
//

#include "..\inputs\in_term.h"
#include "GannRowWnd.h"
#include "GannCommon.h"

/////////////////////////////////////////////////////////////////////////////
// CGanntChartOneLineWnd window

class CGanntChartOneLineWnd : public CWnd
{
// Construction
public:
	CGanntChartOneLineWnd(InputTerminal* _pInTerm);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGanntChartOneLineWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetProcAssnSchd( ProcessorRosterSchedule* _pProcSchd );
	virtual ~CGanntChartOneLineWnd();
	void ZoomOut();
	void ZoomIn();

	// Generated message map functions
protected:
	InputTerminal* m_pInTerm;
	CGannRowWnd* m_pTimeLine;
	CGannRowWnd* m_pGannRowWnd;
	int m_nZoomFactor;

	void CreateRowWindows();

	//{{AFX_MSG(CGanntChartOneLineWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnGCLClick( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnGCLDBClick( WPARAM wParam, LPARAM lParam );
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GANNTCHARONELINEWND_H__97EEDD5C_2B3D_4A16_9CB1_2B73BA7057EE__INCLUDED_)
