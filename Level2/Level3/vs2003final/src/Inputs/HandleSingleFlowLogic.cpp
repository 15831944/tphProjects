#include "stdafx.h"
#include "HandleSingleFlowLogic.h"
#include "SinglePaxTypeFlow.h"
#include "commondata/procid.h"

bool HandleSingleFlowLogic::IfHaveCircleFromProc(CSinglePaxTypeFlow* pSingleFlow,const ProcessorID& _fromProc )
{
	bool bHasCircle = pSingleFlow->IfHaveCircleFromProc(_fromProc);
	//reset the destination list unvisited
	pSingleFlow->ResetDesitinationFlag(false);
	return bHasCircle;
}