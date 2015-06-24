// SimEngineDlg.cpp : implementation file
//

#include "stdafx.h"
#include "simenginedlg.h"
#include "paxtrackdlg.h"
#include "simengsettingdlg.h"
#include "termplandoc.h"
#include "trackwnd.h"
#include "common\enginediagnose.h"
#include "common\projectmanager.h"
#include "common\template.h"
#include "common\winmsg.h"
#include "inputs\in_term.h"
#include "inputs\simparameter.h"
#include "results\paxdestdiagnoselog.h"
#include ".\simenginedlg.h"

#include "../Common/EngineDiagnoseEx.h"
#include "ALTOException.h"
#include "DlgALTOException.h"
#include "../Common/ARCTracker.h"
#include "ARCPort.h"

#include <Common/ARCTracker.h>
#include "Common/SimAndReportManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*
	NOTE: if SimulationDlg need new conditions before run simulation.
	you need check if 'CTermPlanDoc::OnFireImpact()' need too
*/
extern const CString c_setting_regsectionstring;
const CString c_simengine_sorting_entry = "Sim Engine Sorting String";


CSimEngineDlg::CSimEngineDlg(CWnd* pParent)
	: CXTResizeDialog(CSimEngineDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSimEngineDlg)
	//}}AFX_DATA_INIT
	m_nRunNumber = 1;
	m_pUserMsgOwner=NULL;
	m_pTermPlanDoc = NULL;
}

CSimEngineDlg::~CSimEngineDlg()
{
	clearDiagnoseEntryList();
}


void CSimEngineDlg::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSimEngineDlg)
	//DDX_Control(pDX, IDC_LIST_ERROR_MSG, m_listErrorMsg);
	DDX_Control(pDX, IDC_STATIC_ERROR_MSG, m_staticErrorMsg);
	DDX_Control(pDX, IDC_STATIC_PROG2, m_staticProgText2);
	DDX_Control(pDX, IDC_PROGRESS_SIM2, m_progSimStatus2);
	DDX_Control(pDX, IDC_STATIC_TYPE, m_butType);
	DDX_Control(pDX, IDC_STATIC_TIME, m_butTime);
	DDX_Control(pDX, IDC_STATIC_NUMBER, m_butNumber);
	DDX_Control(pDX, IDC_STATIC_FRAME2, m_butFrame2);
	DDX_Control(pDX, IDC_STATIC_FRAME1, m_butFrame1);
	DDX_Control(pDX, IDC_BUTTON_CANCEL, m_btnCancel);
	DDX_Control(pDX, IDC_BUTTON_FILE, m_btnFile);
	DDX_Control(pDX, IDCANCEL, m_btnClose);
	DDX_Control(pDX, IDC_STATIC_PROG, m_staticProgText);
	DDX_Control(pDX, IDC_EDIT_EVENTTYPE, m_editEventType);
	DDX_Control(pDX, IDC_EDIT_EVENTTIME, m_editEventTime);
	DDX_Control(pDX, IDC_EDIT_EVENTNUM, m_editEventNum);
	DDX_Control(pDX, IDC_PROGRESS_SIM, m_progSimStatus);
	DDX_Control(pDX, IDC_LIST_SIMSTATUS, m_listboxSimStatus);
	//DDX_Control(pDX, IDC_LIST_TERMINAL, m_wndTerminalListCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSimEngineDlg, CXTResizeDialog)
	//{{AFX_MSG_MAP(CSimEngineDlg)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, OnButtonCancel)
	ON_BN_CLICKED(IDC_BUTTON_FILE, OnButtonFile)
