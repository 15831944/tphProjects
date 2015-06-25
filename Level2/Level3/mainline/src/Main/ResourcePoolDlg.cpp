// ResourcePoolDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "ResourcePoolDlg.h"
#include "termplandoc.h"
#include "inputs\in_term.h"
#include "common\ProbDistManager.h"
#include "inputs\probab.h"
#include "inputs\resourcepool.h"
#include "common\WINMSG.h"
#include "DlgProbDist.h"
#include "termplandoc.h"
#include "3dview.h"
#include "mainfrm.h"
#include <Common/ProbabilityDistribution.h>
#include <CommonData/Fallback.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const LPTSTR strPoolType[] = {_T("Concurrent"),_T("Post Service")};
/////////////////////////////////////////////////////////////////////////////
// CResourcePoolDlg dialog


CResourcePoolDlg::CResourcePoolDlg( CTermPlanDoc* _pDoc, CWnd* pParent /*=NULL*/)
	: CDialog(CResourcePoolDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CResourcePoolDlg)
	//}}AFX_DATA_INIT
	m_pDoc = _pDoc;
}


void CResourcePoolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CResourcePoolDlg)
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDC_STATIC_TOOLBAR, m_staticToolbar);
	DDX_Control(pDX, IDC_LIST_POOL, m_listPool);
	DDX_Control(pDX, IDC_BUTTON_SAVE, m_btnSave);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CResourcePoolDlg, CDialog)
	//{{AFX_MSG_MAP(CResourcePoolDlg)
	ON_WM_CREATE()
	ON_COMMAND(ID_RESOURCEPOOL_ADD, OnToolbarbuttonAdd)
	ON_COMMAND(ID_RESOURCEPOOL_DEL, OnToolbarbuttonDel)
	ON_UPDATE_COMMAND_UI(ID_TOOLBARBUTTONADD, OnUpdateToolbarbuttonAdd)
	ON_UPDATE_COMMAND_UI(ID_TOOLBARBUTTONDEL, OnUpdateToolbarbuttonDel)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_POOL, OnItemchangedListPool)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_POOL, OnDblclkListPool)
	ON_COMMAND(ID_RESOURCEPOOL_PICK, OnToolbarbuttonPickPoints)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	//}}AFX_MSG_MAP
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXT, 0, 0xFFFF, OnToolTipText)
	ON_MESSAGE( WM_INPLACE_EDIT, OnInplaceEdit)
	ON_MESSAGE( WM_INPLACE_COMBO_KILLFOUCUS, OnInplaceCombox)
	ON_MESSAGE(TP_DATA_BACK, OnTempFallbackFinished)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResourcePoolDlg message handlers

InputTerminal* CResourcePoolDlg::GetInputTerminal()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
    return (InputTerminal*)&pDoc->GetTerminal();
}

CString CResourcePoolDlg::GetProjPath()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	return pDoc->m_ProjInfo.path;
}

BOOL CResourcePoolDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	InitToolBar();
	InitListCtrl();
	
	ReLoadDataset();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CResourcePoolDlg::InitToolBar()
{
	CRect mRect;
	m_staticToolbar.GetWindowRect( &mRect );
	ScreenToClient( &mRect );
	
	m_ToolBar.MoveWindow( &mRect );
	m_ToolBar.ShowWindow( SW_SHOW );
	m_staticToolbar.ShowWindow( SW_HIDE );
	
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_RESOURCEPOOL_ADD, TRUE );
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_RESOURCEPOOL_DEL, FALSE );
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_RESOURCEPOOL_PICK,FALSE );
}

