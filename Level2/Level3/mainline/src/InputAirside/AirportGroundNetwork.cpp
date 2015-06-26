#include "StdAfx.h"
#include ".\airportgroundnetwork.h"
#include "Runway.h"
#include "Taxiway.h"
#include "GroundRoutes.h"
#include "GroundRoute.h"
#include "ALTObject.h"
#include "ALTObjectGroup.h"
#include "common\ALTObjectID.h"
#include "InputAirside.h"
#include "ALTAirport.h"
#include "stand.h"
#include <iostream>
#include <vector>
using namespace std;

CAirportGroundNetwork::CAirportGroundNetwork(int nPrjID)
:m_pGroundRoutes(NULL)
{
	m_vStands.clear();
	m_vTaxiways.clear();

	std::vector<int> vAirportIds;
	InputAirside::GetAirportList(nPrjID, vAirportIds);
	int nAirportID = vAirportIds.at(0);

	GetStandList(nAirportID);
	GetTaxiwayList(nAirportID);
	GetRunwayList(nAirportID);
	
	m_pGroundRoutes = new GroundRoutes(nAirportID);
	m_pGroundRoutes->ReadData();

	m_vIntersectionNodeList.ReadData(nAirportID);

}

CAirportGroundNetwork::~CAirportGroundNetwork(void)
{
	if (m_pGroundRoutes)
	{
		delete m_pGroundRoutes;
		m_pGroundRoutes = NULL;
	}
}

void CAirportGroundNetwork::GetStandList(int nAirportID)
{
	if (!m_vStands.empty())
		return;

	ALTAirport::GetStandList(nAirportID,m_vStands);

}

void CAirportGroundNetwork::GetTaxiwayList(int nAirportID)
{
	if (!m_vTaxiways.empty())
		return;

	ALTAirport::GetTaxiwayList(nAirportID,m_vTaxiways);

}
void CAirportGroundNetwork::GetRunwayList( int nAirportID )
{
	if (!m_vRunways.empty())
		return;

	ALTAirport::GetRunwayList(nAirportID,m_vRunways);
}

ALTObject* CAirportGroundNetwork::GetTaxiwayByID(int nID)
{
	size_t nCount = m_vTaxiways.size();
	for(size_t i =0; i < nCount; i++)
	{
		ALTObject* pObj = m_vTaxiways.at(i).get();
		if (pObj->getID() == nID)
			return pObj;
	}
	return NULL;
}


ALTObject* CAirportGroundNetwork::GetRunwayByID( int nID )
{
	size_t nCount = m_vRunways.size();
	for(size_t i =0; i < nCount; i++)
	{
		ALTObject* pObj = m_vRunways.at(i).get();
		if (pObj->getID() == nID)
			return pObj;
	}
	return NULL;
}
void CAirportGroundNetwork::GetIntersectedTaxiwayRunway(ALTObject* pAltObject, std::set<ALTObject*>& vTaxiwayList, std::set<ALTObject*>& vRunwayList)
{

	if (pAltObject == NULL)
	{
		size_t nTaxiwayCount = m_vTaxiways.size();
		for (size_t i =0; i < nTaxiwayCount; i++)
		{
			ALTObject* pObj = m_vTaxiways.at(i).get();
			vTaxiwayList.insert(pObj);
		}
		size_t nRunwayCount = m_vRunways.size();
		for (size_t j =0; j < nRunwayCount; j++)
		{
			ALTObject* pObj = m_vRunways.at(j).get();
			vRunwayList.insert(pObj);
		}


	}
	else
	{
		if (pAltObject->GetType() == ALT_TAXIWAY)
		{
			IntersectionNodeList vNodeList;
			vNodeList.clear();
			m_vIntersectionNodeList.GetIntersectionNodeOf(((Taxiway*)pAltObject)->getID(),vNodeList);

			size_t nCount = vNodeList.size();
			for (size_t i =0; i < nCount; i++)
			{
				std::vector<ALTObject*> vTaxiways = vNodeList[i].GetOtherObjectList(((Taxiway*)pAltObject)->getID());
				for (int j =0; j < (int)vTaxiways.size(); j++)
				{
					ALTObject* pObj = vTaxiways[j];

					ASSERT(pObj);
					if (pObj->GetType() == ALT_TAXIWAY)
					{
						ALTObject* pTaxiway = GetTaxiwayByID(pObj->getID());
						if (pTaxiway)
							vTaxiwayList.insert(pTaxiway);
					}
					else if(pObj->GetType() == ALT_RUNWAY)
					{
						ALTObject* pRunway = GetRunwayByID(pObj->getID());
						if (pRunway)
							vRunwayList.insert(pRunway);
					}
				}
			}
		}
		else if(pAltObject->GetType() == ALT_RUNWAY)
		{
			IntersectionNodeList vNodeList;
			vNodeList.clear();
			m_vIntersectionNodeList.GetIntersectionNodeOf(((Runway*)pAltObject)->getID(),vNodeList);

			size_t nCount = vNodeList.size();
			for (size_t i =0; i < nCount; i++)
			{
				std::vector<ALTObject*> vTaxiways = vNodeList[i].GetOtherObjectList(((Runway*)pAltObject)->getID());
				for (int j =0; j < (int)vTaxiways.size(); j++)
				{
					ALTObject* pObj = vTaxiways[j];

					ASSERT(pObj);
					if (pObj->GetType() == ALT_TAXIWAY)
					{
						ALTObject* pTaxiway = GetTaxiwayByID(pObj->getID());
						if (pTaxiway)
							vTaxiwayList.insert(pTaxiway);
					}
					else if(pObj->GetType() == ALT_RUNWAY)
					{
						ALTObject* pRunway = GetRunwayByID(pObj->getID());
						if (pRunway)
							vRunwayList.insert(pRunway);
					}
				}
			}

		}
		else
		{
			GroundRouteItemVector vGoundRouteItems = ((GroundRoute*)pAltObject)->GetExitItems();
			size_t nCount = vGoundRouteItems.size();
			for (size_t i = 0; i < nCount; i++)
			{
				GroundRouteItem* pItem = vGoundRouteItems.at(i);
				int nAltID = pItem->GetALTObjectID();
				ALTObject *pALTObj = ALTObject::ReadObjectByID(nAltID);
				ASSERT(pALTObj);

				IntersectionNodeList vNodeList;
				vNodeList.clear();
				switch(pALTObj->GetType())
				{
					case ALT_TAXIWAY:
						{
							Taxiway *pTw = (Taxiway*)pALTObj;
							m_vIntersectionNodeList.GetIntersectionNodeOf(pTw->getID(),vNodeList);
						}
						break;

					case ALT_RUNWAY:
						{
							Runway *pRw = (Runway*)pALTObj;
							m_vIntersectionNodeList.GetIntersectionNodeOf(pRw->getID(),vNodeList);
						}
						break;

					default:
						ASSERT(FALSE);
						break;
				}
				size_t nCount = vNodeList.size();
				for (size_t i =0; i < nCount; i++)
				{
					std::vector<ALTObject*> vTaxiways = vNodeList[i].GetOtherObjectList(pALTObj->getID());
					for (int j =0; j < (int)vTaxiways.size(); j++)
					{
						ALTObject* pObj = vTaxiways[j];

						ASSERT(pObj);
						if (pObj->GetType() == ALT_TAXIWAY)
						{
							ALTObject* pTaxiway = GetTaxiwayByID(pObj->getID());
							if (pTaxiway)
								vTaxiwayList.insert(pTaxiway);
						}
						if(pObj->GetType() == ALT_RUNWAY)
						{
							ALTObject* pTaxiway = GetRunwayByID(pObj->getID());
							if (pTaxiway)
								vTaxiwayList.insert(pTaxiway);
						}
					}
				}

				delete pALTObj;
				pALTObj = NULL;
			}
		}

	}
}

