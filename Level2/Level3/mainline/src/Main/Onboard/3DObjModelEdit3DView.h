#pragma once
#include "../3DDragDropView.h"

#include <Renderer/RenderEngine/3DObjectCommands.h>
#include <Renderer/RenderEngine/3DObjModelEditScene.h>
#include <Renderer/RenderEngine/CoordinateDisplayScene.h>

#include "3DObjModelEditFrame.h"

class C3DObjModelEditScene;
class C3DObjModelEditContext;
class DragDropDataItem;

class C3DObjModelEdit3DView :
	public C3DDragDropView, public C3DObjModelEditScene::Listener
{
public:
	DECLARE_DYNCREATE(C3DObjModelEdit3DView)

	C3DObjModelEdit3DView(void);
	virtual ~C3DObjModelEdit3DView(void);

	virtual void OnUpdateDrawing();
	virtual void OnFinishMeasure(double dDistance);
	virtual void OnSelect3DObj(C3DNodeObject nodeObj);

	virtual void SetupViewport();	

	void SetupCoordScene( CCoordinateDisplayScene& theCoordScene, Ogre::Camera* pActiveCam, CRect rect, VPAlignPos alg );
	C3DSceneBase* GetScene( Ogre::SceneManager* pScene );

	C3DObjModelEditFrame* GetParentFrame() const;
	C3DObjModelEditScene* GetModelEditScene() { return mpModelEditScene; }


	void UpdateView(
		C3DObjModelEditFrame::UpdateViewType updateType = C3DObjModelEditFrame::Type_ReloadData,
		DWORD dwData = 0);

	bool GetViewPortMousePos(ARCVector3& pos);

protected:
	virtual bool MoveCamera(UINT nFlags, const CPoint& ptTo, int vpidx =0 );
	virtual C3DObjModelEditScene* CreateScene() const;

	//on msg functions
protected:

	virtual void OnInitialUpdate();

	virtual void DoDrop(const DragDropDataItem& dropItem, CPoint point);

	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	afx_msg void OnMoveX();
	afx_msg void OnMoveY();
	afx_msg void OnMoveZ();
	afx_msg void OnMoveXY();
	afx_msg void OnMoveYZ();
	afx_msg void OnMoveZX();

	afx_msg void OnScaleXYZ();
	afx_msg void OnScaleX();
	afx_msg void OnScaleY();
	afx_msg void OnScaleZ();

	afx_msg void OnRotateX();
	afx_msg void OnRotateY();
	afx_msg void OnRotateZ();

	afx_msg void OnMirrorXY();
	afx_msg void OnMirrorYZ();
	afx_msg void OnMirrorZX();
	void OnMirror(C3DObjectCommand::EditCoordination coord);

	afx_msg void OnDelete();

	afx_msg void OnUndo();
	afx_msg void OnRedo();

	afx_msg void OnLayoutLock();
	afx_msg void OnDistanceMeasure();

	afx_msg void OnConfigGrid();

	afx_msg void OnUpdateUndo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRedo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateLayoutLock(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDistanceMeasure(CCmdUI* pCmdUI);

	afx_msg void OnViewSolid();
	afx_msg void OnUpdateViewSolid(CCmdUI *pCmdUI);
	afx_msg void OnViewWireframe();
	afx_msg void OnUpdateViewWireframe(CCmdUI *pCmdUI);
	afx_msg void OnViewSolidwireframe();
	afx_msg void OnUpdateViewSolidwireframe(CCmdUI *pCmdUI);
	afx_msg void OnViewTransfarent();
	afx_msg void OnUpdateViewTransfarent(CCmdUI *pCmdUI);
	afx_msg void OnViewHiddenwireframe();
	afx_msg void OnUpdateViewHiddenwireframe(CCmdUI *pCmdUI);
	afx_msg void OnViewTransparentwireframe();
	afx_msg void OnUpdateViewTransparentwireframe(CCmdUI *pCmdUI);

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnUpdateFPS(CCmdUI* pCmdUI);

	afx_msg void OnFloorAdjust();
	afx_msg void OnUpdateFloorAdjust(CCmdUI* pCmdUI);
	afx_msg void OnShowShapesToolbar();
	afx_msg void OnUpdateShowShapesToolbar(CCmdUI* pCmdUI);
	afx_msg void On3DNodeObjectProperty();


	C3DObjModelEditContext* GetEditContext() const;

	void SetCurOpStartPos();

	DECLARE_MESSAGE_MAP()

protected:
	// the edit scene
	C3DObjModelEditScene* mpModelEditScene;
	// the crossed X-Y-Z coordinations at the bottom-left of the 3D-view
	CCoordinateDisplayScene m_coordDisplayScene;
	// the crossed X-Y-Z coordinations at the bottom-left of the 3D-view, of selected node object
	CCoordinateDisplayScene m_coord3DObj;
	ViewPort m_vpCamera; // camera of mModelEditScene;


	//
	BOOL m_bConfigureGrid;
};