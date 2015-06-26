#include "StdAfx.h"
#include "LandsideTaxiInSim.h"

#include "Landside/LandsideResidentVehilceRoute.h"
#include "../LandsideVehicleInSim.h"
#include "Landside/LandsideResidentRelatedContainer.h"
#include "../LandsideSimulation.h"
#include "../LandsideResourceManager.h"
#include "../ARCportEngine.h"
//#include "Goal_MoveInRoad.h"
#include "../LandsideVehicleServiceEndEvent.h"
#include "../LandsideExternalNodeInSim.h"
#include "Common/ProDistrubutionData.h"
#include "Landside/LandsideVehicleAssignment.h"


#include "../LandsideBaseBehavior.h"
#include "../LandsideBusStationInSim.h"
//#include "Goal_MoveInCurbside.h"
#include "../LandsideTaxiPoolInSim.h"
#include "../MobileDynamics.h"
#include "Landside/InputLandside.h"
#include "../TERMINAL.H"
#include "../LandsideTaxiQueueInSim.h"
#include "State_TaxiInQueue.h"
#include "State_InCurbside.h"
#include "Landside/VehicleOperationNonPaxRelatedContainer.h"
#include "../LandsideSimErrorShown.h"
#include "../PAX.H"


LandsideTaxiInSim::LandsideTaxiInSim( const PaxVehicleEntryInfo& entryInfo) 
:LandsidePaxVehicleInSim(entryInfo)
{
	m_CurBehavior = _DEPPAX;
	m_TypeName = entryInfo.vehicleType;
	m_pServTaxiQueu= NULL;
	m_bCalledByQueueFromExt = FALSE;
}

LandsideTaxiInSim::LandsideTaxiInSim(const NonPaxVehicleEntryInfo& entryInfo)
:m_NonPaxEntryInfo(entryInfo)
{
	m_CurBehavior = _NONPAX;
	m_TypeName = entryInfo.vehicleType;
	m_pServTaxiQueu= NULL;
	m_bCalledByQueueFromExt = FALSE;
}



ElapsedTime LandsideTaxiInSim::_UnLoadPax(LandsideResourceInSim* pCurb, const ElapsedTime& t,CARCportEngine* pEngine )
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
		if(pPax->CanPaxTakeOffThisResource(pCurb, pEngine->GetLandsideSimulation()))
		{
			pPax->ChooseTakeOffResource(pCurb->getLayoutObject(),pEngine->GetLandsideSimulation());
			//the destination station
			pPax->setState(GetOffVehicle);
			pPax->setResource(pCurb);
			eEventTime += eTimePerPax;
			pPax->GenerateEvent(eEventTime);			
		}
	}
	return eEventTime;
}

