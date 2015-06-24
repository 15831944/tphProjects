// MoblieElemDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "MoblieElemDlg.h"
#include "MFCExControl\SmartEdit.h"
#include "common\WinMsg.h"
#include "..\Inputs\MobileElemTypeStrDB.h"
#include "termplandoc.h"
#include "inputs\paxdata.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMoblieElemDlg dialog


CMoblieElemDlg::CMoblieElemDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMoblieElemDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMoblieElemDlg)
	//}}AFX_DATA_INIT
	m_bStructureChanged = FALSE;
}


void CMoblieElemDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMoblieElemDlg)
	DDX_Control(pDX, IDC_BUT_SAVE, m_butSave);
	DDX_Control(pDX, IDC_LIST_MOBILE_ELEM, m_listElem);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMoblieElemDlg, CDialog)
	//{{AFX_MSG_MAP(CMoblieElemDlg)
	ON_WM_CREATE()
	ON_COMMAND(ID_BUT_PIPEPRO_NEW, OnButElemNew)
	ON_COMMAND(ID_BUT_PIPEPRO_DEL, OnButElemDel)
	ON_LBN_SELCHANGE(IDC_LIST_MOBILE_ELEM, OnSelchangeListElem)
	ON_MESSAGE(WM_END_EDIT,OnEndEdit)
	ON_BN_CLICKED(IDC_BUT_SAVE, OnButSave)
	ON_COMMAND(ID_PEOPLEMOVER_CHANGE, OnChangeName)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMoblieElemDlg message handlers

BOOL CMoblieElemDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	InitToolBar();
	LoadList();
	m_butSave.EnableWindow( FALSE );
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

//load mobile element from mem to listBox
void CMoblieElemDlg::LoadList()
{
	m_listElem.ResetContent();
	/*
	VISITOR;
	CHECKED BAGGAGE
	HAND BAGGAGE;
	CARTE;
	WHELLCHAIRS;
	ANIMALS;
	CAR;
	USER DEFINE1;
	USER DEFINE2;
	USER DEFINE3;
	*/
	int nCount	= GetInputTerminal()->m_pMobElemTypeStrDB->GetCount();
	for(int i=1; i<nCount; i++)
	{
		int iIndex = m_listElem.InsertString(-1,GetInputTerminal()->m_pMobElemTypeStrDB->GetString( i ) );
		m_listElem.SetItemData( iIndex,i );
	}
}

int CMoblieElemDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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

void CMoblieElemDlg::InitToolBar()
{
	CRect mRect;
	GetDlgItem( IDC_STATIC_TOOLBAR_POS )->GetWindowRect( &mRect );
	ScreenToClient( &mRect );
	
	m_ToolBar.MoveWindow( &mRect );
	m_ToolBar.ShowWindow( SW_SHOW );
	GetDlgItem( IDC_STATIC_TOOLBAR_POS )->ShowWindow( SW_HIDE );

	m_ToolBar.GetToolBarCtrl().HideButton( ID_PEOPLEMOVER_NEW,TRUE );
	m_ToolBar.GetToolBarCtrl().HideButton( ID_PEOPLEMOVER_DELETE, TRUE );
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_CHANGE, FALSE );
}

void CMoblieElemDlg::OnButElemNew() 
{
	assert(0);
	/*
	m_nSelIdx = m_listElem.GetCount();
	m_listElem.InsertString( m_nSelIdx, "" );
	m_listElem.SetCurSel( m_nSelIdx );
	OnSelchangeListElem();
	
	EditItem( m_nSelIdx );	
	
	m_butSave.EnableWindow( true );
	m_bStructureChanged = true;
	*/
}

void CMoblieElemDlg::OnButElemDel() 
{
	assert(0);
	/*
	int nCurSel = m_listElem.GetCurSel();
	if( nCurSel == LB_ERR )
		return;

	if(  m_listElem.GetCount() < 2)				//can't delete all items
	{
		MessageBox("Can't delete all items.");
		return;
	}
	
	int iItemData = m_listElem.GetItemData( nCurSel );
	GetInputTerminal()->m_pMobElemTypeStrDB->RemoveAt( iItemData );
	LoadList();

	m_listElem.SetCurSel( nCurSel-1 );
	OnSelchangeListElem();

	m_butSave.EnableWindow( true );
	m_bStructureChanged = true;
	*/
}

void CMoblieElemDlg::OnChangeName() 
{
	m_nSelIdx = m_listElem.GetCurSel();
	if( m_nSelIdx == LB_ERR || m_listElem.GetItemData(m_nSelIdx)<=7)
	{
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_CHANGE, FALSE );
		return;
	}
		
	EditItem( m_nSelIdx );
	
	m_butSave.EnableWindow( TRUE );
	m_bStructureChanged = true;
}

