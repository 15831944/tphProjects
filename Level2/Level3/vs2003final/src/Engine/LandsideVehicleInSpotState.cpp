#include "StdAfx.h"
#include ".\landsidevehicleinspotstate.h"
#include "LandsideVehicleInSim.h"
#include "LandsideSimulation.h"
#include "ARCportEngine.h"
#include "LandsideParkingLotInSim.h"
#include "LandsideVehicleMoveInRoad.h"
#include "LandsideParkingLotGraph.h"
#include "LandsideExternalNodeInSim.h"
//////////////////////////////////////////////////////////////////////////
IVehicleStateMovement* VehicleEnterParkingLot::proceedTo( CARCportEngine *_pEngine )
{	
	//move to spot
	MobileState state = mpVehicle->getState();

	state.time = mpVehicle->curTime();
	mpVehicle->addNextState(state);

	const ARCPoint3& spotpos = mpSpot->getPos();
	ARCVector3 spotdir = mpSpot->getDir();
	spotdir.Normalize();

	double dDist = spotpos.DistanceTo(state.pos);
	double dSpeed = mpVehicle->mPerform.dMaxSpeed;
	ElapsedTime dT = dDist/dSpeed;
	//write in spot log
	state.time += dT;
	state.mAtRes = mpSpot;
	state.distInRes = 0;
	state.dSpeed = dSpeed;
	//state.motion = _dec;	
	state.pos = spotpos;
	mpVehicle->addNextState(state);
	//write dir log
	state.dSpeed =0;
	state.pos += spotdir;
	mpVehicle->addNextState(state);



	VehicleParkInSpotState* pNextState = new VehicleParkInSpotState(mpVehicle,mpSpot);
	if(mpVehicle->IsDepPaxVehicle())
	{
		mpVehicle->setDropPaxPlace(mpSpot);
	}
	else
		mpVehicle->setPickPaxPlace(mpSpot);

	return pNextState;
}

//ElapsedTime VehicleEnterParkingLot::getNextTime() const
//{
//	return mpVehicle->getState().time;
//}


VehicleEnterParkingLot::VehicleEnterParkingLot( LandsideVehicleInSim* pVehicle,LandsideParkingLotDoorInSim* pDoor, LandsideParkingSpotInSim* pSpot )
:IVehicleStateMovement(pVehicle)
{
	mpLotDoor = pDoor;
	mpSpot = pSpot;
	mpSpot->OnVehicleEnter(pVehicle,pVehicle->getState().time);
}

bool VehicleEnterParkingLot::InResource( LandsideResourceInSim* pRes ) const
{
	LandsideParkingLotInSim* pLot = mpSpot->getParkingLot();
	if(pLot==pRes)
		return true;
	if(mpSpot == pRes)
		return true;
	if(mpLotDoor == pRes)
		return true;
	return false;
}
//////////////////////////////////////////////////////////////////////////
IVehicleStateMovement* VehicleParkInSpotState::proceedTo( CARCportEngine *_pEngine )
{	//
	
	//state to parking in spot
	ElapsedTime t = mpVehicle->curTime();
	MobileState state = mpVehicle->getState();
	state.time = t;

	LandsidePrivateVehicleInSim* pPrivateVehicle = (LandsidePrivateVehicleInSim*)mpVehicle;
	if( pPrivateVehicle->getParkType() == Long_Park && pPrivateVehicle->IsDepPaxVehicle())//departure stay to simulation end
	{
		if(!mpVehicle->IsInLoadUnLoadPax())
		{
			if(mpSpot== mpVehicle->getDropPaxPlace())
			{
				mpVehicle->UnLoadOnPax(t);
				mpVehicle->SetInLoadUnLoadPax(true);
			}	
		}	
		mpVehicle->SetActive(false);		
		return this;
	}
	else if(pPrivateVehicle->getParkType()==Short_Park) //short term park stay for a while
	{	
		if(mpSpot == mpVehicle->getDropPaxPlace() ) //drop pax place
		{
			if(!mpVehicle->IsInLoadUnLoadPax()) //not unload pax 
			{
				if( mpVehicle->HaveOnPax() )//unload all pax
				{
					mpVehicle->UnLoadOnPax(t);
					mpVehicle->SetInLoadUnLoadPax(true);
					mpVehicle->SetActive(false);
					return this;
				}				
			}	
			else 
			{
				//mpVehicle->setTime(t);			
				if(mpVehicle->HaveOnPax())
				{
					mpVehicle->SetActive(false);
					return this;
				}
				else
				{
					mpVehicle->SetInLoadUnLoadPax(false);
					mpVehicle->WriteLog(state);
				
					//go to next phase
					//stay for a while
					state.time += ElapsedTime(10*60L);
					mpVehicle->addNextState(state);
					///leave
					LandsideParkingLotInSim* pLot = mpSpot->getParkingLot();
					LandsideParkingLotDoorInSim* pExitDoor = pLot->getExitDoor(mpVehicle);
					if(pExitDoor)
					{
						VehicleLeaveParkingLot* pLeavelot  = new VehicleLeaveParkingLot(mpVehicle,mpSpot,pExitDoor);
						return pLeavelot;
					}								
				}
			}
			
		}
		else if(mpSpot == mpVehicle->getPickPaxPlace() ) //wait for pax get on vehicle
		{
			if(!mpVehicle->IsInLoadUnLoadPax()) //not unload pax 
			{				
				mpVehicle->CallPaxGetOnVehicle(t,_pEngine);
				mpVehicle->SetInLoadUnLoadPax(true);
				mpVehicle->SetActive(false);
				return this;								
			}	
			else
			{
				if(mpVehicle->AllPaxGetOn()) //all get on move
				{
					//mpVehicle->setTime(t);
					mpVehicle->WriteLog(state);
					//go to next state
					state.time += ElapsedTime(1L);
					mpVehicle->addNextState(state);
					///leave
					LandsideParkingLotInSim* pLot = mpSpot->getParkingLot();
					LandsideParkingLotDoorInSim* pExitDoor = pLot->getExitDoor(mpVehicle);
					if(pExitDoor)
					{
						VehicleLeaveParkingLot* pLeavelot  = new VehicleLeaveParkingLot(mpVehicle,mpSpot,pExitDoor);
						return pLeavelot;
					}								
				}
				else
				{
					mpVehicle->SetActive(false);
					return this;
				}
			}
			//call pax to go in vehicle
			
		}
		else
		{
			ASSERT(FALSE);

		}
		
	}
	mpVehicle->Terminate(t);
	return NULL;
	return this;
}

