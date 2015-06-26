#include "StdAfx.h"
#include ".\landsideresourcemanager.h"
#include "BoostPathFinder.h"
#include "Landside/Landside.h"
#include "LandsideParkingLotInSim.h"
#include "WalkwayInSim.h"
#include "CrossWalkInSim.h"
#include "LandsideBusStationInSim.h"
#include "LandsideIntersectionInSim.h"
#include "LandsideExternalNodeInSim.h"
#include "LandsideRoundaboutInSim.h"
#include "Landside/StretchSpeedConstraintList.h"
#include "common/CPPUtil.h"
#include "landside/FacilityBehavior.h"
#include "LandsideStretchSegmentInSim.h"
#include "Landside/LandsideWalkway.h"
#include "LandsideTaxiQueueInSim.h"
#include "LandsideTaxiPoolInSim.h"
#include "LandsideDecisionPointInSim.h"
#include "Landside/VehicleBehaviorDistributions.h"
class LandsideTaxiPool;

using namespace cpputil;

template<class Tlist>
void InitGraphLink(LandsideRouteGraph* pGraph, Tlist& tlist)
{
	for( int nodeidx = 0; nodeidx <(int)tlist.size();nodeidx++)
	{		
		tlist.at(nodeidx)->InitGraphLink(pGraph);
	}
}
template<class Tlist>
void InitGraphNodes(LandsideRouteGraph* pGraph, Tlist& tlist)
{
	for( int nodeidx = 0; nodeidx <(int)tlist.size();nodeidx++)
	{		
		tlist.at(nodeidx)->InitGraphNodes(pGraph);
	}
}

//////////////////////////////////////////////////////////////////////////
void LandsideRouteGraph::BuildGraph( LandsideResourceManager* allResource )
{
	//init node linkage
	InitGraphNodes(this,allResource->m_vStretches);
	InitGraphNodes(this,allResource->m_vRoundAbouts);


	//init route finder	
	{
		CBoostDijPathFinder* pDijFinder = new CBoostDijPathFinder();
		mpPathFinder = pDijFinder;
	}

	//////////////////////////////////////////////////////////////////////////
	CBoostDirectedGraph& bg = mpPathFinder->mGraph;
	bg.Init(mNodes.size());

	//init boost graph edges
	std::vector<LandsideStretchInSim*>& vStrethes= allResource->m_vStretches;
	std::vector<LandsideIntersectionInSim*>& vIntersections = allResource->m_Intersections;
	InitGraphLink(this,vStrethes);
	InitGraphLink(this,vIntersections);
	InitGraphLink(this,allResource->m_vRoundAbouts);
}



bool LandsideRouteGraph::FindRouteResToRes( LandsideResourceInSim * pResFrom, LandsideResourceInSim* pResTo, LandsideLaneNodeList& routeRet )
{
	LandsideLaneNodeList vFromNodes;
	LandsideLaneNodeList vToNodes;

	_GetEntryExitNode(pResFrom,pResTo,vFromNodes,vToNodes);	
	DistanceUnit dist;
	return _FindRouteNodesToNodes(vFromNodes,vToNodes,routeRet,dist);	
}

bool LandsideRouteGraph::_FindRouteNodeToNode( LandsideLaneNodeInSim* pNodeF,LandsideLaneNodeInSim* pNodeT, LandsideLaneNodeList& routeRet,DistanceUnit& dist )
{
	int srcIndex = pNodeF->m_Index;
	int destIndex = pNodeT->m_Index; 

	ASSERT(srcIndex>=0 && destIndex>=0);
	ASSERT(mpPathFinder);

	if(mpPathFinder && srcIndex>=0 && destIndex>=0 )
	{
		CBoostDirectedGraph& bg = mpPathFinder->mGraph;
		std::vector<myvertex_descriptor> vRetPath;
		//dist = (std::numeric_limits<double>::max)();
		if( mpPathFinder->FindPath(srcIndex, destIndex, vRetPath, dist) )
		{
			routeRet.clear();
			for(int i=0;i<(int)vRetPath.size();i++)
			{
				int nNodeidx1 = vRetPath[i];				
				LandsideLaneNodeInSim* pNode1 = GetNode(nNodeidx1);				
				routeRet.push_back( pNode1 );
			}
			return true;
		}
	}
	return false;
}

