#include "stdafx.h"
#include "OnBoardCorridorInSim.h"
#include "InputOnBoard/OnboardCorridor.h"
#include "OnboardFlightInSim.h"
#include "Common/Path2008.h"
#include <limits>
#include "Common/Line2008.h"
#include "common/DistanceLineLine.h"
#include "common/RayIntersectPath.h"

OnboardCorridorInSim::OnboardCorridorInSim(COnboardCorridor *pCorridor,OnboardFlightInSim* pFlightInSim)
:OnboardAircraftElementInSim(pFlightInSim)
,m_pCorridor(pCorridor)
{
	m_centerIntersectionPoint.clear();
	m_pCorridor->UpdatePolygon();
	CaculateSpans();
}

OnboardCorridorInSim::~OnboardCorridorInSim()
{

}

void OnboardCorridorInSim::CalculateSpace()
{

}

void OnboardCorridorInSim::CalculateIntersection(OnboardFlightInSim* pOnboarFlightInSim)
{
	//clear the data and rebuild intersection data
	int nSourceDeck = m_pCorridor->GetDeckNum();
	
	std::vector<OnboardCorridorInSim*>* pCorridorList = pOnboarFlightInSim->GetOnboardCorridorList();
	int nCorridorCount = (int)pCorridorList->size();
	for (int i = 0; i < nCorridorCount; i++)
	{
		OnboardCorridorInSim* pCorridorInSim = (*pCorridorList)[i];
		int nDestDeck = pCorridorInSim->m_pCorridor->GetDeckNum();
		
		if (pCorridorInSim != this && nSourceDeck == nDestDeck)
		{
			CorridorPointList IntersectionPointList;
			//calculate center path intersection
			CalculateIntersectionWithSingleCorridor(pCorridorInSim,IntersectionPointList);
			m_centerIntersectionPoint.push_back(IntersectionPointList);
		}
	}
}

void OnboardCorridorInSim::CalculateIntersectionWithSingleCorridor(OnboardCorridorInSim* pCorridorInSim,CorridorPointList& IntersectionPointList)
{
	IntersectionPointList.clear();
	//calculate intersection with left path
	IntersectionPath(pCorridorInSim,m_pCorridor->GetPathCenterLine(),pCorridorInSim->m_pCorridor->GetPathLeftLine(),IntersectionPointList);
	//calculate intersection with right path
	IntersectionPath(pCorridorInSim,m_pCorridor->GetPathCenterLine(),pCorridorInSim->m_pCorridor->GetPathRightLine(),IntersectionPointList);
	//calculate intersection with center path
	IntersectionCenterPath(pCorridorInSim,m_pCorridor->GetPathCenterLine(),pCorridorInSim->m_pCorridor->GetPathCenterLine(),IntersectionPointList);
}

int OnboardCorridorInSim::IntersectionPath(OnboardCorridorInSim* pCorridorInSim,const CPath2008& path1, const CPath2008& path2,CorridorPointList& IntersectionPointList)
{
	int nThisSegCount = path1.getCount();
	int nThatSegCount = path2.getCount();

	for( int i=0; i<nThisSegCount - 1; i++ )
	{
		for( int m=0; m<nThatSegCount - 1; m++ )
		{
			CPoint2008 ptThisStart = path1.getPoint(i);
			ptThisStart.setZ(0.0);
			CPoint2008 ptThisEnd =  path1.getPoint( i+1 );
			ptThisEnd.setZ(0.0);
			CLine2008 thisLine( ptThisStart, ptThisEnd );
			CPoint2008 ptThatStart = path2.getPoint( m );
			ptThatStart.setZ(0.0);
			CPoint2008 ptThatEnd = path2.getPoint( m+1 );
			ptThatEnd.setZ(0.0);
			CLine2008 thatLine( ptThatStart, ptThatEnd );

			CPoint2008 ptIntersects;
			if( thisLine.intersects( thatLine, ptIntersects ) )
			{
				CorridorPoint point;
				point.SetDistance(path1.GetPointDist(ptIntersects));
				point.SetSegment(i);
				point.SetIntersectionCorridor(pCorridorInSim);
				double dDistanc = ReflectIntersectionPointOnCorridor(pCorridorInSim->m_pCorridor->GetPathCenterLine(),ptIntersects);
				point.SetInterCorridorDist(dDistanc);
				IntersectionPointList.push_back( point );

				//put the intersection node on the intersection corridor
				CorridorPointList intersectPtList;
				CorridorPoint otherPt;
				otherPt.SetDistance(dDistanc);
				otherPt.SetSegment(m);
				otherPt.SetIntersectionCorridor(this);
				otherPt.SetInterCorridorDist(path1.GetPointDist(ptIntersects));
				intersectPtList.push_back(otherPt);
				pCorridorInSim->m_centerIntersectionPoint.push_back(intersectPtList);
				
			}
		}
	}
	return (int)IntersectionPointList.size();
}

