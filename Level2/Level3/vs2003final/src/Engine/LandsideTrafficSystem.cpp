#include "StdAfx.h"
#include ".\LandsideTrafficSystem.h"
#include "PaxLandsideBehavior.h"
#include "LandsideResourceManager.h"
#include "LandsideStretchInSim.h"
#include "..\Common\exeption.h"
#include <boost\graph\dijkstra_shortest_paths.hpp>
#include <boost\graph\connected_components.hpp>
#include "CrossWalkInSim.h"
#include "LandsideConflictMoveEvent.h"
#include "WalkwayInSim.h"

#include "PERSON.H"

CLandsideTrafficSystem::CLandsideTrafficSystem(void)
{
}

CLandsideTrafficSystem::~CLandsideTrafficSystem(void)
{
	for (size_t i = 0; i < m_freeMovingList.size(); i++)
	{
		delete m_freeMovingList[i];
	}
	m_freeMovingList.clear();
}

//------------------------------------------------------------------------
//Summary:
//		find shortest path and move on the path
//Parameters:
//		time[in]: enter landside traffic system time
//		nNextState[in]: leave landside traffic generate event state
//-------------------------------------------------------------------------
bool CLandsideTrafficSystem::EnterTrafficSystem(const ElapsedTime& time,int nNextState, PaxLandsideBehavior* pPerson,const CPoint2008& startPos,LandsideResourceInSim* pStartResource,const CPoint2008& entPos,LandsideResourceInSim* pEndResource  )
{
	CLandsideWalkTrafficObjectInSim* pStartTrafficObjectSim = pStartResource->getBestTrafficObjectInSim(startPos,m_pResManager);
	if (pStartTrafficObjectSim == NULL)
		return false;

	CLandsideWalkTrafficObjectInSim* pEndTrafficObjectSim = pEndResource->getBestTrafficObjectInSim(entPos,m_pResManager);
	if (pEndTrafficObjectSim == NULL)
		return false;
	
	LandsideTrafficGraphVertexList walkwayNodeList;
	m_landsideGraph.getShortestPath(startPos,pStartResource,pStartTrafficObjectSim,entPos,pEndResource,pEndTrafficObjectSim,walkwayNodeList);

	CFreeMovingLogic* pFreeMoveing = new CFreeMovingLogic(this);
	pFreeMoveing->Initialize(walkwayNodeList,pPerson,nNextState);
	//pFreeMoveing->StartMove2(startPos,time,true);
	pFreeMoveing->ProcessMove(time);

	m_freeMovingList.push_back(pFreeMoveing);
	return true;
}

bool CLandsideTrafficSystem::EnterTrafficSystem( const ElapsedTime& time,int nNextState,PaxLandsideBehavior* pPerson,const CPoint2008& startPos,const CPoint2008& entPos )
{
	CLandsideWalkTrafficObjectInSim* pStartTrafficObjectSim = getBestTrafficObjectInSim(startPos,m_pResManager);
	if (pStartTrafficObjectSim == NULL)
		return false;

	CLandsideWalkTrafficObjectInSim* pEndTrafficObjectSim = getBestTrafficObjectInSim(entPos,m_pResManager);
	if (pEndTrafficObjectSim == NULL)
		return false;

	LandsideTrafficGraphVertexList walkwayNodeList;
	m_landsideGraph.getShortestPath(startPos,pStartTrafficObjectSim,entPos,pEndTrafficObjectSim,walkwayNodeList);

	CFreeMovingLogic* pFreeMoveing = new CFreeMovingLogic(this);
	pFreeMoveing->Initialize(walkwayNodeList,pPerson,nNextState);
	//pFreeMoveing->StartMove2(startPos,time,true);
	pFreeMoveing->ProcessMove(time);

	m_freeMovingList.push_back(pFreeMoveing);
	return true;
}

//------------------------------------------------------------------------
//Summary:
//		Initialize landside traffic system
//------------------------------------------------------------------------
void CLandsideTrafficSystem::Init( LandsideResourceManager* pResManager )
{
	m_pResManager = pResManager;
	m_landsideGraph.initGraphMgr(pResManager);

}
//-------------------------------------------------------------------------
//Summary:
//		leave the landside traffic system and release free moving
//-----------------------------------------------------------------------
void CLandsideTrafficSystem::LeaveTrafficSystem( PaxLandsideBehavior* pPerson,int emEndState,const ElapsedTime& time )
{
	pPerson->setState(emEndState);
	pPerson->GenerateEvent(time);

	releaseFreeMove(pPerson);
}
//-----------------------------------------------------------------------
//Summary:
//		release free moving logic
//----------------------------------------------------------------------
void CLandsideTrafficSystem::releaseFreeMove( PaxLandsideBehavior* pPerson )
{
	std::vector<CFreeMovingLogic*>::iterator iter = m_freeMovingList.begin();
	for (; iter != m_freeMovingList.end(); ++iter)
	{
		CFreeMovingLogic* pFreeMoving = *iter;
		if (pFreeMoving->GetBehavior() == pPerson)
		{
			delete pFreeMoving;
			m_freeMovingList.erase(iter);
			return;
		}
	}
}

