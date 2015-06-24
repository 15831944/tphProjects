#include "StdAfx.h"

#include <iostream>
#include <limits>

#include "taxiwayModel.h"
#include "../../Common/ARCUnit.h"
#include "../../AirsideInput/AirsideInput.h"
#include "../../AirsideInput/TaxiInbound.h"
#include "../../AirsideInput/TaxiOutbound.h"
#include "../../AirsideInput/AMGraph.h"

#include "Flight.h"
#include "Event.h"
#include "AirportModel.h"
#include "GateModel.h"
#include "RunwayModel.h"
#include "AirspaceModel.h"


NAMESPACE_AIRSIDEENGINE_BEGINE

int ShortestTaxiPath::getSrcId()const{
	ASSERT(size()>0);
	return *begin();
}

int ShortestTaxiPath::getDestId()const{
	ASSERT(size()>0);
	return *rbegin();
}
//get the path, if return false ,then not found
bool ShortestTaxiPathList::getPath(int srcId, int destId, ShortestTaxiPath& thepath){
	
	for(int i=0;i<(int)size();i++){
		if( srcId == at(i).getSrcId() && destId == at(i).getDestId() ){
			 thepath = at(i);
			 return true;
		}
	}
	return false;
}


TaxiwayModel::~TaxiwayModel(){
	delete m_pboostGraph;
}


//find the shortest path by boost graph
bool TaxiwayModel::findShortestPath(int nodeFrom ,int nodeTo, ShortestTaxiPath& shortestpath)
{
	using namespace boost;

	if (!m_pboostGraph)
		return false;

	ASSERT(nodeTo != nodeFrom);

	int nVerticesCount = num_vertices(*m_pboostGraph);
	std::vector<vertex_descriptor> parentsVertex(nVerticesCount);
	std::vector<double> distance(nVerticesCount, 0.0);
	vertex_descriptor beginVertex = vertex(nodeFrom, *m_pboostGraph);
	vertex_descriptor endVertex = vertex(nodeTo, *m_pboostGraph);

	dijkstra_shortest_paths(*m_pboostGraph, beginVertex, predecessor_map(&parentsVertex[0]).distance_map(&distance[0]));
	
	//bool pathexist = false;
	//std::vector<int> reverpath;

	//double endDistance = distance[endVertex];
	//if (endDistance < 0.00000001)
	//	return false;

	//pathexist = true;
	//vertex_descriptor prevex = endVertex;
	//do{
	//	reverpath.push_back(prevex);
	//	vertex_descriptor tmpvex = parentsVertex[prevex];

	//	if(prevex == tmpvex)
	//	{
	//		pathexist = false;
	//		break;
	//	}
	//	prevex = tmpvex;
	//}while(prevex != beginVertex );

	//reverpath.push_back(beginVertex);

	bool pathexist = false;
	std::vector<int> reverpath;

	if(distance[endVertex] < (std::numeric_limits<double>::max)()){ // path exist
		pathexist = true;
		vertex_descriptor prevex = endVertex;
		do{
			reverpath.push_back(prevex);
			vertex_descriptor tmpvex = parentsVertex[prevex];
			if(prevex == tmpvex) { pathexist = false; break; }
			prevex = tmpvex;
		}while(prevex != beginVertex );
		reverpath.push_back(beginVertex);
	}
	
	if(pathexist)
	{
		shortestpath.resize(reverpath.size());
		std::reverse_copy(reverpath.begin(),reverpath.end(),shortestpath.begin());
		return true;
	}

	return false;
}
//get a shorest path
bool TaxiwayModel::getShorestPath(int nodeSrc, int nodeDest,TaxiwayNodeList& shortestpath ){
		
	bool reslt = false;
	ShortestTaxiPath shortpath;

	if( nodeDest == nodeSrc )
	{
		shortestpath.push_back(getTaixwayNode(nodeSrc));
		shortestpath.push_back(getTaixwayNode(nodeDest));
		return true;
	}

	if(reslt = m_vShortestPathList.getPath(nodeSrc,nodeDest,shortpath))
	{
	}
	else
		reslt = findShortestPath(nodeSrc,nodeDest,shortpath);
	
	if(reslt == true)
	{
		shortestpath.clear();

		for(int i=0;i<(int)shortpath.size();i++)
			shortestpath.push_back(getTaixwayNode(shortpath[i]));
	}

	return reslt;
}

void TaxiwayModel::Build(CAirsideInput* _inputC,AirsideInput* _inputD)
{
	m_pAirsideInput = _inputD;    // add code here 
}

