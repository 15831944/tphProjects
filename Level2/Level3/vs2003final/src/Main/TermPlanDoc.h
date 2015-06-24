// TermPlanDoc.h : interface of the CTermPlanDoc class
//
/////////////////////////////////////////////////////////////////////////////
#ifndef __TERMPLANDOC_H__
#define __TERMPLANDOC_H__

#include "Engine\EngineConfig.h"
#include "Common\TERM_BIN.H"
#include "TempProcInfo.h"
#include "ModelingSequenceManager.h"
#include "Common\CommandHistory.h"
#include "Floors.h"
#include "ARCPort.h"
#include "DisplayPropOverrides.h"
#include "DefaultDisplayProperties.h"
#include "PaxDispProps.h"
#include "AircraftDispProps.h"
#include "InputAirside\VehicleDispProps.h"
#include "Landside\LandSideVehicleProbDispList.h"
#include "ResourceDispProps.h"
#include "PaxTags.h"
#include "AircraftTags.h"
#include "InputAirside\VehicleTag.h"
#include "Landside\LandsideVehicleTags.h"
#include "ActivityDensityParams.h"
#include "Movies.h"
#include "VideoParams.h"
#include "ProcessorTags.h"
#include "Cameras.h"
#include "CommonData\SurfaceMaterialLib.h"
#include "Common\3DViewCameras.h"
#include "AnimationData.h"
#include "Common\TempTracerData.h"
#include "MathFlow.h"
#include "MathProcess.h"
#include "MathResultManager.h"
#include "XPStyle\NewMenu.h"
#include "Inputs\Walkthrough.h"
#include "SelectableSettings.h"
#include "CallOutData\CallOutDataManager.h"
#include "Proc2DataForClipBoard.h"
//chloe
class OnBoradInput;
struct DocAdapter;

class ALTObject;
class OutputAirside;
class CObjectDisplay;
class CPlacement;
class CAutomaticSaveTimer ;
class CProcessor2;
class C3DCamera;
class CCameraNode;
class C3DView;
class CRender3DView;
class CChildFrame;
class CNodeView;
class CDensityGLView;
class CFIDSView;
class CMainFrame; 
class CFloor2;
class CActivityDensityData;
class CAftemsAVIFile;
class CReportParameter;
class CRecordedCameras;
class CDlgWalkthrough;
class CDlgWalkthroughRender;
class CDlgMovieWriter2;
class CSimEngineDlg;

class LandsideDocument;
class LandsideDView;
class ALTObjectDisplayProp;
class CAircraftLayoutEditor;
class CRender3DEditor;
class IRender3DView;
class CModelessDlgManager;
class CTVNode;
class CNewMenu;
class CCalloutDlgLineData;

enum ARCUnitManagerOwnerID;
class CARCUnit;
//class CAirsideReportManager;
class CDlgPanorama;
//class CAircraftModelEditView;
class COnboardAircraftCommanView;
class CRenderFloor;
class CARCportLayoutEditor;	
class CLayoutView ;
class CARCUnitManager;
class CLandsideSimLogs;
class Processor;
class CWalkthroughs;

NAMESPACE_AIRSIDEENGINE_BEGINE
class Simulation;
NAMESPACE_AIRSIDEENGINE_END

typedef std::vector<MobDescStructEx> MobDescList;
typedef std::pair<Processor*, Processor*> ProcCopyPair;

//enum LoadReportType
//{
//	load_by_system = 0,
//	load_by_user
//}; move to CARCReportManager


enum SimulationType
{
	SimType_MontCarlo = 0,
	SimType_Mathmatical
};


class C3DBaseView;
class CMeshEdit3DView;
class CTermPlanDoc : public CDocument
{
protected: // create from serialization only
	CTermPlanDoc();
	virtual ~CTermPlanDoc();
	DECLARE_DYNCREATE(CTermPlanDoc)
		
		// Attributes
public:
	//BOOL m_bShowGraph; move to CARCReportManager
	CTempProcInfo m_tempProcInfo;
	

	// Operations
public:
	
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTermPlanDoc)
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual void OnCloseDocument();
	virtual void SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU = TRUE);
	//}}AFX_VIRTUAL
	virtual BOOL DoSave(LPCTSTR lpszPathName, BOOL bReplace = TRUE);
	
	// Implementation

