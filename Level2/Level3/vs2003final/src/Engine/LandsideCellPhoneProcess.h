#pragma once
#include "LandsideTerminalLinkageInSim.h"
class ResidentVehicleRoute;
class LandsidePaxVehicleInSim;
class CARCportEngine;
class ResidentVehicleRouteList;


//the pair of passenger and his vehcile
//the landside object they are gonna met
class LandsideIndividualArrivalVehicleConverger
{
public:
	LandsideIndividualArrivalVehicleConverger(const ALTObjectID& objID);
	~LandsideIndividualArrivalVehicleConverger();

	
	int m_nPersonID;
	int m_nVehicleID;
	LandsideObjectLinkageItemInSim m_ConvergerObject;
};


//to manage the bond of pax and his vehicle
//the bond is the landisde object, the passenger takes his car, 
//no matter his car is in parking lot, or somebody dirvers a car to pick him up at curbside
class LandsideIndividualArrivalPaxVehicleConvergerManager
{
public:
	LandsideIndividualArrivalPaxVehicleConvergerManager();
	~LandsideIndividualArrivalPaxVehicleConvergerManager();

	void InsertConverger(int nPaxID, int nVehicle,LandsideObjectLinkageItemInSim linkObject);

	bool GetLinkageObject(int nPaxID, LandsideObjectLinkageItemInSim& objName)const;

private:
	std::vector<LandsideIndividualArrivalVehicleConverger*> m_vConverger;
};

class ResidentVehicleRouteFlow;
class LandsideCellPhoneProcess
{
public:
	LandsideCellPhoneProcess(LandsidePaxVehicleInSim* pVehicle,ResidentVehicleRouteList* pRoute);
	~LandsideCellPhoneProcess(void);

	ResidentVehicleRoute* GetNextVehicleRoute(CARCportEngine* pEngine,ResidentVehicleRouteFlow* pFlow);

	void UnAvaibleRoute(ResidentVehicleRoute* pRoute);

private:
	ResidentVehicleRoute* GetCellPhoneParkingLot(CARCportEngine* pEngine) const;
	bool GetRouteObjectName(CARCportEngine* pEngine,int nFacility,ALTObjectID& objName)const;
private:
	LandsidePaxVehicleInSim* m_pVehicleInSim;
	ResidentVehicleRouteList* m_pVehicleRoute;
	std::vector<ResidentVehicleRoute*> m_UnAvaibelRouteItems;


};
