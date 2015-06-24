#pragma once

#include "common\point.h"
#include "inputs\pipe.h"
#include <boost\config.hpp>	
#include <boost\property_map.hpp>
#include <boost\graph\graph_traits.hpp>
#include <boost\graph\adjacency_list.hpp>

class InputTerminal;

//////////////////////////////////////////////////////////////////////////
// boost graph class
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS , 
boost::no_property, boost::property<boost::edge_weight_t, double> > BoostGraph;

typedef boost::graph_traits < BoostGraph >::vertex_descriptor vertex_descriptor;
typedef boost::graph_traits < BoostGraph >::edge_descriptor edge_descriptor;


//////////////////////////////////////////////////////////////////////////
// define the vertex type
enum GRAPH_VERTEX_TYPE
{
	NORMALPOINT,
	POINT2PIPE,
	POINTONPIPE,
	PIPE2PIPE
};
//////////////////////////////////////////////////////////////////////////
typedef std::vector<CPipeToPipeXPoint> PIPEINTERSECTIONPOINTLIST;
//////////////////////////////////////////////////////////////////////////
// vertex class
class CGraphVertex
{
private:
	Point vertexPoint;	// we can add anything 
	
public:
	CGraphVertex();
	virtual ~CGraphVertex();
	
	bool operator ==( const CGraphVertex& _vertex );

	const Point& getVertexPoint() const;
	void setVertexpoint( const Point& _point);

	void setPipeIndex( int _index );
	int getPipeIndex( void ) const;

	void setVertexType(GRAPH_VERTEX_TYPE _type);
	GRAPH_VERTEX_TYPE getVertexType() const;

	void setPt2Pipe( const CPointToPipeXPoint& _pt2Pipe );
	const CPointToPipeXPoint& getPt2Pipe( void ) const;

	void setPtOnPipe( const PIPEPOINT& _ptOnPipe );
	const PIPEPOINT& getPtOnPipe( void ) const;

	void setPipe2Pipe( const CPipeToPipeXPoint& _pipe2Pipe );
	const CPipeToPipeXPoint& getPipe2Pipe( void ) const;

private:
	GRAPH_VERTEX_TYPE m_vertexType;
	int m_pipeIndex;
	CPointToPipeXPoint  m_pt2Pipe;
	PIPEPOINT			m_ptOnPipe;
	CPipeToPipeXPoint	m_pipe2Pipe;
};

//////////////////////////////////////////////////////////////////////////
// vertex list class
class CGraphVertexList
{
private:
	std::vector< CGraphVertex > m_vertexList;
	double m_length;

public:
	CGraphVertexList();
	virtual ~CGraphVertexList();

	int getCount() const;
	bool insertItem(const CGraphVertex& _item, int& _index);
	void SetItemPoint(int _index, const Point& _itemPoint);
	void addItem( const CGraphVertex& _item );
	int findItem( const CGraphVertex& _item );
	void removeItem( int _index );
	const CGraphVertex& getItem( int _index );
	void clearData();

	CGraphVertexList& operator = ( const CGraphVertexList& _list );
	void invertList();
	std::vector< CGraphVertex >& getVertexList(void) { return m_vertexList; }

	bool FindPath(CGraphVertexList& pipePath, const CGraphVertex& sourceVertex, const CGraphVertex& destVertex);
	double GetLength();
	void ReCalculateLength();
	void SetLength(double dLength);
};

/************************************************************************/
/*   now the edge have no info. maybe we can make a class for Edge      */
/************************************************************************/
//////////////////////////////////////////////////////////////////////////
// edge class	
typedef std::pair<int, int> CEdge;
//////////////////////////////////////////////////////////////////////////
// edge list class
typedef std::vector<CEdge> CEdgeList;
//////////////////////////////////////////////////////////////////////////
// edge weight class
typedef std::vector<double> CEdgeWeightList;

//////////////////////////////////////////////////////////////////////////
// shortest path class
class CShortestPathInGraph
{
private:
	Point source;			// source processor
	Point destination;	// dest processor
	// now the path only save the location, maybe we can add more info into it
	//Path shortestPath;			// the shortest path
	CGraphVertexList shortestPath;	// the shortest path
	double pathDistance;		// the distance of the path
public:
	CShortestPathInGraph();
	virtual ~CShortestPathInGraph();
	
