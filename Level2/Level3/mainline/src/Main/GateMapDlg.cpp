// GateMapDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "GateMapDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "..\MFCExControl\ListCtrlEx.h"
#include "engine\process.h"
#include "engine\proclist.h"
/////////////////////////////////////////////////////////////////////////////
// CGateMapDlg dialog


CGateMapDlg::CGateMapDlg(InputTerminal* _pInTerm,const CString& _strPath,CWnd* pParent /*=NULL*/)
	: CDialog(CGateMapDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGateMapDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pInTerm = _pInTerm;
	m_strProjPath = _strPath;
}


void CGateMapDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGateMapDlg)
	DDX_Control(pDX, IDOK, m_butOK);
	DDX_Control(pDX, IDCANCEL, m_butCancel);
	DDX_Control(pDX, IDC_BUT_SAVE, m_butSave);
	DDX_Control(pDX, IDC_STATIC_TOOLBAR, m_staticToolBar);
	DDX_Control(pDX, IDC_LIST_MAP, m_listMap);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGateMapDlg, CDialog)
	//{{AFX_MSG_MAP(CGateMapDlg)
	ON_WM_CREATE()
	ON_COMMAND(ID_TOOLBARBUTTONADD, OnToolbarbuttonadd)
	ON_COMMAND(ID_TOOLBARBUTTONDEL, OnToolbarbuttondel)
	ON_BN_CLICKED(IDC_BUT_SAVE, OnButSave)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_MAP, OnItemchangedListMap)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_MAP, OnDblclkListMap)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_MAP, OnEndlabeleditListMap)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGateMapDlg message handlers

BOOL CGateMapDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	InitToolBar();
	InitListCtrl();

	ReloadMapData();

	m_butSave.EnableWindow( FALSE );
	m_butOK.EnableWindow(TRUE) ;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

int CGateMapDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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

void CGateMapDlg::InitToolBar()
{
	CRect rc;
	m_staticToolBar.GetWindowRect( &rc );
	ScreenToClient(&rc);
	m_ToolBar.MoveWindow(&rc);
	m_ToolBar.ShowWindow(SW_SHOW);
	m_staticToolBar.ShowWindow(SW_HIDE);
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONADD );
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL ,FALSE);
	m_ToolBar.GetToolBarCtrl().HideButton( ID_TOOLBARBUTTONEDIT );
	
}

void CGateMapDlg::InitListCtrl()
{
	// get all gate
	CStringList strList;
	CString strProcID;
	ProcessorArray processArray;
	m_pInTerm->procList->getProcessorsOfType( GateProc, processArray );
	int nGateCount = processArray.getCount();
	for( int i = 0; i < nGateCount; i++ )
	{
		strProcID = processArray[i]->getID()->GetIDString();
		strList.AddTail( strProcID );
	}

	// set list ctrl	
	DWORD dwStyle = m_listMap.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_listMap.SetExtendedStyle( dwStyle );
	
	LV_COLUMNEX	lvc;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT ;
	char* columnLabel[] = {	"Name", "Aircraft Stand", "ARR Gate", "DEP Gate" };
	int DefaultColumnWidth[] = { 150,150, 150, 150 };
	int nFormat[] = { LVCFMT_CENTER|LVCFMT_EDIT, LVCFMT_DROPDOWN|LVCFMT_CENTER, LVCFMT_DROPDOWN|LVCFMT_CENTER, LVCFMT_DROPDOWN|LVCFMT_CENTER };
	
	for(int i=0; i<4; i++ )
	{
		lvc.csList = &strList;
		lvc.pszText = columnLabel[i];
		lvc.cx = DefaultColumnWidth[i];
		lvc.fmt = nFormat[i];
		m_listMap.InsertColumn( i, &lvc );
	}
	
}

void CGateMapDlg::ReloadMapData()
{
	m_listMap.DeleteAllItems();
	int iCount = m_pInTerm->m_pGateMappingDB->getCount();
	for( int i =0; i<iCount; i++ )
	{
		const CGateMapping& gatemap  = m_pInTerm->m_pGateMappingDB->getItem( i );
		AddAMappingToList( gatemap,i );
	}

	m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL, FALSE );
}


void CGateMapDlg::AddAMappingToList( const CGateMapping& gatemap, int _index)
{
	CString strProcID;
	int iRow = m_listMap.InsertItem(0,gatemap.getMapName());
	
	strProcID = gatemap.getStandGate().GetIDString();
	m_listMap.SetItemText( iRow, 1, strProcID );
	
	strProcID = gatemap.getArrGate().GetIDString();
	m_listMap.SetItemText(iRow, 2, strProcID );

	strProcID = gatemap.getDepGate().GetIDString();
	m_listMap.SetItemText( iRow, 3, strProcID );

	m_listMap.SetItemData( iRow, _index );
}


void CGateMapDlg::OnToolbarbuttonadd() 
{
	// TODO: Add your command handler code here
	CGateMapping gatemap(m_pInTerm);
	int index = m_pInTerm->m_pGateMappingDB->addItem( gatemap );
	
	AddAMappingToList( gatemap, index );

	m_butSave.EnableWindow( TRUE );
}

