// DlgSelectTowRouteItem.cpp : implementation file
//

#include "stdafx.h"
#include "DlgSelectTowRouteItem.h"
#include "resource.h"
#include "../InputAirside/RouteItem.h"
#include "../InputAirside/Runway.h"
#include "../InputAirside/Taxiway.h"
#include "../InputAirside/InputAirside.h"
#include "../InputAirside/TowingRoute.h"
#include "../InputAirside/ALTAirport.h"
#include "../InputAirside/ALTObjectGroup.h"
#include "../Database/DBElementCollection_Impl.h"
#include "..\InputAirside\AirportTowRouteNetwork.h"
#include "../InputAirside/TowOperationRoute.h"

// CDlgSelectTowRouteItem dialog

IMPLEMENT_DYNAMIC(CDlgSelectTowRouteItem, CDialog)
CDlgSelectTowRouteItem::CDlgSelectTowRouteItem(int nProjID,CTowOperationRouteItem* pParentItem,CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DIALOG_SELECTTOWROUTEITEM, pParent)
	, m_nRunwayExitID(-1)
	, m_nSelALTObjID(-1)
	, m_nIntersectNodeID(-1)
	, m_strSelALTObjName("")
	, m_TaxiwayItem(NULL)
	, m_GroundRouteItem(NULL)
	, m_RunwayItem(NULL)
	, m_nProjID(nProjID)
	, m_pParentObj(NULL)
{
	m_enumItemType = CTowOperationRouteItem::ItemType_Unknown;
	m_pAirportGroundNetwork = new CAirportTowRouteNetwork(nProjID);

	if(pParentItem != NULL)
	{
		m_nIntersectNodeIDInALTObj =pParentItem->GetIntersectionID();

		if(pParentItem->GetEnumType() == CTowOperationRouteItem::ItemType_Taxiway ||
			pParentItem->GetEnumType() == CTowOperationRouteItem::ItemType_Runway)
		{
			if (pParentItem->GetObjectID() >=0)
			{
				m_pParentObj =  ALTObject::ReadObjectByID(pParentItem->GetObjectID());
			}
		}
	}
	else
	{
		m_pParentObj = NULL;

	}

}

CDlgSelectTowRouteItem::~CDlgSelectTowRouteItem()
{
	if (m_pAirportGroundNetwork)
	{
		delete m_pAirportGroundNetwork;
		m_pAirportGroundNetwork = NULL;
	}

	if (m_pParentObj)
	{
		delete m_pParentObj;
		m_pParentObj = NULL;
	}
}

void CDlgSelectTowRouteItem::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_ALTOBJNAME, m_strSelALTObjName);
	DDX_Control(pDX, IDC_TREE_ALTOBJLIST, m_ALTObjTreeCtrl);
}


BEGIN_MESSAGE_MAP(CDlgSelectTowRouteItem, CDialog)
	ON_WM_DESTROY() 
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_ALTOBJLIST, OnTvnSelchangedTreeAltobjlist)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgSelectTowRouteItem message handlers

BOOL CDlgSelectTowRouteItem::OnInitDialog()
{
	CDialog::OnInitDialog();
	UpdateData(FALSE);

	SetListCtrlContent();

	return TRUE;
}

int CDlgSelectTowRouteItem::GetSelALTObjID()
{
	return m_nSelALTObjID;
}

void CDlgSelectTowRouteItem::SetSelALTObjID(int nSelALTObjID)
{
	m_nSelALTObjID = nSelALTObjID;
}

int CDlgSelectTowRouteItem::GetIntersectNodeID()
{
	return m_nIntersectNodeID;
}

void CDlgSelectTowRouteItem::SetIntersectNodeID(int nIntersectNodeID)
{
	m_nIntersectNodeID = nIntersectNodeID;
}

CString CDlgSelectTowRouteItem::GetSelALTObjName()
{
	return m_strSelALTObjName;
}

void CDlgSelectTowRouteItem::SetSelALTObjName(CString strALTObjName)
{
	m_strSelALTObjName = strALTObjName;
}

