// PaxFlowByProcDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "PaxFlowByProcDlg.h"
#include "TermPlanDoc.h"
#include "inputs\procdist.h"
#include "processerdialog.h"
#include "PassengerTypeDialog.h"
#include "common\WinMsg.h"
#include <Inputs/PROCIDList.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPaxFlowByProcDlg dialog


CPaxFlowByProcDlg::CPaxFlowByProcDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPaxFlowByProcDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPaxFlowByProcDlg)
	//}}AFX_DATA_INIT
	m_hModifiedItem = NULL;
	m_hRClickItem = NULL;

	m_treeFlow.setInputTerminal( GetInputTerminal() );
}


void CPaxFlowByProcDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPaxFlowByProcDlg)
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDOK, m_btnOk);
	DDX_Control(pDX, IDC_BTN_NEW, m_btnNew);
	DDX_Control(pDX, IDC_STATIC_LISTFRAME, m_BarFrame);
	DDX_Control(pDX, IDC_STATIC_TOOLBARCONTENTER, m_toolbarcontenter);
	DDX_Control(pDX, IDC_BUTTON_SAVE, m_btnSave);
	DDX_Control(pDX, IDC_TREE_FLOW, m_treeFlow);
	DDX_Control(pDX, IDC_BTN_DELETE, m_btnDel);
	DDX_Control(pDX, IDC_LIST_PROCESSOR, m_listProc);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPaxFlowByProcDlg, CDialog)
	//{{AFX_MSG_MAP(CPaxFlowByProcDlg)
	ON_LBN_SELCHANGE(IDC_LIST_PROCESSOR, OnSelchangeListProcessor)
	ON_BN_CLICKED(IDC_BTN_NEW, OnBtnNew)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBtnSave)
	ON_BN_CLICKED(IDC_BTN_DELETE, OnBtnDelete)
	ON_COMMAND(IDC_BUTTON_ADD, OnButtonAdd)
	ON_COMMAND(IDC_BUTTON_DEL, OnButtonDel)
	ON_BN_CLICKED(IDC_BUTTON_EDIT, OnButtonEdit)
	ON_COMMAND(IDC_BUTTON_PRINT, OnButtonPrint)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_PAXFLOWBYPROC_ADDDESTPROCESSOR, OnPaxflowbyprocAdddestprocessor)
	ON_COMMAND(ID_PAXFLOWBYPROC_ADDPASSENGERTYPE, OnPaxflowbyprocAddpassengertype)
	ON_COMMAND(ID_PAXFLOWBYPROC_DELETEPASSENGERTYPE, OnPaxflowbyprocDeletepassengertype)
	ON_COMMAND(ID_PAXFLOWBYPROC_DELETEPROCESSOR, OnPaxflowbyprocDeleteprocessor)
	ON_COMMAND(ID_PAXFLOWBYPROC_MODIFYPERCENT, OnPaxflowbyprocModifypercent)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_FLOW, OnSelchangedTreeFlow)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPaxFlowByProcDlg message handlers

BOOL CPaxFlowByProcDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	InitToolbar();

	ReloadProcList();
	m_btnSave.EnableWindow( false );	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


#define INDEX_BASE 5000

void CPaxFlowByProcDlg::ReloadProcList()
{
	m_listProc.ResetContent();
	int nCount = GetInputTerminal()->m_pPassengerFlowDB->getCount();
	for( int i = 0; i < nCount; i++ )
	{
		char szBuf[256];
		const ProcessorID* pID = GetInputTerminal()->m_pPassengerFlowDB->getProcName( i );
		pID->printID( szBuf );
		int nIdx = m_listProc.AddString( szBuf );
		m_listProc.SetItemData( nIdx, i );
	}

	// now load from the station pax flow.
	nCount = GetInputTerminal()->m_pStationPaxFlowDB->getCount();
	for(int i = 0; i < nCount; i++ )
	{
		char szBuf[256];
		const ProcessorID* pID = GetInputTerminal()->m_pStationPaxFlowDB->getProcName( i );
		pID->printID( szBuf );
		strcat( szBuf, "(DEP)" );
		int nIdx = m_listProc.AddString( szBuf );
		m_listProc.SetItemData( nIdx, i + INDEX_BASE );
	}
	m_listProc.SetCurSel(-1);
	ReloadTree();
}



