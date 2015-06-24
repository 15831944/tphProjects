#include "StdAfx.h"
#include ".\outboundrouteinsim.h"
#include "../../InputAirside/outboundRoute.h"
#include "../../InputAirside/outboundRouteItem.h"
#include "../../InputAirside/GroundRoute.h"
#include "InOutboundRouteBranchInSim.h"
#include "TaxiwayResourceManager.h"
#include "../../InputAirside/AirportGroundNetwork.h"
#include "../../InputAirside/stand.h"
#include "StandInSim.h"
#include "../../InputAirside/Runway.h"

COutboundRouteInSim::COutboundRouteInSim(TaxiwayResourceManager *pTaxiRes, COutboundRoute *pOutboundRoute)
{
	m_pAirportGroundNetWork = new CAirportGroundNetwork;

	ASSERT(pTaxiRes);
	m_pTaxiRes = pTaxiRes;
	ASSERT(pOutboundRoute);
	m_pOutboundRoute = pOutboundRoute;

	int nFirstItemCount = m_pOutboundRoute->GetElementCount();

	for (int i=0; i<nFirstItemCount; i++)
	{
		COutboundRouteItem *pOutboundRouteHead = m_pOutboundRoute->GetItem(i);
		ASSERT(pOutboundRouteHead);

		CInOutboundRouteBranchInSim *pOutboundRouteBranchInSim = new CInOutboundRouteBranchInSim;
		m_OutboundRouteBranchInSimList.push_back(pOutboundRouteBranchInSim);

		InitOutboundRouteBranchInSimList(pOutboundRouteBranchInSim, pOutboundRouteHead);
	}
}

COutboundRouteInSim::~COutboundRouteInSim(void)
{
	delete m_pAirportGroundNetWork;

	vector<CInOutboundRouteBranchInSim*>::iterator iter = m_OutboundRouteBranchInSimList.begin();
	for (; iter!=m_OutboundRouteBranchInSimList.end(); iter++)
	{
		CInOutboundRouteBranchInSim *pBranchInSim = *iter;
		delete pBranchInSim;
	}

	m_OutboundRouteBranchInSimList.clear();
}

BOOL COutboundRouteInSim::GetBranch(StandInSim* pStandInSim, TaxiwayDirectSegList& taxiwayDirectSegList)
{
	ASSERT(pStandInSim);

	int nStandId = pStandInSim->GetStandInput()->getID();

	vector<CInOutboundRouteBranchInSim*>::iterator iter = m_OutboundRouteBranchInSimList.begin();
	for (; iter!=m_OutboundRouteBranchInSimList.end(); iter++)
	{
		CInOutboundRouteBranchInSim* pBranch = *iter;
		ASSERT(pBranch);

		if (nStandId == pBranch->GetStandID())
		{
			(*iter)->GetTaxiwayDirectSegList(taxiwayDirectSegList);

			return TRUE;
		}
	}

	return FALSE;
}

int COutboundRouteInSim::GetBranchCount()
{
	return m_OutboundRouteBranchInSimList.size();
}

COutboundRoute* COutboundRouteInSim::GetOutboundRouteInput()
{
	return m_pOutboundRoute;
}

int COutboundRouteInSim::GetRunwayExitID()
{
	return m_pOutboundRoute->GetRunwayExitID();
}

int COutboundRouteInSim::GetStandGroupID()
{
	return m_pOutboundRoute->GetStandGroupID();
}

void COutboundRouteInSim::InitOutboundRouteBranchInSimList(CInOutboundRouteBranchInSim *pInOutboundRouteBranchInSim,COutboundRouteItem *pOutboundRouteItem)
{
	ASSERT(pInOutboundRouteBranchInSim);
	ASSERT(pOutboundRouteItem);

	ALTObject *pALTObj = ALTObject::ReadObjectByID(pOutboundRouteItem->GetALTObjectID());
	ASSERT(pALTObj);

	switch(pALTObj->GetType())
	{
	case ALT_TAXIWAY:
		{
			Taxiway *pTw = (Taxiway *)pALTObj;
			AddTaxiwayInfoToOutboundRouteBranchInSim(pInOutboundRouteBranchInSim, pTw, pOutboundRouteItem->GetIntersectNodeID(), pOutboundRouteItem);
		}		
	    break;

	case ALT_GROUNDROUTE:
		AddGroundRouteInfoToOutboundRouteBranchInSim(pInOutboundRouteBranchInSim, pOutboundRouteItem);
		break;

	default:
		ASSERT(FALSE);
	    break;
	}

	delete pALTObj;
}

