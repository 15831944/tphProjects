// LeadLagTimeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "LeadLagTimeDlg.h"
#include "..\common\winmsg.h"
#include "..\inputs\con_db.h"
#include "..\inputs\paxdata.h"
#include "..\inputs\probab.h"
#include "..\common\probdistmanager.h"
#include "..\inputs\flightconwithprociddatabase.h"
#include "termplandoc.h"
#include "passengertypedialog.h"
#include "flightdialog.h"
#include "ProbDistDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLeadLagTimeDlg dialog


CLeadLagTimeDlg::CLeadLagTimeDlg(CWnd* pParent /*=NULL*/)
: CDialog(CLeadLagTimeDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLeadLagTimeDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CLeadLagTimeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLeadLagTimeDlg)
	DDX_Control(pDX, IDC_STATIC_TOOLBARCONTENTER, m_toolbarcontentor);
	DDX_Control(pDX, IDC_BUTTON_SAVE, m_btnSave);
	DDX_Control(pDX, IDC_LIST_DATA, m_listctrlData);

	DDX_Control(pDX, IDC_STATIC_FRAME, m_Framebarcontenter);
	DDX_Control(pDX, IDOK, m_btnOk);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDC_DELAY_IMPORT, m_btnImport);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLeadLagTimeDlg, CDialog)
	//{{AFX_MSG_MAP(CLeadLagTimeDlg)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_DATA, OnEndlabeleditListData)
	ON_COMMAND(ID_TOOLBARBUTTONADD, OnToolbarbuttonadd)
	ON_COMMAND(ID_TOOLBARBUTTONDEL, OnToolbarbuttondel)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_DATA, OnItemchangedListData)
	ON_MESSAGE( UM_LIST_CONTENT_CHANGED, OnListContentChanged )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLeadLagTimeDlg message handlers

int CLeadLagTimeDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_ToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_ToolBar.LoadToolBar(IDR_ADDDELEDITTOOLBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}	
	return 0;
}

void CLeadLagTimeDlg::InitToolbar()
{
	CRect rc;
	m_toolbarcontentor.GetWindowRect( &rc );
	ScreenToClient(&rc);
	m_ToolBar.MoveWindow(&rc);
	m_ToolBar.ShowWindow(SW_SHOW);
	m_toolbarcontentor.ShowWindow(SW_HIDE);
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONADD );
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL,FALSE );
	m_ToolBar.GetToolBarCtrl().HideButton( ID_TOOLBARBUTTONEDIT );
}


void CLeadLagTimeDlg::SetListCtrl()
{
	// set list_ctrl	
	DWORD dwStyle = m_listctrlData.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_listctrlData.SetExtendedStyle( dwStyle );
	
	LV_COLUMNEX	lvc;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT ;
	char columnLabel[3][128];
	strcpy( columnLabel[0], "Mobile Element Type" );		
	strcpy( columnLabel[1], "Stand" );
	sprintf( columnLabel[2], "Probability Distribution ( %s )", "MINS" );
	
	int DefaultColumnWidth[] = { 200,100, 320 };
	int nFormat[] = {	LVCFMT_CENTER, LVCFMT_CENTER, LVCFMT_CENTER };

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

	for( int i=0; i<3; i++ )
	{
		lvc.csList = &strList;
		lvc.pszText = columnLabel[i];
		lvc.cx = DefaultColumnWidth[i];
		if( i == 0 || i== 1 )
		{
			lvc.fmt = LVCFMT_NOEDIT;
		}
		else
		{
			lvc.fmt = LVCFMT_DROPDOWN;
		}
		m_listctrlData.InsertColumn( i, &lvc );
	}

	m_listctrlData.SetInputTerminal( GetInputTerminal() );
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	

	m_listctrlData.SetProjectID(pDoc->GetProjectID());
}


InputTerminal* CLeadLagTimeDlg::GetInputTerminal()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
    return (InputTerminal*)&pDoc->GetTerminal();
}