void CAirportGroundNetwork::GetIntersectedGroundRoutes(ALTObject* pAltObject,GroundRouteVector& vGroundRouteList)
{
	if (pAltObject == NULL)
	{
		int nCount = m_pGroundRoutes->GetCount();
		for (int i =0; i < nCount; i++)
		{
			GroundRoute* pRoute = m_pGroundRoutes->GetItem(i);
			vGroundRouteList.push_back(pRoute);
		}
	}
	else
	{
		if (pAltObject->GetType() == ALT_TAXIWAY)
		{
			IntersectionNodeList vNodeList;
			vNodeList.clear();
			m_vIntersectionNodeList.GetIntersectionNodeOf(((Taxiway*)pAltObject)->getID(),vNodeList);
			size_t nCount = vNodeList.size();
			for (size_t i =0; i < nCount; i++)
			{
				std::vector<ALTObject*> vTaxiways = vNodeList[i].GetOtherObjectList(((Taxiway*)pAltObject)->getID());
				size_t nSize = vTaxiways.size();
				for (size_t j = 0; j < nSize; j++)
				{
					if ((vTaxiways.at(j))->GetType() != ALT_TAXIWAY)
						continue;

					GetTaxiwayIntersectGroundRoutes((Taxiway*)vTaxiways.at(j),&vGroundRouteList);
				}
			}
		}
		else
		{
			GetGroundRouteIntersectGroundRoutes((GroundRoute*)pAltObject,&vGroundRouteList);
		}
	}
}


void CAirportGroundNetwork::GetRunwayIntersectGroundRoutes(Runway *pRunway, RUNWAY_MARK runwayMark, GroundRouteVector *pIntersectGRVector)
{
	ASSERT(pRunway);
	ASSERT(pIntersectGRVector);

	RunwayExitList runwayExitList;
	int nRunwayExitListCount = pRunway->GetExitList(runwayMark, runwayExitList);
	vector<RunwayExit>::const_iterator citer;

	//add intersect taxiway
	for (citer=runwayExitList.begin(); citer!=runwayExitList.end(); citer++)
	{
		RunwayExit runwayExit = *citer;
		
		int nObjID = runwayExit.GetTaxiwayID();

		int nGroundRouteCount = m_pGroundRoutes->GetCount();
		for (int i=0; i<nGroundRouteCount; i++)
		{
			GroundRoute *pGroundRoute = m_pGroundRoutes->GetItem(i);
			ASSERT(pGroundRoute);

			if (IsALTObjIntersectWithGroundRoute(nObjID, pGroundRoute))
			{
				GroundRouteIter grIter = std::find(pIntersectGRVector->begin(),pIntersectGRVector->end(), pGroundRoute);
				if (grIter == pIntersectGRVector->end())
					pIntersectGRVector->push_back(pGroundRoute);

			}
		}
	}
}

