#pragma once
#include "LandsideNonPaxVehicleInSim.h"
//////////////////////////////////////////////////////////////////////////
class VehicleTypeEntry;
class CARCportEngine;
class ElapsedTime;
//generate the vehicle by the schedule
class LandsideNonPaxVehicleManager
{
	friend class LandsideNonPaxVehicleEntryEvent;
public:
	void Start(const ElapsedTime&t );
	//initialize and update the start time
	bool Init(CARCportEngine* pEngine, ElapsedTime&t );
		
	bool IsEmpty()const{ return m_vPlanVehicles.empty(); }
protected:
	typedef std::list<NonPaxVehicleEntryInfo> VehicleInfoList;
	VehicleInfoList m_vPlanVehicles;
	void ScheduleNext();
	void UpdateStartTime(const ElapsedTime& t);
	bool Pop(NonPaxVehicleEntryInfo& front);

	bool HasMore(ElapsedTime& nextT);

	void InitEntryTime(CARCportEngine* pEngine);
};