void CDlgSelectTowRouteItem::SetListCtrlContent()
{
	m_TaxiwayItem     = m_ALTObjTreeCtrl.InsertItem(_T("Taxiway"));
	m_RunwayItem = m_ALTObjTreeCtrl.InsertItem(_T("Runway"));
	m_GroundRouteItem = m_ALTObjTreeCtrl.InsertItem(_T("Tow Route"));



	SetALTObjIntersectTaxiway(m_pParentObj);
	SetALTObjIntersectGroundRoute(m_pParentObj);


	if (NULL == m_ALTObjTreeCtrl.GetChildItem(m_TaxiwayItem))
	{
		m_ALTObjTreeCtrl.DeleteItem(m_TaxiwayItem);
		m_TaxiwayItem = NULL;
	}

	if (NULL == m_ALTObjTreeCtrl.GetChildItem(m_GroundRouteItem))
	{
		m_ALTObjTreeCtrl.DeleteItem(m_GroundRouteItem);
		m_GroundRouteItem = NULL;
	}

	if (NULL == m_ALTObjTreeCtrl.GetChildItem(m_RunwayItem))
	{
		m_ALTObjTreeCtrl.DeleteItem(m_RunwayItem);
		m_RunwayItem = NULL;
	}
}

void CDlgSelectTowRouteItem::SetALTObjIntersectTaxiway(ALTObject *pALTObj)
{
	//ASSERT(pALTObj);
	if (pALTObj == NULL)
	{
		SetTaxiwayIntersectTaxiway((Taxiway*)pALTObj);

	}
	else
	{
		switch(pALTObj->GetType())
		{
		case ALT_RUNWAY:
			{
				Runway *pRunway = (Runway*)pALTObj;
				SetRunwayIntersectTaxiway(pRunway);
			}
			break;

		case ALT_TAXIWAY:
			{
				Taxiway *pTaxiway = (Taxiway *)pALTObj;
				SetTaxiwayIntersectTaxiway(pTaxiway);
			}
			break;

		case ALT_GROUNDROUTE:
			{
				CTowingRoute *pGR = (CTowingRoute*)pALTObj;
				SetGroundRouteIntersectTaxiway(pGR);
			}
			break;

		default:
			break;
		}
	}


}

void CDlgSelectTowRouteItem::SetALTObjIntersectGroundRoute(ALTObject *pALTObj)
{
	//ASSERT(pALTObj);
	if (pALTObj == NULL)
	{
		SetTaxiwayIntersectGroundRoute((Taxiway*)pALTObj);
	}
	else
	{
		switch(pALTObj->GetType())
		{
		case ALT_RUNWAY:
			{
				Runway *pRunway = (Runway*)pALTObj;
				SetRunwayIntersectGroundRoute(pRunway);
			}
			break;

		case ALT_TAXIWAY:
			{
				Taxiway *pTaxiway = (Taxiway *)pALTObj;
				SetTaxiwayIntersectGroundRoute(pTaxiway);
			}
			break;

		case ALT_GROUNDROUTE:
			{
				CTowingRoute *pGR = (CTowingRoute*)pALTObj;
				SetGroundRouteIntersectGroundRoute(pGR);
			}
			break;

		default:
			break;
		}
	}


}

