// FltPaxDataDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "FltPaxDataDlg.h"
#include "..\inputs\con_db.h"
#include "..\inputs\fltdata.h"
#include "..\inputs\paxdata.h"
#include "..\inputs\probab.h"
#include "termplandoc.h"
#include "passengertypedialog.h"
#include "flightdialog.h"
#include "..\common\probdistmanager.h"
#include "ProbDistDlg.h"
#include "ImportFlightFromFileDlg.h"
#include "../InputAirside/InputAirside.h"
#include ".\fltpaxdatadlg.h"
#include "..\Inputs\BridgeConnectorPaxData.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CFltPaxDataDlg dialog


CFltPaxDataDlg::CFltPaxDataDlg( enum FLTPAXDATATTYPE _enumType, CWnd* pParent)
	: CDialog(CFltPaxDataDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFltPaxDataDlg)
	//}}AFX_DATA_INIT
	m_enumType = _enumType;

	m_nListPreIndex = -1;
}


void CFltPaxDataDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFltPaxDataDlg)
	DDX_Control(pDX, IDC_STATIC_TOOLBARCONTENTER, m_toolbarcontenter);
	DDX_Control(pDX, IDC_BUTTON_SAVE, m_btnSave);
	DDX_Control(pDX, IDC_LIST_DATA, m_listctrlData);
	DDX_Control(pDX, IDC_STATIC_FRAME, m_Framebarcontenter);
	DDX_Control(pDX, IDOK, m_btnOk);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDC_DELAY_IMPORT, m_btnImport);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_CHECK_NEGLECTSCHEDDATA, m_btnNeglectSchedData);
}


BEGIN_MESSAGE_MAP(CFltPaxDataDlg, CDialog)
	//{{AFX_MSG_MAP(CFltPaxDataDlg)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_DATA, OnEndlabeleditListData)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_DATA, OnItemchangedListData)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_COMMAND(ID_TOOLBARBUTTONADD, OnToolbarbuttonadd)
	ON_COMMAND(ID_TOOLBARBUTTONDEL, OnToolbarbuttondel)
	ON_COMMAND(ID_TOOLBARBUTTONEDIT, OnToolbarbuttonedit)
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_DELAY_IMPORT, OnDelayImport)
	//}}AFX_MSG_MAP
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXT, 0, 0xFFFF, OnToolTipText)
	ON_BN_CLICKED(IDC_CHECK_NEGLECTSCHEDDATA, OnBnClickedCheckNeglectscheddata)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFltPaxDataDlg message handlers

