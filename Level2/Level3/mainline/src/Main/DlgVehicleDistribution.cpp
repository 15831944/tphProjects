#include "stdafx.h"
#include "termplan.h"
#include ".\dlgvehicledistribution.h"
#include "TermPlanDoc.h"
#include "FlightDialog.h"
#include "Landside/LandsideVehicleTypeList.h"
#include "Landside/InputLandside.h"
#include "DlgSelectLandsideVehicleType.h"

#include "common\WinMsg.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgVehicleDistribution dialog


CDlgVehicleDistribution::CDlgVehicleDistribution(CWnd* pParent /*=NULL*/)
: CDialog(CDlgVehicleDistribution::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgVehicleDistribution)
 	m_bSized = false;
	//}}AFX_DATA_INIT

	m_nListPreIndex = -1;
	m_nAllBranchesFlag=-1;
	m_pCopyDistData = NULL;
	m_pVeicleDistList = GetInputLandside()->getVehicleDistributionList();
	m_pVehicleTypeList = GetInputLandside()->getLandsideVehicleTypeList();
	m_pVehicleTypeList->ReadData(-1);
	m_treeDist.setInputTerminal( GetInputTerminal() );
}


CDlgVehicleDistribution::~CDlgVehicleDistribution()
{
	
}

void CDlgVehicleDistribution::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgVehicleDistribution)
	DDX_Control(pDX, IDC_STATIC_FRAME3, m_stcFrame2);
	DDX_Control(pDX, IDC_STATIC_TOOLBARCONTENTER2, m_toolbarcontenter2);
	DDX_Control(pDX, IDC_STATIC_FRAME, m_stcFrame);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDC_STATIC_TOOLBARCONTENTER, m_toolbarcontenter);
	DDX_Control(pDX, IDC_BUTTON_SAVE, m_btnSave);
	DDX_Control(pDX, IDC_TREE_DIST, m_treeDist);
	DDX_Control(pDX, IDC_LIST_FLIGHT, m_listboxFlight);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgVehicleDistribution, CDialog)
//{{AFX_MSG_MAP(CDlgVehicleDistribution)
	ON_WM_CREATE()
	ON_LBN_SELCHANGE(IDC_LIST_FLIGHT, OnSelchangeListVehicle)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_PAXDIST_MODIFYPERCENT, OnPaxdistModifypercent)
	ON_COMMAND(ID_PAXDIST_EVENPERCENT, OnPaxdistEvenpercent)
	ON_COMMAND(ID_PAXDIST_EVENPERCENT_REMAIN, OnPaxdistEvenRemainPercent)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	ON_BN_CLICKED(ID_PAXDISTBUTTON, OnPrintDist)
	ON_COMMAND(ID_PEOPLEMOVER_NEW, OnPeoplemoverNew)
	ON_COMMAND(ID_PEOPLEMOVER_DELETE, OnPeoplemoverDelete)
	ON_WM_SIZE()
	ON_WM_CANCELMODE()
	ON_MESSAGE(WM_INPLACE_SPIN,OnEndEditPercent)
	ON_COMMAND_RANGE(ID_PAXDIST_MODIFYPERCENT_ALLBRANCHES,ID_PAXDIST_EVENPERCENT_REMAIN_ALLBRANCHES,OnAllBranchesPercentEdit)
	ON_NOTIFY(TVN_ITEMEXPANDING,IDC_TREE_PAX, OnTvnItemexpanding)
	ON_BN_CLICKED(IDC_BUTTON_COPY,OnBtnCopy)
	ON_BN_CLICKED(IDC_BUTTON_PASTE,OnBtnPaste)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgVehicleDistribution message handlers

BOOL CDlgVehicleDistribution::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_toolTips.InitTooltips( &m_listboxFlight );
	m_wndSplitter.Create(WS_CHILD | WS_VISIBLE, CRect(0,0,0,0), this, IDC_STATIC_SPLITTER);
	m_wndSplitter.SetStyle(SPS_VERTICAL);

	// TODO: Add extra initialization here
	InitToolbar();
	ReloadVehicleList();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

#define CONSTRAINT_LEN 128
void CDlgVehicleDistribution::ReloadVehicleList(const char* p_str )
{
	m_listboxFlight.ResetContent();
	int nCount = m_pVeicleDistList->getCount();
    for( int i = 0; i < nCount; i++ )
    {
		//char str[CONSTRAINT_LEN];
		CString str;

		VehicleTypeDistribution* pVehicleDist = m_pVeicleDistList->getItem(i);
        str = pVehicleDist->GetVehicleType().toString();
		m_listboxFlight.AddString( str ); 

		// add by bird 2003/8/12
		// make item selected
		//if( p_str && strcmp( str, p_str ) == 0 )
		if(p_str && str == CString(p_str))
			m_listboxFlight.SetCurSel( i );
    }
	if( p_str == NULL )
		m_listboxFlight.SetCurSel(-1);
	OnSelchangeListVehicle();	
}



InputLandside* CDlgVehicleDistribution::GetInputLandside()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
    return pDoc->GetInputLandside();
}

InputTerminal* CDlgVehicleDistribution::GetInputTerminal()
{
	CTermPlanDoc* pDoc = (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();
	return (InputTerminal*)(&pDoc->GetTerminal());
}

void CDlgVehicleDistribution::OnSelchangeListVehicle() 
{
	// TODO: Add your control notification handler code here
	int nFltIdx = m_listboxFlight.GetCurSel();
	if( nFltIdx == LB_ERR )
	{
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE,FALSE );
		m_TreeToolBar.GetToolBarCtrl().EnableButton(ID_PAXDISTBUTTON, FALSE);
		GetDlgItem(IDC_BUTTON_COPY)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_PASTE)->EnableWindow(FALSE);
	}
	//	m_btnDel.EnableWindow( false );
	else
	{
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE );
		m_TreeToolBar.GetToolBarCtrl().EnableButton(ID_PAXDISTBUTTON, TRUE);
		GetDlgItem(IDC_BUTTON_COPY)->EnableWindow(TRUE);
		if (m_pCopyDistData)
		{
			GetDlgItem(IDC_BUTTON_PASTE)->EnableWindow(TRUE);
		}
		else
		{
			GetDlgItem(IDC_BUTTON_PASTE)->EnableWindow(FALSE);
		}
	}
	//	m_btnDel.EnableWindow( true );
	ShowTips( nFltIdx );
	ReloadTree();	
}



