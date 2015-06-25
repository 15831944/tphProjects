// SelectInboundRouteItemDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "SelectInboundRouteItemDlg.h"
#include "../InputAirside/RouteItem.h"
#include "../InputAirside/Runway.h"
#include "../InputAirside/Taxiway.h"
#include "../InputAirside/InputAirside.h"
#include "../InputAirside/GroundRoutes.h"
#include "../InputAirside/GroundRoute.h"
#include "../InputAirside/ALTAirport.h"
#include ".\selectinboundrouteitemdlg.h"
#include "../InputAirside/ALTObjectGroup.h"
#include "../Database/DBElementCollection_Impl.h"


// CSelectInboundRouteItemDlg dialog

IMPLEMENT_DYNAMIC(CSelectInboundRouteItemDlg, CDialog)
CSelectInboundRouteItemDlg::CSelectInboundRouteItemDlg(int nProjID, int nID, int nIntersectNodeIDInALTObj, CWnd* pParent /* = NULL */)
	: CDialog(CSelectInboundRouteItemDlg::IDD, pParent)
	, m_nRunwayExitID(-1)
	, m_nSelALTObjID(-1)
	, m_nIntersectNodeID(-1)
	, m_strSelALTObjName("")
	, m_TaxiwayItem(NULL)
	, m_GroundRouteItem(NULL)
	, m_RunwayItem(NULL)
	, m_nProjID(nProjID)
	, m_nIntersectNodeIDInALTObj(nIntersectNodeIDInALTObj)
	, m_pParentObj(NULL)
{
	m_pAirportGroundNetwork = new CAirportGroundNetwork(nProjID);
	if (nID >=0)
	{
		m_pParentObj =  ALTObject::ReadObjectByID(nID);
	}
}

CSelectInboundRouteItemDlg::~CSelectInboundRouteItemDlg()
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

void CSelectInboundRouteItemDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_ALTOBJNAME, m_strSelALTObjName);
	DDX_Control(pDX, IDC_TREE_ALTOBJLIST, m_ALTObjTreeCtrl);
}


BEGIN_MESSAGE_MAP(CSelectInboundRouteItemDlg, CDialog)
	ON_WM_DESTROY() 
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_ALTOBJLIST, OnTvnSelchangedTreeAltobjlist)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


 //CSelectInboundRouteItemDlg message handlers

BOOL CSelectInboundRouteItemDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	UpdateData(FALSE);

	SetListCtrlContent();

	return TRUE;
}

int CSelectInboundRouteItemDlg::GetSelALTObjID()
{
	return m_nSelALTObjID;
}

void CSelectInboundRouteItemDlg::SetSelALTObjID(int nSelALTObjID)
{
	m_nSelALTObjID = nSelALTObjID;
}

int CSelectInboundRouteItemDlg::GetIntersectNodeID()
{
	return m_nIntersectNodeID;
}

void CSelectInboundRouteItemDlg::SetIntersectNodeID(int nIntersectNodeID)
{
	m_nIntersectNodeID = nIntersectNodeID;
}

CString CSelectInboundRouteItemDlg::GetSelALTObjName()
{
	return m_strSelALTObjName;
}

void CSelectInboundRouteItemDlg::SetSelALTObjName(CString strALTObjName)
{
	m_strSelALTObjName = strALTObjName;
}

