#include "StdAfx.h"
#include ".\airsidepassengerbusstrategy.h"
#include "AirsidePaxBusInSim.h"
#include "..\..\Common\elaptime.h"
#include "AirsideFlightInSim.h"
#include "../person.h"
#include "..\OnboardFlightInSim.h"
#include "..\ARCportEngine.h"
#include "..\..\Inputs\flight.h"
#include "..\OnboardSimulation.h"
#include "..\AirsideMobElementBehavior.h"
#include "AirTrafficController.h"
#include "..\hold.h"
#include "..\terminal.h"
#include "VehicleRequestDispatcher.h"

AirsidePassengerBusStrategy::AirsidePassengerBusStrategy(AirsideFlightInSim* pAirsideFlightInSim)
:m_pAirsideFlightInSim(pAirsideFlightInSim)
{
}

AirsidePassengerBusStrategy::~AirsidePassengerBusStrategy(void)
{
}

AirsideArrPassengerBusStrategy::AirsideArrPassengerBusStrategy( AirsideFlightInSim* pAirsideFlightInSim )
:AirsidePassengerBusStrategy(pAirsideFlightInSim)
{

}

AirsideArrPassengerBusStrategy::~AirsideArrPassengerBusStrategy()
{

}

void AirsideArrPassengerBusStrategy::PassengerBusArrive(const ElapsedTime& time,CAirsidePaxBusInSim* pPaxBus,AirsidePassengerBusContext* pPaxBusContext,bool bGenerateBaggage)
{
	if (m_pAirsideFlightInSim == NULL)
		return;

	Flight* pFlight = m_pAirsideFlightInSim->GetFlightInput();
	if (pFlight == NULL)
		return;

	CARCportEngine* pArcportEngine = m_pAirsideFlightInSim->GetEngine();
	if (pArcportEngine == NULL)
		return;

	std::vector<Person*> _paxlist ;

	Terminal* m_terminal = pFlight->GetTerminal() ;
	if (pArcportEngine->IsOnboardSel() 
		&& pArcportEngine->GetOnboardSimulation() 
		&& pArcportEngine->GetOnboardSimulation()->GetOnboardFlightInSim(m_pAirsideFlightInSim, true))	//on board mode selected 
	{
		OnboardFlightInSim* pOnboardFlight = pArcportEngine->GetOnboardSimulation()->GetOnboardFlightInSim(m_pAirsideFlightInSim, true);
		pOnboardFlight->GetPaxListInFlight(_paxlist);
		if (!_paxlist.empty())
		{
			for (int paxIndex = 0; paxIndex < (int)_paxlist.size();paxIndex++)
			{
				if (_paxlist[paxIndex]->m_pGroupInfo && _paxlist[paxIndex]->m_pGroupInfo->IsFollower())		//ignore follower in group
					continue;
				m_pAirsideFlightInSim->AddPaxInFlight();
			}
			pPaxBusContext->SetPaxListInFlight(_paxlist);
			pOnboardFlight->WakeupPassengerDeplane(time);
			pOnboardFlight->GenerateCloseDoorEvent(time);
		}
	}
	else		
	{
		if (!pPaxBusContext->HaveGenerateMobileElment())	//did not generate person of the flight
		{
			ElapsedTime eEntryTime = time;
			CPaxGenerator paxgenerator(pArcportEngine) ;
			paxgenerator.GenerateDelayMobileElement(pFlight->getFlightIndex(),eEntryTime,_paxlist,bGenerateBaggage,-1) ;
			m_pAirsideFlightInSim->AddPaxCount((int)_paxlist.size());
			pPaxBusContext->SetPaxListInFlight(_paxlist);
		}
	}

	pPaxBusContext->ProcessPassengerTakeonBus(time,pPaxBus);

}


