//OnboardReportTreePerformer.cpp impelment of tree performer
#include "stdafx.h"
#include "OnboardReportTreePerformer.h"
#include "MFCExControl/ARCTreeCtrlEx.h"
#include "OnboardReport/OnboardBaseParameter.h"
#include "OnboardReport/OnboardDurationParameter.h"
#include "Resource.h"
#include "FlightDialog.h"
#include "DlgNewPassengerType.h"
#include "../inputs/in_term.h"


//------------------------------------------------------------------------------------------
//Summary:
//		OnboardReportPerformer instance
//------------------------------------------------------------------------------------------
COnboardReportPerformer::COnboardReportPerformer( CARCTreeCtrlEx *pTreeCtrl,InputTerminal* pInTerm, OnboardBaseParameter *pParam, OnboardReportType enumReportType )
:m_pTreeCtrl(pTreeCtrl)
,m_pInTerm(pInTerm)
,m_pParameter(pParam)
,m_enumReportType(enumReportType)
{

}

//-----------------------------------------------------------------------------------------
//Summary:
//		Destroy class object
//-----------------------------------------------------------------------------------------
COnboardReportPerformer::~COnboardReportPerformer()
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
HTREEITEM COnboardReportPerformer::InsertItem( HTREEITEM hItem,const CString& sNode,TreeNodeData* pNodeData )
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
void COnboardReportPerformer::UpdateOnboardParameter(OnboardBaseParameter* pParameter)
{
	m_pParameter = pParameter;
}

OnboardReportType COnboardReportPerformer::GetReportType() const
{
	return m_enumReportType;
}


COnboardReportTreeDefaultPerformer::COnboardReportTreeDefaultPerformer(CARCTreeCtrlEx *pTreeCtrl,InputTerminal* pInTerm, OnboardBaseParameter *pParam, OnboardReportType enumReportType)
:COnboardReportPerformer(pTreeCtrl,pInTerm,pParam, enumReportType)
{

}

COnboardReportTreeDefaultPerformer::~COnboardReportTreeDefaultPerformer()
{

}

void COnboardReportTreeDefaultPerformer::OnToolBarAdd( HTREEITEM hTreeItem )
{
	if (hTreeItem == NULL)
		return;
	
	COnboardReportTreeDefaultPerformer::DefaultTreeNodeData* pNodeData = (COnboardReportTreeDefaultPerformer::DefaultTreeNodeData*)m_pTreeCtrl->GetItemData(hTreeItem);
	ASSERT(pNodeData);
	if (pNodeData == NULL)
		return;
	
	//passenger type
	if (pNodeData->m_nodeType ==COnboardReportPerformer::PASSGENERTYPE_ITEM_ROOT)
	{
		CPassengerType paxType(m_pInTerm->inStrDict);
		CDlgNewPassengerType dlg(&paxType,m_pInTerm,m_pTreeCtrl->GetParent());
		if( dlg.DoModal() == IDOK )
		{
			CString strNewPaxType;
			strNewPaxType = paxType.PrintStringForShow();

			//passenger type
			COnboardReportTreeDefaultPerformer::DefaultTreeNodeData* pTreeNode = new COnboardReportTreeDefaultPerformer::DefaultTreeNodeData;;
			pTreeNode->m_nodeType = FLIGHTTYPE_ITEM;
			pTreeNode->m_paxtype = paxType;
			InsertItem(hTreeItem,strNewPaxType,pTreeNode);
		}
		m_pTreeCtrl->Expand(hTreeItem,TVE_EXPAND);
	}//flight type
	else if (pNodeData->m_nodeType ==COnboardReportPerformer::FLIGHTTYPE_ITEM_ROOT)
	{
 		CFlightDialog dlg( m_pTreeCtrl->GetParent() );
 		if( dlg.DoModal() == IDOK )
 		{
 			FlightConstraint newFltConstr = dlg.GetFlightSelection();

 			CString strNewFlightType;
 			newFltConstr.screenPrint(strNewFlightType.GetBuffer(1024));
 			strNewFlightType.ReleaseBuffer();

 			//flight type
 			COnboardReportTreeDefaultPerformer::DefaultTreeNodeData* pTreeNode = new COnboardReportTreeDefaultPerformer::DefaultTreeNodeData;;
 			pTreeNode->m_nodeType = FLIGHTTYPE_ITEM;
 			pTreeNode->m_flconst = newFltConstr;
 			InsertItem(hTreeItem,strNewFlightType,pTreeNode);
 		}
		m_pTreeCtrl->Expand(hTreeItem,TVE_EXPAND);
	}
	
}

