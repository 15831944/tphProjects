// HubDataDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "HubDataDlg.h"
#include "inputs\in_term.h"
#include "inputs\hubbing.h"
#include "inputs\probab.h"
#include "PassengerTypeDialog.h"
#include "PaxTransferDBDlg.h"
#include "TermPlanDoc.h"
#include "FlightDialog.h"
#include "PassengerTypeDialog.h"
#include "..\common\ProbDistEntry.h"
#include "..\common\ProbDistManager.h"
#include "common\WinMsg.h"
#include "inputs\HubbingDatabase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const CString CS_HUBBING_DATA = "HUBBING DATA";
const CString CS_TRANSIT = "TRANSIT";
const CString CS_TRANSFER = "TRANSFER";
const CString CS_PERCENT = "PERCENT";
const CString CS_MIN_TRANSFER_TIME = "MIN TRANSFER TIME";
const CString CS_TRANSFER_TIME_WINDOW = "TRANSFER TIME WINDOW";
const CString CS_TRANSFER_DEST = "TRANSFER DESTINATION";

/////////////////////////////////////////////////////////////////////////////
// CHubDataDlg dialog


CHubDataDlg::CHubDataDlg(CWnd* pParent)
	: CDialog(CHubDataDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHubDataDlg)
	//m_nHubType = -1;
	//}}AFX_DATA_INIT

	m_treeHubData.setInputTerminal( GetInputTerminal() );
}


void CHubDataDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHubDataDlg)
	DDX_Control(pDX, IDC_STATIC_TOOLBARCONTENTER, m_toolbarcontenter);
	DDX_Control(pDX, IDOK, m_btnOk);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDC_STATIC_FRAME, m_staFrame);
	DDX_Control(pDX, IDC_TREE_HUBDATA, m_treeHubData);
	DDX_Control(pDX, IDC_BUTTON_SAVE, m_btnSave);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHubDataDlg, CDialog)
	//{{AFX_MSG_MAP(CHubDataDlg)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_HUBBINGDATA_MODIFY, OnHubbingdataModify)
	ON_COMMAND(ID_HUBBINGDATA_DEL, OnHubbingdataDel)
	ON_COMMAND(ID_HUBBINGDATA_ADD, OnHubbingdataAdd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_TOOLBARBUTTONADD, OnHubbingdataAdd)
	ON_COMMAND(ID_TOOLBARBUTTONDEL, OnHubbingdataDel)
	ON_COMMAND(ID_TOOLBARBUTTONEDIT, OnHubbingdataModify)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_HUBDATA, OnSelchangedTreeHubdata)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHubDataDlg message handlers

BOOL CHubDataDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	InitToolbar();
	// set pax dist list
	ReloadTree();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CHubDataDlg::OnButtonSave() 
{
	// TODO: Add your control notification handler code here
	if( !CheckPercent() )
		return;
	AfxGetApp()->BeginWaitCursor();
	GetInputTerminal()->m_pHubbing->saveDataSet(GetProjPath(), true);
	m_btnSave.EnableWindow( false );
	AfxGetApp()->EndWaitCursor();
	
}


void CHubDataDlg::OnCancel() 
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
		GetInputTerminal()->m_pHubbing->loadDataSet( GetProjPath() );		
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

void CHubDataDlg::OnOK() 
{
	// TODO: Add extra validation here
	if(!CheckPercent())
		return;
	if( !m_btnSave.IsWindowEnabled() )  // nonzero if the window was previously disabled
	{
		CDialog::OnOK();
		return;
	}
	
	OnButtonSave();
	CDialog::OnOK();
}

InputTerminal* CHubDataDlg::GetInputTerminal()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
    return (InputTerminal*)&pDoc->GetTerminal();
}


CString CHubDataDlg::GetProjPath()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	return pDoc->m_ProjInfo.path;
}


// load tree from the database.
void CHubDataDlg::ReloadTree()
{
	m_treeHubData.DeleteAllItems();
	m_hRootItem = m_treeHubData.InsertItem( CS_HUBBING_DATA );

	CHubbingDatabase* pDB = GetInputTerminal()->m_pHubbing;
	int nCount = pDB->getCount();
	for( int i=0; i<nCount; i++ )
	{
		CHubbingData* pData = pDB->getItem( i );
		CPassengerConstraint& paxConst = pData->GetConstraint();
		CString szLabel;
		paxConst.screenPrint( szLabel, 0, 256 );
		HTREEITEM hPaxItem = m_treeHubData.InsertItem( szLabel, m_hRootItem ,TVI_SORT);
		m_treeHubData.SetItemData( hPaxItem, (DWORD_PTR)pData);

		// transit
		ProbabilityDistribution* pDist = pData->GetTransiting();
		HTREEITEM hItem = m_treeHubData.InsertItem( GetLabel( CS_TRANSIT, pDist ), hPaxItem );
		m_treeHubData.SetItemData( hItem, TRANSIT_ITEMDATA );

		// transfer
 		HTREEITEM hTransferItem = m_treeHubData.InsertItem( CS_TRANSFER, hPaxItem );
		m_treeHubData.SetItemData( hTransferItem, TRANSFER_ITEMDATA );

		// percent
		pDist = pData->GetTransferring();
		hItem = m_treeHubData.InsertItem( GetLabel( CS_PERCENT, pDist ), hTransferItem );
		m_treeHubData.SetItemData( hItem, TRANSFER_PERCENT_ITEMDATA );

		FlightTypeDistribution* pFlightType = pData->GetTransferDest();
		hItem = m_treeHubData.InsertItem( CS_TRANSFER_DEST, hTransferItem );
		m_treeHubData.SetItemData( hItem, TRANSFER_DEST );

		// transfer dest fligh
		ReloadTransferFlightTree( hItem, pFlightType, -1 );
	}
	m_treeHubData.Expand( m_hRootItem, TVE_EXPAND );
}


