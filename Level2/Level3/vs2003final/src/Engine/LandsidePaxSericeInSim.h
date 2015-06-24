#pragma once
#include "Common\CPPUtil.h"
#include <algorithm>
class CPath2008;
class ElapsedTime;
class PaxLandsideBehavior;
class LandsidePaxSericeInSim
{
public:
	LandsidePaxSericeInSim(void);
	~LandsidePaxSericeInSim(void);

	void AddWaitingPax(PaxLandsideBehavior* pPax){ m_vWaitingPax.Add(pPax);}
	void DelWaitingPax(PaxLandsideBehavior* pPax){ m_vWaitingPax.Remove(pPax); }
	virtual void FlushOnVehiclePaxLog(const ElapsedTime& t);
	int GetWaitingPaxNum()const{ return m_vWaitingPax.size(); }	

public:
	virtual CPath2008 GetQueue() = 0;
	typedef cpputil::TPtrVector<PaxLandsideBehavior> PaxList;
	PaxList m_vWaitingPax;
};
