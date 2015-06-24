// BagDevDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "BagDevDlg.h"
#include "inputs\flt_db.h"
#include "inputs\in_term.h"
#include "inputs\bagdata.h"
#include "inputs\probab.h"
#include <CommonData/procid.h>
#include "FlightDialog.h"
#include "inputs\procdist.h"
#include "FlightDialog.h"
#include "ProcesserDialog.h"
#include "TermPlanDoc.h"
#include "common\WinMsg.h"
#include "common\probdistmanager.h"
#include "inputs\BagGateAssign.h"
#include "DlgProbDist.h"
#include "GateSelectionDlg.h"
#include "inputs\GageLagTimeData.h"
#include "inputs\schedule.h"
#include <Inputs/PROCIDList.h>
#include "./SelectALTObjectDialog.h"
#include "../AirsideGUI/DlgStandFamily.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBagDevDlg dialog


CBagDevDlg::CBagDevDlg(int nAirportID, CWnd* pParent)
	: CDialog(CBagDevDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBagDevDlg)
	m_nFligtPriority = -1;
	//}}AFX_DATA_INIT

	m_bFlightPriority = true;
	m_nListPreIndex = 0;
	m_nAirportID = nAirportID;
	//
	m_AssignTree.setInputTerminal( GetInputTerminal() );
}


void CBagDevDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBagDevDlg)
	DDX_Control(pDX, IDC_LIST_LAGTIME2, m_listctrlLagTime2);
	DDX_Control(pDX, IDC_TREE_ASSIGN, m_AssignTree);
	DDX_Control(pDX, IDC_BUTTON_SAVE, m_btnSave);
	DDX_Control(pDX, IDOK, m_btnOk);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDC_LIST_LAGTIME, m_listctrlLagTime);
	DDX_Control(pDX, IDC_STATIC_TOOLBARCONTENTER, m_toolbarcontenter);
	DDX_Control(pDX, IDC_STATIC_PRIORITY, m_SetPrioritycontenter);
	DDX_Control(pDX, IDC_STATIC_FBDT, m_Fbdtcontenter);
	DDX_Control(pDX, IDC_STATIC_CARBIN, m_Carbincontenter);
	DDX_Radio(pDX, IDC_RADIO_FLIGHTPRIORITY, m_nFligtPriority);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBagDevDlg, CDialog)
	//{{AFX_MSG_MAP(CBagDevDlg)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_NOTIFY(NM_CLICK, IDC_LIST_LAGTIME, OnClickListLagtime)
	ON_BN_CLICKED(IDC_BUTTON_ADD, OnButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnButtonDel)
	ON_BN_CLICKED(IDC_BUTTON_EDIT, OnButtonEdit)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_BAGASSIGN_ADD, OnBagassignAdd)
	ON_COMMAND(ID_BAGASSIGN_ADDBAGDEVICE, OnBagassignAddbagdevice)
	ON_COMMAND(ID_BAGASSIGN_DELETE, OnBagassignDelete)
	ON_COMMAND(ID_BAGASSIGN_PERCENT, OnBagassignPercent)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	ON_COMMAND(ID_PEOPLEMOVER_NEW, OnPeoplemoverNew)
	ON_COMMAND(ID_PEOPLEMOVER_DELETE, OnPeoplemoverDelete)
	ON_COMMAND(ID_PEOPLEMOVER_CHANGE,OnPeoplemoverChange)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_LAGTIME, OnItemchangedListLagtime)
	ON_BN_CLICKED(IDC_RADIO_FLIGHTPRIORITY, OnRadioFlightpriority)
	ON_BN_CLICKED(IDC_RADIO_GATE_PRIORITY, OnRadioGatePriority)
	ON_WM_MOUSEMOVE()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_LAGTIME2, OnItemchangedListLagtime2)
	ON_NOTIFY(NM_CLICK, IDC_LIST_LAGTIME2, OnClickListLagtime2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBagDevDlg message handlers

BOOL CBagDevDlg::OnInitDialog() 
{
	m_toolTips.InitTooltips( &m_listctrlLagTime);

	CDialog::OnInitDialog();
	
	AfxGetApp()->BeginWaitCursor();

	// TODO: Add extra initialization here

	// init list ctr	
	DWORD dwStyle = m_listctrlLagTime.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EDITLABELS;
	m_listctrlLagTime.SetExtendedStyle( dwStyle );
	m_listctrlLagTime2.SetExtendedStyle( dwStyle );

	LV_COLUMNEX	lvc;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT ;
	char columnLabel[4][128];

	int i,m,nCount;
	int DefaultColumnWidth[] = { 120, 250 };
	int nFormat[] = {	LVCFMT_LEFT, LVCFMT_LEFT };
	CStringList strList;
	CString s;
	CProbDistEntry* pPBEntry;

		strcpy( columnLabel[0], "Flight Type" );		
		strcpy( columnLabel[1], "Distribution ( Min )" );		
		
		s.LoadString(IDS_STRING_NEWDIST);
		strList.InsertAfter( strList.GetTailPosition(), s );
		nCount = _g_GetActiveProbMan( GetInputTerminal() )->getCount();
		for( m=0; m<nCount; m++ )
		{
			pPBEntry = _g_GetActiveProbMan( GetInputTerminal() )->getItem( m );			
			strList.InsertAfter( strList.GetTailPosition(), pPBEntry->m_csName );
		}
		lvc.csList = &strList;
		
		for( i=0; i<2; i++ )
		{
			lvc.pszText = columnLabel[i];
			lvc.cx = DefaultColumnWidth[i];
			if( i == 0 )
				lvc.fmt = LVCFMT_NOEDIT;
			if( i == 1 )
				lvc.fmt = LVCFMT_DROPDOWN;
			
			m_listctrlLagTime.InsertColumn( i, &lvc );
			if(i==0)
				lvc.pszText="Gate Type";
			m_listctrlLagTime2.InsertColumn( i, &lvc );
		}

		m_listctrlLagTime.SetInputTerminal(GetInputTerminal());
		m_listctrlLagTime2.SetInputTerminal(GetInputTerminal());

	InitToolbar();
	// load list ctr
	ReloadLagTimeList(-1);
	ReloadLagTimeList2(-1);
	ReloadData(NULL);

	// load tree
	if( GetInputTerminal()->bagGateAssignDB->IfGateHasHigherPriority() )
	{
		OnRadioGatePriority();
		CheckDlgButton( IDC_RADIO_GATE_PRIORITY, TRUE );
	}
	else
	{	
		OnRadioFlightpriority();
		CheckDlgButton(IDC_RADIO_FLIGHTPRIORITY, TRUE);		
	}
	
	// load gate
	//LoadGate();
	
	AfxGetApp()->EndWaitCursor();
	if(m_nFligtPriority==0)
	{
		m_listctrlLagTime.ShowWindow(SW_SHOW);
		m_listctrlLagTime2.ShowWindow(SW_HIDE);
	}
	else
	{
		m_listctrlLagTime2.ShowWindow(SW_SHOW);
		m_listctrlLagTime.ShowWindow(SW_HIDE);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CBagDevDlg::ReloadLagTimeList(int _nDBIdx)
{

	m_listctrlLagTime.DeleteAllItems();

    FlightConstraintDatabase* pLagTimeDB = GetInputTerminal()->bagData->getLagTimeDatabase();
	int nCount = pLagTimeDB->getCount();
	for( int i=0; i<nCount; i++ )
	{
		const FlightConstraint* pFltConstr = pLagTimeDB->getConstraint( i );
		char buf[1024];
	//	CString screenBuf;
		pFltConstr->screenPrint( buf,0,128 );
		int nIdx = m_listctrlLagTime.InsertItem( i, buf, 0 );
		const ProbabilityDistribution* pDist = pLagTimeDB->getItem( i )->getValue();
		pDist->screenPrint( buf);
		m_listctrlLagTime.SetItemText( nIdx, 1, buf );
		m_listctrlLagTime.SetItemData( nIdx, i );
		if( i == _nDBIdx )
			m_listctrlLagTime.SetItemState( nIdx, LVIS_SELECTED, LVIS_SELECTED);
	}
	if( _nDBIdx >= 0 )
	{
			m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE );	
	//	m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_CHANGE );
	//	m_btnDel.EnableWindow( true );
	//	m_btnEdit.EnableWindow( true );
	}
	else
	{
			m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE ,FALSE);	
	//	m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_CHANGE ,FALSE);

	//	m_btnDel.EnableWindow( false );
	//	m_btnEdit.EnableWindow( false );
	}

}

void CBagDevDlg::OnClickListLagtime(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	int nCount = m_listctrlLagTime.GetItemCount();
	for( int i=0; i<nCount; i++ )
	{
		if( m_listctrlLagTime.GetItemState( i, LVIS_SELECTED ) == LVIS_SELECTED )
			break;
	}
	bool bShow = false;
	if( i < nCount )
	{
			m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE );	
			m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_CHANGE);	
	//	m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_CHANGE );

	//	m_btnDel.EnableWindow( true );
	//	m_btnEdit.EnableWindow( true );
	}
	else
	{
			m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE ,FALSE);	
			m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_CHANGE ,FALSE);	
	//	m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_CHANGE ,FALSE);

	//	m_btnDel.EnableWindow( false );
	//	m_btnEdit.EnableWindow( false );
	}

	*pResult = 0;
}

