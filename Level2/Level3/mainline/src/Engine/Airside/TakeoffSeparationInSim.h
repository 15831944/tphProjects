#pragma once
#include "../../InputAirside/ALT_BIN.h"
#include "CommonInSim.h"
#include "../../InputAirside/ALTObject.h"


class AircraftClassificationManager;
class AirsideFlightInSim;
class CTakeoffClearanceCriteria;
class ElapsedTime;
class LogicRunwayInSim;
class CRunwaySeparationRelation;

class CTakeoffSeparationInSim
{
public:
	CTakeoffSeparationInSim(void);
	~CTakeoffSeparationInSim(void);

	void Init(AircraftClassificationManager* pAircraftClassificationManager, int nProjID,const ALTObjectList& vRunways);
	//takeoff clearance separation
	//bIsTimeOrDis=TRUE, estSepTime is valid, bIsTimeOrDis=FALSE, dSepDistance is valid
	void GetTakeoffClearToPositionSeparationTime(AirsideFlightInSim* pTakeoffFlight, AirsideFlightInSim* pFrontFlight,const AirsideMobileElementMode& frontFlightMode, ElapsedTime& estSepTime);
	void GetLandingSeparationTimeOnSameRunway(AirsideFlightInSim* pTakeoffFlight, AirsideFlightInSim* pLandingFlight,const AirsideMobileElementMode& frontFlightMode, ElapsedTime& estSepTime);
	
	//get takeoff position separation time takeoff behind takeoff
	void GetTakeoffClearToPositionSeparationTimeBehindTakeoff(AirsideFlightInSim* pTakeoffFlight, AirsideFlightInSim* pFrontFlight,ElapsedTime& estSepTime);
	void GetTakeoffClearToPositionSeparationTimeBehindLanding(AirsideFlightInSim* pTakeoffFlight, AirsideFlightInSim* pFrontFlight,ElapsedTime& estSepTime);

	//throw AirsideACTypeMatchError exception
	void GetTakeoffSeparationTime(AirsideFlightInSim* pFlight,AirsideFlightInSim* pFrontFlight,const AirsideMobileElementMode& frontFlightMode, double& minTime);
	void GetRelatedRunwayNotConcern(LogicRunwayInSim *pCurPort,
		std::vector<int > vConcernRunway,
		std::vector<std::pair<int,RUNWAY_MARK> >& vNotConcernLandRunway,
		std::vector<std::pair<int,RUNWAY_MARK> >& vNotConcernTakeOffRunway);

	void GetRelatedRunwayListByAcOpType(const std::vector<LogicRunwayInSim*>& vRunways, AIRCRAFTOPERATIONTYPE AcOpType, std::vector<std::pair<LogicRunwayInSim*,LogicRunwayInSim*> >& vRelatedRunways);
	CRunwaySeparationRelation* GetTakeSepRwyRelation();
private:
	AircraftClassificationManager*        m_pAircraftClassificationManager;
	CTakeoffClearanceCriteria*            m_pTakeoffClearanceCriteria;
};
