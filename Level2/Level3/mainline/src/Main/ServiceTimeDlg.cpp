// ServiceTimeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "ServiceTimeDlg.h"
#include "inputs\in_term.h"
#include "inputs\srvctime.h"
#include "inputs\probab.h"
#include "ProcesserDialog.h"
#include "PassengerTypeDialog.h"
//#include "PaxProbDistDlg.h"
#include "TermPlanDoc.h"



#include "..\inputs\con_db.h"
#include "..\inputs\fltdata.h"
#include "..\inputs\paxdata.h"
#include "..\inputs\probab.h"
#include "termplandoc.h"
#include "passengertypedialog.h"
#include "flightdialog.h"
#include "..\common\probdistmanager.h"
#include "ProbDistDlg.h"
#include ".\servicetimedlg.h"
#include <Common/ProbabilityDistribution.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define SELECTED_COLOR RGB(53,151,53)

//const CString CS_NEW_ITEM = "New Probability Distribution";
/////////////////////////////////////////////////////////////////////////////
// CServiceTimeDlg dialog


CServiceTimeDlg::CServiceTimeDlg(CWnd* pParent /*=NULL*/ )
	: CDialog(CServiceTimeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CServiceTimeDlg)
	//}}AFX_DATA_INIT
	m_nProcType=-1;
	m_nListPreIndex = -1;

	m_treeProc.setInputTerminal( GetInputTerminal() );
}


void CServiceTimeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CServiceTimeDlg)
	DDX_Control(pDX, IDC_BUTTON_NEW, m_btnProcNew);
	DDX_Control(pDX, IDC_LIST_PAX, 	m_listctrlData);
	DDX_Control(pDX, IDC_STATIC_TOOLBARCONTENTER, m_toolbarcontenter);
	DDX_Control(pDX, IDC_TREE_PROCESSOR, m_treeProc);
	DDX_Control(pDX, IDC_BUTTON_SAVE, m_btnSave);
	DDX_Control(pDX, IDC_BUTTON_DEL, m_btnProcDel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CServiceTimeDlg, CDialog)
	//{{AFX_MSG_MAP(CServiceTimeDlg)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BUTTON_NEWPROC, OnButtonNewproc)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnButtonDel)
	ON_BN_CLICKED(IDC_BUTTON_EDITPAX, OnButtonEditpax)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_PAX, OnEndlabeleditListPax)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_PAX, OnItemChangedListPax)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_PROCESSOR, OnSelchangedTreeProcessor)
	ON_COMMAND(ID_PEOPLEMOVER_DELETE, OnPeoplemoverDelete)
	ON_COMMAND(ID_PEOPLEMOVER_NEW, OnPeoplemoverNew)
	ON_NOTIFY(NM_CLICK, IDC_LIST_PAX, OnClickListPax)
	ON_BN_CLICKED(IDC_BUTTON_NEW, OnButtonNew)
	ON_WM_MOUSEMOVE()
	ON_MESSAGE( UM_LIST_CONTENT_CHANGED, OnListContentChanged )
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CServiceTimeDlg message handlers


void CServiceTimeDlg::ReloadPaxDistList( int _nDBIdx )
{
	
}