void CAirportGroundNetwork::GetTaxiwayIntersectGroundRoutes(Taxiway *pTaxiway, GroundRouteVector *pIntersectGRVector)
{
	ASSERT(pTaxiway);
	ASSERT(pIntersectGRVector);

	IntersectionNodeList intersectNodeList;
	intersectNodeList.clear();
	m_vIntersectionNodeList.GetIntersectionNodeOf(pTaxiway->getID(),intersectNodeList);

	IntersectionNodeList::const_iterator citer = intersectNodeList.begin();
	for (; citer!=intersectNodeList.end(); citer++)
	{
		IntersectionNode intersectNode = *citer;
		std::vector<int> vOtherObjIDList = intersectNode.GetOtherObjectIDList(pTaxiway->getID());
		
		for(int i=0;i<(int)vOtherObjIDList.size();i++)
		{
			int nALTObjID = vOtherObjIDList.at(i);

			int nGroundRouteCount = m_pGroundRoutes->GetCount();
			for (int j=0; j<nGroundRouteCount; j++)
			{
				GroundRoute *pGroundRoute = m_pGroundRoutes->GetItem(j);
				ASSERT(pGroundRoute);

				if (pGroundRoute == NULL || pGroundRoute->GetFirstItem() == NULL)
					continue;

				if (pGroundRoute->GetFirstItem()->GetALTObjectID() == nALTObjID)	//the taxiway is entry of ground route
				{
					GroundRouteIter iter = std::find(pIntersectGRVector->begin(),pIntersectGRVector->end(),pGroundRoute);

					if (iter == pIntersectGRVector->end())
						pIntersectGRVector->push_back(pGroundRoute);
					
				}
			}
		}
		
	}
	
}

void CAirportGroundNetwork::GetGroundRouteIntersectGroundRoutes(GroundRoute *pGroundRoute, GroundRouteVector *pIntersectGRVector)
{
	ASSERT(pGroundRoute);
	ASSERT(pIntersectGRVector);

	int nGroundRouteCount = m_pGroundRoutes->GetCount();
	for (int i=0; i<nGroundRouteCount; i++)
	{
		GroundRoute *pOtherRoute = m_pGroundRoutes->GetItem(i);
		ASSERT(pOtherRoute);

		if (pGroundRoute == pOtherRoute)
			continue;

		if (IsGroundRouteIntersectWithGroundRoute(pGroundRoute, pOtherRoute))
		{
			GroundRouteIter iter = std::find(pIntersectGRVector->begin(),pIntersectGRVector->end(),pOtherRoute);

			if (iter == pIntersectGRVector->end())
				pIntersectGRVector->push_back(pOtherRoute);
		}
	}
}

BOOL CAirportGroundNetwork::IsGroundRouteIntersectWithGroundRoute(GroundRoute *pFirstGroundRoute, GroundRoute *pSecondGroundRoute)
{
	ASSERT(pFirstGroundRoute);
	ASSERT(pSecondGroundRoute);

	GroundRouteItem *pFirstItem = pSecondGroundRoute->GetFirstItem();

	if (NULL == pFirstItem)
		return FALSE;

	std::vector<GroundRouteItem*> vTailItems = pFirstGroundRoute->GetExitItems();

	size_t nCount = vTailItems.size();
	for (size_t i =0; i < nCount; i++)
	{
		GroundRouteItem* pTailItem = vTailItems.at(i);
		if (IsIntersectedGrouteRouteItems(pFirstItem,pTailItem))
			return TRUE;
	}

	return FALSE;
}

BOOL CAirportGroundNetwork::IsIntersectedGrouteRouteItems(GroundRouteItem *pItem1, GroundRouteItem *pItem2)
{
	int nAltID = pItem1->GetALTObjectID();
	ALTObject* pALTObj = ALTObject::ReadObjectByID(nAltID);
	ASSERT(pALTObj);

	IntersectionNodeList vNodeList;
	vNodeList.clear();
	m_vIntersectionNodeList.GetIntersectionNodeOf(pALTObj->getID(),vNodeList);
	//switch(pALTObj->GetType())
	//{
	//case ALT_TAXIWAY:
	//	{
	//		Taxiway *pTw = (Taxiway*)pALTObj;
	//		vNodeList = pTw->GetIntersectNodes();
	//	}
	//	break;

	//case ALT_RUNWAY:
	//	{
	//		Runway *pRw = (Runway*)pALTObj;
	//		vNodeList = pRw->GetIntersectNodes();
	//	}
	//	break;

	//default:
	//	ASSERT(FALSE);
	//	break;
	//}

	delete pALTObj;
	pALTObj = NULL;

	size_t nCount = vNodeList.size();
	for (size_t i =0; i < nCount; i++)
	{
		if (vNodeList[i].HasObject(pItem2->GetALTObjectID()))
			return TRUE;		
	}

	return FALSE;

}

