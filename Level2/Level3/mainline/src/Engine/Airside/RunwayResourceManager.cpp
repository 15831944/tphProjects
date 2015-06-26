#include "StdAfx.h"

#include "RunwayResourceManager.h"
#include "../../InputAirside/ALTAirport.h"
#include "TaxiwayResourceManager.h"
#include "TakeoffQueueInSim.h"
#include "RunwaySegInSim.h"
#include "FlightGroundRouteResourceManager.h"
#include "../../Common/ARCUnit.h"
//#include "../../Database/DBElementCollection_Impl.h"
#include "../../InputAirside/RunwayExitDiscription.h"

bool RunwayResourceManager::Init( int nPrjID, int nAirportID, AircraftClassificationManager* pAircraftClassification )
{
	//
	ALTObjectList vRunways;	
	
	ALTAirport::GetRunwayList(nAirportID,vRunways);

	m_GoAroundSpecs.ReadData(nPrjID);
	double dMissApporachDist = m_GoAroundSpecs.GetLBLSRWDIS();
	//dMissApporachDist = ARCUnit::ConvertLength(dMissApporachDist,ARCUnit::M,ARCUnit::CM);

	//update input inform
	//
	for(int i =0 ;i< (int)vRunways.size(); ++i )
	{
		Runway *pRunway =(Runway*) vRunways.at(i).get();	

		RunwayInSim * pRwInSim = new RunwayInSim( pRunway, dMissApporachDist);
		m_vRunways.push_back(pRwInSim);		
	}	

	m_pApproachSeparationInSim.Init(pAircraftClassification, nPrjID, vRunways);
	m_pTakeoffSeparationInSim.Init(pAircraftClassification, nPrjID, vRunways);
	m_waveCrossingSpec.ReadData();
	//update Runway intersection info
	//get runway hold position info and update runway exit
	
	

	return true;
}