BOOL CServiceTimeDlg::OnInitDialog() 
{
	m_toolTips.InitTooltips( &m_listctrlData);
	
	CDialog::OnInitDialog();
	AfxGetApp()->BeginWaitCursor();
	CMobileElemConstraint* pPaxConstr = new CMobileElemConstraint(GetInputTerminal());
	pPaxConstr->initDefault();
	ConstantDistribution* pDist = new ConstantDistribution( 0 );

	m_pDefaultEntry = new ConstraintEntry();
	m_pDefaultEntry->initialize( pPaxConstr, pDist );
	
	m_treeProc.m_bDisplayAll=TRUE;
	InitToolbar();
	ReloadDatabase();
	SetListCtrl();
	ReloadListData(NULL);

				
	AfxGetApp()->EndWaitCursor();
	// Added by Xie Bo 2002.5.9
	// Set the new button of toolbar is unable
	m_listctrlData.DeleteAllItems();
	HTREEITEM hItem = m_treeProc.GetSelectedItem();
	if( hItem == NULL )
	{
		m_btnProcNew.EnableWindow(FALSE);
		m_btnProcDel.EnableWindow(FALSE);
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE,FALSE );
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_PEOPLEMOVER_NEW,FALSE );
		return TRUE;
	}
	int nDBIdx = m_treeProc.GetItemData( hItem );
	if(nDBIdx == -1)
	{
		m_btnProcNew.EnableWindow(TRUE);
		m_btnProcDel.EnableWindow(FALSE);
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE,FALSE );
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_PEOPLEMOVER_NEW,FALSE );
	}
	else
	{
		m_btnProcNew.EnableWindow(FALSE);
		m_btnProcDel.EnableWindow(TRUE);
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE,GetSelectedListItem() != -1 );
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_NEW,TRUE);	
	}

	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CServiceTimeDlg::OnButtonNewproc() 
{
	// TODO: Add your control notification handler code here

	// Modified by Xie Bo 2002.5.9
    // I don't know why the system calls this function and I think 
	// OnButtonNew() function should be called.
	OnButtonNew();
//	CProcesserDialog dlg( GetInputTerminal() );
//	if (dlg.DoModal() != IDOK)
//		return;
//
//	ProcessorID id;
//	if( !dlg.GetProcessorID(id) )
//		return;
//
//	// insert into database
//	char buf[128];
//    id.printID( buf );
//	m_treeProc.m_strSelectedID.Format("%s",buf);
//
//	PassengerConstraint* pPaxConstr = new PassengerConstraint();
//	pPaxConstr->initDefault();
//	ConstantDistribution* pDist = new ConstantDistribution( 0 );
//
//	ConstraintEntry* pNewEntry = new ConstraintEntry();
//	pNewEntry->initialize( pPaxConstr, pDist );
//
//	GetInputTerminal()->serviceTimeDB->insert( id, pNewEntry );
//
//	int nCount = GetInputTerminal()->serviceTimeDB->getCount();
//	// TRACE("\nOnButtonNewproc()\nThe GetInputTerminal()->serviceTimeDB->getCount() is %d\n",nCount);
////	ReloadProcessorTree();
////	SelectProc( &id );
////	m_btnSave.EnableWindow();
//
////	CProcesserDialog dlg( GetInputTerminal() );
////	if( dlg.DoModal() == IDCANCEL )
////		return;
////
////	ProcessorID id;
////	if( !dlg.GetProcessorID(id) )
////		return;
////
////
////	// insert a new entry into database.
////	GetInputTerminal()->procAssignDB->addEntry( id );
////
////	int nIdx = GetInputTerminal()->procAssignDB->findEntry( id );
////	assert( nIdx != INT_MAX );
//
//	ReloadDatabase();	
}

void CServiceTimeDlg::OnButtonDel() 
{
	// TODO: Add your control notification handler code here
//	int nCurSel = m_listboxProc.GetCurSel();
//	if( nCurSel < 0 )
//		return;
//
//	int nIdxDB = m_listboxProc.GetItemData( nCurSel );
//	GetInputTerminal()->serviceTimeDB->deleteItem( nIdxDB );
//	ReloadProcessorList();
//	SelectProc( NULL );	
//	m_btnSave.EnableWindow();
	HTREEITEM hSelItem = m_treeProc.GetSelectedItem();
	if( hSelItem == NULL )
		return;

	int nDBIdx = m_treeProc.GetItemData( hSelItem );

	if( nDBIdx == -1 )
		return;

	m_treeProc.m_strSelectedID=m_treeProc.GetItemText(hSelItem);
	GetInputTerminal()->serviceTimeDB->deleteItem( nDBIdx );
	ReloadDatabase();
	ReloadListData(NULL);

	// Added by Xie Bo 2002.5.9
	// Ensure visible
	if(m_treeProc.hSelItem==NULL)
		return;
	HTREEITEM hParent = m_treeProc.GetParentItem(m_treeProc.hSelItem);
	if (hParent != NULL)
		m_treeProc.Expand(hParent, TVE_EXPAND);
	m_treeProc.EnsureVisible(m_treeProc.hSelItem);
	m_treeProc.SelectItem(m_treeProc.hSelItem);
	
	m_btnProcDel.EnableWindow(FALSE);

}



