#pragma once

class FlightTypeDetailsManager;
class InputTerminal;

class CFlightServiceManagerInSim
{
public:
	CFlightServiceManagerInSim(int nPrjID, InputTerminal * pInterm);
	~CFlightServiceManagerInSim(void);

public:
	FlightTypeDetailsManager *GetFlightServiceManager(){ return m_pFlightServiceManager;}


protected:
	FlightTypeDetailsManager *m_pFlightServiceManager;
	InputTerminal *m_pInputTerminal;
	int m_nProjID;
};