void TaxiwayModel::ConvertFromAMGraph(AirportMachine::AirsideGraph& otherGraph)
{
	typedef AirportMachine::AirsideGraph::vexlist otherGraphVertexList;
	typedef otherGraphVertexList::iterator otherVexIter;	
	typedef AirportMachine::AirsideGraph::edgelist otherGraphEdgeList;
	typedef otherGraphEdgeList::iterator otherEdgeIter;

	otherGraphVertexList & vertexlist = otherGraph.getVexlist();
	otherGraphEdgeList & edgelist  = otherGraph.getEdgelist();
	
	
	for(otherVexIter itr = vertexlist.begin();itr!=vertexlist.end();itr++){
		ref_ptr<TaxiwayNode> pNode = new TaxiwayNode;
		pNode->setPosition(itr->first);
		pNode->m_nodeid = itr->second;
		CString idStr;
		idStr.Format("Node%d",pNode->m_nodeid);
		pNode->setStringID(idStr);
		m_allVertex.push_back(pNode);
	}
	for(otherEdgeIter itr = edgelist.begin();itr!=edgelist.end();itr++){
		ref_ptr<TaxiwayEdge> pEdge = new TaxiwayEdge;
		pEdge->m_nodeId1 = itr->first; pEdge->m_nodeId2 = itr->second;
		m_allEdge.push_back(pEdge);
		TaxiwayNode* pNode1 = getTaixwayNode(pEdge->m_nodeId1);
		TaxiwayNode* pNode2 = getTaixwayNode(pEdge->m_nodeId2);
		double weight = pNode1->getPosition().distance3D(pNode2->getPosition());
		m_vEdgeWeight.push_back(weight);
	}
	//now create the boost graph
	delete m_pboostGraph;
	m_pboostGraph = new BoostGraph(vertexlist.size());
	boost::property_map<BoostGraph,boost::edge_weight_t>::type weightmap = boost::get(boost::edge_weight,*m_pboostGraph);
	for(std::size_t i=0;i<m_allEdge.size();i++)
	{
		edge_descriptor edge;
		bool inserted;
		boost::tie(edge,inserted) = boost::add_edge(m_allEdge[i]->m_nodeId1,m_allEdge[i]->m_nodeId2,*m_pboostGraph);
		weightmap[edge] = m_vEdgeWeight[i];
	}
}

TaxiwayNode* TaxiwayModel::getTaixwayNode(int nodeidx){
	
	if(m_allVertex[nodeidx-1]->m_nodeid == nodeidx) return m_allVertex[nodeidx-1].get();
	for(int i=0;i<(int)m_allVertex.size();i++){
		if(m_allVertex[i]->m_nodeid == nodeidx ) return m_allVertex[i].get();
	}
	return NULL;
}
TaxiwayNode* TaxiwayModel::getTaixwayNode(CString node_strid){
	return m_allVertex.getEntity(node_strid);
}

std::vector<int> TaxiwayModel::getAdjacentVertex(int vexid){
	std::vector<int> reslt ;
	if(m_pboostGraph){
		vertex_descriptor vex = boost::vertex(vexid,*m_pboostGraph);
		boost::graph_traits<BoostGraph>::adjacency_iterator ai,aend;
		for(tie(ai,aend) = adjacent_vertices(vex, *m_pboostGraph);ai!=aend;ai++){
			reslt.push_back(*ai);
		}
	}

	return reslt;
}	

void TaxiwayModel::DistributeTaxiInProcess(SimFlight* _flight)
{
	
	FlightEventList* _FEL = _flight->getFEL();
	
	CString NodeId = _FEL->getLastEvent()->GetEntityID();
	TaxiwayNode * pNode = getTaixwayNode(NodeId);
	if(!pNode)
		return;

	int TaxiInId = pNode->m_nodeid;	
	Gate * pGate = m_pAirport->getGateModel()->getGate(_flight);
	if(!pGate)
		return;

	int GateId = pGate->m_inNodeId;
	TaxiwayNodeList TaxiwayNodes;
	if(!getShorestPath(TaxiInId,GateId,TaxiwayNodes) )
		return;

	double TaxiwaySpeed = getTaxiInSpeed(_flight);     // get Taix speed in m/s
	double taxispd = ARCUnit::ConvertVelocity(TaxiwaySpeed,ARCUnit::KNOT,ARCUnit::CMpS);	
	SimClock::TimeType evalTime = _flight->getFEL()->getLastEvent()->GetComputeTime();
	
	for (int i=1;i<int(TaxiwayNodes.size());i++)
	{
		FlightEvent *_event = new FlightEvent(_flight);
		Point OutPos = (TaxiwayNodes.at(i))->getPosition();
		double dS = _FEL->getLastEvent()->GetEventPos().distance(OutPos) ;
		
		evalTime += (SimClock::TimeSpanType)(dS / taxispd* 100);
		_event->SetEventType(FlightEvent::TaxiIn);
		_event->SetComputeTime(evalTime);
		_event->setEventTime(evalTime);
		_event->SetFlightHorizontalSpeed(TaxiwaySpeed);
		_event->SetFlightVerticalSpeed(0);
		_event->SetEntityID((TaxiwayNodes.at(i))->m_strID);
		_event->SetEventPos(OutPos);				
		_FEL->push_back(_event);
	}
}

