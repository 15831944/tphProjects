// DlgCurbsideAssign.cpp : implementation file
//

#include "stdafx.h"
#include "TermPlan.h"
#include "DlgCurbsideAssign.h"
#include "../Landside/CurbsideDetail.h"
#include "../Landside/CurbSideFlight.h"
#include "../Landside/CurbSideFlightList.h"
#include "../Landside/CurbsideTime.h"
#include "DlgTimeRange.h"
#include "Resource.h"
// #include "FlightDialog.h"
#include "PassengerTypeDialog.h"
#include "../Common/FLT_CNST.H"
#include "DlgCurbsideSelect.h"
#include "TermPlanDoc.h"
#include "../Landside/InputLandside.h"
#include "../Landside/LandsideLayoutObject.h"
#include "../Common/TimeRange.h"
// CDlgCurbsideAssign dialog

IMPLEMENT_DYNAMIC(CDlgCurbsideAssign, CXTResizeDialog)
CDlgCurbsideAssign::CDlgCurbsideAssign(CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgCurbsideAssign::IDD, pParent)
{
}
CDlgCurbsideAssign::CDlgCurbsideAssign(CTermPlanDoc *tmpDoc,CWnd* pParent /*=NULL*/)
	: CXTResizeDialog(CDlgCurbsideAssign::IDD, pParent)
{
    pDoc=tmpDoc;
}
CDlgCurbsideAssign::~CDlgCurbsideAssign()
{
}

void CDlgCurbsideAssign::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CURBSIDE, m_listCtrl);
	DDX_Control(pDX, IDC_TREE_CURBSIDE, m_treeCtrl);
	
}

BOOL CDlgCurbsideAssign::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();
	if(!m_treeToolBar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)
		|| !m_treeToolBar.LoadToolBar(IDR_ACTYPE_ALIASNAME))
	{
		TRACE(_T("Create tool bar error "));
		return FALSE;
	}
  

	if (!m_listToolBar.CreateEx(this,TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP)
		|| !m_listToolBar.LoadToolBar(IDR_PEOPLEMOVEBAR))
	{
		TRACE(_T("Create tool bar error"));
		return FALSE;
	}

	InitToolBar();
	m_treeToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_ADD,TRUE);
	m_treeToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_DEL,FALSE);
	m_treeToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_EDIT,FALSE);

	m_listToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_NEW,FALSE) ;
	m_listToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_CHANGE,FALSE);
	m_listToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE,FALSE );
	DWORD dwStyle = m_listCtrl.GetExtendedStyle();
	dwStyle |= LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_GRIDLINES;
	m_listCtrl.SetExtendedStyle(dwStyle);
    m_pLandSide=pDoc->getARCport()->m_pInputLandside;
	// set the runway assign list header contents
	CStringList strList;
	LV_COLUMNEX lvc;
	lvc.mask = LVCF_WIDTH|LVCF_TEXT;
	lvc.pszText = _T("Curbside");
	lvc.cx = 100;
	lvc.fmt = LVCFMT_NOEDIT;
	lvc.csList = &strList;
	m_listCtrl.InsertColumn(0, &lvc);

	lvc.pszText = _T("Percent");
	lvc.fmt = LVCFMT_NUMBER;
	m_listCtrl.InsertColumn(1, &lvc);
	
	InitTree();

	SetResize(IDC_STATIC_CURBSIDEFLIGHT, SZ_TOP_LEFT, SZ_BOTTOM_LEFT);
	SetResize(m_treeToolBar.GetDlgCtrlID(), SZ_TOP_LEFT, SZ_TOP_LEFT);
	SetResize(IDC_TREE_CURBSIDE, SZ_TOP_LEFT, SZ_BOTTOM_LEFT);

	SetResize(IDC_STATIC_CURBSIDEDETAIL,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);
	SetResize(m_listToolBar.GetDlgCtrlID(),SZ_TOP_LEFT,SZ_TOP_LEFT);
	SetResize(IDC_LIST_CURBSIDE,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT);

	SetResize(IDC_BUTTON_CURBSAVE,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_CURBOK,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetResize(IDC_BUTTON_CURBCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),TRUE);
	SetIcon(LoadIcon( AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME) ),FALSE);


	GetDlgItem(IDC_STATIC_CURBSIDEFLIGHT)->SetWindowText("Passenger type");
// 	m_paxList.setInputTerminal(pDoc->GetTerminal());