CLandsideWalkTrafficObjectInSim* CLandsideTrafficSystem::getBestTrafficObjectInSim( const CPoint2008& pt,LandsideResourceManager* allRes )
{
	CLandsideWalkTrafficObjectInSim* pBestWalkwayInSim = NULL;
	double dShortestDist = (std::numeric_limits<double>::max)();

	for (size_t j = 0; j < allRes->m_vTrafficObjectList.size(); j++)
	{
		CLandsideWalkTrafficObjectInSim* pTrafficInSim = allRes->m_vTrafficObjectList[j];

		if (pTrafficInSim->GetTrafficType() != CLandsideWalkTrafficObjectInSim::Walkway_Type)
			continue;

		double dLevel = pTrafficInSim->GetLevel();
		double dZdist = fabsl(dLevel - pt.getZ());
		if (dZdist > (std::numeric_limits<double>::min)())
			continue;
		
		double dDist = pTrafficInSim->GetPointDistance(pt);
		if (dDist < dShortestDist)
		{
			pBestWalkwayInSim = pTrafficInSim;
			dShortestDist = dDist;
		}		
	}
	
	return pBestWalkwayInSim;
}

CLandsideGraph::CLandsideGraph()
:m_pboostGraph(NULL)
{

}

CLandsideGraph::~CLandsideGraph()
{
	clearGraphMgr();
}

bool CLandsideGraph::initGraphMgr(LandsideResourceManager* pResManager)
{
	ASSERT(pResManager);
	if (pResManager == NULL)
		return false;

	//calculate crosswalk and walkway intersection node
	pResManager->m_vTrafficObjectList.CalculateIntersectionNode();

	BuildBoostGraph(pResManager);

	CreateBoostGraph();
	return true;
}

//---------------------------------------------------------------------------
//Summary:
//		set up graph using walkway and crosswalk
//---------------------------------------------------------------------------
void CLandsideGraph::BuildBoostGraph(LandsideResourceManager* pResManager)
{
	for (int i = 0; i < pResManager->m_vTrafficObjectList.getCount(); i++)
	{
		CLandsideWalkTrafficObjectInSim* pTrafficObjectInSim = pResManager->m_vTrafficObjectList.getItem(i);
		BuildSingleTrafficObjectGrpah(pTrafficObjectInSim);
	}
}

//---------------------------------------------------------------------------
//Summary:
//		traffic object to intersected with other traffic object
//---------------------------------------------------------------------------
void CLandsideGraph::BuildSingleTrafficObjectGrpah( CLandsideWalkTrafficObjectInSim* pSingleTraffictObjectInSim )
{
	// for each segment of a traffic object
	const CPath2008& path = pSingleTraffictObjectInSim->GetCenterPath();
	LandsideTrafficGraphVertexList graphVertexList;
	for( int iSegCount = 0; iSegCount < path.getCount() - 1; iSegCount++ )
	{
		// find all intersect point on segment iSegCount
		TrafficeNodeList intersectPointOnSegment;

		pSingleTraffictObjectInSim->GetIntersectionOnSegment( iSegCount, intersectPointOnSegment);

		ConnectIntersectionNode(pSingleTraffictObjectInSim ,intersectPointOnSegment);
		// sort intersect point by from start distance
		LandsideTrafficGraphVertexList vertexList;
		sortPointByFromStartDistance(pSingleTraffictObjectInSim, intersectPointOnSegment, vertexList);

		LandsideTrafficGraphVertex vertex;
		// add two end point
		if (iSegCount == 0)
		{
			vertex.SetDistance(0.0);
			vertex.SetTrafficObjectInSim(pSingleTraffictObjectInSim);
			vertexList.insert( vertexList.begin(), vertex );
		}

		if (iSegCount == path.getCount() - 2)
		{
			// last point
			vertex.SetDistance(path.GetTotalLength());
			vertex.SetTrafficObjectInSim(pSingleTraffictObjectInSim);
			vertexList.push_back( vertex );
		}
		graphVertexList.insert(graphVertexList.end(),vertexList.begin(),vertexList.end());
	}

	std::unique(graphVertexList.begin(),graphVertexList.end());

	insertVertexAndEdge( graphVertexList );
}

//---------------------------------------------------------------------------
//Summary:
//		using boost to create graph
//---------------------------------------------------------------------------
void CLandsideGraph::CreateBoostGraph()
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