void OnboardCorridorInSim::InsertIntersectionNode(int nSegment, double dRealDistance,EntryPointInSim* pEntryPoint)
{
	CorridorPoint pt;
	pt.SetDistance(dRealDistance);
	pt.SetSegment(nSegment);
	pt.SetEntryPointInSim(pEntryPoint);

	CorridorPointList ptList;
	ptList.push_back(pt);

	m_centerIntersectionPoint.push_back(ptList);
}

double OnboardCorridorInSim::GetShortestDistancePoint(const CPoint2008& pt,CPoint2008& destPt,const CPath2008& path,int& nSegment,double& dRealDistance)
{
	int nPointCount = path.getCount();
	if (nPointCount == 0)
	{
		return 0.0;
	}

	CPoint2008 startPoint = path.getPoint(0);
	startPoint.setZ(0.0);

	DistanceUnit dShortestDistance = (std::numeric_limits<double>::max)();
	CPoint2008 ptIntersection;
	for( int i=1; i<nPointCount; i++ )
	{
		CPoint2008 destPoint = path.getPoint( i );
		destPoint.setZ(0.0);
		CLine2008 line( startPoint, destPoint );
		CPoint2008 intersectionPt;
		DistanceUnit dDistFromStartPt;
		if( line.GetIntersectPoint( pt, intersectionPt, dDistFromStartPt ) )
		{
			DistanceUnit dDist = pt.distance( intersectionPt );
			if(dShortestDistance > dDist )
			{
				dShortestDistance = dDist;
				dRealDistance = path.GetPointDist(intersectionPt);
				ptIntersection = intersectionPt;
				nSegment = i - 1;
				destPt = ptIntersection;
			}
		}
		startPoint = destPoint;
	}

	for(int i=0; i<nPointCount; i++ )
	{
		CPoint2008 destPoint =path.getPoint( i );
		DistanceUnit dDist = pt.distance( destPoint );
		if(dShortestDistance > dDist )
		{
			dShortestDistance = dDist;
			dRealDistance = path.GetIndexDist((float)i);
			nSegment = (i == 0 ? i:i-1);
			destPt = destPoint;
		}		
	}		

	return dShortestDistance;
}

double OnboardCorridorInSim::ReflectIntersectionPointOnCorridor(const CPath2008& path,const CPoint2008& pt)const
{
	int nPointCount = path.getCount();
	if (nPointCount == 0)
	{
		return 0.0;
	}

	CPoint2008 startPoint = path.getPoint(0);
	startPoint.setZ(0.0);

	DistanceUnit dShortestDistance = (std::numeric_limits<double>::max)();
	DistanceUnit dResDistance = 0.0;
	CPoint2008 ptIntersection;
	int nResIndex = -1;
	for( int i=1; i<nPointCount; i++ )
	{
		CPoint2008 destPoint = path.getPoint( i );
		destPoint.setZ(0.0);
		CLine2008 line( startPoint, destPoint );
		CPoint2008 intersectionPt;
		DistanceUnit dDistFromStartPt;
		if( line.GetIntersectPoint( pt, intersectionPt, dDistFromStartPt ) )
		{
			DistanceUnit dDist = pt.distance( intersectionPt );
			if(dShortestDistance > dDist )
			{
				dShortestDistance = dDist;
				dResDistance = path.GetPointDist(intersectionPt);
				ptIntersection = intersectionPt;
			}
		}
		startPoint = destPoint;
	}

	for(int i=0; i<nPointCount; i++ )
	{
		CPoint2008 destPoint =path.getPoint( i );
		DistanceUnit dDist = pt.distance( destPoint );
		if(dShortestDistance > dDist )
		{
			dShortestDistance = dDist;
			dResDistance = path.GetIndexDist((float)i);
		}		
	}		

	return dResDistance;
}
int OnboardCorridorInSim::IntersectionCenterPath(OnboardCorridorInSim* pCorridorInSim,const CPath2008& path1, const CPath2008& path2,CorridorPointList& IntersectionPointList)const
{
	int nThisSegCount = path1.getCount();
	int nThatSegCount = path2.getCount();

	for( int i=0; i<nThisSegCount - 1; i++ )
	{
		for( int m=0; m<nThatSegCount - 1; m++ )
		{
			CPoint2008 ptThisStart = path1.getPoint(i);
			ptThisStart.setZ(0.0);
			CPoint2008 ptThisEnd =  path1.getPoint( i+1 );
			ptThisEnd.setZ(0.0);
			CLine2008 thisLine( ptThisStart, ptThisEnd );
			CPoint2008 ptThatStart = path2.getPoint( m );
			ptThatStart.setZ(0.0);
			CPoint2008 ptThatEnd = path2.getPoint( m+1 );
			ptThatEnd.setZ(0.0);
			CLine2008 thatLine( ptThatStart, ptThatEnd );

			CPoint2008 ptIntersects;
			if( thisLine.intersects( thatLine, ptIntersects ) )
			{
				CorridorPoint point;
				point.SetDistance(path1.GetPointDist(ptIntersects));
				point.SetSegment(i);
				point.SetIntersectionCorridor(pCorridorInSim);
				point.SetInterCorridorDist(path2.GetPointDist(ptIntersects));

				IntersectionPointList.push_back( point );
			}
		}
	}
	return (int)IntersectionPointList.size();
}
int OnboardCorridorInSim::GetIntersectionOnSegment(int nSegment,CorridorPointList& ptList)
{
	CorridorPoint point;

	for(unsigned i=0; i<m_centerIntersectionPoint.size(); i++)
	{
		CorridorPointList& IntersectList = m_centerIntersectionPoint[i];
		for( unsigned j=0; j<IntersectList.size(); j++ )
		{
			point = IntersectList[j];
			if( point.GetSegment() == nSegment )
			{
				ptList.push_back( point);
			} 
		}
	}

	return ptList.size();


}