// control the menu enable.
void CHubDataDlg::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: Add your message handler code here
	m_hRClickItem = NULL;

	CRect rectTree;
	m_treeHubData.GetWindowRect( &rectTree );
	if( !rectTree.PtInRect(point) ) 
		return;

	m_treeHubData.SetFocus();
	CPoint pt = point;
	m_treeHubData.ScreenToClient( &pt );
	UINT iRet;
	m_hRClickItem = m_treeHubData.HitTest( pt, &iRet );
	if (iRet != TVHT_ONITEMLABEL)   // Must click on the label
	{
		m_hRClickItem = NULL;
	}

	CMenu menuPopup;
	menuPopup.LoadMenu( IDR_MENU_POPUP );
	CMenu* pMenu = menuPopup.GetSubMenu( 4 );

	int nControlEnableVal = GetControlEnableValue( m_hRClickItem );
	if( nControlEnableVal == 0 )
		m_treeHubData.SelectItem( NULL );
	else 
	{
		pMenu->EnableMenuItem( ID_HUBBINGDATA_ADD, nControlEnableVal & CONTROL_ENABLE_ADD ? MF_ENABLED : MF_GRAYED );
		pMenu->EnableMenuItem( ID_HUBBINGDATA_MODIFY, nControlEnableVal & CONTROL_ENABLE_MOD? MF_ENABLED : MF_GRAYED );
		pMenu->EnableMenuItem( ID_HUBBINGDATA_DEL, nControlEnableVal & CONTROL_ENABLE_DEL ? MF_ENABLED : MF_GRAYED );
		m_treeHubData.SelectItem( m_hRClickItem );

		pMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);
	}
}


// level 1, click on root: add pax
	// level 2, click on pax: delete pax
		// level 3, click on transit: modify
		// level 3, click on transfer : nothing.
			// level 4, click on percent: modify
			// level 4, click on dest: add
				// level 5, click on child of dest: edit, delete
                 //level 6, click on min,window: modify
// 
int CHubDataDlg::GetControlEnableValue( HTREEITEM _hTreeItem )
{

	int nRes = 0;

	int nLevel = 0; 
	int nIdx = 0;
	
	GetCurSelection( _hTreeItem, nLevel, nIdx );
	switch( nLevel )
	{
	case 0:
		return CONTROL_ENABLE_ADD;
	case 1:
		return CONTROL_ENABLE_DEL;
	case 2:
		if( nIdx == TRANSIT_ITEMDATA )
			return CONTROL_ENABLE_MOD;
		else
			return 0;
	case 3:
		if ( nIdx == TRANSFER_PERCENT_ITEMDATA)
			return CONTROL_ENABLE_MOD;
		else
			return CONTROL_ENABLE_ADD;
	case 4:
		return CONTROL_ENABLE_MOD | CONTROL_ENABLE_DEL;
	case 5:
		return CONTROL_ENABLE_MOD;
	}
	
	return 0;
}



// get the level and the index on the level.
// return 0 - 5
void CHubDataDlg::GetCurSelection( HTREEITEM _hTreeItem, int& _nLevel, int& _nIdx )
{
	int nRes = 0;
	HTREEITEM hCurItem = _hTreeItem;
	if( hCurItem == m_hRootItem )
	{
		_nLevel = 0;
		_nIdx = 0;
		return;
	}

	hCurItem = m_treeHubData.GetParentItem( hCurItem );
	if( hCurItem == m_hRootItem )
	{
		_nLevel = 1;
		_nIdx = 0;
		return;
	}

	hCurItem = m_treeHubData.GetParentItem( hCurItem );
	if( hCurItem == m_hRootItem )
	{
		// level 3 
		_nLevel = 2;
		_nIdx = m_treeHubData.GetItemData( _hTreeItem );
		return;
		
	}

	hCurItem = m_treeHubData.GetParentItem( hCurItem );
	if( hCurItem == m_hRootItem )
	{
		_nLevel = 3;
		_nIdx = m_treeHubData.GetItemData( _hTreeItem );
		return;
	}

	hCurItem = m_treeHubData.GetParentItem( hCurItem );
	if( hCurItem == m_hRootItem )
	{
		_nLevel = 4;
		_nIdx = m_treeHubData.GetItemData( _hTreeItem );
		return;
	}
	_nLevel = 5;
	_nIdx = m_treeHubData.GetItemData( _hTreeItem );
	return;

}



LRESULT CHubDataDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (message == WM_INPLACE_SPIN)
	{
		LPSPINTEXT pst = (LPSPINTEXT) lParam;

		int nLevel, nIdx;
		GetCurSelection( m_hModifiedItem, nLevel, nIdx );
		HTREEITEM hPaxItem = GetPaxTypeItem( m_hModifiedItem, nLevel );
		CHubbingData* pData = (CHubbingData*)m_treeHubData.GetItemData( hPaxItem );
		int indx = m_treeHubData.GetItemData(m_hModifiedItem);
		CString buf;
		(pData->GetTransferDest()->getGroup (indx))->getFlightConstraint()->screenPrint(buf);
		buf.Format("%s  (%d%%)",buf,pst->iPercent);
		m_treeHubData.SetItemText( m_hModifiedItem,buf);
		SetPercent( pst->iPercent );
		return TRUE;
	}

	if( message == WM_INPLACE_COMBO )
	{
		if( !m_hRClickItem )
			return FALSE;

		int nSelIdx = (int) lParam;
		CProbDistEntry* pPDEntry = _g_GetActiveProbMan( GetInputTerminal() )->getItem( nSelIdx );
		ProbabilityDistribution* pProbDist = pPDEntry->m_pProbDist;

		SetProbabilityDistribution( pProbDist );
		return TRUE;
	}
	return CDialog::DefWindowProc(message, wParam, lParam);
}


