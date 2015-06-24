#pragma once

#include "../3DDragDropView.h"
#include <Renderer/RenderEngine/AircraftLayoutScene.h>
#include <Renderer/RenderEngine/CoordinateDisplayScene.h>

class CAircraftLayoutEditor;
class CAircraftFurnishing;
class CAircraftLayOutFrame;
class CDlgOnboardDeckAdjust;


class CAircraftLayout3DView : public C3DDragDropView, public CAircraftLayoutScene::Listener
{
protected:
	CAircraftLayout3DView(void);
	DECLARE_DYNCREATE(CAircraftLayout3DView)

public:
	CAircraftLayoutEditor* GetEditor()const ;

	virtual void SetupViewport();

	virtual C3DSceneBase* GetScene(Ogre::SceneManager* pScene);

	bool MoveCamera( UINT nFlags, const CPoint& ptTo , int vpIdx);

	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
	
	CAircraftLayOutFrame* GetParentFrame() const;

	bool GetViewPortMousePos(ARCVector3& pos);

	//Scene Lister 
	void OnFinishDistanceMesure(double dist);
	void OnFinishPickPoints(const ARCPath3&);
	void OnRButtonDownOnElement(CAircraftElementShapePtr,const CPoint & );
	void OnRButtonDownOnEditPath(CAircraftElementShapePtr, int nPathIndex, int nNodeIndex, const CPoint&);
	void OnSceneChange(){ Invalidate(FALSE); }

	virtual bool IsAnimation()const;

	void OnMessageMove();

	//update floor altitude
	void ShowDeckAdjustDialog();//call deck adjust dialog
	void UpdateFloorAltitude(CDeck *pDeck);//update deck in layout view if altitude of deck changed

	void AddNewNamedView(CString strViewName);

	void ReadCamerasSettings(); // read from database
	void SaveCamerasSettings(); // save into database
	void LoadCamerasSettings(); // load into 3d view

	C3DViewCameras& Get3DViewCameras();
	const C3DViewCameras& Get3DViewCameras() const;

protected:
	virtual bool IsDropAllowed(const DragDropDataItem& dropItem) const;
	virtual void DoDrop(const DragDropDataItem& dropItem, CPoint point);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnUpdateGridSpacing(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFPS(CCmdUI *pCmdUI);
	afx_msg LRESULT OnDlgFallback(WPARAM wParam, LPARAM lParam);



	//toolbar functions
	afx_msg void OnUndo();
	afx_msg void OnRedo();
	afx_msg void OnLayoutLock();
	afx_msg void OnDistanceMeasure();	
	afx_msg void OnShowText();
	afx_msg void OnShowShapes();
	afx_msg void OnFloorAdjust();

	afx_msg void OnUpdateUndo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRedo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateLayoutLock(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDistanceMeasure(CCmdUI* pCmdUI);
	afx_msg void OnUpdateShowText(CCmdUI* pCmdUI);
	afx_msg void OnUpdateShowShapes(CCmdUI* pCmdUI);


	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);


	DECLARE_MESSAGE_MAP()

protected:
	HWND m_hFallbackWnd; //pick point dlg 

	ViewPort m_vpCamera;
	C3DViewCameras m_cameras;
	CAircraftLayoutScene mLayoutScene;
	//CAircraftLayoutOverlayDisplay mOverlayDisplay;

	CCoordinateDisplayScene m_coordDisplayScene;

	CDlgOnboardDeckAdjust* m_pDlgDeckAdjust;

	enum EViewStatus
	{
		viewStatus_None = 0,

		viewStatus_ArrayElements,
	};

	EViewStatus m_eViewStatus;

	CAircraftElmentShapeDisplay* m_pathRelatedElement;
	int                          m_nEditedPathIndex;
	int                          m_nEditedPathNodeIndex;
	CPoint                       m_ptPathEditPos;

public:
	//ctx menu
	afx_msg void OnCtxElementProperties();
	afx_msg void OnCtxElementDisplayProp();
	afx_msg void OnCtxElementCopy();
	afx_msg void OnCtxElementDelete();
//	afx_msg void OnCtxSeatSelectRow();
	//afx_msg void OnSeatSelectGroup();
	afx_msg void OnSeatarrayEditarraypath();
	afx_msg void OnCtxSelectrows();
	afx_msg void OnCtxSelectarrays();
	afx_msg void OnCtxSelectseats();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnUpdateCtxSelectseats(CCmdUI *pCmdUI);
	afx_msg void OnUpdateCtxSelectrows(CCmdUI *pCmdUI);
	afx_msg void OnUpdateCtxSelectarrays(CCmdUI *pCmdUI);
	afx_msg void OnSeatEditseatrow();
	afx_msg void OnSeatEditseatarray();
	afx_msg void OnSeatCopy();
	afx_msg void OnSeatDelete();
	afx_msg void OnSeatEnableeditseatarraypath();
	afx_msg void OnUpdateSeatEnableeditseatarraypath(CCmdUI *pCmdUI);
	afx_msg void OnAircraftelementLock();
	afx_msg void OnArrayElements();
	afx_msg void OnUpdateAircraftelementLock(CCmdUI *pCmdUI);
	afx_msg void OnCorridorEnableeditseatpath();
	afx_msg void OnUpdateCorridorEnableeditseatpath(CCmdUI *pCmdUI);
	afx_msg void OnPathAddEditPoint();
	afx_msg void OnPathRemoveEditPoint();
	
	afx_msg void OnCtxElementMove();
	afx_msg void OnCtxElementRotate();
	
	afx_msg void OnCtxScaleShape();
	afx_msg void OnCtxScaleShapeX();
	afx_msg void OnCtxScaleShapeY();
	afx_msg void OnCtxScaleShapeZ();
	afx_msg void OnCtxMoveShapes();
	afx_msg void OnCtxRotateShapes();
	afx_msg void OnCtxMoveshapeZ();
	afx_msg void OnCtxMoveshapeZ0();

	afx_msg void OnTransformshapeTransformproperties();
	afx_msg void OnDestroy();
};
