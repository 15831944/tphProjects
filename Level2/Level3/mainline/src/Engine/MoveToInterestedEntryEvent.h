#pragma once
#include "MOVEVENT.H"

class InputTerminal;
// avoid density event
class MoveToInterestedEntryEvent : public MobileElementMovementEvent  
{
private:
	
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

};