// check if the transfer dest has 100%
BOOL CHubDataDlg::CheckPercent()
{
	//Transfer Defination
	int nCount = GetInputTerminal()->m_pHubbing->getCount();
	for( int i=0; i<nCount; i++ )
	{
		CHubbingData* pHubbingData = GetInputTerminal()->m_pHubbing->getItem( i );		
		FlightTypeDistribution* pDist = (FlightTypeDistribution*)pHubbingData->GetTransferDest();
		int nGroupCount = pDist->getCount();
		if( nGroupCount < 1 )
			continue;
		if( pDist->getProb( nGroupCount - 1 ) != 100 )
		{
			MessageBox( "Total Percentage is not 100%", "Error" );

			// TODO: need highlight the tree item.
			return FALSE;
		}
	}
	return TRUE;
}


// Called by the Message Handler, by serveral case
// only in case when click on dest
void CHubDataDlg::SetPercent(int _nPercent)
{
	assert( m_hModifiedItem );

	// get pax type index, 
	HTREEITEM hItem = m_hModifiedItem;
	//for( int i=0; i<3; i++ )
	for( int i=0; i<3; i++ )
	{
		hItem = m_treeHubData.GetParentItem( hItem );
		assert( hItem );
	}

	//int nDBIdx = m_treeHubData.GetItemData( hItem );
	//CHubbingData* pHubbingData = GetInputTerminal()->m_pHubbing->getItem( nDBIdx );
	CHubbingData* pHubbingData = (CHubbingData*)m_treeHubData.GetItemData( hItem );
	FlightTypeDistribution* pDist = (FlightTypeDistribution*)pHubbingData->GetTransferDest();
	int nFltIdx = m_treeHubData.GetItemData( m_hModifiedItem );
	pDist->setProb( _nPercent, nFltIdx );
	//ReloadTree();
}



void CHubDataDlg::OnHubbingdataModify() 
{
	if( m_hRClickItem == NULL )
		return;

	int nLevel;
	int nIdx;
	GetCurSelection( m_hRClickItem, nLevel, nIdx );

	m_hModifiedItem = m_hRClickItem;
	switch( nLevel )
	{
	case 2:
		if( nIdx == 0 )
			// transit
			m_treeHubData.EditProbabilityDistribution( m_hRClickItem );
		else
			return;
		break;
	//case 3:
	//	if( nIdx < 3 )
	//		// percent, min, window
	//		m_treeHubData.EditProbabilityDistribution( m_hRClickItem );
	//	else
	//		return;
	//	break;
	case 3:
		if ( nIdx == 0)
			m_treeHubData.EditProbabilityDistribution(m_hRClickItem);
		else
			return;
		break;
	case 4:
		m_treeHubData.SpinEditLabel(m_hRClickItem);
		break;		
	case 5:
		m_treeHubData.EditProbabilityDistribution(m_hRClickItem);
		break;
	}

	m_btnSave.EnableWindow();
}


void CHubDataDlg::OnHubbingdataDel() 
{
	if( m_hRClickItem == NULL )
		return;

	int nLevel;
	int nIdx;
	GetCurSelection( m_hRClickItem, nLevel, nIdx );
	switch( nLevel )
	{
	case 1:
		DeletePassengerEntry();
		break;
	case 4:
		// delete flt dist
		DeleteFltDist();

		// reload data.	
		 																
		break;
 	default:
		return;
	}

	m_btnSave.EnableWindow();

	/*
	HTREEITEM hParentItem = m_treeHubData.GetParentItem( m_hRClickItem );
	if( hParentItem == NULL )
		return;

	int nDBIdx = m_treeHubData.GetItemData( m_hRClickItem );
	if( hParentItem == m_hTransitItem )
	{
	 	PassengerConstraintDatabase* pPaxConstrDB = GetInputTerminal()->m_pHubbing->getTransiting();
	 	assert( pPaxConstrDB );
		pPaxConstrDB->removeItem( nDBIdx );
		ReloadTransitTree( m_hTransitItem, -1 );
	}
	else if( hParentItem == m_hTransferDataItem )
	{
	 	PassengerConstraintDatabase* pPaxConstrDB = GetInputTerminal()->m_pHubbing->getTransferring();
	 	assert( pPaxConstrDB );
		pPaxConstrDB->removeItem( nDBIdx );
		ReloadTransferPercentTree( m_hTransferDataItem, -1 );
	}
	else if( hParentItem == m_hTransferMinItem )
	{
	 	PassengerConstraintDatabase* pPaxConstrDB = GetInputTerminal()->m_pHubbing->getMinTransferTime();
	 	assert( pPaxConstrDB );
		pPaxConstrDB->removeItem( nDBIdx );
		ReloadMinTimeTree( m_hTransferMinItem, -1 );
	}
	else if( hParentItem == m_hTransferWinItem )
	{
	 	PassengerConstraintDatabase* pPaxConstrDB = GetInputTerminal()->m_pHubbing->getTransferWindow();
	 	assert( pPaxConstrDB );
		pPaxConstrDB->removeItem( nDBIdx );
		ReloadTimeWindowTree( m_hTransferWinItem, -1 );
	}
	else if( hParentItem == m_hTransferDestItem )
	{
	 	PassengerConstraintDatabase* pPaxConstrDB = GetInputTerminal()->m_pHubbing->getTransferDatabase();
	 	assert( pPaxConstrDB );
		pPaxConstrDB->removeItem( nDBIdx );
		ReloadTransferDestTree( m_hTransferDestItem, -1, -1 );		
	}
	else if( m_treeHubData.GetParentItem( hParentItem ) == m_hTransferDestItem )
	{
		int nParentIdx = m_treeHubData.GetItemData( hParentItem );
	 	PassengerConstraintDatabase* pPaxConstrDB = GetInputTerminal()->m_pHubbing->getTransferDatabase();
	 	assert( pPaxConstrDB );
		FlightTypeDistribution* pDist = (FlightTypeDistribution*)pPaxConstrDB->getDist( nParentIdx );		
		pDist->deleteRow( nDBIdx );
		ReloadTransferFlightTree( hParentItem, pDist, -1 );
	}
	*/
}


