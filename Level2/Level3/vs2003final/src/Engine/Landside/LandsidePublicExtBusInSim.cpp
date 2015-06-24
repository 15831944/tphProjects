#include "StdAfx.h"
#include "LandsidePublicExtBusInSim.h"
#include "..\LandsideBaseBehavior.h"
#include "..\ARCportEngine.h"
#include "..\LandsideBusStationInSim.h"
#include "Landside\LandsideVehicleAssignment.h"
#include "..\PERSON.H"
#include "Inputs\MobileElemConstraint.h"
#include "..\LandsideVehicleEntryInfo.h"
#include "..\PaxLandsideBehavior.h"
#include "State_InBusStation.h"
#include "..\LandsideSimulation.h"
#include "..\LandsideResourceManager.h"


LandsidePublicExtBusInSim::LandsidePublicExtBusInSim( const PaxVehicleEntryInfo& entryInfo ) 
:LandsidePaxVehicleInSim(entryInfo)
{
}


ElapsedTime LandsidePublicExtBusInSim::_UnLoadPax(LandsideBusStationInSim* pBusStation, const ElapsedTime& t,CARCportEngine* pEngine )
{
	const static ElapsedTime eTimePerPax = ElapsedTime(5L);
	ElapsedTime eEventTime = t;

	int nPaxTookOff = 0;
	std::vector<LandsideBaseBehavior*>::iterator iterPax = m_vOnVehiclePax.begin();
	for (; iterPax != m_vOnVehiclePax.end(); ++iterPax)
	{
		LandsideBaseBehavior *pPax = *iterPax;
		if(pPax == NULL)
			continue;
		if(pPax->CanPaxTakeOffThisResource(pBusStation, pEngine->GetLandsideSimulation()))
		{
			pPax->ChooseTakeOffResource(pBusStation,pEngine->GetLandsideSimulation());
			//the destination station
			pPax->setState(GetOffVehicle);
			pPax->setResource(pBusStation);
			eEventTime += eTimePerPax;
			pPax->GenerateEvent(eEventTime);
		}
	}
	return eEventTime;
}

bool LandsidePublicExtBusInSim::IsAllPassengersReady()const
{
	std::vector<LandsideBaseBehavior*>::const_iterator iterPax = m_vOnVehiclePax.begin();
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


void LandsidePublicExtBusInSim::NotifyPaxMoveInto( CARCportEngine*pEngine, LandsideBaseBehavior* pPassenger, const ElapsedTime& eTime )
{
	Activate(eTime);
}

bool LandsidePublicExtBusInSim::IsReadyToLeave(LandsideBusStationInSim* pBustation) const
{
	//2. all passengers stay on vehicle
	if(!IsAllPassengersReady())
		return false;

	if(!IsFull())
	{		
		if( pBustation->GetWaitingPaxNum() > 0 )
			return false;		
	}

	return true;
}

bool LandsidePublicExtBusInSim::IsFull() const
{
	return getCapacity()<=(int)m_vOnVehiclePax.size();
}



bool LandsidePublicExtBusInSim::CanLoadPax( PaxLandsideBehavior* pBehavior )
{
	if(IsFull())
		return false;

	LandsideVehicleAssignEntry*  pAssignEntry = getEntryInfo().getVehicleAssignEntry();
	if(!pAssignEntry->getPaxType().fits(pBehavior->getPerson()->getType()))
	{	
		return false;
	}
	return true;
}

bool LandsidePublicExtBusInSim::InitBirth( CARCportEngine *pEngine )
{
	if(!__super::InitBirth(pEngine))
		return false;
	GenerateDeparturePax(pEngine);

	ChangeDest(NULL);
	LandsideFacilityObject fcObj = *m_entryInfo.pFirstRoute->GetFacilityObject();
	LandsideLayoutObjectInSim * pDest  = pEngine->GetLandsideSimulation()->GetResourceMannager()->getLayoutObjectInSim(fcObj.GetFacilityID());
	if(pDest)
	{
		ChangeDest(pDest);		
		return InitBirthInLayoutObject(getDestLayoutObject(),pEngine);
	}

	return false;
}

void LandsidePublicExtBusInSim::ArrivalLayoutObject( LandsideLayoutObjectInSim* pObj,LandsideResourceInSim* pDetailRes, CARCportEngine* pEngine )
{
	if(LandsideBusStationInSim* pBusStation = pObj->toBusSation())
	{
		ChangeVehicleState(new State_ParkingBusStation(this,pBusStation) );
	}
	else
	{
		return __super::ArrivalLayoutObject(pObj,pDetailRes,pEngine);
	}
}

void LandsidePublicExtBusInSim::SuccessParkInBusStation( LandsideBusStationInSim* pStation ,LaneParkingSpot*spot)
{
	ChangeVehicleState(new State_ProcessAtBusStationExtBus(this,pStation,spot) );
}

bool LandsidePublicExtBusInSim::ProceedToNextFcObject( CARCportEngine* pEngine )
{
	ResidentVehicleRouteList* pflowlist  = m_entryInfo.pPlan->GetVehicleRouteFlow()->GetDestVehicleRoute( getDestLayoutObject()->getID() );
	if(pflowlist)
	{
		ResidentVehicleRoute* pNext  = getNextRouteItem(pflowlist, pEngine);
		if(pNext)
		{
			int nNextID = pNext->GetFacilityObject()->GetFacilityID();
			LandsideLayoutObjectInSim * pDest  = pEngine->GetLandsideSimulation()->GetResourceMannager()->getLayoutObjectInSim(nNextID);
			if(pDest)
			{
				ChangeDest(pDest);
				return true;
			}

			CString sError;
			sError.Format(_T("Error Find Facility Object %d"),nNextID);
			ShowError(sError,"Definition Error");
		}
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
void State_ProcessAtBusStationExtBus::Entry( CARCportEngine* pEngine )
{
	m_pBusStation->AddWaitingBus(getVehicle());
	//doting when begin service
	ElapsedTime tStopTime = ElapsedTime(1*60L);
	if(const CProDistrubution* pdist =  m_pOwner->getEntryInfo().getVehicleAssignEntry()->getDstStop() )
	{
		tStopTime = ElapsedTime(pdist->GetProbDistribution()->getRandomValue()*60L);
	}

	m_waitTimer.StartTimer( this, curTime() +tStopTime );
	ElapsedTime eTimeAfterPaxTakeOff =  m_pOwner->_UnLoadPax(m_pBusStation,curTime(),pEngine);
	m_pBusStation->NoticeWaitingPax(eTimeAfterPaxTakeOff, getVehicle(), pEngine->GetLandsideSimulation());
}

void State_ProcessAtBusStationExtBus::Execute( CARCportEngine* pEngine )
{
	if(!m_waitTimer.isEnded())
		return ;

	if( !m_pOwner->IsReadyToLeave(m_pBusStation) )
		return;

	m_pBusStation->DelWaitingBus(getVehicle());
	m_pOwner->SuccessProcessAtLayoutObject(m_pBusStation,m_spot,pEngine);
}