void COutboundRouteInSim::SetTaxiwayIntersectStandID(Taxiway *pTaxiway, int nTaxiwayIntersectNodeID, CInOutboundRouteBranchInSim *pOutboundRouteBranchInSim)
{
	ASSERT(pTaxiway);

	IntersectionNode twFromNode;
	twFromNode.ReadData(nTaxiwayIntersectNodeID);
	
	std::vector<ALTObject*> vOtherObjList = twFromNode.GetOtherObjectList(pTaxiway->getID());
	//if the taxiway is intersect with stand
	for(int i=0;i< (int)vOtherObjList.size();i++)	
	{
		ALTObject *pOtherObj  = vOtherObjList.at(i);
		if (ALT_STAND == pOtherObj->GetType())
		{
			pOutboundRouteBranchInSim->SetStandID(pOtherObj->getID());
		}
	}	
}

void COutboundRouteInSim::AddTaxiwayInfoToOutboundRouteBranchInSim(CInOutboundRouteBranchInSim *pOutboundRouteBranchInSim, 
																   Taxiway *pTaxiway, int nTaxiwayIntersectNodeID, COutboundRouteItem *pOutboundRouteItem)
{
	ASSERT(pOutboundRouteBranchInSim);
	ASSERT(pTaxiway);
	ASSERT(pOutboundRouteItem);

	CInOutboundRouteBranchInSim *pCurrentOutboundRouteBranchInSim = new CInOutboundRouteBranchInSim(*pOutboundRouteBranchInSim);

	//if the taxiway is intersect with stand out node ,set stand id
	SetTaxiwayIntersectStandID(pTaxiway, nTaxiwayIntersectNodeID, pOutboundRouteBranchInSim);

	//this taxiway is intersect with runway
	if (0 == (int)pOutboundRouteItem->GetElementCount())
	{
		RunwayExit runwayExit;
		runwayExit.ReadData(m_pOutboundRoute->GetRunwayExitID());
		IntersectionNode rwINode = runwayExit.GetIntesectionNode();

		ALTObject *pALTObj = ALTObject::ReadObjectByID(m_pOutboundRoute->GetRunwayID());
		ASSERT(pALTObj);

		//the taxiway isn't intersect with runway
		if (ALT_RUNWAY != pALTObj->GetType())
		{
			BOOL bRet = RemoveBranch(pOutboundRouteBranchInSim);
			ASSERT(bRet);
			return;
		}
		Runway *pRunway = (Runway*)pALTObj;

		AddTaxiwaySegmentToOutboundRouteBranchInSim(pOutboundRouteBranchInSim, pTaxiway->getID(), nTaxiwayIntersectNodeID, rwINode.GetID());

		delete pALTObj;		
	}
	else
	{
		for (int i=0; i<(int)pOutboundRouteItem->GetElementCount(); i++)
		{
			COutboundRouteItem *pChildItem = pOutboundRouteItem->GetItem(i);
			int nChildItemINodeID = pChildItem->GetIntersectNodeID();
			int nChildItemObjID = pChildItem->GetALTObjectID();


			IntersectionNode iNode;
			iNode.ReadData(nChildItemINodeID);

			//int nNodeParentALTObjID = iNode.GetParentObjectID();

			//IntersectNode equalNode;

			//ALTObject *pChildObj = ALTObject::ReadObjectByID(nNodeParentALTObjID);
			//ASSERT(pChildObj);
			//switch(pChildObj->GetType())
			//{
			//case ALT_TAXIWAY:
			//	{
			//		Taxiway* pChildTw = (Taxiway*)pChildObj;
			//		BOOL bExist = m_pAirportGroundNetWork->FindEqualNode(pChildTw, &iNode, equalNode);
			//		ASSERT(bExist);
			//		ASSERT(equalNode.GetParentObjectID() == pTaxiway->getID());
			//	}
			//	break;

			//case ALT_RUNWAY: 
			//	ASSERT(FALSE);
			//	break;

			//default:
			//	ASSERT(FALSE);
			//	break;
			//}

			//More than one branch
			if (0<i)
			{
				CInOutboundRouteBranchInSim *pNewBranchInSim = new CInOutboundRouteBranchInSim(*pCurrentOutboundRouteBranchInSim);
				m_OutboundRouteBranchInSimList.push_back(pNewBranchInSim);

				AddTaxiwaySegmentToOutboundRouteBranchInSim(pNewBranchInSim, pTaxiway->getID(), nTaxiwayIntersectNodeID, iNode.GetID());
				InitOutboundRouteBranchInSimList(pNewBranchInSim, pChildItem);
			}
			else
			{
				AddTaxiwaySegmentToOutboundRouteBranchInSim(pOutboundRouteBranchInSim, pTaxiway->getID(), nTaxiwayIntersectNodeID, iNode.GetID());

				InitOutboundRouteBranchInSimList(pOutboundRouteBranchInSim, pChildItem);
			}
			
		}
	}

	delete pCurrentOutboundRouteBranchInSim;
}