void CServiceTimeDlg::OnButtonEditpax() 
{
	// TODO: Add your control notification handler code here

	// found the selection
//	int nCount = m_listctrlPaxDist.GetItemCount();
//	for( int i=0; i<nCount; i++ )
//	{
//		if( m_listctrlPaxDist.GetItemState( i, LVIS_SELECTED ) == LVIS_SELECTED )
//			break;
//	}
//	if( i == nCount )
//		return;
//
//
//	// get paxdb
//	int nCurSel = m_listboxProc.GetCurSel();
//	if( nCurSel < 0 )
//		return;
//	int nIdxDB = m_listboxProc.GetItemData( nCurSel );
//	PassengerConstraintDatabase* pPaxConstrDB = GetInputTerminal()->serviceTimeDB->getDatabase(nIdxDB);
//
//	int nIdx = m_listctrlPaxDist.GetItemData( i );
//
//	ConstraintEntry* pEntry = pPaxConstrDB->getItem( nIdx );
//
//	CPaxProbDistDlg dlg( pEntry );
//	if( dlg.DoModal() == IDOK )
//	{
//		ReloadPaxDistList( nIdx );
//		m_btnSave.EnableWindow();
//	}
//	
}


void CServiceTimeDlg::OnButtonSave() 
{
	// TODO: Add your control notification handler code here
	GetInputTerminal()->serviceTimeDB->saveDataSet( GetProjPath(), true );
	m_btnSave.EnableWindow( false );
	
}

void CServiceTimeDlg::OnCancel() 
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
		GetInputTerminal()->serviceTimeDB->loadDataSet(GetProjPath());		
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

void CServiceTimeDlg::OnOK() 
{
	// TODO: Add extra validation here
	if( !m_btnSave.IsWindowEnabled() )  // nonzero if the window was previously disabled
	{
		CDialog::OnOK();
		return;
	}
	
	AfxGetApp()->BeginWaitCursor();
	GetInputTerminal()->serviceTimeDB->saveDataSet( GetProjPath(), true );	
	AfxGetApp()->EndWaitCursor();

	OnButtonSave();

	CDialog::OnOK();
}

InputTerminal* CServiceTimeDlg::GetInputTerminal()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
    return (InputTerminal*)&pDoc->GetTerminal();
}


CString CServiceTimeDlg::GetProjPath()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	return pDoc->m_ProjInfo.path;
}