void CDlgSelectTowRouteItem::SetRunwayIntersectTaxiway(Runway *pRunway)
{

	std::set<ALTObject*> vTaxiways;
	std::set<ALTObject *> vRunways;
	vTaxiways.clear();

	m_pAirportGroundNetwork->GetIntersectedTaxiwayRunway((ALTObject*)pRunway,vTaxiways,vRunways);

	if (pRunway == NULL)
	{
		InsertTaxiways(vTaxiways);
		InsertRunways(vRunways);
		return;
	}

	//insert taxiway segments
	for (std::set<ALTObject*>::iterator iterTaxiway = vTaxiways.begin(); iterTaxiway != vTaxiways.end(); ++iterTaxiway)
	{
		ALTObject* pObj = (*iterTaxiway);
		if (pObj->GetType() != ALT_TAXIWAY)
			continue;

		IntersectionNodeList twINodeList;
		m_pAirportGroundNetwork->GetTaxiwayTaxiwayIntersectNodeList(pRunway,(Taxiway*)(*iterTaxiway) , twINodeList);

		if (twINodeList.empty())
			continue;

		for (int i=0; i<(int)twINodeList.size(); i++)
		{
			InsertTaxiwayIntersectItem((Taxiway*)(*iterTaxiway), &twINodeList[i], &twINodeList);
		}
	}

	//insert runways
	for (std::set<ALTObject*>::iterator iterRunway = vRunways.begin(); iterRunway != vRunways.end(); ++iterRunway)
	{
		ALTObject* pObj = (*iterRunway);
		if (pObj->GetType() != ALT_RUNWAY)
			continue;

		IntersectionNodeList twINodeList;
		m_pAirportGroundNetwork->GetTaxiwayTaxiwayIntersectNodeList(pRunway,(Runway*)(*iterRunway) , twINodeList);

		if (twINodeList.empty())
			continue;

		for (int i=0; i<(int)twINodeList.size(); i++)
		{
			InsertRunwayIntersectItem((Runway*)(*iterRunway), &twINodeList[i], &twINodeList);
		}
	}

	//ASSERT(pRunway);

	//RunwayExitList rwExitList;
	//pRunway->GetExitList(RUNWAYMARK_FIRST, rwExitList);
	//
	////get all runway intersections


	//vector<int> taxiwayIDList;

	//RunwayExitList::iterator rwelIter = rwExitList.begin();
	//for (; rwelIter!=rwExitList.end(); rwelIter++)
	//{
	//	RunwayExit rwExit = *rwelIter;
	//	int nTaxiwayID = rwExit.GetTaxiwayID();

	//	Taxiway * pTaxiway  = (Taxiway*)rwExit.GetIntesectionNode().GetObject(nTaxiwayID);
	//	
	//	//IntersectionNodeList twINodeList = pTaxiway->GetIntersectNodes();
	//	//IntersectionNodeList::iterator twINListIter = twINodeList.begin();
	//	//for (; twINListIter!=twINodeList.end(); twINListIter++)
	//	//{
	//	//	std::vector<TaxiwayIntersectItem*> vTaxiwayItems = twINListIter->GetTaxiwayIntersectItemList();
	//	//	for(int i=0;i< (int)vTaxiwayItems.size();i++  )
	//	//	{
	//	//		TaxiwayIntersectItem* taxiItem = vTaxiwayItems[i];
	//	//		Taxiway* pOtherTaxiway = taxiItem->GetTaxiway();

	//	//		if(pOtherTaxiway && pOtherTaxiway->getID()!= nTaxiwayID)
	//	//		{
	//	//			IntersectionNodeList otherTwNodeList;
	//	//			
	//	//			BOOL bExist = FALSE;
	//	//			vector<int>::iterator twIDLIter = taxiwayIDList.begin();
	//	//			for (; twIDLIter!=taxiwayIDList.end(); twIDLIter++)
	//	//			{
	//	//				if (pOtherTaxiway->getID() == *twIDLIter)
	//	//				{
	//	//					bExist = TRUE;
	//	//					break;
	//	//				}
	//	//			}
	//	//			//not exist
	//	//			if (!bExist)
	//	//			{
	//	//				taxiwayIDList.push_back(pOtherTaxiway->getID());
	//	//			}
	//	//		}
	//	//	}
	//	//	
	//	//}
	//}
}

void CDlgSelectTowRouteItem::SetTaxiwayIntersectTaxiway(Taxiway *pTaxiway)
{
	std::set<ALTObject*> vTaxiways;
	std::set<ALTObject *> vRunways;
	vTaxiways.clear();

	m_pAirportGroundNetwork->GetIntersectedTaxiwayRunway((ALTObject*)pTaxiway,vTaxiways,vRunways);

	if (pTaxiway == NULL)
	{
		InsertTaxiways(vTaxiways);
		InsertRunways(vRunways);
		return;
	}

	//insert taxiway segments
	for (std::set<ALTObject*>::iterator iterTaxiway = vTaxiways.begin(); iterTaxiway != vTaxiways.end(); ++iterTaxiway)
	{
		ALTObject* pObj = (*iterTaxiway);
		if (pObj->GetType() != ALT_TAXIWAY)
			continue;

		IntersectionNodeList twINodeList;
		m_pAirportGroundNetwork->GetTaxiwayTaxiwayIntersectNodeList(pTaxiway,(Taxiway*)(*iterTaxiway) , twINodeList);

		if (twINodeList.empty())
			continue;

		for (int i=0; i<(int)twINodeList.size(); i++)
		{
			InsertTaxiwayIntersectItem((Taxiway*)(*iterTaxiway), &twINodeList[i], &twINodeList);
		}
	}

	//insert runways
	for (std::set<ALTObject*>::iterator iterRunway = vRunways.begin(); iterRunway != vRunways.end(); ++iterRunway)
	{
		ALTObject* pObj = (*iterRunway);
		if (pObj == NULL || pObj->GetType() != ALT_RUNWAY)
			continue;

		IntersectionNodeList twINodeList;
		m_pAirportGroundNetwork->GetTaxiwayTaxiwayIntersectNodeList(pTaxiway,(Runway*)(*iterRunway) , twINodeList);

		if (twINodeList.empty())
			continue;

		for (int i=0; i<(int)twINodeList.size(); i++)
		{
			InsertRunwayIntersectItem((Runway*)(*iterRunway), &twINodeList[i], &twINodeList);
		}
	}


}