BOOL CFltPaxDataDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	AfxGetApp()->BeginWaitCursor();

	InitToolbar();

	switch( m_enumType )
	{
	case FLIGHT_DELAYS:
		SetWindowText( "Flight Delays" );
		break;

	case FLIGHT_LOAD_FACTORS:
		{
			SetWindowText( "Passenger Loads" );
			m_btnNeglectSchedData.ShowWindow(SW_SHOW);
			m_btnNeglectSchedData.SetWindowText("Ignore load factor in flight schedule");
			break;
		}


	case FLIGHT_AC_CAPACITIES:
		{
			SetWindowText( "Aircraft Capacity" );
			m_btnNeglectSchedData.ShowWindow(SW_SHOW);
			m_btnNeglectSchedData.SetWindowText("Ignore capacity in flight schedule");
			break;
		}


	//case PAX_BAG_COUNT:
	//	SetWindowText( "Bag Count" );
	//	break;

	//case PAX_CART_COUNT:
	//	SetWindowText( "Cart Count" );
	//	break;

	case PAX_GROUP_SIZE:
		SetWindowText( "Group Size" );
		break;

	case PAX_LEAD_LAG:
		SetWindowText( "Lead Lag Time" );
		break;

	case PAX_IN_STEP:
		SetWindowText( "In Step" );
		break;

	case PAX_SIDE_STEP:
		SetWindowText( "Side Step" );
		break;

	case PAX_SPEED:
		SetWindowText( "Speed" );
		break;

	//case PAX_VISITORS:
	//	SetWindowText( "Visitors" );
	//	break;

	case PAX_VISIT_TIME:
		SetWindowText( "Visit Time" );
		break;
		
	case PAX_RESPONSE_TIME:
		SetWindowText("Response Time");
		break;

	case PAX_SPEED_IMPACT:
		SetWindowText("Speed Impact");
		break;

	case PAX_PUSH_PROPENSITY:
		SetWindowText("Push Propensity");
		break;
		
	case VISITOR_STA_TRIGGER:
		SetWindowText("Visitor STA Trigger");
		break;
	case ENTRY_FLIGHT_TIME_DISTRIBUTION:
		{
			CString s;
			s.LoadString(IDS_TVNN_ENTRYFLIGHTTIMEDISTRIBUTION);
			SetWindowText(s);
			break;
		}
	}
	
	SetListCtrl();
	ReloadData( NULL );

	if (m_enumType == FLIGHT_LOAD_FACTORS || m_enumType == FLIGHT_AC_CAPACITIES)
	{
		ConstraintDatabase* pConDB = GetConstraintDatabase();
		bool bNeglect = ((FlightConstraintDatabaseWithSchedData*)pConDB)->IsNeglectSchedData();
		if (bNeglect)
			m_btnNeglectSchedData.SetCheck(BST_CHECKED);
		else
			m_btnNeglectSchedData.SetCheck(BST_UNCHECKED);
	}

	AfxGetApp()->EndWaitCursor();

	InitTooltips();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CFltPaxDataDlg::SetListCtrl()
{
	// set multi selection (remove single selection)
	m_listctrlData.ModifyStyle(LVS_SINGLESEL | LVS_EDITLABELS, 0);

	// set list ctrl	
	DWORD dwStyle = m_listctrlData.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_listctrlData.SetExtendedStyle( dwStyle );

	LV_COLUMNEX	lvc;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT ;
	char columnLabel[2][128];
	ConstraintDatabase* pConDB = GetConstraintDatabase();
	sprintf( columnLabel[1], "Distribution ( %s )", pConDB->getUnits() );
	

	CString csLabel;
	switch( m_enumType )
	{
	case FLIGHT_DELAYS:
	case FLIGHT_LOAD_FACTORS:
	case FLIGHT_AC_CAPACITIES:
		strcpy( columnLabel[0], "Flight Type" );
		break;

	default:
		strcpy( columnLabel[0], "Mobile Element Type" );		
		break;
	}
	int DefaultColumnWidth[] = { 200, 320 };
	int nFormat[] = {	LVCFMT_CENTER, LVCFMT_CENTER };
	for( int i=0; i<2; i++ )
	{
		CStringList strList;
		CString s;
		s.LoadString( IDS_STRING_NEWDIST );
		strList.InsertAfter( strList.GetTailPosition(), s );
		int nCount = _g_GetActiveProbMan( GetInputTerminal() )->getCount();
		for( int m=0; m<nCount; m++ )
		{
			CProbDistEntry* pPBEntry = _g_GetActiveProbMan( GetInputTerminal() )->getItem( m );			
			strList.InsertAfter( strList.GetTailPosition(), pPBEntry->m_csName );
		}
		lvc.csList = &strList;
		lvc.pszText = columnLabel[i];
		lvc.cx = DefaultColumnWidth[i];
		if( i == 0 )
			lvc.fmt = LVCFMT_NOEDIT;
		else
			lvc.fmt = LVCFMT_DROPDOWN;
		m_listctrlData.InsertColumn( i, &lvc );
	}
	m_listctrlData.SetInputTerminal( GetInputTerminal() );
}


// reload data from database and select the item as parameter
void CFltPaxDataDlg::ReloadData( Constraint* _pSelCon )
{
	ConstraintDatabase* pConDB = GetConstraintDatabase();
	m_listctrlData.SetCurConDB( pConDB );
	m_listctrlData.ReloadData( _pSelCon );
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL,GetSelectedItem() != -1 );
//	m_btnDel.EnableWindow( GetSelectedItem() != -1 );
}


InputTerminal* CFltPaxDataDlg::GetInputTerminal()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
    return (InputTerminal*)&pDoc->GetTerminal();
}


CString CFltPaxDataDlg::GetProjPath()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	return pDoc->m_ProjInfo.path;
}

