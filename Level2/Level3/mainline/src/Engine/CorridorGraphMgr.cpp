#include "stdafx.h"
#include "CorridorGraphMgr.h"
#include <limits>
#include <boost\graph\dijkstra_shortest_paths.hpp>
#include <boost\graph\connected_components.hpp>
#include "Common\exeption.h"
#include "Common\Path2008.h"
#include "InputOnBoard\OnBoardBaseVerticalDevice.h"
#include "InputOnBoard\Seat.h"
#include "InputOnBoard\Door.h"
#include "OnboardFlightInSim.h"
#include "InputOnBoard/Deck.h"
#include "OnboardSeatInSim.h"
#include "OnboardDoorInSim.h"
#include "OnBoardingStairInSim.h"
#include "OnBoardingElevatorInSim.h"
#include "OnBoardingEscalatorInSim.h"
#include "OnBoardDeviceInSim.h"
#include "EntryPointInSim.h"
#include "../Common/RayIntersectPath.h"
#include "PaxOnboardBaseBehavior.h"
//////////////////////////////////////////////////////////////////////////////////////////
//SeatArea////////////////////////////////////////////////////////////////////////////////
SeatArea::SeatArea(int nDeck)
:m_nDeckID(nDeck)
{

}

SeatArea::~SeatArea()
{

}

bool SeatArea::intersectionArea(const CPoint2008& sourcePt,const CPoint2008& destPt)
{
	for (int i = 0; i < (int)m_rgn.size(); i++)
	{
		CPollygon2008& pollygon = m_rgn[i];
		if (pollygon.intersects(sourcePt,destPt))
		{
			return true;
		}
	}
	return false;
}

void SeatArea::InsertPollygon(const CPollygon2008& pollygon)
{
	m_rgn.push_back(pollygon);
}

void SeatArea::clear()
{
	m_rgn.clear();
}
//////CorridorGraph///////////////////////////////////////////////////////////////////////
CorridorGraph::CorridorGraph(CorridorGraphMgr* pCorridorGraphMgr,int nDeck)
:m_pCorridorGraphMgr(pCorridorGraphMgr)
,m_nID(nDeck)
,m_areaList(nDeck)
{

}

CorridorGraph::~CorridorGraph()
{
	clearData();
}

void CorridorGraph::clearData()
{
	for (size_t i = 0; i < m_vSingleGraph.size(); i++)
	{
		delete m_vSingleGraph[i];
	}
	m_vSingleGraph.clear();
}

bool CorridorGraph::BuildCorridorGraph()
{
	//init seat area
 	InitArea();


	int sizeCount = (int)m_vSingleGraph.size();
	if (sizeCount < 0)
		return false;
	
	for (int i = 0; i < sizeCount; i++)
	{
		SingleGraph* pSingleGrap = m_vSingleGraph[i];
		pSingleGrap->BuildSingleGraph();
	}
	return true;
}

bool CorridorGraph::InitEntryPoint(SingleSeatRowGraph* pSingleSeatRowGraph)
{
	//int sizeOfCount = (int)m_vSingleGraph.size();
	//if( sizeOfCount <= 0)
	//	return false;

	//for (int i = 0; i < pSingleSeatRowGraph->GetSeatCountInRow(); i++)
	//{
	//	double dShortestDist = (std::numeric_limits<double>::max)();
	//	OnboardCorridorInSim* pShortestCorridor = NULL;
	//	double dDist = 0.0;
	//	int nCorridorSeg = 0;
	//	OnboardSeatInSim* pShortSeatInSim = NULL;

	//	for (int j = 0; j < sizeOfCount; j++)
	//	{
	//		SingleGraph* pSingleGraph = m_vSingleGraph[j];
	//		if (pSingleGraph == NULL ||!pSingleGraph->IsCorridor())
	//			continue;
	//		SingleCorridorGraph* pSingleCorridorGraph = (SingleCorridorGraph*)pSingleGraph;
	//		OnboardCorridorInSim* corridorInSim = pSingleCorridorGraph->GetCorridorInSim();
	//		
	//		OnboardSeatInSim* pSeatInSim = pSingleSeatRowGraph->GetSeatInSim(i);
	//		ARCVector3 seatPt;
	//		pSeatInSim->GetPosition(seatPt);
	//		ARCVector3 entryPt = pSeatInSim->GetFrontPosition();
	//		ARCVector3 vDir(entryPt-seatPt);
	//		vDir.Normalize();
	//		int nSegment;
	//		double dDistanceFromStart = 0.0;
	//		double dDistance = BuildEntryPoint(entryPt,vDir,dDistanceFromStart,nSegment,pSingleSeatRowGraph,pSingleCorridorGraph);
	//		if (dDistance < dShortestDist)
	//		{
	//			dShortestDist = dDistance;
	//			pShortestCorridor = corridorInSim;
	//			dDist = dDistanceFromStart;
	//			nCorridorSeg = nSegment;
	//			pShortSeatInSim= pSeatInSim;
	//		}
	//	}
	//	if (pShortestCorridor)
	//	{
	//		ARCVector3 entryPoint = pShortestCorridor->GetDistancePoint(dDist);
	//		double dLeftPathDist = (std::numeric_limits<double>::max)();
	//		double dRightPathDist = (std::numeric_limits<double>::max)();

	//		CPoint2008 leftPt, rightPt;
	//		int nLeftSegment = 0;
	//		int nRightSegment = 0;

	//		ARCVector3 seatPos = pShortSeatInSim->GetFrontPosition();

	//		CPoint2008 ptSource(seatPos.n[VX],seatPos.n[VY],0.0);
	//		pShortestCorridor->GetShortestDistancePoint(ptSource,leftPt,pShortestCorridor->m_pCorridor->GetPathLeftLine(),nLeftSegment,dLeftPathDist);
	//		pShortestCorridor->GetShortestDistancePoint(ptSource,rightPt,pShortestCorridor->m_pCorridor->GetPathRightLine(),nRightSegment,dRightPathDist);

	//		ARCVector3 corridorPt = dLeftPathDist < dRightPathDist ?leftPt:rightPt;
	//		corridorPt.n[VZ] = seatPos.n[VZ];

	//		ARCVector3 vDir(entryPoint - seatPos);
	//		vDir.Normalize();

	//		corridorPt += vDir*PAXRADIUS;

	//		EntryPointInSim* pEntryPointInSim = new EntryPointInSim(pShortestCorridor,NULL);
	//		pShortestCorridor->InsertIntersectionNode(nCorridorSeg,dDist,pEntryPointInSim);
	//		pEntryPointInSim->SetCreateSeat(pShortSeatInSim);
	//		pEntryPointInSim->SetDistance(dDist);
	//		pEntryPointInSim->SetWalkType(EntryPointInSim::Straight_Walk);
	//		pEntryPointInSim->SetLocation(corridorPt);
	//		pSingleSeatRowGraph->AddEntryPoint(pEntryPointInSim);
	//		m_pCorridorGraphMgr->GetFlight()->GetSeatEntryPoint().push_back(pEntryPointInSim);
	//	}
	//}
	
	return true;
}

