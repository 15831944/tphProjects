#include "StdAfx.h"
#include ".\enroutequeuecapacityinsim.h"
#include "../../InputAirside/AirsideEnrouteQueueCapacity.h"
#include "AirportResourceManager.h"
#include "RunwayResourceManager.h"
#include "QueueToTakeoffPos.h"
#include "AirsideFlightInSim.h"
#include "AirTrafficController.h"
#include "AirsideResourceManager.h"


EnrouteQueueCapacityInSim::EnrouteQueueCapacityInSim(void)
{
	m_pEnrouteQueueCapacity = NULL;
	m_pAirportres = NULL;
}

EnrouteQueueCapacityInSim::~EnrouteQueueCapacityInSim(void)
{
}

void EnrouteQueueCapacityInSim::Init( AirportResourceManager * pAirportRes )
{
	m_pEnrouteQueueCapacity = new CAirsideEnrouteQueueCapacity();
	m_pEnrouteQueueCapacity->ReadData(-1);
	m_pAirportres = pAirportRes;
}

bool EnrouteQueueCapacityInSim::PushBackExitEnrouteQCapacity( const ElapsedTime& eTime, AirsideFlightInSim* pFlight )
{
	TaxiRouteInSim* pTaxiRoute = pFlight->GetAndAssignOutBoundRoute();
	if (pTaxiRoute == NULL)
		return true;
	
	pTaxiRoute->InitRoute(pFlight,eTime);
	FlightHoldListInTaxiRoute& taxiRouteList = pTaxiRoute->GetHoldList();

	CString strTime = eTime.printTime();

	for (int i = 0; i < taxiRouteList.GetCount(); i++)
	{
		const HoldInTaxiRoute& theHold = taxiRouteList.ItemAt(i);
		if (theHold.m_pNode->GetNodeInput().GetRunwayIntersectItemList().size() == 0)
			continue;
		
		RunwayIntersectItem* pRwItem = theHold.m_pNode->GetNodeInput().GetRunwayIntersectItemList()[0];
		int nRwID = pRwItem->GetObjectID();
		RunwayInSim* pRunway = pFlight->GetAirTrafficController()->GetAirsideResourceManager()->GetAirportResource()->getRunwayResource()->GetRunwayByID(nRwID);
		HoldPositionInSim* pCrossHold = pRunway->GetHoldPosition( theHold.m_nHoldId );
		if (pCrossHold == NULL)
			continue;
			
		size_t nCount = m_pEnrouteQueueCapacity->GetElementCount();

		for (size_t i = 0; i < nCount; i++)
		{
			AirsideEnrouteQCapacityTimeRange* pQCapacityTimeRange = m_pEnrouteQueueCapacity->GetItem(i);

			TimeRange* pTimeRange = pQCapacityTimeRange->getTimeRange();
			if (eTime < pTimeRange->GetStartTime() || eTime > pTimeRange->GetEndTime())
				continue;

			size_t iEnrouteCount = pQCapacityTimeRange->getEnrouteQCapacity()->GetElementCount();

			for (size_t j = 0; j < iEnrouteCount; j++)
			{
				AirsideEnrouteQCapacityItem* pCapacityItem = pQCapacityTimeRange->getEnrouteQCapacity()->GetItem(j);

				int nExitID = pCapacityItem->getTakeOffPosition()->GetID();
				if(nExitID < 0)
					continue;

				LogicRunwayInSim* pRwyPort = m_pAirportres->getRunwayResource()->GetLogicRunway(pCapacityItem->getTakeOffPosition()->GetRunwayID(),pCapacityItem->getTakeOffPosition()->GetRunwayMark());
				if(!pRwyPort)
					continue;
				RunwayExitInSim* pRwyExit = pRwyPort->GetExitByID(nExitID); 
				if(!pRwyExit)
					continue;

				HoldPositionInSim* pHold = pRwyExit->getHoldPosition();
				if(!pHold)
					continue;

				if (pCrossHold != pHold)
					continue;
				

				double dQueueLength = pHold->GetTakeoffQueueLength();
				double dMaxQueue = pCapacityItem->getMaxLength() * 100;
				if (dQueueLength > dMaxQueue)
				{
					pHold->AddEnrouteWaitList(pFlight);
					return false;
				}
			}
		}
	}
	
	
	return true;
}
