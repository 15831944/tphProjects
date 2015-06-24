#pragma once


// COnBoardLayouPerspectiveView view
#include "OnBoardLayoutView.h"

struct OnBoradWindow;

class COnBoardLayouPerspectiveView : public COnBoardLayoutView
{
	DECLARE_DYNCREATE(COnBoardLayouPerspectiveView)

	OnBoradWindow* m_scene;
protected:
	COnBoardLayouPerspectiveView();           // protected constructor used by dynamic creation
	virtual ~COnBoardLayouPerspectiveView();

public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

public:
	virtual void OnInitialUpdate();
	afx_msg void OnPopupmenuonboradEditmode();
	afx_msg void OnPopupmenuonboradFramemode();
	afx_msg void OnPopupmenuonboradShadermode();
	afx_msg void OnPopupmenuonboradCreatesection();
	afx_msg void OnPopupmenuonboradDeleteselectsection();
	afx_msg void OnPopupmenuonboradDeleteallsection();
	afx_msg void OnPopupmenuonboradSetselectedsectionposition();
	afx_msg void OnPopupmenuonboradSettexturename();
	afx_msg void OnPopupmenuonboradSettexturedirx();
	afx_msg void OnPopupmenuonboradSettexturediry();
	afx_msg void OnPopupmenuonboradSettexturedirz();
	afx_msg void OnPopupmenuonboradSetsectionpointnum();
	afx_msg void OnPopupmenuonboradMoveprepxaxis();
	afx_msg void OnPopupmenuonboradMoveprepyaxis();
	afx_msg void OnPopupmenuonboradRotatexaxis();
	afx_msg void OnPopupmenuonboradRotateyaxis();
	afx_msg void OnPopupmenuonboradRotatezaxis();
	afx_msg void OnPopupmenuonboradScalexaxis();
	afx_msg void OnPopupmenuonboradScaleyaxis();
	afx_msg void OnPopupmenuonboradScalezaxis();
	afx_msg void OnPopupmenuonboradScaleallxaxis();
	afx_msg void OnPopupmenuonboradLoadcomponent();
	afx_msg void OnPopupmenuonboradLockunlockeditpoint();
	afx_msg void OnPopupmenuonboradLockunlockcomponet();
	afx_msg void OnUpdatePopupmenuonboradEditmode(CCmdUI *pCmdUI);
	afx_msg void OnUpdatePopupmenuonboradFramemode(CCmdUI *pCmdUI);
	afx_msg void OnUpdatePopupmenuonboradShadermode(CCmdUI *pCmdUI);
	afx_msg void OnUpdatePopupmenuonboradMoveprepxaxis(CCmdUI *pCmdUI);
	afx_msg void OnUpdatePopupmenuonboradMoveprepyaxis(CCmdUI *pCmdUI);
	afx_msg void OnUpdatePopupmenuonboradRotatexaxis(CCmdUI *pCmdUI);
	afx_msg void OnUpdatePopupmenuonboradRotateyaxis(CCmdUI *pCmdUI);
	afx_msg void OnUpdatePopupmenuonboradRotatezaxis(CCmdUI *pCmdUI);
	afx_msg void OnUpdatePopupmenuonboradScalexaxis(CCmdUI *pCmdUI);
	afx_msg void OnUpdatePopupmenuonboradScaleyaxis(CCmdUI *pCmdUI);
	afx_msg void OnUpdatePopupmenuonboradScalezaxis(CCmdUI *pCmdUI);
	afx_msg void OnUpdatePopupmenuonboradScaleallxaxis(CCmdUI *pCmdUI);
	afx_msg void OnEditModelViewGrid();
};


