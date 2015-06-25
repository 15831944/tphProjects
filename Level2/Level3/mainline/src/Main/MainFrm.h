// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__5D301067_30BE_11D4_92F6_0080C8F982B1__INCLUDED_)
#define AFX_MAINFRM_H__5D301067_30BE_11D4_92F6_0080C8F982B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ShapesBar.h"
#include "ShapesBarPFlow.h"
#include "ViewBar.h"
#include "MainBar.h"
#include "ProjectBar.h"
#include "AnimationBar.h"
#include "UnitBar.h"
#include "UnitBar1.h"
#include "CameraBar.h"
#include "LayoutBar.h"
#include "CompRepLogBar.h"
#include "PipeBar.h"
#include "AirRouteBar.h"
#include "XPStyle/StatusBarXP.h"
#include "XPStyle/NewMenu.h"
#include "OnBoardLayoutBar.h"
#include "AircarftModelBar.h"
#include "ACComponentShapesBar.h"
#include "ACFurnishingShapesBar.h"

class C3DView;
class CTermPlanDoc;
class CDlgFloorAdjust;
class CDlgAircraftModelEdit;

class CAirportDatabase;
class CArcTermDockState ;
class CMainFrame : public CNewMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();
	
// Attributes
public:
	BOOL m_bCanPasteProcess;
// Operations
public:
	void SaveState();
	void LoadState();
	void saveMainBarComboBoxToDB() ;
	void DestroyFloorAdjustDlg();
	void SetMainBarComboBox(bool bEnable = true, bool bSetContentDefault = true);
	void Set3DChartBarComboBox(bool bEnable = true, bool bSetContentDefault = true);
	void DeleteItem(const CString& strName);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL DestroyWindow();
	//}}AFX_VIRTUAL
     virtual void OnUpdateFrameMenu (HMENU hMenuAlt);

// Implementation
	public:
	void UpdateProjSelectBar();
	void UpdateActiveFloorCB();
	CMDIChildWnd* CreateOrActivateFrame(CDocTemplate* pTemplate, CRuntimeClass* pViewClass, bool* pbAlreadyExist = NULL);
	CMDIChildWnd* CreateOrActive3DView(bool* pbAlreadyExist = NULL);
	CMDIChildWnd* CreateOrActiveRender3DView(bool* pbAlreadyExist = NULL);
	CMDIChildWnd* CreateOrActiveEnvModeView(bool* pbAlreadyExist = NULL);
	//C3DView* Get3DView();
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:  // control bar embedded members
	BOOL zipFloder(const CString& strProjName, const CString& _strSrcFolder, CString& _strTempZipFile);
	void ChangeSize(const CRect rcWnd, const CRect rcClient, UINT nIndex = 0);
	void ExportAsInputZip(const CString& strProjName, const CString& strProjPath,bool bNotForceDB,bool bOpenProject = true);
	void ExportAndEmail(const CString& strProjName,const CString& strProjPath,bool bNotForceDB,bool bOpenProject = true);
	CComboBox* GetAnimPaxCB();
	BOOL CreateTempBar();
	BOOL CreatePipeBar();
	BOOL CreateAirRouteBar();
	BOOL CreateUnitBar();
	BOOL CreateUnitTempBar();
	BOOL CreateAnimationBar();
	BOOL CreateProjectBar();
	BOOL CreateMainBar();
	BOOL CreateShapesBar();
	//BOOL CreateACComponentBar();
	BOOL CreateAircraftModelBar();
	//void UpdateUnitBar(CTermPlanDoc* pDoc);
	//void UpdateMainFrameBar(CTermPlanDoc* pDoc);
	void AdjustToolbar(void);
	void AdjustMenu();
	void LoadDefaultMenu();
	void CreateCompareReportAndResultView(const CString& strName = _T(""), const CString& strDesc = _T(""));

	CStatusBarXP  m_wndStatusBar;
	CShapesBarPFlow m_wndShapesBarPFlow;
	CShapesBar*	m_wndShapesBar;
	CACComponentShapesBar m_wndACComponentShapesBar;
	CACFurnishingShapesBar m_wndACFurnishingShapesBar;
	CCompRepLogBar m_wndCompRepLogBar;
	CAnimationBar m_wndAnimationBar;
	CMainBar    m_wndToolBar;		//Common toolbar used in windows software -- "new, open, save, print_setup, help_about";
	CProjectBar m_wndProjectBar;
	CUnitBar     m_wndUnitBar;
	CUnitBar1    m_wndUnitBarTemp;
	CCameraBar	m_wndCameraBar;
	CLayoutBar	m_wndLayoutBar;
	CPipeBar	m_wndPipeBar;
	//CAirRouteBar m_wndAirRouteBar;
	//CACComponentModelBar m_wndOnBoardLayoutBar;
	CAircarftModelBar m_wndAircraftModel;
