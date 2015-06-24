#pragma once
#include "Common/elaptime.h"

class BoardingCallEvent;
class FlightItemBoardingCallMgr
{
public:
	FlightItemBoardingCallMgr(int nFltIndex);
	virtual ~FlightItemBoardingCallMgr(void);
	
public:
	int  GetFlightIndex();
	virtual bool IsStarted(){	return m_bIsStarted; }
	
	void ScheduleBoardingCallEvents(const ElapsedTime& currentTime,const ElapsedTime& depTime);
	void ScheduleDirectly();
	void ScheduleSecondDirectly();
	void AddBoardingCallEvent(BoardingCallEvent* pNewItem);
	void SetPlannedDepTime(const ElapsedTime& dtPlannedDepTime);

	void delayEvents(ElapsedTime delay);

protected:
	int		m_nFltIndex;
	ElapsedTime m_dtPlannedDepTime;

	bool    m_bIsStarted;
	std::vector<BoardingCallEvent*> m_vectEvent;
};