public:

	//CAirsideReportManager *m_pAirsideReportManager;// move to CARCReportManager
	CModelingSequenceManager m_msManager;
	//CReportSpecs* m_prepSpecs;
	ProcessorID m_selectProID;
	void setSelectProcID(ProcessorID& _id){m_selectProID = _id;}
	bool m_bCanResposeEvent;
	//Floor Stuff
	CFloor2* GetActiveFloor();
	
	int GetFloorCount()const;

	double GetActiveFloorAltitude();
	BOOL DeleteFloor(int nFloor);
	
	//Proc Stuff
	void OnNewProcessor(CTVNode* pNode);
	void OnMoveFloor(double dx, double dy);
	void OnMoveProc(double dx, double dy);
	void OnMoveProcs(double dx, double dy);
	void OnScaleProc(double dx, double dy);
	void OnScaleProcX(double dx, double dy);
	void OnScaleProcY(double dx, double dy);
	void OnScaleProcZ(double dx, double dy);
	void OnRotateProc(double dx);
	void OnRotateShape(double dx);
	void OnMoveShape(double dx, double dy);
	void OnProcEditFinished();
	void OnMoveRailPt(int nRailIdx, int nPtIdx, double dx, double dy);
	void OnMovePipePt(int nPipeIdx, int nPtIdx, double dx, double dy);
	void OnMovePipeWidthPt(int nPipeIdx, int nPtIdx, double dx, double dy);
	void SelectProcessors(const std::vector<CProcessor2*>& proclist);
	BOOL SelectProc(CProcessor2* pProc, BOOL bAddToSelection=FALSE);
	BOOL SelectProc(GLuint nSelName, BOOL bAddToSelection=FALSE);
	BOOL SelectObject(CObjectDisplay* pObjDisplay, BOOL bAddToSelection);
	BOOL UnSelectProc(CProcessor2* pProc);
	void UnSelectAllProcs();
	CProcessor2* GetProcWithSelName(GLuint nSelName);
	BOOL DoLButtonDownOnProc(GLuint nSelName, BOOL bCtrlDown);
	BOOL DoLButtonUpOnProc(GLuint nSelName, BOOL bCtrlDown);
	
	BOOL AnimTimerCallback();
	BOOL RecordTimerCallback(DWORD dwTime);
	
	void SelectNode(CTVNode* pNode);
	CTVNode* GetSelectedNode() { return m_pSelectedNode; }
	
	//camera stuff
	//std::vector<C3DCamera*>& GetSavedViews() const { return m_cameras.m_vBookmarkedCameras; };
	//void ResetCameraFocusDistance(C3DCamera* pCam);
	void SetCameraWBS();	//sets the coords for the world bounding sphere for all cameras
	//void CreateCustomCamera(BOOL b3D = FALSE);
	//BOOL CreateDefaultCameras();
	//void DeleteAllCameras();
	
	//view functions
	C3DView* Get3DView() const;
	CRender3DView* GetRender3DView()const;
	IRender3DView* GetIRenderView()const;
	//CAircraftModelEditView* GetModelEditView();
	COnboardAircraftCommanView* GetAircraftCommandView();
	CMeshEdit3DView* GetMeshEdit3DView();
	C3DBaseView* Get3DBaseView(); // for onboard editing
	CChildFrame* Get3DViewParent();
	CNodeView* GetNodeView();
	CMainFrame* GetMainFrame();
	CDensityGLView* GetDensityAnimView();

	//layout edit context and functions
	CAircraftLayoutEditor* GetAircraftLayoutEditor();
	CARCportLayoutEditor* GetLayoutEditor()const{ return m_playoutEditor; }

protected:	
	CARCportLayoutEditor* m_playoutEditor;	
