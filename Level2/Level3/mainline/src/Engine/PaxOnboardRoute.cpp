#include "StdAfx.h"
#include ".\paxonboardroute.h"
#include "inputonboard/OnboardCorridor.h"
#include "OnboardAircraftElementInSim.h"
#include "OnBoardCorridorInSim.h"
#include "InputOnBoard/Deck.h"
#include "OnBoardingElevatorInSim.h"
#include "OnBoardingStairInSim.h"
#include "PaxEnplaneBehavior.h"


PaxOnboardRoute::PaxOnboardRoute(void)
{
}

PaxOnboardRoute::~PaxOnboardRoute(void)
{
}

void PaxOnboardRoute::SetRoute( CorridorGraphVertexList& vertexList )
{
	ClearRoute();

	if(vertexList.empty())
		return;

	double startDistInItem = 0;
	double lastDistInItem=0;
	ARCVector3 startPtInItem, lastPtInItem;
	int bSegStart = 0; // 0 not start, 1 corridor start, 2 vertical device start
	OnboardAircraftElementInSim* pPreElm = NULL;

	for(int i=0;i<(int)vertexList.size();i++)
	{
		//CorridorGraphVertex& vertexPre = vertexList[i-1];
		CorridorGraphVertex& vertexThis = vertexList[i];
		
		if(pPreElm!=vertexThis.GetElementInSim())
		{
			if(bSegStart==1 && pPreElm)
			{
				PaxOnBoardRouteSeg newSeg(pPreElm,startDistInItem, lastDistInItem );
				m_vItems.push_back(newSeg);
			}
			if(bSegStart==2 && pPreElm)
			{
				PaxOnBoardRouteSeg newSeg(pPreElm,startPtInItem,lastPtInItem);
				m_vItems.push_back(newSeg);
			}
			
			//start new
			if( vertexThis.GetVertexType()  == Corridor_Vertex )
			{
				bSegStart = 1;
				startDistInItem = vertexThis.GetCorridorVertex().GetDistance();
			}
			else if(vertexThis.IsDeviceVertex() )
			{
				bSegStart = 2;
				startPtInItem = vertexThis.GetDeviceVertex().GetLocation();
			}			
		}
		//
		pPreElm = vertexThis.GetElementInSim();
		if( vertexThis.GetVertexType()  == Corridor_Vertex )
		{
			bSegStart = 1;
			lastDistInItem = vertexThis.GetCorridorVertex().GetDistance();
		}
		else if(vertexThis.IsDeviceVertex() )
		{
			bSegStart = 2;
			lastPtInItem = vertexThis.GetDeviceVertex().GetLocation();
		}	
		else
		{
			bSegStart = 0;
		}

	}
	//add last
	if(bSegStart==1 && pPreElm)
	{
		PaxOnBoardRouteSeg newSeg(pPreElm,startDistInItem, lastDistInItem );
		m_vItems.push_back(newSeg);
	}
	if(bSegStart==2 && pPreElm)
	{
		PaxOnBoardRouteSeg newSeg(pPreElm,startPtInItem,lastPtInItem);
		m_vItems.push_back(newSeg);
	}
	
	
}
void PaxOnboardRoute::SetDirectRoute( CorridorGraphVertexList& vertexList )
{

	ClearRoute();

	if(vertexList.empty())
		return;
	double lastDistInItem=0;
	ARCVector3 lastPtInItem;

	for(int i=1;i<(int)vertexList.size();i++)
	{
		CorridorGraphVertex& vertexPre = vertexList[i-1];
		CorridorGraphVertex& vertexThis = vertexList[i];
		if(vertexPre.GetElementInSim() != vertexThis.GetElementInSim() )
			break;
		
		//check corridor
		if(vertexPre.GetVertexType() == Corridor_Vertex)
		{
			lastDistInItem = vertexPre.GetCorridorVertex().GetDistance();
			PaxOnBoardRouteSeg newSeg(vertexPre.GetElementInSim(),lastDistInItem, vertexThis.GetCorridorVertex().GetDistance());
			m_vItems.push_back(newSeg);
		}
	}
}
void PaxOnboardRoute::SetEndPoint( ARCVector3& endPoint )
{
	m_endPoint = CPoint2008(endPoint[VX],endPoint[VY],endPoint[VZ]);
}

