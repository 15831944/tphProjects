// DlgTaxiInterruptTimes.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "DlgTaxiInterruptTimes.h"
#include "DlgTimeRange.h"
// CDlgTaxiInterruptTimes dialog
#include "DlgHoldShortLineSel.h"
#include "..\InputAirside\IntersectionNode.h"
#include "../InputAirside/TaxiLinkedRunwayData.h"
#include "DlgSelectRunway.h"
#include "../InputAirside/Runway.h"
#include "../MFCExControl/XTResizeDialog.h"

IMPLEMENT_DYNAMIC(CDlgTaxiInterruptTimes, CXTResizeDialog)
CDlgTaxiInterruptTimes::CDlgTaxiInterruptTimes(int ProID ,PSelectFlightType selFlightTy ,CAirportDatabase* pAirportDB, PSelectProbDistEntry pSelectProbDistEntry ,InputAirside* pInputAirside ,CWnd* pParent /*=NULL*/)
	:CXTResizeDialog(CDlgTaxiInterruptTimes::IDD, pParent)
	,m_nPrjID(ProID)
	,m_TaxiInterruptTimes(NULL) 
	,m_pSelectFlightType(selFlightTy)
	,m_pInputAirside(pInputAirside)
	,m_pSelectProbDistEntry(pSelectProbDistEntry) 
{
	m_holdShortLines.ReadData(ProID) ;
    m_TaxiInterruptTimes = new CTaxiInterruptTimes(pAirportDB) ;
	m_TaxiInterruptTimes->ReadData() ;
}

CDlgTaxiInterruptTimes::~CDlgTaxiInterruptTimes()
{
	if(m_TaxiInterruptTimes != NULL)
		delete m_TaxiInterruptTimes ;
}

void CDlgTaxiInterruptTimes::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_TOOL, m_Sta_Tool);
	DDX_Control(pDX, IDC_TREE_INTERRUPT, m_TreeCtrl);
}


BEGIN_MESSAGE_MAP(CDlgTaxiInterruptTimes, CXTResizeDialog)
	ON_WM_SIZE() 
	ON_WM_CREATE() 
	ON_COMMAND(ID_BUTTON_ADD,OnNewButton)
	ON_COMMAND(ID_BUTTON_DEL,OnDelButton)
	ON_COMMAND(ID_SAVE,OnSave)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_INTERRUPT, OnTvnSelchangedTreeHoldShortline)
END_MESSAGE_MAP()

void CDlgTaxiInterruptTimes::OnTvnSelchangedTreeHoldShortline(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	HTREEITEM hItem = m_TreeCtrl.GetSelectedItem() ;

	if(hItem == NULL)
	{
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_ADD,TRUE) ;
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_DEL,FALSE) ;
		return ;
	}
	
	CString strContent = m_TreeCtrl.GetItemText(hItem);
	if((strContent.Find("Linked Runways") > -1 && strContent.Find("Yes") > -1))
	{
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_ADD,TRUE) ;
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_DEL,FALSE) ;
	}
	else if (strContent.Find("Interrupt") > -1)
	{
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_ADD,FALSE) ;
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_DEL,TRUE) ;
	}
	else
	{
		HTREEITEM hParItem = m_TreeCtrl.GetParentItem(hItem);
		if(hParItem == NULL)
		{
			m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_ADD,TRUE) ;
			m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_DEL,TRUE) ;
			return;
		}
		
		strContent =  m_TreeCtrl.GetItemText(hParItem);
		if (strContent.Find("Linked Runways") > -1)
		{
			m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_ADD,FALSE) ;
			m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_DEL,TRUE) ;
		}
		else
		{	
			m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_ADD,FALSE) ;
			m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_DEL,FALSE) ;
		}
	}

	*pResult = 0;
}

