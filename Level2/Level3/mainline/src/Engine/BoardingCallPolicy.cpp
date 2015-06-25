#include "StdAfx.h"
#include ".\boardingcallpolicy.h"
#include "BusBoardingCall.h"
#include "Airside/AirsideSimulation.h"
#include "Airside/FlightServiceReqirementInSim.h"
#include "Airside/AirsidePaxBusInSim.h"
#include "Airside/PaxBusServiceGateEvent.h"
#include "Airside/FlightServiceEvent.h"
#include "Airside/VehicleRequestDispatcher.h"
#include "MobElementLifecycleManager.h"
#include "AirsideMobElementBehavior.h"
#include "ARCportEngine.h"
#include "TerminalMobElementBehavior.h"
#include "OnboardSimulation.h"
#include "OnboardFlightInSim.h"

CPolicyData::CPolicyData(bool bArrial):m_flight(NULL),m_isBusArrived(FALSE),m_isFlightArrived(FALSE)
{
	m_bArrFlight = bArrial;
}
CPolicyData::CPolicyData(Flight* _fli,bool bArrial):m_flight(_fli),m_isBusArrived(FALSE),m_isFlightArrived(FALSE)
{
	m_bArrFlight = bArrial;
}
CPolicyData::~CPolicyData()
{

}
void CPolicyData::RemoveArriveBus(CAirsidePaxBusInSim* _bus) 
{
	if(_bus == NULL)
		return ;
	std::vector<CAirsidePaxBusInSim*>::iterator iter =  std::find( m_Pbus.begin() ,m_Pbus.end(),_bus) ;
	if(iter != m_Pbus.end())
		m_Pbus.erase(iter) ;
}
CPolicyData::CPolicyData(const CPolicyData& _policydata)
{
	m_Pbus = _policydata.m_Pbus ;
	m_isFlightArrived = _policydata.m_isFlightArrived ;
	m_isBusArrived = _policydata.m_isBusArrived ;
	m_flight = _policydata.m_flight ;
}
void CPolicyData::SetArrivedBus(CAirsidePaxBusInSim* _bus)
{
	std::vector<CAirsidePaxBusInSim*>::iterator iter =  std::find( m_Pbus.begin() ,m_Pbus.end(),_bus) ;
	if(iter == m_Pbus.end())
	   m_Pbus.push_back(_bus) ;
}
std::vector<CAirsidePaxBusInSim*>* CPolicyData::GetArrivedBus()
{
		return &m_Pbus ;
}
CAirsidePaxBusInSim* CPolicyData::GetCanServerBus(const Person* pPerson) 
{
	if (m_Pbus.empty())
		return NULL;

	for (int i = 0; i < (int)m_Pbus.size(); i++)
	{
		if (m_Pbus[i]->havePerson(pPerson))
		{
			return m_Pbus[i];
		}

		if(!m_Pbus[i]->IsFull())
		{
			return m_Pbus[i];
		}
	}
		
	return NULL;
	//if(m_Pbus.size() == 0 )
	//	return NULL ;
	//else
	//{
	//	for (int i = 0 ; i < (int)m_Pbus.size() ;i++)
	//	{
	//		if(!m_Pbus[i]->IsFull())
	//		{
	//			return m_Pbus[i] ;
	//		}
	//	}
	//	return NULL ;
	//}
}
void CPolicyData::SetFlight(Flight* _fli)
{
	m_flight = _fli ;
}
Flight* CPolicyData::GetFlight()
{
	return m_flight ;
}

BOOL CPolicyData::GetIsBusArrived()
{
	return m_Pbus.size() ;
}
void CPolicyData::SetIsFlightArrived(BOOL _res)
{
	m_isFlightArrived = _res ;
}
BOOL CPolicyData::GetIsFlightArrived()
{
	return m_isFlightArrived ;
}

bool CPolicyData::arrivalFlight()const
{
	return m_bArrFlight;
}

void CPolicyData::arrivalFlight(bool bArrival)
{
	m_bArrFlight = bArrival;
}

CPolicyDataContainer::CPolicyDataContainer()
{

}
CPolicyDataContainer::~CPolicyDataContainer()
{
	for (int i = 0 ; i < (int)m_Policy.size() ; ++i)
	{
		if(m_Policy[i] != NULL)
			delete m_Policy[i] ;
	}
}