CString CDlgVehicleDistribution::GetItemLabel( const CString& _csName,int _nPercent )
{
	CString str;
	str.Format( "%s (%d%%)", _csName, _nPercent );
	return str;
}


void CDlgVehicleDistribution::ReloadTree()
{
	m_treeDist.DeleteAllItems();
	
	VehicleTypeDistribution* pVehicleDist = GetPaxDist();
	if( !pVehicleDist )
		return;
	
	int IdxPath[LEVELS];
	InsertPaxDistItem( pVehicleDist, TVI_ROOT, 0, IdxPath );
}


void CDlgVehicleDistribution::InsertPaxDistItem( const VehicleTypeDistribution* _pVehicleDist, HTREEITEM _hParent, int _nIdxLevel, int* _pIdxPath )
{
	if( _nIdxLevel >= LEVELS )
		return;
	
	LandsideVehicleType* pVehicleType = m_pVehicleTypeList->GetItem(_nIdxLevel);
	if (pVehicleType == NULL)
		return;
	int nBranch = pVehicleType->GetLeafCount();
	for( int i=0; i<nBranch; i++ )
	{
		
		_pIdxPath[_nIdxLevel] = i;
		for( int j=_nIdxLevel+1; j<LEVELS; j++ )
			_pIdxPath[j] = -1; 
		
		CString str = GetItemLabel( pVehicleType->GetLeaf(i),
			static_cast<int>(_pVehicleDist->getValue( _pIdxPath )) );
		
		HTREEITEM hItem = m_treeDist.InsertItem( str, _hParent );
		m_treeDist.SetItemData( hItem, i );
		if (_nIdxLevel >= LEVELS - 1)
			continue;

		int nNextLevel = _nIdxLevel + 1;
		if (nNextLevel < 0 || nNextLevel >= m_pVehicleTypeList->GetItemCount())
			continue;

		int nChildCount = m_pVehicleTypeList->GetItem( _nIdxLevel + 1 )->GetLeafCount();
		// add a dummy child if the node has children
		if(nChildCount != 0)
			m_treeDist.InsertItem("error",hItem);
	}
}





void CDlgVehicleDistribution::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: Add your message handler code here
	m_hRClickItem = NULL;
	
	CRect rectTree;
	m_treeDist.GetWindowRect( &rectTree );
	if( !rectTree.PtInRect(point) ) 
		return;
	
	m_treeDist.SetFocus();
	CPoint pt = point;
	m_treeDist.ScreenToClient( &pt );
	UINT iRet;
	m_hRClickItem = m_treeDist.HitTest( pt, &iRet );
	if (iRet != TVHT_ONITEMLABEL)   // Must click on the label
	{
		m_hRClickItem = NULL;
	}
	
	CMenu menuPopup;
	menuPopup.LoadMenu( IDR_MENU_POPUP );
	CMenu* pMenu = menuPopup.GetSubMenu( 1 );
	if( m_hRClickItem )
	{
		m_treeDist.SelectItem( m_hRClickItem );   // Select it
		pMenu->TrackPopupMenu(TPM_LEFTALIGN, point.x, point.y, this);	
	}
	else
	{
		m_treeDist.SelectItem( NULL );   // Select it
	}	
}

void CDlgVehicleDistribution::OnPaxdistModifypercent() 
{
	// TODO: Add your command handler code here
	if (!m_hRClickItem )
		return;
	
	// Show edit box
	m_treeDist.SpinEditLabel(m_hRClickItem);
	m_btnSave.EnableWindow();	
}

void CDlgVehicleDistribution::OnPaxdistEvenRemainPercent()
{
	// TODO: Add your command handler code here
	if (!m_hRClickItem )
		return;
	
	// SET DATABASE
	VehicleTypeDistribution* pVehicleDist = GetPaxDist();
	if( !pVehicleDist )
		return;
	
	int nLevels = GetLevelIdx( m_hRClickItem );
	
	HTREEITEM hParentItem = m_treeDist.GetParentItem( m_hRClickItem );
	LandsideVehicleType* pVehicleType = m_pVehicleTypeList->GetItem(nLevels);
	int nChildCount =pVehicleType->GetLeafCount();
	
	HTREEITEM hChildItem;
	float fValue;
	for(int i = 0; i<nChildCount; i++)
	{
		if( i == 0 )
			hChildItem = m_treeDist.GetChildItem( hParentItem );
		else
			hChildItem = m_treeDist.GetNextItem( hChildItem, TVGN_NEXT );
	
		if(hChildItem == m_hRClickItem)			//not set percent for the current selected item
		{
			int pIdxPath[LEVELS];
			GetIdxPath(hChildItem, pIdxPath);
			fValue = pVehicleDist->getValue(pIdxPath);
			break;
		}
	}

	float fRemain = 100 - fValue;
	float fPercent, fLack = 0;
	if((int)fRemain % (nChildCount - 1) == 0 )
		fPercent = fRemain / (nChildCount - 1);
	else
	{
		fPercent = static_cast<float>((fRemain / (nChildCount - 1)));
		fLack = fRemain - fPercent * (nChildCount - 1);
	}
	

	for( int j=0; j<nChildCount; j++ )
	{
		if( j == 0 )
			hChildItem = m_treeDist.GetChildItem( hParentItem );
		else
			hChildItem = m_treeDist.GetNextItem( hChildItem, TVGN_NEXT );
	
		if(hChildItem == m_hRClickItem)			//not set percent for the current selected item
			continue;

		float fPercentUse = fPercent;
		if(fLack != 0)
		{
			fLack--;
			fPercentUse = fPercent + 1;
			
		}
		
		int pIdxPath[LEVELS];
		GetIdxPath( hChildItem, pIdxPath );
		
		pVehicleDist->setValue( pIdxPath, fPercentUse );	
		
		CString csLabel = GetItemLabel( pVehicleType->GetLeaf( j ), static_cast<int>(fPercentUse) );
		m_treeDist.SetItemText( hChildItem, csLabel );
	}
	
	m_btnSave.EnableWindow();	
}