double CorridorGraph::BuildEntryPoint(const ARCVector3& sourcePt,const ARCVector3& vDir,double& dDist,int& nSegment,SingleSeatRowGraph* pSingleSeatRowGraph,SingleCorridorGraph* pSingleCorridorGraph)
{

	CPoint2008 seat2008Pt(sourcePt.n[VX],sourcePt.n[VY],0.0);
	CPoint2008 vARCDir(vDir.n[VX],vDir.n[VY],0.0);

	OnboardCorridorInSim* corridorInSim = pSingleCorridorGraph->GetCorridorInSim();
	CPath2008 path = corridorInSim->m_pCorridor->GetPathCenterLine();
	CRayIntersectPath2D rayPath(seat2008Pt,vARCDir,path);
	if (rayPath.Get())
	{
		double dRayDist = rayPath.m_Out.begin()->m_dist;

		CPoint2008 intersectPt = path.GetIndexPoint((float)rayPath.m_Out.begin()->m_indexInpath);
		nSegment = static_cast<int>(rayPath.m_Out.begin()->m_indexInpath);
		CPoint2008 pt(intersectPt.getX(),intersectPt.getY(),0.0);
		if (!m_areaList.intersectionArea(seat2008Pt,pt))
		{
			dDist = path.GetIndexDist((float)rayPath.m_Out.begin()->m_indexInpath);
			
			return dRayDist;
		}
	}
	return (std::numeric_limits<double>::max)();
}

bool CorridorGraph::InitArea()
{
	m_areaList.clear();

	int sizeOfCount = (int)m_vSingleGraph.size();
	if( sizeOfCount <= 0)
		return false;

	for( int i =0; i<sizeOfCount; i++)
	{
		SingleGraph* pSingleGraph = m_vSingleGraph[i];
		pSingleGraph->PutAreaInto(m_areaList);
	}

	return true;
}


bool CorridorGraph::InitIntersectionWithCorridor(bool bCheckArea,const ARCVector3& destPt,CorridorGraphVertex& vertexS,int& nCorridorSeg,double& dLowDistance)
{
	int sizeOfCorridor =(int) m_vSingleGraph.size();
	if( sizeOfCorridor <= 0)
		return false;

	double dShortestDistance = (std::numeric_limits<double>::max)();
	double dCorridorDist = 0.0;

	CPoint2008 ptSource(destPt.n[VX],destPt.n[VY],0.0);

	OnboardCorridorInSim* pShortCorridorInSim = NULL;
	for( int iCorridorCout =0; iCorridorCout<sizeOfCorridor; iCorridorCout++)
	{

		SingleGraph* pSingleGraph = m_vSingleGraph[iCorridorCout];
		if (pSingleGraph == NULL || !pSingleGraph->IsCorridor())
			continue;
		SingleCorridorGraph* pSingleCorridorGrpah = (SingleCorridorGraph*)pSingleGraph;
		OnboardCorridorInSim* corridorInSim = pSingleCorridorGrpah->GetCorridorInSim();
		int nSegment;
		double dDistance = 0.0;
		CPoint2008 interSectPt;
		double dShortDist = corridorInSim->GetShortestDistancePoint(ptSource,interSectPt,corridorInSim->m_pCorridor->GetPathCenterLine(),nSegment,dDistance);
		bool bSucessed = true;
		if (bCheckArea)
		{
			bSucessed = !m_areaList.intersectionArea(CPoint2008(destPt.n[VX],destPt.n[VY],0.0),interSectPt);
		}

		if (bSucessed && dShortDist < dShortestDistance)
		{
			dShortestDistance = dShortDist;
			nCorridorSeg = nSegment;
			dCorridorDist = dDistance;

			pShortCorridorInSim = corridorInSim;
		}	
	}

	if (pShortCorridorInSim)
	{
		CorridorVertex corridorS;
		corridorS.SetElement(pShortCorridorInSim);
		corridorS.SetDistance(dCorridorDist);
		vertexS.SetCorridorVertex(corridorS);

		dLowDistance = dShortestDistance;
		return true;
	}

	return false;
}

//////CorridorGraphMgr////////////////////////////////////////////////////////////////////
CorridorGraphMgr::CorridorGraphMgr()
:m_pboostGraph(NULL)
,m_pOnBoardFlight(NULL)
{

}

CorridorGraphMgr::~CorridorGraphMgr()
{
	clearGraphMgr();
}

bool CorridorGraphMgr::initGraphMgr(OnboardFlightInSim* pOnboardFightInSim)
{
	clearGraphMgr();

	m_pOnBoardFlight = pOnboardFightInSim;

	//init door
	InitDoorGraph(pOnboardFightInSim);
	//init seat
	InitSeatGraph(pOnboardFightInSim);
	//init seat graph
	InitSeatRowGraph(pOnboardFightInSim);
	//init stair
	InitStairGraph(pOnboardFightInSim);
	//init elevator
	InitElevatorGraph(pOnboardFightInSim);
	//init escalator
	InitEscalatorGraph(pOnboardFightInSim);
	//init corridor
	InitCorridorGraph(pOnboardFightInSim);//corridor must be initialization the last


	BulidCorridorGraph();

	createBoostGraph();

	return true;
}

void CorridorGraphMgr::InitCorridorGraph(OnboardFlightInSim* pOnboardFightInSim)
{
	std::vector<OnboardCorridorInSim*>* pCorridorList = pOnboardFightInSim->GetOnboardCorridorList();
	int iCorridorNum = (int)pCorridorList->size();
	for( int i=0; i<iCorridorNum; i++ )
	{
		OnboardCorridorInSim* pCorridorInSim = (*pCorridorList)[i];
		COnboardCorridor* pCorridor = pCorridorInSim->m_pCorridor;
		CDeck* pDeck = pCorridor->GetDeck();

		if (pDeck)
		{
			CorridorGraph* graph = GetCorridorGraph(pDeck->GetID());
			if (graph == NULL)
			{
				graph = new CorridorGraph(this,pDeck->GetID());
				m_allCorridorGraph.push_back(graph);
			}	

			SingleGraph* pSingleGraph = new SingleCorridorGraph(graph,pCorridorInSim);
			graph->m_vSingleGraph.push_back(pSingleGraph);	
		}
	}
}

void CorridorGraphMgr::InitDoorGraph(OnboardFlightInSim* pOnboardFightInSim)
{
	std::vector<OnboardDoorInSim *>* pDoorSet = pOnboardFightInSim->GetOnboardDoorList();
	int iDoorNum = (int)pDoorSet->size();
	for (int i = 0; i < iDoorNum; i++)
	{
		OnboardDoorInSim* pDoorInSim = (*pDoorSet)[i];
		CDoor* pDoor = pDoorInSim->m_pDoor;
		CDeck* pDeck = pDoor->GetDeck();
		if (pDeck)
		{
			CorridorGraph* graph = GetCorridorGraph(pDeck->GetID());
			if (graph == NULL)
			{
				graph = new CorridorGraph(this,pDeck->GetID());
				m_allCorridorGraph.push_back(graph);
			}	
			SingleGraph* pSingleGraph = new SingleDoorGraph(graph,pDoorInSim);
			graph->m_vSingleGraph.push_back(pSingleGraph);
		}
	}
}

