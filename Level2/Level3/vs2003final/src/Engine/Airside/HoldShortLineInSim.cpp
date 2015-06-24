#include "StdAfx.h"
#include ".\holdshortlineinsim.h"
#include "../../InputAirside/HoldShortLines.h"
#include "AirportResourceManager.h"
#include "../../Common/DynamicMovement.h"
#include "../../Common/Line2008.h"
#include "../../InputAirside/TaxiLinkedRunwayData.h"
#include "AirTrafficController.h"
#include "AirsideResourceManager.h"
#include "AirportResourceManager.h"
#include "RunwayResourceManager.h"
#include "RunwayInSim.h"
#include "FlightPerformancesInSim.h"

TaxiInterruptLineInSim::TaxiInterruptLineInSim(CTaxiInterruptLine* pHoldShortLine)
:AirsideResource()
,m_pHoldShortLine(pHoldShortLine)
{
}

TaxiInterruptLineInSim::~TaxiInterruptLineInSim(void)
{
}

CString TaxiInterruptLineInSim::GetName()
{
	return m_pHoldShortLine->GetName();
}

CPoint2008 TaxiInterruptLineInSim::GetPosition()
{
	return m_pHoldShortLine->GetPosition();
}

CTaxiInterruptLine* TaxiInterruptLineInSim::GetInterruptLineInput()
{
	return m_pHoldShortLine;
}

AirsideResource::ResourceType TaxiInterruptLineInSim::GetType() const
{
	return AirsideResource::ResType_InterruptLine;
}

CString TaxiInterruptLineInSim::GetTypeName() const
{
	return "Taxi Interrupt Line";
}

CString TaxiInterruptLineInSim::PrintResource() const
{
	return m_pHoldShortLine->GetName();
}

CPoint2008 TaxiInterruptLineInSim::GetDistancePoint(double dist) const
{
	return m_pHoldShortLine->GetPosition();
}

#include "..\..\InputAirside\TaxiInterruptTimes.h"
#include "..\..\Results\AirsideFlightEventLog.h"
#include "AirsideFlightInSim.h"
#include "AirTrafficController.h"
void TaxiInterruptLineInSim::getDesc( ResourceDesc& resDesc )
{
	resDesc.resid = GetInterruptLineInput()->GetID();
	resDesc.resType =GetType();
	resDesc.strRes = PrintResource();
}

bool TaxiInterruptLineInSim::bInterruptFlight( AirsideFlightInSim* pflt,int StartNodeId, ElapsedTime& inttime ) 
{
	CTaxiInterruptTimes* pIntrruptTime = pflt->GetAirTrafficController()->GetIntruptTimeSetting();
	if(pIntrruptTime)
	{
		for(int i=0;i<pIntrruptTime->GetCount();i++)
		{
			CTaxiInterruptFlightData* pIntrruptTimepair = pIntrruptTime->GetItem(i);
			if( pflt->fits(*pIntrruptTimepair->GetFlightTy()) )
			{
				for(int idxItem =0;idxItem<pIntrruptTimepair->GetCount();++idxItem)
				{
					CTaxiInterruptTimeItem* pItem =  pIntrruptTimepair->GetItem(idxItem);
					if (pItem->GetStartTime() <= pflt->GetTime() && pflt->GetTime() <= pItem->GetEndTime() )
					{
						if( pItem->GetShortLineID() == m_pHoldShortLine->GetID() )
						{
							if( (m_pHoldShortLine->GetFrontIntersectionNodeID() == StartNodeId && pItem->GetDirection() >0) 
								|| (m_pHoldShortLine->GetFrontIntersectionNodeID()!= StartNodeId && pItem->GetDirection()<=0) )
							{
								inttime = GetFltHoldTime(pflt,pItem);
								return true;
							}
						}
					}
				}
			}
		}
	}
	return false;
}

void TaxiInterruptLineInSim::SetWaitFlight( AirsideFlightInSim* pFlight, const ElapsedTime& tTime )
{
	m_pCurWaitFlight = pFlight;
	
}