void CDlgVehicleDistribution::OnPaxdistEvenpercent() 
{
	// TODO: Add your command handler code here
	if (!m_hRClickItem )
		return;
	
	// SET DATABASE
	VehicleTypeDistribution* pVehicleDist = GetPaxDist();
	if( !pVehicleDist )
		return;
	
	int nLevels = GetLevelIdx( m_hRClickItem );
	
	HTREEITEM hParentItem = m_treeDist.GetParentItem( m_hRClickItem );
	
	LandsideVehicleType* pVehicleType = m_pVehicleTypeList->GetItem(nLevels);
	int nChildCount =pVehicleType->GetLeafCount();
	
	HTREEITEM hChildItem;
	for( int i=0; i<nChildCount; i++ )
	{
		if( i == 0 )
			hChildItem = m_treeDist.GetChildItem( hParentItem );
		else
			hChildItem = m_treeDist.GetNextItem( hChildItem, TVGN_NEXT );
		
		int pIdxPath[LEVELS];
		GetIdxPath( hChildItem, pIdxPath );
		
		float fPercent;
		if( nChildCount == 3 )
		{
			if( i == 2 )
				fPercent = 34;
			else
				fPercent = 33;
		}
		else
			fPercent = static_cast<float>(100 / nChildCount);
		pVehicleDist->setValue( pIdxPath, fPercent );	
		
		CString csLabel = GetItemLabel( pVehicleType->GetLeaf(i), static_cast<int>(fPercent) );
		m_treeDist.SetItemText( hChildItem, csLabel );
	}
	
	m_btnSave.EnableWindow();	
}



LRESULT CDlgVehicleDistribution::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if (message == WM_INPLACE_SPIN)
	{
		LPSPINTEXT pst = (LPSPINTEXT) lParam;
		
		SetPercent( pst->iPercent );
		return TRUE;
	}

	if (message == WM_NOTIFY)
	{
		if (wParam == IDC_STATIC_SPLITTER )
		{	
			SPC_NMHDR* pHdr = (SPC_NMHDR*) lParam;
			DoResize(pHdr->delta);
		}

	}	
	
	return CDialog::DefWindowProc(message, wParam, lParam);
}



void CDlgVehicleDistribution::SetPercent(int _nPercent)
{
	// TODO: Add your command handler code here
	if( !m_hRClickItem )
		return;
	
	VehicleTypeDistribution* pVehicleDist = GetPaxDist();
	if( !pVehicleDist )
		return;
	
	int pIdxPath[LEVELS];
	GetIdxPath( m_hRClickItem, pIdxPath );
	
	pVehicleDist->setValue( pIdxPath, static_cast<float>(_nPercent) );	
	
	CheckPercentFull( m_treeDist.GetParentItem(m_hRClickItem) );
	ReloadTree();
}


// check if the sum of child item reach 100%.
// if not display red.
void CDlgVehicleDistribution::CheckPercentFull( HTREEITEM _hItem )
{
	VehicleTypeDistribution* pVehicleDist = GetPaxDist();
	if( !pVehicleDist )
		return;
	
	int nTotalPercent = 0;
	HTREEITEM hChildItem = m_treeDist.GetChildItem( _hItem ? _hItem : TVI_ROOT );		
	while( hChildItem )
	{
		int pIdxPath[LEVELS];
		GetIdxPath( m_hRClickItem, pIdxPath );
		nTotalPercent += static_cast<int>(pVehicleDist->getValue( pIdxPath ));
		hChildItem = m_treeDist.GetNextItem( hChildItem, TVGN_NEXT );
	}
	if( nTotalPercent != 100 )
	{
		SetChildRed( _hItem );
	}
}

void CDlgVehicleDistribution::SetChildRed( HTREEITEM _hItem )
{
/*
HTREEITEM hChildItem = m_treeDist.GetChildItem( _hItem ? _hItem : TVI_ROOT );		
while( hChildItem )
{
m_treeDist.SetItemState( hChildItem, TVIS_BOLD, TVIS_BOLD );
hChildItem = m_treeDist.GetNextItem( hChildItem, TVGN_NEXT );
}*/
}


void CDlgVehicleDistribution::GetIdxPath( HTREEITEM _hItem, int* _pIdxPath )
{
	std::vector<int> vectPath;
	HTREEITEM hItem = _hItem;
	while( hItem )
	{
		vectPath.push_back( m_treeDist.GetItemData( hItem ));
		hItem = m_treeDist.GetParentItem( hItem );
	}
	int nLevel = vectPath.size();
	for( int i=0; i<nLevel; i++ )
		_pIdxPath[i] = vectPath[nLevel-i-1];
	for( i=nLevel; i<LEVELS; i++ )
		_pIdxPath[i] = -1;
}


int CDlgVehicleDistribution::GetLevelIdx( HTREEITEM _hItem )
{
	HTREEITEM hItem = _hItem;
	int nLevel = 0;
	do
	{
		hItem = m_treeDist.GetParentItem( hItem );
		if( hItem )
			nLevel++;
	}while( hItem );
	
	return nLevel;
}


VehicleTypeDistribution* CDlgVehicleDistribution::GetPaxDist()
{
	int nFltIdx = m_listboxFlight.GetCurSel();
	if( nFltIdx == LB_ERR )
		return NULL;
	
	return (VehicleTypeDistribution*)m_pVeicleDistList->getItem( nFltIdx );
}

