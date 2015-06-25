#pragma once


#include "../InputAirside/ALTObject.h"
#include "../InputAirside/InputAirside.h"
#include "../InputAirside/AirRoute.h"
class CTermPlanDoc;
class ALTAirport;
class Contour;
class CARCBaseTree;
class CDlgScheduleAndRostContent;
class CAirsideObjectBaseDlg;
// CAirsideMSView view

// When inserting an ALTObject menu item, use macro ID_MENUITEM_OBJECTROOT as the menu id,
// else when inserting an menu item but not an ALTObject, use macro ID_MENUITEM_NOTOBJECTITEM
#include "MSVNodeData.h"

namespace AirsideGUI
{
	class NodeViewDbClickHandler;
};

namespace MSV
{
	enum enumNodeType_Dlg
	{
		//aircraft related
		dlg_Unknown =0,
		Dlg_AirRoute,
		Dlg_WingSpan ,
		Dlg_SurfaceBearingWeight,
		Dlg_WakeVortexWeight,
		Dlg_ApproachSpeed,
		Dlg_CompositeAircraftGroups,
		//airline related
		Dlg_AirlineGroup,
		//airfield related
		Dlg_RunwayInventory,
		Dlg_RunwayExit,
		Dlg_TaxiwayWidth,
		//flight
		Dlg_FlightSchedule,
		Dlg_GateAssignment,
		Dlg_FlightPlans,
		Dlg_DepartureSlots,
		//
		Dlg_ItinerantTraffic,

		//external data
		Dlg_ManualData,

		//flight performance,
		Dlg_PerformanceCruise,
		Dlg_PerformanceTerminal,
		Dlg_PerformanceApproach,
		Dlg_PerformanceLanding,
		Dlg_PerformanceRunwayExit,
		Dlg_PerformanceTaxiInBound,
		Dlg_PerformanceStandService,
		Dlg_PerformancePushBack,
		Dlg_PerformanceEngineStartAndParameters,
		Dlg_PerformanceTaxiOutBound,
		Dlg_PerformanceDeicingTimes,
		Dlg_PerformanceTakeOff,
		Dlg_PerformanceDepartureClimbout,

		//Arrivals ETA Offset
		Dlg_ArrivalETAOffset,

		Dlg_NoiseData,
		//airspace
		Dlg_SectorFlightMixSpecification,
		Dlg_AirspaceInTrailSeparation,
		Dlg_DirectRoutingCriteria,
		//Deicing and anti-icings demand
		Dlg_Deicingandanti_demand,
		//weather impacts
		Dlg_WeatherImpacts,

		Dlg_StarAssignment,

		Dlg_CtrlInterventionSpec,

		//runways
		//Dlg_ATCSeparation,
		//Dlg_ExelusiveRunwayUse,
		//Dlg_ExitUsageCriteria,
		//Dlg_TakeOffPosAssignCriteria,
		Dlg_LandingRunwayAssignment,
		Dlg_SIDAssignment,
		Dlg_LandingRunwayExitStrategies,
		Dlg_FollowMeConnection,
		Dlg_RunwayTakeOffPositions,
		Dlg_RunwayTakeOffAssignment,
		Dlg_RunwayTakeOffQueues,
		Dlg_RunwayTakeOffSequencing,
		Dlg_RunwayClearanceCriteria,
		Dlg_RunwayArrivalDelayTriggers,
		Dlg_EnrouteQCapacity,
		Dlg_ServiceRequirement,
		Dlg_VehicularMovementAllowed,
		//Taxiway
		Dlg_TaxiwayUsageCriteria,
		Dlg_TaxiwayIntersectionBlocking,
		Dlg_TrainingFlightsManagement,
		Dlg_TaxiwayWeightConstraints,
		Dlg_TaxiwayWingspanConstraints,
		Dlg_TaxiwayFlightTyperestrictions,
		Dlg_TaxiwayTemporaryParkingCriteria,
		Dlg_DirectionalityConstraints,
		Dlg_TaxiSpeedConstraints,
		Dlg_IntersectionNodeOption,
		Dlg_InboundRouteAssignment,
		Dlg_OutboundRouteAssignment,
		Dlg_ConflictResolutionCriteria,
		Dlg_TowingRoute,
		Dlg_TaxiInterruptTime,
		//gate
		Dlg_StandBuffers,
		Dlg_PushBackBlocking,
		Dlg_StandConstraints,
		Dlg_GatesTowingOperationParam,
		Dlg_StandCriteriaAssignment,
		Dlg_StandAssignment,
		Dlg_OccupiedAssignedStandAction,
		Dlg_Ignition,
		Dlg_DepInTrail,
		Dlg_ArrivalInit,
		Dlg_SimSetting,
		//aircraft surfaces
		Dlg_AircraftSurfaceCondition,
		Dlg_WeatherScript,
		Dlg_SimRun,


