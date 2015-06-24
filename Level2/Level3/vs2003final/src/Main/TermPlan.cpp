// TermPlan.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "TermPlan.h"
#include "version.h"
#include "MainFrm.h"
#include "ChildFrm.h"
#include "ChildFrameSplit.h"
#include "ReportChildFrameSplit.h"
#include "DensityGLView.h"
#include "ChildFrameDensityAnim.h"
#include "CFIDSFrameView.h"
#include "CFIDSView.h"
#include "TermPlanDoc.h"
#include "./MFCExControl/ARCBaseTree.h"
#include "NodeView.h"
#include "3DView.h"
#include "RepListView.h"
#include "MultiRunReportSplitChildFrame.h"
#include "MultiRunRepListView.h"
#include "ProjectSelectDlg.h"
#include "common\ProbDistManager.h"
#include "common\airportdatabaselist.h"
#include "InputdataView.h"
#include "FrameInputdata.h"
#include "ProcessDefinedlg.h"
#include "FrameProcessDefine.h"
#include <atlbase.h>
#include "FrameCompareReport.h"
#include "FrameCompareReportResult.h"
#include "CompareReportDoc.h"
#include "ProcessFlowView.h"
#include "MathematicView.h"
#include "MathematicFrame.h"
#include "XPStyle/NewMenu.h"
#include "AirsideReportView.h"
#include "CompareReportView.h"
#include "../InputAirside/ALTAirport.h"
#include "NodeViewFrame.h"
#include "AirsideMSView.h"
#include "../common/EchoSystem.h"
//#include "AirsideRepControlView.h"
#include "AirsideReportListView.h"
#include "AirsideReportChildFrameSplit.h"
#include "../InputAirside/GlobalDatabase.h"
#include "../InputAirside/DatabaseFatalError.h"
//#include "OnBoardLayoutFrame.h"
#include "OnBoardLayoutView.h"

#include "DlgGateTypedefine.h"
#include "MainBarUpdateArgs.h"

//#include "../Onboard3d/3DAppEngine.h"
//#include "Onboard/ComponentModelEditView.h"
//#include "Onboard/AircraftModelDatabaseView.h"
//#include "Onboard/ComponetSectionEditView.h"
//#include "Onboard/OnboardStationCommentView.h"
//#include "Onboard/OnBoardAircraftModelComman.h"
//#include "./OnBoardAircraftModelFrame.h"
#include "../Common/ARCTermSettingHander.h"
//#include "Onboard/AircraftLayoutView.h"
#include "LayoutView.h"
#include "LayOutFrame.h"
#include "AircraftFurning3DView.h"
#include "AircraftFurningFrame.h"
#include "LicenseExpiredDlg.h"
#include "HandleAirportDatabase.h"

//new onboard
#include "../Renderer/RenderEngine/RenderEngine.h"
#include "OnBoard/MeshEdit3DView.h"
#include "onboard/MeshEditFrame.h"
#include "onboard/ACModelEditFrame.h"
#include "onboard/ACModelEdit3DView.h"
#include "onboard/ACFurnishingEditFrame.h"
#include "onboard/ACFurnishingEdit3DView.h"
#include "onboard/AircraftLayout3DView.h"
#include "onboard/AircraftLayOutFrame.h"
#include "Render3DView.h"
#include "Render3DFrame.h"

#include <Renderer/RenderEngine/Shape3DMeshManager.h>
#include "../InputAirside/ProjectDatabase.h"

#include <Database/DatabaseInitializer.h>
#include <Common/OleDragDropManager.h>
#include <renderer\RenderEngine\Renderer.h>
#include <common\AircraftAliasManager.h>
#include "OnboardReportChildSplitFrame.h"
#include "OnboardReportListView.h"

#include "LandsideReportChildSplitFrame.h"
#include "LandsideReportListView.h"
#include "Common/ARCGUID.h"
#include "CmpReportChildFrameSplit.h"
#include "CmpReportGraphView.h"
//#ifndef _DEBUG
#define _SECURITY_ENABLED_
//#endif

#pragma warning( disable : 4996 )

/////////////////////////////////////////////////////////////////////////////
// CTermPlanApp

//define global to restore Levels(Max_Pax_Typer )`
int gLevels = 4;
// changed by matt 5,12,2004

BEGIN_MESSAGE_MAP(CTermPlanApp, CWinApp)
	//{{AFX_MSG_MAP(CTermPlanApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	//}}AFX_MSG_MAP
	// Standard file based Document commands
	//ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	//ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTermPlanApp construction

#include <Common/TmpFileManager.h>
#include "./ObjectFactoryInstance.h"

