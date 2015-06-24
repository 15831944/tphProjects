#pragma once


#include "FlightInAirsideSimulation.h"

class ENGINE_TRANSFER FlightMovements
{
public:
	
	//
	explicit FlightMovements(FlightInAirsideSimulation * pFlight){ m_pFlight = pFlight; }


	int GetStateCount()const{ return m_vMovements.size(); }
	FlightState& GetState(int idx){ return m_vMovements[idx]; }
	
	FlightStateList& GetStateList(){ return m_vMovements; }

	void BeginState(const FlightState& _fltstate);
	virtual void End(Clearance * pClearance);
	
	ElapsedTime GetElapsedTime()const;
	FlightState GetEndState()const;

protected:
	FlightInAirsideSimulation * m_pFlight;
	FlightStateList m_vMovements;
	FlightState m_beginState;

};


class TaxiwayDirectSegInSim;


//only handle landing movements
class RunwayExitInSim;
class ENGINE_TRANSFER FlightLanding : public FlightMovements
{
public:
	explicit FlightLanding(FlightInAirsideSimulation * pFlight):FlightMovements(pFlight){}

	
	virtual void End(Clearance * pClearance);
protected:
	
};


//final approach 
class ENGINE_TRANSFER FlightFinalApproach : public FlightMovements
{
public:
	explicit FlightFinalApproach(FlightInAirsideSimulation * pFlight):FlightMovements(pFlight){}
	virtual void End(Clearance * pClearance);
};

