#include "StdAfx.h"
#include ".\gatecontroller.h"
#include "Inputs/flight.h"

GateController::GateController(void)
{
}

GateController::~GateController(void)
{
}

ResourceOccupy GateController::ResourceAllocation(Flight *pflight,ResourceOccupy presource)
{


	if (ConflictJudge())							//without conflict
	{

	}
		return presource;

}

bool GateController::ConflictJudge()
{

	return false;
}