void CSelectInboundRouteItemDlg::SetListCtrlContent()
{
	m_TaxiwayItem     = m_ALTObjTreeCtrl.InsertItem(_T("Taxiway"));
	m_RunwayItem = m_ALTObjTreeCtrl.InsertItem(_T("Runway"));
	m_GroundRouteItem = m_ALTObjTreeCtrl.InsertItem(_T("Ground Route"));



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

//void CSelectInboundRouteItemDlg::SetALTObjGroupIntersectTaxiway(ALTObjectGroup *pALTObjGroup)
//{
//	ASSERT(pALTObjGroup);
//
//	switch(pALTObjGroup->getType())
//	{
//	case ALT_STAND:
//		{
//			IntersectionNodeList taxiwayINodeList;
//			vector<int> vIntersectNodeStandID;
//			m_pAirportGroundNetwork.GetStandGroupTaxiwayIntersectNodeList(pALTObjGroup, &taxiwayINodeList, &vIntersectNodeStandID);
//
//			for (int i=0; i<(int)taxiwayINodeList.size(); i++)
//			{
//				IntersectionNode twINode = taxiwayINodeList.at(i);
//				Taxiway * pTaxiway = NULL;
//				std::vector<TaxiwayIntersectItem*> vTaxiItems = twINode.GetTaxiwayIntersectItemList();
//				if( vTaxiItems.size() )
//				{
//					pTaxiway = (*vTaxiItems.rbegin())->GetTaxiway();
//				}
//				else
//				{
//					ASSERT(FALSE);
//				}
//
//				InsertTaxiwayIntersectItem(vIntersectNodeStandID[i], pTaxiway, &twINode, &taxiwayINodeList);
//			}
//		}
//		break;
//
//	default:
//		ASSERT(FALSE);
//	    break;
//	}
//}

//void CSelectInboundRouteItemDlg::SetALTObjGroupIntersectGroundRoute(ALTObjectGroup *pALTObjGroup)
//{
//	ASSERT(pALTObjGroup);
//
//	switch(pALTObjGroup->getType())
//	{
//	case ALT_STAND:
//		{
//			IntersectionNodeList taxiwayINodeList;
//			vector<int> vStandID;
//			m_pAirportGroundNetwork.GetStandGroupTaxiwayIntersectNodeList(pALTObjGroup, &taxiwayINodeList, &vStandID);
//			std::vector<int> vAirportIds;
//			InputAirside::GetAirportList(m_nProjID, vAirportIds);
//			for (std::vector<int>::iterator iterAirportID = vAirportIds.begin(); iterAirportID != vAirportIds.end(); ++iterAirportID)
//			{
//				GroundRoutes groundRoutes(*iterAirportID);
//				groundRoutes.ReadData();
//
//				GroundRouteVector grVector;
//				m_pAirportGroundNetwork.GetStandGroupIntersectGroundRoutes(pALTObjGroup, &groundRoutes, &grVector);
//
//				IntersectionNodeList::iterator iter = taxiwayINodeList.begin();
//				for (; iter!=taxiwayINodeList.end(); iter++)
//				{
//					IntersectionNode twINode = *iter;
//
//					GroundRouteVector::iterator grIter = grVector.begin();
//					for (; grIter!=grVector.end(); grIter++)
//					{
//						IntersectionNodeList grNodeList;
//						m_pAirportGroundNetwork.GetStandGroupGroundRouteIntersectNodeList(pALTObjGroup, *grIter, &grNodeList);
//						ASSERT(1 <= grNodeList.size());						
//
//						InsertGroundRouteIntersectItem(*grIter, &twINode, &grNodeList);
//					}
//				}
//			}
//		}
//		break;
//
//	default:
//		ASSERT(FALSE);
//		break;
//	}
//}

void CSelectInboundRouteItemDlg::SetALTObjIntersectTaxiway(ALTObject *pALTObj)
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
				GroundRoute *pGR = (GroundRoute*)pALTObj;
				SetGroundRouteIntersectTaxiway(pGR);
			}
			break;

		default:
			break;
		}
	}


}

void CSelectInboundRouteItemDlg::SetALTObjIntersectGroundRoute(ALTObject *pALTObj)
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
				GroundRoute *pGR = (GroundRoute*)pALTObj;
				SetGroundRouteIntersectGroundRoute(pGR);
			}
			break;

		default:
			break;
		}
	}


}

void CSelectInboundRouteItemDlg::SetRunwayIntersectTaxiway(Runway *pRunway)
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