void CBagDevDlg::OnButtonAdd() 
{
	// TODO: Add your control notification handler code here

	CFlightDialog dlg( m_pParentWnd );
	if( dlg.DoModal() == IDOK )
	{
		FlightConstraintDatabase* pLagTimeDB = GetInputTerminal()->bagData->getLagTimeDatabase();

		// get new flt constraint
		FlightConstraint newFltConstr = dlg.GetFlightSelection();

		// find if exist
		int nCount = m_listctrlLagTime.GetItemCount();
		for( int i=0; i<nCount; i++ )
		{
			int nIdx = m_listctrlLagTime.GetItemData( i );
			const FlightConstraint* pFltConstr = pLagTimeDB->getConstraint( nIdx );

			if( newFltConstr.isEqual(pFltConstr) )
			{
				// select the item
				SelectLagTimeList( i );
				return;
			}
		}
		if( i == nCount )
		{
			FlightConstraint* pFltConstr = new FlightConstraint;
			*pFltConstr = newFltConstr;
			ConstraintEntry* pNewEntry = new ConstraintEntry();
			ConstantDistribution* pDist = new ConstantDistribution( 0 );
			pNewEntry->initialize( pFltConstr, pDist );
			pLagTimeDB->addEntry( pNewEntry );
			ReloadLagTimeList(-1);
		}
		m_btnSave.EnableWindow();
	}
	
}

void CBagDevDlg::OnButtonDel() 
{
	// TODO: Add your control notification handler code here
	// found the selection
	int nCount = m_listctrlLagTime.GetItemCount();
	for( int i=0; i<nCount; i++ )
	{
		if( m_listctrlLagTime.GetItemState( i, LVIS_SELECTED ) == LVIS_SELECTED )
			break;
	}
	if( i == nCount )
		return;


	// get paxdb
	FlightConstraintDatabase* pLagTimeDB = GetInputTerminal()->bagData->getLagTimeDatabase();

	int nIdx = m_listctrlLagTime.GetItemData( i );

	pLagTimeDB->deleteItem( nIdx );

	ReloadLagTimeList(-1);
	m_btnSave.EnableWindow();
	
}



void CBagDevDlg::SelectLagTimeList(int _nIdx)
{
	int nCount = m_listctrlLagTime.GetItemCount();
	for( int i=0; i<nCount; i++ )
	{
		if( i == _nIdx )
			m_listctrlLagTime.SetItemState( i, LVIS_SELECTED, LVIS_SELECTED );
		else
			m_listctrlLagTime.SetItemState( i, 0, LVIS_SELECTED ); 
	}
}