LRESULT CDlgTaxiInterruptTimes::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
	case UM_CEW_EDITSPIN_END:
		{
			HTREEITEM hItem = (HTREEITEM)wParam;
			CString strValue = *((CString*)lParam);
			double temp = atof( strValue.GetBuffer());
			CTaxiLinkedRunwayData* pData = GetSelectedRunwayData(hItem);
			if (pData)
			{
				HTREEITEM hParItem = m_TreeCtrl.GetParentItem(hItem);
				CString strContent = m_TreeCtrl.GetItemText(m_TreeCtrl.GetParentItem(hParItem));
				if (strContent.Find("Landing") >=0)
				{
					if (m_TreeCtrl.GetNextSiblingItem(hItem) != NULL)		// dist
					{
						pData->m_dLandingHoldDist = temp*100;
						strValue.Format("%.2f m",pData->m_dLandingHoldDist/100);
					}
					else
					{
						pData->m_tLandingHoldTime = ElapsedTime(temp*60L);		// time
						strValue.Format("%d mins",pData->m_tLandingHoldTime.asMinutes());
					}
					m_TreeCtrl.SetItemText(hItem,strValue);
					strValue.Format("Allow passage when aircraft %.2f m or %d mins from interrupt line or past interrupt line", pData->m_dLandingHoldDist/100,pData->m_tLandingHoldTime.asMinutes());
					m_TreeCtrl.SetItemText(hParItem,strValue);
				
				}
				else
				{
					if (m_TreeCtrl.GetNextSiblingItem(hItem) != NULL)		// dist
					{
						pData->m_dTakeoffHoldDist = temp*100;
						strValue.Format("%.2f m",pData->m_dTakeoffHoldDist/100);
					}
					else
					{
						pData->m_tTakeoffHoldTime = ElapsedTime(temp*60L);		// time
						strValue.Format("%d mins",pData->m_tTakeoffHoldTime.asMinutes());
					}
					m_TreeCtrl.SetItemText(hItem,strValue);
					strValue.Format("Allow passage when aircraft %.2f m or %d mins past interrupt line or no aircraft in position for take off", pData->m_dTakeoffHoldDist/100, pData->m_tTakeoffHoldTime.asMinutes());
					m_TreeCtrl.SetItemText(hParItem,strValue);
				}
			}

		}
		break;
     case UM_CEW_SHOW_DIALOGBOX_BEGIN:
		 {
			 HTREEITEM _item = (HTREEITEM)wParam;
			 int nodeTy = m_TreeCtrl.GetItemData(_item) ;
			 HandleDialogShow(nodeTy,_item) ;
		 }
    	break;
	 case UM_CEW_COMBOBOX_BEGIN:
		 {
			 CWnd* pWnd = m_TreeCtrl.GetEditWnd((HTREEITEM)wParam);
			 CRect rectWnd;
			 HTREEITEM hItem = (HTREEITEM)wParam;

			 m_TreeCtrl.GetItemRect((HTREEITEM)wParam,rectWnd,TRUE);
			 pWnd->SetWindowPos(NULL,rectWnd.right,rectWnd.top,0,0,SWP_NOSIZE);
			 CComboBox* pCB=(CComboBox*)pWnd;
			 pCB->ResetContent();
			
			 HTREEITEM hParItem = m_TreeCtrl.GetParentItem(hItem);
			 if (m_TreeCtrl.GetParentItem(hParItem) == NULL)		//interrupt line direction
			 {
				 InitComboString(pCB,hItem) ;
				 pCB->SetCurSel(0);
			 }
			 else
			 {
				int nIdx = pCB->AddString("Yes");
				pCB->SetItemData(nIdx, 1);
				
				nIdx = pCB->AddString("No");
				pCB->SetItemData(nIdx, 0);

				pCB->SetCurSel(0);
			 }
		 }
		 break;
	 case UM_CEW_COMBOBOX_SELCHANGE:
		 {
			 HTREEITEM _item = (HTREEITEM)wParam;
			 CWnd* pWnd=m_TreeCtrl.GetEditWnd(_item);
			 CComboBox* pTCB=(CComboBox*)pWnd;
			 HandleComboxChange(pTCB,_item) ;
		 }
		 break;

	 default:
		 break ;
    }
	return CXTResizeDialog::DefWindowProc(message,wParam,lParam) ;
}
void CDlgTaxiInterruptTimes::InitComboString(CComboBox* _box,HTREEITEM Item)
{
	if(_box == NULL || Item == NULL)
		return ;

     int ndx = _box->AddString(_T("<-")) ;
	 _box->SetItemData(ndx,-1) ;

	 ndx = _box->AddString(_T("->")) ;
	 _box->SetItemData(ndx,1) ;
}
void CDlgTaxiInterruptTimes::HandleComboxChange(CComboBox* pComBox,HTREEITEM hItem)
{
	if(pComBox == NULL || hItem == NULL)
		return ;

	int ndx = pComBox->GetCurSel() ;
	HTREEITEM hGrandItem = m_TreeCtrl.GetParentItem(m_TreeCtrl.GetParentItem(hItem));
	if (hGrandItem == NULL)
	{
		CTaxiInterruptTimeItem* interrupt = (CTaxiInterruptTimeItem*)m_TreeCtrl.GetItemData(hItem) ;
		interrupt->SetDirection( (int)pComBox->GetItemData(ndx)) ;
		CTaxiInterruptLine* pInterruptLine = FindHoldShortLineNodeByID(interrupt->GetShortLineID()) ;
		if(pInterruptLine != NULL)
		{
			IntersectionNode node1,node2;
			node1.ReadData(pInterruptLine->GetFrontIntersectionNodeID());
			node2.ReadData(pInterruptLine->GetBackIntersectionNodeID());
			m_TreeCtrl.SetItemText(hItem,FormatInterrruptNodeString(pInterruptLine->GetName(),(int)interrupt->GetDirection(),node1.GetName(),node2.GetName())) ;
		}
	}
	else
	{
		HTREEITEM hParItem = m_TreeCtrl.GetParentItem(hItem);
		CTaxiInterruptTimeItem* pIntrruptItem = (CTaxiInterruptTimeItem*)m_TreeCtrl.GetItemData(hParItem);
		if (pIntrruptItem)
		{
			int nValue = (int)pComBox->GetItemData(ndx);
			if ((pIntrruptItem->IsLinkedRunway() && nValue == 1) || (!pIntrruptItem->IsLinkedRunway() && nValue == 0))
				return;

			if (pIntrruptItem->IsLinkedRunway())
			{
				HTREEITEM hSubItem = m_TreeCtrl.GetChildItem(hItem);

				while (hSubItem != NULL)
				{
					HTREEITEM hNextItem = m_TreeCtrl.GetNextSiblingItem(hSubItem);
					CTaxiLinkedRunwayData* pData = (CTaxiLinkedRunwayData*)m_TreeCtrl.GetItemData(hSubItem);

					m_TreeCtrl.DeleteItem(hSubItem);
					pIntrruptItem->DelLinkedRunwayData(pData);

					hSubItem = hNextItem;
				}
			}
			else
			{
				HTREEITEM hNextItem = m_TreeCtrl.GetNextSiblingItem(hItem);
				m_TreeCtrl.DeleteItem(hNextItem);
			}

			pIntrruptItem->SetLinkedRunway(nValue >0?true:false);

			CString strContent;
			bool bLinkedRunway = pIntrruptItem->IsLinkedRunway();
			if (bLinkedRunway)
				strContent = "Linked Runways: Yes";
			else
				strContent = "Linked Runways: No";
			m_TreeCtrl.SetItemText(hItem, strContent);
		
			LoadLinkedRunwayData(pIntrruptItem,hItem, hParItem);
		}
	}



}
void CDlgTaxiInterruptTimes::HandleDialogShow(int ty , HTREEITEM hItem)
{
	HTREEITEM hParItem = m_TreeCtrl.GetParentItem(hItem) ;
	CString strContent = m_TreeCtrl.GetItemText(hParItem);
	if (strContent.Find("Linked Runways") > -1)
	{
		CDlgSelectRunway dlg(m_nPrjID);
		if (dlg.DoModal() != IDOK)
			return;

		CTaxiLinkedRunwayData* pData = (CTaxiLinkedRunwayData*)m_TreeCtrl.GetItemData(hItem);
		if (pData)
		{
			pData->m_nRwyID = dlg.GetSelectedRunway();
			pData->m_nRwyMark = dlg.GetSelectedRunwayMarkingIndex();

			Runway altRunway;
			altRunway.ReadObject(pData->m_nRwyID);

			CString strContent = pData->m_nRwyMark >0 ?altRunway.GetMarking2().c_str():altRunway.GetMarking1().c_str();
			m_TreeCtrl.SetItemText(hItem, strContent);
		}
		return;
	}

	CTaxiInterruptTimeItem* pTimeItem = (CTaxiInterruptTimeItem*)m_TreeCtrl.GetItemData(hParItem) ;
	if(m_TreeCtrl.GetItemData(hItem) == TY_HOLD_TIME)
	{
		CProbDistEntry* pPDEntry = NULL;
		pPDEntry = (*m_pSelectProbDistEntry)(NULL, m_pInputAirside);
		if(pPDEntry == NULL)
			return;

		pTimeItem->SetHoldTime(pPDEntry) ;
		strContent.Format("Hold Time(Seconds): %s",pTimeItem->GetHoldTime()->getDistName());
		m_TreeCtrl.SetItemText(hItem,strContent) ;
		return ;
	}
	else
	{
		CDlgTimeRange timeRangeDlg(pTimeItem->GetStartTime(),pTimeItem->GetEndTime()) ;
		if(timeRangeDlg.DoModal() != IDOK)
			return;
		
		pTimeItem->SetStartTime(timeRangeDlg.GetStartTime()) ;
		pTimeItem->SetEndTime(timeRangeDlg.GetEndTime()) ;

		strContent.Format("Time Window: %s - %s",pTimeItem->GetStartTime().PrintDateTime(), pTimeItem->GetEndTime().PrintDateTime());				
		m_TreeCtrl.SetItemText(hItem,strContent) ;		
	}


	
}

