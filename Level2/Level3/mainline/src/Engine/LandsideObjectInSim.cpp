#include "StdAfx.h"
#include ".\landsideobjectinsim.h"
#include "..\Common\Path2008.h"
#include "..\Common\Point2008.h"
#include "..\Common\Pollygon2008.h"
#include "..\Common\Line2008.h"
#include "LandsideTrafficSystem.h"

CLandsideWalkTrafficObjectInSim::CLandsideWalkTrafficObjectInSim(void)
{
}

CLandsideWalkTrafficObjectInSim::~CLandsideWalkTrafficObjectInSim(void)
{
}

//------------------------------------------------------------------------------
//Summary:
//		calculate intersection node with center path and reflect to center path
//Parameters:
//		pWalkwayInSim[in]: intersection walkway
//-----------------------------------------------------------------------------
void CLandsideWalkTrafficObjectInSim::CalculateIntersectionNode( CLandsideWalkTrafficObjectInSim* pTrafficObjectInSim )
{
	if (pTrafficObjectInSim == this)
		return;

	TrafficeNodeList IntersectionPointList;
	BuildIntersectionNode(pTrafficObjectInSim,IntersectionPointList);
	m_vTrafficNode.push_back(IntersectionPointList);
}

//----------------------------------------------------------------------------
//Summary:
//		check walkway whether intersect with pollygon
//----------------------------------------------------------------------------
bool CLandsideWalkTrafficObjectInSim::InterSectWithPollygon( const CPollygon2008& pollygon ) const
{
	//check left path
// 	const CPath2008& leftPath = GetLeftPath();
// 	CPollygon2008 leftPollygon;
// 	leftPollygon.init(leftPath.getCount(),leftPath.getPointList());
// 	if (pollygon.IfOverlapWithOtherPolygon(leftPollygon))
// 	{
// 		return true;
// 	}
// 
// 	//check right path
// 	const CPath2008& rightPath = GetRightPath();
// 	CPollygon2008 rightPollygon;
// 	rightPollygon.init(leftPath.getCount(),leftPath.getPointList());
// 	if (pollygon.IfOverlapWithOtherPolygon(rightPollygon))
// 	{
// 		return true;
// 	}

	//check center path
	const CPath2008& centerPath = GetCenterPath();
	for (int i = 0; i < centerPath.getCount(); i++)
	{
		const CPoint2008& pt = centerPath.getPoint(i);
		if (pollygon.contains2(pt))
		{
			return true;
		}
	}

	CPollygon2008 centerPollygon;
	centerPollygon.init(centerPath.getCount(),centerPath.getPointList());
	if (pollygon.IfOverlapWithOtherPolygon(centerPollygon))
	{
		return true;
	}

// 	//check top path
// 	CPoint2008 path[3];
// 	path[0] = GetLeftPath().getPoint(0);
// 	path[1] = GetCenterPath().getPoint(0);
// 	path[2] = GetRightPath().getPoint(0);
// 	CPath2008 topPath;
// 	topPath.init(3,path);
// 	CPollygon2008 topPollygon;
// 	topPollygon.init(topPath.getCount(),topPath.getPointList());
// 	if (pollygon.IfOverlapWithOtherPolygon(topPollygon))
// 	{
// 		return true;
// 	}
// 
// 	//check bottom path
// 	CPath2008 bottomPath;
// 	path[0] = GetLeftPath().getPoint(GetLeftPath().getCount() - 1);
// 	path[1] = GetCenterPath().getPoint(GetCenterPath().getCount() - 1);
// 	path[2] = GetRightPath().getPoint(GetRightPath().getCount() - 1);
// 	bottomPath.init(3,path);
// 	CPollygon2008 bottomPollygon;
// 	bottomPollygon.init(bottomPath.getCount(),bottomPath.getPointList());
// 	if (pollygon.IfOverlapWithOtherPolygon(bottomPollygon))
// 	{
// 		return true;
// 	}
	return false;
}

