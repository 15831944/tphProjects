#pragma once
#include "../LandsideVehicleInSim.h"
#include <list>
#include "../LandsideVehicleEntryInfo.h"



//////////////////////////////////////////////////////////////////////////
class ResidentVehicleRoute;
class LandsidePaxVehicleInSim : public LandsideVehicleInSim
{
public:
	LandsidePaxVehicleInSim(){}
	LandsidePaxVehicleInSim(const PaxVehicleEntryInfo& entryInfo);

	virtual bool IsPaxsVehicle( int nPaxId ) const; 
	virtual ResidentVehiclePlan* getRoutePlan(){ return m_entryInfo.pPlan; }

	bool HavePaxGetOff( LandsideResourceInSim* pRes ) const;
	void CallPaxGetOff( LandsideResourceInSim* pRes );


	//generate pax on this vehicle , only happens when simulation without terminal
	int GenerateArrivalPax(CARCportEngine* pEngine);

	bool CellPhoneCall(CARCportEngine* pEngine)const;

	const PaxVehicleEntryInfo& getEntryInfo()const{ return m_entryInfo ; }

	virtual void NotifyPaxMoveInto(CARCportEngine*pEngine, LandsideBaseBehavior* pPassenger, const ElapsedTime& eTime);
protected:
	PaxVehicleEntryInfo m_entryInfo;
	int GenerateDeparturePax(CARCportEngine*pEngine); // give birth of the pax on this vehicle. for the departure pax , it birth on the vehicle first.
};