InputTerminal* CPaxFlowByProcDlg::GetInputTerminal()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
    return (InputTerminal*)&pDoc->GetTerminal();
}


CString CPaxFlowByProcDlg::GetProjPath()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	return pDoc->m_ProjInfo.path;
}


void CPaxFlowByProcDlg::ReloadTree()
{
	m_treeFlow.DeleteAllItems();

	int nSelIdx = m_listProc.GetCurSel();
	if( nSelIdx == LB_ERR )
	{
		m_btnDel.EnableWindow( FALSE );
		return;
	}

	m_btnDel.EnableWindow();

	CMobileElemConstraintDatabase* pConDB = GetDBBaseOnSelection( nSelIdx );
		
	int nPaxCount = pConDB->getCount();
	for( int i=0; i<nPaxCount; i++ )
	{
		CMobileElemConstraint* pCon = (CMobileElemConstraint*)pConDB->GetConstraint( i );
		CString szBuf;//[256];
		pCon->screenPrint( szBuf, 0 ,256 );
		HTREEITEM hItem = m_treeFlow.InsertItem( szBuf.GetBuffer(0) );
		m_treeFlow.SetItemData( hItem, i );

		// now load the proc dest.
		ProcessorDistribution* pProcDist = (ProcessorDistribution*)pConDB->getDist( i );
		int nDistCount = pProcDist->GetDestCount();
		int nCum = 0;
		for( int m=0; m<nDistCount; m++ )
		{
			ProcessorID* pProcID = pProcDist->getGroup( m );
			pProcID->SetStrDict( GetInputTerminal()->inStrDict );
			int nProb = static_cast<int>(pProcDist->getProb( m ));
			int nThisProb = nProb - nCum;
			nCum = nProb;
			HTREEITEM hDestItem = m_treeFlow.InsertItem( GetLabel( *pProcID, nThisProb ) , hItem );
			m_treeFlow.SetItemData( hDestItem, m );
		}
		m_treeFlow.Expand( hItem, TVE_EXPAND );
	}
	CheckToolBarEnable();	
}


CString CPaxFlowByProcDlg::GetLabel( const ProcessorID& _procId, int _nPercent )
{
	char szBuf[256];
	_procId.printID(szBuf);
	CString str;
	if( _nPercent >= 0 )
		str.Format( "%s (%d%%)", szBuf, _nPercent );
	else
		str = szBuf;
	return str;
}	



void CPaxFlowByProcDlg::CheckToolBarEnable()
{
	m_ToolBar.GetToolBarCtrl().EnableButton( IDC_BUTTON_ADD,FALSE );
	m_ToolBar.GetToolBarCtrl().EnableButton( IDC_BUTTON_EDIT,FALSE );
	m_ToolBar.GetToolBarCtrl().EnableButton( IDC_BUTTON_DEL,FALSE );

	m_hRClickItem = m_treeFlow.GetSelectedItem();

	if( m_hRClickItem == NULL )
	{
		m_ToolBar.GetToolBarCtrl().EnableButton( IDC_BUTTON_ADD ); // ADD PAX TYPE
		return;
	}

	if( m_treeFlow.GetParentItem( m_hRClickItem ) == NULL )
	{	
		m_ToolBar.GetToolBarCtrl().EnableButton( IDC_BUTTON_ADD );	// ADD DEST PROC
		m_ToolBar.GetToolBarCtrl().EnableButton( IDC_BUTTON_DEL );	// DELETE PAX TYPE
	}
	else
	{
		m_ToolBar.GetToolBarCtrl().EnableButton( IDC_BUTTON_EDIT);	// EDIT PROC
		m_ToolBar.GetToolBarCtrl().EnableButton( IDC_BUTTON_DEL );	// DELETE PROC 
	}
}

