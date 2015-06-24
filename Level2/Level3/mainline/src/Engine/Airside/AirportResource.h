#pragma once
#include "../EngineDll.h"
#include "../EngineConfig.h"
#include "../../Common/Referenced.h"


class CAirsideInput;
class InputAirside;

class AirspaceInSim;
class RunwayInSim;
class TaxiwayInSim;
class GateInSim;
class ALTAirport;

class ENGINE_TRANSFER AirportResource
{
public:
	AirportResource(void);
	~AirportResource(void);

	AirspaceInSim * getAirspace(){return m_pAirspace;}
	RunwayInSim * getRunway(){ return m_pRunway; }
	TaxiwayInSim * getTaxiway(){ return m_pTaxiway; }
	GateInSim * getGate(){ return m_pGate; }

	//build the airport from the Input
	bool Build(int projID);

	void Clear();
private:
	AirspaceInSim * m_pAirspace;
	RunwayInSim * m_pRunway;
	TaxiwayInSim * m_pTaxiway;
	GateInSim * m_pGate;

};