// add a new entry with default prob dist as constant 0
/*void CFltPaxDataDlg::OnButtonAdd() 
{
	bool bDirty = false;
	ConstraintDatabase* pConDB = GetConstraintDatabase();
	assert( pConDB );
	switch( m_enumType )
	{
	case FLIGHT_DELAYS:
	case FLIGHT_LOAD_FACTORS:
	case FLIGHT_AC_CAPACITIES:
	case FLIGHT_BOARDING_CALLS:
	{
		CFlightDialog dlg( m_pParentWnd );
		if( dlg.DoModal() == IDOK )
		{
			FlightConstraint newFltCon = dlg.GetFlightSelection();
			bDirty = m_listctrlData.AddEntry( FLIGHT_CON, &newFltCon );
		}
			
		break;
	}	

	default:
	{
		CPassengerTypeDialog dlg( m_pParentWnd );
		if (dlg.DoModal() == IDOK)
		{
			PassengerConstraint newPaxCon = dlg.GetPaxSelection();
			bDirty = m_listctrlData.AddEntry( PAX_CON, &newPaxCon );
		}
		break;
	}
	}

	if( bDirty )
		m_btnSave.EnableWindow();
}/-*/

/*
void CFltPaxDataDlg::OnButtonDel() 
{
	// TODO: Add your control notification handler code here
	if( m_listctrlData.DeleteEntry() )
		m_btnSave.EnableWindow();
}*/

void CFltPaxDataDlg::OnButtonSave() 
{
	// TODO: Add your control notification handler code here
	switch( m_enumType )
	{
	case FLIGHT_DELAYS:
		GetInputTerminal()->flightData->saveDataSet( GetProjPath(), true );
		break;
	case FLIGHT_LOAD_FACTORS:
		GetInputTerminal()->flightData->saveDataSet( GetProjPath(), true );
		break;
	case FLIGHT_AC_CAPACITIES:
		GetInputTerminal()->flightData->saveDataSet( GetProjPath(), true );
		break;
	//case PAX_BAG_COUNT:
	//	GetInputTerminal()->paxDataList->saveDataSet( GetProjPath(), true );
	//	break;
	//case PAX_CART_COUNT:
	//	GetInputTerminal()->paxDataList->saveDataSet( GetProjPath(), true );
	//	break;
	case PAX_GROUP_SIZE:
		GetInputTerminal()->paxDataList->saveDataSet( GetProjPath(), true );
		break;
	case PAX_LEAD_LAG:
		GetInputTerminal()->paxDataList->saveDataSet( GetProjPath(), true );
		break;
	case PAX_IN_STEP:
		GetInputTerminal()->paxDataList->saveDataSet( GetProjPath(), true );
		break;
	case PAX_SIDE_STEP:
		GetInputTerminal()->paxDataList->saveDataSet( GetProjPath(), true );
		break;
	case PAX_SPEED:
		GetInputTerminal()->paxDataList->saveDataSet( GetProjPath(), true );
		break;
	//case PAX_VISITORS:
	//	GetInputTerminal()->paxDataList->saveDataSet( GetProjPath(), true );
	//	break;
	case PAX_VISIT_TIME:
		GetInputTerminal()->paxDataList->saveDataSet( GetProjPath(), true );
		break;
	case PAX_RESPONSE_TIME:
		GetInputTerminal()->paxDataList->saveDataSet( GetProjPath(), true );
		break;
	case PAX_SPEED_IMPACT:
		GetInputTerminal()->paxDataList->saveDataSet( GetProjPath(), true );
		break;
	case PAX_PUSH_PROPENSITY:
		GetInputTerminal()->paxDataList->saveDataSet( GetProjPath(), true );
		break;
	case VISITOR_STA_TRIGGER:
		GetInputTerminal()->paxDataList->saveDataSet( GetProjPath(), true );
		break;
	case ENTRY_FLIGHT_TIME_DISTRIBUTION:
		GetInputTerminal()->bcPaxData->saveDataSet( GetProjPath(), true );
		break;
	}
	m_btnSave.EnableWindow( FALSE );
}

void CFltPaxDataDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	if( !m_btnSave.IsWindowEnabled() )  // nonzero if the window was previously disabled
	{
		CDialog::OnCancel();
		return;
	}
	AfxGetApp()->BeginWaitCursor();

	try
	{
		switch( m_enumType )
		{
		case FLIGHT_DELAYS:
			GetInputTerminal()->flightData->loadDataSet( GetProjPath() );
			break;
		case FLIGHT_LOAD_FACTORS:
			GetInputTerminal()->flightData->loadDataSet( GetProjPath() );
			break;
		case FLIGHT_AC_CAPACITIES:
			GetInputTerminal()->flightData->loadDataSet( GetProjPath() );
			break;
		//case PAX_BAG_COUNT:
		//	GetInputTerminal()->paxDataList->loadDataSet( GetProjPath() );
		//	break;
		//case PAX_CART_COUNT:
		//	GetInputTerminal()->paxDataList->loadDataSet( GetProjPath() );
		//	break;
		case PAX_GROUP_SIZE:
			GetInputTerminal()->paxDataList->loadDataSet( GetProjPath() );
			break;
		case PAX_LEAD_LAG:
			GetInputTerminal()->paxDataList->loadDataSet( GetProjPath() );
			break;
		case PAX_IN_STEP:
			GetInputTerminal()->paxDataList->loadDataSet( GetProjPath() );
			break;
		case PAX_SIDE_STEP:
			GetInputTerminal()->paxDataList->loadDataSet( GetProjPath() );
			break;
		case PAX_SPEED:
			GetInputTerminal()->paxDataList->loadDataSet( GetProjPath() );
			break;
		//case PAX_VISITORS:
		//	GetInputTerminal()->paxDataList->loadDataSet( GetProjPath() );
		//	break;
		case PAX_VISIT_TIME:
			GetInputTerminal()->paxDataList->loadDataSet( GetProjPath() );
			break;
		case PAX_RESPONSE_TIME:
			GetInputTerminal()->paxDataList->loadDataSet( GetProjPath() );
			break;
		case PAX_SPEED_IMPACT:
			GetInputTerminal()->paxDataList->loadDataSet( GetProjPath() );
			break;
		case PAX_PUSH_PROPENSITY:
			GetInputTerminal()->paxDataList->loadDataSet( GetProjPath() );
			break;
		case VISITOR_STA_TRIGGER:
			GetInputTerminal()->paxDataList->loadDataSet( GetProjPath() );
			break;
		case ENTRY_FLIGHT_TIME_DISTRIBUTION:
			GetInputTerminal()->bcPaxData->loadDataSet(GetProjPath());
			break;
		}
	}
	catch( FileVersionTooNewError* _pError )
	{
		char szBuf[128];
		_pError->getMessage( szBuf );
		MessageBox( szBuf, "Error", MB_OK|MB_ICONWARNING );
		delete _pError;			
	}
	
	
	AfxGetApp()->EndWaitCursor();

	CDialog::OnCancel();
}

void CFltPaxDataDlg::OnOK() 
{
	// TODO: Add extra validation here
	if( !m_btnSave.IsWindowEnabled() )  // nonzero if the window was previously disabled
	{
		CDialog::OnOK();
		return;
	}
	
	AfxGetApp()->BeginWaitCursor();
	OnButtonSave();
	AfxGetApp()->EndWaitCursor();
	
	CDialog::OnOK();
}


// despite different type database return a common condb pointer.
ConstraintDatabase* CFltPaxDataDlg::GetConstraintDatabase()
{
	ConstraintDatabase* pConDB = NULL;
	switch( m_enumType )
	{
	case FLIGHT_DELAYS:
		pConDB = GetInputTerminal()->flightData->getDelays();
		break;
	case FLIGHT_LOAD_FACTORS:
		pConDB = GetInputTerminal()->flightData->getLoads();
		break;
	case FLIGHT_AC_CAPACITIES:
		pConDB = GetInputTerminal()->flightData->getCapacity();
		break;
	//case PAX_BAG_COUNT:
	//	pConDB = GetInputTerminal()->paxDataList->getBags();	//GetMobileElement( int iIndex)
	//	break;
	//case PAX_CART_COUNT:
	//	pConDB = GetInputTerminal()->paxDataList->getCarts();
	//	break;
	case PAX_GROUP_SIZE:
		pConDB = GetInputTerminal()->paxDataList->getGroups();
		break;
	case PAX_LEAD_LAG:
		pConDB = GetInputTerminal()->paxDataList->getLeadLagTime();
		break;
	case PAX_IN_STEP:
		pConDB = GetInputTerminal()->paxDataList->getInStep();
		break;
	case PAX_SIDE_STEP:
		pConDB = GetInputTerminal()->paxDataList->getSideStep();
		break;
	case PAX_SPEED:
		pConDB = GetInputTerminal()->paxDataList->getSpeed();
		break;
	//case PAX_VISITORS:
	//	pConDB = GetInputTerminal()->paxDataList->getVisitors();
	//	break;
	case PAX_VISIT_TIME:
		pConDB = GetInputTerminal()->paxDataList->getVisitTime();
		break;
	case PAX_RESPONSE_TIME:
		pConDB = GetInputTerminal()->paxDataList->getResponseTime();
		break;
	case PAX_SPEED_IMPACT:
		pConDB = GetInputTerminal()->paxDataList->getImpactSpeed();
		break;
	case PAX_PUSH_PROPENSITY:
		pConDB = GetInputTerminal()->paxDataList->getPushPropensity();
		break;
	case VISITOR_STA_TRIGGER:
		pConDB = GetInputTerminal()->paxDataList->getVisitorSTATrigger();
		break;
	case ENTRY_FLIGHT_TIME_DISTRIBUTION:
		pConDB = GetInputTerminal()->bcPaxData->getEntryFlightTimeDestribution();
		break;
	}
	return pConDB;
}

