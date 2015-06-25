#include "StdAfx.h"
#include ".\airsdieflightbaggagemanager.h"
#include "AirsideBaggageCartInSim.h"
#include "AirsideMobElementBaseBehavior.h"
#include "AirsideBaggageBehavior.h"
#include "Airside\AirsideFlightInSim.h"
#include "AirsideBaggageTrainInSim.h"
#include "Airside\VehicleRequestDispatcher.h"

AirsideFlightBaggageManager::AirsideFlightBaggageManager(void)
:m_pFlight(NULL)
{
	m_bBaggageGenerated = false;
	m_nArrBagLoadedByCart = 0;
}

AirsideFlightBaggageManager::~AirsideFlightBaggageManager(void)
{
}

void AirsideFlightBaggageManager::SetFlight( AirsideFlightInSim *pFight )
{
	m_pFlight = pFight;
}

//arrival flight
bool AirsideFlightBaggageManager::LoadBagFromFlightAtStand( AirsideBaggageTrainInSim *pBaggageTrain, ElapsedTime eEntryTime, ElapsedTime& eFinishedTime )
{
	AirsideBaggageCartInSim *pBagCartInSim = pBaggageTrain->getCurBaggageCart();
	if(pBagCartInSim == NULL)
		return true;

	eFinishedTime = eEntryTime;

	int nLoadCount = pBagCartInSim->getCapacity();
	int nFlightBagCount = m_vArrivalBag.size();

	for (int nBag = 0; nBag < nLoadCount; ++ nBag)
	{
		ElapsedTime eBagTime = eEntryTime;

		if(m_nArrBagLoadedByCart < nFlightBagCount)
		{
			Person *pBag = m_vArrivalBag.at(m_nArrBagLoadedByCart);

			AirsideMobElementBaseBehavior*pBagBehavior = pBag->getAirsideBehavior();
			if(pBagBehavior == NULL)
			{
				pBagBehavior = new AirsideBaggageBehavior(pBag, EntryAirside);
				pBag->setBehavior(pBagBehavior);
			}

			AirsideFlightInSim *pFlight = pBaggageTrain->GetServiceFlight();
			CPoint2008 ptCargoDoor;
			pFlight->getCargoDoorPosition(ptCargoDoor);

			//one bag 1 seconds
			eBagTime += ElapsedTime(nBag * 1L);
			//show at cargo door
			pBagBehavior->setLocation(ptCargoDoor);
			pBagBehavior->WriteLog(eBagTime);

			pBagBehavior->setState(MOVETOBAGCART);
			pBagBehavior->WriteLog(eBagTime);
	
			//cart location
			CPoint2008 ptCart = pBaggageTrain->GetPosition();
			pBagBehavior->setDestination(ptCart);

			ElapsedTime eMoveTime = pBagBehavior->moveTime();
			eBagTime += eMoveTime;
			pBagBehavior->setLocation(ptCart);	
			pBagBehavior->setState(ARRIVEATBAGCART);
			pBagBehavior->WriteLog(eBagTime);

			ElapsedTime eServiceTime = ElapsedTime(1L);
			eBagTime += eServiceTime;

			pBagBehavior->setState(STAYATCART);
			pBagBehavior->WriteLog(eBagTime);

			m_nArrBagLoadedByCart += 1;

		//	eFinishedTime = eBagTime;
			eFinishedTime = MAX(eFinishedTime,eBagTime);
			pBagCartInSim->AddBaggage((AirsideBaggageBehavior*)pBagBehavior);
		}
		else
			break;
	}

	if(m_nArrBagLoadedByCart >= nFlightBagCount)
		return true; // all baggage is loaded

	return false;



}

void AirsideFlightBaggageManager::GenerateBaggageIfNeed(const ElapsedTime& time)
{
	if(m_bBaggageGenerated )
		return;

	Flight* pFlightInput = m_pFlight->GetFlightInput();
	if (pFlightInput == NULL)
		return;

	//check the flight need baggage cart service or not
	bool bBagCartService = false;
	if(m_pFlight->GetServiceRequestDispatcher() && 
		m_pFlight->GetServiceRequestDispatcher()->HasVehicleServiceFlight(m_pFlight, VehicleType_BaggageTug))
	{

		bBagCartService = true;
	}

	CPaxGenerator paxgenerator(m_pFlight->GetEngine());
	ElapsedTime eEntryTime = time;
	paxgenerator.GenerateDelayMobileBag(pFlightInput->getFlightIndex(),eEntryTime, m_vArrivalBag, bBagCartService) ;
	
	m_bBaggageGenerated = true;
}

void AirsideFlightBaggageManager::FlightArriveStand(bool bArrival, const ElapsedTime& time )
{
	if(bArrival)
	{
		GenerateBaggageIfNeed(time);
	}
}

bool AirsideFlightBaggageManager::IsGenerateGaggage() const
{
	return m_bBaggageGenerated;
}