void VehicleParkInSpotState::WriteAtStateLog(const ElapsedTime& t)
{
	MobileState state = mpVehicle->getState();
	state.mAtRes = mpSpot;
	state.dSpeed = 0;
	//state.nstate = getState();
	state.pos = mpSpot->getPos()-mpSpot->getDir();
	state.time = t;
	mpVehicle->addNextState(state);
	state.pos = mpSpot->getPos();
	mpVehicle->addNextState(state);		
}

VehicleParkInSpotState::VehicleParkInSpotState( LandsideVehicleInSim* pVehicle, LandsideParkingSpotInSim* pSpot )
:IVehicleStateMovement(pVehicle)
{
	mpSpot = pSpot;
//	bUnloadPickingPax = false;

}


bool VehicleParkInSpotState::InResource( LandsideResourceInSim* pRes ) const
{
	return mpSpot==pRes;
}
//////////////////////////////////////////////////////////////////////////
IVehicleStateMovement* VehicleLeaveParkingLot::proceedTo( CARCportEngine *_pEngine )
{
	//move to door via drive path
	

	ElapsedTime t = mpVehicle->curTime();
	ARCVector3 doorPos = mpLotDoor->getPos();
	MobileState state = mpVehicle->getState();
	double dSpd = mpVehicle->mPerform.dMaxSpeed;//getNormalSpd();

	state.time = t;
	state.dSpeed = dSpd;
	//state.motion = _uniform;
	//state.nstate = getState();
	mpVehicle->addNextState(state);
	mpSpot->OnVehicleExit(mpVehicle,state.time);	
	
	double dist = doorPos.DistanceTo(state.pos);
	state.mAtRes = mpLotDoor;
	state.time = t + ElapsedTime(dist/dSpd);
	state.dSpeed = dSpd;
	state.distInRes = 0;
	//state.nstate = getState();
	state.pos = doorPos;
	mpVehicle->addNextState(state);



	return goToNextState(_pEngine);
	
	
}





VehicleLeaveParkingLot::VehicleLeaveParkingLot( LandsideVehicleInSim* pVehicle, LandsideParkingSpotInSim* pSpot,LandsideParkingLotDoorInSim* pDoor )
:IVehicleStateMovement(pVehicle)
{
	mpSpot = pSpot;
	mpLotDoor = pDoor;
	mDrivePath = new DrivePathInParkingLot();
	pSpot->getParkingLot()->getDriveGraph()->FindPath(pSpot,pDoor,*mDrivePath);
}




bool VehicleLeaveParkingLot::InResource( LandsideResourceInSim* pRes ) const
{
	LandsideParkingLotInSim* pLot = mpSpot->getParkingLot();
	if(pLot==pRes)
		return true;
	if(mpSpot==pRes)
		return true;
	if(mpLotDoor == pRes)
		return true;
	return false;
}

IVehicleStateMovement* VehicleLeaveParkingLot::goToNextState( CARCportEngine* _pEngine )
{
	LandsideSimulation* pSim = _pEngine->GetLandsideSimulation();

	LandsideExternalNodeInSim* pDest = pSim->AssignDestNode(mpVehicle);
	if(pDest)
	{
		LandsideVehicleMoveInRoad* pMoveTodest = new LandsideVehicleMoveInRoad(mpVehicle);
		if(!pMoveTodest->SetOrignDest(mpLotDoor,pDest,_pEngine))
		{
			delete pMoveTodest;			
		}
		else
			return pMoveTodest;
	}	
	return NULL;	
}