void CPaxFlowByProcDlg::OnSelchangeListProcessor() 
{
	// TODO: Add your control notification handler code here
	ReloadTree();
}

void CPaxFlowByProcDlg::OnBtnNew() 
{
	// TODO: Add your control notification handler code here
	CProcesserDialog dlg( GetInputTerminal() );
	if (dlg.DoModal() != IDOK)
		return;

	ProcessorID id;
	ProcessorIDList idList;

	if( !dlg.GetProcessorIDList(idList) )
		return;

	int nIdCount = idList.getCount();
	for(int i = 0; i < nIdCount; i++)
	{
		id = *idList[i];		
		char szName[256];
		id.printID( szName );
		
		// now new processor
		GetInputTerminal()->m_pPassengerFlowDB->addEntry( id );
		
		ReloadProcList();		
		m_listProc.SelectString( 0, szName );
	}	

	ReloadTree();

	m_btnSave.EnableWindow( true );

}

void CPaxFlowByProcDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	if( !m_btnSave.IsWindowEnabled() )  // nonzero if the window was previously disabled
	{
		CDialog::OnCancel();
		return;
	}
	
	// RELOAD DATABASE
	AfxGetApp()->BeginWaitCursor();
	try
	{
		GetInputTerminal()->m_pPassengerFlowDB->loadDataSet( GetProjPath() );
		GetInputTerminal()->m_pStationPaxFlowDB->loadDataSet( GetProjPath() );
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

void CPaxFlowByProcDlg::OnOK() 
{
	// TODO: Add extra validation here
	if( !All100Percent() )
	{
		MessageBox( "Not all Processors has 100% Distributed", "Error", MB_OK|MB_ICONWARNING );
		return;
	}

	if( !m_btnSave.IsWindowEnabled() )  // nonzero if the window was previously disabled
	{
		CDialog::OnOK();
		return;
	}
	
	AfxGetApp()->BeginWaitCursor();
	GetInputTerminal()->m_pPassengerFlowDB->saveDataSet( GetProjPath(), true );	
	GetInputTerminal()->m_pStationPaxFlowDB->saveDataSet( GetProjPath(), true );	
	AfxGetApp()->EndWaitCursor();
	
	CDialog::OnOK();
}

void CPaxFlowByProcDlg::OnBtnSave() 
{
	// TODO: Add your control notification handler code here
	if( !All100Percent() )
	{
		MessageBox( "Not all Processors has 100% Distributed", "Error", MB_OK|MB_ICONWARNING );
		return;
	}

	// SAVE THE WHOLE DATA
	GetInputTerminal()->m_pPassengerFlowDB->saveDataSet( GetProjPath(), true );	
	GetInputTerminal()->m_pStationPaxFlowDB->saveDataSet( GetProjPath(), true );	
	m_btnSave.EnableWindow( FALSE );	
}

bool CPaxFlowByProcDlg::All100Percent( bool _bStationFlowPaxDB )
{
	PassengerFlowDatabase* pFlowDB;
	if( _bStationFlowPaxDB )
		pFlowDB = GetInputTerminal()->m_pStationPaxFlowDB;
	else
		pFlowDB = GetInputTerminal()->m_pPassengerFlowDB;

	int nSrcProcCount = pFlowDB->getCount();
	for( int i=0; i<nSrcProcCount; i++ )
	{
		CMobileElemConstraintDatabase* pPaxConDB = pFlowDB->getDatabase( i );
		int nPaxConCount = pPaxConDB->getCount();
		for( int m=0; m<nPaxConCount; m++ )
		{
			const ProcessorDistribution* pProcDist = (const ProcessorDistribution*)pPaxConDB->getDist( m );
			int nDistCount = pProcDist->getCount();
			if( pProcDist->getProb( nDistCount-1) != 100 )
			{
				return false;
			}
		}
	}
	return true;
}

bool CPaxFlowByProcDlg::All100Percent()
{
	if( !All100Percent( true ) )
		return false;

	return All100Percent( false );	
}

void CPaxFlowByProcDlg::OnBtnDelete() 
{
	// TODO: Add your control notification handler code here
	int nSelIdx = m_listProc.GetCurSel();
	if( nSelIdx == LB_ERR )
		return;

	int nDBIdx = m_listProc.GetItemData( nSelIdx );
	if( nDBIdx < INDEX_BASE )
		GetInputTerminal()->m_pPassengerFlowDB->deleteItem( nDBIdx );
	else
		GetInputTerminal()->m_pStationPaxFlowDB->deleteItem( nDBIdx - INDEX_BASE );
	ReloadProcList();	
	m_btnSave.EnableWindow( TRUE );
}


void CPaxFlowByProcDlg::InitToolbar()
{
	CRect rc;
	m_toolbarcontenter.GetWindowRect( &rc );
	ScreenToClient(&rc);
	m_ToolBar.MoveWindow(&rc);
	m_ToolBar.ShowWindow(SW_SHOW);
	m_toolbarcontenter.ShowWindow(SW_HIDE);
	m_ToolBar.GetToolBarCtrl().EnableButton( IDC_BUTTON_ADD,FALSE );
	m_ToolBar.GetToolBarCtrl().EnableButton( IDC_BUTTON_DEL,FALSE );
	m_ToolBar.GetToolBarCtrl().EnableButton( IDC_BUTTON_EDIT,FALSE );

	m_ToolBar.GetToolBarCtrl().HideButton( ID_BUTTON_SPLIT );		
}


// new pax type
void CPaxFlowByProcDlg::OnButtonAdd() 
{
	m_hRClickItem = m_treeFlow.GetSelectedItem();
	if( m_hRClickItem == NULL )
		OnPaxflowbyprocAddpassengertype();
	else 
		OnPaxflowbyprocAdddestprocessor();
}

void CPaxFlowByProcDlg::OnButtonDel() 
{
	// TODO: Add your command handler code here
	int nSelIdx = m_listProc.GetCurSel();
	if( nSelIdx == LB_ERR )
		return;

	m_hRClickItem = m_treeFlow.GetSelectedItem();
	if( m_hRClickItem == NULL )
		return;

	HTREEITEM hParItem = m_treeFlow.GetParentItem( m_hRClickItem );
	if( hParItem == NULL )
		OnPaxflowbyprocDeletepassengertype();
	else
		OnPaxflowbyprocDeleteprocessor();

}

void CPaxFlowByProcDlg::OnButtonEdit() 
{
	// TODO: Add your command handler code here
	m_hModifiedItem=m_hRClickItem = m_treeFlow.GetSelectedItem();
	m_treeFlow.SpinEditLabel( m_hRClickItem );	
	m_btnSave.EnableWindow();
}

void CPaxFlowByProcDlg::OnButtonPrint() 
{
	// TODO: Add your command handler code here
	m_treeFlow.PrintTree("Tree header","Tree footer");
}


int CPaxFlowByProcDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	if (!m_ToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_ToolBar.LoadToolBar(IDR_FLIGHTSCHEDULEBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	
	return 0;
}

void CPaxFlowByProcDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	CRect  newrc, btnrc,toolbarrc,rc;
	m_btnNew.GetWindowRect( &btnrc );
	m_toolbarcontenter.GetWindowRect( &toolbarrc );
	newrc.left = 270;
	newrc.top = 35;
	newrc.bottom = cy-16;
	newrc.right =  cx -10;
	m_BarFrame.MoveWindow( 270,3,newrc.Width(),newrc.Height() );
	m_toolbarcontenter.MoveWindow( 272,9,newrc.Width()-10,toolbarrc.Height() );
	
//	m_ToolBar.MoveWindow( 157,7,newrc.Width(),toolbarrc.Height() );
	m_toolbarcontenter.GetWindowRect( &rc );
	ScreenToClient(&rc);
	m_toolbarcontenter.ShowWindow( SW_HIDE );
	m_ToolBar.MoveWindow(&rc);
	m_BarFrame.GetWindowRect( &rc );
	ScreenToClient(&rc);
	newrc.bottom = rc.bottom;
	m_treeFlow.MoveWindow( &newrc );
	m_btnNew.MoveWindow( cx-30-5*btnrc.Width(),cy-10-btnrc.Height(),btnrc.Width(),btnrc.Height() );
    m_btnDel.MoveWindow( cx-25-4*btnrc.Width(),cy-10-btnrc.Height(),btnrc.Width(),btnrc.Height() );
	m_btnSave.MoveWindow( cx-20-3*btnrc.Width(),cy-10-btnrc.Height(),btnrc.Width(),btnrc.Height() );
	m_btnOk.MoveWindow( cx-15-2*btnrc.Width(),cy-10-btnrc.Height(),btnrc.Width(),btnrc.Height() );
	m_btnCancel.MoveWindow( cx-10-btnrc.Width(),cy-10-btnrc.Height(),btnrc.Width(),btnrc.Height() );	
}


void CPaxFlowByProcDlg::GetEvenPercent( int _nCount, double *_pProb )
{
	int nTemp = 0;
	for( int i=0; i<_nCount; i++ )
	{
		_pProb[i] = ( 100 - nTemp ) / ( _nCount - i ) ;
		nTemp += static_cast<int>(_pProb[i]);
	}
}


LRESULT CPaxFlowByProcDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	if (message == WM_INPLACE_SPIN)
	{
		LPSPINTEXT pst = (LPSPINTEXT) lParam;

		SetPercent( pst->iPercent );
		return TRUE;
	}
		
	return CDialog::DefWindowProc(message, wParam, lParam);
}


