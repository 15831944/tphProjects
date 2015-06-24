// AutoRosterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "AutoRosterDlg.h"
#include <CommonData/procid.h>
#include "reports\SimGeneralPara.h"
#include "inputs\SimParameter.h"
#include "reports\SimLevelOfService.h"
#include "engine\proclist.h"
#include "engine\autoroster.h"
#include "inputs\assigndb.h"
#include "engine\AutoCreateProcessorContainer.h"
#include <Inputs/IN_TERM.H>
//#include "TermPlan.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#define CANNOT_MODIFY_COLOR		RGB(128,128,128)
const CString strRun = "Run";
/////////////////////////////////////////////////////////////////////////////
// CAutoRosterDlg dialog


CAutoRosterDlg::CAutoRosterDlg( InputTerminal* _inTerm,const CString& _strPath,CTermPlanDoc* _pDoc, CWnd* pParent /*=NULL*/ )
	: CDialog(CAutoRosterDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAutoRosterDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pInTerm = _inTerm;
	m_strPrjPath = _strPath;
	m_pTLOSSim = NULL;
	m_iPreSimResult = -1; 

	m_treeEvent.setInputTerminal( _inTerm );
	m_pDoc = _pDoc;
	m_bMustSelectOneResult= false;
}


void CAutoRosterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAutoRosterDlg)
	DDX_Control(pDX, IDOK, m_butOK);
	DDX_Control(pDX, IDCANCEL, m_butCancel);
	DDX_Control(pDX, IDC_TREE_PROC, m_treeProc);
	DDX_Control(pDX, IDC_TREE_EVENT, m_treeEvent);
	DDX_Control(pDX, IDC_STATIC_RESULT, m_staticResult);
	DDX_Control(pDX, IDC_STATIC_EVENT, m_staticEvent);
	DDX_Control(pDX, IDC_GTCHARTCTRL, m_gtCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAutoRosterDlg, CDialog)
	//{{AFX_MSG_MAP(CAutoRosterDlg)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_PROC, OnSelchangedTreeProc)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAutoRosterDlg message handlers