void CHubDataDlg::OnHubbingdataAdd() 
{
	// TODO: Add your command handler code here

	if( m_hRClickItem == NULL )
		return;

	int nLevel;
	int nIdx;
	GetCurSelection( m_hRClickItem, nLevel, nIdx );
	switch( nLevel )
	{
	case 0:
		AddPassengerTypeEntry();
		break;
	case 3:
		AddTransferDestFlight();
		break;	
	default:
		return;
	}

	m_btnSave.EnableWindow();
}

// add new pax type to the data. and automatic create the default value to the entyr.
void CHubDataDlg::AddPassengerTypeEntry()
{
	CPassengerTypeDialog dlg( m_pParentWnd );
	if( dlg.DoModal() == IDCANCEL )
		return;

	//PassengerConstraint paxConstr = dlg.GetPaxSelection();
	CMobileElemConstraint mobileConstr = dlg.GetMobileSelection();
 
	CHubbingDatabase* pHubbingDB = GetInputTerminal()->m_pHubbing;

	// find if exist.
	int nCount = pHubbingDB->getCount();
	for( int i=0; i<nCount; i++ )
	{
		CHubbingData* hubbingData = pHubbingDB->getItem( i );
		CPassengerConstraint testPaxConst = hubbingData->GetConstraint();
		if( mobileConstr.isEqual(&testPaxConst) )//Exist
		{
			// selecct the item
			HTREEITEM hItem = m_treeHubData.GetChildItem( m_hRClickItem );
			while( hItem )
			{
				if( m_treeHubData.GetItemData( hItem ) == i )
				{
					m_treeHubData.SelectItem( hItem );
					break;
				}
				hItem = m_treeHubData.GetNextItem( hItem, TVGN_NEXT );
			}
			return;
		}
	}
				
	// add new pax constraint.
	CHubbingData* pData = pHubbingDB->AddDefaultEntry(mobileConstr);
	CPassengerConstraint& paxConst = pData->GetConstraint();
	CString szLabel;
	paxConst.screenPrint( szLabel, 0, 256 );
	HTREEITEM hPaxItem = m_treeHubData.InsertItem( szLabel, m_hRootItem,TVI_SORT);
	m_treeHubData.SetItemData( hPaxItem,(DWORD_PTR)pData );

	// transit
	ProbabilityDistribution* pDist = pData->GetTransiting();
	HTREEITEM hItem = m_treeHubData.InsertItem( GetLabel( CS_TRANSIT, pDist ), hPaxItem );
	m_treeHubData.SetItemData( hItem, TRANSIT_ITEMDATA );

	// transfer
 	HTREEITEM hTransferItem = m_treeHubData.InsertItem( CS_TRANSFER, hPaxItem );
	m_treeHubData.SetItemData( hTransferItem, TRANSFER_ITEMDATA );

	// percent
	pDist = pData->GetTransferring();
	hItem = m_treeHubData.InsertItem( GetLabel( CS_PERCENT, pDist ), hTransferItem );
	m_treeHubData.SetItemData( hItem, TRANSFER_PERCENT_ITEMDATA );

	FlightTypeDistribution* pFlightType = pData->GetTransferDest();
	hItem = m_treeHubData.InsertItem( CS_TRANSFER_DEST, hTransferItem );
	m_treeHubData.SetItemData( hItem, TRANSFER_DEST );
	m_treeHubData.Expand( m_hRootItem, TVE_EXPAND );
		// transfer dest fligh
	//ReloadTransferFlightTree( hItem, pFlightType, -1 );
	//ReloadTree();
}


// delete the pax type in the hubbing database.
void CHubDataDlg::DeletePassengerEntry()
{
	assert( m_hRClickItem );

	CHubbingData* pData = (CHubbingData*)m_treeHubData.GetItemData( m_hRClickItem );
	CHubbingDatabase* pDatabase = GetInputTerminal()->m_pHubbing;
	int nCount = pDatabase->getCount();
	for (int i = 0; i < nCount; i++)
	{
		if (pData == pDatabase->getItem(i))
		{
			pDatabase->deleteItem(i);
		}
	}
	m_treeHubData.DeleteItem(m_hRClickItem);
	//////////////////////////////////////////////////////////////////////////
	// modify by bird 2003/5/26
	//HTREEITEM hItem = m_treeHub1Data.GetParentItem( m_hRClickItem );
	//CString strBug1 = m_treeHubData.GetItemText( m_hRClickItem );
	//CString strBug2 = m_treeHubData.GetItemText( hItem );
	//DeleteAllChildItems( hItem );
	//////////////////////////////////////////////////////////////////////////
	//ReloadTree();
}


