#pragma once
#include "EVENT.H"

class SEvent : public Event
{
public:
	virtual ~SEvent();
	virtual void doProcess(CARCportEngine* pEngine){}

	static ElapsedTime curTime();
	void removeFromEventList();	
	bool isInList()const{ return bInList; }
protected:
	SEvent();
	bool bInList;
	void addEvent();
	virtual int kill(){ bInList = false; return FALSE; }
	virtual int process(CARCportEngine* pEngine );
};