#if !defined(AFX_ROWWND_H__19649520_E0FF_4141_AD96_75C1C45D331B__INCLUDED_)
#define AFX_ROWWND_H__19649520_E0FF_4141_AD96_75C1C45D331B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RowWnd.h : header file
//

#include "inputs\GateAssignmentMgr.h"
/////////////////////////////////////////////////////////////////////////////
// CGannRowWnd window

class CGannRowWnd : public CWnd
{
	DECLARE_DYNAMIC(CGannRowWnd)
// Construction
public:
	CGannRowWnd(InputTerminal* _pInTerm);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGannRowWnd)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetProcAssnSchd(ProcessorRosterSchedule *_pProcSchd);
	void SetSelected( bool _bSelected );
	void SetAssignType(bool _bGateAssignment);
	void SetAssignedGate( const CAssignGate& _gate );
	void SetTitle( CString _csTitle );
	void SetTimeLineFlag();
	virtual ~CGannRowWnd();

	// Generated message map functions
protected:
	InputTerminal* m_pInTerm;
	bool m_bSelected;
	std::vector<CString> m_vectLabel;
	bool m_bGateAssignment;
	CToolTipCtrl	m_ctrlTooltip;
	void PaintHeader( CDC * pDC );
	//{{AFX_MSG(CGannRowWnd)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	afx_msg BOOL OnToolTipText(UINT id, NMHDR *pNMHDR, LRESULT *pResult);
	DECLARE_MESSAGE_MAP()
private:
	int m_nCurToolTipsIndex;
	CString m_csToolTips;
	int GetRectIndex( const CPoint& _point );
	bool m_bTimeLine;
	COleDateTime m_timeRight;
	COleDateTime m_timeLeft;
	CString m_csTitle;
	CAssignGate m_assignedGate;
	ProcessorRosterSchedule* m_pProcSchd;
	std::vector<CRect> m_vectRect;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ROWWND_H__19649520_E0FF_4141_AD96_75C1C45D331B__INCLUDED_)