bool LandsideRouteGraph::FindRouteNodeToRes( LandsideLaneNodeInSim* pFromNode, LandsideResourceInSim* pDestRes, LandsideLaneNodeList& routeRet )
{
	LandsideLaneNodeList vFromNodes;
	LandsideLaneNodeList vToNodes;

	_GetEntryExitNode(NULL,pDestRes,vFromNodes,vToNodes);
	
	vFromNodes.clear();
	vFromNodes.push_back(pFromNode);
	DistanceUnit dist;
	return _FindRouteNodesToNodes(vFromNodes,vToNodes,routeRet,dist);	
}

bool LandsideRouteGraph::FindRouteResToNode( LandsideResourceInSim* pFromRes , LandsideLaneNodeInSim* pDsetNode, LandsideLaneNodeList& routeRet )
{
	LandsideLaneNodeList vFromNodes;
	LandsideLaneNodeList vToNodes;
	_GetEntryExitNode(pFromRes,NULL,vFromNodes,vToNodes);	
	vToNodes.clear();
	vToNodes.push_back(pDsetNode);
	DistanceUnit dist;
	return _FindRouteNodesToNodes(vFromNodes,vToNodes,routeRet,dist);
}


void LandsideRouteGraph::_GetEntryExitNode( LandsideResourceInSim * pResFrom, LandsideResourceInSim* pResTo,LandsideLaneNodeList& vFromNodes,LandsideLaneNodeList& vToNodes )
{
	for(LandsideLaneNodeList::iterator nodeItr = mNodes.begin();  nodeItr!= mNodes.end();nodeItr++)
	{
		LandsideLaneNodeInSim* pNode = *nodeItr;
		if(pResFrom &&  pNode->getFromRes() == pResFrom )
		{
			vFromNodes.push_back(pNode);
		}
		if( pResTo && pNode->getToRes() == pResTo )
		{
			vToNodes.push_back(pNode);
		}
	}
}

void LandsideRouteGraph::_GetEntryExitNodeByObject( LandsideResourceInSim * pResFrom, LandsideLayoutObjectInSim* pDest,LandsideLaneNodeList& vFromNodes,LandsideLaneNodeList& vToNodes )
{
	for(LandsideLaneNodeList::iterator nodeItr = mNodes.begin();  nodeItr!= mNodes.end();nodeItr++)
	{
		LandsideLaneNodeInSim* pNode = *nodeItr;
		if(pResFrom &&  pNode->getFromRes() == pResFrom )
		{
			vFromNodes.push_back(pNode);
		}
		if( pDest && pNode->getToRes() && pNode->getToRes()->getLayoutObject() == pDest )
		{
			vToNodes.push_back(pNode);
		}
	}
}

void LandsideRouteGraph::Clear()
{
	mNodes.clear();
	delete mpPathFinder;
	mpPathFinder = NULL;
}

LandsideRouteGraph::LandsideRouteGraph()
{
	mpPathFinder = NULL;
}

void LandsideRouteGraph::AddEdge( LandsideLaneNodeInSim*pNode1, LandsideLaneNodeInSim*pNode2, DistanceUnit dist )
{
	ASSERT(pNode1->m_Index>=0);
	ASSERT(pNode2->m_Index>=0);
	CBoostDirectedGraph& bg = mpPathFinder->mGraph;
	int nodeidx1 = (pNode1->m_Index);
	int nodeidx2 = (pNode2->m_Index);
	bg.AddEdge(nodeidx1, nodeidx2, dist );
}

void LandsideRouteGraph::AddNode( LandsideLaneNodeInSim* pNode )
{
	ASSERT(pNode->getFromRes()!=pNode->getToRes());
	pNode->m_Index = mNodes.size();
	mNodes.push_back(pNode);
}

