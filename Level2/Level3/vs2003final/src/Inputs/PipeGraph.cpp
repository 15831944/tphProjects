// PipeGraph.cpp: implementation of the CPipeGraph class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "PipeGraph.h"
#include "inputs\in_term.h"
#include "inputs\pipedataset.h"
#include "inputs\paxflow.h"
#include "inputs\MobileElemConstraintDatabase.h"
#include "inputs\SubFlow.h"
#include "inputs\SubFlowList.h"
#include "inputs\SinglePaxTypeFlow.h"
#include "common\line.h"
#include "common\CodeTimeTest.h"
#include <limits>
#include <boost\graph\dijkstra_shortest_paths.hpp>
#include <boost\graph\connected_components.hpp>
#include "../Common/ARCTracker.h"


// vertex class
CGraphVertex::CGraphVertex()
 : m_vertexType(NORMALPOINT)
 ,m_pipeIndex(-1)
{
}

CGraphVertex::~CGraphVertex()
{
}

bool CGraphVertex::operator ==(const CGraphVertex& _vertex)
{
	return vertexPoint.preciseCompare(_vertex.vertexPoint);
}

const Point& CGraphVertex::getVertexPoint() const
{
	return vertexPoint;
}

void CGraphVertex::setVertexpoint( const Point& _point)
{
	vertexPoint = _point;
}

void CGraphVertex::setVertexType(GRAPH_VERTEX_TYPE _type)
{
	m_vertexType = _type;
}

GRAPH_VERTEX_TYPE CGraphVertex::getVertexType() const
{
	return m_vertexType;
}

void CGraphVertex::setPipeIndex( int _index )
{
	m_pipeIndex = _index;
}

int CGraphVertex::getPipeIndex( void ) const
{
	return m_pipeIndex;
}

void CGraphVertex::setPt2Pipe( const CPointToPipeXPoint& _pt2Pipe )
{
	assert( m_vertexType == POINT2PIPE );
	m_pt2Pipe = _pt2Pipe;
}

const CPointToPipeXPoint& CGraphVertex::getPt2Pipe( void ) const
{
	assert( m_vertexType ==  POINT2PIPE );
	return m_pt2Pipe;
}

void CGraphVertex::setPtOnPipe( const PIPEPOINT& _ptOnPipe )
{
	assert( m_vertexType == POINTONPIPE );
	m_ptOnPipe = _ptOnPipe;
}

const PIPEPOINT& CGraphVertex::getPtOnPipe( void ) const
{
	assert( m_vertexType == POINTONPIPE );
	return m_ptOnPipe;
}

void CGraphVertex::setPipe2Pipe( const CPipeToPipeXPoint& _pipe2Pipe )
{
	assert( m_vertexType == PIPE2PIPE );
	m_pipe2Pipe = _pipe2Pipe;
}

const CPipeToPipeXPoint& CGraphVertex::getPipe2Pipe( void ) const
{
	assert( m_vertexType == PIPE2PIPE );
	return m_pipe2Pipe;
}

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// vertex list class
CGraphVertexList::CGraphVertexList()
 : m_length(0.0)
{
}

CGraphVertexList::~CGraphVertexList()
{
}

