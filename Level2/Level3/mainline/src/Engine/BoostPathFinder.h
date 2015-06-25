#pragma once



#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/dag_shortest_paths.hpp>

#include <common/Point2008.h>
#include <map>
using namespace boost;

typedef adjacency_list<vecS, vecS, directedS, no_property,  property<edge_weight_t, double> > mygraph_t;
typedef property_map<mygraph_t, edge_weight_t>::type WeightMap;
typedef mygraph_t::vertex_descriptor myvertex_descriptor;
typedef mygraph_t::edge_descriptor myedge_descriptor;
typedef mygraph_t::vertex_iterator vertex_iterator;
typedef std::pair<myvertex_descriptor,double>  out_edge;
typedef std::vector<out_edge> out_edgelist;

typedef std::vector<myvertex_descriptor> PredecessorMap;
typedef std::vector<double> DistanceMap;

class CBoostDirectedGraph
{
public:
	CBoostDirectedGraph(){ mpGraph = NULL;}
	virtual ~CBoostDirectedGraph(){ Clear(); }
	
	void Clear();
	//once call delete old graph
	virtual void Init(int NVertexSize);

	int VertexCount()const;

	mygraph_t& GetGraph(){ return *mpGraph; }	

	void AddEdge(const myvertex_descriptor& f, const myvertex_descriptor& t, double dWeight);
	double GetEdgeWeight(const myvertex_descriptor& f, const myvertex_descriptor& t)const;
	out_edgelist GetOutEdges(const myvertex_descriptor& f)const;

protected:
	mygraph_t* mpGraph;
};


class CBoostPathFinder
{
public:
	virtual bool FindPath(const myvertex_descriptor& src, const myvertex_descriptor& dest, std::vector<myvertex_descriptor>& retPath, double& dPathWeight )  = 0 ;	

	/*void CleanResult(const myvertex_descriptor& src)
	{
		PredecessorMap& srcPredmap = mVertexPredecssor[src];
		DistanceMap& distMap = mVertexDistance[src];
		srcPredmap.clear();
		distMap.clear();
	}*/

	bool GetPathInPredecssorMap(const myvertex_descriptor& src, const myvertex_descriptor& dest,const PredecessorMap& predMap, std::vector<myvertex_descriptor>& retPath)const;
	double GetRouteDist(const myvertex_descriptor& src, const myvertex_descriptor& dest);

	std::map<myvertex_descriptor, PredecessorMap> mVertexPredecssor;
	std::map<myvertex_descriptor, DistanceMap> mVertexDistance;
	CBoostDirectedGraph mGraph;
};

//////////////////////////////////////////////////////////////////////////

class CBoostDijPathFinder : public CBoostPathFinder
{
public:
	virtual bool FindPath(const myvertex_descriptor& src, const myvertex_descriptor& dest, std::vector<myvertex_descriptor>& retPath, double& dPathWeight );

};

//////////////////////////////////////////////////////////////////////////
class CBoostDagPathFinder : public CBoostPathFinder
{
public:	
	virtual bool FindPath(const myvertex_descriptor& src, const myvertex_descriptor& dest, std::vector<myvertex_descriptor>& retPath, double& dPathWeight );

};
//////////////////////////////////////////////////////////////////////////
class CBellmanPathFinder : public CBoostPathFinder
{
public:
	virtual bool FindPath(const myvertex_descriptor& src, const myvertex_descriptor& dest, std::vector<myvertex_descriptor>& retPath, double& dPathWeight );
};


//////////////////////////////////////////////////////////////////////////

class CBoostAStarPathFinder : public CBoostPathFinder
{
public:
	typedef std::vector<CPoint2008> VertexLocationMap;	

	virtual bool FindPath(const myvertex_descriptor& src, const myvertex_descriptor& dest, std::vector<myvertex_descriptor>& retPath, double& dPathWeight );

	VertexLocationMap& GetVertexLocation(){ return m_location; }

protected:
	typedef std::pair<myvertex_descriptor, myvertex_descriptor> VertexPair;
	std::set< VertexPair > mInValidpair;
	bool IsInValidList(const myvertex_descriptor& src, const myvertex_descriptor& dest)const;
	bool GetPathInLib(const myvertex_descriptor& src, const myvertex_descriptor& dest, std::vector<myvertex_descriptor>& retPath, double& dPathWeight);


	VertexLocationMap m_location;


};
