#pragma once
#include "../MobElementBehavior.h"
//
class Person;
//class CDoor;
//class OnboardSeatInSim;
//class OnboardFlightInSim;

class ENGINE_TRANSFER EnplaneAgentBehavior : public MobElementBehavior
{
public:
	EnplaneAgentBehavior(Person* pPerson);
	~EnplaneAgentBehavior();

//public:
//	virtual int performanceMove(ElapsedTime p_time,bool bNoLog);
//	virtual BehaviorType getBehaviorType(){ return MobElementBehavior::OnboardBehavior; }
//
//
//
//
//private:
//	void processEnterOnBoardMode(ElapsedTime p_time);
//	void processWayFinding(ElapsedTime p_time);
//
//	void processArriveAtEntryDoor(ElapsedTime p_time);
//	void processMoveToSeat(ElapsedTime p_time);
//	void processArriveAtSeat(ElapsedTime p_time);
//	void processSeatingOnChair(ElapsedTime p_time);
//	void processCarrierTakeoffWaiting(ElapsedTime p_time);
//	void processDeath(ElapsedTime p_time);
//
//	void GenetateEvent(ElapsedTime time) ;
//
//	void FlushLog(ElapsedTime p_time);
//private:
//	void setState (short newState);
//    int getState (void) const;
//
//private:
//	Person* m_pPerson;
//
//private:
//	OnboardFlightInSim *m_pOnboardFlight;
//	CDoor *m_pDoor;
//	OnboardSeatInSim *m_pSeat;

};

