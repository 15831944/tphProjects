// FlightScheduleAddNewDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "FlightScheduleAddNewDlg.h"
#include "inputs\schedule.h"
#include "inputs\GatePriorityInfo.h"
#include "inputs\FlightScheduleCriteriaDB.h"
#include "common\ProbDistManager.h"
#include "inputs\probab.h"
#include "FlightDialog.h"
#include "TermPlanDoc.h"
#include "common\winmsg.h"
#include "DlgProbDist.h"
#include <CommonData/procid.h>
#include <Common/ProbabilityDistribution.h>
//#include "Inputs\GateAssignmentMgr.h"

/////////////////////////////////////////////////////////////////////////////
// CFlightScheduleAddNewDlg dialog


CFlightScheduleAddNewDlg::CFlightScheduleAddNewDlg(InputTerminal* _pTerm,CTermPlanDoc* _pDoc,CWnd* pParent /*=NULL*/)
	: CDialog(CFlightScheduleAddNewDlg::IDD, pParent)
	,m_pTerm ( _pTerm ),m_bAutoAssign( false ),m_bReAssign( false ),m_pDoc( _pDoc )
	,m_iRowIdx( -1 ),m_iColumnIdx( -1 ),m_gateAssignmentMgr(_pDoc->GetProjectID())
{
	//{{AFX_DATA_INIT(CFlightScheduleAddNewDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
//	m_pAllSchduleUtil = _pTerm->m_pFlightScheduleCriteriaDB->GetScheduleUtils();
}


void CFlightScheduleAddNewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFlightScheduleAddNewDlg)
	DDX_Control(pDX, IDC_CHECK_REASSIGN, m_checkReAssign);
	DDX_Control(pDX, IDC_CHECK_AUTOASSIGN, m_checkAutoAssign);
	DDX_Control(pDX, IDC_STATIC_TOOLBAR, m_toolbarcontenter);
	DDX_Control(pDX, IDC_LIST_VALUE, m_listProperty);
	DDX_Control(pDX, IDSAVE, m_btSave);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFlightScheduleAddNewDlg, CDialog)
	//{{AFX_MSG_MAP(CFlightScheduleAddNewDlg)
	ON_BN_CLICKED(IDSAVE, OnSave)
	ON_BN_CLICKED(IDC_CHECK_AUTOASSIGN, OnCheckAutoassign)
	ON_BN_CLICKED(IDC_CHECK_REASSIGN, OnCheckReassign)	
	ON_WM_CREATE()
	ON_COMMAND(ID_TOOLBARBUTTONADD, OnToolbarbuttonadd)
	ON_COMMAND(ID_TOOLBARBUTTONDEL, OnToolbarbuttondel)	
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_VALUE, OnItemchangedListValue)
	//}}AFX_MSG_MAP
	ON_WM_SIZE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFlightScheduleAddNewDlg message handlers

BOOL CFlightScheduleAddNewDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	SetWindowText(_T("Schedule Modification Criteria"));
	m_bAutoAssign = m_pTerm->m_pFlightScheduleCriteriaDB->GetAutoAssignFlag(); 
	m_bReAssign = m_pTerm->m_pFlightScheduleCriteriaDB->GetReAssignFlag();
	if( m_bAutoAssign )
	{
		m_checkAutoAssign.SetCheck( 1 );
	}
	else
	{
		m_checkAutoAssign.SetCheck( 0 );
	}

	if( m_bReAssign )
	{
		m_checkReAssign.SetCheck( 1 );
	}
	else
	{
		m_checkReAssign.SetCheck( 0 );
	}
	SetListCtrl();
	ReloadListData();
	InitToolbar();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFlightScheduleAddNewDlg::OnSave() 
{
	CWaitCursor temp;
	m_pTerm->m_pFlightScheduleCriteriaDB->SetAutoAssignFlag( m_bAutoAssign ); 
	m_pTerm->m_pFlightScheduleCriteriaDB->SetReAssignFlag( m_bReAssign );
	std::vector<Flight*>vNewFlighs;
	int iCount = m_pTerm->m_pFlightScheduleCriteriaDB->GetCount();
	for( int i=0; i<iCount; ++i )
	{
		m_pTerm->m_pFlightScheduleCriteriaDB->GetItem(i)->BuildNewFlight( vNewFlighs );
	}

	int iNewCount = vNewFlighs.size();
	for(int i=0; i<iNewCount; ++i )
	{
		m_pTerm->flightSchedule->addItem( vNewFlighs[i] );
	}

	m_pTerm->m_pFlightScheduleCriteriaDB->saveDataSet( m_pDoc->m_ProjInfo.path, false );
	m_pTerm->flightSchedule->saveDataSet( m_pDoc->m_ProjInfo.path,false );
	m_btSave.EnableWindow( FALSE );

}