CTermPlanApp::CTermPlanApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance

	m_pMasterDatabase = NULL;
	//ObjectFactoryInstance::Init();
}

CTermPlanApp::~CTermPlanApp()
{

}

/////////////////////////////////////////////////////////////////////////////
// The one and only CTermPlanApp object

CTermPlanApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CTermPlanApp initialization

BOOL CTermPlanApp::InitInstance()
{
	
	if(!AfxOleInit())
	{
		AfxMessageBox(_T("AfxOleInit() failed."));
		return FALSE;
	}

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.

	ReadRegister();
	
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)


	ARCGUID::Init();
	//C3DAppEngine::GetInstane().SetRootPath(PROJMANAGER->GetDefaultDBPath());	//to be delete
	RenderEngine::getSingleton().SetResourcePath(PROJMANAGER->GetDefaultDBPath());

	//set main singleton object to rendererdll
	CRenderer::SetSingletonPtr(PROJMANAGER);
	CRenderer::SetSingletonPtr(ACALIASMANAGER);

	//m_nACTypeFormat = ::RegisterClipboardFormat(_T("ACType"));
	m_nShapeFormat = ::RegisterClipboardFormat(_T("ARCport Shape Format"));
	OleDragDropManager::GetInstance().SetRegisterClipboardFormat(m_nShapeFormat);

#ifdef _SECURITY_ENABLED_
	//check security
// 	if(SimulationControlLicense.GetProductLicenseSecondsRemaining() == 0) {
// 		AfxMessageBox("Your ARCport license has either expired or is not installed.\n"
// 	                "Please contact ARC technical support immediately for help\n"
// 	                "installing a license.");
// 		return FALSE;
// 	}

	// check the license
	CTermPlanApp::CheckLicense(TRUE);


	//Get Simulation Control
	SimulationControlLicense.GetSimualtionControl();
