// DlgDocDisplayTime.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgDocDisplayTime.h"
#include "../InputOnBoard/DocDisplayTimeDB.h"
#include "../Inputs/IN_TERM.H"
#include "../Inputs/probab.h"
#include "../Common/ProbDistManager.h"
#include "../Common/NewPassengerType.h"
#include "DlgNewPassengerType.h"
// CDlgDocDisplayTime dialog

IMPLEMENT_DYNAMIC(CDlgDocDisplayTime, CXTResizeDialog)
CDlgDocDisplayTime::CDlgDocDisplayTime(InputTerminal* _InputTerminal ,CWnd* pParent /*=NULL*/)
	: m_terminal(_InputTerminal),CXTResizeDialog(CDlgDocDisplayTime::IDD, pParent)
{
	m_TimeDB = new CDocDisplayTimeDB ;
}

CDlgDocDisplayTime::~CDlgDocDisplayTime()
{
	if(m_TimeDB != NULL)
		delete m_TimeDB ;
	m_TimeDB = NULL ;
}

void CDlgDocDisplayTime::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_TIME, m_listctrlData);
	DDX_Control(pDX, IDC_STATIC_TOOLBAR, m_sta_tool);

}


BEGIN_MESSAGE_MAP(CDlgDocDisplayTime, CDialog)
	ON_WM_SIZE()
    ON_WM_CREATE() 
	ON_COMMAND(ID_TOOLBAR_DELDISPLAY,OnDelDocDisplayTime)
	ON_COMMAND(ID_TOOLBAR_NEWDISPLAY,OnNewDocDisplayTime)
END_MESSAGE_MAP()

int CDlgDocDisplayTime::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolbar.CreateEx(this,  TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP ) ||
		!m_wndToolbar.LoadToolBar(ID_TOOLBAR_DOCDISPLAY))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	return 0;
}
void CDlgDocDisplayTime::OnSize(UINT nType, int cx, int cy) 
{
	CXTResizeDialog::OnSize(nType,cx,cy) ;
	if(m_listctrlData.m_hWnd)
		m_listctrlData.ResizeColumnWidth();
}
BOOL CDlgDocDisplayTime::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog() ;
	m_TimeDB->ReadDataFromDB(m_terminal->inStrDict) ;
	InitToobar();
	InitList() ;
// 	SetResize(IDC_STATIC_PAX,SZ_TOP_LEFT,SZ_TOP_LEFT) ;
	SetResize(IDC_STATIC_DOCTIME_LIST,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT) ;
	SetResize(IDC_STATIC_TOOLBAR,SZ_TOP_LEFT,SZ_TOP_LEFT) ;
	SetResize(m_wndToolbar.GetDlgCtrlID(),SZ_TOP_LEFT,SZ_TOP_LEFT);
	SetResize(IDC_LIST_TIME,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT) ;
	SetResize(IDOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT) ;
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT) ;
	return true ;
}
void CDlgDocDisplayTime::InitList()
{
 InitListView() ;
 InitListData() ;
}
void CDlgDocDisplayTime::InitListView()
{
	DWORD dwStyle = m_listctrlData.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_listctrlData.SetExtendedStyle( dwStyle );

	LV_COLUMNEX	lvc;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT ;
	char columnLabel[2][128];
	sprintf( columnLabel[1], "Probability Distribution ( SECONDS )" );
	CString csLabel;
	strcpy( columnLabel[0], "Passenger Type" );		
	//		break;
	//	}
	int DefaultColumnWidth[] = { 220, 300 };
	int nFormat[] = {	LVCFMT_CENTER, LVCFMT_CENTER };
	for( int i=0; i<2; i++ )
	{
		CStringList strList;
		CString s;
		s.LoadString(IDS_STRING_NEWDIST);
		strList.InsertAfter( strList.GetTailPosition(), s );
		int nCount = _g_GetActiveProbMan( m_terminal)->getCount();
		for( int m=0; m<nCount; m++ )
		{
			CProbDistEntry* pPBEntry = _g_GetActiveProbMan(m_terminal)->getItem( m );			
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
	m_listctrlData.InitColumnWidthPercent() ;
	m_listctrlData.SetInputTerminal( m_terminal);
}
void CDlgDocDisplayTime::InitListData()
{
	CDocDisplayTimeDB::TY_DISPLAYTIME_DB_ITER iter = m_TimeDB->GetEntryDB()->begin() ;
	for ( ; iter != m_TimeDB->GetEntryDB()->end() ;iter++)
	{
		InsertItemToList(*iter,(*iter)->GetPaxType()) ;
	}
}
void CDlgDocDisplayTime::InsertItemToList(CDocDisplayTimeEntry* _entry,CPassengerType* _paxTY) 
{
	if(_entry == NULL)
		return ;
	int nIdx = m_listctrlData.InsertItem( 0, _paxTY->PrintStringForShow());
	m_listctrlData.SetItemData( nIdx, (DWORD_PTR)_entry);
	m_listctrlData.SetItemText( nIdx, 1, _entry->GetConstantName() );
}
void CDlgDocDisplayTime::InitToobar()
{
   CRect rect ;
   m_sta_tool.GetWindowRect(&rect) ;
   ScreenToClient(&rect);
   m_wndToolbar.MoveWindow(rect) ;
   m_sta_tool.ShowWindow(SW_HIDE) ;
   m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_NEWDISPLAY,TRUE) ;
   m_wndToolbar.GetToolBarCtrl().EnableButton(ID_TOOLBAR_DELDISPLAY,TRUE) ;
}
void CDlgDocDisplayTime::OnOK()
{
	try
	{
		CADODatabase::BeginTransaction() ;
		m_TimeDB->SaveData() ;
		CADODatabase::CommitTransaction() ;
	}
	catch (CADOException e)
	{
		CADODatabase::RollBackTransation() ;
	}

	CXTResizeDialog::OnOK() ;
}
void CDlgDocDisplayTime::OnCancel()
{
	CXTResizeDialog::OnCancel() ;
}
void CDlgDocDisplayTime::OnNewDocDisplayTime()
{
    CPassengerType paxTy(m_terminal->inStrDict) ;
	CDlgNewPassengerType dlgPaxty(&paxTy,m_terminal,this) ;
	if(dlgPaxty.DoModal() == IDOK)
	{
         CDocDisplayTimeEntry* timeEntry = m_TimeDB->AddDocDisplayTimEntry(paxTy.PrintStringForShow(),m_terminal->inStrDict) ;
		 if(timeEntry != NULL)
		 {
			 InsertItemToList(timeEntry,timeEntry->GetPaxType()) ;
		 }
		 else
		 {
			 MessageBox(_T("The passenger type already exists!"), _T("Error"), MB_OK | MB_ICONWARNING);
		 }
	}
}
void CDlgDocDisplayTime::OnDelDocDisplayTime()
{
	POSITION  posi = m_listctrlData.GetFirstSelectedItemPosition() ;
	CDocDisplayTimeEntry* p_timeentry = NULL ;
	if(posi == NULL)
		return ;
	int ndx = 0 ;
	ndx =  m_listctrlData.GetNextSelectedItem(posi) ;
	p_timeentry = (CDocDisplayTimeEntry*)m_listctrlData.GetItemData(ndx) ;
	m_TimeDB->DelDocDisplayTimeEntry(p_timeentry) ;
	m_listctrlData.DeleteItem(ndx) ;

}
// CDlgDocDisplayTime message handlers