CPolicyData* CPolicyDataContainer::FindPolicyData(Flight* _FliId,bool bArrial)
{
	int cout = (int)m_Policy.size() ;
	for (int i = 0; i < cout ; ++i)
	{
		if(m_Policy[i]->GetFlight()->getFlightIndex() == _FliId->getFlightIndex()
			&& bArrial == m_Policy[i]->arrivalFlight())
			return m_Policy[i] ;
	}
	CPolicyData* p_policy = new CPolicyData(_FliId,bArrial) ;
	m_Policy.push_back(p_policy) ;
	return p_policy ;

}
CBoardingCallPolicy::CBoardingCallPolicy()
{

}
CBoardingCallPolicy::~CBoardingCallPolicy()
{}
BOOL CBoardingCallPolicy::FlightIsArrived(Flight* fli,bool bArrival)
{
	return FindPolicyData(fli,bArrival)->GetIsFlightArrived() ;
}
BOOL CBoardingCallPolicy::BusIsArrived(Flight* fli,bool bArrival)
{
	return FindPolicyData(fli,bArrival)->GetIsBusArrived() ;
}
BOOL CBoardingCallPolicy::CanTakeOnTheFlight(Flight* fli, bool bArrival,CARCportEngine*_pEngine)
{
	//Terminal* _terminal = fli->GetTerminal() ;
	//flight have not arrived
	if(!FlightIsArrived(fli,bArrival))
		return FALSE ;
	if(!_pEngine->IsAirsideSel())
			return TRUE ;
	else
	{
          if( BusIsArrived(fli,bArrival) )
			  return TRUE ;
		  else
		  {
			
			  if(!HaveBusServer(fli,bArrival,_pEngine))
				  return TRUE ;
			  else
				  return FALSE ;
		  }

	}
}
BOOL CBoardingCallPolicy::HaveBusServer(Flight* fli,bool bArrival, CARCportEngine*_pEngine)
{
	ASSERT(_pEngine != NULL);
	if(_pEngine == NULL)
		return FALSE;

	AirsideFlightInSim* Airflight = _pEngine->GetAirsideSimulation()->GetAirsideFlight(fli->getFlightIndex()) ;
	ASSERT(Airflight) ;
	FlightServiceReqirementInSim* requestSim =	Airflight->GetAirTrafficController()->GetFlightServiceRequirement() ;
	return requestSim->HavePaxTruckForFlight(Airflight) ;
}
void CBoardingCallPolicy::NoticeBusHasArrived(Flight* fli,bool bArrival,ElapsedTime curTime,CAirsidePaxBusInSim* p_bus, CARCportEngine*_pEngine)
{
	CPolicyData* p_policy = FindPolicyData(fli,bArrival) ;
	p_policy->SetArrivedBus(p_bus) ;
	HandleBusArrivalForDep(p_policy,curTime,p_bus,_pEngine);

}
void CBoardingCallPolicy::NoticeFlightArrived(Flight* fli,bool bArrival,ElapsedTime curTime, CARCportEngine*_pEngine)
{
	CPolicyData* p_policy = FindPolicyData(fli,bArrival) ;
	p_policy->SetIsFlightArrived(TRUE) ;
    HandleFlightArrivalForDep(p_policy,curTime,_pEngine);
}
void CBoardingCallPolicy::NoticeBusHasLeaved(Flight* fli,bool bArrival,CAirsidePaxBusInSim* p_bus)
{
	CPolicyData* p_policy = FindPolicyData(fli,bArrival) ;
    p_policy-> RemoveArriveBus(p_bus) ;
}
void CBoardingCallPolicy::NoticeFlightHasLeaved(Flight* fli,bool bArrival)
{
		CPolicyData* p_policy = FindPolicyData(fli,bArrival) ;
		p_policy->SetIsFlightArrived(FALSE) ;
}
void CBoardingCallPolicy::HandleBusArrivalForDep(CPolicyData* _policydata,ElapsedTime curTime,CAirsidePaxBusInSim* _bus, CARCportEngine*_pEngine )
{
	ASSERT(_pEngine != NULL);
	if(_pEngine == NULL)
		return;

	Flight* pFlight = _policydata->GetFlight() ;
	//Terminal* m_terminal = pFlight->GetTerminal() ;
	if(_pEngine->IsAirsideSel())
	{
		AirsideFlightInSim* Airflight = _pEngine->GetAirsideSimulation()->GetAirsideFlight(_policydata->GetFlight()->getFlightIndex()) ;
		ASSERT(Airflight) ;
		FlightServiceReqirementInSim* requestSim =	Airflight->GetAirTrafficController()->GetFlightServiceRequirement() ;
		if(!_policydata->GetIsFlightArrived() )
		{
			_policydata->SetArrivedBus(_bus) ;
			return ; 
		}
		CBusBoardingCall boardingcall(_pEngine->getTerminal()) ;
		int cout =  boardingcall.BoardingCallHoldingArea(pFlight,curTime,_bus->GetVehicleCapacity(),_bus) ;
		if(cout == 0)
			_bus->BusMoving(curTime) ;
	}
}
BOOL CBoardingCallPolicy::HandleFlightArrivalForDep(CPolicyData* _policydata,ElapsedTime curTime, CARCportEngine*_pEngine)
{
	ASSERT(_pEngine != NULL);
	if(_pEngine == NULL)
		return FALSE;

	Flight* pFlight = _policydata->GetFlight() ;
	CAirsidePaxBusInSim* _bus = NULL ; 
	Terminal* m_terminal = pFlight->GetTerminal() ;
	ASSERT(pFlight) ;
	if(pFlight == NULL)
		return FALSE; 
	if(_pEngine->IsAirsideSel())
	{
		AirsideFlightInSim* Airflight = _pEngine->GetAirsideSimulation()->GetAirsideFlight(_policydata->GetFlight()->getFlightIndex()) ;
		ASSERT(Airflight) ;
	    FlightServiceReqirementInSim* requestSim =	Airflight->GetAirTrafficController()->GetFlightServiceRequirement() ;
		if( (Airflight->GetServiceRequestDispatcher()->HasVehicleServiceFlight(Airflight,VehicleType_PaxTruck) && Airflight->HasBrrdgeServer(FALSE) )&& _policydata->GetIsFlightArrived())
		{
			for (int i = 0 ; i <(int) _policydata->GetArrivedBus()->size() ;i++) // when flight arrived ,notice all waiting bus to take the passenger 
			{
				_bus =  _policydata->GetArrivedBus()->at(i) ;
				CBusBoardingCall boardingcall(m_terminal) ;
				int cout =  boardingcall.BoardingCallHoldingArea(pFlight,curTime,_bus->GetVehicleCapacity(),_bus) ;
 				if(cout == 0)
 					_bus->BusMoving(curTime) ;
			}
		}
	}
// 	else
// 	{
// 		if(_policydata->GetIsFlightArrived())
// 		{
// 			_pEngine->ReflectOnFlightArriveStand(pFlight ,curTime) ;
// 		}
// 	}
	return TRUE ;
}
CPassengerTakeOffCallPolicy::CPassengerTakeOffCallPolicy():CPolicyDataContainer()
{

}
CPassengerTakeOffCallPolicy::~CPassengerTakeOffCallPolicy()
{
	for (size_t i = 0; i < m_vFlightPaxList.size(); i++)
	{
		delete m_vFlightPaxList[i];
	}
	m_vFlightPaxList.clear();
}