void CSelectInboundRouteItemDlg::SetTaxiwayIntersectTaxiway(Taxiway *pTaxiway)
{
	//ASSERT(pTaxiway);

	//IntersectionNodeList taxiwayINodeList = pTaxiway->GetIntersectNodes();

	//IntersectionNodeList::iterator twINodeListIter = taxiwayINodeList.begin();
	//for (; twINodeListIter!=taxiwayINodeList.end(); twINodeListIter++)
	//{

	//	std::vector<TaxiwayIntersectItem*> vTaxiwayItems = twINodeListIter->GetTaxiwayIntersectItemList();
	//	for(int i=0;i<(int)vTaxiwayItems.size();i++)
	//	{
	//		TaxiwayIntersectItem* taxiItem = vTaxiwayItems[i];
	//		Taxiway* pOtherTaxiway = taxiItem->GetTaxiway();

			//if(pOtherTaxiway && pOtherTaxiway->getID()!= pTaxiway->getID())
			//{
			//	IntersectionNodeList otherTwNodeList;
			//	m_pAirportGroundNetwork->GetTaxiwayTaxiwayIntersectNodeList(pTaxiway, pOtherTaxiway, &otherTwNodeList);

			//	InsertTaxiwayIntersectItem( pOtherTaxiway, &(*twINodeListIter), &otherTwNodeList);
			//}
	//	}		
	//}
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
			if (pObj->GetType() != ALT_RUNWAY)
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

void CSelectInboundRouteItemDlg::SetGroundRouteIntersectTaxiway(GroundRoute *pGroundRoute)
{
	//std::vector<int> vAirportIds;
	//InputAirside::GetAirportList(m_nProjID, vAirportIds);
	//for (std::vector<int>::iterator iterAirportID = vAirportIds.begin(); iterAirportID != vAirportIds.end(); ++iterAirportID)
	//{
	//	std::vector<int> vTaxiwayIDList;
	//	ALTAirport::GetTaxiwaysIDs(*iterAirportID, vTaxiwayIDList);
	//	for (std::vector<int>::iterator iterTwID = vTaxiwayIDList.begin(); iterTwID != vTaxiwayIDList.end(); ++iterTwID)
	//	{
	//		Taxiway taxiway;
	//		taxiway.ReadObject(*iterTwID);
	
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

 		m_pAirportGroundNetwork->GetGroundRouteTaxiwayIntersectNodeList(pGroundRoute,*iterTaxiway , &twINodeList);

		if (twINodeList.empty())
			continue;

		for (int i=0; i<(int)twINodeList.size(); i++)
		{
			InsertTaxiwayIntersectItem((Taxiway*)(*iterTaxiway), &twINodeList[i], &twINodeList);
		}
	}

	for (std::set<ALTObject*>::iterator iterRunway = vTaxiways.begin(); iterRunway != vTaxiways.end(); ++iterRunway)
	{
		ALTObject* pObj = (*iterRunway);
		if (pObj->GetType() != ALT_RUNWAY)
			continue;

		IntersectionNodeList twINodeList;

		m_pAirportGroundNetwork->GetGroundRouteTaxiwayIntersectNodeList(pGroundRoute,*iterRunway , &twINodeList);

		if (twINodeList.empty())
			continue;

		for (int i=0; i<(int)twINodeList.size(); i++)
		{
			InsertRunwayIntersectItem((Runway*)(*iterRunway), &twINodeList[i], &twINodeList);
		}
	}
	//	}
	//}
}

