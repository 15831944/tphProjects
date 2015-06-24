// InterestOnCapitalDlg.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "InterestOnCapitalDlg.h"
#include "TermPlanDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ITEMDATA_STARTIDX 100

/////////////////////////////////////////////////////////////////////////////
// CInterestOnCapitalDlg dialog


CInterestOnCapitalDlg::CInterestOnCapitalDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInterestOnCapitalDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInterestOnCapitalDlg)
	m_strLabel = _T("");
	//}}AFX_DATA_INIT

	m_pInterestOnCapitalDB = GetEconomicManager()->m_pCaptIntrCost;

}


void CInterestOnCapitalDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInterestOnCapitalDlg)
	DDX_Control(pDX, IDC_LIST_INTERESTS, m_List);
	DDX_Control(pDX, IDC_STATIC_INTERESTTOOLBAR, m_staticToolBar);
	DDX_Control(pDX, IDC_EDIT_ECONOMIC, m_editLabel);
	DDX_Control(pDX, IDC_BUTTON_SAVE, m_btnSave);
	DDX_Text(pDX, IDC_EDIT_ECONOMIC, m_strLabel);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInterestOnCapitalDlg, CDialog)
	//{{AFX_MSG_MAP(CInterestOnCapitalDlg)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnBtnSave)
	ON_COMMAND(ID_PEOPLEMOVER_NEW, OnPeoplemoverNew)
	ON_COMMAND(ID_PEOPLEMOVER_DELETE, OnPeoplemoverDelete)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_INTERESTS, OnItemchangedListInterests)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_INTERESTS, OnEndlabeleditListInterests)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInterestOnCapitalDlg message handlers

BOOL CInterestOnCapitalDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	AfxGetApp()->BeginWaitCursor();

	InitToolbar();

	// Init list ctrl
	InitListCtrl();
	
	ReloadDatabase();

	m_btnSave.EnableWindow( FALSE );
	
	AfxGetApp()->EndWaitCursor();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CInterestOnCapitalDlg::InitToolbar()
{
	CRect rc;
	m_staticToolBar.GetWindowRect( &rc );
	ScreenToClient(&rc);
	m_ListToolBar.MoveWindow(&rc);
	m_ListToolBar.ShowWindow(SW_SHOW);
	m_staticToolBar.ShowWindow(SW_HIDE);
	m_ListToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_NEW ,TRUE);	
	m_ListToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE ,FALSE);	
	m_ListToolBar.GetToolBarCtrl().HideButton( ID_PEOPLEMOVER_CHANGE );
}



void CInterestOnCapitalDlg::InitListCtrl()
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
	int nFormat[] = { LVCFMT_CENTER, 
						LVCFMT_CENTER | LVCFMT_NUMBER,
						LVCFMT_CENTER | LVCFMT_NUMBER,
						LVCFMT_CENTER | LVCFMT_NUMBER 	};

	char* columnLabel[] = {	"Source of Capital", 
		"Interest Rate/Year(%)", 
		"Interest Cost/Year($)", 
		"Principal Remainning($)" };

	int DefaultColumnWidth[] = { 150, 120, 120, 120 };
	for( int i = 0; i < 4; i++ )
	{
		CStringList strList;
		lvc.csList = &strList;
		lvc.pszText = columnLabel[i];
		lvc.cx = DefaultColumnWidth[i];
		lvc.fmt = nFormat[i];
		m_List.InsertColumn( i, &lvc );
	}	
}

int CInterestOnCapitalDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	if (!m_ListToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_ListToolBar.LoadToolBar(IDR_PEOPLEMOVEBAR))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	
	return 0;
}



void CInterestOnCapitalDlg::ReloadDatabase()
{
	// Clean all the controls
	m_List.DeleteAllItems();
	
	assert( m_pInterestOnCapitalDB );

	std::vector<InterestOnCapitalDataItem>& vData = m_pInterestOnCapitalDB->GetDataList();
	int nCount = vData.size();
	for( int i=0; i<nCount; i++ )
	{
		int nItem = m_List.InsertItem( i, vData[i].m_csSourceCapital );

		CString csVal;
		csVal.Format( "%.2f", vData[i].m_dInterestRatePerYear );
		m_List.SetItemText( nItem, 1, csVal );
		csVal.Format( "%.2f", vData[i].m_dInterestCostPerYear );
		m_List.SetItemText( nItem, 2, csVal );
		csVal.Format( "%.2f", vData[i].m_dPrincipalRem );
		m_List.SetItemText( nItem, 3, csVal );
		
		m_List.SetItemData( nItem, i + ITEMDATA_STARTIDX );

	}
	RefreshEditLabel();
}

