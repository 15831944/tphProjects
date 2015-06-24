// DlgRetailFees.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgRetailFees.h"
#include "TermPlanDoc.h"
#include "..\common\ProbDistEntry.h"
#include "..\common\probdistmanager.h"
#include "..\inputs\probab.h"
#include "economic\ProcEconomicData.h"
#include "economic\ProcEconDatabase.h"
#include "economic\ProcEconomicDataElement.h"
#include "PassengerTypeDialog.h"
#include "DlgProbDist.h"
#include <Common/ProbabilityDistribution.h>
#define SELECTED_COLOR RGB(53,151,53)

// CDlgRetailFees dialog

IMPLEMENT_DYNAMIC(CDlgRetailFees, CDialog)
CDlgRetailFees::CDlgRetailFees(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgRetailFees::IDD, pParent)
{	
	m_ProductStockingList.ReadData(-1);
}

CDlgRetailFees::~CDlgRetailFees()
{
}

void CDlgRetailFees::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_DEL, m_btnDel);
	DDX_Control(pDX, IDC_BUTTON_NEW, m_btnNew);
	DDX_Control(pDX, IDC_BUTTON_SAVE, m_btnSave);
	DDX_Control(pDX, IDC_TREE1, m_treeProc);
	DDX_Control(pDX, IDC_STATIC_TOOLBARCONTENTER, m_toolbarcontenter);
	DDX_Control(pDX, IDC_LIST1, m_listCtrl);
}


BEGIN_MESSAGE_MAP(CDlgRetailFees, CDialog)
	ON_WM_CREATE()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, OnSelchangedTree1)
	ON_COMMAND(ID_TOOLBARBUTTONADD, OnToolbarbuttonadd)
	ON_COMMAND(ID_TOOLBARBUTTONDEL, OnToolbarbuttondel)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST1,OnEndlabeledit)
	ON_BN_CLICKED(IDC_BUTTON_NEW, OnButtonNew)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, OnButtonSave)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, OnItemchangedList1)
	ON_BN_CLICKED(IDC_BUTTON_DEL, OnButtonDel)

END_MESSAGE_MAP()

// CDlgRetailFees message handlers