		//analysis parameters
		Dlg_AircraftDisplay,
		Dlg_AircraftTags,
		Dlg_AircraftAlias,
		Dlg_VehicleDisplay,
		Dlg_VehicleTags,
		Dlg_CalloutDispSpec,

		//resource manager
		//Approach
		Dlg_ApprochSeparation,
		Dlg_SideStepSpecification,

		//database
		Dlg_DatabaseFlight,
		Dlg_DatabaseAirline,
		Dlg_DatabaseAirportSector,
		Dlg_DatabaseAircraft,
		Dlg_VehicleSpecification,

		//GoAroundCirteria
		Dlg_GoAroundCriteriaCustomize,

		//PushBackClearanceCriteria
		Dlg_PushBackClearanceCriteria,
		Dlg_VehiclePoolsAndDeployment,

		Dlg_FlightTypeRelativeServiceLocations,
		Dlg_FlightTypeDimensions,
		Dlg_VehicleRoute,
		Dlg_AirportReferencePoints, 
		Dlg_Paxbusparking,
		Dlg_BagCartsLoadingAreas,

		//towoffstandassignment
		Dlg_TowOffStandAssignment,
		Dlg_HoldShortLines,

		//Deice Pad
		Dlg_DeiceQueue,
		Dlg_PadAssignment,
		Dlg_DeiceRoute,
		Dlg_PostDeiceRoute,
		Dlg_DeiceAntiStrategies,
		//Sector ControllerIntervention
		Dlg_ControllerIntervention,

		Dlg_WaveCrossRunwaySpecification,

		Dlg_PushBackAndTowOperationSpec,
		Dlg_TrainingFlightSpec

	};


class CAirsideMSView : public CView
{
	DECLARE_DYNCREATE(CAirsideMSView)

protected:
	CAirsideMSView();           // protected constructor used by dynamic creation
	virtual ~CAirsideMSView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()

protected:
	HTREEITEM m_hRightClkItem;
	CImageList m_imgList;
	int m_nProjID;
	CARCBaseTree m_wndTreeCtrl;

	CARCBaseTree& GetTreeCtrl(){ return m_wndTreeCtrl;}
	void InitTree();
	HTREEITEM AddItem(const CString& strNode,HTREEITEM hParent = TVI_ROOT,HTREEITEM hInsertAfter = TVI_LAST,int nImage = ID_NODEIMG_DEFAULT,int nSelectImage = ID_NODEIMG_DEFAULT);
	HTREEITEM AddARCBaseTreeItem(const CString& strNode,HTREEITEM hParent = TVI_ROOT,NODE_EDIT_TYPE net = NET_NORMAL,NODE_TYPE nt = NT_NORMAL,HTREEITEM hInsertAfter = TVI_LAST,int nImage = ID_NODEIMG_DEFAULT,int nSelectImage = ID_NODEIMG_DEFAULT);
	void AddAirportToTree(HTREEITEM hItemAirports,const ALTAirport& airport);
	void AddLevelToTree(HTREEITEM hItemAirports,const ALTAirport& airport);
	void AddRunwayandTaxiwaytoTree(HTREEITEM hItem);
	HTREEITEM getLevelItem(HTREEITEM hItem,int nLevel);

