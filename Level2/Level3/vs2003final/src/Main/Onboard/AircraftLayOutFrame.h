#pragma once

#include "../XPStyle/StatusBarXP.h"
// CLayOutFrame frame
//class CLayoutOverUpToolBar ;
class CAircraftLayout3DView ;
class CTermPlanDoc;
class CAircraftLayOutFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CAircraftLayOutFrame)
protected:

	CSplitterWnd m_wndSplitter1;
	CSplitterWnd m_wndSplitter2;
public:
	//	CLayoutOverUpToolBar* m_LayoutTool ;
		CStatusBarXP  m_wndStatusBar;
		CAircraftLayout3DView* m_LayoutView ;
public:
	CAircraftLayOutFrame();           // protected constructor used by dynamic creation
	virtual ~CAircraftLayOutFrame();

	void OnUpdateFrameTitle(BOOL bAddToTitle) ;
	BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) ;

	afx_msg LRESULT ONAIRSelChanged(WPARAM wParam,LPARAM lParam);
	LRESULT OnDlgFallback(WPARAM wParam, LPARAM lParam) ;
	void OnDestroy() ;
	//CTermPlanDoc* GetDocument();
public:
	void CreateLayoutTool() ;
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnUpdate3dMouseMode(CCmdUI *pCmdUI);
	afx_msg void OnUpdate3dMousePos(CCmdUI *pCmdUI);
	afx_msg void OnChangetoSeat();
	afx_msg void OnChangetoStrage();
	afx_msg void OnChangetoGalley();
	afx_msg void OnChangetoDoor();
	afx_msg void OnChangetoEmergencyexit();
	afx_msg void OnChangetoStair();
	afx_msg void OnChangetoEscalator();
	afx_msg void OnChangetoElevator();
	afx_msg void OnViewSaveNamedView();
	afx_msg void OnManageBMViewpoints();
	afx_msg void OnUpdateViewNamedViews2D(CCmdUI *pCmdUI);

	afx_msg void OnUpdateViewNamedViews3D(CCmdUI *pCmdUI);
	afx_msg BOOL OnViewNamedViews3D(UINT nID);
	afx_msg BOOL OnViewNamedViews2D(UINT nID);
	afx_msg void OnView3DDefaultView();
	afx_msg void OnView2DDefaultView();
	afx_msg void OnView2dterminalview();
	afx_msg void OnView3dterminalview();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
};


