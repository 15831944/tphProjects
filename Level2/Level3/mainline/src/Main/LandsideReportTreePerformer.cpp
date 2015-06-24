#include "StdAfx.h"
#include "Resource.h"
#include ".\landsidereporttreeperformer.h"
#include "MFCExControl/ARCTreeCtrlEx.h"
#include "DlgLandsideSelectVehicleType.h"
#include "DlgSelectLandsideObject.h"
#include "..\Landside\InputLandside.h"
#include "../Landside/LandsideLayoutObject.h"
#include "DlgSelectLandsideVehicleType.h"

#define LSR_TREE_ALLOBJECTS _T("ALL")

LandsideReportTreeBasePerformer::LandsideReportTreeBasePerformer( CARCTreeCtrlEx *pTreeCtrl,
																 InputTerminal* pInTerm, 
																 InputLandside *pInLandside,
																 LandsideBaseParam *pParam,																 
																 LandsideReportType enumReportType )
:m_pTreeCtrl(pTreeCtrl)
,m_pInTerm(pInTerm)
,m_pLandside(pInLandside)
,m_pParameter(pParam)
,m_enumReportType(enumReportType)
{

}
LandsideReportTreeBasePerformer::~LandsideReportTreeBasePerformer(void)
{
	for (size_t i = 0; i < m_vNodeDataList.size(); i++)
	{
		delete m_vNodeDataList[i];
	}
	m_vNodeDataList.clear();
}
//-----------------------------------------------------------------------------------------
//Summary:
//		tree operation
//-----------------------------------------------------------------------------------------
HTREEITEM LandsideReportTreeBasePerformer::InsertItem( HTREEITEM hItem,const CString& sNode,TreeNodeData* pNodeData )
{
	ASSERT(m_pTreeCtrl != NULL);
	if(m_pTreeCtrl == NULL)
		return NULL;

	HTREEITEM hInsertItem;
	if (hItem == NULL)
	{
		hInsertItem = m_pTreeCtrl->InsertItem(sNode,TVI_ROOT);
	}
	else
	{
		hInsertItem = m_pTreeCtrl->InsertItem(sNode,hItem);
	}
	
	if(hInsertItem != NULL)
	{
		m_pTreeCtrl->SetItemData(hInsertItem,(DWORD_PTR)pNodeData);
		m_vNodeDataList.push_back(pNodeData);
		return hInsertItem;
	}

	return NULL;
}

//-----------------------------------------------------------------------------------------
//Summary:
//		refresh parameter data when necessary
//-----------------------------------------------------------------------------------------
void LandsideReportTreeBasePerformer::UpdateOnboardParameter(LandsideBaseParam* pParameter)
{
	m_pParameter = pParameter;
}

LandsideReportType LandsideReportTreeBasePerformer::GetReportType() const
{
	return m_enumReportType;
}


//////////////////////////////////////////////////////////////////////////
//default tree perform

LandsideReportTreeDefaultPerformer::LandsideReportTreeDefaultPerformer(CARCTreeCtrlEx *pTreeCtrl,InputTerminal* pInTerm,InputLandside *pInLandside, LandsideBaseParam *pParam, LandsideReportType enumReportType)
:LandsideReportTreeBasePerformer(pTreeCtrl,pInTerm,pInLandside,pParam, enumReportType)
{

}

LandsideReportTreeDefaultPerformer::~LandsideReportTreeDefaultPerformer()
{

}

void LandsideReportTreeDefaultPerformer::OnToolBarAdd( HTREEITEM hTreeItem )
{
	if (hTreeItem == NULL)
		return;
	
	TreeNodeData* pNodeData = (TreeNodeData*)m_pTreeCtrl->GetItemData(hTreeItem);
	ASSERT(pNodeData);
	if (pNodeData == NULL)
		return;

	if(pNodeData->m_nodeType == VEHICLE_ITEM_ROOT)
	{
		CDlgSelectLandsideVehicleType dlg;
		if(dlg.DoModal()== IDOK)
		{
			CString strName = dlg.GetName();
			CHierachyName hierachyName;
			hierachyName.fromString(strName);
			
			TreeNodeData* pTreeNode = new TreeNodeData;
			pTreeNode->m_nodeType = VEHICLE_ITEM;
			pTreeNode->vehicleName = hierachyName;

			InsertItem(hTreeItem,strName,pTreeNode);
			m_pTreeCtrl->Expand(m_hVehicleeRootItem, TVE_EXPAND);

		}
	}
	
	
	
}

void LandsideReportTreeDefaultPerformer::OnToolBarDel( HTREEITEM hTreeItem )
{
	if (hTreeItem == NULL)
		return;

	TreeNodeData* pNodeData = (TreeNodeData*)m_pTreeCtrl->GetItemData(hTreeItem);
	ASSERT(pNodeData);
	if (pNodeData == NULL)
		return;

	if(pNodeData->m_nodeType == VEHICLE_ITEM_ROOT)
		return;


	HTREEITEM hParentItem = m_pTreeCtrl->GetParentItem(hTreeItem);

	m_pTreeCtrl->DeleteItem(hTreeItem);
	if(m_pTreeCtrl->GetChildItem(hParentItem) == NULL)
	{
		TreeNodeData* pTreeNode = new TreeNodeData;
		pTreeNode->m_nodeType = VEHICLE_ITEM;
		pTreeNode->vehicleName = CHierachyName();

		InsertItem(m_hVehicleeRootItem,pTreeNode->vehicleName.toString(),pTreeNode);
		m_pTreeCtrl->Expand(m_hVehicleeRootItem, TVE_EXPAND);
	}

}