void COutboundRouteInSim::AddTaxiwaySegmentToOutboundRouteBranchInSim(CInOutboundRouteBranchInSim *pOutboundRouteBranchInSim, int nTaxiwayID, 
																	  int nIntersectNodeIDFrom, int nIntersectNodeIDTo)
{
	ASSERT(pOutboundRouteBranchInSim);

	TaxiwayDirectSegList twDirectSegLst;
	m_pTaxiRes->GetTaxiwaySegment(nTaxiwayID, nIntersectNodeIDFrom, nIntersectNodeIDTo, twDirectSegLst);

	for (int i=0; i<(int)twDirectSegLst.size(); i++)
	{
		TaxiwayDirectSegInSim *pTwDirectSegInSim = twDirectSegLst.at(i);
		pOutboundRouteBranchInSim->AddTaxiwaySegInSimToBranch(pTwDirectSegInSim);
	}
}

BOOL COutboundRouteInSim::RemoveBranch(CInOutboundRouteBranchInSim * pBranch)
{
	ASSERT(pBranch);

	if (0 == (int)m_OutboundRouteBranchInSimList.size())
	{
		return FALSE;
	}

	for (vector<CInOutboundRouteBranchInSim*>::iterator iter=m_OutboundRouteBranchInSimList.begin(); iter!=m_OutboundRouteBranchInSimList.end(); iter++)
	{
		if (*iter == pBranch)
		{
			m_OutboundRouteBranchInSimList.erase(iter);
			delete pBranch;

			return TRUE;
		}
	}

	return FALSE;
}

void COutboundRouteInSim::AddGroundRouteInfoToOutboundRouteBranchInSim(CInOutboundRouteBranchInSim *pOutboundRouteBranchInSim,  COutboundRouteItem *pOutboundRouteItem)
{
	ASSERT(pOutboundRouteBranchInSim);
	ASSERT(pOutboundRouteItem);

	ALTObject *pALTObj = ALTObject::ReadObjectByID(pOutboundRouteItem->GetALTObjectID());
	ASSERT(pALTObj);

	GroundRoute *pGR = (GroundRoute *)pALTObj;
	ASSERT(pGR);

	int nIntersectNodeID = pOutboundRouteItem->GetIntersectNodeID();

	GroundRouteItem *pGRItemWithIntersectNode = pGR->GetItemWithGivenIntersectNodeID(nIntersectNodeID);
	ASSERT(pGRItemWithIntersectNode);

	AddGroundRouteItemInfoToOutboundRouteBranchInsim(pGRItemWithIntersectNode, nIntersectNodeID, pOutboundRouteBranchInSim, pOutboundRouteItem);

	delete pALTObj;
}

