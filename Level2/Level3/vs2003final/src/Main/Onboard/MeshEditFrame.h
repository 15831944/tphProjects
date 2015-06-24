#pragma once


// COnBoardLayoutFrame frame with splitter
#include "../XPStyle/StatusBarXP.h"
#include <Renderer\RenderEngine\SimpleMeshEditScene.h>

class COnBoardMeshEditFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(COnBoardMeshEditFrame)
protected:
	COnBoardMeshEditFrame();           // protected constructor used by dynamic creation
	virtual ~COnBoardMeshEditFrame();

	CSplitterWnd m_wndSplitterLR;
	CSplitterWnd m_wndSplitterRTB;
	CSplitterWnd m_wndSplitterRBLR;


	//update view in this frame
public:
	CStatusBarXP  m_wndStatusBar;
	CSimpleMeshEditScene mMeshEditScene;
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
	afx_msg void OnUpdate3dMousePos(CCmdUI *pCmdUI);  
	afx_msg void OnOnboardcomponenteditAddstation();
};