void LandsideReportTreeDefaultPerformer::OnToolBarEdit( HTREEITEM hTreeItem )
{

}

LRESULT LandsideReportTreeDefaultPerformer::DefWindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	return 1;
}

void LandsideReportTreeDefaultPerformer::OnUpdateToolBarAdd( CCmdUI *pCmdUI )
{
	if(m_pTreeCtrl == NULL)
		return;

	HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem();
	if( hTreeItem == NULL)
		return;

	TreeNodeData* pNodeData = (TreeNodeData*)m_pTreeCtrl->GetItemData(hTreeItem);
	if(pNodeData == NULL)
		return;

	if(pNodeData->m_nodeType == VEHICLE_ITEM_ROOT)
	{
		pCmdUI->Enable(TRUE);
	}
	else if(pNodeData->m_nodeType == VEHICLE_ITEM)
	{
		pCmdUI->Enable(FALSE);
	}
	else
		pCmdUI->Enable(FALSE);
}

void LandsideReportTreeDefaultPerformer::OnUpdateToolBarDel( CCmdUI *pCmdUI )
{
	if(m_pTreeCtrl == NULL)
		return;

	HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem();
	if( hTreeItem == NULL)
		return;

	TreeNodeData* pNodeData = (TreeNodeData*)m_pTreeCtrl->GetItemData(hTreeItem);
	if(pNodeData == NULL)
		return;

	if(pNodeData->m_nodeType == VEHICLE_ITEM_ROOT)
	{
		pCmdUI->Enable(FALSE);
	}
	else if(pNodeData->m_nodeType == VEHICLE_ITEM)
	{
		pCmdUI->Enable(TRUE);
	}
	else
		pCmdUI->Enable(FALSE);
}

//-----------------------------------------------------------------------------------------
//Summary:
//		Data operation
//-----------------------------------------------------------------------------------------
void LandsideReportTreeDefaultPerformer::InitTree()
{
	ASSERT(m_pTreeCtrl != NULL);
	ASSERT(m_pParameter != NULL);

	if(m_pParameter == NULL || m_pTreeCtrl == NULL)
		return;

	LandsideBaseParam* pParameter = m_pParameter;
	if(m_pParameter == NULL)
		return;

	m_pTreeCtrl->DeleteAllItems();
	
	TreeNodeData *pItemRoot = new TreeNodeData;
	pItemRoot->m_nodeType = VEHICLE_ITEM_ROOT;
	m_hVehicleeRootItem = InsertItem(NULL,_T("Vehicle Types"),pItemRoot);



	if(pParameter->HasVehicleTypeDefined())//init data from parameter
	{
		int nTypeCount = pParameter->GetVehicleTypeNameCount();
		for (int nn = 0; nn < nTypeCount; ++nn)
		{
			CHierachyName vehicleName = m_pParameter->GetVehicleTypeName(nn);

			TreeNodeData* pTreeNode = new TreeNodeData;
			pTreeNode->m_nodeType = VEHICLE_ITEM;
			pTreeNode->vehicleName = vehicleName;

			InsertItem(m_hVehicleeRootItem,vehicleName.toString(),pTreeNode);
			m_pTreeCtrl->Expand(m_hVehicleeRootItem, TVE_EXPAND);
		}

	}
	else//default
	{
		LandsideReportTreeDefaultPerformer::InitDefaultTree();
	}

}
//-----------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------
//Summary:
//		init tree with default data
//-----------------------------------------------------------------------------------------
void LandsideReportTreeDefaultPerformer::InitDefaultTree()
{
	LandsideBaseParam* pParameter = m_pParameter;
	if(pParameter == NULL)
		return;

//	CString strName = LandsideBaseParam::GetVehicleTypeName(m_pLandside, LANDSIDE_ALLVEHICLETYPES);

	TreeNodeData* pTreeNode = new TreeNodeData;
	pTreeNode->m_nodeType = VEHICLE_ITEM;
	pTreeNode->nVehicleType = LANDSIDE_ALLVEHICLETYPES;
	pTreeNode->vehicleName = CHierachyName();

	InsertItem(m_hVehicleeRootItem, pTreeNode->vehicleName.toString(), pTreeNode);
	m_pTreeCtrl->Expand(m_hVehicleeRootItem, TVE_EXPAND);

 
}


void LandsideReportTreeDefaultPerformer::LoadData()
{
	InitTree();
}

void LandsideReportTreeDefaultPerformer::SaveData()
{
	ASSERT(m_pTreeCtrl != NULL);
	ASSERT(m_pParameter != NULL);

	if(m_pParameter == NULL || m_pTreeCtrl == NULL)
		return;

	LandsideBaseParam *pParam = m_pParameter;
	if(pParam == NULL)
		return;

	pParam->ClearVehicleType();
	pParam->ClearVehicleName();
	HTREEITEM hItem = m_pTreeCtrl->GetChildItem(m_hVehicleeRootItem);
	while (hItem != NULL)
	{
		TreeNodeData *pItemData = (TreeNodeData *)m_pTreeCtrl->GetItemData(hItem);
		
		if(pItemData != NULL)
			pParam->AddVehicleTypeName(pItemData->vehicleName);


		hItem = m_pTreeCtrl->GetNextSiblingItem(hItem);
	}




}