void CDlgVehicleDistribution::OnCancel() 
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
		m_pVeicleDistList->loadDataSet( GetProjPath() );	
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

void CDlgVehicleDistribution::OnOK() 
{
	// TODO: Add extra validation here
	if( !m_btnSave.IsWindowEnabled() )  // nonzero if the window was previously disabled
	{
		CDialog::OnOK();
		return;
	}

	VehicleTypeDistribution* pVehicleDist = GetPaxDist();
	if( !pVehicleDist->checkTotals())
	{
		MessageBox( "All branches must total 100%", "Error", MB_ICONWARNING);
		return;
	}
	
	AfxGetApp()->BeginWaitCursor();
	OnButtonSave();	
	AfxGetApp()->EndWaitCursor();

	CDialog::OnOK();
}

void CDlgVehicleDistribution::OnButtonSave() 
{
	// TODO: Add your control notification handler code here
	VehicleTypeDistribution* pVehicleDist = GetPaxDist();
	if( pVehicleDist && !pVehicleDist->checkTotals() )
    {
        MessageBox( "All branches must total 100%", "Error", MB_ICONWARNING );
        return;
    }
	
	m_pVeicleDistList->saveDataSet( GetProjPath(), true );
	m_btnSave.EnableWindow( false );
	
}


void CDlgVehicleDistribution::InitToolbar()
{
	CRect rc,newrc;
	m_toolbarcontenter.GetWindowRect( &rc );
	ScreenToClient(&rc);
	m_listboxFlight.GetWindowRect(&newrc);
	ScreenToClient(&newrc);
	rc.right=rc.left+newrc.Width()/2;
	m_ToolBar.MoveWindow(&rc);
	m_ToolBar.ShowWindow(SW_SHOW);
	m_toolbarcontenter.ShowWindow(SW_HIDE);
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_NEW );
	m_ToolBar.GetToolBarCtrl().HideButton( ID_PEOPLEMOVER_CHANGE );
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE,FALSE );

	


	
	m_toolbarcontenter2.GetWindowRect(&rc);
	ScreenToClient(&rc);
	m_TreeToolBar.MoveWindow(&rc);
	m_TreeToolBar.ShowWindow(SW_SHOW);
	m_toolbarcontenter2.ShowWindow(SW_HIDE);
	m_TreeToolBar.GetToolBarCtrl().EnableButton(ID_PAXDISTBUTTON, FALSE);
	
}

int CDlgVehicleDistribution::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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

	if (!m_TreeToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_TreeToolBar.LoadToolBar(IDR_PAXDIST))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	return 0;
}


void CDlgVehicleDistribution::OnPeoplemoverNew() 
{
	// TODO: Add your command handler code here
	CDlgSelectLandsideVehicleType dlg;
	CString strVehicleType;
	if(dlg.DoModal() == IDOK)
	{
		strVehicleType = dlg.GetName();
		CHierachyName vehicleName;
		vehicleName.fromString(strVehicleType);
		
		// find if exist
		int nCount = m_listboxFlight.GetCount();
		for( int i=0; i<nCount; i++ )
		{
			VehicleTypeDistribution  *pDist = m_pVeicleDistList->getItem(i);
			
			if( pDist->GetVehicleType() == vehicleName)
			{
				// select the item
				m_listboxFlight.SetCurSel( i );
				// reload tree
				OnSelchangeListVehicle();
				return;
			}
		}
		if( i == nCount )
		{
			// add into database.
			VehicleTypeDistribution *pDist = new VehicleTypeDistribution( m_pVehicleTypeList );
			pDist->SetVehicleType(vehicleName);
			m_pVeicleDistList->addItem(pDist);
			ReloadVehicleList( strVehicleType );
		}

		m_btnSave.EnableWindow( TRUE );
	}

}

void CDlgVehicleDistribution::OnPeoplemoverDelete() 
{
	// TODO: Add your command handler code here
		int nFltIdx = m_listboxFlight.GetCurSel();
	if( nFltIdx == -1 )
		return;
	
	m_pVeicleDistList->deleteItem( nFltIdx );
	ReloadVehicleList();
	
	m_btnSave.EnableWindow( TRUE );
}