CString OnboardCorridorInSim::GetType() const
{
	return m_pCorridor->GetType();
}

void OnboardCorridorInSim::CaculateSpans()
{
	if(!m_pCorridor)
		return;
	m_spans.Clear();
	//push first 
	CPath2008& center = m_pCorridor->GetPathCenterLine();
	CPath2008& right = m_pCorridor->GetPathRightLine();
	CPath2008& left = m_pCorridor->GetPathLeftLine();

	if(center.getCount()<1 || right.getCount()<1 || left.getCount()<1)
		return;

	//add fist item
	CoridorSpanItem leftfist;
	leftfist.ptCenter = center.getPoint(0);
	leftfist.dIndexInCenter = 0;
	leftfist.ptSide = left.getPoint(0);
	m_spans.m_leftSpans.push_back(leftfist);

	CoridorSpanItem rightfist;
	rightfist.ptCenter = center.getPoint(0);
	rightfist.dIndexInCenter = 0;
	rightfist.ptSide = right.getPoint(0);
	m_spans.m_rightSpans.push_back(rightfist);


	int preLeftIndex = 0;
	int preRightIndex = 0;	
	for(int i=1;i<center.getCount();i++)
	{
		CLine2008 centerline(center.getPoint(i-1),center.getPoint(i));
		{
			DistancePointPath3D distPath;
			distPath.GetSquared(center.getPoint(i),left);
			double dLeftIndex = max(preLeftIndex,distPath.m_fPathParameter);
			
			double preIndex = i-1;
			for(int ptidx = preLeftIndex+1;ptidx<dLeftIndex;ptidx++)
			{		
				DistancePointLine3D distline;
				distline.GetSquared(left.getPoint(ptidx),centerline);
				CoridorSpanItem leftItem;
				leftItem.dIndexInCenter = max(preIndex, i-1+distline.m_fLineParameter);
				leftItem.ptCenter = distline.m_clostPoint;
				leftItem.ptSide = left.getPoint(ptidx);

				preIndex = leftItem.dIndexInCenter;
				m_spans.m_leftSpans.push_back(leftItem);
			}
			preLeftIndex = (int)dLeftIndex;
		
			CoridorSpanItem leftItem;
			leftItem.dIndexInCenter = i;
			leftItem.ptCenter = center.getPoint(i);
			leftItem.ptSide = left.GetIndexPoint((float)dLeftIndex);
			m_spans.m_leftSpans.push_back(leftItem);
		}

		
		{
			DistancePointPath3D distPath;
			distPath.GetSquared(center.getPoint(i),right);
			double dRightIndex = max(preRightIndex,distPath.m_fPathParameter);
			double preIndex = i-1;
			for(int ptidx = preRightIndex+1;ptidx<dRightIndex;ptidx++)
			{		
				DistancePointLine3D distline;
				distline.GetSquared(right.getPoint(ptidx),centerline);
				CoridorSpanItem rightItem;
				rightItem.dIndexInCenter = max(preIndex, i-1+distline.m_fLineParameter);
				rightItem.ptCenter = distline.m_clostPoint;
				rightItem.ptSide = right.getPoint(ptidx);

				preIndex = rightItem.dIndexInCenter;
				m_spans.m_rightSpans.push_back(rightItem);
			}
			preRightIndex = (int)dRightIndex;
			
			//add last
			CoridorSpanItem rightItem;
			rightItem.dIndexInCenter = i;
			rightItem.ptCenter = center.getPoint(i);
			rightItem.ptSide = right.GetIndexPoint((float)dRightIndex);
			m_spans.m_rightSpans.push_back(rightItem);
		}	

	}
	//add last
	/*CoridorSpanItem leftItem;
	leftItem.dIndexInCenter = center.getCount()-1;
	leftItem.ptCenter = center.getPoint(center.getCount()-1);
	leftItem.ptSide = left.getPoint(left.getCount()-1);
	m_spans.m_leftSpans.push_back(leftItem);

	CoridorSpanItem rightItem;
	rightItem.dIndexInCenter = center.getCount()-1;
	rightItem.ptCenter = center.getPoint(center.getCount()-1);
	rightItem.ptSide = right.getPoint(right.getCount()-1);
	m_spans.m_rightSpans.push_back(rightItem);*/

}