bool LandsideRouteGraph::FindRouteStretchPosToRes( LandsideLaneInSim* plane, DistanceUnit distInLane, LandsideResourceInSim* pRes, LandsideLaneNodeList& routeRet )
{
	if(distInLane >= plane->GetLength() )
	{
		//ASSERT(false);
		distInLane = plane->GetLength()-1;
	}
	std::vector<LandsideLaneExit*> vLaneExit = plane->GetLaneExitsAfterDist(distInLane);
		 
	for(size_t i=0;i<vLaneExit.size();i++)
	{
		LandsideLaneExit*pExit = vLaneExit[i];
		if( pExit->getToRes()==pRes )
		{
			routeRet.push_back(pExit);
			return true;
		}
	 }
	 LandsideLaneNodeList vFromNodes;
	 LandsideLaneNodeList vToNodes;
	 _GetEntryExitNode(NULL,pRes,vFromNodes,vToNodes);	
	 vFromNodes.clear();
	 vFromNodes.insert(vFromNodes.begin(),vLaneExit.begin(),vLaneExit.end());
	 DistanceUnit dist;
	 return _FindRouteNodesToNodes(vFromNodes,vToNodes,routeRet,dist);
}

bool LandsideRouteGraph::_FindRouteNodesToNodes( const LandsideLaneNodeList& vNodesF,const LandsideLaneNodeList& vNodesT, LandsideLaneNodeList& routeRet,DistanceUnit& shortestDist )
{	
	bool bGetRoute = false;
	int iShortestLaneCountToRes = 0;
	for(size_t i=0;i<vNodesF.size();i++)
	{
		LandsideLaneNodeInSim* pNodeF = vNodesF.at(i);
		for(size_t j=0;j<vNodesT.size();j++)
		{
			LandsideLaneNodeInSim*pNodeT = vNodesT.at(j);
			LandsideLaneExit* pExit = pNodeT->toExit();
			int iLaneCountToCurbside = 0;
			if (pExit)
			{
				iLaneCountToCurbside = pExit->GetLaneCountToRes();
			}
			LandsideLaneNodeList routepath;
			DistanceUnit pathDist ;
			if(_FindRouteNodeToNode(pNodeF,pNodeT,routepath,pathDist))
			{
				if(!bGetRoute)
				{
					shortestDist = pathDist;
					routeRet = routepath;
					bGetRoute = true;
					iShortestLaneCountToRes = iLaneCountToCurbside;
				}
				else if (iShortestLaneCountToRes > iLaneCountToCurbside)
				{
					iShortestLaneCountToRes = iLaneCountToCurbside;
					shortestDist = pathDist;
					routeRet = routepath;
				}
				else if (iShortestLaneCountToRes == iLaneCountToCurbside)
				{
					if (pathDist < shortestDist)
					{
						shortestDist = pathDist;
						routeRet = routepath;
					}
				}
				//else if(pathDist < shortestDist)
				//{
				//	shortestDist = pathDist;
				//	routeRet = routepath;
				//}
			}
		}
	}
	return bGetRoute;

}