void CFlightScheduleAddNewDlg::OnCheckAutoassign() 
{
	m_bAutoAssign = !m_bAutoAssign;	
	m_btSave.EnableWindow();
}

void CFlightScheduleAddNewDlg::OnCheckReassign() 
{
	m_bReAssign = !m_bReAssign;	
	m_btSave.EnableWindow();
}



void CFlightScheduleAddNewDlg::OnOK() 
{
	if( m_btSave.IsWindowEnabled() )
	{
		OnSave();

		if( m_bReAssign )
		{	
			m_gateAssignmentMgr.LoadData(m_pTerm , m_pDoc->m_ProjInfo);
			//m_gateAssignmentMgr.UnassignAllFlight();
			GatePriorityAssign( true );
			m_gateAssignmentMgr.Save(m_pTerm,m_pDoc->m_ProjInfo.path);	
			m_pTerm->flightSchedule->loadDataSet( m_pDoc->m_ProjInfo.path );
			CDialog::OnOK();	
			return;
		}

		if( m_bAutoAssign )
		{
			m_gateAssignmentMgr.LoadData(m_pTerm, m_pDoc->m_ProjInfo);
			GatePriorityAssign( false );
			m_gateAssignmentMgr.Save(m_pTerm,m_pDoc->m_ProjInfo.path);	
			m_pTerm->flightSchedule->loadDataSet( m_pDoc->m_ProjInfo.path );
		}
	}

	CDialog::OnOK();
}
void CFlightScheduleAddNewDlg::GatePriorityAssign( bool _bReAssign )
{
	//first select the assigned flights 
	//and remove them from the gantt chart.
	int i = 0;
	int j = 0;
	int k = 0;
	int nGateCount = m_gateAssignmentMgr.GetGateCount();
	
	if( _bReAssign )
	{
		for(i = 0; i < nGateCount; i++)
		{
			CAssignGate* pGate = m_gateAssignmentMgr.GetGate(i);
			int nFlightCount = pGate->GetFlightCount();
			for(j = 0; j < nFlightCount; j ++)
			{
				pGate->SetFlightSelected(j, true);
			}		
		}
		// clear the gate assign flight
		m_gateAssignmentMgr.RemoveSelectedFlight();	
	}
	//make all the unassigned flights in status: 'selected'
	int unassignedCount = m_gateAssignmentMgr.GetUnassignedScheduleFlightList().size();
	for(i = 0; i < unassignedCount; i++)
	{
		m_gateAssignmentMgr.SetUnAssignedFlightSelected(i, 1);
	}

	m_gateAssignmentMgr.SortUnAssignedFlight();
	//int nFltCount = m_gateAssignmentMgr.GetUnassignedScheduleFlightList().size();

	GatePriorityInfo *gatePriorityInfo = m_pTerm->gatePriorityInfo;
	ALTObjectIDList &procIDList = gatePriorityInfo->altObjectIDList;

	//for each GATE in the procIDList
	int nListCount = (int)procIDList.size();
	std::vector<int> vectProcessor;
	for(i = 0; i < nListCount; i++)
	{
		ALTObjectID pID = procIDList.at(i);
		for(j = 0; j < nGateCount; j++)
		{
			CAssignStand* pGate = (CAssignStand*)m_gateAssignmentMgr.GetGate(j);
			if(pID.idFits(pGate->GetStandID()))
				vectProcessor.push_back(j);
		}
	}

	for (int j =0; j < unassignedCount; j++)
	{
		if (m_gateAssignmentMgr.AssignFlightAccordingToGatePrioritySequence(j,vectProcessor))
		{
			j--;
			unassignedCount--;
		}
	}

	//if still remaining unassigned flights
	//un-select them;
	//make all the unassigned flights in status: 'selected'
	unassignedCount = m_gateAssignmentMgr.GetUnassignedScheduleFlightList().size();
	for(i = 0; i < unassignedCount; i++)
	{
		m_gateAssignmentMgr.SetUnAssignedFlightSelected(i, 0);
	}	
	
	m_gateAssignmentMgr.SetAssignedFlightSelected(-1, -1, true);	
}