//////////////////////////////////////////////////////////////////////////


LSReportTreeVehicleDelayPerformer::LSReportTreeVehicleDelayPerformer( CARCTreeCtrlEx *pTreeCtrl, InputTerminal* pInTerm, InputLandside *pInLandside, LandsideBaseParam *pParam, LandsideReportType enumReportType )
:LandsideReportTreeDefaultPerformer(pTreeCtrl,pInTerm,pInLandside,pParam, enumReportType)
{
	m_hObjectRoot	= NULL;
	m_hFromRoot		= NULL;
	m_hToRoot		= NULL;

}

LSReportTreeVehicleDelayPerformer::~LSReportTreeVehicleDelayPerformer()
{

}

void LSReportTreeVehicleDelayPerformer::OnToolBarAdd( HTREEITEM hTreeItem )
{
	if (hTreeItem == NULL)
		return;

	TreeNodeData* pNodeData = (TreeNodeData*)m_pTreeCtrl->GetItemData(hTreeItem);
	ASSERT(pNodeData);
	if (pNodeData == NULL)
		return;

	if(pNodeData->m_nodeType == FROM_ROOT || pNodeData->m_nodeType == TO_ROOT)
	{
		CDlgSelectLandsideObject dlg( &(m_pLandside->getObjectList()));
		
		if(dlg.DoModal() == IDOK)
		{
			ALTObjectID altObj = dlg.getSelectObject();

			CString strObjName;

			if (altObj.IsBlank())
				 strObjName =  LSR_TREE_ALLOBJECTS;
			else
				strObjName = altObj.GetIDString();

			TreeNodeData* pNewTreeNode = new TreeNodeData;
			
			if(pNodeData->m_nodeType == FROM_ROOT)
				pNewTreeNode->m_nodeType = FROM_ITEM;
			if(pNodeData->m_nodeType == TO_ROOT)
				pNewTreeNode->m_nodeType = TO_ITEM;


			pNewTreeNode->altObj	  = altObj;

			InsertItem(hTreeItem,strObjName,pNewTreeNode);
			m_pTreeCtrl->Expand(hTreeItem, TVE_EXPAND);

		}

	}

	return LandsideReportTreeDefaultPerformer::OnToolBarAdd(hTreeItem);
}

void LSReportTreeVehicleDelayPerformer::OnToolBarDel( HTREEITEM hTreeItem )
{
	
	if (hTreeItem == NULL)
		return;

	TreeNodeData* pNodeData = (TreeNodeData*)m_pTreeCtrl->GetItemData(hTreeItem);
	ASSERT(pNodeData);
	if (pNodeData == NULL)
		return;

	if(pNodeData->m_nodeType == FROM_ITEM || pNodeData->m_nodeType == TO_ITEM)
	{
		HTREEITEM hParentItem = m_pTreeCtrl->GetParentItem(hTreeItem);
		
		m_pTreeCtrl->DeleteItem(hTreeItem);

		if(m_pTreeCtrl->GetChildItem(hParentItem) == NULL)
		{
			CString strObjName =  LSR_TREE_ALLOBJECTS;

			TreeNodeData* pNewTreeNode = new TreeNodeData;

			pNewTreeNode->m_nodeType = pNodeData->m_nodeType;

			pNewTreeNode->altObj	  = ALTObjectID();

			InsertItem(hParentItem,strObjName,pNewTreeNode);
			m_pTreeCtrl->Expand(hParentItem, TVE_EXPAND);
		}
		return;
	}
	
	
	return LandsideReportTreeDefaultPerformer::OnToolBarDel(hTreeItem);
}

//void LSReportTreeVehicleDelayPerformer::OnToolBarEdit( HTREEITEM hTreeItem )
//{
//	return LandsideReportTreeDefaultPerformer::OnToolBarEdit(hTreeItem);
//}
//
//LRESULT LSReportTreeVehicleDelayPerformer::DefWindowProc( UINT message, WPARAM wParam, LPARAM lParam )
//{
//	return LandsideReportTreeDefaultPerformer::DefWindowProc(message,wParam,lParam);
//}

void LSReportTreeVehicleDelayPerformer::OnUpdateToolBarAdd( CCmdUI *pCmdUI )
{
	if(m_pTreeCtrl == NULL)
		return;

	HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem();
	if( hTreeItem == NULL)
		return;

	TreeNodeData* pNodeData = (TreeNodeData*)m_pTreeCtrl->GetItemData(hTreeItem);
	if(pNodeData == NULL)
		return;

	if(pNodeData->m_nodeType == FROM_ROOT || pNodeData->m_nodeType == TO_ROOT)
	{
		pCmdUI->Enable(TRUE);
		return;
	}
	else if(pNodeData->m_nodeType ==  FROM_ITEM ||
			pNodeData->m_nodeType ==  TO_ITEM || 
			pNodeData->m_nodeType == OBJECT_ROOT)
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	return LandsideReportTreeDefaultPerformer::OnUpdateToolBarAdd(pCmdUI);
}