LandsideLeadToInfo& LandsideRouteGraph::getLeadToInfo( LandsideLaneNodeInSim* pNode, LandsideLayoutObjectInSim*pDest )
{
	KeyNodeObject key(pNode,pDest);
	NodeLeadToMap::iterator itrFind = m_leadtoMaps.find( key );
	if( itrFind!=m_leadtoMaps.end() )
	{
		return itrFind->second;
	}	

	
	//
	LandsideLeadToInfo& leadinfo = m_leadtoMaps[key];
	if(LandsideResourceInSim* pRes = pNode->getToRes())
	{
		if(pRes->getLayoutObject() == pDest)
		{
			leadinfo.bDirectTo = true;
			leadinfo.distDirectTo = 0;
			return leadinfo;
		}
	}

	//get min to 
	LandsideLeadToInfo minleadInfo;
	OutEdgeList outList = _getOutEdge(pNode);
	for(int i=0;i<(int)outList.size();i++)
	{
		OutEdge& outedge = outList[i];
		LandsideLeadToInfo& ileadinfo = getLeadToInfo(outedge.pNode,pDest);
		if(ileadinfo.bDirectTo)
		{
			DistanceUnit distAcc =  outedge.distToNode + ileadinfo.distDirectTo;
			if(!minleadInfo.bDirectTo)
			{
				minleadInfo.bDirectTo = true;
				minleadInfo.distDirectTo = distAcc;
			}
			else 
			{
				if(  distAcc < minleadInfo.distDirectTo )
				{
					minleadInfo.distDirectTo = distAcc;
				}
			}
		}	
	}	
	//
	leadinfo = minleadInfo;
	return leadinfo;
}
//
//bool LandsideRouteGraph::isNodeDirectLeadTo( LandsideLaneNodeInSim* pNode, LandsideLayoutObjectInSim* pDest, DistanceUnit& dist)
//{
//	LandsideLaneNodeList vFromNodes;
//	LandsideLaneNodeList vToNodes;
//	vFromNodes.push_back(pNode);
//	_GetDestNode(pDest,vToNodes);
//
//	LandsideLaneNodeList ret;
//	return FindRouteNodesToNodes(vFromNodes,vToNodes,ret,dist);
//}



LandsideRouteGraph::OutEdgeList LandsideRouteGraph::_getOutEdge( LandsideLaneNodeInSim* pNode ) const
{
	OutEdgeList ret;
	out_edgelist outedges = mpPathFinder->mGraph.GetOutEdges(pNode->m_Index);
	ret.resize(outedges.size());
	for(int i=0;i<(int)outedges.size();i++)
	{
		out_edge& oute = outedges[i];
		OutEdge& rete = ret[i];
		rete.pNode = GetNode(oute.first);
		rete.distToNode = oute.second;		
	}
	return ret;
}

void LandsideRouteGraph::GetExitResourceNodes( LandsideResourceInSim* pResFrom, LandsideLaneNodeList& outNodes )
{
	for(LandsideLaneNodeList::iterator nodeItr = mNodes.begin();  nodeItr!= mNodes.end();nodeItr++)
	{
		LandsideLaneNodeInSim* pNode = *nodeItr;
		if(pResFrom &&  pNode->getFromRes() == pResFrom )
		{
			outNodes.push_back(pNode);
		}		
	}
}

bool LandsideRouteGraph::FindRouteResToObject( LandsideResourceInSim * pResFrom, LandsideLayoutObjectInSim* pResTo, LandsideLaneNodeList& routeRet )
{
	LandsideLaneNodeList vFromNodes;
	LandsideLaneNodeList vToNodes;

	_GetEntryExitNodeByObject(pResFrom,pResTo,vFromNodes,vToNodes);	
	DistanceUnit dist;
	return _FindRouteNodesToNodes(vFromNodes,vToNodes,routeRet,dist);	
}

//////////////////////////////////////////////////////////////////////////

void LandsideResourceManager::Clear()
{
	deletePtrVector(m_vStretches);
	deletePtrVector(m_vRoundAbouts);
	deletePtrVector(m_extNodes);
	deletePtrVector(m_Curbsides);

	//other data
	//autoPtrReset(m_pSpeedConstraint);
}


template<class T>
void InitRelateWithLane(std::vector<T*>& vTlist, LandsideResourceManager* pRes)
{
	for(size_t i=0;i<vTlist.size();i++)
	{
		T* pT = vTlist[i];
		pT->InitRelateLanes(pRes);
	}
}
template <class Tlist>
void InitRelateWithOtherObj(Tlist& vTlist, LandsideResourceManager* pRes)
{
	for(size_t i=0;i<vTlist.size();i++)
	{	
		vTlist.at(i)->InitRelateWithOtherObject(pRes);
	}
}