void AirsideArrPassengerBusStrategy::FlightArriveStand(const ElapsedTime& time,AirsidePassengerBusContext* pPaxBusContext,bool bGenerateBaggage)
{
	if (m_pAirsideFlightInSim == NULL)
		return;

	Flight* pFlight = m_pAirsideFlightInSim->GetFlightInput();
	if (pFlight == NULL)
		return;

	CARCportEngine* pArcportEngine = m_pAirsideFlightInSim->GetEngine();
	if (pArcportEngine == NULL)
		return;

	//CPaxGenerator paxgenerator(pArcportEngine);
	ElapsedTime eEntryTime = time;
	//paxgenerator.GenerateDelayMobileBag(pFlight->getFlightIndex(),eEntryTime) ;

	Terminal* m_terminal = pFlight->GetTerminal() ;
	
	FlightServiceReqirementInSim* requestSim =	m_pAirsideFlightInSim->GetAirTrafficController()->GetFlightServiceRequirement() ;
	CAirsidePaxBusInSim* _bus = NULL ;

	std::vector<Person*> _paxlist ;

	if((m_pAirsideFlightInSim->GetServiceRequestDispatcher() == NULL || !m_pAirsideFlightInSim->GetServiceRequestDispatcher()->HasVehicleServiceFlight(m_pAirsideFlightInSim,VehicleType_PaxTruck) \
		|| m_pAirsideFlightInSim->HasBrrdgeServer(TRUE)))
	{ // there is no bus server for this flight or has the bridge server , when flight arrived , notice all passengers to take off directly . 

		if (pArcportEngine->IsOnboardSel()
			&& pArcportEngine->GetOnboardSimulation() && pArcportEngine->GetOnboardSimulation()->GetOnboardFlightInSim(m_pAirsideFlightInSim, true))//on board mode selected
		{
			OnboardFlightInSim* pOnboardFlight = pArcportEngine->GetOnboardSimulation()->GetOnboardFlightInSim(m_pAirsideFlightInSim, true);
			pOnboardFlight->GetPaxListInFlight(_paxlist);

			for (int paxIndex = 0; paxIndex < (int)_paxlist.size();paxIndex++)
			{
				if (_paxlist[paxIndex]->m_pGroupInfo && _paxlist[paxIndex]->m_pGroupInfo->IsFollower())		//ignore follower in group
					continue;

				m_pAirsideFlightInSim->AddPaxInFlight();
			}
			pPaxBusContext->SetPaxListInFlight(_paxlist);
			pOnboardFlight->WakeupPassengerDeplane(time);
			pOnboardFlight->GenerateCloseDoorEvent(time);
		}
		else			
		{
			CPaxGenerator paxgenerator(pArcportEngine) ;
			int pax_num = paxgenerator.GenerateDelayMobileElement(pFlight->getFlightIndex(),eEntryTime,_paxlist,bGenerateBaggage,-1) ;
			m_pAirsideFlightInSim->AddPaxCount(pax_num);
			pPaxBusContext->SetPaxListInFlight(_paxlist);
		}	

		for (int i = 0 ; i < (int)_paxlist.size() ;i ++)
		{
			AirsidePassengerBehavior* spAirsideBehavior = (AirsidePassengerBehavior*)_paxlist[i]->getBehavior(MobElementBehavior::AirsideBehavior);
			if (spAirsideBehavior)
			{
				spAirsideBehavior->CancelPaxBusService() ;
			}
		}
		return ;
	}


	int pax_num = 0;

	if (pArcportEngine->IsOnboardSel()
		&& pArcportEngine->GetOnboardSimulation() && pArcportEngine->GetOnboardSimulation()->GetOnboardFlightInSim(m_pAirsideFlightInSim, true))	//on board mode selected
	{
		OnboardFlightInSim* pOnboardFlight = pArcportEngine->GetOnboardSimulation()->GetOnboardFlightInSim(m_pAirsideFlightInSim, true);
		pOnboardFlight->GetPaxListInFlight(_paxlist);

		if (!_paxlist.empty())
		{
			for (int paxIndex = 0; paxIndex < (int)_paxlist.size();paxIndex++)
			{
				if (_paxlist[paxIndex]->m_pGroupInfo && _paxlist[paxIndex]->m_pGroupInfo->IsFollower())		//ignore follower in group
					continue;

				m_pAirsideFlightInSim->AddPaxInFlight();
			}
			pPaxBusContext->SetPaxListInFlight(_paxlist);
			pOnboardFlight->WakeupPassengerDeplane(time);
			pOnboardFlight->GenerateCloseDoorEvent(time);
		}

	}
	else		
	{
		if (!pPaxBusContext->GetPaxBusCount())
		{
			CPaxGenerator paxgenerator(pArcportEngine) ;
			pax_num = paxgenerator.GenerateDelayMobileElement(pFlight->getFlightIndex(),eEntryTime,_paxlist,bGenerateBaggage,-1) ;
			m_pAirsideFlightInSim->AddPaxCount(pax_num);
			pPaxBusContext->SetPaxListInFlight(_paxlist);
		}
	}

	std::deque<Person*> vPerson;
	for (int i = 0 ; i < (int)_paxlist.size() ;i ++)
	{
		if (_paxlist[i]->m_pGroupInfo && _paxlist[i]->m_pGroupInfo->IsFollower())		//ignore follower in group
			continue;

		vPerson.push_back(_paxlist[i]);	
	}

	for (int i = 0 ; i < pPaxBusContext->GetPaxBusCount() ;i++) // when flight arrived ,notice all waiting bus to take the passenger 
	{
		CAirsidePaxBusInSim* pPaxBus = pPaxBusContext->GetPaxBus(i);
		if(pPaxBus != NULL)
		{
			if(pax_num == 0)
				pPaxBus->BusMoving(time) ;

			while(!vPerson.empty())
			{
				Person* pPerson = vPerson.front();

				if (!_bus->IsFull())
				{
					AirsidePassengerBehavior* spAirsideBehavior = (AirsidePassengerBehavior*)_paxlist[i]->getBehavior(MobElementBehavior::AirsideBehavior);
					if (spAirsideBehavior)
					{
						_bus->AddPassenger(pPerson) ;
						spAirsideBehavior->SetAirsideBus(_bus) ;
					}
					vPerson.pop_front();
					continue;
				}
				break;
			}
		}
	}
}

