#include "StdAfx.h"
#include ".\landsideresidentvehicleinsim.h"
#include "Landside\LandsideResidentVehilceRoute.h"
#include "Landside\RensidentVehicleDeployor.h"
#include "Landside\ResidentVehiclTypeContainer.h"
#include "LandsideSimulation.h"
#include "LandsideResourceManager.h"
#include "LandsideBaseBehavior.h"
#include "ARCportEngine.h"
#include "LandsideBusStationInSim.h"
#include "LandsideResidentVehicleGetOffEvent.h"
#include "PERSON.H"
#include "PaxLandsideBehavior.h"
#include "Landside\State_InParkingLot.h"

LandsideResidentVehicleInSim::LandsideResidentVehicleInSim( RensidentVehicleDeployorInSim* pDeploy )
{
	m_TypeName.fromString(pDeploy->m_pData->GetVehicleType());	
	m_curMode = _onBirth;
	m_pCurPlan = NULL;
	m_pDeployer = pDeploy;

	m_bPaxGetoffDone = false;

	m_serviceState = _preService;
	m_bAvaible = false;
}


bool LandsideResidentVehicleInSim::InitBirth( CARCportEngine *pEngine )
{	
	if(!__super::InitBirth(pEngine))
	{
		return false;
	}
	ChangeDest(NULL);
	//assign the vehicle to home base
	if(LandsideFacilityObject* pFacility  = m_pDeployer->getHomeBase())
	{
		LandsideLayoutObjectInSim* pObjInSIm = pEngine->GetLandsideSimulation()->GetResourceMannager()->getLayoutObjectInSim(pFacility->GetFacilityID());
		if(pObjInSIm)
		{
			ChangeDest(pObjInSIm);			
			return InitBirthInLayoutObject(getDestLayoutObject(),pEngine);
		}
	}
	return false;
}

void LandsideResidentVehicleInSim::RetrieveStoppedBusStation( LandsideSimulation* pSim)
{
	if(!m_pCurPlan)
		return;
	//the station that the vehicle could lead to
	//get the bus station the passenger could lead to
	//search over the structure, find all the stations
	m_vBusStationCouldLeadTo.clear();

	ResidentVehicleRouteFlow* pVehicleRouteFlow = m_pCurPlan->GetVehicleRouteFlow();
	ASSERT(pVehicleRouteFlow);


	LandsideFacilityObject* fcObj = m_pCurPlan->GetServiceStartObject();
	while(fcObj)
	{		
		LandsideLayoutObjectInSim* pLayoutObj = pSim->GetResourceMannager()->getLayoutObjectInSim(fcObj->GetFacilityID());
		if(pLayoutObj && pLayoutObj->toBusSation())
			m_vBusStationCouldLeadTo.push_back(pLayoutObj->toBusSation());

		LandsideFacilityObject _fcObject = *fcObj;	
		if(_fcObject.IsServiceStart())
			_fcObject.SetFacilityID(-1); //fix service_start can not find route list

		ResidentVehicleRouteList* routelist = pVehicleRouteFlow->GetDestVehicleRoute(_fcObject);
		if(routelist)
		{
			ResidentVehicleRoute* pNextItem = pVehicleRouteFlow->getChildLeadtoServiceStart(routelist);
			fcObj = pNextItem->GetFacilityObject();
		}	
		else
			fcObj = NULL;

		if(fcObj && fcObj->IsServiceStart() )
			break;
	}
}





bool LandsideResidentVehicleInSim::BeginServicePlan( ResidentRelatedVehicleTypePlan* pPlan ,LandsideSimulation* pSim )
{
	m_pCurPlan = pPlan;
	RetrieveStoppedBusStation(pSim);
	Activate(curTime());	
	return true;
}


void LandsideResidentVehicleInSim::PaxTakeOffTheVehicle(LandsideBusStationInSim* pBusStation , ElapsedTime& eEventTime, CARCportEngine* pEngine)
{
	//release the passengers in the vehicle who will take off at this station

	ElapsedTime eTimePerPax = ElapsedTime(5L);
	int nPaxTookOff = 0;
	std::vector<LandsideBaseBehavior*>::iterator iterPax = m_vOnVehiclePax.begin();
	for (; iterPax != m_vOnVehiclePax.end(); ++iterPax)
	{
		LandsideBaseBehavior *pPax = *iterPax;
		if(pPax == NULL)
			continue;
		if(pPax->CanPaxTkeOffThisBusAtStation(pBusStation, pEngine->GetLandsideSimulation()))
		{
			//the destination station
			pPax->setResource(pBusStation);
			pPax->setState(GetOffVehicle);
			eEventTime += eTimePerPax;
			pPax->GenerateEvent(eEventTime);
		}
	}
}