	bool LoadObjects(HTREEITEM hObjRoot);
	void LoadChildContour(HTREEITEM hParentItem, Contour *pContour);

	void AddObjectToTree(HTREEITEM hObjRoot,ALTObject* pObject);
	void AddAirRouteToTree(HTREEITEM hAirItem,CAirRoute* pAirRoute);

	bool CheckAirRouteByName(CString strAirRouteName,HTREEITEM hItem);

	HTREEITEM GetObjRootNode(HTREEITEM hObjItem);
	void DeleteObjectFromTree(HTREEITEM hObjItem);
    void DeleteChildCoutour(HTREEITEM hObjItem);
	HTREEITEM FindObjNode(HTREEITEM hParentItem,const CString& strNodeText);
	CTermPlanDoc *GetARCDocument();

	HTREEITEM FindTreeItemByTypeID(ALTOBJECT_TYPE type, int nObjID);
	//	HTREEITEM FindTreeItemByObjectID(int nObjID);
	HTREEITEM FindTreeItemByObjectID(HTREEITEM hItem,int nObjID);
	bool FindUnquieName(int nUnqiueID);

	CNodeData* GetSelectedNodeData();

	void DisplayExternalManual();

	HTREEITEM m_hItemAirports;
	HTREEITEM m_hItemAirspace;
	HTREEITEM m_hItemTopography;
	HTREEITEM m_hItemLevels;
	HTREEITEM m_hItemBaseLevel;
	
	HTREEITEM m_hWaypoint;
	//external data ui
	CDlgScheduleAndRostContent* m_pDlgScheduleRosterContent;

	AirsideGUI::NodeViewDbClickHandler* m_pAirsideNodeHandler;
	void UpdateAirportLevelOrder();
	void UpdatePopMenu(CCmdTarget* pThis, CMenu* pPopMenu);
	HTREEITEM FindAirportObjRootByType(ALTOBJECT_TYPE altType);
	HTREEITEM FindAirspaceObjRootByType(ALTOBJECT_TYPE altType);
	HTREEITEM FindTopographyObjRootByType(ALTOBJECT_TYPE altType);

	HTREEITEM FindObjectRootByType(ALTOBJECT_TYPE altType);
	HTREEITEM FindObjectRootByType(ALTOBJECT_TYPE altType, HTREEITEM hStartItem);

protected:
	CAirsideObjectBaseDlg* GetObjectDefineDlg(CNodeData* pNodeData,int nObjID);
	//CDialog* GetObjectDefineDlg(CNodeData* pNodeData,ALTObject* pObj);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
	virtual void OnDraw(CDC* pDC){}
	virtual void OnInitialUpdate();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnSize(UINT nType, int cx, int cy);

