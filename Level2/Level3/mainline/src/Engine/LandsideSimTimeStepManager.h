#pragma once
#include "event.h"
class LandsideVehicleInSim;
class LandsideSimTimeStepEvent;



class LandsideSimTimeStepManager
{
	class LsTimeStepEvent
	{
	public:
		LsTimeStepEvent(const ElapsedTime& eEventTime, const ElapsedTime& eStepTime)
			:m_eEventTime(eEventTime)
			,m_eTimeStep(eStepTime)
		{

		}
		~LsTimeStepEvent(){}


	public:
		ElapsedTime m_eEventTime;
		ElapsedTime m_eTimeStep;
	};


public:
	LandsideSimTimeStepManager();


	void Initialize();

	static bool CompareTimeStep(const LsTimeStepEvent& event1, const LsTimeStepEvent& event2);
	void ScheduleNextEvent();

protected:

protected:


	std::vector< LsTimeStepEvent > m_vEventList;
	LandsideSimTimeStepEvent	*m_pNextEvent;
};