//----------------------------------------------------------------------------
//Summary:
//		Get shortest distance that from current point to walkway
//Parameters:
//		pt[in]: point to walkway
//Return:
//		shortest distance
//----------------------------------------------------------------------------
double CLandsideWalkTrafficObjectInSim::GetPointDistance( const CPoint2008& pt ) const
{
	const CPath2008& path = GetCenterPath();

	double dShortestDistance = (std::numeric_limits<double>::max)();
	//find walkway intersection point include start point and end point
	for (int i = 0; i < path.getCount(); i++)
	{
		CPoint2008 destPt = path.getPoint(i);
		double dDist = destPt.distance3D(pt);
		if (dDist < dShortestDistance)
		{
			dShortestDistance = dDist;
		}
	}

	//find from segment
	CPoint2008 startPt = path.getPoint(0);
	for (int ii = 1; ii < path.getCount(); ii++)
	{
		CPoint2008 currentPt = path.getPoint(ii);
		CLine2008 line;
		line.init(startPt,currentPt);

		double dDist = line.PointDistance(pt);
		if (dDist < dShortestDistance)
		{
			dShortestDistance = dDist;
		}
		startPt = currentPt;
	}

	return dShortestDistance;
}

//------------------------------------------------------------------------------
//Summary:
//		calculate intersection node with center path and reflect to center path
//Parameters:
//		pWalkwayInSim[in]: intersection walkway
//		vIntersection[out]: intersection nodes;
//-----------------------------------------------------------------------------
void CLandsideWalkTrafficObjectInSim::BuildIntersectionNode( CLandsideWalkTrafficObjectInSim* pTrafficObjectInSim,TrafficeNodeList& vIntersection)
{
	ASSERT(pTrafficObjectInSim);

	vIntersection.clear();

	CPoint2008 path[3];
	path[0] = pTrafficObjectInSim->GetLeftPath().getPoint(0);
	path[1] = pTrafficObjectInSim->GetCenterPath().getPoint(0);
	path[2] = pTrafficObjectInSim->GetRightPath().getPoint(0);
	CPath2008 topPath;
	topPath.init(3,path);

	CPath2008 bottomPath;
	path[0] = pTrafficObjectInSim->GetLeftPath().getPoint(pTrafficObjectInSim->GetLeftPath().getCount() - 1);
	path[1] = pTrafficObjectInSim->GetCenterPath().getPoint(pTrafficObjectInSim->GetCenterPath().getCount() - 1);
	path[2] = pTrafficObjectInSim->GetRightPath().getPoint(pTrafficObjectInSim->GetRightPath().getCount() - 1);
	bottomPath.init(3,path);

	//calculate intersection with center path
	IntersectWithCenterPath(pTrafficObjectInSim,GetCenterPath(),pTrafficObjectInSim->GetCenterPath(),vIntersection);
	//calculate intersection with left path
	IntersectWithBoundPath(pTrafficObjectInSim,GetCenterPath(),pTrafficObjectInSim->GetLeftPath(),vIntersection);
	//calculate intersection with right path
	IntersectWithBoundPath(pTrafficObjectInSim,GetCenterPath(),pTrafficObjectInSim->GetRightPath(),vIntersection);
	//calculate intersection with top path
	IntersectWithBoundPath(pTrafficObjectInSim,GetCenterPath(),topPath,vIntersection);
	//calculate intersection with bottom path
	IntersectWithBoundPath(pTrafficObjectInSim,GetCenterPath(),bottomPath,vIntersection);
}