public:
   
	// Return the Flight Information Display frame, if created...
	CFIDSView *GetFIDSView();
	
    BOOL SaveNamedView(const CString& _s);
	BOOL SaveNamedRenderView(const CString& _sName);

	BOOL RouteCmdToAllViews(CView *pView, UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	
	//node stuff
//	CTVNode* FindNodeByName(UINT idName);	// the id of a string resource holding the name
//	CTVNode* FindNodeByName(LPCTSTR sName);	// the name to find
//	CTVNode* FindNodeByData(DWORD dwData);	// the  dwData member to match
	CTVNode* ProcessorsNode();
	CTVNode* UnderConstructionNode();
	CTVNode* FloorsNode();
	CTVNode* LayoutNode();
	
	//processor helper functions
	BOOL AddToUnderConstructionNode(CProcessor2* pProc2);
	BOOL MoveFromUCToDefined(CProcessor2* pProc);
	BOOL RenameProcessor(ProcessorID& id, LPCTSTR sNewName); //returns TRUE if the rename was successful
	BOOL RenameProcessorGroup(ProcessorID& id, int nLevel, LPCTSTR sNewName);
	
	BOOL DeleteProcessor(CProcessor2* pProc2);
	void DeleteProcessors(const CObjectDisplayList& proclist);
	void CopyProcessors(const CObjectDisplayList& proclist);
	CProcessor2* CopyProcessor(CProcessor2* pProc2);
	void MoveProcessor(CProcessor2* pProc2, double dx, double dy);
	void CopySelectedProcessors();
	void MirrorSelectedProcessors(const std::vector<ARCVector3>& vLine);
	void ArraySelectedProcessors(const std::vector<ARCVector3>& vLine, int nCopies);
	void AlignSelectedProcessors(const std::vector<ARCVector3>& vLine);
	void RefreshProcessorTree();
	
	void UpdateViewSharpTextures();
	
	void SetActiveFloorMarker(const ARCVector3& vMousePos);
	
	// return the number of created processor2
	int CreateProcessor2ByProcList( std::vector<ProcCopyPair>& _procPairList );
	
	void CreateMovieFromLiveRecording();
	
	void StopRecording(BOOL bAndWriteMovie=FALSE);
	
	void OnSelectPAX(UINT nPaxID, const CString& sDesc);

	void SetSelectPaxID(UINT nPaxID);

	//CDlgWalkthroughRender* GetDlgWalkthroughRender() const { return m_pDlgWalkthroughRender; }
	
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	
	BOOL StartAnimation(BOOL bGetTimeRangeFromUser); //returns TRUE if successful
	//will warn user and return FALSE if problem
	void StopAnimation();
	
	enum ANIMATIONSTATE {
		anim_none = 0,
			anim_playF,
			anim_playB,
			anim_pause,
			anim_viewmanip, //change viewpoint mode
	};
	
	ANIMATIONSTATE m_eAnimState;
	
	BOOL m_bMobileElementDisplay;
	BOOL m_bActivityDensityDisplay;
	BOOL m_bFlightInfoDisplay;
	
	BOOL m_bRecord;
	//BOOL m_bHideControl;
    double m_iScale;
	BOOL m_bIsOpenStencil;
	
	BOOL m_bLayoutLocked;
	BOOL m_bHideACTags;
	
	BOOL m_bUnselectOnButonUp;
	
	ARCVector3 m_ptProcCmdPosStart;
	CommandHistory m_CommandHistory;
	
	CRecordedCameras* m_pRecordedCameras;
	

	CModelessDlgManager* m_pModelessDlgMan;


protected:
	BOOL m_bNeverSaved;
	CTVNode* m_pSelectedNode;
	BOOL m_bNeedToSaveProjFile;
	CFloors m_floors;
	CLandsideFloors m_landsidefloors;

	


private:
	//CReportParameter* m_pCurrentReportPara;
	//LoadReportType m_eLoadReportType;
	//CString m_strUserLoadReportFile; //move to CARCReportManager
	bool clearLogsIfNecessary();

	//ENUM_REPORT_TYPE m_multiRunsRportType;
	//std::vector<int> m_vSelectedMultiRunsSimResults;
	//CMultiRunReport m_multiReport;//move to CARCReportManager

public:
	CARCReportManager& GetARCReportManager() { return m_arcport.GetARCReportManager(); }
	CProcessor2* getProcessor2ByProcessor( const Processor* _pProc );

	//void SetMultiRunsReportType(ENUM_REPORT_TYPE reportType) { m_arcReportManager.SetMultiRunsReportType(reportType); }
	//ENUM_REPORT_TYPE GetMultiRunsReportType() { return m_arcReportManager.GetMultiRunsReportType(); }
	//std::vector<int>& GetmSelectedMultiRunsSimResults() { return m_arcReportManager.GetmSelectedMultiRunsSimResults(); }


	//CMultiRunReport& getMultiReport(){ return m_arcReportManager.getMultiReport();} 

public:
	//void SetLoadReportType( LoadReportType _type );
	//void SetUserLoadReportFile( const CString& _strFile );
	//LoadReportType GetLoadReportType(void) const;
	//const CString& GetUserLoadReportFile( void ) const;
	
	void UpdateTrackersMenu();
	//void SetReportPara( CReportParameter* _pPara );
	//CReportParameter* GetReportPara(){ return m_arcReportManager.GetReportPara();	}
	void PasteProc(int nPasteToFloorIndex,PROCESSOR2_DATA_FOR_CLIPBOARD* pPDFC);
	void OnMoveProcs(double dx, double dy,CPoint point,CView* pView);
	BOOL m_bOpenDocSuccess;
	void OnMoveShapeZ(double dx, double dy,BOOL bZ2zero=FALSE);
	void SelectByGroup(CObjectDisplay* pProc);
	void AddProcToSelectionList(CObjectDisplay* pProc);
	BOOL ReloadInputData();
	UINT GetUniqueNumber();
	Terminal& GetTerminal(){ return m_arcport.getTerminal(); }
	InputAirside& GetInputAirside(){return m_arcport.GetInputAirside();}
	InputLandside *GetInputLandside(){return m_arcport.GetInputLandside();}
	CEconomicManager& GetEconomicManager(){ return m_arcport.getEconomicManager(); }
	CAirsideSimLogs& GetAirsideSimLogs();
	CLandsideSimLogs& GetLandsideSimLogs();
	//CString GetRepTitle();
	//CString GetAirsideReportTitle();
	//int GetSpecType();
	void GenerateReport();
	//ENUM_REPORT_TYPE m_reportType; //move to CARCReportManager
	//////// Project Specific Data ////////
	PROJECTINFO m_ProjInfo;
	void UpdateProjModifyTime( void );
	
	//////// railway graph
	TrainTrafficGraph m_trainTraffic;
	//////// Begin Model Data ////////
	//Floors

	//CFloor2* m_pActiveFloor;
	BOOL ActivateFloor(int nLevel);
	int m_nActiveFloor;
	int m_nLastActiveFloor;
	//if can paste process
	
	//Processors
	//CPROCESSOR2LIST m_vProcessors;

	CObjectDisplayList & GetSelectProcessors();
	int GetSelectProcessorsCount();
	CObjectDisplay *GetSelectedObjectDisplay(int nIndex);

	
//	CObjectDisplayList m_vSelectPlacements; //pipes , areas , structures
	
	

	//added by frank 0524
public:
	EnvironmentMode m_systemMode;
	EnvironmentMode m_lastSystemMode;
public:
	SimulationType m_simType;
	void UpdateEnvandSimMode();
	
		
	//BOOL m_pbProcDisplayOverrides[6];
	CDisplayPropOverrides m_displayoverrides;

	//BOOL display Taxiway Direction
	BOOL m_bdisplayTaxiDir;

	
	//Areas, Portals
	CPortals m_portals;
	
	
	CDefaultDisplayProperties m_defDispProp;
	
	CPaxDispProps m_paxDispProps;
	CAircraftDispProps m_aircraftDispProps;
	CVehicleDispProps  m_vehicleDispProps;
	CLandSideVehicleProbDispList m_landsideVehicleDispProps;
	CResourceDispProps m_resDispProps;

	
	CPlacement* GetTerminalPlacement()const;
	CPlacement* m_pPlacement;
	//CPlacement m_allPlacement;

	//include the objects not in the placement,
	//such as surface, wall shape,areas, portals,pipes 
	//COtherPlacement m_otherPlacement;
	
	CPaxTags m_paxTags;
	CAircraftTags m_aircraftTags;
	CVehicleTag   m_vehicleTags;
	CLandsideVehicleTagSetList m_landsideVehicleTags;
	
	CActivityDensityParams m_adParams;
	
	CMovies m_movies;
	
	CWalkthroughs *m_walkthroughs;
	CWalkthroughs *m_walkthroughsFlight;
	CWalkthroughs *m_walkthroughsVehicle;
	CVideoParams m_videoParams;
	
	CProcessorTags m_procTags;
	
	//////// End Model Data ////////
	
	//////// Begin View Data ///////
	
	//Cameras
	CCameras m_cameras;
	CSurfaceMaterialLib m_SurfaceMaterialLib;
	//Cameras for render view
	C3DViewCameras m_terminalCameras;
	C3DViewCameras m_airsideCameras;
	C3DViewCameras m_lansideCameras;
	C3DViewCameras m_onboardCameras;

	//Pane layouts
	
	std::vector<int> m_vSelectedACTags;
	std::vector<int> m_vSelectedAirsideTags;
	
	//////// End View Data ////////

	
	//////// Begin Anim Data //////
	AnimationData m_animData;
	
	CActivityDensityData* m_pADData;
	
	//MobDescList m_vMobDesc;
	std::vector<int> m_vPaxDispPropIdx;	//table linking a pax disp property entry to a paxdesc
	std::vector<int> m_vPaxTagPropIdx;	//table linking a pax tag property entry to a paxdesc
	std::vector<int> m_vResDispPropIdx;	//table linking a res disp property entry to a resdesc
	std::vector< std::pair<int,int> > m_vACDispPropIdx;	//table linking an aircraft display property to a flightdesc arr dep
	std::vector<int> m_vACShapeIdx;		//table linking an aircraft shape with a flight desc
	std::vector<int> m_vAVehicleDispPropIdx;  //table linking an airside Vehicle displayprop entry;
	std::vector<CString> m_vAVehicleShapeNameIdx; //table linking an airside Vehicle Shape name;
	std::vector< std::pair<int,int> > m_vACTagPropIdx;	//table linking an aircraft tag property to a flightdesc
	std::vector<int> m_pPaxCountPerDispProp;		//used for tracer densities to count # of pax for each PDPI
	CTempTracerData m_tempTracerData;

	std::vector<int> m_pACCountPerDispProp;   //used for tracer densities to count #of aircraft
	CTempTracerData m_tempAirsideTracerData;  //aircraft trace data

	std::vector<int> m_pVehicleCountPerDispProp;		//used for Vehicle tracer densities to count # of Vehicle for each PDPI
	CTempTracerData m_tempVehicleTracerData;  //vehicle trace data

	std::vector<AircraftDisplaySwitch> m_vACDispSwitchs;
	//////// End Anim Data ////////
	
	//////// Begin Tracer Data ///////
	BOOL m_bShowTracers;
	////////Mathematic Data///////////
	std::vector<CMathFlow> m_vFlow;
	std::vector<CMathProcess> m_vProcess;
	CMathResultManager m_MathResultManager;
	BOOL SaveMathematicData();
	BOOL LoadMathResult();

	//log file
//	CLogDebug m_logFile;
	//Landside field
	//LandsideDView * m_pLandsideView;
	//LandsideDView * GetLandsideView(){ return m_pLandsideView; }

	//LandsideDocument * m_pLandsideDoc;
	//LandsideDocument * GetLandsideDoc(){ return m_pLandsideDoc; } 

	//Unit Manager
	CARCUnitManager& m_umGlobalUnitManager;//global unit manager
	void InitiaUnitManager(void);
public:
	void AdjustStationLinkedRailWay( IntegratedStation* _pStation , bool _bSaveDataToFile= true );
	void GetCurModeFloorsRealAltitude(std::vector<double>& vRealAltitude);
//	void GetCurModeFloorsLogicalAltitude(std::vector<double>& vLogicalAltitude);
	
public:
	BOOL LoadMathematicData();
	CNewMenu* m_pMenuTagsParams;
	CNewMenu* m_pMenuTracerParams;
	CNewMenu m_menuTags;
	CNewMenu m_menuTracersList;
	// get the num of sim_result 
	int  getSimResultCout( void );
	
	//LRESULT OnMsgDestroyDlgWalkthrough(WPARAM wParam, LPARAM lParam);
	//void DestroyDlgWalkthroughRender();

	DistanceUnit ConvertLogZ2RealZ( DistanceUnit _dZ )const;
	DistanceUnit ConvertRealZ2LogZ( DistanceUnit _dZ )const;
	
	CProcessor2* GetProcessor2FromID(const ProcessorID& id, EnvironmentMode mode = EnvMode_Unknow);
	CProcessor2* GetProcessor2FromID(const ALTObjectID& id, EnvironmentMode mode = EnvMode_Unknow);
	CProcessor2* GetProcessor2FromName(const CString strName, EnvironmentMode mode = EnvMode_Unknow);

	MobLogEntry GetMobLogEntry( int idx );
	AirsideFlightLogEntry GetAirsideFlightLogEntry( int idx );

	CString GetPaxTypeID(const MobDescStruct& _MDS);
	CString GetPaxTagFromID(UINT ii);

	// 
	CString GetMobTagFromID(CWalkthrough::WalkthroughMobType mobType, int nMobID);

	void BuildPaxTag( DWORD _ti, const MobDescStruct& _MDS, MobEventStruct& _MESa, CString& _sPaxTag );
	
	OutputAirside * GetOutputAirside(){ return m_arcport.GetOutputAirside(); }

	InputOnboard* GetInputOnboard(){ return  m_arcport.GetInputOnboard(); }

	// Generated message map functions

	CARCPort *getARCport();

	const CMultiMobConstraint& GetCreateMeList()const {return m_createMeList;}
	void SetCreateMeList(const CMultiMobConstraint& MEConList) {m_createMeList = MEConList;}
protected:
	
	CObjectDisplayList m_vSelectedProcessors;
	
	UINT m_uUniqueNumber;

	CARCPort m_arcport;
	
	CMultiMobConstraint  m_createMeList; //Just for import roster

	CSimEngineDlg* m_pDlgSimEngine;
	//CAirsideSimLogs m_airsideSimLogs;
//	AirsideInput m_airsideInput;


	BOOL ReadPAXDescriptions(long* pnFIT, long* pnLOT);
	BOOL ReadACDescriptions(long* pnFIT, long* pnLOT);
	/************************************************************************
	FUNCTION: Load terminal logs
	IN		:
	OUT		:nFirstInTime, the first EVENT time(start time)
	        :nLastOutTime, the last EVENT time(end time)
	RETRUEN	: TRUE,load success
	          FLASE,fail ,The model don't have simulation result 
	/************************************************************************/
	BOOL LoadTerminalLogs(long &nFirstInTime,long &nLastOutTime);
public:
	//according to LoadTerminalLogs
	BOOL LoadAirsideLogs(long &nFirstInTime,long &nLastOutTime);
	//according to LoadTerminalLogs
	BOOL LoadLandSideLogs(long &nFirstInTime,long &nLastOutTime);
protected:	
	
	BOOL BuildTempTracerData();
	BOOL BuildTempAirsideTracerData();
	BOOL BuildTempVehicleTracerData();
	
	void Cleanup();
	
	BOOL InitializeDefault();
	BOOL InitializeProject();
	
	void InitActiveFloorCB();
	
	//void OnAnimationStartRecord();
	//void OnAnimationStopRecord();
	
	//{{AFX_MSG(CTermPlanDoc)
	afx_msg void OnProjectBackup();
	afx_msg void OnProjectRestore();
	afx_msg void OnUpdateCamDolly(CCmdUI* pCmdUI);
	afx_msg void OnCamDolly();
	afx_msg void OnCamPan();
	afx_msg void OnUpdateCamPan(CCmdUI* pCmdUI);
	afx_msg void OnCamTumble();
	afx_msg void OnUpdateCamTumble(CCmdUI* pCmdUI);
	afx_msg void OnLayoutLock();
	afx_msg void OnUpdateLayoutLock(CCmdUI* pCmdUI);
	afx_msg void OnDistanceMeasure();
	afx_msg void OnUpdateDistanceMeasure(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewNamedViews3D(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewNamedViews2D(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCtxCopyProcessor(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCtxProcDispProperties(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCtxRotateProcessors(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCtxScaleProcessors(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCtxScaleProcessorsX(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCtxScaleProcessorsY(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCtxScaleProcessorsZ(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCtxMoveProcessors(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCtxRotateShape(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCtxMoveShape(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAnimationStart(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAnimationStop(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAnimationPause(CCmdUI* pCmdUI);
	afx_msg void OnUpdateActivateFloor(CCmdUI* pCmdUI);
	afx_msg void OnActivateFloor();
	afx_msg void OnAnimationStart();
	afx_msg void OnAnimationStop();
	afx_msg void OnAnimationPause();
	afx_msg void OnAnimationPlayF();
	afx_msg void OnAnimationPlayB();
	afx_msg void OnAnimationPickTime();
	afx_msg void OnCtxNewArea();
	afx_msg void OnCtxNewPortal();
	afx_msg void OnCtxEditArea();
	afx_msg void OnCtxEditPortal();
	afx_msg void OnCtxDeleteArea();
	afx_msg void OnCtxDeletePortal();
	afx_msg void OnCtxAreaColor();
	afx_msg void OnCtxPortalColor();
	afx_msg void OnCTXHelp();
	afx_msg void OnNewFloor();
	afx_msg void OnFloorProperties();
	afx_msg void OnGridVisible();
	afx_msg void OnFloorMonochrome();
	afx_msg void OnFloorVisible();
	afx_msg void OnSetDefaultAreaColor();
	afx_msg void OnSetDefaultPortalColor();
	afx_msg void OnSetDefaultProcDispProperties();
	afx_msg void OnCtxProcDispPropOverride();
	afx_msg void OnUpdateCtxNewFloor(CCmdUI* pCmdUI);
	afx_msg void OnFileSaveas();
	afx_msg void OnUpdateTracersOn(CCmdUI* pCmdUI);
	afx_msg void OnTracersOn();
	afx_msg void OnUpdateCtxMoveshapeZ(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCtxMoveshapeZ0(CCmdUI* pCmdUI);
	//}}AFX_MSG

	afx_msg void OnUpdateFloorsDelete(CCmdUI* pCmdUI);

	afx_msg void OnUpdateAnimationSetSpeed(CCmdUI* pCmdUI);
	afx_msg void OnUpdateProcProperties(CCmdUI* pCmdUI);
	
	afx_msg void OnUpdateViewRadar(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewSharpen(CCmdUI* pCmdUI);
	

	afx_msg void OnAlignFloors();
	afx_msg void OnRemoveMarker();
	afx_msg void OnUpdateAlignFloors(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRemoveMarker(CCmdUI* pCmdUI);
	
	afx_msg void OnDefineVisibleRegions();
	
	afx_msg void OnManageBMViewpoints();
	
	afx_msg void OnTracerParamsOn(UINT nID);
	afx_msg void OnTracerParamsColor(UINT nID);
	afx_msg void OnTracerParamsDensity(UINT nID);
	afx_msg void OnUpdateTracerParamsOn(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTracerParamsColor(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTracerParamsDensity(CCmdUI* pCmdUI);
	afx_msg void OnTagsParamsOn(UINT nID);
	afx_msg void OnUpdateTagsParamsOn(CCmdUI* pCmdUI);
	
	afx_msg void OnUpdateRunDelta(CCmdUI* pCmdUI);
	afx_msg void OnRunDelta();
	
	afx_msg void OnTrackersMovie();
	afx_msg void OnTrackersWalkthrough();
	afx_msg void OnTrackersPanorama();
	afx_msg void OnUpdateTrackersMovie(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTrackersWalkthrough(CCmdUI* pCmdUI);
	afx_msg void OnUpdateTrackersPanorama(CCmdUI* pCmdUI);
	
	afx_msg void OnFireImpact();
	afx_msg void OnUpdateFireImpact(CCmdUI* pCmdUI);
	
	afx_msg void OnAnimationRecord();
	afx_msg void OnUpdateAnimationRecord(CCmdUI* pCmdUI);
	afx_msg void OnAnimationRecsettings();
	
	afx_msg void OnOpaqueFloor();
	afx_msg void OnShowHideMap();
	afx_msg void OnFloorThickness();
	
	afx_msg void OnShowAllFloors();
	afx_msg void OnShowAllFloorMaps();
	afx_msg void OnShowAllFloorGrids();
	afx_msg void OnHideAllFloors();
	afx_msg void OnHideAllFloorMaps();
	afx_msg void OnHideAllFloorGrids();
	afx_msg void OnAllFloorsThickness();
	afx_msg void OnAllFloorsMonochrome();
	
	afx_msg void OnClickInfoToolBtn();
	afx_msg void OnUpdateInfoToolBtn(CCmdUI* pCmdUI);
	
	afx_msg void OnHideProcTags();
	afx_msg void OnHideACTags();
	afx_msg void OnAnimScaleSize();
	afx_msg void OnUpdateAnimScaleSize(CCmdUI* pCmdUI);
	afx_msg void OnUpdateHideProcTags(CCmdUI* pCmdUI);
	afx_msg void OnUpdateHideACTags(CCmdUI* pCmdUI);
	afx_msg void OnHideTaxiwayProc();
	afx_msg void OnUpdateHideTaxiwayProc(CCmdUI* pCmdUI);

public:
	afx_msg void OnCtxProcDisplayProperties();
//	afx_msg void OnCtxTaxiwayDirectionDefine();
	
	DECLARE_MESSAGE_MAP()
		
	///////////old code version//////////////////////
protected:
		bool CopyProcessorToDest(CProcessor2* _pProc, CTermPlanDoc* _pDestDoc, const int _nFloor);
	bool m_bJustAfterCopyProc;
public:
	bool IsJustAftCopyProc(){ return m_bJustAfterCopyProc; }
	void SetJustAftCopyProc(bool _b=true){ m_bJustAfterCopyProc = _b; }
	bool CopyProcessors(CTermPlanDoc* _pDestDoc, const int _nFloor);
public:
	void ShrinkDatabase(CMasterDatabase*pMasterDatabase, const CString& strProjName);
	CFloors& GetFloorByMode( EnvironmentMode mode );
	CFloors& GetCurModeFloor();
	const CFloors& GetCurModeFloor()const;
	const CFloors& GetFloorByMode( EnvironmentMode mode )const;
	CSurfaceMaterialLib& GetSurfaceMaterialLib();
	CCameras& GetCurModeCameras();
	CCameras& GetCamerasByMode( EnvironmentMode mode );
	void GetAllCameras(std::vector<CCameras *>& vCamera);

	// get cameras in render view mode
	C3DViewCameras& GetCurModeRenderCameras();
	C3DViewCameras& GetRenderCamerasByMode( EnvironmentMode mode );

	// save render cameras
	void SaveCurModeRenderCameras();
	void SaveAllCameras();
	void ReadAllCameras(); // read all cameras from database, if not found, translate from old format
	void ReadRenderCamerasByMode( EnvironmentMode envMode );

	inline EnvironmentMode GetCurrentMode()const{ return m_systemMode; }
 
	CPlacement* GetCurrentPlacement(EnvironmentMode mode = EnvMode_Unknow);
	int GetDrawFloorsNumberByMode(EnvironmentMode mode);
	EnvironmentMode GetModeByFloorsNumber( int szFloorNum );
	int GetAbsFloorIndexByMode(int floorcount, EnvironmentMode mode); //Get abs floor index by floor count and mode.
	
	CStructureList & GetCurStructurelist();
	CStructureList & GetStructurelistByMode(EnvironmentMode mode);
	WallShapeList & GetCurWallShapeList(){	return GetWallShapeListByMode(m_systemMode);}
	WallShapeList & GetWallShapeListByMode(EnvironmentMode mode);
	
	ProcessorList& GetProcessorList(EnvironmentMode mode = EnvMode_Unknow);	
	
	bool UpdateStructureChange(bool bRefreshNodeView=false);
	bool UpdateWallShapeChange();
	bool InitializeAirside(BOOL bOpen);

	//AirsideInput& GetAirsieInput()
	//{
	//	return m_airsideInput;
	//}
	//
	
	BOOL m_bShowAirsideNodes;
	BOOL m_bShowAirsideNodeNames;
	SelectableSettings m_SelectSettings;


	void UpdateDeiceStandNode();
	afx_msg void OnEditInvisibleregions();

	HMENU GetDefaultMenu();
	afx_msg void OnCtxLanddeleteproc();
	afx_msg void OnUpdateCtxLanddeleteproc(CCmdUI *pCmdUI);
	afx_msg void OnCtxSmoothCtrlpoints();
	afx_msg void OnCtxEditCtrlpoints();

	afx_msg void OnUpdatePlaceFloors(CCmdUI* pCmdUI) ;
	// remove objects
//new design
	int GetProjectID();
	
	int m_nCurrentAirport;
	int m_nDefaultAirport;
	void SetCurrentAirport(int nAirportID);
	int GetCurrentAirport();
	void DeleteALTObjects(ALTObjectList& vObjList);
	
	ALTObject * m_pTempObj;
	void SetTempObject(ALTObject * pObj);
	ALTObject * GetTempObject()const { return m_pTempObj; } 

	CDlgPanorama * m_pDlgPanorama;
	 //void ExportAirsideDBDataIntoFiles(const CString& strProjName);
	 //void ImportAirsideFilesIntoDB(const CString& strProjName);

	void LoadPaxDisplay();
public:
	
	afx_msg void OnPlaceMarker();
	afx_msg void OnRemoveAlignLine() ;
	afx_msg void OnUpdateRemoveAlignLine(CCmdUI* pCmdUI);

	void DeletePipe(int nPipeIndex);
	void DeleteWallShapeObject(int nIndex);
	void DeleteStructueObject(int nIndex);
	void DeleteAreaObject(int nIndex);
	void DeletePortalObject(int nIndex);

	CCalloutDlgLineData* GetCallOutLineData();

public:
	//void  GetCurrentLayerAlignLine(ARCLine& Line) ;
	//chloe

	//DocAdapter* m_doc_adapter;
	//void UpdateAllViewEx(void* view, UINT message, WPARAM wParam, LPARAM lParam);
	void CTermPlanDoc::SaveUndoProject(CString& pro_path) ;

	CView* GetSubView(CRuntimeClass *pViewClass) ;

	void UpdateAllFloorsInRender3DView();
	void UpdateFloorInRender3DView(CRenderFloor* pFloor);
	void ActiveFloorInRender3DView(CRenderFloor* pFloor);
	
	void UpdateRender3DObject(const CGuid& guid, const GUID& cls_guid) const;
	template <class DataType>
		void UpdateRender3DObject(DataType* pData) const
	{
		UpdateRender3DObject(pData->getGuid(), DataType::getTypeGUID());
	}

protected:
	bool OpenProjectDBForNewProject(CAirportDatabase* _database) ;
	//compatible
	bool loadOldDatabaseFroNewProject(CAirportDatabase* pAirportDatabase);
	void CTermPlanDoc::CopyAircraftFile(CString _path,CString courseDir) ;

protected:
	void CheckTheFlightActype() ;
	BOOL StartAnimView();

public:
	CalloutDispSpecs m_calloutDispSpecs;
	CCallOutDataManager m_CallOutManger ;

	// offer two methods helping load and find cameras' menu
	static void LoadSavedRenderCamerasIntoCmdUI( CCmdUI* pCmdUI, const C3DViewCameras &cams, CCameraData::PROJECTIONTYPE pt );
	static C3DViewCameraDesc& GetRenderCameraDescByIndex(C3DViewCameras& cams, size_t nIndex, CCameraData::PROJECTIONTYPE pt);
	static CViewDesc& GetCameraDescByIndex(CCameras& cams, size_t nIndex, CCameraData::PROJECTIONTYPE pt);
	static void LoadSavedCamerasIntoCmdUI( CCmdUI* pCmdUI, const CCameras& cams, CCameraData::PROJECTIONTYPE pt );
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // include guard