CPoint2008 PaxOnboardRoute::GetPointOfRoute( double dDistance )
{
	ASSERT(0);//need to be implement
	return m_endPoint;
}

void PaxOnboardRoute::CutToCheckingPoint(double& dChekingPointDistInCorridor)
{
	std::vector<PaxOnBoardRouteSeg> vItems = m_vItems;

	for (int nItem = vItems.size() - 1; nItem >= 0; -- nItem)
	{
		PaxOnBoardRouteSeg& seg = vItems[nItem];
		if(seg.m_pElement && seg.m_pElement->GetType() == COnboardCorridor::TypeString)
		{
			if(seg.m_dDistFrom > seg.m_dDistTo) //<----
			{
				seg.m_dDistTo =seg.m_dDistTo  + PAXRADIUS;

			}
			else//--->
			{
				seg.m_dDistTo =seg.m_dDistTo - PAXRADIUS;
			}
			
			dChekingPointDistInCorridor = seg.m_dDistTo;

			m_endPoint = seg.GetDistancePoint(seg.m_dDistTo);
			break;
		}
		else
		{
			vItems.erase(vItems.begin() + nItem);
		}
	}

	if(vItems.size() > 0)
	{
		m_vItems.clear();
		m_vItems = vItems;
	}
}


//bool PaxOnBoardRouteSeg::GetRouteSpan( double dDistInRoute,CPoint2008& pC,CPoint2008& pL,CPoint2008& pR )const 
//{
//	if(!m_pElement)
//		return false;
//
//	const double distInElement = DistInElement(dDistInRoute);
//	if(m_pElement->GetType() == COnboardCorridor::TypeString)
//	{
//
//		COnboardCorridor* pCoord = ((OnboardCorridorInSim*)m_pElement)->m_pCorridor;
//		CPath2008& center = pCoord->GetPathCenterLine();
//		CPath2008& left = pCoord->GetPathLeftLine();
//		CPath2008& right = pCoord->GetPathRightLine();
//		
//		pC = center.GetDistPoint(distInElement);
//		pL = left.getClosestPoint(pC);
//		pR = right.getClosestPoint(pC);
//		return true;
//	}
//	return false;
//}

bool PaxOnBoardRouteSeg::IsOutofSeg( double dDist ,double& dextra) const
{
	if(dDist<0)
	{
		return false;
	}
	if(dDist> abs(m_dDistTo-m_dDistFrom))
	{
		dextra = dDist - abs(m_dDistFrom-m_dDistTo);
		return true;
	}
	return false;
}

DistanceUnit PaxOnBoardRouteSeg::DistInElement( DistanceUnit distInItem ) const
{
	if(m_dDistFrom<m_dDistTo)
	{
		return distInItem+m_dDistFrom;
	}
	return m_dDistFrom-distInItem;
}

CPoint2008 PaxOnBoardRouteSeg::GetBoundForce( const CPoint2008& pos ) const
{
	CPoint2008 ret;
	return ret;
}


double PaxOnBoardRouteSeg::GetDistInRouteItem( const CPoint2008& pt ) const
{
	if(!m_pElement)
		return 0;

	if(m_pElement->GetType() == COnboardCorridor::TypeString)
	{
		COnboardCorridor* pCoord = ((OnboardCorridorInSim*)m_pElement)->m_pCorridor;
		CPath2008& center = pCoord->GetPathCenterLine();
		double distInElm = center.GetPointDist(pt);
		return DistInSeg(distInElm);		
	}
	return 0;
}

double PaxOnBoardRouteSeg::DistInSeg( double distInElm ) const
{
	if(m_dDistFrom<=m_dDistTo)
	{
		return distInElm - m_dDistFrom;
	}
	else
		return m_dDistFrom - distInElm;
}

