// ProcToResPoolDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "ProcToResPoolDlg.h"
#include "termplandoc.h"
#include "inputs\in_term.h"
#include "inputs\resourcepool.h"
#include "inputs\ProcToResource.h"
#include "common\ProbDistManager.h"
#include "Inputs\probab.h"
#include "inputs\proctoresource.h"
#include "processerdialog.h"
#include "resourcepooldlg.h"
#include "common\winmsg.h"
#include "dlgprobdist.h"
#include "passengertypedialog.h"
#include <Common/ProbabilityDistribution.h>
#include <Inputs/PROCIDList.h>
#include ".\proctorespooldlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProcToResPoolDlg dialog


CProcToResPoolDlg::CProcToResPoolDlg(CTermPlanDoc* _pDoc,CWnd* pParent /*=NULL*/)
	: CDialog(CProcToResPoolDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProcToResPoolDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pDoc = _pDoc;
}


void CProcToResPoolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProcToResPoolDlg)
	DDX_Control(pDX, IDC_STATIC_TOOLBAR, m_staticToolbar);
	DDX_Control(pDX, IDC_LIST_DATA, m_listData);
	DDX_Control(pDX, IDC_BUTTON_SAVE, m_btnSave);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDOK, m_btnOk);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
}


BEGIN_MESSAGE_MAP(CProcToResPoolDlg, CDialog)
	//{{AFX_MSG_MAP(CProcToResPoolDlg)
	ON_WM_CREATE()
	ON_COMMAND(ID_TOOLBARBUTTONADD, OnToolbarbuttonadd)
	ON_COMMAND(ID_TOOLBARBUTTONDEL, OnToolbarbuttondel)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_DATA, OnItemchangedListData)
	//ON_NOTIFY(NM_DBLCLK, IDC_LIST_DATA, OnDblclkListData)
	ON_MESSAGE(WM_NOEDIT_DBCLICK,OnDblclkListData)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	//}}AFX_MSG_MAP
	ON_MESSAGE( WM_INPLACE_COMBO_KILLFOUCUS, OnInplaceCombox)
	ON_WM_SIZE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProcToResPoolDlg message handlers

InputTerminal* CProcToResPoolDlg::GetInputTerminal()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
    return (InputTerminal*)&pDoc->GetTerminal();
}

CString CProcToResPoolDlg::GetProjPath()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	return pDoc->m_ProjInfo.path;
}

int CProcToResPoolDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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

BOOL CProcToResPoolDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	InitToolBar();
	InitListCtrl();
	ReloadData();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CProcToResPoolDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if(m_listData)
	{
		CRect btnrc;
		CRect tmpRect;
		CRect clientRect;
		GetClientRect(clientRect);
		m_btnSave.GetWindowRect(tmpRect);
		ScreenToClient(tmpRect);
		m_listData.GetWindowRect(tmpRect);
		ScreenToClient(tmpRect);


		m_btnCancel.GetWindowRect( &btnrc );
		m_ToolBar.MoveWindow(11, 16, cx-24, 26);
		m_listData.MoveWindow(11, 42, cx-24, cy-80-btnrc.Height());
		m_btnSave.MoveWindow(cx-11-10-11-3*btnrc.Width(),cy-11-btnrc.Height(),btnrc.Width(),btnrc.Height());
		m_btnOk.MoveWindow( cx-11-10-2*btnrc.Width(),cy-11-btnrc.Height(),btnrc.Width(),btnrc.Height() );
		m_btnCancel.MoveWindow( cx-11-btnrc.Width(),cy-11-btnrc.Height(),btnrc.Width(),btnrc.Height() );

		CRect wndRect;
		GetWindowRect(wndRect);
		ScreenToClient(wndRect);
		InvalidateRect(&wndRect);
	}
}

