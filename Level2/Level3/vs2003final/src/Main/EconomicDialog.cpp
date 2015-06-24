// EconomicDialog.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "economicdialog.h"
#include "..\common\template.h"
#include "termplandoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ITEMDATA_STARTIDX 100

/////////////////////////////////////////////////////////////////////////////
// CEconomicDialog dialog


CEconomicDialog::CEconomicDialog(CWnd* pParent, int _nEconomicFileType )
	: CDialog(CEconomicDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEconomicDialog)
	m_strLabel = _T("");
	//}}AFX_DATA_INIT

	m_nEconomicFileType = _nEconomicFileType;

	m_arColName.RemoveAll();
	m_pDB = NULL;
}


void CEconomicDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEconomicDialog)
	DDX_Control(pDX, IDC_BTN_ECONOMIC_SAVE, m_btnSave);
	DDX_Control(pDX, IDC_STATIC_ECONOMICTOOLBAR, m_listtoolbarcontenter);
	DDX_Control(pDX, IDC_LIST_ECONOMIC, m_List);
	DDX_Control(pDX, IDC_EDIT_ECONOMIC, m_editLabel);
	DDX_Text(pDX, IDC_STATIC_ECONOMIC, m_strLabel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEconomicDialog, CDialog)
	//{{AFX_MSG_MAP(CEconomicDialog)
	ON_WM_CREATE()
	ON_COMMAND(ID_PEOPLEMOVER_NEW, OnPeoplemoverNew)
	ON_COMMAND(ID_PEOPLEMOVER_DELETE, OnPeoplemoverDelete)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_ECONOMIC, OnEndlabeleditListEconomic)
	ON_BN_CLICKED(IDC_BTN_ECONOMIC_SAVE, OnBtnEconomicSave)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_ECONOMIC, OnItemchangedListEconomic)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEconomicDialog message handlers


BOOL CEconomicDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();


	CStringArray arColName;
				
	switch( m_nEconomicFileType )
	{
	case BuildingDeprecCost:
		SetWindowText("Building Depreciation Cost Factors");
		arColName.Add("Component");
		arColName.Add("Depreciation per year ($)");
		SetStaticLabel("Annual Depreciation (Bldg):");
		break;
	case InsuranceCost:
		SetWindowText("Fixed Insurance Costs");
		arColName.Add("Source");
		arColName.Add("Premiun/yr ($)");
		SetStaticLabel("Annual Premiums ($):");
		break;
	case ContractsCost:
		SetWindowText("Contracted Services Costs");
		arColName.Add("Supplier");
		arColName.Add("Value/yr ($)");
		SetStaticLabel("Annual Contracted Services :");					
		break;
	case ParkingLotsDeprecCost:
		SetWindowText("Parking Lots Depeciation Costs");
		arColName.Add("Component");
		arColName.Add("Amount/yr ($)");
		SetStaticLabel("Annual Depreciation ($):");
		break;
	case LandsideFacilitiesCost:
		SetWindowText("Landside Facilites Depreciation Costs");
		arColName.Add("Component");
		arColName.Add("Amount/yr ($)");
		SetStaticLabel("Annual Facilites Depreciation ($):");
		break;
	case AirsideFacilitiesCost:
		SetWindowText("Airside Facilites Depreciation Costs");
		arColName.Add("Component");
		arColName.Add("Amount/yr ($)");
		SetStaticLabel("Annual Facilites Depreciation ($):");			
		break;

	case LeasesRevenue:
		SetWindowText( "Fixed Revenues from Leases" );
		arColName.Add("Leases");
		arColName.Add("Amount/yr ($)");
		SetStaticLabel("Total Annual ($):");
		break;
	case AdvertisingRevenue:
		SetWindowText("Revenues Fixed from Advertising");
		arColName.Add("Advertiser");
		arColName.Add("Amount/yr ($)");
		SetStaticLabel("Total Revenues/yr ($):");
		break;
	case ContractsRevenue:
		SetWindowText("Fixed Revenues from Contracts");
		arColName.Add("Customer");
		arColName.Add("Value/yr ($)");
		SetStaticLabel("Total Annual Revenues ($):");
		break;
	}

	SetColNameArray(arColName);

	AfxGetApp()->BeginWaitCursor();

	// Init tool bar
	InitToolbar();

	// Init list ctrl
	InitListCtrl();
	
	GetDatabase();

	m_editLabel.SetReadOnly();
	m_editLabel.SetWindowText("0");

	ReloadDatabase();

	m_btnSave.EnableWindow( FALSE );
	
	AfxGetApp()->EndWaitCursor();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

