#include "StdAfx.h"
#include "LandsideNonPaxVehicleInSim.h"
#include "../ARCportEngine.h"
#include "../TERMINAL.H"
#include "landside/LandsideVehicleAssignment.h"
#include "../LandsideSimulation.h"
#include "landside/InputLandside.h"
#include "landside/LandsideResidentRelatedContainer.h"
#include "../LandsideResourceManager.h"
#include "Landside/LandsideEntryOffsetData.h"
#include "landside/VehicleGroupProperty.h"
#include "Landside/NonPaxVehicleAssignment.h"
#include "Landside/VehicleOperationNonPaxRelatedContainer.h"
#include "Goal_MoveInParkingLot.h"
#include "Goal_MoveInCurbside.h"
#include "Goal_PublicBus.h"
#include "Goal_MoveInRoad.h"
#include "../LandsideParkingLotInSim.h"
#include "engine/TurnaroundVisitor.h"
#include "engine/GroupLeaderInfo.h"
#include "engine/turnarnd.h"
#include "../LandsideSimErrorShown.h"

#include "../DeparturePaxLandsideBehavior.h"
#include "State_InParkingLot.h"
#include "../LandsideExternalNodeInSim.h"
#include "State_MoveInRoad.h"
//#include "Process_MoveInRoad.h"


bool LandsideNonPaxVehicleInSim::InitBirth( CARCportEngine*pEngine )
{
	if(!__super::InitBirth(pEngine))
		return false;

	ChangeDest(NULL);
	LandsideFacilityObject fcObj = *m_entryInfo.pFirstRoute->GetFacilityObject();
	LandsideLayoutObjectInSim * pDest  = pEngine->GetLandsideSimulation()->GetResourceMannager()->getLayoutObjectInSim(fcObj.GetFacilityID());
	if(pDest)
	{
		ChangeDest(pDest);
		
		return InitBirthInLayoutObject(getDestLayoutObject(),pEngine);;
	}
	return false;
}


LandsideNonPaxVehicleInSim::LandsideNonPaxVehicleInSim( const NonPaxVehicleEntryInfo& entryInfo )
:m_entryInfo(entryInfo)
{
	m_TypeName = entryInfo.vehicleType;
}

ElapsedTime LandsideNonPaxVehicleInSim::GetParkingLotWaitTime(CARCportEngine* pEngine, const ALTObjectID& parkLot )
{
	LandsideSimulation* pSim = pEngine->GetLandsideSimulation();//LandsideSimulation* pSim 
	NonPaxVehicleAssignment* pNonPaxVehicleAssignment = pSim->GetLandsideNonPaxVehicleAssignment();

	return pNonPaxVehicleAssignment->FindMatchWaitTime(parkLot,getEntryInfo().iDemandID);
}

bool LandsideNonPaxVehicleInSim::ProceedToNextFcObject( CARCportEngine* pEngine )
{
	//m_vFailedItems.clear();	
	ResidentVehicleRouteList* pflowlist  = m_entryInfo.pPlan->GetVehicleRouteFlow()->GetDestVehicleRoute( getDestLayoutObject()->getID() );
	if(pflowlist)
	{
		ResidentVehicleRoute* pNext  = getNextRouteItem(pflowlist, pEngine);
		if(pNext)
		{
			int nNextID = pNext->GetFacilityObject()->GetFacilityID();
			LandsideLayoutObjectInSim * pDest  = pEngine->GetLandsideSimulation()->GetResourceMannager()->getLayoutObjectInSim(nNextID);
			ChangeDest(pDest);
			return true;
		}
	}
	return false;
}






void LandsideNonPaxVehicleInSim::SuccessParkInLotSpot( LandsideParkingSpotInSim* pSpot )
{
	ChangeState(new State_NonPaxVParkingInLotSpot(this,pSpot),getLastState().time);
}



