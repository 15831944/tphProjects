// MainBar.cpp: implementation of the CMainBar class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "termplan.h"
#include "MainBar.h"
#include "MainFrm.h"
#include "ProcessFlowView.h"
#include "3DView.h"
#include "ChildFrm.h"
#include <windows.h>
#include "ChildFrameSplit.h"

#include "Render3DView.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMainBar::CMainBar()
{

}

CMainBar::~CMainBar()
{

}

BEGIN_MESSAGE_MAP(CMainBar, CToolBar24X24)
	//{{AFX_MSG_MAP(CMainBar)
	ON_WM_CREATE()
	ON_CBN_SELCHANGE(IDD_COMBO_ENVMODE, OnSelChangeEnvModeComboBox)
	ON_CBN_SELCHANGE(IDD_COMBO_SIMULATIONTYPE, OnSelChangeSimuTypeComboBox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

int CMainBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CToolBar24X24::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	InitImageList(IDB_TOOLBAR_MAIN_NORMAL,IDB_TOOLBAR_MAIN_HOT,IDB_TOOLBAR_MAIN_DISABLE,RGB(255,0,255));
	SetImageList();
	
	TBBUTTON tbb;
	CToolBar24X24::InitTBBUTTON(tbb) ;
	
	tbb.iBitmap = 0;
	tbb.idCommand = ID_FILE_NEW;
	AddButton(&tbb);
	
	tbb.iBitmap = 1;
	tbb.idCommand = ID_FILE_OPEN;
	AddButton(&tbb);
	
	tbb.iBitmap = 2;
	tbb.idCommand = ID_FILE_PRINT_SETUP;
	AddButton(&tbb);
	
	tbb.iBitmap = 3;
	tbb.idCommand = ID_APP_ABOUT;
	AddButton(&tbb);

	tbb.iBitmap = 4;
	tbb.idCommand = ID_FILE_SAVE;
	AddButton(&tbb);

	m_nCountNotButton = 9; //not sure why needed, if remove this, get button behind combo box
	AddTextComboBox(_T("Environment Mode: "), &m_cmbEnvMode, CSize(165,200), IDD_COMBO_ENVMODE, CBS_DROPDOWNLIST);
	AddTextComboBox(_T("   Simulation Type: "), &m_cmbSimulationType, CSize(180,200), IDD_COMBO_SIMULATIONTYPE, CBS_DROPDOWNLIST);
	for(int j=0; j<9; j++)
	{
		tbb.iBitmap = -1;
		tbb.idCommand = TEMP_ID_FOR_NOT_BUTTON+m_nCountNotButton;;
		tbb.fsState &= ~TBSTATE_ENABLED;
		AddButton(&tbb,0,TRUE);
	}

	SetWindowText (_T ("Main"));

	m_cmbEnvMode.AddString(_T("AirSide"));
	m_cmbEnvMode.AddString(_T("LandSide"));
	m_cmbEnvMode.AddString(_T("Terminal"));
	m_cmbEnvMode.AddString(_T("OnBoard"));
	m_cmbEnvMode.AddString(_T("Cargo"));
	m_cmbEnvMode.AddString(_T("Environment"));
	m_cmbEnvMode.AddString(_T("Finanical"));


	m_cmbSimulationType.AddString(_T("Monte Carlo"));
	m_cmbSimulationType.AddString(_T("Mathematical"));

	EnableComboBox(false, true);

	EnableDocking(CBRS_ALIGN_ANY);
	return 0;
}
CNewMenu m_NewMenu;
void CMainBar::OnSelChangeEnvModeComboBox()
{
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	CView* pView = pMainFrame->GetActiveFrame()->GetActiveView();
	if (!pView || !::IsWindow(pView->m_hWnd))
		return;
	
	CTermPlanDoc* pDoc = (CTermPlanDoc*)pView->GetDocument();
	ASSERT_VALID(pDoc);

	int nSeled = m_cmbEnvMode.GetCurSel();
	if (nSeled < 0)
		return;
	if( pDoc->m_systemMode == (EnvironmentMode) nSeled)
		return;

	if(pDoc)
	{
		CChildFrame* pFrame = pDoc->Get3DViewParent();
		if(pFrame)
			pFrame->StorePanInfo();
	}

	pDoc->SaveCurModeRenderCameras();
	pDoc->m_systemMode = getEnvironModeFromComboxIndex(nSeled) ;
    UpdateEnvironmentView(pDoc) ;

	CRender3DView* pRender3DView = pDoc->GetRender3DView();
	if (pRender3DView)
		pRender3DView->UpdateShapesByMode(pDoc->m_systemMode);

}
void CMainBar::UpdateEnvironmentView(CTermPlanDoc* pDoc)
{
	if(pDoc == NULL)
		return ;
	CString strWindowTitle(pDoc->GetTitle());
	if (pDoc->m_systemMode == EnvMode_Terminal)
		strWindowTitle += _T("        Terminal Modeling Sequence View");
	else if (pDoc->m_systemMode == EnvMode_AirSide)
		strWindowTitle += _T("      Airside Modeling Sequence View");
	else if (pDoc->m_systemMode == EnvMode_LandSide)
		strWindowTitle += _T("      Landside Modeling Sequence View ");		
	else if (pDoc->m_systemMode == EnvMode_OnBoard)
		strWindowTitle += _T("      OnBoard Modeling Sequence View ");
	else if (pDoc->m_systemMode == EnvMode_Cargo)
		strWindowTitle += _T("      Cargo Modeling Sequence View ");
	else if (pDoc->m_systemMode == EnvMode_Environment)
		strWindowTitle += _T("      Environment Modeling Sequence View ");
	else if (pDoc->m_systemMode == EnvMode_Financial)
		strWindowTitle += _T("      Financial Modeling Sequence View ");



   CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	CMDIChildWnd* pActiveChildFrame = pMainFrame->MDIGetActive();
	pActiveChildFrame->SetWindowText(strWindowTitle);

	pMainFrame->m_bViewOpen = false;
	pDoc->UpdateEnvandSimMode();

	if(m_NewMenu.m_hMenu)
		m_NewMenu.DestroyMenu();
	// Load the new menu.
	
	if( EnvMode_AirSide == pDoc->m_systemMode )
	{
		m_NewMenu.LoadMenu(IDR_AIRSIDE);
	}
	else if (EnvMode_OnBoard == pDoc->m_systemMode)
	{
		m_NewMenu.LoadMenu(IDR_ONBOARD);
	}
	else if (EnvMode_LandSide == pDoc->m_systemMode)
	{
		m_NewMenu.LoadMenu(IDR_LANDSIDE);
	}

	else if (EnvMode_Cargo == pDoc->m_systemMode)
	{
		m_NewMenu.LoadMenu(IDR_MAINFRAME_CARGO);
	}
	else if (EnvMode_Environment == pDoc->m_systemMode)
	{
		m_NewMenu.LoadMenu(IDR_MAINFRAME_ENVIRONMENT);
	}
	else if (EnvMode_Financial == pDoc->m_systemMode)
	{
		m_NewMenu.LoadMenu(IDR_MAINFRAME_FINANCIAL);
	}
	else
	{
		m_NewMenu.LoadMenu(IDR_3DVIEWFRAME);
	}
	ASSERT(m_NewMenu);

	// Remove and destroy the old menu
	//pMainFrame->SetMenu(NULL);
	//::DestroyMenu(pMainFrame->m_hMenuDefault);

	// Add the new menu
	pMainFrame->SetMenu(&m_NewMenu);
	pDoc->UpdateTrackersMenu();
	// Assign default menu
	//pMainFrame->m_hMenuDefault = m_NewMenu.GetSafeHmenu();  // or m_NewMenu.m_hMenu;


	/*
	CNewMenu* pTrackerMenu=NULL;
	CNewMenu* pMainMenu =DYNAMIC_DOWNCAST(CNewMenu, pMainFrame->GetMenu());
	int nMenuCount=pMainMenu->GetMenuItemCount();
	int k;
	CString s;
	for(k=0;k<nMenuCount;k++)
	{
	pMainMenu->GetMenuString(k, s, MF_BYPOSITION);
	if(s=="Reports")
	{
	pTrackerMenu=DYNAMIC_DOWNCAST(CNewMenu,pMainMenu->GetSubMenu(k));
	break;
	}
	}
	if(pTrackerMenu!=NULL)
	{
	nMenuCount=pTrackerMenu->GetMenuItemCount();
	for(k=0;k<nMenuCount;k++)
	{
	pTrackerMenu->DeleteMenu(0,MF_BYPOSITION);
	}

	//		pTrackerMenu->DestroyMenu(); 
	//		pTrackerMenu->CreateMenu();
	pTrackerMenu->AppendMenu(MF_STRING, ID_AIRCRAFT_FUELBURNS, "sstwtwtwgwtw");
	}
	*/
	pMainFrame->m_bViewOpen = true;
}
void CMainBar::OnSelChangeSimuTypeComboBox()
{
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	CView* pView = pMainFrame->GetActiveFrame()->GetActiveView();
	if (!pView || !::IsWindow(pView->m_hWnd))
		return;
	CTermPlanDoc* pDoc = (CTermPlanDoc*)pView->GetDocument();
	ASSERT_VALID(pDoc);

	int nSeled = m_cmbSimulationType.GetCurSel();
	if (nSeled < 0)
		return;

	pDoc->m_simType = getSimModeFromComboxIndex(nSeled);
//	if (pDoc->m_simType == SimType_Mathmatical)
//		pMainFrame->SendMessage(UM_SHOW_CONTROL_BAR, 1, 0);

	pDoc->UpdateEnvandSimMode();
}