void CorridorGraphMgr::InitSeatGraph(OnboardFlightInSim* pOnboardFightInSim)
{
	std::vector<OnboardSeatInSim *>* pSeatSet = pOnboardFightInSim->GetOnboardSeatList();
	int iSeatNum = (int)pSeatSet->size();
	for (int i = 0; i < iSeatNum; i++)
	{
		OnboardSeatInSim* pSeatInSim = (*pSeatSet)[i];
		CSeat* pSeat = pSeatInSim->m_pSeat;
		CDeck* pDeck = pSeat->GetDeck();
		if (pDeck)
		{
			CorridorGraph* graph = GetCorridorGraph(pDeck->GetID());
			if (graph == NULL)
			{
				graph = new CorridorGraph(this,pDeck->GetID());
				m_allCorridorGraph.push_back(graph);
			}	
			SingleGraph* pSingleGraph = new SingleSeatGraph(graph,pSeatInSim);
			graph->m_vSingleGraph.push_back(pSingleGraph);
		}
	}
}

void CorridorGraphMgr::InitSeatRowGraph(OnboardFlightInSim* pOnboardFlightInSim)
{
	std::vector<OnboardSeatRowInSim*>* pSeatRowSet = pOnboardFlightInSim->GetOnboardSeatRowList();
	int iSeatRowNum = (int)pSeatRowSet->size();
	for (int i = 0; i < iSeatRowNum; i++)
	{
		OnboardSeatRowInSim* pSeatRowInSim = (*pSeatRowSet)[i];
		CSeatRow* pSeatRow = pSeatRowInSim->GetSeatRowInput();
		CDeck* pDeck = pSeatRow->GetDeck();
		if (pDeck)
		{
			CorridorGraph* graph = GetCorridorGraph(pDeck->GetID());
			if (graph == NULL)
			{
				graph = new CorridorGraph(this,pDeck->GetID());
				m_allCorridorGraph.push_back(graph);
			}
			SingleGraph* pSingleGraph = new SingleSeatRowGraph(graph,pSeatRowInSim);
			graph->m_vSingleGraph.push_back(pSingleGraph);
		}
	}
}

void CorridorGraphMgr::InitStairGraph(OnboardFlightInSim* pOnboardFightInSim)
{
	std::vector<OnboardStairInSim*>* pStairSet = pOnboardFightInSim->GetOnboardStairList();
	int iStairNum = (int)pStairSet->size();
	for (int i = 0; i < iStairNum; i++)
	{
		OnboardStairInSim* pStairInSim = (*pStairSet)[i];
		COnBoardStair* pStair = (COnBoardStair*)(pStairInSim->m_pDevice);
		std::vector<DeckPoint> vDeckPt = pStair->GetDeckPoint();

		CorridorGraphVertexList vertexList;
		for (int j = 0; j < (int)vDeckPt.size(); j++)
		{
			DeckPoint deckPt = vDeckPt[j];
			CDeck* pDeck = deckPt.first;
			CorridorGraphVertex vertexD;
			DeviceGraphVertex deviceD;
			deviceD.SetLocation(deckPt.second);
			deviceD.SetElement(pStairInSim);
			vertexD.SetVertexType(Stair_Vertex);
			vertexD.SetDeviceVertex(deviceD);
			vertexList.push_back(vertexD);

			CorridorGraph* graph = GetCorridorGraph(pDeck->GetID());
			if (graph == NULL)
			{
				graph = new CorridorGraph(this,pDeck->GetID());
				m_allCorridorGraph.push_back(graph);
			}	
			SingleGraph* pSingleGraph = new SingleDeviceGraph(graph,pStairInSim,deckPt.second);
			graph->m_vSingleGraph.push_back(pSingleGraph);
		}
		insertVertexAndEdge(vertexList);
	}
}

void CorridorGraphMgr::InitElevatorGraph(OnboardFlightInSim* pOnboardFightInSim)
{
	std::vector<OnboardElevatorInSim*>* pElevatorSet = pOnboardFightInSim->GetOnboardElevatorList();
	int iElevatorNum = (int)pElevatorSet->size();
	for (int i = 0; i < iElevatorNum; i++)
	{
		OnboardElevatorInSim* pElevatorInSim = (*pElevatorSet)[i];
		COnBoardElevator* pElevator = (COnBoardElevator*)(pElevatorInSim->m_pDevice);
		std::vector<DeckPoint> vDeckPoint = pElevator->GetDeckPoint();
		CorridorGraphVertexList vertexList;
		for (int j = 0; j < (int)vDeckPoint.size(); j++)
		{
			DeckPoint deckPt = vDeckPoint[j];
			CorridorGraphVertex vertexD;
			DeviceGraphVertex deviceD;
			deviceD.SetLocation(deckPt.second);
			deviceD.SetElement(pElevatorInSim);
			vertexD.SetVertexType(Elevator_Vertex);
			vertexD.SetDeviceVertex(deviceD);
			vertexList.push_back(vertexD);

			CDeck* pDeck = deckPt.first;

			CorridorGraph* graph = GetCorridorGraph(pDeck->GetID());
			if (graph == NULL)
			{
				graph = new CorridorGraph(this,pDeck->GetID());
				m_allCorridorGraph.push_back(graph);
			}	
			SingleGraph* pSingleGraph = new SingleDeviceGraph(graph,pElevatorInSim,deckPt.second);
			graph->m_vSingleGraph.push_back(pSingleGraph);
		}
		insertVertexAndEdge(vertexList);
	}
}

void CorridorGraphMgr::InitEscalatorGraph(OnboardFlightInSim* pOnboardFightInSim)
{
	std::vector<OnboardEscalatorInSim*>* pEscalatorSet = pOnboardFightInSim->GetOnboardEscalatorList();
	int iEscalatorNum = (int)pEscalatorSet->size();
	for (int i = 0; i < iEscalatorNum; i++)
	{
		OnboardEscalatorInSim* pEscalatorInSim = (*pEscalatorSet)[i];
		COnBoardEscalator* pEsalator = (COnBoardEscalator*)(pEscalatorInSim->m_pDevice);
		std::vector<DeckPoint> vDeckPoint = pEsalator->GetDeckPoint();
		CorridorGraphVertexList vertexList;
		for (int j = 0; j < (int)vDeckPoint.size(); j++)
		{
			DeckPoint deckPt = vDeckPoint[j];
			CDeck* pDeck = deckPt.first;
			CorridorGraphVertex vertexD;
			DeviceGraphVertex deviceD;
			deviceD.SetLocation(deckPt.second);
			deviceD.SetElement(pEscalatorInSim);
			vertexD.SetVertexType(Escaltor_Vertex);
			vertexD.SetDeviceVertex(deviceD);
			vertexList.push_back(vertexD);

			CorridorGraph* graph = GetCorridorGraph(pDeck->GetID());
			if (graph == NULL)
			{
				graph = new CorridorGraph(this,pDeck->GetID());
				m_allCorridorGraph.push_back(graph);
			}	
			SingleGraph* pSingleGraph = new SingleDeviceGraph(graph,pEscalatorInSim,deckPt.second);
			graph->m_vSingleGraph.push_back(pSingleGraph);
		}
		insertVertexAndEdge(vertexList);
	}
}


