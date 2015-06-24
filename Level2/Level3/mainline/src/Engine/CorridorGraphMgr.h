#pragma once
#include <boost\config.hpp>	
#include <boost\property_map.hpp>
#include <boost\graph\graph_traits.hpp>
#include <boost\graph\adjacency_list.hpp>
#include "Common\ARCVector.h"
#include "InputOnBoard\OnboardCorridor.h"
#include "Common\Pollygon2008.h"
#include "InputOnBoard\OnBoardBaseVerticalDevice.h"
#include "OnBoardCorridorInSim.h"
#include "OnboardAircraftElementInSim.h"
#include "OnboardSeatRowInSim.h"

class CorridorGraphMgr;
class CPath2008;
class CDoor;
class COnBoardElevator;
class COnBoardEscalator;
class COnBoardStair;
class CSeat;
class OnboardFlightInSim;
class OnboardDoorInSim;
class OnboardStairInSim;
class OnboardElevatorInSim;
class OnboardEscalatorInSim;
class OnboardDeviceInSim;
class OnboardAircraftElementInSim;
class OnboardSeatInSim;
class SingleGraph;
class EntryPointInSim;
class SingleSeatRowGraph;
class SingleCorridorGraph;
//////////////////////////////////////////////////////////////////////////
// boost graph class
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS , 
boost::no_property, boost::property<boost::edge_weight_t, double> > BoostGraph;

typedef boost::graph_traits < BoostGraph >::vertex_descriptor vertex_descriptor;
typedef boost::graph_traits < BoostGraph >::edge_descriptor edge_descriptor;

enum VertexType
{
	Deck_Vertex,
	Corridor_Vertex,
	Escaltor_Vertex,
	Elevator_Vertex,
	Stair_Vertex,
	Door_Vertex,
	Seat_Vertex
};
//////////////////////////////////////////////////////////////////////////////////////////
class CorridorGraphEdge
{
public:
	CorridorGraphEdge()
		:m_nSourceSegment(-1)
		,m_nDestSegment(-1)
		,m_dWeight(0.0)
	{

	}
	~CorridorGraphEdge()
	{

	}
public:
	void SetSourceSegment(int nSegment){m_nSourceSegment = nSegment;}
	int GetSourceSegment()const{return m_nSourceSegment;}

	void SetDestSegment(int nSegment){m_nDestSegment = nSegment;}
	int GetDestsegment()const{return m_nDestSegment;}

	void SetWeight(double dWeight)
	{
		m_dWeight = fabs(dWeight);
	}
	double GetWeight()const{return m_dWeight;}

	bool operator== (const CorridorGraphEdge& edge)const
	{
		if (((m_nSourceSegment == edge.m_nSourceSegment&&m_nDestSegment==edge.m_nDestSegment)||
			(m_nSourceSegment == edge.m_nDestSegment&&m_nDestSegment==edge.m_nSourceSegment))&&
			m_dWeight == edge.m_dWeight)
		{
			return true;
		}
		return false;
	}
private:
	int m_nSourceSegment;
	int m_nDestSegment;
	double m_dWeight;//edge weight
};
typedef std::vector<CorridorGraphEdge> CorridorGraphEdgeList;
///////////////////////////////////////////////////////////////////////////////////////////
class GraphVertex
{
public:
	GraphVertex()
		:m_pElementInSim(NULL)
	{

	}

	virtual ~GraphVertex()
	{

	}

	void SetElement(OnboardAircraftElementInSim* pElementInSim)
	{
		m_pElementInSim = pElementInSim;
	}

	OnboardAircraftElementInSim* GetElementInSim()const
	{
		return m_pElementInSim;
	}

protected:
	OnboardAircraftElementInSim* m_pElementInSim;
};
class CorridorVertex : public GraphVertex
{
public:
	CorridorVertex()
		:m_dDistanceFromStart(0.0)
		,m_pEntryPointInSim(NULL)
	{

	}

	~CorridorVertex()
	{

	}

	bool operator==(const CorridorVertex& vertex)const
	{
		return (m_dDistanceFromStart == vertex.m_dDistanceFromStart && m_pElementInSim == vertex.m_pElementInSim
			&&m_pEntryPointInSim == vertex.m_pEntryPointInSim);
	}

	void SetDistance(double dDistance){m_dDistanceFromStart = dDistance;}
	double GetDistance()const {return m_dDistanceFromStart;}
	EntryPointInSim* GetEntryPointInSim()const{return m_pEntryPointInSim;}
	void SetEntryPointInSim(EntryPointInSim* pEntryPointInSim) {m_pEntryPointInSim = pEntryPointInSim;}