void CBagDevDlg::OnButtonEdit() 
{
	// TODO: Add your control notification handler code here
	// found the selection
	ASSERT(0);
	/*
	int nCount = m_listctrlLagTime.GetItemCount();
	for( int i=0; i<nCount; i++ )
	{
		if( m_listctrlLagTime.GetItemState( i, LVIS_SELECTED ) == LVIS_SELECTED )
			break;
	}
	if( i == nCount )
		return;


	FlightConstraintDatabase* pLagTimeDB = GetInputTerminal()->bagData->getLagTimeDatabase();
	int nIdx = m_listctrlLagTime.GetItemData( i );

	ConstraintEntry* pEntry = pLagTimeDB->getItem( nIdx );

	CPaxProbDistDlg dlg( pEntry );
	if( dlg.DoModal() == IDOK )
	{
		ReloadLagTimeList( nIdx );
		m_btnSave.EnableWindow();
	}
	*/	
}

void CBagDevDlg::LoadTree()			// flight tree
{
 	m_AssignTree.DeleteAllItems();
	FlightConstraintDatabase* pAssignDB = GetInputTerminal()->bagData->getAssignmentDatabase();
	int nCount = pAssignDB->getCount();
	for( int i=0; i<nCount; i++ )
	{
		const Constraint* pConstr = pAssignDB->getItem( i )->getConstraint();
		//char buf[128];
		CString screenBuf;
		pConstr->screenPrint( screenBuf,0, 128 );
		HTREEITEM hItem = m_AssignTree.InsertItem( screenBuf.GetBuffer(0) );
		m_AssignTree.SetItemData( hItem, i );


		ProcessorDistribution* pDist = (ProcessorDistribution*)pAssignDB->getItem( i )->getValue();
		int nGroupCount = pDist->getCount();
		unsigned char nPrevVal = 0;
		for( int j=0; j<nGroupCount; j++ )
		{
			ProcessorID * pID = pDist->getGroup( j );
			int nPercentage = static_cast<int>(pDist->getProb( j ));
			
			char szBuf[1024];
			pID->printID(szBuf);
			CString str;
			str.Format( "%s (%d%%)", szBuf, nPercentage - nPrevVal );			
			nPrevVal = nPercentage;
	
			HTREEITEM hChildItem = m_AssignTree.InsertItem( str, hItem );
			m_AssignTree.SetItemData( hChildItem, j );
			m_AssignTree.Expand( hItem, TVE_EXPAND);
		}
	}
}

void CBagDevDlg::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: Add your message handler code here
	m_hRClickItem = NULL;

	CRect rectTree;
	m_AssignTree.GetWindowRect( &rectTree );
	if( !rectTree.PtInRect(point) ) 
		return;

	m_AssignTree.SetFocus();
	CPoint pt = point;
	m_AssignTree.ScreenToClient( &pt );
	UINT iRet;
	m_hRClickItem = m_AssignTree.HitTest( pt, &iRet );
	if (iRet != TVHT_ONITEMLABEL)   // Must click on the label
	{
		m_hRClickItem = NULL;
	}

	CMenu menuPopup;
	menuPopup.LoadMenu( IDR_MENU_POPUP );
	CMenu* pMenu = menuPopup.GetSubMenu( 0 );
	UpdateData();
	if(m_nFligtPriority==0)//Flight Has Higher Priority
	{
		pMenu->ModifyMenu(ID_BAGASSIGN_ADD,MF_BYCOMMAND|MF_STRING,ID_BAGASSIGN_ADD,"Add Flight Type");
	}
	else//Gate Has Higher Priority
	{
		pMenu->ModifyMenu(ID_BAGASSIGN_ADD,MF_BYCOMMAND|MF_STRING,ID_BAGASSIGN_ADD,"Add Gate Type");
	}
	if( m_hRClickItem == NULL )
	{
		m_AssignTree.SelectItem(NULL);
		pMenu->EnableMenuItem( ID_BAGASSIGN_ADDBAGDEVICE, MF_GRAYED );
		pMenu->EnableMenuItem( ID_BAGASSIGN_DELETE, MF_GRAYED );
		pMenu->EnableMenuItem( ID_BAGASSIGN_PERCENT, MF_GRAYED );
	}
	else
	{
		if( m_AssignTree.GetParentItem( m_hRClickItem ) == NULL )
			pMenu->EnableMenuItem( ID_BAGASSIGN_PERCENT, MF_GRAYED );
		else
			pMenu->EnableMenuItem( ID_BAGASSIGN_ADDBAGDEVICE, MF_GRAYED );
		m_AssignTree.SelectItem( m_hRClickItem );   // Select it
	}

	pMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);	
}

void CBagDevDlg::OnBagassignAdd() 
{
	// TODO: Add your command handler code here
	if(m_nFligtPriority==1)//Gate Has Heighter 
	{
		//CGateSelectionDlg dlg(this,-1,TRUE);
		//if(dlg.DoModal()==IDOK)
		//{
		//	ProcessorID gate = dlg.m_ProcID;			
		//	//ProcessorDistribution* pDist = new ProcessorDistribution();
		//	//GetInputTerminal()->bagGateAssignDB->insert( gate, pDist );
		//	GetInputTerminal()->bagGateAssignDB->addEntry( gate );
		//	LoadTree2();
		//	m_btnSave.EnableWindow();
		//	
		//}
		CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();			
	//	CSelectALTObjectDialog objDlg(pDoc->GetProjectID(),m_nAirportID);
	//	objDlg.SetType( ALT_STAND );
		CDlgStandFamily objDlg(pDoc->GetProjectID());
	
		if(objDlg.DoModal() == IDOK )
		{
			ProcessorID procID;
			procID.SetStrDict(GetInputTerminal()->inStrDict);
		//	procID.setID(_T(objDlg.GetSelectString()));
			procID.setID(_T(objDlg.GetSelStandFamilyName()));
			GetInputTerminal()->bagGateAssignDB->addEntry( procID );
			LoadTree2();
			m_btnSave.EnableWindow();
		};

		return;
	}

	
	CFlightDialog dlg(m_pParentWnd);
	if( dlg.DoModal() == IDOK )
	{
		FlightConstraintDatabase* pAssignDB = GetInputTerminal()->bagData->getAssignmentDatabase();

		// get new flt constraint
		FlightConstraint newFltConstr = dlg.GetFlightSelection();

		// find if exist
		HTREEITEM hItem = m_AssignTree.GetRootItem();
		while( hItem )
		{
			int nIdx = m_AssignTree.GetItemData( hItem );
			const FlightConstraint* pFltConstr = pAssignDB->getConstraint( nIdx );
			if( newFltConstr.isEqual(pFltConstr) )
			{
				// select the item
				m_AssignTree.SelectItem( hItem );
				return;
			}

			hItem = m_AssignTree.GetNextItem( hItem, TVGN_NEXT );

		}

		FlightConstraint* pFltConstr = new FlightConstraint;
		*pFltConstr = newFltConstr;
		ConstraintEntry* pEntry = new ConstraintEntry();
		ProcessorDistribution* pDist = new ProcessorDistribution();
		pEntry->initialize( pFltConstr, pDist );
		pAssignDB->addEntry( pEntry );
		LoadTree();		
		m_btnSave.EnableWindow();
	}	
}

