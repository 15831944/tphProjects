#pragma once
#include "mobile.h"

class HoldingArea;
class Person;
class ElapsedTime;

class HoldAreaPax
{
public:
	HoldAreaPax();
	~HoldAreaPax();

	void SetHold(HoldingArea* pHold);
	HoldingArea* GetHold()const;

	void AddPax(Person* pPerson);
	void Process(const ElapsedTime& time);
private:
	MobileElementList m_mobileList;
	HoldingArea*	m_pHoldArea;
};


class HoldAreaProcessPaxBus
{
public:
	HoldAreaProcessPaxBus(void);
	~HoldAreaProcessPaxBus(void);

	void Process(const ElapsedTime& time);
	void AddHoldPax(HoldingArea* pHoldarea, Person* pPerson);

private:
	void Clear();
private:
	std::vector<HoldAreaPax*> m_holdPaxList;
};