void LSReportTreeVehicleDelayPerformer::OnUpdateToolBarDel( CCmdUI *pCmdUI )
{
	
	if(m_pTreeCtrl == NULL)
		return;

	HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem();
	if( hTreeItem == NULL)
		return;

	TreeNodeData* pNodeData = (TreeNodeData*)m_pTreeCtrl->GetItemData(hTreeItem);
	if(pNodeData == NULL)
		return;

	if(pNodeData->m_nodeType ==  FROM_ITEM ||
		pNodeData->m_nodeType ==  TO_ITEM )
	{
		pCmdUI->Enable(TRUE);
		return;
	}
	else if(pNodeData->m_nodeType ==FROM_ROOT
			|| pNodeData->m_nodeType == TO_ROOT  
			|| pNodeData->m_nodeType == OBJECT_ROOT)
	{
		pCmdUI->Enable(FALSE);
		return;
	}

	
	return LandsideReportTreeDefaultPerformer::OnUpdateToolBarDel(pCmdUI);
}

void LSReportTreeVehicleDelayPerformer::InitTree()
{
	LandsideReportTreeDefaultPerformer::InitTree();

	ASSERT(m_pTreeCtrl != NULL);
	ASSERT(m_pParameter != NULL);

	if(m_pParameter == NULL || m_pTreeCtrl == NULL)
		return;

	LandsideBaseParam* pParameter = m_pParameter;
	if(m_pParameter == NULL)
		return;

	///m_pTreeCtrl->DeleteAllItems();

	TreeNodeData *pItemRoot = new TreeNodeData;
	pItemRoot->m_nodeType = OBJECT_ROOT;
	m_hObjectRoot = InsertItem(NULL,_T("Objects"),pItemRoot);

	
	//from, to

	//from node
	TreeNodeData *pFromData = new TreeNodeData;
	pFromData->m_nodeType = FROM_ROOT;
	m_hFromRoot = InsertItem(m_hObjectRoot,_T("From"),pFromData);


	if(pParameter->GetFromObjectCount())//init data from parameter
	{
		int nTypeCount = pParameter->GetFromObjectCount();
		for (int nn = 0; nn < nTypeCount; ++nn)
		{
			ALTObjectID altObj =	m_pParameter->GetFromObject(nn);

			CString strObjName =  altObj.GetIDString();
			if(altObj.IsBlank())
				strObjName = LSR_TREE_ALLOBJECTS;

			TreeNodeData* pTreeNode = new TreeNodeData;
			pTreeNode->m_nodeType = FROM_ITEM;
			pTreeNode->altObj	  = altObj;

			InsertItem(m_hFromRoot,strObjName,pTreeNode);
			m_pTreeCtrl->Expand(m_hFromRoot, TVE_EXPAND);
		}

	}
	else//default
	{


		CString strObjName =  LSR_TREE_ALLOBJECTS;

		TreeNodeData* pTreeNode = new TreeNodeData;
		pTreeNode->m_nodeType = FROM_ITEM;
		pTreeNode->altObj	  = ALTObjectID();

		InsertItem(m_hFromRoot,strObjName,pTreeNode);
		m_pTreeCtrl->Expand(m_hFromRoot, TVE_EXPAND);
		
	}


	//to
	TreeNodeData *pToData = new TreeNodeData;
	pToData->m_nodeType = TO_ROOT;
	m_hToRoot = InsertItem(m_hObjectRoot,_T("To"),pToData);


	if(pParameter->GetToObjectCount())//init data from parameter
	{
		int nTypeCount = pParameter->GetToObjectCount();
		for (int nn = 0; nn < nTypeCount; ++nn)
		{
			ALTObjectID altObj =	m_pParameter->GetToObject(nn);

			CString strObjName =  altObj.GetIDString();
			if(altObj.IsBlank())
				strObjName = LSR_TREE_ALLOBJECTS;

			TreeNodeData* pTreeNode = new TreeNodeData;
			pTreeNode->m_nodeType = TO_ITEM;
			pTreeNode->altObj	  = altObj;

			InsertItem(m_hToRoot,strObjName,pTreeNode);
			m_pTreeCtrl->Expand(m_hToRoot, TVE_EXPAND);
		}

	}
	else//default
	{

		CString strObjName =  LSR_TREE_ALLOBJECTS;

		TreeNodeData* pTreeNode = new TreeNodeData;
		pTreeNode->m_nodeType = TO_ITEM;
		pTreeNode->altObj	  = ALTObjectID();

		InsertItem(m_hToRoot,strObjName,pTreeNode);
		m_pTreeCtrl->Expand(m_hToRoot, TVE_EXPAND);
		
	}
	
	m_pTreeCtrl->Expand(m_hObjectRoot, TVE_EXPAND);

}

//void LSReportTreeVehicleDelayPerformer::LoadData()
//{
//	LandsideReportTreeDefaultPerformer::LoadData();
//}

void LSReportTreeVehicleDelayPerformer::SaveData()
{
	LandsideReportTreeDefaultPerformer::SaveData();

	ASSERT(m_pTreeCtrl != NULL);
	ASSERT(m_pParameter != NULL);

	if(m_pParameter == NULL || m_pTreeCtrl == NULL)
		return;

	LandsideBaseParam *pParam = m_pParameter;
	if(pParam == NULL)
		return;

	pParam->ClearFromObject();
	HTREEITEM hItem = m_pTreeCtrl->GetChildItem(m_hFromRoot);
	while (hItem != NULL)
	{
		TreeNodeData *pItemData = (TreeNodeData *)m_pTreeCtrl->GetItemData(hItem);

		if(pItemData != NULL)
			pParam->AddFromObject(pItemData->altObj);


		hItem = m_pTreeCtrl->GetNextSiblingItem(hItem);
	}

	pParam->ClearToObject();
	hItem = m_pTreeCtrl->GetChildItem(m_hToRoot);
	while (hItem != NULL)
	{
		TreeNodeData *pItemData = (TreeNodeData *)m_pTreeCtrl->GetItemData(hItem);

		if(pItemData != NULL)
			pParam->AddToObject(pItemData->altObj);


		hItem = m_pTreeCtrl->GetNextSiblingItem(hItem);
	}


}