// init intersection node and runway exits
bool RunwayResourceManager::InitRelations( IntersectionNodeInSimList& NodeList,
										  FlightGroundRouteResourceManager * pGroundRouteResource ,
										  TakeoffQueuesManager * pTakeoffQueues,
										  TaxiwayResourceManager& taxiwayResManager)
{
	std::vector<HoldPositionInSim> vHoldShortLineProps;
	// init
	for(int i=0;i<GetRunwayCount();i++)
	{
		//init runway segments
		m_vRunways.at(i)->InitSegments(NodeList,vHoldShortLineProps);
		//init wave crossing information
	}

	for(int i= 0 ;i< (int)m_vRunways.size(); ++i)
	{
		RunwayInSim * pRunwayInSim = m_vRunways.at(i).get();
		Runway * pRunwayInput = pRunwayInSim->GetRunwayInput();
		//pRunwayInput->UpdateRunwayExits(NodeList.GetInputNodes());

		//port1
		{
			RunwayExitList exitList;
			pRunwayInput->GetExitList(RUNWAYMARK_FIRST,exitList);			
			for(int i=0 ;i< (int) exitList.size();i++)
			{
				RunwayExit rwExit  = exitList.at(i);				
				IntersectionNodeInSim * pNode = NodeList.GetNodeByID( rwExit.GetIntersectNodeID() );
				FlightGroundRouteDirectSegList taxidirseglist = pGroundRouteResource->GetLinkedDirectSegments(pNode);
				for(int segidx = 0; segidx < (int)taxidirseglist.size();segidx++)
				{
					FlightGroundRouteDirectSegInSim* pSeg = taxidirseglist.at(segidx);
					IntersectionNodeInSim* pExitNode = pSeg->GetExitNode();

					if(pExitNode->GetNodeInput().HasObject(pRunwayInput->getID())) //ignore self seg
						continue;

					if( pRunwayInSim->GetLogicRunway1()->IsLeftPoint(pExitNode->GetDistancePoint(0)) == (rwExit.GetSideType() == RunwayExit::EXIT_LEFT )  )
					{
						pRunwayInSim->GetLogicRunway1()->m_vExitsList.push_back(RunwayExitInSim(rwExit,pSeg,pRunwayInSim->GetLogicRunway1()));
						//break;
					}
				}
			}			
			std::sort(pRunwayInSim->GetLogicRunway1()->m_vExitsList.begin(),pRunwayInSim->GetLogicRunway1()->m_vExitsList.end());
		
		}
		//port2
		{
			RunwayExitList exitList;
			pRunwayInput->GetExitList(RUNWAYMARK_SECOND,exitList);						
			for(int i=0 ;i< (int) exitList.size();i++)
			{
				RunwayExit rwExit  = exitList.at(i);
				IntersectionNodeInSim * pNode = NodeList.GetNodeByID( rwExit.GetIntersectNodeID() );
				FlightGroundRouteDirectSegList taxidirseglist = pGroundRouteResource->GetLinkedDirectSegments(pNode);
				for(int segidx = 0; segidx < (int)taxidirseglist.size();segidx++)
				{
					FlightGroundRouteDirectSegInSim* pSeg = taxidirseglist.at(segidx);
					IntersectionNodeInSim* pExitNode = pSeg->GetExitNode();

					if(pExitNode->GetNodeInput().HasObject(pRunwayInput->getID())) //ignore self seg
						continue;

					if( pRunwayInSim->GetLogicRunway2()->IsLeftPoint(pExitNode->GetDistancePoint(0)) == (rwExit.GetSideType() == RunwayExit::EXIT_LEFT )  )
					{
						pRunwayInSim->GetLogicRunway2()->m_vExitsList.push_back(RunwayExitInSim(rwExit,pSeg,pRunwayInSim->GetLogicRunway2()));
						//break;
					}
				}				
						
			}
			std::sort(pRunwayInSim->GetLogicRunway2()->m_vExitsList.begin(),pRunwayInSim->GetLogicRunway2()->m_vExitsList.end());
		}

		//add runway intersection nodes
		for(int iNodeidx = 0; iNodeidx < NodeList.GetNodeCount(); iNodeidx++)
		{
			IntersectionNodeInSim* pTheNode = NodeList.GetNodeByIndex(iNodeidx);
			if( pTheNode->GetNodeInput().HasObject(pRunwayInput->getID()) )
			{
				pRunwayInSim->GetLogicRunway1()->AddIntersectionNodeInSim(pTheNode);
				pRunwayInSim->GetLogicRunway2()->AddIntersectionNodeInSim(pTheNode);
			}
		}

		InitWaveCrossing(pRunwayInSim,taxiwayResManager);
	}

	//

	//// intersection runways
	//for(int i =0 ;i <(int)m_vRunways.size(); ++i)
	//{
	//	RunwayInSim * pRunway = m_vRunways.at(i).get();
	//	std::vector<Runway*> vIntersectRunways = pRunway->GetRunwayInput()->GetIntersectRunways();
	//	for(int j=0;j<(int)vIntersectRunways.size();j++)
	//	{
	//		RunwayInSim * pIntersectRunway = GetRunwayByID(vIntersectRunways.at(j)->getID());
	//		pRunway->GetLogicRunway1()->AddIntersectRunway(pIntersectRunway->GetLogicRunway1());
	//		pRunway->GetLogicRunway1()->AddIntersectRunway(pIntersectRunway->GetLogicRunway2());

	//		pRunway->GetLogicRunway2()->AddIntersectRunway(pIntersectRunway->GetLogicRunway1());
	//		pRunway->GetLogicRunway2()->AddIntersectRunway(pIntersectRunway->GetLogicRunway2());
	//	}
	//}
		

	return true;
}


LogicRunwayInSim * RunwayResourceManager::GetLogicRunway( int nrwId, RUNWAY_MARK rwMark )
{
	RunwayInSim * pRunway  = GetRunwayByID(nrwId);
	if(!pRunway)
		return NULL;

	if(rwMark == RUNWAYMARK_FIRST)
		return pRunway->GetLogicRunway1();
	else 
		return pRunway->GetLogicRunway2();
}

