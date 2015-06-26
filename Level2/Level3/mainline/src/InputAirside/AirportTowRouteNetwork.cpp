#include "StdAfx.h"
#include ".\airporttowroutenetwork.h"
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
#include "TowingRoute.h"

using namespace std;


CAirportTowRouteNetwork::CAirportTowRouteNetwork(int nPrjID)
:m_pTowingRoutes(NULL)
{
	m_vStands.clear();
	m_vTaxiways.clear();

	std::vector<int> vAirportIds;
	InputAirside::GetAirportList(nPrjID, vAirportIds);
	int nAirportID = vAirportIds.at(0);

	GetStandList(nAirportID);
	GetTaxiwayList(nAirportID);
	GetRunwayList(nAirportID);

	m_pTowingRoutes = new CTowingRouteList();
	m_pTowingRoutes->ReadData();

	m_vIntersectionNodeList.ReadData(nAirportID);
}

CAirportTowRouteNetwork::~CAirportTowRouteNetwork(void)
{
	if (m_pTowingRoutes)
	{
		delete m_pTowingRoutes;
		m_pTowingRoutes = NULL;
	}
}
void CAirportTowRouteNetwork::GetStandList(int nAirportID)
{
	if (!m_vStands.empty())
		return;

	ALTAirport::GetStandList(nAirportID,m_vStands);

}

void CAirportTowRouteNetwork::GetTaxiwayList(int nAirportID)
{
	if (!m_vTaxiways.empty())
		return;

	ALTAirport::GetTaxiwayList(nAirportID,m_vTaxiways);

}
void CAirportTowRouteNetwork::GetRunwayList( int nAirportID )
{
	if (!m_vRunways.empty())
		return;

	ALTAirport::GetRunwayList(nAirportID,m_vRunways);
}

ALTObject* CAirportTowRouteNetwork::GetTaxiwayByID(int nID)
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


ALTObject* CAirportTowRouteNetwork::GetRunwayByID( int nID )
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
void CAirportTowRouteNetwork::GetIntersectedTaxiwayRunway(ALTObject* pAltObject, std::set<ALTObject*>& vTaxiwayList, std::set<ALTObject*>& vRunwayList)
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

void CAirportTowRouteNetwork::GetIntersectedGroundRoutes(ALTObject* pAltObject, std::vector<CTowingRoute *>& vGroundRouteList)
{
	if (pAltObject == NULL)
	{
		int nCount = m_pTowingRoutes->GetTowingRouteCount();
		for (int i =0; i < nCount; i++)
		{
			CTowingRoute* pRoute = m_pTowingRoutes->GetTowingRouteItem(i);
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
			GetGroundRouteIntersectGroundRoutes((CTowingRoute*)pAltObject,&vGroundRouteList);
		}
	}
}


void CAirportTowRouteNetwork::GetRunwayIntersectGroundRoutes(Runway *pRunway, RUNWAY_MARK runwayMark, std::vector<CTowingRoute *> *pIntersectGRVector)
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

		int nGroundRouteCount = m_pTowingRoutes->GetTowingRouteCount();
		for (int i=0; i<nGroundRouteCount; i++)
		{
			CTowingRoute *pGroundRoute = m_pTowingRoutes->GetTowingRouteItem(i);
			if(pGroundRoute == NULL)
				continue;

			if (IsALTObjIntersectWithGroundRoute(nObjID, pGroundRoute))
			{
				std::vector<CTowingRoute *>::iterator grIter = std::find(pIntersectGRVector->begin(),pIntersectGRVector->end(), pGroundRoute);
				if (grIter == pIntersectGRVector->end())
					pIntersectGRVector->push_back(pGroundRoute);

			}
		}
	}
}

void CAirportTowRouteNetwork::GetTaxiwayIntersectGroundRoutes(Taxiway *pTaxiway, std::vector<CTowingRoute *> *pIntersectGRVector)
{
	if(pTaxiway == NULL || pIntersectGRVector == NULL)
		return;

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

			int nGroundRouteCount = m_pTowingRoutes->GetTowingRouteCount();
			for (int j=0; j<nGroundRouteCount; j++)
			{
				CTowingRoute *pGroundRoute = m_pTowingRoutes->GetTowingRouteItem(j);
				if(pGroundRoute == NULL)
					continue;
				std::vector<CTaxiwayNode *> vFirstNodes = pGroundRoute->GetFirstItems();
				int nFirstCount = (int)vFirstNodes.size();
				for (int nFirst = 0; nFirst < nFirstCount; ++nFirst)
				{
					CTaxiwayNode *pFirstItem = vFirstNodes.at(nFirst);
					if(pFirstItem == NULL)
						continue;

					if (pFirstItem->GetTaxiwayID() == nALTObjID)	//the taxiway is entry of ground route
					{
						std::vector<CTowingRoute *>::iterator iter = std::find(pIntersectGRVector->begin(),pIntersectGRVector->end(),pGroundRoute);

						if (iter == pIntersectGRVector->end())
							pIntersectGRVector->push_back(pGroundRoute);
					}
				}
			}
		}
	}
}