void CorridorGraphMgr::insertVertexAndEdge( CorridorGraphVertexList& _vertexList )
{
	int iSourceIndex,iDestIndex;
	double weight;
	CorridorGraphEdge edgeS2D;

	//////////////////////////////////////////////////////////////////////////
	// because is undirect graph, so shall add two edge between two vertex
	CorridorGraphVertex vertexS = _vertexList[0];
	CorridorGraphVertex vertexD = _vertexList[1];
	weight = GetWeight(vertexS,vertexD);	
	{
		insertVertex( vertexS, iSourceIndex );
		insertVertex( vertexD, iDestIndex );

		edgeS2D.SetSourceSegment(iSourceIndex);
		edgeS2D.SetDestSegment(iDestIndex);
		edgeS2D.SetWeight(weight);
		insertEdge( edgeS2D );
	}

	for( unsigned i=1; i<_vertexList.size()-1; i++ )
	{
		vertexS = vertexD;
		vertexD = _vertexList[i+1];
		weight = GetWeight(vertexS,vertexD);	

		iSourceIndex = iDestIndex;
		insertVertex( vertexD, iDestIndex );

		edgeS2D.SetSourceSegment(iSourceIndex);
		edgeS2D.SetDestSegment(iDestIndex);
		edgeS2D.SetWeight(weight);
		insertEdge( edgeS2D );
	}
}

double CorridorGraphMgr::GetWeight(const CorridorGraphVertex& vertexS,const CorridorGraphVertex& vertexD)const
{
	switch (vertexS.GetVertexType())
	{
	case Corridor_Vertex:
		return (vertexS.GetCorridorVertex().GetDistance()-  vertexD.GetCorridorVertex().GetDistance());
	case Elevator_Vertex:
			return (vertexS.GetDeviceVertex().GetLocation().n[VZ] - vertexD.GetDeviceVertex().GetLocation().n[VZ]);
	case Escaltor_Vertex:
	case Stair_Vertex:
		{
			OnboardDeviceInSim* pDeviceInSim = (OnboardDeviceInSim*)vertexS.GetElementInSim();
			CPath2008 path;
			pDeviceInSim->GetRealPath(path);
			return path.GetTotalLength();
		}
	}
	return 0.0;
}

bool CorridorGraphMgr::BulidCorridorGraph()
{
	for (int i = 0; i < (int)m_allCorridorGraph.size(); i++)
	{
		CorridorGraph* graph = m_allCorridorGraph[i];
		graph->BuildCorridorGraph();
	}
	return true;
}


CorridorGraph* CorridorGraphMgr::GetCorridorGraph(int nDeck)
{
	for (unsigned i = 0; i < m_allCorridorGraph.size(); i++)
	{
		CorridorGraph* corridorGraph = m_allCorridorGraph[i];
		if (corridorGraph->GetID() == nDeck)
		{
			return corridorGraph;
		}
	}
	return NULL;
}

bool CorridorGraphMgr::clearGraphMgr()
{
	for (size_t i = 0 ; i < m_allCorridorGraph.size(); i++)
	{
		delete m_allCorridorGraph[i];
	}

	m_allCorridorGraph.clear();
	m_vertexList.clear();
	m_edgeList.clear();
	return true;
}

VertexType CorridorGraphMgr::GetVertexTypeByDevice(OnBoardDevieType deviceType)const
{
	if (deviceType == OnBoard_Stair)
	{
		return Stair_Vertex;
	}
	else if (deviceType == OnBoard_Elevator)
	{
		return Elevator_Vertex;
	}
	else if (deviceType == OnBoard_Escalator)
	{
		return Escaltor_Vertex;
	}

	return Stair_Vertex;
}

bool CorridorGraphMgr::getShortestPath(const ARCVector3& sourcePoint, const ARCVector3& destPoint, CorridorGraphVertexList& _shortestPath)
{
	CorridorGraphVertex entryVertex;
	CorridorGraphVertex exitVertex;
	if (!GetEntryExitVertex(sourcePoint, destPoint, entryVertex, exitVertex))
		return false;

// 	if (entryVertex.GetVertexType() != Deck_Vertex && exitVertex.GetVertexType() != Deck_Vertex)
// 	{
// 		FindShortestPathInGraph(_shortestPath,entryVertex,exitVertex);
// 	}
// 	else if (entryVertex.GetVertexType() != Deck_Vertex && exitVertex.GetVertexType() == Deck_Vertex)
// 	{
// 
// 	}
// 	else if (entryVertex.GetVertexType() == Deck_Vertex && exitVertex.GetVertexType() == Deck_Vertex)
// 	{
// 
// 	}
// 	else if (entryVertex.GetVertexType() == Deck_Vertex && exitVertex.GetVertexType() != Deck_Vertex)
// 	{
// 
// 	}
	return true;
}

bool CorridorGraphMgr::getShortestPath(const OnboardAircraftElementInSim* pEntryElementInSim,const ARCVector3& entryPos,const OnboardAircraftElementInSim* pExitElementInSim,const ARCVector3& exitPos,CorridorGraphVertexList& _shortestPath)
{
	CorridorGraphVertex entryVertex;
	CorridorGraphVertex exitVertex;
	if(GetVertexByElementInSim(pEntryElementInSim,entryVertex)&&GetVertexByElementInSim(pExitElementInSim,exitVertex))
	{
		FindShortestPathInGraph(_shortestPath,entryVertex,exitVertex);
	}
	else
	{
		DeviceGraphVertex deckS,deckD;
		deckS.SetLocation(entryPos);
		deckD.SetLocation(exitPos);

		entryVertex.SetVertexType(Deck_Vertex);
		exitVertex.SetVertexType(Deck_Vertex);

		entryVertex.SetDeckVertex(deckS);
		exitVertex.SetDeckVertex(deckD);

		// add the source point and dest point
		_shortestPath.push_back(exitVertex);
		_shortestPath.insert(_shortestPath.begin(), entryVertex);
	}
	return true;
}

