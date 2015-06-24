#pragma once
#include <CommonData/3DViewCommon.h>
#include "3DDragDropView.h"

#include <Renderer/RenderEngine/Render3DScene.h>
#include <Renderer/RenderEngine/CoordinateDisplayScene.h>
#include <Renderer/RenderEngine/3DSceneSnapShot.h>
#include "Landside/LandSideVehicleProbDispList.h"
#include "Landside/LandsideVehicleTags.h"
#include <Inputs/Walkthrough.h>
// CRender3DView view

class CRender3DFrame;
class CRender3DContext;
class CTermPlanDoc;
class CCameraData;
class InputAirside;
class OnboardFlightInSim;
class CRenderFloor;
class PanoramaViewData;
class CRenderFloorList;

enum EnvironmentMode;
class CARCportLayoutEditor;

class CRender3DView :
	public C3DDragDropView ,
	public CRender3DScene::Listener,
	public C3DSceneSnapShot
{
	DECLARE_DYNCREATE(CRender3DView)

protected:
	CRender3DView();           // protected constructor used by dynamic creation
	virtual ~CRender3DView();

	
public:

	// listener implements
	virtual void OnUpdateDrawing();
	virtual void OnFinishMeasure(double dDistance);
	virtual void UpdateAlignMarker(ARCVector3 ptMousePos);
	//virtual void* OnQueryDataObject(const CGuid& guid, const GUID& class_guid);
	//virtual bool OnQueryDataObjectList(std::vector<void*>& vData, const GUID& cls_guid);
	//virtual bool OnSaveDataObject(const CGuid& guid, const GUID& cls_guid);
	//virtual CString OnQueryPath(CRender3DScene::PathType type);
	//virtual InputAirside* OnQueryInputAirside();
	virtual bool GetAllRailWayInfo(std::vector<RailwayInfo*>& railWayVect);
	virtual int OnQueryFloorCount();
	virtual bool OnQueryFloorVisible(int floor);
	virtual double OnQueryFloorHeight(int floor);
	virtual void OnClick3DNodeObject(C3DNodeObject* p3DObj,SceneNodeType eNodeType, int id);

	// C3DBaseView interfaces
	virtual void SetupViewport();	
	virtual void PrepareDraw();
	virtual void UnSetupViewport();

	//void BeginPickup(FallbackData* pData, LPARAM lParam);
	void BeginPlaceAlignMarker();

	void UpdateShapesByMode(EnvironmentMode mode);
	void UpdateAll();
	C3DSceneBase* GetScene( Ogre::SceneManager* pScene );

	bool IsLayoutLock() const;

	CRender3DFrame* GetParentFrame() const;
	CRender3DScene& GetModelEditScene();

	CTermPlanDoc* GetDocument() const;
	CRenderFloor* FetchFloors(CRenderFloorList& floors, EnvironmentMode mode) const;

	CCameraData GetUserCurrentCamera()const; // get camera data from Ogre::Camera
	void SetUserCurrentCamera(const CCameraData& cam); // set camera into Ogre::Camera
	CCameraData* GetCameraData() const { return m_pCamera; } // get current located camera data

	void SetCameraData(const CCameraData& camera); // set current located camera data, and load camera
	void LoadCamera(bool bRelocate = false); // 
	void RelocateCameraData(); // reset m_pCamera

	bool GetViewPortMousePos(ARCVector3& pos);

	void UpdateAnimationShapes();
	void UpdateAnimationPax(CTermPlanDoc* pDoc, double* dAlt , BOOL* bOn);
	void UpdateAnimationFlight(const DistanceUnit& airportAlt,BOOL bSelectMode = FALSE, BOOL bShowTag = FALSE);
	void UpdateAnimationVehicle(const DistanceUnit& airportAlt,BOOL bSelectMode = FALSE, BOOL bShowTag = FALSE);
	void UpdateAnimationProcessor(CTermPlanDoc* pDoc, double* dAlt , BOOL* bOn);
	void UpdateMovieCamera();
	void UpdateAnimationEscaltor(CTermPlanDoc* pDoc, double*dAlt, BOOL* bOn);
	void UpdateAnimationTrain(CTermPlanDoc* pDoc, double*dAlt, BOOL* bOn);
	void UpdateAnimationResoureElm(CTermPlanDoc* pDoc, double*dAlt, BOOL* bOn);
	void UpdateLandsideVehicleAnima(BOOL bSelectMode,BOOL _bShowTag);
	void UpdateLandsideTrafficLightAnima();
	void UpdateLandsideVehicleTraces();

	void SnapshotRender(int nWidth,int nHeight,BYTE* pBits,const CCameraData& cameraData);//for make ogre movie
	void SnapshotRender(int nWidth,int nHeight,BYTE* pBits);//for make ogre movie

	void ClearSeledPaxList();

	afx_msg void OnViewExport();

	void OnNewMouseState();
	//virtual void SetWorkHeight(double d);

	virtual void UpdateFloorOverlay( int nLevelID, CPictureInfo& picInfo );

protected:
	virtual bool IsDropAllowed(const DragDropDataItem& dropItem) const;
	virtual void DoDrop(const DragDropDataItem& dropItem, CPoint point);

	virtual bool MoveCamera(UINT nFlags, const CPoint& ptTo, int vpidx = 0 );

	virtual void OnInitialUpdate();
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view

	virtual ViewPort GetSnapshotViewPort();

	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	afx_msg void OnEditUndo();
	afx_msg void OnEditRedo();
	afx_msg void OnDelete();
	//afx_msg void OnLayoutLock();
	afx_msg void OnDistanceMeasure();

	//afx_msg void OnConfigGrid();

	afx_msg void OnUpdateUndo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRedo(CCmdUI* pCmdUI);
	//afx_msg void OnUpdateLayoutLock(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDistanceMeasure(CCmdUI* pCmdUI);

	afx_msg void OnCtxTogglepipeptedit();
	afx_msg void OnUpdateCtxTogglepipeptedit( CCmdUI* pCmdUI );

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	afx_msg void OnUpdateGridSpacing(CCmdUI *pCmdUI);
	afx_msg void OnUpdateAnimTime(CCmdUI *pCmdUI);
	afx_msg void OnUpdateAnimationSpeed(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFPS(CCmdUI *pCmdUI);

	afx_msg void OnCtxProcproperties();
	afx_msg void OnCtxLanddeleteproc();
	afx_msg void OnCtxCopyProc();
	afx_msg void OnCtxDelteControlPoint();
	afx_msg void OnCtxChangeCtrlPointType();
	afx_msg void OnCtxAddControlPoint();
	afx_msg void OnCtxLockALTObject();
	afx_msg void OnUpdateCtxLockALTObject(CCmdUI *pCmdUI);
	afx_msg void OnEnabelEditAltObject();
	afx_msg void OnUpdateCtxEnableEditAltobject(CCmdUI *pCmdUI);

	//on msg set control point edit
	afx_msg void OnSetPointAltitude();
	//on msg set control point to saved altitude of the menu
	//afx_msg void OnSetPointIndexAltitude(UINT nID);
	afx_msg void OnManageSavedAlt();

	afx_msg void OnSnapPointToLevel(UINT nID);

	afx_msg void OnSnapPointToParkLotLevel(UINT nLevel);

	afx_msg void OnCopyParkLotItem();
	afx_msg void OnDelParkLotItem();

	afx_msg void OnContextMenu(CWnd* pWnd, CPoint pos);

	afx_msg void OnCtxDeleteLayoutItem();
	afx_msg void OnCtxLayoutObjectDisplayProp();

	//show tag
	//afx_msg void OnShowObjectTag();
	//afx_msg void OnUpdateShowObjectTag(CCmdUI* pCmdUI);

	afx_msg void OnParkingLotProperty();

	afx_msg LRESULT OnTempFallbackFinished(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnCheckProcessorType(WPARAM wParam,LPARAM lParam);
	afx_msg void OnTimer(UINT nIDEvent);
	CARCportLayoutEditor* GetEditor() const;

	DECLARE_MESSAGE_MAP()

protected:
	void InitSeledPaxList();
	void OnSelMobElement( CWalkthrough::WalkthroughMobType mobType, int nMobID );
	void CreateOnboardFlightInSim();
	OnboardFlightInSim* m_pOnboardFlightInSim;
	UINT_PTR m_nUpdateRedSquaresTimer;
	virtual void ClearContextMenuInfo();

public:
	enum View3DMovieType
	{
		movie_None = -1,

		movie_TrackFirstPerson = 0, // walkthrough
		movie_TrackThirdPerson, // walkthrough
		movie_Panorama,
	};
	bool StartViewTrackMovie( CWalkthrough::WalkthroughMobType mobType, int nMobID = -1, CWalkthrough::VIEWPOINT viewPoint = CWalkthrough::FIRSTPERSON );
	void UpdateTrackMovieCamera();

	bool StartViewPanoramaMovie( const PanoramaViewData* pPanoramaData );
	void UpdatePanoramaMovieCamera( double dRotateDegree );

	void EndViewMovie();

	
	static const UINT SEL_FLIGHT_OFFSET;
	static const UINT SEL_VEHICLE_OFFSET;	

	int m_lastAnimState;


	void DrawDeckCell(CAnimaFlight3D* pFlight3D, OnboardFlightInSim* pOnboardFlightInSim) const;
private:	
	CCoordinateDisplayScene m_coordDisplayScene; // the crossed X-Y-Z coordinations at the bottom-left of the 3D-view
	ViewPort m_vpCamera;

	CCameraData* m_pCamera;

	CWalkthrough::WalkthroughMobType m_eWalkthroughMobType;

	View3DMovieType m_e3DMovieType;
	C3DNodeObject   m_movieNode;
	double          m_dBestSpotAngle;
	ARCVector3      m_movieRelCameraPos;
	ARCVector3      m_movieRelLookAtPos;

	bool m_bReloadPaxDispProp;
	bool m_bReloadPaxTagDispProp;
	bool m_bReloadFlightDispProp;
	bool m_bReloadFlightTagDispProp;
	bool m_bReloadVehicleDispProp; //flag to 
	bool m_bReloadVehicleTagDispProp;

	bool m_bReloadLandsideVehicleTagDispProp;
	bool m_bReloadLandsideVehicleDispProp;

	CLandSideVehicleProbDispList m_landsideVehicleDispProps;
	CLandsideVehicleTagSetList m_landsideVehicleTags;
	struct SelMobDesc
	{
		SelMobDesc(CWalkthrough::WalkthroughMobType _mobType = CWalkthrough::Mob_None, int _nMobID = -1, CString _strTag = _T(""))
			: mobType(_mobType)
			, nMobID(_nMobID)
			, strTag(_strTag)
		{
		}

		CWalkthrough::WalkthroughMobType mobType;
		int nMobID;
		CString strTag;
	};
	class SelPaxDescTypeIDMatch
	{
	public:
		SelPaxDescTypeIDMatch(const SelMobDesc& mobDesc)
			: m_mobDesc(mobDesc)
		{

		}
		bool operator()(const SelMobDesc& other)
		{
			return other.mobType == m_mobDesc.mobType
				&& other.nMobID == m_mobDesc.nMobID;
		}
	private:
		const SelMobDesc& m_mobDesc;
	};
	typedef std::vector<SelMobDesc> SelMobDescList;
	SelMobDescList m_vPaxSelDescList;
	SelMobDesc m_selPaxDesc;

	std::vector<CLandSideVehicleProbDispDetail*> m_displySet;

	CLandSideVehicleProbDispDetail* GetMatchDispProb(int idx, const CString& s);

};


