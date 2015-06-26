#include "StdAfx.h"
#include ".\runwayinsim.h"
#include "../../Common/ARCUnit.h"
#include "../../Common/Line2008.h"
#include "AirsideFlightInSim.h"
#include "AirsideResourceManager.h"
#include "Clearance.h"
#include "FlightPerformanceManager.h"
#include <cmath>
#include "AirTrafficController.h"
#include "RunwaySegInSim.h"
#include "../../InputAirside/WaveCrossingSpecification.h"
#include "RunwayCrossSpecificationInSim.h"
#include "TaxiwayResource.h"
#include "FlightGetClearanceEvent.h"
#include  "WakeUpRunwayEvent.h"
#include "../../Common/BizierCurve.h"
#include "FlightPerformancesInSim.h"
#include "AirsideCircuitFlightInSim.h"
#include <algorithm>

#include <limits>
//#include "../../Database/DBElementCollection_Impl.h"
#define _DEBUGLOG 0
#define _DEBUGWAVECROSSLOG 0
//////////////////////////////////////////////////////////////////////////
#ifdef _DEBUGLOG
#include <iostream>
#include <fstream>

class TakeoffQueueCountLog
{
public:
	
	TakeoffQueueCountLog()
	{
		std::ofstream strOut;
		strOut.open(strLogFile);
		if(!strOut.bad())
		{
			strOut<<"Queue Count Log "<<std::endl;
		}
		strOut.close();
	}
	static void Log(LogicRunwayInSim*pRunway,const ElapsedTime& tTime, AirsideFlightInSim* pFlight, bool bAdd )
	{
		std::ofstream strOut;
		strOut.open(strLogFile,std::ios::app);
		if(!strOut.bad())
		{
			strOut << tTime.printTime() << "(" << (long)tTime << ")" << ":";
			strOut << pRunway->PrintResource() << "," << pRunway->GetDepartureQueueLength();
			if(bAdd)
				strOut << "+";
			else
				strOut <<" -";
			strOut<< pFlight->GetCurrentFlightID()<<std::endl;	
		}	
		strOut.close();

	}
	static TakeoffQueueCountLog& GetInstance(){ return m_singleInstance ; }
protected:
	static TakeoffQueueCountLog m_singleInstance;
	static CString strLogFile;
};
CString TakeoffQueueCountLog::strLogFile =  _T("C:\\arportdebug\\queueCountLog.txt");
TakeoffQueueCountLog TakeoffQueueCountLog::m_singleInstance;
#endif
#include "Common\Range.h"
#include "TaxiwayConflictResolutionInEngine.h"
#include "InputAirside\ConflictResolution.h"
//////////////////////////////////////////////////////////////////////////

bool RunwayExitInSim::operator < (const RunwayExitInSim& rwExit)const
{
	return GetExitRunwayPos() < rwExit.GetExitRunwayPos();
}

int RunwayExitInSim::GetID() const
{
	return m_ExitInput.GetID();
}

int RunwayExitInSim::GetIntersectionID() const
{
	return m_ExitInput.GetIntersectNodeID();
}

double RunwayExitInSim::GetAngle() const
{
	return m_ExitInput.GetAngle();
}

DistanceUnit RunwayExitInSim::GetExitRunwayPos() const
{

	FilletTaxiway * pFillet = m_pRouteDirSeg->GetEntryNode()->GetFilletFromTo(m_pLogicRunway,m_pRouteDirSeg);
	int RunwayID = m_pLogicRunway->GetRunwayInSim()->GetRunwayInput()->getID();
	DistanceUnit nOffset = 0;
	if(pFillet)
	{
		if(RunwayID == pFillet->GetObject1ID() ){
			nOffset = abs( pFillet->GetFilletPoint1().GetDistToIntersect() + pFillet->GetFilletPoint1().GetUsrDist() ); 
		}
		else
		{
			nOffset = abs( pFillet->GetFilletPoint2().GetDistToIntersect() + pFillet->GetFilletPoint2().GetUsrDist() );
		}		
	}
	
	DistanceUnit dDistance = GetPosAtRunway();

	dDistance -= nOffset;

	return dDistance;
}


RunwayExitInSim::RunwayExitInSim( const RunwayExit & rwExitInput, FlightGroundRouteDirectSegInSim* pSeg , LogicRunwayInSim * pLogicRunway)
{
	m_ExitInput = rwExitInput;
	m_pRouteDirSeg = pSeg;
	m_pLogicRunway = pLogicRunway;
	m_vFailedExitFlights.clear();
}

DistanceUnit RunwayExitInSim::GetEnterTaxiwayPos() const
{
	DistanceUnit nOffset = 0;

	FilletTaxiway * pFillet = m_pRouteDirSeg->GetEntryNode()->GetFilletFromTo(m_pLogicRunway,m_pRouteDirSeg);
	int RunwayID = m_pLogicRunway->GetRunwayInSim()->GetRunwayInput()->getID();
	if(pFillet)
	{
		if(RunwayID == pFillet->GetObject1ID()){
			nOffset = abs( pFillet->GetFilletPoint2().GetDistToIntersect() + pFillet->GetFilletPoint2().GetUsrDist() );
		}
		else
		{
			nOffset = abs( pFillet->GetFilletPoint1().GetDistToIntersect() + pFillet->GetFilletPoint1().GetUsrDist() ); 
		}		
	}

	return nOffset;
}

DistanceUnit RunwayExitInSim::GetEnterRunwayPos() const
{
	DistanceUnit nOffset = 0;

	FilletTaxiway * pFillet = m_pRouteDirSeg->GetEntryNode()->GetFilletFromTo(m_pRouteDirSeg->GetOppositeSegment(),m_pLogicRunway);
	int RunwayID = m_pLogicRunway->GetRunwayInSim()->GetRunwayInput()->getID();
	if(pFillet)
	{
		if(RunwayID == pFillet->GetObject1ID() ){
			nOffset = abs( pFillet->GetFilletPoint1().GetDistToIntersect() + pFillet->GetFilletPoint1().GetUsrDist() ); 
		}
		else
		{
			nOffset = abs( pFillet->GetFilletPoint2().GetDistToIntersect() + pFillet->GetFilletPoint2().GetUsrDist() );
		}		
	}

	DistanceUnit dDistance = 0;
	if(m_ExitInput.GetRunwayMark() == RUNWAYMARK_FIRST)
	{
		dDistance = m_ExitInput.GetIntesectionNode().GetDistance(m_ExitInput.GetRunwayID()) ;		
	}
	else
	{
		dDistance = m_pLogicRunway->GetEndDist() - m_ExitInput.GetIntesectionNode().GetDistance(m_ExitInput.GetRunwayID());		
	}

	return dDistance + nOffset;
}

bool RunwayExitInSim::IsLeft()const
{
	return m_ExitInput.GetSideType() == RunwayExit::EXIT_LEFT;
}

bool RunwayExitInSim::CanServeFlight(AirsideFlightInSim *pFlight)
{
	if(m_pRouteDirSeg != NULL)
	{
		return m_pRouteDirSeg->GetRouteSegInSim()->CanServeFlight(pFlight);
	}

	return false;
}

bool RunwayExitInSim::IsQueueWaitingForCrossOtherRunwayFull(AirsideFlightInSim* pFlight)
{
	//get latest flight
	
	//OccupancyInstance& ocyInstance = m_pRouteDirSeg->GetEntryNode()->GetLastOccupyInstance();
	//
	//
	//if(ocyInstance.IsValid())
	//{
	//	AirsideFlightInSim* pInExitFlight = ocyInstance.GetFlight();
	AirsideFlightInSim* pInExitFlight = m_pRouteDirSeg->GetEntryNode()->GetLockedFlight();
		if(pInExitFlight && pInExitFlight->IsInCrossRunwayQueue() )
		{
			return true;
		}
	//}
	
	/*AirsideFlightInSim**/ pInExitFlight = m_pRouteDirSeg->GetLastInResourceFlight();
	if(pInExitFlight && pInExitFlight->IsInCrossRunwayQueue() )//
	{
		DistanceUnit sep = pInExitFlight->GetPerformance()->getTaxiInboundSeparationDistInQ(pInExitFlight);
		DistanceUnit dEnterDist = GetEnterTaxiwayPos();
		DistanceUnit dLeftDist = pInExitFlight->GetDistInResource() - pInExitFlight->GetLength()*0.5 - dEnterDist;
		if(dLeftDist < sep + pFlight->GetLength()) 
		{
			pInExitFlight->GetWaitRunwayHold()->SetQueueFull(true);
			return true;
		}
		else
		{
			pFlight->EnterQueueToRunwayHold( pInExitFlight->GetWaitRunwayHold(),false );
			return false;
		}
	}
	
	return false;
}

RunwayInSim* RunwayExitInSim::GetQueueWaitingRunway()
{
	AirsideFlightInSim* pInExitFlight = m_pRouteDirSeg->GetLastInResourceFlight();
	if(pInExitFlight && pInExitFlight->GetWaitRunwayHold() )//
	{
		return  pInExitFlight->GetWaitRunwayHold()->GetRunwayInSim();
	}
	return NULL;
}

