#if !defined(AFX_GANNCHARTFRAMEWND_H__B86D4AB0_03B5_486E_8138_2E8EFCBAD223__INCLUDED_)
#define AFX_GANNCHARTFRAMEWND_H__B86D4AB0_03B5_486E_8138_2E8EFCBAD223__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GannChartFrameWnd.h : header file
//
#include "../Inputs/GateAssignmentMgr.h"
class CGannChartWnd;
class CListBoxEx;

/////////////////////////////////////////////////////////////////////////////
// CGannChartFrameWnd window

class CGannChartFrameWnd : public CWnd
{
// Construction
public:
	CGannChartFrameWnd(InputTerminal* _pInTerm);

// Attributes
public:

// Operations
public:
	int m_nCurrentPos;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGannChartFrameWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetVScrollBar( int _nCurPos );
	void SetProcStartIndex( int _nIdx );
	int GetIndex( HWND _hWnd );
	void ZoomOut();
	void ZoomIn();

	// called when the assigned information changed. assume the gate label is not changed.
	void RefreshAssignInfo(  std::vector<CAssignGate>& _vectGate, int _nPosHour  );

	// Set the gate for the first time.
	void SetGate( const std::vector<CAssignGate>& _vectGate );

	virtual ~CGannChartFrameWnd();
	int GetVPos(void);

	// Generated message map functions

protected:
	InputTerminal* m_pInTerm;
	int m_nCurSel;
	UINT m_nSBCODE;
	std::vector<CAssignGate> m_vectGate;
	CScrollBar* m_pScrollBar;
	CGannChartWnd* m_pWndGannChart;
	CListBoxEx* m_pListBoxWnd;


	// refresh to the latest gate assign info from the m_vgate.
	void RefreshAssignInfo( int _nPosHour );

	//{{AFX_MSG(CGannChartFrameWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg LRESULT OnGCLClick( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnGCLDBClick( WPARAM wParam, LPARAM lParam );
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GANNCHARTFRAMEWND_H__B86D4AB0_03B5_486E_8138_2E8EFCBAD223__INCLUDED_)