BOOL CDlgTaxiInterruptTimes::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog() ;
	InitToolBar() ;
	InitTree() ;
	SetResize(IDC_STATIC_TOOL,SZ_TOP_LEFT,SZ_TOP_LEFT) ;
	SetResize(IDC_TREE_INTERRUPT,SZ_TOP_LEFT,SZ_BOTTOM_RIGHT) ;
	SetResize(ID_SAVE,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT) ;
	SetResize(IDCANCEL,SZ_BOTTOM_RIGHT,SZ_BOTTOM_RIGHT) ;
	SetResize(IDOK ,SZ_BOTTOM_RIGHT ,SZ_BOTTOM_RIGHT) ;
	return TRUE ;
}
void CDlgTaxiInterruptTimes::OnSize(UINT nType, int cx, int cy)
{
	CXTResizeDialog::OnSize( nType , cx ,cy ) ;

}
int CDlgTaxiInterruptTimes::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CXTResizeDialog::OnCreate(lpCreateStruct) == -1)
	{
         return -1 ;
	}

	// TODO: Add your specialized creation code here
	if (!m_ToolBar.CreateEx(this, TBSTYLE_FLAT,WS_CHILD|WS_VISIBLE|CBRS_ALIGN_TOP) ||
		!m_ToolBar.LoadToolBar(IDR_TOOLBAR_HOLDSHORT_LINE))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	return 0 ;
}
void CDlgTaxiInterruptTimes::InitToolBar()
{
	CRect rect ;
	m_Sta_Tool.GetWindowRect(&rect) ;
	ScreenToClient(&rect) ;

	m_Sta_Tool.EnableWindow(FALSE) ;
	m_Sta_Tool.ShowWindow(SW_HIDE) ;
    
	m_ToolBar.MoveWindow(&rect) ;
	m_ToolBar.ShowWindow(SW_SHOW) ;
	m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_ADD,TRUE) ;
	m_ToolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_DEL,FALSE) ;

}
void CDlgTaxiInterruptTimes::InitTree()
{
	int nCount = m_TaxiInterruptTimes->GetCount();
	for (int i =0; i < nCount; i++)
	{
		CTaxiInterruptFlightData* pFltData = m_TaxiInterruptTimes->GetItem(i);
		AddFlightTyItem(pFltData);
	}
}