//void LSReportTreeVehicleDelayPerformer::InitDefaultTree()
//{
//	LandsideReportTreeDefaultPerformer::LoadData();
//
//}
//////////////////////////////////////////////////////////////////////////
//LSReportTreeResQueuePerformer
LSReportTreeResQueuePerformer::LSReportTreeResQueuePerformer( CARCTreeCtrlEx *pTreeCtrl, InputTerminal* pInTerm, InputLandside *pInLandside, LandsideBaseParam *pParam, LandsideReportType enumReportType )
:LandsideReportTreeBasePerformer(pTreeCtrl,pInTerm,pInLandside,pParam, enumReportType)
{
	m_hObjectRoot = NULL;
}

LSReportTreeResQueuePerformer::~LSReportTreeResQueuePerformer()
{

}

void LSReportTreeResQueuePerformer::OnToolBarAdd( HTREEITEM hTreeItem )
{
	if (hTreeItem == NULL)
		return;

	TreeNodeData* pNodeData = (TreeNodeData*)m_pTreeCtrl->GetItemData(hTreeItem);
	ASSERT(pNodeData);
	if (pNodeData == NULL)
		return;

	if(pNodeData->m_nodeType == VEHICLE_ITEM_ROOT)
	{
		CDlgSelectLandsideVehicleType dlg;
		if(dlg.DoModal()== IDOK)
		{
			CString strName = dlg.GetName();
			CHierachyName hierachyName;
			hierachyName.fromString(strName);

			TreeNodeData* pTreeNode = new TreeNodeData;
			pTreeNode->m_nodeType = VEHICLE_ITEM;
			pTreeNode->vehicleName = hierachyName;

			InsertItem(hTreeItem,strName,pTreeNode);
			m_pTreeCtrl->Expand(m_hVehicleeRootItem, TVE_EXPAND);

		}
	}

	if (pNodeData->m_nodeType == OBJECT_ROOT)
	{
		CDlgSelectLandsideObject dlg( &(m_pLandside->getObjectList()));
		dlg.AddObjType(ALT_LINTERSECTION);
		dlg.AddObjType(ALT_LCURBSIDE);
		dlg.AddObjType(ALT_LCROSSWALK);
		dlg.AddObjType(ALT_LBUSSTATION);
		dlg.AddObjType(ALT_LROUNDABOUT);

		if(dlg.DoModal()== IDOK )
		{
			ALTObjectID altObj = dlg.getSelectObject();

			CString strObjName;

			if (altObj.IsBlank())
				strObjName =  LSR_TREE_ALLOBJECTS;
			else
				strObjName = altObj.GetIDString();

			TreeNodeData* pNewTreeNode = new TreeNodeData;
			pNewTreeNode->m_nodeType = FILTER_ITEM;
			pNewTreeNode->altObj	  = altObj;

			InsertItem(hTreeItem,strObjName,pNewTreeNode);
			m_pTreeCtrl->Expand(hTreeItem, TVE_EXPAND);

			m_pParameter->AddFilterObject(altObj);
		}
	}
	

}

void LSReportTreeResQueuePerformer::OnToolBarDel( HTREEITEM hTreeItem )
{
	if (hTreeItem == NULL)
		return;

	TreeNodeData* pNodeData = (TreeNodeData*)m_pTreeCtrl->GetItemData(hTreeItem);
	ASSERT(pNodeData);
	if (pNodeData == NULL)
		return;

	if(pNodeData->m_nodeType == VEHICLE_ITEM_ROOT || pNodeData->m_nodeType == OBJECT_ROOT)
		return;

	if (pNodeData->m_nodeType == VEHICLE_ITEM)
	{
		HTREEITEM hParentItem = m_pTreeCtrl->GetParentItem(hTreeItem);

		m_pTreeCtrl->DeleteItem(hTreeItem);
		if(m_pTreeCtrl->GetChildItem(hParentItem) == NULL)
		{
			TreeNodeData* pTreeNode = new TreeNodeData;
			pTreeNode->m_nodeType = VEHICLE_ITEM;
			pTreeNode->vehicleName = CHierachyName();

			InsertItem(m_hVehicleeRootItem,pTreeNode->vehicleName.toString(),pTreeNode);
			m_pTreeCtrl->Expand(m_hVehicleeRootItem, TVE_EXPAND);
		}
	}

	if(pNodeData->m_nodeType == FILTER_ITEM)
	{
		HTREEITEM hParentItem = m_pTreeCtrl->GetParentItem(hTreeItem);

		m_pTreeCtrl->DeleteItem(hTreeItem);

		if(m_pTreeCtrl->GetChildItem(hParentItem) == NULL)
		{
			CString strObjName =  LSR_TREE_ALLOBJECTS;

			TreeNodeData* pNewTreeNode = new TreeNodeData;

			pNewTreeNode->m_nodeType = pNodeData->m_nodeType;

			pNewTreeNode->altObj	  = ALTObjectID();

			InsertItem(hParentItem,strObjName,pNewTreeNode);
			m_pTreeCtrl->Expand(hParentItem, TVE_EXPAND);
		}
		return;
	}
}

