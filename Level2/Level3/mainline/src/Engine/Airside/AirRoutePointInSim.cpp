#include "StdAfx.h"
#include "AirRoutePointInSim.h"
#include "../../Common/ARCUnit.h"
#include "AirsideFlightInSim.h"
#include "Clearance.h"
#include "AirsideResourceManager.h"
#include "AirTrafficController.h"
#include "FlightPerformanceManager.h"
#include "../../Common/line.h"
#include "../../InputAirside/AirWayPoint.h"
#include "../../InputAirside/AirRoute.h"
#include "../../InputAirside/AirRouteIntersection.h"
#include "InTrailSeparationInSim.h"


AirRoutePointInSim::AirRoutePointInSim(void)
{
	m_dDistToRunway = 0.0;
	m_dAltHigh = 0.0;
	m_dAltLow = 0.0;
}

AirRoutePointInSim::~AirRoutePointInSim(void)
{

}

void AirRoutePointInSim::CalculateClearanceAtPoint(AirsideFlightInSim* pFlight, AirsideFlightInSim* pFlightLead,ClearanceItem& lastItem, ClearanceItem& newItem)
{

}

ElapsedTime AirRoutePointInSim::RecalculateEnterTime(AirsideFlightInSim* pFlight,AirsideFlightInSim* pFlightLead,ElapsedTime tLeadEnter, ElapsedTime tEnter,const AirspaceIntrailSeparationSpeed& leadSpd)
{
	CInTrailSeparationInSim* pIntrailSeparation = pFlight->GetAirTrafficController()->GetIntrailSeparation();
	double dAlt = (GetAltLow()+GetAltHigh())/2.0;
	ElapsedTime LeadSepTime = 0L;
	DistanceUnit LeadSepDist = 0.0;

	pIntrailSeparation->GetInTrailSeparationDistanceAndTime(pFlight,pFlightLead,dAlt,LeadSepDist,LeadSepTime);

	double dLeadSpd = leadSpd.GetSparationSpeed(pFlight,LeadSepDist);
	ElapsedTime tTime = LeadSepDist/dLeadSpd;

	if (tTime > LeadSepTime)
		return (tEnter - tLeadEnter) >= tTime ? tEnter : (tLeadEnter + tTime);
	return (tEnter - tLeadEnter) >= LeadSepTime ? tEnter : (tLeadEnter + LeadSepTime);

}

bool AirRoutePointInSim::IsIntersectionRoute(AirRouteInSim* pRoute)
{
	int nCount = int(m_vIntersectionRoutes.size());
	for (int i =0; i < nCount; i++)
	{
		if (m_vIntersectionRoutes[i] == pRoute->GetAirRouteInput())
			return true;
	}
	return false;
}

bool AirRoutePointInSim::IsIntersection()
{
	if (int(m_vIntersectionRoutes.size()) >= 2)
		return true;

	return false;
}

OccupancyInstance AirRoutePointInSim::GetFrontOccupancyInstance(AirsideFlightInSim* pFlight, ElapsedTime tEnter)
{
	int nCount = m_vOccupancyTable.size();
	if (nCount == 1 && m_vOccupancyTable.at(0).GetEnterTime() <= tEnter)
		return m_vOccupancyTable.at(0);

	for (int i = nCount -1; i >0; i--)
	{
		OccupancyInstance instanceBack = m_vOccupancyTable.at(i);
		OccupancyInstance instanceFront = m_vOccupancyTable.at(i-1);

		if (instanceBack.GetFlight() == pFlight)
			return instanceFront;

		if(i == nCount -1 && instanceBack.GetEnterTime() <= tEnter)
			return instanceBack;

		if (instanceFront.GetFlight() == pFlight)
			continue;

		if (instanceBack.GetEnterTime() > tEnter && instanceFront.GetEnterTime() <= tEnter)
			return instanceFront;
	}
	OccupancyInstance emptyInstance;
	return emptyInstance;
}

double AirRoutePointInSim::getDistToRunway() const
{
	return m_dDistToRunway;
}

void AirRoutePointInSim::setDistToRunway( double dDist )
{
	m_dDistToRunway = dDist;
}

void AirRoutePointInSim::SetEnterTime( CAirsideMobileElement * pFlight, const ElapsedTime& enterT, AirsideMobileElementMode fltMode )
{
	ASSERT(FALSE);
	__super::SetEnterTime(pFlight, enterT, fltMode);
}

void AirRoutePointInSim::SetEnterTime( CAirsideMobileElement * pFlight, const ElapsedTime& enterT, AirsideMobileElementMode fltMode,double dSpd )
{
	ASSERT(dSpd>0);
	__super::SetEnterTime(pFlight, enterT, fltMode,dSpd);
}

//////////////////////////////AirWaypointInSimList////////////////////////////////
AirRoutePointInSim * AirRoutePointInSimList::GetRoutePointByIdx( int nIdx )
{
	if (nIdx < 0)
		return NULL;

	return m_vRoutePoints.at(nIdx).get() ;
}

