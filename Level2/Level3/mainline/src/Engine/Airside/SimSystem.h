#pragma once
//#include <simulation/event.h>

class SimAgentEvent;
class SimSystem
{
public:	
	SimSystem(){ nState = 0; }

	//void Run();

	//virtual void addEvent(Event *pEvent){ mEvents.add(pEvent); }

	void stateUp(){ nState++; }
	int getState()const{ return nState; }

	static SimSystem& Instance(){
		  static SimSystem ins;
		  return ins;
	}
protected: 
	//EventList mEvents;
	int nState;

};


