// MobileCountDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "MobileCountDlg.h"
#include "..\inputs\con_db.h"
#include "..\inputs\fltdata.h"
#include "..\inputs\paxdata.h"
#include "..\inputs\probab.h"
#include "termplandoc.h"
#include "passengertypedialog.h"
#include "..\common\probdistmanager.h"
#include "ProbDistDlg.h"
#include "inputs\MobileElemTypeStrDB.h"
#include "MoblieElemDlg.h"
#include ".\mobilecountdlg.h"
#include "DlgNonPaxRelativePosSpec.h"
#include "DlgHostSpec.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMobileCountDlg dialog


CMobileCountDlg::CMobileCountDlg(CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CMobileCountDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMobileCountDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	
	m_nListPreIndex = -1;
}


void CMobileCountDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMobileCountDlg)
	DDX_Control(pDX, IDC_LIST_ELEMENT, m_listElem);
	DDX_Control(pDX, IDC_STATIC_TOOLBARCONTENTER, m_toolbarcontenter);
	DDX_Control(pDX, IDC_BUTTON_SAVE, m_btnSave);
	DDX_Control(pDX, IDC_BUTTON_EDITMOBILE, m_btnNonpax);
	DDX_Control(pDX, IDC_LIST_DATA, m_listctrlData);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMobileCountDlg, CXTResizeDialog)
	//{{AFX_MSG_MAP(CMobileCountDlg)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_DATA, OnEndlabeleditListData)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_DATA, OnItemchangedListData)
	ON_WM_CREATE()
	ON_COMMAND(ID_TOOLBARBUTTONADD, OnToolbarbuttonadd)
	ON_COMMAND(ID_TOOLBARBUTTONDEL, OnToolbarbuttondel)
	ON_COMMAND(ID_TOOLBARBUTTONEDIT, OnToolbarbuttonedit)
	ON_WM_MOUSEMOVE()
	ON_LBN_SELCHANGE(IDC_LIST_ELEMENT, OnSelchangeListBoxElement)
	ON_BN_CLICKED(IDC_BUTTON_EDITMOBILE, OnButEditmobile)
	ON_BN_CLICKED(IDC_BUTTON_NONPAXPOSSPEC,OnBnClickPosSpec)
	ON_BN_CLICKED(IDC_BUTTON_HOST,OnBnClickHostSpec)
	ON_MESSAGE( UM_ITEM_CHECK_STATUS_CHANGED, OnCheckStateChanged )
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMobileCountDlg message handlers

BOOL CMobileCountDlg::OnInitDialog() 
{
	CXTResizeDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	AfxGetApp()->BeginWaitCursor();

	InitToolbar();

	SetResize(m_ToolBar.GetDlgCtrlID(), SZ_TOP_CENTER, SZ_TOP_CENTER);
	SetResize(IDC_LIST_ELEMENT, SZ_TOP_LEFT , SZ_BOTTOM_CENTER);
	SetResize(IDC_LIST_DATA, SZ_TOP_CENTER , SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_EDITMOBILE, SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_BUTTON_SAVE, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDOK, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDCANCEL, SZ_BOTTOM_RIGHT, SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_NONPAXPOSSPEC,SZ_BOTTOM_LEFT, SZ_BOTTOM_LEFT);
	SetResize(IDC_BUTTON_HOST,SZ_BOTTOM_LEFT,SZ_BOTTOM_LEFT);

	InitListBox();
	SetListCtrl();
	ReloadData( NULL );

	AfxGetApp()->EndWaitCursor();

	m_toolTips.InitTooltips( &m_listctrlData );
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMobileCountDlg::SetListCtrl()
{
	// set list ctrl	
	DWORD dwStyle = m_listctrlData.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_listctrlData.SetExtendedStyle( dwStyle );

	LV_COLUMNEX	lvc;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT ;
	char columnLabel[6][128];
	ConstraintDatabase* pConDB = GetConstraintDatabase();

	if( pConDB )
	{
		sprintf( columnLabel[1], "Probability Distribution ( %s )", pConDB->getUnits() );
		CString csLabel;
    
		strcpy( columnLabel[0], "Passenger Type" );		

		strcpy( columnLabel[2], "Impact Speed(%)" );	
		strcpy( columnLabel[3], "Impact In-step(%)" );	
		strcpy( columnLabel[4], "Impact side-step(%)");	
		strcpy( columnLabel[5], "Impact Emer(%)" );	
		
		int DefaultColumnWidth[] = { 110, 170, 110, 110, 110, 110 };
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
		CStringList strList;
		lvc.csList=&strList;
		lvc.pszText="Per Passenger";
		lvc.cx=90;
		lvc.fmt=LVCFMT_NOEDIT;
		m_listctrlData.InsertColumn(2,&lvc);

		for (int j = 3; j < 7; j++)
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
			lvc.pszText = columnLabel[j-1];
			lvc.cx = DefaultColumnWidth[j-1];
			//if( i == 0 )
				//lvc.fmt = LVCFMT_NOEDIT;
			//else
			lvc.fmt = LVCFMT_DROPDOWN;
			//lvc.fmt = LVCFMT_NOEDIT;
			m_listctrlData.InsertColumn( j, &lvc );

		}
		
		m_listctrlData.SetInputTerminal( GetInputTerminal() );
	}
}


