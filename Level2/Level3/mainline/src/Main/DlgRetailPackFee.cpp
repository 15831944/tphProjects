// DlgRetailPesFee.cpp : implementation file
//

#include "stdafx.h"
#include "termplan.h"
#include "DlgRetailPackFee.h"
#include "TermPlanDoc.h"
#include "..\common\ProbDistEntry.h"
#include "..\common\probdistmanager.h"
#include "termplan.h"
#include "economic\ProcEconomicData.h"
#include "economic\ProcEconDatabase.h"
#include "economic\ProcEconomicDataElement.h"
#include "PassengerTypeDialog.h"
#include "DlgProbDist.h"
#include "..\inputs\probab.h"
#include <Common/ProbabilityDistribution.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgRetailPackFee dialog


CDlgRetailPackFee::CDlgRetailPackFee(enum FEESTYPE _enumType,CWnd* pParent )
: CDialog(CDlgRetailPackFee::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgRetailPackFee)
	// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_enumType = _enumType;

	m_nListPreIndex = -1;

	m_treeProc.setInputTerminal( GetInputTerminal() );
}


void CDlgRetailPackFee::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgRetailPackFee)
	DDX_Control(pDX, IDC_BUTTON_DEL, m_btnDel);
	DDX_Control(pDX, IDC_BUTTON_NEW, m_btnNew);
	DDX_Control(pDX, IDC_STATIC_TOOLBARCONTENTER, m_toolbarcontenter);
	DDX_Control(pDX, IDC_BUTTON_SAVE, m_btnSave);
	DDX_Control(pDX, IDC_TREE1, m_treeProc);
	DDX_Control(pDX, IDC_LIST1, m_listCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgRetailPackFee, CDialog)
//{{AFX_MSG_MAP(CDlgRetailPackFee)
ON_WM_CREATE()
ON_COMMAND(ID_TOOLBARBUTTONADD, OnToolbarbuttonadd)
ON_COMMAND(ID_TOOLBARBUTTONDEL, OnToolbarbuttondel)
ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, OnSelchangedTree1)
ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST1,OnEndlabeledit)
	ON_BN_CLICKED(IDC_BUTTON_NEW, OnButtonNew)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnButtonDel)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, OnItemchangedList1)
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgRetailPackFee message handlers
BOOL CDlgRetailPackFee::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_toolTips.InitTooltips( &m_listCtrl );
	// TODO: Add extra initialization here
	switch( m_enumType )
	{
	case RETAILPERCENTAGEFEES:
		SetWindowText( "Retail" );
		break;
	case PACKINGFEES:
		SetWindowText( "Parking Fees" );
		break;
	case LANDSIDETRANSFEE:
		SetWindowText( "Landside Transportation Fees" );
		break;
	}

	GetDatabase();
	
	ReloadDatabase();
	
	InitToolbar();
	
	InitListCtrl();
	
	m_btnSave.EnableWindow( FALSE );
	
	// Added by Xie Bo 2002.5.14
	// Set the new button of toolbar is unable
	m_listCtrl.DeleteAllItems();
	HTREEITEM hItem = m_treeProc.GetSelectedItem();
	if( hItem == NULL )
	{
		m_btnNew.EnableWindow(FALSE);
		m_btnDel.EnableWindow(FALSE);
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL,FALSE );
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD,FALSE );
		return TRUE;
	}
	int nDBIdx = m_treeProc.GetItemData( hItem );
	if(nDBIdx == -1)
	{
		m_btnNew.EnableWindow(TRUE);
		m_btnDel.EnableWindow(FALSE);
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL,FALSE );
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD,FALSE );
	}
	else
	{
		m_btnNew.EnableWindow(FALSE);
		m_btnDel.EnableWindow(TRUE);
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL,GetSelectedListItem() != -1 );
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONADD,TRUE);	
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgRetailPackFee::InitToolbar()
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

int CDlgRetailPackFee::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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

