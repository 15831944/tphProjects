#if !defined(AFX_3DVIEW_H__23505B99_36F9_11D4_92FF_0080C8F982B1__INCLUDED_)
#define AFX_3DVIEW_H__23505B99_36F9_11D4_92FF_0080C8F982B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "Renderer\RenderEngine\IRender3DView.h"
#include "Engine\LandfieldProcessor.h"
#include "InputAirside\IntersectionsDefine.h"
#include "SelectionMap.h"
#include "OglTextureResource.h"
#include "3DModelResource.h"
#include "CopyProcToMenuInfo.h"
#include "CommonData\StructureBase.h"
// 3DView.h : header file
//


#define SHOW_FRAMERATE		TRUE

#define CHANGE_SHAPES_MENU_LIST_ID_BEGIN							256	
#define CHANGE_SHAPES_MENU_LIST_ID_END								1024	
#define CHANGE_SHAPES_MENU_LIST_ITEMCOUNT_PER_COLUMN				4

#define SELECT_ACTAGS	0x01

class C3DControlsWnd;
class C3DCamera;
class CTracerTagWnd;
class CChildFrame;
class CNewMenu;
class CPaxDispProps;
class CProcessorTagWnd;
class CShape;
class CTextureResource;
class CTermPlanDoc;
class PBuffer;
typedef PBuffer* PBufferPointer;
class CglShapeResource;
class CAirside3D;
class SelectableSettings;
class CPROCESSOR2LIST;
class CFloor2;
//chloe
struct OnBoradScene;
typedef PBuffer* PBufferPointer;
class WallShape;
class CStructure;
class CPickConveyorTree;

enum EnvironmentMode;
//
/////////////////////////////////////////////////////////////////////////////
// C3DView view
class C3DView : public IRender3DView
{
	//friend class C3DControlsWnd;
protected:
	C3DView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(C3DView)

// Attributes
public:	
	void ClearPointPickStruct();
	
	LandProcPartArray m_vSelectLandProcs;
	InsecObjectPartVector m_vSelectedStretchPos;

	struct SelectConfig{
		
	};

	int m_nViewID;
	static int m_nViewIDCount;
	void InitSeledPaxList();
	void ClearSeledPaxList();
	std::vector<UINT> m_nPaxSeledIDList;
	std::vector<CString> m_nPaxSeledTagList;
	long m_nTickTotal;
	UINT m_nPaxSeledID;
	BOOL m_bAnimPax;



	// the ID for loading name 
    int m_nFlightSelect;
	int m_nVehicleSelect;

	int m_nSelectTaxiSegment;
protected:
	CString m_strTime;
	HGLRC m_hRC;	//opengl RC
	CDC* m_pDC;		//windows DC
	double m_dAspect;	//aspect ratio of view
	int m_iPixelFormat;	//pixel format used
	GLuint m_nSharpTexId; //texture object ID for sharp texture of this view
	BOOL m_bUseSharpTex;  //use sharp texture flag
	ARCVector2 m_ptSharpBL; //bottom,left world coords of sharp texture for 2d
	ARCVector2 m_ptSharpTR; //top, right world coords of sharp texture for 2d
	ARCVector2 m_ptSharpBR; //bottom,right world coords of sharp texture for 2d
	ARCVector2 m_ptSharpTL; //top, left world coords of sharp texture for 2d
	CSize m_SharpTexSize;	//size in texels of sharp texture
	BOOL m_bShowRadar;	//show radar flag
	UINT m_nPanZoomTimeout; //timer ID
	UINT m_nOnSizeTimeout;  //timer ID
	UINT m_nMouseMoveTimeout; //timerID
	UINT m_nRedSquareBlinkTimer; //timerID
	CShape* m_pDragShape;	//ptr to shape in drag & drop
	COleDropTarget m_oleDropTarget;	//to support drag & drop of shapes
	CPoint	m_ptFrom;				// screen point where mouse button was last clicked
	C3DCamera* m_pCamera;			//the view's camera (pointer to camera in Document)

	int m_iMaxTexSize;	//maximum texture size as specified in registry
	BOOL m_bAutoSharp;	//auto-sharp flag

	CProcessorTagWnd* m_pProcessorTagWnd;

