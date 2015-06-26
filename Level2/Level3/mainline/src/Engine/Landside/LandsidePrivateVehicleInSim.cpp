#include "StdAfx.h"
#include "LandsidePrivateVehicleInSim.h"
#include "..\ARCportEngine.h"
#include "..\LandsideSimulation.h"
#include "..\LandsideCurbsideInSim.h"
#include "..\LandsideCellPhoneProcess.h"
#include "..\SimEngineConfig.h"
#include "State_InCurbside.h"
#include "..\LandsideStrategyStateInSim.h"
#include "..\LandsideResourceManager.h"
#include "..\LandsideSimErrorShown.h"
#include "..\LandsideBaseBehavior.h"




LandsidePrivateVehicleInSim::LandsidePrivateVehicleInSim( const PaxVehicleEntryInfo& entryInfo ) 
:LandsidePaxVehicleInSim(entryInfo)
{
}

bool LandsidePrivateVehicleInSim::InitBirth( CARCportEngine *pEngine )
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
		pEngine->GetLandsideSimulation()->GenerateIndividualArrivalVehicleConverger(pEngine,this,curTime());
		return 	InitBirthInLayoutObject(getDestLayoutObject(),pEngine);
	}

	return false;
}



bool LandsidePrivateVehicleInSim::ProceedToNextFcObject( CARCportEngine* pEngine )
{
	ResidentVehicleRouteList* pflowlist  = m_entryInfo.pPlan->GetVehicleRouteFlow()->GetDestVehicleRoute( getDestLayoutObject()->getID() );
	if(pflowlist)
	{
		LandsideCellPhoneProcess findprocess(this,pflowlist);
		ResidentVehicleRoute* pNext  = findprocess.GetNextVehicleRoute(pEngine,m_entryInfo.pPlan->GetVehicleRouteFlow());
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

void LandsidePrivateVehicleInSim::SuccessParkInCurb( LandsideCurbSideInSim*pCurb,IParkingSpotInSim* spot )
{
	if(isArrival())
	{
		ChangeState(new State_PickPaxAtCurbsidePrivate(this,pCurb,spot),getLastState().time);
	}
	else
	{
		ChangeState(new State_DropPaxAtCurbside(this,pCurb,spot), getLastState().time);
	}
}

bool LandsidePrivateVehicleInSim::isArrival() const
{
	return getEntryInfo().vPaxEntryList.begin()->isArrival()!=0;
}






void LandsidePrivateVehicleInSim::SuccessParkInLotSpot( LandsideParkingSpotInSim* pSpot )
{
	if(pSpot->getParkingLot()->IsCellPhone())
	{
		ChangeState(new State_ParkingAtCellPhoneLotPrivate(this,pSpot),getLastState().time);
	}
	else
	{
		if(isArrival())
			ChangeState(new State_ParkingInLotSpotPrivateArr(this,pSpot),getLastState().time);
		else
			ChangeState(new State_ParkingInLotSpotPrivateDep(this,pSpot),getLastState().time);
	}
}

void LandsidePrivateVehicleInSim::OnFailPickPaxAtCurb(LandsideCurbSideInSim* pCurb,  IParkingSpotInSim* spot,CARCportEngine* pEngine )
{
	if( CurbsideStrategyStateInSim* curbStratgy = getCurbStragy() )
	{
		if(LandsideLayoutObjectInSim * pNextDest = curbStratgy->GoNextLoop(pEngine))
		{
			ChangeDest(pNextDest);
			ChangeState(new State_LeaveCurbside(this, pCurb,spot),pEngine);
			if(curbStratgy->isFinalState())
				EndCurbStrategy();
			return;
		}		
	}
	CString sError;
	sError.Format(_T("Fail Pick Arr Pax at %s"),pCurb->getName().GetIDString().GetString() );
	ShowError(sError, "Simulation Error");
	SuccessProcessAtLayoutObject(pCurb,spot,pEngine);
}

void LandsidePrivateVehicleInSim::CallPaxGetOn( CARCportEngine* pEngine ,LandsideResourceInSim* pRes )
{
	ElapsedTime tT = curTime();
	for(size_t i=0;i<m_entryInfo.vPaxEntryList.size();i++)
	{
		int paxid = m_entryInfo.vPaxEntryList[i].getID();
		if(IsPaxOnVehicle(paxid))
			continue;

		if( MobileElement* pMob = pEngine->m_simBobileelemList.GetAvailableElement(paxid) )
		{
			if( LandsideBaseBehavior* pBehave = pMob->getLandsideBehavior() )
			{
				pBehave->OnVehicleParkAtPlace(this,pRes,tT);
			}
		}
	}
}

bool LandsidePrivateVehicleInSim::HavePaxNeedGetOn( CARCportEngine* pEngine , LandsideResourceInSim* pRes ) const
{
	for(size_t i=0;i<m_entryInfo.vPaxEntryList.size();i++)
	{
		int paxid = m_entryInfo.vPaxEntryList[i].getID();
		if(IsPaxOnVehicle(paxid))
			continue;

		if( MobileElement* pMob = pEngine->m_simBobileelemList.GetAvailableElement(paxid) )
		{
			if( LandsideBaseBehavior* pBehave = pMob->getLandsideBehavior() )
			{
				if( pBehave->CanBoardingOrBoarding(pRes) )
					return true;
			}
		}
	}
	return false;
}


bool LandsidePrivateVehicleInSim::AllPaxOnVehicle(CARCportEngine* pEngine)
{
	for(size_t i=0;i<m_entryInfo.vPaxEntryList.size();i++)
	{
		int paxid = m_entryInfo.vPaxEntryList[i].getID();
		if(IsPaxOnVehicle(paxid))
			continue;
		return false;
	}
	return true;
}



bool LandsidePrivateVehicleInSim::isDepLongTerm(LandsideLayoutObjectInSim* pObj)
{
	if(!isArrival())
	{
		ResidentVehicleRouteList* pflowlist  = m_entryInfo.pPlan->GetVehicleRouteFlow()->GetDestVehicleRoute( getDestLayoutObject()->getID() );
		if(pflowlist )
		{
			return pflowlist->GetDestCount()==0;
		}
		else
			return true;
	}
	return false;
}

bool LandsidePrivateVehicleInSim::isDepShortTerm( LandsideLayoutObjectInSim* pObj )
{
	if(!isArrival())
	{
		ResidentVehicleRouteList* pflowlist  = m_entryInfo.pPlan->GetVehicleRouteFlow()->GetDestVehicleRoute( getDestLayoutObject()->getID() );
		if(pflowlist )
		{
			return pflowlist->GetDestCount()!=0;
		}
		else
			return false;
	}
	return false;
}



//////////////////////////////////////////////////////////////////////////
void State_PickPaxAtCurbsidePrivate::Entry( CARCportEngine* pEngine )
{
	if(!simEngineConfig()->isSimTerminalMode())
	{
		m_pOwner->GenerateArrivalPax(pEngine);
	}

	ElapsedTime maxStopTime = m_pCurb->GetMaxStopTime();
	ASSERT(maxStopTime>=ElapsedTime(0L));

	maxStopTime = MAX(maxStopTime,ElapsedTime(0L));;
	m_waitTimer.StartTimer(this, curTime() + maxStopTime);
	m_pOwner->CallPaxGetOn(pEngine,m_pCurb);	
	m_pOwner->SetLoadingPax(true);
}

void State_PickPaxAtCurbsidePrivate::Execute( CARCportEngine* pEngine )
{
	if(m_pOwner->HavePaxNeedGetOn(pEngine,m_pCurb))
	{
		m_pOwner->CallPaxGetOn(pEngine,m_pCurb);
		return;
	}

	if(m_pOwner->AllPaxOnVehicle(pEngine))
	{
		getVehicle()->SuccessProcessAtLayoutObject(m_pCurb,m_spot,pEngine);		
	}
	else
	{
		if( m_waitTimer.isEnded() )
			return m_pOwner->OnFailPickPaxAtCurb(m_pCurb,m_spot,pEngine);
	}
}
void State_PickPaxAtCurbsidePrivate::Exit( CARCportEngine* pEngine )
{
	m_pOwner->SetLoadingPax(false);
}

State_PickPaxAtCurbsidePrivate::State_PickPaxAtCurbsidePrivate( LandsidePrivateVehicleInSim* pV,LandsideCurbSideInSim*pCurb,IParkingSpotInSim* spot) 
:State_LandsideVehicle<LandsidePrivateVehicleInSim>(pV)
{
	m_pCurb = pCurb;
	m_spot = spot;
}


//////////////////////////////////////////////////////////////////////////
void State_ParkingInLotSpotPrivateDep::Execute( CARCportEngine* pEngine )
{
	if(m_pOwner->HavePaxGetOff(m_pSpot) )
	{
		return;
	}


	if(m_ptype == _depshort)
		if(!m_WaitVisitorTimer.isEnded())
			return;

	if(m_ptype == _deplongterm)
		return;
	
	getVehicle()->SuccessProcessAtLayoutObject(m_pSpot->getParkingLot(),m_pSpot,pEngine);
}

void State_ParkingInLotSpotPrivateDep::Entry( CARCportEngine* pEngine )
{
	if(m_ptype == _depshort)//temp code for visitor
		m_WaitVisitorTimer.StartTimer(this, curTime()+ ElapsedTime(20*60L) );
	m_pOwner->CallPaxGetOff(m_pSpot);
	Execute(pEngine);
}

State_ParkingInLotSpotPrivateDep::State_ParkingInLotSpotPrivateDep( LandsidePrivateVehicleInSim* pV,LandsideParkingSpotInSim* pSpot )
: State_LandsideVehicle<LandsidePrivateVehicleInSim>(pV)
,m_pSpot(pSpot)
{
	m_ptype = _other;
	if(m_pOwner->isDepLongTerm(pSpot->getLayoutObject()) )
		m_ptype = _deplongterm;
	if(m_pOwner->isDepShortTerm(pSpot->getLayoutObject()))
		m_ptype = _depshort;
}
//////////////////////////////////////////////////////////////////////////
void State_ParkingAtCellPhoneLotPrivate::Execute( CARCportEngine* pEngine )
{
	if(m_pOwner->CellPhoneCall(pEngine))
	{
		MobileState state = m_pOwner->getLastState();
		state.time = m_pOwner->curTime();
		m_pOwner->UpdateState(state);
		m_pOwner->SuccessProcessAtLayoutObject(mpSpot->getParkingLot(),mpSpot,pEngine);
	}
}
//////////////////////////////////////////////////////////////////////////
void State_ParkingInLotSpotPrivateArr::Entry( CARCportEngine* pEngine )
{
	if(!simEngineConfig()->isSimTerminalMode())
	{
		m_pOwner->GenerateArrivalPax(pEngine);
	}
	m_pOwner->CallPaxGetOn(pEngine,m_pSpot);	
	m_pOwner->SetLoadingPax(true);
}

void State_ParkingInLotSpotPrivateArr::Execute( CARCportEngine* pEngine )
{
	if(m_pOwner->HavePaxNeedGetOn(pEngine,m_pSpot))
	{
		m_pOwner->CallPaxGetOn(pEngine,m_pSpot);
		return;
	}
	if(!m_pOwner->AllPaxOnVehicle(pEngine))
		return;        
	getVehicle()->SuccessProcessAtLayoutObject(m_pSpot->getParkingLot(),m_pSpot,pEngine);
}