void CDlgRetailPackFee::OnToolbarbuttonadd()
{
	HTREEITEM hSelItem = m_treeProc.GetSelectedItem();
	if( hSelItem == NULL )
		return;
    if( m_listCtrl.GetItemCount() == 1 && m_listCtrl.GetItemText( 0 ,0 ) == "" )
		m_listCtrl.DeleteAllItems();
	
    int nDBIdx;
	BOOL bOwn;
    GetDBIdx( nDBIdx,bOwn );
		
	switch( m_enumType )
	{
	case RETAILPERCENTAGEFEES:
		{
			CPassengerTypeDialog dlg( m_pParentWnd );
			if (dlg.DoModal() == IDOK)
			{
				//PassengerConstraint paxConstr = dlg.GetPaxSelection();
				CMobileElemConstraint mobileConstr	= dlg.GetMobileSelection();
				// add new pax constraint.
				m_multiPaxConstr.addConstraint( mobileConstr );
				CString showStr;
				mobileConstr.screenPrint( showStr, 0, 54 );
				int nCount = m_listCtrl.GetItemCount();
				int nIdx = m_listCtrl.InsertItem( nCount, showStr.GetBuffer(0) );
				m_listCtrl.SetItemState( nIdx , LVIS_SELECTED, LVIS_SELECTED);
				
				// add to database
				// Modified by Xie Bo  2002.5.14
				if( bOwn )  // modify item data 
				{	
					CProcEconomicDataElement* pEntry = (CProcEconomicDataElement*)m_pDB->getItem( nDBIdx );
					CRetailFeeData* pData = (CRetailFeeData*)pEntry->getData();
					RetailFeeItem  retailItem;
					retailItem.m_pPaxType = new CMobileElemConstraint(GetInputTerminal());
					//char szRaw[512]; matt
					char szRaw[2560];
					mobileConstr.WriteSyntaxStringWithVersion( szRaw );
					retailItem.m_pPaxType->SetInputTerminal( GetInputTerminal() );
					retailItem.m_pPaxType->setConstraintWithVersion( szRaw );
 					retailItem.m_pSaleDist = new ConstantDistribution(0.0);
					retailItem.m_dAirportCut = 0.0;
					retailItem.m_dProbBuy = 0.0;
					m_listCtrl.SetItemData( nIdx, pData->m_vList.size()+100 );
					pData->m_vList.push_back( retailItem );
					
				}
				else // add new item data 
				{
					m_listCtrl.DeleteAllItems();
					ProcessorID id;
					CString str = m_treeProc.GetItemText( hSelItem );
					id.SetStrDict( GetInputTerminal()->inStrDict );
					id.setID( str );
					
					CRetailFeeData* pData = new CRetailFeeData();
					CProcEconomicDataElement* pEntry = new CProcEconomicDataElement( id );
					RetailFeeItem retailItem;
					retailItem.m_pPaxType = new CMobileElemConstraint(GetInputTerminal());
	
					//char szRaw[512]; matt
					char szRaw[2560];
					mobileConstr.WriteSyntaxStringWithVersion( szRaw );
					retailItem.m_pPaxType->SetInputTerminal( GetInputTerminal() );
					retailItem.m_pPaxType->setConstraintWithVersion( szRaw );
					retailItem.m_pSaleDist = new ConstantDistribution(0.0);
					retailItem.m_dAirportCut = 0.0;
					retailItem.m_dProbBuy = 0.0;
					m_listCtrl.SetItemData( nIdx, pData->m_vList.size()+100 );
					pData->m_vList.push_back( retailItem );
					
					// Added by Xie Bo 2002.5.14
					pEntry->setData(pData);
					m_pDB->addItem(pEntry);

					m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONADD );
				}
				m_treeProc.ResetTreeData( NULL );
				m_btnSave.EnableWindow();
			}
		}
		break;
	case PACKINGFEES:
		{
			CPassengerTypeDialog dlg( m_pParentWnd );
			if (dlg.DoModal() == IDOK)
			{
				//PassengerConstraint paxConstr = dlg.GetPaxSelection();
				CMobileElemConstraint mobileConstr = dlg.GetMobileSelection();
				// add new pax constraint.
				m_multiPaxConstr.addConstraint( mobileConstr );
				CString showStr;
				mobileConstr.screenPrint( showStr, 0, 54 );
				int nCount = m_listCtrl.GetItemCount();
				int nIdx = m_listCtrl.InsertItem( nCount, showStr.GetBuffer(0) );
				m_listCtrl.SetItemState( nIdx , LVIS_SELECTED, LVIS_SELECTED);
				
				// add to database
				// Modified by Xie Bo  2002.5.14
				if( bOwn )
				{			
					CProcEconomicDataElement* pEntry = (CProcEconomicDataElement*)m_pDB->getItem( nDBIdx );
					CParkingFeeData* pData = (CParkingFeeData*)pEntry->getData();
					ParkingFeeItem parkItem;
					parkItem.m_pPaxType = new CMobileElemConstraint(GetInputTerminal());
				
					//char szRaw[512]; matt
					char szRaw[2560];
					mobileConstr.WriteSyntaxStringWithVersion( szRaw );
					parkItem.m_pPaxType->SetInputTerminal( GetInputTerminal() );
					parkItem.m_pPaxType->setConstraintWithVersion( szRaw );
					parkItem.m_pDurDist = new ConstantDistribution(0.0);
					parkItem.m_dRatePerHour = 0.0;
					m_listCtrl.SetItemData( nIdx, pData->m_vList.size()+100 );
					pData->m_vList.push_back( parkItem );
				}
				else
				{
					m_listCtrl.DeleteAllItems();
					ProcessorID id;
					CString str = m_treeProc.GetItemText( hSelItem );
					id.SetStrDict( GetInputTerminal()->inStrDict );
					id.setID( str );
					
					CParkingFeeData* pData = new CParkingFeeData();
					CProcEconomicDataElement* pEntry = new CProcEconomicDataElement( id );
					ParkingFeeItem parkItem;
					parkItem.m_pPaxType = new CMobileElemConstraint(GetInputTerminal());
					
					//char szRaw[512]; matt
					char szRaw[2560];
					mobileConstr.WriteSyntaxStringWithVersion( szRaw );
					parkItem.m_pPaxType->SetInputTerminal( GetInputTerminal() );
					parkItem.m_pPaxType->setConstraintWithVersion( szRaw );
					parkItem.m_pDurDist = new ConstantDistribution(0.0);
					parkItem.m_dRatePerHour = 0.0;
					m_listCtrl.SetItemData( nIdx, pData->m_vList.size()+100 );
					pData->m_vList.push_back( parkItem );

					// Added by Xie Bo 2002.5.14
					pEntry->setData(pData);
					m_pDB->addItem(pEntry);

					m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONADD );
				}
				m_treeProc.ResetTreeData( NULL );
				m_btnSave.EnableWindow();
			}
		}	
		break;
	case LANDSIDETRANSFEE:
		{
			// add to database
			int nIdx = m_listCtrl.GetItemCount();
			// Modified by Xie Bo  2002.5.14
			if( bOwn )
			{	
				CProcEconomicDataElement* pEntry = (CProcEconomicDataElement*)m_pDB->getItem( nDBIdx );
				CLandsdTransPtFeeData* pData = (CLandsdTransPtFeeData*)pEntry->getData();
				LandTransPtItem landItem;
				landItem.m_csMode="DEFAULT_MODE";
				landItem.m_pProbDist = new ConstantDistribution(0.0);
				landItem.m_dTripFee=0.0;
				m_listCtrl.SetItemData( nIdx, pData->m_vList.size()+100 );
				pData->m_vList.push_back( landItem );
			}
			else
			{	
				m_listCtrl.DeleteAllItems();
				HTREEITEM hItem = m_treeProc.GetSelectedItem();
				ProcessorID id;
				CString str = m_treeProc.GetItemText( hItem );
				id.SetStrDict( GetInputTerminal()->inStrDict );
				id.setID( str );
				
				CLandsdTransPtFeeData* pData = new CLandsdTransPtFeeData();
				CProcEconomicDataElement* pEntry = new CProcEconomicDataElement( id );
				
				// get item data from listctrl
				LandTransPtItem landItem;
				landItem.m_csMode="";
				landItem.m_pProbDist = new ConstantDistribution(0.0);
				landItem.m_dTripFee=0.0;
				m_listCtrl.SetItemData( nIdx, pData->m_vList.size()+100 );
				pData->m_vList.push_back( landItem );
				
				// Added by Xie Bo 2002.5.14
				pEntry->setData(pData);
				m_pDB->addItem(pEntry);

				m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONADD );
			}			
			m_treeProc.ResetTreeData( NULL );
			m_listCtrl.EditNew();
		}
		break;
	}
	ShowListData();
}	
void CDlgRetailPackFee::OnToolbarbuttondel()
{
	int nDBIdx;
	BOOL bOwn;
	GetDBIdx( nDBIdx,bOwn );
	
	// only delete the data belong to this item
	if( nDBIdx == -1 || !bOwn ) 
		return;
	
	int nIdx = m_listCtrl.GetCurSel();
	
	switch( m_enumType )
	{
	case RETAILPERCENTAGEFEES:
		{
			CProcEconomicDataElement* pEntry = (CProcEconomicDataElement*)m_pDB->getItem( nDBIdx );
			CRetailFeeData* pData = (CRetailFeeData*)pEntry->getData();
			int nDataDBIdx = m_listCtrl.GetItemData( nIdx ) - 100;
			if( nDataDBIdx >= 0 )
				pData->m_vList.erase( pData->m_vList.begin() + nDataDBIdx );
			m_listCtrl.DeleteItem( nIdx );
			m_treeProc.ResetTreeData( NULL );
			if(0==pData->m_vList.size())
			{
				OnButtonDel();
			}
		}
		break;
	case PACKINGFEES:
		{
			CProcEconomicDataElement* pEntry = (CProcEconomicDataElement*)m_pDB->getItem( nDBIdx );
			CParkingFeeData* pData = (CParkingFeeData*)pEntry->getData();
			int nDataDBIdx = m_listCtrl.GetItemData( nIdx ) - 100;
			if( nDataDBIdx >= 0 )
				pData->m_vList.erase( pData->m_vList.begin() + nDataDBIdx );
			
			m_listCtrl.DeleteItem( nIdx );
			m_treeProc.ResetTreeData( NULL );
			if(0==pData->m_vList.size())
			{
				OnButtonDel();
			}
		}
		break;
	case LANDSIDETRANSFEE:
		{
			CProcEconomicDataElement* pEntry = (CProcEconomicDataElement*)m_pDB->getItem( nDBIdx );
			CLandsdTransPtFeeData* pData = (CLandsdTransPtFeeData*)pEntry->getData();
			int nDataDBIdx = m_listCtrl.GetItemData( nIdx ) - 100;
			if( nDataDBIdx >= 0 )
				pData->m_vList.erase( pData->m_vList.begin() + nDataDBIdx );
			
			m_listCtrl.DeleteItem( nIdx );
			m_treeProc.ResetTreeData( NULL );
			if(0==pData->m_vList.size())
			{
				OnButtonDel();
			}
		}
		break;
	}
	ShowListData();
	m_btnSave.EnableWindow();
}