bool AirsideArrPassengerBusStrategy::Arrival() const
{
	return true;
}

AirsideDepPassengerBusStrategy::AirsideDepPassengerBusStrategy( AirsideFlightInSim* pAirsideFlightInSim )
:AirsidePassengerBusStrategy(pAirsideFlightInSim)
{

}

AirsideDepPassengerBusStrategy::~AirsideDepPassengerBusStrategy()
{

}

void AirsideDepPassengerBusStrategy::PassengerBusArrive(const ElapsedTime& time,CAirsidePaxBusInSim* pPaxBus,AirsidePassengerBusContext* pPaxBusContext,bool bGenerateBaggage)
{
	if(WakeupHoldPassenger(time,pPaxBus->GetVehicleCapacity()) == 0)
		pPaxBus->BusMoving(time); 
}


void AirsideDepPassengerBusStrategy::FlightArriveStand(const ElapsedTime& time,AirsidePassengerBusContext* pPaxBusContext,bool bGenerateBaggage)
{
	if (m_pAirsideFlightInSim == NULL)
		return;

	Flight* pFlight = m_pAirsideFlightInSim->GetFlightInput();
	if (pFlight == NULL)
		return;

	CARCportEngine* pArcportEngine = m_pAirsideFlightInSim->GetEngine();
	if (pArcportEngine == NULL)
		return;

	Terminal* pTerminal = pArcportEngine->getTerminal();
	if (pTerminal == NULL)
		return;

	if(pArcportEngine->IsAirsideSel())
	{
		FlightServiceReqirementInSim* requestSim =	m_pAirsideFlightInSim->GetAirTrafficController()->GetFlightServiceRequirement() ;
		if( (m_pAirsideFlightInSim->GetServiceRequestDispatcher()->HasVehicleServiceFlight(m_pAirsideFlightInSim,VehicleType_PaxTruck)
			&& m_pAirsideFlightInSim->HasBrrdgeServer(FALSE) ))
		{
			for (int i = 0 ; i < pPaxBusContext->GetPaxBusCount() ;i++) // when flight arrived ,notice all waiting bus to take the passenger 
			{
				CAirsidePaxBusInSim* pPaxBus = pPaxBusContext->GetPaxBus(i) ;
				if(WakeupHoldPassenger(time,pPaxBus->GetVehicleCapacity()) == 0)
					pPaxBus->BusMoving(time); 
			}
		}
	}
}

