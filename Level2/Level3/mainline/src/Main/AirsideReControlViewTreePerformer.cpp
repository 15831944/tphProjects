#include "StdAfx.h"
#include ".\airsiderecontrolviewtreeperformer.h"
#include "../AirsideReport/AirsideRunwayUtilizationReportParam.h"
#include "../InputAirside/AirRoute.h"
#include "DlgSelectReportRunway.h"
#include "DlgSelectAirRoute.h"
#include "Resource.h"
#include "FlightDialog.h"
#include "../MFCExControl/ARCTreeCtrlEx.h"
#include "..\Common\WinMsg.h"
#include "../AirsideReport/AirsideFlightRunwayDelayReportPara.h"
#include "../AirsideReport/AirsideRunwayCrossingsPara.h"
#include "DlgSelectALTObject.h"
#include "../AirsideReport/FlightStandOperationParameter.h"
#include "../AirsideReport/AirsideIntersectionReportParam.h"
#include "../AirsideGUI\DlgTaxiwaySelect.h"
#include "../InputAirside/Taxiway.h"
#include "InputAirside/InputAirside.h"
#include "../AirsideGUI/DlgTaxiwayFamilySelect.h"
#include "../AirsideReport/AirsideTaxiwayUtilizationPara.h"
#include "../AirsideReport/AirsideControllerWorkloadParam.h"
#include "DlgSelectMulti_ALTObjectList.h"
#include "../AirsideGUI/NodeViewDbClickHandler.h"
#include "../AirsideReport/AirsideTakeoffProcessParameter.h"

using namespace AirsideReControlView;


AirsideReControlView::CTreePerformer::CTreePerformer(int nProjID, CARCTreeCtrlEx *pTreeCtrl, CParameters *pParam )
{
	ASSERT(pTreeCtrl != NULL);
	ASSERT( pParam != NULL);

	m_pTreeCtrl = pTreeCtrl;
	m_pParam = pParam;
	m_nProjID = nProjID;
}

CTreePerformer::~CTreePerformer(void)
{
	std::vector<BaseTreeItemData *>::iterator iter = m_vPoiterNeedDel.begin();
	for (; iter != m_vPoiterNeedDel.end(); ++iter)
	{
		delete *iter;
	}
	m_vPoiterNeedDel.clear();
}

void AirsideReControlView::CTreePerformer::OnUpdateToolBarAdd( CCmdUI *pCmdUI )
{

}

void AirsideReControlView::CTreePerformer::OnUpdateToolBarDel( CCmdUI *pCmdUI )
{

}

LRESULT AirsideReControlView::CTreePerformer::DefWindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	return 0;
}