// delete flight distrubition
void CHubDataDlg::DeleteFltDist()
{
	assert( m_hRClickItem );

	HTREEITEM hPaxItem = GetPaxTypeItem( m_hRClickItem, 4 );
	//int nPaxDBIdx = m_treeHubData.GetItemData( hPaxItem );
	//CHubbingData* pData = GetInputTerminal()->m_pHubbing->getItem( nPaxDBIdx );
	CHubbingData* pData = (CHubbingData*)m_treeHubData.GetItemData( hPaxItem );
	FlightTypeDistribution* pDist = pData->GetTransferDest();
	int nDBIdx = m_treeHubData.GetItemData( m_hRClickItem );
	pDist->deleteRow( nDBIdx );

	HTREEITEM hParentItem = m_treeHubData.GetParentItem( m_hRClickItem );
	ReloadTransferFlightTree( hParentItem, pDist, -1 );

}


/*
// nDataType 0-Transit, 1-Transfer, 2-Min, 3-TimeWindow
void CHubDataDlg::AddDataItem( int _nDataType )
{
	CPassengerTypeDialog dlg( m_pParentWnd );
	if( dlg.DoModal() == IDCANCEL )
		return;

	PassengerConstraint paxConstr = dlg.GetPaxSelection();
 
 	// get pax db
 	PassengerConstraintDatabase* pPaxConstrDB = NULL;
	switch( _nDataType )
	{
	case 0:
		pPaxConstrDB = GetInputTerminal()->m_pHubbing->getTransiting();
		break;
	case 1:
		pPaxConstrDB = GetInputTerminal()->m_pHubbing->getTransferring();
		break;
	case 2:
		pPaxConstrDB = GetInputTerminal()->m_pHubbing->getMinTransferTime();
		break;
	case 3:
		pPaxConstrDB = GetInputTerminal()->m_pHubbing->getTransferWindow();
		break;
	}
 	assert( pPaxConstrDB );

	// find if exist.
	int nCount = pPaxConstrDB->getCount();
	for( int i=0; i<nCount; i++ )
	{
		const PassengerConstraint* pPaxConstr = pPaxConstrDB->getConstraint( i );
		if( paxConstr.isEqual(pPaxConstr) )//Exist
		{
			// selecct the item
			HTREEITEM hItem = m_treeHubData.GetChildItem( m_hRClickItem );
			while( hItem )
			{
				if( m_treeHubData.GetItemData( hItem ) == i )
				{
					m_treeHubData.SelectItem( hItem );
					break;
				}
				hItem = m_treeHubData.GetNextItem( hItem, TVGN_NEXT );
			}
			return;
		}
	}
				
	// add new pax constraint.
	char buf[128];
	paxConstr.printConstraint( buf );
	PassengerConstraint* pPaxConstr = new PassengerConstraint();
	pPaxConstr->SetInputTerminal( GetInputTerminal() );
	pPaxConstr->setConstraint( buf );
	ConstraintEntry* pNewEntry = new ConstraintEntry();
	ConstantDistribution* pDist = new ConstantDistribution( 0 );
	pNewEntry->initialize( pPaxConstr, pDist );
	pPaxConstrDB->addEntry( pNewEntry );
	int nDBIdx = pPaxConstrDB->findItem( pNewEntry );

	// add to tree
	switch( _nDataType )
	{
	case 0:
		ReloadTransitTree( m_hTransitItem, nDBIdx );
		break;
	case 1:
		ReloadTransferPercentTree( m_hTransferDataItem, nDBIdx );
		break;
	case 2:
		ReloadMinTimeTree( m_hTransferMinItem, nDBIdx );
		break;
	case 3:
		ReloadTimeWindowTree( m_hTransferWinItem, nDBIdx );
		break;
	}
}

void CHubDataDlg::AddTransferDestEntry()
{
	CPassengerTypeDialog dlg( m_pParentWnd );
	if( dlg.DoModal() == IDOK )
	{
		PassengerConstraint newPaxCon = dlg.GetPaxSelection();
		PassengerConstraintDatabase* pPaxConstrDB = GetInputTerminal()->m_pHubbing->getTransferDatabase();
		int nCount = pPaxConstrDB->getCount();
		for( int i=0; i<nCount; i++ )
		{
			const PassengerConstraint* pPaxConstr = pPaxConstrDB->getConstraint( i );
			if(newPaxCon.isEqual(pPaxConstr))//Exist
			{
				// selecct the item
				HTREEITEM hItem = m_treeHubData.GetChildItem( m_hRClickItem );
				while( hItem )
				{
					if( m_treeHubData.GetItemData( hItem ) == i )
					{
						m_treeHubData.SelectItem( hItem );
						break;
					}
					hItem = m_treeHubData.GetNextItem( hItem, TVGN_NEXT );
				}
				return;
			}
		}
		if( i == nCount )
		{
			PassengerConstraint* pPaxCon = new PassengerConstraint;
			*pPaxCon = newPaxCon;
			ConstraintEntry* pNewEntry = new ConstraintEntry();
			FlightTypeDistribution* pFltDist = new FlightTypeDistribution();
			pNewEntry->initialize( pPaxCon, pFltDist );
			pPaxConstrDB->addEntry( pNewEntry );
			int nDBIdx = pPaxConstrDB->findItem( pNewEntry );

			ReloadTransferDestTree( m_hTransferDestItem, nDBIdx, -1 );
		}
	}
}
*/


