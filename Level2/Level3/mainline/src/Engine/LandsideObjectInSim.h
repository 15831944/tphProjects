#pragma once
class CLandsideWalkTrafficObjectInSim;
class CPath2008;
class CPollygon2008;
class CPoint2008;
class LandsideTrafficGraphVertex;
class LandsideResourceInSim;
//----------------------------------------------------------------------------------
//Summary:
//		intersection node with other traffic object resource
//----------------------------------------------------------------------------------
struct trafficIntersectionNode
{
	trafficIntersectionNode()
		:m_dDistanceFromStart(0.0)
		,m_nSegment(-1)
		,m_pTrafficObject(NULL)
		,m_dDistWalkway(0.0)
	{

	}

	double m_dDistanceFromStart;						//distance from start
	int m_nSegment;										//current segment index
	CLandsideWalkTrafficObjectInSim* m_pTrafficObject;	//intersect with traffic object
	double m_dDistWalkway;								//intersect traffic object point distance
};

//-----------------------------------------------------------------------------------
//Summary:
//		use data to calculate traffic object with shortest distance
//-----------------------------------------------------------------------------------
struct ParkingLotIntersectTrafficObject
{
	ParkingLotIntersectTrafficObject()
		:m_pLandsideResInSim(NULL)
		,m_nIdxpolygon(-1)
	{

	}
	LandsideResourceInSim* m_pLandsideResInSim;//intersect landside resource
	int m_nIdxpolygon;							//polygon index
	std::vector<CPath2008> m_OverlapPathList;	//inside of polygon
};
typedef std::vector<ParkingLotIntersectTrafficObject> trafficObjectInfoList;
typedef std::vector<trafficIntersectionNode> TrafficeNodeList;
class CCrossWalkInSim;
class CWalkwayInSim;

class CLandsideWalkTrafficObjectInSim
{
public:
	//--------------------------------------------------------
	//Summary:
	//		traffic type
	//--------------------------------------------------------
	enum TrafficObjectType
	{
		Non_Type,
		Walkway_Type,
		CrossWalk_Type,
	};
	CLandsideWalkTrafficObjectInSim(void);
	virtual ~CLandsideWalkTrafficObjectInSim(void);

	virtual CCrossWalkInSim* toCrossWalk(){ return NULL; }
	virtual CWalkwayInSim* toWalkway(){ return NULL; }

	//-------------------------------------------------------------------
	//retrieve traffic type
	//-------------------------------------------------------------------
	virtual TrafficObjectType GetTrafficType()const = 0;
	
	//-------------------------------------------------------------------
	//Summary:
	//		retrieve center path
	//------------------------------------------------------------------
	virtual CPath2008 GetCenterPath()const = 0;

	//-----------------------------------------------------------------
	//Summary:
	//		retrieve right path
	//----------------------------------------------------------------
	virtual CPath2008 GetRightPath()const = 0;

	//-----------------------------------------------------------------
	//Summary:
	//		retrieve left path
	//-----------------------------------------------------------------
	virtual CPath2008 GetLeftPath()const = 0;

	//------------------------------------------------------------------------------
	//Summary:
	//		calculate intersection node with center path and reflect to center path
	//Parameters:
	//		pWalkwayInSim[in]: intersection walkway
	//-----------------------------------------------------------------------------
	void CalculateIntersectionNode(CLandsideWalkTrafficObjectInSim* pTrafficObjectInSim);

	//----------------------------------------------------------------------------
	//Summary:
	//		check walkway whether intersect with pollygon
	//----------------------------------------------------------------------------
	bool InterSectWithPollygon(const CPollygon2008& pollygon)const;

	//----------------------------------------------------------------------------
	//Summary:
	//		retrieve shortest distance to inside polygon
	//----------------------------------------------------------------------------
	double GetParkinglotPointDist(LandsideResourceInSim* pLandsideResInSim,const CPoint2008& pt)const;
	//----------------------------------------------------------------------------
	//Summary:
	//		Get shortest distance that from current point to walkway
	//Parameters:
	//		pt[in]: point to walkway
	//Return:
	//		shortest distance
	//---------------------------------------------------------------------------
	double GetPointDistance(const CPoint2008& pt)const;

	//---------------------------------------------------------------------------
	//Summary:
	//		get point reflect on center path
	//Parameters:
	//		pt[in]: reflect center path point
	//		resultPt[out]: point relative to pt
	//		dResultDist[out]: get shortest distance
	//Return:
	//		true: successes
	//		false: failed
	//---------------------------------------------------------------------------
	bool GetProjectPoint( const CPoint2008& pt, CPoint2008& resultPt,double& dResultDist)const;