void CPaxFlowByProcDlg::SetPercent(int _nPercent)
{
	// TODO: Add your command handler code here
	if( !m_hModifiedItem )
		return;

	int nSelIdx = m_listProc.GetCurSel();
	if( nSelIdx == LB_ERR )
		return;

	CMobileElemConstraintDatabase* pConDB = GetDBBaseOnSelection( nSelIdx );

	int nDataIdx = m_treeFlow.GetItemData( m_hModifiedItem );
	HTREEITEM hParItem = m_treeFlow.GetParentItem( m_hModifiedItem );
	if( hParItem == NULL )
		return;

	int nConIdx = m_treeFlow.GetItemData( hParItem );
	ProcessorDistribution* pProcDist = (ProcessorDistribution*)pConDB->getDist( nConIdx );
	pProcDist->setProb( _nPercent, nDataIdx );

	ReloadTree();
}


void CPaxFlowByProcDlg::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: Add your message handler code here
	m_hRClickItem = NULL;

	CRect rcFlowTree;
	GetDlgItem(IDC_TREE_FLOW)->GetWindowRect(&rcFlowTree);
	if (!rcFlowTree.PtInRect(point)) 
		return;
	
	m_treeFlow.SetFocus();   // Set focus

	CPoint pt = point;
	m_treeFlow.ScreenToClient(&pt);
	UINT iRet;
	HTREEITEM hItem = m_treeFlow.HitTest(pt, &iRet);
	if (iRet != TVHT_ONITEMLABEL)   // Must click on the label
		return;
							 
	m_hRClickItem = hItem;
	
	CMenu menuPopup;
	menuPopup.LoadMenu( IDR_MENU_POPUP );
	CMenu* pMenu = menuPopup.GetSubMenu( 8 );

	if( m_treeFlow.GetParentItem( m_hRClickItem ) == NULL )	// pax type
	{		
		pMenu->EnableMenuItem( ID_PAXFLOWBYPROC_DELETEPROCESSOR, MF_GRAYED );
		pMenu->EnableMenuItem( ID_PAXFLOWBYPROC_MODIFYPERCENT, MF_GRAYED );
	}
	else
	{
		pMenu->EnableMenuItem( ID_PAXFLOWBYPROC_ADDPASSENGERTYPE, MF_GRAYED );
		pMenu->EnableMenuItem( ID_PAXFLOWBYPROC_DELETEPASSENGERTYPE, MF_GRAYED );
		pMenu->EnableMenuItem( ID_PAXFLOWBYPROC_ADDDESTPROCESSOR, MF_GRAYED );
	}
	m_treeFlow.SelectItem( m_hRClickItem );   // Select it

	pMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);	
	
}