CEconomicManager* CInterestOnCapitalDlg::GetEconomicManager()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
    return (CEconomicManager*)&pDoc->GetEconomicManager();
}


CString CInterestOnCapitalDlg::GetProjPath()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	return pDoc->m_ProjInfo.path;
}


void CInterestOnCapitalDlg::OnBtnSave() 
{
	// TODO: Add your control notification handler code here
	assert( m_pInterestOnCapitalDB );
	m_pInterestOnCapitalDB->saveDataSet( GetProjPath(), true );
	m_btnSave.EnableWindow(FALSE);
}

void CInterestOnCapitalDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	if( m_btnSave.IsWindowEnabled() )
	{
		try
		{
			m_pInterestOnCapitalDB->loadDataSet( GetProjPath() );
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

void CInterestOnCapitalDlg::OnOK() 
{
	// TODO: Add extra validation here
	if( m_btnSave.IsWindowEnabled() )
	{
		m_pInterestOnCapitalDB->saveDataSet( GetProjPath(), true );
	}
	CDialog::OnOK();	
}

void CInterestOnCapitalDlg::OnPeoplemoverNew() 
{
	// TODO: Add your command handler code here
	m_List.EditNew();	
}

void CInterestOnCapitalDlg::OnPeoplemoverDelete() 
{
	// TODO: Add your command handler code here
	int nSelIdx = GetSelectedItem();
	if( nSelIdx == -1 )
		return;

	int nDBIdx = m_List.GetItemData( nSelIdx ) - ITEMDATA_STARTIDX;
	assert( m_pInterestOnCapitalDB );
	std::vector<InterestOnCapitalDataItem>& vData = m_pInterestOnCapitalDB->GetDataList();
	vData.erase( vData.begin() + nDBIdx );

	m_btnSave.EnableWindow();
	m_List.DeleteItem( nSelIdx );
	m_ListToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE ,FALSE);	
	RefreshEditLabel();
}


int CInterestOnCapitalDlg::GetSelectedItem()
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

void CInterestOnCapitalDlg::OnItemchangedListInterests(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	if (GetSelectedItem() >= 0)
		m_ListToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE ,TRUE);
	
	*pResult = 0;
}

void CInterestOnCapitalDlg::OnEndlabeleditListInterests(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here
	
	*pResult = 0;

	int nIdx = pDispInfo->item.iItem;

	int nItemData = m_List.GetItemData( nIdx );

	assert( m_pInterestOnCapitalDB );
	std::vector<InterestOnCapitalDataItem>& vData = m_pInterestOnCapitalDB->GetDataList();

	if( nItemData == 0 )
	{
		// new item
		InterestOnCapitalDataItem item;
		item.m_csSourceCapital = m_List.GetItemText( nIdx, 0 );
		item.m_dInterestRatePerYear = 0.0;
		item.m_dInterestCostPerYear = 0.0;
		item.m_dPrincipalRem = 0.0;

		vData.push_back( item );
		m_List.SetItemData( nIdx, vData.size() - 1 + ITEMDATA_STARTIDX );
	}
	else
	{
		nItemData -= ITEMDATA_STARTIDX;
		
		// modify the exist data.
		if( pDispInfo->item.iSubItem == 0 )
		{
			vData[nItemData].m_csSourceCapital = m_List.GetItemText( nIdx, 0 );
		}
		else
		{
			CString csStr = m_List.GetItemText( nIdx, pDispInfo->item.iSubItem );
			csStr.TrimLeft(); 
			csStr.TrimRight();
			switch( pDispInfo->item.iSubItem )
			{
			case 1:
				vData[nItemData].m_dInterestRatePerYear = atof( csStr );
				break;
			case 2:
				vData[nItemData].m_dInterestCostPerYear = atof( csStr );
				RefreshEditLabel();
				break;
			case 3:
				vData[nItemData].m_dPrincipalRem = atof( csStr );
				break;
			}
			
		}
	}

	m_btnSave.EnableWindow();

}


// refresh the sum of total value
void CInterestOnCapitalDlg::RefreshEditLabel()
{
	double dSum = 0;
	std::vector<InterestOnCapitalDataItem>& vData = m_pInterestOnCapitalDB->GetDataList();
	int nCount = vData.size();
	for( int i=0; i<nCount; i++ )
	{
		dSum += vData[i].m_dInterestCostPerYear;
	}
		
	m_strLabel.Format( "%.2f", dSum );
	UpdateData( false );
}