bool OnboardCorridorInSim::bRayIntesect( const CPoint2008& pt, const CPoint2008& dir, DistanceUnit& dLen, CPoint2008& avoiddir ) const
{
	if(!m_pCorridor)
		return false;

	CPath2008& left = m_pCorridor->GetPathLeftLine();
	CPath2008& right = m_pCorridor->GetPathRightLine();
	
	CRayIntersectPath2D rayLeft(pt,dir,left);
	if( rayLeft.Get() )
	{
		CRayIntersectPath2D::OutRet retfist = *rayLeft.m_Out.begin();
		dLen = retfist.m_dist;
		avoiddir = CPoint2008(left.GetIndexDir((float)retfist.m_indexInpath));
		avoiddir.length(1);

		CRayIntersectPath2D rayRight(pt,dir,right);
		if(rayRight.Get())
		{
			retfist = *rayRight.m_Out.begin();
			if(retfist.m_dist<dLen)
			{
				dLen = retfist.m_dist;
				avoiddir = CPoint2008(right.GetIndexDir((float)retfist.m_indexInpath));		
				avoiddir.length(1);
			}
		}
		return true;
	}
	return false;
}

CPoint2008 OnboardCorridorInSim::GetDistancePoint( double dDistance )
{
	CPoint2008 pt;
	CPath2008& center = m_pCorridor->GetPathCenterLine();
	pt = center.GetDistPoint(dDistance);
	return pt;
}

int OnboardCorridorInSim::GetResourceID() const
{
	if(m_pCorridor)
		return m_pCorridor->GetID();

	return -1;
}

bool OnboardCorridorInSim::CoridorSpans::GetSpan( double cindex,CPoint2008& pL, CPoint2008& pR )const
{
	bool bGetLeft = false;
	bool bGetRight = false;
	for(int i=1;i<(int)m_leftSpans.size();i++)
	{
		const CoridorSpanItem& preItem = m_leftSpans[i-1];
		const CoridorSpanItem& item = m_leftSpans[i];
		if(cindex < item.dIndexInCenter)
		{
			double dR= (cindex-preItem.dIndexInCenter)/(item.dIndexInCenter-preItem.dIndexInCenter);
			pL = preItem.ptSide * (1-dR) + item.ptSide*dR;
			bGetLeft = true;
			break;
		}
	}

	for(int i=1;i<(int)m_rightSpans.size();i++)
	{
		const CoridorSpanItem& preItem = m_rightSpans[i-1];
		const CoridorSpanItem& item = m_rightSpans[i];
		if(cindex < item.dIndexInCenter)
		{
			double dR= (cindex-preItem.dIndexInCenter)/(item.dIndexInCenter-preItem.dIndexInCenter);
			pR = preItem.ptSide * (1-dR) + item.ptSide*dR;
			bGetRight = true;
			break;
		}
	}

	return bGetLeft&& bGetRight;
}

bool OnboardCorridorInSim::CoridorSpans::GetNextSpan( double cindex, CPoint2008& pL, CPoint2008& pR, double& nextIndex ) const
{
	int nIdxLeft = -1;
	int nIdxRight = -1;
	for(int i=0;i<(int)m_leftSpans.size();i++)
	{
		const CoridorSpanItem& item = m_leftSpans[i];
		if( cindex< item.dIndexInCenter )
		{
			nIdxLeft = i;
			break;
		}
	}
	for(int i=0;i<(int)m_rightSpans.size();i++)
	{
		const CoridorSpanItem& item = m_rightSpans[i];
		if(cindex< item.dIndexInCenter)
		{
			nextIndex = i;
			break;
		}
	}
	if(nIdxLeft>1 && nIdxLeft<(int)m_leftSpans.size()
		&& nIdxRight && nIdxRight < (int)m_rightSpans.size() )
	{

	}
	return false;

}

bool OnboardCorridorInSim::CoridorSpans::GetNextSpanReverse( double cindex,CPoint2008& pL, CPoint2008& pR, double& nextIndex ) const
{
	return false;
}