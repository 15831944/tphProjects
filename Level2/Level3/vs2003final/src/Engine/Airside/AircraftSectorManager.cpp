#include "StdAfx.h"
#include ".\aircraftsectormanager.h"
#include "../../InputAirside/ALTAirspace.h"
#include "../../InputAirside/AirSector.h"
#include "../../Common/pollygon2008.h"
#include "AirsideFlightInSim.h"

CAircraftSectorManager::CAircraftSectorManager(int nProjID)
: m_nProjID(nProjID)
{
	ALTObjectUIDList vSectorIDs;
	ALTAirspace::GetSectorList(m_nProjID, vSectorIDs);	
	
	for (ALTObjectUIDList::size_type i = 0; i < vSectorIDs.size(); ++i )
	{		
		AirSector* pnewSector = new AirSector(); 
		pnewSector->ReadObject(vSectorIDs[i]);
		mvSectors.push_back(pnewSector);
	}
}

CAircraftSectorManager::~CAircraftSectorManager(void)
{
	for (size_t i = 0; i < mvSectors.size(); ++i )
	{
		delete mvSectors.at(i);
	}
	mvSectors.clear();
}

int CAircraftSectorManager::GetSectorID(AirsideFlightInSim* pFlightInSim, double dFlightAltitude)
{
	ASSERT(NULL != pFlightInSim);	

	for (size_t i = 0; i < mvSectors.size(); ++i )
	{
		AirSector& airSectorObj = *mvSectors[i];
		//airSectorObj.ReadObject(m_vSectorID[i]);

		double dLowAltitude = airSectorObj.GetLowAltitude();
		double dHeighAltitude = airSectorObj.GetHighAltitude();

		//check if altitude is invalid
		if (dFlightAltitude-dLowAltitude <= 0.000001
			&& dFlightAltitude-dHeighAltitude >= 0.000001)
		{
			continue;
		}

		//check if flight is in bound
		CPath2008 sectorPath = airSectorObj.GetPath();

		CPollygon2008 sectorPollygon;
		sectorPollygon.init(sectorPath.getCount(), sectorPath.getPointList());

		int nFlightInBound = sectorPollygon.contains(pFlightInSim->GetPosition());

		//flight isn't in the sector
		if (nFlightInBound == 0)
		{
			continue;
		}

		return airSectorObj.getID();
	}

	return -1;
}