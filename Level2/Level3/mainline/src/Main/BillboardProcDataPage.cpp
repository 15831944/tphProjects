#include "StdAfx.h"
#include ".\billboardprocdatapage.h"
#include "ProcesserDialog.h"
#include "PassengerTypeDialog.h"
CBillboardProcDataPage::CBillboardProcDataPage( enum PROCDATATYPE _enumProcDataType, InputTerminal* _pInTerm, const CString& _csProjPath )
: CProcDataPage(  _enumProcDataType,  _pInTerm,  _csProjPath )
{
	m_hTimeRemaing = NULL;
	m_hLinkedProc = NULL;
	m_hPaxTypeRoot = NULL;
	m_hTimeStop = NULL;
}

CBillboardProcDataPage::~CBillboardProcDataPage(void)
{
}
void CBillboardProcDataPage::SetTree()
{
	m_TreeData.DeleteAllItems();
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);

	m_hLinkedProc = m_TreeData.InsertItem(_T("Linked Processor"),cni,FALSE);
	cni.net=NET_EDITSPIN_WITH_VALUE;
	cni.nt=NT_NORMAL;
	m_hTimeRemaing = m_TreeData.InsertItem(_T("Time remaining(min) :30"),cni,FALSE);
	m_TreeData.SetItemData(m_hTimeRemaing,30);
	
	m_hTimeStop = m_TreeData.InsertItem(_T("Time Stop(sec) :30"),cni,FALSE);
	m_TreeData.SetItemData(m_hTimeStop,30);	

	cni.net = NET_NORMAL;
	m_hPaxTypeRoot = m_TreeData.InsertItem(_T("Passenger Type Coefficient"),cni,FALSE);
    
}
void CBillboardProcDataPage::SetValue( MiscData* _pMiscData )
{
	ReloadBillboardLinkedProcList((MiscBillboardData *)_pMiscData);
	ASSERT(m_hTimeRemaing);

	//double dCoefficent = ((MiscBillboardData*)_pMiscData)->getBillBoardCoefficient();
	//int nCoefficient = static_cast<int>(dCoefficent*100.0);
	int nTimeremaining = ((MiscBillboardData*)_pMiscData)->getTimeRemaining();
	COOLTREE_NODE_INFO* pInfo = m_TreeData.GetItemNodeInfo( m_hTimeRemaing );
	SetItemTextEx(m_hTimeRemaing,pInfo,"Time remaining(min) ",nTimeremaining);

	int nTimeStop = ((MiscBillboardData*)_pMiscData)->getTimeStop();
	pInfo = m_TreeData.GetItemNodeInfo( m_hTimeStop );
	SetItemTextEx(m_hTimeStop,pInfo,"Time Stop(sec) ",nTimeStop);


	ReloadPaxTypeCoefficient((MiscBillboardData *)_pMiscData);
}
LRESULT CBillboardProcDataPage::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message==UM_CEW_EDITSPIN_END)
	{
		MiscData* pMiscData = GetCurMiscData();
		if( pMiscData != NULL )
		{
			HTREEITEM hItem=(HTREEITEM)wParam;
			COOLTREE_NODE_INFO* pInfo = m_TreeData.GetItemNodeInfo( hItem );
			CString strValue=*((CString*)lParam);
			int nValue=atoi(strValue);
			if(hItem==m_hTimeRemaing)
			{
				((MiscBillboardData*)pMiscData)->setTimeRemaining(nValue);	
				SetItemTextEx(hItem,pInfo,"Time remaining(min) ",nValue);
			}
			else if(hItem==m_hTimeStop)
			{
				((MiscBillboardData*)pMiscData)->setTimeStop(nValue);	
				SetItemTextEx(hItem,pInfo,"Time Stop(sec) ",nValue);
			}
			else if (hItem == m_hDefaultTypeItem.m_hPropensity) 
			{
				CPaxTypeCoefficientItem& defaultCoefficient =((MiscBillboardData*)pMiscData)->GetDafaultCoefficient();
				defaultCoefficient.setPropernsityCoefficient(nValue);
				SetItemTextEx(hItem,pInfo,"Propensity Coefficient(%%) ",nValue);
			}
			else if (hItem == m_hDefaultTypeItem.m_hBuy)
			{
				CPaxTypeCoefficientItem& defaultCoefficient =((MiscBillboardData*)pMiscData)->GetDafaultCoefficient();
				defaultCoefficient.setBuyCoefficient(nValue);
				SetItemTextEx(hItem,pInfo,"Buy Coefficient(%%) ",nValue);
			}
			else //propensity coefficient and buy coefficient
			{

				std::vector<CPaxTypeCoefficientItem>& vPaxTypeCoefficient = ((MiscBillboardData*)pMiscData)->getPaxTypeCoefficient();

				int nCount = static_cast<int>(m_vPaxTypeTreeItem.size());
				for (int i =0; i< nCount; i++)
				{
					paxTypeTreeItem treeItem = m_vPaxTypeTreeItem[i];
					if (treeItem.m_hPropensity == hItem)
					{
						vPaxTypeCoefficient[i].setPropernsityCoefficient(nValue);
						SetItemTextEx(hItem,pInfo,"Propensity Coefficient(%%) ",nValue);
						break;
					}
					else if (treeItem.m_hBuy == hItem)
					{
						vPaxTypeCoefficient[i].setBuyCoefficient(nValue);
						SetItemTextEx(hItem,pInfo,"Buy Coefficient(%%) ",nValue);
						break;						
					}
				}			
			}
		}	
	}
	return CProcDataPage::DefWindowProc(message, wParam, lParam);
}
void CBillboardProcDataPage::OnSelchangedTreeData(NMHDR *pNMHDR, LRESULT *pResult)
{
	CProcDataPage::OnSelchangedTreeData(pNMHDR,pResult);
	
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	HTREEITEM  hSelectedItem=pNMTreeView->itemNew.hItem;
//	m_hCurrentSelItem = hItem;
	if (hSelectedItem == m_hLinkedProc	&& m_TreeData.GetChildItem(hSelectedItem) == NULL)
	{	//only can select one linked processor
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD,TRUE);
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL, FALSE );
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_LINKAGE_ONETOONE, FALSE );
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_EDIT_OR_SPIN_VALUE,FALSE);
	}
	else if (hSelectedItem == m_hTimeRemaing ||hSelectedItem == m_hTimeStop)
	{
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD,FALSE);
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL, FALSE );
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_LINKAGE_ONETOONE, FALSE );
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_EDIT_OR_SPIN_VALUE,TRUE);
	}
	else if (hSelectedItem == m_hDefaultTypeItem.m_hPaxType)
	{
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD,FALSE);
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL, TRUE );
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_LINKAGE_ONETOONE, FALSE );
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_EDIT_OR_SPIN_VALUE,FALSE);
	}
	else if (m_TreeData.GetParentItem(hSelectedItem) == m_hLinkedProc ||
		m_TreeData.GetParentItem(hSelectedItem) == m_hPaxTypeRoot)
	{
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD,FALSE);
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL, TRUE );
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_LINKAGE_ONETOONE, FALSE );
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_EDIT_OR_SPIN_VALUE,FALSE);
	}

	*pResult = 0;
}
void CBillboardProcDataPage::ReLoadDetailed()
{
	ClearControl();
	MiscData* pMiscData = GetCurMiscData();
	if( pMiscData == NULL )
		return;

	SetValue(pMiscData);


}