void CServiceTimeDlg::SetListCtrl()
{
//	DWORD dwStyle = m_lstPax.GetExtendedStyle();
//	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
//	m_lstPax.SetExtendedStyle( dwStyle );
//
//	LV_COLUMNEX	lvc;
//	lvc.mask = LVCF_WIDTH | LVCF_TEXT ;
//	char columnLabel[2][128];
//	ConstraintDatabase* pConDB = GetConstraintDatabase();
//	sprintf( columnLabel[1], "Probability Distribution ( %s )", pConDB->getUnits() );
//	CString csLabel;
////	switch( m_enumType )
////	{
////	case FLIGHT_DELAYS:
////	case FLIGHT_LOAD_FACTORS:
////	case FLIGHT_AC_CAPACITIES:
////	case FLIGHT_BOARDING_CALLS:
////		strcpy( columnLabel[0], "Flight Type" );
////		break;
////
////	default:
//		strcpy( columnLabel[0], "Passenger Type" );		
////		break;
////	}
//	int DefaultColumnWidth[] = { 200, 320 };
//	int nFormat[] = {	LVCFMT_CENTER, LVCFMT_CENTER };
//	for( int i=0; i<2; i++ )
//	{
//		CStringList strList;
//		strList.InsertAfter( strList.GetTailPosition(), CS_NEW_ITEM );
//		int nCount = PROBDISTMANAGER->getCount();
//		for( int m=0; m<nCount; m++ )
//		{
//			CProbDistEntry* pPBEntry = PROBDISTMANAGER->getItem( m );			
//			strList.InsertAfter( strList.GetTailPosition(), pPBEntry->m_csName );
//		}
//		lvc.csList = &strList;
//		lvc.pszText = columnLabel[i];
//		lvc.cx = DefaultColumnWidth[i];
//		if( i == 0 )
//			lvc.fmt = LVCFMT_NOEDIT;
//		else
//			lvc.fmt = LVCFMT_DROPDOWN;
//		m_lstPax.InsertColumn( i, &lvc );
//	}
	DWORD dwStyle = m_listctrlData.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_listctrlData.SetExtendedStyle( dwStyle );

	LV_COLUMNEX	lvc;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT ;
	char columnLabel[2][128];
//	ConstraintDatabase* pConDB = GetConstraintDatabase();
//	sprintf( columnLabel[1], "Probability Distribution ( %s )", pConDB->getUnits() );
	sprintf( columnLabel[1], "Probability Distribution ( SECONDS )" );
	CString csLabel;
//	switch( m_enumType )
//	{
//	case FLIGHT_DELAYS:
//	case FLIGHT_LOAD_FACTORS:
//	case FLIGHT_AC_CAPACITIES:
//	case FLIGHT_BOARDING_CALLS:
//		strcpy( columnLabel[0], "Flight Type" );
//		break;
//
//	default:
		strcpy( columnLabel[0], "Passenger Type" );		
//		break;
//	}
	int DefaultColumnWidth[] = { 150, 250 };
	int nFormat[] = {	LVCFMT_CENTER, LVCFMT_CENTER };
	for( int i=0; i<2; i++ )
	{
		CStringList strList;
		CString s;
		s.LoadString(IDS_STRING_NEWDIST);
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
	m_listctrlData.InsertOtherCol(2);
	m_listctrlData.SetInputTerminal( GetInputTerminal() );
	
}

void CServiceTimeDlg::ReloadListData( Constraint* _pSelCon )
{
//	m_lstPax.DeleteAllItems();
//	ConstraintDatabase* pConDB = GetConstraintDatabase();
//	assert( pConDB );
//	int nCount = pConDB->getCount();
//	for( int i=0; i<nCount; i++ )
//	{
//		ConstraintEntry* pEntry = pConDB->getItem( i );
//		const Constraint* pCon = pEntry->getConstraint();
//		char szName[256];
//		pCon->screenPrint( szName, 256 );
//		const ProbabilityDistribution* pProbDist = pEntry->getValue();
//		char szDist[256];
//		pProbDist->screenPrint( szDist );
//		int nIdx = m_lstPax.InsertItem( i, szName );
//		m_lstPax.SetItemData( nIdx, i );
//		m_lstPax.SetItemText( nIdx, 1, szDist );
//		if( pCon == _pSelCon )
//			m_lstPax.SetItemState( nIdx, LVIS_SELECTED, LVIS_SELECTED );
//	}	
//	m_btnPaxDel.EnableWindow( GetSelectedListItem() != -1 );

	bool bIsOwn;
	ConstraintDatabase* pConDB = GetConstraintDatabase( bIsOwn );
	if(pConDB==NULL)
	{
		m_listctrlData.DeleteAllItems();
		const Constraint* pCon = m_pDefaultEntry->getConstraint();
		//char szName[256];
		CString szName;
		pCon->screenPrint( szName, 0, 256 );
		const ProbabilityDistribution* pProbDist = m_pDefaultEntry->getValue();
		char szDist[256];
		pProbDist->screenPrint( szDist );
		int nIdx = m_listctrlData.InsertItem( 0, szName.GetBuffer(0) );
		m_listctrlData.SetItemData( nIdx, -1 );
		m_listctrlData.SetItemText( nIdx, 1, szDist );

	}
	else
	{
		
		m_listctrlData.SetCurConDB( pConDB, bIsOwn?0:-1 );
		m_listctrlData.ReloadData( _pSelCon );
	}
	// Deleted by Xie Bo 2002.5.9
	//m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE,GetSelectedListItem() != -1 );	
	//	m_btnPaxDel.EnableWindow( GetSelectedListItem() != -1 );
}

ConstraintDatabase* CServiceTimeDlg::GetConstraintDatabase( bool& _bIsOwn )
{
	//ConstraintDatabase* pConDB = NULL;

	//??? Which constraintDatabase should I return?
	//    Xie Bo 2002.3.27
//	switch( m_enumType )
//	{
//	case FLIGHT_DELAYS:
//		pConDB = GetInputTerminal()->flightData->getDelays();
//		break;
//	case FLIGHT_LOAD_FACTORS:
//		pConDB = GetInputTerminal()->flightData->getLoads();
//		break;
//	case FLIGHT_AC_CAPACITIES:
//		pConDB = GetInputTerminal()->flightData->getCapacity();
//		break;
//	case FLIGHT_BOARDING_CALLS:
//		pConDB = GetInputTerminal()->flightData->getCalls();
//		break;
//	case PAX_BAG_COUNT:
//		pConDB = GetInputTerminal()->paxDataList->getBags();
//		break;
//	case PAX_CART_COUNT:
//		pConDB = GetInputTerminal()->paxDataList->getCarts();
//		break;
//	case PAX_GROUP_SIZE:
//		pConDB = GetInputTerminal()->paxDataList->getGroups();
//		break;
//	case PAX_LEAD_LAG:
//		pConDB = GetInputTerminal()->paxDataList->getLeadLagTime();
//		break;
//	case PAX_IN_STEP:
//		pConDB = GetInputTerminal()->paxDataList->getInStep();
//		break;
//	case PAX_SIDE_STEP:
//		pConDB = GetInputTerminal()->paxDataList->getSideStep();
//		break;
//	case PAX_SPEED:
//		pConDB = GetInputTerminal()->paxDataList->getSpeed();
//		break;
//	case PAX_VISITORS:
//		pConDB = GetInputTerminal()->paxDataList->getVisitors();
//		break;
//	case PAX_VISIT_TIME:
//		pConDB = GetInputTerminal()->paxDataList->getVisitTime();
//		break;
//	}
	HTREEITEM hSelItem = m_treeProc.GetSelectedItem();
	if( hSelItem == NULL )
		return NULL;
	int nDBIdx = m_treeProc.GetItemData( hSelItem );
	if( nDBIdx != -1 )
		_bIsOwn = true;
	else
		_bIsOwn = false;
	while( nDBIdx == -1 && hSelItem != NULL ) 
	{
		hSelItem = m_treeProc.GetParentItem( hSelItem );
		if( hSelItem == NULL )
			break;
		nDBIdx = m_treeProc.GetItemData( hSelItem );
	}
	if(nDBIdx == -1)
	{
		return NULL;
	}
	return GetInputTerminal()->serviceTimeDB->getDatabase(nDBIdx);
}

int CServiceTimeDlg::GetSelectedListItem()
{
	int nCount = m_listctrlData.GetItemCount();
	for( int i=0; i<nCount; i++ )
	{
		if( m_listctrlData.GetItemState( i, LVIS_SELECTED ) == LVIS_SELECTED )
			return i;
	}
	return -1;
}

void CServiceTimeDlg::OnEndlabeleditListPax(NMHDR* pNMHDR, LRESULT* pResult) 
{
//	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
//	// TODO: Add your control notification handler code here
//	
//	*pResult = 0;
//	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
//	// TODO: Add your control notification handler code here
// 	LV_ITEM* plvItem = &pDispInfo->item;
//
//	if( plvItem->iSubItem == 1 )
//	{
//
//		// change data.
//		ProbabilityDistribution* pProbDist = NULL;
//		if( strcmp( plvItem->pszText, CS_NEW_ITEM ) == 0 )
//		{
//			CProbDistDlg dlg( true );
//			dlg.DoModal();
//			int nSelIdx = dlg.GetLastSelection();
//			CProbDistEntry* pPDEntry = PROBDISTMANAGER->getItem( nSelIdx );
//			m_lstPax.SetItemText( plvItem->iItem, plvItem->iSubItem, pPDEntry->m_csName );
//			pProbDist = pPDEntry->m_pProbDist;
//		}
//		else
//		{
//			CProbDistEntry* pPDEntry = NULL;
//			int nCount = PROBDISTMANAGER->getCount();
//			for( int i=0; i<nCount; i++ )
//			{
//				pPDEntry = PROBDISTMANAGER->getItem( i );
//				if( strcmp( pPDEntry->m_csName, plvItem->pszText ) == 0 )
//					break;
//			}
//			assert( i < nCount );
//			pProbDist = pPDEntry->m_pProbDist;
//		}
//		assert( pProbDist );
//
//		ConstraintDatabase* pConDB = GetConstraintDatabase();
//		ConstraintEntry* pConEntry = pConDB->getItem( m_lstPax.GetItemData( plvItem->iItem ) );
//
//		int nDistType = pProbDist->getProbabilityType();
//
//		ProbabilityDistribution* pDist;
//
//		switch( nDistType )
//		{
//		case BERNOULLI:
//			pDist = new BernoulliDistribution( ((BernoulliDistribution*)pProbDist)->getValue1(),
//												((BernoulliDistribution*)pProbDist)->getValue2(),
//												((BernoulliDistribution*)pProbDist)->getProb1()	);
//			break;
//		case BETA:
//			pDist = new BetaDistribution( ((BetaDistribution*)pProbDist)->getMin(),
//											((BetaDistribution*)pProbDist)->getMax(),
//											((BetaDistribution*)pProbDist)->getAlpha(),
//											((BetaDistribution*)pProbDist)->getBeta() );
//			break;
//		case CONSTANT:
//			pDist = new ConstantDistribution( ((ConstantDistribution*)pProbDist)->getMean() );
//			break;
//		case EXPONENTIAL:
//			pDist = new ExponentialDistribution( ((ExponentialDistribution*)pProbDist)->getMean() );
//			break;
//		case NORMAL:
//			pDist = new NormalDistribution( ((NormalDistribution*)pProbDist)->getMean(),
//											((NormalDistribution*)pProbDist)->getStdDev() );
//			break;
//		case UNIFORM:
//			pDist = new UniformDistribution( ((UniformDistribution*)pProbDist)->getMin(),
//												((UniformDistribution*)pProbDist)->getMax() );
//			break;
//		case EMPIRICAL:
//			{
//			pDist = new EmpiricalDistribution();
//			char szDist[1024];
//			((EmpiricalDistribution*)pProbDist)->printDistribution( szDist );
//			((EmpiricalDistribution*)pDist)->setDistribution( szDist );
//			break;
//			}
//		case HISTOGRAM:
//			{
//			pDist = new HistogramDistribution();
//			char szDist[1024];
//			((HistogramDistribution*)pProbDist)->printDistribution( szDist );
//			((HistogramDistribution*)pDist)->setDistribution( szDist );
//			break;
//			}
//		}
//		pConEntry->setValue( pDist );
//	}
//	
//	*pResult = 0;
	m_btnSave.EnableWindow();
}

void CServiceTimeDlg::OnItemChangedListPax(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	HTREEITEM hItem = m_treeProc.GetSelectedItem();
	if( hItem == NULL )
		return ;
	int nDBIdx = m_treeProc.GetItemData( hItem );
	if(nDBIdx == -1)
	{
		return;
	}
	if( GetSelectedListItem() != -1 )
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE );
	*pResult = 0;
}