InputTerminal* CDlgRetailPackFee::GetInputTerminal()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	return (InputTerminal*)&pDoc->GetTerminal();
}


CString CDlgRetailPackFee::GetProjPath()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	return pDoc->m_ProjInfo.path;
}

void CDlgRetailPackFee::InitListCtrl()
{	
	DWORD dwStyle = m_listCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_listCtrl.SetExtendedStyle( dwStyle );
	
	LV_COLUMNEX	lvc;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT ;
	char columnLabel[4][128];
	int i,m,nCount;
	int DefaultColumnWidth[] = { 200, 120,100,100 };
	int nFormat[] = {	LVCFMT_CENTER, LVCFMT_CENTER };
	CStringList strList;
	CString s;
	CProbDistEntry* pPBEntry;
	switch( m_enumType )
	{
	case RETAILPERCENTAGEFEES:
		strcpy( columnLabel[0], "Pax Type" );		
		strcpy( columnLabel[1], "Sale Dist ($)" );		
		strcpy( columnLabel[2], "Prob Buy (%)" );		
		strcpy( columnLabel[3], "Airport cut (%)" );		
		
		DefaultColumnWidth[1] = 120;
		s.LoadString(IDS_STRING_NEWDIST);
		strList.InsertAfter( strList.GetTailPosition(), s );
		nCount = _g_GetActiveProbMan( GetInputTerminal() )->getCount();
		for( m=0; m<nCount; m++ )
		{
			pPBEntry = _g_GetActiveProbMan( GetInputTerminal() )->getItem( m );			
			strList.InsertAfter( strList.GetTailPosition(), pPBEntry->m_csName );
		}
		lvc.csList = &strList;
		
		for( i=0; i<4; i++ )
		{
			lvc.pszText = columnLabel[i];
			lvc.cx = DefaultColumnWidth[i];
			if( i == 0 )
				lvc.fmt = LVCFMT_NOEDIT;
			if( i == 1 )
				lvc.fmt = LVCFMT_DROPDOWN;
			if( i > 1 )
				lvc.fmt = LVCFMT_NUMBER;
			
			m_listCtrl.InsertColumn( i, &lvc );
		}
		
		break;
	case PACKINGFEES:
		DefaultColumnWidth[1] = 200;
		DefaultColumnWidth[2] = 120;
		strcpy( columnLabel[0], "Passenger Type" );		
		strcpy( columnLabel[1], "Duration distribution (hrs)" );		
		strcpy( columnLabel[2], "$ rate/hr" );		
		s.LoadString(IDS_STRING_NEWDIST);
		strList.InsertAfter( strList.GetTailPosition(), s );
		nCount = _g_GetActiveProbMan( GetInputTerminal() )->getCount();
		for( m=0; m<nCount; m++ )
		{
			pPBEntry = _g_GetActiveProbMan( GetInputTerminal() )->getItem( m );			
			strList.InsertAfter( strList.GetTailPosition(), pPBEntry->m_csName );
		}
		lvc.csList = &strList;
		
		for( i=0; i<3; i++ )
		{
			lvc.pszText = columnLabel[i];
			lvc.cx = DefaultColumnWidth[i];
			if( i == 0 )
				lvc.fmt = LVCFMT_NOEDIT;
			if( i == 1 )
				lvc.fmt = LVCFMT_DROPDOWN;
			if( i == 2 )
				lvc.fmt = LVCFMT_NUMBER;
			m_listCtrl.InsertColumn( i, &lvc );
		}
		
		break;
	case LANDSIDETRANSFEE:
		DefaultColumnWidth[1] = 200;
		DefaultColumnWidth[2] = 120;
		strcpy( columnLabel[0], "Mode" );		
		strcpy( columnLabel[1], "Load Distribution (hrs)" );		
		strcpy( columnLabel[2], "Trip Fee ($)" );		
		s.LoadString(IDS_STRING_NEWDIST);
		strList.InsertAfter( strList.GetTailPosition(), s );
		nCount = _g_GetActiveProbMan( GetInputTerminal() )->getCount();
		for( m=0; m<nCount; m++ )
		{
			pPBEntry = _g_GetActiveProbMan( GetInputTerminal() )->getItem( m );			
			strList.InsertAfter( strList.GetTailPosition(), pPBEntry->m_csName );
		}
		lvc.csList = &strList;
		
		for( i=0; i<3; i++ )
		{
			lvc.pszText = columnLabel[i];
			lvc.cx = DefaultColumnWidth[i];
			if( i == 0 )
				lvc.fmt = 0;
			if( i == 1 )
				lvc.fmt = LVCFMT_DROPDOWN;
			if( i == 2 )
				lvc.fmt = LVCFMT_NUMBER;
			m_listCtrl.InsertColumn( i, &lvc );
		}
		
		break;
		
	}
	//	m_listCtrl.SetInputTerminal( GetInputTerminal() );
}