#endif

	try
	{
		CGlobalAirportDataBase::OpenGoabalAirportDataBase(PROJMANAGER->GetAppPath()) ;
	//	HandleAirportDatabase::updateShareTemplateDatabase();
		//AIRPORTDBLIST;
		ConnectToMasterDatabase();
	}
	catch( FileVersionTooNewError* _pError )
	{
		char szBuf[128];
		_pError->getMessage( szBuf );
		AfxMessageBox( szBuf, MB_OK|MB_ICONWARNING );
		delete _pError;			
		return FALSE;
	}

	CString pString = PROJMANAGER->GetAppPath();
	// Read Levels from Config.ini
	int nRet = GetPrivateProfileInt("Levels","Max_Pax_Type",4, pString+"\\Config.ini");
	if(nRet>0)  gLevels = nRet;

	// init temporary file manager
	//CTmpFileManager::GetInstance().SetBaseFolder(pString);
	DatabaseInitializer::SetTmpFileBaseFolder(pString);

	//
	VERIFY(CShape3DMeshManager::GetInstance().Reload(pString));


	// Register the application's Document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.
	AfxInitRichEdit();
    
	m_pDetailTemplate = new CNewMultiDocTemplate(
		IDR_DETAILFRAME,
		RUNTIME_CLASS(CTermPlanDoc),
		RUNTIME_CLASS(MSV::CNodeViewFrame), // custom MDI child frame
		RUNTIME_CLASS(CNodeView));
	AddDocTemplate(m_pDetailTemplate);

	m_p3DTemplate = new CNewMultiDocTemplate(
		IDR_3DVIEWFRAME,
		//IDR_DETAILFRAME,
		RUNTIME_CLASS(CTermPlanDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(C3DView));
	AddDocTemplate(m_p3DTemplate);

	m_pNewRender3DTemplate = new CNewMultiDocTemplate(
		IDR_3DVIEWFRAME,
		//IDR_DETAILFRAME,
		RUNTIME_CLASS(CTermPlanDoc),
		RUNTIME_CLASS(CRender3DFrame), // custom MDI child frame
		RUNTIME_CLASS(CRender3DView));
	AddDocTemplate(m_pNewRender3DTemplate);

// 	m_p3DAirSideTemplate = new CNewMultiDocTemplate(
// 		IDR_3DVIEWFRAME,
// 		//IDR_DETAILFRAME,
// 		RUNTIME_CLASS(CTermPlanDoc),
// 		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
// 		RUNTIME_CLASS(C3DView));
// 	AddDocTemplate(m_p3DAirSideTemplate);
// 
// 	m_pNewRender3DAirSideTemplate = new CNewMultiDocTemplate(
// 		IDR_3DVIEWFRAME,
// 		//IDR_DETAILFRAME,
// 		RUNTIME_CLASS(CTermPlanDoc),
// 		RUNTIME_CLASS(CRender3DFrame), // custom MDI child frame
// 		RUNTIME_CLASS(CRender3DView));
// 	AddDocTemplate(m_pNewRender3DAirSideTemplate);

	m_pReportTemplate = new CNewMultiDocTemplate(
		IDR_REPORTVIEWFRAME,
		//IDR_DETAILFRAME,
		RUNTIME_CLASS(CTermPlanDoc),
		RUNTIME_CLASS(CReportChildFrameSplit), // custom MDI child frame
		RUNTIME_CLASS(CRepListView));
	AddDocTemplate(m_pReportTemplate);

	m_pAirsideReportTemplate_New = new CNewMultiDocTemplate(
		IDR_REPORTVIEWFRAME,
		//IDR_DETAILFRAME,
		RUNTIME_CLASS(CTermPlanDoc),
		RUNTIME_CLASS(CAirsideReportChildFrameSplit), // custom MDI child frame
		RUNTIME_CLASS(CAirsideReportListView));
	AddDocTemplate(m_pAirsideReportTemplate_New);

	m_pOnboardReportTemplate = new CNewMultiDocTemplate(
		IDR_ONBOARD_REPORT,
		//IDR_DETAILFRAME,
		RUNTIME_CLASS(CTermPlanDoc),
		RUNTIME_CLASS(OnboardReportChildSplitFrame), // custom MDI child frame
		RUNTIME_CLASS(OnboardReportListView));
	AddDocTemplate(m_pOnboardReportTemplate);


	m_pMultiRunReportTemplate = new CNewMultiDocTemplate(
		IDR_REPORTVIEWFRAME, 
		RUNTIME_CLASS(CTermPlanDoc),
		RUNTIME_CLASS(CMultiRunReportSplitChildFrame), 
		RUNTIME_CLASS(CMultiRunRepListView));
	AddDocTemplate(m_pMultiRunReportTemplate);

	m_pMathReportTemplate = new CNewMultiDocTemplate(
		IDR_MATHFRAME,
		//IDR_DETAILFRAME,
		RUNTIME_CLASS(CTermPlanDoc),
		RUNTIME_CLASS(CReportChildFrameSplit), // custom MDI child frame
		RUNTIME_CLASS(CRepListView));
	AddDocTemplate(m_pMathReportTemplate);

	m_pDensityAnimTemplate = new CNewMultiDocTemplate(
		IDR_3DVIEWFRAME,
		RUNTIME_CLASS(CTermPlanDoc),
		RUNTIME_CLASS(CChildFrameDensityAnim), // custom MDI child frame
		RUNTIME_CLASS(CDensityGLView));
	AddDocTemplate(m_pDensityAnimTemplate);

    // Flight information display view...
    m_pFIDSAnimTemplate = new CNewMultiDocTemplate(
        IDR_3DVIEWFRAME,
        RUNTIME_CLASS(CTermPlanDoc),
        RUNTIME_CLASS(CFIDSFrameView),
        RUNTIME_CLASS(CFIDSView));
    AddDocTemplate(m_pFIDSAnimTemplate);

	m_pInputDataTemplate = new CNewMultiDocTemplate(
		IDR_DETAILFRAME,
		RUNTIME_CLASS(CTermPlanDoc),
		RUNTIME_CLASS(CFrameInputData), // custom MDI child frame
		RUNTIME_CLASS(CInputDataView));
	AddDocTemplate(m_pInputDataTemplate);

	m_pCompareReportTemplate2 = new CNewMultiDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CCompareReportDoc),		// Document class
		RUNTIME_CLASS(CCmpReportChildFrameSplit),		// frame class
		RUNTIME_CLASS(CCompareReportView));//CCmpReportGraphView));		// view class
	AddDocTemplate(m_pCompareReportTemplate2);