void CDlgVehicleDistribution::OnSize(UINT nType, int cx, int cy) 
{
	
	
// 	if(!m_bSized)
// 	{
// 		m_bSized = true;
// 		return;
// 	}

	
	// TODO: Add your message handler code here
	CRect  newrc,rc;
	int nMargin=10;

	if(!m_bSized)
	{
		newrc.left = 233;
		newrc.top = 55;
		newrc.bottom = cy-40;
		newrc.right =  cx - 7;
		m_treeDist.MoveWindow( &newrc );
	}else
	{
		m_treeDist.GetWindowRect(&newrc);
		ScreenToClient(&newrc);
		newrc.bottom = cy-40;
		newrc.right =  cx - 7;
		newrc.top = 55;
// 		newrc.left = newrc.right-rc.Width();		
		m_treeDist.MoveWindow( &newrc );
	}


	m_treeDist.GetWindowRect(&newrc);
	ScreenToClient(&newrc);

	GetDlgItem(IDC_STATIC_PAXDIST)->GetWindowRect(&rc);
	ScreenToClient(&rc);
	int width=rc.right-rc.left;
	rc.left=newrc.left;
	rc.right=rc.left+width;
	GetDlgItem(IDC_STATIC_PAXDIST)->MoveWindow(&rc);

	m_toolbarcontenter2.GetWindowRect(&rc);
	ScreenToClient(&rc);
	width=rc.right-rc.left;
	rc.left=newrc.left;
	rc.right=rc.left+width;
	m_toolbarcontenter2.MoveWindow(&rc);
	m_TreeToolBar.MoveWindow(&rc);


	//cancel-->ok-->save-->paste-->copy
	CRect btnRect;
	GetDlgItem(IDC_BUTTON_SAVE)->GetWindowRect(&btnRect);
	int nBtnWidth = btnRect.Width();

	newrc.right = newrc.right;
	newrc.left = newrc.right - nBtnWidth;
	newrc.top = newrc.bottom + 10;
	newrc.bottom = newrc.top + 25;
	m_btnCancel.MoveWindow(&newrc);

	newrc.right = newrc.left - 15;
	newrc.left = newrc.right - nBtnWidth;
	m_btnOK.MoveWindow(&newrc);

	newrc.right = newrc.left - 15;
	newrc.left = newrc.right - nBtnWidth;
	m_btnSave.MoveWindow(&newrc);

	newrc.right = newrc.left - 15;
	newrc.left = newrc.right - nBtnWidth;
	GetDlgItem(IDC_BUTTON_PASTE)->MoveWindow(&newrc);

	newrc.right = newrc.left - 15;
	newrc.left = newrc.right - nBtnWidth;
	GetDlgItem(IDC_BUTTON_COPY)->MoveWindow(&newrc);

	m_treeDist.GetWindowRect(&newrc);
	ScreenToClient(&newrc);
	m_stcFrame.GetWindowRect(&rc);
	ScreenToClient(&rc);
	rc.bottom = newrc.bottom + 3;
	rc.right=newrc.left-nMargin+1;
	m_stcFrame.MoveWindow(&rc);

	m_stcFrame2.GetWindowRect(&rc);
	ScreenToClient(&rc);
	rc.bottom = newrc.bottom + 3;
	rc.right = newrc.right + 1;
	rc.left = newrc.left - 1;
	m_stcFrame2.MoveWindow(&rc);


	m_listboxFlight.GetWindowRect(&rc);
	ScreenToClient(&rc);
	rc.bottom = newrc.bottom;
	rc.right = newrc.left-nMargin;
	//rc.left=10;
	m_listboxFlight.MoveWindow(&rc);

	m_treeDist.GetWindowRect(&newrc);
	ScreenToClient(&newrc);


	m_wndSplitter.SetWindowPos(NULL,rc.right ,rc.top,nMargin,rc.Height(),NULL);
	m_wndSplitter.SetRange(newrc.left-nMargin-rc.Width()+nMargin/2,newrc.left+newrc.Width()-nMargin/2);

	m_bSized=true;

	InvalidateRect(NULL);

	CDialog::OnSize(nType, cx, cy);
// 	OnPaint();
}

void CDlgVehicleDistribution::OnCancelMode() 
{
	CDialog::OnCancelMode();
	
	// TODO: Add your message handler code here
}

void CDlgVehicleDistribution::OnPrintDist()
{
	m_treeDist.PrintTree("Tree header","Tree footer");
}

BOOL CDlgVehicleDistribution::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	m_toolTips.RelayEvent( pMsg );

	return CDialog::PreTranslateMessage(pMsg);
}

void CDlgVehicleDistribution::ShowTips(int iCurSel)
{
	CString strTips;

	if( iCurSel == LB_ERR)
	{
		m_toolTips.DelTool( &m_listboxFlight );
		return;
	}

	VehicleTypeDistribution* pDist = m_pVeicleDistList->getItem(iCurSel);	
	strTips = pDist->GetVehicleType().toString();
	m_toolTips.ShowTips( strTips );
}

LRESULT CDlgVehicleDistribution::OnEndEditPercent( WPARAM p_wParam, LPARAM p_lParam )
{
	SPINTEXT* pStData=(SPINTEXT*)p_lParam;
	switch(m_nAllBranchesFlag)
	{
	case 0:
		ModifyPercentAllBranch(pStData->iPercent);
		break;
	default:
		{
			int pIdxPath[LEVELS];
			GetIdxPath( m_hRClickItem, pIdxPath );
			GetPaxDist()->setValue(pIdxPath, static_cast<float>(pStData->iPercent));

			//get the sequence of selected item.
			int nSelectedItem=0;
			HTREEITEM hParentItem=m_treeDist.GetParentItem(m_hRClickItem);
			HTREEITEM hChildItem=m_treeDist.GetChildItem(hParentItem);
			while(hChildItem!=NULL)
			{
				if(hChildItem==m_hRClickItem)break;
				hChildItem=m_treeDist.GetNextItem(hChildItem, TVGN_NEXT);
				nSelectedItem++;
			}
			//get level
			int nLevels = GetLevelIdx( m_hRClickItem );
			LandsideVehicleType* pVehicleType = m_pVehicleTypeList->GetItem(nLevels);
			//generate label.
			CString csLabel = GetItemLabel( pVehicleType->GetLeaf( nSelectedItem ) , pStData->iPercent);
			m_treeDist.SetItemText( m_hRClickItem, csLabel );
		}
		break;
	}
	//reset m_nAllBranchesFlag .
	m_nAllBranchesFlag=-1;
	return 0L;
}
void CDlgVehicleDistribution::OnAllBranchesPercentEdit(UINT nID)
{
	m_nAllBranchesFlag = nID-ID_PAXDIST_MODIFYPERCENT_ALLBRANCHES;
	switch(m_nAllBranchesFlag)
	{
	case 0:
		//automaticly update data by OnEndEditPercent(...) in this case.
		if (!m_hRClickItem )
			return;
		m_treeDist.SpinEditLabel(m_hRClickItem);
		m_btnSave.EnableWindow();	
		
		break;
	case 1:
		//even all branches' percent.
		EvenPercentCurrentLevel();
		break;
	case 2:
		//even remain all branches' percent.
		EvenRemainPercentCurrentLevel();
		break;
	default:
		break;
	}
	
}

void CDlgVehicleDistribution::SetEventAllBranchPercent(int nLevel,int nPercent, int nSel)
{
	int nSearch = 0;
	int pIdxPath[LEVELS];
	memset(pIdxPath,-1,sizeof(int)*LEVELS);

	SetEventLevelPercent(nLevel,0,nPercent,nSel,pIdxPath);
}

