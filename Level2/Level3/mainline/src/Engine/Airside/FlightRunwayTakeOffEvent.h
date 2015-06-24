#pragma once
#include "airsidemobileelementevent.h"
#include "Clearance.h"
#include "AirsideFlightInSim.h"

class AirEntrySystemHoldInInSim;

class CFlightDelayTakeoffOnRwyEvent :
	public CAirsideMobileElementEvent
{

public:
	CFlightDelayTakeoffOnRwyEvent(AirsideFlightInSim * pFlight,Clearance& takeoffClearance);
	~CFlightDelayTakeoffOnRwyEvent();

	//void SetClearance(Clearance& takeOffClearance);
	
	int Process();



	AirsideFlightInSim * getFlight() const { return m_pFlight ; } 

	virtual const char *getTypeName (void) const { return "Flight Delay Takeoff On Rwy";}

	virtual int getEventType (void) const {return FlightDelayTakeoffOnRwy;}
	
protected:
	Clearance m_takeOffClearance;
protected:
	AirsideFlightInSim * m_pFlight;


};

class CFlightTakeOffOnRwyEvent :
	public CAirsideMobileElementEvent
{

public:
	CFlightTakeOffOnRwyEvent(AirsideFlightInSim * pFlight);
	~CFlightTakeOffOnRwyEvent();

	int Process();

	AirsideFlightInSim * getFlight() const { return (AirsideFlightInSim*)m_pMobileElement ; } 

	virtual const char *getTypeName (void) const { return "Flight Take Off On Rwy";}

	virtual int getEventType (void) const {return FlightTakeOffOnRwy;}


};

class CFlightDelayOnHoldEvent : public AirsideEvent
{
public:
	CFlightDelayOnHoldEvent(AirsideFlightInSim * pFlight,AirEntrySystemHoldInInSim* pEntrySystemHold);
	~CFlightDelayOnHoldEvent();

	int Process();

	AirsideFlightInSim * getFlight() const { return m_pFlightInSim ; } 

	virtual const char *getTypeName (void) const { return "Flight Delay On Hold";}

	virtual int getEventType (void) const {return FlightDelayOnHold;}

protected:
	AirEntrySystemHoldInInSim* m_pEntrySystemHold;
	AirsideFlightInSim* m_pFlightInSim;
};