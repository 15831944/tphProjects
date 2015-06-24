#include "StdAfx.h"
#include "BoostPathFinder.h"
#include <boost/graph/astar_search.hpp>
#include <boost/graph/bellman_ford_shortest_paths.hpp>

//////////////////////////////////////////////////////////////////////////
void CBoostDirectedGraph::AddEdge( const myvertex_descriptor& f, const myvertex_descriptor& t, double dWeight )
{
	if(mpGraph)
	{
		add_edge(f, t, dWeight, *mpGraph);
	}
}



double CBoostDirectedGraph::GetEdgeWeight( const myvertex_descriptor& f, const myvertex_descriptor& t ) const
{
	if(mpGraph)
	{
		property_map<mygraph_t, edge_weight_t>::type weightmap = get(edge_weight,*mpGraph);

		graph_traits < mygraph_t >::out_edge_iterator ei, ei_end;
		for( tie(ei,ei_end) = out_edges(f, *mpGraph); ei!=ei_end; ++ei )
		{
			return get(weightmap, *ei);
		}
	}
	return (std::numeric_limits<double>::max)();
}

out_edgelist CBoostDirectedGraph::GetOutEdges( const myvertex_descriptor& f ) const
{
	out_edgelist ret;
	if(mpGraph)
	{
		property_map<mygraph_t, edge_weight_t>::type weightmap = get(edge_weight,*mpGraph);

		graph_traits < mygraph_t >::out_edge_iterator ei, ei_end;
		for( tie(ei,ei_end) = out_edges(f, *mpGraph); ei!=ei_end; ++ei )
		{
			out_edge oute;
			oute.first = target(*ei, *mpGraph);
			oute.second = get(weightmap, *ei);
			ret.push_back(oute);
		}
	}
	return ret;	
}


void CBoostDirectedGraph::Init( int NVertexSize )
{
	Clear();
	mpGraph = new mygraph_t(NVertexSize);
}

void CBoostDirectedGraph::Clear()
{
	delete mpGraph;
	mpGraph = NULL;
}

int CBoostDirectedGraph::VertexCount() const
{
	if(mpGraph)
		return num_vertices(*mpGraph);
	return 0;
}

bool CBoostPathFinder::GetPathInPredecssorMap(const myvertex_descriptor& src, const myvertex_descriptor& dest,const PredecessorMap& predMap, std::vector<myvertex_descriptor>& retPath)const
{
	if( mGraph.VertexCount() != predMap.size())
		return false;

	std::vector<myvertex_descriptor> ret;
	for(myvertex_descriptor v = dest;; v = predMap[v]) 
	{
		ret.push_back(v);
		if(predMap[v] == v )
		{
			if(v==src)
			{
				retPath.resize(ret.size());
				std::reverse_copy(ret.begin(),ret.end(),retPath.begin());
				return true;
			}
			else
				return false;
		}			
	}
}

double CBoostPathFinder::GetRouteDist( const myvertex_descriptor& src, const myvertex_descriptor& dest ) 
{
	DistanceMap& srcDistmap = mVertexDistance[src];
	if(srcDistmap.size() == mGraph.VertexCount() )
        return srcDistmap[dest];
	
	return (std::numeric_limits<int>::max)();
}
//////////////////////////////////////////////////////////////////////////
//dij path finder
//////////////////////////////////////////////////////////////////////////
bool CBoostDijPathFinder::FindPath( const myvertex_descriptor& src, const myvertex_descriptor& dest, std::vector<myvertex_descriptor>& retPath, double& dPathWeight )
{
	PredecessorMap& srcPredmap = mVertexPredecssor[src];
	DistanceMap& distMap = mVertexDistance[src];

	if( srcPredmap.size()!= mGraph.VertexCount() ) //if not find once
	{
		srcPredmap.resize(mGraph.VertexCount());
		distMap.resize(mGraph.VertexCount());
		dijkstra_shortest_paths(mGraph.GetGraph(), src, predecessor_map(&srcPredmap[0]).distance_map(&distMap[0]));
	}
	dPathWeight = distMap[dest];
	return GetPathInPredecssorMap(src, dest, srcPredmap, retPath);
}
//////////////////////////////////////////////////////////////////////////
//dag path finder
//////////////////////////////////////////////////////////////////////////
bool CBoostDagPathFinder::FindPath( const myvertex_descriptor& src, const myvertex_descriptor& dest, std::vector<myvertex_descriptor>& retPath, double& dPathWeight )
{
	PredecessorMap& srcPredmap = mVertexPredecssor[src];
	DistanceMap& distMap = mVertexDistance[src];

	if( srcPredmap.size()!= mGraph.VertexCount() ) //if not find once
	{
		srcPredmap.resize(mGraph.VertexCount());
		distMap.resize(mGraph.VertexCount());
		dag_shortest_paths(mGraph.GetGraph(), src, predecessor_map(&srcPredmap[0]).distance_map(&distMap[0]));
	}
	dPathWeight = distMap[dest];
	//get result
	return GetPathInPredecssorMap(src,dest,srcPredmap, retPath);	
}
//////////////////////////////////////////////////////////////////////////