void CDlgVehicleDistribution::SetEventLevelPercent(int nLevel,int nCurLevel, int nUsedPercent, int nSel,int* _pIdxPath)
{
	if (nCurLevel == nLevel)
	{
		VehicleTypeDistribution* pVehicleDist = GetPaxDist();
		LandsideVehicleType* pType = m_pVehicleTypeList->GetItem(nCurLevel);
		int nChildCount = pType->GetLeafCount();
		for(int i=0;i<nChildCount;i++)
		{
			_pIdxPath[nCurLevel] = i;
			pVehicleDist->setValue( _pIdxPath, static_cast<float>(nUsedPercent) );
		}

		_pIdxPath[nCurLevel] = nChildCount - 1;
		pVehicleDist->setValue( _pIdxPath, static_cast<float>(100- nUsedPercent*(nChildCount-1)) );
		return;
	}

	LandsideVehicleType* pType = m_pVehicleTypeList->GetItem(nCurLevel);
	int nChildCount = pType->GetLeafCount();
	for (int i = 0; i < nChildCount; i++)
	{
		_pIdxPath[nCurLevel] = i;
		SetEvenRemainLevelPercent(nLevel,nCurLevel + 1,nUsedPercent,nSel,_pIdxPath);
	}
}

void CDlgVehicleDistribution::SetEvenRemainPercent(int nLevel,int nPercent, int nSel)
{
	int nSearch = 0;
	int pIdxPath[LEVELS];
	memset(pIdxPath,-1,sizeof(int)*LEVELS);

	SetEvenRemainLevelPercent(nLevel,0,nPercent,nSel,pIdxPath);
}

void CDlgVehicleDistribution::SetEvenRemainLevelPercent(int nLevel,int nCurLevel, int nUsedPercent, int nSel,int* _pIdxPath)
{
	if (nCurLevel == nLevel)
	{
		LandsideVehicleType* pType = m_pVehicleTypeList->GetItem(nCurLevel);
		int nChildCount = pType->GetLeafCount();
		int nRemainEvenPercent=(100 - nUsedPercent)/(nChildCount -1);
		VehicleTypeDistribution* pVehicleDist = GetPaxDist();
		int nHasUpdatedItems = 0;
		for (int i = 0; i < nChildCount; i++)
		{
			_pIdxPath[nCurLevel] = i;
			if(i==nSel)
			{
				//set selected item's percent.
				pVehicleDist->setValue( _pIdxPath, static_cast<float>(nUsedPercent) );
			}
			else
			{
				//set even percent.
				if( nHasUpdatedItems == nChildCount-2)
				{
					//ensure total percent ==100
					pVehicleDist->setValue( _pIdxPath, static_cast<float>(100- nUsedPercent - nRemainEvenPercent*(nChildCount-2)) );
				}
				else
				{
					pVehicleDist->setValue( _pIdxPath, static_cast<float>(nRemainEvenPercent) );
				}

				nHasUpdatedItems++;
			}
		}
		return;
	}

	LandsideVehicleType* pType = m_pVehicleTypeList->GetItem(nCurLevel);
	int nChildCount = pType->GetLeafCount();
	for (int i = 0; i < nChildCount; i++)
	{
		_pIdxPath[nCurLevel] = i;
		SetEvenRemainLevelPercent(nLevel,nCurLevel + 1,nUsedPercent,nSel,_pIdxPath);
	}
}

void CDlgVehicleDistribution::SetPercentAllBranch(int nLevel,int nPercent, int nSel)
{
	int nSearch = 0;
	int pIdxPath[LEVELS];
	memset(pIdxPath,-1,sizeof(int)*LEVELS);

	SetLevelBranchPercent(nLevel,0,nPercent,nSel,pIdxPath);
}

void CDlgVehicleDistribution::SetLevelBranchPercent(int nLevel,int nCurLevel, int nPercent, int nSel,int* _pIdxPath )
{
	if (nCurLevel == nLevel)
	{
		_pIdxPath[nLevel] = nSel;
		VehicleTypeDistribution* pVehicleDist = GetPaxDist();
		pVehicleDist->setValue(_pIdxPath,static_cast<float>(nPercent));
		return;
	}

	LandsideVehicleType* pType = m_pVehicleTypeList->GetItem(nCurLevel);
	int nChildCount = pType->GetLeafCount();
	for (int i = 0; i < nChildCount; i++)
	{
		_pIdxPath[nCurLevel] = i;
		SetLevelBranchPercent(nLevel,nCurLevel+1,nPercent,nSel,_pIdxPath);
	}
}

void CDlgVehicleDistribution::EvenPercentCurrentLevel()
{
	// TODO: Add your command handler code here
	if (!m_hRClickItem )
		return;
	
	// SET DATABASE
	VehicleTypeDistribution* pVehicleDist = GetPaxDist();
	if( !pVehicleDist )
		return;
	
	int nLevels = GetLevelIdx( m_hRClickItem );
	HTREEITEM hParentItem = m_treeDist.GetParentItem( m_hRClickItem );
	LandsideVehicleType* pType = m_pVehicleTypeList->GetItem(nLevels);
	int nChildCount = pType->GetLeafCount();
	int nEvenPercent=100/nChildCount;
	
	HTREEITEM hChildItem;
	ASSERT(nChildCount>0);

	std::vector<HTREEITEM> vItems;
	GetAllItemAtDepth(NULL,vItems,nLevels);
	//if level==0
	if(nLevels==0)
	{
		vItems.push_back(NULL);
	}

	int nSelectedItem=0;
	hChildItem=m_treeDist.GetChildItem(hParentItem);
	while(hChildItem!=NULL)
	{
		if(hChildItem==m_hRClickItem)break;
		hChildItem=m_treeDist.GetNextItem(hChildItem, TVGN_NEXT);
		nSelectedItem++;
	}

	SetEventAllBranchPercent(nLevels,nEvenPercent,nSelectedItem);
	for(int nItem=0;nItem<static_cast<int>(vItems.size());nItem++)
	{
		hParentItem=vItems[nItem];

		LandsideVehicleType* pType = m_pVehicleTypeList->GetItem(nLevels);
		for(int i=0;i<nChildCount;i++)
		{
			if( i == 0 )
				hChildItem = m_treeDist.GetChildItem( hParentItem );
			else
				hChildItem = m_treeDist.GetNextItem( hChildItem, TVGN_NEXT );
			
			int pIdxPath[LEVELS];
			GetIdxPath( hChildItem, pIdxPath );
			
			pVehicleDist->setValue( pIdxPath, static_cast<float>(nEvenPercent) );
			
			

			CString csLabel = GetItemLabel( pType->GetLeaf( i ), nEvenPercent );
			m_treeDist.SetItemText( hChildItem, csLabel );
		}

		//ensure total percent ==100
 		int pIdxPath[LEVELS];
 		GetIdxPath( hChildItem, pIdxPath );
 		pVehicleDist->setValue( pIdxPath, static_cast<float>(100- nEvenPercent*(nChildCount-1)) );
		CString csLabel = GetItemLabel(pType->GetLeaf( nChildCount-1), 100- nEvenPercent*(nChildCount-1) );
		m_treeDist.SetItemText( hChildItem, csLabel );
		
	}


	vItems.clear();
	m_btnSave.EnableWindow(TRUE);

}