void CServiceTimeDlg::ReloadDatabase()
{
	m_treeProc.LoadData( GetInputTerminal(), (ProcessorDatabase*)GetInputTerminal()->serviceTimeDB );
	//m_treeProc.SelectItem( NULL );
}

void CServiceTimeDlg::OnSelchangedTreeProcessor(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	ReloadListData(NULL);
	// Modified by Xie Bo 2002.5.9
	// If the selected tree node is not in Database,enable the "new" button,
	// and unenable the "delete" button
	HTREEITEM hItem = m_treeProc.GetSelectedItem();
	if( hItem == NULL )
		return ;
	int nDBIdx = m_treeProc.GetItemData( hItem );
	if(nDBIdx == -1)
	{
		m_btnProcNew.EnableWindow(TRUE);
		m_btnProcDel.EnableWindow(FALSE);
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE,FALSE );
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_PEOPLEMOVER_NEW,FALSE );
	}
	else
	{
		m_btnProcNew.EnableWindow(FALSE);
		m_btnProcDel.EnableWindow(TRUE);
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE,GetSelectedListItem() != -1 );
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_NEW,TRUE);	
	}

	*pResult = 0;
	// TRACE("OnSelchangedTreeProcessor\n");
	//AfxMessageBox("OnSelchangedTreeProcessor");
}