const RunwayInSim * RunwayResourceManager::GetRunwayByID( int rwid )const
{
	for(int i=0 ;i< (int)m_vRunways.size();i++)
	{
		if( m_vRunways.at(i)->GetRunwayInput()->getID() == rwid ) 
			return m_vRunways.at(i).get();
	}
	return NULL;
}
RunwayInSim * RunwayResourceManager::GetRunwayByID( int rwid )
{
	for(int i=0 ;i< (int)m_vRunways.size();i++)
	{
		if( m_vRunways.at(i)->GetRunwayInput()->getID() == rwid ) 
			return m_vRunways.at(i).get();
	}
	return NULL;
}

RunwayInSim * RunwayResourceManager::GetRunwayByIdx( int rwidx )
{
	if(rwidx < GetRunwayCount() && rwidx>=0 )
	{
		return m_vRunways[rwidx].get();
	}
	return NULL;
}

std::vector<RunwayInSim::RefPtr>& RunwayResourceManager::getRunwayResource()
{
	return m_vRunways;
}
FlightGroundRouteDirectSegList RunwayResourceManager::GetLinkedDirectSegments( IntersectionNodeInSim * pNode )
{
	FlightGroundRouteDirectSegList reslt;
	for(int i =0 ;i< GetRunwayCount(); i++)
	{
		RunwayInSim* pRunway = GetRunwayByIdx(i);
		for(int j =0 ; j <pRunway->GetSegmentCount();j++ )
		{
			RunwaySegInSim * pSeg = pRunway->GetSegment(j);
			if( pSeg->GetPositiveDirSeg() )
			{
				if( pNode == pSeg->GetNode1() )
				{
					reslt.push_back(pSeg->GetPositiveDirSeg());
				}
			}
			if( pSeg->GetNegativeDirSeg() )
			{
				if(pNode == pSeg->GetNode2() )
				{
					reslt.push_back(pSeg->GetNegativeDirSeg());
				}
			}
		}
	}

	return reslt;
}
//
FlightGroundRouteDirectSegList RunwayResourceManager::GetAllLinkedDirectSegments( IntersectionNodeInSim * pNode )
{
	FlightGroundRouteDirectSegList reslt;
	for(int i =0 ;i< GetRunwayCount(); i++)
	{
		RunwayInSim* pRuniway = GetRunwayByIdx(i);
		for(int j =0 ; j <pRuniway->GetSegmentCount();j++ )
		{
			RunwaySegInSim * pSeg = pRuniway->GetSegment(j);

			if( pNode == pSeg->GetNode1() || pNode == pSeg->GetNode2() )
			{
				reslt.push_back(pSeg->GetPositiveDirSeg());
				reslt.push_back(pSeg->GetNegativeDirSeg());
			}
		}
	}

	return reslt;
}

