#include "StdAfx.h"
#include ".\airsidemeetingpointinsim.h"

#include "..\..\Results\AirsideFlightEventLog.h"
#include "../../Common/Path2008.h"
#include "../../Common/Pollygon2008.h"
#include "VehicleStretchInSim.h"
#include <algorithm>

AirsideMeetingPointInSim::AirsideMeetingPointInSim(CMeetingPoint* pPoint)
:AirsideResource()
,m_pMeetingPoint(pPoint)
{
	m_pMeetingPoint->GetServicePoint(m_Position);
}

AirsideMeetingPointInSim::~AirsideMeetingPointInSim(void)
{
}
CString AirsideMeetingPointInSim::GetName()
{
	return m_pMeetingPoint->GetObjNameString() ;
}

CPoint2008 AirsideMeetingPointInSim::GetPosition()
{
	return m_Position;
}

CMeetingPoint* AirsideMeetingPointInSim::GetMeetingPointInput()
{
	return m_pMeetingPoint.get() ;
}

AirsideResource::ResourceType AirsideMeetingPointInSim::GetType() const
{
	return AirsideResource::ResType_MeetingPoint;
}

CString AirsideMeetingPointInSim::GetTypeName() const
{
	return "Meeting point";
}

CString AirsideMeetingPointInSim::PrintResource() const
{
	return m_pMeetingPoint->GetObjNameString() ;
}

CPoint2008 AirsideMeetingPointInSim::GetDistancePoint(double dist) const
{
	return m_Position ;
}

void AirsideMeetingPointInSim::getDesc( ResourceDesc& resDesc )
{
	resDesc.resid = m_pMeetingPoint->getID() ;
	resDesc.resType = GetType();
	resDesc.strRes = PrintResource();
}

CPoint2008 AirsideMeetingPointInSim::GetWaitingPosition()
{
	CPoint2008* pointList = m_pMeetingPoint->GetPath()->getPointList();
	int nCount = m_pMeetingPoint->GetPath()->getCount();
	CPollygon2008 polygen;
	polygen.init(nCount, pointList);

	CPoint2008 pos = polygen.getRandPoint();

	return pos;
}

CPoint2008 AirsideMeetingPointInSim::GetCenterPoint()
{
	double davgX = 0; 
	double davgY = 0;

	CPath2008 path = *m_pMeetingPoint->GetPath();
	int nCount = path.getCount();
	for (int i = 0; i <nCount; i++)
	{
		davgX += path[i].getX();
		davgY += path[i].getY();
	}

	CPoint2008 pos(davgX/nCount, davgY/nCount,0);

	return pos;
}

bool AirsideMeetingPointInSim::IsPathOverlapWaitingArea( const CPath2008& path )
{
	CPoint2008* pointList = m_pMeetingPoint->GetPath()->getPointList();
	int nCount = m_pMeetingPoint->GetPath()->getCount();
	CPollygon2008 polygen;
	polygen.init(nCount, pointList);

	const CPoint2008* pointList2 = path.getPointList();
	nCount = path.getCount();
	CPollygon2008 polygen2;
	polygen2.init(nCount,pointList2);

	return polygen.IfOverlapWithOtherPolygon(polygen2);
}

void AirsideMeetingPointInSim::AddOutLane(VehicleLaneEntry* laneEntry )
{
	if (std::find(m_vOutLanes.begin(),m_vOutLanes.end(),laneEntry) != m_vOutLanes.end())
		return;

	m_vOutLanes.push_back(laneEntry);
}