// 	m_pProcessDefineTemplate = new CNewMultiDocTemplate(
// 		IDR_DETAILFRAME,
// 		RUNTIME_CLASS(CTermPlanDoc),
// 		RUNTIME_CLASS(CFrameProcessDefine), // custom MDI child frame
// 		RUNTIME_CLASS(CProcessDefineDlg));
// 	AddDocTemplate(m_pProcessDefineTemplate);
	
	m_pMathematicTemplate = new CNewMultiDocTemplate(
		IDR_MATHFRAME,
		RUNTIME_CLASS(CTermPlanDoc),
		RUNTIME_CLASS(CMathematicFrame), // custom MDI child frame
		RUNTIME_CLASS(CMathematicView));
	AddDocTemplate(m_pMathematicTemplate);

	m_pProcessFlowTemplate = new CNewMultiDocTemplate(
		IDR_MATHFRAME,
		RUNTIME_CLASS(CTermPlanDoc),
		RUNTIME_CLASS(CMathematicFrame), // custom MDI child frame
		RUNTIME_CLASS(CProcessFlowView));
	AddDocTemplate(m_pProcessFlowTemplate);

	m_pAirSideReportTemplate = new CNewMultiDocTemplate(
		IDR_AIRSIDE,
		//IDR_DETAILFRAME,
		RUNTIME_CLASS(CTermPlanDoc),
		RUNTIME_CLASS(CMDIChildWnd), // custom MDI child frame
		RUNTIME_CLASS(CAirsideReportView));
	AddDocTemplate(m_pAirSideReportTemplate);

	//OnBoardLayoutFrame view
	m_pOnBoardLayoutTemplate = new CNewMultiDocTemplate(
		IDR_ONBOARD,
		RUNTIME_CLASS(CTermPlanDoc),
		RUNTIME_CLASS(COnBoardMeshEditFrame),
		RUNTIME_CLASS(CSimpleMeshEditView));
	AddDocTemplate( m_pOnBoardLayoutTemplate);

	m_pAircraftModelDatabaseTermplate = new CNewMultiDocTemplate(
		IDR_ONBOARD,
		RUNTIME_CLASS(CTermPlanDoc),
		RUNTIME_CLASS(CACModelEditFrame),
		RUNTIME_CLASS(CACModelEdit3DView));
	AddDocTemplate( m_pAircraftModelDatabaseTermplate );	

	m_pAircraftFurnishingTermplate = new CNewMultiDocTemplate(
		IDR_ONBOARD,
		RUNTIME_CLASS(CTermPlanDoc),
		RUNTIME_CLASS(CACFurnishingEditFrame),
		RUNTIME_CLASS(CACFurnishingEdit3DView));
	AddDocTemplate( m_pAircraftFurnishingTermplate );	

	//aircraft layout edit frame
	m_pAircraftLayoutTermplate = new CNewMultiDocTemplate(
		IDR_ONBOARD,
		RUNTIME_CLASS(CTermPlanDoc),
		RUNTIME_CLASS(CAircraftLayOutFrame),
		RUNTIME_CLASS(CAircraftLayout3DView) ) ;
    AddDocTemplate(m_pAircraftLayoutTermplate) ;
	

	m_pLandsideReportTemplate = new CNewMultiDocTemplate(
		IDR_LANDSIDE,
		//IDR_DETAILFRAME,
		RUNTIME_CLASS(CTermPlanDoc),
		RUNTIME_CLASS(LandsideReportChildSplitFrame), // custom MDI child frame
		RUNTIME_CLASS(LandsideReportListView));
	AddDocTemplate(m_pLandsideReportTemplate);



	CNewMenu::SetMenuDrawMode(CNewMenu::STYLE_XP);
	
	m_pDetailTemplate->m_NewMenuShared.LoadToolBar(IDR_MAINFRAME); 
	m_p3DTemplate->m_NewMenuShared.LoadToolBar(IDR_MAINFRAME); 
	m_pNewRender3DTemplate->m_NewMenuShared.LoadToolBar(IDR_MAINFRAME); 
// 	m_p3DAirSideTemplate->m_NewMenuShared.LoadToolBar(IDR_MAINFRAME); 
// 	m_pNewRender3DAirSideTemplate->m_NewMenuShared.LoadToolBar(IDR_MAINFRAME); 
	m_pReportTemplate->m_NewMenuShared.LoadToolBar(IDR_MAINFRAME); 
	m_pDensityAnimTemplate->m_NewMenuShared.LoadToolBar(IDR_MAINFRAME); 
	m_pInputDataTemplate->m_NewMenuShared.LoadToolBar(IDR_MAINFRAME); 
	m_pCompareReportTemplate2->m_NewMenuShared.LoadToolBar(IDR_MAINFRAME);
	m_pMathematicTemplate->m_NewMenuShared.LoadToolBar(IDR_MAINFRAME); 

	//onboard
	m_pOnBoardLayoutTemplate->m_NewMenuShared.LoadToolBar(IDR_MAINFRAME);

	//check Access
	if(!CheckUserAccess())
		return FALSE ;
	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;

	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();

	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;

	cmdInfo.m_nShellCommand=CCommandLineInfo::FileNothing;
	
	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	((CMainFrame*)m_pMainWnd)->UpdateProjSelectBar();

	// The main window has been initialized, so show and update it.
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();
    AIRPORTDBLIST;
	return TRUE;
}