	// adjust PorcessorID is equal
	// if  source == _soure && destination == _dest return 1;
	// if  sourec == _dest && destination == _source return -1;
	// else return 0;
	int isFit( const Point& _soure, const Point& _dest ) const ;
	
	// get & set
	const Point& getSource() const;
	const Point& getDestination() const;
	void getShortestPath( CGraphVertexList& _path ) const;
	double getPathDistance() const;

	void setSource( const Point& _source );
	void setDestination( const Point& _dest );
	void setShortestPath( const CGraphVertexList& _list );
	void setPathDistance( double _distance );
	// clear data
	virtual void clearData();
};


//////////////////////////////////////////////////////////////////////////
// shortest path list class
class CShortestPathList
{
private:
	std::vector< CShortestPathInGraph *> pathList;

public:
	CShortestPathList();
	virtual ~CShortestPathList();
	
	int getCount() const;
	void addItem( const CShortestPathInGraph& _path );
	void clearData();

	bool isExist( const Point& _source, const Point& _dest ) const ;
	bool getShortestPath( const Point& _source, const Point& _dest , CGraphVertexList& _path );
};

//////////////////////////////////////////////////////////////////////////
// pipe graph class
// boost::graph
class CPipeGraph  
{
private:
	CGraphVertexList	m_allVertex;			// vertex list
	CEdgeList			m_allEdge;				// edge list
	CEdgeWeightList		m_allEdgeWeight;		// edge weight list
	BoostGraph*			m_pboostGraph;			// boost graph
	InputTerminal*		m_pInTerm;
	std::vector<vertex_descriptor> m_connectedComponents;
	std::map<int, std::vector<int> > m_conntectedPipeList;

public:
	std::vector<int>	m_pipeList;				// pipe index on the same layer

public:
	CPipeGraph(InputTerminal* pInTerm);
	virtual ~CPipeGraph();

	// build the boost graph from pipeList
	int buildBoostGraph();

	// find the shortest path 
	void findShortestPath(const Point& _pointSource, const Point& _pointDest, CGraphVertexList& shortestPath );

	// clear data
	void clearData();

private:
	void sortPointByFromStartDistance( PIPEINTERSECTIONPOINTLIST& _pointList, std::vector<CGraphVertex>& _vectorList ,int _pipeIndex = -1);
	void insertVertexAndEdge( std::vector<CGraphVertex>& _vertexList );
	void createBoostGraph();

	void FindShortestPathInGraph(CGraphVertexList& pipePath, const CGraphVertex& sourceVertex, const CGraphVertex& destVertex);
	bool FindShortestPathFromLib(CGraphVertexList& pipePath, const CGraphVertex& sourceVertex, const CGraphVertex& destVertex);
	bool GetEntryExitVertex(const Point& sourcePoint, const Point& destPoint, CGraphVertex& entryVertex, CGraphVertex& exitVertex);
	void GetEntryVertex(const Point& point, const int compontentIndex, CGraphVertex& vertex);
	int GetPipeConnectedComponentIndex(CPipe* pipe);

	std::vector<CGraphVertexList> m_shortestPathLib;
};

//////////////////////////////////////////////////////////////////////////
// pipe graph manger class

typedef std::pair<int,int> ViaPipeProcessorPair;
typedef std::vector< ViaPipeProcessorPair > ProcessorPairList;

class CPipeGraphMgr
{
public:
	CPipeGraphMgr();
	virtual ~CPipeGraphMgr();

	// initial the pipe graph system
	bool initGraphMgr(InputTerminal* _pInTerm, int _numOfLayer);

	// clear the pipe graph system
	bool clearGraphMgr();

	// get the shortest path from m_allShortestPath
	bool getShortestPathFromLib(const Point& sourcePoint, const Point& destPoint, CGraphVertexList& _shortestPath);
	
	// check can move in pipe or not( if no pipe, than can not, else can )
	bool checkCanMoveByPipe(int _iLayer) const;

private:
	std::vector<CPipeGraph> m_allPipeGraph;				// all graph
	CShortestPathList m_allShortestPath;	// all shortest path

	int removeRedundantPoint( CGraphVertexList& _shortestPath );
};