bool LandsideResidentVehicleInSim::CouldTakeToStation( ALTObjectID altDesination )
{
	//here will check 2 object
	//one is the bus station, and other one is the parking lot linked to this station
	std::vector<LandsideBusStationInSim* >::iterator iterStation =  m_vBusStationCouldLeadTo.begin();
	for (; iterStation != m_vBusStationCouldLeadTo.end(); ++ iterStation)
	{
		LandsideBusStationInSim* pThisStation = *iterStation;
		if(pThisStation == NULL)
			continue;
		if(pThisStation->getInput()->getName() == altDesination)
			return true;

		//check linked parking lot
		LandsideParkingLotInSim *pLinkedPakringLot = pThisStation->getEmbeddedParkingLotInSim();
		if(pLinkedPakringLot)
		{
			if(pLinkedPakringLot->getInput()->getName() == altDesination)
				return true;
		}
	}
	return false;
}

void LandsideResidentVehicleInSim::NotifyPaxMoveInto(CARCportEngine*pEngine,  LandsideBaseBehavior* pPassenger, const ElapsedTime& eTime )
{
	//check the vehicle ready to move
	Activate(eTime);
}

bool LandsideResidentVehicleInSim::IsAllPassengersReady()
{
	std::vector<LandsideBaseBehavior*>::iterator iterPax = m_vOnVehiclePax.begin();
	for (; iterPax != m_vOnVehiclePax.end(); ++iterPax)
	{
		LandsideBaseBehavior *pPax = *iterPax;
		if(pPax == NULL)
			continue;
		if(pPax->getState() != StayOnVehicle)
			return false;
	}
	return true;
}



void LandsideResidentVehicleInSim::SetPaxGetoffDoneTag( bool bGetoffDone )
{
	m_bPaxGetoffDone = bGetoffDone;
}

bool LandsideResidentVehicleInSim::GetPaxGetoffDoneTag() const
{
	return m_bPaxGetoffDone;
}

void LandsideResidentVehicleInSim::GeneratePaxGetOffDoneTag( const ElapsedTime& eEventTime )
{
	LandsideResidentVehicleGetOffEvent* pGetoffEvent = new LandsideResidentVehicleGetOffEvent(this, eEventTime);
	pGetoffEvent->addEvent();
}

bool LandsideResidentVehicleInSim::CapacityAvailable( PaxLandsideBehavior *pBehavior ) const
{
	int nPaxCount = 0;
	std::vector<LandsideBaseBehavior*>::const_iterator iterPax = m_vOnVehiclePax.begin();
	for (; iterPax != m_vOnVehiclePax.end(); ++iterPax)
	{
		LandsideBaseBehavior *pPax = *iterPax;
		Person* pPerson = pPax->getPerson();
		if (pPerson == NULL)
			continue;

		nPaxCount += pPerson->GetAdminGroupSize();
	}

	Person* pGetonPax = pBehavior->getPerson();
	if (pGetonPax == NULL)
		return false;
	
	int nTotalCount = nPaxCount + pGetonPax->GetAdminGroupSize();
	return nTotalCount > m_nCapacity ? false : true;
}






ElapsedTime LandsideResidentVehicleInSim::GetTurnAroundTime() const
{
	if(m_pDeployer)
	{
		return m_pDeployer->getParkingTime();
	}
	return ElapsedTime(0L);
}


LandsideLayoutObjectInSim* LandsideResidentVehicleInSim::_getLayoutObject(LandsideFacilityObject& obj, CARCportEngine* pEngine)
{
	LandsideResourceManager* pResM = pEngine->GetLandsideSimulation()->GetResourceMannager();
	int nFcId = -1;
	if(obj.IsServiceStart())
	{
		nFcId = m_pCurPlan->GetServiceStartObject()->GetFacilityID();
	}
	else if(obj.IsHomeBase())
	{
		nFcId = m_pDeployer->getHomeBase()->GetFacilityID();
	}
	else
		nFcId = obj.GetFacilityID();

	return pResM->getLayoutObjectInSim(nFcId);
}


