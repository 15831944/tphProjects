#pragma once
#include "vector"
#include "map"
#include "TempParkingSegmentsInSim.h"

class CTemporaryParkingCriteriaList;
class TaxiwayInSim;

class AirportResourceManager;
class AirsideFlightInSim;
class TempParkingSegmentsInSim;


class TemporaryParkingInSim
{
public:
	TemporaryParkingInSim(void);
	~TemporaryParkingInSim(void);

	void Init(int nPrjID,AirportResourceManager* pResManager);
	TempParkingSegmentsInSim* GetTemporaryParkingTaxiway(TaxiwayInSim* pTaxiway,AirsideFlightInSim* pFlight);
	TempParkingSegmentsInSim* GetTemporaryParkingTaxiway(AirsideFlightInSim* pFlight);

	int GetParkingCount()const{ return m_vTempParkings.size(); }
	TempParkingSegmentsInSim* GetParkingSeg (int idx){ return m_vTempParkings.at(idx).get(); }
private:
	CTemporaryParkingCriteriaList* m_pTemporaryParkingList;
	std::vector< TempParkingSegmentsInSim::RefPtr > m_vTempParkings;

};