void CBagDevDlg::OnBagassignAddbagdevice() 
{
	// get current db index
	if( !m_hRClickItem || m_AssignTree.GetParentItem( m_hRClickItem )  )
		return;

	int nIdx = m_AssignTree.GetItemData( m_hRClickItem );

	// pop up the processor dialog
	CProcesserDialog dlg( GetInputTerminal() );
	dlg.SetType( BaggageProc );
	if( dlg.DoModal() == IDCANCEL )
		return;

	ProcessorID id;
	ProcessorIDList idList;
	if( !dlg.GetProcessorIDList(idList) )
		return;
   
	int nIdCount =  idList.getCount();
	BOOL bChnaged1 = FALSE ;
	BOOL bChnaged2 = FALSE ;
	
	for( int i = 0; i < nIdCount; i++ )
	{
		id = *idList[i];
		
		
		if( m_nFligtPriority == 1)
		{
			BagGateEntry* pEntry = (BagGateEntry*)GetInputTerminal()->bagGateAssignDB->getItem( nIdx );
			ProcessorDistribution* pDist = pEntry->getProbDist();
			if( pDist == NULL )
			{
				ProcessorDistribution* pNewDist = new ProcessorDistribution();
				pEntry->setProbDist( pNewDist );
				pDist = pNewDist;
			}
			
			BOOL bExist = FALSE;
			// check if process exists
			HTREEITEM hChildItem = m_AssignTree.GetChildItem( m_hRClickItem );
			while( hChildItem )
			{
				int nValIdx = m_AssignTree.GetItemData( hChildItem );
				ProcessorID * pId = pDist->getGroup( nValIdx );
				if( *pId == id )
				{
					m_AssignTree.SelectItem( hChildItem );
					bExist = TRUE ;
					//return;
					break;
				}
				hChildItem = m_AssignTree.GetNextItem( hChildItem, TVGN_NEXT );
			}
			if ( !bExist )
			{	
				int nCount = pDist->getCount();
				nCount++;
				
				ProcessorID* dests = new ProcessorID[nCount];
				double* nPercent = new double[nCount];
				unsigned char nPrevVal = 0;

				for(int j = 0; j < nCount-1; j++ )
				{
					dests[j] = *( pDist->getGroup(j) );
				}
				dests[nCount-1] = id;
				EvenPercent( nPercent, nCount );
				
				pDist->initDestinations( dests, nPercent, nCount );
				delete []dests;
				delete [] nPercent;

				bChnaged1 = TRUE ;				
				
				//return;
				continue;				
			}

			
		}
		
		
		FlightConstraintDatabase* pAssignDB = GetInputTerminal()->bagData->getAssignmentDatabase();
		ConstraintEntry* pEntry = pAssignDB->getItem( nIdx );
		ProcessorDistribution* pDist = (ProcessorDistribution*)pEntry->getValue();
		
		BOOL bExist = FALSE;		
		// check if process exists
		HTREEITEM hChildItem = m_AssignTree.GetChildItem( m_hRClickItem );
		while( hChildItem )
		{
			int nValIdx = m_AssignTree.GetItemData( hChildItem );
			ProcessorID * pId = pDist->getGroup( nValIdx );
			if( *pId == id )
			{
				m_AssignTree.SelectItem( hChildItem );
				//return;
				bExist = TRUE ;				
				break;
			}
			hChildItem = m_AssignTree.GetNextItem( hChildItem, TVGN_NEXT );
		}		

		if ( !bExist )
		{
			// create the dist
			int nCount = pDist->getCount();
			nCount++;
			ProcessorID* dests = new ProcessorID[nCount];
			double* nPercent = new double[nCount];
			unsigned char nPrevVal = 0;
			for( int j=0; j<nCount-1; j++ )
			{
				dests[j] = *( pDist->getGroup(j) );
				//nPercent[i] = pDist->getProb(i) - nPrevVal;
				//nPrevVal = pDist->getProb(i);
			}
			dests[nCount-1] = id;
			//nPercent[i] = 100;
			EvenPercent( nPercent, nCount );
			
			ProcessorDistribution* pNewDist = new ProcessorDistribution;
			pNewDist->initDestinations( dests, nPercent, nCount );
			
			pEntry->setValue( pNewDist );
			
			delete [] dests;
			delete [] nPercent;

			bChnaged2 = TRUE ;
			
			// reload tree
		//	LoadTree();			
		}
	}
	if ( bChnaged1 )
	{
		LoadTree2();
	}
	if ( bChnaged2 )
	{	// reload tree
		LoadTree();	
	}
	if ( bChnaged1 || bChnaged2)
	{
		m_btnSave.EnableWindow();
	}
}