template<class Tlist>
void InitBehavior(Tlist& vTlist,ALTOBJECT_TYPE objType, CFacilityBehaviorList* pBehaior, StretchSpeedControlDataList& spdControl, VehicleBehaviorDistributions* serviceTime)
{
	std::vector<CFacilityBehavior*> blist;
	if( pBehaior->getBehaviorList(objType,blist) )
	{
		for(size_t i=0;i<vTlist.size();i++)
		{
			LandsideLayoutObjectInSim* pObj = vTlist[i];
			for(size_t k=0;k<blist.size();++k)
			{
				const ALTObjectID& objname = pObj->getName();
				CFacilityBehavior* pb = blist[k];
				if( objname.idFits(pb->getFacilityName()) )
				{
					pObj->SetBehavior(pb);
				}				
			}
		}
	}
	//speed control
	for(size_t i=0;i<vTlist.size();i++)
	{
		LandsideLayoutObjectInSim* pObj = vTlist[i];		
		ALTObjectID& objname = pObj->getInput()->getName();		
		if( StretchSpeedControlData* pData = spdControl.FindMatch(objname) )
		{
			pObj->SetSpeedControlData(pData);
		}			
		
	}
	//service time
	for(size_t i=0;i<vTlist.size();i++)
	{
		LandsideLayoutObjectInSim* pObj = vTlist[i];		
		ALTObjectID& objname = pObj->getInput()->getName();		
		if( VBDistributionObject* pData = serviceTime->FindMatch(objname) )
		{
			pObj->SetServiceTime(pData);
		}			

	}
}


void LandsideResourceManager::Init( InputLandside* pInput ,CFacilityBehaviorList* pBehaior)
{
	bool bLeftDrive = pInput->IsLeftDrive();

	LandsideFacilityLayoutObjectList& layoutObjlist=  pInput->getObjectList();
	for(int i=0;i<layoutObjlist.getCount();i++)
	{
		CreatInSimObject(layoutObjlist.getObject(i),bLeftDrive);	
	}
	//init graph nodes
	InitRelateWithOtherObj(m_vStretches,this);
	InitRelateWithOtherObj(m_Intersections,this);
	InitRelateWithOtherObj(m_ParkingLots,this);
	InitRelateWithOtherObj(m_Curbsides,this);
	InitRelateWithOtherObj(m_extNodes,this);
	InitRelateWithOtherObj(m_vRoundAbouts,this);
	InitRelateWithOtherObj(m_BusStation,this);
	InitRelateWithOtherObj(m_vTaxiPools,this);
	InitRelateWithOtherObj(m_vTaxiQueues,this);
	InitRelateWithOtherObj(m_vDecidePoints,this);

	m_vTrafficObjectList.InitRelateWithOtherRes(this);
	
	//init route graph
	mRouteGraph.BuildGraph(this);

	//init parking lot constraint
	for (int i = 0; i < (int)m_ParkingLots.size(); i++)
	{
		LandsideParkingLotInSim* pParkingLotInSim = m_ParkingLots.at(i);
		pParkingLotInSim->SetParkingLotConstraint(pInput->getParkingSpotConstraint());
	}
	//init facility behaviors;
	StretchSpeedControlDataList& spdControlData = pInput->getStretchSpeedControlDataList();
	VehicleBehaviorDistributions* pServiceT = pInput->getServiceTimeList();

	InitBehavior(m_vStretches,		ALT_LSTRETCH,		pBehaior, spdControlData ,pServiceT);
	InitBehavior(m_vStretchSegs,    ALT_LSTRETCHSEGMENT,pBehaior, spdControlData ,pServiceT);
	InitBehavior(m_Intersections,	ALT_LINTERSECTION,	pBehaior, spdControlData ,pServiceT);
	InitBehavior(m_Curbsides,		ALT_LCURBSIDE,		pBehaior, spdControlData ,pServiceT);
	InitBehavior(m_BusStation,		ALT_LBUSSTATION,	pBehaior, spdControlData ,pServiceT);
	InitBehavior(m_ParkingLots,		ALT_LPARKINGLOT,	pBehaior, spdControlData, pServiceT);
	InitBehavior(m_extNodes,		ALT_LEXT_NODE,		pBehaior, spdControlData, pServiceT);
	InitBehavior(m_vRoundAbouts,	ALT_LROUNDABOUT,	pBehaior, spdControlData, pServiceT);
	InitBehavior(m_vTaxiQueues,		ALT_LTAXIQUEUE,		pBehaior, spdControlData, pServiceT);
	InitBehavior(m_vTaxiPools,		ALT_LTAXIPOOL,		pBehaior, spdControlData, pServiceT);
	InitBehavior(m_vDecidePoints,	ALT_LDECISIONPOINT,	pBehaior, spdControlData, pServiceT);
}