CPoint2008 PaxOnBoardRouteSeg::GetDir( double dDistInItem ) const
{
	double distInElm = DistInElement(dDistInItem);
	ARCVector3 ptdir;
	if(m_pElement && m_pElement->GetType() == COnboardCorridor::TypeString)
	{
		COnboardCorridor* pCoord = ((OnboardCorridorInSim*)m_pElement)->m_pCorridor;
		CPath2008& center = pCoord->GetPathCenterLine();
		ptdir = center.GetDistDir(distInElm);
		ptdir.z = 0;
		if(m_dDistFrom<=m_dDistTo)
			ptdir.SetLength(1);
		else
			ptdir.SetLength(-1);
	}
	return CPoint2008(ptdir);
}

bool PaxOnBoardRouteSeg::PointInSeg( const CPoint2008& pt, DistanceUnit& distInSeg )const
{
	if(m_pElement && m_pElement->GetType() == COnboardCorridor::TypeString)
	{
		COnboardCorridor* pCoridor = ((OnboardCorridorInSim*)m_pElement)->m_pCorridor;
		if(!pCoridor->m_polygon.contains2(pt))
			return false;
		DistanceUnit distInElm = pCoridor->GetPathCenterLine().GetClosestPtDist(pt);
		distInSeg = DistInSeg(distInElm);		
		return true;
	}	
	return false;
}

bool PaxOnBoardRouteSeg::GetFollowSideDir( const CPoint2008& pos, CPoint2008& dir, CPoint2008& closetPt )
{
	if(m_pElement && m_pElement->GetType() == COnboardCorridor::TypeString)
	{
		COnboardCorridor* pCoridor = ((OnboardCorridorInSim*)m_pElement)->m_pCorridor;
		CPoint2008 clostL,clostR;
		float Lindex = pCoridor->GetPathLeftLine().GetPointIndex(pos,clostL);
		float Rindex = pCoridor->GetPathRightLine().GetPointIndex(pos,clostR);
		double dWay = (m_dDistFrom<=m_dDistTo)?1:-1;
		if(clostL.distance(pos) < clostR.distance(pos) )
		{
			dir = CPoint2008(pCoridor->GetPathLeftLine().GetIndexDir(Lindex)*dWay);
			closetPt = clostL;
		}
		else
		{
			dir = CPoint2008(pCoridor->GetPathRightLine().GetIndexDir(Rindex)*dWay);
			closetPt = clostR;
		}
		return true;
	}	
	return false;
}



//CPoint2008 PaxOnBoardRouteSeg::GetClosetPoint( const CPoint2008& pt )const
//{
//
//	return pt;
//}

PaxOnBoardRouteSeg::PaxOnBoardRouteSeg()
:m_pElement(NULL)
{

}

PaxOnBoardRouteSeg::PaxOnBoardRouteSeg( OnboardAircraftElementInSim* pElm, const ARCVector3& pF,const ARCVector3& pT ) :m_pElement(pElm)
{
	m_ptFrom = CPoint2008(pF.x,pF.y,pF.z);
	m_ptTo = CPoint2008(pT.x,pT.y,pT.z);
	InitPtToDist();
}


CPoint2008 PaxOnBoardRouteSeg::GetFirstPointInSeg() const
{
	if(m_pElement && m_pElement->GetType() == COnboardCorridor::TypeString)
	{
		COnboardCorridor* pCoridor = ((OnboardCorridorInSim*)m_pElement)->m_pCorridor;
		return pCoridor->GetPathCenterLine().GetDistPoint(m_dDistFrom);	
	}	
	return CPoint2008(0,0,0);
}

int rayIntersect(const ARCVector2& pt, const ARCVector2& dir, const ARCVector2& p1, const ARCVector2& p2, ARCVector2& outPt)
{
	double sinA1= (p1-pt).cross(dir);
	double sinA2 = (p2-pt).cross(dir);
	if( sinA1*sinA2<=0 )
	{
		double a1 = sinA1;
		double a2 = sinA2;
		if(a1+a2>0)
		{
			outPt =  (a2*p1 + a1*p2)/(a1+a2);
			double positive = (outPt - pt ).dot(dir);
			return positive>0?1:-1;
		}
	}
	return 0;
}



