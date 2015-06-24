
#pragma once
#include "XPStyle/StatusBarXP.h"


/////////////////////////////////////////////////////////////////////////////
// CChildFrameSplit frame
class CTermPlanDoc;
class CChildFrameSplit : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CChildFrameSplit)

protected:
	virtual ~CChildFrameSplit();
	CChildFrameSplit();

public: 
	CTermPlanDoc* GetDocument();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()

	CStatusBarXP  m_wndStatusBar;

	virtual void OnUpdateFrameMenu (BOOL bActivate, CWnd* pActivateWnd, HMENU hMenuAlt);
	virtual void OnUpdateFrameTitle(BOOL bAddToTitle);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
