//-------------------------------------------------------------------------------------------
//				Class:			CLandsideWalkwaySystem
//				Author:			sky.wen
//				Date:			Jun 28,2011
//				Purpose:		construct graph system and provide shortest path algorithm lib,
//								passenger walk logic on graph system
//--------------------------------------------------------------------------------------------
#pragma once
#include "..\Common\Point2008.h"
#include "..\Common\Path2008.h"
#include "LandsideObjectInSim.h"
#include <boost\config.hpp>	
#include <boost\property_map.hpp>
#include <boost\graph\graph_traits.hpp>
#include <boost\graph\adjacency_list.hpp>
#include "Common\elaptime.h"
#include "MovingTrace.h"

class LandsideResourceManager;
class LandsideResourceInSim;
class PaxLandsideBehavior;
class CCrossWalkInSim;

//////////////////////////////////////////////////////////////////////////
// boost graph class
typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS , 
boost::no_property, boost::property<boost::edge_weight_t, double> > BoostGraph;

typedef boost::graph_traits < BoostGraph >::vertex_descriptor vertex_descriptor;
typedef boost::graph_traits < BoostGraph >::edge_descriptor edge_descriptor;
//-------------------------------------------------------------------------------
//Summary:
//		vertex of landside graph
//-------------------------------------------------------------------------------
class LandsideTrafficGraphVertex
{
public:
	LandsideTrafficGraphVertex()
		:m_dDistanceFromStart(0.0)
		,m_pTrafficObjectInSim(NULL)
	{

	}

	inline bool operator == (const LandsideTrafficGraphVertex& vertex)const
	{
		if (m_dDistanceFromStart == vertex.m_dDistanceFromStart && m_pTrafficObjectInSim == vertex.m_pTrafficObjectInSim \
			&& m_Location == vertex.m_Location)
		{
			return true;
		}
		return false;
	}
	//---------------------------------------------------------------------------
	//Summary:
	//		adjust vertex whether on walkway
	//---------------------------------------------------------------------------
	inline int GetTrafficObjectType()const
	{
	//	ASSERT(m_pTrafficObjectInSim);
		if (m_pTrafficObjectInSim)
		{
			return m_pTrafficObjectInSim->GetTrafficType();
		}
		return CLandsideWalkTrafficObjectInSim::Non_Type;
	}

	//---------------------------------------------------------------------------
	//Summary:
	//		set and get distance on the resource point distance
	//---------------------------------------------------------------------------
	inline void SetDistance(double dDist)
	{
		m_dDistanceFromStart = dDist;
	}

	inline double GetDistance()const
	{
		return m_dDistanceFromStart;
	}

	CPoint2008 GetPointDist()const
	{
		ASSERT(m_pTrafficObjectInSim);
		return m_pTrafficObjectInSim->GetCenterPath().GetDistPoint(m_dDistanceFromStart);
	}
	//--------------------------------------------------------------------------

	//--------------------------------------------------------------------------
	//Summary:
	//		set and get vertex on which resource
	//--------------------------------------------------------------------------
	inline void SetTrafficObjectInSim(CLandsideWalkTrafficObjectInSim* pTrafficeObjectInSim)
	{
		ASSERT(pTrafficeObjectInSim);
		m_pTrafficObjectInSim = pTrafficeObjectInSim;
	}
	
	inline CLandsideWalkTrafficObjectInSim* GetTrafficInSim()const
	{
		return m_pTrafficObjectInSim;
	}

	//--------------------------------------------------------------------------
	//Summary:
	//		set and get not no walkway and crosswalk
	//-------------------------------------------------------------------------
	inline void SetPoint(const CPoint2008& locationPos)
	{
		m_Location = locationPos;
	}

	CPoint2008 GetPoint()const
	{
		if (m_pTrafficObjectInSim)
		{
			return GetPointDist();
		}
		return m_Location;
	}
private:
	double m_dDistanceFromStart;						//point on the resource
	CLandsideWalkTrafficObjectInSim* m_pTrafficObjectInSim;	//vertex on traffic object
	CPoint2008	m_Location;// does not on the walkway and crosswalk
};