void CResourcePoolDlg::InitListCtrl()
{
	// set list_ctrl	
	DWORD dwStyle = m_listPool.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_listPool.SetExtendedStyle( dwStyle );
	
	LV_COLUMNEX	lvc;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT ;
	char columnLabel[5][128];
	strcpy( columnLabel[0], "Resource Pool Name" );		
	strcpy( columnLabel[1], "Resource Speed(m/s)" );
	strcpy( columnLabel[2], "Resource Count" );
	strcpy( columnLabel[3], "Resource Pool Base" );
	strcpy(	columnLabel[4],	"Service Sequence");
	
	int DefaultColumnWidth[] = { 100,180, 180,170 ,120};
	int nFormat[] = {	LVCFMT_CENTER | LVCFMT_EDIT, LVCFMT_CENTER | LVCFMT_DROPDOWN, LVCFMT_CENTER | LVCFMT_DROPDOWN , LVCFMT_CENTER | LVCFMT_NOEDIT,\
	LVCFMT_CENTER | LVCFMT_DROPDOWN};
	
	// init distribution string list
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
	
	for( int i=0; i<4; i++ )
	{
		lvc.csList = &strList;
		lvc.pszText = columnLabel[i];
		lvc.cx = DefaultColumnWidth[i];
		lvc.fmt = nFormat[i];
		m_listPool.InsertColumn( i, &lvc );
	}
	
	CStringList strToolTypeList;
	strToolTypeList.AddTail(_T("Concurrent"));
	strToolTypeList.AddTail(_T("Post Service"));
	lvc.csList = &strToolTypeList;
	lvc.pszText = columnLabel[4];
	lvc.cx = DefaultColumnWidth[4];
	lvc.fmt = nFormat[4];
	m_listPool.InsertColumn(4,&lvc);
}

int CResourcePoolDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	if (!m_ToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_ToolBar.LoadToolBar(IDR_RESOURCEPOOL_BAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}		
	
	return 0;
}

//////////////////////////////////////////////////////////////////////////
void CResourcePoolDlg::ReLoadDataset( void )
{
	m_listPool.DeleteAllItems();
	
	CString strItem;
	char szBuffer[1024];
	RESOURCEPOOLSET::const_iterator _iter;
	const RESOURCEPOOLSET& _poolList = GetInputTerminal()->m_pResourcePoolDB->getResourcePoolList();
	
	for( _iter = _poolList.begin(); _iter!= _poolList.end(); ++_iter )
	{
		// name
		strItem = _iter->getPoolName();
		int _idx = m_listPool.InsertItem( m_listPool.GetItemCount(), strItem );
		// speed
		_iter->getProSpeed()->screenPrint( szBuffer );
		m_listPool.SetItemText( _idx, 1, szBuffer );
		// count
		_iter->getProCount()->screenPrint( szBuffer );
		m_listPool.SetItemText( _idx, 2, szBuffer );
		// path
		strItem = "";
		for( int i=0; i<_iter->getPoolBasePath().getCount(); i++ )
		{
			_iter->getPoolBasePath().getPoint(i).printPoint( szBuffer,2 );

			strItem = strItem +"(" + szBuffer + ") ";
		}
		m_listPool.SetItemText( _idx, 3, strItem );
		
		if (_iter->getPoolType() == ConcurrentType)
		{
			m_listPool.SetItemText( _idx, 4, strPoolType[0]);
		}
		else
		{
			m_listPool.SetItemText( _idx, 4, strPoolType[1]);
		}
	}
}

void CResourcePoolDlg::OnToolbarbuttonAdd() 
{
	CResourcePool _newpool;
	
	// add the new pool to pool dataset
	if( !GetInputTerminal()->m_pResourcePoolDB->addNewPool( _newpool) )
	{
		CString strError;
		strError.Format("The pool name \"%s\" already exists!",_newpool.getPoolName() );
		AfxMessageBox( strError, MB_OK|MB_ICONINFORMATION );
		return;
	}
	
	// add the new pool to the list end
	char szBuffer[1024];
	//// name
	int _idx = m_listPool.InsertItem( m_listPool.GetItemCount(), _newpool.getPoolName() );
	//// speed
	_newpool.getProSpeed()->screenPrint( szBuffer );
	m_listPool.SetItemText( _idx, 1, szBuffer );
	//// count
	_newpool.getProCount()->screenPrint( szBuffer );
	m_listPool.SetItemText( _idx, 2, szBuffer );
	//// path
	CString strItem;
	for( int i=0; i<_newpool.getPoolBasePath().getCount(); i++ )
	{
		_newpool.getPoolBasePath().getPoint(i).printPoint( szBuffer,2 );

		strItem = strItem +"(" + szBuffer + ") ";
	}
	m_listPool.SetItemText( _idx, 3, strItem );
	//pool type
	m_listPool.SetItemText( _idx, 4 ,strPoolType[0]);

	// to edit the name
	m_listPool.SetFocus();
	m_listPool.DblClickItem(_idx, 0);


	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	pDoc->m_resDispProps.AddItemToAllSets(_idx);

	//////////////////////////////////////////////////////////////////////////
	// enable save button
	m_btnSave.EnableWindow( TRUE );
}