void LandsideResourceManager::CreatInSimObject( LandsideFacilityLayoutObject* pObj,bool bLeftDrive)
{
	switch(pObj->GetType())
	{
	case ALT_LSTRETCH:
		{
			LandsideStretch* pStretch = (LandsideStretch*)pObj;
			if(pStretch->getControlPath().getCount()>1)
			{
				m_vStretches.push_back(new LandsideStretchInSim(pStretch));
			}
		}
		
		break;
	case ALT_LINTERSECTION:
		m_Intersections.push_back(new LandsideIntersectionInSim((LandsideIntersectionNode*)pObj));
		break;
	case ALT_LCURBSIDE:
		m_Curbsides.push_back(new LandsideCurbSideInSim((LandsideCurbSide*)pObj,bLeftDrive));
		break;
	case ALT_LBUSSTATION:
		m_BusStation.push_back(new LandsideBusStationInSim((LandsideBusStation*)pObj,bLeftDrive));
		break;
	case ALT_LPARKINGLOT:
		m_ParkingLots.push_back(new LandsideParkingLotInSim((LandsideParkingLot*)pObj,bLeftDrive));
		break;
	case ALT_LEXT_NODE:
		m_extNodes.push_back(new LandsideExternalNodeInSim((LandsideExternalNode*)pObj));
		break;
	case ALT_LWALKWAY:
		m_vTrafficObjectList.addItem(new CWalkwayInSim((LandsideWalkway*)pObj));
		break;
	case ALT_LCROSSWALK:
		m_vTrafficObjectList.addItem(new CCrossWalkInSim((LandsideCrosswalk*)pObj));
		break;
	case ALT_LROUNDABOUT:
		m_vRoundAbouts.push_back(new LandsideRoundaboutInSim((LandsideRoundabout*)pObj));
		break;
	case ALT_LSTRETCHSEGMENT:
		m_vStretchSegs.push_back(new LandsideStretchSegmentInSim((LandsideStretchSegment*)pObj));
		break;	
	case ALT_LTAXIQUEUE:
		m_vTaxiQueues.push_back(new LandsideTaxiQueueInSim((LandsideTaxiQueue*)pObj,bLeftDrive));
		break;
	case ALT_LTAXIPOOL:
		m_vTaxiPools.push_back(new LandsideTaxiPoolInSim((LandsideTaxiPool*)pObj));
		break;
	case ALT_LDECISIONPOINT:
		m_vDecidePoints.push_back(new LandsideDecisionPointInSim((LandsideDecisionPoint*)pObj));
		break;
	}
}

LandsideLaneNodeInSim * LandsideResourceManager::GetlaneNode( const LaneNode& node )
{
	for(size_t i=0;i<m_vStretches.size();i++)
	{
		LandsideStretchInSim* pStretch = m_vStretches[i];
		if(node.m_pStretch->getID() == pStretch->getInput()->getID())
		{
			LandsideStretchLaneInSim* pLane = pStretch->GetLane(node.nLaneIdx);
			if(pLane)
			{
				if(node.IsLaneEntry())
				{ 
					return	pLane->GetFirstLaneEntry();
				}
				if(node.IsLaneExit())
				{
					return pLane->GetLastLaneExit();
				}
			}
		}
	}
	return NULL;
}