// 	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_ERROR_MSG, OnColumnclickListErrorMsg)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SIM_STATUS_MSG, OnSimStatusMsg)
	ON_MESSAGE(WM_SIM_STATUS_MSG2, OnSimStatusMsg2)
	ON_MESSAGE(WM_SIM_STATUS_EVENT, OnSimStatusEvent)
	ON_MESSAGE(WM_SIM_STATUS_FORMATEX_MSG, OnSimStatusFormatExMsg)

	ON_MESSAGE(WM_SIM_STATUS_APPEND_DIAGNOSE, OnAppendDiagnose)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_MESSAGE(WM_SIM_SORTEVENTLOG, OnSimSortEventLog)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSimEngineDlg message handlers

BOOL CSimEngineDlg::OnInitDialog() 
{
	CXTResizeDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	//m_listErrorMsg.SetShowCheckBtn(FALSE,FALSE);
	m_progSimStatus.SetRange( 0, 100 );
	m_progSimStatus2.SetRange( 0, 100 );
	m_nCurProgPos = 0;
	m_nCurProgPos2 = 0;

	ShowCurrentSetting();

	UpdateData( false );

	//SendDlgItemMessage( IDC_BUTTON_GENERATE, BN_CLICKED );		
	m_btnFile.EnableWindow( false );
	m_btnClose.EnableWindow( false );

	CRect rect;
	CRect rectTop;
	GetDlgItem(IDC_STATIC_FRAME1)->GetWindowRect( &rectTop );
	ScreenToClient( &rectTop );
	rect.left = rectTop.left;
	rect.right = rectTop.right;
	rect.top = rectTop.bottom + 20;
	
	CRect rectBottom;
	GetDlgItem(IDC_STATIC_FRAME2)->GetWindowRect(rectBottom);
	ScreenToClient(&rectBottom);
	rect.bottom = rectBottom.top - 12;

	m_wndErrorMsgPanel.SetTerminal(GetEngine()->getTerminal());
	m_wndErrorMsgPanel.SetTermPlanDoc(m_pTermPlanDoc);
	m_wndErrorMsgPanel.Create(this, rect);
//// 	m_wndErrorMsgPanel.SetBehaviour(xtpTaskPanelBehaviourToolbox);
//// 	m_wndErrorMsgPanel.SetTheme(xtpTaskPanelThemeToolbox);
//// 	m_wndErrorMsgPanel.SetAnimation(xtpTaskPanelAnimationNo);
//// 	m_wndErrorMsgPanel.SetTheme(xtpTaskPanelThemeNativeWinXPPlain);
//// 	m_wndErrorMsgPanel.SetTheme(xtpTaskPanelThemeOffice2000);
//// 	m_wndErrorMsgPanel.SetSingleSelection(FALSE);

	SetResize(IDC_STATIC_FRAME1, SZ_TOP_LEFT, SZ_TOP_RIGHT);
	SetResize(IDC_STATIC, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_LIST_SIMSTATUS, SZ_TOP_LEFT, SZ_TOP_RIGHT);

	SetResize(IDC_STATIC_ERROR_MSG, SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(m_wndErrorMsgPanel.GetDlgCtrlID(), SZ_TOP_LEFT, SZ_BOTTOM_RIGHT);

	SetResize(IDC_STATIC_FRAME2, SZ_BOTTOM_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_STATIC_NUMBER, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_EDIT_EVENTNUM, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_STATIC_TIME, SZ_BOTTOM_CENTER, SZ_BOTTOM_CENTER);
	SetResize(IDC_EDIT_EVENTTIME, SZ_BOTTOM_CENTER, SZ_BOTTOM_CENTER);
	SetResize(IDC_STATIC_TYPE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_EDIT_EVENTTYPE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);

	SetResize(IDC_STATIC_PROG, SZ_BOTTOM_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_PROGRESS_SIM, SZ_BOTTOM_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_STATIC_PROG2, SZ_BOTTOM_LEFT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_PROGRESS_SIM2, SZ_BOTTOM_LEFT, SZ_BOTTOM_RIGHT);

	SetResize(IDC_BUTTON_FILE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_CANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);


	//SetTimer( 1,1000,NULL );
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
void CSimEngineDlg::setSelectedSimulationData(const SelectedSimulationData& simSelData)
{
	m_simSelData =  simSelData;

// 	m_wndErrorMsgPanel.unExpandAll();

	if(m_simSelData.IsTerminalSel())
	{
		//m_wndErrorMsgPanel.ExpandTerminal();
		m_wndErrorMsgPanel.SetCurSel(2);
		m_wndErrorMsgPanel.UpdateTableDisplay(2);
	}
	else if (m_simSelData.IsAirsideSel())
	{
		//m_wndErrorMsgPanel.ExpandAirside();
		m_wndErrorMsgPanel.SetCurSel(0);
		m_wndErrorMsgPanel.UpdateTableDisplay(0);
	}
	else if( m_simSelData.IsLandsideSel())
	{
		//m_wndErrorMsgPanel.ExpandLandside();
		m_wndErrorMsgPanel.SetCurSel(1);
		m_wndErrorMsgPanel.UpdateTableDisplay(1);
	}
	else if( m_simSelData.IsOnBoardSel())
	{
		//m_wndErrorMsgPanel.ExpandOnBoard();
		m_wndErrorMsgPanel.SetCurSel(3);
		m_wndErrorMsgPanel.UpdateTableDisplay(3);
	}
}
LRESULT CSimEngineDlg::OnSimStatusFormatExMsg(WPARAM wParam,LPARAM lParam)
{
	MSG msg;
    // Handle dialog messages
    while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
		if(!IsDialogMessage(&msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);  
		}
    }

	// set percent
	int nNewPos = (int)wParam;
	if( m_nCurProgPos != nNewPos )
	{
		m_nCurProgPos = nNewPos;
		m_progSimStatus.SetPos( m_nCurProgPos );
		if(m_pUserMsgOwner)
			m_pUserMsgOwner->SendMessage(UM_SimEngineDlg_ProgressBar_SetPos,m_nCurProgPos);
		CString csProg;
		csProg.Format( "Run Number %d  ( % 3d%% )", m_nRunNumber, m_nCurProgPos );
		m_staticProgText.SetWindowText( csProg );
		m_progSimStatus.Invalidate();
	}
	
	// set format_ex msg
	if( lParam != 0 )
	{
		
		const FORMATMSGEX* msgEx =( const FORMATMSGEX* ) lParam;
		m_wndErrorMsgPanel.AddSimStatusFormatExMsg(msgEx);
	}
	return 0;
}

LRESULT CSimEngineDlg::OnAppendDiagnose(WPARAM wParam, LPARAM lParam)
{
	MSG msg;
	// Handle dialog messages
	while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if(!IsDialogMessage(&msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);  
		}
	}


	ARCDiagnose::DiagnoseType emType = (ARCDiagnose::DiagnoseType)wParam;
	ARCDiagnose* pDiagnose = (ARCDiagnose*)lParam;
	switch(emType)
	{
	case ARCDiagnose::em_TerminalDiagnose:
		m_wndErrorMsgPanel.AppendTerminalDiagnose( (TerminalDiagnose*)pDiagnose );
		break;
	case ARCDiagnose::em_AirsideDiagnose:
		m_wndErrorMsgPanel.AppendAirsideDiagnose( (AirsideDiagnose*)pDiagnose );
		break;
	case ARCDiagnose::em_LandsideDiagnose:
		m_wndErrorMsgPanel.AppendLandsideDiagnose( (LandsideDiagnose*)pDiagnose );
	    break;
	case ARCDiagnose::em_OnBoardDiagnose:
		m_wndErrorMsgPanel.AppendOnBoardDiagnose( (OnBoardDiagnose*)pDiagnose );
		break;
	default:
		ASSERT(FALSE);
	    break;
	}

	delete pDiagnose;

	return 0;
}