void CAirportTowRouteNetwork::GetGroundRouteIntersectGroundRoutes(CTowingRoute *pGroundRoute, std::vector<CTowingRoute *> *pIntersectGRVector)
{
	if(pGroundRoute == NULL || pIntersectGRVector == NULL)
		return;

	int nGroundRouteCount = m_pTowingRoutes->GetTowingRouteCount();
	for (int i=0; i<nGroundRouteCount; i++)
	{
		CTowingRoute *pOtherRoute = m_pTowingRoutes->GetTowingRouteItem(i);
		if(pOtherRoute == NULL)
			continue;

		if (pGroundRoute == pOtherRoute)
			continue;

		if (IsGroundRouteIntersectWithGroundRoute(pGroundRoute, pOtherRoute))
		{
			std::vector<CTowingRoute *>::iterator iter = std::find(pIntersectGRVector->begin(),pIntersectGRVector->end(),pOtherRoute);

			if (iter == pIntersectGRVector->end())
				pIntersectGRVector->push_back(pOtherRoute);
		}
	}
}

BOOL CAirportTowRouteNetwork::IsGroundRouteIntersectWithGroundRoute(CTowingRoute *pFirstGroundRoute, CTowingRoute *pSecondGroundRoute)
{
	if(pFirstGroundRoute == NULL || pSecondGroundRoute == NULL)
		return FALSE;

	std::vector<CTaxiwayNode *> vFirstRouteItems = pSecondGroundRoute->GetFirstItems();


	std::vector<CTaxiwayNode*> vTailItems = pFirstGroundRoute->GetExitItems();
	size_t nFirstCount = vFirstRouteItems.size();
	size_t nTrailCount = vTailItems.size();

	for (size_t nFirst =0 ;nFirst < nFirstCount; ++ nFirst)
	{	
		CTaxiwayNode* pFirstItem = vFirstRouteItems.at(nFirst);
		for (size_t i =0; i < nTrailCount; i++)
		{
			CTaxiwayNode* pTailItem = vTailItems.at(i);
			if (IsIntersectedGrouteRouteItems(pFirstItem,pTailItem))
				return TRUE;
		}

	}


	return FALSE;
}

BOOL CAirportTowRouteNetwork::IsIntersectedGrouteRouteItems(CTaxiwayNode *pItem1, CTaxiwayNode *pItem2)
{
	if(pItem1 == NULL || pItem2 == NULL)
		return FALSE;


	IntersectionNodeList vNodeList;
	vNodeList.clear();
	m_vIntersectionNodeList.GetIntersectionNodeOf(pItem1->GetTaxiwayID(),vNodeList);

	size_t nCount = vNodeList.size();
	for (size_t i =0; i < nCount; i++)
	{
		if (vNodeList[i].HasObject(pItem2->GetTaxiwayID()))
			return TRUE;		
	}

	return FALSE;

}

//
BOOL CAirportTowRouteNetwork::IsALTObjIntersectWithGroundRoute(int nALTObjID, CTowingRoute *pGroundRoute)
{
	if(pGroundRoute == NULL)
		return FALSE;

	IntersectionNodeList iNodeList;
	iNodeList.clear();
	m_vIntersectionNodeList.GetIntersectionNodeOf(nALTObjID,iNodeList);
	
	std::vector<CTaxiwayNode *> vFirstNode = pGroundRoute->GetFirstItems();
	int nRouteItemCount = (int)vFirstNode.size();

	for (int nRoute = 0; nRoute < nRouteItemCount; ++nRoute)
	{
		CTaxiwayNode* pRouteItem = vFirstNode.at(nRoute);
		if(pRouteItem == NULL)
			continue;

		size_t nCount = iNodeList.size();
		for (size_t i =0; i < nCount; i++)
		{
			if (iNodeList[i].HasObject(pRouteItem->GetTaxiwayID()))
				return TRUE;
		}
	}
	return FALSE;
}


void CAirportTowRouteNetwork::GetRunwayTaxiwayIntersectNodeList(Runway *pRunway, Taxiway *pTaxiway, IntersectionNodeList& pIntersectNodeList)
{
	if(pRunway == NULL ||pTaxiway == NULL)
		return;

	m_vIntersectionNodeList.GetIntersectionNodes(pRunway,pTaxiway,pIntersectNodeList) ;

}
//
void CAirportTowRouteNetwork::GetTaxiwayTaxiwayIntersectNodeList(ALTObject *pFirstObject, ALTObject *pSecondObject, IntersectionNodeList& pIntersectNodeList)
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

void CAirportTowRouteNetwork::GetRunwayGroundRouteIntersectNodeList(Runway *pRunway, CTowingRoute *pGroundRoute, IntersectionNodeList* pIntersectNodeList)
{
	if(pRunway == NULL ||pGroundRoute == NULL ||pIntersectNodeList)
		return;



	CTowingRoute tempTowingRoute;
	CTaxiwayNode *pTaxiNode = new CTaxiwayNode;
	pTaxiNode->SetTaxiwayID(pRunway->getID());
	tempTowingRoute.AddTaxiwayNodeItem(pTaxiNode);
	GetGroundRouteGroundRouteIntersectNodeList(&tempTowingRoute, pGroundRoute, pIntersectNodeList);
}