	afx_msg LRESULT OnBeginLabelEdit(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEndLabelEdit(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSelChanged(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnItemExpanding(WPARAM wParam, LPARAM lParam);
	afx_msg void OnRClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnNewAirsideObject(UINT nID);
	afx_msg void OnCtxIputLatlong();
	afx_msg void OnHideControl();
	afx_msg void OnFloorProperties();

	afx_msg void OnFloorPictureInsert();
	afx_msg void OnFloorPictureHide();
	afx_msg void OnFloorPictureShow();
	afx_msg void OnFloorPictureRemove();
	afx_msg void OnFloorPictureSave();
	afx_msg void OnFloorPictureLoad();

	afx_msg void OnNewAirport();
	afx_msg void OnDeleteAirport();
	afx_msg void OnRenameAirport();
	afx_msg void OnNewLevelAboveARP();
	afx_msg void OnNewLevelBelowARP();
	afx_msg void OnDeleteAirsideLevel();
	afx_msg void OnRenameAirsideLevel();
	afx_msg void OnSetVehicleRoutes(void);
	afx_msg void OnDefineAirRoutes(void);
	afx_msg void OnFlightSchedule(void);
	afx_msg void OnSetItinerantTraffic(void);
	afx_msg void OnEditAirportSectorsDB(void);
	afx_msg void OnEditAirlineGroupDB(void);
	afx_msg void OnEditAircraftTypeAndCategory(void);
	afx_msg void OnEditFliTypeDimension(void);
	afx_msg void OnEditWingspanCategory(void);
	afx_msg void OnEditSurfaceBearingCategory(void);
	afx_msg void OnEditWakeVortexCategory(void);
	afx_msg void OnEditApproachSpeedCategory(void);
	afx_msg void OnEditCruise(void);
	afx_msg void OnEditTerminalManeuver(void);
	afx_msg void OnEditApproachToLand(void);
	afx_msg void OnEditLandingPerformance(void);
	afx_msg void OnEditTaxiInbound(void);
	afx_msg void OnEditStandServicePerformance(void);
	afx_msg void OnEditPushBackPerformance(void);
	afx_msg void OnEditTaxiOutboundPerformance(void);
	afx_msg void OnEditTakeoffPerformance(void);
	afx_msg void OnEditDepartureClimbOut(void);
	afx_msg void OnEditFlightPlan(void);
	afx_msg void OnEditDepartureSlot(void);
	afx_msg void OnEditVehicle(void);
	afx_msg void OnEditFliTypeLocation(void);
	afx_msg void OnEditServiceRequirement(void);
	afx_msg void OnEditWeatherImpact(void);
	afx_msg void OnEditSectorFlightMixSpec(void);
	afx_msg void OnEditInTrail(void);
	afx_msg void OnEditDirectRouting(void);
	afx_msg void OnEditApproachSeparation(void);
	afx_msg void OnControllerIntervention(void);
	afx_msg void OnEditSideStep(void);
	afx_msg void OnEditGoAround(void);
	afx_msg void OnEditSTARAssignment(void);
	afx_msg void OnEditLandingRunwayAssignment(void);
	afx_msg void OnEditLandingRunwayExitStrategy(void);
	afx_msg void OnEditTaxiwayWeight(void);
	afx_msg void OnEditTaxiwayWingspanConstraint(void);
	afx_msg void OnEditTaxiwayFliTypeRestriction(void);
	afx_msg void OnEditTrainFlightsManagement(void);
	afx_msg void OnEditDirectionalityConstraint(void);
	afx_msg void OnEditTaxiSpeedConstraint(void);
	afx_msg void OnEditInboundRouteAssignment(void);
	afx_msg void OnEditOutboundRouteAssignment(void);
	afx_msg void OnEditTemporaryParking(void);
	afx_msg void OnEditConflictResolution(void);
	afx_msg void OnEditParkingStandBuffer(void);
	afx_msg void OnEditStandAssignment(void);
	afx_msg void OnEditFlightGroupsDB(void);
	afx_msg void OnOccupyStandAction(void);
	afx_msg void OnTaxiOutPushBack(void);
	afx_msg void OnEditTakeoffRunwayAssignment(void);
	afx_msg void OnEditSIDAssignment(void);
	afx_msg void OnEditTakeoffQueue(void);
	afx_msg void OnEditTakeoffClearance(void);
	afx_msg void OnEditRunwayTakeoffPosition(void);
	afx_msg void OnEditTakeoffSequence(void);
	afx_msg void OnEditArrivalDelayTrigger(void);
	afx_msg void OnEditEnrouteQueueCapacity(void);
	afx_msg void OnEditVehiclePools(void);
	afx_msg void OnSetSimulationEngine(void);
	afx_msg void OnRunSimulation(void);
	afx_msg void OnDisplayAircraft(void);
	afx_msg void OnDisplayVehicle(void);
	afx_msg void OnSetAircraftTags(void);
	afx_msg void OnSetVehicleTags(void);
	afx_msg void OnCalloutDispSpec(void);
	afx_msg void OnHoldShortLines(void);
	afx_msg void OnEidtAirRoute(void);
	afx_msg void OnDeleteAirRoute(void);
	afx_msg void OnCopyAirRoute(void);
	afx_msg void OnDisplayAirRouteProperties(void);
	afx_msg void OnDisplayAllAirRouteProperties(void);

	afx_msg void OnUpdateFloorVisible(CCmdUI* pCmdUI);
	afx_msg void OnUpdateGridVisible(CCmdUI* pCmdUI);
	afx_msg void OnUpdateHideControl(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFloorMonochrome(CCmdUI* pCmdUI);
	afx_msg void OnUpdateShowHideMap(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOpaqueFloor(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFloorsDelete(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePlaceMarker(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePlaceMarkerLine(CCmdUI* pCmdUI);
	afx_msg void OnUpdateThicknessValue(CCmdUI* pCmdUI);
	afx_msg void OnUpdateVisibleRegins(CCmdUI* pCmdUI);
	afx_msg void OnUpdateInvisibleRegins(CCmdUI* pCmdUI);

	afx_msg void OnUpdateFloorPictureHide(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFloorPictureShow(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFloorPictureRemove(CCmdUI* pCmdUI);


    afx_msg void OnUpdateRemoveAlignLine(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRemoveMarker(CCmdUI* pCmdUI);
	afx_msg void  OnRemoveAlignLine() ;
	afx_msg void OnRemoveMarker() ;
    afx_msg void  OnsetMarker() ;
    afx_msg void OnsetAlignLine() ;

	afx_msg void  OnUpdateAlignMarker(CCmdUI* pCmdUI);

	afx_msg void OnAddSurface();
	afx_msg void OnRemoveSurface();
	afx_msg void OnShowSurface();
	afx_msg void OnHideSurface();

	afx_msg void OnUpdateShowSurface(CCmdUI* pCmdUI);
	afx_msg void OnUpdateHideSurface(CCmdUI* pCmdUI);

	//afx_msg void OnAddCountour();
	//afx_msg void OnUpdateAddContour(CCmdUI* pCmdUI);
	afx_msg void OnEditControlPoint();
	afx_msg void OnDisableControlPoint();
	afx_msg void OnUpdateEditControlPoint(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDisableControlPoint(CCmdUI* pCmdUI);
	afx_msg void OnReportingAreaDisplayProperty();

	afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMThemeChanged(NMHDR *pNMHDR, LRESULT *pResult);
//	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	void OnLButtonDblClk(WPARAM wParam, LPARAM lParam);
	afx_msg void OnObjectProperties();
	afx_msg void OnUpdateObjectDelete(CCmdUI* pCmdUI);
	afx_msg void OnObjectDelete();

	afx_msg void OnGridVisible();
	afx_msg void OnFloorMonochrome();
	afx_msg void OnFloorVisible();
	afx_msg void OnShowHideMap();
	afx_msg void OnStretchOption(void);

	afx_msg void OnLockALTObject(void);
	afx_msg void OnUnlockALTObject(void);

	afx_msg void OnDefineTaxidRoute();
	afx_msg void OnImportWayPoints();

	void LockALTObject(bool bLock);
	void LockALTObject(HTREEITEM hTreeItem, bool bLock);
	void  OnAlignMarker() ;
	LRESULT SetActiveFloorMarkerLine(WPARAM wParam, LPARAM lParam) ;

private:
	BOOL  use_Marker ;
	BOOL  use_Marker_line ;
	CString m_strData;
	int m_nType;
	HTREEITEM m_hObjSurface;
	CNodeDataAllocator* mpNodeDataAlloc;
};


}