void CPaxFlowByProcDlg::OnPaxflowbyprocAdddestprocessor() 
{
	// TODO: Add your command handler code here
	if( !m_hRClickItem )
		return;

	int nSelIdx = m_listProc.GetCurSel();
	if( nSelIdx == LB_ERR )
		return;

	CMobileElemConstraintDatabase* pConDB = GetDBBaseOnSelection( nSelIdx );

	CProcesserDialog dlg( GetInputTerminal() );
	if (dlg.DoModal() != IDOK)
		return;

	ProcessorID id;
	ProcessorIDList idList;
	if( !dlg.GetProcessorIDList(idList) )
		return;

	int nIDList = idList.getCount();
	for( int i = 0; i < nIDList; i++)
	{
		id = *idList[i];
		
		char szName[256];
		id.printID( szName );
		
		int nConIdx = m_treeFlow.GetItemData( m_hRClickItem );
		ProcessorDistribution* pProcDist = (ProcessorDistribution*)pConDB->getDist( nConIdx );
		assert( pProcDist );
		
		// set destination.
		int nCurDestCount = pProcDist->getCount();
		int nNewDestCount = nCurDestCount + 1;
		if( nCurDestCount == 1 )
		{
			// check if is "END"
			Processor* pProc = GetInputTerminal()->procList->getProcessor( END_PROCESSOR_INDEX );
			if( *(pProcDist->getGroup(0)) == *( pProc->getID() ) )
				nNewDestCount = 1;
		}
		
		ProcessorID* pDestList = new ProcessorID[nNewDestCount];
		double* pProbabilities = new double[nNewDestCount];
		GetEvenPercent( nNewDestCount, pProbabilities );
		for( int i=0; i<nNewDestCount-1; i++ )
			pDestList[i] = *( pProcDist->getGroup( i ) );
		pDestList[nNewDestCount-1] = id;
		pProcDist->initDestinations( pDestList, pProbabilities, nNewDestCount );
		
	}
	ReloadTree();
	m_btnSave.EnableWindow();
	m_hRClickItem = NULL;
}

