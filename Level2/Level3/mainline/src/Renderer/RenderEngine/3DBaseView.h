#pragma once
#include "AfxHeader.h"
#include "RenderDevice.h"
#include "IRender3DView.h"
class ARCVector3;

class RENDERENGINE_API C3DBaseWnd 
{
public:
	RenderWnd m_rdc;
	CPoint m_lastMousePt;
	virtual C3DSceneBase* GetScene(Ogre::SceneManager* pScene)=0;
	bool GetMouseWorldPos(const CPoint& pt, ARCVector3& wdPos ,int vpIndx = 0);
	virtual void SetupViewport()=0;
	
}; 



class RENDERENGINE_API C3DBaseView :  public IRender3DView,public C3DBaseWnd
{
protected:
	DECLARE_DYNCREATE(C3DBaseView)
	// Attributes
public:
	// Operations
	C3DBaseView();
public:
	// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view

	// Implementation
public:
	//virtual CString GetCurCamera()const{ return mViewCameras[mCurCamera]; }		
	//this two need to override 	
	CString mViewCameraName; //unique

public:
	bool MakeSureSetuped();

protected:		
	double m_dFrameRate;
	UINT_PTR m_nAnimationTimer; //the timer if the edit has some effect ,select red square
	UINT m_nAnimationInterval;

	//BOOL m_bBusy;	//this flag is true while view is rendering


	void CalculateFrameRate();
	virtual void SetupViewport(){}
	virtual void UnSetupViewport(){}
	virtual C3DSceneBase* GetScene( Ogre::SceneManager* pScene ){ return NULL; }


	virtual bool MoveCamera(UINT nFlags, const CPoint& ptTo, int vpIdx = 0);
	virtual bool IsAnimation()const{ return false; }
	void SetAnimInterval(UINT newTime);
	virtual void PrepareDraw(){}
	// Generated message map functions

	virtual void ClearContextMenuInfo(){}
protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC); 
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);

};