LRESULT CSimEngineDlg::OnSimStatusMsg( WPARAM wParam, LPARAM lParam )
{
    MSG msg;
    // Handle dialog messages
    while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
      if(!IsDialogMessage(&msg))
      {
        TranslateMessage(&msg);
        DispatchMessage(&msg);  
      }
    }

	int nNewPos = (int)wParam;

	if( m_nCurProgPos != nNewPos )
	{
		m_nCurProgPos = nNewPos;
		m_progSimStatus.SetPos( m_nCurProgPos );
		if(m_pUserMsgOwner)
			m_pUserMsgOwner->SendMessage(UM_SimEngineDlg_ProgressBar_SetPos,m_nCurProgPos);
		CString csProg;
		csProg.Format( "Run Number %d  ( % 3d%% )", m_nRunNumber, m_nCurProgPos );
		m_staticProgText.SetWindowText( csProg );
		m_progSimStatus.Invalidate();
	}

	if( lParam != 0 )
	{
		CString str( (char*)lParam );
		int index = m_listboxSimStatus.AddString( str );
		if( index > 10 )
			m_listboxSimStatus.SetTopIndex( index - 7 );
	}
	return 0;
}


LRESULT CSimEngineDlg::OnSimStatusMsg2( WPARAM wParam, LPARAM lParam )
{
    MSG msg;
    // Handle dialog messages
    while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
      if(!IsDialogMessage(&msg))
      {
        TranslateMessage(&msg);
        DispatchMessage(&msg);  
      }
    }

	int nNewPos = (int)wParam;

	m_nRunNumber = (int)lParam;
	if( m_nCurProgPos2 != nNewPos )
	{
		m_nCurProgPos2 = nNewPos;
		m_progSimStatus2.SetPos( m_nCurProgPos2 );
		CString csProg;
		csProg.Format( "All Runs  ( % 3d%% )", m_nCurProgPos2 );
		m_staticProgText2.SetWindowText( csProg );
		m_progSimStatus2.Invalidate();
	}
	if( nNewPos == 100&&m_pUserMsgOwner==NULL )
	{
		MessageBox( "Simulation Done", "Simulation", MB_OK|MB_ICONWARNING );
		m_editEventType.SetWindowText(NULL);
		m_editEventTime.SetWindowText(NULL);
		m_editEventNum.SetWindowText(NULL);
		m_progSimStatus.SetPos(0);
		m_progSimStatus2.SetPos(0);

		m_wndErrorMsgPanel.SetShowErrorInfo(TRUE);
	}
	return 0;
}