void CBillboardProcDataPage::OnToolbarbuttonadd() 
{
	// TODO: Add your command handler code here
	HTREEITEM hItem = m_hCurrentSelItem;

	if (hItem == NULL)
	{
		return;
	}
	if (m_nProcDataType == BillboardProcessor)
	{
		if (m_hCurrentSelItem == m_hLinkedProc)
			AddBillboardProc();
		else if (m_hCurrentSelItem == m_hPaxTypeRoot)
			AddPaxTypeCoefficient();	
	}
}

void CBillboardProcDataPage::OnToolbarbuttondel() 
{
	HTREEITEM hItem = m_hCurrentSelItem;
	if (hItem == NULL)
	{
		return;
	}		

	HTREEITEM hParentItem = m_TreeData.GetParentItem( hItem );
	if( hParentItem == m_hLinkedProc )
		DelBillboardLinkedProc();
	else if (hParentItem == m_hPaxTypeRoot) 
		DelPaxTypeCoefficient();
}
void CBillboardProcDataPage::AddBillboardProc()
{
	std::vector<int> procTypeList;
	for (int i = PointProc; i< BillboardProcessor; i++)
	{
		procTypeList.push_back(i);
	}

	CProcesserDialog dlg( m_pInTerm );
	dlg.SetTypeList( procTypeList );
	if( dlg.DoModal() == IDCANCEL )
		return;	

	MiscData* pMiscData = GetCurMiscData();
	if( pMiscData == NULL )
		return;

	ProcessorID id;	
	ProcessorIDList idList;
	ProcessorIDList idListShow;

	if( !dlg.GetProcessorIDList(idList) )
		return;	

	int nIDcount = idList.getCount();
	for(int i = 0; i < nIDcount; i++)
	{
		if (!SelectBillboardLinkedProc(*(idList.getID(i))))  // check if exist
		{
			idListShow.Add(idList[i]);
		}
	}

	nIDcount = idListShow.getCount();
	for (int i = 0; i < nIDcount; i++ )
	{
		id = *(idListShow.getID(i));		
		MiscProcessorIDList* pProcIDList = (MiscProcessorIDList*)((MiscBillboardData *)pMiscData)->GetBillBoardLinkedProcList();
		MiscProcessorIDWithOne2OneFlag* pID = new MiscProcessorIDWithOne2OneFlag( id );
		pProcIDList->addItem( pID );
	}
	if ( nIDcount > 0)
	{
		ReloadBillboardLinkedProcList( (MiscBillboardData*)pMiscData );
		SelectBillboardLinkedProc( id );	// select new item.
		SetModified();		
	}
}
bool CBillboardProcDataPage::SelectBillboardLinkedProc(ProcessorID _id)
{
	MiscData* pMiscData = GetCurMiscData();
	if( pMiscData == NULL )
		return false;

	if( m_hGateItem == NULL )
		return false;

	ProcessorIDList* pProcIDList = ((MiscBillboardData*)pMiscData)->GetBillBoardLinkedProcList();
	HTREEITEM hItem = m_TreeData.GetChildItem( m_hLinkedProc );
	while( hItem )
	{
		int nDBIdx = m_TreeData.GetItemData( hItem );

		const ProcessorID* pProcID = pProcIDList->getID( nDBIdx );
		if( *pProcID == _id )
		{
			m_TreeData.SelectItem( hItem );
			return true;
		}
		hItem = m_TreeData.GetNextItem( hItem, TVGN_NEXT );
	}	
	return false;
}