return true;
	
}
void CDlgCurbsideAssign::InitTree()
{
	CCurbSidePax *tmpPax;
// 	CCurbsideTime   *timeRang;
	HTREEITEM hParentItem=NULL;
	HTREEITEM hChildItem=NULL;
	CString strTimeName;
	m_treeCtrl.DeleteAllItems();
	CString strPaxType;
	//m_treeCtrl.SetRedraw(FALSE);
	int j=0;
	for (int i=0;i<m_pLandSide->getCurbsidePaxTypeList().getItemCount();i++)
	{
		tmpPax=m_pLandSide->getCurbsidePaxTypeList().getItem(i);
		tmpPax->getMobElemContraint().screenPrint(strPaxType.GetBuffer(1024));
		if (hParentItem==NULL)
		{
			hParentItem=m_treeCtrl.InsertItem(strPaxType,TVI_ROOT);
			m_treeCtrl.SetItemData(hParentItem,(DWORD_PTR) tmpPax);
		}
		else 
		{
			hParentItem=m_treeCtrl.InsertItem(strPaxType,TVI_ROOT,hParentItem);
			m_treeCtrl.SetItemData(hParentItem,(DWORD_PTR)tmpPax);
		}
   
// 		for ( j=0;j<tmpPax->getItemCount();j++)
// 		{
// 			 timeRang=tmpPax->getItem(j);
// 			 timeRang->getName(strTimeName);
// 			 if(j==0)
// 			 {
// 				 
// 				 hChildItem=m_treeCtrl.InsertItem(strTimeName,hParentItem);
// 			 }
// 			 else 
// 			 {
// 				 hChildItem=m_treeCtrl.InsertItem(strTimeName,hParentItem);
// 			 }
// 			 m_treeCtrl.SetItemData(hChildItem,(DWORD_PTR)timeRang);
// 			 m_treeCtrl.Expand(hChildItem,TVE_EXPAND);
// 		}
		m_treeCtrl.Expand(hParentItem,TVE_EXPAND);
	}
	//m_treeCtrl.SetRedraw(TRUE);
}
void CDlgCurbsideAssign::InitToolBar()
{
	CRect rect; 
	CRect listRect;
	m_treeCtrl.GetWindowRect(&rect);
	ScreenToClient(&rect);
	rect.top=rect.top-26;
	rect.left=rect.left+2;
	rect.bottom=rect.top+22;
	m_treeToolBar.MoveWindow(rect);
	m_treeToolBar.ShowWindow(SW_SHOW);
	//m_treeCtrl(&rect);
	m_listCtrl.GetWindowRect(&listRect);
	ScreenToClient(&listRect);
	listRect.top=listRect.top-26;
	listRect.left=listRect.left+4;
	listRect.bottom=listRect.top+22;
	m_listToolBar.MoveWindow(listRect);
	
}
void CDlgCurbsideAssign::OnTvnSelChangedFlight(NMHDR*pNMHDR,LRESULT *pResult)
{
	HTREEITEM selectItem = m_treeCtrl.GetSelectedItem();
	if (selectItem==NULL)
	{
		m_listToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_NEW,FALSE) ;
		return;
	}
	m_listToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_NEW) ;
// 	CString strPaxName;
// 	StrCpy("DEFAULT",strPaxName);
	LPNMTREEVIEW pNMTreeView=reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	UpdateTreeToolBar();
	OnSelChangedFltTimeTree(pNMHDR,pResult);
	*pResult=0;
// 	if (m_treeCtrl.GetItemText(selectItem)=="DEFAULT")
// 	{
// 		m_listToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_NEW) ;
// 		m_listToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_CHANGE,FALSE);
// 		m_listToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE,FALSE );
// 	}else
// 	{
// 		m_listToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_NEW) ;
// 		m_listToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_CHANGE);
// 		m_listToolBar.GetToolBarCtrl().EnableButton( ID_PEOPLEMOVER_DELETE );
// 	}

}
void CDlgCurbsideAssign::OnSelChangedFltTimeTree(NMHDR *pNMHDR, LRESULT *pResult)
{
// 	CCurbsideTime *timeRange;
	HTREEITEM paxTypeItem=NULL;
	m_listCtrl.DeleteAllItems();
	paxTypeItem=m_treeCtrl.GetSelectedItem();
	if(paxTypeItem!=NULL)
	{
// 		timeRange=(CCurbsideTime*)m_treeCtrl.GetItemData(m_hTimeRangeTreeItem);
		CCurbSidePax *paxType = (CCurbSidePax *)m_treeCtrl.GetItemData(paxTypeItem);
        CString strPercent;
		for(int i=0;i<paxType->getItemCount();i++)
		{
			CCurbsideDetail *detail=paxType->getItem(i);
			m_listCtrl.InsertItem(i,"");
			m_listCtrl.SetItemText(i,0,detail->getName());
			m_listCtrl.SetItemData(m_listCtrl.GetItemCount()-1,(DWORD_PTR)detail);
			strPercent.Format("%d",detail->getPecent());
			m_listCtrl.SetItemText(i,1,strPercent);
		}
	}
}
/*
BOOL CDlgCurbsideAssign::getTreeItemByType(TreeNodeType nodeType,HTREEITEM &hItem )
{
	HTREEITEM selectItem=m_treeCtrl.GetSelectedItem();
	HTREEITEM hParentItem;
	if (selectItem==NULL)
		return false;
	switch(nodeType)
	{
	case TREENODE_FLTTYPE:
		hParentItem=m_treeCtrl.GetParentItem(selectItem);
		if (hParentItem!=NULL)
		{
			return FALSE;
		}
		hItem=selectItem;
		break;
	case TREENODE_TIMERANGE:
		hParentItem=m_treeCtrl.GetParentItem(selectItem);
		if(hParentItem==NULL)
		{
			return FALSE;
		}
		hItem=selectItem;

		break;
	default:
		return FALSE;
		

	}
	return TRUE;
}*/