void CorridorGraphMgr::FindShortestPathInGraph(CorridorGraphVertexList& shortestPath, const CorridorGraphVertex& sourceVertex, const CorridorGraphVertex& destVertex)
{
	shortestPath.clear();

	int sourceVertexIndex = findVertex(sourceVertex);
	int destVertexIndex = findVertex(destVertex);

	// not stored in the shortest path library, need calculate it
	boost::property_map<BoostGraph, boost::edge_weight_t>::type weightmap = boost::get(boost::edge_weight, *m_pboostGraph);
	std::vector<vertex_descriptor> parentsVertex(boost::num_vertices(*m_pboostGraph));
	std::vector<double> distanced(boost::num_vertices(*m_pboostGraph));
	boost::property_map<BoostGraph, boost::vertex_index_t>::type indexmap = boost::get(boost::vertex_index, *m_pboostGraph);

	boost::dijkstra_shortest_paths(*m_pboostGraph, sourceVertexIndex, boost::predecessor_map(&parentsVertex[0]).distance_map(&distanced[0]));

	if (parentsVertex[destVertexIndex] == destVertexIndex)
	{
		shortestPath.push_back(GetVertex(sourceVertexIndex));
		return;
	}

	std::vector<int> indexList;
	// store the index of vertex in the shortest path
	int iIndex = destVertexIndex;
	int iNextIndex = iIndex;
	do
	{
		indexList.push_back(iIndex);
		iNextIndex = parentsVertex[iIndex];

		if (iNextIndex == iIndex)
			break;

		iIndex = iNextIndex;
	}
	while (iIndex != destVertexIndex);
	indexList.push_back(iIndex);

	//////////////////////////////////////////////////////////////////////////
	// make a path from the pathVertexList
	int pointNum = indexList.size();
	for( int i=0; i < pointNum; i++ )
	{
		shortestPath.push_back(GetVertex(indexList[i]));
	}
	std::reverse(shortestPath.begin(), shortestPath.end());
}

const CorridorGraphVertex& CorridorGraphMgr::GetVertex(int vertexIndex)
{
	ASSERT(vertexIndex>= 0 && vertexIndex < (int)m_vertexList.size());
	return m_vertexList[vertexIndex];
}
bool CorridorGraphMgr::GetEntryExitVertex(const ARCVector3& sourcePoint, const ARCVector3& destPoint, CorridorGraphVertex& entryVertex, CorridorGraphVertex& exitVertex)
{
	if(!GetVertexByLocation(sourcePoint,entryVertex))
	{
		entryVertex.SetVertexType(Deck_Vertex);
		DeviceGraphVertex vertexD;
		vertexD.SetLocation(sourcePoint);
		entryVertex.SetDeckVertex(vertexD);
	}

	if (!GetVertexByLocation(destPoint,exitVertex))
	{
		entryVertex.SetVertexType(Deck_Vertex);
		DeviceGraphVertex vertexD;
		vertexD.SetLocation(destPoint);
		entryVertex.SetDeckVertex(vertexD);
	}
	
	return true;
}

bool CorridorGraphMgr::GetVertexByElementInSim(const OnboardAircraftElementInSim* pElementInSim,CorridorGraphVertex& vertex)
{
	for (int i = 0; i < (int)m_vertexList.size(); i++)
	{
		CorridorGraphVertex& vertexRef = m_vertexList[i];
		if (pElementInSim->GetType() == EntryPointString)
		{
			if(vertexRef.GetCorridorVertex().GetEntryPointInSim() == pElementInSim)
			{
				vertex = m_vertexList[i];
				return true;
			}
		}
		else if (m_vertexList[i].GetElementInSim() == pElementInSim)
		{
			vertex = m_vertexList[i];
			return true;
		}
	}

	return false;
}

bool CorridorGraphMgr::GetVertexByLocation(const ARCVector3& sourcePoint,CorridorGraphVertex& vertex)
{
	return IsExist(sourcePoint,vertex);
}

int CorridorGraphMgr::findVertex(const CorridorGraphVertex& vertex)const
{
	for( unsigned i =0; i< m_vertexList.size(); i++ )
	{
		if (m_vertexList[i] == vertex )
			return i;
	}
	// can not find the item
	return -1;
}

bool CorridorGraphMgr::IsExist(const ARCVector3& point,CorridorGraphVertex& vertex)
{
	for (int i = 0; i < (int)m_vertexList.size(); i++)
	{
		CorridorGraphVertex& vertex = m_vertexList[i];
		VertexType vertexType = vertex.GetVertexType();
		switch (vertexType)
		{
		case Corridor_Vertex:
			{
				CPoint2008 destPt(point.n[VX],point.n[VY],point.n[VZ]);
				CorridorVertex vertexD = vertex.GetCorridorVertex();
				COnboardCorridor* pCorridor = vertexD.GetCorridor();
				CPath2008& path = pCorridor->GetPathCenterLine();
				CPoint2008 pt = path.GetDistPoint(vertexD.GetDistance());
				if (pt == destPt)
				{
					vertex.SetVertexType(Corridor_Vertex);
					vertex.SetCorridorVertex(vertexD);
					return true;
				}
			}
			break;
		case Seat_Vertex:
			{
				const DeviceGraphVertex& vertexD = vertex.GetSeatVertex();
				if (point == vertexD.GetLocation())
				{
					vertex.SetVertexType(Seat_Vertex);
					vertex.SetSeatVertex(vertexD);
					return true;
				}
			}
			break;
		case Door_Vertex:
			{
				const DeviceGraphVertex& vertexD = vertex.GetDoorVertex();
				if (point == vertexD.GetLocation())
				{
					vertex.SetVertexType(Door_Vertex);
					vertex.SetDoorVertex(vertexD);
					return true;
				}
			}
			break;
		case Stair_Vertex:
			{
				const DeviceGraphVertex& vertexD = vertex.GetDeviceVertex();
				if (point == vertexD.GetLocation())
				{
					vertex.SetVertexType(Stair_Vertex);
					vertex.SetDeviceVertex(vertexD);
					return true;
				}
			}
		case Elevator_Vertex:
			{
				const DeviceGraphVertex& vertexD = vertex.GetDeviceVertex();
				if (point == vertexD.GetLocation())
				{
					vertex.SetVertexType(Elevator_Vertex);
					vertex.SetDeviceVertex(vertexD);
					return true;
				}
			}
		case Escaltor_Vertex:
			{
				const DeviceGraphVertex& vertexD = vertex.GetDeviceVertex();
				if (point == vertexD.GetLocation())
				{
					vertex.SetVertexType(Escaltor_Vertex);
					vertex.SetDeviceVertex(vertexD);
					return true;
				}
			}
			break;
		default:
			{
				ASSERT(FALSE);
				return false;
			}
		}
	}
	return false;
}

void CorridorGraphMgr::createBoostGraph()
{
	if( m_pboostGraph )
	{
		delete m_pboostGraph;
		m_pboostGraph = NULL;
	}

	// new a boost graph ,then add all edge
	int numofVertex = (int)m_vertexList.size();
	m_pboostGraph = new BoostGraph( numofVertex );
	if( m_pboostGraph == NULL )
	{
		throw new StringError( " Create Boost Graph Failed! "); 
	}

	boost::property_map< BoostGraph, boost::edge_weight_t >::type weightmap = boost::get(boost::edge_weight, *m_pboostGraph );
	for( std::size_t i=0; i<m_edgeList.size(); i++ )
	{
		edge_descriptor edge;
		bool inserted;
		boost::tie(edge, inserted ) = boost::add_edge( m_edgeList[i].GetSourceSegment(), m_edgeList[i].GetDestsegment(), *m_pboostGraph );
		weightmap[edge] = m_edgeList[i].GetWeight();
	}

	m_connectedComponents.resize(boost::num_vertices(*m_pboostGraph));
	boost::connected_components(*m_pboostGraph, &m_connectedComponents[0]);
}