void CDlgSelectTowRouteItem::SetGroundRouteIntersectTaxiway(CTowingRoute *pGroundRoute)
{
	std::set<ALTObject*> vTaxiways;
	std::set<ALTObject*> vRunways;
	vTaxiways.clear();
	vRunways.clear();

	m_pAirportGroundNetwork->GetIntersectedTaxiwayRunway((ALTObject*)pGroundRoute,vTaxiways,vRunways);
	for (std::set<ALTObject*>::iterator iterTaxiway = vTaxiways.begin(); iterTaxiway != vTaxiways.end(); ++iterTaxiway)
	{
		ALTObject* pObj = (*iterTaxiway);
		if (pObj->GetType() != ALT_TAXIWAY)
			continue;

		IntersectionNodeList twINodeList;

		//m_pAirportGroundNetwork->GetGroundRouteTaxiwayIntersectNodeList(pGroundRoute,*iterTaxiway , &twINodeList);

		//if (twINodeList.empty())
		//	continue;

		//for (int i=0; i<(int)twINodeList.size(); i++)
		{
			InsertTaxiwayIntersectItem((Taxiway*)(*iterTaxiway), NULL, &twINodeList);
		}
	}

	for (std::set<ALTObject*>::iterator iterRunway = vTaxiways.begin(); iterRunway != vTaxiways.end(); ++iterRunway)
	{
		ALTObject* pObj = (*iterRunway);
		if (pObj->GetType() != ALT_RUNWAY)
			continue;

		IntersectionNodeList twINodeList;

		//m_pAirportGroundNetwork->GetGroundRouteTaxiwayIntersectNodeList(pGroundRoute,*iterRunway , &twINodeList);

	//	if (twINodeList.empty())
		//	continue;

		for (int i=0; i<(int)twINodeList.size(); i++)
		{
			InsertRunwayIntersectItem((Runway*)(*iterRunway), NULL, &twINodeList);
		}
	}
	//	}
	//}
}

void CDlgSelectTowRouteItem::SetRunwayIntersectGroundRoute(Runway *pRunway)
{
	if(pRunway == NULL)
		return;

	std::vector<CTowingRoute *> vGroundRoutes;
	vGroundRoutes.clear();
	m_pAirportGroundNetwork->GetRunwayIntersectGroundRoutes(pRunway,RUNWAYMARK_FIRST,&vGroundRoutes);

	std::vector<CTowingRoute *>::iterator grIter = vGroundRoutes.begin();
	for (; grIter!=vGroundRoutes.end(); grIter++)
	{
		IntersectionNodeList grNodeList;
		m_pAirportGroundNetwork->GetRunwayGroundRouteIntersectNodeList(pRunway, *grIter, &grNodeList);
		//ASSERT(1 <= grNodeList.size());

		InsertGroundRouteIntersectItem(*grIter,&grNodeList);
	}

}



void CDlgSelectTowRouteItem::SetTaxiwayIntersectGroundRoute(Taxiway *pTaxiway)
{

	std::vector<CTowingRoute *> grVector;
	grVector.clear();
	if (pTaxiway == NULL)
	{
		m_pAirportGroundNetwork->GetIntersectedGroundRoutes((ALTObject*)pTaxiway,grVector);
		InsertGroundRoutes(grVector);
		return;
	}

	m_pAirportGroundNetwork->GetTaxiwayIntersectGroundRoutes(pTaxiway,  &grVector);

	std::vector<CTowingRoute *>::iterator grIter = grVector.begin();
	for (; grIter!=grVector.end(); grIter++)
	{
		IntersectionNodeList grNodeList;
		m_pAirportGroundNetwork->GetTaxiwayGroundRouteIntersectNodeList(pTaxiway, *grIter, &grNodeList);
		ASSERT(1 <= grNodeList.size());

		InsertGroundRouteIntersectItem(*grIter,&grNodeList);
	}

	
}

