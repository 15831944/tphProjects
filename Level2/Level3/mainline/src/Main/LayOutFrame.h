#pragma once

#include "XPStyle/StatusBarXP.h"
// CLayOutFrame frame
class CLayoutOverUpToolBar ;
class CAircraftLayoutView ;
class CTermPlanDoc;
class CLayOutFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CLayOutFrame)
protected:

	CSplitterWnd m_wndSplitter1;
	CSplitterWnd m_wndSplitter2;
	CAircraftLayoutView* m_LayoutView ;
public:
		CLayoutOverUpToolBar* m_LayoutTool ;
		CStatusBarXP  m_wndStatusBar;
protected:
	CLayOutFrame();           // protected constructor used by dynamic creation
	virtual ~CLayOutFrame();

	void OnUpdateFrameTitle(BOOL bAddToTitle) ;
	BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) ;

	afx_msg LRESULT ONAIRSelChanged(WPARAM wParam,LPARAM lParam);
  LRESULT OnDlgFallback(WPARAM wParam, LPARAM lParam) ;
	void OnDestroy() ;
	CTermPlanDoc* GetDocument();
public:
		void CreateLayoutTool() ;
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnUpdate3dMouseMode(CCmdUI *pCmdUI);
	afx_msg void OnUpdate3dMousePos(CCmdUI *pCmdUI);
};