void CMainBar::EnableComboBox(bool bEnable, bool bSetContentDefault)
{
 	ASSERT(::IsWindow(m_cmbEnvMode.m_hWnd) && ::IsWindow(m_cmbSimulationType));
	m_cmbEnvMode.EnableWindow(bEnable);
	m_cmbSimulationType.EnableWindow(bEnable);

	if (bSetContentDefault)
	{
		m_cmbEnvMode.SetCurSel(getEnvComboxIndexFromEnvironMode(EnvMode_Terminal));
		m_cmbSimulationType.SetCurSel(0);
	}
}
void CMainBar::getCurrentEnviro(EnvironmentMode& enviro , SimulationType& sim_type)
{

	setting.getDataFromDB() ;
	if(setting.IsEmpty() != TRUE)
	{
		enviro = (EnvironmentMode)setting.getEnvironmentMode() ;
		sim_type = (SimulationType)setting.getSimulation() ;
	}else
	{
		enviro = EnvMode_Terminal  ;
		sim_type =  (SimulationType)0 ;
		setting.Empty() ;
	}
 //OnSelChangeEnvModeComboBox();
//OnSelChangeSimuTypeComboBox();
}
void CMainBar::setCombox(CDocument* doc)
{
	if(doc == NULL)
		return ;
	CTermPlanDoc* pDoc = (CTermPlanDoc*)doc ;
	m_cmbEnvMode.SetCurSel(getEnvComboxIndexFromEnvironMode(pDoc->m_systemMode));
	m_cmbSimulationType.SetCurSel(getSimComboxIndexFromSimMode(pDoc->m_simType));
	UpdateEnvironmentView(pDoc);
}
void CMainBar::SaveComBoxToDB()
{

	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	CView* pView = pMainFrame->GetActiveFrame()->GetActiveView();
	if (!pView || !::IsWindow(pView->m_hWnd))
		return;
	CTermPlanDoc* pDoc = (CTermPlanDoc*)pView->GetDocument();
	setting.setEnvironmentMode(pDoc->GetCurrentMode()) ;
	setting.setSimulation(pDoc->m_simType);
	setting.saveDataToDB() ;
}