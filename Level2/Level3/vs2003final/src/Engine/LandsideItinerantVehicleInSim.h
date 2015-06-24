#pragma once
#include "LandsideVehicleInSim.h"
#include <deque>
class LandsideItinerantVehicleInSim : public LandsideVehicleInSim
{

public:
	LandsideItinerantVehicleInSim( CVehicleItemDetail* pVehicleType );

	virtual bool InitBirth(CARCportEngine *_pEngine);
};



class CVehicleItemDetailList;
class LandsideItineratVehicleGenerator : public SAgent
{
public:
	LandsideItineratVehicleGenerator(){ pNextType = NULL; }
	void InitSchedules(InputTerminal* pInTerm,CVehicleItemDetailList & typelist);

	virtual void OnTerminate(CARCportEngine*pEngine){}
 
	void ScheduleNext();

	virtual void OnActive(CARCportEngine*pEngine);


protected:
	std::vector<LandsideVehicleInSim*> m_vGeneratedVehicles;
	CVehicleItemDetail* pNextType;

	struct ItrVehicleGenSchedule
	{
		CVehicleItemDetail* pType;
		ElapsedTime mGenTime;
		bool operator<(const ItrVehicleGenSchedule& sch)const{
			return mGenTime< sch.mGenTime;
		}
	};
	void AddSchedule(CVehicleItemDetail* pType, const ElapsedTime& t)
	{
		ItrVehicleGenSchedule sch;
		sch.pType = pType;
		sch.mGenTime = t;
		m_vSchedules.push_back(sch);
	}
	void SortSchedule(){ std::sort(m_vSchedules.begin(),m_vSchedules.end()); }
	
	std::deque<ItrVehicleGenSchedule> m_vSchedules; 
};