void CSelectInboundRouteItemDlg::SetRunwayIntersectGroundRoute(Runway *pRunway)
{
	ASSERT(pRunway);
	GroundRouteVector vGroundRoutes;
	vGroundRoutes.clear();
	m_pAirportGroundNetwork->GetRunwayIntersectGroundRoutes(pRunway,RUNWAYMARK_FIRST,&vGroundRoutes);

	GroundRouteVector::iterator grIter = vGroundRoutes.begin();
	for (; grIter!=vGroundRoutes.end(); grIter++)
	{
		IntersectionNodeList grNodeList;
		m_pAirportGroundNetwork->GetRunwayGroundRouteIntersectNodeList(pRunway, *grIter, &grNodeList);
		//ASSERT(1 <= grNodeList.size());

		InsertGroundRouteIntersectItem(*grIter,&grNodeList);
	}

	//RunwayExitList rwExitList;
	//pRunway->GetExitList(RUNWAYMARK_FIRST, rwExitList);

	//RunwayExitList::iterator rwelIter = rwExitList.begin();
	//for (; rwelIter!=rwExitList.end(); rwelIter++)
	//{
	//	RunwayExit rwExit = *rwelIter;
	//	int nTaxiwayID = rwExit.GetTaxiwayID();

	//	Taxiway taxiway;
	//	taxiway.ReadObject(nTaxiwayID);

	//	SetTaxiwayIntersectGroundRoute(&taxiway);
	//}
}



void CSelectInboundRouteItemDlg::SetTaxiwayIntersectGroundRoute(Taxiway *pTaxiway)
{
	//ASSERT(pTaxiway);



	//std::vector<int> vAirportIds;
	//InputAirside::GetAirportList(m_nProjID, vAirportIds);
	//for (std::vector<int>::iterator iterAirportID = vAirportIds.begin(); iterAirportID != vAirportIds.end(); ++iterAirportID)
	//{

	GroundRouteVector grVector;
	grVector.clear();
	if (pTaxiway == NULL)
	{
		m_pAirportGroundNetwork->GetIntersectedGroundRoutes((ALTObject*)pTaxiway,grVector);
		InsertGroundRoutes(grVector);
		return;
	}

	m_pAirportGroundNetwork->GetTaxiwayIntersectGroundRoutes(pTaxiway,  &grVector);

	GroundRouteVector::iterator grIter = grVector.begin();
	for (; grIter!=grVector.end(); grIter++)
	{
		IntersectionNodeList grNodeList;
		m_pAirportGroundNetwork->GetTaxiwayGroundRouteIntersectNodeList(pTaxiway, *grIter, &grNodeList);
		ASSERT(1 <= grNodeList.size());

		InsertGroundRouteIntersectItem(*grIter,&grNodeList);
	}

	//}
}

void CSelectInboundRouteItemDlg::SetGroundRouteIntersectGroundRoute(GroundRoute *pGroundRoute)
{
	if (pGroundRoute == NULL)
		return;

	//std::vector<int> vAirportIds;
	//InputAirside::GetAirportList(m_nProjID, vAirportIds);
	//for (std::vector<int>::iterator iterAirportID = vAirportIds.begin(); iterAirportID != vAirportIds.end(); ++iterAirportID)
	//{
		GroundRouteVector grVector;
		m_pAirportGroundNetwork->GetGroundRouteIntersectGroundRoutes(pGroundRoute, &grVector);

		GroundRouteVector::iterator grIter = grVector.begin();
		for (; grIter!=grVector.end(); grIter++)
		{
			IntersectionNodeList grNodeList;
			m_pAirportGroundNetwork->GetGroundRouteGroundRouteIntersectNodeList(pGroundRoute, *grIter, &grNodeList);

			//if no intersect node
			if (grNodeList.empty())
				continue;

			InsertGroundRouteIntersectItem(*grIter, &grNodeList);

		}
	//}
}