void CProcToResPoolDlg::InitToolBar()
{
	CRect mRect;
	m_staticToolbar.GetWindowRect( &mRect );
	ScreenToClient( &mRect );
	
	m_ToolBar.MoveWindow( &mRect );
	m_ToolBar.ShowWindow( SW_SHOW );
	m_staticToolbar.ShowWindow( SW_HIDE );
	
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONADD, TRUE );
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL, FALSE );
	m_ToolBar.GetToolBarCtrl().HideButton( ID_TOOLBARBUTTONEDIT );
}

void CProcToResPoolDlg::InitListCtrl()
{
	// set list_ctrl	
	DWORD dwStyle = m_listData.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_listData.SetExtendedStyle( dwStyle );
	
	LV_COLUMNEX	lvc;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT ;
	char columnLabel[5][128];
	strcpy( columnLabel[0], "Processor" );		
	strcpy( columnLabel[1], "Pax Type" );
	strcpy( columnLabel[2], "Resource Pool" );
	strcpy( columnLabel[3], "Service Time(Sec)" );
	strcpy( columnLabel[4], "Pipe" );
	
	int DefaultColumnWidth[] = { 130,150, 150, 150, 150 };
	int nFormat[] = {	LVCFMT_CENTER | LVCFMT_NOEDIT, LVCFMT_CENTER | LVCFMT_NOEDIT, LVCFMT_CENTER | LVCFMT_DROPDOWN , 
		LVCFMT_CENTER | LVCFMT_DROPDOWN , LVCFMT_CENTER | LVCFMT_DROPDOWN };
	
	// init distribution string list
	CStringList strDistList;
	CStringList strPoolList;
	InitListString( strDistList, strPoolList );

	for( int i=0; i<5; i++ )
	{
		if( i==2 )
			lvc.csList = &strPoolList;
		else
			lvc.csList = &strDistList;
		lvc.pszText = columnLabel[i];
		lvc.cx = DefaultColumnWidth[i];
		lvc.fmt = nFormat[i];
		m_listData.InsertColumn( i, &lvc );
	}	
}


void CProcToResPoolDlg::InitListString( CStringList& _strDistList, CStringList& _strPoolList )
{
	// distribution
	CString str;
	str.LoadString( IDS_STRING_NEWDIST );
	_strDistList.InsertAfter( _strDistList.GetTailPosition(), str );
	int nCount = _g_GetActiveProbMan( GetInputTerminal() )->getCount();
	for( int m=0; m<nCount; m++ )
	{
		CProbDistEntry* pPBEntry = _g_GetActiveProbMan( GetInputTerminal() )->getItem( m );			
		_strDistList.InsertAfter( _strDistList.GetTailPosition(), pPBEntry->m_csName );
	}
	
	// resource pool
	str.LoadString( IDS_NEW_RESOURCE_POOL );
	_strPoolList.InsertAfter( _strPoolList.GetTailPosition(), str );
	const RESOURCEPOOLSET& _poolList = GetInputTerminal()->m_pResourcePoolDB->getResourcePoolList();	
	RESOURCEPOOLSET::const_iterator iter;
	for( iter = _poolList.begin(); iter!=_poolList.end(); ++iter )
	{
		str = iter->getPoolName();
		_strPoolList.InsertAfter( _strPoolList.GetTailPosition(), str );
	}	
}


void CProcToResPoolDlg::ReloadData( void )
{
	char szBuffer[256];
	CString conBuf;
	CString strItem;
	m_listData.DeleteAllItems();

	PROC2RESSET& proc2ResList = GetInputTerminal()->m_pProcToResourceDB->getProc2ResList();
	PROC2RESSET::iterator iter;
	for( iter = proc2ResList.begin(); iter!=proc2ResList.end(); ++iter )
	{
		// processor id
		strItem = iter->getProcessorID().GetIDString();
		int _idx = m_listData.InsertItem( m_listData.GetItemCount(), strItem );

		// pax type
		iter->getMobileConstraint().screenPrint(conBuf,0, 256 );
		m_listData.SetItemText( _idx, 1, conBuf.GetBuffer(0) );

		// pool
		strItem = iter->getResourcePoolName();
		m_listData.SetItemText( _idx, 2, strItem );

		// service time
		iter->getProServiceTime()->screenPrint( szBuffer );
		m_listData.SetItemText( _idx, 3, szBuffer );
	}
}