void CDlgRetailPackFee::OnButtonSave()
{
	CWaitCursor wc;
	m_pDB->saveDataSet( GetProjPath(), false );
	m_btnSave.EnableWindow( FALSE );
}

void CDlgRetailPackFee::OnCancel()
{
	if( m_btnSave.IsWindowEnabled() )
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

void CDlgRetailPackFee::OnOK()
{
	if( m_btnSave.IsWindowEnabled() )
		OnButtonSave();
	
	CDialog::OnOK();
}

void CDlgRetailPackFee::OnSelchangedTree1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	ShowListData();

	// Modified by Xie Bo 2002.5.14
	// If the selected tree node is not in Database,enable the "new" button,
	// and unenable the "delete" button
	HTREEITEM hItem = m_treeProc.GetSelectedItem();
	if( hItem == NULL )
		return ;
	int nDBIdx = m_treeProc.GetItemData( hItem );
	if(nDBIdx == -1)
	{
		m_btnNew.EnableWindow(TRUE);
		m_btnDel.EnableWindow(FALSE);
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL,FALSE );
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD,FALSE );
	}
	else
	{
		m_btnNew.EnableWindow(FALSE);
		m_btnDel.EnableWindow(TRUE);
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL,GetSelectedListItem() != -1 );
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONADD,TRUE);	
	}
	
	*pResult = 0;
}

void CDlgRetailPackFee::ShowDefaultListData()
{
	switch( m_enumType )
	{
	case RETAILPERCENTAGEFEES:
		m_listCtrl.InsertItem( 0 , "" );
		break;
	case PACKINGFEES:
		m_listCtrl.InsertItem( 0 , "" );
		break;
	case LANDSIDETRANSFEE:
		m_listCtrl.InsertItem( 0 , "" );
		break;
	}
	m_listCtrl.SetItemData( 0, -1 );
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL,FALSE );
}

void CDlgRetailPackFee::ShowListData()
{
	m_listCtrl.DeleteAllItems();
    int nDBIdx;
	BOOL bOwn;
    GetDBIdx( nDBIdx,bOwn );
	
	// if no selected data, show default data
	if( nDBIdx == -1 )
		ShowDefaultListData();
	else
	{
		// else show data from database
		switch( m_enumType )
		{
		case RETAILPERCENTAGEFEES:
			{
				
				CProcEconomicDataElement* pEntry = (CProcEconomicDataElement*)m_pDB->getItem( nDBIdx );
				CRetailFeeData* pData = (CRetailFeeData*)pEntry->getData();
				for( int i = 0; i< static_cast<int>(pData->m_vList.size()); i++ )
				{
					RetailFeeItem retailItem = pData->m_vList[i];
					CString str;
					// Modified by Xie Bo 2002.5.14
					retailItem.m_pPaxType->screenPrint( str );
					int nIdx = m_listCtrl.InsertItem( m_listCtrl.GetItemCount(), str );
					str.ReleaseBuffer();

					ProbabilityDistribution* pProbDist = retailItem.m_pSaleDist;
					char szDist[1024];
		            pProbDist->screenPrint( szDist );
					if( pProbDist != NULL )
						m_listCtrl.SetItemText(nIdx, 1,szDist);
					
					str.Format( "%.2f",retailItem.m_dProbBuy );
					m_listCtrl.SetItemText( nIdx,2,str );
					str.Format( "%.2f",retailItem.m_dAirportCut );
					m_listCtrl.SetItemText( nIdx,3,str );
					if( bOwn )
						m_listCtrl.SetItemData( nIdx,i+100 );
					else
						m_listCtrl.SetItemData( nIdx, -1 );
				}
				
			}
			break;
		case PACKINGFEES:
			{
				
				CProcEconomicDataElement* pEntry = (CProcEconomicDataElement*)m_pDB->getItem( nDBIdx );
				CParkingFeeData* pData = (CParkingFeeData*)pEntry->getData();
				for( int i = 0; i< static_cast<int>(pData->m_vList.size()); i++ )
				{
					ParkingFeeItem parkItem = pData->m_vList[i];
					CString str;
					parkItem.m_pPaxType->screenPrint( str );
					int nIdx = m_listCtrl.InsertItem( m_listCtrl.GetItemCount(), str.GetBuffer(0) );
					str.ReleaseBuffer();
					ProbabilityDistribution* pProbDist = parkItem.m_pDurDist;
				     char szDist[1024];
		            pProbDist->screenPrint( szDist );
					if( pProbDist != NULL )
						m_listCtrl.SetItemText(nIdx, 1,szDist );
					str.Format( "%.2f",parkItem.m_dRatePerHour );
					m_listCtrl.SetItemText( nIdx,2,str );
					if( bOwn )
						m_listCtrl.SetItemData( nIdx,i+100 );
					else
						m_listCtrl.SetItemData( nIdx, -1 );
				}
				
			}
			break;
		case LANDSIDETRANSFEE:
			{
				
				CProcEconomicDataElement* pEntry = (CProcEconomicDataElement*)m_pDB->getItem( nDBIdx );
				CLandsdTransPtFeeData* pData = (CLandsdTransPtFeeData*)pEntry->getData();
				for( int i = 0; i< static_cast<int>(pData->m_vList.size()); i++ )
				{
					LandTransPtItem landItem = pData->m_vList[i];
					int nIdx = m_listCtrl.InsertItem( m_listCtrl.GetItemCount(), landItem.m_csMode );
					ProbabilityDistribution* pProbDist = landItem.m_pProbDist;
					char szDist[1024];
		            pProbDist->screenPrint( szDist );
					if( pProbDist != NULL )
						m_listCtrl.SetItemText(nIdx, 1,szDist);
					CString str;
					str.Format( "%.2f",landItem.m_dTripFee );
					m_listCtrl.SetItemText( nIdx,2,str );
					if( bOwn )
						m_listCtrl.SetItemData( nIdx,i+100 );
					else
						m_listCtrl.SetItemData( nIdx, -1 );
				}
				
			}
			break;
		}
		//m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL );
	}
	if( GetSelectedListItem() != -1 )
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL );
	else
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL,FALSE );
}

