// UtilitiesDialog.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include <Inputs/InputsDll.h>
#include "UtilitiesDialog.h"
#include "TermPlanDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUtilitiesDialog dialog

#define ITEMDATA_STARTIDX 100

CUtilitiesDialog::CUtilitiesDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CUtilitiesDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CUtilitiesDialog)
	m_dFixCost = 0.0;
	m_dHighCost = 0.0;
	m_nHighHrs = 0;
	m_dLowCost = 0.0;
	m_nLowHrs = 0;
	m_dMedCost = 0.0;
	m_nMedHrs = 0;
	m_nTotalHrs = 0;
	//}}AFX_DATA_INIT

	m_pUtilCostDB = GetEconomicManager()->m_pUtilityCost;

}

void CUtilitiesDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUtilitiesDialog)
	DDX_Control(pDX, IDC_STATIC_UTILITIES_TOOLBARCONTENTER, m_listtoolbarcontenter);
	DDX_Control(pDX, IDC_SPIN_UTILITIES_MED, m_spinMed);
	DDX_Control(pDX, IDC_SPIN_UTILITIES_LOW, m_spinLow);
	DDX_Control(pDX, IDC_SPIN_UTILITIES_HIGH, m_spinHigh);
	DDX_Control(pDX, IDC_LIST_UTILITIES_COMPONENT, m_List);
	DDX_Control(pDX, IDC_EDIT_UTILITIES_TOTAL_HRS, m_editTotalHrs);
	DDX_Control(pDX, IDC_EDIT_UTILITIES_MED_HRS, m_editMedHrs);
	DDX_Control(pDX, IDC_EDIT_UTILITIES_MED_COST, m_editMedCost);
	DDX_Control(pDX, IDC_EDIT_UTILITIES_LOW_HRS, m_editLowHrs);
	DDX_Control(pDX, IDC_EDIT_UTILITIES_LOW_COST, m_editLowCost);
	DDX_Control(pDX, IDC_EDIT_UTILITIES_HIGH_HRS, m_editHighHrs);
	DDX_Control(pDX, IDC_EDIT_UTILITIES_HIGH_COST, m_editHighCost);
	DDX_Control(pDX, IDC_EDIT_UTILITIES_FIXEDCOST, m_editFixedCost);
	DDX_Control(pDX, IDC_BTN_UTILITIES_SAVE, m_btnSave);
	DDX_Control(pDX, IDC_BTN_UTILITIES_CLEAR, m_btnClear);
	DDX_Text(pDX, IDC_EDIT_UTILITIES_FIXEDCOST, m_dFixCost);
	DDX_Text(pDX, IDC_EDIT_UTILITIES_HIGH_COST, m_dHighCost);
	DDX_Text(pDX, IDC_EDIT_UTILITIES_HIGH_HRS, m_nHighHrs);
	DDX_Text(pDX, IDC_EDIT_UTILITIES_LOW_COST, m_dLowCost);
	DDX_Text(pDX, IDC_EDIT_UTILITIES_LOW_HRS, m_nLowHrs);
	DDX_Text(pDX, IDC_EDIT_UTILITIES_MED_COST, m_dMedCost);
	DDX_Text(pDX, IDC_EDIT_UTILITIES_MED_HRS, m_nMedHrs);
	DDX_Text(pDX, IDC_EDIT_UTILITIES_TOTAL_HRS, m_nTotalHrs);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUtilitiesDialog, CDialog)
	//{{AFX_MSG_MAP(CUtilitiesDialog)
	ON_WM_CREATE()
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_UTILITIES_COMPONENT, OnEndlabeleditListUtilitiesComponent)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_UTILITIES_COMPONENT, OnItemchangedListUtilitiesComponent)
	ON_BN_CLICKED(IDC_BTN_UTILITIES_CLEAR, OnBtnUtilitiesClear)
	ON_BN_CLICKED(IDC_BTN_UTILITIES_SAVE, OnBtnUtilitiesSave)
	ON_COMMAND(ID_PEOPLEMOVER_NEW, OnPeoplemoverNew)
	ON_COMMAND(ID_PEOPLEMOVER_DELETE, OnPeoplemoverDelete)
	ON_EN_KILLFOCUS(IDC_EDIT_UTILITIES_HIGH_COST, OnKillfocusEdit)
	ON_EN_KILLFOCUS(IDC_EDIT_UTILITIES_LOW_COST, OnKillfocusEdit)
	ON_EN_KILLFOCUS(IDC_EDIT_UTILITIES_MED_COST, OnKillfocusEdit)
	ON_EN_KILLFOCUS(IDC_EDIT_UTILITIES_MED_HRS, OnKillfocusEdit)
	ON_EN_KILLFOCUS(IDC_EDIT_UTILITIES_HIGH_HRS, OnKillfocusEdit)
	ON_EN_KILLFOCUS(IDC_EDIT_UTILITIES_LOW_HRS, OnKillfocusEdit)
	ON_EN_KILLFOCUS(IDC_EDIT_UTILITIES_FIXEDCOST, OnKillfocusEdit)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_UTILITIES_HIGH, OnDeltaposSpinUtilitiesHigh)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_UTILITIES_LOW, OnDeltaposSpinUtilitiesLow)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_UTILITIES_MED, OnDeltaposSpinUtilitiesMed)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUtilitiesDialog message handlers