bool CGraphVertexList::insertItem( const CGraphVertex& _item,int& _index )
{
	for( unsigned i=0; i<m_vertexList.size(); i++ )
	{
		CGraphVertex vertex = m_vertexList[i];
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

void CGraphVertexList::SetItemPoint(int _index, const Point& _itemPoint)
{
	if (_index < 0 || _index >= (int)m_vertexList.size())
		return;

	m_vertexList.at(_index).setVertexpoint(_itemPoint);
}

int CGraphVertexList::findItem( const CGraphVertex& _item )
{
	for( unsigned i =0; i< m_vertexList.size(); i++ )
	{
		if (m_vertexList[i] == _item )
			return i;
	}
	// can not find the item
	return -1;
}

const CGraphVertex& CGraphVertexList::getItem( int _index )
{
	assert( _index>=0 && (unsigned)_index<m_vertexList.size() );

	return m_vertexList[_index];
}

void CGraphVertexList::clearData()
{
	m_vertexList.clear();
}

int CGraphVertexList::getCount() const
{
	return m_vertexList.size();
}

void CGraphVertexList::removeItem( int _index )
{
	m_vertexList.erase( m_vertexList.begin() + _index );
}

CGraphVertexList& CGraphVertexList::operator = ( const CGraphVertexList& _list )
{
	m_vertexList = _list.m_vertexList;
	return *this;
}

void CGraphVertexList::invertList()
{
	std::reverse(m_vertexList.begin(), m_vertexList.end());
	return;
}

void CGraphVertexList::addItem( const CGraphVertex& _item )
{
	m_vertexList.push_back( _item );
	return;
}

bool CGraphVertexList::FindPath(CGraphVertexList& pipePath, const CGraphVertex& sourceVertex, const CGraphVertex& destVertex)
{
	pipePath.clearData();
	if (m_vertexList.empty())
		return false;

	std::vector<CGraphVertex>::iterator iterSourceVertex = m_vertexList.end();
	std::vector<CGraphVertex>::iterator iterDestVertex = m_vertexList.end();
	for (std::vector<CGraphVertex>::iterator iter = m_vertexList.begin(); iter != m_vertexList.end(); ++iter)
	{
		if (*iter == sourceVertex)
			iterSourceVertex = iter;
		else if (*iter == destVertex)
			iterDestVertex = iter;

		if (iterSourceVertex != m_vertexList.end() && iterDestVertex != m_vertexList.end())
		{
			if (iterSourceVertex < iterDestVertex)
				pipePath.getVertexList().assign(iterSourceVertex, iterDestVertex + 1);
			else
			{
				pipePath.getVertexList().assign(iterDestVertex, iterSourceVertex + 1);
				pipePath.invertList();
			}
			
			pipePath.ReCalculateLength();
			return true;
		}
	}

	return false;
}

double CGraphVertexList::GetLength()
{
	return m_length;
}

void CGraphVertexList::SetLength(double dLength)
{
	m_length = dLength;
}

void CGraphVertexList::ReCalculateLength()
{
	m_length = 0.0;
	if (m_vertexList.empty())
		return;
	
	std::vector<CGraphVertex>::iterator iterFirst = m_vertexList.begin();
	for (std::vector<CGraphVertex>::iterator iterSecond = m_vertexList.begin() + 1;
		 iterSecond != m_vertexList.end(); ++iterSecond)
	{
		m_length += iterFirst->getVertexPoint().distance(iterSecond->getVertexPoint());
		iterFirst = iterSecond;
	}
}



//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// shortest path class
CShortestPathInGraph::CShortestPathInGraph()
{
pathDistance = 0.0;
}

CShortestPathInGraph::~CShortestPathInGraph()
{

}

// adjust PorcessorID is equal
// if  source == _soure && destination == _dest return 1;
// if  sourec == _dest && destination == _source return -1;
// else return 0;
int CShortestPathInGraph::isFit( const Point& _soure, const Point& _dest ) const
{
	if (source.distance3D(_soure) < 100.0 && destination.distance3D(_dest) < 100.0)
		return 1;
	else if (source.distance3D(_dest) < 100.0 && destination.distance3D(_soure) < 100.0)
		return -1;

	return 0;	// no fits
}

// get & set
const Point& CShortestPathInGraph::getSource() const
{
	return source;
}

const Point& CShortestPathInGraph::getDestination() const
{
	return destination;
}

void CShortestPathInGraph::getShortestPath( CGraphVertexList& _path ) const 
{
	_path = shortestPath;
}

double CShortestPathInGraph::getPathDistance() const
{
	return pathDistance;
}

void CShortestPathInGraph::setSource( const Point& _source )
{
	source = _source;
}

void CShortestPathInGraph::setDestination( const Point& _dest )
{
	destination = _dest;
}

void CShortestPathInGraph::setShortestPath( const CGraphVertexList& _path )
{
	shortestPath = _path;
}

void CShortestPathInGraph::setPathDistance( double _distance )
{
	pathDistance = _distance;
}

void CShortestPathInGraph::clearData()
{
	shortestPath.clearData();
	pathDistance = 0.0;
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// shortest path list class
CShortestPathList::CShortestPathList()
{

}
CShortestPathList::~CShortestPathList()
{

}

int CShortestPathList::getCount() const
{
	return pathList.size();
}

void CShortestPathList::addItem( const CShortestPathInGraph& _path )
{
	CShortestPathInGraph *pShortestPath = new CShortestPathInGraph;
	*pShortestPath = _path;
	pathList.push_back( pShortestPath );
}

void CShortestPathList::clearData()
{
	for( unsigned i=0; i<pathList.size(); i++ )
	{
		CShortestPathInGraph *pShortestPath = pathList[i];
		if(pShortestPath)
			pShortestPath->clearData();
		delete pShortestPath;
	}
	pathList.clear();
}

bool CShortestPathList::isExist( const Point& _source, const Point& _dest ) const 
{
	for( unsigned i = 0; i<pathList.size(); i++ )
	{
		CShortestPathInGraph* pShortestPath = pathList[i];
		if(pShortestPath && pShortestPath->isFit( _source, _dest ) )
			return true;
	}

	return false;
}

bool CShortestPathList::getShortestPath( const Point& _source, const Point& _dest , CGraphVertexList& _path )
{
	for( unsigned i = 0; i<pathList.size(); i++ )
	{
		CShortestPathInGraph* pShortestPath = pathList[i];
		if(pShortestPath )
		{
			int iReturn =  pShortestPath->isFit( _source, _dest );
			if( iReturn == 0 )
				continue;

		if( iReturn == 1 )
			pShortestPath->getShortestPath( _path );
		else if( iReturn == -1 ) // must reserve the path
		{
			pShortestPath->getShortestPath( _path );
			_path.invertList();
		}
		return true;

		}
	}
				
	return false;	// can not find a shortest path
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// pipe graph class
CPipeGraph::CPipeGraph(InputTerminal* pInTerm)
 : m_pInTerm(pInTerm)
 , m_pboostGraph(NULL)
{
}

CPipeGraph::~CPipeGraph()
{
	delete m_pboostGraph;
	m_pboostGraph = NULL;
}

void CPipeGraph::clearData()
{
	m_allVertex.clearData();
	m_allEdge.clear();
	m_allEdgeWeight.clear();

	delete m_pboostGraph;
	m_pboostGraph = NULL;
}


// build the boost graph by pipeList
int CPipeGraph::buildBoostGraph()
{
	// clear old data
	clearData();

	// for each pipe on the layer
	int sizeOfPipe = m_pipeList.size();
	if( sizeOfPipe <= 0)
		return -1;

	for( int iPipeCout =0; iPipeCout<sizeOfPipe; iPipeCout++ )
	{
		CPipe* pipe = m_pInTerm->m_pPipeDataSet->GetPipeAt( m_pipeList[iPipeCout] );
		// for each segement of a pipe
		for( int iSegCount = 0; iSegCount < pipe->GetPipePointCount()-1; iSegCount++ )
		{
			// find all intersect point on segment iSegCount
			PIPEINTERSECTIONPOINTLIST intersectPointOnSegment;
			pipe->getIntersectPointOnSegement( iSegCount, intersectPointOnSegment );
			
			// sort intersect point by from start distance
			std::vector<CGraphVertex> vertexList;
			sortPointByFromStartDistance( intersectPointOnSegment, vertexList , m_pipeList[iPipeCout] );
			
			// add two end point
			CGraphVertex vertex;
			vertex.setVertexpoint( pipe->GetPipePointAt( iSegCount ).m_point );
			//////////////////////////////////////////////////////////////////////////
			vertex.setVertexType( POINTONPIPE );
			vertex.setPtOnPipe( pipe->GetPipePointAt( iSegCount ) );
			vertex.setPipeIndex( m_pipeList[iPipeCout] );
			//////////////////////////////////////////////////////////////////////////
			vertexList.insert( vertexList.begin(), vertex );
			vertex.setVertexpoint( pipe->GetPipePointAt( iSegCount+1 ).m_point );
			//////////////////////////////////////////////////////////////////////////
			vertex.setVertexType( POINTONPIPE );
			vertex.setPtOnPipe( pipe->GetPipePointAt( iSegCount+1 ) );
			vertex.setPipeIndex( m_pipeList[iPipeCout] );
			//////////////////////////////////////////////////////////////////////////
			vertexList.push_back( vertex );
			
			//add vertex to m_allVertexList
			//add edge to m_allEdgeList
			//add edge weight to m_allEdgeWeight
			insertVertexAndEdge( vertexList );
		}
	}
	// ok! now we have push all vertex, all edge and all weight of edge to list
	// now it time to build boost Graph
	createBoostGraph();
	
	return 0;
}

static bool sortByDistance(const CPipeToPipeXPoint& _point1, const CPipeToPipeXPoint& _point2 )
{
	return _point1.GetDistanceFromStartPt() < _point2.GetDistanceFromStartPt(); 
}

void CPipeGraph::sortPointByFromStartDistance( PIPEINTERSECTIONPOINTLIST& _pointList, std::vector<CGraphVertex>& _vectorList ,int _pipeIndex)
{
	_vectorList.clear();
	if( _pointList.size()<=0 )
		return;

	std::sort( _pointList.begin(), _pointList.end(), sortByDistance );

	for( unsigned i=0; i<_pointList.size(); i++ )
	{
		CPipeToPipeXPoint Xpoint = _pointList[i];
		CGraphVertex vertex;
		vertex.setVertexpoint( Xpoint.GetPoint() );
		if( _pipeIndex != -1)
		{
			//////////////////////////////////////////////////////////////////////////
			vertex.setVertexType( PIPE2PIPE );
			vertex.setPipe2Pipe( Xpoint );
			vertex.setPipeIndex( _pipeIndex );
			//////////////////////////////////////////////////////////////////////////
			}
		_vectorList.push_back( vertex );
		
	}

	return;
}



void CPipeGraph::insertVertexAndEdge( std::vector<CGraphVertex>& _vertexList )
{
	int iSourceIndex,iDestIndex;
	double weight;
	CEdge edgeS2D,edgeD2S;

	//////////////////////////////////////////////////////////////////////////
	// because is undirect graph, so shall add two edge between two vextex
	CGraphVertex vertexS = _vertexList[0];
	CGraphVertex vertexD = _vertexList[1];
	weight = vertexS.getVertexPoint().distance( vertexD.getVertexPoint() );	
	if( weight == 0.0 )
	{
		m_allVertex.insertItem( vertexS, iDestIndex );
		iSourceIndex = iDestIndex;
	}
	else
	{
		m_allVertex.insertItem( vertexS, iSourceIndex );
		m_allVertex.insertItem( vertexD, iDestIndex );
		
		edgeS2D.first = iSourceIndex;
		edgeS2D.second = iDestIndex;
		m_allEdge.push_back( edgeS2D );
		m_allEdgeWeight.push_back( weight);
	}

	for( unsigned i=1; i<_vertexList.size()-1; i++ )
	{
		vertexS = vertexD;
		vertexD = _vertexList[i+1];
		weight = vertexS.getVertexPoint().distance( vertexD.getVertexPoint() );	

		if( weight != 0.0 )
		{
			iSourceIndex = iDestIndex;
			m_allVertex.insertItem( vertexD, iDestIndex );
			
			edgeS2D.first = iSourceIndex;
			edgeS2D.second = iDestIndex;
			m_allEdge.push_back( edgeS2D );
			m_allEdgeWeight.push_back( weight );
		}
	}

	return;
}

// new a boost graph
void CPipeGraph::createBoostGraph()
{
	if( m_pboostGraph )
	{
		delete m_pboostGraph;
		m_pboostGraph = NULL;
	}
	
	// new a boost graph ,then add all edge
	int numofVertex = m_allVertex.getCount();
	m_pboostGraph = new BoostGraph( numofVertex );
	if( m_pboostGraph == NULL )
	{
		throw new StringError( " Create Boost Graph Failed! "); 
	}

	boost::property_map< BoostGraph, boost::edge_weight_t >::type weightmap = boost::get(boost::edge_weight, *m_pboostGraph );
	for( std::size_t i=0; i<m_allEdge.size(); i++ )
	{
		edge_descriptor edge;
		bool inserted;
		boost::tie(edge, inserted ) = boost::add_edge( m_allEdge[i].first, m_allEdge[i].second, *m_pboostGraph );
		weightmap[edge] = m_allEdgeWeight[i];
	}

	m_connectedComponents.resize(boost::num_vertices(*m_pboostGraph));
	boost::connected_components(*m_pboostGraph, &m_connectedComponents[0]);

	for (std::vector<int>::iterator iterPipeIndex = m_pipeList.begin(); iterPipeIndex != m_pipeList.end(); ++iterPipeIndex)
	{
		CPipe* pipe = m_pInTerm->m_pPipeDataSet->GetPipeAt(*iterPipeIndex);
		int componentIndex = GetPipeConnectedComponentIndex(pipe);
		std::map<int, std::vector<int> >::iterator iterComponent = m_conntectedPipeList.find(componentIndex);
		if (iterComponent == m_conntectedPipeList.end())
		{
			m_conntectedPipeList[componentIndex] = std::vector<int>(1, *iterPipeIndex);
		}
		else
		{
			iterComponent->second.push_back(*iterPipeIndex);
		}
	}

	return;
}

int CPipeGraph::GetPipeConnectedComponentIndex(CPipe* pipe)
{
	CGraphVertex pipeVertex;
	pipeVertex.setVertexpoint(pipe->GetPipePointAt(0).m_point);

	int vertexIndex = m_allVertex.findItem(pipeVertex);
	if (vertexIndex == -1)
		return -1;

	return m_connectedComponents[vertexIndex];
}

void CPipeGraph::findShortestPath(const Point& _pointSource, const Point& _pointDest, CGraphVertexList& shortestPath)
{
	CGraphVertex entryVertex;
	CGraphVertex exitVertex;
	if (!GetEntryExitVertex(_pointSource, _pointDest, entryVertex, exitVertex))
		return;
	
	if (entryVertex.getVertexType() == POINTONPIPE && exitVertex.getVertexType() == POINTONPIPE)
	{
		FindShortestPathInGraph(shortestPath, entryVertex, exitVertex);
	}
	else if (entryVertex.getVertexType() == POINTONPIPE && exitVertex.getVertexType() == POINT2PIPE)
	{
		const CPointToPipeXPoint& exitPoint = exitVertex.getPt2Pipe();
		CPipe* exitPipe = m_pInTerm->m_pPipeDataSet->GetPipeAt(exitPoint.GetPipeIndex());
		
		CGraphVertex exitVertex1, exitVertex2;
		exitPipe->GetNearestVertex(exitPoint, exitVertex1, exitVertex2);

		CGraphVertexList path1, path2;
		FindShortestPathInGraph(path1, entryVertex, exitVertex1);
		FindShortestPathInGraph(path2, entryVertex, exitVertex2);

		double startDistance1 = exitPoint.distance(exitVertex1.getVertexPoint());
		double startDistance2 = exitPoint.distance(exitVertex2.getVertexPoint());

		if (path1.GetLength() + startDistance1 < path2.GetLength() + startDistance2)
			shortestPath = path1;
		else
			shortestPath = path2;
		
		shortestPath.getVertexList().push_back(exitVertex);
	}
	else if (entryVertex.getVertexType() == POINT2PIPE && exitVertex.getVertexType() == POINTONPIPE)
	{
		const CPointToPipeXPoint& entryPoint = entryVertex.getPt2Pipe();
		CPipe* entryPipe = m_pInTerm->m_pPipeDataSet->GetPipeAt(entryPoint.GetPipeIndex());

		CGraphVertex entryVertex1, entryVertex2;
		entryPipe->GetNearestVertex(entryPoint, entryVertex1, entryVertex2);

		CGraphVertexList path1, path2;
		FindShortestPathInGraph(path1, entryVertex1, exitVertex);
		FindShortestPathInGraph(path2, entryVertex2, exitVertex);

		double startDistance1 = entryPoint.distance(entryVertex1.getVertexPoint());
		double startDistance2 = entryPoint.distance(entryVertex2.getVertexPoint());

		if (path1.GetLength() + startDistance1 < path2.GetLength() + startDistance2)
			shortestPath = path1;
		else
			shortestPath = path2;

		shortestPath.getVertexList().insert(shortestPath.getVertexList().begin(), entryVertex);
	}
	else if (entryVertex.getVertexType() == POINT2PIPE && exitVertex.getVertexType() == POINT2PIPE)
	{
		const CPointToPipeXPoint& entryPoint = entryVertex.getPt2Pipe();
		const CPointToPipeXPoint& exitPoint = exitVertex.getPt2Pipe();
		if (!(entryPoint.GetPipeIndex() == exitPoint.GetPipeIndex() && entryPoint.GetSegmentIndex() == exitPoint.GetSegmentIndex()))
		{
			CPipe* entryPipe = m_pInTerm->m_pPipeDataSet->GetPipeAt(entryPoint.GetPipeIndex());
			CPipe* exitPipe = m_pInTerm->m_pPipeDataSet->GetPipeAt(exitPoint.GetPipeIndex());

			CGraphVertex entryVertex1, entryVertex2, exitVertex1, exitVertex2;
			entryPipe->GetNearestVertex(entryPoint, entryVertex1, entryVertex2);
			exitPipe->GetNearestVertex(exitPoint, exitVertex1, exitVertex2);

			CGraphVertexList path[4];
			FindShortestPathInGraph(path[0], entryVertex1, exitVertex1);
			FindShortestPathInGraph(path[1], entryVertex1, exitVertex2);
			FindShortestPathInGraph(path[2], entryVertex2, exitVertex1);
			FindShortestPathInGraph(path[3], entryVertex2, exitVertex2);

			double exDistance[4];
			exDistance[0] = entryPoint.distance(entryVertex1.getVertexPoint()) + exitPoint.distance(exitVertex1.getVertexPoint());
			exDistance[1] = entryPoint.distance(entryVertex1.getVertexPoint()) + exitPoint.distance(exitVertex2.getVertexPoint());
			exDistance[2] = entryPoint.distance(entryVertex2.getVertexPoint()) + exitPoint.distance(exitVertex1.getVertexPoint());
			exDistance[3] = entryPoint.distance(entryVertex2.getVertexPoint()) + exitPoint.distance(exitVertex2.getVertexPoint());

			int shortestIndex = 0;
			for (int i = 1; i < 4; i++)
			{
				if (path[i].GetLength() + exDistance[i] < path[shortestIndex].GetLength() + exDistance[shortestIndex])
				{
					shortestIndex = i;
				}
			}

			shortestPath = path[shortestIndex];
		}
		shortestPath.getVertexList().insert(shortestPath.getVertexList().begin(), entryVertex);
		shortestPath.getVertexList().push_back(exitVertex);
	}

	// add the source point and dest point
	CGraphVertex sourceVertex;
	sourceVertex.setVertexType(NORMALPOINT);
	sourceVertex.setVertexpoint(_pointSource);

	CGraphVertex destVertex;
	sourceVertex.setVertexType(NORMALPOINT);
	sourceVertex.setVertexpoint(_pointDest);

	shortestPath.getVertexList().push_back(destVertex);
	shortestPath.getVertexList().insert(shortestPath.getVertexList().begin(), sourceVertex);
}

bool CPipeGraph::GetEntryExitVertex(const Point& sourcePoint, const Point& destPoint, CGraphVertex& entryVertex, CGraphVertex& exitVertex)
{
	double minDistance = (std::numeric_limits<double>::max)();
	CGraphVertex thisEntryVertex, thisExitVertex;

	for (std::map<int, std::vector<int> >::iterator iter = m_conntectedPipeList.begin();
		 iter != m_conntectedPipeList.end(); ++iter)
	{
		GetEntryVertex(sourcePoint, iter->first, thisEntryVertex);
		GetEntryVertex(destPoint, iter->first, thisExitVertex);
		double thisDistance = thisEntryVertex.getVertexPoint().distance(sourcePoint) + thisExitVertex.getVertexPoint().distance(destPoint);
		if (thisDistance < minDistance)
		{
			entryVertex = thisEntryVertex;
			exitVertex = thisExitVertex;
			minDistance = thisDistance;
		}
	}

	return true;
}

void CPipeGraph::GetEntryVertex(const Point& point, const int componentIndex, CGraphVertex& vertex)
{
	std::map<int, std::vector<int> >::iterator iterComponent = m_conntectedPipeList.find(componentIndex);
	if (iterComponent == m_conntectedPipeList.end())
		return;

	double minDistance = (std::numeric_limits<double>::max)();
	CPointToPipeXPoint xPoint;
	int xPipeIndex = -1;
	
	for (std::vector<int>::iterator iter = iterComponent->second.begin();
		iter != iterComponent->second.end(); ++iter)
	{
		CPipe* pipe = m_pInTerm->m_pPipeDataSet->GetPipeAt(*iter);
		CPointToPipeXPoint thisXpoint = pipe->GetIntersectionPoint(point);
		double thisDistance = thisXpoint.distance(point);
		if (thisDistance < minDistance)
		{
			minDistance = thisDistance;
			xPoint = thisXpoint;
			xPipeIndex = *iter;
		}
	}

	xPoint.SetPipe(xPipeIndex);
	vertex.setVertexpoint(xPoint);
	vertex.setPipeIndex(xPipeIndex);

	if (xPoint.GetPointOnPipe() == false)//not point in path of pipe
	{
		vertex.setVertexType(POINT2PIPE);
		vertex.setPt2Pipe(xPoint);
	}
	else
	{
		vertex.setVertexType(POINTONPIPE);
		vertex.setPtOnPipe(m_pInTerm->m_pPipeDataSet->GetPipeAt(xPipeIndex)->GetPipePointAt(xPoint.GetPointIndex()));
	}
}

void CPipeGraph::FindShortestPathInGraph(CGraphVertexList& pipePath, const CGraphVertex& sourceVertex, const CGraphVertex& destVertex)
{
	pipePath.clearData();
	if (FindShortestPathFromLib(pipePath, sourceVertex, destVertex))
		return;

	int sourceVertexIndex = m_allVertex.findItem(sourceVertex);
	int destVertexIndex = m_allVertex.findItem(destVertex);

	// not stored in the shortest path library, need calculate it
	boost::property_map<BoostGraph, boost::edge_weight_t>::type weightmap = boost::get(boost::edge_weight, *m_pboostGraph);
	std::vector<vertex_descriptor> parentsVertex(boost::num_vertices(*m_pboostGraph));
	std::vector<double> distanced(boost::num_vertices(*m_pboostGraph));
	boost::property_map<BoostGraph, boost::vertex_index_t>::type indexmap = boost::get(boost::vertex_index, *m_pboostGraph);

	boost::dijkstra_shortest_paths(*m_pboostGraph, sourceVertexIndex, boost::predecessor_map(&parentsVertex[0]).distance_map(&distanced[0]));

	if (parentsVertex[destVertexIndex] == destVertexIndex)
	{
		pipePath.getVertexList().push_back(m_allVertex.getItem(sourceVertexIndex));
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
		pipePath.addItem(m_allVertex.getItem(indexList[i]));
	}
	pipePath.invertList();
	pipePath.SetLength(distanced[destVertexIndex]);

	//m_shortestPathLib.push_back(pipePath);
}

bool CPipeGraph::FindShortestPathFromLib(CGraphVertexList& pipePath, const CGraphVertex& sourceVertex, const CGraphVertex& destVertex)
{
	for (std::vector<CGraphVertexList>::iterator pathIter = m_shortestPathLib.begin();
		pathIter != m_shortestPathLib.end(); ++pathIter)
	{
		if (pathIter->FindPath(pipePath, sourceVertex, destVertex))
			return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
//CPipeGraphMgr
CPipeGraphMgr::CPipeGraphMgr()
{
}

CPipeGraphMgr::~CPipeGraphMgr()
{
	clearGraphMgr();
}

// initial the pipe graph system
bool CPipeGraphMgr::initGraphMgr( InputTerminal* _pInTerm, int _numOfLayer )
{
	PLACE_METHOD_TRACK_STRING();
	assert( _pInTerm );
	clearGraphMgr();
	
	m_allPipeGraph.resize(_numOfLayer, CPipeGraph(_pInTerm));
	
	int iPipeNum = _pInTerm->m_pPipeDataSet->GetPipeCount();
	for( int i=0; i<iPipeNum; i++ )
	{
		CPipe *pipe = _pInTerm->m_pPipeDataSet->GetPipeAt( i );
		int pipePtNum = pipe->GetPipePointCount();
		if( pipePtNum <=1 )
			continue;

		const PIPEPOINT pipePoint = pipe->GetPipePointAt( 0 );
		int layerIndex = (int)(pipePoint.m_point.getZ() / SCALE_FACTOR);
		assert( layerIndex < _numOfLayer );

		m_allPipeGraph[layerIndex].m_pipeList.push_back( i );
	}
	
	// init every pipeGraph
	for( i=0; i<_numOfLayer; i++ )
	{
		m_allPipeGraph[i].buildBoostGraph();
	}
	return true;
}

// clear the pipe grap system
bool CPipeGraphMgr::clearGraphMgr( )
{
	m_allPipeGraph.clear();
	m_allShortestPath.clearData();
	
	return true;
}

bool CPipeGraphMgr::getShortestPathFromLib(const Point& sourcePoint, const Point& destPoint, CGraphVertexList& _shortestPath )
{
	// still exist in library
	if (m_allShortestPath.getShortestPath(sourcePoint, destPoint, _shortestPath))
		return true;
	
	int iFloor = (int)(sourcePoint.getZ()/SCALE_FACTOR);
	assert(iFloor >= 0 && iFloor < (int)m_allPipeGraph.size());
	m_allPipeGraph[iFloor].findShortestPath(sourcePoint, destPoint, _shortestPath);
	int iRemovePointCount = removeRedundantPoint( _shortestPath );
	 
	CShortestPathInGraph shortestPath;
	shortestPath.setSource(sourcePoint);
	shortestPath.setDestination(destPoint);
	shortestPath.setShortestPath(_shortestPath);

	//m_allShortestPath.addItem(shortestPath);

	return true;
}

int CPipeGraphMgr::removeRedundantPoint( CGraphVertexList& _shortestPath )
{
	std::vector<int> removeIndexList;
	int iCount = _shortestPath.getCount();

	if(iCount <3)
		return 0;
	
	assert( iCount>=3 );

	CGraphVertex vertex;
	Point point1,point2,point3;
	point1.SetPoint( _shortestPath.getItem(0).getVertexPoint() );
	point2.SetPoint( _shortestPath.getItem(1).getVertexPoint() );
	Line line1( point1, point2 );
	int prePointIndex = 1;
	
	for( int i=2; i<iCount-1; i++ )
	{
		vertex = _shortestPath.getItem( i );
		if( vertex.getVertexType() == POINT2PIPE && i<iCount-2 )
			continue;

		point3.SetPoint( _shortestPath.getItem(i).getVertexPoint() );
		if( line1.containsex( point3 ) )
		{
			removeIndexList.push_back( prePointIndex );
			prePointIndex = i;
			point2 = point3;
		}
		else
		{
			point1 = point2;
			point2 = point3;
			line1.init( point1,point2 );
			prePointIndex = i;
		}
	}
	
	//remove Redundant point
	int iRemoveNum = 0;
	std::sort( removeIndexList.begin(), removeIndexList.end() );
	for( i= removeIndexList.size()-1; i>=0; i-- )
	{
		vertex = _shortestPath.getItem( removeIndexList[i] );
		if( vertex.getVertexType() == PIPE2PIPE )
		{
			_shortestPath.getVertexList().erase( _shortestPath.getVertexList().begin() + removeIndexList[i] );
			iRemoveNum++;
		}
	}
	
	return iRemoveNum;
}
// check can move in pipe or not( if no pipe, than can not, else can )
bool CPipeGraphMgr::checkCanMoveByPipe( int _iLayer ) const
{
	assert(_iLayer >= 0 && _iLayer < (int)m_allPipeGraph.size());
	return m_allPipeGraph[_iLayer].m_pipeList.size()!=0;
}