void CResourcePoolDlg::OnUpdateToolbarbuttonAdd(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	
}


void CResourcePoolDlg::OnToolbarbuttonDel() 
{
	int _iCurSel = m_listPool.GetCurSel();
	if( _iCurSel<0 || _iCurSel>= m_listPool.GetItemCount() )
		return;

	CString strName = m_listPool.GetItemText( _iCurSel,0 );

	// remove the pool from data set
	bool _bResult = GetInputTerminal()->m_pResourcePoolDB->removeResourcePoolByName( strName );
	assert( _bResult == true );
	
	// remove the row from the list ctrl
	m_listPool.DeleteItem( _iCurSel );

	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	pDoc->m_resDispProps.DeleteItemFromAllSets(_iCurSel);


	//////////////////////////////////////////////////////////////////////////
	// enable save button
	m_btnSave.EnableWindow( TRUE );
	m_btnOK.EnableWindow( FALSE );
}


void CResourcePoolDlg::OnUpdateToolbarbuttonDel(CCmdUI* pCmdUI) 
{
	
}

void CResourcePoolDlg::OnToolbarbuttonPickPoints() 
{
	int _iCurSel = m_listPool.GetCurSel();
	if( _iCurSel<0 || _iCurSel>= m_listPool.GetItemCount() )
		return;


	assert( m_pDoc );
	C3DView* p3DView = m_pDoc->Get3DView();
	if( p3DView == NULL )
	{
		m_pDoc->GetMainFrame()->CreateOrActive3DView();
		p3DView = m_pDoc->Get3DView();
	}
	if (!p3DView)
	{
		ASSERT(FALSE);
		return;
	}
	
	HideDialog( m_pDoc->GetMainFrame() );
	
	
	FallbackData data;
	data.hSourceWnd = GetSafeHwnd();
	enum FallbackReason enumReason;
	enumReason = PICK_MANYPOINTS;
	
	
	if( !m_pDoc->GetMainFrame()->SendMessage( TP_TEMP_FALLBACK, (WPARAM)&data, (LPARAM)enumReason ) )
	{
		MessageBox( "Error" );
		ShowDialog( m_pDoc->GetMainFrame() );
		return;
	}

	//////////////////////////////////////////////////////////////////////////
	// enable save button
	m_btnSave.EnableWindow( TRUE );
}


LRESULT CResourcePoolDlg::OnTempFallbackFinished(WPARAM wParam, LPARAM lParam)
{
	CWnd* wnd = GetParent();
	while (!wnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)) )
	{
		wnd->ShowWindow(SW_SHOW);
		wnd->EnableWindow(false);
		wnd = wnd->GetParent();
	}
	wnd->EnableWindow(false); 
	ShowWindow(SW_SHOW);	
	EnableWindow();
	std::vector<ARCVector3>* pData = (std::vector<ARCVector3>*) wParam;
	
	if( pData->size() <= 2)
	{
		MessageBox("You must pick more than one point!");
		return FALSE;
	}

	// pick from map
	int _iCurSel = m_listPool.GetCurSel();
	if( _iCurSel<0 || _iCurSel>= m_listPool.GetItemCount() )
	{
		MessageBox("Please select one resource item in the list and  try again.");
		return FALSE;
	}
	
	CString strName = m_listPool.GetItemText( _iCurSel,0 );
	CResourcePool* _pool = GetInputTerminal()->m_pResourcePoolDB->getResourcePoolByName( strName );
	assert( _pool );
	
	std::vector<Point> _ptList;
	ARCVector3 v3D;
	for( int i = 0; i<static_cast<int>(pData->size()); i++ )
	{
		v3D = pData->at(i);
		Point pt( v3D[VX], v3D[VY], m_pDoc->m_nActiveFloor * SCALE_FACTOR );
		_ptList.push_back( pt);
	}
	// make the path to circle
	v3D = pData->at(0);
	_ptList.push_back( Point( v3D[VX], v3D[VY], m_pDoc->m_nActiveFloor * SCALE_FACTOR ) );

	Path _path;
	_path.init( _ptList.size(), (const Point*) &(_ptList[0]) );
	// update resource pool
	_pool->setPoolBasePath( _path );
	// update list control
	char szBuffer[128];
	CString strItem;
	for( int k=0; k<_pool->getPoolBasePath().getCount(); k++ )
	{
		_pool->getPoolBasePath().getPoint(k).printPoint( szBuffer,2 );
		strItem = strItem + "(" + szBuffer +") ";
	}
	m_listPool.SetItemText( _iCurSel, 3, strItem );

	m_pDoc->UpdateAllViews(NULL);
	return TRUE;	
}