//------------------------------------------------------------------------------
//Summary:
//		calculate left and right path intersection node reflect to center path
//Parameters:
//		pWalkwayInSim[in]: intersect walkway
//		centerPath[in]:	intersect path with other path
//		boundPath[in]:	intersect with centerPath
//		vIntersection[out]: intersection nodes;
//------------------------------------------------------------------------------
int CLandsideWalkTrafficObjectInSim::IntersectWithBoundPath( CLandsideWalkTrafficObjectInSim* pTrafficObjectInSim,const CPath2008& centerPath,const CPath2008& boundPath,TrafficeNodeList& vIntersection )
{
	int nThisSegCount = centerPath.getCount();
	int nThatSegCount = boundPath.getCount();

	for( int i=0; i<nThisSegCount - 1; i++ )
	{
		for( int m=0; m<nThatSegCount - 1; m++ )
		{
			CPoint2008 ptThisStart = centerPath.getPoint(i);
			CPoint2008 ptThisEnd =  centerPath.getPoint( i+1 );
			CPath2008 thisRealPath;
			thisRealPath.init(2);
			thisRealPath[0] = ptThisStart;
			thisRealPath[1] = ptThisEnd;

			ptThisStart.setZ(0.0);
			ptThisEnd.setZ(0.0);
			CLine2008 thisLine( ptThisStart, ptThisEnd );


			CPoint2008 ptThatStart = boundPath.getPoint( m );
			CPoint2008 ptThatEnd = boundPath.getPoint( m+1 );
			CPath2008 thatRealPath;
			thatRealPath.init(2);
			thatRealPath[0] = ptThatStart;
			thatRealPath[1] = ptThatEnd;

			ptThatStart.setZ(0.0);
			ptThatEnd.setZ(0.0);
			CLine2008 thatLine( ptThatStart, ptThatEnd );

			CPoint2008 ptIntersects;
			if( thisLine.intersects( thatLine, ptIntersects ) )
			{
				double dRealThisDist = thisRealPath.GetTotalLength();
				double dThisDist = thisLine.GetLineLength();
				double dPointThis = ptIntersects.distance3D(ptThisStart);
				double dRate1 = 0.0;
				if (dThisDist > 0.0)
				{
					dRate1 = dPointThis / dThisDist;
				}
				double dRealThatDist = thatRealPath.GetTotalLength();
				double dThatDist = thatLine.GetLineLength();
				double dPointThat = ptIntersects.distance3D(ptThatStart);
				double dRate2 = 0.0;
				if (dThatDist > 0.0)
				{
					dRate2 = dPointThat / dThatDist;
				}

				double dRealPoint1 = dRealThisDist*dRate1;
				double dRealPoint2 = dRealThatDist*dRate2;

				//check intersection point z is the same
				CPoint2008 ptRealThis = thisRealPath.GetDistPoint(dRealPoint1);
				CPoint2008 ptRealThat = thatRealPath.GetDistPoint(dRealPoint2);

				double dDeta = fabsl(ptRealThat.getZ() - ptRealThis.getZ());
				if (dDeta > (std::numeric_limits<double>::min)())
					continue;
				
				double dDistanc = ReflectIntersectionNodeToCenterPath(pTrafficObjectInSim->GetCenterPath(),ptRealThis);
				int nSeg = (int)pTrafficObjectInSim->GetCenterPath().GetDistIndex(dDistanc);
				if (exsitSameSegmentIntersectionNode(i,pTrafficObjectInSim,nSeg,vIntersection))
					continue;
	
				trafficIntersectionNode point;
				point.m_dDistanceFromStart = centerPath.GetPointDist(ptRealThis);
				point.m_nSegment = i;
				point.m_pTrafficObject = pTrafficObjectInSim;
				point.m_dDistWalkway = dDistanc;
				vIntersection.push_back( point );

				//put the intersection node on the intersection walkway
				TrafficeNodeList intersectPtList;
				trafficIntersectionNode otherPt;
				otherPt.m_dDistanceFromStart = dDistanc;
				otherPt.m_nSegment = nSeg;
				otherPt.m_pTrafficObject = this;
				otherPt.m_dDistWalkway = centerPath.GetPointDist(ptRealThat);
				intersectPtList.push_back(otherPt);
				pTrafficObjectInSim->m_vTrafficNode.push_back(intersectPtList);
			}
		}
	}
	return (int)vIntersection.size();
}

