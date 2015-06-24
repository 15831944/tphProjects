#pragma once
#include "processorfamily.h"
#include "engine\proclist.h"
#include "engine\ElevatorProc.h"

class ElevProcessorFamily : public ProcessorFamily
{
public:
	ElevProcessorFamily(void);
	~ElevProcessorFamily(void);

	bool GetMaxMinFloorOfFamily( ProcessorList* pProcList, int& _nMaxFloor, int& _nMinFloor );

};