void CFlightScheduleAddNewDlg::SetListCtrl()
{
	DWORD dwStyle = m_listProperty.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_listProperty.SetExtendedStyle( dwStyle );

	LV_COLUMNEX	lvc;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT ;

	// set columns' header
	char columnLabel[7][32];
	sprintf( columnLabel[0], "Flight Type " );
	strcpy( columnLabel[1], "Change(%)" );
	strcpy( columnLabel[2], "Time offset(mins)" );
	strcpy( columnLabel[3], "Start Day" );
	strcpy( columnLabel[4], "Start Time" );
	strcpy( columnLabel[5], "End Day" );
	strcpy( columnLabel[6], "End Time" );


	int DefaultColumnWidth[7] = {	150,110,110,
									70,110,70,
									110
									};
	int nFormat[7] = {	LVCFMT_CENTER, LVCFMT_CENTER ,LVCFMT_CENTER,
						LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER,
						LVCFMT_CENTER
						};

	int lvcFmt[7] = {	LVCFMT_NOEDIT, LVCFMT_DROPDOWN, LVCFMT_DROPDOWN,
						LVCFMT_SPINEDIT, LVCFMT_DATETIME, LVCFMT_SPINEDIT,
						LVCFMT_DATETIME
						};
	for( int i=0; i<7; i++ )
	{
		CStringList strList;
		CString s;
		s.LoadString(IDS_STRING_NEWDIST);
		strList.InsertAfter( strList.GetTailPosition(), s );

		int nCount = _g_GetActiveProbMan( m_pTerm )->getCount();
		for( int m=0; m<nCount; m++ )
		{
			CProbDistEntry* pPBEntry = _g_GetActiveProbMan( m_pTerm )->getItem( m );			
			strList.InsertAfter( strList.GetTailPosition(), pPBEntry->m_csName );
		}
		lvc.csList = &strList;
		lvc.pszText = columnLabel[i];
		lvc.cx = DefaultColumnWidth[i];
		lvc.fmt = lvcFmt[i];

		if (3 == i || 5 == i)
		{
			// set range
			lvc.nParam1 = 1;
			lvc.nParam2 = 366;
		}
		else
		{
			lvc.nParam1 = -1;
			lvc.nParam2 = -1;
		}

		m_listProperty.InsertColumn( i, &lvc );
	}

	m_listProperty.SetSpinDisplayType(1);
}