void COnboardReportTreeDefaultPerformer::OnToolBarDel( HTREEITEM hTreeItem )
{
	if (hTreeItem == NULL)
		return;

	COnboardReportTreeDefaultPerformer::DefaultTreeNodeData* pNodeData = (COnboardReportTreeDefaultPerformer::DefaultTreeNodeData*)m_pTreeCtrl->GetItemData(hTreeItem);
	ASSERT(pNodeData);
	if (pNodeData == NULL)
		return;

	if (pNodeData->m_nodeType ==COnboardReportPerformer::PASSENGERTYPE_ITEM)
	{
		m_pTreeCtrl->DeleteItem(hTreeItem);
		if (m_pParameter->getPaxTypeCount() == 0)
		{
			InitDefaultTree();
		}
	}
	else if(pNodeData->m_nodeType ==COnboardReportPerformer::FLIGHTTYPE_ITEM)
	{
		m_pTreeCtrl->DeleteItem(hTreeItem);
	}
}

void COnboardReportTreeDefaultPerformer::OnToolBarEdit( HTREEITEM hTreeItem )
{

}

LRESULT COnboardReportTreeDefaultPerformer::DefWindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	return 1;
}

void COnboardReportTreeDefaultPerformer::OnUpdateToolBarAdd( CCmdUI *pCmdUI )
{
	if(m_pTreeCtrl == NULL)
		return;

	HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem();
	if( hTreeItem == NULL)
		return;

	COnboardReportTreeDefaultPerformer::DefaultTreeNodeData* pNodeData = (COnboardReportTreeDefaultPerformer::DefaultTreeNodeData*)m_pTreeCtrl->GetItemData(hTreeItem);
	if(pNodeData == NULL)
		return;

	if(pNodeData->m_nodeType == PASSGENERTYPE_ITEM_ROOT)
	{
		pCmdUI->Enable(TRUE);
	}
	else if(pNodeData->m_nodeType == FLIGHTTYPE_ITEM_ROOT)
	{
		pCmdUI->Enable(TRUE);
	}
	else
		pCmdUI->Enable(FALSE);
}