void LSReportTreeResQueuePerformer::OnUpdateToolBarAdd( CCmdUI *pCmdUI )
{
	if(m_pTreeCtrl == NULL)
		return;

	HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem();
	if( hTreeItem == NULL)
		return;

	TreeNodeData* pNodeData = (TreeNodeData*)m_pTreeCtrl->GetItemData(hTreeItem);
	if(pNodeData == NULL)
		return;

	if(pNodeData->m_nodeType == OBJECT_ROOT)
	{
		pCmdUI->Enable(TRUE);
		return;
	}
	else if (pNodeData->m_nodeType == VEHICLE_ITEM_ROOT)
	{
		pCmdUI->Enable(TRUE);
		return;
	}
	else 
	{
		pCmdUI->Enable(FALSE);
		return;
	}
}

void LSReportTreeResQueuePerformer::OnUpdateToolBarDel( CCmdUI *pCmdUI )
{
	if(m_pTreeCtrl == NULL)
		return;

	HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem();
	if( hTreeItem == NULL)
		return;

	TreeNodeData* pNodeData = (TreeNodeData*)m_pTreeCtrl->GetItemData(hTreeItem);
	if(pNodeData == NULL)
		return;

	if(pNodeData->m_nodeType == FILTER_ITEM)
	{
		pCmdUI->Enable(TRUE);
		return;
	}
	else if (pNodeData->m_nodeType == VEHICLE_ITEM)
	{
		pCmdUI->Enable(TRUE);
		return;
	}
	else 
	{
		pCmdUI->Enable(FALSE);
		return;
	}
}

void LSReportTreeResQueuePerformer::InitTree()
{
	ASSERT(m_pTreeCtrl != NULL);
	ASSERT(m_pParameter != NULL);

	if(m_pParameter == NULL || m_pTreeCtrl == NULL)
		return;

	LandsideBaseParam* pParameter = m_pParameter;
	if(m_pParameter == NULL)
		return;

	m_pTreeCtrl->DeleteAllItems();
	TreeNodeData *pItemRoot = new TreeNodeData;
	pItemRoot->m_nodeType = VEHICLE_ITEM_ROOT;
	m_hVehicleeRootItem = InsertItem(NULL,_T("Vehicle Types"),pItemRoot);



	if(pParameter->HasVehicleTypeDefined())//init data from parameter
	{
		int nTypeCount = pParameter->GetVehicleTypeNameCount();
		for (int nn = 0; nn < nTypeCount; ++nn)
		{
			CHierachyName vehicleName = m_pParameter->GetVehicleTypeName(nn);

			TreeNodeData* pTreeNode = new TreeNodeData;
			pTreeNode->m_nodeType = VEHICLE_ITEM;
			pTreeNode->vehicleName = vehicleName;

			InsertItem(m_hVehicleeRootItem,vehicleName.toString(),pTreeNode);
			m_pTreeCtrl->Expand(m_hVehicleeRootItem, TVE_EXPAND);
		}

	}
	else//default
	{
		TreeNodeData* pTreeNode = new TreeNodeData;
		pTreeNode->m_nodeType = VEHICLE_ITEM;
		pTreeNode->vehicleName = CHierachyName();

		InsertItem(m_hVehicleeRootItem,pTreeNode->vehicleName.toString(),pTreeNode);
		m_pTreeCtrl->Expand(m_hVehicleeRootItem, TVE_EXPAND);
	}

	TreeNodeData *pItemRootData = new TreeNodeData;
	pItemRootData->m_nodeType = OBJECT_ROOT;
	m_hObjectRoot = InsertItem(NULL,_T("Objects"),pItemRootData);



	if(pParameter->GetFilterObjectCount())//init data from parameter
	{
		int nTypeCount = pParameter->GetFilterObjectCount();
		for (int nn = 0; nn < nTypeCount; ++nn)
		{
			ALTObjectID altObj =	m_pParameter->GetFilterObject(nn);

			CString strObjName =  altObj.GetIDString();
			if(altObj.IsBlank())
				strObjName = LSR_TREE_ALLOBJECTS;

			TreeNodeData* pTreeNode = new TreeNodeData;
			pTreeNode->m_nodeType = FILTER_ITEM;
			pTreeNode->altObj	  = altObj;

			InsertItem(m_hObjectRoot,strObjName,pTreeNode);

		}

	}
	else//default
	{

		CString strObjName =  LSR_TREE_ALLOBJECTS;

		TreeNodeData* pTreeNode = new TreeNodeData;
		pTreeNode->m_nodeType = FILTER_ITEM;
		pTreeNode->altObj	  = ALTObjectID();

		InsertItem(m_hObjectRoot,strObjName,pTreeNode);


	}

	m_pTreeCtrl->Expand(m_hObjectRoot, TVE_EXPAND);



}