LRESULT CSimEngineDlg::OnSimStatusEvent( WPARAM wParam, LPARAM lParam )
{
    MSG msg;
    // Handle dialog messages
    while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
      if(!IsDialogMessage(&msg))
      {
        TranslateMessage(&msg);
        DispatchMessage(&msg);  
      }
    }

	int nNewPos = (int)wParam;
	if( m_nCurProgPos != nNewPos )
	{
		m_nCurProgPos = nNewPos;
		m_progSimStatus.SetPos( m_nCurProgPos );
		if(m_pUserMsgOwner)
			m_pUserMsgOwner->SendMessage(UM_SimEngineDlg_ProgressBar_SetPos,m_nCurProgPos);
		CString csProg;
		csProg.Format( "Run Number %d  ( % 3d%% )", m_nRunNumber, m_nCurProgPos );
		m_staticProgText.SetWindowText( csProg );
		m_progSimStatus.Invalidate();
	}

	if( lParam != 0 )
	{
		EventMsg* pEventMsg = (EventMsg*)lParam;
		CString str;
		str.Format( "(%d)%8d", GetEngine()->m_simBobileelemList.m_lliveCount, pEventMsg->nEventNum );
		//str.Format( "%8d", pEventMsg->nEventNum );
		m_editEventNum.SetWindowText( str );
		m_editEventTime.SetWindowText( pEventMsg->csEventTime );
		m_editEventType.SetWindowText( pEventMsg->csEventType );
		m_editEventNum.Invalidate();
		m_editEventTime.Invalidate();
		m_editEventType.Invalidate();
	}
	return 0;
}