void CDlgSelectTowRouteItem::SetGroundRouteIntersectGroundRoute(CTowingRoute *pGroundRoute)
{
	if (pGroundRoute == NULL)
		return;


	std::vector<CTowingRoute *> grVector;
	m_pAirportGroundNetwork->GetGroundRouteIntersectGroundRoutes(pGroundRoute, &grVector);

	std::vector<CTowingRoute *>::iterator grIter = grVector.begin();
	for (; grIter!=grVector.end(); grIter++)
	{
		IntersectionNodeList grNodeList;
		//m_pAirportGroundNetwork->GetGroundRouteGroundRouteIntersectNodeList(pGroundRoute, *grIter, &grNodeList);

		//if no intersect node
		//if (grNodeList.empty())
		//	continue;

		InsertGroundRouteIntersectItem(*grIter, &grNodeList);

	}

}


void CDlgSelectTowRouteItem::InsertTaxiways(std::set<ALTObject*>& vTaxiways)
{
	if(vTaxiways.empty())
		return;

	for (std::set<ALTObject*>::iterator iter = vTaxiways.begin(); iter != vTaxiways.end();iter++)
	{

		ALTObject* pObj = (*iter);
		if (pObj->GetType() != ALT_TAXIWAY)
			continue;

		CString strTexiwayName;
		Taxiway* pTaxiway = (Taxiway*)(*iter);
		strTexiwayName.Format("%s", pTaxiway->GetMarking().c_str());
		HTREEITEM hItem = m_ALTObjTreeCtrl.InsertItem(strTexiwayName, m_TaxiwayItem, TVI_LAST);

		CRouteItem *pRouteItem = new CRouteItem;
		pRouteItem->SetALTObjectID((*iter)->getID());
		pRouteItem->SetIntersectNodeID(-1);
		m_ALTObjTreeCtrl.SetItemData(hItem, (DWORD_PTR)pRouteItem);

		m_ALTObjTreeCtrl.Expand(m_TaxiwayItem, TVE_EXPAND);
	}
}
void CDlgSelectTowRouteItem::InsertRunways( std::set<ALTObject*>& vRunways )
{
	if(vRunways.empty())
		return;

	for (std::set<ALTObject*>::iterator iter = vRunways.begin(); iter != vRunways.end();iter++)
	{

		ALTObject* pObj = (*iter);
		if (pObj == NULL || pObj->GetType() != ALT_RUNWAY)
			continue;

		CString strRunwayName;
		Runway* pRunway = (Runway*)(*iter);
		strRunwayName.Format("%s(%s)", pRunway->GetMarking1().c_str(),pRunway->GetMarking2().c_str());
		HTREEITEM hItem = m_ALTObjTreeCtrl.InsertItem(strRunwayName, m_RunwayItem, TVI_LAST);

		CRouteItem *pRouteItem = new CRouteItem;
		pRouteItem->SetALTObjectID((*iter)->getID());
		pRouteItem->SetIntersectNodeID(-1);
		m_ALTObjTreeCtrl.SetItemData(hItem, (DWORD_PTR)pRouteItem);

		m_ALTObjTreeCtrl.Expand(m_RunwayItem, TVE_EXPAND);
	}
}
void CDlgSelectTowRouteItem::InsertGroundRoutes(std::vector<CTowingRoute *>& vGroundRoutes)
{
	if (vGroundRoutes.empty())
		return;

	for (std::vector<CTowingRoute *>::iterator iter = vGroundRoutes.begin(); iter != vGroundRoutes.end();iter++)
	{
		CString strRouteName;
		strRouteName.Format("%s", (*iter)->GetName());
		HTREEITEM hItem = m_ALTObjTreeCtrl.InsertItem(strRouteName, m_GroundRouteItem, TVI_LAST);

		CRouteItem *pRouteItem = new CRouteItem;
		pRouteItem->SetALTObjectID((*iter)->GetID());
		pRouteItem->SetIntersectNodeID(-1);
		m_ALTObjTreeCtrl.SetItemData(hItem, (DWORD_PTR)pRouteItem);

		m_ALTObjTreeCtrl.Expand(m_GroundRouteItem, TVE_EXPAND);
	}
}

