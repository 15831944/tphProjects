#ifndef __AIRSIDE_TAXIWAYMODEL_DEF
#define  __AIRSIDE_TAXIWAYMODEL_DEF
#include <boost\config.hpp>							// for boost libaray
#include <boost\graph\graph_traits.hpp>				// for boost libaray
#include <boost\graph\adjacency_list.hpp>			// for boost libaray
#include <boost\graph\dijkstra_shortest_paths.hpp>	// for boost libaray


#include "../../Common/ARCVector.h"

#include "../EngineDll.h"
#include "../EngineConfig.h"
#include "Entity.h"

#include "../../Common/point.h"

class AirsideInput;
class CAirsideInput;
namespace AirportMachine{
	class AirsideGraph;
}

NAMESPACE_AIRSIDEENGINE_BEGINE

class AirportModel;
class SimFlight;

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, 
							  boost::no_property, boost::property<boost::edge_weight_t, double> > BoostGraph;

typedef boost::graph_traits < BoostGraph >::vertex_descriptor vertex_descriptor;
typedef boost::graph_traits < BoostGraph >::edge_descriptor edge_descriptor;


class ENGINE_TRANSFER TaxiwayNode : public Entity{
public:
	Point m_pos;
	int m_nodeid;
	Point getPosition(){return m_pos;}
	void setPosition(const Point& pos) { m_pos = pos; }
};
typedef EntContainer<TaxiwayNode> TaxiwayNodeList;
typedef TaxiwayNodeList::iterator TaxiwayNodeIter;
typedef TaxiwayNodeList::const_iterator TaxiwayNodeIter_const;

class ENGINE_TRANSFER TaxiwayEdge : public Entity{
public:	
	int m_nodeId1;
	int m_nodeId2;
};
typedef EntContainer<TaxiwayEdge> TaxiwayEdgeList;
typedef TaxiwayEdgeList::iterator TaxiwayEdgeIter;
typedef TaxiwayEdgeList::const_iterator TaxiwayEdgeIter_const;

typedef std::vector<double> TaxiwayEdgeWeightList;



//record the shortest path 
class ENGINE_TRANSFER ShortestTaxiPath : public std::vector<int>{
public:	
	int getSrcId()const;
	int getDestId()const;
	
	std::vector<int> m_path;        //vertex id list of the path	
};

class ENGINE_TRANSFER ShortestTaxiPathList : public std::vector<ShortestTaxiPath> {
public:	
	bool getPath(int srcId, int destId, ShortestTaxiPath& thepath);
};




class ENGINE_TRANSFER TaxiwayModel{
public:
	TaxiwayModel(AirportModel * pAirport){ m_pAirport = pAirport;m_pboostGraph = NULL; }
	~TaxiwayModel();

	AirportModel * getAirport(){ return m_pAirport; }

	bool getShorestPath(int nodeSrc, int nodeDest,TaxiwayNodeList& shortestpath ); //get shortest path

	void Build(CAirsideInput* _inputC,AirsideInput* _inputD);   

	void ConvertFromAMGraph(AirportMachine::AirsideGraph& otherGraph);   // 

	TaxiwayNode* getTaixwayNode(int nodeidx); // get the node from the index
	TaxiwayNode* getTaixwayNode(CString node_strid);

	std::vector<int> getAdjacentVertex(int vexid);  // get the Adjacent Vertex of vexid
	
	double getTaxiInSpeed(SimFlight  *_flight);     // get Taix speed in ms
	double getTaxiOutSpeed(SimFlight  *_flight);

	void DistributeTaxiInProcess(SimFlight* _flight);
	void DistributeTaxiOutProcess(SimFlight* _flight);

	
private:
	
	BoostGraph * m_pboostGraph;	
	TaxiwayNodeList m_allVertex;
	TaxiwayEdgeList m_allEdge;
	TaxiwayEdgeWeightList m_vEdgeWeight;
	
	ShortestTaxiPathList m_vShortestPathList;

	bool findShortestPath(int nodeFrom ,int nodeTo,ShortestTaxiPath& shortestpath); // use the boost library to find the shortest path
	
	AirportModel * m_pAirport;

	AirsideInput  *m_pAirsideInput;
};










NAMESPACE_AIRSIDEENGINE_END

#endif