int CEconomicDialog::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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

	InitListCtrl();
	
	return 0;
}

void CEconomicDialog::InitListCtrl()
{
	if (!::IsWindow(m_List.GetSafeHwnd()))
		return;

	// In all the Economic
	ASSERT(m_arColName.GetSize() == 2);

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

	for( int i = 0; i < 2 ; i++ )
	{
		CStringList strList;
		strList.AddTail( CString("OPEN") );
		strList.AddTail( CString("CLOSE") );
		lvc.csList = &strList;
		lvc.pszText = (LPSTR) ((LPCTSTR) m_arColName.GetAt(i));
		lvc.cx = (i== 0 ? w1:w);
		lvc.fmt = nFormat[i];
		m_List.InsertColumn( i, &lvc );
	}
}

void CEconomicDialog::InitToolbar()
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

void CEconomicDialog::SetColNameArray(const CStringArray& arColName)
{
	m_arColName.RemoveAll();
	
	for (int i = 0; i < arColName.GetSize(); i++)
		m_arColName.Add(arColName.GetAt(i));
}


void CEconomicDialog::OnPeoplemoverNew() 
{
	m_List.EditNew();
}

void CEconomicDialog::OnPeoplemoverDelete()
{
	int nSelIdx = GetSelectedItem();
	if( nSelIdx == -1 )
		return;

	int nDBIdx = m_List.GetItemData( nSelIdx ) - ITEMDATA_STARTIDX;
	assert( m_pDB );
	std::vector<FixedCostRevDataItem>& vData = m_pDB->GetDataList();
	vData.erase( vData.begin() + nDBIdx );
	
	m_btnSave.EnableWindow();
	m_List.DeleteItem( nSelIdx );
	m_ListToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE ,FALSE);

	RefreshEditLabel();
}

void CEconomicDialog::OnEndlabeleditListEconomic(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	*pResult = 0;

	int nIdx = pDispInfo->item.iItem;

	int nItemData = m_List.GetItemData( nIdx );

	assert( m_pDB );
	std::vector<FixedCostRevDataItem>& vData = m_pDB->GetDataList();
	CString csLabel = m_List.GetItemText( nIdx, 0 );
	csLabel.TrimLeft();
	csLabel.TrimRight();
	if( csLabel.IsEmpty() )
	{
		csLabel = "New Item";
		m_List.SetItemText( nIdx, 0, csLabel );
	}

	if( nItemData == 0 )
	{
		// new item
		FixedCostRevDataItem item;
		item.m_csLabel = csLabel;
		item.m_dVal = 0.0;
		vData.push_back( item );
		m_List.SetItemData( nIdx, vData.size() - 1 + ITEMDATA_STARTIDX );
		m_List.SetItemText( nIdx, 1, "0" );
	}
	else
	{
		nItemData -= ITEMDATA_STARTIDX;
		
		// modify the exist data.
		if( pDispInfo->item.iSubItem == 0 )
		{
			vData[nItemData].m_csLabel = csLabel;
		}
		else
		{
			CString csStr = m_List.GetItemText( nIdx, 1 );
			csStr.TrimLeft(); 
			csStr.TrimRight();
			vData[nItemData].m_dVal = atof( csStr );
			// modify the val
			RefreshEditLabel();
		}
	}

	m_btnSave.EnableWindow();
}