void CBagDevDlg::EvenPercent( double* _cPercent, int _iCount )
{
	
	if( _iCount > 0 )
	{
		int iPercent = 100 / _iCount ;
		if( 100 % _iCount == 0 )
		{
			for( int i=0; i< _iCount; ++i )
			{
				_cPercent[ i ] = iPercent ;
			}
		}
		else
		{
			for( int i=0; i< _iCount-1; ++i )
			{
				_cPercent[ i ] = iPercent ;
			}
		
			_cPercent[ _iCount-1 ] = 100- iPercent*(_iCount-1) ;
		}
	}

}
void CBagDevDlg::OnBagassignDelete() 
{
	// TODO: Add your command handler code here
	if( !m_hRClickItem )
		return;

	if( m_nFligtPriority == 1)		// gate
	{
		HTREEITEM hParentItem = m_AssignTree.GetParentItem( m_hRClickItem );
		if( hParentItem )
		{
			// remove dist entry
			int nIdx = m_AssignTree.GetItemData( hParentItem );
			BagGateEntry* pEntry =(BagGateEntry*) GetInputTerminal()->bagGateAssignDB->getItem( nIdx );
			nIdx = m_AssignTree.GetItemData( m_hRClickItem );
			ProcessorDistribution* pDist = pEntry->getProbDist();
			assert( pDist );
			ProcessorID* pId = pDist->getGroup( nIdx );
			pDist->removeEntry( *pId ,GetInputTerminal());
			pDist->EvenPercent();
		}
		else
		{
			// remove the gate db entry.
			int nIdx = m_AssignTree.GetItemData( m_hRClickItem );
			GetInputTerminal()->bagGateAssignDB->deleteItem( nIdx );
		}
		LoadTree2();			
		m_btnSave.EnableWindow();
		return;
	}

	FlightConstraintDatabase* pAssignDB = GetInputTerminal()->bagData->getAssignmentDatabase();
	HTREEITEM hParentItem = m_AssignTree.GetParentItem( m_hRClickItem );
	if( hParentItem )
	{
		// remove dist entry
		int nIdx = m_AssignTree.GetItemData( hParentItem );
		ConstraintEntry* pEntry = pAssignDB->getItem( nIdx );
		nIdx = m_AssignTree.GetItemData( m_hRClickItem );
		ProcessorDistribution* pDist = (ProcessorDistribution*)pEntry->getValue();
		ProcessorID* pId = pDist->getGroup( nIdx );
		pDist->removeEntry( *pId,GetInputTerminal() );
		pDist->EvenPercent();
	}
	else
	{
		// remove the flight db entry.
		int nIdx = m_AssignTree.GetItemData( m_hRClickItem );
		pAssignDB->deleteItem( nIdx );
	}
	LoadTree();			
	m_btnSave.EnableWindow();
}

void CBagDevDlg::OnBagassignPercent() 
{
	// TODO: Add your command handler code here
	if (!m_hRClickItem || !m_AssignTree.GetParentItem(m_hRClickItem))
		return;    // Must be a child item
	
	// Show edit box
	m_AssignTree.SpinEditLabel(m_hRClickItem);
	m_btnSave.EnableWindow();
}


LRESULT CBagDevDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if (message == WM_INPLACE_SPIN)
	{
		LPSPINTEXT pst = (LPSPINTEXT) lParam;

		SetPercent( pst->iPercent );
		return TRUE;
	}
	
	return CDialog::DefWindowProc(message, wParam, lParam);
}


void CBagDevDlg::SetPercent(int _nPercent)
{
	// TODO: Add your command handler code here
	if( !m_hRClickItem )
		return;

	HTREEITEM hParentItem = m_AssignTree.GetParentItem( m_hRClickItem );
	if( !hParentItem )
		return;

	if( m_nFligtPriority == 1 ) // gate
	{
		int nIdx = m_AssignTree.GetItemData( hParentItem );
		BagGateEntry* pEntry = (BagGateEntry*)GetInputTerminal()->bagGateAssignDB->getItem( nIdx );
		nIdx = m_AssignTree.GetItemData( m_hRClickItem );
		ProcessorDistribution* pDist = pEntry->getProbDist();
		pDist->setProb( _nPercent, nIdx );
		LoadTree2();
		return;
	}

	FlightConstraintDatabase* pAssignDB = GetInputTerminal()->bagData->getAssignmentDatabase();
	int nIdx = m_AssignTree.GetItemData( hParentItem );
	ConstraintEntry* pEntry = pAssignDB->getItem( nIdx );
	nIdx = m_AssignTree.GetItemData( m_hRClickItem );
	ProcessorDistribution* pDist = (ProcessorDistribution*)pEntry->getValue();
	pDist->setProb( _nPercent, nIdx );
	LoadTree();
}

void CBagDevDlg::OnButtonSave() 
{
	// TODO: Add your control notification handler code here
	if( !CheckPercent() )
		return;

	GetInputTerminal()->bagData->saveDataSet(GetProjPath(), true);
	GetInputTerminal()->bagGateAssignDB->SetPriorityFlag( m_nFligtPriority==1 );
	GetInputTerminal()->bagGateAssignDB->removeAllEmptyEntry();
	GetInputTerminal()->bagGateAssignDB->saveDataSet(GetProjPath(), true);
    GetInputTerminal()->bagData->BuildFlightAssignToBaggageDevice( GetInputTerminal()->flightSchedule );
	GetInputTerminal()->flightSchedule->saveDataSet(GetProjPath(),false);
	GetInputTerminal()->flightSchedule->assignBaggageCarosels(GetInputTerminal()->bagData,GetInputTerminal());
	
	if( m_nFligtPriority == 1 )
		LoadTree2();
}	

void CBagDevDlg::OnOK() 
{
	if( !CheckPercent() )
		return;
	
	GetInputTerminal()->bagData->saveDataSet(GetProjPath(), true);
	GetInputTerminal()->bagGateAssignDB->SetPriorityFlag( m_nFligtPriority==1 );
	GetInputTerminal()->bagGateAssignDB->removeAllEmptyEntry();
	GetInputTerminal()->bagGateAssignDB->saveDataSet(GetProjPath(), true);
    GetInputTerminal()->bagData->BuildFlightAssignToBaggageDevice( GetInputTerminal()->flightSchedule );
	GetInputTerminal()->flightSchedule->saveDataSet(GetProjPath(),false);
	GetInputTerminal()->flightSchedule->assignBaggageCarosels(GetInputTerminal()->bagData,GetInputTerminal());

	CDialog::OnOK();
}

void CBagDevDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	if( !m_btnSave.IsWindowEnabled() )  
	{
		CDialog::OnCancel();
		return;
	}
	
	AfxGetApp()->BeginWaitCursor();
	try
	{
		GetInputTerminal()->bagData->loadDataSet( GetProjPath() );
		GetInputTerminal()->bagGateAssignDB->loadDataSet( GetProjPath() );
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

bool CBagDevDlg::CheckPercent()
{
	if( m_nFligtPriority == 1)
	{
		HTREEITEM hItem = m_AssignTree.GetRootItem();
		while( hItem )
		{
			int nIdx = m_AssignTree.GetItemData( hItem );
			BagGateEntry* pEntry = (BagGateEntry*)GetInputTerminal()->bagGateAssignDB->getItem( nIdx );
			ProcessorDistribution* pDist = pEntry->getProbDist();
			if( pDist )
			{
				int nGroupCount = pDist->getCount();
				if( nGroupCount>0 )
				{
					int nLastPercent = static_cast<int>(pDist->getProb( nGroupCount - 1 ));
					if( nLastPercent != 100 )
					{
						MessageBox( "Total Percentage is not 100%%", "Error" );
						m_AssignTree.SelectItem( hItem );
						return false;
					}
				}
			}
			
			hItem = m_AssignTree.GetNextItem( hItem, TVGN_NEXT );
		}
		return true;
	}

	FlightConstraintDatabase* pAssignDB = GetInputTerminal()->bagData->getAssignmentDatabase();
	HTREEITEM hItem = m_AssignTree.GetRootItem();
	while( hItem )
	{
		int nIdx = m_AssignTree.GetItemData( hItem );
		ProcessorDistribution* pDist = (ProcessorDistribution*)pAssignDB->getItem( nIdx )->getValue();
		if( pDist && pDist->getCount()>0 )
		{
			int nLastPercent = static_cast<int>( pDist->getProb(pDist->getCount() - 1 ) );
			if( nLastPercent != 100 )
			{
				MessageBox( "Total Percentage is not 100%%", "Error" );
				m_AssignTree.SelectItem( hItem );
				return false;
			}
		}

		hItem = m_AssignTree.GetNextItem( hItem, TVGN_NEXT );
	}
	return true;
}

InputTerminal* CBagDevDlg::GetInputTerminal()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
    return (InputTerminal*)&pDoc->GetTerminal();
}


CString CBagDevDlg::GetProjPath()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	return pDoc->m_ProjInfo.path;
}

int CBagDevDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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


void CBagDevDlg::InitToolbar()
{

	
	CRect rc;

	//flight toolbar
	m_toolbarcontenter.GetWindowRect( &rc );
	ScreenToClient(&rc);
	m_ToolBar.MoveWindow(&rc);
	m_ToolBar.ShowWindow(SW_SHOW);
	m_toolbarcontenter.ShowWindow(SW_HIDE);
	m_ToolBar.GetToolBarCtrl().EnableButton(ID_PEOPLEMOVER_NEW,TRUE);
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE ,FALSE);	
	m_ToolBar.GetToolBarCtrl().HideButton( ID_PEOPLEMOVER_CHANGE);	

}

void CBagDevDlg::OnPeoplemoverNew() 
{
	// TODO: Add your command handler code here
	if(m_nFligtPriority==1)
	{
		CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();			
	//	CSelectALTObjectDialog objDlg(pDoc->GetProjectID(),m_nAirportID);

	//	objDlg.SetType( ALT_STAND );
		CDlgStandFamily objDlg(pDoc->GetProjectID());
		if(objDlg.DoModal() == IDOK )
		{
			CGateLagTimeDB* pGateLagTimeDB = GetInputTerminal()->bagData->getGateLagTimeDB();
			ProcessorID procID;
			procID.SetStrDict(GetInputTerminal()->inStrDict);
		//	procID.setID(objDlg.GetSelectString().GetBuffer());
			procID.setID(objDlg.GetSelStandFamilyName().GetBuffer());

			if( ! pGateLagTimeDB->FindEntry( procID  ) )
			{
				CGageLagTimeData* pData = new CGageLagTimeData( procID );
				ConstantDistribution* pDist = new ConstantDistribution( 0 );
				pData->SetDistribution( pDist );
				pGateLagTimeDB->addItem( pData );
				pGateLagTimeDB->buildHierarchy();
				ReloadLagTimeList2(-1);
			}			
		}
		return;

	}
	CFlightDialog dlg( m_pParentWnd );
	if( dlg.DoModal() == IDOK )
	{
		FlightConstraintDatabase* pLagTimeDB = GetInputTerminal()->bagData->getLagTimeDatabase();

		// get new flt constraint
		FlightConstraint newFltConstr = dlg.GetFlightSelection();

		// find if exist
		int nCount = m_listctrlLagTime.GetItemCount();
		for( int i=0; i<nCount; i++ )
		{
			int nIdx = m_listctrlLagTime.GetItemData( i );
			const FlightConstraint* pFltConstr = pLagTimeDB->getConstraint( nIdx );

			if( newFltConstr.isEqual(pFltConstr) )
			{
				// select the item
				SelectLagTimeList( i );
				return;
			}
		}
		if( i == nCount )
		{
			FlightConstraint* pFltConstr = new FlightConstraint;
			*pFltConstr = newFltConstr;
			ConstraintEntry* pNewEntry = new ConstraintEntry();
			ConstantDistribution* pDist = new ConstantDistribution( 0 );
			pNewEntry->initialize( pFltConstr, pDist );
			pLagTimeDB->addEntry( pNewEntry );
			ReloadLagTimeList(-1);
		}
		m_btnSave.EnableWindow();
	}

}

void CBagDevDlg::OnPeoplemoverDelete() 
{
	// TODO: Add your command handler code here
	if(m_nFligtPriority==1)
	{
		int nCount = m_listctrlLagTime2.GetItemCount();
		for( int i=0; i<nCount; i++ )
		{
			if( m_listctrlLagTime2.GetItemState( i, LVIS_SELECTED ) == LVIS_SELECTED )
				break;
		}
		if( i == nCount )
			return;
		
		
		// get paxdb
		CGateLagTimeDB* pLagTimeDB = GetInputTerminal()->bagData->getGateLagTimeDB();
		int nIdx = m_listctrlLagTime2.GetItemData( i );
		
		pLagTimeDB->deleteItem( nIdx );
		
		ReloadLagTimeList2(-1);
		m_btnSave.EnableWindow();
		return;
	}


	int nCount = m_listctrlLagTime.GetItemCount();
	for( int i=0; i<nCount; i++ )
	{
		if( m_listctrlLagTime.GetItemState( i, LVIS_SELECTED ) == LVIS_SELECTED )
			break;
	}
	if( i == nCount )
		return;


	// get paxdb
	FlightConstraintDatabase* pLagTimeDB = GetInputTerminal()->bagData->getLagTimeDatabase();

	int nIdx = m_listctrlLagTime.GetItemData( i );

	pLagTimeDB->deleteItem( nIdx );

	ReloadLagTimeList(-1);
	m_btnSave.EnableWindow();

}