// Add a flight 
void CHubDataDlg::AddTransferDestFlight()
{
	CFlightDialog dlg( m_pParentWnd );
	if( dlg.DoModal() == IDOK )
	{
		// get new flt constraint
		FlightConstraint newFltConstr = dlg.GetFlightSelection();

		assert( m_hRClickItem );

		// get pax type index, 
		HTREEITEM hItem = m_hRClickItem;
		for( int i=0; i<2; i++ )
		{
			hItem = m_treeHubData.GetParentItem( hItem );
			assert( hItem );
		}

		//int nDBIdx = m_treeHubData.GetItemData( hItem );
		//CHubbingData* pHubbingData = GetInputTerminal()->m_pHubbing->getItem( nDBIdx );
		CHubbingData* pHubbingData = (CHubbingData*)m_treeHubData.GetItemData( hItem );
		FlightTypeDistribution* pDist = (FlightTypeDistribution*)pHubbingData->GetTransferDest();
		assert( pDist );
		int nCount = pDist->getCount();
		for( i=0; i<nCount; i++ )
		{
			FlightConstraint* flightCons = (pDist->getGroup (i))->getFlightConstraint();
			if( flightCons->isEqual(&newFltConstr) )
			{
				// select the item
				HTREEITEM hItem = m_treeHubData.GetChildItem( m_hRClickItem );
				while( hItem )
				{
					if( m_treeHubData.GetItemData( hItem ) == i )
					{
						m_treeHubData.SelectItem( hItem );
						break;
					}
					hItem = m_treeHubData.GetNextItem( hItem, TVGN_NEXT );
				}
				return;
			}
		}
		if( i == nCount )
		{
			CTransferFlightConstraint* pFltConstr = new CTransferFlightConstraint;
			pFltConstr->setFlightConstriant(newFltConstr);
			pDist->addRow( pFltConstr, 100 );

		    unsigned char* pProbabilities = new unsigned char[nCount+1];
			GetEvenPercent( nCount+1, pProbabilities );
			for( int i=0; i<=nCount; i++ )
				pDist->setProb( pProbabilities[i], i );

			delete [] pProbabilities;
			ReloadTransferFlightTree( m_hRClickItem, pDist, nCount );
		}
	}
}


	
// evenly distribute 100 into _nCount. make sure total is 100
void CHubDataDlg::GetEvenPercent( int _nCount, unsigned char *_pProb )
{
	int nTemp = 0;
	for( int i=0; i<_nCount; i++ )
	{
		_pProb[i] = ( 100 - nTemp ) / ( _nCount - i ) ;
		nTemp += _pProb[i];
	}
}



CString CHubDataDlg::GetLabel( const CString& _csLabel, const ProbabilityDistribution* _pProb )
{
	CString csLabel;
	csLabel = _csLabel;
	char buf[1024];
	_pProb->screenPrint( buf );
	csLabel = csLabel + "    (" + CString(buf) + ")";
	return csLabel;
}


