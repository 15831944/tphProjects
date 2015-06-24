#pragma once
#include "terminal.h"
class CBusBoardingCall
{
public:
	CBusBoardingCall(Terminal* _terminal);
	~CBusBoardingCall(void);
	typedef std::vector<Passenger*>  PAX_LIST ;
	typedef PAX_LIST::iterator PAX_LIST_ITER ;
public:
	//boarding call holdingarea 
	//pFlight In: boardingcall flight
	//time In: boardingCall time 
	//_paxlist Out : the passengers which holdingarea release 
	//pax_num  In : the number of passenger who will be released 
	//return : actual number of passenger who have been released 
	int BoardingCallHoldingArea(Flight* pFlight , ElapsedTime time ,int pax_num,CAirsidePaxBusInSim* _bus) ;
private:
	Terminal* m_pterminal ;

};