void TaxiwayModel::DistributeTaxiOutProcess(SimFlight* _flight)
{
	FlightEvent *_event = new FlightEvent(_flight);
	FlightEventList* _FEL = _flight->getFEL();
	FlightEvent * preEvent = _FEL->getLastEvent();

	CString NodeId = _FEL->getLastEvent()->GetEntityID();
	TaxiwayNode * pTaxiway = getTaixwayNode(NodeId);
	if(!pTaxiway)return ;

	int OutId =pTaxiway->m_nodeid;	
	airroute * pdepartureRoute = m_pAirport->getAirspaceModel()->getDepatureRoute(_flight);
	if(!pdepartureRoute)return ;

	Runway *runway = m_pAirport->getRunwayModel()->getRunway(pdepartureRoute->m_RwID);
	RunwayExit * ExitTaxi =  m_pAirport->getRunwayModel()->getInExit(runway,_flight); 
	if (ExitTaxi == NULL)
		return;

	int TaxiOutId = ExitTaxi->m_NodeId;
	TaxiwayNodeList TaxiwayNodes;
	getShorestPath(OutId,TaxiOutId,TaxiwayNodes);
	double TaxiwaySpeed =  getTaxiOutSpeed(_flight);
	double taxispd = ARCUnit::ConvertVelocity(TaxiwaySpeed,ARCUnit::KNOT,ARCUnit::CMpS);
	
	SimClock::TimeType evalTime = preEvent->GetComputeTime();
	for (int i=1;i<int(TaxiwayNodes.size());i++)
	{
		_event = new FlightEvent(_flight);
		Point SegmentOutPos = (TaxiwayNodes.at(i))->getPosition();
		double dS = _FEL->getLastEvent()->GetEventPos().distance(SegmentOutPos) ;
		
		evalTime += (SimClock::TimeSpanType)(dS / taxispd*100);
		_event->SetEventType(FlightEvent::TaxiOut);
		_event->SetComputeTime(evalTime);
		_event->setEventTime(evalTime);
		_event->SetFlightHorizontalSpeed(TaxiwaySpeed);
		_event->SetEntityID((TaxiwayNodes.at(i))->m_strID);
		_event->SetEventPos(SegmentOutPos);				
		_FEL->push_back(_event);	
	}

}

double TaxiwayModel::getTaxiInSpeed(SimFlight  *_flight)
{
	double defspeed  = 20;
	FlightDescStruct& fltdesc = _flight-> GetCFlight()->getLogEntry();
	int num = m_pAirsideInput->GetTaxiInbound()->GetTaxiInboundNECList()->GetCount();
	CTaxiInboundNEC* pTaxiIn = NULL;
	for (int i =0;i<num;i++)
	{
		pTaxiIn = m_pAirsideInput->GetTaxiInbound()->GetTaxiInboundNECList()->GetRecordByIdx(i);
		FlightConstraint& fltcont = pTaxiIn->GetFltType();
		if(fltcont.fits(fltdesc))
		{
			defspeed = pTaxiIn->GetNormalSpeed();
			break;
		}
	}
	return defspeed;
}

double TaxiwayModel::getTaxiOutSpeed(SimFlight  *_flight)
{
	double defspeed  = 20;
	FlightDescStruct& fltdesc = _flight-> GetCFlight()->getLogEntry();
	int num = m_pAirsideInput->GetTaxiOutbound()->GetCount();
	CTaxiOutboundNEC* pTaxiOut = NULL;
	for (int i =0;i<num;i++)
	{
		pTaxiOut = m_pAirsideInput->GetTaxiOutbound()->GetRecordByIdx(i);
		FlightConstraint& fltcont = pTaxiOut->GetFltType();
		if(fltcont.fits(fltdesc))
		{
			defspeed = pTaxiOut->GetNormalSpeed();
			break;
		}
	}
	return defspeed;
}
NAMESPACE_AIRSIDEENGINE_END