CString CLeadLagTimeDlg::GetProjPath()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	return pDoc->m_ProjInfo.path;
}

BOOL CLeadLagTimeDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	//////////////////////////////////////////////////////////////////////////
	InitToolbar();
	SetWindowText("Lead Lag Time");
	SetListCtrl();
	ReloadData(NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


//Reload data from database and select the item as parameter.
void CLeadLagTimeDlg::ReloadData( Constraint* _pSelCon )
{
	CMobileConWithProcIDDatabase* pConDB = GetInputTerminal()->paxDataList->getLeadLagTime();
	m_listctrlData.SetCurConDB( (ConstraintDatabase *)pConDB );
	m_listctrlData.ReloadData( _pSelCon );
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL,m_listctrlData.GetCurSel() != -1 );
}
	

void CLeadLagTimeDlg::OnToolbarbuttonadd() 
{
	bool bDirty = false;

	CPassengerTypeDialog dlg( m_pParentWnd );
	if (dlg.DoModal() == IDOK)
	{
		CMobileElemConstraint mobileConstr = dlg.GetMobileSelection();
		bDirty = m_listctrlData.AddEntry( PAX_CON, &mobileConstr );
	}
			
	if( bDirty )
		m_btnSave.EnableWindow();
	
	
}

void CLeadLagTimeDlg::OnToolbarbuttondel() 
{
	if( m_listctrlData.DeleteEntry() )
		m_btnSave.EnableWindow();

	m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL,FALSE );
}

void CLeadLagTimeDlg::OnItemchangedListData(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if( m_listctrlData.GetCurSel()!= -1 )
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL );
	
	*pResult = 0;
}

void CLeadLagTimeDlg::OnEndlabeleditListData(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here
	LV_ITEM* plvItem = &pDispInfo->item;
	
	m_btnSave.EnableWindow();
	*pResult = 0;
}


void CLeadLagTimeDlg::OnOK() 
{
	if( !m_btnSave.IsWindowEnabled() )  // nonzero if the window was previously disabled
	{
		CDialog::OnOK();
		return;
	}
	
	AfxGetApp()->BeginWaitCursor();
	ResortAndSaveData( false );
	AfxGetApp()->EndWaitCursor();
	CDialog::OnOK();
}

void CLeadLagTimeDlg::OnButtonSave() 
{
	CWaitCursor wc;
	
	ResortAndSaveData( true );
	
	m_btnSave.EnableWindow( FALSE );
}

void CLeadLagTimeDlg::ResortAndSaveData( bool _bNeedReloadList /*= false*/ )
{
	GetInputTerminal()->paxDataList->getLeadLagTime()->resortDatebase();
	GetInputTerminal()->paxDataList->saveDataSet( GetProjPath(), true );

	if( _bNeedReloadList )
		ReloadData( NULL );
}

void CLeadLagTimeDlg::OnCancel() 
{
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

LRESULT CLeadLagTimeDlg::OnListContentChanged( WPARAM wParam, LPARAM lParam )
{
	m_btnSave.EnableWindow( TRUE );
	return TRUE;
}

void CLeadLagTimeDlg::OnGetMinMaxInfo( MINMAXINFO FAR* lpMMI )
{
	// TODO: Add your message handler code here and/or call default

	lpMMI->ptMinTrackSize.x = 552;
	lpMMI->ptMinTrackSize.y = 348;

	CDialog::OnGetMinMaxInfo(lpMMI);
}

void CLeadLagTimeDlg::OnSize( UINT nType, int cx, int cy )
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
	m_ToolBar.GetWindowRect( &Barrc );
	m_toolbarcontentor.MoveWindow(10,10,cx-20,Barrc.Height());
	m_ToolBar.MoveWindow(11,10,cx-20,Barrc.Height());
	m_listctrlData.MoveWindow(10,10+Barrc.Height(),cx-20,cy-15-2*rc.Height()-Barrc.Height()-15);
	InvalidateRect(NULL);
}