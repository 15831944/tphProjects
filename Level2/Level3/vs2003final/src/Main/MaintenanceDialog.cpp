// MaintenanceDialog.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "MaintenanceDialog.h"
#include "TermPlanDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ITEMDATA_STARTIDX 100

/////////////////////////////////////////////////////////////////////////////
// CMaintenanceDialog dialog


CMaintenanceDialog::CMaintenanceDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CMaintenanceDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMaintenanceDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_arColName.RemoveAll();
	m_strTitle.Empty();

	m_pMaintenanceCostDB = GetEconomicManager()->m_pMaintenanceCost;

}


void CMaintenanceDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMaintenanceDialog)
	DDX_Control(pDX, IDC_STATIC_MAINTENANCETOOLBAR, m_listtoolbarcontenter);
	DDX_Control(pDX, IDC_LIST_MAINTENANCE, m_List);
	DDX_Control(pDX, IDC_BTN_MAINTENANCE_SAVE, m_btnSave);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMaintenanceDialog, CDialog)
	//{{AFX_MSG_MAP(CMaintenanceDialog)
	ON_WM_CREATE()
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_MAINTENANCE, OnEndlabeleditListMaintenance)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_MAINTENANCE, OnItemchangedListMaintenance)
	ON_BN_CLICKED(IDC_BTN_MAINTENANCE_SAVE, OnBtnMaintenanceSave)
	ON_COMMAND(ID_PEOPLEMOVER_NEW, OnPeoplemoverNew)
	ON_COMMAND(ID_PEOPLEMOVER_DELETE, OnPeoplemoverDelete)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMaintenanceDialog message handlers


BOOL CMaintenanceDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	AfxGetApp()->BeginWaitCursor();

	SetWindowText(m_strTitle);

	// Init tool bar
	InitToolbar();

	// Init list ctrl
	InitListCtrl();
	
	ReloadDatabase();

	m_btnSave.EnableWindow( FALSE );
	
	AfxGetApp()->EndWaitCursor();	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

int CMaintenanceDialog::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_ListToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_ListToolBar.LoadToolBar(IDR_PEOPLEMOVEBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	
	// Init list ctrl
	InitListCtrl();
	
	return 0;
}

void CMaintenanceDialog::OnEndlabeleditListMaintenance(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	*pResult = 0;

	int nIdx = pDispInfo->item.iItem;

	int nItemData = m_List.GetItemData( nIdx );

	assert( m_pMaintenanceCostDB );
	std::vector<MaintenanceCostDataItem>& vData = m_pMaintenanceCostDB->GetDataList();

	if( nItemData == 0 )
	{
		// new item
		MaintenanceCostDataItem item;
		item.m_csLabel = m_List.GetItemText( nIdx, 0 );
		item.m_dCostPerLanding = 0.0;
		item.m_dCostPerYear = 0.0;
		item.m_dCostPerFlight = 0.0;
		item.m_dCostPerDay = 0.0;

		vData.push_back( item );
		m_List.SetItemData( nIdx, vData.size() - 1 + ITEMDATA_STARTIDX );
	}
	else
	{
		nItemData -= ITEMDATA_STARTIDX;
		
		// modify the exist data.
		if( pDispInfo->item.iSubItem == 0 )
		{
			vData[nItemData].m_csLabel = m_List.GetItemText( nIdx, 0 );
		}
		else
		{
			CString csStr = m_List.GetItemText( nIdx, pDispInfo->item.iSubItem );
			csStr.TrimLeft(); 
			csStr.TrimRight();
			switch( pDispInfo->item.iSubItem )
			{
			case 1:
				vData[nItemData].m_dCostPerLanding = atof( csStr );
				break;
			case 2:
				vData[nItemData].m_dCostPerYear = atof( csStr );
				break;
			case 3:
				vData[nItemData].m_dCostPerFlight = atof( csStr );
				break;
			case 4:
				vData[nItemData].m_dCostPerDay = atof( csStr );
				break;
			}
			
		}
	}

	m_btnSave.EnableWindow();


}

void CMaintenanceDialog::OnItemchangedListMaintenance(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	if (GetSelectedItem() >= 0)
		m_ListToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE ,TRUE);
	
	*pResult = 0;
}

void CMaintenanceDialog::OnBtnMaintenanceSave() 
{
	assert( m_pMaintenanceCostDB );
	CWaitCursor wc;
	m_pMaintenanceCostDB->saveDataSet( GetProjPath(), true );
	m_btnSave.EnableWindow(FALSE);
}

void CMaintenanceDialog::OnPeoplemoverNew() 
{
	m_List.EditNew();
}

void CMaintenanceDialog::OnPeoplemoverDelete() 
{
	int nSelIdx = GetSelectedItem();
	if( nSelIdx == -1 )
		return;

	int nDBIdx = m_List.GetItemData( nSelIdx ) - ITEMDATA_STARTIDX;
	assert( m_pMaintenanceCostDB );
	std::vector<MaintenanceCostDataItem>& vData = m_pMaintenanceCostDB->GetDataList();
	vData.erase( vData.begin() + nDBIdx );

	m_btnSave.EnableWindow();
	m_List.DeleteItem( nSelIdx );
	m_ListToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE ,FALSE);
}