void CBillboardProcDataPage::ReloadBillboardLinkedProcList( MiscBillboardData* _pMiscData )
{
	// get the item handler
	if( !m_hLinkedProc )
		return;

	DeleteAllChildItems( m_hLinkedProc );

	// inster item
	MiscProcessorIDList* pProcIDList = (MiscProcessorIDList*)((MiscBillboardData*)_pMiscData)->GetBillBoardLinkedProcList();
	char str[80];
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	for (int i = 0; i < pProcIDList->getCount (); i++)
	{
		const MiscProcessorIDWithOne2OneFlag* pProcID = (MiscProcessorIDWithOne2OneFlag*)pProcIDList->getID( i );
		pProcID->printID( str );
		if( pProcID->getOne2OneFlag() )
			strcat(str ,"{1:1}");

		HTREEITEM hItem = m_TreeData.InsertItem( str,cni,FALSE,FALSE, m_hLinkedProc );
		m_TreeData.SetItemData( hItem, i );
	}
	m_TreeData.Expand( m_hLinkedProc, TVE_EXPAND );
	m_TreeData.SelectItem(m_hLinkedProc);

}

void CBillboardProcDataPage::DelBillboardLinkedProc()
{
	HTREEITEM hItem = m_hCurrentSelItem;
	if( hItem == NULL )
		return;

	MiscData* pMiscData = GetCurMiscData();
	if( pMiscData == NULL )
		return;

	ProcessorIDList* pProcIDList = ((MiscBillboardData*)pMiscData)->GetBillBoardLinkedProcList();
	if( !pProcIDList )
		return;

	int nDBIdx = m_TreeData.GetItemData( hItem );
	pProcIDList->removeItem( nDBIdx );
	ReloadBillboardLinkedProcList( (MiscBillboardData*)pMiscData );	
	SetModified();
}