bool PaxOnBoardRouteSeg::RayIntersect( const CPoint2008& pos, const CPoint2008& dir, CPoint2008& intersectpt,CNumRange& dirAnlgeRange )
{
	if(m_pElement && m_pElement->GetType() == COnboardCorridor::TypeString)
	{
		COnboardCorridor* pCoridor = ((OnboardCorridorInSim*)m_pElement)->m_pCorridor;
		CPollygon2008& poly =  pCoridor->m_polygon;
		
		for(int i=0;i<poly.getCount();i++)
		{
			
		}


	}	

	return false;
}

bool PaxOnBoardRouteSeg::GetSpan( double distInSeg,CPoint2008& pC, CPoint2008& pL, CPoint2008& pR ) const
{
	double dExtra;
	if(IsOutofSeg(distInSeg,dExtra))
		return false;

	if(m_pElement && m_pElement->GetType() == COnboardCorridor::TypeString)
	{
		OnboardCorridorInSim* pCoridor = ((OnboardCorridorInSim*)m_pElement);
		DistanceUnit distInElm =  DistInElement(distInSeg);
		double distIndex = pCoridor->m_pCorridor->GetPathCenterLine().GetDistIndex(distInElm);
		pC = pCoridor->m_pCorridor->GetPathCenterLine().GetIndexPoint(distIndex);
		return pCoridor->m_spans.GetSpan(distIndex, pL, pR);		
	}	
	return false;
}

DistanceUnit PaxOnBoardRouteSeg::GetHeight( double distInSeg ) const
{
	if(m_pElement && m_pElement->GetType() == COnboardCorridor::TypeString)
	{
		OnboardCorridorInSim* pCoridor = ((OnboardCorridorInSim*)m_pElement);
		//DistanceUnit distInElm =  DistInElement(distInSeg);
		return pCoridor->m_pCorridor->GetDeck()->RealAltitude();//GetPathCenterLine().GetDistPoint(distInElm);
		//return distPt.z;
	}	
	return 0;
}

void PaxOnBoardRouteSeg::InitPtToDist()
{
	if(m_pElement->GetType() == COnBoardStair::TypeString)
	{
		OnboardStairInSim* pStair = (OnboardStairInSim*)m_pElement;
		m_dDistFrom = pStair->m_realpath.GetPointDist(m_ptFrom);
		m_dDistTo = pStair->m_realpath.GetPointDist(m_ptTo);
	}
}

bool PaxOnBoardRouteSeg::GetNextSpan( double distInSeg, CPoint2008& pL, CPoint2008& pR, double& nextDistInSeg ) const
{
	return false;
}

bool PaxOnBoardRouteSeg::GetPath( CPath2008& path ) const
{
	if(m_pElement->GetType() == COnBoardStair::TypeString)
	{
		OnboardStairInSim* pStair = (OnboardStairInSim*)m_pElement;
		path = pStair->m_realpath.GetSubPath(m_dDistFrom,m_dDistTo);
		return true;
	}
	if(m_pElement->GetType() == COnBoardEscalator::TypeString)
	{
		path.init(2);
		path[0] = m_ptFrom;
		path[1] = m_ptTo;
		return true;
	}
	return false;
}
CPoint2008 PaxOnBoardRouteSeg::GetDistancePointInSeg( double dDistance ) const
{
	if(dDistance < m_dDistFrom)
		dDistance = m_dDistFrom;

	if(dDistance > m_dDistTo)
		dDistance = m_dDistTo;
	return GetDistancePoint(dDistance);


}

CPoint2008 PaxOnBoardRouteSeg::GetDistancePoint( double dDistance ) const
{
	CPoint2008 pt;
	if(m_pElement && m_pElement->GetType() == COnboardCorridor::TypeString)
	{
		COnboardCorridor* pCoord = ((OnboardCorridorInSim*)m_pElement)->m_pCorridor;
		CPath2008& center = pCoord->GetPathCenterLine();
		pt = center.GetDistPoint(dDistance);
	}
	return pt;
}