void CDlgRetailPackFee::ReloadDatabase()
{ 
	if (m_enumType==RETAILPERCENTAGEFEES)
	{
		m_treeProc.LoadData( GetInputTerminal(), m_pDB ,RetailProc);
	}else
	{
		m_treeProc.LoadData( GetInputTerminal(), m_pDB );
	}
	
}

void CDlgRetailPackFee::GetDBIdx( int& nDBIdx, BOOL& bOwn )
{
	HTREEITEM hSelItem = m_treeProc.GetSelectedItem();
	if( hSelItem == NULL )
	{
		nDBIdx = -1;
		bOwn = FALSE;
		return;
	}
	nDBIdx = m_treeProc.GetItemData( hSelItem );
	bOwn = TRUE;
	if( nDBIdx != -1 )
		return;
	bOwn = FALSE;
	hSelItem = m_treeProc.GetParentItem( hSelItem );
	while( nDBIdx == -1 && hSelItem != NULL )
	{
		nDBIdx = m_treeProc.GetItemData( hSelItem );
		hSelItem = m_treeProc.GetParentItem( hSelItem );
	}
	
}

void CDlgRetailPackFee::ModifyItemData( ProbabilityDistribution* _pProbDist,int nSubItem )
{
	HTREEITEM hSelItem = m_treeProc.GetSelectedItem();
	if( hSelItem == NULL )
		return;
    
    int nDBIdx;
	BOOL bOwn;
    GetDBIdx( nDBIdx,bOwn );
	
	switch( m_enumType )
	{
	case RETAILPERCENTAGEFEES:
		{
			int nIdx = m_listCtrl.GetCurSel();
			int nDataIdx = m_listCtrl.GetItemData( nIdx ) -100;
			if( nDataIdx < 0 )
				return;
						
			if( bOwn ) // modify data
			{
				CProcEconomicDataElement* pEntry = (CProcEconomicDataElement*)m_pDB->getItem( nDBIdx );
				CRetailFeeData* pData = (CRetailFeeData*)pEntry->getData();
				
				//		pData->m_vList.clear();
				
				RetailFeeItem retailItem;
				CString str = m_listCtrl.GetItemText( nIdx,1 );
				
				CProbDistEntry* pPDEntry = NULL;
				int nCount = _g_GetActiveProbMan( GetInputTerminal() )->getCount();
				int i=0;
				for( ; i<nCount; i++ )
				{
					pPDEntry = _g_GetActiveProbMan( GetInputTerminal() )->getItem( i );
					if( strcmp( pPDEntry->m_csName, str ) == 0 )
						break;
				}
				if( i == nCount )
					retailItem.m_pSaleDist = NULL;
				else
					retailItem.m_pSaleDist = pPDEntry->m_pProbDist;
				str = m_listCtrl.GetItemText( nIdx,2 );
				if( !str.IsEmpty() )
				{	
					char* csStop;
					double dValue = strtod( str,&csStop );
					if( dValue >= 0 && *csStop != str[0] )
						retailItem.m_dProbBuy = dValue;
				}	
				else
					retailItem.m_dProbBuy = 0.0;
				str = m_listCtrl.GetItemText( nIdx,3 );
				if( !str.IsEmpty() )
				{	
					char* csStop;
					double dValue = strtod( str,&csStop );
					if( dValue >= 0 && *csStop != str[0] )
						retailItem.m_dAirportCut = dValue;
				}	
				else
					retailItem.m_dAirportCut = 0.0;
				
				pData->m_vList[nDataIdx] = retailItem;
				
				ShowListData();
			}
			}/*
			 else   // add new item data
			 {
			 HTREEITEM hItem = m_treeProc.GetSelectedItem();
			 if( hItem == NULL )
			 return;
			 ProcessorID id;
			 CString str = m_treeProc.GetItemText( hItem );
			 id.SetStrDict( GetInputTerminal()->inStrDict );
			 id.setID( str );
			 
			   CLandsdTransPtFeeData* pData = new CLandsdTransPtFeeData();
			   CProcEconomicDataElement* pEntry = new CProcEconomicDataElement( id );
			   
				 // get item data from listctrl
				 LandTransPtItem landItem;
				 landItem.m_csMode =  m_listCtrl.GetItemText( nIdx,0 );
				 landItem.m_pProbDist = _pProbDist;
				 str = m_listCtrl.GetItemText( nIdx,2 );
				 if( !str.IsEmpty() )
				 {	
				 char* csStop;
				 double dValue = strtod( str,&csStop );
				 if( dValue >= 0 && *csStop != str[0] )
				 landItem.m_dTripFee = dValue;					
				 }
				 else
				 landItem.m_dTripFee = 0.0;
				 pData->m_vList.push_back( landItem );
				 
				   pEntry->setData( pData );
				   pProcEconDB->addItem( pEntry );
				   
					 m_treeProc.ResetTreeData( NULL );
					 ShowListData();
	}*/
		break;
	case PACKINGFEES:
		{
			int nIdx = m_listCtrl.GetCurSel();
			int nDataIdx = m_listCtrl.GetItemData( nIdx ) -100;
			if( nDataIdx < 0 )
				return;
			
			if( bOwn ) // modify data
			{
				CProcEconomicDataElement* pEntry = (CProcEconomicDataElement*)m_pDB->getItem( nDBIdx );
				CParkingFeeData* pData = (CParkingFeeData*)pEntry->getData();
				
				//				pData->m_vList.clear();
				
				ParkingFeeItem parkItem;
				CString str = m_listCtrl.GetItemText( nIdx,1 );
				
				CProbDistEntry* pPDEntry = NULL;
				int nCount = _g_GetActiveProbMan( GetInputTerminal() )->getCount();
				int i=0;
				for( ; i<nCount; i++ )
				{
					pPDEntry = _g_GetActiveProbMan( GetInputTerminal() )->getItem( i );
					if( strcmp( pPDEntry->m_csName, str ) == 0 )
						break;
				}
				if( i == nCount )
					parkItem.m_pDurDist = NULL;
				else
					parkItem.m_pDurDist = pPDEntry->m_pProbDist;
				str = m_listCtrl.GetItemText( nIdx,2 );
				if( !str.IsEmpty() )
				{	
					char* csStop;
					double dValue = strtod( str,&csStop );
					if( dValue >= 0 && *csStop != str[0] )
						parkItem.m_dRatePerHour = dValue;
				}	
				else
					parkItem.m_dRatePerHour = 0.0;
				pData->m_vList[nDataIdx] = parkItem;
				ShowListData();
				
			}
			}/*
			 else   // add new item data
			 {
			 HTREEITEM hItem = m_treeProc.GetSelectedItem();
			 if( hItem == NULL )
			 return;
			 ProcessorID id;
			 CString str = m_treeProc.GetItemText( hItem );
			 id.SetStrDict( GetInputTerminal()->inStrDict );
			 id.setID( str );
			 
			   CParkingFeeData* pData = new CParkingFeeData();
			   CProcEconomicDataElement* pEntry = new CProcEconomicDataElement( id );
			   
				 // get item data from listctrl
				 ParkingFeeItem parkItem;
				 parkItem.m_pPaxType = 
				 landItem.m_csMode =  m_listCtrl.GetItemText( nIdx,0 );
				 landItem.m_pProbDist = _pProbDist;
				 str = m_listCtrl.GetItemText( nIdx,2 );
				 if( !str.IsEmpty() )
				 {	
				 char* csStop;
				 double dValue = strtod( str,&csStop );
				 if( dValue >= 0 && *csStop != str[0] )
				 landItem.m_dTripFee = dValue;					
				 }
				 else
				 landItem.m_dTripFee = 0.0;
				 pData->m_vList.push_back( landItem );
				 
				   pEntry->setData( pData );
				   pProcEconDB->addItem( pEntry );
				   
					 m_treeProc.ResetTreeData( NULL );
					 ShowListData();
			}*/
		break;
		
	case LANDSIDETRANSFEE:
		{
			int nIdx = m_listCtrl.GetCurSel();
			int nDataIdx = m_listCtrl.GetItemData( nIdx ) -100;
			if( nDataIdx < 0 )
				return;
			
			if( bOwn ) // modify data
			{
				CProcEconomicDataElement* pEntry = (CProcEconomicDataElement*)m_pDB->getItem( nDBIdx );
				CLandsdTransPtFeeData* pData = (CLandsdTransPtFeeData*)pEntry->getData();
				LandTransPtItem landItem;
				landItem.m_csMode =  m_listCtrl.GetItemText( nIdx,0 );
				CString str = m_listCtrl.GetItemText( nIdx,1 );
				
				CProbDistEntry* pPDEntry = NULL;
				int nCount = _g_GetActiveProbMan( GetInputTerminal() )->getCount();
				int i=0;
				for(; i<nCount; i++ )
				{
					pPDEntry = _g_GetActiveProbMan( GetInputTerminal() )->getItem( i );
					if( strcmp( pPDEntry->m_csName, str ) == 0 )
						break;
				}
				if( i == nCount )
					landItem.m_pProbDist = NULL;
				else
					landItem.m_pProbDist = pPDEntry->m_pProbDist;
				str = m_listCtrl.GetItemText( nIdx,2 );
				if( !str.IsEmpty() )
				{	
					char* csStop;
					double dValue = strtod( str,&csStop );
					if( dValue >= 0 && *csStop != str[0] )
						landItem.m_dTripFee = dValue;
				}	
				else
					landItem.m_dTripFee = 0.0;
				//		if( nIdx < m_listCtrl.GetItemCount() -1 ) // modify
				pData->m_vList[nDataIdx] = landItem;
				//		else  // add
				//			pData->m_vList.push_back( landItem );
				ShowListData();
				}/*
				 else   // add new item data
				 {
				 HTREEITEM hItem = m_treeProc.GetSelectedItem();
				 if( hItem == NULL )
				 return;
				 ProcessorID id;
				 CString str = m_treeProc.GetItemText( hItem );
				 id.SetStrDict( GetInputTerminal()->inStrDict );
				 id.setID( str );
				 
				   CLandsdTransPtFeeData* pData = new CLandsdTransPtFeeData();
				   CProcEconomicDataElement* pEntry = new CProcEconomicDataElement( id );
				   
					 // get item data from listctrl
					 LandTransPtItem landItem;
					 landItem.m_csMode =  m_listCtrl.GetItemText( nIdx,0 );
					 landItem.m_pProbDist = _pProbDist;
					 str = m_listCtrl.GetItemText( nIdx,2 );
					 if( !str.IsEmpty() )
					 {	
					 char* csStop;
					 double dValue = strtod( str,&csStop );
					 if( dValue >= 0 && *csStop != str[0] )
					 landItem.m_dTripFee = dValue;					
					 }
					 else
					 landItem.m_dTripFee = 0.0;
					 pData->m_vList.push_back( landItem );
					 
					   pEntry->setData( pData );
					   pProcEconDB->addItem( pEntry );
					   
						 m_treeProc.ResetTreeData( NULL );
						 ShowListData();
		}*/
			break;
		}
	}
	
}