void CBillboardProcDataPage::AddPaxTypeCoefficient()
{
	MiscData* pMiscData = GetCurMiscData();
	if( pMiscData == NULL )
		return;
	CPassengerTypeDialog dlg( m_pParentWnd );
	if (dlg.DoModal() == IDOK)
	{
//		CString strPaxType;
		CMobileElemConstraint paxType = dlg.GetMobileSelection();
//		paxType.screenPrint(strPaxType);

		CPaxTypeCoefficientItem item;
		item.setPaxType(paxType);

		std::vector<CPaxTypeCoefficientItem>& vPaxTypeCoefficient = ((MiscBillboardData*)pMiscData)->getPaxTypeCoefficient();
		vPaxTypeCoefficient.push_back(item);

		InsertPaxTypeCoefficient(item);
	}
	m_TreeData.Expand(m_hPaxTypeRoot,TVE_EXPAND);
}
void CBillboardProcDataPage::InsertPaxTypeCoefficient(const CPaxTypeCoefficientItem& paxTypeitem)
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);

	paxTypeTreeItem item;

	//pax type
	CString strPaxType= "";
	paxTypeitem.getPaxType().screenPrint(strPaxType);
	HTREEITEM hPaxType = m_TreeData.InsertItem(strPaxType,cni,FALSE,FALSE,m_hPaxTypeRoot);

	item.m_hPaxType = hPaxType;


	cni.net=NET_EDITSPIN_WITH_VALUE;
	cni.nt=NT_NORMAL;

	CString strPropensity;
	int nPeopensity = paxTypeitem.getPropensityCoefficient();
	strPropensity.Format(_T("Propensity Coefficient(%%) :%d"),nPeopensity);
	HTREEITEM hPropensity = m_TreeData.InsertItem(strPropensity,cni,FALSE,FALSE,hPaxType);
	m_TreeData.SetItemData(hPropensity,nPeopensity);

	item.m_hPropensity = hPropensity;


	CString strBuy;
	int nBuy= paxTypeitem.getBuyCoefficient();
	strBuy.Format(_T("Buy Coefficient(%%) :%d"),nBuy);
	HTREEITEM hBuy = m_TreeData.InsertItem(strBuy,cni,FALSE,FALSE,hPaxType);
	m_TreeData.SetItemData(hBuy,nBuy);

	item.m_hBuy = hBuy;


	m_vPaxTypeTreeItem.push_back(item);
	m_TreeData.Expand(hPaxType,TVE_EXPAND);

}


void CBillboardProcDataPage::ReloadPaxTypeCoefficient( MiscBillboardData* _pMiscData )
{
	if (m_hPaxTypeRoot == NULL)
		return;	
	DeleteAllChildItems( m_hPaxTypeRoot );
//////////////////////////////////////////////////////////////////////////

	//see if the default BillBoard value is deleted
	CPaxTypeCoefficientItem defaultPaxCoefficient = _pMiscData->GetDafaultCoefficient();
	if (( defaultPaxCoefficient.getBuyCoefficient() != 0) || ( defaultPaxCoefficient.getPropensityCoefficient() != 0))
	{
		//default coefficient
		COOLTREE_NODE_INFO cni;
		CCoolTree::InitNodeInfo(this,cni);

		CString strPaxType= "";
		defaultPaxCoefficient.getPaxType().screenPrint(strPaxType);
		HTREEITEM hPaxType = m_TreeData.InsertItem(strPaxType,cni,FALSE,FALSE,m_hPaxTypeRoot);

		m_hDefaultTypeItem.m_hPaxType = hPaxType;


		cni.net=NET_EDITSPIN_WITH_VALUE;
		cni.nt=NT_NORMAL;

		CString strPropensity;
		int nPeopensity = defaultPaxCoefficient.getPropensityCoefficient();
		strPropensity.Format(_T("Propensity Coefficient(%%) :%d"),nPeopensity);
		HTREEITEM hPropensity = m_TreeData.InsertItem(strPropensity,cni,FALSE,FALSE,hPaxType);
		m_TreeData.SetItemData(hPropensity,nPeopensity);

		m_hDefaultTypeItem.m_hPropensity = hPropensity;


		CString strBuy;
		int nBuy= defaultPaxCoefficient.getBuyCoefficient();
		strBuy.Format(_T("Buy Coefficient(%%) :%d"),nBuy);
		HTREEITEM hBuy = m_TreeData.InsertItem(strBuy,cni,FALSE,FALSE,hPaxType);
		m_TreeData.SetItemData(hBuy,nBuy);

		m_hDefaultTypeItem.m_hBuy = hBuy;
		m_TreeData.Expand(m_hDefaultTypeItem.m_hPaxType,TVE_EXPAND);

	}
	//////////////////////////////////////////////////////////////////////////
	std::vector<CPaxTypeCoefficientItem> vPaxTypeCoefficient = ((MiscBillboardData*)_pMiscData)->getPaxTypeCoefficient();
	int nCount = static_cast<int>(vPaxTypeCoefficient.size());
	
		for (int i=0;i <nCount; i++)
		{	
			InsertPaxTypeCoefficient(vPaxTypeCoefficient[i]);
		}
		
	m_TreeData.Expand(m_hPaxTypeRoot,TVE_EXPAND);
}

