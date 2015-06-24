#include "StdAfx.h"
#include ".\retailprocdatapage.h"
#include "ProcesserDialog.h"
#include "PassengerTypeDialog.h"




CRetailProcDataPage::CRetailProcDataPage( enum PROCDATATYPE _enumProcDataType, InputTerminal* _pInTerm, const CString& _csProjPath )
: CProcDataPage(  _enumProcDataType,  _pInTerm,  _csProjPath )
{
	m_hSkipRoot		= NULL;
	m_hCheckOutRoot	= NULL;
	m_hBypassRoot	= NULL;

}
CRetailProcDataPage::~CRetailProcDataPage(void)
{
}

void CRetailProcDataPage::SetTree()
{
	CProcDataPage::SetTree();

}

void CRetailProcDataPage::SetValue( MiscData* _pMiscData )
{
	CProcDataPage::SetValue(_pMiscData);
	MiscRetailProcData *pData = (MiscRetailProcData *)_pMiscData;
	//set checkout value
	if(pData == NULL)
		return;
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);

	pData->SortSkipCondition();

	if(m_hCheckOutRoot != NULL)
	{
		DeleteAllChildItems(m_hCheckOutRoot);
		ProcessorIDList* procIDList = pData->getCheckoutProcList();
		
		//char str[80]={0};
		//for (int i = 0; i < procIDList->getCount(); i++)
		//{
		//	const ProcessorID* pProcID = procIDList->getID( i );
		//	pProcID->printID( str );

		//	HTREEITEM hItem = m_TreeData.InsertItem( str,cni,FALSE,FALSE, m_hCheckOutRoot );
		//	m_TreeData.SetItemData( hItem, (DWORD_PTR)pProcID );
		//}
		//m_TreeData.Expand(m_hCheckOutRoot, TVE_EXPAND);
		ReloadRetailProcList(m_hCheckOutRoot,procIDList);

	}

	if(m_hBypassRoot != NULL)
	{
		DeleteAllChildItems(m_hBypassRoot);
		ProcessorIDList* procIDList = pData->getBypassProcList();

		//char str[80]={0};
		//for (int i = 0; i < procIDList->getCount(); i++)
		//{
		//	const ProcessorID* pProcID = procIDList->getID( i );
		//	pProcID->printID( str );

		//	HTREEITEM hItem = m_TreeData.InsertItem( str,cni,FALSE,FALSE, m_hBypassRoot );
		//	m_TreeData.SetItemData( hItem, (DWORD_PTR)pProcID );
		//}
		//m_TreeData.Expand(m_hBypassRoot, TVE_EXPAND);
		ReloadRetailProcList(m_hBypassRoot,procIDList);
	}

	//pax skip condition
	
	int nConditionCount = pData->getConditionCount();
	for (int nItem = 0; nItem < nConditionCount; ++ nItem)
	{

		MiscRetailProcData::PaxSkipCondition *pCondition = pData->getCondition(nItem);
		if(pCondition == NULL)
			continue;

		AddSkipToTree(pCondition);
	}
	m_TreeData.Expand(m_hSkipRoot, TVE_EXPAND);

}