void CDlgRetailPackFee::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	LV_ITEM* plvItem = &pDispInfo->item;
	ProbabilityDistribution* pProbDist = NULL;
	CProbDistEntry* pPDEntry = NULL;
	CProcEconomicDataElement* pEntry=NULL;
	
	// Added by Xie Bo 2002.5.15
	HTREEITEM hSelItem = m_treeProc.GetSelectedItem();
	if( hSelItem == NULL )
		return;
    int nDBIdx;
	BOOL bOwn;
    GetDBIdx( nDBIdx,bOwn );

	int nIdx,nDataIdx;
	CString s;
	//ProbabilityDistribution* pProbDist = NULL;
	switch( m_enumType )
	{
	case RETAILPERCENTAGEFEES:
		nIdx = m_listCtrl.GetCurSel();
		nDataIdx = m_listCtrl.GetItemData( nIdx ) -100;
		if( nDataIdx < 0 )
			return;

		if( bOwn ) // modify data
		{
			pEntry = (CProcEconomicDataElement*)m_pDB->getItem( nDBIdx );
			CRetailFeeData* pData = (CRetailFeeData*)pEntry->getData();
			CString str = m_listCtrl.GetItemText( nIdx,1 );
			
			switch(plvItem->iSubItem)
			{
			case 0: //Pax Type
				break;
			case 1: //Sale Dist ($)
				s.LoadString( IDS_STRING_NEWDIST );
				if( strcmp( plvItem->pszText, s ) == 0 )
				{
					CDlgProbDist dlg(GetInputTerminal()->m_pAirportDB, true );
					if( dlg.DoModal() == IDOK )  
					{
						CProbDistEntry* pPDEntry = dlg.GetSelectedPD();
						ASSERT(pPDEntry);
						m_listCtrl.SetItemText( plvItem->iItem, plvItem->iSubItem, pPDEntry->m_csName );
						pProbDist = pPDEntry->m_pProbDist;
					}
				}
				else
				{
					CProbDistEntry* pPDEntry = NULL;
					int nCount = _g_GetActiveProbMan( GetInputTerminal() )->getCount();
					for( int i=0; i<nCount; i++ )
					{
						pPDEntry = _g_GetActiveProbMan( GetInputTerminal() )->getItem( i );
						if( strcmp( pPDEntry->m_csName, plvItem->pszText ) == 0 )
							break;
					}
					//assert( i < nCount );
					pProbDist = pPDEntry->m_pProbDist;
				}
				if( pProbDist )
					pData->m_vList[nIdx].m_pSaleDist= pProbDist;
				break;
			case 2: //Prob Buy (%)
				str = m_listCtrl.GetItemText( nIdx,2 );
				if( !str.IsEmpty() )
				{	
					char* csStop;
					double dValue = strtod( str,&csStop );
					if( dValue >= 0 /*&& *csStop != str[0]*/ )
						pData->m_vList[nIdx].m_dProbBuy = dValue;
				}	
				break;
			case 3: //Airport cut (%)
				str = m_listCtrl.GetItemText( nIdx,3 );
				if( !str.IsEmpty() )
				{	
					char* csStop;
					double dValue = strtod( str,&csStop );
					if( dValue >= 0 /*&& *csStop != str[0] */)
						pData->m_vList[nIdx].m_dAirportCut = dValue;
				}	
				break;
			}
			ShowListData();
		}
		break;
	case PACKINGFEES:
		nIdx = m_listCtrl.GetCurSel();
		nDataIdx = m_listCtrl.GetItemData( nIdx ) -100;
		if( nDataIdx < 0 )
			return;
		if( bOwn ) // modify data
		{
			pEntry = (CProcEconomicDataElement*)m_pDB->getItem( nDBIdx );
			CParkingFeeData* pData = (CParkingFeeData*)pEntry->getData();
			CString str = m_listCtrl.GetItemText( nIdx,1 );
			switch(plvItem->iSubItem)
			{
			case 0: //Passenger Type
				break;
			case 1: //Duration distribution (hrs)
				s.LoadString( IDS_STRING_NEWDIST );
				if( strcmp( plvItem->pszText, s ) == 0 )
				{
					CDlgProbDist dlg(GetInputTerminal()->m_pAirportDB, true );
					if( dlg.DoModal() == IDOK )  
					{
						CProbDistEntry* pPDEntry = dlg.GetSelectedPD();
						ASSERT(pPDEntry);
						m_listCtrl.SetItemText( plvItem->iItem, plvItem->iSubItem, pPDEntry->m_csName );
						pProbDist = pPDEntry->m_pProbDist;
					}
				}
				else
				{
					CProbDistEntry* pPDEntry = NULL;
					int nCount = _g_GetActiveProbMan( GetInputTerminal() )->getCount();
					for( int i=0; i<nCount; i++ )
					{
						pPDEntry = _g_GetActiveProbMan( GetInputTerminal() )->getItem( i );
						if( strcmp( pPDEntry->m_csName, plvItem->pszText ) == 0 )
							break;
					}
					//assert( i < nCount );
					pProbDist = pPDEntry->m_pProbDist;
				}
				if( pProbDist )
					pData->m_vList[nIdx].m_pDurDist= pProbDist;
				break;
			case 2: //$ rate/hr
				str = m_listCtrl.GetItemText( nIdx,2 );
				if( !str.IsEmpty() )
				{	
					char* csStop;
					double dValue = strtod( str,&csStop );
					if( dValue >= 0 /*&& *csStop != str[0] */)
						pData->m_vList[nIdx].m_dRatePerHour = dValue;
				}
				break;
			}
			ShowListData();
		}
		break;
	case LANDSIDETRANSFEE:
		nIdx = m_listCtrl.GetCurSel();
		nDataIdx = m_listCtrl.GetItemData( nIdx ) -100;
		if( nDataIdx < 0 )
			return;

		if( bOwn ) // modify data
		{
			pEntry = (CProcEconomicDataElement*)m_pDB->getItem( nDBIdx );
			CLandsdTransPtFeeData* pData = (CLandsdTransPtFeeData*)pEntry->getData();
			CString str = m_listCtrl.GetItemText( nIdx,1 );
			switch(plvItem->iSubItem)
			{
			case 0: //Mode
				str = m_listCtrl.GetItemText( nIdx,0 );
				pData->m_vList[nIdx].m_csMode =str;
				break;
			case 1: //Load Distribution (hrs)
				s.LoadString( IDS_STRING_NEWDIST );
				if( strcmp( plvItem->pszText, s ) == 0 )
				{
					CDlgProbDist dlg(GetInputTerminal()->m_pAirportDB, true );
					if( dlg.DoModal() == IDOK )  
					{
						CProbDistEntry* pPDEntry = dlg.GetSelectedPD();
						ASSERT(pPDEntry);
						m_listCtrl.SetItemText( plvItem->iItem, plvItem->iSubItem, pPDEntry->m_csName );
						pProbDist = pPDEntry->m_pProbDist;
					}
				}
				else
				{
					CProbDistEntry* pPDEntry = NULL;
					int nCount = _g_GetActiveProbMan( GetInputTerminal() )->getCount();
					for( int i=0; i<nCount; i++ )
					{
						pPDEntry = _g_GetActiveProbMan( GetInputTerminal() )->getItem( i );
						if( strcmp( pPDEntry->m_csName, plvItem->pszText ) == 0 )
							break;
					}
					//assert( i < nCount );
					pProbDist = pPDEntry->m_pProbDist;
				}
				if( pProbDist )
					pData->m_vList[nIdx].m_pProbDist= pProbDist;
				break;
			case 2: //Trip Fee ($)
				str = m_listCtrl.GetItemText( nIdx,2 );
				if( !str.IsEmpty() )
				{	
					char* csStop;
					double dValue = strtod( str,&csStop );
					if( dValue >= 0 /*&& *csStop != str[0]*/ )
						pData->m_vList[nIdx].m_dTripFee = dValue;
				}	
				break;
			}
		}
		break;
	}
	
	*pResult = 0;
	m_btnSave.EnableWindow();
	return;
}