void CResourcePoolDlg::OnItemchangedListPool(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	int _iCurSel = m_listPool.GetCurSel();
	BOOL bEnable = ( -1 != _iCurSel );

	m_ToolBar.GetToolBarCtrl().EnableButton( ID_RESOURCEPOOL_DEL, bEnable );
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_RESOURCEPOOL_PICK, bEnable );
	
	if( _iCurSel<0 || _iCurSel>= m_listPool.GetItemCount() )
		return;
	
	m_btnOK.EnableWindow( TRUE );

	*pResult = 0;
	return;
}


void CResourcePoolDlg::OnDblclkListPool(NMHDR* pNMHDR, LRESULT* pResult) 
{
	int _iCurSel = m_listPool.GetCurSel();
	if( _iCurSel<0 || _iCurSel>= m_listPool.GetItemCount() )
	{
		m_strDoubleClkItemName = "";
		return;
	}
	
	m_strDoubleClkItemName = m_listPool.GetItemText( _iCurSel,0);

	*pResult = 0;
}

LRESULT CResourcePoolDlg::OnInplaceEdit(WPARAM wParam, LPARAM lParam)
{
	int _iCurSel = m_listPool.GetCurSel();
	if( _iCurSel<0 || _iCurSel>= m_listPool.GetItemCount() )
		return TRUE;

	CString strOldName = m_strDoubleClkItemName;
	CString strNewName = *((CString*) lParam);

	if( strOldName == strNewName )
		return TRUE;

	// update pool's name in  pool data set
	if( GetInputTerminal()->m_pResourcePoolDB->checkNameIfExist( strNewName ) )
	{
		CString strError;
		strError.Format("The pool name \"%s\" already exists!", strNewName );
		AfxMessageBox( strError, MB_OK|MB_ICONINFORMATION );
		
		m_listPool.SetItemText( _iCurSel, 0, strOldName );
		return TRUE;
	}	

	CResourcePool* _pool = GetInputTerminal()->m_pResourcePoolDB->getResourcePoolByName( strOldName );
	assert( _pool );
	_pool->setPoolName( strNewName );
	
	//////////////////////////////////////////////////////////////////////////
	// enable save button
	m_btnSave.EnableWindow( TRUE );
	return TRUE;
}