//
BOOL CAirportGroundNetwork::IsALTObjIntersectWithGroundRoute(int nALTObjID, GroundRoute *pGroundRoute)
{
	ASSERT(pGroundRoute);

	GroundRouteItem *pFirstItem = pGroundRoute->GetFirstItem();

	if (NULL == pFirstItem)
		return FALSE;

	ALTObject *pALTObj = ALTObject::ReadObjectByID(nALTObjID);
	ASSERT(pALTObj);

	IntersectionNodeList iNodeList;
	iNodeList.clear();
	m_vIntersectionNodeList.GetIntersectionNodeOf(pALTObj->getID(),iNodeList);
	//switch(pALTObj->GetType())
	//{
	//case ALT_TAXIWAY:
	//	{
	//		Taxiway *pTw = (Taxiway*)pALTObj;
	//		iNodeList = pTw->GetIntersectNodes();
	//	}
	//	break;

	//case ALT_RUNWAY:
	//	{
	//		Runway *pRw = (Runway*)pALTObj;
	//		iNodeList = pRw->GetIntersectNodes();
	//	}
	//	break;

	//default:
	//	ASSERT(FALSE);
	//	break;
	//}

	delete pALTObj;
	pALTObj = NULL;

	int nAltID = pFirstItem->GetALTObjectID();

	size_t nCount = iNodeList.size();
	for (size_t i =0; i < nCount; i++)
	{
		if (iNodeList[i].HasObject(nAltID))
			return TRUE;
	}

	return FALSE;
}


void CAirportGroundNetwork::GetRunwayTaxiwayIntersectNodeList(Runway *pRunway, Taxiway *pTaxiway, IntersectionNodeList& pIntersectNodeList)
{
	ASSERT(pRunway);
	ASSERT(pTaxiway);


	//int nRunwayID = pRunway->getID();

	//IntersectionNodeList iNodeList;
	//iNodeList.clear();
	m_vIntersectionNodeList.GetIntersectionNodes(pRunway,pTaxiway,pIntersectNodeList) ;
	//IntersectionNodeList::const_iterator citer = iNodeList.begin();
	//for (; citer!=iNodeList.end(); citer++)
	//{
	//	IntersectionNode node = *citer;
	//	std::vector<int> vOtherObjIDs = node.GetOtherObjectIDList(pTaxiway->getID());
	//	if ( vOtherObjIDs.end() != std::find(vOtherObjIDs.begin(),vOtherObjIDs.end(),nRunwayID) )
	//	{
	//		pIntersectNodeList->push_back(*citer);
	//	}
	//}
}
//
void CAirportGroundNetwork::GetTaxiwayTaxiwayIntersectNodeList(ALTObject *pFirstObject, ALTObject *pSecondObject, IntersectionNodeList& pIntersectNodeList)
{
	ASSERT(pFirstObject);
	ASSERT(pSecondObject);

	if(pFirstObject->GetType() != ALT_RUNWAY && pFirstObject->GetType() != ALT_TAXIWAY)
		return;
	if(pSecondObject->GetType() != ALT_TAXIWAY && pSecondObject->GetType() != ALT_RUNWAY)
		return;

	//m_vIntersectionNodeList.GetIntersectionNodes(pFirstTaxiway,pSecondTaxiway, pIntersectNodeList);

	int nFirstObjectID = pFirstObject->getID();

	IntersectionNodeList iNodeList;
	if(pSecondObject->GetType() == ALT_TAXIWAY)
	{
		iNodeList = ((Taxiway *)pSecondObject)->GetIntersectNodes();
	}
	else 
	{
		iNodeList = ((Runway *)pSecondObject)->GetIntersectNodes();
	}
	IntersectionNodeList::const_iterator citer = iNodeList.begin();
	for (; citer!=iNodeList.end(); citer++)
	{
		IntersectionNode node = *citer;
		std::vector<int> vOtherObjIDs = node.GetOtherObjectIDList(pSecondObject->getID());
		if ( vOtherObjIDs.end()!= std::find(vOtherObjIDs.begin(),vOtherObjIDs.end(),nFirstObjectID) )
		{
			pIntersectNodeList.push_back(node);
		}
	}
}

void CAirportGroundNetwork::GetRunwayGroundRouteIntersectNodeList(Runway *pRunway, GroundRoute *pGroundRoute, IntersectionNodeList* pIntersectNodeList)
{
	ASSERT(pRunway);
	ASSERT(pGroundRoute);
	ASSERT(pIntersectNodeList);

	GroundRoute runwayGroundRoute(pRunway->getAptID());
	GroundRouteItem *pRwunwayGRItem = new GroundRouteItem;
	pRwunwayGRItem->SetALTObjectID(pRunway->getID());
	runwayGroundRoute.SetFirstItem(pRwunwayGRItem);

	GetGroundRouteGroundRouteIntersectNodeList(&runwayGroundRoute, pGroundRoute, pIntersectNodeList);
}
//
void CAirportGroundNetwork::GetTaxiwayGroundRouteIntersectNodeList(Taxiway *pTaxiway, GroundRoute *pGroundRoute, IntersectionNodeList *pIntersectNodeList)
{
	ASSERT(pTaxiway);
	ASSERT(pGroundRoute);
	ASSERT(pIntersectNodeList);

	GroundRoute taxiwayGRoute(pTaxiway->getAptID());
	GroundRouteItem *pTaxiwayGRItem = new GroundRouteItem;
	pTaxiwayGRItem->SetALTObjectID(pTaxiway->getID());
	taxiwayGRoute.SetFirstItem(pTaxiwayGRItem);

	GetGroundRouteGroundRouteIntersectNodeList(&taxiwayGRoute, pGroundRoute, pIntersectNodeList);
}