BOOL CUtilitiesDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	AfxGetApp()->BeginWaitCursor();
	

	m_spinMed.SetRange( 0, 24 );
	m_spinLow.SetRange( 0, 24 );
	m_spinHigh.SetRange( 0, 24 );

	// Init tool bar
	InitToolbar();

	// Init list ctrl
	InitListCtrl();
	
	m_btnSave.EnableWindow( FALSE );
	m_btnClear.EnableWindow( FALSE );

	ReloadDatabase();
	
	AfxGetApp()->EndWaitCursor();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

int CUtilitiesDialog::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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

void CUtilitiesDialog::InitListCtrl()
{
	if (!::IsWindow(m_List.GetSafeHwnd()))
		return;
	
	// set list ctrl	
	DWORD dwStyle = m_List.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_List.SetExtendedStyle( dwStyle );

	LV_COLUMNEX	lvc;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT ;	

	CRect rcList;
	m_List.GetClientRect(&rcList);
	
	for( int i = 0; i < 1; i++ )
	{
		CStringList strList;
		strList.AddTail( CString("OPEN") );
		strList.AddTail( CString("CLOSE") );
		lvc.csList = &strList;
		lvc.pszText = "Component";
		lvc.cx = rcList.Width();
		lvc.fmt = LVCFMT_CENTER;
		m_List.InsertColumn( i, &lvc );
	}
}

void CUtilitiesDialog::InitToolbar()
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

void CUtilitiesDialog::OnEndlabeleditListUtilitiesComponent(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	*pResult = 0;

	int nIdx = pDispInfo->item.iItem;

	int nItemData = m_List.GetItemData( nIdx );

	assert( m_pUtilCostDB );
	std::vector<UtilityCostDataItem>& vData = m_pUtilCostDB->GetDataList();

	if( nItemData == 0 )
	{
		// new item
		UtilityCostDataItem item;
		item.m_csLabel = m_List.GetItemText( nIdx, 0 );
		item.m_dFixCostPerYear = 0.0;
		item.m_dLowCostPerHour = 0.0;
		item.m_dMedCostPerHour = 0.0;
		item.m_dHighCostPerHour = 0.0;
		item.m_nLowHoursPerDay = 0;
		item.m_nMedHoursPerDay = 0;
		item.m_nHighHoursPerDay = 0;

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
	}

	m_btnSave.EnableWindow();

	*pResult = 0;
}

void CUtilitiesDialog::OnItemchangedListUtilitiesComponent(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	*pResult = 0;

	if( GetSelectedItem() < 0 )
		return;
		
	RefreshData();
	
	m_ListToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE ,TRUE);
}

void CUtilitiesDialog::OnBtnUtilitiesClear() 
{
	int nSelIdx = GetSelectedItem();
	if( nSelIdx == -1 )
		return;

	int nItemData = m_List.GetItemData( nSelIdx ) - ITEMDATA_STARTIDX;
	std::vector<UtilityCostDataItem>& vData = m_pUtilCostDB->GetDataList();
	vData.erase( vData.begin() + nItemData );

	RefreshData();
}