bool RunwayExitInSim::CanHoldFlight( AirsideFlightInSim* pFlight )
{
	if(m_pRouteDirSeg->IsForbidDirection())
		return false;

	if (!m_pRouteDirSeg->IsActiveFor(pFlight))
		return false;

	if(IsFlightsGoingToTakeoff())
		return false;

	std::vector<CAirsideMobileElement *> vInSegFlights = m_pRouteDirSeg->GetInResouceMobileElement();
	int nCount = vInSegFlights.size();

	if (nCount ==0)
		return true;

	DistanceUnit dLeftDist = abs(m_pRouteDirSeg->GetEntryHoldDist() - m_pRouteDirSeg->GetExitHoldDist());

	if(pFlight->GetLength() > dLeftDist)	//if segment length less than flight length, cannot hold flight
		return false;

	for (int i =0; i <nCount; i++)
	{
		AirsideFlightInSim* pInExitFlight = (AirsideFlightInSim*)vInSegFlights.at(i);
		if(pInExitFlight)
		{
			DistanceUnit sep = pInExitFlight->GetPerformance()->getTaxiInboundSeparationDistInQ(pInExitFlight);
			if (i == 0)
				dLeftDist = dLeftDist - (int)(pInExitFlight->GetLength()) - sep;
			else
				dLeftDist = dLeftDist - pInExitFlight->GetLength() - sep;
		}
	}
	if(dLeftDist < (int)pFlight->GetLength()) 
	{
		//notify queue is full
		
		for (int i =0; i <nCount; i++)
		{
			AirsideFlightInSim* pInExitFlight = (AirsideFlightInSim*)vInSegFlights.at(i);
			if(pInExitFlight)
			{
				if(pInExitFlight->IsInCrossRunwayQueue() && pInExitFlight->GetWaitRunwayHold() != NULL)
				{
					pInExitFlight->GetWaitRunwayHold()->SetQueueFull(true);
				}
			}
		}
		return false;
	}
	
	//if(dLeftDist < (int)pFlight->GetLength()*2.0) 
	//{
	//	//notify queue is full
	//	
	//	for (int i =0; i <nCount; i++)
	//	{
	//		AirsideFlightInSim* pInExitFlight = (AirsideFlightInSim*)vInSegFlights.at(i);
	//		if(pInExitFlight)
	//		{
	//			if(pInExitFlight->IsInCrossRunwayQueue() && pInExitFlight->GetWaitRunwayHold() != NULL)
	//			{
	//				pInExitFlight->GetWaitRunwayHold()->SetQueueFull(true);
	//			}
	//		}
	//	}		
	//}

	if (!m_vFailedExitFlights.empty())		//if the flight can enter exit segment, remove the record of fore failed flights
	{
		m_vFailedExitFlights.clear();
	}

	return true;
}

void RunwayExitInSim::AddFaildExitFlight(AirsideFlightInSim* pFlight)
{
	if(m_vFailedExitFlights.empty())
	{
		m_vFailedExitFlights.push_back(pFlight);
		return;
	}


	if (m_vFailedExitFlights.end() != std::find(m_vFailedExitFlights.begin(),m_vFailedExitFlights.end(),pFlight))
		return;

	m_vFailedExitFlights.push_back(pFlight);		
}

bool RunwayExitInSim::IsExistFailedExitFlight()
{
	if (m_vFailedExitFlights.empty())
		return false;

	return true;
}

DistanceUnit RunwayExitInSim::GetPosAtRunway() const
{
	DistanceUnit dDistance=0;
	if(m_ExitInput.GetRunwayMark() == RUNWAYMARK_FIRST)
	{
		dDistance = m_ExitInput.GetIntesectionNode().GetDistance(m_ExitInput.GetRunwayID()) ;		
	}
	else
	{
		dDistance = m_pLogicRunway->GetEndDist() - m_ExitInput.GetIntesectionNode().GetDistance(m_ExitInput.GetRunwayID());		
	}
	return dDistance;
}

CPoint2008 RunwayExitInSim::getPos() const
{
	if(m_pLogicRunway)
		return m_pLogicRunway->GetDistancePoint(GetPosAtRunway());
	return CPoint2008(0,0,0);
}

bool RunwayExitInSim::CheckRunwayExitFull( AirsideFlightInSim* pFlight )
{
	std::vector<CAirsideMobileElement *> vInSegFlights = m_pRouteDirSeg->GetInResouceMobileElement();
	int nCount = vInSegFlights.size();

	if (nCount ==0)
		return true;

	DistanceUnit dLeftDist = abs(m_pRouteDirSeg->GetEntryHoldDist() - m_pRouteDirSeg->GetExitHoldDist());

	if(pFlight->GetLength() > dLeftDist)	//if segment length less than flight length, cannot hold flight
		return false;

	for (int i =0; i <nCount; i++)
	{
		AirsideFlightInSim* pInExitFlight = (AirsideFlightInSim*)vInSegFlights.at(i);
		if(pInExitFlight)
		{
			DistanceUnit sep = pInExitFlight->GetPerformance()->getTaxiInboundSeparationDistInQ(pInExitFlight);
			if (i == 0)
				dLeftDist = dLeftDist - (int)(pInExitFlight->GetLength()) - sep;
			else
				dLeftDist = dLeftDist - pInExitFlight->GetLength() - sep;
		}
	}
	if(dLeftDist < (int)pFlight->GetLength()) 
	{
		//notify queue is full

		for (int i =0; i <nCount; i++)
		{
			AirsideFlightInSim* pInExitFlight = (AirsideFlightInSim*)vInSegFlights.at(i);
			if(pInExitFlight)
			{
				if(pInExitFlight->IsInCrossRunwayQueue() && pInExitFlight->GetWaitRunwayHold() != NULL)
				{
					pInExitFlight->GetWaitRunwayHold()->SetQueueFull(true);
				}
			}
		}
		return false;
	}
	return true;
}


static CPath2008 GenSmoothPath(const CPoint2008* _inPath,int nCount , DistanceUnit smoothLen)
{
	CPath2008 reslt;

	if(nCount<3)
	{
		CPath2008 oldPath;
		oldPath.init(nCount,_inPath);
		return oldPath;
	}
	const static int outPtsCnt = 6;


	std::vector<CPoint2008> _out;
	_out.reserve( (nCount-2)*outPtsCnt + 2);
	_out.push_back(_inPath[0]);


	CPoint2008 ctrPts[3];

	for(int i=1;i<(int)nCount-1;i++)
	{
		ctrPts[0] = _inPath[i-1];
		ctrPts[1] = _inPath[i];
		ctrPts[2] = _inPath[i+1];

		//get the two side point
		ARCVector3 v1 = ctrPts[0] - ctrPts[1];
		ARCVector3 v2 = ctrPts[2] - ctrPts[1];
		double minLen1,minLen2;
		minLen1 = smoothLen;//.45 * (ctrPts[1]-ctrPts[0]).length();
		minLen2	= smoothLen;//.45 * (ctrPts[2]-ctrPts[1]).length();
		if(v1.Length() * 0.45 <smoothLen) 
			minLen1= v1.Length() * 0.45;
		if(v2.Length() * 0.45 <smoothLen) 
			minLen2 = v2.Length() * 0.45;
		v1.SetLength(minLen1);
		v2.SetLength(minLen2);
		ctrPts[0] = ctrPts[1] + v1 ; 
		ctrPts[2] = ctrPts[1] + v2;
		std::vector<CPoint2008> ctrlPoints(ctrPts,ctrPts+3);
		std::vector<CPoint2008> output;
		BizierCurve::GenCurvePoints(ctrlPoints,output,outPtsCnt);
		for(int i = 0;i<outPtsCnt;i++)
		{
			_out.push_back(output[i]);
		}
	}

	_out.push_back( _inPath[nCount-1] );
	reslt.init(_out.size(), &_out[0]);
	return reslt;
}

bool RunwayExitInSim::getFilletPath(CPath2008& filletPath,RUNWAY_MARK port,DistanceUnit dDist /*= 0.0*/)const
{
	FlightGroundRouteDirectSegInSim* pRouteEnterRunway = m_pRouteDirSeg->GetOppositeSegment();
	IntersectionNodeInSim* pNode = pRouteEnterRunway->GetExitNode();
	if (pNode->GetID() != GetIntersectionID())
	{
		ASSERT(FALSE);
		pRouteEnterRunway = m_pRouteDirSeg;
	}

	LogicRunwayInSim * pLogicRunway =
		port == m_pLogicRunway->getLogicRunwayType() ? m_pLogicRunway->GetOtherPort(): m_pLogicRunway;
	FilletTaxiway* pFillet = pRouteEnterRunway->GetExitNode()->GetFilletFromTo(pRouteEnterRunway, pLogicRunway);
	if (!pFillet)
		return false;
	filletPath = pNode->GetFiletRoutePath( *pFillet, pRouteEnterRunway->GetObjectID());
	return true;
}

bool RunwayExitInSim::getPath(CPath2008& fullPath, RUNWAY_MARK  port, DistanceUnit dDist /*= 0.0*/) const
{
	do 
	{
		FlightGroundRouteDirectSegInSim* pRouteEnterRunway = m_pRouteDirSeg->GetOppositeSegment();
		IntersectionNodeInSim* pNode = pRouteEnterRunway->GetExitNode();
		if (pNode->GetID() != GetIntersectionID())
		{
			ASSERT(FALSE);
			pRouteEnterRunway = m_pRouteDirSeg;
		}

		LogicRunwayInSim * pLogicRunway =
			port == m_pLogicRunway->getLogicRunwayType() ? m_pLogicRunway : m_pLogicRunway->GetOtherPort();
		FilletTaxiway* pFillet = pRouteEnterRunway->GetExitNode()->GetFilletFromTo(pRouteEnterRunway, pLogicRunway);
		if (!pFillet)
			break;
		CPath2008 filletPath = pNode->GetFiletRoutePath( *pFillet, pRouteEnterRunway->GetObjectID());

		CPath2008 segPath = pRouteEnterRunway->GetPath().GetLeftPath(dDist);
		if (segPath.getCount() >= 2)
		{
			segPath[segPath.getCount() - 1] = filletPath[0]; // set segment segPath exit point to fillet's entry node
		}
		CPath2008 segSmoothPath = GenSmoothPath(segPath.getPointList(), segPath.getCount(), 1000);

		std::vector<CPoint2008> vPts;
		vPts.reserve(filletPath.getCount() + segSmoothPath.getCount());

		vPts.insert(vPts.end(),&segSmoothPath[0], &segSmoothPath[0]+segSmoothPath.getCount());
		vPts.insert(vPts.end(),&filletPath[0], &filletPath[0]+filletPath.getCount());
		fullPath.init(vPts.size(), &vPts[0]);
		return true;
	} while (false);

	return false;
}

bool RunwayExitInSim::IsFlightsGoingToTakeoff() const
{
	return !m_vRegtakeoffFlts.empty();
}

void RunwayExitInSim::RegTakeoffPos( AirsideFlightInSim* pFlt )
{
	m_vRegtakeoffFlts.insert(pFlt);
}	

void RunwayExitInSim::UnRegTakeoffPos( AirsideFlightInSim* pFlt )
{
	m_vRegtakeoffFlts.erase(pFlt);
}

QueueToTakeoffPos* RunwayExitInSim::getTakeoffQueue()
{
	return GetLogicRunway()->GetQueueList().GetAddQueue(this);
}