void CBillboardProcDataPage::NoReloadDefaultPaxType(MiscBillboardData* _pMiscData)
{
	std::vector<CPaxTypeCoefficientItem> vPaxTypeCoefficient = ((MiscBillboardData*)_pMiscData)->getPaxTypeCoefficient();
	int nCount = static_cast<int>(vPaxTypeCoefficient.size());
	for (int i=0;i <nCount; i++)
	{	
		InsertPaxTypeCoefficient(vPaxTypeCoefficient[i]);
	}
	m_TreeData.Expand(m_hPaxTypeRoot,TVE_EXPAND);

}
void CBillboardProcDataPage::DelPaxTypeCoefficient()
{
	HTREEITEM hItem = m_hCurrentSelItem;
	if( hItem == NULL )
		return;
	if (hItem == m_hDefaultTypeItem.m_hPaxType)
	{
		if (m_hPaxTypeRoot == NULL) return;	
		DeleteAllChildItems( m_hPaxTypeRoot );

		MiscData* pMiscData = GetCurMiscData();
		if( pMiscData == NULL ) return;

		//set the default value as 0
		CPaxTypeCoefficientItem& defaultPaxCoefficient = ((MiscBillboardData*)pMiscData)->GetDafaultCoefficient();
		defaultPaxCoefficient.setPropernsityCoefficient(0);
		defaultPaxCoefficient.setBuyCoefficient(0);

		std::vector<CPaxTypeCoefficientItem>& vPaxTypeCoefficient = ((MiscBillboardData*)pMiscData)->getPaxTypeCoefficient();
		int nCount = static_cast<int>(vPaxTypeCoefficient.size());
		for (int i=0;i <nCount; i++)
		{	
			InsertPaxTypeCoefficient(vPaxTypeCoefficient[i]);
		}
		m_TreeData.Expand(m_hPaxTypeRoot,TVE_EXPAND);

		return;
	}

	MiscData* pMiscData = GetCurMiscData();
	if( pMiscData == NULL )
		return;

	std::vector<CPaxTypeCoefficientItem>& vPaxTypeCoefficient = ((MiscBillboardData*)pMiscData)->getPaxTypeCoefficient();

	int nCount = static_cast<int>(m_vPaxTypeTreeItem.size());
	for (int i =0; i< nCount; i++)
	{
		paxTypeTreeItem treeItem = m_vPaxTypeTreeItem[i];
		if (treeItem.m_hPaxType == hItem)
		{
			vPaxTypeCoefficient.erase(vPaxTypeCoefficient.begin()+i);
			break;
		}
	}
	ReloadPaxTypeCoefficient((MiscBillboardData*)pMiscData);

}
void CBillboardProcDataPage::OnSelchangeListProcessor() 
{

	SetTree();

	// TODO: Add your control notification handler code here
	ReLoadDetailed();	
}