AirWayPointInSim* AirRoutePointInSimList::GetAirWaypointInSim(ARWaypoint* pWaypoint)
{
	for(int i =0; i < int(m_vRoutePoints.size()); i++)
	{
		AirRoutePointInSim* pPoint = GetRoutePointByIdx(i);
		if (pPoint->GetType() == AirsideResource::ResType_WayPoint && pPoint->getID() == pWaypoint->getWaypoint().getID())
		{		
			return (AirWayPointInSim*)pPoint;
		}
	}

	return NULL;
}

AirWayPointInSim* AirRoutePointInSimList::GetAirWaypointInSimByID(int nID)
{
	for(int i =0; i < int(m_vRoutePoints.size()); i++)
	{
		AirRoutePointInSim* pPoint = GetRoutePointByIdx(i);
		if (pPoint->GetType() == AirsideResource::ResType_WayPoint && pPoint->getID() == nID)
		{		
			return (AirWayPointInSim*)pPoint;
		}
	}

	return NULL;
}

AirspaceIntersectionInSim* AirRoutePointInSimList::GetRouteIntersectionInSim(CAirRouteIntersection* pIntersection)
{
	for(int i =0; i < int(m_vRoutePoints.size()); i++)
	{
		AirRoutePointInSim* pPoint = GetRoutePointByIdx(i);
		if (pPoint->GetType() == AirsideResource::ResType_AirRouteIntersection && pPoint->getID() == pIntersection->getID())
		{		
			return (AirspaceIntersectionInSim*)pPoint;
		}
	}

	return NULL;
}

bool AirRoutePointInSimList::Add( AirRoutePointInSim* pRoutePoint )
{
	for (int i =0; i < int(m_vRoutePoints.size()); i++)
	{
		if (m_vRoutePoints[i] == pRoutePoint)
			return false;
	}

	m_vRoutePoints.push_back(pRoutePoint);
	return true;
}

void AirRoutePointInSimList::InsertPoint(int nIdx, AirRoutePointInSim* pRoutePoint)
{
	for (int i =0; i < int(m_vRoutePoints.size()); i++)
	{
		if (m_vRoutePoints[i] == pRoutePoint)
			return;
	}
	m_vRoutePoints.insert(m_vRoutePoints.begin()+nIdx, pRoutePoint);
}

int AirRoutePointInSimList::GetAirWaypointIdxInListByID(int nID)
{
	for(int i = 0; i < int(m_vRoutePoints.size()); i++)
	{
		AirRoutePointInSim* pPoint = GetRoutePointByIdx(i);
		if (pPoint->GetType() == AirsideResource::ResType_WayPoint && pPoint->getID() == nID)
		{		
			return i;
		}
	}

	return -1;
}

int AirRoutePointInSimList::GetRouteIntersectionIdxInListByID(int nID)
{
	for(int i = 0; i < int(m_vRoutePoints.size()); i++)
	{
		AirRoutePointInSim* pPoint = GetRoutePointByIdx(i);
		if (pPoint->GetType() == AirsideResource::ResType_AirRouteIntersection && pPoint->getID() == nID)
		{		
			return i;
		}
	}

	return -1;
}

/////////////////////////////////AirWaypointInSim////////////////////////////////////
AirWayPointInSim::AirWayPointInSim( int i ,const ALTAirport& refARP)
{
	m_wayPtInput = new AirWayPoint;
	m_wayPtInput->ReadObject(i);
	//
	m_pos = m_wayPtInput->GetLocation(refARP);
	m_dAltHigh = m_wayPtInput->GetHighLimit();
	m_dAltLow = m_wayPtInput->GetLowLimit();
	m_pHold = NULL;

}

CString AirWayPointInSim::PrintResource() const
{
	return m_wayPtInput->GetObjNameString();	
}

#include "..\..\Results\AirsideFlightEventLog.h"
void AirWayPointInSim::getDesc( ResourceDesc& resDesc )
{
	resDesc.resid = getID();
	resDesc.resType = GetType();
	resDesc.strRes = PrintResource();
}

/////////////////////////////AirspaceIntersectionInSim////////////////////////////////
AirspaceIntersectionInSim::AirspaceIntersectionInSim( CAirRouteIntersection* pIntersection)
{
	m_pRouteIntersection = pIntersection;

	m_pos = m_pRouteIntersection->getPos();
	m_dAltHigh = m_pRouteIntersection->getAltHigh();
	m_dAltLow =  m_pRouteIntersection->getAltLow();

}

CString AirspaceIntersectionInSim::PrintResource() const
{
	return m_pRouteIntersection->getName();	
}

int AirspaceIntersectionInSim::getID() const
{
	return m_pRouteIntersection->getID();
}


#include "..\..\Results\AirsideFlightEventLog.h"
void AirspaceIntersectionInSim::getDesc( ResourceDesc& resDesc )
{
	resDesc.resid = getID();
	resDesc.resType = GetType();
	resDesc.strRes = PrintResource();
}
//////////////////////////////////////////////////////////////////////////
//
FlightPlanPhasePointInSim::FlightPlanPhasePointInSim()
{

}

CString FlightPlanPhasePointInSim::PrintResource() const
{
	return _T("Flight Plan Phase");
}

void FlightPlanPhasePointInSim::getDesc( ResourceDesc& resDesc )
{
	resDesc.resid	= getID();
	resDesc.resType = GetType();
	resDesc.strRes	=  PrintResource();
}

int FlightPlanPhasePointInSim::getID() const
{
	return -1;
}