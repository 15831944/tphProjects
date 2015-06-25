#pragma once
#include "MOVEVENT.H"
#include "Inputs\ProcessorDistributionWithPipe.h"
#include "Inputs\FlowItemEx.h"
#include "Common\POINT.H"
#include "Inputs\TimePointOnSideWalk.h"

class InputTerminal;

class MoveToInterestedEntryEvent : public MobileElementMovementEvent  
{
private:
	std::vector<CTimePointOnSideWalk> m_pipePointList;
public:
	MoveToInterestedEntryEvent();
	virtual ~MoveToInterestedEntryEvent();

public:
	
	void init(MobileElement *aMover, ElapsedTime eventTime, bool bNoLog);
	
	// processor event
	virtual int process(CARCportEngine *_pEngine);

	//Returns event's name
    virtual const char *getTypeName (void) const { return "MobileElement Move To Interested Entry Point Event"; };
	
    //Returns event type
    int getEventType (void) const { return MobileElementMoveToIntsEntryEvent; };

	// calculate pipe
	void calculateMovingPipe(Point ptFrom, Point ptTo);

private:
    void calculateUserPipe(Point ptFrom, Point ptTo, CFlowItemEx* pFlowItem);
	void calculateSystemPipe(Point ptFrom, Point ptTo);
	CFlowItemEx* findFlowItem();
};