void CAirportGroundNetwork::GetGroundRouteTaxiwayIntersectNodeList(GroundRoute* pGroundRoute, ALTObject *pObject, IntersectionNodeList *pIntersectNodeList)
{
	ASSERT(pGroundRoute);
	ASSERT(pObject);
	ASSERT(pIntersectNodeList);

	IntersectionNodeList taxiwayIntersectNodeList;
	taxiwayIntersectNodeList.clear();
	m_vIntersectionNodeList.GetIntersectionNodeOf(pObject->getID(),taxiwayIntersectNodeList);

	std::vector<GroundRouteItem*> vItems = pGroundRoute->GetExitItems();
	size_t nCount = vItems.size();

	IntersectionNodeList::const_iterator citer = taxiwayIntersectNodeList.begin();
	for (; citer!=taxiwayIntersectNodeList.end(); citer++)
	{
		for (size_t i =0; i< nCount; i++)
		{
			if ((*citer).HasObject(vItems[i]->GetALTObjectID()))
			{
				pIntersectNodeList->push_back(*citer);
				break;
			}
		}
	}
}

void CAirportGroundNetwork::GetGroundRouteRunwayIntersectNodeList(GroundRoute *pGroundRoute, Runway *pRunway, IntersectionNodeList *pIntersectNodeList)
{
	ASSERT(pGroundRoute);
	ASSERT(pRunway);
	ASSERT(pIntersectNodeList);

	IntersectionNodeList taxiwayIntersectNodeList;
	taxiwayIntersectNodeList.clear();
	m_vIntersectionNodeList.GetIntersectionNodeOf(pRunway->getID(),taxiwayIntersectNodeList);

	std::vector<GroundRouteItem*> vItems = pGroundRoute->GetExitItems();
	size_t nCount = vItems.size();

	IntersectionNodeList::const_iterator citer = taxiwayIntersectNodeList.begin();
	for (; citer!=taxiwayIntersectNodeList.end(); citer++)
	{
		for (size_t i =0; i< nCount; i++)
		{
			if ((*citer).HasObject(vItems[i]->GetALTObjectID()))
			{
				pIntersectNodeList->push_back(*citer);
				break;
			}
		}
	}
}

void CAirportGroundNetwork::GetGroundRouteGroundRouteIntersectNodeList(GroundRoute *pFirstRoute, GroundRoute *pSecondRoute, IntersectionNodeList *pIntersectNodeList)
{
	ASSERT(pFirstRoute);
	ASSERT(pSecondRoute);
	ASSERT(pIntersectNodeList);

	GroundRouteItem *pSecondRouteFirstItem = pSecondRoute->GetFirstItem();

	if (NULL == pSecondRouteFirstItem)
		return ;

	int nObjID = pSecondRouteFirstItem->GetALTObjectID();
	ALTObject *pAltObj = ALTObject::ReadObjectByID(nObjID);

	ASSERT(pAltObj);

	IntersectionNodeList vNodeList;
	vNodeList.clear();
	m_vIntersectionNodeList.GetIntersectionNodeOf(pAltObj->getID(),vNodeList);
	//switch(pAltObj->GetType())
	//{
	//case ALT_RUNWAY:
	//	{
	//		Runway *pRunway = (Runway*)pAltObj;
	//		vNodeList = pRunway->GetIntersectNodes();
	//	}
	//	break;

	//case ALT_TAXIWAY:
	//	{
	//		Taxiway *pTaxiway = (Taxiway*)pAltObj;
	//		vNodeList = pTaxiway->GetIntersectNodes();
	//	}
	//	break;
	//default:
	//	ASSERT(FALSE);
	//	break;
	//}
	delete pAltObj;
	pAltObj = NULL;

	std::vector<GroundRouteItem*> vItems = pFirstRoute->GetExitItems();
	size_t nCount = vItems.size();

	IntersectionNodeList::const_iterator citer = vNodeList.begin();
	for (; citer!=vNodeList.end(); citer++)
	{
		for (size_t i =0; i < nCount; i++)
		{
			if ((*citer).HasObject(vItems[i]->GetALTObjectID()))
			{
				pIntersectNodeList->push_back(*citer);
				break;
			}

		}
	}
}