/*void CBagDevDlg::OnPeoplemoverChange() 
{
	// TODO: Add your command handler code here
		int nCount = m_listctrlLagTime.GetItemCount();
	for( int i=0; i<nCount; i++ )
	{
		if( m_listctrlLagTime.GetItemState( i, LVIS_SELECTED ) == LVIS_SELECTED )
			break;
	}
	if( i == nCount )
		return;


	FlightConstraintDatabase* pLagTimeDB = GetInputTerminal()->bagData->getLagTimeDatabase();
	int nIdx = m_listctrlLagTime.GetItemData( i );

	ConstraintEntry* pEntry = pLagTimeDB->getItem( nIdx );

	CPaxProbDistDlg dlg( pEntry );
	if( dlg.DoModal() == IDOK )
	{
		ReloadLagTimeList( nIdx );
		m_btnSave.EnableWindow();
	}

}*/

void CBagDevDlg::ReloadData( Constraint* _pSelCon )
{
    FlightConstraintDatabase* pConDB = GetInputTerminal()->bagData->getLagTimeDatabase();
	m_listctrlLagTime.SetCurConDB( pConDB );
	m_listctrlLagTime.ReloadData( _pSelCon );

    CGateLagTimeDB* pConDB2 = GetInputTerminal()->bagData->getGateLagTimeDB();
	m_listctrlLagTime2.m_bIsDefault=FALSE;
	m_listctrlLagTime2.m_pGLTConDB=pConDB2;
	m_listctrlLagTime2.ReloadData( _pSelCon );

//	m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL,GetSelectedItem() != -1 );
}

int CBagDevDlg::GetSelectedItem()
{
	int nCount = m_listctrlLagTime.GetItemCount();
	for( int i=0; i<nCount; i++ )
	{
		if( m_listctrlLagTime.GetItemState( i, LVIS_SELECTED ) == LVIS_SELECTED )
			return i;
	}
	return -1;
}

void CBagDevDlg::OnItemchangedListLagtime(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	int nCount = m_listctrlLagTime.GetItemCount();
	for( int i=0; i<nCount; i++ )
	{
		if( m_listctrlLagTime.GetItemState( i, LVIS_SELECTED ) == LVIS_SELECTED )
			break;
	}
	bool bShow = false;
	if( i < nCount )
	{
			m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE );	
//	m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_CHANGE );

	//	m_btnDel.EnableWindow( true );
	//	m_btnEdit.EnableWindow( true );
	}
	else
	{
			m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE ,FALSE);	
//	m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_CHANGE ,FALSE);

	//	m_btnDel.EnableWindow( false );
	//	m_btnEdit.EnableWindow( false );
	}

	*pResult = 0;	
}

void CBagDevDlg::OnRadioFlightpriority() 
{
	m_nFligtPriority = 0;
	m_bFlightPriority = true;

	LoadTree();
	m_btnSave.EnableWindow( TRUE );
	if(m_nFligtPriority==0)
	{
		m_listctrlLagTime.ShowWindow(SW_SHOW);
		m_listctrlLagTime2.ShowWindow(SW_HIDE);
	}
	else
	{
		m_listctrlLagTime2.ShowWindow(SW_SHOW);
		m_listctrlLagTime.ShowWindow(SW_HIDE);
	}
	
}

void CBagDevDlg::OnRadioGatePriority() 
{
	m_nFligtPriority = 1;
	m_bFlightPriority = false;
	
	LoadTree2();
	m_btnSave.EnableWindow( TRUE );
	if(m_nFligtPriority==0)
	{
		m_listctrlLagTime.ShowWindow(SW_SHOW);
		m_listctrlLagTime2.ShowWindow(SW_HIDE);
	}
	else
	{
		m_listctrlLagTime2.ShowWindow(SW_SHOW);
		m_listctrlLagTime.ShowWindow(SW_HIDE);
	}
}

BOOL CBagDevDlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	m_toolTips.RelayEvent( pMsg );
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CBagDevDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	int nIndex = m_listctrlLagTime.HitTest( point );

	if( nIndex!=-1 )
	{
		if(m_nListPreIndex != nIndex)
		{
			int iItemData = m_listctrlLagTime.GetItemData( nIndex );
			ShowTips( iItemData );
			m_nListPreIndex = nIndex;
		}
	}
	else
	{
		m_nListPreIndex = -1;
		m_toolTips.DelTool( &m_listctrlLagTime );
	}
	CDialog::OnMouseMove(nFlags, point);
}

void CBagDevDlg::ShowTips(int iItemData)
{
	if( iItemData >= 0 )
	{
		ConstraintEntry* pEntry = m_listctrlLagTime.GetConDB()->getItem( iItemData);
		const Constraint* pCon = pEntry->getConstraint();

		CString strTips ;
		pCon->screenTips( strTips );
		m_toolTips.ShowTips( strTips );
	}
}


void CBagDevDlg::LoadTree2()
{
	m_AssignTree.DeleteAllItems();
	
	int iCount = GetInputTerminal()->bagGateAssignDB->getCount();
	for( int i=0; i<iCount; i++ )
	{
		BagGateEntry* pEntry = (BagGateEntry*)GetInputTerminal()->bagGateAssignDB->getItem( i );
		CString strID = pEntry->getID()->GetIDString();
		
		HTREEITEM hItem = m_AssignTree.InsertItem( strID );
		m_AssignTree.SetItemData( hItem, i );
		
		ProcessorDistribution* pDist = pEntry->getProbDist();
		if( pDist)
		{
			int nGroupCount = pDist->getCount();
			unsigned char nPrevVal = 0;
			for( int j = 0; j<nGroupCount; j++ )
			{
				ProcessorID* pID = pDist->getGroup( j );
				int pPercentage = static_cast<int>(pDist->getProb( j ));
				
				char szBuf[256];
				pID->printID( szBuf );
				CString str;
				str.Format( "%s (%d%%)", szBuf, pPercentage - nPrevVal );
				nPrevVal = pPercentage;
				
				HTREEITEM hChildItem = m_AssignTree.InsertItem( str, hItem );
				m_AssignTree.SetItemData( hChildItem, j );
			}
		}
		m_AssignTree.Expand( hItem, TVE_EXPAND );
	}
}