ElapsedTime TaxiInterruptLineInSim::GetFltHoldTime( AirsideFlightInSim* pflt, CTaxiInterruptTimeItem* pSettingItem )
{
	if (!pSettingItem->IsLinkedRunway())
		return ElapsedTime(pSettingItem->GetHoldTime()->GetProbDistribution()->getRandomValue());

	AirportResourceManager* pAirportRes = pflt->GetAirTrafficController()->GetAirsideResourceManager()->GetAirportResource();

	ElapsedTime tHoldTime = 0L;

	int nCount = pSettingItem->GetLinkedRunwayDataCount();
	for (int i =0; i <nCount; i++)
	{
		CTaxiLinkedRunwayData* pRunwayData = pSettingItem->GetLinkedRunwayDataByIdx(i);
		LogicRunwayInSim* pLogicRunway = pAirportRes->getRunwayResource()->GetLogicRunway(pRunwayData->GetLinkRwyID(),(RUNWAY_MARK)pRunwayData->GetLinkRwyMark());
		if (pLogicRunway == NULL)
			continue;

		CLine2008 rwyLine(pLogicRunway->GetDistancePoint(0),pLogicRunway->GetOtherPort()->GetDistancePoint(0));
		CPoint2008 ProjectPoint = rwyLine.getProjectPoint(m_pHoldShortLine->GetPosition());
		double dDist = rwyLine.GetPoint1().distance(ProjectPoint);

		ElapsedTime tCurTime = pflt->GetTime();
		OccupancyTable OccTable = pLogicRunway->GetOccupancyTable();

		ElapsedTime tSepTime = 0L;
		ElapsedTime tSepToNext = 0L;

		ElapsedTime tPreToNode =0L;
		ElapsedTime tNextToNode =0L;

		OccupancyInstance instance;
		OccupancyInstance nextIns;

		int nItemCount = OccTable.size();
		for (int j =1; j < nItemCount; j++)
		{
			instance = OccTable.at(j-1);
			nextIns = OccTable.at(j);

			if (tCurTime > nextIns.GetExitTime())
				continue;

			if (instance.GetOccupyType() == OnTakeoff)
			{
				tSepTime = pRunwayData->GetTakeoffHoldTime();

				if (ProjectPoint != m_pHoldShortLine->GetPosition())	//the projection point on runway
				{
					double Takeoffspd = instance.GetFlight()->GetPerformance()->getLiftoffSpeed(instance.GetFlight());
					double dDistToPort = instance.GetFlight()->GetAndAssignTakeoffPosition()->GetExitRunwayPos();

					DynamicMovement flightmove(0, Takeoffspd,instance.GetFlight()->GetTakeoffRoll());
					tPreToNode = flightmove.GetDistTime(dDist - dDistToPort);	
				}	
				else
				{
					if (rwyLine.GetPoint1().distance(m_pHoldShortLine->GetPosition()) < rwyLine.GetPoint2().distance(m_pHoldShortLine->GetPosition()))
						tPreToNode = instance.GetExitTime() - instance.GetEnterTime();
				}

			}
			if (instance.GetOccupyType() == OnLanding)
			{
				tSepTime = pRunwayData->GetLandingHoldTime();

				if (ProjectPoint != m_pHoldShortLine->GetPosition())	//the projection point on runway
				{
					double landspd = instance.GetFlight()->GetPerformance()->getLandingSpeed(instance.GetFlight());
					double dTouchDownDist = instance.GetFlight()->GetPerformance()->getDistThresToTouch(instance.GetFlight());
					double dRollingDist = instance.GetFlight()->GetRunwayExit()->GetExitRunwayPos();

					DynamicMovement flightmove(landspd, instance.GetFlight()->GetExitSpeed(),dRollingDist - dTouchDownDist);
					tPreToNode = flightmove.GetDistTime(dDist - dTouchDownDist);	
				}
			}
			
			if (j ==1)			//passed interrupt line before first landing/takeoff flight
			{
				if (instance.GetEnterTime() + tPreToNode >= tCurTime + tSepTime)
					tHoldTime = 0L;
			}

			if (nextIns.GetOccupyType() == OnTakeoff)
			{
				tSepToNext = pRunwayData->GetTakeoffHoldTime();

				if (ProjectPoint != m_pHoldShortLine->GetPosition())	//the projection point on runway
				{
					double Takeoffspd = nextIns.GetFlight()->GetPerformance()->getLiftoffSpeed(nextIns.GetFlight());
					double dDistToPort = nextIns.GetFlight()->GetAndAssignTakeoffPosition()->GetExitRunwayPos();

					DynamicMovement flightmove(0, Takeoffspd,nextIns.GetFlight()->GetTakeoffRoll());
					tNextToNode = flightmove.GetDistTime(dDist - dDistToPort);	
				}	
				else
				{
					if (rwyLine.GetPoint1().distance(m_pHoldShortLine->GetPosition()) < rwyLine.GetPoint2().distance(m_pHoldShortLine->GetPosition()))
						tNextToNode = nextIns.GetExitTime() - nextIns.GetEnterTime();
				}

			}
			if (instance.GetOccupyType() == OnLanding)
			{
				tSepToNext = pRunwayData->GetLandingHoldTime();

				if (ProjectPoint != m_pHoldShortLine->GetPosition())	//the projection point on runway
				{
					double landspd = nextIns.GetFlight()->GetPerformance()->getLandingSpeed(nextIns.GetFlight());
					double dTouchDownDist = nextIns.GetFlight()->GetPerformance()->getDistThresToTouch(nextIns.GetFlight());
					double dRollingDist = nextIns.GetFlight()->GetRunwayExit()->GetExitRunwayPos();

					DynamicMovement flightmove(landspd, nextIns.GetFlight()->GetExitSpeed(),dRollingDist - dTouchDownDist);
					tNextToNode = flightmove.GetDistTime(dDist - dTouchDownDist);	
				}
			}

			ElapsedTime tCurTime2;
			if (instance.GetEnterTime() + tPreToNode > tCurTime - tSepTime)
				tCurTime = instance.GetEnterTime() + tPreToNode + tSepTime;
			
			if (nextIns.GetEnterTime() + tNextToNode < tCurTime + tSepToNext)
				tCurTime2 = nextIns.GetEnterTime() + tNextToNode - tSepToNext;
			
			if (tCurTime2 >= tCurTime)
			{
				tHoldTime = max(tHoldTime, tCurTime - pflt->GetTime());
			}
		}

		if (nextIns.GetEnterTime() + tNextToNode > tCurTime - tSepToNext)
		{
			tCurTime = nextIns.GetEnterTime() + tNextToNode + tSepToNext;
			tHoldTime = max(tHoldTime, tCurTime - pflt->GetTime());
		}
	
	}

	return tHoldTime;
}
/////////////////////////////////////////////////////
TaxiInterruptLineInSimList::TaxiInterruptLineInSimList()
{
	m_vHoldShortLines.clear();

	m_pHoldShortLines = new TaxiInterruptLineList;

}