//
//BOOL CAirportGroundNetwork::IsGroundRouteItemIntersectWithGroundRoute(GroundRouteItem *pGroundRouteItem, GroundRoute *pOtherGroundRoute)
//{
//	ASSERT(pGroundRouteItem);
//	ASSERT(pOtherGroundRoute);
//
//	int nObjID = pGroundRouteItem->GetALTObjectID();
//
//	if (IsALTObjIntersectWithGroundRoute(nObjID, pOtherGroundRoute))
//	{
//		return TRUE;
//	}
//
//	GroundRouteItemVector gRItemVector = pGroundRouteItem->GetChildItems();
//	GroundRouteItemIter iter = gRItemVector.begin();
//	for (; iter!=gRItemVector.end(); iter++)
//	{
//		return IsGroundRouteItemIntersectWithGroundRoute(*iter, pOtherGroundRoute);
//	}
//
//	return FALSE;
//}
//
//BOOL CAirportGroundNetwork::IsALTObjIntersectWithGroundRouteItem(int nALTObjID, GroundRouteItem *pGRItem)
//{
//	ASSERT(pGRItem);
//
//	int nGRItemALTObjID = pGRItem->GetALTObjectID();
//
//	ALTObject *pALTObj = ALTObject::ReadObjectByID(nGRItemALTObjID);
//	ASSERT(pALTObj);
//
//	IntersectionNodeList iNodeList;
//	switch(pALTObj->GetType())
//	{
//	case ALT_TAXIWAY:
//		{
//			Taxiway *pTw = (Taxiway*)pALTObj;
//			iNodeList = pTw->GetIntersectNodes();
//		}
//		break;
//
//	case ALT_RUNWAY:
//		{
//			Runway *pRw = (Runway*)pALTObj;
//			iNodeList = pRw->GetIntersectNodes();
//		}
//	    break;
//
//	default:
//		ASSERT(FALSE);
//	    break;
//	}
//
//	 
//
//
//	//find if intersect with item
//	for (int i=0; i<pGRItem->GetChildCount(); i++)
//	{
//		GroundRouteItem *pChildItem = pGRItem->GetChildItem(i);
//		ASSERT(pChildItem);
//
//		BOOL bRet = IsALTObjIntersectWithGroundRouteItem(nALTObjID, pChildItem);
//
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
//BOOL CAirportGroundNetwork::IsALTObjInGroundRoute(int nALTObjID, GroundRoute *pGroundRoute)
//{
//	ASSERT(pGroundRoute);
//
//	GroundRouteItem *pFirstGroundRouteItem = pGroundRoute->GetFirstItem();
//
//	if (NULL == pFirstGroundRouteItem)
//	{
//		return FALSE;
//	}
//
//	if (pFirstGroundRouteItem->GetALTObjectID() == nALTObjID)
//	{
//		return TRUE;
//	}
//
//	GroundRouteItemVector gRItemVector = pFirstGroundRouteItem->GetChildItems();
//	GroundRouteItemIter iter = gRItemVector.begin();
//	for (; iter!=gRItemVector.end(); iter++)
//	{
//		if ((*iter)->GetALTObjectID() == nALTObjID)
//		{
//			return TRUE;
//		}
//
//		if (IsALTObjEqualWithGroundRouteItem(nALTObjID, *iter))
//		{
//			return TRUE;
//		}		
//	}
//
//	return FALSE;
//}
////
//BOOL CAirportGroundNetwork::IsALTObjEqualWithGroundRouteItem(int nALTObjID, GroundRouteItem *pGroundRouteItem)
//{
//	ASSERT(pGroundRouteItem);
//
//	GroundRouteItemVector gRItemVector = pGroundRouteItem->GetChildItems();
//	GroundRouteItemIter iter = gRItemVector.begin();
//	for (; iter!=gRItemVector.end(); iter++)
//	{
//		if ((*iter)->GetALTObjectID() == nALTObjID)
//		{
//			return TRUE;
//		}
//
//		if (IsALTObjEqualWithGroundRouteItem(nALTObjID, *iter))
//		{
//			return TRUE;
//		}
//	}
//
//	return FALSE;
//}


//
//void CAirportGroundNetwork::GetGroundRouteGroundRouteIntersectNodeList(GroundRoute *pFirstRoute, GroundRoute *pSecondRoute, IntersectionNodeList *pIntersectNodeList)
//{
//	ASSERT(pFirstRoute);
//	ASSERT(pSecondRoute);
//	ASSERT(pIntersectNodeList);
//
//	GroundRouteItem *pSecondRouteFirstItem = pSecondRoute->GetFirstItem();
//
//	if (NULL == pSecondRouteFirstItem)
//	{
//		return ;
//	}
//
//	int nObjID = pSecondRouteFirstItem->GetALTObjectID();
//	ALTObject *pAltObj = ALTObject::ReadObjectByID(nObjID);
//	ASSERT(pAltObj);
//
//	//the obj isn't in first route
//	//if (!IsALTObjInGroundRoute(pAltObj->getID(), pFirstRoute))
//	//{
//		IntersectionNodeList intersectNodeList;
//		switch(pAltObj->GetType())
//		{
//		case ALT_RUNWAY:
//			{
//				Runway *pRunway = (Runway*)pAltObj;
//				intersectNodeList = pRunway->GetIntersectNodes();
//
//			}
//			break;
//
//		case ALT_TAXIWAY:
//			{
//				Taxiway *pTaxiway = (Taxiway*)pAltObj;
//				intersectNodeList = pTaxiway->GetIntersectNodes();
//			}
//			break;
//		default:
//			ASSERT(FALSE);
//			break;
//		}
//
//		IntersectionNodeList::const_iterator citer = intersectNodeList.begin();
//
//		for (; citer!=intersectNodeList.end(); citer++)
//		{
//			IntersectionNode node = (*citer);
//			std::vector<int> vOtherObjIDs = node.GetOtherObjectIDList(pAltObj->getID());
//			for(int i=0;i<(int)vOtherObjIDs.size();i++)
//			{
//				nObjID = vOtherObjIDs.at(i);
//				if (IsALTObjInGroundRoute(nObjID, pFirstRoute))
//				{
//					pIntersectNodeList->push_back(*citer);
//					break;
//				}
//			}			
//		}
//	//}
//	delete pAltObj;

	//GroundRouteItemVector gRItemVector = pSecondRouteFirstItem->GetChildItems();
	//GroundRouteItemIter iter = gRItemVector.begin();
	//for (; iter!=gRItemVector.end(); iter++)
	//{
	//	nObjID = (*iter)->GetALTObjectID();
	//	pAltObj = ALTObject::ReadObjectByID(nObjID);
	//	ASSERT(pAltObj);

	//	//if the obj is in firstroute
	//	if (IsALTObjInGroundRoute(pAltObj->getID(), pFirstRoute))
	//	{
	//		delete pAltObj;
	//		continue;
	//	}

	//	IntersectionNodeList iNodeList;
	//	switch(pAltObj->GetType())
	//	{
	//	case ALT_RUNWAY:
	//		{
	//			Runway *pRunway = (Runway*)pAltObj;
	//			iNodeList = pRunway->GetIntersectNodes();

	//		}
	//		break;

	//	case ALT_TAXIWAY:
	//		{
	//			Taxiway *pTaxiway = (Taxiway*)pAltObj;
	//			iNodeList = pTaxiway->GetIntersectNodes();
	//		}
	//		break;
	//	default:
	//		ASSERT(FALSE);
	//		break;
	//	}

	//	IntersectionNodeList::const_iterator citer = iNodeList.begin();
	//	for (; citer!=iNodeList.end(); citer++)
	//	{
	//		IntersectionNode node = *citer;
	//		std::vector<int> vOtherObjIDs =  node.GetOtherObjectIDList(pAltObj->getID());

	//		for(int i=0;i< (int)vOtherObjIDs.size();i++)
	//		{
	//			int nObjID = vOtherObjIDs.at(i);
	//			if (IsALTObjInGroundRoute(nObjID, pFirstRoute))
	//			{
	//				pIntersectNodeList->push_back(*citer);
	//				break;
	//			}
	//		}
	//		
	//	}
	//	delete pAltObj;

	//	GetGroundRouteGroundRouteItemIntersectNodeList(pFirstRoute, *iter, pIntersectNodeList);
	//}
