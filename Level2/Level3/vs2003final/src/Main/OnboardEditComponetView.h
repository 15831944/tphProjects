#pragma once
#include "AircraftComponent3DSceneWnd.h"

class ~COnBoardEditComponentView :
	public CView
{
	
	DECLARE_DYNCREATE(~COnBoardEditComponentView)

public:
	~COnBoardEditComponentView(void);
	~~COnBoardEditComponentView(void);

	void SetupRenderer();
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view

	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);	
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);

	CAircraftComponent3DSceneWnd m_SceneWindow;
	bool m_bRenderSetuped;
protected:
	CPoint m_mouseLast;
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