// 	CDlgAircraftModelEdit* m_pDlgAircraftModel;
	int m_nOnSizeType;

	void ShapesBarPFlowMsg(BOOL bShow);
	//temp toolbar for menu icon
	CToolBar m_wndTempBar;

	std::vector<CToolBar*> m_vToolBars;

	CImageList m_ilCameraTools;
	CImageList m_ilCameraTools_G;
	CImageList m_ilCameraTools_W;
	CImageList m_ilAnimTools;
	CImageList m_ilAnimTools_G;
	CImageList m_ilAnimTools_W;
	CImageList m_ilSpeedTools;
	CImageList m_ilLayoutTools;
	CImageList m_ilLayoutTools_G;
	CImageList m_ilLayoutTools_W;
	void OnShowProcessDefine();
		


// Generated message map functions
protected:
	CDlgFloorAdjust* m_pDlgFloorAdjust;

	void LoadReportProcessFlow(enum ENUM_REPORT_TYPE _enumRepType);
	void LoadReport(enum ENUM_REPORT_TYPE _enumRepType );

	//will be removed
	void LoadAirsideReport(enum reportType airsideRpType);
	
	void LoadAirsideReport_New(enum reportType airsideRpType);

	//onboard report
	void LoadOnboardReport(enum OnboardReportType _enumRepType );
	
	BOOL CreateCameraBar();
	BOOL CreateLayoutBar();
	BOOL CreateCompRepLogBar();
	BOOL CreateACComponentShapesBar();
	BOOL CreateACFurnishingShapesBar();
	BOOL CreateShapesBarPFlow();
	void DockControlBarLeftOf(CToolBar* pBar, CToolBar* pLeftOf);
	BOOL VerifyBarState(LPCTSTR lpszProfileName);

	bool zipProjectDB( const CString& strProjName, CString& _strTempDBZipFile );
	bool zipTempZipToTermzip( const CString& strProjName, const CString& strProjPath,
		const CString& _strTempInputZip, const CString& _strTempDBZip, 
		const CString& _strTempMathematicZip, const CString& _strTempMathResultZip,
		const CString& _strTerminalZip,bool bNotForceDB,CTermPlanDoc* _pTermDoc=NULL );
	void createArcExportIniFile( const CString& _strTempInputZip, const CString& _strTempDBZip, 
		const CString& _strTempMathematicZip, const CString& _strTempMathResultZip,
		CString& _strTmpIniFile ,bool bNotForceDB);
	void clear2VArray( char** _p2VArray, int _xCount );
	CString getTempPath( const CString& _strDefault = "c:\\" );

	void ZipAirportInfo(const CString& strProjName,CString projPath,std::vector<CString> &vAirportName);

	void ZipAirsideExportFiles(const CString& strProjName,const CString& projPath, CString& strZipFilePath);
	BOOL zipFloder(const CString& strProjName, const CString& _strSrcFolder,std::vector<CString> vAddFile,CString& _strTempZipFile);

	void AdjustMSV(CDocument* pTermDoc);
	CMDIChildWnd* GetNodViewFrame(CRuntimeClass *pViewClass);