void CEconomicDialog::OnBtnEconomicSave() 
{
	m_btnSave.EnableWindow(FALSE);
	assert( m_pDB );
	CWaitCursor wc;
	m_pDB->saveDataSet( GetProjPath(), true );
}

int CEconomicDialog::GetSelectedItem()
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

void CEconomicDialog::OnItemchangedListEconomic(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	if (GetSelectedItem() >= 0)
		m_ListToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE ,TRUE);

	*pResult = 0;
}

void CEconomicDialog::RefreshEditLabel()
{
	double dSum = 0;
	std::vector<FixedCostRevDataItem>& vData = m_pDB->GetDataList();
	int nCount = vData.size();
	for( int i=0; i<nCount; i++ )
	{
		dSum += vData[i].m_dVal;
	}
		
	CString csSum;
	csSum.Format( "%.2f", dSum );
	m_editLabel.SetWindowText( csSum );
}

void CEconomicDialog::OnOK() 
{
	if( m_btnSave.IsWindowEnabled() )
	{
		assert( m_pDB );
		CWaitCursor wc;
		m_pDB->saveDataSet( GetProjPath(), true );
	}
	CDialog::OnOK();
}

void CEconomicDialog::OnCancel() 
{
	if (m_btnSave.IsWindowEnabled())
	{
		try
		{
			m_pDB->loadDataSet( GetProjPath() );
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

void CEconomicDialog::GetDatabase()
{
	m_pDB = NULL;

	switch( m_nEconomicFileType )
	{
	case BuildingDeprecCost:
		m_pDB = GetEconomicManager()->m_pBuildingDeprcCost;
		break;
	case InsuranceCost:
		m_pDB = GetEconomicManager()->m_pInsuranceCost;
		break;
	case ContractsCost:
		m_pDB = GetEconomicManager()->m_pContractsCost;
		break;
	case ParkingLotsDeprecCost:
		m_pDB = GetEconomicManager()->m_pParkingLotsDeprecCost;
		break;
	case LandsideFacilitiesCost:
		m_pDB = GetEconomicManager()->m_pLandsideFacilitiesCost;
		break;
	case AirsideFacilitiesCost:
		m_pDB = GetEconomicManager()->m_pAirsideFacilitiesCost;
		break;

	case LeasesRevenue:
		m_pDB = GetEconomicManager()->m_pLeasesRevenue;
		break;

	case AdvertisingRevenue:
		m_pDB = GetEconomicManager()->m_pAdvertisingRevenue;
		break;
	
	case ContractsRevenue:
		m_pDB = GetEconomicManager()->m_pContractsRevenue;
		break;
	}
}

void CEconomicDialog::ReloadDatabase()
{
	m_List.DeleteAllItems();
	if( !m_pDB )
		return;

	std::vector<FixedCostRevDataItem>& vData = m_pDB->GetDataList();
	int nCount = vData.size();
	for( int i=0; i<nCount; i++ )
	{
		int nItem = m_List.InsertItem( i, vData[i].m_csLabel );
		CString csVal;
		csVal.Format( "%.2f", vData[i].m_dVal );
		m_List.SetItemText( nItem, 1, csVal );
		m_List.SetItemData( nItem, i + ITEMDATA_STARTIDX );
	}
	
	RefreshEditLabel();
}


CEconomicManager* CEconomicDialog::GetEconomicManager()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
    return (CEconomicManager*)&pDoc->GetEconomicManager();
}


void CEconomicDialog::SelectList( int _nIdx )
{
	int nCount = m_List.GetItemCount();
	for( int i=0; i<nCount; i++ )
	{
		if( i == _nIdx )
			m_List.SetItemState( i, LVIS_SELECTED, LVIS_SELECTED );
		else
			m_List.SetItemState( i, 0, LVIS_SELECTED );
	}	
}


CString CEconomicDialog::GetProjPath()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	return pDoc->m_ProjInfo.path;
}