void LSReportTreeResQueuePerformer::SaveData()
{
	ASSERT(m_pTreeCtrl != NULL);
	ASSERT(m_pParameter != NULL);

	if(m_pParameter == NULL || m_pTreeCtrl == NULL)
		return;

	LandsideBaseParam *pParam = m_pParameter;
	if(pParam == NULL)
		return;

	pParam->ClearVehicleType();
	pParam->ClearVehicleName();
	HTREEITEM hVehicleItem = m_pTreeCtrl->GetChildItem(m_hVehicleeRootItem);
	while (hVehicleItem != NULL)
	{
		TreeNodeData *pItemData = (TreeNodeData *)m_pTreeCtrl->GetItemData(hVehicleItem);

		if(pItemData != NULL)
			pParam->AddVehicleTypeName(pItemData->vehicleName);


		hVehicleItem = m_pTreeCtrl->GetNextSiblingItem(hVehicleItem);
	}

	pParam->ClearFilterObject();
	HTREEITEM hItem = m_pTreeCtrl->GetChildItem(m_hObjectRoot);
	while (hItem != NULL)
	{
		TreeNodeData *pItemData = (TreeNodeData *)m_pTreeCtrl->GetItemData(hItem);

		if(pItemData != NULL)
			pParam->AddFilterObject(pItemData->altObj);


		hItem = m_pTreeCtrl->GetNextSiblingItem(hItem);
	}

	
}

LRESULT LSReportTreeResQueuePerformer::DefWindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	return 0;
}

void LSReportTreeResQueuePerformer::InitDefaultTree()
{

}

void LSReportTreeResQueuePerformer::OnToolBarEdit( HTREEITEM hTreeItem )
{

}

void LSReportTreeResQueuePerformer::LoadData()
{
	InitTree();
}



LSReportTreeResThroughputPerformer::LSReportTreeResThroughputPerformer( CARCTreeCtrlEx *pTreeCtrl, InputTerminal* pInTerm, InputLandside *pInLandside, LandsideBaseParam *pParam, LandsideReportType enumReportType )
:LandsideReportTreeDefaultPerformer(pTreeCtrl,pInTerm,pInLandside,pParam, enumReportType)
{
	m_hObjectRoot = NULL;
}


LSReportTreeResThroughputPerformer::~LSReportTreeResThroughputPerformer()
{

}

void LSReportTreeResThroughputPerformer::OnToolBarAdd( HTREEITEM hTreeItem )
{
	if (hTreeItem == NULL)
		return;

	TreeNodeData* pNodeData = (TreeNodeData*)m_pTreeCtrl->GetItemData(hTreeItem);
	ASSERT(pNodeData);
	if (pNodeData == NULL)
		return;

	if(pNodeData->m_nodeType == VEHICLE_ITEM_ROOT)
	{
		CDlgSelectLandsideVehicleType dlg;
		if(dlg.DoModal()== IDOK)
		{
			CString strName = dlg.GetName();
			CHierachyName hierachyName;
			hierachyName.fromString(strName);

			TreeNodeData* pTreeNode = new TreeNodeData;
			pTreeNode->m_nodeType = VEHICLE_ITEM;
			pTreeNode->vehicleName = hierachyName;

			InsertItem(hTreeItem,strName,pTreeNode);
			m_pTreeCtrl->Expand(m_hVehicleeRootItem, TVE_EXPAND);

		}
	}

	if (pNodeData->m_nodeType == OBJECT_ROOT)
	{
		CDlgSelectLandsideObject dlg( &(m_pLandside->getObjectList()));
		dlg.AddObjType(ALT_LINTERSECTION);
		dlg.AddObjType(ALT_LCURBSIDE);
		dlg.AddObjType(ALT_LROUNDABOUT);
		dlg.AddObjType(ALT_LTRAFFICLIGHT);
		dlg.AddObjType(ALT_LTOLLGATE);
		dlg.AddObjType(ALT_LSTOPSIGN);
		dlg.AddObjType(ALT_LYIELDSIGN);
		dlg.AddObjType(ALT_LSPEEDSIGN);

		if(dlg.DoModal()== IDOK )
		{
			ALTObjectID altObj = dlg.getSelectObject();

			CString strObjName;

			if (altObj.IsBlank())
				strObjName =  LSR_TREE_ALLOBJECTS;
			else
				strObjName = altObj.GetIDString();

			TreeNodeData* pNewTreeNode = new TreeNodeData;
			pNewTreeNode->m_nodeType = FILTER_ITEM;
			pNewTreeNode->altObj	  = altObj;

			InsertItem(hTreeItem,strObjName,pNewTreeNode);
			m_pTreeCtrl->Expand(hTreeItem, TVE_EXPAND);

			m_pParameter->AddFilterObject(altObj);
		}
	}
	
}

void LSReportTreeResThroughputPerformer::OnToolBarDel( HTREEITEM hTreeItem )
{
	if (hTreeItem == NULL)
		return;

	TreeNodeData* pNodeData = (TreeNodeData*)m_pTreeCtrl->GetItemData(hTreeItem);
	ASSERT(pNodeData);
	if (pNodeData == NULL)
		return;

	if(pNodeData->m_nodeType == VEHICLE_ITEM_ROOT || pNodeData->m_nodeType == OBJECT_ROOT)
		return;

	m_pTreeCtrl->DeleteItem(hTreeItem);
}

void LSReportTreeResThroughputPerformer::OnToolBarEdit( HTREEITEM hTreeItem )
{

}

void LSReportTreeResThroughputPerformer::OnUpdateToolBarAdd( CCmdUI *pCmdUI )
{
	if(m_pTreeCtrl == NULL)
		return;

	HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem();
	if( hTreeItem == NULL)
		return;

	TreeNodeData* pNodeData = (TreeNodeData*)m_pTreeCtrl->GetItemData(hTreeItem);
	if(pNodeData == NULL)
		return;

	if(pNodeData->m_nodeType == OBJECT_ROOT || pNodeData->m_nodeType == VEHICLE_ITEM_ROOT)
	{
		pCmdUI->Enable(TRUE);
		return;
	}
	else 
	{
		pCmdUI->Enable(FALSE);
		return;
	}
}