void CSimEngineDlg::ShowCurrentSetting()
{
	CString str;
	str.Format( "---------------------------------------------------------" );
	m_listboxSimStatus.AddString( str );
	str.Format( "Current Setting:" );
	m_listboxSimStatus.AddString( str );
	if( GetEngine()->getTerminal()->m_pSimParam->GetRandomFlag() )
		str = "True Random.";
	else
		str.Format( "Use Random Seed: %d", GetEngine()->getTerminal()->m_pSimParam->GetSeed()  );
	m_listboxSimStatus.AddString( str );
	// set pax count string
	if( GetEngine()->getTerminal()->m_pSimParam->GetPaxCount() == -1 )
		str = "Maximum Passenger Count: unlimited ";
	else
		str.Format( "Maximum Passenger Count: %d", GetEngine()->getTerminal()->m_pSimParam->GetPaxCount() );
	m_listboxSimStatus.AddString( str );
	
	str = "Options: ";
	if( GetEngine()->getTerminal()->m_pSimParam->GetVisitorsFlag() )
		str += "Non Passenger, ";
	if( GetEngine()->getTerminal()->m_pSimParam->GetBarrierFlag() )
		str += "Barrier, ";
	if( GetEngine()->getTerminal()->m_pSimParam->GetHubbingFlag() )
		str += "Hubbing, ";
	m_listboxSimStatus.AddString( str );
	str.Format( "---------------------------------------------------------" );
	m_listboxSimStatus.AddString( str );
}

CARCportEngine* CSimEngineDlg::GetEngine()
{
	ASSERT(m_pTermPlanDoc != NULL);
	return &(m_pTermPlanDoc->getARCport()->m_engine);
	//return (InputTerminal*)&m_pTermPlanDoc->GetTerminal();
}


CString CSimEngineDlg::GetProjPath()
{
	ASSERT(m_pTermPlanDoc != NULL);
	return m_pTermPlanDoc->m_ProjInfo.path;
}


void CSimEngineDlg::OnTimer(UINT nIDEvent) 
{
	PLACE_METHOD_TRACK_STRING();
	// TODO: Add your message handler code here and/or call default
	KillTimer( nIDEvent );

	AfxGetMainWnd()->EnableWindow(FALSE);	//disable main window
	EnableWindow(TRUE);

	m_btnClose.EnableWindow( false );
	HWND hWnd = this->GetSafeHwnd();

	unsigned int nSeed;
	if( GetEngine()->getTerminal()->m_pSimParam->GetRandomFlag() )
		nSeed = rand();
	else
		nSeed = (unsigned int)GetEngine()->getTerminal()->m_pSimParam->GetSeed();
	srand( nSeed );

	ALTOException::setALTOExceptionHandler();

	try
	{
		GetEngine()->runSimulation( hWnd, GetProjPath(),m_simSelData, m_pTermPlanDoc->getARCport());
	}
	catch( ALTOException & simRunningError)
	{
		// Dump ARCTracker stack
		std::ofstream os;
		os.open(GetProjPath() + "\\simulation_failure_dump.log");
		if (os.is_open())
		{
			simRunningError.DumpToStream(os);
			os << std::endl;
			DUMP_TRACK_TO_STREAM(os);
			ARCStringTracker::ClearTrackerImage();
		}

		CDlgALTOException dlg( CDlgALTOException::SimRunningError, simRunningError);
		dlg.DoModal();

		return;
		//GetEngine()->SetPercentOfRuns( 100 );
		//GetEngine()->SendSimMessageOfRuns();
	}

#ifndef _DEBUG

	catch (...)
	{
		DUMP_TRACK_TO_FILE(GetProjPath() + "\\simulation_failure_dump.log");
		ARCStringTracker::ClearTrackerImage();
		CDlgALTOException dlg(CDlgALTOException::SimRunningError, ALTOException(NULL, "Uncaught Exception"));
		dlg.DoModal();

		return;
		//GetEngine()->SetPercentOfRuns( 100 );
		//GetEngine()->SendSimMessageOfRuns();
	}

#endif

	m_btnCancel.EnableWindow( false );
	m_btnClose.EnableWindow( true );
	m_btnFile.EnableWindow( true );
//	m_listErrorMsg.SetShowCheckBtn(TRUE,TRUE);
	CXTResizeDialog::OnTimer(nIDEvent);

	AfxGetMainWnd()->EnableWindow(TRUE);	//enable main window
}