void CPaxFlowByProcDlg::OnPaxflowbyprocAddpassengertype() 
{
	// TODO: Add your command handler code here
	int nSelIdx = m_listProc.GetCurSel();
	if( nSelIdx == LB_ERR )
		return;

	CMobileElemConstraintDatabase* pConDB = GetDBBaseOnSelection( nSelIdx );

	CPassengerTypeDialog dlg( m_pParentWnd );
	if (dlg.DoModal() == IDCANCEL)
		return;


	//PassengerConstraint paxConstr = dlg.GetPaxSelection();
	CMobileElemConstraint mobileConstr = dlg.GetMobileSelection();
	//char rawString[256]; MATT
	char rawString[2560];
	
	mobileConstr.WriteSyntaxStringWithVersion( rawString );

	// check if exist
	int nCount = pConDB->getCount();
	for( int i=0; i<nCount; i++ )
	{
		//char csThisRaw[256]; MATT
		char csThisRaw[2560];
		
		const CMobileElemConstraint* pCon = pConDB->GetConstraint( i );
		pCon->WriteSyntaxStringWithVersion( csThisRaw );
		if( strcmp( csThisRaw, rawString ) == 0 )
		{
			// select item
			return;
		}
	}

	// pax con not exist. should insert pax con in the db with "END" as child processor
	ProcessorDistribution* pProcDist = new ProcessorDistribution();
	ProcessorID* pDestList = new ProcessorID[1];
	Processor* pProc = GetInputTerminal()->procList->getProcessor( END_PROCESSOR_INDEX );
	pDestList[0] = *( pProc->getID());
	double* pProbabilities = new double[1];
	pProbabilities[0] = 100;
	pProcDist->initDestinations( pDestList, pProbabilities, 1 );

	CMobileElemConstraint* pPaxConstr = new CMobileElemConstraint(GetInputTerminal());
	//pPaxConstr->SetInputTerminal( GetInputTerminal() );
	pPaxConstr->setConstraintWithVersion(rawString );
	ConstraintEntry* pNewEntry = new ConstraintEntry();
	pNewEntry->initialize( pPaxConstr, pProcDist );
	pConDB->addEntry( pNewEntry );
	
	ReloadTree();
	m_btnSave.EnableWindow();
	m_hRClickItem = NULL;

}