// added by cj, for check if have access to file writing, 
BOOL CTermPlanApp::CheckUserAccess()
{
	FileManager file;
	CFileException exception ;
	CString filePath ;
	filePath.Format("%s",PROJMANAGER->GetAppPath() + "\\" + "AccessTestFile") ;
	if(!file.MakePath(filePath))
	{
		MessageBox(NULL,_T("The Current User Account have no access to file Writing,Please check with the Administrator!"),"Warning",MB_OK) ;
		return FALSE ;
	}else
	{
		FileManager Filemanger ;
		if(Filemanger.IsDirectory(filePath))
			Filemanger.DeleteDirectory(filePath) ;
		return TRUE ;
	}
        
}
/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CString	m_csBuildNumber;
	CString	m_csTimeLeft;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnAboutUrl();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	m_csBuildNumber = _T("");
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Text(pDX, IDC_STATIC_BUILD, m_csBuildNumber);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_BN_CLICKED(IDC_ABOUT_URL, OnAboutUrl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CTermPlanApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CTermPlanApp message handlers


void CTermPlanApp::OnFileNew() 
{
	// check the license
	if(FALSE == CTermPlanApp::CheckLicense(TRUE))
	{
		return;
	}

	POSITION pos = m_pDocManager->GetFirstDocTemplatePosition();
	CDocTemplate* pTemplate = m_pDocManager->GetNextDocTemplate(pos);
	if(!pTemplate)
	{	
		TRACE0("Error: no Document templates registered with CWinApp.\n");
		AfxMessageBox(AFX_IDP_FAILED_TO_CREATE_DOC);
		return;
	}
	ASSERT(pTemplate != NULL);
	ASSERT_KINDOF(CDocTemplate, pTemplate);

	if(pTemplate->OpenDocumentFile(NULL))
	{	
		ToolBarUpdateParameters updatePara;
		updatePara.m_operatType = ToolBarUpdateParameters::OpenProject;
		updatePara.m_nCurMode = EnvMode_Terminal;		
		AfxGetMainWnd()->SendMessage(UM_SHOW_CONTROL_BAR,1,(LPARAM)&updatePara) ;
		// if returns NULL, the user has already been alerted
		((CMainFrame*)m_pMainWnd)->UpdateProjSelectBar();
	}
}

void CTermPlanApp::OnFileOpen()
{
	// check the license
	if(FALSE == CTermPlanApp::CheckLicense(TRUE))
	{
		return;
	}


	AfxGetApp()->BeginWaitCursor();
	// prompt the user (with all Document templates)
	CString newName;
	//if (!DoPromptFileName(newName, AFX_IDS_OPENFILE,
	//  OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, TRUE, NULL))
	//	return; // open cancelled

	if(!DoPromptForProjectName(newName))
		return; // open cancelled

	ASSERT(!newName.IsEmpty());

	AfxGetApp()->OpenDocumentFile(newName);
		// if returns NULL, the user has already been alerted

	AfxGetApp()->EndWaitCursor();
}

UINT CTermPlanApp::GetClipboardFormat()
{
	return m_nShapeFormat;
}

BOOL CTermPlanApp::DoPromptForProjectName(CString &fileName)
{
	CDlgProjectSelect dlg;
	if(dlg.DoModal() == IDOK) {
		fileName = dlg.m_sSelProjName;
		return TRUE;
	}
	else
		return FALSE;
}

void CTermPlanApp::OnCloseProject()
{
	((CMainFrame*)m_pMainWnd)->UpdateProjSelectBar();
}

CDocument* CTermPlanApp::OpenDocumentFile(LPCTSTR lpszFileName) 
{
	//Terminal terminal;
	//ProcessorList* pList = new ProcessorList( terminal.inStrDict );
	//pList->SetInputTerminal(&terminal);
	//int fileType =  (int)ProcessorLayoutFile;
	//char filename[_MAX_PATH];
	//PROJMANAGER->getFileName (CString(lpszFileName), fileType, filename);
	//pList->loadDataSetFromOtherFile(CString(filename));

	//try{
	//	ArctermFile file;
	//	file.openFile (filename, READ);
	//	StringList strDict;
	//	pList->getAllGroupNames (strDict, GateProc);
	//	if (file.getVersion() < float(2.4) && strDict.getCount() > 0)
	//	{
	//		CDlgGateTypedefine dlg(terminal.inStrDict,pList);
	//		if (IDOK == dlg.DoModal())
	//		{
	//			pList->saveDataSetToOtherFile(filename);
	//		}
	//		else
	//		{
	//			if (pList)
	//			{
	//				delete pList;
	//				pList = NULL;
	//			}
	//			return NULL;
	//		}
	//	}

	//	if (pList)
	//	{
	//		delete pList;
	//		pList = NULL;
	//	}
	//}
	//catch (FileOpenError *exception) {
	//	delete exception;
	//}
	CDocument* pRet =  CWinApp::OpenDocumentFile(lpszFileName);
	if(pRet)
	{
		((CMainFrame*)m_pMainWnd)->UpdateProjSelectBar();
		((CMainFrame*)m_pMainWnd)->GetMainBarTool().setCombox( ((CMainFrame*)m_pMainWnd)->MDIGetActive()->GetActiveDocument()) ;
	}
	return pRet;
}

BOOL CAboutDlg::OnInitDialog()  
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_csBuildNumber.Format("%s",ARCTERM_VERSION_STRING);// = "(3.1.039)";ARCTERM_VERSION_STRING
    
    // Format time remaining...
    int nTemp = ((CTermPlanApp *) AfxGetApp())->SimulationControlLicense.GetProductLicenseSecondsRemaining();

        // Expired...
        if(nTemp == 0)
            m_csTimeLeft.Format("Expired");
    
        // Unlimited...
        else if(nTemp == -1)
            m_csTimeLeft.Format("Unlimited");
            
        // More than one day...
        else if(nTemp > (24 * 60 * 60))
            m_csTimeLeft.Format("%d days", (int) ((float) nTemp / (24.0f * 60.0f * 60.0f)));

        // Less than a day...
        else
            m_csTimeLeft.Format("%d hours", (int) ((float) nTemp / (60.0f * 60.0f)));

    // Show time remaining...
    GetDlgItem(IDC_ABOUT_LICENSE)->SetWindowText(m_csTimeLeft);


	UpdateData( FALSE );
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CTermPlanApp::ReadRegister()
{
	//fisrt read from HKEY_LOCAL_MACHINE\Softawre\ARCTERM,
	//if exits ,then read data
	//else read from HKEY_CURRENT_USER\Softawre\ARCTERM,
	//    if exits,read data.
	//	  else set default value; 
	BOOL bExists = TRUE;
	CString sAirportPath;
	CString sACTypePath;
	CString sProbDistPath;
	CString sDefaultDBPath;

#ifdef _DEBUG
	unsigned char szReigterWord[256];
	unsigned long lDataLength =256;
	HKEY m_RegKey ;
	BOOL oldReg = FALSE ;
	if( RegOpenKey(HKEY_LOCAL_MACHINE,"SOFTWARE\\Aviation Research Corporation\\ARCTerm\\Paths",&m_RegKey) != ERROR_SUCCESS)
	{
		//if can not open HKEY_LOCAL_MACHINE\\SOFTWARE\\ARCTerm\\Paths
		//try to read from HKEY_CURRENT_USER\\Software\\Local AppWizard-Generated Applications\\ARCTerm\\Paths
		if	( RegOpenKey(HKEY_LOCAL_MACHINE,"SOFTWARE\\ARCTerm\\Paths",&m_RegKey) != ERROR_SUCCESS)
		{
			
			if( RegOpenKey(HKEY_CURRENT_USER,"Software\\Local AppWizard-Generated Applications\\ARCTerm\\Paths",&m_RegKey) != ERROR_SUCCESS)
			{
				// TRACE("Can't read data from register!You shall install ARCTerm!\n");
				return FALSE;
			}
		}
		bExists = FALSE;
		oldReg = true;
	}
 

	//read app paths
	if( RegQueryValueEx( m_RegKey,"AppPath",0,NULL,szReigterWord,&lDataLength) != ERROR_SUCCESS )
		m_sAppPath = _T("C:\\ARCTerm");
	else
	{
		szReigterWord[lDataLength] = '\0';
		m_sAppPath = CString( szReigterWord );
	}
		RegCloseKey(m_RegKey);
#else
	bool oldReg = false;
	TCHAR TH[1024] = {0} ;
	if(GetModuleFileName(AfxGetInstanceHandle(),TH,1024) == 0 )
		m_sAppPath = _T("C:\\ARCTerm");
	else
	{
		PathRemoveFileSpec(TH) ;
		m_sAppPath.Format(_T("%s"),TH) ; 
	}
#endif
	//read airport path
		sAirportPath =m_sAppPath +"\\"+ _T("Databases");


	//read actype path
		sACTypePath =m_sAppPath+ "\\"+_T("Databases");

	//read ProbDistPath
		sProbDistPath =m_sAppPath+ "\\"+_T("Databases");
	//read shapedbpath
		m_sShapesDBPath =m_sAppPath+"\\"+ _T("Databases");

	//read ShapesDataPath
		m_sShapeDataPath =m_sAppPath+ "\\"+_T("Databases\\Shapes");

	//read ResourceDataPath
		m_sResourceDataPath =m_sAppPath+ "\\"+_T("Databases\\Resource");

	//read texture path
		m_sTexturePath = m_sAppPath+ "\\"+_T("Databases\\Textures");

	//read surface texture path
		m_sSurfaceTexturePath = m_sAppPath+ "\\"+_T("Databases\\media\\Texture.Surface");
	//read airline img path

		m_sAirlineImgPath = m_sAppPath+ "\\"+_T("Databases\\AirlineImages");

      
	//read ProjectPath
		m_sProjPath =m_sAppPath+ "\\"+_T("Project");
	
	// read default db path
		sDefaultDBPath = m_sAppPath + "\\" + _T("Databases");
		m_sLandsideVehicleShapePath=m_sAppPath+"\\"+_T("DataBases\\media\\Shapes\\LandsideVehicle");
		m_sLandsideVehicleImgPath=m_sAppPath+"\\"+_T("DataBases\\media\\Shapes\\LandsideVehicle");
	
		PROJMANAGER->SetPath( m_sAppPath, m_sProjPath, sAirportPath, sACTypePath, sProbDistPath,sDefaultDBPath );
		ECHOLOG->SetPath(m_sAppPath);
	return TRUE;
}


//if you want to read/write register in ARCterm
//you must use (CTermPlanApp*)AfxGetApp()->,not use AfxGetApp()->
//(CTermPlanApp*)AfxGetApp() make sure read/write from HKEY_LOCAL_MACHINE\\SOFTWARE\\ARCTerm
//if you use AfxGetApp(), I don't know the application will read/write from where.
CString CTermPlanApp::GetSettingINIFilePath()
{
	return CARCTermSettingHander::GetSettingINIFilePath() ;
}
UINT CTermPlanApp::GetProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nDefault)
{
	//bool oldReg = false;
	//HKEY m_RegKey;
	//DWORD dwLength;
	//int iResult = 0;
 //   
	//if( RegOpenKey(HKEY_LOCAL_MACHINE,"SOFTWARE\\Aviation Research Corporation\\ARCTerm\\"+CString(lpszSection),&m_RegKey) != ERROR_SUCCESS)
	//{
	//	if( RegOpenKey(HKEY_LOCAL_MACHINE,"SOFTWARE\\ARCTerm\\"+CString(lpszSection),&m_RegKey) != ERROR_SUCCESS)
	//		iResult = 0;
	//	else
	//		oldReg = true;
 //   }
	//if( RegQueryValueEx( m_RegKey, lpszEntry,0,NULL,(LPBYTE)&iResult,&dwLength) != ERROR_SUCCESS)
	//	iResult = nDefault;

	//RegCloseKey( m_RegKey );
	//if(oldReg) WriteProfileInt(lpszSection,lpszEntry,nDefault);
	//return iResult;
	return CARCTermSettingHander::GetProfileInt(lpszSection,lpszEntry,nDefault) ;
	//INIFileHandle.SaveCfgData(GetSettingINIFilePath()) ;
}

