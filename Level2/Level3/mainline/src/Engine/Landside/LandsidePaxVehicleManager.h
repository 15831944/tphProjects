#pragma once
#include "LandsidePaxVehicleInSim.h"
#include "../EVENT.H"
//////////////////////////////////////////////////////////////////////////

//generate the vehicle by the schedule
class LandsidePaxVehicleManager : public Event
{
	//friend class LandsidePaxVehicleEntryEvent;
public:
//	functions called by simulation
	bool Init( CARCportEngine *pEngine, ElapsedTime& t );
	void Start(const ElapsedTime&t );
	bool IsEmpty()const { return m_vPlanVehicles.empty(); }
//
//	other functions
	bool GetVehicleInfo(int nPax,PaxVehicleEntryInfo& vehicleInfo);


// override functions of event
	virtual int process (CARCportEngine* pEngine );
	virtual int kill (void){ return FALSE; }
	virtual const char *getTypeName (void) const{ return _T("Pax Vehicle Entry Event"); }
	virtual int getEventType (void) const{  return NormalEvent; }
//


protected:
	typedef std::list<PaxVehicleEntryInfo> VehicleInfoList;
	VehicleInfoList m_vPlanVehicles;
	void InitEntryTime(CARCportEngine* pEngine);  //estimate the entry time of the vehicles and start

	PaxVehicleEntryInfo* CreateNewVehicleInfo(LandsideVehicleAssignEntry* pVehicleAssignEntry, const ElapsedTime& tEntry,CARCportEngine *pEngine);
	PaxVehicleEntryInfo* FindtheCloseVehicleInfo(LandsideVehicleAssignEntry* pVehicleAssignEntry,const MobLogEntry& paxEntry);
	void UpdateStartTime( const ElapsedTime& t );

	//if success, return true
	bool AssignPaxToVehicleInfo(LandsideVehicleAssignEntry* pVehicleAssignEntry,MobLogEntry& paxEntry, CARCportEngine *pEngine);
	
	void ScheduleNext();

};
