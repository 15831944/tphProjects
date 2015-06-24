#if !defined(AFX_CHARTWND_H__F0CCE887_BDD8_4A03_96F4_7E59F77C7203__INCLUDED_)
#define AFX_CHARTWND_H__F0CCE887_BDD8_4A03_96F4_7E59F77C7203__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChartWnd.h : header file
//

#include "GannRowWnd.h"
#include "GannCommon.h"
#include <vector>
#include "inputs\GateAssignmentMgr.h"

/////////////////////////////////////////////////////////////////////////////
// CGannChartWnd window

class CGannChartWnd : public CWnd
{
// Construction
public:
	CGannChartWnd(InputTerminal* _pInTerm);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGannChartWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetSelected( int _nIdx );
	int GetNumLinesOnScreen();
	void SetAssignType( bool _bGateAssignment );
	int GetIndex( HWND _hWnd );
	void ZoomOut();
	void ZoomIn();
	
	// refresh the gate assign information and start postion of the GUI.
	void RefreshAssignInfo( const std::vector<CAssignGate>& _vectGate, int _nPosHour );

	// Set the gate for the first time.
	void SetGate( const std::vector<CAssignGate>& _vectGate );

	void Clear();
	virtual ~CGannChartWnd();

	// Generated message map functions
protected:
	InputTerminal* m_pInTerm;
	bool m_bGateAssignment;
	CGannRowWnd* m_pTimeLine;
	CGannRowWnd* m_pGannRowWnd[MAX_NUM_ROWS_ONSCREEN];
	std::vector<CAssignGate> m_vectGate;
	void CreateRowWindows();
	//{{AFX_MSG(CGannChartWnd)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnGCLClick( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnGCLDBClick( WPARAM wParam, LPARAM lParam );
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	int m_nZoomFactor;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHARTWND_H__F0CCE887_BDD8_4A03_96F4_7E59F77C7203__INCLUDED_)