void CDlgSelectTowRouteItem::InsertTaxiwayIntersectItem( Taxiway *pTaxiway, IntersectionNode *pNode, IntersectionNodeList *pNodeList)
{
	ASSERT(pTaxiway);
	ASSERT(pNode);
	ASSERT(pNodeList);

	//the intersect node is more than one
	int nCount = pNodeList->size();

	if (1 < nCount)
	{
		BOOL bExist = IntersectNodeInList(pNode, pNodeList);
		ASSERT(bExist);

		CString strTexiwayName;

		if (m_pParentObj && m_pParentObj->GetType() == ALT_STAND)
		{
			strTexiwayName.Format("%s(%s:%d)", m_pParentObj->GetObjNameString(), pTaxiway->GetMarking().c_str(), pNode->GetIndex());
		}

		if (m_pParentObj && m_pParentObj->GetType() == ALT_TAXIWAY)
		{
			strTexiwayName.Format("%s(%s&%d)", pTaxiway->GetMarking().c_str(), ((Taxiway*)m_pParentObj)->GetMarking().c_str(), pNode->GetIndex());
		}
		else if(m_pParentObj)
		{
			strTexiwayName.Format("%s(%s&%d)", pTaxiway->GetMarking().c_str(), m_pParentObj->GetObjNameString(), pNode->GetIndex());
		}

		HTREEITEM hItem = m_ALTObjTreeCtrl.InsertItem(strTexiwayName, m_TaxiwayItem, TVI_LAST);

		CRouteItem *pRouteItem = new CRouteItem;
		pRouteItem->SetALTObjectID(pTaxiway->getID());
		pRouteItem->SetIntersectNodeID(pNode->GetID());
		m_ALTObjTreeCtrl.SetItemData(hItem, (DWORD_PTR)pRouteItem);

		m_ALTObjTreeCtrl.Expand(m_TaxiwayItem, TVE_EXPAND);
	}
	//only one intersect node
	else if( nCount == 1)
	{
		//if (!pNode->GetID() == pNodeList->at(0).GetID())
		//	return;

		CString strTexiwayName;
		strTexiwayName.Format("%s", pTaxiway->GetMarking().c_str());
		HTREEITEM hItem = m_ALTObjTreeCtrl.InsertItem(strTexiwayName, m_TaxiwayItem, TVI_LAST);

		CRouteItem *pRouteItem = new CRouteItem;
		pRouteItem->SetALTObjectID(pTaxiway->getID());
		pRouteItem->SetIntersectNodeID(pNode->GetID());
		m_ALTObjTreeCtrl.SetItemData(hItem, (DWORD_PTR)pRouteItem);

		m_ALTObjTreeCtrl.Expand(m_TaxiwayItem, TVE_EXPAND);
	}
}
void CDlgSelectTowRouteItem::InsertRunwayIntersectItem( Runway *pRunway, IntersectionNode *pNode, IntersectionNodeList *pNodeList )
{
	if(pRunway == NULL ||pNode == NULL || pNodeList == NULL)
		return;

	//the intersect node is more than one
	int nCount = pNodeList->size();

	if (1 < nCount)
	{
		BOOL bExist = IntersectNodeInList(pNode, pNodeList);

		CString strTexiwayName;

		CString strRunwayMark;
		strRunwayMark.Format(_T("%s(%s)"),pRunway->GetMarking1().c_str(),pRunway->GetMarking2().c_str());



		if (m_pParentObj && m_pParentObj->GetType() == ALT_STAND)
		{
			strTexiwayName.Format("%s(%s:%d)", m_pParentObj->GetObjNameString(), strRunwayMark, pNode->GetIndex());
		}

		if (m_pParentObj && m_pParentObj->GetType() == ALT_TAXIWAY)
		{
			strTexiwayName.Format("%s(%s&%d)", strRunwayMark, ((Taxiway*)m_pParentObj)->GetMarking().c_str(), pNode->GetIndex());
		}
		else if(m_pParentObj)
		{
			strTexiwayName.Format("%s(%s&%d)", strRunwayMark, m_pParentObj->GetObjNameString(), pNode->GetIndex());
		}

		HTREEITEM hItem = m_ALTObjTreeCtrl.InsertItem(strTexiwayName, m_RunwayItem, TVI_LAST);

		CRouteItem *pRouteItem = new CRouteItem;
		pRouteItem->SetALTObjectID(pRunway->getID());
		pRouteItem->SetIntersectNodeID(pNode->GetID());
		m_ALTObjTreeCtrl.SetItemData(hItem, (DWORD_PTR)pRouteItem);

		m_ALTObjTreeCtrl.Expand(m_TaxiwayItem, TVE_EXPAND);
	}
	//only one intersect node
	else if( nCount == 1)
	{
		if (!pNode->GetID() == pNodeList->at(0).GetID())
			return;

		CString strRunwayMark;
		strRunwayMark.Format(_T("%s(%s)"),pRunway->GetMarking1().c_str(),pRunway->GetMarking2().c_str());

		CString strTexiwayName;
		strTexiwayName.Format("%s", strRunwayMark);
		HTREEITEM hItem = m_ALTObjTreeCtrl.InsertItem(strTexiwayName, m_RunwayItem, TVI_LAST);

		CRouteItem *pRouteItem = new CRouteItem;
		pRouteItem->SetALTObjectID(pRunway->getID());
		pRouteItem->SetIntersectNodeID(pNode->GetID());
		m_ALTObjTreeCtrl.SetItemData(hItem, (DWORD_PTR)pRouteItem);

		m_ALTObjTreeCtrl.Expand(m_RunwayItem, TVE_EXPAND);
	}
}