/*
void CDlgCurbsideAssign::OnContextMenu(CWnd* pWnd, CPoint pos)
{
     CRect rect;
	 m_treeCtrl.GetWindowRect(&rect);
	 if (!rect.PtInRect(pos))
		 return;
	 m_treeCtrl.SetFocus();
	 CPoint pt =pos;
	 m_treeCtrl.ScreenToClient(&pt);
	 UINT iRet;
	 HTREEITEM hRClickItem=m_treeCtrl.HitTest(pt,&iRet);
	 if(iRet!=TVHT_ONITEMLABEL)
		hRClickItem=NULL;
	 if (hRClickItem==NULL)
		 return;
	 m_treeCtrl.SelectItem(hRClickItem);
	 CMenu menu;
	 menu.LoadMenu(IDR_MENU27);

	// menu.LoadMenu(IDR_CTX_MENU_STRATEGY_TIMERANGE);
	 CMenu *pMenu=menu.GetSubMenu(0);
	 if(getTreeItemByType(TREENODE_TIMERANGE,m_hTimeRangeTreeItem))
	 {
		 pMenu->EnableMenuItem(ID_LANDSIDE_NEWTIMERANGE,MF_GRAYED);
	 }
	 else if(getTreeItemByType(TREENODE_FLTTYPE,m_hTimeRangeTreeItem))
	 {
		 pMenu->EnableMenuItem(ID_LANDSIDE_DELTIMERANGE,MF_GRAYED);
		 pMenu->EnableMenuItem(ID_LANDSIDE_EDITTIMERAGE,MF_GRAYED);

	 }
    if (pMenu)
		pMenu->TrackPopupMenu(TPM_LEFTALIGN,pos.x,pos.y,this);


}
*/
void CDlgCurbsideAssign::OnNewPaxType()
{
    CPassengerTypeDialog dlg( m_pParentWnd ,FALSE,TRUE);
	this->EnableWindow(FALSE);
	if( dlg.DoModal() == IDOK )
	{
		CMobileElemConstraint paxConstr = dlg.GetMobileSelection();
		char szBuffer[1024] = {0};
		paxConstr.screenPrint(szBuffer);

		CString strBuffer = szBuffer;
		HTREEITEM hItem = m_treeCtrl.GetRootItem();
		while (hItem)
		{
			if (m_treeCtrl.GetItemText(hItem) == strBuffer)
			{
				MessageBox(_T("Passenger Type: \" ") +strBuffer+ _T(" \" already exists."));
				this->EnableWindow(TRUE);
				this->SetActiveWindow();
				return;
			}
			hItem = m_treeCtrl.GetNextItem(hItem , TVGN_NEXT);
		}

	   CCurbSidePax *tmpPax=new CCurbSidePax();
	   paxConstr.WriteSyntaxStringWithVersion(szBuffer);
	   tmpPax->setInputTerminal(&pDoc->GetTerminal());
	   tmpPax->setAirportDataBase(pDoc->GetTerminal().m_pAirportDB);
	   tmpPax->setName(szBuffer);
	  // m_treeCtrl.SetRedraw(FALSE);
	  hItem=m_treeCtrl.InsertItem(strBuffer,TVI_ROOT);
	   m_treeCtrl.SetItemData(hItem,(DWORD_PTR)tmpPax);
	  
	   m_pLandSide->getCurbsidePaxTypeList().AddItem(tmpPax);
	   //m_treeCtrl.SetRedraw(true);
	   m_treeCtrl.Expand(hItem,TVE_EXPAND);
	   m_treeCtrl.SelectItem(hItem);
	   if (m_listCtrl.GetItemCount()>=0)
	   {
		   m_listCtrl.DeleteAllItems();
	   }
	   /*m_pLandSide->getCurbsideFlightTypeList().Sort();
	   InsertTreeItemorder(tmpFlight);*/
	}	
	this->EnableWindow(TRUE);
	this->SetActiveWindow();
}
void CDlgCurbsideAssign::InsertTreeItemorder(CCurbSideFlight *tmpFligType)
{
	CString strBuffer;
	HTREEITEM hBrotherItem;
	CCurbSideFlight *flightType=NULL;
	for (int i=0;i<m_pLandSide->getCurbsideFlightTypeList().getItemCount();i++)
	{
		if (i==0&&m_pLandSide->getCurbsideFlightTypeList().getItem(i)==tmpFligType)
		{
			break;
		}
		if (m_pLandSide->getCurbsideFlightTypeList().getItem(i)==tmpFligType)
		{
			flightType=m_pLandSide->getCurbsideFlightTypeList().getItem(i-1);
			break;
		}
	}
	HTREEITEM HparentItem=NULL;
	if (flightType)
	{
		HparentItem=m_treeCtrl.GetRootItem();
	}
	
	while (true&&flightType)
	{
		CCurbSideFlight *fType=(CCurbSideFlight*)m_treeCtrl.GetItemData(HparentItem);
		if (fType==flightType)
		{
			break;
		}
		HparentItem=m_treeCtrl.GetNextItem(HparentItem,TVGN_NEXT);
	}
	tmpFligType->getFlightContraint().screenPrint(strBuffer.GetBuffer(1024));
	if (HparentItem)
	{

		hBrotherItem=m_treeCtrl.InsertItem(strBuffer,TVI_ROOT,HparentItem);
	}
	else
	{

		hBrotherItem=m_treeCtrl.InsertItem(strBuffer,TVI_ROOT);
	}
	m_treeCtrl.SetItemData(hBrotherItem,(DWORD_PTR)tmpFligType);
	m_treeCtrl.SetItemState(hBrotherItem,TVIS_SELECTED,TVIS_STATEIMAGEMASK);
	
	HTREEITEM hChildItem;
	CString strFltTimeName;
	for ( i=0;i<tmpFligType->getItemCount();i++)
	{
		CCurbsideTime *timeRange=tmpFligType->getItem(i);
		timeRange->getName(strFltTimeName);
		if (hChildItem=NULL)
		{
			hChildItem=m_treeCtrl.InsertItem(strFltTimeName,hBrotherItem);
		}
		else
		{
			hChildItem=m_treeCtrl.InsertItem(strFltTimeName,hBrotherItem,hChildItem);
		}
		m_treeCtrl.SetItemData(hChildItem,(DWORD_PTR)timeRange);
		m_treeCtrl.Expand(hChildItem,TVE_EXPAND);
	}
	m_treeCtrl.Expand(hBrotherItem,TVE_EXPAND);
}
void CDlgCurbsideAssign::OnEditPaxType()
{
// 	 if(!getTreeItemByType(TREENODE_FLTTYPE,m_hTimeRangeTreeItem))
// 		 return;
// 	 if (m_hTimeRangeTreeItem==NULL)
// 		 return;
	// HTREEITEM hItem;
	HTREEITEM paxTypeItem=NULL;
	paxTypeItem=m_treeCtrl.GetSelectedItem();
	if (!paxTypeItem)
		return;

	CCurbSidePax *tmpPax=(CCurbSidePax*)m_treeCtrl.GetItemData(paxTypeItem);
	CPassengerTypeDialog dlg( m_pParentWnd ,FALSE,TRUE);

	if( dlg.DoModal() == IDOK )
	{
	 	CMobileElemConstraint paxConstr = dlg.GetMobileSelection();
	 	char szBuffer[1024] = {0};
	 	paxConstr.screenPrint(szBuffer);

	 	CString strBuffer = szBuffer;
	 	HTREEITEM hItem = m_treeCtrl.GetRootItem();
	 	while (hItem)
	 	{
	 		if (m_treeCtrl.GetItemText(hItem) == strBuffer)
	 		{
	 			MessageBox(_T("Flight Type: \" ") +strBuffer+ _T(" \" already exists."));
	 			return;
	 		}
	 		hItem = m_treeCtrl.GetNextItem(hItem , TVGN_NEXT);
	 	}
	 	paxConstr.WriteSyntaxStringWithVersion(szBuffer);
	 	tmpPax->setName(szBuffer);
	 	m_treeCtrl.SetItemText(paxTypeItem,strBuffer);
	//m_treeCtrl.DeleteItem(m_hTimeRangeTreeItem);
	/*m_pLandSide->getCurbsideFlightTypeList().Sort();
	InsertTreeItemorder(tmpFlight);*/
	}
	 
}
void CDlgCurbsideAssign::OnDelPaxType()
{

// 	if (!getTreeItemByType(TREENODE_FLTTYPE,m_hTimeRangeTreeItem)||!m_hTimeRangeTreeItem)
//	return;
 //	 HTREEITEM hitem;
	HTREEITEM paxTypeItem=NULL;
	paxTypeItem=m_treeCtrl.GetSelectedItem();
	if (!paxTypeItem)
		return;
   	CCurbSidePax *tmpPax;
   	tmpPax=(CCurbSidePax*)m_treeCtrl.GetItemData(paxTypeItem);
   	for(int i=0;i<m_pLandSide->getCurbsidePaxTypeList().getItemCount();i++)
   	{
   		if(tmpPax==m_pLandSide->getCurbsidePaxTypeList().getItem(i))
   		{
   			m_pLandSide->getCurbsidePaxTypeList().deleteItem(i);
   			m_treeCtrl.DeleteItem(paxTypeItem);
   			m_listCtrl.DeleteAllItems();
   			break;
   		}
   	}
      
   

}
void CDlgCurbsideAssign::UpdateTreeToolBar()
{
	BOOL bEnabled=TRUE; //(m_treeCtrl.GetItemText(m_treeCtrl.GetSelectedItem())!="DEFAULT");//getTreeItemByType(TREENODE_FLTTYPE,m_hTimeRangeTreeItem);
	m_treeToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_DEL,bEnabled);
    m_treeToolBar.GetToolBarCtrl().EnableButton(ID_ACTYPE_ALIAS_EDIT,bEnabled);
}
/*
void CDlgCurbsideAssign::OnNewTimeRange()
{

	ElapsedTime startTime ;
	ElapsedTime endtime ;
	HTREEITEM hItem;
	if(!getTreeItemByType(TREENODE_FLTTYPE,m_hTimeRangeTreeItem)||!m_hTimeRangeTreeItem)
		return ;
	CDlgTimeRange DlgTimeRanger(startTime,endtime) ;
	CCurbsideTime *timeRange=new CCurbsideTime();
	CCurbSideFlight *tmpFlight=(CCurbSideFlight*)m_treeCtrl.GetItemData(m_hTimeRangeTreeItem);
	hItem=m_treeCtrl.GetChildItem(m_hTimeRangeTreeItem);
	if(DlgTimeRanger.DoModal() == IDOK)
	{
		CString strTimeRange;
		TimeRange tRange(DlgTimeRanger.GetStartTime(), DlgTimeRanger.GetEndTime());	
		strTimeRange.Format("%s - %s", DlgTimeRanger.GetStartTimeString(), DlgTimeRanger.GetEndTimeString());
		while (hItem)
		{
			if (!IsvalidTimeRange(m_treeCtrl.GetItemText(hItem), strTimeRange)&&m_hTimeRangeTreeItem!=hItem)
			{
				MessageBox(_T("TimeRange: \" ") +strTimeRange+ _T(" \" invalid."));
				return;
			}
			hItem = m_treeCtrl.GetNextItem(hItem , TVGN_NEXT);
		}
		timeRange->setName(strTimeRange);
		timeRange->setParentID(tmpFlight->getID());
		timeRange->SetTimeRange(tRange);
		tmpFlight->AddItem(timeRange);
		HTREEITEM hItem=m_treeCtrl.InsertItem(strTimeRange,m_hTimeRangeTreeItem);
		m_treeCtrl.SetItemData(hItem,(DWORD_PTR) timeRange);
		m_treeCtrl.SelectItem(hItem);
		m_treeCtrl.Expand(m_hTimeRangeTreeItem,TVE_EXPAND) ;
		if (m_listCtrl.GetItemCount()>=0)
		{
			m_listCtrl.DeleteAllItems();
		}
		
	}
}
*/
void CDlgCurbsideAssign::UpdateListToolBar()
{
	POSITION pos = m_listCtrl.GetFirstSelectedItemPosition();
    BOOL bEnable = TRUE;
	if(pos == NULL)
		bEnable = FALSE;

	m_listToolBar.GetToolBarCtrl().EnableButton(ID_PEOPLEMOVER_CHANGE, bEnable);
	m_listToolBar.GetToolBarCtrl().EnableButton(ID_PEOPLEMOVER_DELETE, bEnable);
}
void CDlgCurbsideAssign::OnLvnItemChangedCurbSideList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LV_DISPINFO* plvDispInfo = (LV_DISPINFO *)pNMHDR;
	LV_ITEM* plvItem = &plvDispInfo->item;

	int nItem = plvItem->iItem;
	int nSubItem = plvItem->iSubItem;
	if (nItem < 0)
		return;

	//OnListItemChanged(nItem, nSubItem);
	UpdateListToolBar();
	*pResult = 0;

}
/*
void CDlgCurbsideAssign::OnEditTimeRange()
{
	ElapsedTime startTime;
	ElapsedTime endTime;
	HTREEITEM hItem;
	if(!getTreeItemByType(TREENODE_TIMERANGE,m_hTimeRangeTreeItem)||!m_hTimeRangeTreeItem)
		return ;
	CDlgTimeRange dlg(startTime,endTime);
	CCurbsideTime *timeRange=(CCurbsideTime*)m_treeCtrl.GetItemData(m_hTimeRangeTreeItem);
	hItem=m_treeCtrl.GetChildItem(m_treeCtrl.GetParentItem(m_hTimeRangeTreeItem));
    if (dlg.DoModal()==IDOK)
	{
		CString strTimeRange;
		TimeRange tRange(dlg.GetStartTime(), dlg.GetEndTime());	
		strTimeRange.Format("%s - %s", dlg.GetStartTimeString(), dlg.GetEndTimeString());
		while (hItem)
		{
			if (!IsvalidTimeRange(m_treeCtrl.GetItemText(hItem), strTimeRange)&&m_hTimeRangeTreeItem!=hItem)
			{
				MessageBox(_T("TimeRange: \" ") +strTimeRange+ _T(" \" invalid."));
				return;
			}
			hItem = m_treeCtrl.GetNextItem(hItem , TVGN_NEXT);
		}
		timeRange->setName(strTimeRange);
		timeRange->SetTimeRange(tRange);
		m_treeCtrl.SetItemText(m_hTimeRangeTreeItem,strTimeRange);
	}

}
*/
/*
void CDlgCurbsideAssign::OnDelTimeRange()
{
	if (!getTreeItemByType(TREENODE_TIMERANGE,m_hTimeRangeTreeItem)||!m_hTimeRangeTreeItem)
		return;
	HTREEITEM hParentItem=m_treeCtrl.GetParentItem(m_hTimeRangeTreeItem);
	CCurbsideTime *timeRange=(CCurbsideTime*)m_treeCtrl.GetItemData(m_hTimeRangeTreeItem);
	CCurbSideFlight *tmpFlight=(CCurbSideFlight*)m_treeCtrl.GetItemData(hParentItem);
	for(int i=0;i<tmpFlight->getItemCount();i++)
	{
		if (timeRange==tmpFlight->getItem(i))
		{
			tmpFlight->deleteItem(i);
			m_treeCtrl.DeleteItem(m_hTimeRangeTreeItem);
			m_listCtrl.DeleteAllItems();
			break;
		}
	}
}

*/
void CDlgCurbsideAssign::OnSave()
{
	int nCount = 0;
	int iTotals = 0;
	/*nCount = m_listCtrl.GetItemCount();
	if (nCount > 0)
	{
		for (int i = 0; i < nCount ; i++)
		{
			iTotals += _ttol(m_listCtrl.GetItemText(i,1) );
		}
		if (iTotals != 100)
		{
			MessageBox(_T("The sum of operation percent should equal 100%"));
			return;
		}
	}*/
	for (int i=0;i<m_pLandSide->getCurbsidePaxTypeList().getItemCount();i++)
	{
		iTotals=0;
		CCurbSidePax *paxType=m_pLandSide->getCurbsidePaxTypeList().getItem(i);
		for (int j=0;j<paxType->getItemCount();j++)
		{
// 			iTotals=0;
// 			CCurbsideTime *timeRange=paxType->getItem(j);
// 			for (int k=0;k<timeRange->getItemCount();k++)
// 			{
// 				CCurbsideDetail *detail=timeRange->getItem(k);
// 				iTotals+=detail->getPecent();
// 			}			

			CCurbsideDetail *detail=paxType->getItem(j);
			iTotals+=detail->getPecent();
		}
		if (iTotals!=100 && paxType->getItemCount()>0)
		{
			MessageBox(_T("The sum of operation percent should equal 100%"));
			return;
		}
	}
	m_pLandSide->getCurbsidePaxTypeList().SaveData();
	if (m_pLandSide->getCurbsidePaxTypeList().getItemCount()<=0)
	{
		m_pLandSide->getCurbsidePaxTypeList().ReadData();
	}
}
void CDlgCurbsideAssign::OnOk()
{
	OnSave();
	CXTResizeDialog::OnOK();
	
}
void CDlgCurbsideAssign::OnCancel()
{
	m_pLandSide->getCurbsidePaxTypeList().ReadData();
	CXTResizeDialog::OnCancel();
}
void CDlgCurbsideAssign::OnNewCurbSide()
{
		
// 	if (!getTreeItemByType(TREENODE_TIMERANGE,m_hTimeRangeTreeItem)||!m_hTimeRangeTreeItem)
// 		return;
	HTREEITEM paxTypeItem=NULL;
	paxTypeItem=m_treeCtrl.GetSelectedItem();
	if (!paxTypeItem)
		return;
    LandsideLayoutObjectList *objectList= &(pDoc->getARCport()->m_pInputLandside->getObjectList());
	CDlgLandsideObjectSelect dlg(objectList,ALT_LCURBSIDE,m_pParentWnd);
	int nCount = m_listCtrl.GetItemCount();	
	this->EnableWindow(FALSE);
	
	if (dlg.DoModal()==IDOK)
	{
		CString strExistName;
		CCurbsideDetail *tmpType;
		int i = 0;
		for (i = 0; i < nCount ; i++)
		{
            tmpType = (CCurbsideDetail *)m_listCtrl.GetItemData(i);
			if(tmpType->getName().Trim() == dlg.getCurbSideName().Trim())
			{
				MessageBox(_T("CurbSide already exists in the list."));
				this->EnableWindow(TRUE);
				this->SetActiveWindow();
				return;
			}
		}
		m_listCtrl.InsertItem(m_listCtrl.GetItemCount(),"");
		CString strPercent = _T("");
		CCurbsideDetail *detail=new CCurbsideDetail();
		i = 0;
		strPercent.Format(_T("%d"),100/(nCount + 1));
		for (i = 0; i < nCount + 1; i++)
		{
			if(i == nCount)
				strPercent.Format(_T("%d"),100-nCount*(100/(nCount + 1)));
			m_listCtrl.SetItemText(i,1,strPercent);
		}
		//CCurbsideTime *timeRange=new CCurbsideTime();
// 		CCurbsideTime *timeRange=(CCurbsideTime*)m_treeCtrl.GetItemData(m_hTimeRangeTreeItem);
		CCurbSidePax *paxType = (CCurbSidePax *)m_treeCtrl.GetItemData(paxTypeItem);
		paxType->AddItem(detail);
		//detail->setCurbSideID(dlg.getLayoutObject()->getID());
		detail->setName(dlg.getCurbSideName());
		m_listCtrl.SetItemText(m_listCtrl.GetItemCount()-1,0,dlg.getCurbSideName());
		m_listCtrl.SetItemData(m_listCtrl.GetItemCount()-1,(DWORD_PTR)detail);
		i = 0;
		for(int j=0;j<paxType->getItemCount();j++)
		{
			CCurbsideDetail *itrPercent=paxType->getItem(j);
			if(i == nCount)
				//(itrPercent) = 1.0*100-1.0*nCount*100/(nCount + 1);
				itrPercent->setPercent(100-nCount*(100/(nCount + 1)));
			else
			//	(itrPercent)->m_dPercent = 1.0*100/(nCount + 1);
			itrPercent->setPercent(100/(nCount + 1));
			itrPercent->setParentID(paxType->getID());			 
		}
		//HTREEITEM m_treeCtrl.GetSelectedItem();
// 		CCurbSidePax *paxType=(CCurbSidePax *)m_treeCtrl.GetItemData(m_treeCtrl.GetSelectedItem());
		//CCurbSidePax *paxType=m_pLandSide->getCurbsidePaxTypeList().getItem(iii);
// 		paxType->AddItem(timeRange);
	
	}
	this->EnableWindow(TRUE);
	this->SetActiveWindow();
	

}
void CDlgCurbsideAssign::OnEditCurbSide()
{
//    if (!getTreeItemByType(TREENODE_TIMERANGE,m_hTimeRangeTreeItem)||!m_hTimeRangeTreeItem)
//    {
// 	   return ;
//    }

	HTREEITEM paxTypeItem = NULL;
	paxTypeItem = m_treeCtrl.GetSelectedItem();
	if (!paxTypeItem)
	{
		return;
	}
	int nCursel=m_listCtrl.GetCurSel();
	if (nCursel<0)
		return;
// 	CCurbsideTime *timeRange=(CCurbsideTime*)m_treeCtrl.GetItemData(m_hTimeRangeTreeItem);
	CCurbSidePax *paxType = (CCurbSidePax *)m_treeCtrl.GetItemData(paxTypeItem);
	CCurbsideDetail *detail=paxType->getItem(nCursel);
	LandsideLayoutObjectList *objectList=&(pDoc->getARCport()->m_pInputLandside->getObjectList());
	CDlgLandsideObjectSelect dlg(objectList,ALT_LCURBSIDE,m_pParentWnd);
	if (dlg.DoModal()==IDOK)
	{
		detail->setName(dlg.getCurbSideName());
		//detail->setCurbSideID(dlg.getLayoutObject()->getID());
		m_listCtrl.SetItemText(nCursel,0,dlg.getCurbSideName());

	}
}
void CDlgCurbsideAssign::OnDelCurbSide()
{
    int nCursel=m_listCtrl.GetCurSel();
	if (nCursel<0)
		return;
	m_listCtrl.DeleteItem(nCursel);
// 	if (!getTreeItemByType(TREENODE_TIMERANGE,m_hTimeRangeTreeItem)||!m_hTimeRangeTreeItem)
// 		return;
	HTREEITEM paxTypeItem = NULL;
	paxTypeItem = m_treeCtrl.GetSelectedItem();
	if (!paxTypeItem)
	{
		return;
	}
// 	CCurbsideTime *timeRange=(CCurbsideTime*)m_treeCtrl.GetItemData(m_hTimeRangeTreeItem);
	CCurbSidePax *paxType = (CCurbSidePax *)m_treeCtrl.GetItemData(paxTypeItem);
	paxType->deleteItem(nCursel);
	int alTotal=paxType->getItemCount();
	if (alTotal<=0)
	{
		return;
	}
	int averPercent=100/alTotal;
	CCurbsideDetail *detail;
	CString strPercent="";
	for (int i=0;i<alTotal-1;i++)
	{

		detail=paxType->getItem(i);
		detail->setPercent(averPercent);
		strPercent.Format("%d",averPercent);
		m_listCtrl.SetItemText(i,1,strPercent);
	}
	detail=paxType->getItem(i);
	detail->setPercent(100-(i*averPercent));
	strPercent.Format("%d",detail->getPecent());
	m_listCtrl.SetItemText(i,1,strPercent);
}
void CDlgCurbsideAssign::OnLvnEndlabeleditListContents(NMHDR *pNMHDR, LRESULT *pResult)
{
	int iTotals=0,nCount=0;
	LV_DISPINFO *plvDispInfo=(LV_DISPINFO*)pNMHDR;
	LV_ITEM *plvItem=&plvDispInfo->item;
	int nItem=plvItem->iItem;
	int nsubItem=plvItem->iSubItem;
	if (nItem<0)
		return ;
// 	if(!getTreeItemByType(TREENODE_TIMERANGE,m_hTimeRangeTreeItem)||!m_hTimeRangeTreeItem)
// 		return;
	HTREEITEM paxTypeItem = NULL;
	paxTypeItem = m_treeCtrl.GetSelectedItem();
	if (!paxTypeItem)
	{
		return;
	}
	CCurbSidePax *paxType=(CCurbSidePax*)m_treeCtrl.GetItemData(paxTypeItem);
	for (int j=0;j<paxType->getItemCount();j++)
	{
		CCurbsideDetail *detail=paxType->getItem(j);
		if(plvItem->iItem!=j)
		iTotals+=detail->getPecent();
	}
	if(iTotals+_ttoi(m_listCtrl.GetItemText(nItem,1))>100)
	{
		CString strReplace = _T("0");
		strReplace.Format(_T("%d"),100 - iTotals);
		m_listCtrl.SetItemText(nItem,1,strReplace);
		MessageBox(_T("The sum of operation percent can not exceed 100%."));
		return;
	}
	m_listCtrl.OnEndlabeledit(pNMHDR,pResult);
	OnListItemEndLabelEdit(nItem,nsubItem);
}
/*
bool CDlgCurbsideAssign::IsvalidTimeRange(CString strTimeRange1,CString strTimeRange2)
{
	CString str1 = _T("");
	CString str2 = _T("");
	str1 =  strTimeRange1.Mid(strTimeRange1.Find('-') + 1);
	str2 = strTimeRange2.Left(strTimeRange2.Find('-'));
	str1.Trim();
	str2.Trim();
	str1 = str1.Left(str1.Find(' '));
	str2 = str2.Left(str2.Find(' '));
	str1.MakeLower();
	str2.MakeLower();
	if(str1 > str2)
		return (false);
	else if(str1 < str2)
		return (true);
	str1 =  strTimeRange1.Mid(strTimeRange1.Find('-') + 1);
	str2 = strTimeRange2.Left(strTimeRange2.Find('-'));
	str1.Trim();
	str2.Trim();
	str1 = str1.Mid(str1.Find(' ') + 1);
	str2 = str2.Mid(str2.Find(' ') + 1);
	if(str1 > str2)
		return (false);
	else
		return (true);
}
*/
void CDlgCurbsideAssign::OnListItemEndLabelEdit(int nItem, int nSubItem)
{
	CString strValue = m_listCtrl.GetItemText(nItem, nSubItem);
	int nCurSel=m_listCtrl.GetCurSel();
// 	if(!getTreeItemByType(TREENODE_TIMERANGE,m_hTimeRangeTreeItem)||!m_hTimeRangeTreeItem)
// 		return;
	HTREEITEM paxTypeItem = NULL;
	paxTypeItem = m_treeCtrl.GetSelectedItem();
	if (!paxTypeItem)
	{
		return;
	}
// 	CCurbsideTime *timeRange=(CCurbsideTime*)m_treeCtrl.GetItemData(m_hTimeRangeTreeItem);
	CCurbSidePax *paxType=(CCurbSidePax*)m_treeCtrl.GetItemData(paxTypeItem);
	CCurbsideDetail *detail=paxType->getItem(nCurSel);
	detail->setPercent(atoi(strValue));
}
BEGIN_MESSAGE_MAP(CDlgCurbsideAssign, CXTResizeDialog)
	ON_NOTIFY(TVN_SELCHANGED,IDC_TREE_CURBSIDE,OnTvnSelChangedFlight)
	ON_COMMAND(ID_ACTYPE_ALIAS_ADD,OnNewPaxType)
	ON_COMMAND(ID_ACTYPE_ALIAS_DEL,OnDelPaxType)
	ON_COMMAND(ID_ACTYPE_ALIAS_EDIT,OnEditPaxType)
// 	ON_COMMAND(ID_LANDSIDE_EDITTIMERAGE,OnEditTimeRange)
// 	ON_COMMAND(ID_LANDSIDE_NEWTIMERANGE,OnNewTimeRange)
// 	ON_COMMAND(ID_LANDSIDE_DELTIMERANGE,OnDelTimeRange)
	ON_COMMAND(ID_PEOPLEMOVER_NEW,OnNewCurbSide)
	ON_COMMAND(ID_PEOPLEMOVER_CHANGE,OnEditCurbSide)
	ON_COMMAND(ID_PEOPLEMOVER_DELETE,OnDelCurbSide)
	ON_BN_CLICKED(IDC_BUTTON_CURBSAVE,OnSave)
	ON_BN_CLICKED(IDC_BUTTON_CURBOK,OnOk)
	ON_BN_CLICKED(IDC_BUTTON_CURBCANCEL,OnCancel)
	ON_NOTIFY(LVN_ITEMCHANGED,IDC_LIST_CURBSIDE,OnLvnItemChangedCurbSideList)
	ON_NOTIFY(LVN_ENDLABELEDIT,IDC_LIST_CURBSIDE,OnLvnEndlabeleditListContents)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()


// CDlgCurbsideAssign message handlers