HoldPositionInSim* RunwayExitInSim::getHoldPosition()
{
	int nTaxiID = m_ExitInput.GetTaxiwayID();
	int nNodeID = m_ExitInput.GetIntersectNodeID();
	return GetLogicRunway()->GetRunwayInSim()->GetHoldPosition(nTaxiID,nNodeID);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

RunwayInSim::RunwayInSim(Runway* pRunway, double dMissApproachDist)
:FlightGroundRouteInSim(pRunway->GetWidth(), pRunway->getID(),pRunway->GetPath())
{
	m_RunwayInput = pRunway;
	m_dMissApproachDistance = dMissApproachDist;
	m_LogicRwy1 = new LogicRunwayInSim(this,RUNWAYMARK_FIRST);
	m_LogicRwy2 = new LogicRunwayInSim(this, RUNWAYMARK_SECOND);
	m_activePort =(RUNWAY_MARK) -1;
	m_pLockFlight = NULL;
	m_runwayState = RunwayState_Landing;
	m_pWakeUpRunwayEvent = NULL;
}

class IntersectionNodeInSimCompareDist
{
public:
	IntersectionNodeInSimCompareDist(int nTaxiID){ m_nTaxiId = nTaxiID; }
	bool operator()(IntersectionNodeInSim* pNode1, IntersectionNodeInSim* pNode2){
		return pNode1->GetNodeInput().GetDistance(m_nTaxiId) < pNode2->GetNodeInput().GetDistance(m_nTaxiId);
	}
protected:
	int m_nTaxiId;
};

const static DistanceUnit DefaultHoldOffset = 3200;

bool RunwayInSim::InitSegments( IntersectionNodeInSimList& NodeList,const HoldShortLinePropList& vHoldList )
{

	std::vector<IntersectionNodeInSim*> vNodesInSim;

	for(int i=0;i< (int)NodeList.GetNodeCount();i++)
	{
		IntersectionNodeInSim* pNodeInSim = NodeList.GetNodeByIndex(i);
		if( pNodeInSim->GetNodeInput().HasObject( GetRunwayID() ) )
		{
			vNodesInSim.push_back( pNodeInSim );
		}
	}

	std::sort(vNodesInSim.begin(),vNodesInSim.end(), IntersectionNodeInSimCompareDist(GetRunwayID()) );

	for(int i=0;i<(int)vNodesInSim.size() -1; i++)
	{
		IntersectionNodeInSim * pNode1 = vNodesInSim.at(i);
		IntersectionNodeInSim * pNode2 = vNodesInSim.at(i+1);

		DistanceUnit dOffset1 = 0;
		DistanceUnit doffset2 = 0;
		{			
			DistanceUnit dMin =0;DistanceUnit dMax = 0;
			GetMinMaxFilletDistToNode(pNode1, GetRunwayID() , dMin, dMax);
			dOffset1 = max(dMax, DefaultHoldOffset);
		}	


		{
			DistanceUnit dMin =0;DistanceUnit dMax = 0;
			GetMinMaxFilletDistToNode( pNode2 ,GetRunwayID(), dMin, dMax);
			doffset2 = min(dMin, -DefaultHoldOffset);

		}		

		for(int j=0;j<(int)vHoldList.size();j++)
		{
			HoldPositionInSim hold = vHoldList[j];
			if (GetRunwayID() == hold.GetRunwayInSim()->GetRunwayID())
			{
				if(hold.m_nNodeID == pNode1->GetID() && hold.m_dDistOffNode > 0 )
				{
					dOffset1 = hold.m_dDistOffNode;
				}

				if(hold.m_nNodeID == pNode2->GetID() && hold.m_dDistOffNode < 0 )
				{
					doffset2 = hold.m_dDistOffNode;
				}
			}
			
		}
		RunwaySegInSim * newSeg = new RunwaySegInSim(pNode1,pNode2,this);
		newSeg->SetHold1Offset(dOffset1);
		newSeg->SetHold2Offset(doffset2);
		//newSeg->InitLanes();
		m_vSegments.push_back(newSeg);
	}

	return true;

}

int RunwayInSim::GetRunwayID()
{
	return GetRunwayInput()->getID();
}

RunwaySegInSim * RunwayInSim::GetSegment( int idx )const
{
	ASSERT(idx<(int)m_vSegments.size()); 
	if(idx>=0 && idx< (int)m_vSegments.size())
		return m_vSegments.at(idx);
	return NULL;
}

void RunwayInSim::GetRunwayDirectSegmentList( int nIntersectNodeIDFrom, int nIntersectNodeIDTo, FlightGroundRouteDirectSegList& taxiwayDirectSegList )
{
	{//find positive dir segments
		FlightGroundRouteDirectSegList vFindreslt;
		bool bStartFlag = false;
		bool bEndFlag = false;
		for(int i=0;i<(int) m_vSegments.size();++i)
		{
			FlightGroundRouteDirectSegInSim* pPosSeg = m_vSegments[i]->GetPositiveDirSeg();

			if(pPosSeg->GetEntryNode()->GetID() == nIntersectNodeIDFrom)
			{
				bStartFlag = true;
			}		

			if(bStartFlag && !bEndFlag )
				vFindreslt.push_back(pPosSeg);

			if(pPosSeg->GetExitNode()->GetID() == nIntersectNodeIDTo && bStartFlag)
			{
				bEndFlag  = true;
			}

			if(bStartFlag  && bEndFlag && !vFindreslt.empty())
			{
				taxiwayDirectSegList.insert(taxiwayDirectSegList.end(), vFindreslt.begin(),vFindreslt.end());
				return ;
			}
		}
	}

	//	
	{//find positive dir segments
		FlightGroundRouteDirectSegList vFindreslt;
		bool bStartFlag = false;
		bool bEndFlag = false;
		std::vector<RunwaySegInSim*>::reverse_iterator revIter;
		for(revIter = m_vSegments.rbegin(); revIter!=m_vSegments.rend(); ++revIter)
		{
			FlightGroundRouteDirectSegInSim* pNegSeg = (*revIter)->GetNegativeDirSeg();
			if(pNegSeg->GetEntryNode()->GetID() == nIntersectNodeIDFrom)
			{
				bStartFlag = true;
			}		

			if(bStartFlag && !bEndFlag )
				vFindreslt.push_back(pNegSeg);

			if(pNegSeg->GetExitNode()->GetID() == nIntersectNodeIDTo && bStartFlag)
			{
				bEndFlag  = true;
			}

			if(bStartFlag  && bEndFlag && !vFindreslt.empty())
			{
				taxiwayDirectSegList.insert(taxiwayDirectSegList.end(), vFindreslt.begin(),vFindreslt.end());
				return ;
			}
		}
	}
}

void RunwayInSim::SetWaveCrossSpecification( CWaveCrossingSpecificationItem* pWaveCrossSettingItem )
{
	//just check it, remind 
	ASSERT(pWaveCrossSettingItem != NULL);
	if(pWaveCrossSettingItem == NULL)
		return;
   
	CRunwayCrossSpecificationInSim *pRunwayCross = new CRunwayCrossSpecificationInSim(this);

	for (int nHold = 0;nHold < pWaveCrossSettingItem->GetHoldCount();++nHold)
	{
		HoldPosition *pHoldPosition = pWaveCrossSettingItem->GetHoldPosition(nHold);
		if(pHoldPosition != NULL)
		{
			HoldPositionInSim *pHoldPositionInSim = GetHoldPosition(pHoldPosition->GetUniqueID());
			if(pHoldPositionInSim)
			{
				pRunwayCross->AddHoldPositionInSim(pHoldPositionInSim);
			}
		}
	}
	pRunwayCross->SetWaveCrossSpecificationItem(pWaveCrossSettingItem);
	m_vWaveCrossSettings.push_back(pRunwayCross);
}

CRunwayCrossSpecificationInSim* RunwayInSim::GetWaveCrossSpecification()
{
	if(m_vWaveCrossSettings.size() > 0)
		return m_vWaveCrossSettings.at(0);

	return NULL;
}

void RunwayInSim::AddTaxiwayHoldPosition( HoldPositionInSim *pHoldPosition )
{
	m_vTaxiwayHodPostions.push_back(pHoldPosition);
}

HoldPositionInSim* RunwayInSim::GetHoldPosition( int nHoldPosID )const
{
	std::vector<HoldPositionInSim *>::const_iterator iter = m_vTaxiwayHodPostions.begin();
	for (;iter != m_vTaxiwayHodPostions.end(); ++iter)
	{
		if(((*iter) != NULL) &&(*iter)->m_nID == nHoldPosID)
		{
			return *iter;
		}
	}
	return NULL;
}

HoldPositionInSim* RunwayInSim::GetHoldPosition( int nTaxiID, int nNodeID ) const
{
	std::vector<HoldPositionInSim *>::const_iterator iter = m_vTaxiwayHodPostions.begin();
	for (;iter != m_vTaxiwayHodPostions.end(); ++iter)
	{
		if(((*iter) != NULL) && (*iter)->GetNodeID() == nNodeID && (*iter)->GetTaxiwayID()== nTaxiID )
		{
			return *iter;
		}
	}
	return NULL;
}
//remove the occupancy information of this runway
//include logic runway 1,logic runway 2, runway's intersections
void RunwayInSim::RemoverRunwayOccupancyInfo( AirsideFlightInSim *pFlight )
{
	ASSERT(pFlight != NULL);
	if(pFlight == NULL)
		return;

	//clear information on logic runway 1
	LogicRunwayInSim* pLogicRunway = GetLogicRunway1();
	if(pLogicRunway)
	{
		pLogicRunway->RemoveElementOccupancyInfo(pFlight);
		IntersectionNodeInSimList& IntNodeList = pLogicRunway->GetIntersectionList();
		int nNodeCount = IntNodeList.GetNodeCount();
		for (int nNode = 0; nNode < nNodeCount; ++nNode)
		{
			IntersectionNodeInSim *pNode = IntNodeList.GetNodeByIndex(nNode);
			if(pNode)
			{
				pNode->RemoveElementOccupancyInfo(pFlight);
			}
		}
	}
	//clear logic runway 2
	LogicRunwayInSim *pLogicRunway2 = GetLogicRunway2();
	if(pLogicRunway2)
		pLogicRunway2->RemoveElementOccupancyInfo(pFlight);
	//for logic runway 2 and logic runway 1 share the intersection node, 
	//so, it is not necessary to remove the information of nodes again


}

void RunwayInSim::CheckWaveCross(AirsideFlightInSim *pFlight,const ElapsedTime& eTime)
{
	std::vector<CRunwayCrossSpecificationInSim*>::iterator iter = m_vWaveCrossSettings.begin();
	for (;iter != m_vWaveCrossSettings.end(); ++iter)
	{
		if(*iter)
		{
			if((*iter)->CheckNeedWaveCross(pFlight,eTime))
				break;
		}
	}
}

RunwayInSim::~RunwayInSim()
{
	std::vector<CRunwayCrossSpecificationInSim*>::iterator iter = m_vWaveCrossSettings.begin();
	for (;iter != m_vWaveCrossSettings.end();++iter)
	{
		delete *iter;
	}
	m_vWaveCrossSettings.clear();
}

std::vector<AirsideFlightInSim *> RunwayInSim::GetNextFlightAfter( AirsideFlightInSim *pCurFlight )
{
	std::vector<AirsideFlightInSim *> vFlights;
	if(pCurFlight == NULL)
		return vFlights;

	LogicRunwayInSim* pActiveLogicRunway = NULL;
	if(GetLogicRunway1() != NULL && GetActivePort() == GetLogicRunway1()->getLogicRunwayType())
		pActiveLogicRunway = GetLogicRunway1();
	else
		pActiveLogicRunway = GetLogicRunway2();

	if(pActiveLogicRunway == NULL)
		return vFlights;

	return pActiveLogicRunway->GetFlightsAfterFlight(pCurFlight);

}

void RunwayInSim::SetRunwayState( RunwayState runwayState )
{
	m_runwayState = runwayState;
}

void RunwayInSim::AddWaitingFlight( AirsideFlightInSim *pFlight )
{
	ASSERT(pFlight != NULL);
	if(pFlight != NULL)
	{
		if(std::find(m_vWaitingFlight.begin(),m_vWaitingFlight.end(),pFlight) == m_vWaitingFlight.end())//not find in list
			m_vWaitingFlight.push_back(pFlight);
	}
}

void RunwayInSim::WakeUpWaitingFlights(const ElapsedTime& eTime )
{	
	SetRunwayState(RunwayState_Landing);

	if(m_vWaitingFlight.size() > 0)
	{
		if(m_vWaitingFlight[0]->GetMode() < OnTaxiToRunway || m_vWaitingFlight[0]->GetMode() == OnWaitInHold)
		{
			SetRunwayState(RunwayState_Landing);
		}
		else
		{
			SetRunwayState(RunwayState_TakeOff);
		}
	}


	std::vector<AirsideFlightInSim *>::iterator iter = m_vWaitingFlight.begin();
	for (;iter != m_vWaitingFlight.end(); ++iter)
	{
		FlightGetClearanceEvent *pEvent = new FlightGetClearanceEvent(*iter);
		pEvent->setTime(eTime);
		pEvent->addEvent();
	}
	m_vWaitingFlight.clear();
	
}

//void RunwayInSim::AddWaveCrossFlight( AirsideFlightInSim *pFlight )
//{
//	ASSERT(pFlight != NULL);
//	if(pFlight != NULL)
//	{
//		if(std::find(m_vWaveCrossFlight.begin(),m_vWaveCrossFlight.end(),pFlight) == m_vWaveCrossFlight.end())//not find in list
//			m_vWaveCrossFlight.push_back(pFlight);
//	}
//}

void RunwayInSim::RemoveWaveCrossFlight( AirsideFlightInSim *pFlight )
{
	if(pFlight == NULL)
		return;

	if(m_vWaveCrossHold.size() == 0)
		return;

	std::vector<HoldPositionInSim *>::iterator iterFind = 
		std::find(m_vWaveCrossHold.begin(),m_vWaveCrossHold.end(),pFlight->m_pCrossRunwayHold);
	
	if(iterFind != m_vWaveCrossHold.end())
	{
		(*iterFind)->DecreaseWaveCrossFlight();
	}

	std::vector<HoldPositionInSim *>::iterator iter = m_vWaveCrossHold.begin();
	for(; iter != m_vWaveCrossHold.end(); ++iter)
	{
		if(!(*iter)->HasFinishWaveCross())
			return;
	}


	WakeUpWaitingFlights(pFlight->GetTime());

	//all the waiting flight has been passed , so does not need to wake up runway any more
	if(m_pWakeUpRunwayEvent != NULL)
		m_pWakeUpRunwayEvent->SetInvalid(false);
	


	//std::vector<AirsideFlightInSim *>::iterator iterFind = 
	//	std::find(m_vWaveCrossFlight.begin(),m_vWaveCrossFlight.end(),pFlight);
	//
	//if(iterFind != m_vWaveCrossFlight.end())
	//	m_vWaveCrossFlight.erase(iterFind);


	//if(m_vWaveCrossFlight.size() == 0)
	//{
	//	WakeUpWaitingFlights(pFlight->GetTime());

	//	//all the waiting flight has been passed , so does not need to wake up runway any more
	//	if(m_pWakeUpRunwayEvent != NULL)
	//		m_pWakeUpRunwayEvent->SetInvalid(false);
	//}

}


void RunwayInSim::WakeUpRunwayLanding( const ElapsedTime& eEventTime )
{
	//SetRunwayState(RunwayState_Landing);
	
	//clear cross runway flight
	m_vWaveCrossHold.clear();
	//wakeup waiting runway flights
	WakeUpWaitingFlights(eEventTime);
}

bool RunwayInSim::bCanFlightCross( AirsideFlightInSim* pFlight,const ElapsedTime& eTime ) const
{
	if(m_runwayState == RunwayState_Landing)
	{
		if(eTime < m_eLandingSafeTime)
		{
#ifdef _DEBUGWAVECROSSLOG

			CString strFlightUID;
			if(pFlight != NULL)
				strFlightUID.Format(_T("%d  "),pFlight->GetUID());
			else
				strFlightUID.Format(_T("Have no next flight "));

			ofsstream echoFile ("d:\\wavecross.log", stdios::app);
			echoFile<<"bCanFlightCross --- landing: "
				<<"flight id: "<<strFlightUID
				<<"  flight  time: "<<pFlight->GetTime().printTime()
				<<"  pass time "<<eTime.printTime()
				<<"  safe  time: "<<m_eLandingSafeTime.printTime()<<"\n";
			echoFile.close();
#endif
			return true;
		}
	}
	std::vector<HoldPositionInSim *>::const_iterator consIter =
		std::find(m_vWaveCrossHold.begin(),m_vWaveCrossHold.end(), pFlight->m_pCrossRunwayHold);
	if(consIter != m_vWaveCrossHold.end())
	{
#ifdef _DEBUGWAVECROSSLOG

		CString strFlightUID;
		if(pFlight != NULL)
			strFlightUID.Format(_T("%d  "),pFlight->GetUID());
		else
			strFlightUID.Format(_T("Have no next flight "));

		ofsstream echoFile ("d:\\wavecross.log", stdios::app);
		echoFile<<"bCanFlightCross --- take off: "
			<<" flight id: "<<strFlightUID
			<<"  flight  time: "<<pFlight->GetTime().printTime()
			<<"  pass time "<<eTime.printTime()<<"\n";
		echoFile.close();
#endif
		//need wave cross
		if(!(*consIter)->HasFinishWaveCross())
			return true;
	}


//	std::vector<AirsideFlightInSim *>::const_iterator consIter =
//		std::find(m_vWaveCrossFlight.begin(),m_vWaveCrossFlight.end(), pFlight);
//	if(consIter != m_vWaveCrossFlight.end())
//	{
//#ifdef _DEBUGWAVECROSSLOG
//
//		CString strFlightUID;
//		if(pFlight != NULL)
//			strFlightUID.Format(_T("%d  "),pFlight->GetUID());
//		else
//			strFlightUID.Format(_T("Have no next flight "));
//
//		ofsstream echoFile ("d:\\wavecross.log", stdios::app);
//		echoFile<<"bCanFlightCross --- take off: "
//			<<" flight id: "<<strFlightUID
//			<<"  flight  time: "<<pFlight->GetTime().printTime()
//			<<"  pass time "<<eTime.printTime()<<"\n";
//		echoFile.close();
//#endif
//
//
//		return true;
//	}

	return false;
}

void RunwayInSim::NotifyWaveCross( AirsideFlightInSim *pFlight,const ElapsedTime& eTime )
{
	std::vector<CRunwayCrossSpecificationInSim*>::iterator iter = m_vWaveCrossSettings.begin();
	for (;iter != m_vWaveCrossSettings.end(); ++iter)
	{
		if(*iter)
		{
			if((*iter)->NotifyWaveCross(pFlight,eTime))
				break;
		}
	}
}

RunwayInSim::RunwayState RunwayInSim::GetRunwayState() const
{
	return m_runwayState;
}

void RunwayInSim::SetWakeupRunwayEvent( CWakeUpRunwayEvent *pWakeUpRunwayEvent )
{
	if(m_pWakeUpRunwayEvent)
		m_pWakeUpRunwayEvent->SetInvalid(false);

	m_pWakeUpRunwayEvent = pWakeUpRunwayEvent;
}

CWakeUpRunwayEvent * RunwayInSim::GetWakeupRunwayEvent()
{
	return m_pWakeUpRunwayEvent;
}

void RunwayInSim::AddWaveCrossHold( HoldPositionInSim *pHold, int nWaveCrossCount )
{
	if(pHold)
	{
		pHold->SetWaveCrossFlightCount(nWaveCrossCount);
		m_vWaveCrossHold.push_back(pHold);
	}
}

IntersectionNodeInSimList RunwayInSim::GetIntersectionNodeList() const
{
	IntersectionNodeInSimList vNodeList;
	int nCount = GetSegmentCount();
	for(int i=0;i<nCount;i++)
	{
		if (i == 0)
		{
			vNodeList.Add(GetSegment(i)->GetNode1());
		}
		vNodeList.Add(GetSegment(i)->GetNode2());
	}
	return vNodeList;
}



//initialization
LogicRunwayInSim::LogicRunwayInSim( RunwayInSim * pRunway, RUNWAY_MARK porttype )
{
	m_pRunway = pRunway;
	m_port = porttype;	
	m_bHasMissedApproachFlight = false;
}

CPoint2008 LogicRunwayInSim::GetDistancePoint( double dist ) const
{
	if(m_port == RUNWAYMARK_FIRST)
	{ 
		return m_pRunway->GetRunwayInput()->GetPath().GetDistPoint(dist);
	}
	else{
		double negdist = m_pRunway->GetRunwayInput()->GetPath().GetTotalLength() - dist;	
		return m_pRunway->GetRunwayInput()->GetPath().GetDistPoint(negdist);
	}
	
}

double LogicRunwayInSim::GetPointDist( const CPoint2008& pt ) const
{
	const CPath2008& rwPath = m_pRunway->GetRunwayInput()->getPath();	
	double reslt = rwPath.GetPointDist(pt);	

	if(m_port == RUNWAYMARK_FIRST)
	{
		return reslt;
	}else 
		return rwPath.GetTotalLength() - reslt;
}

double LogicRunwayInSim::GetEndDist() const
{
	return m_pRunway->GetRunwayInput()->GetPath().GetTotalLength();
}

LogicRunwayInSim * LogicRunwayInSim::GetOtherPort()const
{
	if(m_port == RUNWAYMARK_FIRST) return GetRunwayInSim()->GetLogicRunway2();
	else return GetRunwayInSim()->GetLogicRunway1();
}

void LogicRunwayInSim::SetEnterTime(CAirsideMobileElement * pFlight, const ElapsedTime& enterT, AirsideMobileElementMode fltMode, double dSpd)
{
	if(pFlight->IsKindof(typeof(AirsideFlightInSim)))
	{
		AirsideResource::SetEnterTime(pFlight,enterT,fltMode,dSpd);		
	}
		//for(int i=0;i<m_vIntersectionWithRunways.GetNodeCount();i++)
	//{
	//	m_vIntersectionWithRunways.GetNodeByIndex(i)->SetEnterTime(pFlight,enterT,fltMode);
	//}
}

void LogicRunwayInSim::SetExitTime( CAirsideMobileElement * pFlight, const ElapsedTime& exitT )
{
	if(pFlight->IsKindof(typeof(AirsideFlightInSim)))
	{
		AirsideResource::SetExitTime(pFlight,exitT);		
	}
	//for(int i=0;i<m_vIntersectionWithRunways.GetNodeCount();i++)
	//{
	//	m_vIntersectionWithRunways.GetNodeByIndex(i)->SetExitTime(pFlight,exitT);
	//}
}

RunwayExitInSim * LogicRunwayInSim::GetExitByID( int Id )
{
	for(int i=0;i< GetExitCount();i++)
	{
		RunwayExitInSim * pExit = GetExitAt(i);
		if(pExit->GetID() == Id)
			return pExit;
	}
	return NULL;
}

RunwayExitInSim* LogicRunwayInSim::GetExitByRunwayExitDescription(const RunwayExitDescription& runwayDes)
{
	for(int i=0;i< GetExitCount();i++)
	{
		RunwayExitInSim * pExit = GetExitAt(i);
		if(pExit->GetExitInput().EqualToRunwayExitDescription(runwayDes))
			return pExit;
	}
	return NULL;
}

ARCVector3 LogicRunwayInSim::GetDirection() const
{
	return GetRunwayInSim()->GetRunwayInput()->GetDir(m_port);
}


bool LogicRunwayInSim::FindClearanceInConcern( AirsideFlightInSim * pFlight,ClearanceItem& lastItem ,DistanceUnit radius, Clearance& newClearance )
{	
	
	if(lastItem.GetMode() < OnLanding ) //assign landing clearance
	{
		
		////find the flight at the runway
		AirsideFlightInSim *pLeadFlight = NULL;		
		{
			pLeadFlight = GetLastInResourceFlight();
			if(!pLeadFlight)
				pLeadFlight = GetOtherPort()->GetLastInResourceFlight();
		}

		if(pLeadFlight) // wait for lead flight
		{
			if(pLeadFlight->GetTime() >= lastItem.GetTime() ) //delay
			{
				lastItem.SetDelayTime( pLeadFlight->GetTime() - lastItem.GetTime());
				lastItem.SetTime(pLeadFlight->GetTime());
				newClearance.AddItem(lastItem);
				
			}
			return true;
		}
		else   //assign landing clearance
		{
			double landspd = pFlight->GetPerformance()->getLandingSpeed(pFlight);	
			
			//landing item
			DistanceUnit touchDist = pFlight->GetTouchDownDistAtRuwnay(this);//pFlight->GetPerformance()->getDistThresToTouch(pFlight);
			ClearanceItem landingItem(this,OnLanding,touchDist);		
			landingItem.SetSpeed(landspd);
			ElapsedTime dT = pFlight->GetTimeBetween(lastItem,landingItem);
			ElapsedTime nextTime = lastItem.GetTime() + dT;		

			OccupancyInstance preFlightOcy = GetLastOccupyInstance();		
			if(preFlightOcy.IsValid() && preFlightOcy.IsEnterTimeValid())
			{
				AirsideFlightInSim * preFlight = preFlightOcy.GetFlight();
				AirsideMobileElementMode preFlightMode = (AirsideMobileElementMode)preFlightOcy.GetOccupyType();
				ElapsedTime separationTime = pFlight->GetAirTrafficController()->GetSeparationTime(preFlight,preFlightMode,pFlight,lastItem.GetMode());	
				
				ElapsedTime preFlightExitTime = preFlightOcy.GetEnterTime();
				if(nextTime - preFlightExitTime < separationTime ) //delay
				{
					landingItem.SetDelayTime( separationTime + preFlightExitTime - nextTime );
					nextTime = separationTime + preFlightExitTime;
				}
			}
			
			landingItem.SetTime(nextTime);
			lastItem = landingItem;
			newClearance.AddItem(lastItem);
			return true;		
			
		}
	}
	
	
	if(lastItem.GetMode() == OnLanding )  //exit runway clearance
	{		
		RunwayExitInSim  * pRunwayExit  = pFlight->GetRunwayExit();
		if(pRunwayExit)
		{			
			double eixtspd = pFlight->GetPerformance()->getExitSpeed(pFlight);
			double exitPos = pRunwayExit->GetExitRunwayPos();
			
			if(lastItem.GetDistInResource() < exitPos )
			{
				ClearanceItem atExitItem(this,OnLanding,pRunwayExit->GetExitRunwayPos());
				atExitItem.SetSpeed(eixtspd);

				ElapsedTime dT = pFlight->GetTimeBetween(lastItem,atExitItem);
				atExitItem.SetTime(dT + lastItem.GetTime());
				lastItem = atExitItem;
				newClearance.AddItem( lastItem );
				return true;
			}			

			//exit Runway to taxiway
			FlightGroundRouteDirectSegInSim * pDirSeg = pRunwayExit->GetRouteSeg();
			ASSERT(pDirSeg);

			ClearanceItem AtTaxiwayItem(pDirSeg,OnExitRunway, pRunwayExit->GetEnterTaxiwayPos());
			AtTaxiwayItem.SetSpeed(lastItem.GetSpeed());
			ElapsedTime dT = pFlight->GetTimeBetween(lastItem,AtTaxiwayItem);
			ElapsedTime nextTime = lastItem.GetTime() + dT;
			//add time delay here			
			AtTaxiwayItem.SetTime(nextTime);
			newClearance.AddItem(AtTaxiwayItem);

			return true;
		}			
	}
	

	if(lastItem.GetMode() < OnPreTakeoff )  //pre takeoff clearance
	{		
		
		{//enter runway test
			ElapsedTime preTakeoffValidTime = GetNearestPositionTakeoffTime(pFlight);
			if( lastItem.GetTime() < preTakeoffValidTime )
			{
				ElapsedTime delayT = preTakeoffValidTime - lastItem.GetTime();
				lastItem.SetDelayTime(delayT);
				//add a delay item 
				lastItem.SetTime(preTakeoffValidTime);					
				newClearance.AddItem(lastItem);			
				return true;
			}				
		}
		//go to take off position
		RunwayExitInSim * pTakeoffPos = pFlight->GetAndAssignTakeoffPosition();		
		DistanceUnit takeoffPos = 0; 
		if(pTakeoffPos){
			takeoffPos = pTakeoffPos->GetEnterRunwayPos();
		}	
		ClearanceItem preTakeoffItem(this,OnPreTakeoff,takeoffPos);
		preTakeoffItem.SetSpeed(lastItem.GetSpeed());
		ElapsedTime dT = pFlight->GetTimeBetween(lastItem,preTakeoffItem);
		//add pre takeoff item
		preTakeoffItem.SetTime(lastItem.GetTime() +dT);
		lastItem = preTakeoffItem;	
		newClearance.AddItem(lastItem);

		////add at take off pos item
		ClearanceItem atPosItem = lastItem;
		atPosItem.SetDistInResource(lastItem.GetDistInResource() + 1);
		dT = pFlight->GetTimeBetween(lastItem,atPosItem);
		atPosItem.SetTime(lastItem.GetTime() + dT);
		lastItem = atPosItem;
		newClearance.AddItem(lastItem);

		//takeoff position time
		double dPositionTime = pFlight->GetTakeoffPositionTime();
		ClearanceItem positionTimeItem = lastItem;
		ElapsedTime estPositionTime(dPositionTime);
		positionTimeItem.SetTime(lastItem.GetTime() + estPositionTime);
		positionTimeItem.SetSpeed(0.0);
		lastItem = positionTimeItem;
		newClearance.AddItem(positionTimeItem);

		//take off item
		{
			ClearanceItem takeoffItem = lastItem;
			takeoffItem.SetMode(OnTakeoff);
			ElapsedTime dT = pFlight->GetTimeBetween(lastItem,takeoffItem);
			takeoffItem.SetTime(lastItem.GetTime() + dT);
			lastItem = takeoffItem;
			newClearance.AddItem(lastItem);
		}

		// roll item
		double dTakeoffRoll = ARCUnit::ConvertLength(pFlight->GetTakeoffRoll(), ARCUnit::M, ARCUnit::CM);
		double dLiftOffSpeed = ARCUnit::ConvertVelocity(pFlight->GetLiftoffSpeed(), ARCUnit::KNOT, ARCUnit::CMpS);
		{
			ClearanceItem rollitem = lastItem;
			rollitem.SetDistInResource( rollitem.GetDistInResource()+ dTakeoffRoll );
			rollitem.SetSpeed(dLiftOffSpeed);
			ElapsedTime dT = pFlight->GetTimeBetween(lastItem,rollitem);
			rollitem.SetTime(rollitem.GetTime()+ dT);
			rollitem.SetMode(OnTakeoff);
			lastItem = rollitem;
			newClearance.AddItem(lastItem);
		}

		return true;
	}
	//assign takeoff clearance
	if(lastItem.GetMode() == OnPreTakeoff) 
	{		
		//take off condition test		
		ElapsedTime takeoffValidTime = GetNearestTakeoffTime(pFlight);
		if(lastItem.GetTime() < takeoffValidTime)
		{
			ElapsedTime delayT = takeoffValidTime - lastItem.GetTime();
			lastItem.SetDelayTime(delayT);
			//add a delay item 
			lastItem.SetTime(takeoffValidTime);					
			newClearance.AddItem(lastItem);			
			return true;
		}

		//take off item
		{
			ClearanceItem takeoffItem = lastItem;
			takeoffItem.SetMode(OnTakeoff);
			ElapsedTime dT = pFlight->GetTimeBetween(lastItem,takeoffItem);
			takeoffItem.SetTime(lastItem.GetTime() + dT);
			lastItem = takeoffItem;
			newClearance.AddItem(lastItem);
		}

		
		//double takeoffSpd = pFlight->GetPerformance()->getClimboutSpeed(pFlight);
		//double takeoffacce = pFlight->GetPerformance()->getTakeoffAcceleration(pFlight);
		//double dT = (takeoffSpd/takeoffacce);
		//DistanceUnit takeoffDist = dT * (takeoffSpd) *0.5;

		// roll item
		double dTakeoffRoll = ARCUnit::ConvertLength(pFlight->GetTakeoffRoll(), ARCUnit::M, ARCUnit::CM);
		double dLiftOffSpeed = ARCUnit::ConvertVelocity(pFlight->GetLiftoffSpeed(), ARCUnit::KNOT, ARCUnit::CMpS);
		{
			ClearanceItem rollitem = lastItem;
			rollitem.SetDistInResource(rollitem.GetDistInResource()+ dTakeoffRoll);
			rollitem.SetSpeed(dLiftOffSpeed);
			ElapsedTime dT = pFlight->GetTimeBetween(lastItem,rollitem);
			rollitem.SetTime(rollitem.GetTime()+ dT);
			rollitem.SetMode(OnTakeoff);
			lastItem = rollitem;
			newClearance.AddItem(lastItem);
		}
		return true;
	}	
	return false;
}

void LogicRunwayInSim::ReleaseLock(AirsideFlightInSim * pFlight,const ElapsedTime& tTime)
{
	if(GetLockedFlight() == pFlight){
		m_pRunway->SetLockFlight(NULL);
		RemoveLeftFlight(pFlight);
	}
	m_releaseTime = tTime;

}

bool LogicRunwayInSim::GetLock( AirsideFlightInSim * pFlight )
{
	ASSERT(pFlight);
	if(GetLockedFlight() && GetLockedFlight() != pFlight)
	{ return false; }
	else{ 
		m_pRunway->SetActivePort(pFlight,m_port);
		return true;
	}
}

AirsideFlightInSim * LogicRunwayInSim::GetLockedFlight()
{
	return m_pRunway->GetLockFlight();
}

bool LogicRunwayInSim::TryLock( AirsideFlightInSim * pFlight )
{
	if(GetLockedFlight() && GetLockedFlight() != pFlight)
	{ return false; }
	return true;
}

const ElapsedTime LogicRunwayInSim::GetLastReleaseTime() const
{
	ElapsedTime sameRwT;
	ElapsedTime otherRwT;
	{
		OccupancyInstance lastOcy = GetLastOccupyInstance();
		if(lastOcy.IsValid() && lastOcy.IsExitTimeValid())
		{
			sameRwT = lastOcy.GetExitTime();
		}
	}
	{
		OccupancyInstance lastOcy = GetOtherPort()->GetLastOccupyInstance();
		if(lastOcy.IsValid() && lastOcy.IsExitTimeValid())
		{
			otherRwT = lastOcy.GetExitTime();
		}
	}
	return max(sameRwT,otherRwT);	 
}

CString LogicRunwayInSim::PrintResource() const
{
	if(m_port == RUNWAYMARK_FIRST) 
		return m_pRunway->GetRunwayInput()->GetMarking1().c_str();
	else 
		return m_pRunway->GetRunwayInput()->GetMarking2().c_str();
}

ElapsedTime LogicRunwayInSim::GetNearestLandingTime( AirsideFlightInSim *pFlight )	const
{
	//char ACType[256];
	//char otherACType[256];
	//pFlight->getACType(ACType);
	//AircraftSeparationManager * pSeparationMan = pFlight->GetAirTrafficController()->GetSeparationManager();

	ElapsedTime landingValidTime;
	//{//check the same runway			
	//	OccupancyInstance preFlightOcy = GetLastOccupyInstance();			
	//	if(preFlightOcy.IsValid() && preFlightOcy.IsEnterTimeValid())
	//	{			
	//		preFlightOcy.GetFlight()->getACType(otherACType);
	//		ElapsedTime separationTime;
	//		if(preFlightOcy.GetOccupyType() == OnPreTakeoff )
	//		{				
	//			separationTime = pSeparationMan->GetLandingBehindTakeoffSepTime(ACType,otherACType);
	//		}
	//		if(preFlightOcy.GetOccupyType() == OnLanding )
	//		{
	//			separationTime = pSeparationMan->GetLandingBehindLandingSepTime(ACType,otherACType);
	//		}
	//		landingValidTime = max(landingValidTime, separationTime + preFlightOcy.GetEnterTime() );
	//	}
	//}			
	//{//check the other port
	//	LogicRunwayInSim * pOtherPort = GetOtherPort();
	//	OccupancyInstance preFlightOcy = pOtherPort->GetLastOccupyInstance();
	//	if(preFlightOcy.IsValid() && preFlightOcy.IsEnterTimeValid())
	//	{
	//		preFlightOcy.GetFlight()->getACType(otherACType);
	//		ElapsedTime separationTime;			
	//		if(preFlightOcy.GetOccupyType() == OnPreTakeoff )
	//		{				
	//			separationTime = pSeparationMan->GetLandingBehindTakeoffSepTime(ACType,otherACType, false);
	//		}
	//		if(preFlightOcy.GetOccupyType() == OnLanding )
	//		{
	//			separationTime = pSeparationMan->GetLandingBehindLandingSepTime(ACType,otherACType, false);
	//		}
	//		landingValidTime = max(landingValidTime, separationTime + preFlightOcy.GetEnterTime() );
	//	}
	//}
	////check other intersect runway
	//for(int i = 0; i< GetIntersectRunwayCount();i ++)
	//{
	//	LogicRunwayInSim * pIntersectRunway = GetIntersectRunway(i);
	//	ASSERT(pIntersectRunway);
	//	OccupancyInstance preFlightOcy = pIntersectRunway->GetLastOccupyInstance();
	//	if(preFlightOcy.IsValid() && preFlightOcy.IsEnterTimeValid())
	//	{
	//		ElapsedTime separationTime;
	//		if(preFlightOcy.GetOccupyType() == OnPreTakeoff )
	//		{				
	//			separationTime = pSeparationMan->GetLandingBehindTakeoffSepTime(ACType,otherACType,false);
	//		}
	//		if(preFlightOcy.GetOccupyType() == OnLanding )
	//		{
	//			separationTime = pSeparationMan->GetLandingBehindLandingSepTime(ACType,otherACType,false);
	//		}
	//		landingValidTime = max(landingValidTime, separationTime + preFlightOcy.GetEnterTime() );
	//	}
	//}
	return landingValidTime;
}

ElapsedTime LogicRunwayInSim::GetNearestPositionTakeoffTime( AirsideFlightInSim * pFlight ) const
{	
	
	//AircraftSeparationManager * pSeparationMan = pFlight->GetAirTrafficController()->GetSeparationManager();
	
	ElapsedTime posTakeoffTime;
	//OccupancyInstance preFlightOcy = GetLastOccupyInstance();				
	//if(preFlightOcy.IsValid() )
	//{				
	//	AirsideFlightInSim * preFlight = preFlightOcy.GetFlight();
	//	
	//	if(preFlightOcy.IsExitTimeValid())
	//	{
	//		ElapsedTime separationTime;		
	//		if(preFlightOcy.GetOccupyType() == OnPreTakeoff )
	//		{				
	//			separationTime = pSeparationMan->GetPosTakeoffBehindTakeoffSeparationTime();
	//		}
	//		if(preFlightOcy.GetOccupyType() == OnLanding )
	//		{
	//			separationTime = pSeparationMan->GetPosTakeoffBehindLandingSeparationTime();
	//		}

	//		posTakeoffTime = separationTime + preFlightOcy.GetExitTime();
	//	}
	//	else
	//	{
	//		posTakeoffTime = preFlight->GetTime() + ElapsedTime(0.2);
	//	}
	//}
	////delay for approach flights
	//if( GetApproachFlightsCount() )
	//{
	//	AirsideFlightInSim * lastApproachFlight = GetLastApproachFlight();
	//	ElapsedTime t =  lastApproachFlight->GetTime();
	//	posTakeoffTime = max(t, posTakeoffTime);
	//}

	return posTakeoffTime;
}

ElapsedTime LogicRunwayInSim::GetNearestTakeoffTime( AirsideFlightInSim * pFlight )
{	
	
	//char ACType[256];
	//char otherACType[256];
	//pFlight->getACType(ACType);
	//AircraftSeparationManager * pSeparationMan = pFlight->GetAirTrafficController()->GetSeparationManager();

	ElapsedTime takeoffValidTime;
	//{//check the same runway			
	//	OccupancyInstance preFlightOcy = GetPreviousMobileElmentOccupancy(pFlight);			
	//	if(preFlightOcy.IsValid() )
	//	{			
	//		//ASSERT(preFlightOcy.IsExitTimeValid());
	//		preFlightOcy.GetFlight()->getACType(otherACType);
	//		ElapsedTime separationTime;
	//		if(preFlightOcy.GetOccupyType() == OnPreTakeoff )
	//		{				
	//			separationTime = pSeparationMan->GetTakeoffBehineTakeoffSepTime(ACType,otherACType);
	//		}
	//		if(preFlightOcy.GetOccupyType() == OnLanding )
	//		{
	//			separationTime = pSeparationMan->GetTakeoffBehindLandingSepTime(ACType,otherACType);
	//		}

	//		if(preFlightOcy.IsExitTimeValid())  //exit time know
	//		{				
	//			takeoffValidTime = max(takeoffValidTime, separationTime + preFlightOcy.GetExitTime() );	
	//		}
	//		else
	//		{
	//			takeoffValidTime = max(takeoffValidTime, preFlightOcy.GetFlight()->GetTime() + separationTime);
	//		}			
	//	}

	//	//check node is valid
	//	{
	//		for (int i=0; i<m_vIntersectionWithRunways.GetNodeCount(); i++)
	//		{
	//			IntersectionNodeInSim* pIntersectionNodeInSim = m_vIntersectionWithRunways.GetNodeByIndex(i);
	//			ASSERT(pIntersectionNodeInSim);

	//			OccupancyInstance lastOccupancyInstance = pIntersectionNodeInSim->GetLastOccupyInstance();
	//			if (lastOccupancyInstance.IsValid()
	//				&& !lastOccupancyInstance.IsExitTimeValid())
	//			{
	//				takeoffValidTime = max(takeoffValidTime, lastOccupancyInstance.GetFlight()->GetTime());
	//			}
	//		}
	//	}
	//}			
	//{//check the other port
	//	LogicRunwayInSim * pOtherPort = GetOtherPort();
	//	OccupancyInstance preFlightOcy = pOtherPort->GetLastOccupyInstance();
	//	if(preFlightOcy.IsValid())
	//	{
	//		preFlightOcy.GetFlight()->getACType(otherACType);
	//		
	//		if(preFlightOcy.IsExitTimeValid())
	//		{
	//			ElapsedTime separationTime;			
	//			if(preFlightOcy.GetOccupyType() == OnPreTakeoff )
	//			{				
	//				separationTime = pSeparationMan->GetTakeoffBehineTakeoffSepTime(ACType,otherACType, false);
	//			}
	//			if(preFlightOcy.GetOccupyType() == OnLanding )
	//			{
	//				separationTime = pSeparationMan->GetTakeoffBehindLandingSepTime(ACType,otherACType, false);
	//			}
	//			takeoffValidTime = max(takeoffValidTime, separationTime + preFlightOcy.GetEnterTime() );
	//		}
	//		else
	//		{
	//			takeoffValidTime = max(takeoffValidTime, preFlightOcy.GetFlight()->GetTime() );
	//		}
	//		
	//		
	//	}
	//}
	////check other intersect runway
	////for(int i = 0; i< GetIntersectRunwayCount();i ++)
	////{
	////	LogicRunwayInSim * pIntersectRunway = GetIntersectRunway(i);
	////	ASSERT(pIntersectRunway);
	////	OccupancyInstance preFlightOcy = pIntersectRunway->GetLastOccupyInstance();
	////	if( preFlightOcy.IsValid() && preFlightOcy.GetOccupyType() == OnPreTakeoff)
	////	{
	////		if( preFlightOcy.IsExitTimeValid() )
	////		{
	////			ElapsedTime separationTime;
	////			{				
	////				separationTime = pSeparationMan->GetTakeoffBehineTakeoffSepTime(ACType,otherACType,false);
	////				takeoffValidTime = max(takeoffValidTime, separationTime + preFlightOcy.GetEnterTime() );
	////			}				
	////		}
	////		else
	////		{
	////			takeoffValidTime = max(takeoffValidTime, preFlightOcy.GetFlight()->GetTime() );
	////		}
	////	}
	////}
	return takeoffValidTime;
}

void LogicRunwayInSim::AddFlightOnRunway( AirsideFlightInSim* pFlight )
{
	if(std::find(m_vOccupiedFlights.begin(),m_vOccupiedFlights.end(),pFlight) != m_vOccupiedFlights.end())
		return;
	
	m_vOccupiedFlights.push_back(pFlight);
	
}

void LogicRunwayInSim::RemoveLeftFlight( AirsideFlightInSim * pFlight )
{
	std::vector<AirsideFlightInSim*>::iterator itr = std::find(m_vOccupiedFlights.begin(),m_vOccupiedFlights.end(),pFlight);
	if(itr!= m_vOccupiedFlights.end())
	{
		m_vOccupiedFlights.erase(itr);
	}
	if (m_vOccupiedFlights.empty())
	{
		m_bHasMissedApproachFlight = false;
		NotifyObservers(pFlight->GetTime());
	}
		
}

void LogicRunwayInSim::RemoveLeftFlight( AirsideFlightInSim* pFlight, const ElapsedTime& t )
{
	std::vector<AirsideFlightInSim*>::iterator itr = std::find(m_vOccupiedFlights.begin(),m_vOccupiedFlights.end(),pFlight);
	if(itr!= m_vOccupiedFlights.end())
	{
		m_vOccupiedFlights.erase(itr);
	}
	if (m_vOccupiedFlights.empty())
	{
		m_bHasMissedApproachFlight = false;
		NotifyObservers(t);
	}
}

bool LogicRunwayInSim::IsRunwayClear(AirsideFlightInSim* pFlight) const
{
	if (m_vOccupiedFlights.empty())
		return true;

	if (m_vOccupiedFlights.size() ==1)
	{
		AirsideFlightInSim* pOccupyFlight = m_vOccupiedFlights.at(0);
		if (pOccupyFlight == pFlight && pOccupyFlight->GetRunwayExit()->CanHoldFlight(pOccupyFlight))
			return true;
	}

	return false;
}

int LogicRunwayInSim::GetDepartureQueueLength() const
{
	/*std::vector<AirsideFlightInSim*> vFlights;
	for(int i=0;i<GetTakeoffQueueCount();i++)
	{
		TakeoffQueueInSim * pQueue = GetTakeoffQueue(i);
		pQueue->GetInQueueFlights(vFlights);
	}
	return (int)vFlights.size();*/
	return m_vQueueList.GetAllFlightCount();
}

const int LogicRunwayInSim::GetTakeoffPostionQueueLength(RunwayExitInSim* pExit)
{
	QueueToTakeoffPos* pQueue = m_vQueueList.GetAddQueue(pExit) ;

	return pQueue->GetItemCount();
}

ElapsedTime LogicRunwayInSim::GetDepartureQueueWaitingTime( const ElapsedTime& curTime   ) const
{
	/*ElapsedTime allTime;
	int allCount = 0;
	for(int i=0;i<GetTakeoffQueueCount();i++)
	{
		TakeoffQueueInSim *pQueue = GetTakeoffQueue(i);
		std::vector<AirsideFlightInSim*> vFlights;
		int nCount = pQueue->GetInQueueFlights(vFlights);
		allCount += nCount;
		for(int j =0 ;j< nCount;j++)
		{
			AirsideFlightInSim * pFlight = vFlights.at(j);
			allTime += pQueue->GetInQueueTime(pFlight,curTime);
		}
	}
	allTime/=allCount;
	return allTime;*/
	return m_vQueueList.GetAvgInQueueTime(curTime);
}

bool LogicRunwayInSim::IsLeftPoint( const CPoint2008& pt )
{
	return m_pRunway->GetRunwayInput()->IsPointLeft( m_port, pt);
}


void LogicRunwayInSim::GetIntersectRunways(std::vector<std::pair<int,IntersectionNodeInSim *> >& vRunwayID)
{
	int nCurrentRunwayID = GetRunwayInSim()->GetRunwayInput()->getID();
	int nCount = m_vIntersectionWithRunways.GetNodeCount();
	
	for (int i =0; i < nCount; ++i)
	{
		IntersectionNodeInSim *pNode = m_vIntersectionWithRunways.GetNodeByIndex(i);

		 std::vector<RunwayIntersectItem*> vRunwayIntNode = pNode->GetNodeInput().GetRunwayIntersectItemList();
		 
		 for (size_t nRunwayInt =0; nRunwayInt < vRunwayIntNode.size(); ++ nRunwayInt)
		 {
			 RunwayIntersectItem*  runwayItem = vRunwayIntNode[nRunwayInt];
			 int nIntRunwayID = runwayItem->GetRunway()->getID();
			 if(nIntRunwayID != nCurrentRunwayID)
			 {
				 vRunwayID.push_back(std::make_pair(nIntRunwayID,pNode));
			 }
		 }
	}
}

void LogicRunwayInSim::AddIntersectionNodeInSim( IntersectionNodeInSim* pIntersectionNodeInSim )
{
	//m_vIntersectionWithRunways.Add(pIntersectionNodeInSim);

	double dDistance = this->GetPointDist(pIntersectionNodeInSim->GetNodeInput().GetPosition());
	int nExistCount = m_vIntersectionWithRunways.GetNodeCount();
	if(nExistCount == 0)//no node in vector
	{
		m_vIntersectionWithRunways.Add(pIntersectionNodeInSim);
	}
	else
	{
		double dCurDistance = this->GetPointDist(pIntersectionNodeInSim->GetNodeInput().GetPosition());

		int nNodeCount = m_vIntersectionWithRunways.GetNodeCount();
		int nCurNode = 0;
		for(nCurNode =0; nCurNode< nNodeCount; ++nCurNode)
		{
			IntersectionNodeInSim* pCurIntersectionNodeInSim = m_vIntersectionWithRunways.GetNodeByIndex(nCurNode);
			if (this->GetPointDist(pCurIntersectionNodeInSim->GetNodeInput().GetPosition()) > dCurDistance)//find the postion
			{
				m_vIntersectionWithRunways.Add(nCurNode,pIntersectionNodeInSim);
				break;
			}
		}
		//the last one
		if (nCurNode == nNodeCount)
		{
			m_vIntersectionWithRunways.Add(pIntersectionNodeInSim);
		}
	}
}
void LogicRunwayInSim::GetIntersectNodeInRange(double dMinDist,double dMaxDist, std::vector<IntersectionNodeInSim *>& vRunwayIntNode)
{
	vRunwayIntNode.clear();

	int nNodeCount = m_vIntersectionWithRunways.GetNodeCount();
	int nCurNode = 0;
	for(nCurNode =0; nCurNode< nNodeCount; ++nCurNode)
	{
		IntersectionNodeInSim* pCurIntersectionNodeInSim = m_vIntersectionWithRunways.GetNodeByIndex(nCurNode);
		double dDistance =	this->GetPointDist(pCurIntersectionNodeInSim->GetNodeInput().GetPosition());
		if ( dDistance>= dMinDist && dDistance < dMaxDist)//find the position
		{
			vRunwayIntNode.push_back(pCurIntersectionNodeInSim);
		}
	}
}

void LogicRunwayInSim::AddFlightToTakeoffQueue( AirsideFlightInSim*pFlight, RunwayExitInSim* pTakeoffPos,const ElapsedTime& enterTime )
{
	QueueToTakeoffPos * pQueue = m_vQueueList.GetAddQueue(pTakeoffPos);
	ASSERT(pQueue);
	if(pQueue)
	{
		if(!pQueue->IsFlightInQueue(pFlight))
		{
			pQueue->AddInQueueFlight(pFlight,enterTime);	
			//log
			TakeoffQueueCountLog::GetInstance().Log(this, enterTime, pFlight, true);
		}
	}

}

void LogicRunwayInSim::RemoveFlightInQueue( AirsideFlightInSim* pFlight )
{
	m_vQueueList.RemoveFlight(pFlight);
	TakeoffQueueCountLog::Log(this, pFlight->GetTime(),pFlight, false);
}

RunwayExitInSim* LogicRunwayInSim::GetSameNodeRunwayExitWith( RunwayExitInSim* pOppsiteRunwayExit )
{
	if(!pOppsiteRunwayExit)
		return NULL; 

	for(int i=0;i<GetExitCount();++i)
	{
		RunwayExitInSim* pExit = GetExitAt(i);
		if(pExit->GetRouteSeg() == pOppsiteRunwayExit->GetRouteSeg() )
			return pExit;
	}
	return NULL;
}

CPoint2008 LogicRunwayInSim::GetMissApproachCheckPoint(CPoint2008 LastWaypointPos)
{
	double dMissApproachDist = m_pRunway->GetMissApproachDistanceToRunway();
	double dDist = LastWaypointPos.distance(GetDistancePoint(0));
	if (dDist<= dMissApproachDist)
		return LastWaypointPos;

	CLine2008 line(GetDistancePoint(0), LastWaypointPos);
	return line.getInlinePoint(dMissApproachDist/dDist);
}

CPoint2008 LogicRunwayInSim::GetLowAndOverPoint(AirsideCircuitFlightInSim* pFlight,const CPoint2008& LastWaypointPos)const
{
	double dHighHeight = LastWaypointPos.getZ();
	double dLowHeight = GetDistancePoint(0).getZ();

	double dDis = dHighHeight - dLowHeight;
	double dRate = 0.0;
	double dFeet = pFlight->GetLowHigh();
	if (dDis >  (std::numeric_limits<double>::min)())
	{
		dRate = dFeet/dDis;
	}

	CLine2008 line(GetDistancePoint(0), LastWaypointPos);
	return line.getInlinePoint(dRate);
}

CPoint2008 LogicRunwayInSim::GetFlightTouchdownPoint(AirsideFlightInSim* pFlight)
{
	double dTouchdownDist = pFlight->GetTouchDownDistAtRuwnay(this);//pFlight->GetPerformance()->getDistThresToTouch(pFlight);
	CPoint2008 touchPoint = GetDistancePoint(dTouchdownDist);
	touchPoint.setZ(0.0);
	return touchPoint;
}

RunwayExitInSim* LogicRunwayInSim::GetAvailableExit(AirsideFlightInSim* pFlight)
{
	double dMinLandingDist = pFlight->GetPerformance()->getMinLandingDist(pFlight) + pFlight->GetTouchDownDistAtRuwnay(this); //pFlight->GetPerformance()->getDistThresToTouch(pFlight);
	std::vector<RunwayExitInSim*> vBackupExits;
	vBackupExits.clear();
	std::vector<RunwayExitInSim*> vOppsiteSideExits;
	vOppsiteSideExits.clear();

	CPoint2008 _point;
	if (pFlight->GetPlanedParkingStand(ARR_PARKING))
		_point = pFlight->GetPlanedParkingStand(ARR_PARKING)->GetDistancePoint(0);

	bool IsleftStand = GetRunwayInSim()->GetRunwayInput()->IsPointLeft(RUNWAY_MARK(getLogicRunwayType()),_point);

	int nCount = m_vExitsList.size();
	for (int i = 0; i < nCount; i++)
	{
		RunwayExitInSim* pExit = & m_vExitsList.at(i);

		if (IsleftStand != pExit->IsLeft())
		{
			vOppsiteSideExits.push_back(pExit);
			continue;
		}

		if (pExit->GetExitRunwayPos() < dMinLandingDist)
			vBackupExits.push_back(pExit);
		else
		{
			if (pExit->CanHoldFlight(pFlight))
				return pExit;
		}
	}

	//nCount = vBackupExits.size();
	//for (int i = 0; i < nCount; i++)
	//{
	//	RunwayExitInSim* pExit = vBackupExits.at(i);
	//	if (pExit->CanHoldFlight(pFlight))
	//		return pExit;
	//}

	nCount = vOppsiteSideExits.size();
	for (int i = 0; i < nCount; i++)
	{
		RunwayExitInSim* pExit = vOppsiteSideExits.at(i);
		if (pExit->CanHoldFlight(pFlight))
			return pExit;
	}

	return NULL;
}

bool LogicRunwayInSim::IsFlightInQueueToTakeoffPos( AirsideFlightInSim* pFlight, RunwayExitInSim* pTakeoffPos )
{
	for(int i=0;i<m_vQueueList.GetQueueCount();i++)
	{
		const QueueToTakeoffPos* pQueue = m_vQueueList.GetQueueByIdx(i);
		if(pQueue)
		{
			if(pQueue->IsFlightInQueue(pFlight))
				return true;
		}
	}
	return false;
	
	QueueToTakeoffPos * pQueue = m_vQueueList.GetAddQueue(pTakeoffPos);
	ASSERT(pQueue);
	if(pQueue)
	{
		return pQueue->IsFlightInQueue(pFlight);
	}
	return false;
}

int LogicRunwayInSim::GetAheadFlightCountInQueueToTakeoffPos(AirsideFlightInSim* pFlight)
{
	int nFltCount = 0;
	QueueToTakeoffPos * pFltQueue = m_vQueueList.GetAddQueue(pFlight->GetAndAssignTakeoffPosition());
	ASSERT(pFltQueue);
	if(pFltQueue)
	{
		ElapsedTime tTime = pFltQueue->GetFlightEnterTime(pFlight);
		int nQCount = m_vQueueList.GetQueueCount();
		for(int i=0; i<nQCount; i++)
		{
			const QueueToTakeoffPos* pQueue = m_vQueueList.GetQueueByIdx(i);
			if(pQueue && pQueue != pFltQueue)
			{
				nFltCount += pQueue->GetAheadFlightInQ(tTime);
			}
		}
	}

	return nFltCount;
}

ElapsedTime LogicRunwayInSim::GetFlightEnterQueueTime( AirsideFlightInSim* pFlight )const
{
	for(int i=0;i<m_vQueueList.GetQueueCount();i++)
	{
		const QueueToTakeoffPos* pQueue = m_vQueueList.GetQueueByIdx(i);
		if(pQueue)
		{
			for(int j=0;j<pQueue->GetItemCount();j++)
				if(pQueue->ItemAt(j).GetFlight() == pFlight)
					return pQueue->ItemAt(j).GetEnterTime();
		}
	}
	return pFlight->GetTime();
}

bool LogicRunwayInSim::IsFlightInQueueToRunway( AirsideFlightInSim* pFlight, LogicRunwayInSim* pRunway ) const
{
	for(int i=0;i<m_vQueueList.GetQueueCount();i++)
	{
		const QueueToTakeoffPos* pQueue = m_vQueueList.GetQueueByIdx(i);
		if(pQueue)
		{
			for(int j=0;j<pQueue->GetItemCount();j++)
				if(pQueue->ItemAt(j).GetFlight() == pFlight)
					return true;
		}
	}
	return false;
}

//void LogicRunwayInSim::AddFlightOccupyRunwayInfo(AirsideFlightInSim* pFlight, const CRunwaySystem::RunwayOccupyInfoList& vOccupyInfo)
//{
//	if (pFlight == NULL)
//		return;
//
//	CRunwaySystem::RunwayOccupyInfoList vRunwayOccupyInfo;
//	vRunwayOccupyInfo.assign(vOccupyInfo.begin(),vOccupyInfo.end());
//
//	int nCount = m_mapFlightOccupyRunwayInfo.size();
//
//	std::map<AirsideFlightInSim*,CRunwaySystem::RunwayOccupyInfoList>::iterator iter = m_mapFlightOccupyRunwayInfo.find(pFlight);
//	if ( iter != m_mapFlightOccupyRunwayInfo.end())
//	{
//		iter->second = vRunwayOccupyInfo;
//		return;
//	}
//
//	m_mapFlightOccupyRunwayInfo.insert(std::map<AirsideFlightInSim*,CRunwaySystem::RunwayOccupyInfoList>::value_type(pFlight,vRunwayOccupyInfo));
//}
//
//CRunwaySystem::RunwayOccupyInfoList LogicRunwayInSim::GetFlightOccupyRunwayInfo(AirsideFlightInSim* pFlight)
//{
//	std::map<AirsideFlightInSim*,CRunwaySystem::RunwayOccupyInfoList>::iterator iter = m_mapFlightOccupyRunwayInfo.find(pFlight);
//	if ( iter != m_mapFlightOccupyRunwayInfo.end())
//	{
//		return iter->second;
//	}
//
//	CRunwaySystem::RunwayOccupyInfoList vRunwayOccupyInfo;
//	vRunwayOccupyInfo.clear();
//	return vRunwayOccupyInfo;
//}

void LogicRunwayInSim::SetHasMissedApproachFlight()
{
	m_bHasMissedApproachFlight = true;
}

CString LogicRunwayInSim::GetMarkName()
{
	if(GetRunwayInSim() && GetRunwayInSim()->GetRunwayInput())
	{
		if(getLogicRunwayType() == RUNWAYMARK_FIRST)
			return CString(GetRunwayInSim()->GetRunwayInput()->GetMarking1().c_str());
		else
			return CString(GetRunwayInSim()->GetRunwayInput()->GetMarking2().c_str());
	}

	return _T("");
}

#include "..\..\Results\AirsideFlightEventLog.h"
void LogicRunwayInSim::getDesc( ResourceDesc& resDesc )
{
	resDesc.resid = GetRunwayInSim()->GetRunwayID();
	resDesc.resdir = getLogicRunwayType()==RUNWAYMARK_FIRST?ResourceDesc::POSITIVE:ResourceDesc::NEGATIVE;
	resDesc.resType = GetType();
	resDesc.strRes = PrintResource();

}

DistanceUnit LogicRunwayInSim::GetLandingThreshold() const
{
	if(getLogicRunwayType() == RUNWAYMARK_FIRST)
	{
		return GetRunwayInSim()->GetRunwayInput()->GetMark1LandingThreshold();
	}
	else
		return GetRunwayInSim()->GetRunwayInput()->GetMark2LandingThreshold();
}
