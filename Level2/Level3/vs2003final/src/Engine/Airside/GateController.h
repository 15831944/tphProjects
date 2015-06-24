#pragma once
#include "../EngineDll.h"
#include "common\elaptime.h"
#include "common\util.h"
#include "AirsideController.h"

class Flight;

class ENGINE_TRANSFER GateController : public AirsideController
{
public:
	GateController(void);
	~GateController(void);

	virtual ResourceOccupy ResourceAllocation(Flight *pflight,ResourceOccupy presource);
	virtual bool ConflictJudge();
};