int AirsideDepPassengerBusStrategy::WakeupHoldPassenger( const ElapsedTime& time ,int nPaxCount )
{
	if (m_pAirsideFlightInSim == NULL)
		return 0;

	Flight* pFlight = m_pAirsideFlightInSim->GetFlightInput();
	if (pFlight == NULL)
		return 0;
	
	CARCportEngine* pArcportEngine = m_pAirsideFlightInSim->GetEngine();
	if (pArcportEngine == NULL)
		return 0;
	
	Terminal* pTerminal = pArcportEngine->getTerminal();
	if (pTerminal == NULL)
		return 0;
	
	ProcessorArray vHoldingAreas;
	pTerminal->procList->getProcessorsOfType (HoldAreaProc, vHoldingAreas);
	int iHoldAreaCount = vHoldingAreas.getCount();

	CMobileElemConstraint paxType(pTerminal);
	int nReleaseCount = 0 ;
	for (int i = 0 ; i < iHoldAreaCount; i++)
	{
		HoldingArea* pHoldArea =(HoldingArea*) vHoldingAreas.getItem(i);
		FlightConstraint fltcnst = pFlight->getType('D');
		fltcnst.SetAirportDB(paxType.GetAirportDB());
		FlightConstraint& paxCnst = paxType;
		paxCnst = fltcnst;
		nReleaseCount = nReleaseCount + pHoldArea->releasePaxToAirside(paxType , nPaxCount ,time ) ;
	}
	return nReleaseCount ;
}

bool AirsideDepPassengerBusStrategy::Arrival() const
{
	return false;
}


AirsidePassengerBusContext::AirsidePassengerBusContext()
:m_bGenerateMobile(false)
,m_pPaxBusStrategy(NULL)
{

}

AirsidePassengerBusContext::~AirsidePassengerBusContext()
{
	m_vArrPaxBusList.clear();
	m_vDepPaxBusList.clear();
	m_vPerson.clear();
}

void AirsidePassengerBusContext::PassengerBusArrive(const ElapsedTime& time,CAirsidePaxBusInSim* pPaxBus,bool bGenerateBaggage)
{
	if (m_pPaxBusStrategy == NULL)
		return;

	if (m_pPaxBusStrategy->Arrival())
	{
		std::vector<CAirsidePaxBusInSim*>::const_iterator iter = std::find(m_vArrPaxBusList.begin(),m_vArrPaxBusList.end(),pPaxBus);
		if (iter == m_vArrPaxBusList.end())
		{
			m_vArrPaxBusList.push_back(pPaxBus);
		}
	}
	else
	{
		std::vector<CAirsidePaxBusInSim*>::const_iterator iter = std::find(m_vDepPaxBusList.begin(),m_vDepPaxBusList.end(),pPaxBus);
		if (iter == m_vDepPaxBusList.end())
		{
			m_vDepPaxBusList.push_back(pPaxBus);
		}
	}

	m_pPaxBusStrategy->PassengerBusArrive(time,pPaxBus,this,bGenerateBaggage);
}

void AirsidePassengerBusContext::PassengerBusLeave(CAirsidePaxBusInSim* pPaxBus)
{
	if (m_pPaxBusStrategy == NULL)
		return;

	if (m_pPaxBusStrategy->Arrival())
	{
		std::vector<CAirsidePaxBusInSim*>::iterator iter = std::find(m_vArrPaxBusList.begin(),m_vArrPaxBusList.end(),pPaxBus);
		if (iter != m_vArrPaxBusList.end())
		{
			m_vArrPaxBusList.erase(iter);
		}
	}
	else
	{
		std::vector<CAirsidePaxBusInSim*>::iterator iter = std::find(m_vDepPaxBusList.begin(),m_vDepPaxBusList.end(),pPaxBus);
		if (iter != m_vDepPaxBusList.end())
		{
			m_vDepPaxBusList.erase(iter);
		}
	}

}