	CTracerTagWnd* m_pTracerTagWnd;

	
	ARCVector3 m_ptMousePos;
	ARCVector3 m_ptMousePosWithZ;
	ARCPoint3 m_ptAltObjectCmdPosStart;
	double m_dRotate;
	ARCPoint3 m_ptMove;
	std::vector<ARCVector3> m_vMousePosList;
	
	
	CPoint m_ptRMouseDown;	

	HCURSOR m_hcurDefault;				// Standard cursor

	std::vector<GLuint> m_vMultiSelectIDs;
	
	BOOL m_bTrackLButton, m_bTrackMButton, m_bTrackRButton;

	BOOL m_bDragSelect;

	BOOL m_bMovingProc;
	BOOL m_bProcHasMoved;
	GLuint m_nLastSelIdx;

	BOOL m_bSelRailEdit;
	BOOL m_bMovingRailPoint;
	int m_nSelectedRailWay;
	int m_nSelectedRailPt;

	BOOL m_bSelPipeEdit;
	BOOL m_bMovingPipePoint;
	BOOL m_bMovingPipeWidthPoint;
	int m_nSelectedPipe;
	int m_nSelectedPipePt;
	int m_nSelectedPipeWidthPt;

	double m_dFrameRate;

	CProcessor2 * m_procCurEditCtrlPoint;
	SelectableList m_listProcCtrlPoint;

	//chloe
	OnBoradScene* m_OnBoradScene;
	//
public:

	BOOL m_bSelectStructureEdit;
	BOOL m_bMovingStructurePoint;
	BOOL m_bMovingStructure;
	BOOL m_bEnableMovingStructure;
	
	int m_nSelectedStructure;
	int m_nSelectedStructurePoint;

	int m_nSelectedTaxiway;
	int m_nSelectedTaxiwayPoint;
	int m_nSelectedTaxiwayWidthPoint;
	BOOL m_bMovingSelectTaxiwayMark;
	BOOL m_bMovingSelectTaxiwayMainPt;
	BOOL m_bMovingSelectTaxiwayWidthPt;

	int m_nSelectWallShape;
	int m_nSelectWallShapePoint;
	BOOL m_bMovingSelectWallShape;
	BOOL m_bMovingSelectWallShapePoint;

	int m_nSelectHoldshortLine;
	BOOL m_bMovingSelectHoldshortLine;

	static const UINT SEL_FLIGHT_OFFSET;
	static const UINT SEL_VEHICLE_OFFSET;	

	int m_nSelectedAircraft;

public:
	int m_nSelectedAirRout;

	OglTextureResource m_TextureResInstance;
	C3DModelResource m_ModelsResInstance;

public:
	//
	void setSelectWallShape(WallShape * pShape);
	//
	void setSelectStructure(CStructure * pStruct);	
	BOOL m_bCoutourEditMode;
	BOOL m_bHillDirtflag;
	BOOL m_bMovingCamera;
	//called by doc when play Work through
	void SetSelectPaxID(int nID){	m_nPaxSeledID = nID; }
	

	OglTextureResource & getTextureRes(){ return m_TextureResInstance; }
	CTextureResource * getTextureResource();
	C3DModelResource & getModelRes(){ return m_ModelsResInstance; }
//member functions
public:
	CTermPlanDoc* GetDocument(); //get document associated with this view
	C3DCamera* GetCamera(); // get the current camera associated with this view
	const C3DCamera* GetCamera() const;
	CCameraData* GetCameraData() const;

	CChildFrame* GetParentFrame() { return (CChildFrame*)(CView::GetParentFrame()); }

	void GenerateSharpTexture(); //for 2d&3d.  generates a texture that is the size of the window and sharp 
	void DestroySharpTexture();
	BOOL UseSharpTexture() { return m_bUseSharpTex; }
	void UseSharpTexture(BOOL _b) { m_bUseSharpTex = _b; }
	BOOL IsRadarOn() { return m_bShowRadar; }
	BOOL IsAutoSharp() { return m_bAutoSharp; }
	void SetAutoSharp(BOOL b = TRUE){ m_bAutoSharp=b; }
	CDC* GetClientDC() { return m_pDC; }
	HGLRC GetHGLRC() { return m_hRC; }