LandsideStretchInSim* LandsideResourceManager::getStretchByID( int id )
{
	for(size_t i=0;i<m_vStretches.size();i++)
	{
		LandsideStretchInSim* pStretch = m_vStretches[i];
		if(pStretch->getInput()->getID()==id)
			return pStretch;
	}
	return NULL;
}
LandsideParkingLotInSim* LandsideResourceManager::getParkingLotByID( int id )
{
	for(size_t i=0;i<m_vStretches.size();i++)
	{
		LandsideParkingLotInSim* pParkingLot = m_ParkingLots[i];
		if(pParkingLot->getInput()->getID()==id)
			return pParkingLot;
	}
	return NULL;
}

LandsideBusStationInSim* LandsideResourceManager::getBusStationByID( int id )
{
	for(size_t i=0;i<m_BusStation.size();i++)
	{
		LandsideBusStationInSim* pBusStation = m_BusStation[i];
		if(pBusStation->getInput()->getID()==id)
			return pBusStation;
	}
	return NULL;
}

LandsideResourceManager::LandsideResourceManager()
{
	//m_pSpeedConstraint = NULL;
}

LandsideResourceManager::~LandsideResourceManager()
{
	Clear();
}

#define FINDINSIMOBJ(T, id) for(size_t i=0;i<T.size();i++){  LandsideLayoutObjectInSim* pObj = T[i];if(pObj->getInput()->getID()==id){ return pObj;}  }

template<class T>
typename T::value_type getRandomObject(const T& vObjectList, const ALTObjectID& id)
{
	T vlist;
	for(size_t i=0;i<vObjectList.size();i++){
		T::value_type pCurb = vObjectList.at(i);
		if( pCurb->getInput()->getName().idFits(id) ){
			vlist.push_back(pCurb);
		}
	}

	if(!vlist.empty()){
		int r = rand()%vlist.size();
		return vlist.at(r);
	}
	return NULL;
}
#define FINDINSIMOBJBYNAME(vlist, name)  if(LandsideLayoutObjectInSim* pObj = getRandomObject(vlist,name)){ return pObj; }


LandsideLayoutObjectInSim* LandsideResourceManager::getLayoutObjectInSim( int nObjID )
{
	FINDINSIMOBJ(m_vStretches,nObjID)
	FINDINSIMOBJ(m_Intersections,nObjID)
	FINDINSIMOBJ(m_vRoundAbouts,nObjID)
	FINDINSIMOBJ(m_Curbsides,nObjID)
	FINDINSIMOBJ(m_BusStation,nObjID)
	FINDINSIMOBJ(m_ParkingLots,nObjID)
	FINDINSIMOBJ(m_extNodes,nObjID)
	FINDINSIMOBJ(m_vTaxiPools,nObjID)
	FINDINSIMOBJ(m_vTaxiQueues,nObjID)
	FINDINSIMOBJ(m_vDecidePoints,nObjID)
	return NULL;
}

LandsideLayoutObjectInSim* LandsideResourceManager::getLayoutObjectInSim( const ALTObjectID& name )
{
	FINDINSIMOBJBYNAME(m_vStretches,name)
	FINDINSIMOBJBYNAME(m_Intersections,name)
	FINDINSIMOBJBYNAME(m_vRoundAbouts,name)
	FINDINSIMOBJBYNAME(m_Curbsides,name)
	FINDINSIMOBJBYNAME(m_BusStation,name)
	FINDINSIMOBJBYNAME(m_ParkingLots,name)
	FINDINSIMOBJBYNAME(m_extNodes,name)
	FINDINSIMOBJBYNAME(m_vTaxiPools,name)
	FINDINSIMOBJBYNAME(m_vTaxiQueues,name)
	FINDINSIMOBJBYNAME(m_vDecidePoints,name)	
	return NULL;
}

LandsideCurbSideInSim* LandsideResourceManager::getRandomCurb( const ALTObjectID& id )
{
   return getRandomObject(m_Curbsides,id);
}

LandsideBusStationInSim* LandsideResourceManager::getRandomBusStation( const ALTObjectID& id )
{
	return getRandomObject(m_BusStation,id);
}