void CDlgTaxiInterruptTimes::AddFlightTyItem(CTaxiInterruptFlightData* pFltData) 
{
	if(pFltData == NULL)
		return ;
	COOLTREE_NODE_INFO info ;
    m_TreeCtrl.InitNodeInfo(this ,info) ;
	info.net = NET_NORMAL ;
	info.nt = NT_NORMAL ;
	info.bAutoSetItemText  = FALSE ;
	CString FlightName ;
	pFltData->GetFlightTy()->screenPrint(FlightName) ;

	HTREEITEM FlightTyNode = m_TreeCtrl.InsertItem(FlightName,info,FALSE,FALSE) ;
	m_TreeCtrl.SetItemData(FlightTyNode,(DWORD_PTR)pFltData) ;

	int nCount = pFltData->GetCount();
	for (int i =0; i < nCount; i++)
	{
		CTaxiInterruptTimeItem* pInterruptItem = pFltData->GetItem(i);
		AddInterruptLineItem(pInterruptItem , FlightTyNode) ;
	}
	m_TreeCtrl.Expand(FlightTyNode,TVE_EXPAND) ;
}

void CDlgTaxiInterruptTimes::AddInterruptLineItem(CTaxiInterruptTimeItem* pInterruptItem , HTREEITEM _ParItem)
{
	if(pInterruptItem == NULL || _ParItem == NULL)
		return ;

	COOLTREE_NODE_INFO info ;
	m_TreeCtrl.InitNodeInfo(this ,info) ;
	info.net = NET_COMBOBOX ;
	info.nt = NT_NORMAL ;
	info.bAutoSetItemText  = FALSE ;

	CTaxiInterruptLine* holdShortLine = FindHoldShortLineNodeByID(pInterruptItem->GetShortLineID()) ;
	if(holdShortLine == NULL)
		return ;

	IntersectionNode node1,node2; 
	node1.ReadData(holdShortLine->GetFrontIntersectionNodeID());
	node2.ReadData(holdShortLine->GetBackIntersectionNodeID());
	HTREEITEM  hItem =  m_TreeCtrl.InsertItem(FormatInterrruptNodeString(holdShortLine->GetName(),(int)pInterruptItem->GetDirection(),node1.GetName(),node2.GetName()),info,FALSE,FALSE,_ParItem) ;
	m_TreeCtrl.SetItemData(hItem,(DWORD_PTR)pInterruptItem) ;
    
	info.net = NET_SHOW_DIALOGBOX ;
	CString strContent;
	strContent.Format("Time Window: %s - %s",pInterruptItem->GetStartTime().PrintDateTime(), pInterruptItem->GetEndTime().PrintDateTime());
	HTREEITEM sub_Item = m_TreeCtrl.InsertItem(strContent , info ,FALSE ,FALSE ,hItem);
    m_TreeCtrl.SetItemData(sub_Item,TY_TIME_WINDOW);

	info.net = NET_COMBOBOX ;
	bool bLinkedRunway = pInterruptItem->IsLinkedRunway();
	if (bLinkedRunway)
		strContent = "Linked Runways: Yes";
	else
		strContent = "Linked Runways: No";
	HTREEITEM hLinkedRwyItem = m_TreeCtrl.InsertItem(strContent , info ,FALSE ,FALSE ,hItem); 

	LoadLinkedRunwayData(pInterruptItem,hLinkedRwyItem,hItem);

	m_TreeCtrl.Expand(hItem, TVE_EXPAND);
	m_TreeCtrl.Expand(hLinkedRwyItem,TVE_EXPAND);

}