//------------------------------------------------------------------------------
//Summary:
//		calculate left and right path intersection node reflect to center path
//Parameters:
//		pWalkwayInSim[in]: intersect walkway
//		centerPath[in]:	intersect path with other path
//		middlePath[in]:	intersect with centerPath
//		vIntersection[out]: intersection nodes;
//------------------------------------------------------------------------------
int CLandsideWalkTrafficObjectInSim::IntersectWithCenterPath( CLandsideWalkTrafficObjectInSim* pTrafficObjectInSim,const CPath2008& centerPath,const CPath2008& middlePath,TrafficeNodeList& vIntersection )
{
	int nThisSegCount = centerPath.getCount();
	int nThatSegCount = middlePath.getCount();

	for( int i=0; i<nThisSegCount - 1; i++ )
	{
		for( int m=0; m<nThatSegCount - 1; m++ )
		{
			CPoint2008 ptThisStart = centerPath.getPoint(i);
			CPoint2008 ptThisEnd =  centerPath.getPoint( i+1 );
			CPath2008 thisRealPath;
			thisRealPath.init(2);
			thisRealPath[0] = ptThisStart;
			thisRealPath[1] = ptThisEnd;

			ptThisStart.setZ(0.0);
			ptThisEnd.setZ(0.0);
			CLine2008 thisLine( ptThisStart, ptThisEnd );

			CPoint2008 ptThatStart = middlePath.getPoint( m );
			CPoint2008 ptThatEnd = middlePath.getPoint( m+1 );
			CPath2008 thatRealPath;
			thatRealPath.init(2);
			thatRealPath[0] = ptThatStart;
			thatRealPath[1] = ptThatEnd;

			ptThatStart.setZ(0.0);
			ptThatEnd.setZ(0.0);
			CLine2008 thatLine( ptThatStart, ptThatEnd );

			CPoint2008 ptIntersects;
			if( thisLine.intersects( thatLine, ptIntersects ) )
			{
				double dRealThisDist = thisRealPath.GetTotalLength();
				double dThisDist = thisLine.GetLineLength();
				double dPointThis = ptIntersects.distance3D(ptThisStart);
				double dRate1 = 0.0;
				if (dThisDist > 0.0)
				{
					dRate1 = dPointThis / dThisDist;
				}
				double dRealThatDist = thatRealPath.GetTotalLength();
				double dThatDist = thatLine.GetLineLength();
				double dPointThat = ptIntersects.distance3D(ptThatStart);
				double dRate2 = 0.0;
				if (dThatDist > 0.0)
				{
					dRate2 = dPointThat / dThatDist;
				}

				double dRealPoint1 = dRealThisDist*dRate1;
				double dRealPoint2 = dRealThatDist*dRate2;

				//check intersection point z is the same
				CPoint2008 ptRealThis = thisRealPath.GetDistPoint(dRealPoint1);
				CPoint2008 ptRealThat = thatRealPath.GetDistPoint(dRealPoint2);

				double dDeta = fabsl(ptRealThat.getZ() - ptRealThis.getZ());
				if (dDeta > (std::numeric_limits<double>::min)())
					continue;

				trafficIntersectionNode point;
				point.m_dDistanceFromStart = centerPath.GetPointDist(ptRealThis);
				point.m_nSegment = i;
				point.m_pTrafficObject = pTrafficObjectInSim;
				point.m_dDistWalkway = middlePath.GetPointDist(ptRealThat);

				vIntersection.push_back( point );
			}
		}
	}
	return (int)vIntersection.size();
}

//-----------------------------------------------------------------------------
//Summary:
//		calculate bound path intersection node reflect to center path distance
//Parameter:
//		path[in]: reflect path
//		pt[in]: need convert point
//return:
//		distance from start to current point distance on center path
//-----------------------------------------------------------------------------
double CLandsideWalkTrafficObjectInSim::ReflectIntersectionNodeToCenterPath( const CPath2008& path,const CPoint2008& pt ) const
{
	int nPointCount = path.getCount();
	if (nPointCount == 0)
		return 0.0;

	CPoint2008 locationPt = pt;
	locationPt.setZ(0.0);
	CPoint2008 startPoint = path.getPoint(0);
	CPoint2008 startRealPt = startPoint;
	startPoint.setZ(0.0);

	DistanceUnit dShortestDistance = (std::numeric_limits<double>::max)();
	DistanceUnit dResDistance = 0.0;
	CPoint2008 ptIntersection;
	int nResIndex = -1;
	for( int i=1; i<nPointCount; i++ )
	{
		CPoint2008 destPoint = path.getPoint( i );
		CPoint2008 destRealPt = destPoint;
		destPoint.setZ(0.0);
		CPath2008 thisRealPath;
		thisRealPath.init(2);
		thisRealPath[0] = startRealPt;
		thisRealPath[1] = destRealPt;
		double dRealLength = thisRealPath.GetTotalLength();

		CLine2008 line( startPoint, destPoint );
		CPoint2008 intersectionPt;
		DistanceUnit dDistFromStartPt;
		if( line.GetIntersectPoint( locationPt, intersectionPt, dDistFromStartPt ) )
		{
			double dLine = line.GetLineLength();
			double dIntersect = intersectionPt.distance3D(startPoint);
			double dRate = 0.0;
			if (dLine > 0.0)
				dRate = dIntersect / dLine;
			double dRealDist = dRealLength*dRate;
			CPoint2008 ptRealIntersect = thisRealPath.GetDistPoint(dRealDist);

			DistanceUnit dDist = pt.distance3D( ptRealIntersect );
			if(dShortestDistance > dDist )
			{
				dShortestDistance = dDist;
				dResDistance = path.GetPointDist(ptRealIntersect);
				ptIntersection = intersectionPt;
			}
		}
		startPoint = destPoint;
		startRealPt = destRealPt;
	}

	for(int i=0; i<nPointCount; i++ )
	{
		CPoint2008 destPoint =path.getPoint( i );
		DistanceUnit dDist = pt.distance3D( destPoint );
		if(dShortestDistance > dDist )
		{
			dShortestDistance = dDist;
			dResDistance = path.GetIndexDist((float)i);
		}		
	}		

	return dResDistance;
}