void LandsideResourceManager::GetWalkWayListAtLevel( std::vector<CWalkwayInSim *>& vWakwayInSim, double dLevel ) const
{
	int nCount =  m_vTrafficObjectList.getCount();
	for (int nItem = 0; nItem < nCount; ++ nItem)
	{
		CLandsideWalkTrafficObjectInSim *pObject = m_vTrafficObjectList.getItem(nItem);
		if(pObject == NULL)
			continue;

		CWalkwayInSim *pWalkwayInSim = pObject->toWalkway();
		if (pWalkwayInSim)
		{
			int nLevel = static_cast<int>(dLevel);
			if(pWalkwayInSim->GetInputWalkway()->GetLevel(nLevel))
				vWakwayInSim.push_back(pWalkwayInSim);

		}
	}
}

CCrossWalkInSim* LandsideResourceManager::GetCrosswalk( LandsideCrosswalk* pCrossWalk )
{
	int nCount =  m_vTrafficObjectList.getCount();
	for (int nItem = 0; nItem < nCount; ++ nItem)
	{
		CLandsideWalkTrafficObjectInSim *pObject = m_vTrafficObjectList.getItem(nItem);
		if(pObject == NULL || pObject->GetTrafficType() != CLandsideWalkTrafficObjectInSim::CrossWalk_Type)
			continue;

		CCrossWalkInSim* pCrosswalk = pObject->toCrossWalk();
		if (pCrosswalk->GetCrosswalk()->getID() == pCrossWalk->getID())
		{
			return pCrosswalk;
		}
	}
	return NULL;
}

LandsideTaxiQueueInSim* LandsideResourceManager::getRandomTaxiQueue( const ALTObjectID& id )
{
	std::vector<LandsideTaxiQueueInSim*> vlist;
	for(size_t i=0;i<m_vTaxiQueues.size();i++)
	{
		LandsideTaxiQueueInSim* pBusStation = m_vTaxiQueues.at(i);
		if( pBusStation->getInput()->getName().idFits(id) )
		{
			vlist.push_back(pBusStation);
		}
	}

	if(!vlist.empty())
	{
		int r = rand()%vlist.size();
		return vlist.at(r);
	}
	return NULL;
}


void LandsideResourceManager::FlushOnResourcePax( CARCportEngine* pEngine, const ElapsedTime& endTime )
{
	for(size_t i=0;i<m_vTaxiQueues.size();i++)
	{
		LandsideTaxiQueueInSim* pTaxiQ = m_vTaxiQueues.at(i);
		pTaxiQ->FlushOnVehiclePaxLog(pEngine,endTime);
	}
	for(size_t i=0;i<m_BusStation.size();i++)
	{
		LandsideBusStationInSim* pBusStation = m_BusStation.at(i);
		pBusStation->FlushOnVehiclePaxLog(pEngine,endTime);
	}
	for(size_t i=0;i<m_Curbsides.size();i++){
		LandsideCurbSideInSim* pCurb = m_Curbsides.at(i);
		pCurb->FlushOnVehiclePaxLog(pEngine,endTime);
	}
}

CWalkwayInSim * LandsideResourceManager::GetWalkwayInSim( const ALTObjectID& walkWayID ) const
{
	int nCount =  m_vTrafficObjectList.getCount();
	for (int nItem = 0; nItem < nCount; ++ nItem)
	{
		CLandsideWalkTrafficObjectInSim *pObject = m_vTrafficObjectList.getItem(nItem);
		if(pObject == NULL)
			continue;

		CWalkwayInSim *pWalkwayInSim = pObject->toWalkway();
		if (pWalkwayInSim)
		{
			if(pWalkwayInSim->GetInputWalkway()->getName().idFits(walkWayID))
				return pWalkwayInSim;
		}
	}

	return NULL;
}

LandsideParkingLotInSim* LandsideResourceManager::getRandomParkinglot( const ALTObjectID& id )
{
	return getRandomObject(m_ParkingLots,id);
}


