void CServiceTimeDlg::InitToolbar()
{
	CRect rc;
	m_toolbarcontenter.GetWindowRect( &rc );
	ScreenToClient(&rc);
	m_ToolBar.MoveWindow(&rc);
	m_ToolBar.ShowWindow(SW_SHOW);
	m_toolbarcontenter.ShowWindow(SW_HIDE);
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_NEW );
	m_ToolBar.GetToolBarCtrl().HideButton( ID_PEOPLEMOVER_CHANGE );
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE,FALSE );

	
	
}

int CServiceTimeDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	if (!m_ToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_ToolBar.LoadToolBar(IDR_PEOPLEMOVEBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	return 0;
}



void CServiceTimeDlg::OnPeoplemoverDelete() 
{
	// TODO: Add your command handler code here
	if( m_listctrlData.DeleteEntry() )
		m_btnSave.EnableWindow();

	int nItemCount=m_listctrlData.GetItemCount();
	if(nItemCount==0)
	{
		OnButtonDel();
	}

}

void CServiceTimeDlg::OnPeoplemoverNew() 
{
	// TODO: Add your command handler code here
	
	bool bDirty = false;
	HTREEITEM hItem = m_treeProc.GetSelectedItem();
	if( hItem == NULL )
		return ;

	// Added by Xie Bo 2002.5.9
	int nDBIdx = m_treeProc.GetItemData( hItem );
	if(nDBIdx == -1)
	{
		return;
	}
	
	// the item is already selected
	CPassengerTypeDialog dlg( m_pParentWnd );
	if (dlg.DoModal() == IDOK)
	{
		// Modified by Xie Bo 2002.5.9
		// This "new" button can only add new element to a exist canstraint
		// We'll add a new "new" button to do "only add a new constraint to a database"
//		if(nDBIdx == -1)
//		{
//			m_listctrlData.DeleteAllItems();
//			
//			ProcessorID id;
//			CString str = m_treeProc.GetItemText( hItem );
//			id.SetStrDict( GetInputTerminal()->inStrDict );
//			id.setID( str );
//			
//			// insert into database
//			
//			PassengerConstraint* pPaxConstr = new PassengerConstraint();
//			*pPaxConstr = dlg.GetPaxSelection();
//			ConstantDistribution* pDist = new ConstantDistribution( 0 );
//			
//			ConstraintEntry* pNewEntry = new ConstraintEntry();
//			pNewEntry->initialize( pPaxConstr, pDist );
//			
//			GetInputTerminal()->serviceTimeDB->insert( id, pNewEntry );
//
//			m_treeProc.ResetTreeData( NULL );
//
//			bool bIsOwn;
//			ConstraintDatabase* pConDB = GetConstraintDatabase( bIsOwn );
//					
//			m_listctrlData.SetCurConDB( pConDB, bIsOwn?0:-1 );
//			m_listctrlData.ReloadData( NULL );
//			bDirty = TRUE;
//	//		bDirty = m_listctrlData.AddEntry( PAX_CON, pPaxConstr );
//
//		}
//		else
//		{
//			PassengerConstraint newPaxCon = dlg.GetPaxSelection();
//			bool bIsOwn;
//			ConstraintDatabase* pConDB = GetConstraintDatabase( bIsOwn );
//					
//			m_listctrlData.SetCurConDB( pConDB, bIsOwn?0:-1 );
//			bDirty = m_listctrlData.AddEntry( PAX_CON, &newPaxCon );
//
//		}
		//PassengerConstraint newPaxCon = dlg.GetPaxSelection();
		CMobileElemConstraint mobileConstr = dlg.GetMobileSelection();
		bool bIsOwn;
		ConstraintDatabase* pConDB = GetConstraintDatabase( bIsOwn );
		
		m_listctrlData.SetCurConDB( pConDB, bIsOwn?0:-1 );
		bDirty = m_listctrlData.AddEntry( PAX_CON, &mobileConstr );	
	}
	
	if( bDirty )
		m_btnSave.EnableWindow();
}

BOOL CServiceTimeDlg::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	delete m_pDefaultEntry;

	return CDialog::DestroyWindow();
}



