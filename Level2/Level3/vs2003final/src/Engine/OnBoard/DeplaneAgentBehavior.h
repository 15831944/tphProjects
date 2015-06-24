#pragma once
#include "../MobElementBehavior.h"

class DeplaneAgent;

class ENGINE_TRANSFER DeplaneAgentBehavior : public MobElementBehavior
{
public:
	DeplaneAgentBehavior(DeplaneAgent* pAgent);
	~DeplaneAgentBehavior();

//public:
//	virtual int performanceMove(ElapsedTime p_time,bool bNoLog);
//	virtual BehaviorType getBehaviorType(){ return MobElementBehavior::OnboardBehavior; }
//
//private:
//	void processEnterOnBoardMode(ElapsedTime p_time);
//	void processWayFinding(ElapsedTime p_time);
//	void processGeneralMovement(ElapsedTime p_time);
//	void processArriveAtExitDoor(ElapsedTime p_time);
//
//	void processSeatingOnChair(ElapsedTime p_time);
//	void processWaitingForLeaveCarrier(ElapsedTime p_time);
//
//private:
//	DeplaneAgent* m_pAgent;
//
//	// debug poupose.
//	bool mNeedStandup;
};