//------------------------------------------------------------------------
//Summary:
//		clear the walkway graph system
//------------------------------------------------------------------------
bool CLandsideGraph::clearGraphMgr()
{
	if( m_pboostGraph )
	{
		delete m_pboostGraph;
		m_pboostGraph = NULL;
	}
	m_vertexList.clear();
	m_edgeList.clear();

	return true;
}
//-----------------------------------------------------------------------
//Summary:
//		find shortest path from entry point to exit point
//Parameters:
//		entryPos[in]: start point
//		pStartResource[in]: start landside traffic object
//		exitPos[in]: end point
//		pEndResource[in]: end landside traffic object
//		_shortestPath[out]: shortest path from start to end
//Return:
//		true: find the path
//		false: failed
//-----------------------------------------------------------------------
bool CLandsideGraph::getShortestPath( const CPoint2008& entryPos,LandsideResourceInSim* pStartLandsideResource,CLandsideWalkTrafficObjectInSim* pStartResource,const CPoint2008& exitPos,LandsideResourceInSim* pEndLandsideResource,CLandsideWalkTrafficObjectInSim* pEndResource,LandsideTrafficGraphVertexList& _shortestPath )
{
	CPoint2008 startPos;
	CPoint2008 endPos;
	if (pStartResource->GetShortestProjectPoint(pStartLandsideResource,entryPos,startPos) && pEndResource->GetShortestProjectPoint(pEndLandsideResource,exitPos,endPos))
	{
		LandsideTrafficGraphVertex startVertex, endVertex;

		pStartResource->GetWalkwayVertex(startVertex,startPos);
		pEndResource->GetWalkwayVertex(endVertex,endPos);

		LandsideTrafficGraphVertex entryVertex1, entryVertex2, exitVertex1, exitVertex2;
		pStartResource->GetNearestVertex(startPos, entryVertex1, entryVertex2);
		pEndResource->GetNearestVertex(endPos, exitVertex1, exitVertex2);

		LandsideTrafficGraphVertexList path[4];
		findShortestPath(entryVertex1, exitVertex1, path[0]);
		findShortestPath(entryVertex1, exitVertex2, path[1]);
		findShortestPath(entryVertex2, exitVertex1, path[2]);
		findShortestPath(entryVertex2, exitVertex2, path[3]);

		double exDistance[4];
		exDistance[0] = startPos.distance(entryVertex1.GetPointDist()) + endPos.distance(exitVertex1.GetPointDist());
		exDistance[1] = startPos.distance(entryVertex1.GetPointDist()) + endPos.distance(exitVertex2.GetPointDist());
		exDistance[2] = startPos.distance(entryVertex2.GetPointDist()) + endPos.distance(exitVertex1.GetPointDist());
		exDistance[3] = startPos.distance(entryVertex2.GetPointDist()) + endPos.distance(exitVertex2.GetPointDist());

		int shortestIndex = 0;
		for (int i = 1; i < 4; i++)
		{
			if (GetLength(path[i]) + exDistance[i] < GetLength(path[shortestIndex]) + exDistance[shortestIndex])
			{
				shortestIndex = i;
			}
		}

		_shortestPath = path[shortestIndex];
	
		_shortestPath.insert(_shortestPath.begin(), startVertex);
		_shortestPath.push_back(endVertex);
	}
	// add the source point and dest point
	LandsideTrafficGraphVertex sourceVertex;
	sourceVertex.SetPoint(entryPos);

	LandsideTrafficGraphVertex destVertex;
	destVertex.SetPoint(exitPos);

	_shortestPath.push_back(destVertex);
	_shortestPath.insert(_shortestPath.begin(), sourceVertex);
	return true;
}