void CDlgTaxiInterruptTimes::LoadLinkedRunwayData( CTaxiInterruptTimeItem* pInterruptItem, HTREEITEM hParItem, HTREEITEM hGrandItem)
{
	if(pInterruptItem == NULL || hParItem == NULL)
		return ;

	COOLTREE_NODE_INFO info ;
	m_TreeCtrl.InitNodeInfo(this ,info) ;
	info.net = NET_SHOW_DIALOGBOX;
	info.nt = NT_NORMAL ;
	info.bAutoSetItemText  = FALSE;

	CString strContent;
	bool bLinkedRunway = pInterruptItem->IsLinkedRunway();
	if (!bLinkedRunway)
	{
		strContent.Format("Hold Time(Seconds): %s",pInterruptItem->GetHoldTime()->getDistName());
		HTREEITEM hHoldItem = m_TreeCtrl.InsertItem(strContent, info ,FALSE ,FALSE ,hGrandItem) ;
		m_TreeCtrl.SetItemData(hHoldItem,TY_HOLD_TIME) ;
	}
	else
	{
		int nCount = pInterruptItem->GetLinkedRunwayDataCount();
		if (nCount ==0)
		{
			CTaxiLinkedRunwayData* pData = new CTaxiLinkedRunwayData;
			pInterruptItem->AddLinkedRunwayData(pData);
		}
		nCount = pInterruptItem->GetLinkedRunwayDataCount();
		for (int i =0; i <nCount; i++)
		{
			CTaxiLinkedRunwayData* pData = pInterruptItem->GetLinkedRunwayDataByIdx(i);
			LoadRunwayOperationData(pData,hParItem);
		}
	}
}


CString CDlgTaxiInterruptTimes::FormatInterrruptNodeString(CString& _time,int direct,CString strNode1, CString strNode2)
{
	CString strRet;
	if(direct <= 0)
	{
		strRet.Format(_T("Interrupt line: %s     Directionality:  %s <- %s "),_time.GetString(), strNode1.GetString(),strNode2.GetString() );
	}
	else
	{
		strRet.Format(_T("Interrupt line: %s     Directionality:  %s -> %s "),_time.GetString(), strNode1.GetString(),strNode2.GetString() );
	}
	return strRet;
}