void CFlightScheduleAddNewDlg::ReloadListData( CFlightScheduleAddUtil* _pSelect /*= NULL*/ )
{
	m_listProperty.DeleteAllItems();
	int iCount = m_pTerm->m_pFlightScheduleCriteriaDB->GetCount();
	//char tempChars[2560];
	for( int i=0; i<iCount; ++i )
	{
		CFlightScheduleAddUtil* pItemUitl = m_pTerm->m_pFlightScheduleCriteriaDB->GetItem( i );
		char sStr[1024] = {0};
		pItemUitl->GetFlightConstraint().screenPrint( sStr );


		int iItemIndex = m_listProperty.InsertItem(i,sStr );
		m_listProperty.SetItemData( iItemIndex, (DWORD)pItemUitl );

		//pItemUitl->GetChageDistribution()->printDistribution( tempChars );
		pItemUitl->GetChageDistribution()->screenPrint( sStr );

		m_listProperty.SetItemText(iItemIndex, 1 ,sStr );

		pItemUitl->GetTimeOffsetDistribution()->screenPrint( sStr );
		m_listProperty.SetItemText(iItemIndex, 2 ,sStr );

		CString sValue;
		ElapsedTime elapsedTime;


		elapsedTime = pItemUitl->GetStartTime();
		sValue.Format(_T("%d"), elapsedTime.GetDay());
		m_listProperty.SetItemText(iItemIndex, 3 ,sValue );
		sValue.Format(_T("%02d:%02d:%02d"), elapsedTime.GetHour(), elapsedTime.GetMinute(), elapsedTime.GetSecond());
		m_listProperty.SetItemText(iItemIndex, 4 ,sValue );


		elapsedTime = pItemUitl->GetEndTime();
		sValue.Format(_T("%d"), elapsedTime.GetDay());
		m_listProperty.SetItemText(iItemIndex, 5 ,sValue );
		sValue.Format(_T("%02d:%02d:%02d"), elapsedTime.GetHour(), elapsedTime.GetMinute(), elapsedTime.GetSecond());
		m_listProperty.SetItemText(iItemIndex, 6 ,sValue );
	}
}
void CFlightScheduleAddNewDlg::InitToolbar()
{
	CRect rc;
	m_toolbarcontenter.GetWindowRect( &rc );
	ScreenToClient(&rc);
	m_ToolBar.MoveWindow(&rc);
	m_ToolBar.ShowWindow(SW_SHOW);
	m_toolbarcontenter.ShowWindow(SW_HIDE);
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONADD );
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL,FALSE );
	m_ToolBar.GetToolBarCtrl().HideButton( ID_TOOLBARBUTTONEDIT );
}

int CFlightScheduleAddNewDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
// TODO: Add your specialized creation code here
	if (!m_ToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_ToolBar.LoadToolBar(IDR_ADDDELEDITTOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}	// TODO: Add your specialized creation code here
	
	return 0;
}

void CFlightScheduleAddNewDlg::OnToolbarbuttonadd() 
{
	CFlightDialog dlg( m_pParentWnd );
	if( dlg.DoModal() == IDOK )
	{
		CFlightScheduleAddUtil* pNewScheduleUtil = new CFlightScheduleAddUtil( m_pTerm );
		ConstantDistribution* pChangeDistribution = new ConstantDistribution(0.0);
		ConstantDistribution* pTimeOffsetDistribution = new ConstantDistribution(0.0);
		pNewScheduleUtil->SetChageDistribution( pChangeDistribution );
		pNewScheduleUtil->SetTimeOffsetDistribution( pTimeOffsetDistribution );
		pNewScheduleUtil->SetFlightConstraint( dlg.GetFlightSelection() );

		m_pTerm->m_pFlightScheduleCriteriaDB->AddNewItem( pNewScheduleUtil );

		ReloadListData();	
	}
}

void CFlightScheduleAddNewDlg::OnToolbarbuttondel() 
{
	int iSelectItemIdx = m_listProperty.GetCurSel();
	CFlightScheduleAddUtil* pUtil = (CFlightScheduleAddUtil*)m_listProperty.GetItemData( iSelectItemIdx );
	m_pTerm->m_pFlightScheduleCriteriaDB->RemoveItem( pUtil );
	m_btSave.EnableWindow();
	ReloadListData();
}

LRESULT CFlightScheduleAddNewDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if (message == WM_INPLACE_COMBO_KILLFOUCUS)
	{
		LV_DISPINFO* pInfo = (LV_DISPINFO*) lParam;

		int iSub = pInfo->item.iSubItem;
		int iItem = pInfo->item.iItem;
		int iSel = (int)wParam;
		CFlightScheduleAddUtil* pUtil = (CFlightScheduleAddUtil*)m_listProperty.GetItemData( iItem );
		ASSERT( pUtil );
		/*
		CString str;
		str.Format( "%d,%d,%d", iItem, iSub, iSel );

		AfxMessageBox(str);
		*/
		if( iSel == 0 && (iSub == 1 || iSub == 2 ) )
		{
		//	AfxMessageBox("aa");
			CDlgProbDist dlg(m_pTerm->m_pAirportDB, true );
			if(dlg.DoModal()==IDOK) 
			{
				int idxPD = dlg.GetSelectedPDIdx();
				ASSERT(idxPD!=-1);
				CProbDistEntry* pde = _g_GetActiveProbMan( m_pTerm )->getItem(idxPD);
				m_listProperty.SetItemText( iItem, iSub, pde->m_csName );				
				ProbabilityDistribution* pDist = ProbabilityDistribution::CopyProbDistribution(pde->m_pProbDist);				

				iSub == 1? pUtil->ReplaceChangeDistribution( pDist ) : pUtil->ReplaceTimeoffsetDistribution( pDist );				
			}
		}
		else
		{
			ASSERT( iSel-1 >= 0 && iSel-1 < static_cast<int>(_g_GetActiveProbMan( m_pTerm )->getCount()) );
			CProbDistEntry* pPBEntry = _g_GetActiveProbMan( m_pTerm )->getItem( iSel-1 );
			ProbabilityDistribution* pDist = ProbabilityDistribution::CopyProbDistribution(pPBEntry->m_pProbDist);
		
			iSub == 1? pUtil->ReplaceChangeDistribution( pDist ) : pUtil->ReplaceTimeoffsetDistribution( pDist );			
		}

		m_btSave.EnableWindow();
		return TRUE;
	}
	else if( message == WM_INPLACE_DATETIME )
	{

		switch( wParam ) 
		{
			case 1:
				{
					LV_DISPINFO* pDispinfo=(LV_DISPINFO*)lParam;
					m_iRowIdx = pDispinfo->item.iItem;
					m_iColumnIdx = pDispinfo->item.iSubItem;
				}
				break;
			case 2:
				{
					CFlightScheduleAddUtil* pUtil = (CFlightScheduleAddUtil*)m_listProperty.GetItemData( m_iRowIdx );
					ASSERT( pUtil );
					COleDateTime *pTime = (COleDateTime *)lParam;
					if( m_iColumnIdx == 4 )//start time
					{
						ElapsedTime newTime(
							(pUtil->GetStartTime().GetDay() - 1)*24*3600l
							+ pTime->GetHour()*3600l
							+ pTime->GetMinute()*60l
							+ pTime->GetSecond()*1l
							);
						pUtil->SetStartTime( newTime );
					}
					else
					{
						ASSERT(6 == m_iColumnIdx); //end time
						ElapsedTime newTime(
							(pUtil->GetEndTime().GetDay() - 1)*24*3600l
							+ pTime->GetHour()*3600l
							+ pTime->GetMinute()*60l
							+ pTime->GetSecond()*1l
							);
						pUtil->SetEndTime( newTime );
					}

					m_btSave.EnableWindow();
				}
				break;	
		}
		return TRUE;
	}
	else if( message == WM_INPLACE_SPIN )
	{
		int column = -1;
		CFlightScheduleAddUtil* pUtil =
			(CFlightScheduleAddUtil*)m_listProperty.GetItemData( m_listProperty.GetLastDblClk(column) );
		ASSERT( pUtil );
		SPINTEXT* pSt =  (SPINTEXT*)lParam;
		if( 3 == column )//start time's day
		{
			ElapsedTime newTime( pUtil->GetStartTime() );
			newTime.SetDay(pSt->iPercent);
			pUtil->SetStartTime( newTime );
		}
		else
		{
			ASSERT(5 == column); //end time's day
			ElapsedTime newTime( pUtil->GetEndTime() );
			newTime.SetDay(pSt->iPercent);
			pUtil->SetEndTime( newTime );
		}

		m_btSave.EnableWindow();
		return TRUE;

	}
	
	return CDialog::DefWindowProc(message, wParam, lParam);
}