void CSimEngineDlg::OnButtonCancel() 
{
	// TODO: Add your control notification handler code here
	GetEngine()->CancelSimulation();
}

void CSimEngineDlg::OnButtonFile() 
{
	// TODO: Add your control notification handler code here
	CFileDialog filedlg( FALSE,"txt", NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Text File (*.txt)|*.txt;*.TXT|All type (*.*)|*.*|", NULL, 0, FALSE );
	
	if(filedlg.DoModal()!=IDOK)
		return;
	
	CString csFileName=filedlg.GetPathName();

	FILE* f = fopen( csFileName, "wt" );
	if( !f )
		return;

	int nCount = m_listboxSimStatus.GetCount();
	for( int i=0; i<nCount; i++ )
	{
		CString csStr;
		m_listboxSimStatus.GetText( i, csStr );
		fputs( csStr, f );
		fputs( "\n", f );
	}
	
	m_wndErrorMsgPanel.SaveErrorMessage(f);

	fclose( f );

	m_btnFile.EnableWindow( false );
	
}

// void CSimEngineDlg::OnSize(UINT nType, int cx, int cy) 
// {
// 	CXTResizeDialog::OnSize(nType, cx, cy);
// 	
// 	// TODO: Add your message handler code here
// 	if( m_btnClose.m_hWnd == NULL )
// 		return;
// 
// 	// first move but
// 	CRect rc;
// 	m_btnClose.GetWindowRect( &rc );
// 
// 	int y = 10;
// 	m_btnClose.MoveWindow( cx-10-rc.Width(), cy-y-rc.Height(),rc.Width(),rc.Height() );
// 	m_btnCancel.MoveWindow( cx-10-2*rc.Width()-15,cy-y-rc.Height(),rc.Width(),rc.Height() );
// 	m_btnFile.MoveWindow( cx-10-3*rc.Width()-2*15,cy-y-rc.Height(),rc.Width(),rc.Height() );
// 	
// 	// move progress ctrl
// 	y += 83;
// 	m_progSimStatus2.MoveWindow( 10, cy-y+30, cx-2*10,20 );
// 	y += 16;
// 	m_staticProgText2.MoveWindow( cx/2-60, cy-y+30, 120, 14);
// 
// 	y += 27;
// 	m_progSimStatus.MoveWindow( 10, cy-y+30, cx-2*10,20 );
// 	y += 16;
// 	m_staticProgText.MoveWindow( cx/2-60, cy-y+30, 120, 14);
// 
// 	// move status edit
// 	y += 65;
// 	m_editEventNum.MoveWindow( 40, cy-y+50, 120, 25 );
// 	m_editEventTime.MoveWindow( cx/2-60, cy-y+50, 120,25 );
// 	m_editEventType.MoveWindow( cx-40-120,cy-y+50, 120,25 );
// 
// 	y += 15;
// 	m_butNumber.MoveWindow( 40, cy-y+50, 80,14);
// 	m_butTime.MoveWindow( cx/2-60, cy-y+50, 80,14 );
// 	m_butType.MoveWindow( cx-40-120,cy-y+50, 80,14 );
// 
// 	// move list box and list ctrl
// 	int cyHeight = cy-265;
// 	y += 15;
// //	m_butFrame1.MoveWindow( 10,0, cx-2*10,cy-170);
// 	m_butFrame1.MoveWindow( 10,0, cx-2*10,cyHeight/2-10);
// 	m_butFrame2.MoveWindow( 18, cy-y+50 ,cx-2*18, 65 );
// 	
// 
// 	y += cyHeight/2;
// 	m_wndErrorMsgPanel.MoveWindow( 18, cy-y-10, cx-2*18, 426 );
// //	m_wndErrorMsgPanel.MoveWindow( 18, cy-y-10, cx-2*18, cyHeight/2+60 );
// 	//m_wndErrorMsgPanel.ResizeWindow(18, cy-y-10, cx-2*18, cyHeight/2+60 );
// 	//m_wndErrorMsgPanel.Reposition();
// 	y += 15;
// 	m_staticErrorMsg.MoveWindow( 18,cy-y-10, cx-2*18, 14 );
// 
// 	m_listboxSimStatus.MoveWindow( 18,35, cx-2*18,cyHeight/2-50 );
// 	
// 	InvalidateRect(NULL);
// }
// 
// set the min size of the dialog
void CSimEngineDlg::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	// TODO: Add your message handler code here and/or call default
	lpMMI->ptMinTrackSize.x = 600;
	lpMMI->ptMinTrackSize.y = 600;

	CXTResizeDialog::OnGetMinMaxInfo(lpMMI);
}