void CFltPaxDataDlg::OnEndlabeleditListData(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here
 	LV_ITEM* plvItem = &pDispInfo->item;

	*pResult = 0;
	m_btnSave.EnableWindow();
}

void CFltPaxDataDlg::OnItemchangedListData(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	if( GetSelectedItem() != -1 )
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL );

	//	m_btnDel.EnableWindow();
	
	*pResult = 0;
}

int CFltPaxDataDlg::GetSelectedItem()
{
	int nCount = m_listctrlData.GetItemCount();
	for( int i=0; i<nCount; i++ )
	{
		if( m_listctrlData.GetItemState( i, LVIS_SELECTED ) == LVIS_SELECTED )
			return i;
	}
	return -1;
}

void CFltPaxDataDlg::InitToolbar()
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

int CFltPaxDataDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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
	}
	return 0;
}


void CFltPaxDataDlg::OnToolbarbuttonadd() 
{
	// TODO: Add your command handler code here
		bool bDirty = false;
	ConstraintDatabase* pConDB = GetConstraintDatabase();
	assert( pConDB );
	switch( m_enumType )
	{
	case FLIGHT_DELAYS:
	case FLIGHT_LOAD_FACTORS:
	case FLIGHT_AC_CAPACITIES:
	{
		CFlightDialog dlg( m_pParentWnd );
		if( dlg.DoModal() == IDOK )
		{
			FlightConstraint newFltCon = dlg.GetFlightSelection();
			bDirty = m_listctrlData.AddEntry( FLIGHT_CON, &newFltCon );
		}
			
		break;
	}	

	default:
	{
		CPassengerTypeDialog dlg( m_pParentWnd );
		if (dlg.DoModal() == IDOK)
		{
			//PassengerConstraint newPaxCon = dlg.GetPaxSelection();
			CMobileElemConstraint mobileConstr = dlg.GetMobileSelection();
			bDirty = m_listctrlData.AddEntry( PAX_CON, &mobileConstr );
		}
		break;
	}
	}

	if( bDirty )
		m_btnSave.EnableWindow();

}

void CFltPaxDataDlg::OnToolbarbuttondel() 
{
	// TODO: Add your command handler code here
	if( m_listctrlData.DeleteMultiEntry() )
		m_btnSave.EnableWindow();
}

void CFltPaxDataDlg::OnToolbarbuttonedit() 
{
	// TODO: Add your command handler code here
	
}

void CFltPaxDataDlg::InitTooltips()
{
	m_toolTips.InitTooltips( &m_listctrlData);
}

BOOL CFltPaxDataDlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	m_toolTips.RelayEvent( pMsg );

	return CDialog::PreTranslateMessage(pMsg);
}

void CFltPaxDataDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	int nIndex = m_listctrlData.HitTest( point );

	if( nIndex!=-1 )
	{
		if(m_nListPreIndex != nIndex)
		{
			int iItemData = m_listctrlData.GetItemData( nIndex );
			ShowTips( iItemData );
			m_nListPreIndex = nIndex;
		}
	}
	else
	{
		m_nListPreIndex = -1;
		m_toolTips.DelTool( &m_listctrlData );
	}
	
	CDialog::OnMouseMove(nFlags, point);
}

void CFltPaxDataDlg::ShowTips(int iItemData)
{
	if( iItemData >= 0 )
	{
		ConstraintEntry* pEntry = m_listctrlData.GetConDB()->getItem( iItemData);
		const Constraint* pCon = pEntry->getConstraint();

		CString strTips ;
		pCon->screenTips( strTips );
		m_toolTips.ShowTips( strTips );
	}
}