bool CLandsideGraph::getShortestPath( const CPoint2008& entryPos,CLandsideWalkTrafficObjectInSim* pStartResource,const CPoint2008& exitPos,CLandsideWalkTrafficObjectInSim* pEndResource,LandsideTrafficGraphVertexList& _shortestPath )
{
	CPoint2008 startPos;
	CPoint2008 endPos;
	double dStart = 0.0;
	double dEnd = 0.0;
	if (pStartResource->GetProjectPoint(entryPos,startPos,dStart) && pEndResource->GetProjectPoint(exitPos,endPos,dEnd))
	{
		bool bSameSegPoint = false;
		if (pStartResource == pEndResource)
		{
			bSameSegPoint = pStartResource->SameSegment(startPos,endPos);
		}

		LandsideTrafficGraphVertex startVertex, endVertex;

		pStartResource->GetWalkwayVertex(startVertex,startPos);
		pEndResource->GetWalkwayVertex(endVertex,endPos);
		if (!bSameSegPoint)
		{
			LandsideTrafficGraphVertex entryVertex1, entryVertex2, exitVertex1, exitVertex2;
			pStartResource->GetNearestVertex(startPos, entryVertex1, entryVertex2);
			pEndResource->GetNearestVertex(endPos, exitVertex1, exitVertex2);

			LandsideTrafficGraphVertexList path[4];
			findShortestPath(entryVertex1, exitVertex1, path[0]);
			findShortestPath(entryVertex1, exitVertex2, path[1]);
			findShortestPath(entryVertex2, exitVertex1, path[2]);
			findShortestPath(entryVertex2, exitVertex2, path[3]);

			double exDistance[4];
			exDistance[0] = startPos.distance(entryVertex1.GetPointDist()) + endPos.distance(exitVertex1.GetPointDist());
			exDistance[1] = startPos.distance(entryVertex1.GetPointDist()) + endPos.distance(exitVertex2.GetPointDist());
			exDistance[2] = startPos.distance(entryVertex2.GetPointDist()) + endPos.distance(exitVertex1.GetPointDist());
			exDistance[3] = startPos.distance(entryVertex2.GetPointDist()) + endPos.distance(exitVertex2.GetPointDist());

			int shortestIndex = 0;
			for (int i = 1; i < 4; i++)
			{
				if (GetLength(path[i]) + exDistance[i] < GetLength(path[shortestIndex]) + exDistance[shortestIndex])
				{
					shortestIndex = i;
				}
			}

			_shortestPath = path[shortestIndex];
		}
		

		_shortestPath.insert(_shortestPath.begin(), startVertex);
		_shortestPath.push_back(endVertex);
	}
	// add the source point and dest point
	LandsideTrafficGraphVertex sourceVertex;
	sourceVertex.SetPoint(entryPos);

	LandsideTrafficGraphVertex destVertex;
	destVertex.SetPoint(exitPos);

	_shortestPath.push_back(destVertex);
	_shortestPath.insert(_shortestPath.begin(), sourceVertex);
	return true;
}

//----------------------------------------------------------------------------
//Summary:
//		find shortest path from start vertex and to end vertex
//Parameter:
//		sourceVertex[in]: start vertex
//		destVertex[in]: end vertex
//		_shortestPath[out]: passed shortest path
//----------------------------------------------------------------------------
void CLandsideGraph::findShortestPath(const LandsideTrafficGraphVertex& sourceVertex, const LandsideTrafficGraphVertex& destVertex,LandsideTrafficGraphVertexList& _shortestPath)
{
	_shortestPath.clear();

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
		_shortestPath.push_back(GetVertex(sourceVertexIndex));
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
		_shortestPath.push_back(GetVertex(indexList[i]));
	}
	std::reverse(_shortestPath.begin(), _shortestPath.end());
}

//---------------------------------------------------------------------------
//Summary:
//		connect intersection node and set up graph
//Parameters:
//		pSingleTraffictObjectInSim[in]: connect traffic object intersect node
//		_pointList: intersection nodes
//--------------------------------------------------------------------------
void CLandsideGraph::ConnectIntersectionNode(CLandsideWalkTrafficObjectInSim* pSingleTraffictObjectInSim, TrafficeNodeList& _pointList )
{
	if( _pointList.size()<=0 )
		return;

	for (unsigned i = 0; i < _pointList.size(); i++)
	{
		const trafficIntersectionNode& pt = _pointList[i];
		if (pt.m_pTrafficObject == NULL)
			continue;

		LandsideTrafficGraphVertex vertexS;
		LandsideTrafficGraphVertex vertexD;

		LandsideTrafficGraphVertexList vertexList;
		vertexS.SetTrafficObjectInSim(pSingleTraffictObjectInSim);
		vertexS.SetDistance(pt.m_dDistanceFromStart);
		vertexList.push_back(vertexS);

		vertexD.SetTrafficObjectInSim(pt.m_pTrafficObject);
		vertexD.SetDistance(pt.m_dDistWalkway);
		vertexList.push_back(vertexD);

		insertVertexAndEdge(vertexList);

	}
}

static bool sortByDistance(const trafficIntersectionNode& _point1, const trafficIntersectionNode& _point2 )
{
	return _point1.m_dDistanceFromStart < _point2.m_dDistanceFromStart; 
}

//--------------------------------------------------------------------------
//Summary:
//		convert intersection node to graph vertex and sort it
//Parameters:
//		pSingleTraffictObjectInSim[in]: convert traffic object
//		_pointList[in]: need to convert
//		_vectorList[out]: passed result vertex
//-------------------------------------------------------------------------
void CLandsideGraph::sortPointByFromStartDistance(CLandsideWalkTrafficObjectInSim* pSingleTraffictObjectInSim, TrafficeNodeList& _pointList, LandsideTrafficGraphVertexList& _vectorList )
{
	_vectorList.clear();
	if( _pointList.size()<=0 )
		return;

	std::sort( _pointList.begin(), _pointList.end(), sortByDistance );

	for( unsigned i=0; i<_pointList.size(); i++ )
	{
		const trafficIntersectionNode& point = _pointList[i];
		LandsideTrafficGraphVertex vertex;
		
		vertex.SetTrafficObjectInSim(pSingleTraffictObjectInSim);
		vertex.SetDistance(point.m_dDistanceFromStart);
		_vectorList.push_back( vertex );
	}
}