bool CPassengerTakeOffCallPolicy::AlreadyGeneratePassengerforFlight(const Flight* pFlight)const
{
	for (size_t i = 0; i < m_vFlightPaxList.size(); i++)
	{
		if (pFlight == m_vFlightPaxList.at(i)->getFlight())
		{
			return true;
		}
	}

	return false;
}

int CPassengerTakeOffCallPolicy::GetIndexOfFlightInfo(const Flight* pFlight)const
{
	for (int i = 0; i < (int)m_vFlightPaxList.size(); i++)
	{
		if (pFlight == m_vFlightPaxList.at(i)->getFlight())
		{
			return i;
		}
	}

	return -1;
}

void CPassengerTakeOffCallPolicy::NoticeBusHasArrived(Flight* fli,bool bArrival,ElapsedTime curTime,CAirsidePaxBusInSim* p_bus, CARCportEngine*_pEngine)
{
	Terminal* m_terminal = fli->GetTerminal() ;
	CPolicyData* p_policy = FindPolicyData(fli,bArrival)  ;
	HandBusHasArrived(p_policy,curTime,p_bus,_pEngine) ;
}

void CPassengerTakeOffCallPolicy::NoticeFlightArrived(Flight* fli,bool bArrival,ElapsedTime curTime, CARCportEngine*_pEngine)
{
	CPolicyData* p_poilcy = FindPolicyData(fli,bArrival) ;
	p_poilcy->SetIsFlightArrived(TRUE) ;

	CPaxGenerator paxgenerator(_pEngine) ;
	
	std::vector<Person *> vBags;
	paxgenerator.GenerateDelayMobileBag(fli->getFlightIndex(),curTime, vBags, false) ;

	HandleFlightArrived(p_poilcy,curTime,_pEngine) ;
}