//}
//
//void CAirportGroundNetwork::GetGroundRouteGroundRouteItemIntersectNodeList(GroundRoute *pFirstRoute, GroundRouteItem *pRouteItem, 
//																		   IntersectionNodeList *pInsersectNodeList)
//{
//	ASSERT(pFirstRoute);
//	ASSERT(pRouteItem);
//	ASSERT(pInsersectNodeList);
//
//	GroundRouteItemVector gRItemVector = pRouteItem->GetChildItems();
//	GroundRouteItemIter iter = gRItemVector.begin();
//	for (; iter!=gRItemVector.end(); iter++)
//	{
//		int nAltObjID = (*iter)->GetALTObjectID();
//		ALTObject *pALTObj = ALTObject::ReadObjectByID(nAltObjID);
//
//		//if the obj is in firstroute
//		if (IsALTObjInGroundRoute(pALTObj->getID(), pFirstRoute))
//		{
//			continue;
//		}
//
//		IntersectionNodeList intersectNodeList;
//		switch(pALTObj->GetType())
//		{
//		case ALT_RUNWAY:
//			{
//				Runway *pRunway = (Runway*)pALTObj;
//				intersectNodeList = pRunway->GetIntersectNodes();
//
//			}
//			break;
//
//		case ALT_TAXIWAY:
//			{
//				Taxiway *pTaxiway = (Taxiway*)pALTObj;
//				intersectNodeList = pTaxiway->GetIntersectNodes();
//			}
//			break;
//		default:
//			ASSERT(FALSE);
//			break;
//		}
//
//		IntersectionNodeList::const_iterator citer = intersectNodeList.begin();
//
//		for (; citer!=intersectNodeList.end(); citer++)
//		{
//			IntersectionNode node = (*citer);
//			std::vector<int> vOtherObjIDs = node.GetOtherObjectIDList(pALTObj->getID());
//
//			for(int iIDIdx=0;iIDIdx< (int)vOtherObjIDs.size();iIDIdx++)
//			{
//				int nObjID = vOtherObjIDs.at(iIDIdx);
//				if (IsALTObjInGroundRoute(nObjID, pFirstRoute))
//				{
//					pInsersectNodeList->push_back(*citer);
//					break;
//				}
//			}
//		
//		}
//		delete pALTObj;
//
//		GetGroundRouteGroundRouteItemIntersectNodeList(pFirstRoute, *iter, pInsersectNodeList);
//	}
//}



//void CAirportGroundNetwork::GetStandOutNodeList(Stand *pStand, IntersectionNodeList& outNodeList, vector<int>* vStandID)
//{
//	ASSERT(pStand);
//	ASSERT(NULL != vStandID);
//
//	IntersectionNodeList standIntersectNodeList = pStand->GetIntersectNodes();
//
//	//haven't intersectnode
//	if(standIntersectNodeList.size())
//	{
//		outNodeList.push_back(*standIntersectNodeList.rbegin());
//		vStandID->push_back(pStand->getID());
//	}
//
//}