BOOL CDlgRetailFees::OnInitDialog() 
{
	CDialog::OnInitDialog();

// 	m_toolTips.InitTooltips( &m_listCtrl );
	// TODO: Add extra initialization here
	SetWindowText( "Retail" );

	m_pDB = GetEconomicManager()->m_pRetailPercRevenue;
	m_treeProc.setInputTerminal(GetInputTerminal());
	SetRetailTreeContent();
	InitToolbar();
// 	CRect rc;
//  	GetWindowRect(&rc);
// 	ScreenToClient(&rc);
// 	rc.right+=100;
// 	rc.bottom+=50;
// 	MoveWindow(rc);
// 
// 	ReloadDatabase();
// 
// 	InitToolbar();
// 
 	InitListCtrl();
// 
 	m_btnSave.EnableWindow( FALSE );
// 

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
void CDlgRetailFees::OnButtonSave()
{
	CWaitCursor wc;
	m_pDB->saveDataSet( GetProjPath(), false );
	m_btnSave.EnableWindow( FALSE );
}
void CDlgRetailFees::OnCancel()
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

void CDlgRetailFees::OnOK()
{
	if( m_btnSave.IsWindowEnabled() )
		OnButtonSave();

	CDialog::OnOK();
}
int CDlgRetailFees::OnCreate(LPCREATESTRUCT lpCreateStruct) 
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
void CDlgRetailFees::InitToolbar()
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
void CDlgRetailFees::SetRetailTreeContent()
{
	m_treeProc.DeleteAllItems();
	ProcessorList *procList = GetInputTerminal()->GetTerminalProcessorList();
	if( procList == NULL )
		return;

	StringList strDict;
 	ProcessorList *retailList=new ProcessorList(GetInputTerminal()->inStrDict);
	retailList->SetInputTerminal(GetInputTerminal());
	for (int i=0;i<procList->getProcessorCount();i++)
	{
		Processor *curProcessor=procList->getProcessor(i);
		if (curProcessor->getProcessorType()==RetailProc)
		{
			if(((RetailProcessor *)curProcessor)->GetRetailType()==RetailProcessor::Retial_Shop_Section)
			{
// 				strDict.addString(curProcessor->getIDName());
				retailList->addProcessor(curProcessor);
			}
		}
	}
// 	for (int i=0;i<strDict.getCount();i++)
// 	{
// 		AddStrToTree(strDict.getString(i));
// 	}	
	m_treeProc.LoadData(retailList,m_pDB);
    	
}

CEconomicManager *CDlgRetailFees::GetEconomicManager()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	return (CEconomicManager*)&pDoc->GetEconomicManager();
}
InputTerminal *CDlgRetailFees::GetInputTerminal()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	return (InputTerminal*)&pDoc->GetTerminal();
}
CString CDlgRetailFees::GetProjPath()
{
	CTermPlanDoc* pDoc	= (CTermPlanDoc*)((CView*)m_pParentWnd)->GetDocument();	
	return pDoc->m_ProjInfo.path;
}
void CDlgRetailFees::InitListCtrl()
{	
	DWORD dwStyle = m_listCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_listCtrl.SetExtendedStyle( dwStyle );

	LV_COLUMNEX	lvc;
	lvc.mask = LVCF_WIDTH | LVCF_TEXT ;
	char columnLabel[6][128];
	int i,m,nCount;
	int DefaultColumnWidth[] = { 120, 80,70,60,80,70 };
	int nFormat[] = {	LVCFMT_CENTER, LVCFMT_CENTER };
	CStringList strListDist,strListProduct;
	CString s;
	CProbDistEntry* pPBEntry;
	
	strcpy( columnLabel[0], "Pax Type" );		
	strcpy( columnLabel[1], "Product" );		
	strcpy( columnLabel[2], "Prob Buy (%)" );		
	strcpy( columnLabel[3], "Price/unit" );		
	strcpy( columnLabel[4], "Units distbn" );
	strcpy( columnLabel[5], "Airport cut (%)" );

	DefaultColumnWidth[1] = 120;
	s.LoadString(IDS_STRING_NEWDIST);
	strListDist.InsertAfter( strListDist.GetTailPosition(), s );
	nCount = _g_GetActiveProbMan( GetInputTerminal() )->getCount();
	for( m=0; m<nCount; m++ )
	{
		pPBEntry = _g_GetActiveProbMan( GetInputTerminal() )->getItem( m );			
		strListDist.InsertAfter( strListDist.GetTailPosition(), pPBEntry->m_csName );
	}
	
	for (i=0;i<m_ProductStockingList.GetItemCount();i++)
	{
		strListProduct.InsertAfter(strListProduct.GetTailPosition(),m_ProductStockingList.GetItem(i)->GetProductName());
	}

	for(i=0; i<6; i++ )
	{
		lvc.pszText = columnLabel[i];
		lvc.cx = DefaultColumnWidth[i];
		if( i == 0 )
		{
			lvc.fmt = LVCFMT_NOEDIT;
		}
		else if( i == 1 || i ==4)
		{
			lvc.fmt = LVCFMT_DROPDOWN;
		}
		else
		{
			lvc.fmt = LVCFMT_NUMBER;
		}
		if(i!=1)
		{
			lvc.csList = &strListDist;
			m_listCtrl.InsertColumn( i, &lvc );
		}
		else
		{
			lvc.csList = &strListProduct;
			m_listCtrl.InsertColumn( i, &lvc );
		}
	}

}
void CDlgRetailFees::ShowDefaultListData()
{

	m_listCtrl.InsertItem( 0 , "" );		
	m_listCtrl.SetItemData( 0, -1 );
	m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL,FALSE );
}
void CDlgRetailFees::ShowListData()
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
		CProcEconomicDataElement* pEntry = (CProcEconomicDataElement*)m_pDB->getItem( nDBIdx );
		CRetailFeeData* pData = (CRetailFeeData*)pEntry->getData();
		for( int i = 0; i< static_cast<int>(pData->m_vList.size()); i++ )
		{
			RetailFeeItem retailItem = pData->m_vList[i];
			CString str;
			retailItem.m_pPaxType->screenPrint( str );
			int nIdx = m_listCtrl.InsertItem( m_listCtrl.GetItemCount(), str );
			str.ReleaseBuffer();

			ProductStockingSpecification *tmpStock=m_ProductStockingList.GetByID(retailItem.m_nProductID);
			if (tmpStock!=NULL)
			{
				m_listCtrl.SetItemText(nIdx, 1,tmpStock->GetProductName());
			}

			str.Format( "%.2f",retailItem.m_dProbBuy );
			m_listCtrl.SetItemText( nIdx,2,str );


			char szDist[1024];
			str.Format( "%.2f",retailItem.m_dPrice );
			m_listCtrl.SetItemText( nIdx,3,str );

			ProbabilityDistribution* pProbDist = retailItem.m_pUnitsDist;			
			pProbDist->screenPrint( szDist );
			if( pProbDist != NULL )
				m_listCtrl.SetItemText(nIdx, 4,szDist);

			
			str.Format( "%.2f",retailItem.m_dAirportCut );
			m_listCtrl.SetItemText( nIdx,5,str );
			if( bOwn )
				m_listCtrl.SetItemData( nIdx,i+100 );
			else
				m_listCtrl.SetItemData( nIdx, -1 );	
		}
	}
	if( GetSelectedListItem() != -1 )
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL );
	else
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL,FALSE );
}
void CDlgRetailFees::GetDBIdx( int& nDBIdx, BOOL& bOwn )
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
void CDlgRetailFees::OnSelchangedTree1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here

	ShowListData();

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
void CDlgRetailFees::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	LV_ITEM* plvItem = &pDispInfo->item;
	ProbabilityDistribution* pProbDist = NULL;
	CProbDistEntry* pPDEntry = NULL;
	CProcEconomicDataElement* pEntry=NULL;

	HTREEITEM hSelItem = m_treeProc.GetSelectedItem();
	if( hSelItem == NULL )
		return;
	int nDBIdx;
	BOOL bOwn;
	GetDBIdx( nDBIdx,bOwn );

	int nIdx,nDataIdx;
	CString s;
	//ProbabilityDistribution* pProbDist = NULL;

	//nIdx = m_listCtrl.GetCurSel();
	nIdx = plvItem->iItem;
	nDataIdx = m_listCtrl.GetItemData( nIdx ) -100;
	if( nDataIdx < 0 )
		return;

	if( bOwn ) // modify data
	{
		pEntry = (CProcEconomicDataElement*)m_pDB->getItem( nDBIdx );
		CRetailFeeData* pData = (CRetailFeeData*)pEntry->getData();
		CString str;

		switch(plvItem->iSubItem)
		{
		case 0: //Pax Type
			{
				break;
			}
		case 1:
			{
				str = m_listCtrl.GetItemText( nIdx,1 );
				if(!str.IsEmpty())
				{
					ProductStockingSpecification *tmpProduct=m_ProductStockingList.GetProductByName((CString)plvItem->pszText);
					if (tmpProduct!=NULL)
					{
						pData->m_vList[nIdx].m_nProductID=tmpProduct->GetID();
					}else
					{
						pData->m_vList[nIdx].m_nProductID=-1;
					}
				}
                
				break;
			}
			
		case 2: //Prob Buy (%)
			{
				str = m_listCtrl.GetItemText( nIdx,2 );
				if( !str.IsEmpty() )
				{	
					char* csStop;
					double dValue = strtod( str,&csStop );
					if( dValue >= 0 /*&& *csStop != str[0]*/ )
						pData->m_vList[nIdx].m_dProbBuy = dValue;
				}	
				break;
			}
		case 3:
			{
				str = m_listCtrl.GetItemText( nIdx,3 );
				if( !str.IsEmpty() )
				{	
					char* csStop;
					double dValue = strtod( str,&csStop );
					if( dValue >= 0 /*&& *csStop != str[0] */)
						pData->m_vList[nIdx].m_dPrice = dValue;
				}	
				break;
			}
		case 4:
			{
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
					assert( i < nCount );
					pProbDist = pPDEntry->m_pProbDist;
				}
				if( pProbDist )
					pData->m_vList[nIdx].m_pUnitsDist= pProbDist;
				break;
			}
		case 5: //Airport cut (%)
			str = m_listCtrl.GetItemText( nIdx,5 );
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
	

	*pResult = 0;
	m_btnSave.EnableWindow();
	return;
}
int CDlgRetailFees::GetSelectedListItem()
{
	int nCount = m_listCtrl.GetItemCount();
	for( int i=0; i<nCount; i++ )
	{
		if( m_listCtrl.GetItemState( i, LVIS_SELECTED ) == LVIS_SELECTED )
			return i;
	}
	return -1;
}