void RunwayResourceManager::GetRunwaySegments( int nRunwayID, int nIntersectNodeIDFrom, int nIntersectNodeIDTo, FlightGroundRouteDirectSegList& taxiwayDirectSegLst )
{
	if (nRunwayID < 0 || nIntersectNodeIDFrom < 0 || nIntersectNodeIDTo < 0)
		return;

	RunwayInSim* pRunway = GetRunwayByID(nRunwayID);
	if(pRunway)
	{
		return pRunway->GetRunwayDirectSegmentList(nIntersectNodeIDFrom, nIntersectNodeIDTo, taxiwayDirectSegLst);
	}	
}
//init runway's wave crossing specification
void RunwayResourceManager::InitWaveCrossing(RunwayInSim * pRunwayInSim, TaxiwayResourceManager& taxiwayResManager )
{

	if(pRunwayInSim == NULL)
		return;

	LogicRunwayInSim* pLogicRunway = pRunwayInSim->GetLogicRunway1();

	if(pLogicRunway == NULL)
		return;

	//init runway's hold positions, filter from taxiway's hold position in simulation 
	IntersectionNodeInSimList& InterNodeList = pLogicRunway->GetIntersectionList();
	for (int nNode = 0; nNode < InterNodeList.GetNodeCount(); ++ nNode)
	{
		//get intersection node
		IntersectionNodeInSim *pNodeInSim = InterNodeList.GetNodeByIndex(nNode);
		if(pNodeInSim)
		{
			//intersection node id
			int nIntID = pNodeInSim->GetNodeInput().GetID();
			
			//the intersection items
			std::vector<IntersectItem*> vIntersecItems = pNodeInSim->GetNodeInput().GetTypeItemList(IntersectItem::TAXIWAY_INTERSECT);
			for (int nItem = 0; nItem < static_cast<int>(vIntersecItems.size());++nItem)
			{
				int nTaxiwayID = vIntersecItems.at(nItem)->GetObjectID();
				std::vector<HoldPositionInSim *> vHoldPosition = taxiwayResManager.GetHoldPositionInSim(nTaxiwayID,nIntID);
			
				std::vector<HoldPositionInSim *>::iterator iter = vHoldPosition.begin();

				for (; iter != vHoldPosition.end(); ++iter)
				{
					if(*iter)
					{
						(*iter)->SetNode(pNodeInSim);
						(*iter)->SetRunwayInSim(pRunwayInSim);
					}
					pRunwayInSim->AddTaxiwayHoldPosition(*iter);
				}
			}
		}
	}
	//init runway's wave crossing wave crossing specification
	int nItemCount = m_waveCrossingSpec.GetElementCount();
	for (int nWave = 0; nWave < nItemCount; ++nWave)
	{
		 CWaveCrossingSpecificationItem *pWaveItem = m_waveCrossingSpec.GetItem(nWave);
		 if (pWaveItem->GetHoldCount() > 0)
		 {
			 HoldPosition *pHoldPosition = pWaveItem->GetHoldPosition(0);
			 if(pHoldPosition)
			 {
				 //check the runway has this hold position or not, if have add this item specification to runway
				if(pRunwayInSim->GetHoldPosition(pHoldPosition->GetUniqueID()) != NULL)
					pRunwayInSim->SetWaveCrossSpecification(pWaveItem);

			 }
		 }
	}
}

RunwayResourceManager::~RunwayResourceManager()
{
}

RunwayExitInSim* RunwayResourceManager::GetExitByRunwayExitDescription( const RunwayExitDescription& runwayDes )
{
	if(LogicRunwayInSim* pLRnway = GetLogicRunway(runwayDes.GetRunwayID(),runwayDes.GetRunwayMark()))
	{
		return pLRnway->GetExitByRunwayExitDescription(runwayDes);
	}
	return NULL;
}

RunwayExitInSim* RunwayResourceManager::GetRunwayExitByID( int nExitID )
{
	for (size_t i = 0; i < m_vRunways.size(); i++)
	{
		RunwayInSim* pRunwayInSim = m_vRunways[i].get();
		LogicRunwayInSim* pLRunway = pRunwayInSim->GetLogicRunway1();
		LogicRunwayInSim* pRRunway = pRunwayInSim->GetLogicRunway2();
		
		//find in logic1
		RunwayExitInSim* pLRunwayExitInSim = pLRunway->GetExitByID(nExitID);
		if (pLRunwayExitInSim)
		{
			return pLRunwayExitInSim;
		}

		//find in logic2
		RunwayExitInSim* pRRunwayExitInSim = pRRunway->GetExitByID(nExitID);
		if (pRRunwayExitInSim)
		{
			return pRRunwayExitInSim;
		}
	}
	return NULL;
}

bool RunwayResourceManager::IsNodeOnRunway(int nRunwayID, int nIntersectNodeID) const
{
	const RunwayInSim* pRunway = GetRunwayByID(nRunwayID);
	if (pRunway)
	{
		IntersectionNodeInSimList vNodeList = pRunway->GetIntersectionNodeList();
		int nCount = vNodeList.GetNodeCount();
		for (int i=0;i<nCount;i++)
		{
			if ( vNodeList.GetNodeByIndex(i)->GetID() == nIntersectNodeID )
			{
				return true;
			}
		}
	}
	return false;
}