	BOOL InitOGL();
	
	PBuffer * getViewPBuffer(){return m_pViewPBuffer;}
	PBufferPointer & getFloorPBufferPt(){return m_pFloorPBuffer;}
protected:
	void Draw();
	void DrawScene();
	void DrawStencil();
	void DrawText();
	void DrawPax(CTermPlanDoc* pDoc,BOOL* bOn,double* dAlt,BOOL bSelectMode=FALSE);
	void DrawFlight(const DistanceUnit& airportAlt,BOOL bSelectMode = FALSE, BOOL bShowTag = FALSE, const double* mvmatrix = NULL, const double* projmatrix=NULL, const int* viewport=NULL);
	void DrawVehicle(const DistanceUnit& airportAlt,BOOL bSelectMode = FALSE, BOOL bShowTag = FALSE,const double* mvmatrix = NULL, const double* projmatrix=NULL, const int* viewport=NULL);
	void DrawCars(BOOL bSelectMode=FALSE);
	void DrawStairs();
	//void DrawAirsideTags();
	void SetTitle();
	void CalculateFrameRate();
	BOOL SetupPixelFormat(HDC hDC, DWORD dwFlags);
	BOOL SetupViewport(int cx, int cy);
	BOOL ApplyPerspective();
	BOOL ApplyViewXForm();
	BOOL PreRenderScene();
	BOOL PostRenderScene();
	BOOL RenderScene(BOOL bRenderFloors = TRUE);
public:	
	int SelectScene(UINT nFlags, int x, int y, GLuint* pSelProc,CSize sizeSelection=CSize(2,2),BOOL bDragSelect=FALSE,int procType=-1, BOOL bIncludUCProcs=TRUE);
protected:	
	int SelectACTags(UINT nFlags, int x, int y, CSize sizeSelection=CSize(2,2));
	BOOL RenderCompass(); // assumes that a RC is already current
	void RenderText();
	void RenderProc2Text(CPROCESSOR2LIST& proc2List, double* pdAlt, BOOL* pbOn);
	void RenderPipeText(double* pdAlt, BOOL* pbOn);
	void OnMouseHover(CPoint point);

	void OnProcEditFinished();
	void OnAltObjectEditFinished();
	static void GetMobileElmentOnDifferentFloorZ(const MobEventStruct& pesA,double* dAlt,double& z);
//for ouline font 
	GLYPHMETRICSFLOAT gmf[256];	// Storage For Information About Our Outline Font Characters
	GLuint	fontbase;				// Base Display List For The Font Set
	
	//Contour stuff
	
	GLuint m_hillDList;

	
	PBufferPointer m_pViewPBuffer;
	PBufferPointer m_pFloorPBuffer;
	// Operations
public:
	void Pickconveyor(int nInsertType);
	CPickConveyorTree* m_pPickConveyorTree;
	BOOL m_bPickConveyor;
	ARCVector3 m_ptPickForCopy;
	int m_nPasteToFloorIndex;
	void LoadFromPaste(COleDataObject* pDataObject,ARCVector3& arcVector);
	COleDataSource* SaveForCopy();
	void BeginDragSelect(UINT nFlags,CTermPlanDoc* pDoc,BOOL& bDragSelect,CPoint& point,CSize& sizeSelection);
	void SelectProc(int& nSelectCount,UINT nFlags,CPoint point,GLuint* idx,CTermPlanDoc* pDoc,int procType=-1,CSize sizeSelection=CSize(2,2),BOOL bDragSelect=FALSE);

	ARCVector3 Get3DMousePos(const CPoint& p,BOOL bWithZ=FALSE,int WithFloorIndex=-1);
	ARCVector3 Get3DMousePos(const CPoint& p,double logicAlt);
	void InsertMenuToChangeShapes(CNewMenu* pCtxMenu);

	const ARCVector3& GetMousePos() const { return m_ptMousePos; }

	void ShowMovieDialog();

	void ShowDistanceDialog();

	void OnPrint(CDC* pDC, CPrintInfo* pInfo);

	void RenderOffScreen( PBufferPointer & Hpbuff, int nWidth, int nHeight, BYTE* pBits );

	//void initOutlineFont();
	//void RenderOutlineFont(std::string str);