bool CorridorGraphMgr::insertVertex( const CorridorGraphVertex& _item,int& _index )
{
	CorridorGraphVertex newitem = _item;
	
	for( unsigned i=0; i<m_vertexList.size(); i++ )
	{
		CorridorGraphVertex vertex = m_vertexList[i];
		if( vertex == _item )
		{
			_index = i;
			return false;	// find a old data
		}
	}

	m_vertexList.push_back( _item );
	_index = m_vertexList.size()-1;
	return true;			// add a new data
}

bool CorridorGraphMgr::insertEdge(const CorridorGraphEdge& _item)
{
	for( unsigned i=0; i<m_edgeList.size(); i++ )
	{
		CorridorGraphEdge edge = m_edgeList[i];
		if( edge == _item )
		{
			return false;	// find a old data
		}
	}

	m_edgeList.push_back( _item );
	return true;			// add a new data
}

OnboardFlightInSim * CorridorGraphMgr::GetFlight()
{
	return m_pOnBoardFlight;
}
/////////////////////////////////////////////////////////////////////////////////////////
//////single graph///////////////////////////////////////////////////////////////////////
SingleGraph::SingleGraph(CorridorGraph* pCorridorGraph)
:m_pCorridorGraph(pCorridorGraph)
{

}

SingleGraph::~SingleGraph()
{

}
//////////////////////////////////////////////////////////////////////////////////////////
///SingleCorridorGraph////////////////////////////////////////////////////////////////////
SingleCorridorGraph::SingleCorridorGraph(CorridorGraph* pCorridorGraph,OnboardCorridorInSim* pCorridorInSim)
:SingleGraph(pCorridorGraph)
,m_pCorridorInSim(pCorridorInSim)
{

}

SingleCorridorGraph::~SingleCorridorGraph()
{

}

bool SingleCorridorGraph::BuildSingleGraph()
{
	return BuildSingleCorridorGraph();
}

bool SingleCorridorGraph::BuildSingleCorridorGraph()
{
	// for each segement of a corridor
	CPath2008& path = m_pCorridorInSim->m_pCorridor->GetPathByIndex(COnboardCorridor::CenterPath);
	CorridorGraphVertexList corridorVertexList;
	for( int iSegCount = 0; iSegCount < path.getCount() - 1; iSegCount++ )
	{
		// find all intersect point on segment iSegCount
		CorridorPointList intersectPointOnSegment;

		m_pCorridorInSim->GetIntersectionOnSegment( iSegCount, intersectPointOnSegment);
		
		ConnectIntersectionNode(intersectPointOnSegment);
		// sort intersect point by from start distance
		CorridorGraphVertexList vertexList;
		sortPointByFromStartDistance( intersectPointOnSegment, vertexList);

		CorridorGraphVertex vertex;
		// add two end point
		if (iSegCount == 0)
		{
			CorridorVertex corridorVertex;
			corridorVertex.SetDistance(0.0);
			corridorVertex.SetElement(m_pCorridorInSim);
			vertex.SetCorridorVertex(corridorVertex);
			vertex.SetVertexType( Corridor_Vertex );
			vertexList.insert( vertexList.begin(), vertex );
		}
	
		if (iSegCount == path.getCount() - 2)
		{
			// last point
			CorridorVertex corridorVertex;
			corridorVertex.SetDistance(path.GetTotalLength());
			corridorVertex.SetElement(m_pCorridorInSim);
			vertex.SetCorridorVertex(corridorVertex);
			vertex.SetVertexType( Corridor_Vertex );
			vertexList.push_back( vertex );
		}
		corridorVertexList.insert(corridorVertexList.end(),vertexList.begin(),vertexList.end());
	}

	std::unique(corridorVertexList.begin(),corridorVertexList.end());

	GetGraphMgr()->insertVertexAndEdge( corridorVertexList );
	return true;
}


static bool sortByDistance(const CorridorPoint& _point1, const CorridorPoint& _point2 )
{
	return _point1.GetDistance() < _point2.GetDistance(); 
}

void SingleCorridorGraph::sortPointByFromStartDistance(CorridorPointList& _pointList, CorridorGraphVertexList& _vectorList)
{
	_vectorList.clear();
	if( _pointList.size()<=0 )
		return;

	std::sort( _pointList.begin(), _pointList.end(), sortByDistance );

	for( unsigned i=0; i<_pointList.size(); i++ )
	{
		const CorridorPoint& point = _pointList[i];
		CorridorGraphVertex vertex;
		CorridorVertex corridorVertex;
		vertex.SetVertexType(Corridor_Vertex);
		corridorVertex.SetElement(m_pCorridorInSim);
		corridorVertex.SetDistance(point.GetDistance());
		corridorVertex.SetEntryPointInSim(point.GetEntryPointInSim());
		vertex.SetCorridorVertex(corridorVertex);
		_vectorList.push_back( vertex );
	}
}

//connect corridor intersection node, start node and end node
void SingleCorridorGraph::ConnectIntersectionNode(CorridorPointList& _pointList)
{
	if( _pointList.size()<=0 )
		return;

	for (unsigned i = 0; i < _pointList.size(); i++)
	{
		const CorridorPoint& pt = _pointList[i];
		if (pt.GetIntersectionCorridor() == NULL)
			continue;
		
		CorridorGraphVertex vertexS;
		CorridorGraphVertex vertexD;
		CorridorVertex corridorS;
		CorridorVertex corridorD;

		CorridorGraphVertexList vertexList;
		corridorS.SetElement(m_pCorridorInSim);
		corridorS.SetDistance(pt.GetDistance());
		vertexS.SetVertexType(Corridor_Vertex);
		vertexS.SetCorridorVertex(corridorS);
		vertexList.push_back(vertexS);

		corridorD.SetElement(pt.GetIntersectionCorridor());
		corridorD.SetDistance(pt.GetInterCorridorDist());
		vertexD.SetVertexType(Corridor_Vertex);
		vertexD.SetCorridorVertex(corridorD);
		vertexList.push_back(vertexD);

		GetGraphMgr()->insertVertexAndEdge(vertexList);

	}
}

//////////////////////////////////////////////////////////////////////////////////////////
SingleDoorGraph::SingleDoorGraph(CorridorGraph* pCorridorGraph,OnboardDoorInSim* pDoorInSim)
:SingleGraph(pCorridorGraph)
,m_pDoorInSim(pDoorInSim)
{

}

SingleDoorGraph::~SingleDoorGraph()
{

}

bool SingleDoorGraph::BuildSingleGraph()
{
	return BuildSingleDoorGraph();
}