void CGateMapDlg::OnToolbarbuttondel() 
{
	// TODO: Add your command handler code here
	int iCurSel =  m_listMap.GetCurSel();
	if( iCurSel == -1 )	// no item selected
		return;

	int iItemData = m_listMap.GetItemData( iCurSel);
	
	assert( iItemData >= 0);

	m_pInTerm->m_pGateMappingDB->removeItem( iItemData );
	ReloadMapData();
	m_butSave.EnableWindow( TRUE );
}


void CGateMapDlg::OnButSave() 
{
	// TODO: Add your control notification handler code here
	m_pInTerm->m_pGateMappingDB->saveDataSet( m_strProjPath, true );
	m_butSave.EnableWindow( FALSE );
}

void CGateMapDlg::OnOK() 
{
	// TODO: Add extra validation here
	if( m_butSave.IsWindowEnabled() )
		OnButSave();
	CDialog::OnOK();
}

void CGateMapDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	m_pInTerm->m_pGateMappingDB->loadDataSet( m_strProjPath);
	CDialog::OnCancel();
}

void CGateMapDlg::OnItemchangedListMap(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	int iCurSel = m_listMap.GetCurSel();
	m_ToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONDEL, iCurSel == -1 ? FALSE :TRUE );
	
	*pResult = 0;
}

void CGateMapDlg::OnDblclkListMap(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	int iCurSel = m_listMap.GetCurSel();
	if( iCurSel == -1 )
		return;
	
	*pResult = 0;
}


void CGateMapDlg::OnEndlabeleditListMap(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here
 	LV_ITEM* plvItem = &pDispInfo->item;

	int iCurSel = m_listMap.GetCurSel();
	if( iCurSel == -1 )
		return;
	int iItemData = m_listMap.GetItemData( iCurSel );
	assert( iItemData >= 0 );
	// when you close the windows, myabe crash
	// by doing this check to avoid the crash
	if( iItemData >= m_pInTerm->m_pGateMappingDB->getCount() )
		return;

	CGateMapping* gatemap = m_pInTerm->m_pGateMappingDB->GetItem( iItemData );

	CString strItemText;
	ProcessorID procID;
	procID.SetStrDict(m_pInTerm->inStrDict);

	switch( plvItem->iSubItem )
	{
	case 0:
		strItemText = m_listMap.GetItemText(plvItem->iItem,0);
		strItemText.TrimLeft();
		strItemText.TrimRight();
		strItemText.Remove(' ');
		if( strItemText.IsEmpty()  )
		{
			MessageBox("The Name is empty!",NULL,MB_OK|MB_ICONINFORMATION );
			m_listMap.SetItemText( iCurSel, 0, gatemap->getMapName() );
			return;
		}
		if( m_pInTerm->m_pGateMappingDB->ifExistInDB( strItemText, iItemData ) )
		{
			MessageBox("The Name still exist in the db,please chang a name!",NULL,MB_OK|MB_ICONINFORMATION );
			m_listMap.SetItemText( iCurSel, 0, gatemap->getMapName() );
			return;
		}
		gatemap->setMapName( strItemText );
		m_listMap.SetItemText( iCurSel, 0, strItemText );
		break;
		
	case 1:
		strItemText = m_listMap.GetItemText(plvItem->iItem,1);
		procID.setID( strItemText );
		gatemap->setStandGate( procID );
		break;

	case 2:
		strItemText = m_listMap.GetItemText(plvItem->iItem,2);
		procID.setID( strItemText );
		gatemap->setArrGate( procID );
		break;

	case 3:
		strItemText = m_listMap.GetItemText(plvItem->iItem,3);
		procID.setID( strItemText );
		gatemap->setDepGate( procID );
		break;
	}	

	*pResult = 0;
	m_butSave.EnableWindow( TRUE );
}

void CGateMapDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	if( m_butCancel.m_hWnd == NULL )
		return;
	
	// TODO: Add your message handler code here
	CRect rc;
	m_butCancel.GetWindowRect(&rc);
	m_butCancel.MoveWindow( cx - 10 - rc.Width(), cy - 10 - rc.Height(), rc.Width(),rc.Height(), TRUE );
	m_butOK.MoveWindow( cx - 10 -10 - 2*rc.Width(), cy - 10 - rc.Height(), rc.Width(),rc.Height(), TRUE );
	m_butSave.MoveWindow( cx - 10-10-10 - 3*rc.Width(), cy - 10 - rc.Height(), rc.Width(),rc.Height(), TRUE );

	m_listMap.MoveWindow(10,35, cx-20, cy-80 , TRUE );

	m_ToolBar.MoveWindow(10,5,  cx-20, 25, TRUE );

	InvalidateRect( NULL );
}

void CGateMapDlg::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	// TODO: Add your message handler code here and/or call default
	lpMMI->ptMinTrackSize.x = 450;
	lpMMI->ptMinTrackSize.y = 300;
	CDialog::OnGetMinMaxInfo(lpMMI);
}