CString CTermPlanApp::GetProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszDefault)
{
	//bool oldReg = false;
	//HKEY m_RegKey;
	//DWORD dwLength;
	//TCHAR szResult[256];

	//if( RegOpenKey(HKEY_LOCAL_MACHINE,"SOFTWARE\\Aviation Research Corporation\\ARCTerm\\"+CString(lpszSection),&m_RegKey) != ERROR_SUCCESS)
	//{
	//	if( RegOpenKey(HKEY_LOCAL_MACHINE,"SOFTWARE\\ARCTerm\\"+CString(lpszSection),&m_RegKey) != ERROR_SUCCESS)
	//	  szResult[0] = '\0';
	//	else
	//		oldReg = true;
	//}

	//if( RegQueryValueEx( m_RegKey, lpszEntry,0,NULL,(LPBYTE)szResult,&dwLength) != ERROR_SUCCESS)
	//	strcpy( szResult, lpszDefault);
	//else
	//	szResult[dwLength] = '\0';

	//RegCloseKey( m_RegKey );
	//if(oldReg) WriteProfileString(lpszSection,lpszEntry,lpszDefault);
	//return CString(szResult);
	return CARCTermSettingHander::GetProfileString(lpszSection,lpszEntry,lpszDefault) ;
	//INIFileHandle.SaveCfgData(GetSettingINIFilePath()) ;
}