//	void OnMessageView2dterminalview();
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnView3dterminalview();
	afx_msg void OnMessageChangeEnvView();
	afx_msg void OnView2dterminalview();
	afx_msg void OnView3DDefaultView();
	afx_msg void OnView2DDefaultView();

	afx_msg void OnViewDetailview();
//	afx_msg void OnViewOnBoardLayoutView();
	afx_msg	void OnDatabaseAircraftComponents();
	afx_msg void OnClose();
	//chloe
	afx_msg void OnDestroy();
	//
	afx_msg void OnDatabaseAirports();
	afx_msg void OnDatabaseProbDists();
	afx_msg void OnReportsPassengerActivitylog();
	afx_msg void OnDatabaseShapes();
	afx_msg void OnReportsProcessorUtilization();
	afx_msg void OnDatabaseACTypes();
	afx_msg void OnDatabaseACTypeModels();
	afx_msg void OnDatabaseACFurnishings();
	afx_msg void OnReportsPassengerDistancetravelled();
	afx_msg void OnReportsSpaceOccupancy();
	afx_msg void OnDatabasesAirlines();
	afx_msg void OnReportsPassengerTimeinqueues();
	afx_msg void OnReportsPassengerNumber();
	afx_msg void OnReportsProcessorThroughput();
	afx_msg void OnReportsProcessorQueuelength();
	afx_msg void OnReportsProcessorCriticalqueueparameters();
	afx_msg void OnReportsPassengerTimeInService();
	afx_msg void OnReportsPassengerTimeinTerminal();
	afx_msg void OnReportsBaggageBaggagecount();
	afx_msg void OnReportsBaggageBaggagewaitingtime();
	afx_msg void OnReportsSpaceDensity();
	afx_msg void OnReportsPassengerActivitybreakdown();
	afx_msg void OnAnimationActivitydensitydisplay();
	afx_msg void OnAnimationMobileelementdisplay();
	afx_msg void OnAnimationFlightInfoDisplay();
	afx_msg void OnAnimationTrafficcountdisplay();
	afx_msg void OnProjectDelete();
	afx_msg void OnProjectUndelete();
	afx_msg void OnProjectPurge();
	afx_msg void OnReportsInputdata();
	afx_msg void OnReportsSpaceThroughput();
	afx_msg void OnReportsSpaceCollisions();
	afx_msg void OnReportsEconomic();
	afx_msg void OnReportsBaggageDeliverytimes();
	afx_msg void OnReportsBaggageDeparturedistribution();
	afx_msg void OnReportsAircraftoperations();
	afx_msg void OnUpdateAnimationActivitydensitydisplay(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAnimationMobileelementdisplay(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAnimationFlightInfoDisplay(CCmdUI* pCmdUI);
	afx_msg void OnWindowSidebyside();
	afx_msg void OnUpdateWindowSidebyside(CCmdUI* pCmdUI);
	afx_msg void OnProjectUndo();
	afx_msg void OnProjectOptions();
	afx_msg void OnProjectComment();
	afx_msg void OnProjectControl();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnRenderOptions();
	afx_msg void OnBarShowMain();
	afx_msg void OnUpdateBarShowMain(CCmdUI* pCmdUI);
	afx_msg void OnBarShowAnimation();
	afx_msg void OnUpdateBarShowAnimation(CCmdUI* pCmdUI);
	afx_msg void OnBarShowProject();
	afx_msg void OnUpdateBarShowProject(CCmdUI* pCmdUI);
	afx_msg void OnBarShowCamera();
	afx_msg void OnUpdateBarShowCamera(CCmdUI* pCmdUI);
	afx_msg void OnBarShowShape();
	afx_msg void OnBarShowACComponentShape();
	afx_msg void OnBarShowACFurnishingShape();
	afx_msg void OnUpdateBarShowShape(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBarShowACComponentShape(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBarShowACFurnishingShape(CCmdUI* pCmdUI);
	afx_msg void OnBarShowUnit();
	afx_msg void OnUpdateBarShowUnit(CCmdUI* pCmdUI);
	afx_msg void OnBarShowLayout();
	afx_msg void OnUpdateBarShowLayout(CCmdUI* pCmdUI);
	afx_msg void OnBarShowPipe();
	afx_msg void OnUpdateBarShowPipe(CCmdUI* pCmdUI);
	afx_msg void OnButPipebar();
	afx_msg void OnUpdateButPipebar(CCmdUI* pCmdUI);
	afx_msg void OnHelp();
	afx_msg void OnHelpIndex();
	afx_msg void OnHelpTechSupport();
	afx_msg void OnCompareReport(UINT nID);
	afx_msg void OnProjectExportAsInputzip();
	afx_msg void OnProjectExportAndEmail();
	afx_msg void OnProjectExportAsInputzipWithoutDB();
	afx_msg void OnProjectExportAndEmailWithoutDB();
	afx_msg void OnProjectImportIntoOldProject();
	afx_msg void OnProjectImportIntoNewProject();
	afx_msg void OnUpdateDatabaseMenu(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRenderOptions(CCmdUI* pCmdUI);
	afx_msg void OnDatabaseDblistman();
	afx_msg void OnUpdateDatabaseDblistman(CCmdUI* pCmdUI);
	afx_msg void OnReportsPassengerFireevacution();
	afx_msg void OnReportsConveyorWaitLength();
	afx_msg void OnReportsPassengerConveyorWaitTime();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnComparativereportopen();
	afx_msg void OnComparativereportnew();
	afx_msg void OnComparativerun();
	afx_msg void OnUpdateComparativerun(CCmdUI* pCmdUI);
	afx_msg void OnCompareWindowSidebyside();
	afx_msg void OnUpdateCompareWindowSidebyside(CCmdUI* pCmdUI);
	afx_msg void OnBarShowShapemath();
	afx_msg void OnUpdateBarShowShapemath(CCmdUI* pCmdUI);
	afx_msg void OnViewMathflowview();
	afx_msg void OnReportsMathqtime();
	afx_msg void OnReportsMathqlength();
	afx_msg void OnReportsMaththroughput();
	afx_msg void OnReportsMathpaxcount();
	afx_msg void OnReportsMathutilization();
	afx_msg void OnBarShowRunlog();
	afx_msg void OnUpdateBarShowRunlog(CCmdUI* pCmdUI);
	afx_msg void OnReportsPassengerMissflight();
	afx_msg void OnReportsDuration();
	afx_msg void OnRptAirsideDistTrv();
	afx_msg void OnRptAirsideFltDelay();
	afx_msg void OnRptAirsideObstructions();
	afx_msg void OnReportRetailReport();
	//}}AFX_MSG

	afx_msg BOOL OnViewNamedViews3D(UINT nID);
	afx_msg BOOL OnViewNamedViews2D(UINT nID);
	afx_msg void OnSelectSelectProject();
	afx_msg void OnDropDownSelectProject();
	//afx_msg void OnSelectActiveFloor();
	//afx_msg void OnDropDownActiveFloor();
	afx_msg LRESULT OnCabinSelChanged(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSLBSelChanged(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT ONAIRSelChanged(WPARAM wParam,LPARAM lParam);
	afx_msg void OnAnimationSetSpeed(UINT nID);
	afx_msg LRESULT OnDlgFallback(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnOnboardLayoutEditDlgFallback(WPARAM wParam, LPARAM lParam);
	afx_msg void OnUpdateAnimButtons(CCmdUI* pCmdUI);
	afx_msg LRESULT OnSetAnimationSpeed(WPARAM wParam, LPARAM lParam);
	afx_msg void OnShowShapesToolbar();
	afx_msg void OnShowACComponentShapesToolbar();
	afx_msg void OnUpdateShowShapesToolbar(CCmdUI* pCmdUI);
	afx_msg void OnFloorAdjust();
	afx_msg void OnModelEdit();
	afx_msg void OnUpdateFloorAdjust(CCmdUI* pCmdUI);
// 	afx_msg void OnUpdateModelEdit(CCmdUI* pCmdUI);
	afx_msg LRESULT OnShowControlBar(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnChangeAnimToolBtnState(WPARAM wParam,LPARAM lParam);
	afx_msg void OnChangeShape(UINT nID);

	afx_msg LRESULT OnSetToolBtnRunDelta_Run(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnSetToolBtnRunDelta_Delta(WPARAM wParam,LPARAM lParam);

	afx_msg LRESULT OnSendExportZipToARC(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnSendExportEmailToARC(WPARAM wParam,LPARAM lParam);
	//afx_msg void OnViewDeleteNamedView();
	//afx_msg void OnViewToggle2D3D();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnDropDownUnitBarArrow(void);

	afx_msg void OnNoOpenProjectExportAsZipFile();
	afx_msg void OnNoOpenProjectExportAsInputAndEmail();
	afx_msg void OnUpdateACTypeModels(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAircraftComponents(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDatabaseFurnishings(CCmdUI* pCmdUI);
	afx_msg void OnMobileElementsDiagnosis();
	afx_msg LRESULT OnSystemCrashClose(WPARAM wParam,LPARAM lParam);

public:
		bool m_bViewOpen;

private:
	bool ExportProjDBInfo(CAirportDatabase*  pDB,const CString& sDestPath,CString& sFullProjInfoFile);


public:
	afx_msg void OnBillboardExposureincidence();
	afx_msg void OnBillboardIncrementalvisit();
	afx_msg void OnLogSetting();
	afx_msg void OnLogViewselect();
	afx_msg void OnLogClearlog();
	afx_msg void OnReportAirsideActivity();
	//afx_msg void OnReportAirsideOperation();
	afx_msg void OnReportAirsideOperational();
	afx_msg void OnReportAirsideNodeDelay();

	//vehicle report 
	afx_msg void OnVehicleReportForOperate();

	afx_msg void OnTaxiwayReportOperate();
	afx_msg void OnTaxiwayUtilizationReport();
	afx_msg void OnRunwaysDelayReport();
	afx_msg void OnRunwayCrossingsReport();
	afx_msg void OnStandOperationsReport();
	afx_msg void OnTakeoffProcessReport();
	afx_msg void OnRunwaysOperationsReport();
	afx_msg void OnRunwaysUtilizationReport();
	afx_msg void OnFlightConflictReport();
	afx_msg void OnAirsideIntersectionOperationReport();
	afx_msg void OnAirsideControllerWorkloadReport();
	afx_msg void OnRunwayExitReport();
	afx_msg void OnFlightFuelBurnReport();
	afx_msg void OnAircraftDoorSetting();
	afx_msg void OnAircraftTypeRelationShipSetting();
	afx_msg void OnLoadDefaultAirportDB();
	afx_msg void OnCommitToDefaultAirportDB() ;


	//Onboard Report
	afx_msg void OnOnboardReportPaxActivityReport();
	afx_msg void OnOnboardReportPaxDurationReport();
	afx_msg void OnOnboardReportPaxDistanceTravelReport();
	afx_msg void OnOnboardReportPaxCarryonReport();

	afx_msg void OnOnboardReportPaxConflictReport();
	afx_msg void OnOnboardReportCarryonReport();

public:
	void getCurrentEnviro(EnvironmentMode& enviro , SimulationType& sim_type);
	CMainBar& GetMainBarTool() {return m_wndToolBar ;} ;

protected:
		void LoadBarState(LPCTSTR lpszProfileName) ;
		void SaveBarState(LPCTSTR lpszProfileName) ;
		void GetDockState(CArcTermDockState* state) ;
        void SetDockState(const CArcTermDockState* state) ;

		void UpdateMainframeMenu(EnvironmentMode envMode);

		void LoadLandsideReport( enum LandsideReportType _enumRepType );

public:
	afx_msg void OnLandsideVehicleActivity();
	afx_msg void OnLandsideVehicleDelay();
	afx_msg void OnLandsideVehicleTotalDelay();

	afx_msg void OnLandsideResourceVehicleQueueReport();
	afx_msg void OnLandsideResourceVehicleThroughputReport();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__5D301067_30BE_11D4_92F6_0080C8F982B1__INCLUDED_)