void CDlgTaxiInterruptTimes::OnNewButton()
{
	HTREEITEM hItem = m_TreeCtrl.GetSelectedItem() ; 
	if(hItem == NULL)
	{
		OnNewFlightTy() ;
		return ;
	}
	HTREEITEM _parItem = m_TreeCtrl.GetParentItem(hItem) ;
	if(_parItem == NULL )
		OnNewInterruptLine() ;	
	else
	{
		CString strContent = m_TreeCtrl.GetItemText(hItem);
		if (strContent.Find("Linked Runways") < 0)
			return;

		OnNewLinkedRwy();
	}
}

void CDlgTaxiInterruptTimes::OnDelButton()
{
	HTREEITEM hItem = m_TreeCtrl.GetSelectedItem() ; 
	if(hItem == NULL)
		return ;

	HTREEITEM _parItem = m_TreeCtrl.GetParentItem(hItem) ;
	if(_parItem == NULL )
		OnDelFlightTy() ;
	else
	{
		HTREEITEM hGrandItem = m_TreeCtrl.GetParentItem(_parItem);
		if (hGrandItem == NULL)
			OnDelInterruptLine() ;
		else
		{
			CString strContent = m_TreeCtrl.GetItemText(_parItem);
			if (strContent.Find("Linked Runways") < 0)
				return;

			OnDelLinkedRwy();
		}
	}
}

BOOL CDlgTaxiInterruptTimes::CheckFLightTyIsDefined(FlightConstraint& _constraint)
{
   HTREEITEM item = m_TreeCtrl.GetRootItem() ;
   CTaxiInterruptFlightData* pair ;
   while(item)
   {
        pair = (CTaxiInterruptFlightData*)m_TreeCtrl.GetItemData(item) ;
		if(pair->GetFlightTy()->isEqual(&_constraint))
			return TRUE ;
		item = m_TreeCtrl.GetNextSiblingItem(item) ;
   }
   return FALSE ;
}

BOOL CDlgTaxiInterruptTimes::CheckInterruptIsDefined(HTREEITEM _item ,CTaxiInterruptTimeItem* _interrupt)
{
	HTREEITEM ChildItem = m_TreeCtrl.GetChildItem(_item) ;
	CTaxiInterruptTimeItem* pData = NULL ;
	while(ChildItem)
	{
		pData = (CTaxiInterruptTimeItem*)m_TreeCtrl.GetItemData(ChildItem) ;
        if(pData->GetShortLineID() == _interrupt->GetShortLineID() )
			  return TRUE;
		ChildItem = m_TreeCtrl.GetNextSiblingItem(ChildItem) ;
	}
	return FALSE ;
}

void CDlgTaxiInterruptTimes::OnNewFlightTy()
{
	FlightConstraint fltcons = (*m_pSelectFlightType)(NULL);

   if(!CheckFLightTyIsDefined(fltcons))
	{
			CTaxiInterruptFlightData* pair = m_TaxiInterruptTimes->AddFlightTy(fltcons) ;
			AddFlightTyItem(pair) ;
	}
}

void CDlgTaxiInterruptTimes::OnDelFlightTy()
{
   HTREEITEM _ITEM = m_TreeCtrl.GetSelectedItem() ;
   if(_ITEM == NULL)
	   return ;
   CTaxiInterruptFlightData* pair = (CTaxiInterruptFlightData*)m_TreeCtrl.GetItemData(_ITEM) ;
   if(pair != NULL)
   {
	   m_TaxiInterruptTimes->DelFlightData(pair) ;
	   m_TreeCtrl.DeleteItem(_ITEM) ;
   }
}

void CDlgTaxiInterruptTimes::OnNewInterruptLine()
{
   CDlgHoldShortLineSel DlgSel(&m_holdShortLines) ;
  
   if (DlgSel.DoModal() == IDOK)
   {
	    CTaxiInterruptLine* linenode = DlgSel.GetSelHoldShortLine() ;
	   CTaxiInterruptTimeItem* taxiInterruptTimes = new CTaxiInterruptTimeItem() ;
	   taxiInterruptTimes->SetShortLineID(linenode->GetID()) ;
	   HTREEITEM item = m_TreeCtrl.GetSelectedItem() ;
	   if(CheckInterruptIsDefined(item,taxiInterruptTimes))
	   {
		   delete taxiInterruptTimes ;
		   MessageBox(_T(" This Short Line has defined"),NULL,MB_OK) ;
		   return ;
	   }
	   CTaxiInterruptFlightData* pair = (CTaxiInterruptFlightData*)m_TreeCtrl.GetItemData(item) ;
	   taxiInterruptTimes->SetFlightTyID(pair->GetFlightTyID()) ;
	   pair->AddTaxiInterruptTimeItem(taxiInterruptTimes) ;
	   AddInterruptLineItem(taxiInterruptTimes,item) ;
   }
}