BOOL CTermPlanApp::WriteProfileInt(LPCTSTR lpszSection, LPCTSTR lpszEntry, int nValue)
{
	//HKEY m_RegKey;
	//BOOL bResult = TRUE;

	//if( RegCreateKey(HKEY_LOCAL_MACHINE,"SOFTWARE\\Aviation Research Corporation\\ARCTerm\\"+CString(lpszSection),&m_RegKey) != ERROR_SUCCESS )
	//	bResult = FALSE;

	//if( RegSetValueEx(m_RegKey,lpszEntry,0,REG_DWORD,(CONST BYTE*)&nValue,sizeof(int)) !=ERROR_SUCCESS )
	//	bResult = FALSE;

	//RegCloseKey( m_RegKey );
	//return bResult;

	return CARCTermSettingHander::WriteProfileInt(lpszSection,lpszEntry,nValue) ;
}

BOOL CTermPlanApp::WriteProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszValue)
{
	//HKEY m_RegKey;
	//BOOL bResult = TRUE;

	//if( RegCreateKey(HKEY_LOCAL_MACHINE,"SOFTWARE\\Aviation Research Corporation\\ARCTerm\\"+CString(lpszSection),&m_RegKey) != ERROR_SUCCESS )
	//	bResult = FALSE;

	//if( RegSetValueEx(m_RegKey,lpszEntry,0,REG_SZ,(CONST BYTE*)lpszValue,lstrlen(lpszValue)) !=ERROR_SUCCESS )
	//	bResult = FALSE;

	//RegCloseKey( m_RegKey );
	//return bResult;
	return CARCTermSettingHander::WriteProfileString(lpszSection,lpszEntry,lpszValue) ;
}