bool SingleDoorGraph::BuildSingleDoorGraph()
{
	CorridorGraphVertex vertexS;
	vertexS.SetVertexType(Corridor_Vertex);
	double dShortestDistance = 0.0;
	ARCVector3 doorPos;
	m_pDoorInSim->GetPosition(doorPos);
	int nSegment = 0;
	if (m_pCorridorGraph->InitIntersectionWithCorridor(false,doorPos,vertexS,nSegment,dShortestDistance))
	{
		CorridorVertex deviceS = vertexS.GetCorridorVertex();
		OnboardCorridorInSim* pCorridorInsim = (OnboardCorridorInSim*)vertexS.GetElementInSim();
		pCorridorInsim->InsertIntersectionNode(nSegment,deviceS.GetDistance());

		CorridorGraphVertex vertexD;
		DeviceGraphVertex deviceD;
		vertexD.SetVertexType(Door_Vertex);
		deviceD.SetLocation(doorPos);
		deviceD.SetElement(m_pDoorInSim);
		vertexD.SetDoorVertex(deviceD);

		CorridorGraphEdge edge;
		int iSourceIndex,iDestIndex;
		GetGraphMgr()->insertVertex( vertexS, iSourceIndex );
		GetGraphMgr()->insertVertex( vertexD, iDestIndex );

		edge.SetSourceSegment(iSourceIndex);
		edge.SetDestSegment(iDestIndex);
		edge.SetWeight(dShortestDistance);
		GetGraphMgr()->insertEdge( edge );
	}
	
	return true;
}
//////////////////////////////////////////////////////////////////////////////////////////
SingleSeatRowGraph::SingleSeatRowGraph(CorridorGraph* pCorridorGraph,OnboardSeatRowInSim* pSeatRowInSim)
:SingleGraph(pCorridorGraph)
,m_pSeatRowInSim(pSeatRowInSim)
{

}

SingleSeatRowGraph::~SingleSeatRowGraph()
{

}

bool SingleSeatRowGraph::BuildSingleGraph()
{
	return BuildSingleSeatRowGraph();
}

bool SingleSeatRowGraph::BuildSingleSeatRowGraph()
{
	return InitEntryPoint();
}

//Get seat entry point
EntryPointInSim* SingleSeatRowGraph::GetSeatEntryPoint(OnboardSeatInSim* pSeatInSim,ARCVector3&vDir,double& dDist,int& nSegment)
{
	//CorridorGraphVertex vertexS;
	//vertexS.SetVertexType(Corridor_Vertex);

	//ARCVector3 seatPos = pSeatInSim->GetFrontPosition();
	//if (m_pCorridorGraph->InitIntersectionWithCorridor(true,seatPos,vertexS,nSegment,dDist))
	//{
	//	CorridorVertex deviceS = vertexS.GetCorridorVertex();
	//	OnboardCorridorInSim* pCorridorInSim = (OnboardCorridorInSim*)deviceS.GetElementInSim();
	//	ARCVector3 entryPoint = pCorridorInSim->GetDistancePoint(deviceS.GetDistance());

	//	double dLeftPathDist = (std::numeric_limits<double>::max)();
	//	double dRightPathDist = (std::numeric_limits<double>::max)();

	//	CPoint2008 leftPt, rightPt;
	//	int nLeftSegment = 0;
	//	int nRightSegment = 0;

	//	CPoint2008 ptSource(seatPos.n[VX],seatPos.n[VY],0.0);
	//	pCorridorInSim->GetShortestDistancePoint(ptSource,leftPt,pCorridorInSim->m_pCorridor->GetPathLeftLine(),nLeftSegment,dLeftPathDist);
	//	pCorridorInSim->GetShortestDistancePoint(ptSource,rightPt,pCorridorInSim->m_pCorridor->GetPathRightLine(),nRightSegment,dRightPathDist);

	//	ARCVector3 corridorPt = dLeftPathDist < dRightPathDist ?leftPt:rightPt;
	//	corridorPt.n[VZ] = seatPos.n[VZ];

	//	vDir = entryPoint - seatPos;
	//	vDir.Normalize();

	//	corridorPt += vDir*PAXRADIUS;

	//	EntryPointInSim* pEntryPointInSim = new EntryPointInSim(pCorridorInSim,NULL);
	//	pEntryPointInSim->SetCreateSeat(pSeatInSim);
	//	pEntryPointInSim->SetDistance(deviceS.GetDistance());
	//	pEntryPointInSim->SetWalkType(EntryPointInSim::TurnDirection_Walk);
	//	pEntryPointInSim->SetLocation(corridorPt);

	//	return pEntryPointInSim;
	//}
	return NULL;
}

//front and back find entry point
void SingleSeatRowGraph::SetLeftAndRightEntryPoint()
{
	
	if (GetSeatCountInRow() == 0)
		return;

	OnboardSeatInSim* pFrontSeatInSim = NULL;
	OnboardSeatInSim* pBackSeatInSim = NULL;

	//set front entry point
	ARCVector3 vDir1;
	EntryPointInSim* pEntryPointInSim1= NULL; 
	int nSegment1 = 0;
	double dShortestDistance1 = 0.0;
	{
		OnboardSeatInSim* pFrontSeatInSim = front();
		if (pFrontSeatInSim)
		{
			pEntryPointInSim1 = GetSeatEntryPoint(pFrontSeatInSim,vDir1,dShortestDistance1,nSegment1);
		}
	}

	//set back entry point
	ARCVector3 vDir2;
	EntryPointInSim* pEntryPointInSim2= NULL; 
	int nSegment2 = 0;
	double dShortestDistance2 = 0.0;
	{
		pBackSeatInSim = back();
		if (pBackSeatInSim)
		{
			pEntryPointInSim2 = GetSeatEntryPoint(pBackSeatInSim,vDir2,dShortestDistance2,nSegment2);
		}
	}

	if (pEntryPointInSim1 && pEntryPointInSim2 && pEntryPointInSim1->GetCorridor() == pEntryPointInSim2->GetCorridor())
	{
		double degrees = vDir1.GetCosOfTwoVector(vDir2);
		if(degrees>= 0 &&degrees<=1)
		{
			if(dShortestDistance1 < dShortestDistance2)
			{
				pEntryPointInSim1->GetCorridor()->InsertIntersectionNode(nSegment1,pEntryPointInSim1->GetDistance(),pEntryPointInSim1);
				
				m_pSeatRowInSim->AddEntryPoint(pEntryPointInSim1);
				delete pEntryPointInSim2;
				pEntryPointInSim2 = NULL;
			}
			else
			{
				pEntryPointInSim2->GetCorridor()->InsertIntersectionNode(nSegment2,pEntryPointInSim2->GetDistance(),pEntryPointInSim2);
				m_pSeatRowInSim->AddEntryPoint(pEntryPointInSim2);
				delete pEntryPointInSim1;
				pEntryPointInSim1 = NULL;
			}
			return;
		}
	}
	
	m_pSeatRowInSim->AddEntryPoint(pEntryPointInSim1);
	if (pEntryPointInSim1)
	{
		pEntryPointInSim1->GetCorridor()->InsertIntersectionNode(nSegment1,pEntryPointInSim1->GetDistance(),pEntryPointInSim1);
	}

	m_pSeatRowInSim->AddEntryPoint(pEntryPointInSim2);
	if (pEntryPointInSim2)
	{
		pEntryPointInSim2->GetCorridor()->InsertIntersectionNode(nSegment2,pEntryPointInSim2->GetDistance(),pEntryPointInSim2);
	}
	
}