void CFltPaxDataDlg::OnDelayImport() 
{
	// TODO: Add your control notification handler code here
	
	//CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	//std::vector<int> vAirports;
	//InputAirside::GetAirportList(pDoc->GetProjectID(),vAirports);
	//	
	//CImportFlightFromFileDlg dlg(GetInputTerminal(),GetProjPath(),vAirports.at(0));
	//if (dlg.DoModal() == IDOK)
	//{
	//	//	Codes begin
	//}
}


BOOL CFltPaxDataDlg::OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult)
{
	ASSERT(pNMHDR->code == TTN_NEEDTEXT);

	// if there is a top level routing frame then let it handle the message
	if (GetRoutingFrame() != NULL) return FALSE;

	TOOLTIPTEXTA* pTTT = (TOOLTIPTEXT*)pNMHDR;
	CString strTipText;
	UINT nID = pNMHDR->idFrom;

	if (pNMHDR->code == TTN_NEEDTEXT && (pTTT->uFlags & TTF_IDISHWND))
	{
		// idFrom is actually the HWND of the tool 
		nID = ::GetDlgCtrlID((HWND)nID);
		// add, delete, edit
	}

	if (nID != 0) // will be zero on a separator
	{
		strTipText.LoadString(nID);
		_tcsncpy(pTTT->szText, strTipText, sizeof(pTTT->szText));

		*pResult = 0;

		// bring the tooltip window above other pop-up windows
		::SetWindowPos(pNMHDR->hwndFrom, HWND_TOP, 0, 0, 0, 0,
			SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE|SWP_NOOWNERZORDER);

		return TRUE;
	}

	return FALSE;
}

void CFltPaxDataDlg::OnBnClickedCheckNeglectscheddata()
{
	// TODO: Add your control notification handler code here
	bool bSelect = (m_btnNeglectSchedData.GetCheck() == BST_CHECKED)?true:false;

	if (m_enumType == FLIGHT_LOAD_FACTORS)
	{
		FlightConstraintDatabaseWithSchedData* pData = (FlightConstraintDatabaseWithSchedData*)GetInputTerminal()->flightData->getLoads();
		pData->SetNeglectSchedData(bSelect);
	}

	if (m_enumType == FLIGHT_AC_CAPACITIES)
	{
		FlightConstraintDatabaseWithSchedData* pData = (FlightConstraintDatabaseWithSchedData*)GetInputTerminal()->flightData->getCapacity();
		pData->SetNeglectSchedData(bSelect);
	}

	m_btnSave.EnableWindow(TRUE);
}

void CFltPaxDataDlg::OnSize( UINT nType, int cx, int cy )
{
	CDialog::OnSize(nType, cx, cy);
	if( m_btnOk.m_hWnd == NULL )
		return;

	// TODO: Add your message handler code here
	CRect Barrc,rc,Selrc;
	m_btnCancel.GetWindowRect(&rc);
	m_Framebarcontenter.MoveWindow(10,4,cx-20,cy-15-2*rc.Height()-9);
	m_btnCancel.MoveWindow(cx-rc.Width()-10,cy-15-rc.Height(),rc.Width(),rc.Height());
	m_btnOk.MoveWindow(cx-2*rc.Width()-30,cy-15-rc.Height(),rc.Width(),rc.Height());
	m_btnSave.MoveWindow(cx-3*rc.Width()-50,cy-15-rc.Height(),rc.Width(),rc.Height());	
	m_btnImport.MoveWindow(10,cy-15-rc.Height(),rc.Width(),rc.Height());
	if (m_enumType == FLIGHT_LOAD_FACTORS || m_enumType == FLIGHT_AC_CAPACITIES)
	{
		m_btnNeglectSchedData.GetWindowRect(&Selrc);
		m_btnNeglectSchedData.MoveWindow(10,cy-15-2*rc.Height(),Selrc.Width(),Selrc.Height());
	}
	m_ToolBar.GetWindowRect( &Barrc );
	m_toolbarcontenter.MoveWindow(10,10,cx-20,Barrc.Height());
	m_ToolBar.MoveWindow(11,10,cx-20,Barrc.Height());
	m_listctrlData.MoveWindow(10,10+Barrc.Height(),cx-20,cy-15-2*rc.Height()-Barrc.Height()-15);
	InvalidateRect(NULL);
}

void CFltPaxDataDlg::OnGetMinMaxInfo( MINMAXINFO FAR* lpMMI )
{
	// TODO: Add your message handler code here and/or call default

	lpMMI->ptMinTrackSize.x = 552;
	lpMMI->ptMinTrackSize.y = 348;

	CDialog::OnGetMinMaxInfo(lpMMI);
}