void CAirportTowRouteNetwork::GetTaxiwayGroundRouteIntersectNodeList(Taxiway *pTaxiway, CTowingRoute *pGroundRoute, IntersectionNodeList *pIntersectNodeList)
{
	if(pTaxiway == NULL || pGroundRoute == NULL || pIntersectNodeList == NULL)
		return;

	//GroundRoute taxiwayGRoute(pTaxiway->getAptID());
	//GroundRouteItem *pTaxiwayGRItem = new GroundRouteItem;
	//pTaxiwayGRItem->SetALTObjectID(pTaxiway->getID());
	//taxiwayGRoute.SetFirstItem(pTaxiwayGRItem);

	CTowingRoute tempTowingRoute;
	CTaxiwayNode *pTaxiNode = new CTaxiwayNode;
	pTaxiNode->SetTaxiwayID(pTaxiway->getID());
	tempTowingRoute.AddTaxiwayNodeItem(pTaxiNode);

	GetGroundRouteGroundRouteIntersectNodeList(&tempTowingRoute, pGroundRoute, pIntersectNodeList);
}

void CAirportTowRouteNetwork::GetGroundRouteTaxiwayIntersectNodeList(CTowingRoute* pGroundRoute, ALTObject *pObject, IntersectionNodeList *pIntersectNodeList)
{
	if(pGroundRoute == NULL ||pObject == NULL ||pIntersectNodeList == NULL)
		return;

	IntersectionNodeList taxiwayIntersectNodeList;
	taxiwayIntersectNodeList.clear();
	m_vIntersectionNodeList.GetIntersectionNodeOf(pObject->getID(),taxiwayIntersectNodeList);

	std::vector<CTaxiwayNode*> vItems = pGroundRoute->GetExitItems();
	size_t nCount = vItems.size();

	IntersectionNodeList::const_iterator citer = taxiwayIntersectNodeList.begin();
	for (; citer!=taxiwayIntersectNodeList.end(); citer++)
	{
		for (size_t i =0; i< nCount; i++)
		{
			if ((*citer).HasObject(vItems[i]->GetTaxiwayID()))
			{
				pIntersectNodeList->push_back(*citer);
				break;
			}
		}
	}
}

void CAirportTowRouteNetwork::GetGroundRouteRunwayIntersectNodeList(CTowingRoute *pGroundRoute, Runway *pRunway, IntersectionNodeList *pIntersectNodeList)
{
	if(pGroundRoute == NULL || pRunway == NULL||pIntersectNodeList == NULL)
		return;

	IntersectionNodeList taxiwayIntersectNodeList;
	taxiwayIntersectNodeList.clear();
	m_vIntersectionNodeList.GetIntersectionNodeOf(pRunway->getID(),taxiwayIntersectNodeList);

	std::vector<CTaxiwayNode*> vItems = pGroundRoute->GetExitItems();
	size_t nCount = vItems.size();

	IntersectionNodeList::const_iterator citer = taxiwayIntersectNodeList.begin();
	for (; citer!=taxiwayIntersectNodeList.end(); citer++)
	{
		for (size_t i =0; i< nCount; i++)
		{
			if ((*citer).HasObject(vItems[i]->GetTaxiwayID()))
			{
				pIntersectNodeList->push_back(*citer);
				break;
			}
		}
	}
}

void CAirportTowRouteNetwork::GetGroundRouteGroundRouteIntersectNodeList(CTowingRoute *pFirstRoute, CTowingRoute *pSecondRoute, IntersectionNodeList *pIntersectNodeList)
{
	ASSERT(pFirstRoute);
	ASSERT(pSecondRoute);
	ASSERT(pIntersectNodeList);

	std::vector<CTaxiwayNode *> vFirstItems = pSecondRoute->GetFirstItems();

	for (size_t nFirst = 0; nFirst < vFirstItems.size(); ++nFirst)
	{
		CTaxiwayNode *pFirstItem = vFirstItems.at(nFirst);
		int nObjID = pFirstItem->GetTaxiwayID();

		IntersectionNodeList vNodeList;
		vNodeList.clear();
		m_vIntersectionNodeList.GetIntersectionNodeOf(nObjID,vNodeList);


		std::vector<CTaxiwayNode *> vItems = pFirstRoute->GetExitItems();
		size_t nCount = vItems.size();

		IntersectionNodeList::const_iterator citer = vNodeList.begin();
		for (; citer!=vNodeList.end(); citer++)
		{
			for (size_t i =0; i < nCount; i++)
			{
				if ((*citer).HasObject(vItems[i]->GetTaxiwayID()))
				{
					pIntersectNodeList->push_back(*citer);
					break;
				}

			}
		}
	}
 

}