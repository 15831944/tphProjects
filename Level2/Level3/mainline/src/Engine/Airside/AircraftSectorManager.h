#pragma once
class AirsideFlightInSim;
#include <boost/noncopyable.hpp>
class AirSector;
class CAircraftSectorManager : public boost::noncopyable
{
public:
	CAircraftSectorManager(int nProjID);
	~CAircraftSectorManager(void);

	//return the Sector id, if can't find the sector return -1
	int GetSectorID(AirsideFlightInSim* pFlightInSim, double dFlightAltitude);

private:
	int m_nProjID;
	//ALTObjectUIDList m_vSectorID;
	std::vector<AirSector*> mvSectors;
	
};
