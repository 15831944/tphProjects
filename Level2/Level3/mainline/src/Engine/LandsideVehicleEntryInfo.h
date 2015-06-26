#pragma once
#include "..\Common\HierachyName.h"
#include "..\Common\elaptime.h"
#include "Landside\VehicleOperationNonPaxRelatedContainer.h"


class ResidentVehicleRoute;
class NonResidentVehicleTypePlan;
class ResidentVehicleRouteList;
class NonResidentVehicleTypePlan;
class CARCportEngine;
class LandsideVehicleProperty;


class VehicleEntryInfo
{
public:
	VehicleEntryInfo();
	CHierachyName vehicleType;
	ElapsedTime tEntryTime;
	//ResidentVehicleRouteList* pStartRoute;  //need this info to get the entry offset
	ResidentVehicleRoute* pFirstRoute;
	NonResidentVehicleTypePlan* pPlan;
	//init route plan non resident 
	void InitName(const CString& name,CARCportEngine *pEngine);
	virtual bool InitNoResidentRoute(CARCportEngine* pEngine);
	bool IsBirthAtParkingLot()const;
	bool operator<(const VehicleEntryInfo& other)const;
};

//non pax vehicle entry
class VehicleTypeEntry;
class NonPaxVehicleEntryInfo : public VehicleEntryInfo
{
public:
	//ElapsedTime tActiveTime;
	//ElapsedTime tWaittingTime;
	VehicleDemandAndPoolData::DemandType  iDemandType;
	int iDemandID;

	bool InitEntryTimeOfVehicle(CARCportEngine* pEngine);
	virtual bool InitNoResidentRoute(CARCportEngine* pEngine);
};

//////////////////////////////////////////////////////////////////////////
class MobLogEntry;
class LandsideVehicleAssignEntry;
class CMobileElemConstraint;
class PaxVehicleEntryInfo : public VehicleEntryInfo
{	
public:
	bool InitEntryTimeOfVehicle(CARCportEngine* pEngine);
	virtual bool InitNoResidentRoute(CARCportEngine* pEngine);


	MobLogEntry* GetLogEntry(int nId);
	const MobLogEntry* GetLogEntry(int nId)const;
	bool isPublic()const;

	//for 
	bool isTaxi()const;
	bool isPrivate()const;
	bool isScheduleBus()const;


	PaxVehicleEntryInfo();
	void AddPaxEntry(const MobLogEntry& mobEntry);

	void setVehicleAssignEntry(LandsideVehicleAssignEntry* p); 
	LandsideVehicleAssignEntry* getVehicleAssignEntry()const;

	//
	static CMobileElemConstraint getConstrain(MobLogEntry& m_logEntry,CARCportEngine* pEngine );
	
	int getPaxCountInEntryList() const;

	bool IsVacant(int nCount) const;

	LandsideVehicleProperty* getVehicleProperty() const;
	void setVehicleProperty(LandsideVehicleProperty* pVehicleProp);
public:
	std::vector<MobLogEntry> vPaxEntryList;
protected:
	//vehicle assignment 
	LandsideVehicleAssignEntry* pVehicleAssignEntry;
	LandsideVehicleProperty* m_pVehicleProperty;

};