void CUtilitiesDialog::OnBtnUtilitiesSave() 
{
	CWaitCursor wc;
	m_btnSave.EnableWindow(FALSE);
	m_pUtilCostDB->saveDataSet( GetProjPath(), true );
}

void CUtilitiesDialog::OnPeoplemoverNew() 
{
	m_List.EditNew();
}

void CUtilitiesDialog::OnPeoplemoverDelete() 
{

	int nSelIdx = GetSelectedItem();
	if( nSelIdx == -1 )
		return;

	int nDBIdx = m_List.GetItemData( nSelIdx ) - ITEMDATA_STARTIDX;
	assert( m_pUtilCostDB );
	std::vector<UtilityCostDataItem>& vData = m_pUtilCostDB->GetDataList();
	vData.erase( vData.begin() + nDBIdx );
	
	m_btnSave.EnableWindow();
	m_List.DeleteItem( nSelIdx );
	m_ListToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE ,FALSE);

	RefreshData();
}

int CUtilitiesDialog::GetSelectedItem()
{
	if (!::IsWindow(m_List.GetSafeHwnd()))
		return -1;
	
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

void CUtilitiesDialog::OnOK() 
{
	if( m_btnSave.IsWindowEnabled() )
		OnBtnUtilitiesSave();
	CDialog::OnOK();
}

void CUtilitiesDialog::OnCancel() 
{
	if( m_btnSave.IsWindowEnabled() )
	{
		try
		{
			m_pUtilCostDB->loadDataSet( GetProjPath() );
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

void CUtilitiesDialog::RefreshTotalHrs()
{
	m_nTotalHrs = m_nLowHrs + m_nMedHrs + m_nHighHrs;
	UpdateData( false );
}


void CUtilitiesDialog::RefreshData()
{
	int nIndex = GetSelectedItem();
	if (nIndex < 0 || nIndex >= m_List.GetItemCount() )
	{
		SetData( NULL );
		return;
	}

	int nItemData = m_List.GetItemData( nIndex ) - ITEMDATA_STARTIDX;
	if( nItemData < 0 )
	{
		SetData( NULL );
		return;
	}

	std::vector<UtilityCostDataItem>& vData = m_pUtilCostDB->GetDataList();
	SetData( &vData[nItemData] );
}

void CUtilitiesDialog::SetData( UtilityCostDataItem* _pItem )
{
	if( _pItem == NULL )
	{
		EnableControl( false );
		return;
	}

	EnableControl( true );

	m_dFixCost = _pItem->m_dFixCostPerYear;
	m_dLowCost = _pItem->m_dLowCostPerHour;
	m_nLowHrs = _pItem->m_nLowHoursPerDay;
	m_dMedCost = _pItem->m_dMedCostPerHour;
	m_nMedHrs = _pItem->m_nMedHoursPerDay;
	m_dHighCost = _pItem->m_dHighCostPerHour;
	m_nHighHrs = _pItem->m_nHighHoursPerDay;
	m_nTotalHrs = m_nLowHrs + m_nMedHrs + m_nHighHrs;

	UpdateData( false );
}



void CUtilitiesDialog::ReloadDatabase()
{
	// Clean all the controls
	m_List.DeleteAllItems();
	
	assert( m_pUtilCostDB );

	std::vector<UtilityCostDataItem>& vData = m_pUtilCostDB->GetDataList();
	int nCount = vData.size();
	for( int i=0; i<nCount; i++ )
	{
		int nItem = m_List.InsertItem( i, vData[i].m_csLabel );
		m_List.SetItemData( nItem, i + ITEMDATA_STARTIDX );
	}
	
	RefreshData();
}


CEconomicManager* CUtilitiesDialog::GetEconomicManager()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
    return (CEconomicManager*)&pDoc->GetEconomicManager();
}


CString CUtilitiesDialog::GetProjPath()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	return pDoc->m_ProjInfo.path;
}



void CUtilitiesDialog::EnableControl( bool _bEnable )
{
	m_spinMed.EnableWindow( _bEnable );
	m_spinLow.EnableWindow( _bEnable );
	m_spinHigh.EnableWindow( _bEnable );
	m_editTotalHrs.EnableWindow( _bEnable );
	m_editMedHrs.EnableWindow( _bEnable );
	m_editMedCost.EnableWindow( _bEnable );
	m_editLowHrs.EnableWindow( _bEnable );
	m_editLowCost.EnableWindow( _bEnable );
	m_editHighHrs.EnableWindow( _bEnable );
	m_editHighCost.EnableWindow( _bEnable );
	m_editFixedCost.EnableWindow( _bEnable );
}

void CUtilitiesDialog::OnKillfocusEdit() 
{
	// TODO: Add your control notification handler code here
	UpdateData();

	// Set DB, 
	assert( m_pUtilCostDB );
	std::vector<UtilityCostDataItem>& vData = m_pUtilCostDB->GetDataList();

	int nSelIdx = GetSelectedItem();
	if( nSelIdx == -1 )
		return;

	int nItemData = m_List.GetItemData( nSelIdx ) - ITEMDATA_STARTIDX;
	vData[nItemData].m_dFixCostPerYear = m_dFixCost;
	vData[nItemData].m_dLowCostPerHour = m_dLowCost;
	vData[nItemData].m_dMedCostPerHour = m_dMedCost;
	vData[nItemData].m_dHighCostPerHour = m_dHighCost;
	vData[nItemData].m_nLowHoursPerDay = m_nLowHrs;
	vData[nItemData].m_nMedHoursPerDay = m_nMedHrs;
	vData[nItemData].m_nHighHoursPerDay = m_nHighHrs;
	
	RefreshTotalHrs();
	m_btnSave.EnableWindow();
}



void CUtilitiesDialog::OnDeltaposSpinUtilitiesHigh(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;

	// TODO: Add your control notification handler code here
	int	nLowerBound;
	int	nUpperBound;

	m_spinHigh.GetRange( nLowerBound, nUpperBound );
	if( pNMUpDown->iDelta > 0 )
	{
		m_nHighHrs = pNMUpDown->iPos + pNMUpDown->iDelta;
		m_nHighHrs = m_nHighHrs > nUpperBound ? nUpperBound : m_nHighHrs;
	}
	else if( pNMUpDown->iDelta < 0 && pNMUpDown->iPos > nLowerBound )
	{
		m_nHighHrs = pNMUpDown->iPos + pNMUpDown->iDelta;
		m_nHighHrs = m_nHighHrs < nLowerBound ? nLowerBound : m_nHighHrs;
	}	
	m_btnSave.EnableWindow();
	RefreshTotalHrs();
	*pResult = 0;
}

void CUtilitiesDialog::OnDeltaposSpinUtilitiesLow(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	int	nLowerBound;
	int	nUpperBound;

	m_spinLow.GetRange( nLowerBound, nUpperBound );
	if( pNMUpDown->iDelta > 0 )
	{
		m_nLowHrs = pNMUpDown->iPos + pNMUpDown->iDelta;
		m_nLowHrs = m_nLowHrs > nUpperBound ? nUpperBound : m_nLowHrs;
	}
	else if( pNMUpDown->iDelta < 0 && pNMUpDown->iPos > nLowerBound )
	{
		m_nLowHrs = pNMUpDown->iPos + pNMUpDown->iDelta;
		m_nLowHrs = m_nLowHrs < nLowerBound ? nLowerBound : m_nLowHrs;
	}	
	m_btnSave.EnableWindow();
	RefreshTotalHrs();
	*pResult = 0;
}

void CUtilitiesDialog::OnDeltaposSpinUtilitiesMed(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	// TODO: Add your control notification handler code here
	int	nLowerBound;
	int	nUpperBound;

	m_spinMed.GetRange( nLowerBound, nUpperBound );
	if( pNMUpDown->iDelta > 0 )
	{
		m_nMedHrs = pNMUpDown->iPos + pNMUpDown->iDelta;
		m_nMedHrs = m_nMedHrs > nUpperBound ? nUpperBound : m_nMedHrs;
	}
	else if( pNMUpDown->iDelta < 0 && pNMUpDown->iPos > nLowerBound )
	{
		m_nMedHrs = pNMUpDown->iPos + pNMUpDown->iDelta;
		m_nMedHrs = m_nMedHrs < nLowerBound ? nLowerBound : m_nMedHrs;
	}	
	m_btnSave.EnableWindow();
	RefreshTotalHrs();
	*pResult = 0;
}