void CFlightScheduleAddNewDlg::OnCancel() 
{
	m_pTerm->m_pFlightScheduleCriteriaDB->loadDataSet( m_pDoc->m_ProjInfo.path );
	CDialog::OnCancel();
}

/*
 *	
 void CFlightScheduleAddNewDlg::OnInplaceDateTime( WPARAM wParam, LPARAM lParam )
{
	switch( wParam ) 
	{
	case 1:
		{
			LV_DISPINFO* pDispinfo=(LV_DISPINFO*)lParam;
			m_iRowIdx = pDispinfo->item.iItem;
			m_iColumnIdx = pDispinfo->item.iSubItem;
		}
		break;
	case 2:
		{
			COleDateTime *pTime = (COleDateTime *)lParam;
			ElapsedTime newTime( pTime->GetHour()*3600l + pTime->GetMinute()*60l + pTime->GetSecond()*1l );
		}
		break;	
	}
}


*/


void CFlightScheduleAddNewDlg::OnItemchangedListValue(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL );
	*pResult = 0;
}


void CFlightScheduleAddNewDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	//return;

	if(m_listProperty.GetSafeHwnd() == NULL)
		return;

	// TODO: Add your message handler code here
	//the frame 
	CRect rcWindow;
	GetClientRect(&rcWindow);
	
	CWnd *pWndFrame = GetDlgItem(IDC_STATIC_MAINFRAME);
	CRect rcFrameClient;
	pWndFrame->GetWindowRect(&rcFrameClient);
	ScreenToClient(rcFrameClient);
	pWndFrame->MoveWindow(rcFrameClient.left, rcFrameClient.top,rcWindow.Width() - 20, rcWindow.Height() - 45);

	//toolbar
	CRect rcToolBar;
	m_ToolBar.GetWindowRect(&rcToolBar);
	ScreenToClient(&rcToolBar);
	m_ToolBar.MoveWindow(rcToolBar.left, rcToolBar.top,rcWindow.Width() - 20,30);

	//list
	CRect rcList;
	m_listProperty.GetWindowRect(&rcList);
	ScreenToClient(&rcList);
	m_listProperty.MoveWindow(rcList.left, rcList.top,rcWindow.Width() - 20, rcWindow.Height() - 45 - 40);

	//check box
	CRect rcAutoAssignChkBox;
	m_checkAutoAssign.GetWindowRect(&rcAutoAssignChkBox);
	ScreenToClient(&rcAutoAssignChkBox);
	m_checkAutoAssign.MoveWindow(rcAutoAssignChkBox.left, rcWindow.bottom - 40, rcAutoAssignChkBox.Width(), rcAutoAssignChkBox.Height());

	CRect rcReassignGates;
	m_checkReAssign.GetWindowRect(&rcReassignGates);
	ScreenToClient(&rcReassignGates);
	m_checkReAssign.MoveWindow(rcReassignGates.left, rcWindow.bottom - 40, rcReassignGates.Width(), rcReassignGates.Height());


	//Save
	CRect rcSave;
	m_btSave.GetWindowRect(&rcSave);
	ScreenToClient(&rcSave);
	m_btSave.MoveWindow(rcWindow.right - 280, rcWindow.bottom - 30, rcSave.Width(), rcSave.Height());

	//IDOK
	CRect rcOK;
	GetDlgItem(IDOK)->GetWindowRect(&rcOK);
	ScreenToClient(&rcOK);
	GetDlgItem(IDOK)->MoveWindow(rcWindow.right - 190, rcWindow.bottom - 30, rcOK.Width(), rcOK.Height());
	//ID Cancel
	CRect rcCancel;
	GetDlgItem(IDCANCEL)->GetWindowRect(&rcCancel);
	ScreenToClient(&rcCancel);
	GetDlgItem(IDCANCEL)->MoveWindow(rcWindow.right - 100, rcWindow.bottom - 30, rcCancel.Width(), rcCancel.Height());



}
