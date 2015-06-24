#pragma once

#include <CommonData/procid.h>
#include "../Inputs/MobileElemConstraint.h"
//#include "../Common/elaptime.h"

#include <vector>

class Person;

class AllMobileElementState
{
public:
	AllMobileElementState():bReady(false), bWait(false){}
	Person* pMobileElement;
	bool bReady;
	bool bWait;
};

class NonPaxSyncItem
{
public:

	long	nonPaxIndex;
	ProcessorID nonPaxProcID;
};

class CSimFlowSyncItem
{
public:
	CSimFlowSyncItem(void);
	~CSimFlowSyncItem(void);

	bool CanIGoFurther(Person* pMobElement, const ProcessorID& procID, const ElapsedTime& curTime);
 
	//TODO: access level
	CMobileElemConstraint m_paxConstraint;	// used for find more detail one.
	ProcessorID m_paxProcID;
	std::vector<NonPaxSyncItem> m_vectNonPaxSyncItem;
	std::vector<AllMobileElementState> m_vectAllPaxState;

private:
	bool NeedToSync(Person* pMobElement, const ProcessorID& procID);
	void NotifyPaxToGoFurther(const ElapsedTime& curTime);

};