int CTermPlanApp::ExitInstance() 
{
	// TODO: Add your specialized code here and/or call the base class
	if(m_pMasterDatabase)
		m_pMasterDatabase->Close();
	delete m_pMasterDatabase;
	DATABASECONNECTION.CloseConnection(DATABASESOURCE_TYPE_ACCESS_GLOBALDB) ;

	// clear temporary files
	CTmpFileManager::GetInstance().DeleteBaseFolder();

	OleDragDropManager::GetInstance().SetRegisterClipboardFormat(INVALID_CLIPBOARD_FORMAT);

	return CWinApp::ExitInstance();
}

//While Delete Current Project from main menu,  Update MRU info
void CTermPlanApp::UpdateMRUList(CString& prjpath)
{
	ASSERT(!prjpath.IsEmpty());
	int idx=0, nMRUCount = m_pRecentFileList->GetSize();

	while (idx<nMRUCount)
	{
		if(!prjpath.CompareNoCase((LPCTSTR)(*m_pRecentFileList)[idx]))
		{
			m_pRecentFileList->Remove(idx);
			break;
		}
		idx++;
	}
}

CMasterDatabase * CTermPlanApp::GetMasterDatabase()
{
	return m_pMasterDatabase;
}

BOOL CTermPlanApp::ConnectToMasterDatabase()
{
	delete m_pMasterDatabase;
	m_pMasterDatabase = new CMasterDatabase(m_sAppPath);
	
	try
	{
		//bool bConnect = m_pMasterDatabase->Connect();

		//if (bConnect)
			return TRUE;
	}
	catch(CDatabaseFatalError& err)
	{
		AfxMessageBox(_T("Fatal Error Occured: ")+err.GetErrorMessage());

	}
	return FALSE;
}

BOOL CTermPlanApp::CheckLicense(BOOL bPopDlg/* = FALSE*/)
{
	int nSeconds = (
		(CTermPlanApp *)AfxGetApp())->SimulationControlLicense.GetProductLicenseSecondsRemaining();
	if(0 == nSeconds)
	{
		if(bPopDlg)
		{
			CLicenseExpiredDlg dlg;
			dlg.DoModal();
		}
		return FALSE;
	}
	return TRUE;

}


//////////////////////////////////////////////////////////////////////////
//CAboutDlg
void CAboutDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if(nFlags==9)
	{
		unsigned long len = 9999;
		char path[1024];
		CString sLockName;
		// get the ARCterm installed path from the register
		CRegKey regkey;
		regkey.Open(HKEY_LOCAL_MACHINE,"SOFTWARE\\Aviation Research Corporation\\ARCTerm\\Paths");
		regkey.QueryValue(path,"AppPath",&len);
		regkey.Close();	
		if ( len == 9999 )
		{
			AfxMessageBox("ARCport is not installed on this computer");
			CDialog::OnCancel();
			return ;
		}

	}
	CDialog::OnLButtonDown(nFlags, point);
}

void CAboutDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	m_csTimeLeft = "";
	UpdateData(false);
	CDialog::OnLButtonUp(nFlags, point);
}

void CAboutDlg::OnAboutUrl() 
{
	// Launch website in browser...
	ShellExecute(NULL, "open", "http://www.arc-us-ca.com/", NULL, NULL, SW_SHOW);
}


