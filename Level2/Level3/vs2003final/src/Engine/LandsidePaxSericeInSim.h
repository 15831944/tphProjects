#pragma once
#include "Common\CPPUtil.h"
#include <algorithm>
class CPath2008;
class ElapsedTime;
class Person;
class CARCportEngine;
class LandsidePaxSericeInSim
{
public:
	LandsidePaxSericeInSim(void);
	~LandsidePaxSericeInSim(void);

	void AddWaitingPax(int PaxId);
	void DelWaitingPax(int PaxId);
	virtual void FlushOnVehiclePaxLog(CARCportEngine* pEngine,const ElapsedTime& t);
	int GetWaitingPaxNum()const;	

public:
	virtual CPath2008 GetQueue() = 0;
	typedef std::vector<int> PaxList;
	PaxList m_vWaitingPax;
};