void CDlgTaxiInterruptTimes::OnDelInterruptLine()
{
   HTREEITEM item = m_TreeCtrl.GetSelectedItem() ;
   if(item == NULL)
	   return ;
   HTREEITEM parItem = m_TreeCtrl.GetParentItem(item) ;
    CTaxiInterruptTimeItem* taxiInterruptTimes = (CTaxiInterruptTimeItem*)m_TreeCtrl.GetItemData(item) ;
	CTaxiInterruptFlightData* pair = (CTaxiInterruptFlightData*)m_TreeCtrl.GetItemData(parItem) ;
	pair->DelTaxiInterruptTimeItem(taxiInterruptTimes) ;
	m_TreeCtrl.DeleteItem(item) ;
}

CTaxiInterruptLine* CDlgTaxiInterruptTimes::FindHoldShortLineNodeByID(int id )
{
   int count = m_holdShortLines.GetElementCount() ;
   for (int i = 0 ; i < count ;i++)
   {
	   if(m_holdShortLines.GetItem(i)->GetID() == id)
		   return m_holdShortLines.GetItem(i) ;
   }
   return NULL ;
}

// CDlgTaxiInterruptTimes message handlers
void CDlgTaxiInterruptTimes::OnOK()
{
   OnSave() ;
   CXTResizeDialog::OnOK() ;
}

void CDlgTaxiInterruptTimes::OnSave()
{
	m_TaxiInterruptTimes->SaveData() ;
}

void CDlgTaxiInterruptTimes::OnNewLinkedRwy()
{
	HTREEITEM hItem = m_TreeCtrl.GetSelectedItem();
	if(hItem == NULL)
		return;

	CDlgSelectRunway dlg(m_nPrjID);
	if (dlg.DoModal() != IDOK)
		return;

	HTREEITEM hParItem = m_TreeCtrl.GetParentItem(hItem) ;
	CTaxiInterruptTimeItem* pTime = (CTaxiInterruptTimeItem*)m_TreeCtrl.GetItemData(hParItem);

	CTaxiLinkedRunwayData* pData = new CTaxiLinkedRunwayData;
	pData->m_nRwyID = dlg.GetSelectedRunway();
	pData->m_nRwyMark = dlg.GetSelectedRunwayMarkingIndex();
	pTime->AddLinkedRunwayData(pData);

	LoadRunwayOperationData(pData, hItem);

}

void CDlgTaxiInterruptTimes::OnDelLinkedRwy()
{
	HTREEITEM hItem = m_TreeCtrl.GetSelectedItem();
	if(hItem == NULL)
		return;

	HTREEITEM hParItem = m_TreeCtrl.GetParentItem(hItem) ;
	CTaxiInterruptTimeItem* pTime = (CTaxiInterruptTimeItem*)m_TreeCtrl.GetItemData(m_TreeCtrl.GetParentItem(hParItem));
	CTaxiLinkedRunwayData* pRwyData = (CTaxiLinkedRunwayData*)m_TreeCtrl.GetItemData(hItem);
	pTime->DelLinkedRunwayData(pRwyData);

	m_TreeCtrl.DeleteItem(hItem);
}