bool LandsideTaxiInSim::IsAllPassengersGetOn()
{
	if(m_vOnVehiclePax.empty())
		return false;

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



void LandsideTaxiInSim::ChangeToPoolPlan(LandsideTaxiPoolInSim* pPool, CARCportEngine* pEngine )
{
	m_CurBehavior = _NONPAX;
	//
	LandsideSimulation* pSim = pEngine->GetLandsideSimulation();
	VehicleOperationNonPaxRelatedContainer* pNonPaxRelatedCon = pSim->GetLandsideNonPaxRelatedCon();
	NonResidentVehicleTypePlan* pPlan =  pNonPaxRelatedCon->FindMatchPlan(curTime(), VehicleDemandAndPoolData::Demand_LayoutObject, pPool->getID() );
	if(!pPlan)
	{
		CString sError;
		sError.Format(_T("Can not find Operation Plan for TaxiPool %s "), pPool->getName().GetIDString().GetString() );		
		ShowError( sError,"Simulation Error");
		Terminate(curTime());
		return;
	}
	
	m_NonPaxEntryInfo.iDemandID  = pPool->getID();
	m_NonPaxEntryInfo.iDemandType = VehicleDemandAndPoolData::Demand_LayoutObject;
	m_NonPaxEntryInfo.pPlan = pPlan;
}

bool LandsideTaxiInSim::haveRouteToTaxiQ( LandsideTaxiQueueInSim*pTaxiQ )
{
	return true;
}

bool LandsideTaxiInSim::StartServiceQueue( LandsideTaxiQueueInSim*pTaxiQ )
{
	ASSERT(m_pServTaxiQueu==NULL);
	pTaxiQ->addServiceVehicle(this);  
	m_pServTaxiQueu = pTaxiQ;
	Continue();
	return true;
}



bool LandsideTaxiInSim::ProceedToNextFcObject( CARCportEngine* pEngine )
{
	NonResidentVehicleTypePlan* pPlan = getEntryInfo()->pPlan;
	if(!pPlan)
		return false;

	ResidentVehicleRouteList* pflowlist  = pPlan->GetVehicleRouteFlow()->GetDestVehicleRoute( getDestLayoutObject()->getID() );
	if(pflowlist)
	{
		ResidentVehicleRoute* pNext  = NULL;
		if(m_CurBehavior== _NONPAX && m_pServTaxiQueu )
		{
			pNext = pPlan->GetVehicleRouteFlow()->getChildLeadtoObject(m_pServTaxiQueu->getID(), pflowlist);
			ASSERT(pNext);
		}
		if(!pNext)
			pNext = getNextRouteItem(pflowlist, pEngine);

		if(pNext)
		{
			int nNextID = pNext->GetFacilityObject()->GetFacilityID();
			LandsideLayoutObjectInSim * pDest  = pEngine->GetLandsideSimulation()->GetResourceMannager()->getLayoutObjectInSim(nNextID);
			if(pDest)
			{
				if(pDest->toTaxiPool() && isCalledByQueue() )
				{
					ASSERT(m_pServTaxiQueu);
					if(m_pServTaxiQueu)
					{
						CString sWarn;
						sWarn.Format(_T("Taxi called by the Queue should not go to the Pool, pls check the Route Plan of %s") , m_pServTaxiQueu->print().GetString() );
						ShowError(sWarn,"Definition Error");
					}				
				}
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

void LandsideTaxiInSim::ArrivalLayoutObject( LandsideLayoutObjectInSim* pObj,LandsideResourceInSim* pDetailRes, CARCportEngine* pEngine )
{
	if(LandsideTaxiQueueInSim* pQ = pObj->toTaxiQueue())
	{
		ChangeVehicleState(new State_EnterTaxiQ(this,pQ));
	}
	else if(LandsideTaxiPoolInSim* pPool = pObj->toTaxiPool())
	{
		ChangeVehicleState(new State_EnterTaxiPool(this,pPool) );
	}
	else
        __super::ArrivalLayoutObject(pObj,pDetailRes,pEngine);
}

void LandsideTaxiInSim::LeaveLayoutObject( LandsideLayoutObjectInSim* pObj,LandsideResourceInSim* pDetailRes,CARCportEngine* pEngine )
{
	if(LandsideTaxiQueueInSim* pQ = pObj->toTaxiQueue())
	{
		if(IParkingSpotInSim* spot = pDetailRes->toLaneSpot())
		{
			ChangeVehicleState(new State_LeaveTaxiQ(this, pQ,spot) );	
			return;
		}
	}
	return __super::LeaveLayoutObject(pObj,pDetailRes,pEngine);	
}

bool LandsideTaxiInSim::InitBirth( CARCportEngine*pEngine )
{
	if(!__super::InitBirth(pEngine))
		return false;

	GenerateDeparturePax(pEngine);

	ChangeDest(NULL);
	
	ResidentVehicleRoute* pFirstRoute = getEntryInfo()->pFirstRoute;
	ASSERT(pFirstRoute);
	if(pFirstRoute)
	{
		LandsideFacilityObject* fcObj = pFirstRoute->GetFacilityObject();
		LandsideLayoutObjectInSim * pDest  = pEngine->GetLandsideSimulation()->GetResourceMannager()->getLayoutObjectInSim(fcObj->GetFacilityID());
		if(pDest)
		{
			ChangeDest(pDest);
			return InitBirthInLayoutObject(getDestLayoutObject(),pEngine);
		}
	}
	return false;
}

void LandsideTaxiInSim::SuccessParkInCurb( LandsideCurbSideInSim*pCurb,IParkingSpotInSim* spot )
{
	ChangeVehicleState(new State_DropPaxAtCurbside(this,pCurb,spot));
}

VehicleEntryInfo* LandsideTaxiInSim::getEntryInfo()
{
	if(m_CurBehavior == _ARRPAX )
	{
		return &m_ArrPaxEntryInfo;
	}
	else if(m_CurBehavior == _NONPAX)
	{
		return &m_NonPaxEntryInfo;
	}
	else 
	{
		return &m_entryInfo;
	}
}

bool LandsideTaxiInSim::CanServeTaxiQ(LandsideTaxiPoolInSim* pool, LandsideTaxiQueueInSim*pTaxiQ )
{
	ASSERT(m_CurBehavior == _NONPAX);
	NonResidentVehicleTypePlan* pPlan = getEntryInfo()->pPlan;
	if(!pPlan)
	{
		CString sError;
		sError.Format(_T("No Operation Plan Defined for %s"), pool->print().GetString() );
		ShowError(sError,_T("Definition Error"));
		return false;
	}

	ResidentVehicleRouteList* pflowlist  = pPlan->GetVehicleRouteFlow()->GetDestVehicleRoute( pool->getID() );
	if(pflowlist)
	{
		if(pPlan->GetVehicleRouteFlow()->getChildLeadtoObject(pTaxiQ->getID(), pflowlist) )
		{			
			return true;
		}
	}
	CString sError;
	sError.Format(_T("The Operation Plan of %s can not lead to %s "), pool->print().GetString(), pTaxiQ->print().GetString() );
	ShowError(sError,_T("Definition Error"));
	return false;	
}

void LandsideTaxiInSim::ChangeToArrPaxPlan(CARCportEngine* pEngine)
{
	if(m_CurBehavior==_ARRPAX)
		return;

	m_CurBehavior = _ARRPAX;
	LandsideSimulation* pSim = pEngine->GetLandsideSimulation();
	NonResidentVehicleTypeContainer* pNonresidentSpec = pSim->getInput()->getNonResidentVehiclePlan();
	if(m_vOnVehiclePax.empty())
	{
		return;
	}
	LandsideBaseBehavior* pPax = m_vOnVehiclePax.front();
	CString sPaxType;
	const CMobileElemConstraint& paxType = pPax->getPerson()->getType();
	paxType.screenPaxPrint(sPaxType);
	NonResidentVehicleTypePlan* pPlan =  pNonresidentSpec->FindMatchPlan(curTime(),m_TypeName, paxType);	
	if(!pPlan)
	{
		CString sError;
		sError.Format(_T("Can not find Operation Plan for %s(%s) "), m_TypeName.toString().GetString(), sPaxType.GetString() );
		ShowError(sError,_T("Simulation Error"));
		return;
	}
	
	m_ArrPaxEntryInfo.pPlan = pPlan;
}

void LandsideTaxiInSim::setCalledByQueue( LandsideTaxiQueueInSim* pServTaxiQueu, BOOL bCallFromExt )
{
	m_bCalledByQueueFromExt = bCallFromExt; 
	m_pServTaxiQueu = pServTaxiQueu;
}