void CPassengerTakeOffCallPolicy::HandBusHasArrived(CPolicyData* _policydata , ElapsedTime curTime,CAirsidePaxBusInSim* _arrivalBus, CARCportEngine*_pEngine)
{
	if(!_policydata->GetIsFlightArrived()) //if flight has not arrival ,add bus to waiting queue.
	{
		_policydata->SetArrivedBus(_arrivalBus)  ;
		return ;
	}
	Flight* pFlight = _policydata->GetFlight() ;// flight arrived , notice the passengers to take on bus

	std::vector<Person*> _paxlist ;

 	Terminal* m_terminal = pFlight->GetTerminal() ;
 	AirsideFlightInSim* Airflight = _pEngine->GetAirsideSimulation()->GetAirsideFlight(pFlight->getFlightIndex()) ;
 	//check flight person if generate
  	//if ()			
  	{
  		if (_pEngine->IsOnboardSel() 
			&& _pEngine->GetOnboardSimulation() && _pEngine->GetOnboardSimulation()->GetOnboardFlightInSim(Airflight, true))	//on board mode selected 
  		{
			OnboardFlightInSim* pOnboardFlight = _pEngine->GetOnboardSimulation()->GetOnboardFlightInSim(Airflight, true);
			pOnboardFlight->GetPaxListInFlight(_paxlist);
			if (!_paxlist.empty())
			{
				FlightMobileElmentInfo* pNewFlightInfo = new FlightMobileElmentInfo(pFlight);

				for (int paxIndex = 0; paxIndex < (int)_paxlist.size();paxIndex++)
				{
					if (_paxlist[paxIndex]->m_pGroupInfo && _paxlist[paxIndex]->m_pGroupInfo->IsFollower())		//ignore follower in group
						continue;

					pNewFlightInfo->push_back(_paxlist[paxIndex]);
					Airflight->AddPaxInFlight();
				}
				m_vFlightPaxList.push_back(pNewFlightInfo);

				pOnboardFlight->WakeupPassengerDeplane(curTime);
				pOnboardFlight->GenerateCloseDoorEvent(curTime);
			}
  		}
		else		
		{
			if (!AlreadyGeneratePassengerforFlight(pFlight))	//did not generate person of the flight
			{
				CPaxGenerator paxgenerator(_pEngine) ;
				paxgenerator.GenerateDelayMobileElement(pFlight->getFlightIndex(),curTime,_paxlist,false,-1) ;
				Airflight->AddPaxCount((int)_paxlist.size());

				FlightMobileElmentInfo* pNewFlightInfo = new FlightMobileElmentInfo(pFlight);
				for (int paxIndex = 0; paxIndex < (int)_paxlist.size();paxIndex++)
				{
					pNewFlightInfo->push_back(_paxlist[paxIndex]);
				}
				m_vFlightPaxList.push_back(pNewFlightInfo);

			}
		}



  	}
  
  	int nFlightInfoIndex = GetIndexOfFlightInfo(pFlight);
  	FlightMobileElmentInfo* pFlightInfo = m_vFlightPaxList[nFlightInfoIndex];
  
  	if(pFlightInfo->empty())
  		_arrivalBus->BusMoving(curTime) ;
  	
  	while(!pFlightInfo->empty())
  	{
  		Person* pPerson = pFlightInfo->front();
  		if (!_arrivalBus->IsFull())
  		{
			AirsidePassengerBehavior* spAirsideBehavior = (AirsidePassengerBehavior*)pPerson->getBehavior(MobElementBehavior::AirsideBehavior);
			if (spAirsideBehavior)
			{
  				_arrivalBus->AddPassenger(pPerson);
  				spAirsideBehavior->SetAirsideBus(_arrivalBus);
				spAirsideBehavior->WakeupHoldArea(curTime);
			}
		
			pFlightInfo->pop_front();
  			continue;
  		}
  		break;
  	}

}
void CPassengerTakeOffCallPolicy::HandleFlightArrived(CPolicyData* _policydata , ElapsedTime curTime, CARCportEngine*_pEngine)
{
	Flight* pFlight = _policydata->GetFlight() ;
	Terminal* m_terminal = pFlight->GetTerminal() ;
	AirsideFlightInSim* Airflight = _pEngine->GetAirsideSimulation()->GetAirsideFlight(_policydata->GetFlight()->getFlightIndex()) ;
	ASSERT(Airflight) ;
	FlightServiceReqirementInSim* requestSim =	Airflight->GetAirTrafficController()->GetFlightServiceRequirement() ;
    CAirsidePaxBusInSim* _bus = NULL ;

  	std::vector<Person*> _paxlist ;

  	if(_policydata->GetIsFlightArrived() && (Airflight->GetServiceRequestDispatcher() == NULL || !Airflight->GetServiceRequestDispatcher()->HasVehicleServiceFlight(Airflight,VehicleType_PaxTruck) \
  		|| Airflight->HasBrrdgeServer(TRUE)))
  	{ // there is no bus server for this flight or has the bridge server , when flight arrived , notice all passengers to take off directly . 

		if (_pEngine->IsOnboardSel()
			&& _pEngine->GetOnboardSimulation() && _pEngine->GetOnboardSimulation()->GetOnboardFlightInSim(Airflight, true))//on board mode selected
		{
			OnboardFlightInSim* pOnboardFlight = _pEngine->GetOnboardSimulation()->GetOnboardFlightInSim(Airflight, true);
			pOnboardFlight->GetPaxListInFlight(_paxlist);

			for (int paxIndex = 0; paxIndex < (int)_paxlist.size();paxIndex++)
			{
				if (_paxlist[paxIndex]->m_pGroupInfo && _paxlist[paxIndex]->m_pGroupInfo->IsFollower())		//ignore follower in group
					continue;

				Airflight->AddPaxInFlight();
			}

			pOnboardFlight->WakeupPassengerDeplane(curTime);
			pOnboardFlight->GenerateCloseDoorEvent(curTime);
		}
		else			
		{
			CPaxGenerator paxgenerator(_pEngine) ;
			int pax_num = paxgenerator.GenerateDelayMobileElement(pFlight->getFlightIndex(),curTime,_paxlist,false,-1) ;
			Airflight->AddPaxCount(pax_num);
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

	if (_pEngine->IsOnboardSel()
		&& _pEngine->GetOnboardSimulation() && _pEngine->GetOnboardSimulation()->GetOnboardFlightInSim(Airflight, true))	//on board mode selected
	{
		OnboardFlightInSim* pOnboardFlight = _pEngine->GetOnboardSimulation()->GetOnboardFlightInSim(Airflight, true);
		pOnboardFlight->GetPaxListInFlight(_paxlist);

		if (!_paxlist.empty())
		{
			FlightMobileElmentInfo* pNewFlightInfo = new FlightMobileElmentInfo(pFlight);

			for (int paxIndex = 0; paxIndex < (int)_paxlist.size();paxIndex++)
			{
				if (_paxlist[paxIndex]->m_pGroupInfo && _paxlist[paxIndex]->m_pGroupInfo->IsFollower())		//ignore follower in group
					continue;

				pNewFlightInfo->push_back(_paxlist[paxIndex]);
				Airflight->AddPaxInFlight();
			}
			m_vFlightPaxList.push_back(pNewFlightInfo);

			pOnboardFlight->WakeupPassengerDeplane(curTime);
			pOnboardFlight->GenerateCloseDoorEvent(curTime);
		}

	}
	else		
	{
		if (!_policydata->GetArrivedBus()->empty())
		{
			CPaxGenerator paxgenerator(_pEngine) ;
			pax_num = paxgenerator.GenerateDelayMobileElement(pFlight->getFlightIndex(),curTime,_paxlist,false,-1) ;
			Airflight->AddPaxCount(pax_num);
		}
	}

  	std::deque<Person*> vPerson;
  	for (int i = 0 ; i < (int)_paxlist.size() ;i ++)
  	{
		if (_paxlist[i]->m_pGroupInfo && _paxlist[i]->m_pGroupInfo->IsFollower())		//ignore follower in group
			continue;

		vPerson.push_back(_paxlist[i]);	
  	}
  
  	for (int i = 0 ; i < (int)_policydata->GetArrivedBus()->size() ;i++) // when flight arrived ,notice all waiting bus to take the passenger 
  	{
  		_bus =  _policydata->GetArrivedBus()->at(i) ;
  		if(_bus != NULL && _policydata->GetIsFlightArrived())
  		{
  			if(pax_num == 0)
  				_bus->BusMoving(curTime) ;
  			
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
void CPassengerTakeOffCallPolicy::NoticeBusHasLeaved(Flight* fli,bool bArrival,CAirsidePaxBusInSim* p_bus)
{
	CPolicyData* p_policy = FindPolicyData(fli,bArrival) ;
	p_policy->RemoveArriveBus(p_bus) ;
}
void CPassengerTakeOffCallPolicy::NoticeFlightHasLeaved(Flight* fli,bool bArrival)
{
	CPolicyData* p_policy = FindPolicyData(fli,bArrival) ;
	p_policy->SetIsFlightArrived(FALSE) ;
}