BOOL CAutoRosterDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	InitGTCtrl();
	InitProcTree();
	if( m_pInTerm->procList->GetDynamicCreatedProcs() && m_pInTerm->procList->GetDynamicCreatedProcs()->getProcessorCount() > 0 )
	{
		m_bMustSelectOneResult = true;
		m_butCancel.EnableWindow( TRUE );
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CAutoRosterDlg::InitGTCtrl()
{
	m_gtCtrl.InitControl( (long)AfxGetThread() );
	m_gtCtrl.SetMoveItemSnapLineable(false);
	m_gtCtrl.ShowBtn(0,false);
	m_gtCtrl.ShowBtn(2,false);
	//m_gtCtrl.ShowBtn(2,false);
	m_gtCtrl.ShowBtn(5,false);
	m_gtCtrl.ShowBtn(6,false);
	m_gtCtrl.ShowBtn(7,false);
	m_gtCtrl.ShowBtn(8,false);
	m_gtCtrl.ShowBtn(9,false);
	m_gtCtrl.ShowBtn(10,false);
	m_gtCtrl.ShowBtn(11,false);
	m_gtCtrl.ShowBtn(12,false);
	m_gtCtrl.ShowBtn(17,false);
}


void CAutoRosterDlg::InitProcTree()
{
	CSimGeneralPara* pSim = m_pInTerm->m_pSimParam->GetSimPara();
	assert( pSim );
	assert( pSim->GetClassType() == "LEVEOFSERVICE" ) ;
	m_pTLOSSim = (CSimLevelOfService*) pSim;

	// clear proc tree
	m_treeProc.DeleteAllItems();

	ProcessorID id;
	id.SetStrDict( m_pInTerm->inStrDict);
	
	int iRunNumber = pSim->GetNumberOfRun();
	for( int i= 0; i<iRunNumber; i++ )
	{
		CString strRunItem = GetRunName( i );
		HTREEITEM hRunItem = m_treeProc.InsertItem(strRunItem);
		m_treeProc.SetItemData( hRunItem, i );

		int iProcNum = m_pTLOSSim->getSettingCout();
		for( int j=0; j<iProcNum; j++ )
		{
			const CSimLevelOfServiceItem& item = m_pTLOSSim->getSetting( j );
			CString strProcName = item.GetProcessorName();
			id.setID( strProcName );
			AddProcToTree( hRunItem, id );
		}
		m_treeProc.Expand( hRunItem, TVE_EXPAND );
	}
}

void CAutoRosterDlg::AddProcToTree(HTREEITEM _hParent, const ProcessorID &_id)
{
	StringList strDict;
	ProcessorID ProcID;
	ProcID.SetStrDict( m_pInTerm->inStrDict );
	
	// insert item to tree
	CString strProc = _id.GetIDString();
	HTREEITEM hProcItem = m_treeProc.InsertItem( strProc, _hParent );
		
	m_pInTerm->procList->getMemberNames( _id, strDict, -1);
	for( int i= 0; i<strDict.getCount(); i++ )
	{
		CString strTemp = strDict.getString (i);
		strTemp.TrimLeft(); 
		strTemp.TrimRight();
		if (strTemp.IsEmpty())
			continue;
				
		strTemp = strProc + "-" + strTemp;
		ProcID.setID(strTemp);
		AddProcToTree( hProcItem, ProcID );
	}
	
	m_treeProc.Expand( hProcItem, TVE_EXPAND);
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// helper
void CAutoRosterDlg::ReloadRosterData(int _index)
{
	CWaitCursor wc;

	m_pInTerm->procAssignDB->loadDataSet( m_strPrjPath );
	m_pInTerm->m_pAutoRoster->clearTLOSProcRoster( m_pTLOSSim, m_pInTerm, false );
	m_pInTerm->m_pAutoRoster->addAutoRostorToDateSet( _index, m_pInTerm );
}

void CAutoRosterDlg::AddProcessorIDToList(const ProcessorID &_id)
{
	std::vector<ProcessorID>::iterator iter;
	iter = std::find( m_vProcessID.begin(), m_vProcessID.end(), _id );
	if( iter == m_vProcessID.end() )
	{
		m_vProcessID.push_back( _id );
	}
}


HTREEITEM CAutoRosterDlg::GetTheTopItem( CTreeCtrl* _treeCtrl, HTREEITEM _hCurItem,BOOL& _bIsTop)
{
	assert( _treeCtrl );
	_bIsTop = FALSE;

	HTREEITEM hResultItem = _hCurItem;
	_hCurItem = _treeCtrl->GetParentItem( _hCurItem );
	_bIsTop = _hCurItem == NULL ? TRUE : FALSE;

	while( _hCurItem )
	{
		hResultItem = _hCurItem;
		_hCurItem = _treeCtrl->GetParentItem( _hCurItem );
	}
	
	return hResultItem;
}


CString CAutoRosterDlg::GetRunName(int _index)
{
	CString strName;

	CString strNum;
	strNum.Format("%d", _index );
	int iLen= strNum.GetLength();
	for( int i=0; i<3-iLen; i++ )
	{
		strName += "0";
	}
	strName = strRun + strName + strNum;
	return strName;
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void CAutoRosterDlg::OnSelchangedTreeProc(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	m_butOK.EnableWindow( FALSE );

	HTREEITEM hCurSelItem = m_treeProc.GetSelectedItem();
	if( hCurSelItem == NULL )
		return;
	
	BOOL bIsTopItem;
	HTREEITEM hParentItem = GetTheTopItem( &m_treeProc, hCurSelItem, bIsTopItem );
	assert( hParentItem );
	CString strItem = m_treeProc.GetItemText( hCurSelItem );

	ShowAutoRoster( hParentItem, strItem, bIsTopItem );

	*pResult = 0;
}



void CAutoRosterDlg::ShowAutoRoster(HTREEITEM _hParent, const CString &_strItem, BOOL _bIsTopItem)
{
	int iCurSimResult = m_treeProc.GetItemData( _hParent );
	assert( iCurSimResult>=0 );
	if( iCurSimResult != m_iPreSimResult )
	{
		ReloadRosterData( iCurSimResult );
		m_iPreSimResult = iCurSimResult;
	}

	// clear data & GUI
	m_vProcessID.clear();
	m_treeEvent.DeleteAllItems();
	m_gtCtrl.Clear( TRUE );

	// insert data to GUI( read only )
	if( _bIsTopItem )
	{
		ShowAllRoster();
		m_butOK.EnableWindow( TRUE );
	}
	else
	{
		ShowProcGroupRoster( _strItem );
		m_butOK.EnableWindow( FALSE );
	}
}


void CAutoRosterDlg::ShowAllRoster()
{
	ProcessorID procID;
	procID.SetStrDict( m_pInTerm->inStrDict );

	int iCount = m_pTLOSSim->getSettingCout();
	for( int i=0; i<iCount; i++ )
	{
		const CSimLevelOfServiceItem& item= m_pTLOSSim->getSetting(i);
		CString strProcName = item.GetProcessorName();
		procID.setID( strProcName );
		// get all processor
		GroupIndex group = m_pInTerm->procList->getGroupIndex( procID );
		for( int i= group.start; i<= group.end; i++ )
		{
			ProcessorID id = *(m_pInTerm->procList->getProcessor(i)->getID());
			AddProcessorIDToList( id );
		}
	}

	// insert all processor to event tree and gt_ctrl
	for( i = 0; i<static_cast<int>(m_vProcessID.size()); i++ )
	{
		InsertEventsOfAProcssor( m_vProcessID[i] );
	}
}

void CAutoRosterDlg::ShowProcGroupRoster(const CString &_strProcName)
{
	ProcessorID procID;
	procID.SetStrDict( m_pInTerm->inStrDict );
	procID.setID( _strProcName );

	// get all processor id
	GroupIndex group = m_pInTerm->procList->getGroupIndex( procID );
	for( int i= group.start; i<= group.end; i++ )
	{
		ProcessorID id = *(m_pInTerm->procList->getProcessor(i)->getID());
		AddProcessorIDToList( id );
	}
	
	// insert all processor to event tree and gt_ctrl
	for( i = 0; i<static_cast<int>(m_vProcessID.size()); i++ )
	{
		InsertEventsOfAProcssor( m_vProcessID[i] );
	}
}



void CAutoRosterDlg::InsertEventsOfAProcssor(const ProcessorID &_procID)
{
	long lItemIdx;
	CString strItem;
	UnmergedAssignments unMergedAssgn;

	CString strProcName = _procID.GetIDString();

	// insert the root node
	int nDBIdx = m_pInTerm->procAssignDB->findEntry( _procID );
	if( nDBIdx == INT_MAX )
		nDBIdx = -1;
	HTREEITEM hRootItem = m_treeEvent.InsertItem( strProcName );
	
	long lLineIdx = m_gtCtrl.AddLine( strProcName,NULL);
	m_gtCtrl.SetLineModifyable( lLineIdx,true);

	if(nDBIdx == -1)	//the processor don't have assignment data
	{
		// use default data
		strItem.Empty();
		strItem = "(0:00 - 24:00)  DEFAULT";
		HTREEITEM hItem = m_treeEvent.InsertItem( strItem, hRootItem);
		// color for the tree
		m_treeEvent.SetItemColor( hItem,CANNOT_MODIFY_COLOR );
		
		m_gtCtrl.AddItem(&lItemIdx,lLineIdx,"DEFAULT",NULL,0,24*60,false );
		m_treeEvent.Expand(hRootItem,TVE_EXPAND);
	}

	else
	{
		ProcessorRosterSchedule* pProcSchd = m_pInTerm->procAssignDB->getDatabase( nDBIdx );

		unMergedAssgn.SetInputTerminal( m_pInTerm );
		unMergedAssgn.split( pProcSchd );

		int nCount = unMergedAssgn.getCount();
		for( int i = 0; i < nCount; i++ )
		{
			ProcessorRoster* pAssign = unMergedAssgn.getItem( i );
			
			ElapsedTime timeOpen = pAssign->getAbsOpenTime();
			ElapsedTime timeClose = pAssign->getAbsCloseTime();
			char szOpen[128],szClose[128];
			CString szType;
			timeOpen.printTime( szOpen,0 );
			timeClose.printTime( szClose,0 );	
			strItem.Format("(%s - %s)  ", szOpen, szClose );
			const CMultiMobConstraint* pMultiConst = pAssign->getAssignment();	
			const CMobileElemConstraint* pConst = pMultiConst->getConstraint( 0 );
			pConst->screenPrint( szType,0,256 );
			strItem += szType ;
			if( pAssign->isOpen() == 0 )	//close
				strItem += CString( "<CLOSE>" );
			HTREEITEM hItem = m_treeEvent.InsertItem( strItem, hRootItem );
			////////////////////////////////////////////////////////////////////////////////
			//add a item to gt_ctrl
			m_gtCtrl.AddItem(&lItemIdx,lLineIdx,szType,NULL,(long)timeOpen.asMinutes(),(long)(timeClose-timeOpen).asMinutes(),false ) ;
		}
		m_treeEvent.Expand(hRootItem,TVE_EXPAND);
	}
	m_gtCtrl.SetLineModifyable( lLineIdx, false );	
}

void CAutoRosterDlg::OnOK() 
{
	// TODO: Add extra validation here
	CString strMsg;
	strMsg.Format(" Result of %s will be applied to the Roster!\r\nAre you sure?",GetRunName( m_iPreSimResult ) );
	if( MessageBox( strMsg, "Sure", MB_YESNO|MB_ICONINFORMATION ) == IDYES )
	{
		m_pInTerm->procAssignDB->saveDataSet( m_strPrjPath, false );
		m_pInTerm->m_pAutoCreatedProcContainer->addAutoProcToProcList( 0, m_pDoc );
		m_pInTerm->procList->saveDataSet( m_strPrjPath, false );
		CDialog::OnOK();
	}
}

void CAutoRosterDlg::OnCancel() 
{
	//if( m_bMustSelectOneResult )
	//{
	//	return;
	//}
	// TODO: Add extra cleanup here
	ASSERT(m_pInTerm);
	m_pInTerm->procAssignDB->loadDataSet( m_strPrjPath );
	m_pInTerm->m_pAutoCreatedProcContainer->addAutoProcToProcList( 0, m_pDoc );
	m_pInTerm->procList->saveDataSet( m_strPrjPath, false );
	CDialog::OnCancel();
}

//////////////////////////////////////////////////////////////////////////
// do size move
void CAutoRosterDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
	if( m_butCancel.m_hWnd== NULL )
		return;
	const int iEdgeWidth = 8;	
	// first move but
	CRect rc;
	m_butCancel.GetWindowRect( &rc );
	m_butCancel.MoveWindow( cx - iEdgeWidth-rc.Width(), cy-iEdgeWidth-rc.Height(), rc.Width(),rc.Height(),FALSE );
	m_butOK.MoveWindow( cx-2*iEdgeWidth-2*rc.Width(),cy-iEdgeWidth-rc.Height(), rc.Width(),rc.Height(),FALSE );
	
	//resize gt_ctrl
	CRect rcGt;
	m_gtCtrl.GetWindowRect(&rcGt);
	ScreenToClient(&rcGt);
	m_gtCtrl.MoveWindow(11,205,cx-21,cy-250);

	//resize processor tree
	CRect rcProcTree;
	m_treeProc.GetWindowRect(&rcProcTree);
	ScreenToClient(&rcProcTree);
	m_treeProc.MoveWindow(rcProcTree.left,rcProcTree.top,cx*2/5,rcProcTree.Height());
	
	//resize Events tree
	CRect rcEventsTree;
	m_treeProc.GetWindowRect(&rcProcTree);
	ScreenToClient(&rcProcTree);
	m_treeEvent.GetWindowRect(&rcEventsTree);
	ScreenToClient(&rcEventsTree);
	m_treeEvent.MoveWindow( rcProcTree.right+7,rcEventsTree.top,cx-rcProcTree.right-16,rcEventsTree.Height());
	
	m_treeEvent.GetWindowRect(&rcEventsTree);
	ScreenToClient(&rcEventsTree);
	m_staticEvent.GetWindowRect(&rc);
	ScreenToClient(&rc);
	m_staticEvent.MoveWindow(rcEventsTree.left,rc.top,rc.Width(),rc.Height() );
	
	Invalidate();
}

void CAutoRosterDlg::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	// TODO: Add your message handler code here and/or call default
	lpMMI->ptMinTrackSize.x = 300;
	lpMMI->ptMinTrackSize.x = 500;
	CDialog::OnGetMinMaxInfo(lpMMI);
}

void CAutoRosterDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	HWND hwndT=::GetWindow(m_hWnd, GW_CHILD);
	CRect rectCW;
	CRgn rgnCW;
	rgnCW.CreateRectRgn(0,0,0,0);
	while (hwndT != NULL)
	{
		CWnd* pWnd=CWnd::FromHandle(hwndT)  ;
		if(
			pWnd->IsKindOf(RUNTIME_CLASS(CTreeCtrl))||
			pWnd->IsKindOf(RUNTIME_CLASS(CGTChart)))
		{
			pWnd->GetWindowRect(rectCW);
			ScreenToClient(rectCW);
			CRgn rgnTemp;
			rgnTemp.CreateRectRgnIndirect(rectCW);
			rgnCW.CombineRgn(&rgnCW,&rgnTemp,RGN_OR);
		}
		hwndT=::GetWindow(hwndT,GW_HWNDNEXT);
		
	}
	CRect rect;
	GetClientRect(&rect);
	CRgn rgn;
	rgn.CreateRectRgnIndirect(rect);
	CRgn rgnDraw;
	rgnDraw.CreateRectRgn(0,0,0,0);
	rgnDraw.CombineRgn(&rgn,&rgnCW,RGN_DIFF);
	CBrush br(GetSysColor(COLOR_BTNFACE));
	dc.FillRgn(&rgnDraw,&br);
	CRect rectRight=rect;
	rectRight.left=rectRight.right-10;
	dc.FillRect(rectRight,&br);
	rectRight=rect;
	rectRight.top=rect.bottom-44;
	dc.FillRect(rectRight,&br);	
}

BOOL CAutoRosterDlg::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	
	return TRUE;
}

