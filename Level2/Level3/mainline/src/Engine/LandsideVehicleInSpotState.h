#pragma once
#include "LandsideVehicleStateMovement.h"

///////////////////////////////parking at the parking spot///////////////////////////////////////////
class LandsideParkingSpotInSim;
class VehicleParkInSpotState: public IVehicleStateMovement
{
public:
	VehicleParkInSpotState(LandsideVehicleInSim* pVehicle, LandsideParkingSpotInSim* pSpot);

	virtual IVehicleStateMovement* proceedTo(CARCportEngine *_pEngine);
	//virtual ElapsedTime getNextTime()const;	
	virtual int getState()const{ return _onparking; }
	virtual bool InResource(LandsideResourceInSim* pRes)const;

	void WriteAtStateLog(const ElapsedTime& t);
protected:
	LandsideParkingSpotInSim* mpSpot;
	//bool bUnloadPickingPax;
};
///////////////////////////////entering parking lot///////////////////////////////////////////
class LandsideParkingLotInSim;
class LandsideParkingLotDoorInSim;
class VehicleEnterParkingLot : public IVehicleStateMovement
{
public:
	VehicleEnterParkingLot(LandsideVehicleInSim* pVehicle,LandsideParkingLotDoorInSim* pDoor, LandsideParkingSpotInSim* pSpot);
	virtual IVehicleStateMovement* proceedTo(CARCportEngine *_pEngine);

	//virtual ElapsedTime getNextTime()const;	
	virtual int getState()const{ return _enterparking; }
	virtual bool InResource(LandsideResourceInSim* pRes)const;

protected:
	LandsideParkingLotDoorInSim* mpLotDoor;
	LandsideParkingSpotInSim* mpSpot;
};
/////////////////////////////leavening parking lot/////////////////////////////////////////////
class LandsideParkingSpotInSim;
class DrivePathInParkingLot;
class VehicleLeaveParkingLot : public IVehicleStateMovement
{
public:
	VehicleLeaveParkingLot(LandsideVehicleInSim* pVehicle,  LandsideParkingSpotInSim* pSpot,LandsideParkingLotDoorInSim* pDoor);
	virtual IVehicleStateMovement* proceedTo(CARCportEngine *_pEngine);


	virtual IVehicleStateMovement* goToNextState(CARCportEngine* _pEngine);

	//virtual ElapsedTime getNextTime()const;	
	virtual int getState()const{ return _leaveparking; }
	virtual bool InResource(LandsideResourceInSim* pRes)const;
protected:	
	LandsideParkingLotDoorInSim* mpLotDoor;	
	LandsideParkingSpotInSim* mpSpot;
	DrivePathInParkingLot* mDrivePath;
};