BOOL CSimEngineDlg::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	
	return CXTResizeDialog::OnEraseBkgnd(pDC);
}


void CSimEngineDlg::OnPaint() 
{
	CXTResizeDialog::OnPaint();
// 	CPaintDC dc(this); // device context for painting
// 	
// 	HWND hwndT=::GetWindow(m_hWnd, GW_CHILD);
// 	CRect rectCW;
// 	CRgn rgnCW;
// 	rgnCW.CreateRectRgn(0,0,0,0);
// 	while (hwndT != NULL)
// 	{
// 		CWnd* pWnd=CWnd::FromHandle(hwndT)  ;
// 		if( /*pWnd->IsKindOf(RUNTIME_CLASS(CListBox)) &&*/ pWnd->IsWindowVisible() )
// 		{
// 			pWnd->GetWindowRect(rectCW);
// 			ScreenToClient(rectCW);
// 			CRgn rgnTemp;
// 			rgnTemp.CreateRectRgnIndirect(rectCW);
// 			rgnCW.CombineRgn(&rgnCW,&rgnTemp,RGN_OR);
// 		}
// 		hwndT=::GetWindow(hwndT,GW_HWNDNEXT);
// 		
// 	}
// 	CRect rect;
// 	GetClientRect(&rect);
// 	CRgn rgn;
// 	rgn.CreateRectRgnIndirect(rect);
// 	CRgn rgnDraw;
// 	rgnDraw.CreateRectRgn(0,0,0,0);
// 	rgnDraw.CombineRgn(&rgn,&rgnCW,RGN_DIFF);
// 	CBrush br(GetSysColor(COLOR_BTNFACE));
// 	dc.FillRgn(&rgnDraw,&br);
// 	CRect rectRight=rect;
// 	rectRight.left=rectRight.right-10;
// 	dc.FillRect(rectRight,&br);
// 	rectRight=rect;
// 	rectRight.top=rect.bottom-44;
// 	dc.FillRect(rectRight,&br);
}

void CSimEngineDlg::OnColumnclickListErrorMsg(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	int nTestIndex = pNMListView->iSubItem;
	if( nTestIndex >= 0 )
	{
		CWaitCursor	wCursor;
		if(::GetKeyState( VK_CONTROL ) < 0 ) // Is <CTRL> Key Down
			m_ctlHeaderCtrl.SortColumn( nTestIndex, MULTI_COLUMN_SORT );
		else
			m_ctlHeaderCtrl.SortColumn( nTestIndex, SINGLE_COLUMN_SORT );
		m_ctlHeaderCtrl.SaveSortList();
	}	
	*pResult = 0;
}

