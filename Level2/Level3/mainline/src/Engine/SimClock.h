#ifndef __SIMULATION_CLOCK_DEF
#define __SIMULATION_CLOCK_DEF

#pragma once
#include "enginedll.h"
#include <iostream>
#include <ctime>


#include "../common/elaptime.h"

// The Simulation Clock
class ENGINE_TRANSFER SimClock
{
public:
	typedef long TimeType;                  // as ElapseTime
	typedef long TimeSpanType;

public:
	SimClock(void);
	SimClock(const TimeType& _start_time,const TimeType& _end_time);
	~SimClock(void);
	
	void setStartTime(const TimeType& _start_time){m_startTime = _start_time; }
	void setEndTime(const TimeType& _end_time){m_endTime = _end_time; }
	void setCurrentTime(const TimeType& _cur_time ){m_curTime = _cur_time; }

	TimeType getStartTime()const { return m_startTime; }
	TimeType getEndTime()const { return m_endTime; }
	TimeType getCurrentTime()const { return m_curTime ;}

	TimeSpanType getElapsedTime()const {return (m_curTime - m_startTime); }
	TimeSpanType getLeftTime()const {return (m_endTime - m_curTime);}

	
	SimClock& operator += (const TimeSpanType& _time_span);	
	SimClock& operator -= (const TimeSpanType& _time_span);

	bool IsSimStart()const{
		return m_curTime >= m_startTime;
	}
	bool IsSimEnd()const { 
		return m_curTime > m_endTime;
	}


private:
	
	TimeType m_startTime;               // start time of the simulation 
	TimeType m_endTime;                 // end time of the simulation
	TimeType m_curTime;                 // current time of the simulation


};



#endif