bool CBellmanPathFinder::FindPath( const myvertex_descriptor& src, const myvertex_descriptor& dest, std::vector<myvertex_descriptor>& retPath, double& dPathWeight )
{
	PredecessorMap& srcPredmap = mVertexPredecssor[src];
	DistanceMap& distMap = mVertexDistance[src];
	if( srcPredmap.size()!= mGraph.VertexCount() ) //if not find once
	{
		srcPredmap.resize(mGraph.VertexCount());
		for(int i=0;i<mGraph.VertexCount();i++)
			srcPredmap[i] = i;
		distMap.clear();
		distMap.resize(mGraph.VertexCount(), (std::numeric_limits < double >::max)() );
		bellman_ford_shortest_paths(mGraph.GetGraph(), src, predecessor_map(&srcPredmap[0]).distance_map(&distMap[0]) );
	}
	dPathWeight = distMap[dest];
	return GetPathInPredecssorMap(src,dest, srcPredmap, retPath );
}

//////////////////////////////////////////////////////////////////////////
//A star finder
//////////////////////////////////////////////////////////////////////////
struct found_goal {}; // exception for termination

// visitor that terminates when we find the goal
template <class Vertex>
class astar_goal_visitor : public default_astar_visitor
{
public:
	astar_goal_visitor(Vertex goal) : m_goal(goal) {}
	template <class Graph>
		void examine_vertex(Vertex u, Graph& g) {
			if(u == m_goal)
				throw found_goal();
		}
private:
	Vertex m_goal;
};
// euclidean distance heuristic
template <class Graph, class CostType, class LocMap>
class distance_heuristic : public astar_heuristic<Graph, CostType>
{
public:
	typedef typename graph_traits<Graph>::vertex_descriptor Vertex;
	distance_heuristic(LocMap& l, Vertex goal)
		: m_location(l), m_goal(goal) {}
		CostType operator()(Vertex u)
		{
			CostType dx = m_location[m_goal].getX() - m_location[u].getX();
			CostType dy = m_location[m_goal].getY() - m_location[u].getY();
			return ::sqrt(dx *dx   + dy * dy);
		}
private:
	LocMap& m_location;
	Vertex m_goal;
};


bool CBoostAStarPathFinder::FindPath( const myvertex_descriptor& src, const myvertex_descriptor& dest, std::vector<myvertex_descriptor>& retPath, double& dPathWeight )
{
	if(IsInValidList(src, dest))
		return false;

	if( GetPathInLib(src, dest, retPath, dPathWeight) )
	{
		return true;
	}

	//find once
	PredecessorMap& srcPredmap = mVertexPredecssor[src];
	DistanceMap& distMap = mVertexDistance[src];
	srcPredmap.clear();
	distMap.clear();

	if( srcPredmap.size()!= mGraph.VertexCount() ) //if not find once
	{
		srcPredmap.resize(mGraph.VertexCount());
		distMap.resize(mGraph.VertexCount());
		//
		try {
			// call astar named parameter interface
			astar_search
				(mGraph.GetGraph(), src,	distance_heuristic<mygraph_t, double,VertexLocationMap>(m_location, dest),
				predecessor_map(&srcPredmap[0]).distance_map(&distMap[0]).
				visitor(astar_goal_visitor<myvertex_descriptor>(dest)));
		} 
		catch(found_goal)
		{ // found a path to the goal			
			return GetPathInPredecssorMap(src,dest, srcPredmap, retPath);
		}
		mInValidpair.insert( VertexPair(src,dest) );
		return false;
	}
	return false;
}

bool CBoostAStarPathFinder::GetPathInLib( const myvertex_descriptor& src, const myvertex_descriptor& dest, std::vector<myvertex_descriptor>& retPath, double& dPathWeight ) 
{
	PredecessorMap& srcPredmap = mVertexPredecssor[src];
	DistanceMap& distMap = mVertexDistance[src];
	if( srcPredmap.size()!= mGraph.VertexCount() )
		return false;
	dPathWeight = distMap[dest];
	return GetPathInPredecssorMap(src,dest, srcPredmap, retPath) ;	
}

bool CBoostAStarPathFinder::IsInValidList( const myvertex_descriptor& src, const myvertex_descriptor& dest ) const
{
	return mInValidpair.find( VertexPair(src, dest) ) != mInValidpair.end();
}
//////////////////////////////////////////////////////////////////////////