/*
// reload the tree from the _hItem, 
void CHubDataDlg::ReloadTransitTree( HTREEITEM _hItem, int _nSelDBIdx )
{
	DeleteAllChildItems( _hItem );
	PassengerConstraintDatabase* pPaxConstrDB = GetInputTerminal()->m_pHubbing->getTransiting();
 	if(pPaxConstrDB )
	{
		int nCount = pPaxConstrDB->getCount();
		for( int i=0; i<nCount; i++ )
		{
			const PassengerConstraint* pPaxConstr = pPaxConstrDB->getConstraint( i );
			const ProbabilityDistribution* pProb = pPaxConstrDB->getItem( i )->getValue();
			HTREEITEM hItem = m_treeHubData.InsertItem( GetLabel( pPaxConstr, pProb ), _hItem );
			m_treeHubData.SetItemData( hItem, i );
			if( i == _nSelDBIdx )
				m_treeHubData.SelectItem( hItem );
		}
	}
	m_treeHubData.Expand( m_hRootItem, TVE_EXPAND );
	m_treeHubData.Expand( m_hTransitItem, TVE_EXPAND );
}
void CHubDataDlg::ReloadTransferPercentTree(HTREEITEM _hItem, int _nSelDBIdx )
{
	DeleteAllChildItems( _hItem );
	PassengerConstraintDatabase* pPaxConstrDB = GetInputTerminal()->m_pHubbing->getTransferring();
 	if(pPaxConstrDB )
	{
		int nCount = pPaxConstrDB->getCount();
		for( int i=0; i<nCount; i++ )
		{
			const PassengerConstraint* pPaxConstr = pPaxConstrDB->getConstraint( i );
			const ProbabilityDistribution* pProb = pPaxConstrDB->getItem( i )->getValue();
			HTREEITEM hItem = m_treeHubData.InsertItem( GetLabel( pPaxConstr, pProb ), _hItem );
			m_treeHubData.SetItemData( hItem, i );
			if( i == _nSelDBIdx )
				m_treeHubData.SelectItem( hItem );
		}
	}
	m_treeHubData.Expand( m_hRootItem, TVE_EXPAND );
	m_treeHubData.Expand( m_hTransferItem, TVE_EXPAND );
	m_treeHubData.Expand( m_hTransferDataItem, TVE_EXPAND );
}

void CHubDataDlg::ReloadMinTimeTree(HTREEITEM _hItem, int _nSelDBIdx )
{
	DeleteAllChildItems( _hItem );
	PassengerConstraintDatabase* pPaxConstrDB = GetInputTerminal()->m_pHubbing->getMinTransferTime();
 	if(pPaxConstrDB )
	{
		int nCount = pPaxConstrDB->getCount();
		for( int i=0; i<nCount; i++ )
		{
			const PassengerConstraint* pPaxConstr = pPaxConstrDB->getConstraint( i );
			const ProbabilityDistribution* pProb = pPaxConstrDB->getItem( i )->getValue();
			HTREEITEM hItem = m_treeHubData.InsertItem( GetLabel( pPaxConstr, pProb ), _hItem );
			m_treeHubData.SetItemData( hItem, i );
			if( i == _nSelDBIdx )
				m_treeHubData.SelectItem( hItem );
		}
	}
	m_treeHubData.Expand( m_hRootItem, TVE_EXPAND );
	m_treeHubData.Expand( m_hTransferItem, TVE_EXPAND );
	m_treeHubData.Expand( m_hTransferMinItem, TVE_EXPAND );
}

void CHubDataDlg::ReloadTimeWindowTree(HTREEITEM _hItem, int _nSelDBIdx )
{
	DeleteAllChildItems( _hItem );
	PassengerConstraintDatabase* pPaxConstrDB = GetInputTerminal()->m_pHubbing->getTransferWindow();
 	if(pPaxConstrDB )
	{
		int nCount = pPaxConstrDB->getCount();
		for( int i=0; i<nCount; i++ )
		{
			const PassengerConstraint* pPaxConstr = pPaxConstrDB->getConstraint( i );
			const ProbabilityDistribution* pProb = pPaxConstrDB->getItem( i )->getValue();
			HTREEITEM hItem = m_treeHubData.InsertItem( GetLabel( pPaxConstr, pProb ), _hItem );
			m_treeHubData.SetItemData( hItem, i );
			if( i == _nSelDBIdx )
				m_treeHubData.SelectItem( hItem );
		}
	}
	m_treeHubData.Expand( m_hRootItem, TVE_EXPAND );
	m_treeHubData.Expand( m_hTransferItem, TVE_EXPAND );
	m_treeHubData.Expand( m_hTransferWinItem, TVE_EXPAND );
}


void CHubDataDlg::ReloadTransferDestTree(HTREEITEM _hItem, FlightTypeDistribution* _pFltDist )
{
	DeleteAllChildItems( _hItem );

	int nCount = _pFltDist->getCount();
	for( int i=0; i<nCount; i++ )
	{
			const PassengerConstraint* pPaxConstr = pPaxConstrDB->getConstraint( i );
			pPaxConstr->screenPrint( buf );
			HTREEITEM hEntryItem=m_treeHubData.InsertItem( CString(buf), _hItem);
			m_treeHubData.SetItemData( hEntryItem, i );
			
			FlightTypeDistribution *dist = (FlightTypeDistribution *)((ProbabilityDistribution *)pPaxConstrDB->getDist (i));
			if (dist)
			{
				int nIdx = -1;
				if( i == _nPaxDBIdx )
					nIdx = _nFltDBIdx;
				ReloadTransferFlightTree( hEntryItem, dist, nIdx );
			}
			if( i == _nPaxDBIdx && _nFltDBIdx == -1 )
				m_treeHubData.SelectItem( hEntryItem );
		}
	}
	m_treeHubData.Expand( m_hRootItem, TVE_EXPAND );
	m_treeHubData.Expand( m_hTransferItem, TVE_EXPAND );
	m_treeHubData.Expand( m_hTransferDestItem, TVE_EXPAND );
}
*/

void CHubDataDlg::ReloadTransferFlightTree( HTREEITEM _hItem, FlightTypeDistribution* _pDist, int _nSelDBIdx )
{
	if( !_pDist )
		return;

	CString buf;
	DeleteAllChildItems( _hItem );
	int prob, prevProb = 0;
	int count = _pDist->getCount();
	for( int i = 0; i < count; i++ )
	{
		buf.Empty();
		//(_pDist->getGroup (i))->screenPrint (buf);
		(_pDist->getGroup (i))->getFlightConstraint()->screenPrint(buf);
		prob = (int)_pDist->getProb(i);
		CString strBuf;
		strBuf.Format("%d",prob - prevProb);
		prevProb = (int)prob;
		HTREEITEM hItem = m_treeHubData.InsertItem( buf+"  ("+strBuf+"%)", _hItem);
		m_treeHubData.SetItemData( hItem, i );
		if( i == _nSelDBIdx )
			m_treeHubData.SelectItem( hItem );

		ProbabilityDistribution* pDist;

		// min transfer time
		pDist = _pDist->getGroup(i)->getMinTransferTime();
		HTREEITEM hSubItem = m_treeHubData.InsertItem( GetLabel( CS_MIN_TRANSFER_TIME, pDist ), hItem );
		m_treeHubData.SetItemData( hSubItem, TRANSFER_MIN_TIME );


		//// transfer time window
		pDist = _pDist->getGroup(i)->getTransferWindow();
		hSubItem = m_treeHubData.InsertItem( GetLabel( CS_TRANSFER_TIME_WINDOW, pDist ), hItem );
		m_treeHubData.SetItemData( hSubItem, TRANSFER_TIME_WINDOW );
	}
}


void CHubDataDlg::DeleteAllChildItems(HTREEITEM _hParentItem)
{
	HTREEITEM hItem = m_treeHubData.GetChildItem( _hParentItem );
	while( hItem )
	{
		DeleteAllChildItems( hItem );
		m_treeHubData.DeleteItem( hItem );
		hItem = m_treeHubData.GetChildItem( _hParentItem );
	}
}


