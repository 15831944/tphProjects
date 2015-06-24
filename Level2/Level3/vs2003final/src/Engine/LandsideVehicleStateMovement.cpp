#include "StdAfx.h"
#include ".\landsidevehiclestatemovement.h"
#include "LandsideSimulation.h"
#include "ARCportEngine.h"
#include "LandsideVehicleInSim.h"
#include "LandsideParkingLotInSim.h"
#include "LandsideVehicleInSpotState.h"
#include "LandsideResourceManager.h"
#include "LandsideVehicleMoveInRoad.h"
#include "LandsideCurbsideInSim.h"
#include "LandsideExternalNodeInSim.h"

CString strVehicleState[] = 
{
	"_birth",
	"_moveOnRoad",
	"_moveOnCurbside",
	"_enterparking",
	"_onparking",
	"_leaveparking",
	"_terminate",
};
//////////////////////////////////////////////////////////////////////////
IVehicleStateMovement* LandsideVehicleBirthState::proceedTo( CARCportEngine *_pEngine )
{	
	ElapsedTime t = mpVehicle->curTime();
	LandsideSimulation* pLandSim = _pEngine->GetLandsideSimulation();
	LandsidePrivateVehicleInSim* pPrivateVehicle = mpVehicle->toPrivateVehicle(); //deal with private vehicle 
	if(!pPrivateVehicle)
		return NULL;	

	enumPrivateVehicleParkType nParkType = pPrivateVehicle->getParkType();
	if(!mpBirthPlace)//find birth place for the vehicle and set the birth time
	{
		if(!mpVehicle->IsDepPaxVehicle() && nParkType == Long_Park) //arrival long term park vehicle change to parking in spot state directly
		{
			LandsideParkingLotInSim *pLot = pLandSim->AssignLongTermParkingLot(mpVehicle);
			if(pLot && !pLot->IsSpotLess() ) 
			{
				LandsideParkingSpotInSim* pSpot = pLot->getFreeSpot(mpVehicle);
				if(pSpot)//got a free spot stay in there
				{
					//got to next state parking in lot
					VehicleParkInSpotState* patLot = new VehicleParkInSpotState(mpVehicle,pSpot);	
					patLot->WriteAtStateLog(t);
					return patLot;
				}
				else //wait for spot free let lot notify vehicle
				{
					//pLot->AddWaitVehicle(mpVehicle);
					mpVehicle->SetActive(false); //set inactive so it will not generate event 
					return this;
				}
			}
			else//no parking lot throw error message 
			{
				mpVehicle->Terminate(t);
				return NULL;
			}

		}
		else//birth at origin node
		{
			LandsideExternalNodeInSim* pNode = pLandSim->AssignOrignNode(mpVehicle);			
			if(mpBirthPlace = pNode)
			{
				MobileState birthState;
				birthState.distInRes = 0;
				birthState.time = t;
				birthState.dSpeed = 0;
				birthState.mAtRes = mpBirthPlace;
				birthState.pos = pNode->getPos();
				//birthState.motion = _stopped;
				mpVehicle->initState(birthState);
				return this;
			}
			else
			{
				mpVehicle->Terminate(t);
				return NULL;
			}
		}		
	}
	else //have birth place,birthed write log and goto next movement state
	{		

		if(mpVehicle->IsDepPaxVehicle())
		{			
			mpVehicle->CallPaxGetOnVehicle(t,_pEngine);
		}
		//		
		if(nParkType == 0)//no park vehicle
		{
			LandsideCurbSideInSim* pCurbSide = pLandSim->AssignCurbside(mpVehicle);
			if(pCurbSide)//got curbside get a way to it and move
			{
				if(mpVehicle->IsDepPaxVehicle())
				{
					mpVehicle->setDropPaxPlace(pCurbSide);					
				}
				else{
					mpVehicle->setPickPaxPlace(pCurbSide);
				}
				LandsideVehicleMoveInRoad* pNextMove = new LandsideVehicleMoveInRoad(mpVehicle);
				if(!pNextMove->SetOrignDest(mpBirthPlace,pCurbSide,_pEngine))
				{
					delete pNextMove;
					mpVehicle->Terminate(t);
					return NULL;
				};
				return pNextMove;
			}
			else //can not find curbside throw exception
			{
			
				mpVehicle->Terminate(t);
				return NULL;
			}			
		}
		else if(nParkType == Short_Park)//short term park vehicle
		{
			LandsideParkingLotInSim* pShortTermPark = pLandSim->AssingShortTermParkinLot(mpVehicle);
			if(pShortTermPark)//change state to move to parking place
			{
				LandsideParkingLotDoorInSim* pDoor = pShortTermPark->getEntryDoor(mpVehicle);
				if(pDoor)
				{
					LandsideVehicleMoveInRoad* pNextMove = new LandsideVehicleMoveInRoad(mpVehicle);
					if(!pNextMove->SetOrignDest(mpBirthPlace,pDoor,_pEngine))
					{
						delete pNextMove;
						mpVehicle->Terminate(t);
						return NULL;
					}
					return pNextMove;
				}
			}
			//can not find parking lot throw exception or just go to curbside?			
			mpVehicle->Terminate(t);
			return NULL;			
			
		}
		else if(nParkType == Long_Park)	//long term park vehicle   
		{	
			if(mpVehicle->IsDepPaxVehicle()) //departure vehicle
			{
				LandsideParkingLotInSim* pLongTermPark = pLandSim->AssignLongTermParkingLot(mpVehicle);
				if(pLongTermPark)
				{
					LandsideParkingLotDoorInSim* pDoor = pLongTermPark->getEntryDoor(mpVehicle);
					if(pDoor)
					{
						LandsideVehicleMoveInRoad* pNextMove = new LandsideVehicleMoveInRoad(mpVehicle);
						if(!pNextMove->SetOrignDest(mpBirthPlace,pDoor,_pEngine))
						{
							delete pNextMove;
							mpVehicle->Terminate(t);
							return NULL;
						}
						return pNextMove;
					}
				}
				////can not find parking lot throw exception or just go to curbside?				
				mpVehicle->Terminate(t);
				return NULL;								
			}					
		}	
	}
	return this;	
}



LandsideVehicleBirthState::LandsideVehicleBirthState( LandsidePrivateVehicleInSim*pVehicle )
:IVehicleStateMovement(pVehicle)
{	
	mpBirthPlace = NULL;
}

bool LandsideVehicleBirthState::InResource( LandsideResourceInSim* pRes ) const
{
	return mpBirthPlace == pRes;
}




//bool IVehicleStateMovement::GetTimeState( const ElapsedTime& t, LandsideVehicleInSim::State& tState )
//{	
//	return false;
//}
//
//LandsideVehicleInSim::State& IVehicleStateMovement::GetEndState()
//{
//	return mpVehicle->getState();
//}