//--------------------------------------------------------------------------
//Summary:
//		insert vertex and relative edge
//Parameter:
//		_vertexList[in]: vertex list and create edge
//------------------------------------------------------------------------
void CLandsideGraph::insertVertexAndEdge( LandsideTrafficGraphVertexList& _vertexList )
{
	int iSourceIndex,iDestIndex;
	double weight;
	LandsideTrafficGraphEdge edgeS2D;

	//////////////////////////////////////////////////////////////////////////
	// because is undirect graph, so shall add two edge between two vertex
	LandsideTrafficGraphVertex vertexS = _vertexList[0];
	LandsideTrafficGraphVertex vertexD = _vertexList[1];
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

//----------------------------------------------------------------------
//Summary:
//		insert vertex to graph
//Parameters:
//		_item[in]: inserted item
//		_index[out]: index of insert item
//Return:
//		whether insert success
//----------------------------------------------------------------------
bool CLandsideGraph::insertVertex( const LandsideTrafficGraphVertex& _item, int& _index )
{
	LandsideTrafficGraphVertex newitem = _item;

	for( unsigned i=0; i<m_vertexList.size(); i++ )
	{
		LandsideTrafficGraphVertex vertex = m_vertexList[i];
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

//------------------------------------------------------------------------
//Summary:
//		insert edge to graph
//-----------------------------------------------------------------------
bool CLandsideGraph::insertEdge( const LandsideTrafficGraphEdge& edge )
{
	for( unsigned i=0; i<m_edgeList.size(); i++ )
	{
		LandsideTrafficGraphEdge item = m_edgeList[i];
		if( edge == item )
		{
			return false;	// find a old data
		}
	}

	m_edgeList.push_back( edge );
	return true;			// add a new data
}

//------------------------------------------------------------------------
//Summary:
//		get distance from two vertex
//------------------------------------------------------------------------
double CLandsideGraph::GetWeight( const LandsideTrafficGraphVertex& vertexS,const LandsideTrafficGraphVertex& vertexD )const
{
	if (vertexS.GetTrafficObjectType() == vertexD.GetTrafficObjectType())
	{
		return vertexS.GetDistance() - vertexD.GetDistance();
	}
	else
	{
		return vertexS.GetPoint().distance3D(vertexD.GetPoint());
	}
}

//-----------------------------------------------------------------------
//Summary:
//		find the index of graph vertex
//-----------------------------------------------------------------------
int CLandsideGraph::findVertex( const LandsideTrafficGraphVertex& vertex ) const
{
	for( unsigned i =0; i< m_vertexList.size(); i++ )
	{
		if (m_vertexList[i] == vertex )
			return i;
	}
	// can not find the item
	return -1;
}

//-----------------------------------------------------------------------
//Summary:
//		retrieve vertex by index
//-----------------------------------------------------------------------
const LandsideTrafficGraphVertex& CLandsideGraph::GetVertex( int vertexIndex ) const
{
	ASSERT(vertexIndex>= 0 && vertexIndex < (int)m_vertexList.size());
	return m_vertexList[vertexIndex];
}

//-----------------------------------------------------------------------
//Summary:
//		get vertex list total length
//-----------------------------------------------------------------------
double CLandsideGraph::GetLength( const LandsideTrafficGraphVertexList& path ) const
{
	double dLenght = 0.0;
	if (path.empty())
		return dLenght;

	LandsideTrafficGraphVertex vertex = path.at(0);
	for (size_t i = 1; i < path.size(); i++)
	{
		LandsideTrafficGraphVertex nextVertex = path.at(i);
		dLenght += vertex.GetPoint().distance3D(nextVertex.GetPoint());
		vertex = nextVertex;
	}
	return dLenght;
}

//----------------------------------------------------------------------------
//Summary:
//		step on conflict crosswalk
//----------------------------------------------------------------------------
void CFreeMovingLogic::StepConflictEvent( ElapsedTime& time )
{
	LandsideTrafficGraphVertex& vertex = m_routePath.back();
	if (vertex.GetTrafficObjectType() == CLandsideWalkTrafficObjectInSim::CrossWalk_Type)
	{
		CCrossWalkInSim* pCrossWalkInSim = (CCrossWalkInSim*)vertex.GetTrafficInSim();

		if (pCrossWalkInSim->getCrossType()==Cross_Zebra)
		{
			if (!pCrossWalkInSim->PaxValid())
			{
				pCrossWalkInSim->AddPaxApproach(this);
			}
			else
			{
				pCrossWalkInSim->StepOnCrossWalk(time,m_routePath,this);
			}
		}else if (pCrossWalkInSim->getCrossType()==Cross_Pelican)
		{
			if (pCrossWalkInSim->getPaxLightState()==CROSSLIGHT_RED)
			{
				pCrossWalkInSim->pressCtrlButton();
				pCrossWalkInSim->AddPaxApproach(this);
				
				CPoint2008 tmpCrossPos0 = pCrossWalkInSim->GetRightPath()[0];
				CPoint2008 tmpCrossPos1 = pCrossWalkInSim->GetRightPath()[1];

				ARCVector3 paxPos=GetBehavior()->getPoint();

				ARCVector3 dir;
				ARCVector3 crossPos0(tmpCrossPos0.x,tmpCrossPos0.y,tmpCrossPos0.z);
				ARCVector3 crossPos1(tmpCrossPos1.x,tmpCrossPos1.y,tmpCrossPos1.z);

				if (paxPos.DistanceTo(crossPos0)<paxPos.DistanceTo(crossPos1))
				{
                    dir=crossPos1-crossPos0;
				}else
				{
					dir=crossPos0-crossPos1;
				}
//                 GetBehavior()->setDestination(GetBehavior()->getPoint()+dir/100);
// 				GetBehavior()->setDestination(GetBehavior()->getPoint()+dir);
				GetBehavior()->setDestination(GetBehavior()->getPoint()+dir.Normalize()*100);
				GetBehavior()->WriteLogEntry(time + GetBehavior()->moveTime());
				m_routePath.pop_back();
			}else if(pCrossWalkInSim->getPaxLightState()==CROSSLIGHT_BUFFER)
			{
				pCrossWalkInSim->AddPaxApproach(this);
			}else if (pCrossWalkInSim->getPaxLightState()==CROSSLIGHT_GREEN)
			{
				if (pCrossWalkInSim->PaxValid())
				{					
					pCrossWalkInSim->StepOnCrossWalk(time,m_routePath,this);
				}
			}
		}else if (pCrossWalkInSim->getCrossType()==Cross_Intersection)
		{
			if (pCrossWalkInSim->getPaxLightState()==CROSSLIGHT_RED)
			{
				pCrossWalkInSim->AddPaxApproach(this);

				CPoint2008 tmpCrossPos0 = pCrossWalkInSim->GetRightPath()[0];
				CPoint2008 tmpCrossPos1 = pCrossWalkInSim->GetRightPath()[1];

				ARCVector3 paxPos=GetBehavior()->getPoint();

				ARCVector3 dir;
				ARCVector3 crossPos0(tmpCrossPos0.x,tmpCrossPos0.y,tmpCrossPos0.z);
				ARCVector3 crossPos1(tmpCrossPos1.x,tmpCrossPos1.y,tmpCrossPos1.z);

				if (paxPos.DistanceTo(crossPos0)<paxPos.DistanceTo(crossPos1))
				{
					dir=crossPos1-crossPos0;
				}else
				{
					dir=crossPos0-crossPos1;
				}
// 				GetBehavior()->setDestination(GetBehavior()->getPoint()+dir/100);
// 				GetBehavior()->setDestination(GetBehavior()->getPoint()+dir);
				GetBehavior()->setDestination(GetBehavior()->getPoint()+dir.Normalize()*100);
				GetBehavior()->WriteLogEntry(time + GetBehavior()->moveTime());
				m_routePath.pop_back();
			}else if(pCrossWalkInSim->getPaxLightState()==CROSSLIGHT_BUFFER)
			{
				pCrossWalkInSim->AddPaxApproach(this);

				CPoint2008 tmpCrossPos0 = pCrossWalkInSim->GetRightPath()[0];
				CPoint2008 tmpCrossPos1 = pCrossWalkInSim->GetRightPath()[1];

				ARCVector3 paxPos=GetBehavior()->getPoint();

				ARCVector3 dir;
				ARCVector3 crossPos0(tmpCrossPos0.x,tmpCrossPos0.y,tmpCrossPos0.z);
				ARCVector3 crossPos1(tmpCrossPos1.x,tmpCrossPos1.y,tmpCrossPos1.z);

				if (paxPos.DistanceTo(crossPos0)<paxPos.DistanceTo(crossPos1))
				{
					dir=crossPos1-crossPos0;
				}else
				{
					dir=crossPos0-crossPos1;
				}
// 				GetBehavior()->setDestination(GetBehavior()->getPoint()+dir/100);
// 				GetBehavior()->setDestination(GetBehavior()->getPoint()+dir);
				GetBehavior()->setDestination(GetBehavior()->getPoint()+dir.Normalize()*100);
				GetBehavior()->WriteLogEntry(time + GetBehavior()->moveTime());
				m_routePath.pop_back();
			}else if (pCrossWalkInSim->getPaxLightState()==CROSSLIGHT_GREEN)
			{
				if (pCrossWalkInSim->PaxValid())
				{					
					pCrossWalkInSim->StepOnCrossWalk(time,m_routePath,this);
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------
//Summary:
//		person move process in traffic system
//-----------------------------------------------------------------------------
void CFreeMovingLogic::ProcessMove( const ElapsedTime& time )
{
	ElapsedTime eTime = time;
	while(!m_routePath.empty())
	{
		LandsideTrafficGraphVertex& vertex = m_routePath.back();
		if (vertex.GetTrafficObjectType() == CLandsideWalkTrafficObjectInSim::CrossWalk_Type)
			return GenerateConflictEvent(eTime);
		else if(vertex.GetTrafficObjectType() == CLandsideWalkTrafficObjectInSim::Walkway_Type)
		{
			CWalkwayInSim* pWalkwayInSim = (CWalkwayInSim*)vertex.GetTrafficInSim();
			pWalkwayInSim->stepIt(m_pPaxLandsideBehavior,vertex,this,eTime);
		}
		else
		{
			//eTime += m_pPaxLandsideBehavior->moveTime();
			MoveTo(eTime,vertex.GetPoint());
			m_routePath.pop_back();
		}
	}

	m_pLandsideTrafficSys->LeaveTrafficSystem(m_pPaxLandsideBehavior,m_emEndState,eTime);
}

//------------------------------------------------------------------------------
//Summary:
//		Initialize free moving logic
//Parameters:
//		routePath[in]: moving path
//		pLandsideBehavior[in]: which person behavior
//		endState[in]: leave traffic system will generate state
//-----------------------------------------------------------------------------
void CFreeMovingLogic::Initialize( const LandsideTrafficGraphVertexList& routePath,PaxLandsideBehavior* pLandsideBehavior,int endState )
{
	m_routePath = routePath;
	std::reverse(m_routePath.begin(), m_routePath.end());

	m_pPaxLandsideBehavior = pLandsideBehavior;
	m_emEndState = endState;

}


//-----------------------------------------------------------------------
//Summary:
//		move to next point and write log
//-----------------------------------------------------------------------
void CFreeMovingLogic::MoveTo(ElapsedTime& eTime,const CPoint2008& pos ) 
{
	m_pPaxLandsideBehavior->setDestination(pos);
	eTime += m_pPaxLandsideBehavior->moveTime();
	m_pPaxLandsideBehavior->WriteLogEntry(eTime,false);		
}
//----------------------------------------------------------------------
//Summary:
//		generate conflict event to hand conflict
//----------------------------------------------------------------------
void CFreeMovingLogic::GenerateConflictEvent( const ElapsedTime& time )
{
	LandsideConflictMoveEvent *pEvent =  new LandsideConflictMoveEvent(this);
	pEvent->setTime(time);
	pEvent->addEvent();
}
//---------------------------------------------------------------------------
//Summary:
//		move step for vertex or point to next vertex or point
//---------------------------------------------------------------------------
void CFreeMovingLogic::Step( ElapsedTime& time, const LandsideTrafficGraphVertex& vertex)
{
	MoveTo(time,vertex.GetPoint());
	m_routePath.pop_back();
}

//---------------------------------------------------------------------------
//Summary:
//		move step for vertex or point to next vertex or point
//---------------------------------------------------------------------------
void CFreeMovingLogic::Step( ElapsedTime& time, const CPoint2008& pt )
{
	MoveTo(time,pt);
}

//--------------------------------------------------------------------------
//Summary:
//		popback vector item
//--------------------------------------------------------------------------
void CFreeMovingLogic::Move()
{
	m_routePath.pop_back();
}
//-------------------------------------------------------------------------
//Summary:
//		retrieve person behavior of free moving logic
//-------------------------------------------------------------------------
PaxLandsideBehavior* CFreeMovingLogic::GetBehavior() const
{
	ASSERT(m_pPaxLandsideBehavior);
	return m_pPaxLandsideBehavior;
}

//---------------------------------------------------------------------------
//Summary:
//		leave crosswalk and release the crosswalk
//--------------------------------------------------------------------------
void CFreeMovingLogic::ReleaseCrossWalk(CCrossWalkInSim* pCrossWalkInSim, const ElapsedTime& time )
{
	pCrossWalkInSim->ReleasePaxOccupied(this);
	pCrossWalkInSim->NoticefyVehicle(time);
	ProcessMove(time);
}

CFreeMovingLogic::CFreeMovingLogic(CLandsideTrafficSystem* pLandsideTrafficSys)
:m_pLandsideTrafficSys(pLandsideTrafficSys)
,m_pPaxLandsideBehavior(NULL)
,m_emEndState(-1)
{

}

CFreeMovingLogic::~CFreeMovingLogic()
{

}

void CFreeMovingLogic::ProcessMove2( const ElapsedTime& time )
{
	////move to the end leave free moving
	if(m_nextSteps.IsEmpty() && m_routePath.empty()){
		m_pLandsideTrafficSys->LeaveTrafficSystem(m_pPaxLandsideBehavior,m_emEndState,time);
		return;
	}


	static DistanceUnit minDist  = 50; //one step for a human
	//check conflict with vehicles , get the steps can move
	double dS = 150;

	//
	if(dS < minDist ){	 //wait	
		//update the time of past and future steps  
		ElapsedTime nextTime; //est waiting time
		
		//update past 
		m_pastSteps.WaitAtLastPosToTime(nextTime);
		m_nextSteps.ResetBeginTime(nextTime);
		
		//generate next event and return;
		GenerateConflictEvent(nextTime);
		return;
	}

	//move dS dist return 
	MovingTrace newpast;
	MovingTrace newfuture;
	m_nextSteps.Split(dS, newpast,newfuture);

	
	m_pastSteps = newpast;
	m_nextSteps = newfuture;

	//write log move the past steps
	ElapsedTime eTime = time;
	for(int i=0;i<m_pastSteps.GetCount();i++){
		MobileState& ptInfo= m_pastSteps.at(i);
		CPoint2008 mp = ptInfo.pos;
		m_pPaxLandsideBehavior->setDestination(mp);
		eTime += m_pPaxLandsideBehavior->moveTime();
		m_pPaxLandsideBehavior->setLocation(mp);
		ptInfo.time = eTime;
	
		if(ptInfo.isConerPt)
		{
			m_pPaxLandsideBehavior->WriteLogEntry(eTime,false);		
		}		
	}

	//update next steps in radius of concern
	MobileState ptInfo ;
	m_pastSteps.GetEndPoint(ptInfo);
	UpdateNextSteps(ptInfo);

	GenerateConflictEvent(eTime);

}

void CFreeMovingLogic::UpdateNextSteps(const MobileState& ptInfo )
{
	double dSpd = m_pPaxLandsideBehavior->getPerson()->getSpeed();
	//ElapsedTime futureend = m_nextSteps.RawCalcualteTime(eTime,dSpd );
	DistanceUnit dRadOfConern = 500;

	////future steps in 15 secs; do  need more future steps
	if( m_nextSteps.GetLength() < dRadOfConern ) 
	{
		//get more points from the path
		while(!m_routePath.empty())
		{			
			
			MobileState lastPtInfo;
			if(!m_nextSteps.GetEndPoint(lastPtInfo))
			{
				lastPtInfo = ptInfo;				
				m_nextSteps.AddPoint(lastPtInfo);
			}

			LandsideTrafficGraphVertex& vertex = m_routePath.back();
			if(CLandsideWalkTrafficObjectInSim* pObj =  vertex.GetTrafficInSim())
			{				
				if(CWalkwayInSim* pWalkwayInSim = pObj->toWalkway() )
				{				
					if(m_pPaxLandsideBehavior->getCurrentVertex().GetTrafficInSim() == pWalkwayInSim)
					{
						CPath2008 path = pWalkwayInSim->GetCenterPath();
						double dStartPos = m_pPaxLandsideBehavior->getCurrentVertex().GetDistance();
						double dEndPos = vertex.GetDistance();
						CPath2008 walkPath = path.GetSubPath(dStartPos,dEndPos);

					
						MobileState newPtInfo = lastPtInfo;

						for (int i = 1; i < walkPath.getCount(); i++)
						{
							newPtInfo.pos = walkPath.getPoint(i);
							newPtInfo.isConerPt = true;					
							m_nextSteps.AddPoint(lastPtInfo);
							
						}
						m_pPaxLandsideBehavior->setCurrentVertex(vertex);
						m_routePath.pop_back();
						if(m_nextSteps.GetLength()>dRadOfConern){
							break;
						}
						continue;
					}
				}
			}
			
			//
			{				
				MobileState newPtInfo;
				newPtInfo.pos = vertex.GetPoint();
				newPtInfo.isConerPt = true;
				m_nextSteps.AddPoint(newPtInfo);	

				
				m_pPaxLandsideBehavior->setCurrentVertex(vertex);
				m_routePath.pop_back();
				if( m_nextSteps.GetLength() > dRadOfConern){
					break;
				}
			}
		}
	}
	m_nextSteps.RawCalcualteTime(ptInfo.time,dSpd);
	//generate next event and return
}

void CFreeMovingLogic::StartMove2( const CPoint2008& curPos,const ElapsedTime& curTime, bool bConerPt )
{
	MobileState ptinfo;
	ptinfo.pos = curPos;
	ptinfo.time = curTime;
	ptinfo.isConerPt = bConerPt;
	UpdateNextSteps(ptinfo);
	GenerateConflictEvent(curTime);
}


