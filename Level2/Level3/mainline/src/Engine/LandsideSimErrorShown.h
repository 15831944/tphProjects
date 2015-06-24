#pragma once
#include "LandsideParkingLotInSim.h"


class ElapsedTime;
class LandsideVehicleInSim;
class Person; 

class LandsideSimErrorShown
{
public:
	static void VehicleSimWarning(LandsideVehicleInSim* pVehicle, const CString& strError, const CString& errorType, const ElapsedTime& tTime);
	static void PaxSimWarning(Person* pPax, const CString& strError, const CString& errorType, const ElapsedTime& tTime );

public:
	static void NoSpotInParkingLot(LandsideParkingLotInSim* pLot, LandsideVehicleInSim* pVehicle);
	
	static CString sDefineError;
	static CString sRuntimeError;
};