void AirsidePassengerBusContext::FlightArriveStand(const ElapsedTime& time,bool bGenerateBaggage)
{
	if (m_pPaxBusStrategy)
	{
		m_pPaxBusStrategy->FlightArriveStand(time,this,bGenerateBaggage);
	}
}

void AirsidePassengerBusContext::FlightLeaveStand()
{
	if (m_pPaxBusStrategy == NULL)
		return;

	if (m_pPaxBusStrategy->Arrival())
	{
		m_vArrPaxBusList.clear();
	}
	else
	{
		m_vDepPaxBusList.clear();
	}
}

CAirsidePaxBusInSim* AirsidePassengerBusContext::GetAvailablePassengerBus(Person* pPerson)
{
	if (m_pPaxBusStrategy == NULL)
		return NULL;
	
	if (m_pPaxBusStrategy->Arrival())
	{
		for (unsigned i = 0; i < m_vArrPaxBusList.size(); i++)
		{
			CAirsidePaxBusInSim* pPaxBus = m_vArrPaxBusList.at(i);
			if (pPaxBus->havePerson(pPerson) || !pPaxBus->IsFull())
			{
				return pPaxBus;
			}
		}
	}
	else
	{
		for (unsigned i = 0; i < m_vDepPaxBusList.size(); i++)
		{
			CAirsidePaxBusInSim* pPaxBus = m_vDepPaxBusList.at(i);
			if (pPaxBus->havePerson(pPerson) || !pPaxBus->IsFull())
			{
				return pPaxBus;
			}
		}
	}

	return NULL;
}

void AirsidePassengerBusContext::SetPaxBusStrategy(AirsidePassengerBusStrategy* pPaxBusStrategy)
{
	m_pPaxBusStrategy = pPaxBusStrategy;
}

int AirsidePassengerBusContext::GetPaxBusCount() const
{
	if (m_pPaxBusStrategy == NULL)
		return 0;
	
	if (m_pPaxBusStrategy->Arrival())
	{
		return (int)m_vArrPaxBusList.size();
	}
	
	return (int)m_vDepPaxBusList.size();
}

CAirsidePaxBusInSim* AirsidePassengerBusContext::GetPaxBus( int idx )
{
	if (m_pPaxBusStrategy == NULL)
		return NULL;

	if (idx >= 0 && idx < GetPaxBusCount())
	{
		if (m_pPaxBusStrategy->Arrival())
		{
			return m_vArrPaxBusList.at(idx);
		}
		else
		{
			return m_vDepPaxBusList.at(idx);
		}
	}
	
	return NULL;
}

bool AirsidePassengerBusContext::HaveGenerateMobileElment() const
{
	return m_bGenerateMobile;
}

void AirsidePassengerBusContext::GenerateMobileElement()
{
	m_bGenerateMobile = true;
}

void AirsidePassengerBusContext::AddPax( Person* pPerson )
{
	if (pPerson == NULL)
		return;
	
	if (std::find(m_vPerson.begin(),m_vPerson.end(),pPerson) != m_vPerson.end())
	{
		m_vPerson.push_back(pPerson);
	}
}

void AirsidePassengerBusContext::SetPaxListInFlight( const std::vector<Person*>& vPaxList )
{
	if (HaveGenerateMobileElment())
		return;

	GenerateMobileElement();
	m_vPerson = vPaxList;
}

void AirsidePassengerBusContext::ProcessPassengerTakeonBus( const ElapsedTime& time,CAirsidePaxBusInSim* pPaxBus )
{
	if(m_vPerson.empty())
		pPaxBus->BusMoving(time) ;

	while(!m_vPerson.empty())
	{
		Person* pPerson = m_vPerson.back();
		if (!pPaxBus->IsFull())
		{
			AirsidePassengerBehavior* spAirsideBehavior = (AirsidePassengerBehavior*)pPerson->getBehavior(MobElementBehavior::AirsideBehavior);
			if (spAirsideBehavior)
			{
				pPaxBus->AddPassenger(pPerson);
				spAirsideBehavior->SetAirsideBus(pPaxBus);
				spAirsideBehavior->WakeupHoldArea(time);
			}

			m_vPerson.pop_back();
			continue;
		}
		break;
	}
}