//void CAirportGroundNetwork::GetStandGroupTaxiwayIntersectNodeList(ALTObjectGroup *pALTObjGroup, IntersectionNodeList *pIntersectNodeList, vector<int>* vStandID)
//{
//	ASSERT(pALTObjGroup);
//	ASSERT(pIntersectNodeList);
//	ASSERT(NULL != vStandID);
//
//	//int nProjID = pALTObjGroup->GetProjID();
//	//ALTObjectID altObjGroupNameID = pALTObjGroup->getName();
//	//GetStandList(nProjID);
//
//		for(int i =0; i <(int)m_vStands.size(); i++)
//		{
//			Stand * pStand =(Stand*) m_vStands.at(i).get();
//			ALTObjectID standNameID = pStand->GetObjectName();
//
//			//the stand isn't in group
//			if (!standNameID.idFits(altObjGroupNameID))
//			{
//				continue;
//			}
//
//			IntersectionNodeList standOutNodeList;
//			vector<int> vIntersectNodeStandID;
//			GetStandOutNodeList(pStand, standOutNodeList, &vIntersectNodeStandID);
//			pIntersectNodeList->insert(pIntersectNodeList->end(),standOutNodeList.begin(),standOutNodeList.end());
//			vStandID->insert(vStandID->end(), vIntersectNodeStandID.begin(), vIntersectNodeStandID.end());
//
//			/*for (IntersectionNodeList::iterator iter=standOutNodeList.begin(); iter!=standOutNodeList.end(); iter++)
//			{
//				IntersectionNodeList iNode = *iter;
//
//				IntersectNode taxiwayEqualNode;
//				BOOL bEqualNodeExist = StandOutNodeFindEqualNode(&stand, &iNode, taxiwayEqualNode);
//				ASSERT(bEqualNodeExist);
//
//				pIntersectNodeList->push_back(iNode);
//			}*/
//		}
//	//}
//}


//

//void CAirportGroundNetwork::GetStandGroupIntersectGroundRoutes(ALTObjectGroup *pALTObjGroup, GroundRoutes *pAllGroundRoutes, 
//															   GroundRouteVector *pIntersectGRVector)
//{
//	ASSERT(pALTObjGroup);
//	ASSERT(pAllGroundRoutes);
//	ASSERT(pIntersectGRVector);
//
//	//int nProjID = pALTObjGroup->GetProjID();
//	//ALTObjectID altObjGroupNameID = pALTObjGroup->getName();
//	//GetStandList(nProjID);
//
//
//		for(int i =0; i <(int)m_vStands.size(); i++)
//		{
//			Stand * pStand =(Stand*) m_vStands.at(i).get();
//			ALTObjectID standNameID = pStand->GetObjectName();
//
//			//the stand isn't in group
//			if (!standNameID.idFits(altObjGroupNameID))
//			{
//				continue;
//			}
//
//			IntersectionNodeList standOutNodeList;
//			vector<int> vStandID;
//			GetStandOutNodeList(pStand, standOutNodeList, &vStandID);
//
//			for (IntersectionNodeList::iterator iter=standOutNodeList.begin(); iter!=standOutNodeList.end(); iter++)
//			{
//				IntersectionNode standOutNode = *iter;
//
//
//				std::vector<int> vTaxiwayIDs = standOutNode.GetOtherObjectIDList(pStand->getID());
//				for(int i=0;i<(int)vTaxiwayIDs.size();i++)
//				{	
//					int nTaxiwayID = vTaxiwayIDs.at(i);
//					int nGroundRoutesCount = pAllGroundRoutes->GetCount();
//					for (int j=0; j<nGroundRoutesCount; j++)
//					{
//						GroundRoute *pGR = pAllGroundRoutes->GetItem(j);
//						ASSERT(pGR);
//
//						//if the taxiway isn't in the ground route
//						if (!IsALTObjInGroundRoute(nTaxiwayID, pGR))
//						{
//							continue;
//						}
//
//						BOOL bExist = FALSE;
//						GroundRouteIter iter = pIntersectGRVector->begin();
//						for (; iter != pIntersectGRVector->end(); iter++)
//						{
//							if (pGR == *iter)
//							{
//								bExist = TRUE;
//								break;
//							}
//						}
//
//						//not exist
//						if (!bExist)
//						{
//							pIntersectGRVector->push_back(pGR);
//						}
//					}
//				}			
//			}
//		}
//	//}
//}
////
//void CAirportGroundNetwork::GetStandGroupGroundRouteIntersectNodeList(ALTObjectGroup *pALTObjGroup, GroundRoute *pGroundRoute, 
//																	  IntersectionNodeList *pIntersectNodeList)
//{
//	ASSERT(pALTObjGroup);
//	ASSERT(pGroundRoute);
//	ASSERT(pIntersectNodeList);
//
//	//int nProjID = pALTObjGroup->GetProjID();
//	//ALTObjectID altObjGroupNameID = pALTObjGroup->getName();
//	//GetStandList(nProjID);
//
//
//		for(int i =0; i <(int)m_vStands.size(); i++)
//		{
//			Stand * pStand =(Stand*) m_vStands.at(i).get();
//			ALTObjectID standNameID = pStand->GetObjectName();
//
//			//the stand isn't in group
//			if (!standNameID.idFits(altObjGroupNameID))
//			{
//				continue;
//			}
//
//			IntersectionNodeList standOutNodeList;
//			vector<int> vStandID;
//			GetStandOutNodeList(pStand, standOutNodeList, &vStandID);
//			
//			for (IntersectionNodeList::iterator iter=standOutNodeList.begin(); iter!=standOutNodeList.end(); iter++)
//			{
//				IntersectionNode standOutNode = *iter;
//	
//				std::vector<int> vOtherIDs  = standOutNode.GetOtherObjectIDList(pStand->getID());
//				
//				for(int i=0;i< (int)vOtherIDs.size();i++)
//				{
//					int nTaxiwayID = vOtherIDs.at(i); 
//					if (IsALTObjInGroundRoute(nTaxiwayID, pGroundRoute))
//					{
//						pIntersectNodeList->push_back(standOutNode);
//						break;
//					}
//				}
//				
//			}
//			
//		}
//}