bool LandsideResidentVehicleInSim::ProceedToNextFcObject( CARCportEngine* pEngine )
{
	ResidentVehicleRouteFlow* pVehicleRouteFlow = m_pCurPlan->GetVehicleRouteFlow();
	if (pVehicleRouteFlow == NULL)
		return false;
	if(!getDestLayoutObject())
		return false;

	ResidentVehicleRouteList* pCurItem = NULL;
	if(m_pCurPlan->GetServiceStartObject()->GetFacilityID()== getDestLayoutObject()->getID() )
	{
		LandsideFacilityObject serviceStart = *m_pCurPlan->GetServiceStartObject();
		serviceStart.SetFacilityID(-1);
		pCurItem = pVehicleRouteFlow->GetDestVehicleRoute(serviceStart);
	}
	else if(m_pDeployer->getHomeBase()->GetFacilityID() == getDestLayoutObject()->getID())
	{
		LandsideFacilityObject homebase = *m_pDeployer->getHomeBase();
		homebase.SetFacilityID(-1);
		pCurItem = pVehicleRouteFlow->GetDestVehicleRoute(homebase);
	}
	else 
		pCurItem = pVehicleRouteFlow->GetDestVehicleRoute( getDestLayoutObject()->getID() );

	if(!pCurItem)
		return false;
	
	ResidentVehicleRoute* pNextItem = NULL;
	if(m_serviceState!=_endOfService)
	{
		pNextItem = pVehicleRouteFlow->getChildLeadtoServiceStart(pCurItem);
	}
	else
		pNextItem = pVehicleRouteFlow->getChildLeadtoHomeBase(pCurItem);

	if(pNextItem)
	{
		LandsideLayoutObjectInSim* pObj = _getLayoutObject(*pNextItem->GetFacilityObject(),pEngine);//pEngine->GetLandsideSimulation()->GetResourceMannager()->getLayoutObjectInSim(pNextItem->GetFacilityObject()->GetFacilityID());
		if(pObj)
		{
			ChangeDest(pObj);
			return true;
		}
	}
	return false;
	
}

void LandsideResidentVehicleInSim::ArrivalLayoutObject( LandsideLayoutObjectInSim* pObj,LandsideResourceInSim* pDetailRes, CARCportEngine* pEngine )
{
	if(LandsideBusStationInSim* pStation = pObj->toBusSation())
	{
		if(m_serviceState == _preService && IsServiceStart(pStation) ) 
		{
			SetServiceState(_inService);
		}
		if(GetServiceState() != _inService)  //not in service , skip all stations
		{
			if(pObj->toBusSation())
			{
				SkipProcessLayoutObject(pObj,pEngine);
			}
			else
				SuccessProcessAtLayoutObject(pObj, pDetailRes, pEngine);
			return;
		}				
	}
	__super::ArrivalLayoutObject(pObj,pDetailRes,pEngine);
}

bool LandsideResidentVehicleInSim::IsWaitOnBase() const
{
	return GetServiceState() == _waitOnbase;
}

void LandsideResidentVehicleInSim::SuccessParkInBusStation( LandsideBusStationInSim* pStation,IParkingSpotInSim* spot )
{
	ChangeVehicleState(new State_ProcessAtBusStationResidentVehicle(this,pStation,spot) );
}

bool LandsideResidentVehicleInSim::IsServiceStart( LandsideBusStationInSim* pStation )
{
	if(m_pCurPlan)
	{
		return m_pCurPlan->GetServiceStartObject()->GetFacilityID() == pStation->getID();
	}
	return false;
}

ResidentVehiclePlan* LandsideResidentVehicleInSim::getRoutePlan()
{
	return m_pCurPlan;
}

