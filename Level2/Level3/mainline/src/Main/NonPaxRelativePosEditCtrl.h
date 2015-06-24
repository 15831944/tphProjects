#pragma once

//#include <Onboard3d/3DScene.h>
//#include <Onboard3d/3DSceneWnd.h>
//#include <Onboard3d/OnboardAnimaPax.h>
//////////////////////////////////////////////////////////////////////////
#include <Renderer\RenderEngine\NonPaxRelativePosDefScene.h>
#include <renderer\RenderEngine\RenderDevice.h>
#include <renderer\RenderEngine\3DBaseView.h>

class CNonPaxRelativePos;
class CNonPaxRelativePosSpec;
//////////////////////////////////////////////////////////////////////////

enum EMouseState
{
	_default,
	_moveShape,
	_rotateshapeX,
	_rotateshapeY,
	_rotateshapeZ,
};
class CNonPaxRelativePosEditCtrl :
	public CWnd, public C3DBaseWnd
{	
	DECLARE_DYNCREATE(CNonPaxRelativePosEditCtrl)

public:
	CNonPaxRelativePosEditCtrl(void);
	~CNonPaxRelativePosEditCtrl(void);


	void SetSceneCameType(emCameraType camType);

	void RotateSelectShape(double dAngle, const ARCVector3& axis);
	void MoveShape(CPoint ptFrom, CPoint ptTo);
	void DoneEdit();

	//C3DSceneWnd m_sceneWnd;	
	CNonPaxRelativePosDefScene m_scene;
	CNonPaxRelativePosSpec* m_pNonPaxSpec;
	CNonPaxShape3D mpSelectShape;
	CPoint m_ptFrom;  //operation point from	
public:
	void SetPosSpec(CNonPaxRelativePosSpec* pSpec);
	
	virtual void SetupViewport();
	virtual C3DSceneBase* GetScene( Ogre::SceneManager* pScene );


	void AddNewNonPaxPos(int nTypeIdx, const CPoint& pos);
	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	UINT mnSizeType;
	int mnSizex;
	int mnSizeY;
	afx_msg void OnPaint();

	bool MoveCamera( UINT nFlags, const CPoint& ptTo,int vpIdx );

	bool mbSceneWndCreated;
	EMouseState mMouseState;

	BOOL RegisterWndClass();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	
	afx_msg void OnNonpaxpickDelete();
	afx_msg void OnRotateX();
	afx_msg void OnRotateY();
	afx_msg void OnRotateZ();
};