//----------------------------------------------------------------------------
//Summary:
//		retrieve all the intersection node on the segment
//Parameters:
//		nSegment[in]: relative segment
//		ptList[out]: all intersection node on the segment
//Return:
//		count of the intersection nodes
//----------------------------------------------------------------------------
int CLandsideWalkTrafficObjectInSim::GetIntersectionOnSegment( int nSegment,TrafficeNodeList& ptList )
{
	trafficIntersectionNode point;

	for(unsigned i=0; i< m_vTrafficNode.size(); i++)
	{
		TrafficeNodeList& IntersectList = m_vTrafficNode[i];
		for( unsigned j=0; j<IntersectList.size(); j++ )
		{
			point = IntersectList[j];
			if( point.m_nSegment == nSegment )
			{
				ptList.push_back( point);
			} 
		}
	}

	return ptList.size();
}

//---------------------------------------------------------------------------
//Summary:
//		get point reflect on center path
//Parameters:
//		pt[in]: reflect center path point
//		resultPt[out]: point relative to pt
//Return:
//		true: successes
//		false: failed
//---------------------------------------------------------------------------
bool CLandsideWalkTrafficObjectInSim::GetProjectPoint( const CPoint2008& pt, CPoint2008& resultPt,double& dResultDist) const
{
	const CPath2008& centerPath = GetCenterPath();
	return GetProjectPoint(centerPath,pt,resultPt,dResultDist);
}
//---------------------------------------------------------------------------
//Summary:
//		retrieve project point
//---------------------------------------------------------------------------
bool CLandsideWalkTrafficObjectInSim::GetProjectPoint(const CPath2008& path, const CPoint2008& pt, CPoint2008& resultPt,double& dResultDist) const
{
	const CPath2008& centerPath = path;
	int nPointCount = centerPath.getCount();
	if (nPointCount == 0)
		return false;

	CPoint2008 locationPt = pt;
	locationPt.setZ(0.0);
	CPoint2008 startPoint = centerPath.getPoint(0);
	CPoint2008 startRealPt = startPoint;
	startPoint.setZ(0.0);

	DistanceUnit dShortestDistance = (std::numeric_limits<double>::max)();
	DistanceUnit dResDistance = 0.0;
	int nResIndex = -1;
	for( int i=1; i<nPointCount; i++ )
	{
		CPoint2008 destPoint = centerPath.getPoint( i );
		CPoint2008 destRealPt = destPoint;
		destPoint.setZ(0.0);
		CPath2008 thisRealPath;
		thisRealPath.init(2);
		thisRealPath[0] = startRealPt;
		thisRealPath[1] = destRealPt;
		double dRealLength = thisRealPath.GetTotalLength();

		CLine2008 line( startPoint, destPoint );
		CPoint2008 intersectionPt;
		DistanceUnit dDistFromStartPt;
		if( line.GetIntersectPoint( locationPt, intersectionPt, dDistFromStartPt ) )
		{
			double dLine = line.GetLineLength();
			double dIntersect = intersectionPt.distance3D(startPoint);
			double dRate = 0.0;
			if (dLine > 0.0)
				dRate = dIntersect / dLine;
			double dRealDist = dRealLength*dRate;
			CPoint2008 ptRealIntersect = thisRealPath.GetDistPoint(dRealDist);

			DistanceUnit dDist = pt.distance3D( ptRealIntersect );
			if(dShortestDistance > dDist )
			{
				dShortestDistance = dDist;
				resultPt = ptRealIntersect;
			}
		}
		startPoint = destPoint;
		startRealPt = destRealPt;
	}

	for(int i=0; i<nPointCount; i++ )
	{
		CPoint2008 destPoint =centerPath.getPoint( i );
		DistanceUnit dDist = pt.distance3D( destPoint );
		if(dShortestDistance > dDist )
		{
			dShortestDistance = dDist;
			resultPt = destPoint;
		}		
	}		
	dResultDist = dShortestDistance;
	return true;
}
//--------------------------------------------------------------------------
//Summary:
//		retrieve inside polygon with shortest project point
//--------------------------------------------------------------------------
bool CLandsideWalkTrafficObjectInSim::GetShortestProjectPoint(LandsideResourceInSim* pLandsideResInSim,const CPoint2008& pt, CPoint2008& resultPt) const
{
	CPoint2008 locationPt;

	for (int i = 0; i < (int)m_trafficInfo.size(); i++)
	{
		ParkingLotIntersectTrafficObject trafficObject = m_trafficInfo.at(i);
		if (trafficObject.m_pLandsideResInSim == pLandsideResInSim)
		{
			DistanceUnit dShortestDistance = (std::numeric_limits<double>::max)();
			for (int j = 0; j < (int)(trafficObject.m_OverlapPathList.size()); j++)
			{
				double dDist = 0.0;
				if(GetProjectPoint(trafficObject.m_OverlapPathList.at(j),pt,locationPt,dDist))
				{
					if (dDist < dShortestDistance)
					{
						dShortestDistance = dDist;
						resultPt = locationPt;
					}
				}
			}
			return true;
		}
	}
	return false;
}

