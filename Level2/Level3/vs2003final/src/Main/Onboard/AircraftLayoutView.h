#pragma once
//#include "./3DSceneView.h"

//#include <Onboard3d/AircraftLayouScene.h>
#include <afxole.h>

class CAircraftLayoutEditContext;
class CAircraftFurnishing;
class CTermPlanDoc;

class CAircraftLayoutView  : public C3DSceneView
{
protected:
	CAircraftLayoutView(void);
	DECLARE_DYNCREATE(CAircraftLayoutView)

public:
	void SetupViewport();
	virtual void SetupScene();

	enum EMouseState { _default, _floormanip,
		_rotateproc, _placeproc, 
		_scaleproc, _scaleprocX, _scaleprocY, _scaleprocZ, 
		_moveprocs,
		_moveshapes,_moveshapesZ, _rotateshapes,
		_getonepoint, _gettwopoints, _getmanypoints,
		_getonerect,
		_tumble, _dolly, _pan,
		_distancemeasure,
		_placemarker,
		_placemarker2,
		_getmirrorline,
		_getarrayline,
		_pickgate,
		_pickproc,
		_pasteproc,
		_selectlandproc,
		_rotateairsideobject,
		_pickstretchpos,
		//
		_moveSeatRow,
		_rotateSeatRow,
		_moveElements,	
		_moveElementsZ,
		_rotateElements,
		_scaleElements,
		_moveSeatArray,
		_moveSeatArrayPoint,
	};

	enum EmSelectItemType
	{
		NONE,
		SEAT,
		SEAT_ARRAY,
		SEAT_ROW,
		AIRCRAFT_ELMENT,
		SEAT_ARRAY_PATH_POINT,
	};
	CCmdUI *g_ptCmdUI;
	double m_dFrameRate;
	EMouseState m_eMouseState;
	// for pick
	LPARAM m_lFallbackParam;
	HWND m_hFallbackWnd;

	BOOL m_bAnimDirectionChanged;
	int m_nAnimDirection;

	~CAircraftLayoutView(void);
	CAircraftLayoutEditContext* GetEditContext()const;

	UINT m_nPanZoomTimeout; //timer ID
	UINT m_nOnSizeTimeout;  //timer ID
	UINT m_nMouseMoveTimeout; //timerID
	UINT m_nRedSquareBlinkTimer; //timerID

	COleDropTarget m_oleDropTarget;	//to support drag & drop of shapes

	AircraftLayoutScene m_aircraftlayoutScene;

	CAircraftFurnishing* m_pDragShape;
	BOOL m_bTrackLButton;
	std::vector<ARCVector3> m_vMousePosList;

public: //3d view 
	virtual DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual void OnDragLeave();
	virtual BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);

	virtual void PreOnDraw();
	ARCVector3 GetMousePos() const { return m_ptMouseWorldPos; }
protected:
	void CalculateFrameRate();
	DECLARE_MESSAGE_MAP()
	EmSelectItemType SelectScene( const CPoint& point);
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
protected:
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
protected:
	//{{AFX_MSG(C3DView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnUpdateAnimPax(CCmdUI* pCmdUI);
	afx_msg void OnUpdateGridSpacing(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFPS(CCmdUI *pCmdUI);
	afx_msg void OnUpdateAnimTime(CCmdUI *pCmdUI);
	afx_msg void OnEditUndo();
	afx_msg void OnEditRedo();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnAnimPax();
	//}}AFX_MSG
	afx_msg void OnTimer(UINT nIDEvent);
	//


	//seat menu funcs
	afx_msg void OnAircraftElementProperties();
	afx_msg void OnAircraftElementDisplayProp();
	afx_msg void OnCreateSeatRow();
	afx_msg void OnCopyElements();
	afx_msg void OnDeleteElements();
	afx_msg void OnMoveElements();
	afx_msg void OnMoveElmentsZ();
	afx_msg void OnRotateElements();
	afx_msg void OnScaleElements();

	//seat row funs
	afx_msg void OnUnRow();
	afx_msg void OnDeleteRow();
	afx_msg void OnCreateSeatArray();
	afx_msg void OnMoveSeatRow();
	afx_msg void OnRotateSeatRow();
	
	//seat group 
	afx_msg void OnSeatArrayProp();
	afx_msg void OnUnArraySeats();
	afx_msg void OnDeleteArray();
	afx_msg void OnEditArrayEditPoint();
	afx_msg void OnSeatrowCopy();
	afx_msg void OnUpdateSeatarrayEditarraypath(CCmdUI *pCmdUI);
	afx_msg void OnArraypointDelete();

	afx_msg void OnScenepickRestoredefaultcamera();

	afx_msg void OnMenuruler();
	afx_msg void OnScenepickStartanim();
	afx_msg void OnScenepickStopanim();	
	afx_msg void OnScenepickShowtext();
	afx_msg void OnUpdateScenepickShowtext(CCmdUI *pCmdUI);

	void ShowDistanceDialog();
	void ShowDisplayPropertyDialog(CAircraftElement* pElement);

	ARCVector3 GetWorldMousePos(const CPoint& pt);
	ARCVector3 m_ptMouseWorldPos;
	bool m_bEditArrayPath;
	int mSelectArrayPtIdx;


public:
	void UpdateAnimationPax();
	
	
public:

};