TaxiInterruptLineInSimList::~TaxiInterruptLineInSimList()
{
	std::vector<TaxiInterruptLineInSim*>::iterator iter = m_vHoldShortLines.begin();
	for (; iter!= m_vHoldShortLines.end(); iter++)
	{
		delete *iter;
		*iter = NULL;
	}
	m_vHoldShortLines.clear();

	delete m_pHoldShortLines;
	m_pHoldShortLines = NULL;

	
}

void TaxiInterruptLineInSimList::Init(AirportResourceManager* pAirportRes)
{
	m_pHoldShortLines->ReadData(-1);
	int nCount = m_pHoldShortLines->GetElementCount();
	for (int i = 0; i < nCount; i++)
	{
		CTaxiInterruptLine* pHoldShortLine = m_pHoldShortLines->GetItem(i);
		TaxiInterruptLineInSim* pHoldShortLineInSim = new TaxiInterruptLineInSim(pHoldShortLine);

		//mark hold short line on which taxiway segment

		IntersectionNodeInSim* pStartNode = pAirportRes->GetIntersectionNodeList().GetNodeByID(pHoldShortLine->GetFrontIntersectionNodeID());
		IntersectionNodeInSim* pBackNode = pAirportRes->GetIntersectionNodeList().GetNodeByID(pHoldShortLine->GetBackIntersectionNodeID());
		TaxiwayResourceManager* pTaxiwayManager = pAirportRes->getTaxiwayResource();
		TaxiwaySegInSim* pSeg = pTaxiwayManager->GetTaxiwaySegment(pStartNode,pBackNode);
		if (pSeg != NULL)
		{
			pHoldShortLineInSim->SetAttachedTaxiwaySegment(pSeg);
			pSeg->SetHoldShortLine(pHoldShortLineInSim);
			m_vHoldShortLines.push_back(pHoldShortLineInSim);
		}

	}
}

TaxiInterruptLineInSim* TaxiInterruptLineInSimList::GetHoldShortLineByIdx(int nIdx)
{
	if (nIdx < 0 || nIdx >= GetCount())
		return NULL;

	return m_vHoldShortLines.at(nIdx);
}

int TaxiInterruptLineInSimList::GetCount()
{
	if (m_vHoldShortLines.empty())
		return 0;

	return (int)m_vHoldShortLines.size();
}

TaxiInterruptLineInSim* TaxiInterruptLineInSimList::GetHoldShortLineById( int nId ) const
{
	size_t nSize = m_vHoldShortLines.size();
	for (size_t i =0; i < nSize; i++)
	{
		TaxiInterruptLineInSim* pLine = m_vHoldShortLines.at(i);
		if (pLine->GetInterruptLineInput()->GetID() == nId )
			return pLine;
	}

	return NULL;
}