//void CSelectInboundRouteItemDlg::GetValidNodeIntersectWithGroundRoute(GroundRoute* pGroundRoute, 
//																	  int nIntersectNodeIDInGR, IntersectionNodeList& iNodeList)
//{
//	ASSERT(pGroundRoute);
//
//	GroundRouteItem *pGRItem = pGroundRoute->GetFirstItem();
//	ASSERT(pGRItem);
//
//	GroundRouteItem *pGRItemWithTheNode = pGroundRoute->GetItemWithGivenIntersectNodeID(nIntersectNodeIDInGR);
//	ASSERT(pGRItemWithTheNode);
//
//	GetValidNodeIntersectWithGRItem(pGRItemWithTheNode, nIntersectNodeIDInGR, iNodeList);
//}
//
//void CSelectInboundRouteItemDlg::GetValidNodeIntersectWithGRItem(GroundRouteItem* pGRItem, int nIntersectNodeIDInGR, IntersectionNodeList& iNodeList)
//{
//	ASSERT(pGRItem);
//
//	for (IntersectionNodeList::iterator iter=iNodeList.begin(); iter!=iNodeList.end(); iter++)
//	{
//		//if node not intersect with ground route item,remove it
//		//if (!IsNodeIntersectWithGRItem(pGRItem, nIntersectNodeIDInGR, *iter))
//		{
//			iNodeList.erase(iter);
//			iter--;
//		}
//	}
//}
//
//BOOL CSelectInboundRouteItemDlg::IsNodeIntersectWithGRItem(GroundRouteItem* pGRItem, int nIntersectNodeIDInGR, IntersectionNode& iNode)
//{
//	ASSERT(pGRItem);
//
//	ALTObject::RefPtr pALTObj = ALTObject::ReadObjectByID(pGRItem->GetALTObjectID());
//
//	IntersectionNodeList iNodeList;
//	switch(pALTObj->GetType())
//	{
//	case ALT_TAXIWAY:
//		{
//			Taxiway *pTw = (Taxiway*)pALTObj.get();
//			iNodeList = pTw->GetIntersectNodes();
//
//			for (int i=0; i<(int)iNodeList.size(); i++)
//			{
//				if (iNodeList.at(i).GetID() == nIntersectNodeIDInGR)
//				{
//					continue;
//				}
//
//				//IntersectNode equalNode;
//				//BOOL bExist = m_pAirportGroundNetwork.FindEqualNode(pTw, &iNodeList.at(i), equalNode);
//				//if the equalNode is stand in/out node
//			/*	if (!bExist)
//				{
//					continue;
//				}*/
//
//				if (equalNode.m_nUniqueID == iNode.m_nUniqueID)
//				{
//					delete pALTObj;
//					return TRUE;
//				}
//			}
//		}
//		break;
//
//	case ALT_RUNWAY:
//		{
//			Runway *pRw = (Runway*)pALTObj;
//			iNodeList = pRw->GetIntersectNodes();
//
//			for (int i=0; i<(int)iNodeList.size(); i++)
//			{
//				if (iNodeList.at(i).m_nUniqueID == nIntersectNodeIDInGR)
//				{
//					continue;
//				}
//
//				IntersectNode equalNode;
//				BOOL bExist = m_pAirportGroundNetwork.FindEqualNode(pRw, &iNodeList.at(i), equalNode);
//				ASSERT(bExist);
//
//				if (equalNode.m_nUniqueID == iNode.m_nUniqueID)
//				{
//					delete pALTObj;
//					return TRUE;
//				}
//			}
//		}
//	    break;
//
//	default:
//		ASSERT(FALSE);
//	    break;
//	}
//
//	for (int i=0; i<pGRItem->GetChildCount(); i++)
//	{
//		GroundRouteItem *pChildItem = pGRItem->GetChildItem(i);
//
//		BOOL bRet = IsNodeIntersectWithGRItem(pChildItem, nIntersectNodeIDInGR, iNode);
//		if (bRet)
//		{
//			delete pALTObj;
//			return TRUE;
//		}
//	}
//
//	delete pALTObj;
//	return FALSE;
//}
//