void CProcToResPoolDlg::OnToolbarbuttonadd() 
{
	char szBuffer[1024];
	CString conBuffer;
	CString strItem;

	ProcessorID procID;
	procID.SetStrDict( GetInputTerminal()->inStrDict );
	CMobileElemConstraint mobCon(GetInputTerminal());
	//mobCon.SetInputTerminal( GetInputTerminal() );
	mobCon.initDefault();

	CProcesserDialog dlg( GetInputTerminal() );
	if( dlg.DoModal() == IDOK )
	{
		ProcessorIDList idList;
		if( dlg.GetProcessorIDList( idList ) )	
		{
			int nIdCount = idList.getCount();
			for(int i = 0; i < nIdCount; i++ )
			{
				procID = *idList[i];				
				// add the item to data set
				CProcToResource prco2Res( procID, mobCon );
				if( !GetInputTerminal()->m_pProcToResourceDB->addNewProcToRes( prco2Res ) )
				{
					CString strError;
					mobCon.screenPrint( conBuffer );
					strError.Format(" processor: %s\r\n pax type: %s\r\n already exist", procID.GetIDString(), conBuffer.GetBuffer(0) );
					AfxMessageBox( strError, MB_OK|MB_ICONINFORMATION );
					//return;
					continue;
				}
				// add the item to list end
				// processor id
				strItem = prco2Res.getProcessorID().GetIDString();
				int _idx = m_listData.InsertItem( m_listData.GetItemCount(), strItem );
				
				// pax type
				prco2Res.getMobileConstraint().screenPrint(conBuffer,0, 256 );
				m_listData.SetItemText( _idx, 1, conBuffer );
				
				// pool
				strItem = prco2Res.getResourcePoolName();
				m_listData.SetItemText( _idx, 2, strItem );
				
				// service time
				prco2Res.getProServiceTime()->screenPrint( szBuffer );
				m_listData.SetItemText( _idx, 3, szBuffer );
				//////////////////////////////////////////////////////////////////////////
				// enable save button
				m_btnSave.EnableWindow( TRUE );				
			}
			
		}
	}
}

void CProcToResPoolDlg::OnToolbarbuttondel() 
{
	int _iCurSel = m_listData.GetCurSel();
	if( _iCurSel<0 || _iCurSel >= m_listData.GetItemCount() )
		return;
	
	// update data set
	CString strProcID	= m_listData.GetItemText( _iCurSel, 0 );
	CString strMob		= m_listData.GetItemText( _iCurSel, 1 );
	bool _bResult = GetInputTerminal()->m_pProcToResourceDB->removeItemByName( strProcID, strMob);
	assert( _bResult );
	// update list ctrl
	m_listData.DeleteItem( _iCurSel );
	//////////////////////////////////////////////////////////////////////////
	// enable save button
	m_btnSave.EnableWindow( TRUE );
}

void CProcToResPoolDlg::OnItemchangedListData(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL, m_listData.GetSelectedCount() );
	*pResult = 0;
}