LRESULT CResourcePoolDlg::OnInplaceCombox( WPARAM wParam, LPARAM lParam)
{
	CString str;
	char szBuffer[1024];

	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)lParam;
	LV_ITEM* plvItem = &pDispInfo->item;
	
	int _iCurSel = m_listPool.GetCurSel();
	if( _iCurSel<0 || _iCurSel>= m_listPool.GetItemCount() )
		return TRUE;

	CString strPoolName = m_listPool.GetItemText( _iCurSel, 0);
	CResourcePool* _pool = GetInputTerminal()->m_pResourcePoolDB->getResourcePoolByName( strPoolName );
	assert( _pool );

	if( plvItem->iSubItem == 1  || plvItem->iSubItem == 2)		
	{
		ProbabilityDistribution* pProbDist = NULL;
		str.LoadString( IDS_STRING_NEWDIST );
		if( strcmp( plvItem->pszText, str ) == 0 )	// define new distribution
		{
			CDlgProbDist dlg(GetInputTerminal()->m_pAirportDB, true );
			if(dlg.DoModal()==IDOK) 
			{
				int idxPD = dlg.GetSelectedPDIdx();
				ASSERT(idxPD!=-1);
				CProbDistEntry* pde = _g_GetActiveProbMan( GetInputTerminal() )->getItem(idxPD);
				m_listPool.SetItemText( plvItem->iItem, plvItem->iSubItem, pde->m_csName );
				pProbDist = pde->m_pProbDist;
			}
			else 
			{
				if( plvItem->iSubItem == 1 )	// speed
				{
					_pool->getProSpeed()->screenPrint( szBuffer );	
					m_listPool.SetItemText( _iCurSel, 1, szBuffer );
				}
				else							// count
				{
					_pool->getProCount()->screenPrint( szBuffer );	
					m_listPool.SetItemText( _iCurSel, 2, szBuffer );
				}
				return TRUE;
			}
		}
		else
		{
			CProbDistEntry* pPDEntry = NULL;
			int nCount = _g_GetActiveProbMan( GetInputTerminal() )->getCount();
			for( int i=0; i<nCount; i++ )
			{
				pPDEntry = _g_GetActiveProbMan( GetInputTerminal() )->getItem( i );
				if( strcmp( pPDEntry->m_csName, plvItem->pszText ) == 0 )
					break;
			}
			//assert( i < nCount );
			pProbDist = pPDEntry->m_pProbDist;
		}
		assert( pProbDist );
		ProbabilityDistribution* pDist = ProbabilityDistribution::CopyProbDistribution(pProbDist);
		
		if( plvItem->iSubItem == 1)
		{
			_pool->setProSpeed( pDist );			
		}
		else
		{
			_pool->setProCount( pDist );
		}	
	}
	else
	{
		if (strcmp(strPoolType[0],plvItem->pszText) == 0)
		{
			_pool->setPoolType(ConcurrentType);
		}
		else
		{
			_pool->setPoolType(PostServiceType);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// enable save button
	m_btnSave.EnableWindow( TRUE );
	return TRUE;
}


void CResourcePoolDlg::ShowDialog(CWnd* parentWnd)
{
	while (!parentWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd)) )
	{
		parentWnd->ShowWindow(SW_SHOW);
		parentWnd->EnableWindow(FALSE);
		parentWnd = parentWnd->GetParent();
	}
	parentWnd->EnableWindow(FALSE);
	ShowWindow(SW_SHOW);	
	EnableWindow();
}

void CResourcePoolDlg::HideDialog(CWnd* parentWnd)
{
	ShowWindow(SW_HIDE);
	while(!parentWnd->IsKindOf(RUNTIME_CLASS(CMDIFrameWnd))) {
		parentWnd->ShowWindow(SW_HIDE);
		parentWnd = parentWnd->GetParent();
	}
	parentWnd->EnableWindow(TRUE);
	parentWnd->SetActiveWindow();
	
}

bool CResourcePoolDlg::CanSave(void)
{
	const RESOURCEPOOLSET& poolList = GetInputTerminal()->m_pResourcePoolDB->getResourcePoolList();
	RESOURCEPOOLSET::const_iterator iter;

	for( iter = poolList.begin(); iter!= poolList.end(); ++iter )
	{
		if( iter->getPoolBasePath().getCount()<=1 )
			return false;
	}

	return true;
}

bool CResourcePoolDlg::DoSave( void )
{
	if( !CanSave() )	
	{
		AfxMessageBox(" the pool base path can not be empty!", MB_OK | MB_ICONINFORMATION );
		return false;
	}
	
	GetInputTerminal()->m_pResourcePoolDB->saveDataSet( GetProjPath(), false );

	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	pDoc->m_resDispProps.saveInputs( GetProjPath(), false );
	
	m_btnSave.EnableWindow( FALSE );
	return true;
}

void CResourcePoolDlg::OnButtonSave() 
{
	DoSave();	
}

void CResourcePoolDlg::OnOK() 
{
	int _iCurSel = m_listPool.GetCurSel();
	if( _iCurSel<0 || _iCurSel>= m_listPool.GetItemCount() )
	{
		m_btnOK.EnableWindow( FALSE );
		return;
	}
	
	m_strSelectPoolName = m_listPool.GetItemText( _iCurSel, 0 );
	m_nSelectPoolIdx = _iCurSel;

	if( m_btnSave.IsWindowEnabled() )
	{
		if( !DoSave() )
			return;
	}
	
	
	CDialog::OnOK();
}

void CResourcePoolDlg::OnCancel() 
{
	GetInputTerminal()->m_pResourcePoolDB->loadDataSet( GetProjPath() );

	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	pDoc->m_resDispProps.loadInputs( GetProjPath(), GetInputTerminal() );

	CDialog::OnCancel();

}


CString CResourcePoolDlg::getSelectPoolName()
{
	return m_strSelectPoolName;
}

BOOL CResourcePoolDlg::OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult)
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