void CBagDevDlg::OnPeoplemoverChange()
{
	if(m_nFligtPriority==1)
	{

		POSITION pos = m_listctrlLagTime2.GetFirstSelectedItemPosition();
		while (pos)
		{
			CRect rect;
			int nItem = m_listctrlLagTime2.GetNextSelectedItem(pos);
			m_listctrlLagTime2.GetSubItemRect(nItem,1,LVIR_BOUNDS,rect);
			CPoint pt=rect.CenterPoint();
			m_listctrlLagTime2.SendMessage(WM_LBUTTONDBLCLK,0,MAKELPARAM(pt.x,pt.y));
			break;
		}
			
	}
	else
	{
		
		POSITION pos = m_listctrlLagTime.GetFirstSelectedItemPosition();
		while (pos)
		{
			CRect rect;
			int nItem = m_listctrlLagTime.GetNextSelectedItem(pos);
			m_listctrlLagTime.GetSubItemRect(nItem,1,LVIR_BOUNDS,rect);
			CPoint pt=rect.CenterPoint();
			m_listctrlLagTime.SendMessage(WM_LBUTTONDBLCLK,0,MAKELPARAM(pt.x,pt.y));
			break;
		}
	}
}

void CBagDevDlg::OnItemchangedListLagtime2(NMHDR* pNMHDR, LRESULT* pResult)
{
	int nCountSeled=m_listctrlLagTime2.GetSelectedCount();
	m_ToolBar.GetToolBarCtrl().EnableButton(ID_PEOPLEMOVER_DELETE,nCountSeled!=0);
	*pResult=0;
}



void CBagDevDlg::OnClickListLagtime2(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	int nCountSeled=m_listctrlLagTime2.GetSelectedCount();
	m_ToolBar.GetToolBarCtrl().EnableButton(ID_PEOPLEMOVER_DELETE,nCountSeled!=0);
	
	*pResult = 0;
}

void CBagDevDlg::ReloadLagTimeList2(int _nDBIdx)
{
	
	m_listctrlLagTime2.DeleteAllItems();
	

	CGateLagTimeDB* pGateLagTimeDB = GetInputTerminal()->bagData->getGateLagTimeDB();
				
	int nCount = pGateLagTimeDB->getCount();
	for( int i=0; i<nCount; i++ )
	{
		CGageLagTimeData* pFltConstr =(CGageLagTimeData*) pGateLagTimeDB->getItem(i);
		CString strTemp=pFltConstr->getID()->GetIDString();
		int nIdx = m_listctrlLagTime2.InsertItem( i, strTemp, 0 );
		char buf[1024];
		//CString sBuf;
		const ProbabilityDistribution* pDist = pFltConstr->GetDistribution();
		pDist->screenPrint( buf );
		m_listctrlLagTime2.SetItemText( nIdx, 1, buf );
		m_listctrlLagTime2.SetItemData( nIdx, i );
		if( i == _nDBIdx )
			m_listctrlLagTime2.SetItemState( nIdx, LVIS_SELECTED, LVIS_SELECTED);
	}
	if( _nDBIdx >= 0 )
	{
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE );	
		//	m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_CHANGE );
		//	m_btnDel.EnableWindow( true );
		//	m_btnEdit.EnableWindow( true );
	}
	else
	{
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE ,FALSE);	
		//	m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_CHANGE ,FALSE);
		
		//	m_btnDel.EnableWindow( false );
		//	m_btnEdit.EnableWindow( false );
	}
	
}

void CBagDevDlg::OnSize( UINT nType, int cx, int cy )
{
	CDialog::OnSize(nType, cx, cy);
	if( m_btnOk.m_hWnd == NULL )
		return;
	// TODO: Add your message handler code here
	CRect rc,timerc,assrc,gaterc;
	m_btnCancel.GetWindowRect(&rc);
	m_btnCancel.MoveWindow(cx-10-rc.Width(),cy-10-rc.Height(),rc.Width(),rc.Height());
	m_btnOk.MoveWindow(cx-20-2*rc.Width(),cy-10-rc.Height(),rc.Width(),rc.Height());
	m_btnSave.MoveWindow(cx-30-3*rc.Width(),cy-10-rc.Height(),rc.Width(),rc.Height());

	m_SetPrioritycontenter.MoveWindow(10,5,cx-20,50);

	GetDlgItem(IDC_RADIO_GATE_PRIORITY)->GetWindowRect(&gaterc);
	GetDlgItem(IDC_RADIO_GATE_PRIORITY)->MoveWindow(cx-50-gaterc.Width(),25,gaterc.Width(),gaterc.Height());
	//m_nGatePriority.GetWindowRect(&gaterc);
	//m_nGatePriority.MoveWindow(cx-10-gaterc.Width(),30,gaterc.Width(),gaterc.Height());
	m_Fbdtcontenter.MoveWindow(10,60,150,15);
	m_toolbarcontenter.MoveWindow(10,75,cx-20,30);
	m_ToolBar.MoveWindow(11,75,cx-20,30);

	m_listctrlLagTime.GetWindowRect(&timerc);
	m_listctrlLagTime.MoveWindow(10,100,cx-20,timerc.Height());
	m_listctrlLagTime2.MoveWindow(10,100,cx-20,timerc.Height());
	m_Carbincontenter.MoveWindow(10,100+timerc.Height(),150,30);

	m_AssignTree.GetWindowRect(&assrc);
	m_AssignTree.MoveWindow(10,130+timerc.Height(),cx-20,cy-150-rc.Height()-timerc.Height());
	InvalidateRect(NULL);
}

void CBagDevDlg::OnGetMinMaxInfo( MINMAXINFO FAR* lpMMI )
{
	// TODO: Add your message handler code here and/or call default

	lpMMI->ptMinTrackSize.x = 513;
	lpMMI->ptMinTrackSize.y = 673;

	CDialog::OnGetMinMaxInfo(lpMMI);
}