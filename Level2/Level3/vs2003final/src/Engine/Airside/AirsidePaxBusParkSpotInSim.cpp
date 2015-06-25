#include "StdAfx.h"
#include ".\airsidepaxbusparkspotinsim.h"
#include "../../Results/AirsideFlightEventLog.h"
#include "Clearance.h"
#include "AirsideVehicleInSim.h"
#include "../../Common/DistanceLineLine.h"

AirsidePaxBusParkSpotInSim::AirsidePaxBusParkSpotInSim( AirsidePaxBusParkSpot * pParkSpot )
:m_pParkSpot(pParkSpot)
{
	InitParkSpotPath(pParkSpot);
}

AirsidePaxBusParkSpotInSim::~AirsidePaxBusParkSpotInSim()
{

}
AirsidePaxBusParkSpot* AirsidePaxBusParkSpotInSim::GetParkSpot()
{
	return m_pParkSpot.get();
}


CString AirsidePaxBusParkSpotInSim::PrintResource() const
{
	return m_pParkSpot->getName().GetIDString();
}

CPoint2008 AirsidePaxBusParkSpotInSim::GetDistancePoint( double dist ) const
{
	return m_pParkSpot->GetServicePoint();
}

void AirsidePaxBusParkSpotInSim::getDesc( ResourceDesc& resDesc )
{
	resDesc.resType = GetType();
	resDesc.strRes = PrintResource();
	resDesc.resid = m_pParkSpot->getID();
}

bool AirsidePaxBusParkSpotInSim::GetEnterParkSpotClearance( AirsideVehicleInSim * pVehicleInSim, ClearanceItem& lastItem)
{
	CPoint2008 pLocation = lastItem.GetPosition();
	CPath2008 subPath;
	DistancePointPath3D distPtPath(pLocation,m_leadinPath);
	if (distPtPath.GetSquared() > 10.0)
	{
		subPath = m_leadinPath;
		subPath.insertPointBeforeAt(pLocation,0);
		double dLength = subPath.GetTotalLength();
		subPath = subPath.GetSubPath(0,dLength - pVehicleInSim->GetLength() *0.5);
	}
	else
	{
		double dDist = m_leadinPath.GetPointDist(pLocation);
		double dLength = m_leadinPath.GetTotalLength();
		subPath = m_leadinPath.GetSubPath(dDist,dLength - pVehicleInSim->GetLength() *0.5);
	}

	DistanceUnit distOfPath = subPath.GetTotalLength();
	double avgSpd = pVehicleInSim->GetSpeed() * 0.5;
	ElapsedTime dT = ElapsedTime(distOfPath/avgSpd);

	ElapsedTime endTime = lastItem.GetTime() + dT;

	pVehicleInSim->SetMode(OnMoveInGate);
	WriteVehiclePathLog(pVehicleInSim,subPath,lastItem.GetTime(),pVehicleInSim->GetSpeed(),endTime,0.0,false);
	return true;
}

bool AirsidePaxBusParkSpotInSim::GetExitParkSpotClearance( AirsideVehicleInSim * pVehicleInSim, const ElapsedTime& tTime, const CPoint2008& ptLocation)
{
	CPath2008 subPath;
	DistancePointPath3D distPtPath(ptLocation,m_leadoutPath);

	if (distPtPath.GetSquared() > 10.0)
	{
		subPath = m_leadoutPath;
		subPath.insertPointAfterAt(ptLocation,subPath.getCount() - 1);
	}
	else
	{
		double dDist = m_leadoutPath.GetPointDist(ptLocation);
		subPath = m_leadoutPath.GetSubPath(0,dDist);
	}

	DistanceUnit distOfPath = subPath.GetTotalLength();
	double avgSpd = pVehicleInSim->GetOnRouteSpeed() * 0.5;
	ElapsedTime dT = ElapsedTime(distOfPath/avgSpd);
	ElapsedTime endTime = tTime + dT;

	
	WriteVehiclePathLog(pVehicleInSim,subPath,tTime,0.0,endTime,pVehicleInSim->GetOnRouteSpeed(),m_pParkSpot->IsPushBack());
	return true;
}

void AirsidePaxBusParkSpotInSim::WriteVehiclePathLog( AirsideVehicleInSim * pVehicleInSim,const CPath2008& path, const ElapsedTime& startTime,double startSpeed,const ElapsedTime& endTime,double endSpd,BOOL bPushBack)
{
	DistanceUnit DDist = path.GetTotalLength();
	ElapsedTime DT = endTime - startTime;

	if(DDist > 0 &&( startSpeed + endSpd ) >0)
	{
		for (int i = 0; i < path.getCount(); i++)
		{
			DistanceUnit realDist = path.GetIndexDist((float)i);
			double b = realDist / DDist;
			double r;

			r = b;

			CPoint2008 pPosition = path.getPoint(i);
			double dSpeed = startSpeed * (1.0-r) + endSpd * r;
			ElapsedTime dT = ElapsedTime ((double)DT * r);	
			ElapsedTime tTime = startTime + dT;
			pVehicleInSim->SetPosition(pPosition);
			pVehicleInSim->SetTime(tTime);
			pVehicleInSim->SetSpeed(dSpeed);
			pVehicleInSim->WirteLog(pPosition,dSpeed,tTime,bPushBack?true:false);
		}
	}
	
}

void AirsidePaxBusParkSpotInSim::InitParkSpotPath( AirsidePaxBusParkSpot * pParkSpot )
{
	//lead in path
	m_leadinPath = pParkSpot->getLeadInLine();
	if (m_leadinPath.getCount() != 0)
	{
		m_leadinPath.insertPointAfterAt(pParkSpot->GetServicePoint(), m_leadinPath.getCount()-1);
	}
	else
	{
		m_leadinPath.init(m_leadinPath.getCount()+1);
		m_leadinPath[0] = pParkSpot->GetServicePoint();
	}

	//lead out path
	m_leadoutPath.init(pParkSpot->getLeadOutLine().getCount()+1);
	m_leadoutPath[0] = pParkSpot->GetServicePoint();
	for(int i=0;i<pParkSpot->getLeadOutLine().getCount();++i)
		m_leadoutPath[i+1] = pParkSpot->getLeadOutLine()[i];
}