void CDlgRetailPackFee::OnButtonNew() 
{
	// TODO: Add your control notification handler code here
	OnToolbarbuttonadd();

	// Modified by Xie Bo 2002.5.17
	// If the selected tree node is not in Database,enable the "new" button,
	// and unenable the "delete" button
	HTREEITEM hItem = m_treeProc.GetSelectedItem();
	if( hItem == NULL )
		return ;
	int nDBIdx = m_treeProc.GetItemData( hItem );
	if(nDBIdx == -1)
	{
		m_btnNew.EnableWindow(TRUE);
		m_btnDel.EnableWindow(FALSE);
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL,FALSE );
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD,FALSE );
	}
	else
	{
		m_btnNew.EnableWindow(FALSE);
		m_btnDel.EnableWindow(TRUE);
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL,GetSelectedListItem() != -1 );
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONADD,TRUE);	
	}
	
}

void CDlgRetailPackFee::OnButtonDel() 
{
	// TODO: Add your control notification handler code here
		HTREEITEM hSelItem = m_treeProc.GetSelectedItem();
	if( hSelItem == NULL )
		return;

	int nDBIdx = m_treeProc.GetItemData( hSelItem );

	if( nDBIdx == -1 )
		return;

	m_treeProc.m_strSelectedID=m_treeProc.GetItemText(hSelItem);
	//GetInputTerminal()->serviceTimeDB->deleteItem( nDBIdx );

	m_pDB->deleteItem(nDBIdx);
	ReloadDatabase();
	ShowListData();

	// Added by Xie Bo 2002.5.9
	// Ensure visible
	if(m_treeProc.hSelItem==NULL)
		return;
	HTREEITEM hParent = m_treeProc.GetParentItem(m_treeProc.hSelItem);
	if (hParent != NULL)
		m_treeProc.Expand(hParent, TVE_EXPAND);
	m_treeProc.EnsureVisible(m_treeProc.hSelItem);
	m_treeProc.SelectItem(m_treeProc.hSelItem);
	
	m_btnDel.EnableWindow(FALSE);
}

