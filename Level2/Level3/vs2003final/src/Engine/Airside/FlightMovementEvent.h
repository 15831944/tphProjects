#pragma  once

#include "AirsideMobileElementEvent.h"
#include "FlightInAirsideSimulation.h"
#include "Clearance.h"

//event the flight move to next state
class FlightMovementEvent : public CAirsideMobileElementEvent
{
public:

	FlightMovementEvent(AirsideFlightInSim * ,const ClearanceItem& item);
	int Process();	//Event process
	


	AirsideFlightInSim * getFlight() const { return m_pFlight ; } 
	
	virtual const char *getTypeName (void) const { return "FlightMovement";}

	virtual int getEventType (void) const {return FlightMovement;}


private:
	
	AirsideFlightInSim * m_pFlight;
	ClearanceItem m_clearanceitem;

};