void LandsideResidentVehicleInSim::SuccessBirthInLotSpot( LandsideParkingSpotInSim* spot, CARCportEngine* pEngine )
{
	IState* pNextState = new State_ParkingInHomeBaseResidentVehicle(this,spot,true);
	ChangeState(pNextState, pEngine);
}
void LandsideResidentVehicleInSim::SuccessParkInLotSpot( LandsideParkingSpotInSim* spot )
{
	IState* pNextState = new State_ParkingInHomeBaseResidentVehicle(this,spot);
	ChangeVehicleState(pNextState);
}
//////////////////////////////////////////////////////////////////////////
void State_ProcessAtBusStationResidentVehicle::Entry( CARCportEngine* pEngine )
{
	m_pOwner->SetAvailable(false);//make it not picking first
	//drop pax
	ElapsedTime eTimeAfterPaxTakeOff = curTime();
	m_pOwner->PaxTakeOffTheVehicle(m_pBusStation,eTimeAfterPaxTakeOff, pEngine);
	m_allPaxGetOffTimer.StartTimer(this,eTimeAfterPaxTakeOff);


	//keep head way time
	if(m_bIsServieStartStation)
	{
		ElapsedTime lastServiceTime;
		if(m_pBusStation->GetLastServiceTime(m_pOwner->GetCurrentPlan(),lastServiceTime) )
		{
			ElapsedTime nextServiceTime = lastServiceTime + m_pOwner->GetCurrentPlan()->GetHeadwayTime();
			m_pBusStation->SetLastServiceTime(m_pOwner->GetCurrentPlan(),nextServiceTime);
			if(curTime()<nextServiceTime)
                m_headwayTimer.StartTimer(this, nextServiceTime);
		}	
		else
			m_pBusStation->SetLastServiceTime(m_pOwner->GetCurrentPlan(),curTime());
	}

	//
	Execute(pEngine);
}

void State_ProcessAtBusStationResidentVehicle::Execute( CARCportEngine* pEngine )
{
	if(!m_allPaxGetOffTimer.isEnded()) //wait all pax get off
		return;

	if(CheckEndOfServiceLoop(pEngine)) //check if not need to start service
		return;

	if(!m_headwayTimer.isEnded()) //wait headway time to start service
		return;

	
	ASSERT(m_pOwner->GetServiceState()== _inService);
	
	if(!m_pOwner->IsAvailable()) //start picking service
	{
		m_pOwner->SetAvailable(true);
		m_minServiceTimer.StartTimer(this,curTime()+ElapsedTime(60L));
		m_pBusStation->AddWaitingBus(m_pOwner);
		m_pBusStation->NoticeWaitingPax(curTime(), m_pOwner, pEngine->GetLandsideSimulation());
		return;
	}

	if(!m_minServiceTimer.isEnded()) //wait min service time
		return;

	if(!m_pOwner->IsAllPassengersReady())
	{
		return;
	}

	m_pOwner->SetAvailable(false);
	//end of service begin 
	m_pOwner->SuccessProcessAtLayoutObject(m_pBusStation,m_spot, pEngine);
}

State_ProcessAtBusStationResidentVehicle::State_ProcessAtBusStationResidentVehicle( LandsideResidentVehicleInSim* pV,LandsideBusStationInSim* pStation,IParkingSpotInSim* spot ) 
:State_LandsideVehicle<LandsideResidentVehicleInSim>(pV)
{
	m_pBusStation = pStation;
	m_spot = spot;
	m_bIsServieStartStation = m_pOwner->IsServiceStart(pStation);
	m_headwayTimer.SetEnd(true);
}

bool State_ProcessAtBusStationResidentVehicle::CheckEndOfServiceLoop( CARCportEngine* pEngine )
{
	if(m_bIsServieStartStation)
	{
		//If service start need to end service at time
		if( curTime()>= m_pOwner->GetCurrentPlan()->GetTimeRange().GetEndTime() )
		{
			m_pOwner->SetServiceState(_endOfService);
			m_pOwner->SuccessProcessAtLayoutObject(m_pBusStation,m_spot,pEngine);
			return true;
		}
	}
	return false;
}

	
//////////////////////////////////////////////////////////////////////////

void State_ParkingInHomeBaseResidentVehicle::Entry( CARCportEngine* pEngine )
{
	if(m_birthInLot)
	{
		m_turnAroundTimer.SetEnd(true);
	}
	else
		m_turnAroundTimer.StartTimer(this, curTime()+ m_pOwner->GetTurnAroundTime() );

	m_pOwner->SetCurrentPlan(NULL);
	m_pOwner->SetServiceState(_waitOnbase);
}

void State_ParkingInHomeBaseResidentVehicle::Execute( CARCportEngine* pEngine )
{
	if(!m_turnAroundTimer.isEnded())
		return;
	if(!m_pOwner->GetCurrentPlan())
		return;
	//start service
	m_pOwner->SetServiceState(_preService);
	m_pOwner->SuccessProcessAtLayoutObject(m_spot->getLayoutObject(),m_spot, pEngine);
}