void CRetailProcDataPage::ReLoadDetailed()
{
	CProcDataPage::ReLoadDetailed();

}
LRESULT CRetailProcDataPage::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message==UM_CEW_EDITSPIN_END)
	{
		//check if retail item
		HTREEITEM hItem=(HTREEITEM)wParam;
		HTREEITEM hPaxType = m_TreeData.GetParentItem(hItem);
		if(hPaxType != NULL)
		{
			if(m_TreeData.GetParentItem(hPaxType) == m_hSkipRoot)
			{
				MiscRetailProcData::PaxSkipCondition *pCondition = (MiscRetailProcData::PaxSkipCondition *) m_TreeData.GetItemData(hPaxType);
				if( pCondition != NULL )
				{
					HTREEITEM hItem=(HTREEITEM)wParam;
					COOLTREE_NODE_INFO* pInfo = m_TreeData.GetItemNodeInfo( hItem );
					CString strValue=*((CString*)lParam);
					int nValue=atoi(strValue);


					if(hItem == m_TreeData.GetChildItem(hPaxType))//first node
					{
						pCondition->setMins(nValue);
						CString strText;
						strText.Format("Skip when time to STD < (Min): %d",pCondition->getMins());
						m_TreeData.SetItemText(hItem,strText);
					}
					else
					{	
						//Q len
						pCondition->setQlenth(nValue);
						CString strText;
						strText.Format("Skip when queue length > : %d",pCondition->getQlenth());
						m_TreeData.SetItemText(hItem,strText);
					}
					return 1;
				}	
			}
		}
	}
	if(message==UM_CEW_STATUS_CHANGE)
	{
		HTREEITEM hItem=(HTREEITEM)wParam;
		HTREEITEM hPaxType = m_TreeData.GetParentItem(hItem);
		if(hPaxType != NULL)
		{
			if(m_TreeData.GetParentItem(hPaxType) == m_hSkipRoot)
			{
				MiscRetailProcData::PaxSkipCondition *pCondition = (MiscRetailProcData::PaxSkipCondition *) m_TreeData.GetItemData(hPaxType);
				if( pCondition != NULL )
				{
					if(hItem == m_TreeData.GetChildItem(hPaxType))//first node
					{
						BOOL b=IsCheckTreeItem(hItem);
						if(b==BST_CHECKED)
						{
							pCondition->setTimeLimit(true);
						}
						else
						{
							pCondition->setTimeLimit(false);
						}
						SetModified();
					}
					else//queue length
					{
						BOOL b=IsCheckTreeItem(hItem);
						if(b==BST_CHECKED)
						{
							pCondition->setQLimit(true);
						}
						else
						{
							pCondition->setQLimit(false);
						}
						SetModified();
					}
				}
			}
		}
	}

	if(message == UM_CEW_EDITSPIN_BEGIN)
	{

	}
	return CProcDataPage::DefWindowProc(message, wParam, lParam);
}
void CRetailProcDataPage::OnSelchangedTreeData(NMHDR *pNMHDR, LRESULT *pResult)
{
	CProcDataPage::OnSelchangedTreeData(pNMHDR,pResult);
	
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	HTREEITEM  hSelectedItem=pNMTreeView->itemNew.hItem;
	HTREEITEM hParentItem = NULL;
	if(hSelectedItem != NULL)
		hParentItem = m_TreeData.GetParentItem(hSelectedItem);

	if (hSelectedItem == m_hBypassRoot || hSelectedItem == m_hCheckOutRoot)
	{	//only can select one linked processor
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD,TRUE);
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL, FALSE );
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_LINKAGE_ONETOONE, FALSE );
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_EDIT_OR_SPIN_VALUE,FALSE);
	}
	else if (hSelectedItem == m_hSkipRoot)
	{	//only can select one linked processor
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD,TRUE);
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL, FALSE );
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_LINKAGE_ONETOONE, FALSE );
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_EDIT_OR_SPIN_VALUE,FALSE);
	}
	else if (hParentItem == m_hSkipRoot)
	{
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD,FALSE);
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL, TRUE );
		m_ToolBar.GetToolBarCtrl().EnableButton( ID_LINKAGE_ONETOONE, FALSE );
		m_ToolBar.GetToolBarCtrl().EnableButton(ID_EDIT_OR_SPIN_VALUE,FALSE);
	}

	if (hParentItem != NULL)
	{
		if(m_TreeData.GetParentItem(hParentItem) == m_hSkipRoot)
		{
			m_ToolBar.GetToolBarCtrl().EnableButton(ID_TOOLBARBUTTONADD,FALSE);
			m_ToolBar.GetToolBarCtrl().EnableButton( ID_TOOLBARBUTTONDEL, FALSE );
			m_ToolBar.GetToolBarCtrl().EnableButton( ID_LINKAGE_ONETOONE, FALSE );
			m_ToolBar.GetToolBarCtrl().EnableButton(ID_EDIT_OR_SPIN_VALUE,FALSE);
		}
	}


	*pResult = 0;
}