HTREEITEM AirsideReControlView::CTreePerformer::InsertItem( HTREEITEM hParentItem, const CString& strName, BaseTreeItemData *pItemData )
{
	ASSERT(hParentItem != NULL);
	ASSERT(m_pTreeCtrl != NULL);
	if(hParentItem == NULL || m_pTreeCtrl == NULL)
		return NULL;

	HTREEITEM hInsertItem = m_pTreeCtrl->InsertItem(strName,hParentItem);
	if(hInsertItem != NULL)
	{
		m_pTreeCtrl->SetItemData(hInsertItem,(DWORD_PTR)pItemData);
		m_vPoiterNeedDel.push_back(pItemData);
		return hInsertItem;
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////
//AirsideReControlView::CRunwayUtilizationTreePerformer
AirsideReControlView::CRunwayUtilizationTreePerformer::CRunwayUtilizationTreePerformer(int nProjID,  CARCTreeCtrlEx *pTreeCtrl, CParameters *pParam )
:CTreePerformer(nProjID,pTreeCtrl,pParam)
{
	m_hItemRunwayRoot = NULL;
	m_hOperationItemSelected = NULL;
}

AirsideReControlView::CRunwayUtilizationTreePerformer::~CRunwayUtilizationTreePerformer()
{

}

void AirsideReControlView::CRunwayUtilizationTreePerformer::InitTree()
{
	ASSERT(m_pTreeCtrl != NULL);
	ASSERT(m_pParam != NULL);
	
	if(m_pParam == NULL || m_pTreeCtrl == NULL)
		return;

	CAirsideRunwayUtilizationReportParam *pUtiParam = (CAirsideRunwayUtilizationReportParam *)m_pParam;
	if(pUtiParam == NULL)
		return;

	m_pTreeCtrl->DeleteAllItems();

	m_hItemRunwayRoot = m_pTreeCtrl->InsertItem(_T("Runway"));
	TreeItemData *pItemDataRoot = new TreeItemData;
	pItemDataRoot->itemType = Item_RunwayRoot;
	m_pTreeCtrl->SetItemData(m_hItemRunwayRoot,(DWORD_PTR)pItemDataRoot);


	if(pUtiParam->m_vReportMarkParam.size() == 0)//default
	{
		InitDefaltTree();
	}
	else//init data from parameter
	{
		int nRunwayParamCount =(int)pUtiParam->m_vReportMarkParam.size();
		for (int nRunwayParam = 0; nRunwayParam < nRunwayParamCount; ++ nRunwayParam)
		{
			CAirsideRunwayUtilizationReportParam::CRunwayMarkParam& runwayParam = pUtiParam->m_vReportMarkParam[nRunwayParam];

			//runway, all
			HTREEITEM hItemRunway = m_pTreeCtrl->InsertItem(runwayParam.GetRunwayNames(),m_hItemRunwayRoot);
			TreeItemData *pItemData = new TreeItemData;
			pItemData->itemType = Item_Runway;
			pItemData->vRunwayMark = runwayParam.m_vRunwayMark;
			m_pTreeCtrl->SetItemData(hItemRunway,(DWORD_PTR)pItemData);
			
			int nOperationCount = runwayParam.m_vOperationParam.size();

			for (int nOperation = 0; nOperation < nOperationCount; ++ nOperation)
			{
				//operation
				HTREEITEM hItemOperation = m_pTreeCtrl->InsertItem(_T("Operation : ") + runwayParam.m_vOperationParam[nOperation].GetOperationName(),hItemRunway);
				pItemData = new TreeItemData;
				pItemData->itemType = Item_Operation;
				pItemData->nOperation = (int)runwayParam.m_vOperationParam[nOperation].m_enumOperation;

				m_pTreeCtrl->SetItemData(hItemOperation,(DWORD_PTR)pItemData);

				//airroute root
				HTREEITEM hItemAirRouteRoot = m_pTreeCtrl->InsertItem(_T("AirRoute"),hItemOperation);
				pItemData = new TreeItemData;
				pItemData->itemType = Item_AirRouteRoot;
				m_pTreeCtrl->SetItemData(hItemAirRouteRoot,(DWORD_PTR)pItemData);

				for (int nAirRoute = 0; nAirRoute <  (int)runwayParam.m_vOperationParam[nOperation].m_vAirRoute.size(); ++nAirRoute)
				{
					//airroute
					HTREEITEM hItemAirRoute = m_pTreeCtrl->InsertItem(runwayParam.m_vOperationParam[nOperation].m_vAirRoute[nAirRoute].m_nRouteName,hItemAirRouteRoot);
					pItemData = new TreeItemData;
					pItemData->itemType = Item_AirRoute;
					pItemData->nObjID = runwayParam.m_vOperationParam[nOperation].m_vAirRoute[nAirRoute].m_nRouteID;
					pItemData->strObjName = runwayParam.m_vOperationParam[nOperation].m_vAirRoute[nAirRoute].m_nRouteName;
					m_pTreeCtrl->SetItemData(hItemAirRoute,(DWORD_PTR)pItemData);
				}

				//flight type root
				HTREEITEM hItemFlightRoot = m_pTreeCtrl->InsertItem(_T("Flight Constraint"),hItemOperation);
				pItemData = new TreeItemData;
				pItemData->itemType = Item_FlightRoot;
				m_pTreeCtrl->SetItemData(hItemFlightRoot,(DWORD_PTR)pItemData);
				
				for (int nCons = 0; nCons < (int)runwayParam.m_vOperationParam[nOperation].m_vFlightConstraint.size(); ++ nCons)
				{
					CString strFlightType;
					runwayParam.m_vOperationParam[nOperation].m_vFlightConstraint[nCons].screenPrint(strFlightType.GetBuffer(1024));
					strFlightType.ReleaseBuffer();

					//flight type
					HTREEITEM hItemFlight = m_pTreeCtrl->InsertItem(strFlightType,hItemFlightRoot);
					pItemData = new TreeItemData;
					pItemData->itemType = Item_Flight;
					pItemData->fltCons = runwayParam.m_vOperationParam[nOperation].m_vFlightConstraint[nCons];
					m_pTreeCtrl->SetItemData(hItemFlight,(DWORD_PTR)pItemData);
				}	
				m_pTreeCtrl->Expand(hItemAirRouteRoot,TVE_EXPAND);
				m_pTreeCtrl->Expand(hItemFlightRoot,TVE_EXPAND);
				m_pTreeCtrl->Expand(hItemOperation, TVE_EXPAND);
			}


			m_pTreeCtrl->Expand(hItemRunway,TVE_EXPAND);
		}

	}
	m_pTreeCtrl->Expand(m_hItemRunwayRoot,TVE_EXPAND);
}

void AirsideReControlView::CRunwayUtilizationTreePerformer::LoadData()
{
	InitTree();
}

void AirsideReControlView::CRunwayUtilizationTreePerformer::SaveData()
{
	ASSERT(m_pTreeCtrl != NULL);
	ASSERT(m_pParam != NULL);

	if(m_pParam == NULL || m_pTreeCtrl == NULL)
		return;

	CAirsideRunwayUtilizationReportParam *pUtiParam = (CAirsideRunwayUtilizationReportParam *)m_pParam;
	if(pUtiParam == NULL)
		return;

	pUtiParam->m_vReportMarkParam.clear();
	
	HTREEITEM hItemRunway = NULL;
	hItemRunway = m_pTreeCtrl->GetChildItem(m_hItemRunwayRoot);
	if(hItemRunway == NULL)
		return;

	while(hItemRunway != NULL)
	{
		
		CAirsideRunwayUtilizationReportParam::CRunwayMarkParam runwayParam;
		//runway item
		TreeItemData *pItemData = (TreeItemData *)m_pTreeCtrl->GetItemData(hItemRunway);
		if(pItemData == NULL)
			return;
		runwayParam.m_vRunwayMark = pItemData->vRunwayMark;

		//operation
		HTREEITEM hItemOperation = m_pTreeCtrl->GetChildItem(hItemRunway);

		while(hItemOperation != NULL)
		{
			CAirsideRunwayUtilizationReportParam::CRunwayOperationParam runwayOperation;
			pItemData = (TreeItemData *)m_pTreeCtrl->GetItemData(hItemOperation);
			runwayOperation.m_enumOperation = (CAirsideRunwayUtilizationReportParam::RunwayOperation)pItemData->nOperation;
			
			//airroute route
			HTREEITEM hItemAirRouteRoot = m_pTreeCtrl->GetChildItem(hItemOperation);
			if(hItemAirRouteRoot != NULL)
			{
				HTREEITEM hItemAirRoute = m_pTreeCtrl->GetChildItem(hItemAirRouteRoot);
				while(hItemAirRoute != NULL)
				{
					pItemData = (TreeItemData *)m_pTreeCtrl->GetItemData(hItemAirRoute);
					if(pItemData)
					{
						CAirsideRunwayUtilizationReportParam::CAirRouteParam airRouteParam;

						airRouteParam.m_nRouteID = pItemData->nObjID;
						airRouteParam.m_nRouteName = pItemData->strObjName;
						runwayOperation.m_vAirRoute.push_back(airRouteParam);

					}
					hItemAirRoute = m_pTreeCtrl->GetNextSiblingItem(hItemAirRoute);
				}
			}

			//flight constraint
			HTREEITEM hItemFltRoot = m_pTreeCtrl->GetNextSiblingItem(hItemAirRouteRoot);
			
			if(hItemFltRoot != NULL)
			{
				HTREEITEM hItemFlight = m_pTreeCtrl->GetChildItem(hItemFltRoot);
				while(hItemFlight)
				{
					pItemData = (TreeItemData *)m_pTreeCtrl->GetItemData(hItemFlight);
					if(pItemData)
					{
						runwayOperation.m_vFlightConstraint.push_back(pItemData->fltCons);
					}

					hItemFlight = m_pTreeCtrl->GetNextSiblingItem(hItemFlight);
				}
			}

			//runway operation
			hItemOperation = m_pTreeCtrl->GetNextSiblingItem(hItemOperation);
			runwayParam.m_vOperationParam.push_back(runwayOperation);
		}
	
		//runway
	 hItemRunway = m_pTreeCtrl->GetNextSiblingItem(hItemRunway);
	 pUtiParam->m_vReportMarkParam.push_back(runwayParam);
	}


}

void AirsideReControlView::CRunwayUtilizationTreePerformer::OnToolBarAdd( HTREEITEM hTreeItem )
{
	if(m_pTreeCtrl == NULL)
		return;

	if(hTreeItem == NULL)
		return;

	TreeItemData *pItemData = (TreeItemData *)m_pTreeCtrl->GetItemData(hTreeItem);
	if(pItemData == NULL)
		return;

	if(pItemData->itemType == Item_RunwayRoot)//new runway
	{
		CDlgSelectReportRunway dlg(m_nProjID,m_pTreeCtrl->GetParent());
		if(dlg.DoModal() == IDOK)
		{


			TreeItemData *pItemDataRunway = new TreeItemData;
			pItemDataRunway->itemType = Item_Runway;
			pItemDataRunway->vRunwayMark = dlg.m_vRunwayMarkSel;

			HTREEITEM hItemRunway = m_pTreeCtrl->InsertItem(GetRunwayNames(pItemDataRunway->vRunwayMark),m_hItemRunwayRoot);
			m_pTreeCtrl->SetItemData(hItemRunway,(DWORD_PTR)pItemDataRunway);

			InitDefaltRunway(hItemRunway);
			m_pTreeCtrl->Expand(m_hItemRunwayRoot,TVE_EXPAND);

		}
	}
	else if(pItemData->itemType == Item_Runway)//new operation
	{
		InitDefaltRunway(hTreeItem);//default operation
	}
	else if(pItemData->itemType == Item_AirRouteRoot)//new airroute
	{
		CDlgReportSelectAirRoute dlg(m_nProjID,m_pTreeCtrl->GetParent());
		if(dlg.DoModal() == IDOK)
		{
			int nAirRouteCount = (int)dlg.m_vAirARouteSel.size();

			for (int nAirRoute = 0; nAirRoute < nAirRouteCount; ++nAirRoute)
			{	
				CAirRoute *pAirRoute = dlg.m_vAirARouteSel[nAirRoute];
				if(pAirRoute == NULL)
					continue;

				HTREEITEM hItemAirRoute = m_pTreeCtrl->InsertItem(pAirRoute->getName(),hTreeItem);
				TreeItemData * pItemDataAirRoute = new TreeItemData;
				pItemDataAirRoute->itemType = Item_AirRoute;
				pItemDataAirRoute->nObjID = pAirRoute->getID();
				pItemDataAirRoute->strObjName = pAirRoute->getName();
				m_pTreeCtrl->SetItemData(hItemAirRoute,(DWORD_PTR)pItemDataAirRoute);

				m_pTreeCtrl->Expand(hTreeItem,TVE_EXPAND);

			}
			m_pTreeCtrl->Expand(hTreeItem,TVE_EXPAND);
		}
	}
	else if(pItemData->itemType == Item_FlightRoot)//new flight root
	{
		CFlightDialog dlg( m_pTreeCtrl->GetParent() );
		if( dlg.DoModal() == IDOK )
		{
			FlightConstraint newFltConstr = dlg.GetFlightSelection();

			CString strNewFlightType;
			newFltConstr.screenPrint(strNewFlightType.GetBuffer(1024));
			strNewFlightType.ReleaseBuffer();

			//flight type
			HTREEITEM hItemFlight = m_pTreeCtrl->InsertItem(strNewFlightType,hTreeItem);
			TreeItemData* pItemDataFlight = new TreeItemData;
			pItemDataFlight->itemType = Item_Flight;
			pItemDataFlight->fltCons = newFltConstr;
			m_pTreeCtrl->SetItemData(hItemFlight,(DWORD_PTR)pItemDataFlight);
		}
		m_pTreeCtrl->Expand(hTreeItem,TVE_EXPAND);


	}
	else if(pItemData->itemType == Item_Operation)
	{
		OnTreeItemDoubleClick(hTreeItem);
	}

}

void AirsideReControlView::CRunwayUtilizationTreePerformer::OnToolBarDel( HTREEITEM hTreeItem )
{
	if(hTreeItem == NULL)
		return;

	TreeItemData *pItemData = (TreeItemData *)m_pTreeCtrl->GetItemData(hTreeItem);
	if(pItemData == NULL)
		return;

	if(pItemData->itemType == Item_Runway)
	{
		HTREEITEM hParentItem = m_pTreeCtrl->GetParentItem(hTreeItem);
		m_pTreeCtrl->DeleteItem(hTreeItem);
		HTREEITEM hNextSibItem = m_pTreeCtrl->GetChildItem(hParentItem);

		if(hNextSibItem == NULL)
			InitDefaltTree();

		m_pTreeCtrl->Expand(m_hItemRunwayRoot,TVE_EXPAND);

	}
	else if(pItemData->itemType == Item_AirRoute)
	{	

		HTREEITEM hParentItem = m_pTreeCtrl->GetParentItem(hTreeItem);
		m_pTreeCtrl->DeleteItem(hTreeItem);	
		HTREEITEM hNextSibItem = m_pTreeCtrl->GetChildItem(hParentItem);
		if(hNextSibItem == NULL && hParentItem != NULL)
		{
			//airroute all
			HTREEITEM hItemAirRoute = m_pTreeCtrl->InsertItem(_T("All"),hParentItem);
			pItemData = new TreeItemData;
			pItemData->itemType = Item_AirRoute;
			pItemData->strObjName = _T("All");
			m_pTreeCtrl->SetItemData(hItemAirRoute,(DWORD_PTR)pItemData);

			m_pTreeCtrl->Expand(hParentItem,TVE_EXPAND);
		}
	}
	else if(pItemData->itemType == Item_Flight)
	{	
		HTREEITEM hParentItem = m_pTreeCtrl->GetParentItem(hTreeItem);
		m_pTreeCtrl->DeleteItem(hTreeItem);
		HTREEITEM hNextSibItem = m_pTreeCtrl->GetChildItem(hParentItem);

		if(hNextSibItem == NULL && hParentItem != NULL)
		{
			//flight type
			HTREEITEM hItemFlight = m_pTreeCtrl->InsertItem(_T("Default"),hParentItem);
			pItemData = new TreeItemData;
			pItemData->itemType = Item_Flight;
			m_pTreeCtrl->SetItemData(hItemFlight,(DWORD_PTR)pItemData);

			m_pTreeCtrl->Expand(hParentItem,TVE_EXPAND);
		}
	}
	else if(pItemData->itemType == Item_Operation)
	{
		HTREEITEM hParentItem = m_pTreeCtrl->GetParentItem(hTreeItem);
		m_pTreeCtrl->DeleteItem(hTreeItem);
		HTREEITEM hNextSibItem = m_pTreeCtrl->GetChildItem(hParentItem);


		if(hNextSibItem == NULL && hParentItem != NULL)
		{
			InitDefaltRunway(hParentItem);
		}
		m_pTreeCtrl->Expand(hParentItem,TVE_EXPAND);

	}
}

void AirsideReControlView::CRunwayUtilizationTreePerformer::OnToolBarEdit( HTREEITEM hTreeItem )
{

}

void AirsideReControlView::CRunwayUtilizationTreePerformer::OnUpdateToolBarAdd( CCmdUI *pCmdUI )
{
	if(m_pTreeCtrl == NULL)
		return;

	HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem();
	if( hTreeItem == NULL)
		return;

	TreeItemData *pItemData = (TreeItemData *)m_pTreeCtrl->GetItemData(hTreeItem);
	if(pItemData == NULL)
		return;

	if(pItemData->itemType == Item_RunwayRoot)
	{
		pCmdUI->Enable(TRUE);
	}
	else if(pItemData->itemType == Item_AirRouteRoot)
	{
		pCmdUI->Enable(TRUE);
	}
	else if(pItemData->itemType == Item_FlightRoot)
	{
		pCmdUI->Enable(TRUE);
	}
	else if(pItemData->itemType == Item_Operation)
	{
		pCmdUI->Enable(TRUE);
	}
	else if(pItemData->itemType == Item_Runway)
	{
		pCmdUI->Enable(TRUE);
	}
	else
		pCmdUI->Enable(FALSE);

}

void AirsideReControlView::CRunwayUtilizationTreePerformer::OnUpdateToolBarDel( CCmdUI *pCmdUI )
{
	if(m_pTreeCtrl == NULL)
		return;

	HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem();
	if( hTreeItem == NULL)
		return;

	TreeItemData *pItemData = (TreeItemData *)m_pTreeCtrl->GetItemData(hTreeItem);
	if(pItemData == NULL)
		return;

	if(pItemData->itemType == Item_Runway)
	{
		pCmdUI->Enable(TRUE);
	}
	else if(pItemData->itemType == Item_AirRoute)
	{
		pCmdUI->Enable(TRUE);
	}
	else if(pItemData->itemType == Item_Flight)
	{
		pCmdUI->Enable(TRUE);
	}
	else if(pItemData->itemType == Item_Operation)
	{
		pCmdUI->Enable(TRUE);
	}
	else
		pCmdUI->Enable(FALSE);
}

void AirsideReControlView::CRunwayUtilizationTreePerformer::InitDefaltTree()
{
	//runway, all
	HTREEITEM hItemRunway = m_pTreeCtrl->InsertItem(_T("All"),m_hItemRunwayRoot);
	TreeItemData *pItemData = new TreeItemData;
	pItemData->itemType = Item_Runway;
	CAirsideReportRunwayMark defaultMark;
	defaultMark.SetRunway(-1,RUNWAYMARK_FIRST,_T("All"));
	pItemData->vRunwayMark.push_back(defaultMark);
	m_pTreeCtrl->SetItemData(hItemRunway,(DWORD_PTR)pItemData);
	InitDefaltRunway(hItemRunway);
	m_pTreeCtrl->Expand(hItemRunway,TVE_EXPAND);

	m_pTreeCtrl->Expand(m_hItemRunwayRoot,TVE_EXPAND);

}

CString AirsideReControlView::CRunwayUtilizationTreePerformer::GetRunwayNames( std::vector<CAirsideReportRunwayMark> & vRunwayMark )
{
	CString strRunwayParamName = _T("All");
	if(vRunwayMark.size() > 0)
		strRunwayParamName = _T("");

	for (int nRunway = 0; nRunway < (int)vRunwayMark.size(); ++nRunway)
	{
		strRunwayParamName += vRunwayMark[nRunway].m_strMarkName;
		strRunwayParamName +=_T(", ");
	}
	strRunwayParamName.Trim();
	strRunwayParamName.TrimRight(_T(","));

	return strRunwayParamName;
}

CString AirsideReControlView::CRunwayUtilizationTreePerformer::GetOperationName( int nOperation )
{
	if(nOperation == 0)
		return _T("Operation: ALL");

	if(nOperation == 1)
		return _T("Operation: Landing");

	if(nOperation == 2)
		return _T("Operation: Take Off");

	return _T("Operation: ALL");
}

void AirsideReControlView::CRunwayUtilizationTreePerformer::InitDefaltRunway( HTREEITEM hRunwayItem )
{
	//operation
	HTREEITEM hItemOperation = m_pTreeCtrl->InsertItem(_T("Operation: All"),hRunwayItem);
	TreeItemData *pItemData = new TreeItemData;
	pItemData->itemType = Item_Operation;
	pItemData->nOperation = 0;

	m_pTreeCtrl->SetItemData(hItemOperation,(DWORD_PTR)pItemData);

	//airroute root
	HTREEITEM hItemAirRouteRoot = m_pTreeCtrl->InsertItem(_T("AirRoute"),hItemOperation);
	pItemData = new TreeItemData;
	pItemData->itemType = Item_AirRouteRoot;

	m_pTreeCtrl->SetItemData(hItemAirRouteRoot,(DWORD_PTR)pItemData);

	//airroute all
	HTREEITEM hItemAirRoute = m_pTreeCtrl->InsertItem(_T("All"),hItemAirRouteRoot);
	pItemData = new TreeItemData;
	pItemData->itemType = Item_AirRoute;
	pItemData->nObjID = -1;
	pItemData->strObjName = _T("All");
	m_pTreeCtrl->SetItemData(hItemAirRoute,(DWORD_PTR)pItemData);


	//flight type root
	HTREEITEM hItemFlightRoot = m_pTreeCtrl->InsertItem(_T("Flight Constraint"),hItemOperation);
	pItemData = new TreeItemData;
	pItemData->itemType = Item_FlightRoot;

	m_pTreeCtrl->SetItemData(hItemFlightRoot,(DWORD_PTR)pItemData);

	//flight type
	HTREEITEM hItemFlight = m_pTreeCtrl->InsertItem(_T("Default"),hItemFlightRoot);
	pItemData = new TreeItemData;
	pItemData->itemType = Item_Flight;
	m_pTreeCtrl->SetItemData(hItemFlight,(DWORD_PTR)pItemData);

	m_pTreeCtrl->Expand(hItemOperation,TVE_EXPAND);
	m_pTreeCtrl->Expand(hItemAirRouteRoot,TVE_EXPAND);
	m_pTreeCtrl->Expand(hItemFlightRoot,TVE_EXPAND);

}

void AirsideReControlView::CRunwayUtilizationTreePerformer::OnTreeItemDoubleClick( HTREEITEM hTreeItem )
{

	if( hTreeItem == NULL)
		return;

	TreeItemData *pItemData = (TreeItemData *)m_pTreeCtrl->GetItemData(hTreeItem);
	if(pItemData == NULL)
		return;

	if(pItemData->itemType == Item_Operation)
	{
		std::vector<CString> vStr;
		vStr.clear();
		CString str = _T("All");
		vStr.push_back(str);

		str = _T("Landing");
		vStr.push_back(str);

		str = _T("Take off");
		vStr.push_back(str);

		m_pTreeCtrl->SetComboWidth(120);
		m_pTreeCtrl->SetComboString(hTreeItem,vStr);

		m_hOperationItemSelected = hTreeItem;
	}
}

LRESULT AirsideReControlView::CRunwayUtilizationTreePerformer::DefWindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	if (message == WM_INPLACE_COMBO2)
	{
		if( m_hOperationItemSelected == NULL)
			return 0;

		TreeItemData *pItemData = (TreeItemData *)m_pTreeCtrl->GetItemData(m_hOperationItemSelected);
		if(pItemData == NULL)
			return 0;

		int nSel = m_pTreeCtrl->m_comboBox.GetCurSel();
		if (nSel >= 0 && nSel != pItemData->nOperation)
		{
			pItemData->nOperation = nSel;
			m_pTreeCtrl->SetItemText(m_hOperationItemSelected,GetOperationName(pItemData->nOperation));
		}
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////
//AirsideReControlView::CRunwayDelayParaTreePerformer
AirsideReControlView::CRunwayDelayParaTreePerformer::CRunwayDelayParaTreePerformer(int nProjID,  CARCTreeCtrlEx *pTreeCtrl, CParameters *pParam )
:CTreePerformer(nProjID,pTreeCtrl,pParam)
{
	m_hItemRunwayRoot = NULL;
	m_hOperationItemSelected = NULL;
}

AirsideReControlView::CRunwayDelayParaTreePerformer::~CRunwayDelayParaTreePerformer()
{

}

void AirsideReControlView::CRunwayDelayParaTreePerformer::InitTree()
{
	ASSERT(m_pTreeCtrl != NULL);
	ASSERT(m_pParam != NULL);

	if(m_pParam == NULL || m_pTreeCtrl == NULL)
		return;

	AirsideFlightRunwayDelayReportPara *pParam = (AirsideFlightRunwayDelayReportPara *)m_pParam;
	if(pParam == NULL)
		return;

	m_pTreeCtrl->DeleteAllItems();

	m_hItemRunwayRoot = m_pTreeCtrl->InsertItem(_T("Runway"));
	TreeItemData *pItemDataRoot = new TreeItemData;
	pItemDataRoot->itemType = Item_RunwayRoot;
	m_pTreeCtrl->SetItemData(m_hItemRunwayRoot,(DWORD_PTR)pItemDataRoot);


	if(pParam->m_vReportMarkParam.size() == 0)//default
	{
		InitDefaltTree();
	}
	else//init data from parameter
	{
		int nRunwayParamCount =(int)pParam->m_vReportMarkParam.size();
		for (int nRunwayParam = 0; nRunwayParam < nRunwayParamCount; ++ nRunwayParam)
		{
			CAirsideRunwayUtilizationReportParam::CRunwayMarkParam& runwayParam = pParam->m_vReportMarkParam[nRunwayParam];

			//runway, all
			HTREEITEM hItemRunway = m_pTreeCtrl->InsertItem(runwayParam.GetRunwayNames(),m_hItemRunwayRoot);
			TreeItemData *pItemData = new TreeItemData;
			pItemData->itemType = Item_Runway;
			pItemData->vRunwayMark = runwayParam.m_vRunwayMark;
			m_pTreeCtrl->SetItemData(hItemRunway,(DWORD_PTR)pItemData);

			int nOperationCount = runwayParam.m_vOperationParam.size();

			for (int nOperation = 0; nOperation < nOperationCount; ++ nOperation)
			{
				//operation
				HTREEITEM hItemOperation = m_pTreeCtrl->InsertItem(_T("Operation : ") + runwayParam.m_vOperationParam[nOperation].GetOperationName(),hItemRunway);
				pItemData = new TreeItemData;
				pItemData->itemType = Item_Operation;
				pItemData->nOperation = (int)runwayParam.m_vOperationParam[nOperation].m_enumOperation;

				m_pTreeCtrl->SetItemData(hItemOperation,(DWORD_PTR)pItemData);

				//flight type root
				HTREEITEM hItemFlightRoot = m_pTreeCtrl->InsertItem(_T("Flight Constraint"),hItemOperation);
				pItemData = new TreeItemData;
				pItemData->itemType = Item_FlightRoot;
				m_pTreeCtrl->SetItemData(hItemFlightRoot,(DWORD_PTR)pItemData);

				int nSize = (int)runwayParam.m_vOperationParam[nOperation].m_vFlightConstraint.size();
				for (int nCons = 0; nCons < nSize; ++ nCons)
				{
					CString strFlightType;
					runwayParam.m_vOperationParam[nOperation].m_vFlightConstraint[nCons].screenPrint(strFlightType.GetBuffer(1024));
					strFlightType.ReleaseBuffer();

					//flight type
					HTREEITEM hItemFlight = m_pTreeCtrl->InsertItem(strFlightType,hItemFlightRoot);
					pItemData = new TreeItemData;
					pItemData->itemType = Item_Flight;
					pItemData->fltCons = runwayParam.m_vOperationParam[nOperation].m_vFlightConstraint[nCons];
					m_pTreeCtrl->SetItemData(hItemFlight,(DWORD_PTR)pItemData);
				}	
				m_pTreeCtrl->Expand(hItemFlightRoot,TVE_EXPAND);
				m_pTreeCtrl->Expand(hItemOperation, TVE_EXPAND);
			}
			m_pTreeCtrl->Expand(hItemRunway,TVE_EXPAND);
		}

	}
	m_pTreeCtrl->Expand(m_hItemRunwayRoot,TVE_EXPAND);
}

void AirsideReControlView::CRunwayDelayParaTreePerformer::LoadData()
{
	InitTree();
}

void AirsideReControlView::CRunwayDelayParaTreePerformer::SaveData()
{
	ASSERT(m_pTreeCtrl != NULL);
	ASSERT(m_pParam != NULL);

	if(m_pParam == NULL || m_pTreeCtrl == NULL)
		return;

	AirsideFlightRunwayDelayReportPara *pParam = (AirsideFlightRunwayDelayReportPara *)m_pParam;
	if(pParam == NULL)
		return;

	pParam->m_vReportMarkParam.clear();

	HTREEITEM hItemRunway = NULL;
	hItemRunway = m_pTreeCtrl->GetChildItem(m_hItemRunwayRoot);
	if(hItemRunway == NULL)
		return;

	while(hItemRunway != NULL)
	{

		CAirsideRunwayUtilizationReportParam::CRunwayMarkParam runwayParam;
		//runway item
		TreeItemData *pItemData = (TreeItemData *)m_pTreeCtrl->GetItemData(hItemRunway);
		if(pItemData == NULL)
			return;
		runwayParam.m_vRunwayMark = pItemData->vRunwayMark;

		//operation
		HTREEITEM hItemOperation = m_pTreeCtrl->GetChildItem(hItemRunway);

		while(hItemOperation != NULL)
		{
			CAirsideRunwayUtilizationReportParam::CRunwayOperationParam runwayOperation;
			pItemData = (TreeItemData *)m_pTreeCtrl->GetItemData(hItemOperation);
			runwayOperation.m_enumOperation = (CAirsideRunwayUtilizationReportParam::RunwayOperation)pItemData->nOperation;

			//flight constraint
			HTREEITEM hItemFltRoot = m_pTreeCtrl->GetChildItem(hItemOperation);

			if(hItemFltRoot != NULL)
			{
				HTREEITEM hItemFlight = m_pTreeCtrl->GetChildItem(hItemFltRoot);
				while(hItemFlight)
				{
					pItemData = (TreeItemData *)m_pTreeCtrl->GetItemData(hItemFlight);
					if(pItemData)
					{
						runwayOperation.m_vFlightConstraint.push_back(pItemData->fltCons);
					}

					hItemFlight = m_pTreeCtrl->GetNextSiblingItem(hItemFlight);
				}
			}

			//runway operation
			hItemOperation = m_pTreeCtrl->GetNextSiblingItem(hItemOperation);
			runwayParam.m_vOperationParam.push_back(runwayOperation);
		}

		//runway
		hItemRunway = m_pTreeCtrl->GetNextSiblingItem(hItemRunway);
		pParam->m_vReportMarkParam.push_back(runwayParam);
	}
}

void AirsideReControlView::CRunwayDelayParaTreePerformer::OnToolBarAdd( HTREEITEM hTreeItem )
{
	if(m_pTreeCtrl == NULL || hTreeItem == NULL)
		return;

	TreeItemData *pItemData = (TreeItemData *)m_pTreeCtrl->GetItemData(hTreeItem);
	if(pItemData == NULL)
		return;

	if(pItemData->itemType == Item_RunwayRoot)//new runway
	{
		CDlgSelectReportRunway dlg(m_nProjID,m_pTreeCtrl->GetParent());
		if(dlg.DoModal() == IDOK)
		{
			TreeItemData *pItemDataRunway = new TreeItemData;
			pItemDataRunway->itemType = Item_Runway;
			pItemDataRunway->vRunwayMark = dlg.m_vRunwayMarkSel;

			HTREEITEM hItemRunway = m_pTreeCtrl->InsertItem(GetRunwayNames(pItemDataRunway->vRunwayMark),m_hItemRunwayRoot);
			m_pTreeCtrl->SetItemData(hItemRunway,(DWORD_PTR)pItemDataRunway);

			InitDefaltRunway(hItemRunway);
			m_pTreeCtrl->Expand(m_hItemRunwayRoot,TVE_EXPAND);

		}
	}
	else if(pItemData->itemType == Item_Runway)//new operation
	{
		InitDefaltRunway(hTreeItem);//default operation
	}
	else if(pItemData->itemType == Item_FlightRoot)//new flight root
	{
		CFlightDialog dlg( m_pTreeCtrl->GetParent() );
		if( dlg.DoModal() == IDOK )
		{
			FlightConstraint newFltConstr = dlg.GetFlightSelection();

			CString strNewFlightType;
			newFltConstr.screenPrint(strNewFlightType.GetBuffer(1024));
			strNewFlightType.ReleaseBuffer();

			//flight type
			HTREEITEM hItemFlight = m_pTreeCtrl->InsertItem(strNewFlightType,hTreeItem);
			TreeItemData* pItemDataFlight = new TreeItemData;
			pItemDataFlight->itemType = Item_Flight;
			pItemDataFlight->fltCons = newFltConstr;
			m_pTreeCtrl->SetItemData(hItemFlight,(DWORD_PTR)pItemDataFlight);
		}
		m_pTreeCtrl->Expand(hTreeItem,TVE_EXPAND);


	}
	else if(pItemData->itemType == Item_Operation)
	{
		OnTreeItemDoubleClick(hTreeItem);
	}

}

void AirsideReControlView::CRunwayDelayParaTreePerformer::OnToolBarDel( HTREEITEM hTreeItem )
{
	if(hTreeItem == NULL)
		return;

	TreeItemData *pItemData = (TreeItemData *)m_pTreeCtrl->GetItemData(hTreeItem);
	if(pItemData == NULL)
		return;

	if(pItemData->itemType == Item_Runway)
	{
		HTREEITEM hParentItem = m_pTreeCtrl->GetParentItem(hTreeItem);
		m_pTreeCtrl->DeleteItem(hTreeItem);
		HTREEITEM hNextSibItem = m_pTreeCtrl->GetChildItem(hParentItem);

		if(hNextSibItem == NULL)
			InitDefaltTree();

		m_pTreeCtrl->Expand(m_hItemRunwayRoot,TVE_EXPAND);

	}
	else if(pItemData->itemType == Item_Operation)
	{
		HTREEITEM hParentItem = m_pTreeCtrl->GetParentItem(hTreeItem);
		m_pTreeCtrl->DeleteItem(hTreeItem);
		HTREEITEM hNextSibItem = m_pTreeCtrl->GetChildItem(hParentItem);


		if(hNextSibItem == NULL && hParentItem != NULL)
		{
			InitDefaltRunway(hParentItem);
		}
		m_pTreeCtrl->Expand(hParentItem,TVE_EXPAND);

	}
	else if(pItemData->itemType == Item_Flight)
	{	
		HTREEITEM hParentItem = m_pTreeCtrl->GetParentItem(hTreeItem);
		m_pTreeCtrl->DeleteItem(hTreeItem);
		HTREEITEM hNextSibItem = m_pTreeCtrl->GetChildItem(hParentItem);

		if(hNextSibItem == NULL && hParentItem != NULL)
		{
			//flight type
			HTREEITEM hItemFlight = m_pTreeCtrl->InsertItem(_T("Default"),hParentItem);
			pItemData = new TreeItemData;
			pItemData->itemType = Item_Flight;
			m_pTreeCtrl->SetItemData(hItemFlight,(DWORD_PTR)pItemData);

			m_pTreeCtrl->Expand(hParentItem,TVE_EXPAND);
		}
	}

}

void AirsideReControlView::CRunwayDelayParaTreePerformer::OnUpdateToolBarAdd( CCmdUI *pCmdUI )
{
	if(m_pTreeCtrl == NULL)
		return;

	HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem();
	if( hTreeItem == NULL)
		return;

	TreeItemData *pItemData = (TreeItemData *)m_pTreeCtrl->GetItemData(hTreeItem);
	if(pItemData == NULL)
		return;

	if(pItemData->itemType == Item_RunwayRoot)
	{
		pCmdUI->Enable(TRUE);
	}
	else if(pItemData->itemType == Item_FlightRoot)
	{
		pCmdUI->Enable(TRUE);
	}
	else if(pItemData->itemType == Item_Operation)
	{
		pCmdUI->Enable(TRUE);
	}
	else if(pItemData->itemType == Item_Runway)
	{
		pCmdUI->Enable(TRUE);
	}
	else
		pCmdUI->Enable(FALSE);

}

void AirsideReControlView::CRunwayDelayParaTreePerformer::OnUpdateToolBarDel( CCmdUI *pCmdUI )
{
	if(m_pTreeCtrl == NULL)
		return;

	HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem();
	if( hTreeItem == NULL)
		return;

	TreeItemData *pItemData = (TreeItemData *)m_pTreeCtrl->GetItemData(hTreeItem);
	if(pItemData == NULL)
		return;

	if(pItemData->itemType == Item_Runway)
	{
		pCmdUI->Enable(TRUE);
	}
	else if(pItemData->itemType == Item_Flight)
	{
		pCmdUI->Enable(TRUE);
	}
	else if(pItemData->itemType == Item_Operation)
	{
		pCmdUI->Enable(TRUE);
	}
	else
		pCmdUI->Enable(FALSE);
}

void AirsideReControlView::CRunwayDelayParaTreePerformer::InitDefaltTree()
{
	//runway, all
	HTREEITEM hItemRunway = m_pTreeCtrl->InsertItem(_T("All"),m_hItemRunwayRoot);
	TreeItemData *pItemData = new TreeItemData;
	pItemData->itemType = Item_Runway;
	CAirsideReportRunwayMark defaultMark;
	defaultMark.SetRunway(-1,RUNWAYMARK_FIRST,_T("All"));
	pItemData->vRunwayMark.push_back(defaultMark);
	m_pTreeCtrl->SetItemData(hItemRunway,(DWORD_PTR)pItemData);
	InitDefaltRunway(hItemRunway);
	m_pTreeCtrl->Expand(hItemRunway,TVE_EXPAND);

	m_pTreeCtrl->Expand(m_hItemRunwayRoot,TVE_EXPAND);

}

CString AirsideReControlView::CRunwayDelayParaTreePerformer::GetRunwayNames( std::vector<CAirsideReportRunwayMark> & vRunwayMark )
{
	CString strRunwayParamName = _T("All");
	if(vRunwayMark.size() > 0)
		strRunwayParamName = _T("");

	for (int nRunway = 0; nRunway < (int)vRunwayMark.size(); ++nRunway)
	{
		strRunwayParamName += vRunwayMark[nRunway].m_strMarkName;
		strRunwayParamName +=_T(", ");
	}
	strRunwayParamName.Trim();
	strRunwayParamName.TrimRight(_T(","));

	return strRunwayParamName;
}

CString AirsideReControlView::CRunwayDelayParaTreePerformer::GetOperationName( int nOperation )
{
	if(nOperation == 0)
		return _T("Operation: ALL");

	if(nOperation == 1)
		return _T("Operation: Landing");

	if(nOperation == 2)
		return _T("Operation: Take Off");

	return _T("Operation: ALL");
}

void AirsideReControlView::CRunwayDelayParaTreePerformer::InitDefaltRunway( HTREEITEM hRunwayItem )
{
	//operation
	HTREEITEM hItemOperation = m_pTreeCtrl->InsertItem(_T("Operation: All"),hRunwayItem);
	TreeItemData *pItemData = new TreeItemData;
	pItemData->itemType = Item_Operation;
	pItemData->nOperation = 0;

	m_pTreeCtrl->SetItemData(hItemOperation,(DWORD_PTR)pItemData);

	//flight type root
	HTREEITEM hItemFlightRoot = m_pTreeCtrl->InsertItem(_T("Flight Constraint"),hItemOperation);
	pItemData = new TreeItemData;
	pItemData->itemType = Item_FlightRoot;

	m_pTreeCtrl->SetItemData(hItemFlightRoot,(DWORD_PTR)pItemData);

	//flight type
	HTREEITEM hItemFlight = m_pTreeCtrl->InsertItem(_T("DEFAULT"),hItemFlightRoot);
	pItemData = new TreeItemData;
	pItemData->itemType = Item_Flight;
	m_pTreeCtrl->SetItemData(hItemFlight,(DWORD_PTR)pItemData);

	m_pTreeCtrl->Expand(hItemOperation,TVE_EXPAND);
	m_pTreeCtrl->Expand(hItemFlightRoot,TVE_EXPAND);

}

void AirsideReControlView::CRunwayDelayParaTreePerformer::OnTreeItemDoubleClick( HTREEITEM hTreeItem )
{

	if( hTreeItem == NULL)
		return;

	TreeItemData *pItemData = (TreeItemData *)m_pTreeCtrl->GetItemData(hTreeItem);
	if(pItemData == NULL)
		return;

	if(pItemData->itemType == Item_Operation)
	{
		std::vector<CString> vStr;
		vStr.clear();
		CString str = _T("All");
		vStr.push_back(str);

		str = _T("Landing");
		vStr.push_back(str);

		str = _T("Take off");
		vStr.push_back(str);

		m_pTreeCtrl->SetComboWidth(120);
		m_pTreeCtrl->SetComboString(hTreeItem,vStr);

		m_hOperationItemSelected = hTreeItem;
	}
}

LRESULT AirsideReControlView::CRunwayDelayParaTreePerformer::DefWindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	if (message == WM_INPLACE_COMBO2)
	{
		if( m_hOperationItemSelected == NULL)
			return 0;

		TreeItemData *pItemData = (TreeItemData *)m_pTreeCtrl->GetItemData(m_hOperationItemSelected);
		if(pItemData == NULL)
			return 0;

		int nSel = m_pTreeCtrl->m_comboBox.GetCurSel();
		if (nSel >= 0 && nSel != pItemData->nOperation)
		{
			pItemData->nOperation = nSel;
			m_pTreeCtrl->SetItemText(m_hOperationItemSelected,GetOperationName(pItemData->nOperation));
		}
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
AirsideReControlView::CRunwayCrossingsTreePerformer::CRunwayCrossingsTreePerformer(int nProjID, CARCTreeCtrlEx *pTreeCtrl, CParameters *pParam)
:CTreePerformer(nProjID,pTreeCtrl,pParam)
{

}

AirsideReControlView::CRunwayCrossingsTreePerformer::~CRunwayCrossingsTreePerformer()
{

}

void AirsideReControlView::CRunwayCrossingsTreePerformer::LoadData()
{
	InitTree();
}

void AirsideReControlView::CRunwayCrossingsTreePerformer::InitTree()
{
	ASSERT(m_pTreeCtrl != NULL);
	ASSERT(m_pParam != NULL);

	if(m_pParam == NULL || m_pTreeCtrl == NULL)
		return;

	AirsideRunwayCrossingsParameter *pRunwayParam = (AirsideRunwayCrossingsParameter *)m_pParam;
	if(pRunwayParam == NULL)
		return;

	m_pTreeCtrl->DeleteAllItems();

	if( pRunwayParam->getCount() == 0)//default
	{
		InitDefaltTree();
	}
	else
	{
		HTREEITEM hItemRunwayRoot = m_pTreeCtrl->InsertItem(_T("Runway"));
		RunwayCrossingsTreeItemData *pItemDataRoot = new RunwayCrossingsTreeItemData;
		pItemDataRoot->itemType = Item_RunwayRoot;
		m_pTreeCtrl->SetItemData(hItemRunwayRoot,(DWORD_PTR)pItemDataRoot);
		for (int i = 0; i < pRunwayParam->getCount(); i++)
		{
			AirsideRunwayCrossingsParameter::RunwayCrossingsItem& runwayItem =  pRunwayParam->getItem(i);
			RunwayCrossingsTreeItemData* pItemData = new RunwayCrossingsTreeItemData;
			pItemData->itemType = Item_Runway;
			pItemData->sObjName = runwayItem.getRunway();
			HTREEITEM hRunwayItem = m_pTreeCtrl->InsertItem(pItemData->sObjName.GetIDString(),hItemRunwayRoot);
			m_pTreeCtrl->SetItemData(hRunwayItem,(DWORD_PTR)pItemData);
			HTREEITEM hTaxiRootItem = m_pTreeCtrl->InsertItem(_T("Taxiway"),hRunwayItem);
			pItemData = new RunwayCrossingsTreeItemData;
			pItemData->itemType = Item_TaxiwayRoot;
			m_pTreeCtrl->SetItemData(hTaxiRootItem,(DWORD_PTR)pItemData);
			for (int j = 0; j < runwayItem.getCount(); j++)
			{
				ALTObjectID& mapItem = runwayItem.getItem(j);
				pItemData = new RunwayCrossingsTreeItemData;
				pItemData->itemType = Item_Taxiway;
				pItemData->sObjName = mapItem;
				HTREEITEM hTaxiItem = m_pTreeCtrl->InsertItem(mapItem.GetIDString(),hTaxiRootItem);
				m_pTreeCtrl->SetItemData(hTaxiItem,(DWORD_PTR)pItemData);
			}
			m_pTreeCtrl->Expand(hTaxiRootItem,TVE_EXPAND);
			m_pTreeCtrl->Expand(hRunwayItem,TVE_EXPAND);
		}
		m_pTreeCtrl->Expand(hItemRunwayRoot,TVE_EXPAND);
	}
}


void AirsideReControlView::CRunwayCrossingsTreePerformer::InitDefaltTree()
{
	HTREEITEM hItemRunwayRoot = m_pTreeCtrl->InsertItem(_T("Runway"));
	RunwayCrossingsTreeItemData *pItemDataRoot = new RunwayCrossingsTreeItemData;
	pItemDataRoot->itemType = Item_RunwayRoot;
	m_pTreeCtrl->SetItemData(hItemRunwayRoot,(DWORD_PTR)pItemDataRoot);

	RunwayCrossingsTreeItemData* pItemData = new RunwayCrossingsTreeItemData;
	pItemData->itemType = Item_Runway;
	pItemData->sObjName = ALTObjectID(CString(_T("ALL")).GetBuffer());
	HTREEITEM hRunwayItem = m_pTreeCtrl->InsertItem(pItemData->sObjName.GetIDString(),hItemRunwayRoot);
	m_pTreeCtrl->SetItemData(hRunwayItem,(DWORD_PTR)pItemData);
	HTREEITEM hTaxiRootItem = m_pTreeCtrl->InsertItem(_T("Taxiway"),hRunwayItem);
	pItemData = new RunwayCrossingsTreeItemData;
	pItemData->itemType = Item_TaxiwayRoot;
	m_pTreeCtrl->SetItemData(hTaxiRootItem,(DWORD_PTR)pItemData);
	
	pItemData = new RunwayCrossingsTreeItemData;
	pItemData->itemType = Item_Taxiway;
	pItemData->sObjName =ALTObjectID(CString(_T("ALL")).GetBuffer());
	HTREEITEM hTaxiItem = m_pTreeCtrl->InsertItem(pItemData->sObjName.GetIDString(),hTaxiRootItem);
	m_pTreeCtrl->SetItemData(hTaxiItem,(DWORD_PTR)pItemData);
	m_pTreeCtrl->Expand(hTaxiRootItem,TVE_EXPAND);
	m_pTreeCtrl->Expand(hRunwayItem,TVE_EXPAND);
	m_pTreeCtrl->Expand(hItemRunwayRoot,TVE_EXPAND);

}

void AirsideReControlView::CRunwayCrossingsTreePerformer::OnToolBarAdd(HTREEITEM hTreeItem)
{
	if(m_pTreeCtrl == NULL)
		return;

	if(hTreeItem == NULL)
		return;

	RunwayCrossingsTreeItemData *pItemData = (RunwayCrossingsTreeItemData *)m_pTreeCtrl->GetItemData(hTreeItem);
	if(pItemData == NULL)
		return;

	if(pItemData->itemType == Item_RunwayRoot)
	{
		CDlgSelectALTObject dlg(m_nProjID,ALT_RUNWAY,m_pTreeCtrl->GetParent());
		if(dlg.DoModal() == IDOK)
		{
			RunwayCrossingsTreeItemData *pItemDataRunway = new RunwayCrossingsTreeItemData;
			pItemDataRunway->itemType = Item_Runway;
			pItemDataRunway->sObjName = dlg.GetALTObject();

			HTREEITEM hItemRunway = m_pTreeCtrl->InsertItem(pItemDataRunway->sObjName.GetIDString(),hTreeItem);
			m_pTreeCtrl->SetItemData(hItemRunway,(DWORD_PTR)pItemDataRunway);

			HTREEITEM hTaxiRootItem = m_pTreeCtrl->InsertItem(_T("Taxiway"),hItemRunway);
			pItemData = new RunwayCrossingsTreeItemData;
			pItemData->itemType = Item_TaxiwayRoot;
			m_pTreeCtrl->SetItemData(hTaxiRootItem,(DWORD_PTR)pItemData);

			pItemData = new RunwayCrossingsTreeItemData;
			pItemData->itemType = Item_Taxiway;
			pItemData->sObjName =ALTObjectID(CString(_T("ALL")).GetBuffer());
			HTREEITEM hTaxiItem = m_pTreeCtrl->InsertItem(pItemData->sObjName.GetIDString(),hTaxiRootItem);
			m_pTreeCtrl->SetItemData(hTaxiItem,(DWORD_PTR)pItemData);
			m_pTreeCtrl->Expand(hTreeItem,TVE_EXPAND);

		}
	}
	else if(pItemData->itemType == Item_TaxiwayRoot)
	{
		CDlgSelectALTObject dlg(m_nProjID,ALT_TAXIWAY,m_pTreeCtrl->GetParent());
		if(dlg.DoModal() == IDOK)
		{
			RunwayCrossingsTreeItemData *pItemDataRunway = new RunwayCrossingsTreeItemData;
			pItemDataRunway->itemType = Item_Taxiway;
			pItemDataRunway->sObjName = dlg.GetALTObject();

			HTREEITEM hItemRunway = m_pTreeCtrl->InsertItem(dlg.GetALTObject().GetIDString(),hTreeItem);
			m_pTreeCtrl->SetItemData(hItemRunway,(DWORD_PTR)pItemDataRunway);
		}
	}
}

void AirsideReControlView::CRunwayCrossingsTreePerformer::OnToolBarDel(HTREEITEM hTreeItem)
{
	if(hTreeItem == NULL)
		return;

	RunwayCrossingsTreeItemData *pItemData = (RunwayCrossingsTreeItemData *)m_pTreeCtrl->GetItemData(hTreeItem);
	if(pItemData == NULL)
		return;

	if(pItemData->itemType == Item_Runway)
	{
		HTREEITEM hParentItem = m_pTreeCtrl->GetParentItem(hTreeItem);
		m_pTreeCtrl->DeleteItem(hTreeItem);
		HTREEITEM hNextSibItem = m_pTreeCtrl->GetChildItem(hParentItem);

		if(hNextSibItem == NULL)
		{
			RunwayCrossingsTreeItemData* pItemData = new RunwayCrossingsTreeItemData;
			pItemData->itemType = Item_Runway;
			pItemData->sObjName = ALTObjectID(CString(_T("ALL")).GetBuffer());
			HTREEITEM hRunwayItem = m_pTreeCtrl->InsertItem(pItemData->sObjName.GetIDString(),hParentItem);
			m_pTreeCtrl->SetItemData(hRunwayItem,(DWORD_PTR)pItemData);
			HTREEITEM hTaxiRootItem = m_pTreeCtrl->InsertItem(_T("Taxiway"),hRunwayItem);
			pItemData = new RunwayCrossingsTreeItemData;
			pItemData->itemType = Item_TaxiwayRoot;
			m_pTreeCtrl->SetItemData(hTaxiRootItem,(DWORD_PTR)pItemData);

			pItemData = new RunwayCrossingsTreeItemData;
			pItemData->itemType = Item_Taxiway;
			pItemData->sObjName =ALTObjectID(CString(_T("ALL")).GetBuffer());
			HTREEITEM hTaxiItem = m_pTreeCtrl->InsertItem(pItemData->sObjName.GetIDString(),hTaxiRootItem);
			m_pTreeCtrl->SetItemData(hTaxiItem,(DWORD_PTR)pItemData);
		}

		m_pTreeCtrl->Expand(hParentItem,TVE_EXPAND);

	}
	else if(pItemData->itemType == Item_Taxiway)
	{
		HTREEITEM hParentItem = m_pTreeCtrl->GetParentItem(hTreeItem);
		m_pTreeCtrl->DeleteItem(hTreeItem);
		HTREEITEM hNextSibItem = m_pTreeCtrl->GetChildItem(hParentItem);


		if(hNextSibItem == NULL && hParentItem != NULL)
		{
			pItemData = new RunwayCrossingsTreeItemData;
			pItemData->itemType = Item_Taxiway;
			pItemData->sObjName =ALTObjectID(CString(_T("ALL")).GetBuffer());
			HTREEITEM hTaxiItem = m_pTreeCtrl->InsertItem(pItemData->sObjName.GetIDString(),hParentItem);
			m_pTreeCtrl->SetItemData(hTaxiItem,(DWORD_PTR)pItemData);
		}
		m_pTreeCtrl->Expand(hParentItem,TVE_EXPAND);

	}
}

void AirsideReControlView::CRunwayCrossingsTreePerformer::OnUpdateToolBarAdd(CCmdUI *pCmdUI)
{
	if(m_pTreeCtrl == NULL)
		return;

	HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem();
	if( hTreeItem == NULL)
		return;

	RunwayCrossingsTreeItemData *pItemData = (RunwayCrossingsTreeItemData *)m_pTreeCtrl->GetItemData(hTreeItem);
	if(pItemData == NULL)
		return;

	if(pItemData->itemType == Item_RunwayRoot)
	{
		pCmdUI->Enable(TRUE);
	}
	else if(pItemData->itemType == Item_TaxiwayRoot)
	{
		pCmdUI->Enable(TRUE);
	}
	else
		pCmdUI->Enable(FALSE);
}

void AirsideReControlView::CRunwayCrossingsTreePerformer::OnUpdateToolBarDel(CCmdUI *pCmdUI)
{
	if(m_pTreeCtrl == NULL)
		return;

	HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem();
	if( hTreeItem == NULL)
		return;

	RunwayCrossingsTreeItemData *pItemData = (RunwayCrossingsTreeItemData *)m_pTreeCtrl->GetItemData(hTreeItem);
	if(pItemData == NULL)
		return;

	if(pItemData->itemType == Item_Runway)
	{
		pCmdUI->Enable(TRUE);
	}
	else if(pItemData->itemType == Item_Taxiway)
	{
		pCmdUI->Enable(TRUE);
	}
	else
		pCmdUI->Enable(FALSE);
}

void AirsideReControlView::CRunwayCrossingsTreePerformer::SaveData()
{
	ASSERT(m_pTreeCtrl != NULL);
	ASSERT(m_pParam != NULL);

	if(m_pParam == NULL || m_pTreeCtrl == NULL)
		return;

	AirsideRunwayCrossingsParameter *pParam = (AirsideRunwayCrossingsParameter *)m_pParam;
	if(pParam == NULL)
		return;

	pParam->clear();

	HTREEITEM hItemRunway = NULL;
	hItemRunway = m_pTreeCtrl->GetChildItem(m_pTreeCtrl->GetRootItem());
	if(hItemRunway == NULL)
		return;

	while(hItemRunway != NULL)
	{

		AirsideRunwayCrossingsParameter::RunwayCrossingsItem runwayParam;
		//runway item
		RunwayCrossingsTreeItemData *pItemData = (RunwayCrossingsTreeItemData *)m_pTreeCtrl->GetItemData(hItemRunway);
		if(pItemData == NULL)
			continue;
		runwayParam.setRunway(pItemData->sObjName);
	
		HTREEITEM hTaxiRootItem = m_pTreeCtrl->GetChildItem(hItemRunway);
		if(hTaxiRootItem == NULL)
			continue;

		HTREEITEM hTaxiItem = m_pTreeCtrl->GetChildItem(hTaxiRootItem);
		if (hTaxiItem == NULL)
			continue;

		while(hTaxiItem != NULL)
		{
			pItemData = (RunwayCrossingsTreeItemData *)m_pTreeCtrl->GetItemData(hTaxiItem);
			if(pItemData)
			{
				runwayParam.addItem(pItemData->sObjName);
			}
			hTaxiItem = m_pTreeCtrl->GetNextSiblingItem(hTaxiItem);
		}

		hItemRunway = m_pTreeCtrl->GetNextSiblingItem(hItemRunway);
		pParam->addItem(runwayParam);
	}
}

//////////////////////////////////////////////////////////////////////////
//AirsideReControlView
AirsideReControlView::CAirsideIntersectionTreePerformer::CAirsideIntersectionTreePerformer( int nProjID, CARCTreeCtrlEx *pTreeCtrl, CParameters *pParam )
:CTreePerformer(nProjID,pTreeCtrl,pParam)
{
	m_hRootItem = NULL;
	m_hFromOrToNodeSelected = NULL;
	m_hAllTaxiwayItem = NULL;
}

AirsideReControlView::CAirsideIntersectionTreePerformer::~CAirsideIntersectionTreePerformer()
{

}

void AirsideReControlView::CAirsideIntersectionTreePerformer::InitTree()
{
	ASSERT(m_pParam != NULL);

	if(m_pParam == NULL || m_pTreeCtrl == NULL)
		return;

	CAirsideIntersectionReportParam *pParam = (CAirsideIntersectionReportParam *)m_pParam;
	if(pParam == NULL)
		return;

	m_pTreeCtrl->DeleteAllItems();

	InitDefaltTree();
	ASSERT(m_hRootItem != NULL);
	if(m_hRootItem == NULL)
		return;

	if (!pParam->GetUseAllTaxiway())
	{
		int nItemCount = pParam->getItemCount();
		if (nItemCount)
		{
			for (int nItem = 0; nItem < nItemCount; ++ nItem)
			{
				CTaxiwayItem *taxiwayItem = pParam->getItem(nItem);
				AddTaxiwayItem(*taxiwayItem);
			}
			return;
		}
	}
	InsertAllTaxiwayItem();
}

void AirsideReControlView::CAirsideIntersectionTreePerformer::LoadData()
{
	InitTree();
}

void AirsideReControlView::CAirsideIntersectionTreePerformer::SaveData()
{
	ASSERT(m_pParam != NULL);

	if(m_pParam == NULL || m_pTreeCtrl == NULL)
		return;

	CAirsideIntersectionReportParam *pParam = (CAirsideIntersectionReportParam *)m_pParam;
	if(pParam == NULL)
		return;

	//get data from tree
	if(m_hRootItem == NULL)
		return;

	pParam->ClearTaxiwayItems();
	//get taxiway items
	HTREEITEM hTaxiwayItem = m_pTreeCtrl->GetChildItem(m_hRootItem);
	while(hTaxiwayItem != NULL)
	{
		//get taxiway information
		CTaxiwayItem taxiwayItem;
		TreeItemData *pTaxiwayData = (TreeItemData *)m_pTreeCtrl->GetItemData(hTaxiwayItem);
		if(pTaxiwayData)
		{
			taxiwayItem.m_nTaxiwayID = pTaxiwayData->m_nTaxiwayID;
			taxiwayItem.m_strTaxiwayName = pTaxiwayData->strObjName;
		

			//get from node
			HTREEITEM hFromNode = m_pTreeCtrl->GetChildItem(hTaxiwayItem);
			if(hFromNode != NULL)
			{
				TreeItemData *pFromData = (TreeItemData *)m_pTreeCtrl->GetItemData(hFromNode);
				if(pFromData)
				{
					taxiwayItem.m_nodeFrom.m_nNodeID = pFromData->m_nNodeID;
					taxiwayItem.m_nodeFrom.m_strName = pFromData->strObjName;
				}
			}

			//get to node
			HTREEITEM hToNode = m_pTreeCtrl->GetNextSiblingItem(hFromNode);
			if(hToNode != NULL)
			{
				TreeItemData *pToData = (TreeItemData *)m_pTreeCtrl->GetItemData(hToNode);
				if(pToData)
				{
					taxiwayItem.m_nodeTo.m_nNodeID = pToData->m_nNodeID;
					taxiwayItem.m_nodeTo.m_strName = pToData->strObjName;
				}
			}

			InitTaxiwayItemNodes(taxiwayItem);
			pParam->AddItem(taxiwayItem);
		}

		//get next taxiway item
		hTaxiwayItem = m_pTreeCtrl->GetNextSiblingItem(hTaxiwayItem);
	}

	// selected all taxiways
	if (0 == pParam->getItemCount())
	{
		pParam->SetUseAllTaxiway(true);
		std::vector<int> vAirportIds;
		InputAirside::GetAirportList(m_nProjID, vAirportIds);
		for (std::vector<int>::iterator iterAirportID = vAirportIds.begin();
			iterAirportID != vAirportIds.end();++iterAirportID)
		{
			ALTObjectList vTaxiways;
			ALTAirport::GetTaxiwayList(*iterAirportID,vTaxiways);

			for (ALTObjectList::iterator iteTaxiway = vTaxiways.begin();
				iteTaxiway != vTaxiways.end();++iteTaxiway)
			{
				CTaxiwayItem taxiwayItem;
				taxiwayItem.m_nTaxiwayID = (*iteTaxiway)->getID();
				taxiwayItem.m_strTaxiwayName = (*iteTaxiway)->GetObjectName().GetIDString();
				taxiwayItem.m_nodeFrom.m_nNodeID = -1;
				taxiwayItem.m_nodeFrom.m_strName = _T("");
				taxiwayItem.m_nodeTo.m_nNodeID = -1;
				taxiwayItem.m_nodeTo.m_strName = _T("");
				InitTaxiwayItemNodes(taxiwayItem);
				pParam->AddItem(taxiwayItem);
			}
		}
	}

}

void AirsideReControlView::CAirsideIntersectionTreePerformer::OnToolBarAdd( HTREEITEM hTreeItem )
{
	//check item
	if(m_pTreeCtrl == NULL)
		return;

	if(hTreeItem == NULL)
		return;

	TreeItemData *pItemData = (TreeItemData *)m_pTreeCtrl->GetItemData(hTreeItem);
	if(pItemData == NULL)
		return;

	if(pItemData->itemType == Item_Root)
	{
		CDlgTaxiwaySelect dlg(m_nProjID);
		if(dlg.DoModal() != IDOK)
			return;
		int nTaxiwayID = dlg.GetSelTaxiwayID();
		CString strTaxiwayName = dlg.GetSelTaxiwayName();

		//Insert the item to tree
		CTaxiwayItem taxiwayItem;
		taxiwayItem.m_nTaxiwayID = nTaxiwayID;
		taxiwayItem.m_strTaxiwayName = strTaxiwayName;

		AddTaxiwayItem(taxiwayItem);

		if (m_hAllTaxiwayItem)
		{
			((CAirsideIntersectionReportParam*)m_pParam)->SetUseAllTaxiway(false);
			m_pTreeCtrl->DeleteItem(m_hAllTaxiwayItem);
			m_hAllTaxiwayItem = NULL;
		}
	}
// 	else if(pItemData->itemType == Item_NodeFrom)
// 	{
// 		OnEditIntersectionFrom(hTreeItem,pItemData->m_nTaxiwayID);
// 	}
// 	else if(pItemData->itemType == Item_NodeTo)
// 	{
// 		OnEditIntersectionTo(hTreeItem,pItemData->m_nTaxiwayID);
// 	}

}

void AirsideReControlView::CAirsideIntersectionTreePerformer::OnToolBarDel( HTREEITEM hTreeItem )
{
	//check item
	if(m_pTreeCtrl == NULL)
		return;

	if(hTreeItem == NULL)
		return;

	TreeItemData *pItemData = (TreeItemData *)m_pTreeCtrl->GetItemData(hTreeItem);
	if(pItemData == NULL)
		return;

	if(pItemData->itemType == Item_TaxiwayRoot)
	{
		m_pTreeCtrl->DeleteItem(hTreeItem);
	}

	// if all deleted, add all taxiway
	if (NULL == m_pTreeCtrl->GetChildItem(m_hRootItem))
		InsertAllTaxiwayItem();
}

void AirsideReControlView::CAirsideIntersectionTreePerformer::OnUpdateToolBarAdd( CCmdUI *pCmdUI )
{
	if(m_pTreeCtrl == NULL)
		return;

	HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem();
	if( hTreeItem == NULL)
		return;

	TreeItemData *pItemData = (TreeItemData *)m_pTreeCtrl->GetItemData(hTreeItem);
	pCmdUI->Enable(pItemData && pItemData->itemType == Item_Root ? TRUE : FALSE);
}

void AirsideReControlView::CAirsideIntersectionTreePerformer::OnUpdateToolBarDel( CCmdUI *pCmdUI )
{
	if(m_pTreeCtrl == NULL)
		return;

	HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem();
	if( hTreeItem == NULL)
		return;

	TreeItemData *pItemData = (TreeItemData *)m_pTreeCtrl->GetItemData(hTreeItem);
	pCmdUI->Enable(pItemData && pItemData->itemType == Item_TaxiwayRoot);
}

void AirsideReControlView::CAirsideIntersectionTreePerformer::InitDefaltTree()
{

	TreeItemData *pItemData = new TreeItemData;
	pItemData->itemType = Item_Root;

	//if taxiway, taxiway name
	//if node, node name
	pItemData->strObjName = _T("Intersections");
	m_hRootItem = InsertItem(TVI_ROOT,_T("Intersections"),pItemData);

	ASSERT(m_hRootItem != NULL);
}

void AirsideReControlView::CAirsideIntersectionTreePerformer::AddTaxiwayItem( const CTaxiwayItem& taxiwayItem )
{
	CString strNodeName;
	strNodeName = _T("On Taxiway: ") + taxiwayItem.m_strTaxiwayName;

	TreeItemData *pItemDataRoot = new TreeItemData;
	pItemDataRoot->itemType = Item_TaxiwayRoot;
	pItemDataRoot->m_nTaxiwayID = taxiwayItem.m_nTaxiwayID;
	pItemDataRoot->strObjName = taxiwayItem.m_strTaxiwayName;

	HTREEITEM hTaxiwayRoot = InsertItem(m_hRootItem,strNodeName,pItemDataRoot);

	ASSERT(hTaxiwayRoot != NULL);
	if(hTaxiwayRoot == NULL)
		return;

	//add node from
	{
		CString strFromNodeName;
		strFromNodeName = _T("From Intersection: ") + taxiwayItem.m_nodeFrom.m_strName;

		TreeItemData *pItemDataRoot = new TreeItemData;
		pItemDataRoot->itemType = Item_NodeFrom;
		pItemDataRoot->m_nNodeID = taxiwayItem.m_nodeFrom.m_nNodeID;
		pItemDataRoot->m_nTaxiwayID = taxiwayItem.m_nTaxiwayID;
		pItemDataRoot->strObjName = taxiwayItem.m_nodeFrom.m_strName;

		InsertItem(hTaxiwayRoot,strFromNodeName,pItemDataRoot);
	}
	//add node to
	{
		CString strToNodeName;
		strToNodeName = _T("To Intersection: ") + taxiwayItem.m_nodeTo.m_strName;

		TreeItemData *pItemDataRoot = new TreeItemData;
		pItemDataRoot->itemType = Item_NodeTo;
		pItemDataRoot->m_nNodeID = taxiwayItem.m_nodeTo.m_nNodeID;
		pItemDataRoot->m_nTaxiwayID = taxiwayItem.m_nTaxiwayID;
		pItemDataRoot->strObjName = taxiwayItem.m_nodeTo.m_strName;

		InsertItem(hTaxiwayRoot,strToNodeName,pItemDataRoot);
	}
	m_pTreeCtrl->Expand(hTaxiwayRoot,TVE_EXPAND);
	m_pTreeCtrl->Expand(m_hRootItem,TVE_EXPAND);

}

std::vector<CAirsideIntersectionTreePerformer::NodeInfo> AirsideReControlView::CAirsideIntersectionTreePerformer::GetTaxiwayNodes(int nTaxiwayID)
{

	std::map<int, std::vector<NodeInfo> >::iterator iterFind = m_mapTaxiwayNodes.find(nTaxiwayID);
	if( iterFind != m_mapTaxiwayNodes.end())
		return (*iterFind).second;

	std::vector<CAirsideIntersectionTreePerformer::NodeInfo> vNodeInfo;
	//find the taxiway nodes
	Taxiway taxiway;
	std::vector<IntersectionNode> vrInterNode;
	taxiway.ReadObject(nTaxiwayID);
	vrInterNode.clear();
	vrInterNode = taxiway.GetIntersectNodes();
	for (std::vector<IntersectionNode>::iterator itrNode = vrInterNode.begin();itrNode != vrInterNode.end();++itrNode)
	{
		CAirsideIntersectionTreePerformer::NodeInfo nodeInfo;
		nodeInfo.strNodeName = (*itrNode).GetName();
		nodeInfo.nNodeID = (*itrNode).GetID();
		nodeInfo.dDistFromStart = (*itrNode).GetDistance(nTaxiwayID);

		vNodeInfo.push_back(nodeInfo);
	}
	std::sort(vNodeInfo.begin(),vNodeInfo.end());
	m_mapTaxiwayNodes[nTaxiwayID] = vNodeInfo;



	return vNodeInfo;
}

void AirsideReControlView::CAirsideIntersectionTreePerformer::InitTaxiwayItemNodes(CTaxiwayItem &taxiwayItem )
{
	//get all the intersection node will be in report
	std::vector<CAirsideIntersectionTreePerformer::NodeInfo> vNodeInfo = GetTaxiwayNodes(taxiwayItem.m_nTaxiwayID);

	int nStartPos= -1;
	int nEndPos = -1;
	for (int nNode = 0; nNode < static_cast<int>(vNodeInfo.size()); ++nNode )
	{
		CAirsideIntersectionTreePerformer::NodeInfo& nodeinfo = vNodeInfo[nNode];
		if(nodeinfo.nNodeID == taxiwayItem.m_nodeFrom.m_nNodeID)
			nStartPos = nNode;
		if(nodeinfo.nNodeID == taxiwayItem.m_nodeTo.m_nNodeID)
			nEndPos = nNode;
	}

	if(nStartPos == -1)
		nStartPos = 0;
	if(nEndPos == -1)
		nEndPos =static_cast<int>(vNodeInfo.size());

// 	if(nStartPos > nEndPos)
// 		std::swap(nStartPos, nEndPos);

	for (int nPos = nStartPos; nPos < nEndPos; ++nPos)
	{	
		CAirsideIntersectionTreePerformer::NodeInfo& nodeinfo = vNodeInfo[nPos];

		CTaxiwayItem::CTaxiwayNodeItem item;
		item.m_nNodeID = nodeinfo.nNodeID;
		item.m_strName = nodeinfo.strNodeName;
		taxiwayItem.m_vNodeItems.push_back(item);
	}
}
LRESULT AirsideReControlView::CAirsideIntersectionTreePerformer::DefWindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	if (message == WM_INPLACE_COMBO2)
	{
		if( m_hFromOrToNodeSelected == NULL)
			return 0;

		TreeItemData *pItemData = (TreeItemData *)m_pTreeCtrl->GetItemData(m_hFromOrToNodeSelected);
		if(pItemData == NULL)
			return 0;
		int nSel = m_pTreeCtrl->m_comboBox.GetCurSel();

		if(nSel >=0 && pItemData->itemType == Item_NodeFrom)
		{
			if(nSel == 0)//start
			{
				m_pTreeCtrl->SetItemText(m_hFromOrToNodeSelected,_T("From Intersection: START"));
				pItemData->m_nNodeID = -1;
				pItemData->strObjName = _T("START");
			}
			else
			{
				std::vector<CAirsideIntersectionTreePerformer::NodeInfo> vNodeInfo = GetTaxiwayNodes(pItemData->m_nTaxiwayID);
				if(nSel-1 < static_cast<int>(vNodeInfo.size())) 
				{
					m_pTreeCtrl->SetItemText(m_hFromOrToNodeSelected,_T("From Intersection: ") + vNodeInfo[nSel-1].strNodeName);
					pItemData->m_nNodeID = vNodeInfo[nSel-1].nNodeID;
					pItemData->strObjName = vNodeInfo[nSel-1].strNodeName;
				}
			}

		}
		else if(nSel >=0 && pItemData->itemType == Item_NodeTo)
		{	
			std::vector<CAirsideIntersectionTreePerformer::NodeInfo> vNodeInfo = GetTaxiwayNodes(pItemData->m_nTaxiwayID);
			if(nSel < static_cast<int>(vNodeInfo.size()))//start
			{
				m_pTreeCtrl->SetItemText(m_hFromOrToNodeSelected,_T("From Intersection: ") + vNodeInfo[nSel].strNodeName);
				pItemData->m_nNodeID = vNodeInfo[nSel].nNodeID;
				pItemData->strObjName = vNodeInfo[nSel].strNodeName;
			}
			else
			{
				m_pTreeCtrl->SetItemText(m_hFromOrToNodeSelected,_T("From Intersection: END") );
				pItemData->m_nNodeID = -1;
				pItemData->strObjName = _T("END");
			}
		}
	}

	return 0;
}

void AirsideReControlView::CAirsideIntersectionTreePerformer::OnTreeItemDoubleClick( HTREEITEM hTreeItem )
{
	//check item
	if(m_pTreeCtrl == NULL)
		return;

	if(hTreeItem == NULL)
		return;

	TreeItemData *pItemData = (TreeItemData *)m_pTreeCtrl->GetItemData(hTreeItem);
	if(pItemData == NULL)
		return;

	if(pItemData->itemType == Item_NodeFrom)
	{
		OnEditIntersectionFrom(hTreeItem,pItemData->m_nTaxiwayID);
	}
	else if(pItemData->itemType == Item_NodeTo)
	{
		OnEditIntersectionTo(hTreeItem,pItemData->m_nTaxiwayID);
	}
}

void AirsideReControlView::CAirsideIntersectionTreePerformer::OnEditIntersectionFrom(HTREEITEM hTreeItem, int nTaxiwayID)
{
	std::vector<CString> vStr;
	vStr.push_back(_T("START"));

	std::vector<CAirsideIntersectionTreePerformer::NodeInfo> vNodeInfo = GetTaxiwayNodes(nTaxiwayID);
	for(int nNode = 0; nNode < static_cast<int>(vNodeInfo.size()); ++ nNode)
	{
		CAirsideIntersectionTreePerformer::NodeInfo& nodeinfo = vNodeInfo[nNode];
		vStr.push_back(nodeinfo.strNodeName);
	}

	m_pTreeCtrl->SetComboWidth(120);
	m_pTreeCtrl->SetComboString(hTreeItem,vStr);

	m_hFromOrToNodeSelected = hTreeItem;
}

void AirsideReControlView::CAirsideIntersectionTreePerformer::OnEditIntersectionTo( HTREEITEM hTreeItem, int nTaxiwayID )
{
	std::vector<CString> vStr;
	std::vector<CAirsideIntersectionTreePerformer::NodeInfo> vNodeInfo = GetTaxiwayNodes(nTaxiwayID);
	for(int nNode = 0; nNode < static_cast<int>(vNodeInfo.size()); ++ nNode)
	{
		CAirsideIntersectionTreePerformer::NodeInfo& nodeinfo = vNodeInfo[nNode];
		vStr.push_back(nodeinfo.strNodeName);
	}
	vStr.push_back(_T("END"));
	m_pTreeCtrl->SetComboWidth(120);
	m_pTreeCtrl->SetComboString(hTreeItem,vStr);
	m_hFromOrToNodeSelected = hTreeItem;
}

void AirsideReControlView::CAirsideIntersectionTreePerformer::InsertAllTaxiwayItem()
{
	if (m_hAllTaxiwayItem)
	{
		m_pTreeCtrl->DeleteItem(m_hAllTaxiwayItem);
		m_hAllTaxiwayItem = NULL;
	}
	m_hAllTaxiwayItem = InsertItem(m_hRootItem, _T("All taxiways"), NULL);
	InsertItem(m_hAllTaxiwayItem,_T("From Intersection: START"), NULL);
	InsertItem(m_hAllTaxiwayItem,_T("To Intersection: END"), NULL);
	m_pTreeCtrl->Expand(m_hAllTaxiwayItem,TVE_EXPAND);
	m_pTreeCtrl->Expand(m_hRootItem,TVE_EXPAND);
}

//////////////////////taxiway utilization///////////////////////////////////////////////////////
AirsideReControlView::CAirsideTaxiwayUtilizationTreePerformer::CAirsideTaxiwayUtilizationTreePerformer( int nProjID, CARCTreeCtrlEx *pTreeCtrl, CParameters *pParam )
:CAirsideIntersectionTreePerformer(nProjID,pTreeCtrl,pParam)
{
	m_hRootItem = NULL;
	m_hFromOrToNodeSelected = NULL;
	m_hAllTaxiwayItem = NULL;
}

AirsideReControlView::CAirsideTaxiwayUtilizationTreePerformer::~CAirsideTaxiwayUtilizationTreePerformer()
{

}

void AirsideReControlView::CAirsideTaxiwayUtilizationTreePerformer::InitTaxiwaySpecifiedItemNodes(CTaxiwayItem &taxiwayItem)
{
	ASSERT(m_pParam != NULL);

	if(m_pParam == NULL)
		return;

	CAirsideTaxiwayUtilizationPara *pParam = (CAirsideTaxiwayUtilizationPara *)m_pParam;
	if(pParam == NULL)
		return;

	ALTObjectGroup altGroup;
	altGroup.ReadData(taxiwayItem.m_nTaxiwayID);
	std::vector< ALTObject> vObjct;
	altGroup.GetObjects(vObjct);

	if(vObjct.empty())
		return;

	for (int i = 0; i < static_cast<int>(vObjct.size()); i++)
	{
		ALTObject& obj = vObjct.at(i);
		CTaxiwayItem resultItem;
		resultItem.m_nTaxiwayID = obj.getID();
		resultItem.m_strTaxiwayName = obj.GetObjectName().GetIDString();
		resultItem.m_nodeFrom.m_nNodeID = -1;
		resultItem.m_nodeFrom.m_strName = _T("");
		resultItem.m_nodeTo.m_nNodeID = -1;
		resultItem.m_nodeTo.m_strName = _T("");

		std::vector<CAirsideIntersectionTreePerformer::NodeInfo> vNodeInfo = AirsideReControlView::CAirsideIntersectionTreePerformer::GetTaxiwayNodes(resultItem.m_nTaxiwayID);

		int nStartPos= -1;
		int nEndPos = -1;
		for (int nNode = 0; nNode < static_cast<int>(vNodeInfo.size()); ++nNode )
		{
			CAirsideIntersectionTreePerformer::NodeInfo& nodeinfo = vNodeInfo[nNode];
			if(nodeinfo.nNodeID == taxiwayItem.m_nodeFrom.m_nNodeID)
				nStartPos = nNode;
			if(nodeinfo.nNodeID == taxiwayItem.m_nodeTo.m_nNodeID)
				nEndPos = nNode;
		}

		if(nStartPos == -1)
			nStartPos = 0;
		if(nEndPos == -1)
			nEndPos =static_cast<int>(vNodeInfo.size())-1;

		if(nStartPos > nEndPos)
		{	
			std::swap(nStartPos,nEndPos);
		}

		for (int nPos = nStartPos; nPos <= nEndPos; ++nPos)
		{	
			CAirsideIntersectionTreePerformer::NodeInfo& nodeinfo = vNodeInfo[nPos];

			CTaxiwayItem::CTaxiwayNodeItem item;
			item.m_nNodeID = nodeinfo.nNodeID;
			item.m_strName = nodeinfo.strNodeName;
			resultItem.m_vNodeItems.push_back(item);
		}

		pParam->GetResultTaxiwayItemList().AddItem(resultItem);
	}
	
}

void AirsideReControlView::CAirsideTaxiwayUtilizationTreePerformer::SaveData()
{
	ASSERT(m_pParam != NULL);

	if(m_pParam == NULL || m_pTreeCtrl == NULL)
		return;

	CAirsideTaxiwayUtilizationPara *pParam = (CAirsideTaxiwayUtilizationPara *)m_pParam;
	if(pParam == NULL)
		return;

	pParam->ClearTaxiwayItems();
	pParam->GetResultTaxiwayItemList().ClearTaxiwayItems();
	//get taxiway items
	
	HTREEITEM hRootItem = m_pTreeCtrl->GetRootItem();
	while(hRootItem != NULL)
	{
		//get taxiway information
		HTREEITEM hTaxiwayItem = m_pTreeCtrl->GetChildItem(hRootItem);
		if (hTaxiwayItem == NULL)
			continue;
		
		CTaxiwayItem taxiwayItem;
		TreeItemData *pTaxiwayData = (TreeItemData *)m_pTreeCtrl->GetItemData(hTaxiwayItem);
		if(pTaxiwayData)
		{
			taxiwayItem.m_nTaxiwayID = pTaxiwayData->m_nTaxiwayID;
			taxiwayItem.m_strTaxiwayName = pTaxiwayData->strObjName;


			//get from node
			HTREEITEM hFromNode = m_pTreeCtrl->GetChildItem(hTaxiwayItem);
			if(hFromNode != NULL)
			{
				TreeItemData *pFromData = (TreeItemData *)m_pTreeCtrl->GetItemData(hFromNode);
				if(pFromData)
				{
					taxiwayItem.m_nodeFrom.m_nNodeID = pFromData->m_nNodeID;
					taxiwayItem.m_nodeFrom.m_strName = pFromData->strObjName;
				}
			}

			//get to node
			HTREEITEM hToNode = m_pTreeCtrl->GetNextSiblingItem(hFromNode);
			if(hToNode != NULL)
			{
				TreeItemData *pToData = (TreeItemData *)m_pTreeCtrl->GetItemData(hToNode);
				if(pToData)
				{
					taxiwayItem.m_nodeTo.m_nNodeID = pToData->m_nNodeID;
					taxiwayItem.m_nodeTo.m_strName = pToData->strObjName;
				}
			}

		//	InitTaxiwayItemNodes(taxiwayItem);
			pParam->AddItem(taxiwayItem);

			//for init result 
			InitTaxiwaySpecifiedItemNodes(taxiwayItem);
			pParam->SetUseAllTaxiway(false);
		}

		//get next taxiway item
		hRootItem = m_pTreeCtrl->GetNextSiblingItem(hRootItem);
	}

	// selected all taxiways
	if (0 == pParam->getItemCount())
	{
		pParam->SetUseAllTaxiway(true);
		std::vector<int> vAirportIds;
		InputAirside::GetAirportList(m_nProjID, vAirportIds);
		for (std::vector<int>::iterator iterAirportID = vAirportIds.begin();
			iterAirportID != vAirportIds.end();++iterAirportID)
		{
			ALTObjectList vTaxiways;
			ALTAirport::GetTaxiwayList(*iterAirportID,vTaxiways);

			for (ALTObjectList::iterator iteTaxiway = vTaxiways.begin();
				iteTaxiway != vTaxiways.end();++iteTaxiway)
			{
				CTaxiwayItem taxiwayItem;
				taxiwayItem.m_nTaxiwayID = (*iteTaxiway)->getID();
				taxiwayItem.m_strTaxiwayName = (*iteTaxiway)->GetObjectName().GetIDString();
				taxiwayItem.m_nodeFrom.m_nNodeID = -1;
				taxiwayItem.m_nodeFrom.m_strName = _T("");
				taxiwayItem.m_nodeTo.m_nNodeID = -1;
				taxiwayItem.m_nodeTo.m_strName = _T("");
			//	InitTaxiwayItemNodes(taxiwayItem);
				std::vector<CAirsideIntersectionTreePerformer::NodeInfo> vNodeInfo = AirsideReControlView::CAirsideIntersectionTreePerformer::GetTaxiwayNodes(taxiwayItem.m_nTaxiwayID);

				int nStartPos= -1;
				int nEndPos = -1;
				for (int nNode = 0; nNode < static_cast<int>(vNodeInfo.size()); ++nNode )
				{
					CAirsideIntersectionTreePerformer::NodeInfo& nodeinfo = vNodeInfo[nNode];
					if(nodeinfo.nNodeID == taxiwayItem.m_nodeFrom.m_nNodeID)
						nStartPos = nNode;
					if(nodeinfo.nNodeID == taxiwayItem.m_nodeTo.m_nNodeID)
						nEndPos = nNode;
				}

				if(nStartPos == -1)
					nStartPos = 0;
				if(nEndPos == -1)
					nEndPos =static_cast<int>(vNodeInfo.size())-1;

				if(nStartPos > nEndPos)
				{	
					std::swap(nStartPos,nEndPos);
				}

				for (int nPos = nStartPos; nPos <= nEndPos; ++nPos)
				{	
					CAirsideIntersectionTreePerformer::NodeInfo& nodeinfo = vNodeInfo[nPos];

					CTaxiwayItem::CTaxiwayNodeItem item;
					item.m_nNodeID = nodeinfo.nNodeID;
					item.m_strName = nodeinfo.strNodeName;
					taxiwayItem.m_vNodeItems.push_back(item);
				}

				pParam->GetResultTaxiwayItemList().AddItem(taxiwayItem);
				pParam->AddItem(taxiwayItem);
			}
		}
	}
}

void AirsideReControlView::CAirsideTaxiwayUtilizationTreePerformer::OnUpdateToolBarDel( CCmdUI *pCmdUI )
{
	if(m_pTreeCtrl == NULL)
		return;

	HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem();
	if( hTreeItem == NULL)
		return;

	TreeItemData *pItemData = (TreeItemData *)m_pTreeCtrl->GetItemData(hTreeItem);
	pCmdUI->Enable(pItemData && pItemData->itemType == Item_Root);
}

void AirsideReControlView::CAirsideTaxiwayUtilizationTreePerformer::InitTree()
{
	ASSERT(m_pParam != NULL);

	if(m_pParam == NULL || m_pTreeCtrl == NULL)
		return;

	CAirsideTaxiwayUtilizationPara *pParam = (CAirsideTaxiwayUtilizationPara *)m_pParam;
	if(pParam == NULL)
		return;

	m_pTreeCtrl->DeleteAllItems();

	int nItemCount = pParam->getItemCount();
	for (int nItem = 0; nItem < nItemCount; ++ nItem)
	{
		CTaxiwayItem *taxiwayItem = pParam->getItem(nItem);
		AddTaxiwayItem(*taxiwayItem);
	}
	if (0 == nItemCount)
	{
		InsertAllTaxiwayItem();
	}
}

HTREEITEM AirsideReControlView::CAirsideTaxiwayUtilizationTreePerformer::InitDefaltTree()
{
	TreeItemData *pItemData = new TreeItemData;
	pItemData->itemType = Item_Root;

	//if taxiway, taxiway name
	//if node, node name
	pItemData->strObjName = _T("Taxiway");
	HTREEITEM hRootItem = InsertItem(TVI_ROOT,_T("Taxiway"),pItemData);

	ASSERT(hRootItem != NULL);
	return hRootItem;
}

void AirsideReControlView::CAirsideTaxiwayUtilizationTreePerformer::InsertAllTaxiwayItem()
{
	HTREEITEM hRootItem = InitDefaltTree();
	ASSERT(hRootItem);
	if (m_hAllTaxiwayItem)
	{
		m_pTreeCtrl->DeleteItem(m_hAllTaxiwayItem);
		m_hAllTaxiwayItem = NULL;
	}
	m_hAllTaxiwayItem = hRootItem;
	HTREEITEM hAllTaxiwayItem = InsertItem(hRootItem, _T("All taxiway"), NULL);
	InsertItem(hAllTaxiwayItem,_T("From: START"), NULL);
	InsertItem(hAllTaxiwayItem,_T("To: END"), NULL);
	m_pTreeCtrl->Expand(hAllTaxiwayItem,TVE_EXPAND);
	m_pTreeCtrl->Expand(m_hRootItem,TVE_EXPAND);
}

void AirsideReControlView::CAirsideTaxiwayUtilizationTreePerformer::OnToolBarAdd(HTREEITEM hTreeItem)
{
	//check item
	if(m_pTreeCtrl == NULL)
		return;

	if(hTreeItem == NULL)
		return;

	TreeItemData *pItemData = (TreeItemData *)m_pTreeCtrl->GetItemData(hTreeItem);
	if(pItemData == NULL)
		return;

	if(pItemData->itemType == Item_Root)
	{
		CDlgTaxiwayFamilySelect dlg(m_nProjID);
		if(dlg.DoModal() != IDOK)
			return;
		int nTaxiwayID = dlg.GetSelTaxiwayFamilyID();
		CString strTaxiwayName = dlg.GetSelTaxiwayFamilyName();

		//Insert the item to tree
		CTaxiwayItem taxiwayItem;
		taxiwayItem.m_nTaxiwayID = nTaxiwayID;
		taxiwayItem.m_strTaxiwayName = strTaxiwayName;

		AddTaxiwayItem(taxiwayItem);

		if (m_hAllTaxiwayItem)
		{
			((CAirsideIntersectionReportParam*)m_pParam)->SetUseAllTaxiway(false);
			m_pTreeCtrl->DeleteItem(m_hAllTaxiwayItem);
			m_hAllTaxiwayItem = NULL;
		}
	}
}

void AirsideReControlView::CAirsideTaxiwayUtilizationTreePerformer::OnToolBarDel(HTREEITEM hTreeItem)
{
	//check item
	if(m_pTreeCtrl == NULL)
		return;

	if(hTreeItem == NULL)
		return;

	TreeItemData *pItemData = (TreeItemData *)m_pTreeCtrl->GetItemData(hTreeItem);
	if(pItemData == NULL)
		return;

	if(pItemData->itemType == Item_Root)
	{
		m_pTreeCtrl->DeleteItem(hTreeItem);
	}

	// if all deleted, add all taxiway
	if (NULL == m_pTreeCtrl->GetRootItem())
		InsertAllTaxiwayItem();
}

void AirsideReControlView::CAirsideTaxiwayUtilizationTreePerformer::AddTaxiwayItem(const CTaxiwayItem& taxiwayItem)
{
	CString strNodeName;
	strNodeName = _T("Taxiway: ") + taxiwayItem.m_strTaxiwayName;

	TreeItemData *pItemDataRoot = new TreeItemData;
	pItemDataRoot->itemType = Item_TaxiwayRoot;
	pItemDataRoot->m_nTaxiwayID = taxiwayItem.m_nTaxiwayID;
	pItemDataRoot->strObjName = taxiwayItem.m_strTaxiwayName;

	HTREEITEM hRootItem = InitDefaltTree();
	HTREEITEM hTaxiwayRoot = InsertItem(hRootItem,strNodeName,pItemDataRoot);

	ASSERT(hTaxiwayRoot != NULL);
	if(hTaxiwayRoot == NULL)
		return;

	//add node from
	{
		CString strFromNodeName;
		strFromNodeName = _T("From: ") + taxiwayItem.m_nodeFrom.m_strName;

		TreeItemData *pItemDataRoot = new TreeItemData;
		pItemDataRoot->itemType = Item_NodeFrom;
		pItemDataRoot->m_nNodeID = taxiwayItem.m_nodeFrom.m_nNodeID;
		pItemDataRoot->m_nTaxiwayID = taxiwayItem.m_nTaxiwayID;
		pItemDataRoot->strObjName = taxiwayItem.m_nodeFrom.m_strName;

		InsertItem(hTaxiwayRoot,strFromNodeName,pItemDataRoot);
	}
	//add node to
	{
		CString strToNodeName;
		strToNodeName = _T("To: ") + taxiwayItem.m_nodeTo.m_strName;

		TreeItemData *pItemDataRoot = new TreeItemData;
		pItemDataRoot->itemType = Item_NodeTo;
		pItemDataRoot->m_nNodeID = taxiwayItem.m_nodeTo.m_nNodeID;
		pItemDataRoot->m_nTaxiwayID = taxiwayItem.m_nTaxiwayID;
		pItemDataRoot->strObjName = taxiwayItem.m_nodeTo.m_strName;

		InsertItem(hTaxiwayRoot,strToNodeName,pItemDataRoot);
	}
	m_pTreeCtrl->Expand(hTaxiwayRoot,TVE_EXPAND);
	m_pTreeCtrl->Expand(m_hRootItem,TVE_EXPAND);
}

std::vector<CAirsideIntersectionTreePerformer::NodeInfo> CAirsideTaxiwayUtilizationTreePerformer::GetTaxiwayNodes(int nTaxiwayID)
{
	std::map<int, std::vector<NodeInfo> >::iterator iterFind = m_mapTaxiwayNodes.find(nTaxiwayID);
	if( iterFind != m_mapTaxiwayNodes.end())
		return (*iterFind).second;

	std::vector<CAirsideIntersectionTreePerformer::NodeInfo> vNodeInfo;
	//find the taxiway group nodes
	ALTObjectGroup altGroup;
	altGroup.ReadData(nTaxiwayID);
	std::vector< ALTObject> vObjct;
	altGroup.GetObjects(vObjct);
	for (int i = 0; i < static_cast<int>(vObjct.size()); i++)
	{
		ALTObject& obj = vObjct.at(i);
		Taxiway taxiway;
		taxiway.ReadObject(obj.getID());
		
		std::vector<IntersectionNode> vrInterNode;
		vrInterNode.clear();
		vrInterNode = taxiway.GetIntersectNodes();
		for (std::vector<IntersectionNode>::iterator itrNode = vrInterNode.begin();itrNode != vrInterNode.end();++itrNode)
		{
			CAirsideIntersectionTreePerformer::NodeInfo nodeInfo;
			nodeInfo.strNodeName = (*itrNode).GetName();
			nodeInfo.nNodeID = (*itrNode).GetID();
			nodeInfo.dDistFromStart = (*itrNode).GetDistance(obj.getID());

			vNodeInfo.push_back(nodeInfo);
		}

		std::sort(vNodeInfo.begin(),vNodeInfo.end());
	}
	
	m_mapTaxiwayNodes[nTaxiwayID] = vNodeInfo;


	return vNodeInfo;
}

void CAirsideTaxiwayUtilizationTreePerformer::OnEditIntersectionFrom(HTREEITEM hTreeItem, int nTaxiwayID)
{
	std::vector<CString> vStr;

	ALTObjectGroup altGroup;
	altGroup.ReadData(nTaxiwayID);
	std::vector< ALTObject> vObjct;
	altGroup.GetObjects(vObjct);

	if (vObjct.size() == 1)
	{
		vStr.push_back(_T("START"));
		std::vector<CAirsideIntersectionTreePerformer::NodeInfo> vNodeInfo = GetTaxiwayNodes(nTaxiwayID);
		for(int nNode = 0; nNode < static_cast<int>(vNodeInfo.size()); ++ nNode)
		{
			CAirsideIntersectionTreePerformer::NodeInfo& nodeinfo = vNodeInfo[nNode];
			vStr.push_back(nodeinfo.strNodeName);
		}
	}

	m_pTreeCtrl->SetComboWidth(120);
	m_pTreeCtrl->SetComboString(hTreeItem,vStr);

	m_hFromOrToNodeSelected = hTreeItem;
}

void CAirsideTaxiwayUtilizationTreePerformer::OnEditIntersectionTo(HTREEITEM hTreeItem, int nTaxiwayID)
{
	std::vector<CString> vStr;

	ALTObjectGroup altGroup;
	altGroup.ReadData(nTaxiwayID);
	std::vector< ALTObject> vObjct;
	altGroup.GetObjects(vObjct);

	if (vObjct.size() == 1)
	{
		std::vector<CAirsideIntersectionTreePerformer::NodeInfo> vNodeInfo = GetTaxiwayNodes(nTaxiwayID);
		for(int nNode = 0; nNode < static_cast<int>(vNodeInfo.size()); ++ nNode)
		{
			CAirsideIntersectionTreePerformer::NodeInfo& nodeinfo = vNodeInfo[nNode];
			vStr.push_back(nodeinfo.strNodeName);
		}
		vStr.push_back(_T("END"));
	}
	
	m_pTreeCtrl->SetComboWidth(120);
	m_pTreeCtrl->SetComboString(hTreeItem,vStr);
	m_hFromOrToNodeSelected = hTreeItem;
}

LRESULT AirsideReControlView::CAirsideTaxiwayUtilizationTreePerformer::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_INPLACE_COMBO2)
	{
		if( m_hFromOrToNodeSelected == NULL)
			return 0;

		TreeItemData *pItemData = (TreeItemData *)m_pTreeCtrl->GetItemData(m_hFromOrToNodeSelected);
		if(pItemData == NULL)
			return 0;
		int nSel = m_pTreeCtrl->m_comboBox.GetCurSel();

		if(nSel >=0 && pItemData->itemType == Item_NodeFrom)
		{
			if(nSel == 0)//start
			{
				m_pTreeCtrl->SetItemText(m_hFromOrToNodeSelected,_T("From : START"));
				pItemData->m_nNodeID = -1;
				pItemData->strObjName = _T("START");
			}
			else
			{
				std::vector<CAirsideIntersectionTreePerformer::NodeInfo> vNodeInfo = GetTaxiwayNodes(pItemData->m_nTaxiwayID);
				if(nSel-1 < static_cast<int>(vNodeInfo.size())) 
				{
					m_pTreeCtrl->SetItemText(m_hFromOrToNodeSelected,_T("From: ") + vNodeInfo[nSel-1].strNodeName);
					pItemData->m_nNodeID = vNodeInfo[nSel-1].nNodeID;
					pItemData->strObjName = vNodeInfo[nSel-1].strNodeName;
				}
			}

		}
		else if(nSel >=0 && pItemData->itemType == Item_NodeTo)
		{	
			std::vector<CAirsideIntersectionTreePerformer::NodeInfo> vNodeInfo = GetTaxiwayNodes(pItemData->m_nTaxiwayID);
			if(nSel < static_cast<int>(vNodeInfo.size()))//start
			{
				m_pTreeCtrl->SetItemText(m_hFromOrToNodeSelected,_T("To: ") + vNodeInfo[nSel].strNodeName);
				pItemData->m_nNodeID = vNodeInfo[nSel].nNodeID;
				pItemData->strObjName = vNodeInfo[nSel].strNodeName;
			}
			else
			{
				m_pTreeCtrl->SetItemText(m_hFromOrToNodeSelected,_T("To: END") );
				pItemData->m_nNodeID = -1;
				pItemData->strObjName = _T("END");
			}
		}
	}

	return 0;
}
////////////////////stand operation report////////////////////////////////////////////////////////////
CStandOperationsTreePerformer::CStandOperationsTreePerformer(int nProjID, CARCTreeCtrlEx *pTreeCtrl, CParameters *pParam)
:CTreePerformer(nProjID,pTreeCtrl,pParam)
{

}

CStandOperationsTreePerformer::~CStandOperationsTreePerformer()
{

}

void CStandOperationsTreePerformer::InitTree()
{
	ASSERT(m_pTreeCtrl != NULL);
	ASSERT(m_pParam != NULL);

	if(m_pParam == NULL || m_pTreeCtrl == NULL)
		return;

	CFlightStandOperationParameter *pStandParam = (CFlightStandOperationParameter *)m_pParam;
	if(pStandParam == NULL)
		return;

	m_pTreeCtrl->DeleteAllItems();

	if( pStandParam->getCount() == 0)//default
	{
		InitDefaltTree();
	}
	else
	{
		for (int i = 0; i < pStandParam->getCount(); i++)
		{
			ALTObjectID standObjID =  pStandParam->getItem(i);
			CString strStand(_T(""));
			if (standObjID.GetIDString().IsEmpty())
			{
				strStand.Format(_T("Family: ALL"));
			}
			else
			{
				strStand.Format(_T("Family: %s"),standObjID.GetIDString());
			}
		
			HTREEITEM hStandItem = m_pTreeCtrl->InsertItem(strStand,TVI_ROOT);
		}
	}
}

void CStandOperationsTreePerformer::InitDefaltTree()
{
	HTREEITEM hRunwayItem = m_pTreeCtrl->InsertItem(_T("Family: ALL"),TVI_ROOT);
}

void CStandOperationsTreePerformer::LoadData()
{
	InitTree();
}

void CStandOperationsTreePerformer::SaveData()
{
	ASSERT(m_pTreeCtrl != NULL);
	ASSERT(m_pParam != NULL);

	if(m_pParam == NULL || m_pTreeCtrl == NULL)
		return;

	CFlightStandOperationParameter *pParam = (CFlightStandOperationParameter *)m_pParam;
	if(pParam == NULL)
		return;

	pParam->clear();

	//stand item

	HTREEITEM hStandItem = m_pTreeCtrl->GetRootItem();

	while(hStandItem != NULL)
	{
		CString strStandID = m_pTreeCtrl->GetItemText(hStandItem);
		int nPos = strStandID.Find(':');
		strStandID = strStandID.Right(strStandID.GetLength() - nPos -1);
		strStandID.TrimLeft();
		strStandID.TrimRight();
		if (strStandID == _T("ALL"))
		{
			strStandID = CString(_T(""));
		}
		ALTObjectID standObjID(strStandID.GetBuffer());

		pParam->addItem(standObjID);	
		hStandItem = m_pTreeCtrl->GetNextSiblingItem(hStandItem);
	}
}

void CStandOperationsTreePerformer::OnToolBarAdd(HTREEITEM hTreeItem)
{
	if(m_pTreeCtrl == NULL)
		return;

	CDlgSelectALTObject dlg(m_nProjID,ALT_STAND,m_pTreeCtrl->GetParent());
	if(dlg.DoModal() == IDOK)
	{
		CString strStand(_T(""));
		if (dlg.GetALTObject().GetIDString().IsEmpty())
		{
			strStand.Format(_T("Family: ALL"));
		}
		else
		{
			strStand.Format(_T("Family: %s"),dlg.GetALTObject().GetIDString());
		}
	
		HTREEITEM hItemRunway = m_pTreeCtrl->InsertItem(strStand,TVI_ROOT);
	}
}

void CStandOperationsTreePerformer::OnToolBarDel(HTREEITEM hTreeItem)
{
	if(hTreeItem == NULL)
		return;

	m_pTreeCtrl->DeleteItem(hTreeItem);	
}

void CStandOperationsTreePerformer::OnUpdateToolBarAdd(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void CStandOperationsTreePerformer::OnUpdateToolBarDel(CCmdUI *pCmdUI)
{
	HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem();
	if( hTreeItem == NULL)
		pCmdUI->Enable(FALSE);
	else
		pCmdUI->Enable(TRUE);
}

//////////////////////////////////////////////////////////////////////////
//
AirsideReControlView::CAirsideControllerWorkloadTreePerformer::CAirsideControllerWorkloadTreePerformer( int nProjID, CARCTreeCtrlEx *pTreeCtrl, CParameters *pParam )
:CTreePerformer(nProjID,pTreeCtrl,pParam)
{
	m_hAreaRoot = NULL;
	m_hSectorRoot = NULL;
	m_hWeightRoot = NULL;
}

AirsideReControlView::CAirsideControllerWorkloadTreePerformer::~CAirsideControllerWorkloadTreePerformer()
{

}

void AirsideReControlView::CAirsideControllerWorkloadTreePerformer::InitTree()
{
	ASSERT(m_pTreeCtrl != NULL);
	ASSERT(m_pParam != NULL);

	if(m_pParam == NULL || m_pTreeCtrl == NULL)
		return;

	CAirsideControllerWorkloadParam *pParam = (CAirsideControllerWorkloadParam *)m_pParam;
	if(pParam == NULL)
		return;

	m_pTreeCtrl->DeleteAllItems();
	//add sector root

	m_hSectorRoot = InsertItem(TVI_ROOT,_T("Sectors"),NewItemData(Item_Root, ALTObjectID(_T("Sectors"))));
	if( pParam->getSectorCout() == 0)//default
	{
			InsertItem(m_hSectorRoot, "All", NewItemData(Item_Item,ALTObjectID()));		
	}
	else
	{
		int nItemCount = pParam->getSectorCout();
		for(int nItem = 0; nItem < nItemCount; ++ nItem)
		{
			InsertItem(m_hSectorRoot, pParam->getSector(nItem).GetIDString(), NewItemData(Item_Item, pParam->getSector(nItem)));
		}
	}

	//add area root
	m_hAreaRoot = InsertItem(TVI_ROOT,_T("Areas"), NewItemData(Item_Root,  ALTObjectID(_T("Areas"))));
	if( pParam->getSectorCout() == 0)//default
	{
			InsertItem(m_hAreaRoot, "All", NewItemData(Item_Item, ALTObjectID()));		
	}
	else
	{
		int nItemCount = pParam->getAreaCout();
		for(int nItem = 0; nItem < nItemCount; ++ nItem)
		{
			InsertItem(m_hAreaRoot, pParam->getArea(nItem).GetIDString(), NewItemData(Item_Item, pParam->getArea(nItem)));
		}
	}

	//add weights
	m_hWeightRoot = InsertItem(TVI_ROOT,_T("Weights/event"), NewItemData(Item_Root,  ALTObjectID(_T("Weights"))));
	for (int weight = CAirsideControllerWorkloadParam::weight_PushBacks;
		weight < CAirsideControllerWorkloadParam::weight_count; ++weight)
	{
		//default weight is 1
		CString strWeightName = pParam->getWeightName((CAirsideControllerWorkloadParam::enumWeight)weight);
		
		InsertItem(m_hWeightRoot,FormatWeightText(strWeightName, 1), NewItemData(Item_Operation,strWeightName ,1));

	}



	m_pTreeCtrl->Expand(m_hSectorRoot, TVE_EXPAND);
	m_pTreeCtrl->Expand(m_hAreaRoot, TVE_EXPAND);
	//m_pTreeCtrl->Expand(m_hWeightRoot, TVE_EXPAND);
}

void AirsideReControlView::CAirsideControllerWorkloadTreePerformer::SaveData()
{
	CAirsideControllerWorkloadParam *pParam = (CAirsideControllerWorkloadParam *)m_pParam;
	if(pParam == NULL)
		return;
	pParam->Clear();


	//sector
	HTREEITEM hItem = m_pTreeCtrl->GetChildItem(m_hSectorRoot);
	while (hItem)
	{
		TreeItemData *pData = (TreeItemData *)m_pTreeCtrl->GetItemData(hItem);
		if(pData && !pData->altObjID.IsBlank())
		{
			pParam->AddSector(pData->altObjID);			
		}
		hItem = m_pTreeCtrl->GetNextSiblingItem(hItem);
	}

	//area
	hItem = m_pTreeCtrl->GetChildItem(m_hAreaRoot);
	while (hItem)
	{
		TreeItemData *pData = (TreeItemData *)m_pTreeCtrl->GetItemData(hItem);
		if(pData && !pData->altObjID.IsBlank())
		{
			pParam->AddArea(pData->altObjID);			
		}
		hItem = m_pTreeCtrl->GetNextSiblingItem(hItem);
	}


}

reportType AirsideReControlView::CAirsideControllerWorkloadTreePerformer::GetType()
{
	return Airside_ControllerWorkload;
}

LRESULT AirsideReControlView::CAirsideControllerWorkloadTreePerformer::DefWindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	if( message == WM_INPLACE_SPIN )
	{
		if(m_hItemSelected == NULL)
			return 0;

		TreeItemData *pNodeData = (TreeItemData *)m_pTreeCtrl->GetItemData(m_hItemSelected);
		if(pNodeData == NULL)
			return 0;
		if(pNodeData->itemType != Item_Operation)
			return 0;

		LPSPINTEXT pst = (LPSPINTEXT) lParam;
		ASSERT(pst->iPercent >0 && pst->iPercent< 11);

		if(pst->iPercent <=0)
			pst->iPercent = 0;

		if (pst->iPercent > 10)
		{
			pst->iPercent = 10;
		}

		//changed the text
		pNodeData->nObjID = pst->iPercent;

		m_pTreeCtrl->SetItemText(m_hItemSelected, FormatWeightText(pNodeData->strObjName, pNodeData->nObjID));

	}
	return 0;
}

HTREEITEM AirsideReControlView::CAirsideControllerWorkloadTreePerformer::InitDefaltTree()
{
	return NULL;
}

void AirsideReControlView::CAirsideControllerWorkloadTreePerformer::OnToolBarAdd( HTREEITEM hTreeItem )
{
	int nProjectID = m_nProjID;
	std::vector<ALTOBJECT_TYPE> vObjectType;

	//add sector
	if(hTreeItem == m_hSectorRoot)
	{
		vObjectType.push_back(ALT_SECTOR);

		DlgSelectMulti_ALTObjectList dlg(nProjectID, vObjectType);
		if(dlg.DoModal() == IDOK)
		{
			ALTObjectID altSector = dlg.GetALTObject();

			CString strObject = altSector.GetIDString();
			//delete all node, if exists
			HTREEITEM hChildItem = m_pTreeCtrl->GetChildItem(hTreeItem);
			if(hChildItem != NULL)
			{
				TreeItemData *pNodeData = (TreeItemData *)m_pTreeCtrl->GetItemData(hChildItem);
				if(pNodeData != NULL)
				{
					if(pNodeData->altObjID.IsBlank())//all areas
					{
						m_pTreeCtrl->DeleteItem(hChildItem);
					}
				}
			}	

			if (IsObjectSelected(m_hSectorRoot, altSector))
				return;

			if (strObject == _T(""))
				strObject = "All";

			InsertItem(m_hSectorRoot, strObject, NewItemData(Item_Item, altSector));
		}
	}

	//add reporting area
	if(hTreeItem == m_hAreaRoot)
	{
		vObjectType.push_back(ALT_REPORTINGAREA);
		DlgSelectMulti_ALTObjectList dlg(nProjectID, vObjectType);
		if(dlg.DoModal() == IDOK)
		{
			ALTObjectID altArea = dlg.GetALTObject();

			CString strObject = altArea.GetIDString();
			//delete all node, if exists
			HTREEITEM hChildItem = m_pTreeCtrl->GetChildItem(hTreeItem);
			if(hChildItem != NULL)
			{
				TreeItemData *pNodeData = (TreeItemData *)m_pTreeCtrl->GetItemData(hChildItem);
				if(pNodeData != NULL)
				{
					if(pNodeData->altObjID.IsBlank())//all areas
					{
						m_pTreeCtrl->DeleteItem(hChildItem);
					}
				}
			}	

			if (IsObjectSelected(hTreeItem, altArea))
				return;

			if (strObject == _T(""))
				strObject = "All";

			InsertItem(hTreeItem, strObject, NewItemData(Item_Item, altArea));
		}
	}


}
BOOL AirsideReControlView::CAirsideControllerWorkloadTreePerformer::IsObjectSelected(HTREEITEM hObjRoot,const ALTObjectID& altID)
{
	HTREEITEM hChildItem = m_pTreeCtrl->GetChildItem(hObjRoot);
	while (hChildItem != NULL)
	{
		TreeItemData *pNodeData = (TreeItemData *)m_pTreeCtrl->GetItemData(hChildItem);
		if(pNodeData != NULL)
		{

			if (altID.idFits(pNodeData->altObjID))
				return TRUE;

		}
		hChildItem = m_pTreeCtrl->GetNextSiblingItem(hChildItem);
	}
	return FALSE;
}

void AirsideReControlView::CAirsideControllerWorkloadTreePerformer::OnToolBarDel( HTREEITEM hTreeItem )
{
	if(hTreeItem == NULL)
		return;

	TreeItemData *pNodeData = (TreeItemData *)m_pTreeCtrl->GetItemData(hTreeItem);
	if(pNodeData == NULL)
		return;
	if(pNodeData->itemType == Item_Root)
		return;

	if(m_pTreeCtrl->GetNextSiblingItem(hTreeItem))
	{		
		m_pTreeCtrl->DeleteItem(hTreeItem);
	}
	else
	{
		HTREEITEM hParentItem = m_pTreeCtrl->GetParentItem(hTreeItem);
		m_pTreeCtrl->DeleteItem(hTreeItem);
		InsertItem(hParentItem, "All", NewItemData(Item_Item, ALTObjectID()));		
	
		m_pTreeCtrl->Expand(hParentItem, TVE_EXPAND);
	}

		
}

void AirsideReControlView::CAirsideControllerWorkloadTreePerformer::OnUpdateToolBarDel( CCmdUI *pCmdUI )
{

	if(m_pTreeCtrl == NULL)
		return;

	HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem();
	if( hTreeItem == NULL)
		return;

	TreeItemData *pItemData = (TreeItemData *)m_pTreeCtrl->GetItemData(hTreeItem);
	pCmdUI->Enable(pItemData && pItemData->itemType == Item_Item);

}

TreeItemData * AirsideReControlView::CAirsideControllerWorkloadTreePerformer::NewItemData( TreeItemType enumType, ALTObjectID objName )
{
	TreeItemData *pData = new TreeItemData;
	pData->itemType = enumType;//Item_Root; Item_Item;
	pData->altObjID = objName;

	return pData;
}

TreeItemData * AirsideReControlView::CAirsideControllerWorkloadTreePerformer::NewItemData( TreeItemType enumType, CString strWeightName ,int nWeight )
{
	TreeItemData *pData = new TreeItemData;
	pData->itemType = enumType;//Item_Root; Item_Item;
	pData->nObjID = nWeight;
	pData->strObjName = strWeightName;

	return pData;
}
void AirsideReControlView::CAirsideControllerWorkloadTreePerformer::LoadData()
{
	InitTree();

}

void AirsideReControlView::CAirsideControllerWorkloadTreePerformer::OnTreeItemDoubleClick( HTREEITEM hTreeItem )
{
	if(hTreeItem == NULL)
		return;

	TreeItemData *pNodeData = (TreeItemData *)m_pTreeCtrl->GetItemData(hTreeItem);
	if(pNodeData == NULL)
		return;
	if(pNodeData->itemType != Item_Operation)
		return;
	m_hItemSelected = hTreeItem;


	//set the spin 
	m_pTreeCtrl->SetDisplayType( 2 );
	m_pTreeCtrl->SetDisplayNum( pNodeData->nObjID );
	m_pTreeCtrl->SetSpinRange( 1,10 );
	m_pTreeCtrl->SpinEditLabel( hTreeItem );


}

CString AirsideReControlView::CAirsideControllerWorkloadTreePerformer::FormatWeightText( CString strWeightName ,int nWeight ) const
{
	CString strText;
	strText.Format(_T("%s : %d"),strWeightName, nWeight);
	return strText;
}


AirsideReControlView::CTakeoffProcessTreePerformer::CTakeoffProcessTreePerformer( int nProjID, CARCTreeCtrlEx *pTreeCtrl, CParameters *pParam )
:CTreePerformer(nProjID,pTreeCtrl,pParam)
{

}

AirsideReControlView::CTakeoffProcessTreePerformer::~CTakeoffProcessTreePerformer()
{
	for (size_t i = 0; i < m_vData.size(); i++)
	{
		delete m_vData[i];
	}
	m_vData.clear();
}

void AirsideReControlView::CTakeoffProcessTreePerformer::InitTree()
{
	CAirsideTakeoffProcessParameter* pParam = (CAirsideTakeoffProcessParameter*)m_pParam;

	HTREEITEM hItemFlightRoot = m_pTreeCtrl->InsertItem(_T("Flight Type"),TVI_ROOT);
	TreeItemData* pItemData = new TreeItemData;
	pItemData->itemType = Item_FlightRoot;

	m_pTreeCtrl->SetItemData(hItemFlightRoot,(DWORD_PTR)pItemData);
	if (pParam->getFlightConstraintCount() == 0)
	{
		//flight type
		HTREEITEM hItemFlight = m_pTreeCtrl->InsertItem(_T("DEFAULT"),hItemFlightRoot);
		pItemData = new TreeItemData;
		pItemData->itemType = Item_Flight;
		m_pTreeCtrl->SetItemData(hItemFlight,(DWORD_PTR)pItemData);
		m_vData.push_back(pItemData);
	}
	else
	{
		for (size_t i = 0; i < pParam->getFlightConstraintCount(); i++)
		{
			FlightConstraint& newFltConstr = pParam->getFlightConstraint(i);
			CString strNewFlightType;
			newFltConstr.screenPrint(strNewFlightType.GetBuffer(1024));
			strNewFlightType.ReleaseBuffer();

			HTREEITEM hItemFlight = m_pTreeCtrl->InsertItem(strNewFlightType,hItemFlightRoot);
			TreeItemData* pItemDataFlight = new TreeItemData;
			pItemDataFlight->itemType = Item_Flight;
			pItemDataFlight->fltCons = newFltConstr;
			m_pTreeCtrl->SetItemData(hItemFlight,(DWORD_PTR)pItemDataFlight);
			m_vData.push_back(pItemData);
		}
	}
	m_pTreeCtrl->Expand(hItemFlightRoot,TVE_EXPAND);

	HTREEITEM hItemTakeoffRoot = m_pTreeCtrl->InsertItem(_T("Take off positions"));
	pItemData = new TreeItemData;
	pItemData->itemType = Item_Root;
	m_pTreeCtrl->SetItemData(hItemTakeoffRoot,(DWORD_PTR)pItemData);

	if (pParam->GetPositionCount() == 0)
	{
		HTREEITEM hDefaultItem = m_pTreeCtrl->InsertItem("All",hItemTakeoffRoot);
		pItemData = new TreeItemData;
		pItemData->itemType = Item_Item;
		pItemData->strObjName = CString(_T("All"));
		m_pTreeCtrl->SetItemData(hDefaultItem,(DWORD_PTR)pItemData);
		m_vData.push_back(pItemData);
	}
	else
	{
		for (int i = 0; i < pParam->GetPositionCount(); i++)
		{
			CString sTakeoff = pParam->GetPosition(i);
			HTREEITEM hItemTakeoff = m_pTreeCtrl->InsertItem(sTakeoff,hItemTakeoffRoot);
			TreeItemData* pTakeoffItem = new TreeItemData;
			pTakeoffItem->itemType = Item_Item;
			pTakeoffItem->strObjName = sTakeoff;
			m_pTreeCtrl->SetItemData(hItemTakeoff,(DWORD_PTR)pTakeoffItem);
			m_vData.push_back(pItemData);
		}
	}
	m_pTreeCtrl->Expand(hItemTakeoffRoot,TVE_EXPAND);
}

void AirsideReControlView::CTakeoffProcessTreePerformer::LoadData()
{
	InitTree();
}

void AirsideReControlView::CTakeoffProcessTreePerformer::SaveData()
{
	//clear old data
	CAirsideTakeoffProcessParameter* pParam = (CAirsideTakeoffProcessParameter*)m_pParam;
	pParam->ClearPosition();
	pParam->ClearFlightConstraint();

	HTREEITEM hFlightRoot = m_pTreeCtrl->GetRootItem();
	HTREEITEM hFlightItem = m_pTreeCtrl->GetChildItem(hFlightRoot);
	while(hFlightItem)
	{
		TreeItemData *pItemData = (TreeItemData *)m_pTreeCtrl->GetItemData(hFlightItem);
		if (pItemData)
		{
			pParam->AddFlightConstraint(pItemData->fltCons);
		}
		hFlightItem = m_pTreeCtrl->GetNextSiblingItem(hFlightItem);
	}

	HTREEITEM hTakeoffRoot = m_pTreeCtrl->GetNextSiblingItem(hFlightRoot);
	if (hTakeoffRoot)
	{
		HTREEITEM hTakoffItem = m_pTreeCtrl->GetChildItem(hTakeoffRoot);
		while (hTakoffItem)
		{	
			TreeItemData *pItemData = (TreeItemData *)m_pTreeCtrl->GetItemData(hTakoffItem);
			if (pItemData)
			{
				pParam->AddPosition(pItemData->strObjName);
			}
			hTakoffItem = m_pTreeCtrl->GetNextSiblingItem(hTakoffItem);
		}
	}
}

void AirsideReControlView::CTakeoffProcessTreePerformer::OnToolBarAdd( HTREEITEM hTreeItem )
{
	if(hTreeItem == NULL)
		return;
	TreeItemData *pItemData = (TreeItemData *)m_pTreeCtrl->GetItemData(hTreeItem);
	if (pItemData)
	{
		if (pItemData->itemType == Item_FlightRoot)
		{
			CFlightDialog dlg( m_pTreeCtrl->GetParent() );
			if( dlg.DoModal() == IDOK )
			{
				FlightConstraint newFltConstr = dlg.GetFlightSelection();

				CString strNewFlightType;
				newFltConstr.screenPrint(strNewFlightType.GetBuffer(1024));
				strNewFlightType.ReleaseBuffer();

				//flight type
				HTREEITEM hItemFlight = m_pTreeCtrl->InsertItem(strNewFlightType,hTreeItem);
				TreeItemData* pItemDataFlight = new TreeItemData;
				pItemDataFlight->itemType = Item_Flight;
				pItemDataFlight->fltCons = newFltConstr;
				m_pTreeCtrl->SetItemData(hItemFlight,(DWORD_PTR)pItemDataFlight);
				m_vData.push_back(pItemData);
			}
			m_pTreeCtrl->Expand(hTreeItem,TVE_EXPAND);
		}
		else if (pItemData->itemType == Item_Root)
		{
			CString sTakeoff;
			if(AirsideGUI::NodeViewDbClickHandler::TakeoffPositionSelect(m_nProjID,sTakeoff,m_pParam->GetAirportDB()))
			{
				//Take off
				HTREEITEM hItemTakeoff = m_pTreeCtrl->InsertItem(sTakeoff,hTreeItem);
				TreeItemData* pTakeoffItem = new TreeItemData;
				pTakeoffItem->itemType = Item_Item;
				pTakeoffItem->strObjName = sTakeoff;
				m_pTreeCtrl->SetItemData(hItemTakeoff,(DWORD_PTR)pTakeoffItem);
				m_vData.push_back(pItemData);
			}	
			m_pTreeCtrl->Expand(hTreeItem,TVE_EXPAND);
		}
	}
}

void AirsideReControlView::CTakeoffProcessTreePerformer::OnToolBarDel( HTREEITEM hTreeItem )
{
	if(m_pTreeCtrl == NULL)
		return;

	if( hTreeItem == NULL)
		return;

	TreeItemData *pItemData = (TreeItemData *)m_pTreeCtrl->GetItemData(hTreeItem);
	m_pTreeCtrl->DeleteItem(hTreeItem);
	std::vector<TreeItemData*>::iterator iter = std::find(m_vData.begin(),m_vData.end(),pItemData);
	if (pItemData&&iter != m_vData.end())
	{
		m_vData.erase(iter);

		delete pItemData;
		pItemData = NULL;
	}

}

void AirsideReControlView::CTakeoffProcessTreePerformer::OnUpdateToolBarAdd( CCmdUI *pCmdUI )
{
	if(m_pTreeCtrl == NULL)
		return;

	HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem();
	if( hTreeItem == NULL)
		return;

	TreeItemData *pItemData = (TreeItemData *)m_pTreeCtrl->GetItemData(hTreeItem);
	if (pItemData)
	{
		if (pItemData->itemType == Item_Flight || pItemData->itemType == Item_Item)
		{
			pCmdUI->Enable(FALSE);
			return;
		}
	}

	pCmdUI->Enable(TRUE);
}

void AirsideReControlView::CTakeoffProcessTreePerformer::OnUpdateToolBarDel( CCmdUI *pCmdUI )
{
	if(m_pTreeCtrl == NULL)
		return;

	HTREEITEM hTreeItem = m_pTreeCtrl->GetSelectedItem();
	if( hTreeItem == NULL)
		return;

	TreeItemData *pItemData = (TreeItemData *)m_pTreeCtrl->GetItemData(hTreeItem);
	if (pItemData)
	{
		if (pItemData->itemType == Item_Flight || pItemData->itemType == Item_Item)
		{
			pCmdUI->Enable(TRUE);
			return;
		}
	}

	pCmdUI->Enable(FALSE);
}

void AirsideReControlView::CTakeoffProcessTreePerformer::InitDefaltTree()
{
	//flight type root
	HTREEITEM hItemFlightRoot = m_pTreeCtrl->InsertItem(_T("Flight Type"),TVI_ROOT);
	TreeItemData* pItemData = new TreeItemData;
	pItemData->itemType = Item_FlightRoot;

	m_pTreeCtrl->SetItemData(hItemFlightRoot,(DWORD_PTR)pItemData);
	m_vData.push_back(pItemData);

	//flight type
	HTREEITEM hItemFlight = m_pTreeCtrl->InsertItem(_T("DEFAULT"),hItemFlightRoot);
	pItemData = new TreeItemData;
	pItemData->itemType = Item_Flight;
	m_pTreeCtrl->SetItemData(hItemFlight,(DWORD_PTR)pItemData);
	m_vData.push_back(pItemData);

	m_pTreeCtrl->Expand(hItemFlightRoot,TVE_EXPAND);


	//take off position
	HTREEITEM hItemTakeoffRoot = m_pTreeCtrl->InsertItem(_T("Take off positions"));
	pItemData = new TreeItemData;
	pItemData->itemType = Item_Root;
	m_pTreeCtrl->SetItemData(hItemTakeoffRoot,(DWORD_PTR)pItemData);
	m_vData.push_back(pItemData);

	//take off position item
	HTREEITEM hDefaultItem = m_pTreeCtrl->InsertItem("All",hItemTakeoffRoot);
	pItemData = new TreeItemData;
	pItemData->itemType = Item_Item;
	pItemData->strObjName = CString(_T("All"));
	m_pTreeCtrl->SetItemData(hDefaultItem,(DWORD_PTR)pItemData);
	m_vData.push_back(pItemData);
}