CTaxiLinkedRunwayData* CDlgTaxiInterruptTimes::GetSelectedRunwayData( HTREEITEM hItem )
{
	if (m_TreeCtrl.GetChildItem(hItem) != NULL)
	{
		CString strContent = m_TreeCtrl.GetItemText(hItem);
		if (strContent.Find("Allow passage") > -1)
		{
			HTREEITEM hGrandItem = m_TreeCtrl.GetParentItem(m_TreeCtrl.GetParentItem(hItem));
			HTREEITEM hGrandParItem = m_TreeCtrl.GetParentItem(hGrandItem);

			return (CTaxiLinkedRunwayData*)m_TreeCtrl.GetItemData(hGrandParItem);
		}

		if (strContent.Find("Landing") > -1 || strContent.Find("Take off") > -1)
		{
			HTREEITEM hGrandItem = m_TreeCtrl.GetParentItem(m_TreeCtrl.GetParentItem(hItem));
			return (CTaxiLinkedRunwayData*)m_TreeCtrl.GetItemData(hGrandItem);
		}

		if (strContent.Find("Operation") > -1)
		{
			HTREEITEM hParItem = m_TreeCtrl.GetParentItem(hItem);
			return (CTaxiLinkedRunwayData*)m_TreeCtrl.GetItemData(hParItem);
		}
	}

	HTREEITEM hGrandItem = m_TreeCtrl.GetParentItem(m_TreeCtrl.GetParentItem(hItem));
	HTREEITEM hGrandGrandItem = m_TreeCtrl.GetParentItem(m_TreeCtrl.GetParentItem(hGrandItem));
	
	return (CTaxiLinkedRunwayData*)m_TreeCtrl.GetItemData(hGrandGrandItem);

}

void CDlgTaxiInterruptTimes::LoadRunwayOperationData( CTaxiLinkedRunwayData* pData, HTREEITEM hParItem )
{
	COOLTREE_NODE_INFO info ;
	m_TreeCtrl.InitNodeInfo(this ,info) ;
	info.net = NET_SHOW_DIALOGBOX;
	info.nt = NT_NORMAL ;
	info.bAutoSetItemText  = FALSE;

	CString strContent;
	if (pData->m_nRwyID >=0)
	{
		Runway altRunway;
		altRunway.ReadObject(pData->m_nRwyID);
		strContent = pData->m_nRwyMark >0 ?altRunway.GetMarking2().c_str():altRunway.GetMarking1().c_str();
	}
	else
		strContent = "All";

	HTREEITEM hRwyItem = m_TreeCtrl.InsertItem(strContent, info ,FALSE ,FALSE ,hParItem);
	m_TreeCtrl.SetItemData(hRwyItem,(DWORD_PTR)pData);

	info.net = NET_NORMAL;
	HTREEITEM hOpItem = m_TreeCtrl.InsertItem("Operation", info ,FALSE ,FALSE ,hRwyItem);
	HTREEITEM hLandingItem = m_TreeCtrl.InsertItem("Landing", info ,FALSE ,FALSE ,hOpItem);
	HTREEITEM hTakeoffItem = m_TreeCtrl.InsertItem("Take off", info ,FALSE ,FALSE ,hOpItem);

	strContent.Format("Allow passage when aircraft %.2f m or %d mins from interrupt line or past interrupt line", pData->m_dLandingHoldDist/100,pData->m_tLandingHoldTime.asMinutes());
	HTREEITEM hLandSettingItem = m_TreeCtrl.InsertItem(strContent, info ,FALSE ,FALSE ,hLandingItem);
	strContent.Format("Allow passage when aircraft %.2f m or %d mins past interrupt line or no aircraft in position for take off", pData->m_dTakeoffHoldDist/100, pData->m_tTakeoffHoldTime.asMinutes());
	HTREEITEM hTakeSettingItem = m_TreeCtrl.InsertItem(strContent, info ,FALSE ,FALSE ,hTakeoffItem);			

	info.net = NET_EDITSPIN_WITH_VALUE;
	strContent.Format("%.2f m",pData->m_dLandingHoldDist/100);
	m_TreeCtrl.InsertItem(strContent, info ,FALSE ,FALSE ,hLandSettingItem);

	strContent.Format("%d mins",pData->m_tLandingHoldTime.asMinutes());
	m_TreeCtrl.InsertItem(strContent, info ,FALSE ,FALSE ,hLandSettingItem);

	strContent.Format("%.2f m",pData->m_dTakeoffHoldDist/100);
	m_TreeCtrl.InsertItem(strContent, info ,FALSE ,FALSE ,hTakeSettingItem);

	strContent.Format("%d mins",pData->m_tTakeoffHoldTime.asMinutes());
	m_TreeCtrl.InsertItem(strContent, info ,FALSE ,FALSE ,hTakeSettingItem);

	m_TreeCtrl.Expand(hLandSettingItem,TVE_EXPAND);
	m_TreeCtrl.Expand(hTakeSettingItem,TVE_EXPAND);
	m_TreeCtrl.Expand(hRwyItem,TVE_EXPAND);

}