void CRetailProcDataPage::OnToolbarbuttonadd() 
{
	// TODO: Add your command handler code here
	HTREEITEM hItem = m_hCurrentSelItem;

	if (hItem == NULL)
		return;
	
	if (m_nProcDataType != RetailProc)
		return;
	MiscRetailProcData* pMiscData = (MiscRetailProcData *)GetCurMiscData();
	if( pMiscData == NULL )
		return ;

	if(hItem == m_hCheckOutRoot || hItem == m_hBypassRoot)
	{

		CProcesserDialog dlg( m_pInTerm );
		dlg.SetType( RetailProc );
		if( dlg.DoModal() == IDCANCEL )
			return;	

		ProcessorID id;	
		ProcessorIDList idList;
		ProcessorIDList idListShow;
		if( !dlg.GetProcessorIDList(idList) )
			return;	

	

		ProcessorIDList* pProcIDList = NULL;
		HTREEITEM hRootItem = NULL;
		if (hItem == m_hCheckOutRoot)
		{
			hRootItem = m_hCheckOutRoot;
			pProcIDList = pMiscData->getCheckoutProcList();
		}
		else if(hItem == m_hBypassRoot)
		{
			hRootItem = m_hBypassRoot;
			pProcIDList = pMiscData->getBypassProcList();
		}

		ASSERT(pProcIDList != NULL);
		ASSERT(hRootItem != NULL);
		if(pProcIDList == NULL || hRootItem == NULL)
			return;



		int nIDcount = idList.getCount();
		for(int i = 0; i < nIDcount; i++)
		{
			if (!SelectRetailProcItem(*(idList.getID(i)), hRootItem, pProcIDList) ) // check if exist
			{
				idListShow.addItem(idList[i]);
			}
		}

		nIDcount = idListShow.getCount();
		for (int i = 0; i < nIDcount; i++ )
		{
			id = *(idListShow.getID(i));	
			ProcessorID* pID = new ProcessorID( id );
			pProcIDList->addItem( pID );		
		}
		if ( nIDcount > 0)
		{
			ReloadRetailProcList( hRootItem, pProcIDList);
			SelectRetailProcItem( id,  hRootItem, pProcIDList );	// select new item.
			SetModified();
		}
		return;
	}
	else if(hItem == m_hSkipRoot)
	{
		//add new skip item
		CPassengerTypeDialog dlg( m_pParentWnd );
		if (dlg.DoModal() == IDOK)
		{
			CMobileElemConstraint paxType = dlg.GetMobileSelection();
			//		paxType.screenPrint(strPaxType);

			MiscRetailProcData::PaxSkipCondition *pCondition = new MiscRetailProcData::PaxSkipCondition;
			pCondition->setConstraint(paxType);
			pMiscData->addCondition(pCondition);

			AddSkipToTree(pCondition);
			SetModified();
		}
		m_TreeData.Expand(m_hSkipRoot,TVE_EXPAND);




		return;
	}


	CProcDataPage::OnToolbarbuttonadd();
}
bool CRetailProcDataPage::SelectRetailProcItem(ProcessorID _id,HTREEITEM hRootItem, ProcessorIDList* pProcList)
{
	if( hRootItem == NULL )
		return false;

	HTREEITEM hItem = m_TreeData.GetChildItem( hRootItem);
	while( hItem )
	{
		int nDBIdx = m_TreeData.GetItemData( hItem );

		const ProcessorID* pProcID = pProcList->getID( nDBIdx );
		if( *pProcID == _id )
		{
			m_TreeData.SelectItem( hItem );
			return true;
		}
		hItem = m_TreeData.GetNextItem( hItem, TVGN_NEXT );
	}	
	return false;
}
void CRetailProcDataPage::ReloadRetailProcList(HTREEITEM hRootItem, ProcessorIDList* pProcList)
{
	// get the item handler
	// get the item handler
	if( !m_hCheckOutRoot )
		return;

	DeleteAllChildItems( hRootItem );

	// inster item
	char str[80];
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	for (int i = 0; i < pProcList->getCount (); i++)
	{
		const ProcessorID* pProcID = pProcList->getID( i );
		pProcID->printID( str );

		HTREEITEM hItem = m_TreeData.InsertItem( str,cni,FALSE,FALSE, hRootItem );
		m_TreeData.SetItemData( hItem, i );
	}
	m_TreeData.Expand( hRootItem, TVE_EXPAND );
	m_TreeData.SelectItem(hRootItem);
}