//void CSelectInboundRouteItemDlg::SetRunwayExitIntersectTaxiway(RunwayExit *pRunwayExit)
//{
//	ASSERT(pRunwayExit);
//
//	int nRunwayID = pRunwayExit->GetRunwayID();
//
//	int nTaxiwayID = pRunwayExit->GetTaxiwayID();
//	Taxiway taxiway;
//	taxiway.ReadObject(nTaxiwayID);
//
//	IntersectionNodeList taxiwayINodeList;
//	IntersectionNode::ReadIntersectNodeList(nRunwayID,nTaxiwayID,taxiwayINodeList);
//	ASSERT(1 <= taxiwayINodeList.size());
//
//	Runway runway;
//	runway.ReadObject(nRunwayID);
//
//	InsertTaxiwayIntersectItem(runway.getID(), &taxiway, &pRunwayExit->GetIntesectionNode(), &taxiwayINodeList);	
//}

void CSelectInboundRouteItemDlg::InsertTaxiways(std::set<ALTObject*>& vTaxiways)
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
void CSelectInboundRouteItemDlg::InsertRunways( std::set<ALTObject*>& vRunways )
{
	if(vRunways.empty())
		return;

	for (std::set<ALTObject*>::iterator iter = vRunways.begin(); iter != vRunways.end();iter++)
	{

		ALTObject* pObj = (*iter);
		if (pObj->GetType() != ALT_RUNWAY)
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
void CSelectInboundRouteItemDlg::InsertGroundRoutes(GroundRouteVector& vGroundRoutes)
{
	if (vGroundRoutes.empty())
		return;

	for (GroundRouteVector::iterator iter = vGroundRoutes.begin(); iter != vGroundRoutes.end();iter++)
	{
		CString strRouteName;
		strRouteName.Format("%s", (*iter)->GetObjNameString());
		HTREEITEM hItem = m_ALTObjTreeCtrl.InsertItem(strRouteName, m_GroundRouteItem, TVI_LAST);

		CRouteItem *pRouteItem = new CRouteItem;
		pRouteItem->SetALTObjectID((*iter)->getID());
		pRouteItem->SetIntersectNodeID(-1);
		m_ALTObjTreeCtrl.SetItemData(hItem, (DWORD_PTR)pRouteItem);

		m_ALTObjTreeCtrl.Expand(m_GroundRouteItem, TVE_EXPAND);
	}
}

void CSelectInboundRouteItemDlg::InsertTaxiwayIntersectItem( Taxiway *pTaxiway, IntersectionNode *pNode, IntersectionNodeList *pNodeList)
{
	ASSERT(pTaxiway);
	ASSERT(pNode);
	ASSERT(pNodeList);

	//vector<ALTObject*> pObjectList = pNode->GetOtherObjectList(pTaxiway->getID());
	//vector<ALTObject*>::iterator iter = pObjectList.begin();
	//for (; iter!= pObjectList.end(); iter++)
	//{
	//	if ((*iter)->getID() == m_nALTObjID)
	//	{
	//		pParentObj = *iter;
	//		break;
	//	}		
	//}

	//ASSERT(NULL != pParentObj);

	//the intersect node is more than one
	int nCount = pNodeList->size();

	if (1 < nCount)
	{
		BOOL bExist = IntersectNodeInList(pNode, pNodeList);
		ASSERT(bExist);

		CString strTexiwayName;

		if (m_pParentObj->GetType() == ALT_STAND)
		{
			strTexiwayName.Format("%s(%s:%d)", m_pParentObj->GetObjNameString(), pTaxiway->GetMarking().c_str(), pNode->GetIndex());
		}

		if (m_pParentObj->GetType() == ALT_TAXIWAY)
		{
			strTexiwayName.Format("%s(%s&%d)", pTaxiway->GetMarking().c_str(), ((Taxiway*)m_pParentObj)->GetMarking().c_str(), pNode->GetIndex());
		}
		else
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
		if (!pNode->GetID() == pNodeList->at(0).GetID())
			return;

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
void CSelectInboundRouteItemDlg::InsertRunwayIntersectItem( Runway *pRunway, IntersectionNode *pNode, IntersectionNodeList *pNodeList )
{
	ASSERT(pRunway);
	ASSERT(pNode);
	ASSERT(pNodeList);

	//vector<ALTObject*> pObjectList = pNode->GetOtherObjectList(pTaxiway->getID());
	//vector<ALTObject*>::iterator iter = pObjectList.begin();
	//for (; iter!= pObjectList.end(); iter++)
	//{
	//	if ((*iter)->getID() == m_nALTObjID)
	//	{
	//		pParentObj = *iter;
	//		break;
	//	}		
	//}

	//ASSERT(NULL != pParentObj);

	//the intersect node is more than one
	int nCount = pNodeList->size();

	if (1 < nCount)
	{
		BOOL bExist = IntersectNodeInList(pNode, pNodeList);
		ASSERT(bExist);

		CString strTexiwayName;

		CString strRunwayMark;
		strRunwayMark.Format(_T("%s(%s)"),pRunway->GetMarking1().c_str(),pRunway->GetMarking2().c_str());



		if (m_pParentObj->GetType() == ALT_STAND)
		{
			strTexiwayName.Format("%s(%s:%d)", m_pParentObj->GetObjNameString(), strRunwayMark, pNode->GetIndex());
		}

		if (m_pParentObj->GetType() == ALT_TAXIWAY)
		{
			strTexiwayName.Format("%s(%s&%d)", strRunwayMark, ((Taxiway*)m_pParentObj)->GetMarking().c_str(), pNode->GetIndex());
		}
		else
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

void CSelectInboundRouteItemDlg::InsertGroundRouteIntersectItem(GroundRoute* pGroundRoute, IntersectionNode *pNode, IntersectionNodeList *pNodeList)
{
	ASSERT(pGroundRoute);
	ASSERT(pNode);
	ASSERT(pNodeList);

	//the intersect node is more than one
	int nCount = pNodeList->size();

	if (nCount >1)
	{
		BOOL bExist = IntersectNodeInList(pNode, pNodeList);

		if(bExist)
		{
			CString strMarkName = pNode->GetName();
			CString strGroundRouteName;		

			strGroundRouteName.Format("%s(%s:%d)", pGroundRoute->GetObjNameString(), strMarkName, pNode->GetIndex());

			HTREEITEM hItem = m_ALTObjTreeCtrl.InsertItem(strGroundRouteName, m_GroundRouteItem, TVI_LAST);

			CRouteItem *pIBRItem = new CRouteItem;
			pIBRItem->SetALTObjectID(pGroundRoute->getID());
			pIBRItem->SetIntersectNodeID(pNode->GetID());
			m_ALTObjTreeCtrl.SetItemData(hItem, (DWORD_PTR)pIBRItem);

			m_ALTObjTreeCtrl.Expand(m_GroundRouteItem, TVE_EXPAND);
		}	
	}
	else				//only one intersect node
	{
		ASSERT(nCount == 1);
		//ASSERT(pNode->GetID() == pNodeList->at(0).GetID());

		CString strGroundRouteName;
		strGroundRouteName.Format("%s", pGroundRoute->GetObjNameString());
		HTREEITEM hItem = m_ALTObjTreeCtrl.InsertItem(strGroundRouteName, m_GroundRouteItem, TVI_LAST);

		CRouteItem *pIBRItem = new CRouteItem;
		pIBRItem->SetALTObjectID(pGroundRoute->getID());
		pIBRItem->SetIntersectNodeID(pNode->GetID());
		m_ALTObjTreeCtrl.SetItemData(hItem, (DWORD_PTR)pIBRItem);

		m_ALTObjTreeCtrl.Expand(m_GroundRouteItem, TVE_EXPAND);
	}

}

void CSelectInboundRouteItemDlg::InsertGroundRouteIntersectItem(GroundRoute* pGroundRoute, IntersectionNodeList *pNodeList)
{
	ASSERT(pGroundRoute);
	ASSERT(pNodeList);

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

			strGroundRouteName.Format("%s(%s:%d)", pGroundRoute->GetObjNameString(), strMarkName, node.GetIndex());


			HTREEITEM hItem = m_ALTObjTreeCtrl.InsertItem(strGroundRouteName, m_GroundRouteItem, TVI_LAST);

			CRouteItem *pIBRItem = new CRouteItem;
			pIBRItem->SetALTObjectID(pGroundRoute->getID());
			pIBRItem->SetIntersectNodeID(node.GetID());
			m_ALTObjTreeCtrl.SetItemData(hItem, (DWORD_PTR)pIBRItem);

			m_ALTObjTreeCtrl.Expand(m_GroundRouteItem, TVE_EXPAND);
		}
	}
	else				//only one intersect node
	{
		CString strGroundRouteName;
		strGroundRouteName.Format("%s", pGroundRoute->GetObjNameString());
		HTREEITEM hItem = m_ALTObjTreeCtrl.InsertItem(strGroundRouteName, m_GroundRouteItem, TVI_LAST);

		CRouteItem *pIBRItem = new CRouteItem;
		pIBRItem->SetALTObjectID(pGroundRoute->getID());
		pIBRItem->SetIntersectNodeID(pNodeList->at(0).GetID());
		m_ALTObjTreeCtrl.SetItemData(hItem, (DWORD_PTR)pIBRItem);

		m_ALTObjTreeCtrl.Expand(m_GroundRouteItem, TVE_EXPAND);
	}

}



//void CSelectInboundRouteItemDlg::SetRunwayExitIntersectGroundRoute(RunwayExit *pRunwayExit)
//{
//	ASSERT(pRunwayExit);
//
//	int nRunwayID = pRunwayExit->GetRunwayID();
//	Runway runway;
//	runway.ReadObject(nRunwayID);
//
//	int nTaxiwayID = pRunwayExit->GetTaxiwayID();
//	Taxiway taxiway;
//	taxiway.ReadObject(nTaxiwayID);
//
//	std::vector<int> vAirportIds;
//	InputAirside::GetAirportList(m_nProjID, vAirportIds);
//	for (std::vector<int>::iterator iterAirportID = vAirportIds.begin(); iterAirportID != vAirportIds.end(); ++iterAirportID)
//	{
//		GroundRoutes groundRoutes(*iterAirportID);
//		groundRoutes.ReadData();		
//
//		GroundRouteVector grVector;
//		m_pAirportGroundNetwork.GetRunwayIntersectGroundRoutes(&runway, pRunwayExit->GetRunwayMark(), &groundRoutes, &grVector);
//
//		GroundRouteVector::iterator grIter = grVector.begin();
//		for (; grIter!=grVector.end(); grIter++)
//		{
//			IntersectionNodeList grNodeList;
//			m_pAirportGroundNetwork.GetRunwayGroundRouteIntersectNodeList(&runway, *grIter, &grNodeList);
//			
//
//			InsertGroundRouteIntersectItem(*grIter, &pRunwayExit->GetIntesectionNode(), &grNodeList);
//		}
//	}
//}


BOOL CSelectInboundRouteItemDlg::IntersectNodeInList(IntersectionNode *pNode, IntersectionNodeList *pNodeList)
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

void CSelectInboundRouteItemDlg::ClearListCtrlContent()
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

void CSelectInboundRouteItemDlg::OnDestroy() 
{
	ClearListCtrlContent();
	CDialog::OnDestroy();	
}
void CSelectInboundRouteItemDlg::OnTvnSelchangedTreeAltobjlist(NMHDR *pNMHDR, LRESULT *pResult)
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
	}
	else if (!strParentItemText.CompareNoCase(_T("Ground Route")))
	{
		m_strSelALTObjName.Format("Route:%s", strALTObjName);
	}
	else if (!strParentItemText.CompareNoCase(_T("Runway")))
	{
		m_strSelALTObjName.Format("Runway:%s", strALTObjName);
	}
	else
	{
		ASSERT(FALSE);
	}

	UpdateData(FALSE);

	*pResult = 0;
}

void CSelectInboundRouteItemDlg::OnBnClickedOk()
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