	COnboardCorridor* GetCorridor() {return ((OnboardCorridorInSim*)GetElementInSim())->m_pCorridor;}
private:
	double m_dDistanceFromStart;
	EntryPointInSim* m_pEntryPointInSim;
};
//stair,escalator,elevator, door and seat node
class DeviceGraphVertex : public GraphVertex
{
public:
	DeviceGraphVertex()
		:m_lcation(0.0,0.0,0.0)
	{

	}
	~DeviceGraphVertex()
	{

	}

	bool operator==(const DeviceGraphVertex& vertex)const
	{
		return (m_lcation == vertex.m_lcation && m_pElementInSim == vertex.m_pElementInSim);
	}

	void SetLocation(const ARCVector3& pt){m_lcation = pt;}
	const ARCVector3& GetLocation()const {return m_lcation;}

private:
	ARCVector3 m_lcation;
};


//corridor node
class CorridorGraphVertex
{
public:
	CorridorGraphVertex()
		:m_nVertexType(Deck_Vertex)
	{

	}
	~CorridorGraphVertex()
	{

	}

	bool operator==(const CorridorGraphVertex& vertex)const
	{
		return (m_nVertexType == vertex.m_nVertexType && m_corridorVertex == vertex.m_corridorVertex
			&& m_deviceVertex == vertex.m_deviceVertex && m_doorVertex == vertex.m_doorVertex
			&& m_seatVertex == vertex.m_seatVertex && m_deckVertex == vertex.m_deckVertex);
	}

	//DistanceUnit GetDistInElement()const;
	void SetVertexType(VertexType nVertexType) {m_nVertexType = nVertexType;}
	VertexType GetVertexType()const{return m_nVertexType;}

	void SetCorridorVertex(const CorridorVertex& vertex){m_corridorVertex = vertex;}
	const CorridorVertex& GetCorridorVertex()const {return m_corridorVertex;}

	void SetDeviceVertex(const DeviceGraphVertex& vertex) {m_deviceVertex = vertex;}
	const DeviceGraphVertex& GetDeviceVertex()const {return m_deviceVertex;}

	void SetSeatVertex(const DeviceGraphVertex& vertex) {m_seatVertex = vertex;}
	const DeviceGraphVertex& GetSeatVertex()const {return m_seatVertex;}

	void SetDoorVertex(const DeviceGraphVertex& vertex){m_doorVertex = vertex;}
	const DeviceGraphVertex& GetDoorVertex()const {return m_doorVertex;}

	void SetDeckVertex(const DeviceGraphVertex& vertex){m_deckVertex = vertex;}
	const DeviceGraphVertex& GetDeckVertex()const {return m_deckVertex;}

	OnboardAircraftElementInSim* GetElementInSim()const;

	bool IsDeviceVertex()const;
private:
	VertexType m_nVertexType;
	CorridorVertex m_corridorVertex;
	DeviceGraphVertex m_deviceVertex;//stair,escalator,elevator
	DeviceGraphVertex m_doorVertex;
	DeviceGraphVertex m_seatVertex;
	DeviceGraphVertex m_deckVertex;
};

//////////////////////////////////////////////////////////////////////////////////////////
///seat area/////////////////////////////////////////////////////////////////////////////
class SeatArea
{
public:
	SeatArea(int nDeck);
	~SeatArea();
	
	bool intersectionArea(const CPoint2008& sourcePt,const CPoint2008& destPt);

	void InsertPollygon(const CPollygon2008& pollygon);

	void clear();
private:
	int m_nDeckID;
	POLLYGON2008VECTOR m_rgn;
};

typedef std::vector<CorridorGraphVertex> CorridorGraphVertexList;
typedef std::pair<OnboardDeviceInSim*,ARCVector3> DevicePoint;
//////////////////////////////////////////////////////////////////////////////////////////
class CorridorGraph
{
public:
	CorridorGraph(CorridorGraphMgr* pCorridorGraphMgr,int nDeck);
	~CorridorGraph();

	bool BuildCorridorGraph();

	bool InitIntersectionWithCorridor(bool bCheckArea,const ARCVector3& destPt,CorridorGraphVertex& vertexS,int& nCorridorSeg,double& dLowDistance);
	bool InitSeatIntersectionWithCorridor(const ARCVector3& destPt,CorridorGraphVertex& vertexS,double& dLowDistance);
	bool InitArea();
	bool InitEntryPoint(SingleSeatRowGraph* pSingleSeatRowGraph);