void CRetailProcDataPage::OnToolbarbuttondel() 
{
	HTREEITEM hItem = m_hCurrentSelItem;
	if (hItem == NULL)
	{
		return;
	}
	MiscRetailProcData* pMiscData = (MiscRetailProcData *)GetCurMiscData();
	if( pMiscData == NULL )
		return;

	HTREEITEM hParentItem = m_TreeData.GetParentItem( hItem );
	if(hParentItem == m_hCheckOutRoot || hParentItem == m_hBypassRoot)
	{



		int nDBIdx = m_TreeData.GetItemData( hItem );

		ProcessorIDList* pProcIDList = NULL;
		HTREEITEM hRootItem = NULL;
		if (hParentItem == m_hCheckOutRoot)
		{
			hRootItem = m_hCheckOutRoot;
			pProcIDList = pMiscData->getCheckoutProcList();
		}
		else if(hParentItem == m_hBypassRoot)
		{
			hRootItem = m_hBypassRoot;
			pProcIDList = pMiscData->getBypassProcList();
		}

		ASSERT(pProcIDList != NULL);
		ASSERT(hRootItem != NULL);
		if(pProcIDList == NULL || hRootItem == NULL)
			return;

		pProcIDList->removeItem( nDBIdx );
		ReloadRetailProcList(hRootItem, pProcIDList);
		SetModified();
		return;
	}

	else if(hParentItem == m_hSkipRoot)//condition
	{
		MiscRetailProcData::PaxSkipCondition *pCondition = (MiscRetailProcData::PaxSkipCondition *) m_TreeData.GetItemData(hItem);
		if(pCondition == NULL)
			return;
		pMiscData->delCondition(pCondition);
		m_TreeData.DeleteItem(hItem);
		SetModified();
		return;
	}

	CProcDataPage::OnToolbarbuttondel();

}

void CRetailProcDataPage::OnSelchangeListProcessor() 
{

	SetTree();

	// TODO: Add your control notification handler code here
	ReloadCapacityList();
	ReLoadDetailed();	
}

void CRetailProcDataPage::InitSpecificBehvaior()
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);
	cni.nt = NT_NORMAL;
	cni.net = NET_NORMAL;

	m_hCheckOutRoot = m_TreeData.InsertItem("Checkout Processor", cni, FALSE, FALSE, m_hLinkage);
	m_hBypassRoot	= m_TreeData.InsertItem("Bypass Processor",	  cni, FALSE, FALSE, m_hLinkage);

	m_hSkipRoot = m_TreeData.InsertItem("Pax Skip Condition", cni, FALSE);

}

void CRetailProcDataPage::AddSkipToTree( MiscRetailProcData::PaxSkipCondition *pCondition )
{
	COOLTREE_NODE_INFO cni;
	CCoolTree::InitNodeInfo(this,cni);

	//pax type
	CString strPaxType= "";
	pCondition->getConstraint().screenPrint(strPaxType);
	HTREEITEM hPaxType = m_TreeData.InsertItem(strPaxType,cni,FALSE,FALSE,m_hSkipRoot);
//	TItemData *pData = new TItemData(TI_PAXTYPE,pCondition,0);
	m_TreeData.SetItemData(hPaxType,(DWORD_PTR)pCondition);

	//minus
	cni.nt=NT_CHECKBOX;
	cni.net=NET_EDITSPIN_WITH_VALUE;

	CString strText;
	strText.Format("Skip when time to STD < (Min): %d",pCondition->getMins());
	HTREEITEM hTime=m_TreeData.InsertItem(strText,cni,TRUE,FALSE,hPaxType);
//	pData = new TItemData(TI_TIME,pCondition,pCondition->getMins());
	m_TreeData.SetItemData(hTime,pCondition->getMins());
	if(pCondition->hasTimeLimit())
		m_TreeData.SetCheckStatus(hTime,TRUE);
	else
		m_TreeData.SetCheckStatus(hTime,FALSE);

	//Q len

	strText.Format("Skip when queue length > : %d",pCondition->getQlenth());
	HTREEITEM hLen=m_TreeData.InsertItem(strText,cni,TRUE,FALSE,hPaxType);
//	pData = new TItemData(TI_TIME,pCondition,pCondition->getQlenth());
	m_TreeData.SetItemData(hLen,pCondition->getQlenth());

	if(pCondition->hasQLimit())
		m_TreeData.SetCheckStatus(hLen,TRUE);
	else
		m_TreeData.SetCheckStatus(hLen,FALSE);
	

	m_TreeData.Expand(hPaxType, TVE_EXPAND);
}












































