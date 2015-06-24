#pragma once

////generate terminal ,airside , landside,
///pax, train , vehicle ,also can  flight all in one because they are all connected with the schedule
//////////////////////////////////////////////////////////////////////////
////
class MobileAgentsGenerator
{
public:
	MobileAgentsGenerator(void);
	~MobileAgentsGenerator(void);


public:

protected:
	// make next agent Entry event
	int scheduleNextEntry ();

};
