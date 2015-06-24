#pragma once

class CSinglePaxTypeFlow;
class ProcessorID;

class HandleSingleFlowLogic
{
public:
	HandleSingleFlowLogic();
    ~HandleSingleFlowLogic();

public:
	static bool IfHaveCircleFromProc(CSinglePaxTypeFlow* pSingleFlow,const ProcessorID& _fromProc );
};