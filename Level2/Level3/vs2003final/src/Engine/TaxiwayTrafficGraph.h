#pragma once

#include "TaxiwayProc.h"
#include <vector>

class Terminal;

class TaxiwayTrafficGraph
{
public:
	TaxiwayTrafficGraph(void);
	~TaxiwayTrafficGraph(void);

public:

	typedef std::vector<TaxiwayProc*> TaxiwayList;
	typedef std::vector<TaxiwayProc*>::iterator TaxiwayItr;
	typedef std::vector<Point>::iterator PtItr;

	TaxiwayTrafficGraph& operator = (const TaxiwayTrafficGraph rhs_ );

	void Initialize(Terminal* _terminal, const CString& strPrjPath);

	bool AddTaxiwayProc(TaxiwayProc *pProc, bool bInit = false);
	bool DeleteTaxiwayProc(TaxiwayProc *pProc);
	bool UpdateTaxiwayProc(TaxiwayProc *pProc);
	void RebuildGraph(){	return RebuildAll(); }
	
protected:
	
	void ClearGraph();
	void RebuildAll();
	void RefreshTaxiwayProcInfo(TaxiwayProc* pProcSource,bool bInit = false);
	
	static void SortProcIntersections(TaxiwayProc* pProc, std::vector<Point>&vPoints);
	static bool IsServicelocationVertex(TaxiwayProc* pProc, Point &point);
	static bool IsPointOnPath(Path *path_, Point& point);

public:

	typedef std::pair<Point, int> vertex;
	typedef std::pair<int, int> edge;
	typedef std::list<vertex> vexlist;
	typedef std::list<edge> edgelist;
	typedef std::list<vertex>::iterator vexItr;
	typedef std::list<edge>::iterator edgeItr;

protected:
	//add vertex make sure no same vertex
	void addVertex( const vertex & );
	//add edge make sure no same edge;filter out invalid edge
	void addEdge( const edge & newedge);
	//add a path to the graph return the count of intersection point;
	int addPath( const Path * );
	//add a new vertex to the graph and return the id; if this new vertex is in the edge ,then it will split the edge.
	int addVertex( const Point & );
	// add an line to the graph ,if intersect other ,it will generate new vertexs and edges 
	void addEdge( const Point & pFrom , const Point & pTo );
	//find the same point id else return -1;
	int findvertex( const Point&  );
	//get vertex iterator of id;
	vexItr getVertex( int id );
	// in: a line Point from , Point to
	// out: point list of intersection in order
	std::vector<Point> intersectPoints( const Point & pFrom , const Point & pTo );
	//get EdgeList 
	edgelist& getEdgelist(){	return m_vEdges;	}
	//get VertexList
	vexlist& getVexlist(){	 return m_vVertexs;  }

protected:
	vexlist m_vVertexs;
	edgelist m_vEdges;
	TaxiwayList m_ProcList;

	Terminal* m_pTerminal;
	CString m_strPrjPath;

	static const int TaxiwayTrafficGraph::minimize_distance = 50;
};