void CDlgSelectTowRouteItem::InsertGroundRouteIntersectItem(CTowingRoute* pGroundRoute, IntersectionNode *pNode, IntersectionNodeList *pNodeList)
{
	if(pGroundRoute == NULL ||pNode == NULL ||pNodeList == NULL)
		return;

	//the intersect node is more than one
	int nCount = pNodeList->size();

	if (nCount >1)
	{
		BOOL bExist = IntersectNodeInList(pNode, pNodeList);

		if(bExist)
		{
			CString strMarkName = pNode->GetName();
			CString strGroundRouteName;		

			strGroundRouteName.Format("%s(%s:%d)", pGroundRoute->GetName(), strMarkName, pNode->GetIndex());

			HTREEITEM hItem = m_ALTObjTreeCtrl.InsertItem(strGroundRouteName, m_GroundRouteItem, TVI_LAST);

			CRouteItem *pIBRItem = new CRouteItem;
			pIBRItem->SetALTObjectID(pGroundRoute->GetID());
			pIBRItem->SetIntersectNodeID(pNode->GetID());
			m_ALTObjTreeCtrl.SetItemData(hItem, (DWORD_PTR)pIBRItem);

			m_ALTObjTreeCtrl.Expand(m_GroundRouteItem, TVE_EXPAND);
		}	
	}
	else				//only one intersect node
	{
		//ASSERT(nCount == 1);
		//ASSERT(pNode->GetID() == pNodeList->at(0).GetID());

		CString strGroundRouteName;
		strGroundRouteName.Format("%s", pGroundRoute->GetName());
		HTREEITEM hItem = m_ALTObjTreeCtrl.InsertItem(strGroundRouteName, m_GroundRouteItem, TVI_LAST);

		CRouteItem *pIBRItem = new CRouteItem;
		pIBRItem->SetALTObjectID(pGroundRoute->GetID());
		pIBRItem->SetIntersectNodeID(pNode->GetID());
		m_ALTObjTreeCtrl.SetItemData(hItem, (DWORD_PTR)pIBRItem);

		m_ALTObjTreeCtrl.Expand(m_GroundRouteItem, TVE_EXPAND);
	}

}

void CDlgSelectTowRouteItem::InsertGroundRouteIntersectItem(CTowingRoute* pGroundRoute, IntersectionNodeList *pNodeList)
{

	if(pGroundRoute == NULL || pNodeList == NULL)
		return;

	int nCount = pNodeList->size();
	if(nCount == 0)
		return;

	if (nCount >1)
	{

		for (int i =0; i< nCount; i++)
		{
			IntersectionNode node = pNodeList->at(i);
			CString strMarkName = node.GetName();
			CString strGroundRouteName;		

			strGroundRouteName.Format("%s(%s:%d)", pGroundRoute->GetName(), strMarkName, node.GetIndex());


			HTREEITEM hItem = m_ALTObjTreeCtrl.InsertItem(strGroundRouteName, m_GroundRouteItem, TVI_LAST);

			CRouteItem *pIBRItem = new CRouteItem;
			pIBRItem->SetALTObjectID(pGroundRoute->GetID());
			pIBRItem->SetIntersectNodeID(node.GetID());
			m_ALTObjTreeCtrl.SetItemData(hItem, (DWORD_PTR)pIBRItem);

			m_ALTObjTreeCtrl.Expand(m_GroundRouteItem, TVE_EXPAND);
		}
	}
	else				//only one intersect node
	{
		CString strGroundRouteName;
		strGroundRouteName.Format("%s", pGroundRoute->GetName());
		HTREEITEM hItem = m_ALTObjTreeCtrl.InsertItem(strGroundRouteName, m_GroundRouteItem, TVI_LAST);

		CRouteItem *pIBRItem = new CRouteItem;
		pIBRItem->SetALTObjectID(pGroundRoute->GetID());
		pIBRItem->SetIntersectNodeID(pNodeList->at(0).GetID());
		m_ALTObjTreeCtrl.SetItemData(hItem, (DWORD_PTR)pIBRItem);

		m_ALTObjTreeCtrl.Expand(m_GroundRouteItem, TVE_EXPAND);
	}

}