	//--------------------------------------------------------------------------
	//Summary:
	//		retrieve inside polygon with shortest project point
	//--------------------------------------------------------------------------
	bool GetShortestProjectPoint(LandsideResourceInSim* pLandsideResInSim,const CPoint2008& pt, CPoint2008& resultPt)const;
	//---------------------------------------------------------------------------
	//Summary:
	//		get vertex on the point left and right on the center path
	//Parameters:
	//		pt[in]: retrieve the points on the left and right around the point
	//		leftVertex[out]: on left hand
	//		rightVertex[out]: on right hand
	//Return:
	//		true: successes
	//		false: failed
	//---------------------------------------------------------------------------
	bool GetNearestVertex(const CPoint2008& pt,LandsideTrafficGraphVertex& leftVertex,LandsideTrafficGraphVertex& rightVertex);
	bool SameSegment(const CPoint2008& pt1, const CPoint2008& pt2)const;
	//----------------------------------------------------------------------------
	//Summary:
	//		retrieve all the intersection node on the segment
	//Parameters:
	//		nSegment[in]: relative segment
	//		ptList[out]: all intersection node on the segment
	//Return:
	//		count of the intersection nodes
	//-----------------------------------------------------------------------------
	int GetIntersectionOnSegment(int nSegment,TrafficeNodeList& ptList);
	//-----------------------------------------------------------------------------
	//Summary:
	//		create vertex on the traffic node
	//Parameters:
	//		vertex[out]: create vertex that passed
	//		pt[in]: relative point to create vertex
	//Return:
	//		true: successes
	//		false: failed
	//---------------------------------------------------------------------------
	bool GetWalkwayVertex(LandsideTrafficGraphVertex& vertex,const CPoint2008& pt);

	//------------------------------------------------------------------------------
	//Summary:
	//		set traffic object intersect with parking lot
	//-----------------------------------------------------------------------------
	void AddTafficParkinglot(const ParkingLotIntersectTrafficObject& trafficInfo);

	//-----------------------------------------------------------------------------
	//Summary:
	//		retrieve landside level
	//-----------------------------------------------------------------------------
	double GetLevel()const;
private:
	//------------------------------------------------------------------------------
	//Summary:
	//		calculate intersection node with center path and reflect to center path
	//Parameters:
	//		pWalkwayInSim[in]: intersection walkway
	//		vIntersection[out]: intersection nodes;
	//-----------------------------------------------------------------------------
	void BuildIntersectionNode(CLandsideWalkTrafficObjectInSim* pTrafficObjectInSim,TrafficeNodeList& vIntersection);

	//------------------------------------------------------------------------------
	//Summary:
	//		calculate left and right path intersection node reflect to center path
	//Parameters:
	//		pWalkwayInSim[in]: intersect walkway
	//		centerPath[in]:	intersect path with other path
	//		boundPath[in]:	intersect with centerPath
	//		vIntersection[out]: intersection nodes;
	//------------------------------------------------------------------------------
	int IntersectWithBoundPath(CLandsideWalkTrafficObjectInSim* pTrafficObjectInSim,const CPath2008& centerPath,const CPath2008& boundPath,TrafficeNodeList& vIntersection);
	//------------------------------------------------------------------------------
	//Summary:
	//		calculate left and right path intersection node reflect to center path
	//Parameters:
	//		pWalkwayInSim[in]: intersect walkway
	//		centerPath[in]:	intersect path with other path
	//		middlePath[in]:	intersect with centerPath
	//		vIntersection[out]: intersection nodes;
	//------------------------------------------------------------------------------
	int IntersectWithCenterPath(CLandsideWalkTrafficObjectInSim* pTrafficObjectInSim,const CPath2008& centerPath,const CPath2008& middlePath,TrafficeNodeList& vIntersection);

	//-----------------------------------------------------------------------------
	//Summary:
	//		calculate bound path intersection node reflect to center path distance
	//Parameter:
	//		path[in]: reflect path
	//		pt[in]: need convert point
	//return:
	//		distance from start to current point distance on center path
	//-----------------------------------------------------------------------------
	double ReflectIntersectionNodeToCenterPath(const CPath2008& path,const CPoint2008& pt)const;

	//-----------------------------------------------------------------------------
	//Summary:
	//		build intersection node with start and end node then sort it by distance from start
	//-----------------------------------------------------------------------------
	void BuildTrafficNode();

	//-----------------------------------------------------------------------------
	//Summary:
	//		left, right,bottom,top and center path two or more intersected with same resource with same segment
	//		such as: L,R,C,T,B with same segment nSeg intersected with other resource with same segment
	//-----------------------------------------------------------------------------
	bool exsitSameSegmentIntersectionNode(int nSegment,CLandsideWalkTrafficObjectInSim* pTrafficObjectInSim,int nOrtherSeg,const TrafficeNodeList& vIntersection)const;

	//---------------------------------------------------------------------------
	//Summary:
	//		retrieve project point
	//---------------------------------------------------------------------------
	bool GetProjectPoint(const CPath2008& path, const CPoint2008& pt, CPoint2008& resultPt,double& dResultDist)const;
public:
	std::vector<TrafficeNodeList>	  m_vTrafficNode;//intersection node
	std::vector<double>				  m_vDistNode; //include intersection node and start and end node
	trafficObjectInfoList			  m_trafficInfo;//traffic intersect with parking lot information
};