void CDlgVehicleDistribution::GetAllItemAtDepth(const HTREEITEM hParent,std::vector<HTREEITEM>& vItems, int nDepth)
{
	if(nDepth<1)return;

	HTREEITEM hChild=m_treeDist.GetChildItem(hParent);

	if(nDepth==1)
	{
		while(hChild!=NULL)
		{
			vItems.push_back(hChild);
			hChild=m_treeDist.GetNextItem(hChild , TVGN_NEXT);
		}
	}
	else
	{
		nDepth--;
		while(hChild!=NULL)
		{
			GetAllItemAtDepth(hChild,vItems,nDepth);
			hChild=m_treeDist.GetNextItem(hChild , TVGN_NEXT);
		}
	}
				
}
void CDlgVehicleDistribution::EvenRemainPercentCurrentLevel()
{
	// TODO: Add your command handler code here
	if (!m_hRClickItem )
		return;
	
	// SET DATABASE
	VehicleTypeDistribution* pVehicleDist = GetPaxDist();
	if( !pVehicleDist )
		return;
	
	int nLevels = GetLevelIdx( m_hRClickItem );
	HTREEITEM hParentItem = m_treeDist.GetParentItem( m_hRClickItem );
	LandsideVehicleType* pType = m_pVehicleTypeList->GetItem(nLevels);
	int nChildCount = pType->GetLeafCount();
	
	if( nChildCount==1)return;

	HTREEITEM hChildItem;
	int idxPath[LEVELS];
	GetIdxPath(m_hRClickItem, idxPath);
	int nUsedPercent = (int)pVehicleDist->getValue(idxPath);
	int nRemainEvenPercent=(100 - nUsedPercent)/(nChildCount -1);

	//get the sequence of selected item.
	int nSelectedItem=0;
	hChildItem=m_treeDist.GetChildItem(hParentItem);
	while(hChildItem!=NULL)
	{
		if(hChildItem==m_hRClickItem)break;
		hChildItem=m_treeDist.GetNextItem(hChildItem, TVGN_NEXT);
		nSelectedItem++;
	}
	
	//get all brothers item in the same level .
	std::vector<HTREEITEM> vItems;
	GetAllItemAtDepth(NULL,vItems,nLevels);

	//if level==0
	if(nLevels==0)
	{
		vItems.push_back(NULL);
	}
	
	SetEvenRemainPercent(nLevels,nUsedPercent,nSelectedItem);
	//even remaining percent 
	int nHasUpdatedItems;
	for(int nItem=0;nItem<static_cast<int>(vItems.size());nItem++)
	{
		hParentItem=vItems[nItem];
		
		nHasUpdatedItems = 0;
		for(int i=0;i<nChildCount;i++)
		{
			if( i == 0 )
				hChildItem = m_treeDist.GetChildItem( hParentItem );
			else
				hChildItem = m_treeDist.GetNextItem( hChildItem, TVGN_NEXT );
			
			int pIdxPath[LEVELS];
			GetIdxPath( hChildItem, pIdxPath );
			
// 			if(i==nSelectedItem)
// 			{
// 				//set selected item's percent.
// 				pPaxDist->setValue( pIdxPath, static_cast<float>(nUsedPercent) );
// 			}
// 			else
// 			{
// 				//set even percent.
// 				if( nHasUpdatedItems == nChildCount-2)
// 				{
// 					//ensure total percent ==100
// 					pPaxDist->setValue( pIdxPath, static_cast<float>(100- nUsedPercent - nRemainEvenPercent*(nChildCount-2)) );
// 				}
// 				else
// 				{
// 					pPaxDist->setValue( pIdxPath, static_cast<float>(nRemainEvenPercent) );
// 				}
// 				
// 				nHasUpdatedItems++;
// 			}
			
			CString csLabel = GetItemLabel(pType->GetLeaf( i ), (int)pVehicleDist->getValue(pIdxPath) );
			m_treeDist.SetItemText( hChildItem, csLabel );
		}
		
	}

	vItems.clear();
	m_btnSave.EnableWindow(TRUE);
	
}
void CDlgVehicleDistribution::ModifyPercentAllBranch(int _nNewPercent)
{
	// TODO: Add your command handler code here
	if (!m_hRClickItem )
		return;
	
	// SET DATABASE
	VehicleTypeDistribution* pVehicleDist = GetPaxDist();
	if( !pVehicleDist )
		return;
	
	int nLevels = GetLevelIdx( m_hRClickItem );
	HTREEITEM hParentItem = m_treeDist.GetParentItem( m_hRClickItem );
	LandsideVehicleType* pType = m_pVehicleTypeList->GetItem(nLevels);
	int nChildCount = pType->GetLeafCount();
	
	if( nChildCount==1)return;
	
	HTREEITEM hChildItem;
	//get the sequence of selected item.
	int nSelectedItem=0;
	hChildItem=m_treeDist.GetChildItem(hParentItem);
	while(hChildItem!=NULL)
	{
		if(hChildItem==m_hRClickItem)break;
		hChildItem=m_treeDist.GetNextItem(hChildItem, TVGN_NEXT);
		nSelectedItem++;
	}
	
	//get all brothers item in the same level .
	std::vector<HTREEITEM> vItems;
	GetAllItemAtDepth(NULL,vItems,nLevels);
	
	//if level==0
	if(nLevels==0)
	{
		vItems.push_back(NULL);
	}
	
	SetPercentAllBranch(nLevels,_nNewPercent,nSelectedItem);
	//even remaining percent 
	int nHasUpdatedItems;
	for(int nItem=0;nItem<static_cast<int>(vItems.size());nItem++)
	{
		hParentItem=vItems[nItem];
		
		nHasUpdatedItems = 0;
		for(int i=0;i<nChildCount;i++)
		{
			if( i == 0 )
				hChildItem = m_treeDist.GetChildItem( hParentItem );
			else
				hChildItem = m_treeDist.GetNextItem( hChildItem, TVGN_NEXT );
			
			int pIdxPath[LEVELS];
			GetIdxPath( hChildItem, pIdxPath );
			
			if(i==nSelectedItem)
			{
				//set selected item's percent.
				pVehicleDist->setValue( pIdxPath, static_cast<float>(_nNewPercent) );
				CString csLabel = GetItemLabel( pType->GetLeaf( i ), (int)pVehicleDist->getValue(pIdxPath) );
				m_treeDist.SetItemText( hChildItem, csLabel );
				break;
			}
			
		}
		
	}
	
	vItems.clear();
	m_btnSave.EnableWindow(TRUE);
}