// reload data from database and select the item as parameter
void CMobileCountDlg::ReloadData( Constraint* _pSelCon )
{
	ConstraintDatabase* pInStep = GetInStepDatabase();
	int nCount4 = pInStep->getCount();
	m_listctrlData.SetImpactInstepDB(pInStep);

	ConstraintDatabase* pConDB = GetConstraintDatabase();
	int count = pConDB->getCount();
	m_listctrlData.SetCurConDB( pConDB );

	ConstraintDatabase* pImpactSpeedDB = GetImpactSpeedDatabase();
	int count2= pImpactSpeedDB->getCount();
	
	m_listctrlData.SetImpactSpeedDB(pImpactSpeedDB);

	ConstraintDatabase* pSideDB = GetSideStepDatabase();
	int ncount3 = pSideDB->getCount();
	m_listctrlData.SetSideStepDB(pSideDB);



	ConstraintDatabase* pEmer = GetEmerDatabase();
	m_listctrlData.SetEmerImpactDB(pEmer);

	m_listctrlData.ReloadData( _pSelCon );
	m_btnSave.EnableWindow( TRUE );
	m_btnNonpax.EnableWindow( TRUE );
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL,GetSelectedItem() != -1 );
}


InputTerminal* CMobileCountDlg::GetInputTerminal()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
    return (InputTerminal*)&pDoc->GetTerminal();
}


CString CMobileCountDlg::GetProjPath()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	return pDoc->m_ProjInfo.path;
}


void CMobileCountDlg::OnButtonSave() 
{
	// TODO: Add your control notification handler code here
	GetInputTerminal()->paxDataList->saveDataSet( GetProjPath(), true );
	m_btnSave.EnableWindow( FALSE );
}

void CMobileCountDlg::OnCancel() 
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
		GetInputTerminal()->paxDataList->loadDataSet( GetProjPath() );
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

void CMobileCountDlg::OnOK() 
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
ConstraintDatabase* CMobileCountDlg::GetConstraintDatabase()
{
	ConstraintDatabase* pConDB = NULL;
	
	int iCurSel = m_listElem.GetCurSel();
	if( iCurSel != LB_ERR )
	{
		int iNopaxIndex = m_listElem.GetItemData( iCurSel );
		pConDB = GetInputTerminal()->paxDataList->getNopaxData( iNopaxIndex );
	}

	return pConDB;
}

ConstraintDatabase* CMobileCountDlg::GetImpactSpeedDatabase()
{
	ConstraintDatabase * pImpactSpeedDB = NULL;

	int iCurSel = m_listElem.GetCurSel();
	if (iCurSel != LB_ERR)
	{
		int iImpactSpeedItem = m_listElem.GetItemData(iCurSel);
		pImpactSpeedDB = GetInputTerminal()->paxDataList->getImpactSpeed();
	}

	return pImpactSpeedDB;
}

ConstraintDatabase* CMobileCountDlg::GetSideStepDatabase()
{
	ConstraintDatabase *pSideDB = NULL;
	int iCurSel = m_listElem.GetCurSel();
	if (iCurSel != LB_ERR)
	{
		int iSideItem = m_listElem.GetItemData(iCurSel);
		pSideDB = GetInputTerminal()->paxDataList->getImpactSideStep();
	}

	return pSideDB;
}

ConstraintDatabase* CMobileCountDlg::GetInStepDatabase()
{
	ConstraintDatabase *pSideDB = NULL;
	int iCurSel = m_listElem.GetCurSel();
	if (iCurSel != LB_ERR)
	{
		int iSideItem = m_listElem.GetItemData(iCurSel);
		pSideDB = GetInputTerminal()->paxDataList->getImpactInStep();
	}

	return pSideDB;
}