typedef std::vector<LandsideTrafficGraphVertex> LandsideTrafficGraphVertexList;

//----------------------------------------------------------------------------
//Summary:
//		edge of graph
//----------------------------------------------------------------------------
class LandsideTrafficGraphEdge
{
public:
	LandsideTrafficGraphEdge()
	{

	}

	//-----------------------------------------------------------------------
	//Summary:
	//		set and get source index
	//-----------------------------------------------------------------------
	inline void SetSourceSegment(int nSegment){m_nSourceSegment = nSegment;}
	inline int GetSourceSegment()const{return m_nSourceSegment;}

	//-----------------------------------------------------------------------
	//Summary:
	//		set and get dest index
	//-----------------------------------------------------------------------
	inline void SetDestSegment(int nSegment){m_nDestSegment = nSegment;}
	inline int GetDestsegment()const{return m_nDestSegment;}

	//-----------------------------------------------------------------------
	//Summary:
	//		set and get weight of edge 
	//-----------------------------------------------------------------------
	inline void SetWeight(double dWeight)
	{
		m_dWeight = fabs(dWeight);
	}
	inline double GetWeight()const{return m_dWeight;}

	//----------------------------------------------------------------------
	//Summary:
	//		overwrite equal "==" operation
	//----------------------------------------------------------------------
	inline bool operator== (const LandsideTrafficGraphEdge& edge)const
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
	int m_nSourceSegment;	//index of source in graph 
	int m_nDestSegment;		//index of dest in graph
	double m_dWeight;		//edge weight
};

typedef std::vector<LandsideTrafficGraphEdge> LandsideTrafficGraphEdgeList;

//-----------------------------------------------------------------------------
//Summary:
//		construct graph and retrieve shortest path
//-----------------------------------------------------------------------------
class CLandsideGraph
{
public:
	CLandsideGraph();
	~CLandsideGraph();
	//------------------------------------------------------------------------
	//Summary:
	//		initial the walkway graph system
	//------------------------------------------------------------------------
	bool initGraphMgr(LandsideResourceManager* pResManager);

