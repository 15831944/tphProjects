#pragma once


class CSideStepSpecification;
class AirportResourceManager;
class AirsideFlightInSim;
class ClearanceItem;
class LogicRunwayInSim;

class SideStepIntervention
{
public:
	SideStepIntervention(void);

	void Init(int nprjId, AirportResourceManager * pAirportRes );
	~SideStepIntervention(void);

	LogicRunwayInSim * GetSideStepRunway(AirsideFlightInSim * pFlight, ClearanceItem& lastItem);

protected:
	CSideStepSpecification* m_pSideStepSpec;
	AirportResourceManager * m_pAirportRes;

};
