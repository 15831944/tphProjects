#pragma once
#include "common\IEnvModeEditContext.h"



class CAirsideEditContext : public IAirsideEditContext
{
public:
	CAirsideEditContext(void);
	~CAirsideEditContext(void);

	virtual InputAirside* getInput();
	virtual double GetActiveFloorHeight()const;

};