// called by message handler to setup the processor.
// case level 2 transit, level 3, click on percent
//level 5  min, window: modify
void CHubDataDlg::SetProbabilityDistribution( ProbabilityDistribution* _pProbDist )
{
	assert( m_hModifiedItem );

	// find the database entry.
	ProbabilityDistribution* pDist = ProbabilityDistribution::CopyProbDistribution(_pProbDist);

	int nLevel;
	int nIdx;
	GetCurSelection( m_hModifiedItem, nLevel, nIdx );
	assert( nLevel == 2 || nLevel == 3 || nLevel ==5 );

	HTREEITEM hPaxItem = GetPaxTypeItem( m_hModifiedItem, nLevel );
	//int nPaxDBIdx = m_treeHubData.GetItemData( hPaxItem );
	//CHubbingData* pData = GetInputTerminal()->m_pHubbing->getItem( nPaxDBIdx );
	CHubbingData* pData = (CHubbingData*)m_treeHubData.GetItemData( hPaxItem );
	ConstraintEntry* pEntry = NULL;
	if( nLevel == 2 )
	{
		pData->SetTransiting( pDist );
		m_treeHubData.SetItemText( m_hModifiedItem, GetLabel( CS_TRANSIT, pDist ) );		
	}
	else if( nLevel == 3 )
	{
		pData->SetTransferring( pDist );
		m_treeHubData.SetItemText( m_hModifiedItem, GetLabel( CS_PERCENT, pDist ) );		
	}
	else if(( nLevel == 5) && ( nIdx == 0 ))
	{
		HTREEITEM hItem = m_treeHubData.GetParentItem(m_hModifiedItem);
		int nHDIdx = m_treeHubData.GetItemData( hItem );
        FlightTypeDistribution* pFlightDist = pData->GetTransferDest();
		CTransferFlightConstraint* transferFlight = pFlightDist->getGroup(nHDIdx);
		transferFlight->setMinTransferTime(pDist);
		m_treeHubData.SetItemText( m_hModifiedItem, GetLabel( CS_MIN_TRANSFER_TIME, pDist ) );		
	}
	else if(( nLevel == 5) && ( nIdx == 1 ))
	{
		HTREEITEM hItem = m_treeHubData.GetParentItem(m_hModifiedItem);
		int nHDIdx = m_treeHubData.GetItemData( hItem );
		FlightTypeDistribution* pFlightDist = pData->GetTransferDest();
		CTransferFlightConstraint* transferFlight = pFlightDist->getGroup(nHDIdx);
		transferFlight->setTransferWindow(pDist);
		m_treeHubData.SetItemText( m_hModifiedItem, GetLabel( CS_TRANSFER_TIME_WINDOW, pDist ) );		
	}
	else
		return;

	m_btnSave.EnableWindow();

}


void CHubDataDlg::InitToolbar()
{
	CRect rc;
	m_toolbarcontenter.GetWindowRect( &rc );
	ScreenToClient(&rc);
	m_ToolBar.MoveWindow(&rc);
	m_ToolBar.ShowWindow(SW_SHOW);
	m_toolbarcontenter.ShowWindow(SW_HIDE);
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONADD,FALSE );
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL,FALSE );
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONEDIT,FALSE );	
}

int CHubDataDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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


void CHubDataDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	CRect btnrc,toolbarrc,rc;
	m_btnSave.GetWindowRect( &btnrc );
	m_toolbarcontenter.GetWindowRect( &toolbarrc );

	m_staFrame.MoveWindow( 9,4,cx-20,cy-25-btnrc.Height() );
	m_toolbarcontenter.MoveWindow( 11,10,cx-40,toolbarrc.Height() );
	
	m_toolbarcontenter.GetWindowRect( &rc );
	ScreenToClient(&rc);
	m_toolbarcontenter.ShowWindow( SW_HIDE );
	m_ToolBar.MoveWindow(&rc);
	m_staFrame.GetWindowRect( &rc );
	ScreenToClient(&rc);
	rc.left = 9;
	rc.top = 36;
	rc.right = cx-11;
	m_treeHubData.MoveWindow( &rc );
	m_btnSave.MoveWindow( cx-20-3*btnrc.Width(),cy-10-btnrc.Height(),btnrc.Width(),btnrc.Height() );
	m_btnOk.MoveWindow( cx-15-2*btnrc.Width(),cy-10-btnrc.Height(),btnrc.Width(),btnrc.Height() );
	m_btnCancel.MoveWindow( cx-10-btnrc.Width(),cy-10-btnrc.Height(),btnrc.Width(),btnrc.Height() );

}



// enable the toolbar
void CHubDataDlg::OnSelchangedTreeHubdata(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	m_hRClickItem = m_treeHubData.GetSelectedItem();
	if( !m_hRClickItem )
		return;
	
	int nCtrlEnableVal = GetControlEnableValue( m_hRClickItem );
	if( nCtrlEnableVal == 0 )
	{
		m_treeHubData.SelectItem( NULL );
		return;
	}

	m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONADD, nCtrlEnableVal & CONTROL_ENABLE_ADD ? TRUE : FALSE );
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONEDIT, nCtrlEnableVal & CONTROL_ENABLE_MOD ? TRUE : FALSE );
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL, nCtrlEnableVal & CONTROL_ENABLE_DEL ? TRUE : FALSE );

	*pResult = 0;
}


// give HTREEITEM, and level, return treeitem of the paxtype
HTREEITEM CHubDataDlg::GetPaxTypeItem( HTREEITEM _hItem, int _nLevel )
{
	//assert( _nLevel >= 1 && _nLevel <= 4 );
	assert( _nLevel >= 1 && _nLevel <= 5 );
	HTREEITEM hItem = _hItem;
	for( int i=0; i<_nLevel-1; i++ )
	{
		hItem = m_treeHubData.GetParentItem( hItem );
	}
	return hItem;	
}