	//------------------------------------------------------------------------
	//Summary:
	//		clear the walkway graph system
	//------------------------------------------------------------------------
	bool clearGraphMgr();

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
	bool getShortestPath(const CPoint2008& entryPos,LandsideResourceInSim* pStartLandsideResource,CLandsideWalkTrafficObjectInSim* pStartResource,const CPoint2008& exitPos,LandsideResourceInSim* pEndLandsideResource,CLandsideWalkTrafficObjectInSim* pEndResource,LandsideTrafficGraphVertexList& _shortestPath);
	bool getShortestPath(const CPoint2008& entryPos,CLandsideWalkTrafficObjectInSim* pStartResource,const CPoint2008& exitPos,CLandsideWalkTrafficObjectInSim* pEndResource,LandsideTrafficGraphVertexList& _shortestPath);
private:
	//----------------------------------------------------------------------------
	//Summary:
	//		find shortest path from start vertex and to end vertex
	//Parameter:
	//		sourceVertex[in]: start vertex
	//		destVertex[in]: end vertex
	//		_shortestPath[out]: passed shortest path
	//----------------------------------------------------------------------------
	void findShortestPath(const LandsideTrafficGraphVertex& sourceVertex, const LandsideTrafficGraphVertex& destVertex,LandsideTrafficGraphVertexList& _shortestPath);
	//---------------------------------------------------------------------------
	//Summary:
	//		set up graph using walkway and crosswalk
	//---------------------------------------------------------------------------
	void BuildBoostGraph(LandsideResourceManager* pResManager);
	//---------------------------------------------------------------------------
	//Summary:
	//		using boost to create graph
	//---------------------------------------------------------------------------
	void CreateBoostGraph();
	//---------------------------------------------------------------------------
	//Summary:
	//		traffic object to intersected with other traffic object
	//---------------------------------------------------------------------------
	void BuildSingleTrafficObjectGrpah(CLandsideWalkTrafficObjectInSim* pSingleTraffictObjectInSim);
	//---------------------------------------------------------------------------
	//Summary:
	//		connect intersection node and set up graph
	//Parameters:
	//		pSingleTraffictObjectInSim[in]: connect traffic object intersect node
	//		_pointList: intersection nodes
	//--------------------------------------------------------------------------
	void ConnectIntersectionNode(CLandsideWalkTrafficObjectInSim* pSingleTraffictObjectInSim,TrafficeNodeList& _pointList);
	//--------------------------------------------------------------------------
	//Summary:
	//		convert intersection node to graph vertex and sort it
	//Parameters:
	//		pSingleTraffictObjectInSim[in]: convert traffic object
	//		_pointList[in]: need to convert
	//		_vectorList[out]: passed result vertex
	//-------------------------------------------------------------------------
	void sortPointByFromStartDistance(CLandsideWalkTrafficObjectInSim* pSingleTraffictObjectInSim, TrafficeNodeList& _pointList, LandsideTrafficGraphVertexList& _vectorList );
	//--------------------------------------------------------------------------
	//Summary:
	//		insert vertex and relative edge
	//Parameter:
	//		_vertexList[in]: vertex list and create edge
	//------------------------------------------------------------------------
	void insertVertexAndEdge( LandsideTrafficGraphVertexList& _vertexList );
	//----------------------------------------------------------------------
	//Summary:
	//		insert vertex to graph
	//Parameters:
	//		_item[in]: inserted item
	//		_index[out]: index of insert item
	//Return:
	//		whether insert success
	//-----------------------------------------------------------------------
	bool insertVertex(const LandsideTrafficGraphVertex& _item, int& _index);
	//------------------------------------------------------------------------
	//Summary:
	//		insert edge to graph
	//-----------------------------------------------------------------------
	bool insertEdge(const LandsideTrafficGraphEdge& edge);
	//-----------------------------------------------------------------------
	//Summary:
	//		find the index of graph vertex
	//-----------------------------------------------------------------------
	int findVertex(const LandsideTrafficGraphVertex& vertex)const;
	//-----------------------------------------------------------------------
	//Summary:
	//		retrieve vertex by index
	//-----------------------------------------------------------------------
	const LandsideTrafficGraphVertex& GetVertex(int vertexIndex)const;
	//------------------------------------------------------------------------
	//Summary:
	//		get distance from two vertex
	//------------------------------------------------------------------------
	double GetWeight(const LandsideTrafficGraphVertex& vertexS,const LandsideTrafficGraphVertex& vertexD)const;
	//-----------------------------------------------------------------------
	//Summary:
	//		get vertex list total length
	//-----------------------------------------------------------------------
	double GetLength(const LandsideTrafficGraphVertexList& path)const;
private:
	BoostGraph*							m_pboostGraph;			//boost graph system
	std::vector<vertex_descriptor>		m_connectedComponents;	//component
	LandsideTrafficGraphVertexList		m_vertexList;			//vertex list
	LandsideTrafficGraphEdgeList		m_edgeList;				//edge list
};

//----------------------------------------------------------------------------------
class CLandsideTrafficSystem;
class CFreeMovingLogic
{
public:
	CFreeMovingLogic(CLandsideTrafficSystem* pLandsideTrafficSys );
	~CFreeMovingLogic();

	//------------------------------------------------------------------------------
	//Summary:
	//		Initialize free moving logic
	//Parameters:
	//		routePath[in]: moving path
	//		pLandsideBehavior[in]: which person behavior
	//		endState[in]: leave traffic system will generate state
	//-----------------------------------------------------------------------------
	void Initialize(const LandsideTrafficGraphVertexList& routePath,PaxLandsideBehavior* pLandsideBehavior,int endState);
	//-----------------------------------------------------------------------------
	//Summary:
	//		person move process in traffic system
	//-----------------------------------------------------------------------------
	void ProcessMove(const ElapsedTime& time);
	//----------------------------------------------------------------------------
	//Summary:
	//		step on conflict crosswalk
	//----------------------------------------------------------------------------
	void StepConflictEvent(ElapsedTime& time);