	void clearData();

	void SetID(int nID){m_nID = nID;}
	int GetID()const{return m_nID;}

	std::vector<SingleGraph*> m_vSingleGraph;

	CorridorGraphMgr* GetGraphMgr()const {return m_pCorridorGraphMgr;}

protected:
	double BuildEntryPoint(const ARCVector3& sourcePt,const ARCVector3& vDir,double& dDist,int& nSegment,SingleSeatRowGraph* pSingleSeatRowGraph,SingleCorridorGraph* pSingleCorridorGraph);
private:
	int m_nID;//reference deck id
	CorridorGraphMgr* m_pCorridorGraphMgr;
	SeatArea m_areaList;
};

/////////////////////////////////////////////////////////////////////////////////////////
class OnboardFlightInSim;
class CorridorGraphMgr
{
public:
	CorridorGraphMgr();
	~CorridorGraphMgr();

	// initial the corridor graph system
	bool initGraphMgr(OnboardFlightInSim* pOnboardFightInSim);

	// clear the corridor graph system
	bool clearGraphMgr();

	// get shortest path
	bool getShortestPath(const ARCVector3& sourcePoint, const ARCVector3& destPoint, CorridorGraphVertexList& _shortestPath);
	bool getShortestPath(const OnboardAircraftElementInSim* pEntryElementInSim,const ARCVector3& entryPos,const OnboardAircraftElementInSim* pExitElementInSim,const ARCVector3& exitPos,CorridorGraphVertexList& _shortestPath);

	bool insertVertex(const CorridorGraphVertex& _item, int& _index);
	bool insertEdge(const CorridorGraphEdge& edge);

	double GetWeight(const CorridorGraphVertex& vertexS,const CorridorGraphVertex& vertexD)const;
	void insertVertexAndEdge( CorridorGraphVertexList& _vertexList );
	VertexType GetVertexTypeByDevice(OnBoardDevieType deviceType)const;

	OnboardFlightInSim *GetFlight();
private:
	void createBoostGraph();
	CorridorGraph* GetCorridorGraph(int nDeck);
	bool BulidCorridorGraph();

	void InitCorridorGraph(OnboardFlightInSim* pOnboardFightInSim);
	void InitDoorGraph(OnboardFlightInSim* pOnboardFightInSim);
	void InitSeatGraph(OnboardFlightInSim* pOnboardFightInSim);
	void InitSeatRowGraph(OnboardFlightInSim* pOnboardFlightInSim);
	void InitStairGraph(OnboardFlightInSim* pOnboardFightInSim);
	void InitElevatorGraph(OnboardFlightInSim* pOnboardFightInSim);
	void InitEscalatorGraph(OnboardFlightInSim* pOnboardFightInSim);

	bool GetEntryExitVertex(const ARCVector3& sourcePoint, const ARCVector3& destPoint, CorridorGraphVertex& entryVertex, CorridorGraphVertex& exitVertex);
	bool GetVertexByLocation(const ARCVector3& sourcePoint,CorridorGraphVertex& vertex);
	bool sExist(const ARCVector3& point);
	bool IsExist(const ARCVector3& point,CorridorGraphVertex& vertex);
	int findVertex(const CorridorGraphVertex& vertex)const;
	const CorridorGraphVertex& GetVertex(int vertexIndex);
	bool GetVertexByElementInSim(const OnboardAircraftElementInSim* pElementInSim,CorridorGraphVertex& vertex);
	void FindShortestPathInGraph(CorridorGraphVertexList& shortestPath, const CorridorGraphVertex& sourceVertex, const CorridorGraphVertex& destVertex);
protected:
	std::vector<CorridorGraph*> m_allCorridorGraph;
	//boost graph for every deck
	BoostGraph* m_pboostGraph;
	std::vector<vertex_descriptor> m_connectedComponents;

	OnboardFlightInSim *m_pOnBoardFlight;
public:
	CorridorGraphVertexList m_vertexList;
	CorridorGraphEdgeList m_edgeList;
};

///////////////////////////////////////////////////////////////////////////////////////////
//@ is the intersection or user defines
//@--------@--------@-----------------@ L-Path
//  @---------@---------@				C-Path
// @----@-------@----------@			R-Path