BOOL CDlgSelectTowRouteItem::IntersectNodeInList(IntersectionNode *pNode, IntersectionNodeList *pNodeList)
{
	if (pNode == NULL || pNodeList == NULL)
		return FALSE;

	IntersectionNodeList::iterator iter = pNodeList->begin();
	for (; iter!=pNodeList->end(); iter++)
	{
		if (pNode->GetID() == (*iter).GetID())
		{
			return TRUE;
		}
	}

	return FALSE;
}

void CDlgSelectTowRouteItem::ClearListCtrlContent()
{
	HTREEITEM hChildItem = NULL;

	if (NULL != m_TaxiwayItem)
	{
		hChildItem = m_ALTObjTreeCtrl.GetChildItem(m_TaxiwayItem);

		while (NULL != hChildItem)
		{

			CRouteItem *pRouteItem = (CRouteItem *)m_ALTObjTreeCtrl.GetItemData(hChildItem);
			ASSERT(pRouteItem);
			delete pRouteItem;

			hChildItem = m_ALTObjTreeCtrl.GetNextSiblingItem(hChildItem);
		}

	}	

	if (NULL != m_GroundRouteItem)
	{
		hChildItem = m_ALTObjTreeCtrl.GetChildItem(m_GroundRouteItem);

		while (NULL != hChildItem)
		{
			CRouteItem *pRouteItem = (CRouteItem *)m_ALTObjTreeCtrl.GetItemData(hChildItem);
			ASSERT(pRouteItem);
			delete pRouteItem;

			hChildItem = m_ALTObjTreeCtrl.GetNextSiblingItem(hChildItem);
		}
	}
}

void CDlgSelectTowRouteItem::OnDestroy() 
{
	ClearListCtrlContent();
	CDialog::OnDestroy();	
}
void CDlgSelectTowRouteItem::OnTvnSelchangedTreeAltobjlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	HTREEITEM hSelItem = m_ALTObjTreeCtrl.GetSelectedItem();
	if (NULL == hSelItem
		|| NULL == m_ALTObjTreeCtrl.GetParentItem(hSelItem))
	{
		m_strSelALTObjName = "";
		UpdateData(FALSE);
		return;
	}

	CRouteItem *pInboundRouteItem = (CRouteItem*)m_ALTObjTreeCtrl.GetItemData(hSelItem);
	ASSERT(pInboundRouteItem);
	m_nSelALTObjID = pInboundRouteItem->GetALTObjectID();
	m_nIntersectNodeID = pInboundRouteItem->GetIntersectNodeID();

	CString strALTObjName = m_ALTObjTreeCtrl.GetItemText(hSelItem);

	HTREEITEM hParentItem = m_ALTObjTreeCtrl.GetParentItem(hSelItem);
	ASSERT(hParentItem);

	CString strParentItemText = m_ALTObjTreeCtrl.GetItemText(hParentItem);
	if (!strParentItemText.CompareNoCase(_T("Taxiway")))
	{
		m_strSelALTObjName.Format("Taxiway:%s", strALTObjName);
		m_enumItemType = CTowOperationRouteItem::ItemType_Taxiway;
	}
	else if (!strParentItemText.CompareNoCase(_T("Tow Route")))
	{
		m_strSelALTObjName.Format("Route:%s", strALTObjName);
		m_enumItemType = CTowOperationRouteItem::ItemType_TowRoute;
	}
	else if (!strParentItemText.CompareNoCase(_T("Runway")))
	{
		m_strSelALTObjName.Format("Runway:%s", strALTObjName);
		m_enumItemType = CTowOperationRouteItem::ItemType_Runway;
	}
	else
	{
		ASSERT(FALSE);
	}

	UpdateData(FALSE);

	*pResult = 0;
}

void CDlgSelectTowRouteItem::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);

	if (m_strSelALTObjName.IsEmpty())
	{
		AfxMessageBox(_T("Please select an item!"), MB_OK|MB_ICONEXCLAMATION);
		return;
	}

	OnOK();
}

































