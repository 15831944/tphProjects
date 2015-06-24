#include "StdAfx.h"
#include ".\airspaceholdinsim.h"
#include "AirspaceResourceManager.h"
#include "AirRoutePointInSim.h"

#include "../../Common/path.h"
#include "../../Common/point.h"
#include "../../Common/elaptime.h"
#include "../../Common/ARCUnit.h"



AirspaceHoldInSim::AirspaceHoldInSim(AirHold* pHold,AirRouteNetworkInSim* pNetwork, AirWayPointInSim* pWaypoint, double dAirportVar)
{
	m_pAirHoldInput = pHold;
	m_pAirRouteNetwork = pNetwork;
	m_pWaypoint = pWaypoint;

	LevelHeight = ARCUnit::ConvertLength(1000,ARCUnit::FEET,ARCUnit::CM);
	m_nMaxFlightNum = int((pHold->getMaxAltitude() - pHold->getMinAltitude())/LevelHeight);
	if (m_nMaxFlightNum <1)
	{
		m_nMaxFlightNum = 1;
		LevelHeight = pHold->getMaxAltitude() - pHold->getMinAltitude() ;
	}

	Point pos;
	pos.setX(m_pWaypoint->GetPosition().getX());
	pos.setY(m_pWaypoint->GetPosition().getY());

	m_path = m_pAirHoldInput->GetPath(pos,dAirportVar);
	m_path.insertPointAfterAt(m_pWaypoint->GetPosition(),m_path.getCount()-1);

	m_dATCManagedDist = 0;

	m_vLockFlights.clear();
	m_vHoldingFlights.clear();
}

AirspaceHoldInSim::~AirspaceHoldInSim(void)
{
	if (m_pAirHoldInput)
	{
		delete m_pAirHoldInput;
		m_pAirHoldInput = NULL;
	}
}

CString AirspaceHoldInSim::PrintResource()const
{
	CString strName;
	strName.Format("%s",m_pAirHoldInput->GetObjNameString());
	return strName;
}

CPoint2008 AirspaceHoldInSim::GetDistancePoint(double dist)const
{
	return m_path.GetDistPoint(dist);
}

CPath2008 AirspaceHoldInSim::GetPath() const
{
	return m_path;
}

void AirspaceHoldInSim::ReleaseLock(AirsideFlightInSim * pFlight, const ElapsedTime& tTime)
{
	RemoveLockFlight(pFlight);
	NotifyObservers(tTime);

	if (m_vHoldingFlights.empty())
		return;

	if (m_vHoldingFlights[0] == pFlight)
	{
		m_vHoldingFlights.erase(m_vHoldingFlights.begin());

		OccupancyInstance ocyInstance  = GetOccupyInstance(pFlight);
		if (ocyInstance.GetFlight() == pFlight)
		{
			SetExitTime(pFlight,tTime);
		}
	}
}

bool AirspaceHoldInSim::GetLock(AirsideFlightInSim * pFlight)
{
	if (TryLock(pFlight))
	{
		std::vector<AirsideFlightInSim*>::iterator iter = std::find(m_vLockFlights.begin(),m_vLockFlights.end(),pFlight);

		if (iter == m_vLockFlights.end())
		{
			m_vLockFlights.push_back(pFlight);
		}
		return true;
	}
	return false;
}

bool AirspaceHoldInSim::TryLock(AirsideFlightInSim * pFlight)
{
	std::vector<AirsideFlightInSim*>::iterator iter = std::find(m_vLockFlights.begin(),m_vLockFlights.end(),pFlight);

	if (iter != m_vLockFlights.end())
		return true;

	int nLockSize = m_vLockFlights.size();
	return nLockSize < m_nMaxFlightNum;
}

void AirspaceHoldInSim::SetEnterTime(CAirsideMobileElement * pFlight, const ElapsedTime& enterT, AirsideMobileElementMode fltMode)
{
	OccupancyInstance ocyInstance  = GetOccupyInstance(pFlight);
	if (ocyInstance.GetFlight() == pFlight && (ocyInstance.GetEnterTime()< enterT && ocyInstance.GetEnterTime() == enterT))
		return;

	AirsideResource::SetEnterTime(pFlight,enterT,fltMode);
	AddHoldingFlight((AirsideFlightInSim*)pFlight);
}

void AirspaceHoldInSim::RemoveLockFlight(AirsideFlightInSim* pFlight)
{
	int nCount = m_vLockFlights.size();
	for (int i =0; i < nCount; i++)
	{
		if (m_vLockFlights.at(i) == pFlight)
		{
			m_vLockFlights.erase(m_vLockFlights.begin() +i);
			return;
		}
	}
}

AirHold* AirspaceHoldInSim::GetHoldInput() const
{
	return m_pAirHoldInput;
}

AirWayPointInSim* AirspaceHoldInSim::GetWaypointInSim() const
{
	return m_pWaypoint;
}

int AirspaceHoldInSim::GetHeadFlightCount(AirsideFlightInSim* pFlight)
{
	if (m_vHoldingFlights.empty())
		return 0;

	int nCount = m_vHoldingFlights.size();
	for (int i =0; i < nCount; i++)
	{
		if (m_vHoldingFlights[i] == pFlight)
			return i;
	}

	return nCount;
}

double AirspaceHoldInSim::getAvailableHeight(AirsideFlightInSim* pFlight)
{
	int nHeadFlight = GetHeadFlightCount(pFlight);
	double dAlt = pFlight->GetCurState().m_dAlt;

	if (nHeadFlight == 0)
		return m_pAirHoldInput->getMinAltitude() + LevelHeight/2.0;

	double dCurMinHeight = m_pAirHoldInput->getMinAltitude();

	for (int i = 0; i < nHeadFlight; i ++)
	{
		AirsideFlightInSim* pHoldingFlight = m_vHoldingFlights.at(i);

		if (pHoldingFlight == pFlight)
			return dAlt;

		if (pHoldingFlight->GetCurState().m_dAlt > dCurMinHeight)
			dCurMinHeight = pHoldingFlight->GetCurState().m_dAlt;		
	}

	return dCurMinHeight + LevelHeight;
}

bool AirspaceHoldInSim::IsAtFirstHeightLevel(AirsideFlightInSim* pFlight)
{
	if (GetHeadFlightCount(pFlight) >0)
		return false;

	double dAlt = pFlight->GetCurState().m_dAlt;
	return (dAlt - m_pAirHoldInput->getMinAltitude())< LevelHeight;
}

void AirspaceHoldInSim::AddHoldingFlight(AirsideFlightInSim* pFlight)
{
	std::vector<AirsideFlightInSim*>::iterator iter = std::find(m_vHoldingFlights.begin(),m_vHoldingFlights.end(),pFlight);

	if (iter != m_vHoldingFlights.end())
		return;

	m_vHoldingFlights.push_back(pFlight);
}

AirRouteNetworkInSim* AirspaceHoldInSim::GetAirRouteNetwork()
{
	return m_pAirRouteNetwork;
}

#include "..\..\Results\AirsideFlightEventLog.h"
void AirspaceHoldInSim::getDesc( ResourceDesc& resDesc )
{
	resDesc.resid = GetHoldInput()->getID();
	resDesc.resType = GetType();
	resDesc.strRes = PrintResource();
}