void CMoblieElemDlg::OnSelchangeListElem() 
{
	/***************
	VISITOR;
	CHECKED BAGGAGE
	HAND BAGGAGE;
	CARTE;
	WHELLCHAIRS;
	ANIMALS;
	CAR;
	...(some new items)
	USER DEFINE1;
	USER DEFINE2;
	USER DEFINE3;
	****************/
	//if item data more than 6, it must add by use!
	int nCurSel = m_listElem.GetCurSel();
	if( nCurSel == LB_ERR )
	{
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_CHANGE,FALSE );
	}
	else
	{	
		BOOL bEnable = m_listElem.GetItemData(nCurSel) > NOPAX_COUNT_EXCLUDE_USER_DEFINE/* ? TRUE : FALSE */;
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_CHANGE,bEnable );
	}
}

void CMoblieElemDlg::EditItem(int _nIdx)
{
	CRect rectItem;
	CRect rectLB;
	m_listElem.GetWindowRect( &rectLB );
	ScreenToClient( &rectLB );
	m_listElem.GetItemRect( _nIdx, &rectItem );
	
	rectItem.OffsetRect( rectLB.left+2, rectLB.top+1 );
	
	rectItem.right += 1;
	rectItem.bottom += 4;
	
	CString csItemText;
	m_listElem.GetText( _nIdx, csItemText );
	CSmartEdit* pEdit = new CSmartEdit( csItemText );
	DWORD dwStyle = ES_LEFT | WS_BORDER|WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL;
	pEdit->Create( dwStyle, rectItem, this, IDC_IPEDIT );	

	m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_CHANGE,FALSE );
}


LONG CMoblieElemDlg::OnEndEdit( WPARAM p_wParam, LPARAM p_lParam )
{
	if( p_lParam != IDC_IPEDIT )
		return 0;
	
	if( m_nSelIdx == LB_ERR )
		return 0;
	
	CString csStr( (LPTSTR)p_wParam );
	if( csStr.IsEmpty() )
	{
		MessageBox( "Empty String", "Error", MB_OK|MB_ICONWARNING );
		//m_listElem.DeleteString( m_nSelIdx );
		m_listElem.SetCurSel( -1 );
		OnSelchangeListElem();
	}
	else
	{
		// check if dup
		int i=0; 
		for(; i<m_nSelIdx; i++ )
		{
			CString csTemp;
			m_listElem.GetText( i, csTemp );
			if( csTemp.CompareNoCase( csStr ) == 0 )
			{
				MessageBox( "Duplicated Name", "Error", MB_OK|MB_ICONWARNING );
				//m_listElem.DeleteString( m_nSelIdx );
				m_listElem.SetCurSel( -1 );
				OnSelchangeListElem();
				break;
			}
		}

		if( i == m_nSelIdx )
		{
			// modify to the database
			int iItemData = m_listElem.GetItemData( m_nSelIdx );
			assert( iItemData>7 );
			GetInputTerminal()->m_pMobElemTypeStrDB->ModifyElem( iItemData, csStr );
			// reload list
			LoadList();
			m_listElem.SetCurSel( m_nSelIdx );
			OnSelchangeListElem();
		}
	}
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_CHANGE,TRUE );
	return 0;
}


void CMoblieElemDlg::OnButSave() 
{
	// TODO: Add your control notification handler code here
	
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	CString strPrjPath	= pDoc->m_ProjInfo.path;

	GetInputTerminal()->m_pMobElemTypeStrDB->saveDataSet( strPrjPath, FALSE );
	//after MobileElemTypeDB been changed
	// must reload PassengerData to make sure noPaxData is validate
	try
	{
		GetInputTerminal()->paxDataList->loadDataSet( strPrjPath );
	}
	catch( FileVersionTooNewError* _pError )
	{
		char szBuf[128];
		_pError->getMessage( szBuf );
		MessageBox( szBuf, "Error", MB_OK|MB_ICONWARNING );
		delete _pError;			
	}

	m_butSave.EnableWindow( false );
	m_bStructureChanged = FALSE;
}

void CMoblieElemDlg::OnOK() 
{
	// TODO: Add extra validation here
	OnButSave();	
	CDialog::OnOK();
}

void CMoblieElemDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	CString strPrjPath	= pDoc->m_ProjInfo.path;

	try
	{
		GetInputTerminal()->m_pMobElemTypeStrDB->loadDataSet( strPrjPath );
	}
	catch( FileVersionTooNewError* _pError )
	{
		char szBuf[128];
		_pError->getMessage( szBuf );
		MessageBox( szBuf, "Error", MB_OK|MB_ICONWARNING );
		delete _pError;			
	}
		
	CDialog::OnCancel();
}

InputTerminal* CMoblieElemDlg::GetInputTerminal()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
    return (InputTerminal*)&pDoc->GetTerminal();
}



bool CMoblieElemDlg::isNumString(const CString &_str) const
{
	for( int i=0; i<_str.GetLength(); i++ )
		if( !isNumeric( _str[i] ) )
			return false;
	return true;
}