int CDlgRetailPackFee::GetSelectedListItem()
{
	int nCount = m_listCtrl.GetItemCount();
	for( int i=0; i<nCount; i++ )
	{
		if( m_listCtrl.GetItemState( i, LVIS_SELECTED ) == LVIS_SELECTED )
			return i;
	}
	return -1;
}

//DEL void CDlgRetailPackFee::AddItemData()
//DEL {
//DEL 
//DEL }

void CDlgRetailPackFee::OnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	HTREEITEM hItem = m_treeProc.GetSelectedItem();
	if( hItem == NULL )
		return ;
	int nDBIdx = m_treeProc.GetItemData( hItem );
	if(nDBIdx == -1)
	{
		return;
	}
	if( GetSelectedListItem() != -1 )
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL );
	*pResult = 0;
}


CEconomicManager* CDlgRetailPackFee::GetEconomicManager()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
    return (CEconomicManager*)&pDoc->GetEconomicManager();
}


void CDlgRetailPackFee::GetDatabase()
{
	m_pDB = NULL;

	switch( m_enumType )
	{
	case RETAILPERCENTAGEFEES:
		m_pDB = GetEconomicManager()->m_pRetailPercRevenue;
		break;
	case PACKINGFEES:
		m_pDB = GetEconomicManager()->m_pParkingFeeRevenue;
		break;
	case LANDSIDETRANSFEE:
		m_pDB = GetEconomicManager()->m_pLandsideTransRevenue;
		break;
	}
}

BOOL CDlgRetailPackFee::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	m_toolTips.RelayEvent( pMsg );
	
	return CDialog::PreTranslateMessage(pMsg);
}

void CDlgRetailPackFee::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	int nIndex = m_listCtrl.HitTest( point );

	if( nIndex!=-1 )
	{
		if(m_nListPreIndex != nIndex)
		{
			//int iItemData = m_listCtrl.GetItemData( nIndex ) - ITEMDATA_STARTIDX;
			int iItemData = m_listCtrl.GetItemData( nIndex ) - 100;
			// TRACE("iItemData=%d nIndex = %d\r\n",iItemData,nIndex);
			ShowTips( iItemData );
			m_nListPreIndex = nIndex;
		}
	}
	else
	{
		m_nListPreIndex = -1;
		m_toolTips.DelTool( &m_listCtrl );
	}
	
	CDialog::OnMouseMove(nFlags, point);
}

void CDlgRetailPackFee::ShowTips(int iItemData)
{
	Constraint* pCon = NULL;
	CString strTips;

	int nDBIdx;
	BOOL bOwn;
    GetDBIdx( nDBIdx,bOwn );

	if( !bOwn)	return;
	// TRACE("nDBIdx = %d\r\n", nDBIdx);
	
	switch( m_enumType )
	{
	case RETAILPERCENTAGEFEES:
		{
			CProcEconomicDataElement* pEntry = (CProcEconomicDataElement*)m_pDB->getItem( nDBIdx );
			CRetailFeeData* pData = (CRetailFeeData*)pEntry->getData();
			pCon = pData->m_vList[ iItemData ].m_pPaxType;
			break;
		}
	case PACKINGFEES:
		{
			CProcEconomicDataElement* pEntry = (CProcEconomicDataElement*)m_pDB->getItem( nDBIdx );
			CParkingFeeData* pData = (CParkingFeeData*)pEntry->getData();
			pCon = pData->m_vList[ iItemData ].m_pPaxType;
			break;
		}
	default:
		{
			pCon = NULL;
			break;
		}
	}

	if( pCon )
	{
		pCon->screenTips( strTips );
		m_toolTips.ShowTips( strTips );
	}	
}