	void RenderContours(CTermPlanDoc * pDoc,double * dAlt,BOOL * bOn);
	
	
	void UpdateFloorOverlay(int nLevelID, CPictureInfo& picInfo);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(C3DView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	virtual DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual void OnDragLeave();
	virtual BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	//}}AFX_VIRTUAL
// Implementation
protected:
	virtual ~C3DView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(C3DView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnCtxProcproperties();
	afx_msg void OnCtxCopyProcessor();
	afx_msg void OnCtxRotateProcessors();
	afx_msg void OnCtxScaleProcessors();
	afx_msg void OnCtxScaleProcessorsX();
	afx_msg void OnCtxScaleProcessorsY();
	afx_msg void OnCtxScaleProcessorsZ();
	afx_msg void OnCtxMoveProcessors();
	afx_msg void OnCtxDeleteProc();
	afx_msg void OnCtxMoveShapes();
	afx_msg void OnCtxRotateShapes();
	afx_msg void OnCtxGroup();
	afx_msg void OnCtxAlignProcessors();
	afx_msg void OnUpdateAlign(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCtxGroup(CCmdUI* pCmdUI);
	afx_msg void OnCtxUngroup();
	afx_msg void OnUpdateCtxUngroup(CCmdUI* pCmdUI);
	afx_msg void OnCtxMoveshapeZ();
	afx_msg void OnCtxMoveshapeZ0();
	afx_msg void OnCopyToClipboard();
	afx_msg void OnCutToClipboard();
	afx_msg void OnPickconveyorAdddestination();
	afx_msg void OnPickconveyorInsertafter();
	afx_msg void OnPickconveyorInsertbefore();
	afx_msg void OnProcessPaste();
	afx_msg void OnClearClipboard();
	afx_msg void OnUpdatePro2Name(CCmdUI* pCmdUI);
	afx_msg void OnRenamePro2();
	afx_msg void OnCtxDeletePipe();
	afx_msg void OnBeginCrossWalk();
	afx_msg void OnEndCrossWalk();
	afx_msg void OnDetachCrossWalk(UINT nID);
	//}}AFX_MSG
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnViewPrint();
	afx_msg void OnViewExport();
	afx_msg void OnUpdateGridSpacing(CCmdUI *pCmdUI);
	afx_msg void OnUpdateFPS(CCmdUI *pCmdUI);
	afx_msg void OnUpdateAnimTime(CCmdUI *pCmdUI);
	afx_msg void OnUpdateAnimationSpeed(CCmdUI* pCmdUI);
	afx_msg void OnToggleRailPtEdit();
	afx_msg void OnUpdateToggleRailPtEdit(CCmdUI *pCmdUI);
	afx_msg void OnInsertRailEditPt();
	afx_msg void OnDeleteRailEditPt();
	
	afx_msg void OnChangeShapesMenuList(UINT nID);
	afx_msg void OnSharpenView();
	afx_msg void OnViewRadar();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnCtxFlipProcessors();
	afx_msg void OnCtxReflectProcessors();
	afx_msg void OnCtxArrayProcessors();
	afx_msg void OnAnimPax();
	afx_msg void OnUpdateAnimPax(CCmdUI* pCmdUI);
	afx_msg void OnPasteToFloor(UINT nID);
	afx_msg void OnUpdatePasteToFloor(CCmdUI* pCmdUI);
	afx_msg LRESULT OnMsgDestroyDlgWalkthrough(WPARAM wParam, LPARAM lParam);
	afx_msg void OnEditUndo();
	afx_msg void OnEditRedo();
	afx_msg void OnCopyProcessors(UINT nID);
	afx_msg void OnChooseMultipleSelectItem(UINT nID);
	afx_msg void OnUpdateAddPoint(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()
	CCopyProcToMenuInfo m_cpmi;
public:
	afx_msg void OnCtxTogglestructureptedit();
	afx_msg void OnUpdateCtxTogglestructureptedit(CCmdUI *pCmdUI);
	
	
	afx_msg void OnCtxDirectionBidirection();
	afx_msg	void OnCtxDirectionPositive();
	afx_msg void OnCtxDirectionNegative();
	afx_msg void OnUpdateTaxiwayDir(CCmdUI* pCmdUI);

	
	afx_msg void OnCtxTogglepipeptedit();
	afx_msg void OnUpdateCtxTogglepipeptedit(CCmdUI *pCmdUI);
	afx_msg void OnSubmenuRemovesurface();
	afx_msg void OnSubmenuProprerties();

	ColorMode _currentColorMode;
	void setColorMode(ColorMode mode){_currentColorMode=mode;}
	ColorMode getColorMode(){return _currentColorMode;}
	afx_msg void OnColormodeVivid();
	afx_msg void OnColormodeDesaturated();
	afx_msg void OnClose();
	
	afx_msg void OnTaxiwayPointEnableEdit();
	afx_msg void OnUpdateTaxiwayEnable(CCmdUI *pCmdUI);
	afx_msg void OnTaxiwayAddpointhere();
	afx_msg void OnTaxiwayDeletepoint();
	afx_msg void OnWallshapePointedit();
	afx_msg void OnUpdateWallshapePointedit(CCmdUI *pCmdUI);
	afx_msg void OnWallshapemenuAddpointhere();
	afx_msg void OnWallshapepointmenuDelete();
	afx_msg void OnWallshapeDelete();
	afx_msg void OnCtxWallshapeDispProperties();
	afx_msg void OnWallshapemenuWallshapeproperty();
	afx_msg void OnViewClose();
	afx_msg void OnAircraftdisplayToggleontag();
	afx_msg void OnUpdateAircraftdisplayToggleontag(CCmdUI *pCmdUI);
//	afx_msg void OnAirrouteDisplayproperties();


	CglShapeResource* GetShapeResource(void);
	void RenderRadar();
	void RenderGrids();

	std::vector<CFloor2*> GetFloorsByMode(EnvironmentMode mode);
	std::vector<CFloor2*> GetCurModeFloors();
	
// new design
public:
	
	CAirside3D * GetAirside3D();
	
	virtual void OnNewMouseState();
	
	CSelectionMap & GetSelectionMap(){ return m_SelectionMap; } 
	int SelectAirsideScene(int nFlag, int x, int y , CSize sel_size, SelectableSettings& selsetting , SelectableList& reslt );
	void SelectableMenuPop( Selectable * pSel,const CPoint& point);

	

	//render billboard stuff like text, selection box, etc
	void RenderBillBoard();

	void GenerateDetachSubMenu(CNewMenu*pCtxMenu);
	
	
protected:
	CSelectionMap m_SelectionMap;
	void RotateSelectObject(double dx);
	
public:
	afx_msg void OnALTObjectProperty();
	afx_msg void OnALTObjectLock();
	afx_msg void OnALTObjectDisplayProperty();
	afx_msg void OnAirRouteProperties();
	afx_msg void OnAltobjectCopy();
	afx_msg void OnAltobjectMove();
	afx_msg void OnAltobjectRotate();
	afx_msg void OnAltobjectDelete();
	afx_msg void OnStandAlign();
	afx_msg void OnUpdateStandAlign(CCmdUI *pCmdUI);
	//afx_msg void OnAltobjectCopyClipboard();
	//afx_msg void OnAltobjectReflect();
	//afx_msg void OnAltobjectFlip();
	afx_msg void OnAltobjectEditcontrolpoint();
	afx_msg void OnSubmenuEnableeditcontrolpoint();
	afx_msg void OnUpdateSubmenuEnableeditcontrolpoint(CCmdUI *pCmdUI);
	afx_msg void OnFillettaxiwayEditcontrolpoint();
	afx_msg void OnWallshapeCopy();
	afx_msg void OnSubmenuCopysurface();
	afx_msg void OnAltobjectAddeditpoint();
	afx_msg void OnRemovePoint();
	afx_msg void OnIntersectedstretchEditpoint();
};
/////////////////////////////////////////////////////////////////////////////

#ifndef _DEBUG  // debug version in TermPlanView.cpp
inline CTermPlanDoc* C3DView::GetDocument()
	{ return (CTermPlanDoc*)m_pDocument; }

inline C3DCamera* C3DView::GetCamera()
	{ return m_pCamera; }
#endif
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_3DVIEW_H__23505B99_36F9_11D4_92FF_0080C8F982B1__INCLUDED_)