ConstraintDatabase* CMobileCountDlg::GetEmerDatabase()
{
	ConstraintDatabase *pSideDB = NULL;
	int iCurSel = m_listElem.GetCurSel();
	if (iCurSel != LB_ERR)
	{
		int iSideItem = m_listElem.GetItemData(iCurSel);
		pSideDB = GetInputTerminal()->paxDataList->getImpactEmer();
	}

	return pSideDB;

}

void CMobileCountDlg::OnEndlabeleditListData(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here
 	LV_ITEM* plvItem = &pDispInfo->item;

	*pResult = 0;
	m_btnSave.EnableWindow();
}

void CMobileCountDlg::OnItemchangedListData(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	if( GetSelectedItem() != -1 )
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL );
	
	*pResult = 0;
}

int CMobileCountDlg::GetSelectedItem()
{
	int nCount = m_listctrlData.GetItemCount();
	for( int i=0; i<nCount; i++ )
	{
		if( m_listctrlData.GetItemState( i, LVIS_SELECTED ) == LVIS_SELECTED )
			return i;
	}
	return -1;
}

void CMobileCountDlg::InitToolbar()
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

int CMobileCountDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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


void CMobileCountDlg::OnToolbarbuttonadd() 
{
	// TODO: Add your command handler code here
	bool bDirty = false;
	ConstraintDatabase* pConDB = GetConstraintDatabase();
	assert( pConDB );

	CPassengerTypeDialog dlg( m_pParentWnd, TRUE );
	if( dlg.DoModal() == IDOK )
	{
		CMobileElemConstraint mobileConstr = dlg.GetMobileSelection();
		bDirty = m_listctrlData.AddEntry( PAX_CON, &mobileConstr );
	}

	if( bDirty )
		m_btnSave.EnableWindow();

}

void CMobileCountDlg::OnToolbarbuttondel() 
{
	// TODO: Add your command handler code here
		if( m_listctrlData.DeleteEntry() )
		m_btnSave.EnableWindow();

}

void CMobileCountDlg::OnToolbarbuttonedit() 
{
	// TODO: Add your command handler code here
}

BOOL CMobileCountDlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	m_toolTips.RelayEvent( pMsg );

	return CDialog::PreTranslateMessage(pMsg);
}

void CMobileCountDlg::OnMouseMove(UINT nFlags, CPoint point) 
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

void CMobileCountDlg::ShowTips(int iItemData)
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

void CMobileCountDlg::InitListBox()
{
	m_listElem.ResetContent();
	
	int iNopaxCount = GetInputTerminal()->m_pMobElemTypeStrDB->GetCount();
	for( int i=1; i<iNopaxCount; i++ )
	{	
		CString strLabel = GetInputTerminal()->m_pMobElemTypeStrDB->GetString( i );
		int iIndex = m_listElem.AddString( strLabel );
		assert( iIndex!=LB_ERR );
		m_listElem.SetItemData( iIndex, i ); 
	}

	m_listElem.SetCurSel(0);
}

void CMobileCountDlg::OnSelchangeListBoxElement() 
{
	// TODO: Add your control notification handler code here
	AfxGetApp()->BeginWaitCursor();

	m_listctrlData.DeleteAllItems();
	ReloadData( NULL );

	AfxGetApp()->EndWaitCursor();	
}

void CMobileCountDlg::OnButEditmobile() 
{
	CWaitCursor wc;
	m_btnNonpax.EnableWindow( FALSE );
	//save change first 
	OnButtonSave();
	wc.Restore();

	CMoblieElemDlg dlg(m_pParentWnd);
	if( dlg.DoModal() == IDOK )
	{
		InitListBox();
		m_listctrlData.DeleteAllItems();
		ReloadData( NULL );
		m_btnSave.EnableWindow( TRUE );
	}

	m_btnNonpax.EnableWindow( TRUE );
}

LRESULT CMobileCountDlg::OnCheckStateChanged( WPARAM wParam, LPARAM lParam )
{
	m_btnSave.EnableWindow(TRUE );
	return TRUE;
}

void CMobileCountDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	OnOK();
}

void CMobileCountDlg::OnBnClickPosSpec()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	CDlgNonPaxRelativePosSpec dlg(pDoc);
	dlg.DoModal();
}

void CMobileCountDlg::OnBnClickHostSpec()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	CDlgHostSpec dlg(pDoc->GetProjectID(),GetInputTerminal());
	dlg.DoModal();
}