void CMaintenanceDialog::InitListCtrl()
{
	if (!::IsWindow(m_List.GetSafeHwnd()))
		return;

	if (m_arColName.GetSize() <= 0)
		return;
	
	// set list ctrl	
	DWORD dwStyle = m_List.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_List.SetExtendedStyle( dwStyle );

	LV_COLUMNEX	lvc;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT ;	

	CRect rcList;
	m_List.GetClientRect(&rcList);
	int w = rcList.Width()/m_arColName.GetSize(),
		w1 = rcList.Width() - w*(m_arColName.GetSize()-1);
	int nFormat[] = { LVCFMT_CENTER, LVCFMT_CENTER | LVCFMT_NUMBER };

	for( int i = 0; i < m_arColName.GetSize(); i++ )
	{
		CStringList strList;
		strList.AddTail( CString("OPEN") );
		strList.AddTail( CString("CLOSE") );
		lvc.csList = &strList;
		lvc.pszText = (LPSTR) ((LPCTSTR) m_arColName.GetAt(i));
		lvc.cx = (i== 0 ? w1:w);
		lvc.fmt = (i == 0 ? nFormat[0] : nFormat[1]);
		m_List.InsertColumn( i, &lvc );
	}
}

void CMaintenanceDialog::InitToolbar()
{
	CRect rc;
	m_listtoolbarcontenter.GetWindowRect( &rc );
	ScreenToClient(&rc);
	m_ListToolBar.MoveWindow(&rc);
	m_ListToolBar.ShowWindow(SW_SHOW);
	m_listtoolbarcontenter.ShowWindow(SW_HIDE);
	m_ListToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_NEW ,TRUE);	
	m_ListToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE ,FALSE);	
	m_ListToolBar.GetToolBarCtrl().HideButton( ID_PEOPLEMOVER_CHANGE );
}

void CMaintenanceDialog::SetTitle(LPCTSTR szTitle)
{
	m_strTitle = szTitle;

	if (!::IsWindow(GetSafeHwnd()))
		return;

	SetWindowText(m_strTitle);
}

void CMaintenanceDialog::SetColNameArray(const CStringArray& arColName)
{
	m_arColName.RemoveAll();
	
	for (int i = 0; i < arColName.GetSize(); i++)
		m_arColName.Add(arColName.GetAt(i));
}

int CMaintenanceDialog::GetSelectedItem()
{
	int nCount = m_List.GetItemCount();
	for( int i=0; i<nCount; i++ )
	{
		if( m_List.GetItemState( i, LVIS_SELECTED ) == LVIS_SELECTED )
		{
			return i;
		}
	}
	return -1;
}

void CMaintenanceDialog::OnOK() 
{
	if( m_btnSave.IsWindowEnabled() )
	{
		assert( m_pMaintenanceCostDB );
		CWaitCursor wc;
		m_pMaintenanceCostDB->saveDataSet( GetProjPath(), true );
	}
	CDialog::OnOK();
}

void CMaintenanceDialog::OnCancel() 
{
	if( m_btnSave.IsWindowEnabled() )
	{
		CWaitCursor wc;
		try
		{
			m_pMaintenanceCostDB->loadDataSet( GetProjPath() );
		}
		catch( FileVersionTooNewError* _pError )
		{
			char szBuf[128];
			_pError->getMessage( szBuf );
			MessageBox( szBuf, "Error", MB_OK|MB_ICONWARNING );
			delete _pError;			
		}
	}		
	CDialog::OnCancel();
}


void CMaintenanceDialog::ReloadDatabase()
{
	// Clean all the controls
	m_List.DeleteAllItems();
	
	assert( m_pMaintenanceCostDB );

	std::vector<MaintenanceCostDataItem>& vData = m_pMaintenanceCostDB->GetDataList();
	int nCount = vData.size();
	for( int i=0; i<nCount; i++ )
	{
		int nItem = m_List.InsertItem( i, vData[i].m_csLabel );

		CString csVal;
		csVal.Format( "%.2f", vData[i].m_dCostPerLanding );
		m_List.SetItemText( nItem, 1, csVal );
		csVal.Format( "%.2f", vData[i].m_dCostPerYear );
		m_List.SetItemText( nItem, 2, csVal );
		csVal.Format( "%.2f", vData[i].m_dCostPerFlight );
		m_List.SetItemText( nItem, 3, csVal );
		csVal.Format( "%.2f", vData[i].m_dCostPerDay );
		m_List.SetItemText( nItem, 4, csVal );
		
		m_List.SetItemData( nItem, i + ITEMDATA_STARTIDX );

	}
}


CEconomicManager* CMaintenanceDialog::GetEconomicManager()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
    return (CEconomicManager*)&pDoc->GetEconomicManager();
}


CString CMaintenanceDialog::GetProjPath()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	return pDoc->m_ProjInfo.path;
}