void CServiceTimeDlg::OnClickListPax(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	HTREEITEM hItem = m_treeProc.GetSelectedItem();
	if( hItem == NULL )
		return;
	if( m_treeProc.GetItemData( hItem ) == -1 )
		return;
	*pResult = 0;
}


void CServiceTimeDlg::OnButtonNew() 
{
	// TODO: Add your control notification handler code here
	bool bDirty = false;
	HTREEITEM hItem = m_treeProc.GetSelectedItem();
	if( hItem == NULL )
		return ;
	int nDBIdx = m_treeProc.GetItemData( hItem );
	if(nDBIdx == -1)
	{
		CPassengerTypeDialog dlg( m_pParentWnd );
		if (dlg.DoModal() == IDOK)
		{
			m_listctrlData.DeleteAllItems();
			
			ProcessorID id;
			CString str = m_treeProc.GetItemText( hItem );
			id.SetStrDict( GetInputTerminal()->inStrDict );
			id.setID( str );
			
			// insert into database
			
			//PassengerConstraint* pPaxConstr = new PassengerConstraint();
			//*pPaxConstr = dlg.GetPaxSelection();
			CMobileElemConstraint* pMobileConstr = new CMobileElemConstraint(GetInputTerminal());
			*pMobileConstr = dlg.GetMobileSelection();
			ConstantDistribution* pDist = new ConstantDistribution( 0 );
			
			ConstraintEntryEx* pNewEntryEx = new ConstraintEntryEx();
			pNewEntryEx->initialize( pMobileConstr, pDist );
			
			GetInputTerminal()->serviceTimeDB->insert( id, pNewEntryEx );
			
			m_treeProc.ResetTreeData( NULL );
			
			bool bIsOwn;
			ConstraintDatabase* pConDB = GetConstraintDatabase( bIsOwn );
			
			m_listctrlData.SetCurConDB( pConDB, bIsOwn?0:-1 );
			m_listctrlData.ReloadData( NULL );
			bDirty = TRUE;
		}
		
	}
	if( bDirty )
		m_btnSave.EnableWindow();


	// Refresh the state
	hItem = m_treeProc.GetSelectedItem();
	if( hItem == NULL )
		return ;
	nDBIdx = m_treeProc.GetItemData( hItem );
	if(nDBIdx == -1)
	{
		m_btnProcNew.EnableWindow(TRUE);
		m_btnProcDel.EnableWindow(FALSE);
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE,FALSE );
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_PEOPLEMOVER_NEW,FALSE );
	}
	else
	{
		m_btnProcNew.EnableWindow(FALSE);
		m_btnProcDel.EnableWindow(TRUE);
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE,GetSelectedListItem() != -1 );
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_NEW,TRUE);	
	}
}

BOOL CServiceTimeDlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	switch(pMsg->message)
	{
	case WM_LBUTTONDOWN:
	case WM_MOUSEMOVE:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
		m_toolTips.RelayEvent( pMsg );
		break;
	}
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CServiceTimeDlg::OnMouseMove(UINT nFlags, CPoint point) 
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

void CServiceTimeDlg::ShowTips(int iItemData)
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

LRESULT CServiceTimeDlg::OnListContentChanged( WPARAM wParam, LPARAM lParam )
{
	m_btnSave.EnableWindow( TRUE );
	return TRUE;
}
void CServiceTimeDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}