void LSReportTreeResThroughputPerformer::OnUpdateToolBarDel( CCmdUI *pCmdUI )
{
	if(m_pTreeCtrl == NULL)
		return;

	HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem();
	if( hTreeItem == NULL)
		return;

	TreeNodeData* pNodeData = (TreeNodeData*)m_pTreeCtrl->GetItemData(hTreeItem);
	if(pNodeData == NULL)
		return;

	if(pNodeData->m_nodeType == FILTER_ITEM || pNodeData->m_nodeType == VEHICLE_ITEM)
	{
		pCmdUI->Enable(TRUE);
		return;
	}
	else 
	{
		pCmdUI->Enable(FALSE);
		return;
	}
}

void LSReportTreeResThroughputPerformer::InitTree()
{
	ASSERT(m_pTreeCtrl != NULL);
	ASSERT(m_pParameter != NULL);
	LandsideBaseParam* pParameter = m_pParameter;
	if(pParameter == NULL)
		return;
	m_pTreeCtrl->DeleteAllItems();

	TreeNodeData *pItemRoot = new TreeNodeData;
	pItemRoot->m_nodeType = VEHICLE_ITEM_ROOT;
	m_hVehicleeRootItem = InsertItem(NULL,_T("Vehicle Types"),pItemRoot);



	if(pParameter->HasVehicleTypeDefined())//init data from parameter
	{
		int nTypeCount = pParameter->GetVehicleTypeNameCount();
		for (int nn = 0; nn < nTypeCount; ++nn)
		{
			CHierachyName vehicleName = m_pParameter->GetVehicleTypeName(nn);

			TreeNodeData* pTreeNode = new TreeNodeData;
			pTreeNode->m_nodeType = VEHICLE_ITEM;
			pTreeNode->vehicleName = vehicleName;

			InsertItem(m_hVehicleeRootItem,vehicleName.toString(),pTreeNode);
			m_pTreeCtrl->Expand(m_hVehicleeRootItem, TVE_EXPAND);
		}

	}
	else//default
	{
		LandsideReportTreeDefaultPerformer::InitDefaultTree();
	}

	TreeNodeData *pItemRootData = new TreeNodeData;
	pItemRootData->m_nodeType = OBJECT_ROOT;
	m_hObjectRoot = InsertItem(NULL,_T("Resource"),pItemRootData);



	if(pParameter->GetFilterObjectCount())//init data from parameter
	{
		int nTypeCount = pParameter->GetFilterObjectCount();
		for (int nn = 0; nn < nTypeCount; ++nn)
		{
			ALTObjectID altObj =	m_pParameter->GetFilterObject(nn);

			CString strObjName =  altObj.GetIDString();
			if(altObj.IsBlank())
				strObjName = LSR_TREE_ALLOBJECTS;

			TreeNodeData* pTreeNode = new TreeNodeData;
			pTreeNode->m_nodeType = FILTER_ITEM;
			pTreeNode->altObj	  = altObj;

			InsertItem(m_hObjectRoot,strObjName,pTreeNode);

		}

	}
	else//default
	{

		CString strObjName =  LSR_TREE_ALLOBJECTS;

		TreeNodeData* pTreeNode = new TreeNodeData;
		pTreeNode->m_nodeType = FILTER_ITEM;
		pTreeNode->altObj	  = ALTObjectID();

		InsertItem(m_hObjectRoot,strObjName,pTreeNode);


	}

	m_pTreeCtrl->Expand(m_hObjectRoot, TVE_EXPAND);
}

void LSReportTreeResThroughputPerformer::SaveData()
{
	ASSERT(m_pTreeCtrl != NULL);
	ASSERT(m_pParameter != NULL);

	if(m_pParameter == NULL || m_pTreeCtrl == NULL)
		return;

	LandsideBaseParam *pParam = m_pParameter;
	if(pParam == NULL)
		return;

	pParam->ClearVehicleType();
	pParam->ClearVehicleName();
	HTREEITEM hVehicleItem = m_pTreeCtrl->GetChildItem(m_hVehicleeRootItem);
	while (hVehicleItem != NULL)
	{
		TreeNodeData *pItemData = (TreeNodeData *)m_pTreeCtrl->GetItemData(hVehicleItem);

		if(pItemData != NULL)
			pParam->AddVehicleTypeName(pItemData->vehicleName);


		hVehicleItem = m_pTreeCtrl->GetNextSiblingItem(hVehicleItem);
	}

	pParam->ClearFilterObject();
	HTREEITEM hObjectItem = m_pTreeCtrl->GetChildItem(m_hObjectRoot);
	while (hObjectItem != NULL)
	{
		TreeNodeData *pItemData = (TreeNodeData *)m_pTreeCtrl->GetItemData(hObjectItem);

		if(pItemData != NULL)
			pParam->AddFilterObject(pItemData->altObj);


		hObjectItem = m_pTreeCtrl->GetNextSiblingItem(hObjectItem);
	}
}

void LSReportTreeResThroughputPerformer::InitDefaultTree()
{

}

LRESULT LSReportTreeResThroughputPerformer::DefWindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	return 0;
}

void LSReportTreeResThroughputPerformer::LoadData()
{
	InitTree();
}