void CPaxFlowByProcDlg::OnPaxflowbyprocDeletepassengertype() 
{
	// TODO: Add your command handler code here
	int nSelIdx = m_listProc.GetCurSel();
	if( nSelIdx == LB_ERR )
		return;

	CMobileElemConstraintDatabase* pConDB = GetDBBaseOnSelection( nSelIdx );

	if( m_hRClickItem == NULL )
		return;

	int nDataIdx = m_treeFlow.GetItemData( m_hRClickItem );
	pConDB->deleteItem( nDataIdx );

	ReloadTree();
	m_btnSave.EnableWindow();
	m_hRClickItem = NULL;
	
}

void CPaxFlowByProcDlg::OnPaxflowbyprocDeleteprocessor() 
{
	// TODO: Add your command handler code here
	int nSelIdx = m_listProc.GetCurSel();
	if( nSelIdx == LB_ERR )
		return;

	CMobileElemConstraintDatabase* pConDB = GetDBBaseOnSelection( nSelIdx );

	if( m_hRClickItem == NULL )
		return;

	int nDataIdx = m_treeFlow.GetItemData( m_hRClickItem );
	HTREEITEM hParItem = m_treeFlow.GetParentItem( m_hRClickItem );
	if( hParItem == NULL )
		return;

	int nConIdx = m_treeFlow.GetItemData( hParItem );
	ProcessorDistribution* pProcDist = (ProcessorDistribution*)pConDB->getDist( nConIdx );
	ProcessorID* pId = pProcDist->getGroup( nDataIdx );

	assert( pProcDist );

	// set destination.
	int nCurDestCount = pProcDist->getCount();
	if( nCurDestCount == 1 )
	{
		// change to "END"
		Processor* pProc = GetInputTerminal()->procList->getProcessor( END_PROCESSOR_INDEX );
		pProcDist->changeEntry( *pId, *pProc->getID() ,GetInputTerminal());
	}
	else
	{
		int nNewDestCount = nCurDestCount - 1;
		ProcessorID* pDestList = new ProcessorID[nNewDestCount];
		double* pProbabilities = new double[nNewDestCount];
		GetEvenPercent( nNewDestCount, pProbabilities );
		int nIdx = 0;
		for( int i=0; i<nCurDestCount; i++ )
		{
			if( i == nDataIdx )
				continue;
			pDestList[nIdx++] = *( pProcDist->getGroup( i ) );
		}
		pProcDist->initDestinations( pDestList, pProbabilities, nNewDestCount );
	}	

	ReloadTree();
	m_btnSave.EnableWindow();
	m_hRClickItem = NULL;
	
}

void CPaxFlowByProcDlg::OnPaxflowbyprocModifypercent() 
{
	// TODO: Add your command handler code here
	if( !m_hRClickItem )
		return;    // Must be a child item
	
	// Show edit box
	m_hModifiedItem = m_hRClickItem;
	m_treeFlow.SpinEditLabel(m_hRClickItem);	
	m_btnSave.EnableWindow();	
}


void CPaxFlowByProcDlg::OnSelchangedTreeFlow(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	CheckToolBarEnable();	
	*pResult = 0;
}



// input: selection on the list
// output: database
CMobileElemConstraintDatabase* CPaxFlowByProcDlg::GetDBBaseOnSelection( int _nSelIdx )
{
	int nDBIdx = m_listProc.GetItemData( _nSelIdx );
	if( nDBIdx < INDEX_BASE )
		return GetInputTerminal()->m_pPassengerFlowDB->getDatabase( nDBIdx );

	return GetInputTerminal()->m_pStationPaxFlowDB->getDatabase( nDBIdx - INDEX_BASE );
	
}
