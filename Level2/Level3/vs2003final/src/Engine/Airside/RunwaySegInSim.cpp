#include "StdAfx.h"
#include ".\runwayseginsim.h"
#include "RunwayInSim.h"




RunwayDirectSegInSim::RunwayDirectSegInSim(RunwaySegInSim *pSeg, FlightGroundRouteSegDirection dir)
:FlightGroundRouteDirectSegInSim(pSeg,dir)
{
	m_dirPath = GetSubPath(GetRunwaySeg()->getRunway()->GetRunwayInput()->GetPath(),GetEnterPosInTaxiway(),GetExitPosInTaxiway());
	m_distance = m_dirPath.GetTotalLength();
}

RunwayDirectSegInSim::~RunwayDirectSegInSim()
{

}

CString RunwayDirectSegInSim::PrintResource() const
{
	CString strName;
	strName.Format("(%d - %d)", GetEntryNode()->GetID(),GetExitNode()->GetID());
	return strName;
}

RUNWAY_MARK RunwayDirectSegInSim::GetLogicRunwayType()
{
	RunwaySegInSim* pRunwaySeg = (RunwaySegInSim *)m_pSeg;

	LogicRunwayInSim *pLogicRunway = pRunwaySeg->GetLogicRunway(RUNWAYMARK_FIRST);
	
	if (pLogicRunway)
	{
		IntersectionNodeInSim *pEntryNode = GetEntryNode();
		IntersectionNodeInSim *pExitNode = GetExitNode();
		
		DistanceUnit entryDistance = pLogicRunway->GetPointDist(pEntryNode->GetNodeInput().GetPosition());
		DistanceUnit exitDistance =pLogicRunway->GetPointDist(pExitNode->GetNodeInput().GetPosition());
		if(entryDistance < exitDistance )
			return RUNWAYMARK_FIRST;
		else
			return RUNWAYMARK_SECOND;

	}

	return RUNWAYMARK_FIRST;
}

RunwaySegInSim * RunwayDirectSegInSim::GetRunwaySeg()const
{
	return (RunwaySegInSim *)m_pSeg;
}

#include "..\..\Results\AirsideFlightEventLog.h"
void RunwayDirectSegInSim::getDesc( ResourceDesc& resDesc )
{
	resDesc.resid = GetRunwaySeg()->GetRunwayID();
	resDesc.resType = GetType();
	resDesc.strRes =PrintResource();
	resDesc.fromnode = GetEntryNode()->GetID();
	resDesc.tonode = GetExitNode()->GetID();
}

int RunwayDirectSegInSim::GetObjectID() const
{
	return GetRunwaySeg()->GetRunwayID();
}

void RunwayDirectSegInSim::SetEnterTime( CAirsideMobileElement * pFlight, const ElapsedTime& enterT, AirsideMobileElementMode fltMode )
{
	__super::SetEnterTime(pFlight, enterT, fltMode);
	LogicRunwayInSim* pRunway = GetRunwaySeg()->GetLogicRunway(GetLogicRunwayType());
	if(pRunway)
	{
		pRunway->SetEnterTime(pFlight,enterT, fltMode);
	}
}

void RunwayDirectSegInSim::SetEnterTime( CAirsideMobileElement * pFlight, const ElapsedTime& enterT, AirsideMobileElementMode fltMode,double dSpd )
{
	__super::SetEnterTime(pFlight, enterT, fltMode,dSpd);
	LogicRunwayInSim* pRunway = GetRunwaySeg()->GetLogicRunway(GetLogicRunwayType());
	if(pRunway)
	{
		pRunway->SetEnterTime(pFlight,enterT, fltMode);
	}
}

void RunwayDirectSegInSim::SetExitTime( CAirsideMobileElement * pFlight, const ElapsedTime& exitT )
{
	__super::SetExitTime(pFlight, exitT);
	LogicRunwayInSim* pRunway = GetRunwaySeg()->GetLogicRunway(GetLogicRunwayType());
	if(pRunway)
	{
		pRunway->SetExitTime(pFlight,exitT);
	}
}





//////////////////////////////////////////////////////////////////////////
//RunwaySegInSim
RunwaySegInSim::RunwaySegInSim( IntersectionNodeInSim * pNodeFrom , IntersectionNodeInSim * pNodeTo, RunwayInSim * pRunwayInSim )
:FlightGroundRouteSegInSim(pNodeFrom,pNodeTo,pRunwayInSim)
{	
	m_pRunwayInSim = pRunwayInSim;
	m_positiveDirSeg = new RunwayDirectSegInSim(this, PositiveDirection);
	m_negativeDirSeg = new RunwayDirectSegInSim(this, NegativeDirection);

}
RunwaySegInSim::~RunwaySegInSim(void)
{
}

double RunwaySegInSim::GetNode1Pos() const
{
	return m_pNode1->GetNodeInput().GetDistance(m_pRunwayInSim->GetRunwayInput()->getID());
}

double RunwaySegInSim::GetNode2Pos() const
{	
	return m_pNode2->GetNodeInput().GetDistance(m_pRunwayInSim->GetRunwayInput()->getID());

}

int RunwaySegInSim::GetRunwayID()const
{
	return m_pRunwayInSim->GetRunwayInput()->getID();
}

LogicRunwayInSim * RunwaySegInSim::GetLogicRunway( RUNWAY_MARK runwayMark )
{
	if(runwayMark == RUNWAYMARK_FIRST)
		return m_pRunwayInSim->GetLogicRunway1();
	
	if(runwayMark == RUNWAYMARK_SECOND)
		return m_pRunwayInSim->GetLogicRunway2();

	return NULL;

}

RunwayInSim * RunwaySegInSim::getRunway()
{
	return m_pRunwayInSim;
}