void CDlgRetailFees::OnButtonNew() 
{
	// TODO: Add your control notification handler code here
	OnToolbarbuttonadd();

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
void CDlgRetailFees::OnButtonDel() 
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
// 	m_pDB = GetEconomicManager()->m_pRetailPercRevenue;
// 	m_treeProc.setInputTerminal(GetInputTerminal());
	SetRetailTreeContent();
	ShowListData();

	m_btnDel.EnableWindow(FALSE);
	m_btnSave.EnableWindow(TRUE);
	
	if(m_treeProc.hSelItem==NULL)
		return;
	HTREEITEM hParent = m_treeProc.GetParentItem(m_treeProc.hSelItem);
	if (hParent != NULL)
		m_treeProc.Expand(hParent, TVE_EXPAND);
	m_treeProc.EnsureVisible(m_treeProc.hSelItem);
	m_treeProc.SelectItem(m_treeProc.hSelItem);

	m_btnDel.EnableWindow(FALSE);

}

void CDlgRetailFees::OnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult) 
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
void CDlgRetailFees::OnToolbarbuttonadd()
{
	HTREEITEM hSelItem = m_treeProc.GetSelectedItem();
	if( hSelItem == NULL )
		return;
	if( m_listCtrl.GetItemCount() == 1 && m_listCtrl.GetItemText( 0 ,0 ) == "" )
		m_listCtrl.DeleteAllItems();

	int nDBIdx;
	BOOL bOwn;
	GetDBIdx( nDBIdx,bOwn );


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
			retailItem.m_pUnitsDist = new ConstantDistribution(0.0);
			retailItem.m_dAirportCut = 0.0;
			retailItem.m_dProbBuy = 0.0;
			retailItem.m_dPrice = 0.0;
			retailItem.m_nProductID = -1;
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
			retailItem.m_pUnitsDist = new ConstantDistribution(0.0);

			retailItem.m_dAirportCut = 0.0;
			retailItem.m_dProbBuy = 0.0;
			retailItem.m_dPrice = 0.0;
			retailItem.m_nProductID = -1;

			m_listCtrl.SetItemData( nIdx, pData->m_vList.size()+100 );
			pData->m_vList.push_back( retailItem );
		
			pEntry->setData(pData);
			m_pDB->addItem(pEntry);

			m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONADD );
		}
		m_treeProc.ResetTreeData( NULL );
		m_btnSave.EnableWindow();
	}
	ShowListData();
}
void CDlgRetailFees::OnToolbarbuttondel()
{
	int nDBIdx;
	BOOL bOwn;
	GetDBIdx( nDBIdx,bOwn );

	// only delete the data belong to this item
	if( nDBIdx == -1 || !bOwn ) 
		return;

	int nIdx = m_listCtrl.GetCurSel();

	
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
	
	ShowListData();
	m_btnSave.EnableWindow();
}