bool CLandsideWalkTrafficObjectInSim::SameSegment(const CPoint2008& pt1, const CPoint2008& pt2)const
{
	int iSeg1 = static_cast<int>(GetCenterPath().GetPointIndex(pt1));
	int iSeg2 = static_cast<int>(GetCenterPath().GetPointIndex(pt2));

	if (iSeg1 == iSeg2)
		return true;
	
	return false;
}
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
bool CLandsideWalkTrafficObjectInSim::GetNearestVertex(const CPoint2008& pt,LandsideTrafficGraphVertex& leftVertex,LandsideTrafficGraphVertex& rightVertex)
{
	double dStartDistance = GetCenterPath().GetPointDist(pt);

	if (m_vDistNode.empty())
		BuildTrafficNode();
	
	double dLeft = (std::numeric_limits<double>::max)();
	double dRight = (std::numeric_limits<double>::max)();
	double dSelectLeft = 0.0;
	double dSelectRight = 0.0;
	for (int i = 0; i < (int)m_vDistNode.size(); i++)
	{
		double trafficNodeDist = m_vDistNode[i];
		double dDist = trafficNodeDist - dStartDistance;
		if (dDist > 0)
		{
			if (dDist < dRight)
			{
				dRight = dDist;
				dSelectRight = trafficNodeDist;
			}
		}
		else
		{
			dDist = fabs(dDist);
			if (dDist < dLeft)
			{
				dLeft = dDist;
				dSelectLeft = trafficNodeDist;
			}
		}
	}

	leftVertex.SetDistance(dSelectLeft);
	leftVertex.SetTrafficObjectInSim(this);

	rightVertex.SetDistance(dSelectRight);
	rightVertex.SetTrafficObjectInSim(this);

	return true;
}
//-----------------------------------------------------------------------------
//Summary:
//		build intersection node with start and end node then sort it by distance from start
//-----------------------------------------------------------------------------
void CLandsideWalkTrafficObjectInSim::BuildTrafficNode()
{
	for (int i = 0; i < (int)m_vTrafficNode.size();i++)
	{
		TrafficeNodeList& IntersectList = m_vTrafficNode[i];
		for (int j = 0; j < (int)IntersectList.size(); j++)
		{
			trafficIntersectionNode& trafficNode = IntersectList[j];
			double dDist = trafficNode.m_dDistanceFromStart;
			if(std::find(m_vDistNode.begin(),m_vDistNode.end(),dDist) == m_vDistNode.end())
			{
				m_vDistNode.push_back(dDist);
			}
		}	
	}
	m_vDistNode.push_back(0.0);
	m_vDistNode.push_back(GetCenterPath().GetTotalLength());
	std::sort(m_vDistNode.begin(),m_vDistNode.end());
}

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
bool CLandsideWalkTrafficObjectInSim::GetWalkwayVertex( LandsideTrafficGraphVertex& vertex,const CPoint2008& pt )
{
	double dStartDistance = GetCenterPath().GetPointDist(pt);
	vertex.SetDistance(dStartDistance);
	vertex.SetTrafficObjectInSim(this);
	return true;
}

