#pragma once
#include "../../InputAirside/ApproachSeparationCirteria.h"
#include "../../InputAirside/ALT_BIN.h"
#include "CommonInSim.h"
class AirsideFlightInSim;
class CApproachTypeList;
class LogicRunwayInSim;
enum SEPSTD;
class AircraftClassificationManager;
class CApproachSeparationCriteria;
class CRunwaySeparationRelation;


class CApproachSeparationInSim
{
public:
	CApproachSeparationInSim(void);
	~CApproachSeparationInSim(void);

	void Init(AircraftClassificationManager* pAircraftClassificationManager, int nProjID,const ALTObjectList& vRunways);

	SEPSTD GetApproachSeparationStandard(AirsideFlightInSim* pTrailFlight, AirsideFlightInSim* pLeadFlight);
	//return TRUE:use the LAHSO, return FALSE: don't use the LASHO
	BOOL   IsTakeTheLAHSO(AirsideFlightInSim* pTrailFlight, AirsideFlightInSim* pLeadFlight);
	//return TRUE:use the LAHSO, return FALSE: don't use the LASHO
	//if aircraftOperationType is AIRCRAFTOPERATION_TYPE_LANDING_BEHIND_LANDED, pLandOrTakeoffRunway is land runway
	//if aircraftOperationType is AIRCRAFTOPERATION_TYPE_LANDING_BEHIND_TAKEOFF, pLandOrTakeoffRunway is takeoff runway
	BOOL   IsTakeTheLAHSO(LogicRunwayInSim* pLandRunway, LogicRunwayInSim* pLandOrTakeoffRunway, AIRCRAFTOPERATIONTYPE aircraftOperationType);

	//Called the GetAircraftClass() function but not catch the exception,throw AirsideDiagnose exception
	void GetApproachSeparationTime(AirsideFlightInSim* pFlight,AirsideFlightInSim* pFrontFlight,const AirsideMobileElementMode& frontFlightMode,double& dMinTime);
	
	
	void GetRelatedRunwayNotConcern(LogicRunwayInSim *pCurPort,
										std::vector<int > vConcernRunway,
										std::vector<std::pair<int,RUNWAY_MARK> >& vNotConcernLandRunway,
										std::vector<std::pair<int,RUNWAY_MARK> >& vNotConcernTakeOffRunway);

	double GetTimeAccordSeparationDist(AirsideFlightInSim* pFlight, AirsideFlightInSim* pFrontFlight,double dMinDist);

	void GetRelatedRunwayListByACOpType(const std::vector<LogicRunwayInSim*>& vRunways,AIRCRAFTOPERATIONTYPE AcOpType, std::vector<std::pair<LogicRunwayInSim*,LogicRunwayInSim*> >& vRelatedRunways );
	CRunwaySeparationRelation* GetApproachSepRwyRelation();
protected:
	CApproachSeparationCriteria*       m_pApproachSeparation;
	AircraftClassificationManager*     m_pAircraftClassificationManager;
};