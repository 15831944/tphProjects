#pragma once
class AircraftClassificationManager;
class CInTrailSeparationListEx;
class CAircraftSectorManager;
class AirsideFlightInSim;
class ElapsedTime;

class CInTrailSeparationInSim
{
public:
	CInTrailSeparationInSim(void);
	~CInTrailSeparationInSim(void);

	void Init(AircraftClassificationManager* pAircraftClassificationManager, int nProjID);
	void GetInTrailSeparationRadiusOfConcern(double& dRadiusofConcern);

	//Called the GetAircraftClass() function but not catch the exception,throw AirsideDiagnose exception
	void GetInTrailSeparationDistanceAndTime(AirsideFlightInSim* pTrailFlight, AirsideFlightInSim* pLeadFlight, double dTrailFlightAltitude, double& dMinDistance, ElapsedTime& tMinTime);

	//Called the GetAircraftClass() function but not catch the exception,throw AirsideDiagnose exception
	void GetInTrailSeparationSpatialConvergingAndDiverging(AirsideFlightInSim* pTrailFlight, AirsideFlightInSim* pLeadFlight, double dTrailFlightAltitude, double& dSpatialConverging, double& dSpatialDiverging);

private:
	AircraftClassificationManager*        m_pAircraftClassificationManager;
	CInTrailSeparationListEx*             m_pInTrailSeparationListEx;
	CAircraftSectorManager*               m_pAircraftSectorManager;
};