void CSimEngineDlg::SetUserMsgOwner(CWnd *pWnd)
{
	m_pUserMsgOwner=pWnd;
}

void CSimEngineDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	if(m_pUserMsgOwner)
	{
		EndDialog(2);
		((CXTResizeDialog*)m_pUserMsgOwner)->EndDialog(1);
	}
	else
	CXTResizeDialog::OnClose();
}

void CSimEngineDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	if(m_pUserMsgOwner)
	{
		EndDialog(2);
		((CXTResizeDialog*)m_pUserMsgOwner)->EndDialog(1);
	}
	else
	CXTResizeDialog::OnCancel();
}

void CSimEngineDlg::clearDiagnoseEntryList( void )
{
	for( int i=0; i< static_cast<int>(m_vDiagnoseEntryList.size()); i++ )
	{
		if( m_vDiagnoseEntryList[i] )
			delete m_vDiagnoseEntryList[i];
	}
	
	m_vDiagnoseEntryList.clear();
}

void CSimEngineDlg::reloadLogIfNeed( const DiagnoseEntry* _pEntry )
{
	Terminal* _pTerm = GetEngine()->getTerminal();
	
	static long _iCallCount=0l;
	if( _iCallCount++ == 0 )	// the first call the function
	{
		_pTerm->clearLogs();
	}

	if( _pEntry->iSimResultIndex == _pTerm->getCurrentSimResult() )
		return;

	_pTerm->clearLogs();
	_pTerm->setCurrentSimResult( _pEntry->iSimResultIndex );
	_pTerm->GetSimReportManager()->SetCurrentSimResult( _pEntry->iSimResultIndex );
}

void CSimEngineDlg::ClearText()
{
	if (::IsWindow(m_listboxSimStatus.m_hWnd)/* && ::IsWindow(m_listErrorMsg.m_hWnd)*/)
	{
		m_listboxSimStatus.ResetContent();
		//m_listErrorMsg.DeleteAllItems();
		m_wndErrorMsgPanel.ClearErrorMessage();
		m_btnCancel.EnableWindow(true);
	}
	
}
void CSimEngineDlg::OnBnClickedCancel()
{
	OnCancel();
}

//BOOL CSimEngineDlg::Create(LPCTSTR lpszTemplateName, CWnd* pParentWnd)
//{
//	
//	return CXTResizeDialog::Create(lpszTemplateName, pParentWnd);
//}

BOOL CSimEngineDlg::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Add your specialized code here and/or call the base class
	cs.style |= WS_VISIBLE|WS_MAXIMIZE;
	return CXTResizeDialog::PreCreateWindow(cs);
}

LRESULT CSimEngineDlg::OnSimSortEventLog( WPARAM wParam, LPARAM lParam )
{
	MSG msg;
	// Handle dialog messages
	while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		if(!IsDialogMessage(&msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);  
		}
	}

	int nNewPos = (int)wParam;
	std::vector<int>* vParams = (std::vector<int> *)lParam;

	if(  nNewPos )
	{

		m_progSimStatus.SetPos( nNewPos );
		if(m_pUserMsgOwner)
			m_pUserMsgOwner->SendMessage(UM_SimEngineDlg_ProgressBar_SetPos,nNewPos);

		CString csProg;
		csProg.Format( "Run Number %d  ( % 3d%% ) - Sorting File %d of %d", m_nRunNumber, (*vParams)[0], (*vParams)[1], (*vParams)[2] );
		m_staticProgText.SetWindowText( csProg );
		m_progSimStatus.Invalidate();
	}

	//if( lParam != 0 )
	//{
	//	CString str( (char*)lParam );
	//	int index = m_listboxSimStatus.AddString( str );
	//	if( index > 10 )
	//		m_listboxSimStatus.SetTopIndex( index - 7 );
	//}
	return 0;

}