void COutboundRouteInSim::AddGroundRouteItemInfoToOutboundRouteBranchInsim(GroundRouteItem *pGRItem, int nIntersectNodeID, 
																		   CInOutboundRouteBranchInSim *pOutboundRouteBranchInSim,  COutboundRouteItem *pOutboundRouteItem)
{
	ASSERT(pGRItem);
	ASSERT(pOutboundRouteBranchInSim);
	ASSERT(pOutboundRouteItem);

	CInOutboundRouteBranchInSim *pCurOutboundRouteBranchInSim = new CInOutboundRouteBranchInSim(*pOutboundRouteBranchInSim);

	ALTObject *pALTObj = ALTObject::ReadObjectByID(pGRItem->GetALTObjectID());
	ASSERT(pALTObj);

	//if the item is the end of the groundroute
	if (0 == pGRItem->GetChildCount())
	{
		switch(pALTObj->GetType())
		{
		case ALT_TAXIWAY:
			{
				Taxiway *pTw = (Taxiway *)pALTObj;
				AddTaxiwayInfoToOutboundRouteBranchInSim(pOutboundRouteBranchInSim, pTw, nIntersectNodeID, pOutboundRouteItem);
			}
			break;

		case ALT_RUNWAY:
			ASSERT(FALSE);
			break;

		default:
			ASSERT(FALSE);
			break;
		}
	}
	else
	{
		for (int i=0; i<pGRItem->GetChildCount(); i++)
		{
			GroundRouteItem *pChildGRItem = pGRItem->GetChildItem(i);

			int nChildALTObjID = pChildGRItem->GetALTObjectID();
			ALTObject *pChildALTObj = ALTObject::ReadObjectByID(nChildALTObjID);

			switch(pChildALTObj->GetType())
			{
			case ALT_TAXIWAY:
				{
					Taxiway *pChildTw = (Taxiway *)pChildALTObj;
					switch(pALTObj->GetType())
					{
					case ALT_TAXIWAY:
						{
							Taxiway *pTw = (Taxiway *)pALTObj;

							IntersectionNodeList twINodeList;
							m_pAirportGroundNetWork->GetTaxiwayTaxiwayIntersectNodeList(pChildTw, pTw, &twINodeList);
							ASSERT(0 < (int)twINodeList.size());

							IntersectionNode iNode = twINodeList.at(0);
							//IntersectionNode childEqualNode;
							//BOOL bRet = m_pAirportGroundNetWork->FindEqualNode(pTw, &iNode, childEqualNode);
							//ASSERT(bRet);

							//more than one child
							if (0<i)
							{
								CInOutboundRouteBranchInSim *pNewBranchInSim = new CInOutboundRouteBranchInSim(*pCurOutboundRouteBranchInSim);
								m_OutboundRouteBranchInSimList.push_back(pNewBranchInSim);

								AddTaxiwaySegmentToOutboundRouteBranchInSim(pNewBranchInSim, pTw->getID(), nIntersectNodeID, iNode.GetID());
								AddGroundRouteItemInfoToOutboundRouteBranchInsim(pChildGRItem, iNode.GetID(), pNewBranchInSim, pOutboundRouteItem);
							}
							//the first child
							else
							{
								SetTaxiwayIntersectStandID(pTw, nIntersectNodeID, pOutboundRouteBranchInSim);
								AddTaxiwaySegmentToOutboundRouteBranchInSim(pOutboundRouteBranchInSim, pTw->getID(), nIntersectNodeID, iNode.GetID());
								AddGroundRouteItemInfoToOutboundRouteBranchInsim(pChildGRItem, iNode.GetID(), pOutboundRouteBranchInSim, pOutboundRouteItem);
							}

						}
						break;

					case ALT_RUNWAY:
						ASSERT(FALSE);
						break;

					default:
						ASSERT(FALSE);
						break;
					}
				}
				break;

			case ALT_RUNWAY:
				ASSERT(FALSE);
				break;

			default:
				ASSERT(FALSE);
				break;
			}

			delete pChildALTObj;
		}
	}

	delete pCurOutboundRouteBranchInSim;
	delete pALTObj;
}