//make left path,right path and center path set up the graph
//L-Path does not set up graph(order by path)
//C-path set up graph(order by path)
//R-path does not set up graph(order by path)
//L/C connected with shortest point
//R/C connected with shortest point

/*
	left and center make graph(order by the shortest distance)
	right and center make graph(order by the shortest distance)
	left and right path can not make connected graph
*/
///////////////////////////////////////////////////////////////////////////////////////////
class SingleGraph
{
public:
	SingleGraph(CorridorGraph* pCorridorGraph);
	virtual ~SingleGraph();

public:
	virtual bool BuildSingleGraph() = 0;
	virtual bool PutAreaInto(SeatArea& areaList){return true;};
	virtual bool IsCorridor()const {return false;}

protected:
	CorridorGraphMgr* GetGraphMgr()const {return m_pCorridorGraph->GetGraphMgr();}
protected:
	CorridorGraph* m_pCorridorGraph;
};
///////////////////////////////////////////////////////////////////////////////////////////
class SingleCorridorGraph : public SingleGraph
{
public:
	SingleCorridorGraph(CorridorGraph* pCorridorGraph,OnboardCorridorInSim* pCorridorInSim);
	~SingleCorridorGraph();
	
	virtual bool BuildSingleGraph();
	virtual bool IsCorridor()const {return true;}

	OnboardCorridorInSim* GetCorridorInSim()const {return m_pCorridorInSim;}
protected:
	bool BuildSingleCorridorGraph();
	void ConnectIntersectionNode(CorridorPointList& _pointList);
	void sortPointByFromStartDistance(CorridorPointList& _pointList, CorridorGraphVertexList& _vectorList);
private:
	OnboardCorridorInSim* m_pCorridorInSim;
};

/////////////////////////////////////////////////////////////////////////////////////////
class SingleDoorGraph : public SingleGraph
{
public:
	SingleDoorGraph(CorridorGraph* pCorridorGraph,OnboardDoorInSim* pDoorInSim);
	~SingleDoorGraph();

	virtual bool BuildSingleGraph();
protected:
	bool BuildSingleDoorGraph();
private:
	OnboardDoorInSim* m_pDoorInSim;
};

////////////////////////////////////////////////////////////////////////////////////////
class SingleSeatRowGraph : public SingleGraph
{
public:
	SingleSeatRowGraph(CorridorGraph* pCorridorGraph,OnboardSeatRowInSim* pSeatRowInSim);
	~SingleSeatRowGraph();

	virtual bool BuildSingleGraph();

	int GetSeatCountInRow()const {return m_pSeatRowInSim->GetCount();}
	OnboardSeatInSim* GetSeatInSim(int idx) {return m_pSeatRowInSim->GetSeatInSim(idx);}
	OnboardSeatInSim* front(){return m_pSeatRowInSim->front();}
	OnboardSeatInSim* back(){return m_pSeatRowInSim->back();}

	void AddEntryPoint(EntryPointInSim* pEntryInSim){m_pSeatRowInSim->AddEntryPoint(pEntryInSim);}
	bool empty()const {return m_pSeatRowInSim->empty();}
protected:
	bool BuildSingleSeatRowGraph();
	bool InitEntryPoint();
	void SetLeftAndRightEntryPoint();
	EntryPointInSim* GetSeatEntryPoint(OnboardSeatInSim* pSeatInSim,ARCVector3&vDir,double& dDist,int& nSegment);
private:
	OnboardSeatRowInSim* m_pSeatRowInSim;
};
/////////////////////////////////////////////////////////////////////////////////////////
class SingleSeatGraph : public SingleGraph //seat does not assign row
{
public:
	SingleSeatGraph(CorridorGraph* pCorridorGraph,OnboardSeatInSim* pSeatInSim);
	~SingleSeatGraph();

	virtual bool BuildSingleGraph();
	virtual bool PutAreaInto(SeatArea& areaList);
protected:
	bool BuildSingleSeatGraph();
private:
	OnboardSeatInSim* m_pSeatInSim;
};

/////////////////////////////////////////////////////////////////////////////////////////
class SingleDeviceGraph : public SingleGraph
{
public:
	SingleDeviceGraph(CorridorGraph* pCorridorGraph,OnboardDeviceInSim* pDeviceInSim,const ARCVector3& location);
	~SingleDeviceGraph();

	virtual bool BuildSingleGraph();
protected:
	bool BuildSingleDeviceGraph();
private:
	OnboardDeviceInSim* m_pDeviceInSim;
	ARCVector3 m_location;
};