bool SingleSeatRowGraph::InitEntryPoint()
{
	m_pCorridorGraph->InitEntryPoint(this);
	if (empty())
	{
		SetLeftAndRightEntryPoint();
	}
	return !empty();
}
//////////////////////////////////////////////////////////////////////////////////////////
SingleSeatGraph::SingleSeatGraph(CorridorGraph* pCorridorGraph,OnboardSeatInSim* pSeatInSim)
:SingleGraph(pCorridorGraph)
,m_pSeatInSim(pSeatInSim)
{

}

SingleSeatGraph::~SingleSeatGraph()
{

}

bool SingleSeatGraph::BuildSingleGraph()
{
	if (!m_pSeatInSim->m_pSeat->getRow())
	{
		return BuildSingleSeatGraph();
	}
	return false;
}

bool SingleSeatGraph::PutAreaInto(SeatArea& areaList)
{
	CSeat* seat = m_pSeatInSim->m_pSeat;

	CPollygon2008 pollygon;
	CPoint2008 ptList[4];
	ptList[0] = CPoint2008(seat->getLeftTop().n[VX],seat->getLeftTop().n[VY],0.0);
	ptList[1] = CPoint2008(seat->getRightTop().n[VX],seat->getRightTop().n[VY],0.0);
	ptList[2] = CPoint2008(seat->getRightBottom().n[VX],seat->getRightBottom().n[VY],0.0);
	ptList[3] = CPoint2008(seat->getLeftBottom().n[VX],seat->getLeftBottom().n[VY],0.0);
	pollygon.init(4,ptList);

	areaList.InsertPollygon(pollygon);
	return true;
}

bool SingleSeatGraph::BuildSingleSeatGraph()
{
	//CorridorGraphVertex vertexS;
	//vertexS.SetVertexType(Corridor_Vertex);
	//double dShortestDistance = 0.0;
	//ARCVector3 seatPos = m_pSeatInSim->GetFrontPosition();
	//int nSegment = 0;
	//if (m_pCorridorGraph->InitIntersectionWithCorridor(true,seatPos,vertexS,nSegment,dShortestDistance))
	//{
	//	CorridorVertex deviceS = vertexS.GetCorridorVertex();
	//	OnboardCorridorInSim* pCorridorInSim = (OnboardCorridorInSim*)deviceS.GetElementInSim();

	//	ARCVector3 entryPoint = pCorridorInSim->GetDistancePoint(deviceS.GetDistance());
	//	double dLeftPathDist = (std::numeric_limits<double>::max)();
	//	double dRightPathDist = (std::numeric_limits<double>::max)();

	//	CPoint2008 leftPt, rightPt;
	//	int nLeftSegment = 0;
	//	int nRightSegment = 0;

	//	CPoint2008 ptSource(seatPos.n[VX],seatPos.n[VY],0.0);
	//	pCorridorInSim->GetShortestDistancePoint(ptSource,leftPt,pCorridorInSim->m_pCorridor->GetPathLeftLine(),nLeftSegment,dLeftPathDist);
	//	pCorridorInSim->GetShortestDistancePoint(ptSource,rightPt,pCorridorInSim->m_pCorridor->GetPathRightLine(),nRightSegment,dRightPathDist);

	//	ARCVector3 corridorPt = dLeftPathDist < dRightPathDist ?leftPt:rightPt;
	//	corridorPt.n[VZ] = seatPos.n[VZ];

	//	ARCVector3 vDir(entryPoint - seatPos);
	//	vDir.Normalize();

	//	corridorPt += vDir*PAXRADIUS;

	//	EntryPointInSim* pEntryPointInSim = new EntryPointInSim(pCorridorInSim,NULL);
	//	pCorridorInSim->InsertIntersectionNode(nSegment,deviceS.GetDistance(),pEntryPointInSim);
	//	pEntryPointInSim->SetCreateSeat(m_pSeatInSim);
	//	pEntryPointInSim->SetDistance(deviceS.GetDistance());
	//	pEntryPointInSim->SetWalkType(EntryPointInSim::TurnDirection_Walk);
	//	pEntryPointInSim->SetLocation(corridorPt);
	//	m_pSeatInSim->SetEntryPoint(pEntryPointInSim);
	//}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////
SingleDeviceGraph::SingleDeviceGraph(CorridorGraph* pCorridorGraph,OnboardDeviceInSim* pDeviceInSim,const ARCVector3& location)
:SingleGraph(pCorridorGraph)
,m_pDeviceInSim(pDeviceInSim)
,m_location(location)
{

}

SingleDeviceGraph::~SingleDeviceGraph()
{

}

bool SingleDeviceGraph::BuildSingleGraph()
{
	return BuildSingleDeviceGraph();
}

bool SingleDeviceGraph::BuildSingleDeviceGraph()
{
	CorridorGraphVertex vertexS;
	vertexS.SetVertexType(Corridor_Vertex);

	double dShortestDistance = 0.0;
	int nSegment = 0;
	if (m_pCorridorGraph->InitIntersectionWithCorridor(false,m_location,vertexS,nSegment,dShortestDistance))
	{
		CorridorVertex deviceS = vertexS.GetCorridorVertex();
		OnboardCorridorInSim* pCorridorInsim = (OnboardCorridorInSim*)vertexS.GetElementInSim();
		pCorridorInsim->InsertIntersectionNode(nSegment,deviceS.GetDistance());
		CorridorGraphVertex vertexD;
		DeviceGraphVertex deviceD;
		VertexType vertexType = GetGraphMgr()->GetVertexTypeByDevice(m_pDeviceInSim->m_pDevice->GetDeviceType());
		vertexD.SetVertexType(vertexType);
		deviceD.SetLocation(m_location);
		deviceD.SetElement(m_pDeviceInSim);
		vertexD.SetDeviceVertex(deviceD);

		CorridorGraphEdge edge;
		int iSourceIndex,iDestIndex;
		GetGraphMgr()->insertVertex( vertexS, iSourceIndex );
		GetGraphMgr()->insertVertex( vertexD, iDestIndex );

		edge.SetSourceSegment(iSourceIndex);
		edge.SetDestSegment(iDestIndex);
		edge.SetWeight(dShortestDistance);
		GetGraphMgr()->insertEdge( edge );
	}

	return true;
}

OnboardAircraftElementInSim* CorridorGraphVertex::GetElementInSim()const
{
	switch (m_nVertexType)
	{
	case Corridor_Vertex:
		return m_corridorVertex.GetElementInSim();
	case Escaltor_Vertex:
	case Elevator_Vertex:
	case Stair_Vertex:
		return m_deviceVertex.GetElementInSim();
	case Door_Vertex:
		return m_doorVertex.GetElementInSim();
	case Seat_Vertex:
		return m_seatVertex.GetElementInSim();
	}
	return NULL;
}

bool CorridorGraphVertex::IsDeviceVertex() const
{
	switch(m_nVertexType)
	{
	case Escaltor_Vertex:
	case Elevator_Vertex:
	case Stair_Vertex:
		return true;
	}
	return false;
}