#pragma once


#include "XPstyle/StatusBarXP.h"
#include "SplitterWndEx.h"
#include <Renderer/RenderEngine/Render3DScene.h>
#include <InputAirside/ALTAirport.h>
#include "IRenderFrame.h"

// CRender3DFrame frame with splitter
class CRender3DView;
class CCameraData;
class CTermPlanDoc;
class CARCportLayoutEditor;
struct C3DViewPaneInfo;

class CReCalcSplitterWnd : public CSplitterWnd2X2
{
public:
	CReCalcSplitterWnd();

	virtual void RecalcLayout();    // call after changing sizes

	void LoadPaneInfo(const C3DViewPaneInfo& paneInfo);

private:
	BOOL m_bLoadingPaneInfo;
};

class CRender3DFrame : public IRenderFrame
{
	DECLARE_DYNCREATE(CRender3DFrame)

public:
	CARCportLayoutEditor* GetEditContext();
	void OnUpdateFrameTitle(BOOL bAddToTitle);

	CRender3DView* GetActivePane(int* _pRow=NULL, int* _pCol=NULL);
	CTermPlanDoc* GetDocument();
	void LoadWorkingViews();
	BOOL UnSplitRow();
	BOOL UnSplitColumn();
	void OnSplitterLayoutChange();

	BOOL IsChildPane(CWnd* _pWnd, int& _row, int& _col) { return m_wndSplitter.IsChildPane(_pWnd,&_row,&_col); }

	ALTAirport& GetAltAirport() { return m_altAirport; }
	CStatusBarXP& GetStatusBar() { return m_wndStatusBar; }
	CRender3DScene mModelEditScene;

protected:
	CRender3DFrame();           // protected constructor used by dynamic creation
	virtual ~CRender3DFrame();

	CReCalcSplitterWnd m_wndSplitter;

protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual void OnUpdateFrameMenu (BOOL bActivate, CWnd* pActivateWnd, HMENU hMenuAlt);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);

	afx_msg void OnSplitHorizontal();
	afx_msg void OnSplitVertical();
	afx_msg void OnUnSplit();
	afx_msg void OnDestroy();
	afx_msg void OnUpdate3dMouseMode(CCmdUI *pCmdUI);
	afx_msg void OnUpdate3dMousePos(CCmdUI *pCmdUI);
	afx_msg void OnViewSaveNamedView();

	BOOL SplitRow(int nTop = 150, int nBottom = 150);
	BOOL SplitColumn(int nLeft = 200, int nRight = 200);


	DECLARE_MESSAGE_MAP()

public:
	afx_msg BOOL OnViewNamedViews3D(UINT nID);
	afx_msg BOOL OnViewNamedViews2D(UINT nID);
	afx_msg void OnView3DDefaultView();
	afx_msg void OnView2DDefaultView();
	afx_msg void OnView2dterminalview();
	afx_msg void OnView3dterminalview();

private:

	ALTAirport m_altAirport; // airport data, would be moved to data layer if possible
};