	//---------------------------------------------------------------------------
	//Summary:
	//		move step for vertex or point to next vertex or point
	//---------------------------------------------------------------------------
	void Step(ElapsedTime& time,const LandsideTrafficGraphVertex& vertex);
	void Step(ElapsedTime& time, const CPoint2008& pt);
	void Move();
	//--------------------------------------------------------------------------

	//---------------------------------------------------------------------------
	//Summary:
	//		leave crosswalk and release the crosswalk
	//--------------------------------------------------------------------------
	void ReleaseCrossWalk(CCrossWalkInSim* pCrossWalkInSim, const ElapsedTime& time );

	//-------------------------------------------------------------------------
	//Summary:
	//		retrieve person behavior of free moving logic
	//-------------------------------------------------------------------------
	PaxLandsideBehavior* GetBehavior()const;


	//next steps ,for conflict resolve	
	MovingTrace m_nextSteps;   //the future steps
	MovingTrace m_pastSteps;   //the past steps

	//move by time intervals
	void ProcessMove2(const ElapsedTime& time);
	void StartMove2(const CPoint2008& curPos,const ElapsedTime& curTime, bool bConerPt);
	
	bool m_bWaitng;

private:
	//-----------------------------------------------------------------------
	//Summary:
	//		move to next point and write log
	//-----------------------------------------------------------------------
	void MoveTo(ElapsedTime& eTime,const CPoint2008& pos );
	//----------------------------------------------------------------------
	//Summary:
	//		generate conflict event to hand conflict
	//----------------------------------------------------------------------
	void GenerateConflictEvent(const ElapsedTime& time);
	void UpdateNextSteps(const MobileState& ptInfo);

private:
	CLandsideTrafficSystem*			m_pLandsideTrafficSys;//landside traffic system
	PaxLandsideBehavior*			m_pPaxLandsideBehavior;//person behavior
	LandsideTrafficGraphVertexList	m_routePath;		//move on path
	int								m_emEndState;		//state of leave landside graph
	

};

//-----------------------------------------------------------------------------------
class CLandsideTrafficSystem
{
public:
	CLandsideTrafficSystem(void);
	~CLandsideTrafficSystem(void);

	//------------------------------------------------------------------------
	//Summary:
	//		Initialize landside traffic system
	//------------------------------------------------------------------------
	void Init(LandsideResourceManager* pResManager);
	//------------------------------------------------------------------------
	//Summary:
	//		find shortest path and move on the path
	//Parameters:
	//		time[in]: enter landside traffic system time
	//		nNextState[in]: leave landside traffic generate event state
	//-------------------------------------------------------------------------
	bool EnterTrafficSystem(const ElapsedTime& time,int nNextState,PaxLandsideBehavior* pPerson,const CPoint2008& startPos,LandsideResourceInSim* pStartResource,const CPoint2008& entPos,LandsideResourceInSim* pEndResource  );
	bool EnterTrafficSystem(const ElapsedTime& time,int nNextState,PaxLandsideBehavior* pPerson,const CPoint2008& startPos,const CPoint2008& entPos);
	//-------------------------------------------------------------------------
	//Summary:
	//		leave the landside traffic system and release free moving
	//-----------------------------------------------------------------------
	void LeaveTrafficSystem(PaxLandsideBehavior* pPerson,int emEndState,const ElapsedTime& time);

	//
	//
	//
private:
	//-----------------------------------------------------------------------
	//Summary:
	//		release free moving logic
	//----------------------------------------------------------------------
	void releaseFreeMove(PaxLandsideBehavior* pPerson);

	//---------------------------------------------------------------------
	//Summary:
	//		get best traffic object
	//---------------------------------------------------------------------
	CLandsideWalkTrafficObjectInSim* getBestTrafficObjectInSim(const CPoint2008& pt,LandsideResourceManager* allRes);
private:
	CLandsideGraph					m_landsideGraph; //graph
	std::vector<CFreeMovingLogic*>	m_freeMovingList; //every person moving logic
	LandsideResourceManager*		m_pResManager;	//landside crosswalk and walkway
};