#pragma once
#include "afxwin.h"
//#include <Onboard3d/ogreMFCheader.h>
//#include <Onboard3d/3DSceneWnd.h>
//
class InputOnboard;


class C3DSceneCtrl
{
public:

};

class C3DSceneView : public CView
{
protected:
	C3DSceneView();
	DECLARE_DYNCREATE(C3DSceneView)

	
	// Attributes
public:
	InputOnboard* GetInputOnboard()const;
	// Operations
public:
	virtual void SetupViewport();
	virtual void SetupScene();
	// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual void PreOnDraw();
	bool IsViewSetuped()const;

private:
	bool m_bViewportSetuped;

	// Implementation
public:
	virtual ~C3DSceneView();

protected:		
	CPoint m_mousePtDown;
	C3DSceneWnd m_sceneWnd;
	CPoint m_ptFrom;

	Ogre::Ray m_lastRBtnDownRay;
	Ogre::Ray m_lastLBtnDownRay;
	Ogre::Ray m_lastMBtnDownRay;

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);

	afx_msg void OnSize(UINT nType, int cx, int cy);
};
