#pragma once
#include "./ResourceManager.h"
#include "./AirspaceResourceManager.h"
#include "./AirportResourceManager.h"

//interface of the Airside Resources
class AirsideSimConfig;
class AirsideReportingAreaInSim;
class AircraftClassificationManager;
class ENGINE_TRANSFER AirsideResourceManager : public ResourceManager
{

friend class AirsidePlaner;
friend class AirsideTrafficController;

public:
	
	bool Init(int nPrjID, CAirportDatabase* pAirportDB, AircraftClassificationManager* pAircraftClassification,const AirsideSimConfig& simconf);

	~AirsideResourceManager();

	AirportResourceManager * GetAirportResource(int nAirportID);
	AirportResourceManager * GetAirportResource();

	AirRouteNetworkInSim * GetAirspaceResource();

	//RunwayInSim * GetRunway(const RunwayKeyInSim& rwKey)const;
	//RunwayInSim * GetAnyRuway()const;

	AirsideReportingAreaInSim* GetInReportingArea(const CPoint2008& point);
	CString GetInSectorNameAndID(const CPoint2008& point,double dFltAlt,int& nSectorID);

	typedef std::vector<AirportResourceManager*> AirportResourceList;

protected:
	bool InitReportingArea(int nPrjID, int nAirportID);

protected:
	
	AirRouteNetworkInSim m_AirspaceRes;
	AirportResourceList m_vAirportRes;
	std::vector<AirsideReportingAreaInSim*> m_vReportingArea; 
	ALTObjectList vSectors;	

	
};