//-----------------------------------------------------------------------------
//Summary:
//		left, right,bottom,top and center path two or more intersected with same resource with same segment
//		such as: L,R,C,T,B with same segment nSeg intersected with other resource with same segment
//-----------------------------------------------------------------------------
bool CLandsideWalkTrafficObjectInSim::exsitSameSegmentIntersectionNode( int nSegment,CLandsideWalkTrafficObjectInSim* pTrafficObjectInSim,int nOrtherSeg,const TrafficeNodeList& vIntersection ) const
{
	trafficIntersectionNode point;

	for(unsigned i=0; i< vIntersection.size(); i++)
	{
		point = vIntersection.at(i);
		if( point.m_nSegment == nSegment )
		{
			if (point.m_pTrafficObject = pTrafficObjectInSim)
			{
				int nSeg = (int)pTrafficObjectInSim->GetCenterPath().GetDistIndex(point.m_dDistWalkway);
				if (nSeg == nOrtherSeg)
				{
					return true;
				}
			}
		} 
	}	
	return false;
}

//----------------------------------------------------------------------------
//Summary:
//		retrieve shortest distance to inside polygon
//----------------------------------------------------------------------------
double CLandsideWalkTrafficObjectInSim::GetParkinglotPointDist( LandsideResourceInSim* pLandsideResInSim,const CPoint2008& pt ) const
{
	DistanceUnit dShortestDistance = (std::numeric_limits<double>::max)();
	if (m_trafficInfo.empty())
		return dShortestDistance;
	
	for (int i = 0; i < (int)m_trafficInfo.size(); i++)
	{
		ParkingLotIntersectTrafficObject trafficObject = m_trafficInfo.at(i);
		if (trafficObject.m_pLandsideResInSim == pLandsideResInSim)
		{
			for (int j = 0; j < (int)(trafficObject.m_OverlapPathList.size()); j++)
			{
				CPath2008 path;
				path = trafficObject.m_OverlapPathList.at(j);
				for (int m = 0; m < path.getCount(); m++)
				{
					CPoint2008 destPt = path.getPoint(m);
					double dDist = destPt.distance3D(pt);
					if (dDist < dShortestDistance)
					{
						dShortestDistance = dDist;
					}
				}

				//find from segment
				CPoint2008 startPt = path.getPoint(0);
				for (int m = 1; m < path.getCount(); m++)
				{
					CPoint2008 currentPt = path.getPoint(m);
					CLine2008 line;
					line.init(startPt,currentPt);

					double dDist = line.PointDistance(pt);
					if (dDist < dShortestDistance)
					{
						dShortestDistance = dDist;
					}
					startPt = currentPt;
				}
			}
		}
	}

	return dShortestDistance;
}

void CLandsideWalkTrafficObjectInSim::AddTafficParkinglot( const ParkingLotIntersectTrafficObject& trafficInfo )
{
	m_trafficInfo.push_back(trafficInfo);
}

double CLandsideWalkTrafficObjectInSim::GetLevel() const
{
	const CPath2008& centerPath = GetCenterPath();
	if (centerPath.getCount() > 0)
	{
		CPoint2008 pt = centerPath.getPoint(0);
		return pt.getZ();
	}
	return 0.0;
}