void CDlgVehicleDistribution::DoResize(int delta)
{
	//resize the listbox and the treectrl

	CSplitterControl::ChangeWidth(&m_listboxFlight,delta);
	CSplitterControl::ChangeWidth(&m_treeDist,-delta,CW_RIGHTALIGN);
	CSplitterControl::ChangeWidth(&m_stcFrame,delta);
	CSplitterControl::ChangeWidth(&m_stcFrame2,-delta,CW_RIGHTALIGN);

	CRect  newrc,rc;

	m_treeDist.GetWindowRect(&newrc);
	ScreenToClient(&newrc);
	m_toolbarcontenter2.GetWindowRect(&rc);
	ScreenToClient(&rc);
	int width=rc.right-rc.left;
	rc.left=newrc.left;
	rc.right=rc.left+width;
	m_toolbarcontenter2.MoveWindow(&rc);
	m_TreeToolBar.MoveWindow(&rc);

	int nMargin=10;
	m_wndSplitter.SetRange(newrc.left-nMargin-rc.Width()+nMargin/2,newrc.left+newrc.Width()-nMargin/2);

	GetDlgItem(IDC_STATIC_PAXDIST)->GetWindowRect(&rc);
	ScreenToClient(&rc);
	rc.left+=delta;
	rc.right+=delta;
	GetDlgItem(IDC_STATIC_PAXDIST)->MoveWindow(&rc);

// 	int nMin,nMax;
// 	m_wndSplitter.GetRange(nMin,nMax);
// 	m_wndSplitter.SetRange(nMin+delta,nMax);


	InvalidateRect(NULL);
// 	OnPaint();
}

void CDlgVehicleDistribution::DeleteAllChildren(HTREEITEM hItem)
{
	HTREEITEM hChild = m_treeDist.GetChildItem(hItem);
	while(hChild)
	{
		HTREEITEM hNext = m_treeDist.GetNextSiblingItem(hChild);
		m_treeDist.DeleteItem(hChild);
		hChild = hNext;
	} 
}

void CDlgVehicleDistribution::OnTvnItemexpanding( NMHDR *pNMHDR, LRESULT *pResult )
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	HTREEITEM hItem = pNMTreeView->itemNew.hItem;
	*pResult = 0;

	int nIdx = GetLevelIdx(hItem);
	int nBranch = 0;
	nIdx++;
	if (nIdx == LB_ERR || nIdx >= LEVELS)
		return;

	VehicleTypeDistribution* pVehicleDist = GetPaxDist();
	if( !pVehicleDist )
		return;

	int IdxPath[LEVELS];
	GetIdxPath(hItem,IdxPath);
	if(pNMTreeView->action == TVE_EXPAND)
	{
		DeleteAllChildren(hItem);
		// add the real children (only if they themselves have children
		InsertPaxDistItem(pVehicleDist,hItem,nIdx,IdxPath);
	}
	else 
	{
		DeleteAllChildren(hItem);
		LandsideVehicleType* pType = m_pVehicleTypeList->GetItem(nIdx);
		nBranch = pType->GetLeafCount();
		// add a dummy child if the node has children
		if(nBranch != 0)
			m_treeDist.InsertItem("error",hItem);
	}
}

void CDlgVehicleDistribution::OnBtnCopy()
{
	m_pCopyDistData = new VehicleTypeDistribution(*GetPaxDist());
	GetDlgItem(IDC_BUTTON_PASTE)->EnableWindow(TRUE);
}

void CDlgVehicleDistribution::OnBtnPaste()
{
	if (m_pCopyDistData == NULL)
		return;

	int nFltIdx = m_listboxFlight.GetCurSel();
	if( nFltIdx == LB_ERR )
		return;

	VehicleTypeDistribution* pType = m_pVeicleDistList->getItem(nFltIdx);
	*pType = *m_pCopyDistData;
	m_pCopyDistData = NULL;

	OnSelchangeListVehicle();	

	GetDlgItem(IDC_BUTTON_PASTE)->EnableWindow(FALSE);
}

CString CDlgVehicleDistribution::GetProjPath()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	return pDoc->m_ProjInfo.path;
}