LRESULT CProcToResPoolDlg::OnInplaceCombox( WPARAM wParam, LPARAM lParam)
{
	CString str;
	char szBuffer[1024];
	
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)lParam;
	LV_ITEM* plvItem = &pDispInfo->item;
	
	int _iCurSel = m_listData.GetCurSel();
	if( _iCurSel<0 || _iCurSel>= m_listData.GetItemCount() )
		return TRUE;
	
	CString strProcID = m_listData.GetItemText( _iCurSel, 0 );
	CString strMobType = m_listData.GetItemText( _iCurSel, 1 );
	CProcToResource* _proc2Res = GetInputTerminal()->m_pProcToResourceDB->getItemByName( strProcID, strMobType );
	assert( _proc2Res );

	if( plvItem->iSubItem == 2)		// resource pool list
	{
		str.LoadString( IDS_NEW_RESOURCE_POOL );
		if( strcmp( plvItem->pszText, str ) == 0 )	// define new distribution
		{
			CResourcePoolDlg dlg( m_pDoc, m_pParentWnd );
			ShowWindow( SW_HIDE );
			if(dlg.DoModal()==IDOK) 
			{
				CString strPoolName = dlg.getSelectPoolName();
				_proc2Res->setResourcePoolName( strPoolName );
				m_listData.SetItemText( plvItem->iItem, plvItem->iSubItem, strPoolName );
			}
			else 
			{
				m_listData.SetItemText( _iCurSel, 2, _proc2Res->getResourcePoolName());
			}
			ShowWindow( SW_SHOW );
		}
		else
		{
			_proc2Res->setResourcePoolName( CString( plvItem->pszText) );

		}
	}

	else if( plvItem->iSubItem == 3)		// dist list
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
				m_listData.SetItemText( plvItem->iItem, plvItem->iSubItem, pde->m_csName );
				pProbDist = pde->m_pProbDist;
			}
			else 
			{
				_proc2Res->getProServiceTime()->screenPrint( szBuffer );
				m_listData.SetItemText( _iCurSel, 3, szBuffer );
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
			assert( i < nCount );
			pProbDist = pPDEntry->m_pProbDist;
		}
		assert( pProbDist );
		ProbabilityDistribution* pDist = ProbabilityDistribution::CopyProbDistribution(pProbDist);
		
		_proc2Res->setServiceTime( pDist );	
	}
	//////////////////////////////////////////////////////////////////////////
	// enable save button
	m_btnSave.EnableWindow( TRUE );
	return TRUE;
}

LRESULT CProcToResPoolDlg::OnDblclkListData(WPARAM wParam, LPARAM lParam) 
{
	CPoint pt = ::GetMessagePos();
	m_listData.ScreenToClient( &pt);

	LVHITTESTINFO testinfo;
	testinfo.pt = pt;
	LRESULT result = m_listData.SendMessage( LVM_SUBITEMHITTEST, 0,(LPARAM)(LVHITTESTINFO FAR *)&testinfo );
	
	if( result != -1 )
	{
		if( testinfo.iSubItem == 1 )	// double click pax type
		{
			CPassengerTypeDialog dlg( m_pParentWnd );
			if( dlg.DoModal() == IDOK )
			{
				CMobileElemConstraint mobCon = dlg.GetMobileSelection();
				mobCon.SetInputTerminal( GetInputTerminal() );

				// update data set
				//char szBuffer[256];
				CString szBuffer;
				mobCon.screenPrint( szBuffer, 0, 256 );
				CString strProcID = m_listData.GetItemText( testinfo.iItem, 0 );
				CString strPaxType = m_listData.GetItemText( testinfo.iItem, 1 );
				if( !GetInputTerminal()->m_pProcToResourceDB->modifyItemPaxType( strProcID, strPaxType, mobCon  ) )
				{
					CString strError;
					strError.Format("The processorID: %s\r\n PaxType: %s\r\n already exist", strProcID, szBuffer.GetBuffer(0) );
					AfxMessageBox( strError, MB_OK|MB_ICONINFORMATION );

				}
				else
				{
					// update list ctrl
					m_listData.SetItemText( testinfo.iItem,1, szBuffer.GetBuffer(0) );
				}
			}
			//////////////////////////////////////////////////////////////////////////
			// enable save button
			m_btnSave.EnableWindow( TRUE );
		}	
	}
	
	return result;
//	*pResult = 0;
}

void CProcToResPoolDlg::OnOK() 
{
	if( m_btnSave.IsWindowEnabled() )
	{
		OnButtonSave();
	}
	
	CDialog::OnOK();
}

void CProcToResPoolDlg::OnButtonSave() 
{
	GetInputTerminal()->m_pProcToResourceDB->saveDataSet( GetProjPath(), false );
	m_btnSave.EnableWindow( FALSE );
}

void CProcToResPoolDlg::OnCancel() 
{	
	GetInputTerminal()->m_pProcToResourceDB->loadDataSet( GetProjPath() );
	
	CDialog::OnCancel();
}