void COnboardReportTreeDefaultPerformer::OnUpdateToolBarDel( CCmdUI *pCmdUI )
{
	if(m_pTreeCtrl == NULL)
		return;

	HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem();
	if( hTreeItem == NULL)
		return;

	COnboardReportTreeDefaultPerformer::DefaultTreeNodeData* pNodeData = (COnboardReportTreeDefaultPerformer::DefaultTreeNodeData*)m_pTreeCtrl->GetItemData(hTreeItem);
	if(pNodeData == NULL)
		return;

	if(pNodeData->m_nodeType == PASSENGERTYPE_ITEM)
	{
		pCmdUI->Enable(TRUE);
	}
	else if(pNodeData->m_nodeType == FLIGHTTYPE_ITEM)
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
void COnboardReportTreeDefaultPerformer::InitTree()
{
	ASSERT(m_pTreeCtrl != NULL);
	ASSERT(m_pParameter != NULL);

	if(m_pParameter == NULL || m_pTreeCtrl == NULL)
		return;

	OnboardBaseParameter* pParameter = m_pParameter;
	if(m_pParameter == NULL)
		return;

	m_pTreeCtrl->DeleteAllItems();

	COnboardReportTreeDefaultPerformer::DefaultTreeNodeData *pItemPaxRoot = new COnboardReportTreeDefaultPerformer::DefaultTreeNodeData;
	pItemPaxRoot->m_nodeType = PASSGENERTYPE_ITEM_ROOT;
	m_hPaxTypeRootItem = InsertItem(NULL,_T("Pax Type"),pItemPaxRoot);

	COnboardReportTreeDefaultPerformer::DefaultTreeNodeData* pItemFltPaxRoot = new COnboardReportTreeDefaultPerformer::DefaultTreeNodeData; 
	pItemFltPaxRoot->m_nodeType = FLIGHTTYPE_ITEM_ROOT;
	m_hFltTypeRootItem = InsertItem(NULL,_T("Flight Type"),pItemFltPaxRoot);


	if(pParameter->Empty())//default
	{
		InitDefaultTree();
	}
	else//init data from parameter
	{
		//passenger type
		int nPaxTypeCount =(int)pParameter->getPaxTypeCount();
		for (int nPaxType = 0; nPaxType < nPaxTypeCount; ++ nPaxType)
		{
			CPassengerType& passengerype = pParameter->getPaxType(nPaxType);
			CString szPassengerType;
			szPassengerType = passengerype.PrintStringForShow();
			
			COnboardReportTreeDefaultPerformer::DefaultTreeNodeData *pItemData = new COnboardReportTreeDefaultPerformer::DefaultTreeNodeData;
			pItemData->m_nodeType = PASSENGERTYPE_ITEM;
			pItemData->m_paxtype = passengerype;
			HTREEITEM hItemPaxType = InsertItem(m_hPaxTypeRootItem,szPassengerType,pItemData);
		}
		//flight type
		int nFltTypeCount = (int)pParameter->getFlightConstraintCount();
		for (int nFltType = 0; nFltType < nFltTypeCount; ++ nFltType)
		{
			FlightConstraint& fltType = pParameter->getFlightConstraint(nFltType);
			CString szFltType;
			fltType.screenFltPrint(szFltType);

			COnboardReportTreeDefaultPerformer::DefaultTreeNodeData *pItemData = new COnboardReportTreeDefaultPerformer::DefaultTreeNodeData;
			pItemData->m_nodeType = FLIGHTTYPE_ITEM;
			pItemData->m_flconst = fltType;
			HTREEITEM hItemFltType = InsertItem(m_hFltTypeRootItem,szFltType,pItemData);
		}
	}
	m_pTreeCtrl->Expand(m_hPaxTypeRootItem,TVE_EXPAND);
	m_pTreeCtrl->Expand(m_hFltTypeRootItem,TVE_EXPAND);
}

void COnboardReportTreeDefaultPerformer::LoadData()
{
	InitTree();
}

void COnboardReportTreeDefaultPerformer::SaveData()
{
	ASSERT(m_pTreeCtrl != NULL);
	ASSERT(m_pParameter != NULL);

	if(m_pParameter == NULL || m_pTreeCtrl == NULL)
		return;

	OnboardBaseParameter *pParam = m_pParameter;
	if(pParam == NULL)
		return;

	pParam->ClearPaxType();
	pParam->ClearFlightConstraint();

	if (!ValidFlightConstraint())
	{
		AfxMessageBox( "Please define arrival or departure of flight type");
		return;
	}
	//passenger type
	{
		HTREEITEM hItemPax = NULL;
		hItemPax = m_pTreeCtrl->GetChildItem(m_hPaxTypeRootItem);
		if(hItemPax == NULL)
			return;

		while(hItemPax != NULL)
		{
			//passenger type item
			COnboardReportTreeDefaultPerformer::DefaultTreeNodeData *pItemData = (COnboardReportTreeDefaultPerformer::DefaultTreeNodeData *)m_pTreeCtrl->GetItemData(hItemPax);
			if(pItemData == NULL)
				return;

			hItemPax = m_pTreeCtrl->GetNextSiblingItem(hItemPax);
			pParam->AddPaxType(pItemData->m_paxtype);
		}
	}
	
	//flight type
	{
		HTREEITEM hItemFlt = NULL;
		hItemFlt = m_pTreeCtrl->GetChildItem(m_hFltTypeRootItem);
		if(hItemFlt == NULL)
			return;

		while(hItemFlt != NULL)
		{
			//flight type item
			COnboardReportTreeDefaultPerformer::DefaultTreeNodeData *pItemData = (COnboardReportTreeDefaultPerformer::DefaultTreeNodeData *)m_pTreeCtrl->GetItemData(hItemFlt);
			if(pItemData == NULL)
				return;

			hItemFlt = m_pTreeCtrl->GetNextSiblingItem(hItemFlt);
			pParam->AddFlightConstraint(pItemData->m_flconst);
		}
	}
}


//-----------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------
//Summary:
//		init tree with default data
//-----------------------------------------------------------------------------------------
void COnboardReportTreeDefaultPerformer::InitDefaultTree()
{
	OnboardBaseParameter* pParameter = m_pParameter;
	if(pParameter == NULL)
		return;
 	//passenger type
 	{
 		CPassengerType passengerype(NULL);
 		CString szPassengerType;
 		szPassengerType = passengerype.PrintStringForShow();
 
 		COnboardReportTreeDefaultPerformer::DefaultTreeNodeData *pItemData = new COnboardReportTreeDefaultPerformer::DefaultTreeNodeData;
 		pItemData->m_nodeType = PASSENGERTYPE_ITEM;
		pItemData->m_paxtype = passengerype;
 		HTREEITEM hItemPaxType = InsertItem(m_hPaxTypeRootItem,szPassengerType,pItemData);
 	}
}

bool COnboardReportTreeDefaultPerformer::ValidFlightConstraint( void )
{
	HTREEITEM hItemFlt = m_pTreeCtrl->GetChildItem(m_hFltTypeRootItem);

	if (hItemFlt == NULL)
		return false;
	
	while(hItemFlt != NULL)
	{
		//flight type item
		COnboardReportTreeDefaultPerformer::DefaultTreeNodeData *pItemData = (COnboardReportTreeDefaultPerformer::DefaultTreeNodeData *)m_pTreeCtrl->GetItemData(hItemFlt);
		if(pItemData == NULL)
			continue;

		hItemFlt = m_pTreeCtrl->GetNextSiblingItem(hItemFlt);
		FLTCNSTR_MODE fltMode = pItemData->m_flconst.GetFltConstraintMode();
		if (fltMode == ENUM_FLTCNSTR_MODE_ALL || fltMode == ENUM_FLTCNSTR_MODE_THU)
		{
			return false;
		}
	}

	return true;
}