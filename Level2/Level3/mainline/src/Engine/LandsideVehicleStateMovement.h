#pragma once
#include "Common/ref_ptr.h"
#include "Common/Referenced.h"
#include "common/elaptime.h"
#include "LandsideStretchInSim.h"
#include "LandsideVehicleInSim.h"

class ElapsedTime;
class LandsideVehicleInSim;
class CARCportEngine;

//enum emVehicleState
//{
//	_birth,
//	_moveOnRoad,
//	_moveOnCurbside,
//	_enterparking,
//	_onparking,
//	_leaveparking,
//	_terminate,
//};

extern CString strVehicleState[];

//state and planned movements of the vehicle
class IVehicleStateMovement : public Referenced
{
public:	
	IVehicleStateMovement(LandsideVehicleInSim* pVehicle){ mpVehicle = pVehicle; } 
	virtual ~IVehicleStateMovement(){}
	//return next movement
	virtual IVehicleStateMovement* proceedTo(CARCportEngine *_pEngine)=0;	
	//virtual ElapsedTime getNextTime()const =0;	
	virtual int getState()const=0;
	//virtual bool InResource(LandsideResourceInSim* pRes)const=0;
protected:
	LandsideVehicleInSim * mpVehicle;

};	

//////////////////////////////////////////////////////////////////////////
class LandsideResourceInSim;
class LandsideVehicleBirthState : public IVehicleStateMovement
{
	friend class LandsidePaxVehicleInSim;
public:
	LandsideVehicleBirthState(LandsidePaxVehicleInSim*pVehicle);

	virtual IVehicleStateMovement* proceedTo(CARCportEngine *_pEngine);	
	//virtual ElapsedTime getNextTime()const;	
	//virtual int getState()const{ return _birth; }
	virtual bool InResource(LandsideResourceInSim* pRes)const;

protected:
	LandsideResourceInSim* mpBirthPlace;
};	

//////////////////////////////////////////////////////////////////////////
class VehicleFlow
{
public:
	virtual IVehicleStateMovement* NextMovement(IVehicleStateMovement* preMove);
};

