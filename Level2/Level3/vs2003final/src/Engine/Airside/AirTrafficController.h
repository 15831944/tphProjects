#pragma once

class AirsideFlightInSim;
enum FLIGHT_PARKINGOP_TYPE;

class Clearance;
class AirsideResourceManager;
class AirsideResource;
//class CInboundRouteInSimList;
//class COutboundRouteInSimList;
class CDynamicStandAssignmentInSim;
class AircraftClassificationManager;
class PushBackClearanceInSim;
class TemporaryParkingInSim;
class CTakeoffSeparationInSim;
class CInTrailSeparationInSim;
class CApproachSeparationInSim;
class TakeoffSequencingInSim;
class In_OutBoundRouteAssignmentInSim;
class FlightGroundRouteResourceManager;
class RunwayExitInSim;
class TaxiRouteInSim;

#include "./CommonInSim.h"
#include "../../Common/elaptime.h"
#include "FlightTypeRunwayAssignmentStrategiesInSim.h"
#include "RunwayExitAssignmentStrategiesInSim.h"
#include "SID_STARAssignmentInSim.h"
#include "RunwayTakeoffPositionAssignInSim.h"
#include "FinalApproachController.h"
#include "ArrivalDelayTriggerInSim.h"
#include "FlightRouteSegInSim.h"
//#include "MissApproachFindClearanceInConcern.h"
#include "InputAirside/TowoffStandStrategyContainer.h"

namespace ns_FlightPlan
{
	class FlightPlans;
	class FlightPlanSpecific;
}
class CAirportDatabase;
class CDynamicStandAssignmentInSim;
class COccupiedAssignedStandActionInSim;
class FlightServiceReqirementInSim;
class CTaxiwayConflictResolutionInEngine;
class StandBufferInSim;
class GateAssignmentConstraintList;
class CRunwaySystem;
class InboundStatus;
class OutboundStatus;
class TakeoffQueueStatus;
class RunwayOccupancyStatus;
class AirportStatusManager;
class TakeoffSequencingInSim;
class CStand2GateConstraint;
// class CBoardingCallPolicy ;
// class CPassengerTakeOffCallPolicy ;
class FlightPriorityInfo;
class Push_TowOperationSpecInSim;

class WeatherInSim;
class CAircraftInitSurfaceSettingInSim;
class CDeiceAndAnticeManager;
class CAircraftDeicingAndAnti_icingsSettings;
class CDeiceAndAnticeInitionalSurfaceCondition;
class FlightDeiceRequirment;
class OutputAirside;
class CTaxiInterruptTimes;
class CTowOffStandAssignmentDataList;
class FltOperatingDoorSpecInSim;
class StandInSim;
class DynamicConflictGraph;
class AirsideCircuitFlightInSim;

#include <InputAirside/DeiceAndAnticeManager.h>
// the role of ATC in the air side system , assign clearance to flight
class AirTrafficController
{
public:
	AirTrafficController();
	~AirTrafficController(void);

	bool Init(int nPrjId,
		AirsideResourceManager * pResources,
		AircraftClassificationManager* pAircraftClassification,
		CAirportDatabase * pAirportDB,
		CStand2GateConstraint* pStand2gateConstraint,
		OutputAirside *pOutput);
	void GetNextClearance(AirsideFlightInSim * pFlight, Clearance& clearance);
	void GetNextCircuitClearance(AirsideCircuitFlightInSim* pFlight,ClearanceItem& lastItem, Clearance& newClearance );

	
	//Assignment's;
	void AssignLandingRunway(AirsideFlightInSim * pFlight);
	void AssignTakeoffRunway(AirsideFlightInSim * pFlight);
	void AssignARRFlightPlan(AirsideFlightInSim * pFlight);
	void AssignDEPFlightPlan(AirsideFlightInSim * pFlight);
	void AssignInBoundRoute(AirsideFlightInSim * pFlight);
	void GetCircuitRoute(AirsideCircuitFlightInSim* pFlight);
	void AssignOutBoundRoute(AirsideFlightInSim * pFlight);
	void AssignCircuitOutBoundRoute(AirsideCircuitFlightInSim * pFlight);
	void AssignCircuitInBoundRoute(AirsideCircuitFlightInSim * pFlight);
	void AssignOperationParkingStand(AirsideFlightInSim * pFlight,FLIGHT_PARKINGOP_TYPE type);
	void AssignRunwayExit(AirsideFlightInSim * pFlight);
	void AssignTakeoffPosition(AirsideFlightInSim * pFlight);
	void AssignSTAR(AirsideFlightInSim * pFlight);
	//circuit flight taking off and landing circuit route
	void AssignLandingCircuitRoute(AirsideFlightInSim * pFlight);
	void AssignTakeoffCircuitRoute(AirsideFlightInSim* pFlight);
	void AssignSID(AirsideFlightInSim * pFlight);
	void AssignRouteToTemporaryParking(AirsideFlightInSim* pFlight);
	void AssignRouteToStand(AirsideFlightInSim* pFlight);
	void AssignTowOffRoute(AirsideFlightInSim* pFlight,AirsideResource* pOrigin, AirsideResource* pDest);
	void AssignIntermediateParking(AirsideFlightInSim* pFlight);
	void AssignRouteToDeice(AirsideFlightInSim* pFlight);
	void AssignOutboundRoute(AirsideResource* pFrom, AirsideFlightInSim* pFlight);


	void ChangeFlightTakeoffPos(AirsideFlightInSim* pFlt,const ElapsedTime& t);

	//will check can hold flight
	void AssignAvaibleRunwayExit(AirsideFlightInSim* pFlight);

	//AircraftSeparationManager * GetSeparationManager(){ return m_pAircraftSeparationManager; }
	ElapsedTime GetSeparationTime(AirsideFlightInSim * pFlightLead, AirsideMobileElementMode leadmode, AirsideFlightInSim * pFlightTrail, AirsideMobileElementMode trailmode);
	DistanceUnit GetSeparationDistance(AirsideFlightInSim * pFlightLead, AirsideMobileElementMode leadmode, AirsideFlightInSim * pFlightTrail, AirsideMobileElementMode trailmode);
	FlightServiceReqirementInSim* GetFlightServiceRequirement(){ return m_pFlightServiceRequirement;}
	StandBufferInSim* GetStandBuffer(){ return m_pStandBuffer; }
	CInTrailSeparationInSim* GetIntrailSeparation(){ return m_pInTrailSeparationInSim; }
	FlightPlanInSim* GetFlightPlan(){ return m_pFlightPlan; }
	CFlightTypeRunwayAssignmentStrategiesInSim* GetRunwayAssignmentStrategy(){ return &m_RunwayAssignmentStrategies; }
	In_OutBoundRouteAssignmentInSim* GetBoundRouteAssignment(){ return m_pBoundRouteAssignment; }

	void SetFlightList(	std::vector<AirsideFlightInSim*>& vFlights);
	bool IsDelayPushback(AirsideFlightInSim* pFlight, ClearanceItem FltCurItem);

	AirsideResourceManager *GetAirsideResourceManager(){ return m_pResources;}
	CTaxiwayConflictResolutionInEngine *GetTaxiwayConflictResolution() {return m_pTaxiwayConflictResolution;}
	CRunwaySystem* GetRunwaysController(){ return m_pRunwaysController;}

	void SetGateAssignmentConstraints(GateAssignmentConstraintList* pConstraints);
	void SetStandAssignmentPriority(FlightPriorityInfo* pInfo);

	int GetTakeoffQueueLength(LogicRunwayInSim* pRunway);

	ArrivalDelayTriggerInSim* GetArrivalDelayTrigger(){ return m_pArrivalDelayTrigger;}

	AirportStatusManager* GetAirportStatusManager(){ return m_pAirportStatusManager; }
	InboundStatus* GetInboundStatus(){ return m_pInboundStatus; }
	TakeoffQueueStatus* GetTakeoffQueueStatus(){ return m_pTakeoffQueueStatus; }
	RunwayOccupancyStatus* GetRunwayOccupancyStatus(){return m_pRunwayOccupancyStatus; }
	
	TakeoffSequencingInSim* GetTakeoffSequenceInSim() {return m_pTakeoffSequencingInSim;}
//	CBoardingCallPolicy* GetBoardingCall() { return m_pBoardingCallPolicy ;} ;
//	CPassengerTakeOffCallPolicy* GetPassengerTakeOffFlightPolicy() { return m_pPaxTakeOffFlightPolicy ;} ;


	void UpdateFlightDeiceRequirment(AirsideFlightInSim* pFlight , FlightDeiceRequirment& deiceRequir);

	Push_TowOperationSpecInSim* GetPushAndTowOperationSpec();

	CRunwayExitAssignmentStrategiesInSim* GetRunwayExitAssignmentStrategiesInSim() { return &m_RunwayExitAssignmentStrategies;}

	CTaxiInterruptTimes * GetIntruptTimeSetting(){ return m_pInterruptTimes; }

	FltOperatingDoorSpecInSim* GetFltOperatingDoorSpec();
	void SetFltOperatingDoorSpec(const FltOperatingDoorSpecInSim* pSpec);

	int GetTaxiToRunwayFlightCount(LogicRunwayInSim* pRwyPort);

	void SetResourceManager(AirsideResourceManager * pRes);

	//check temp parking to parking stand adjancy
	StandInSim* IsCurrentOccupiedFlightAdjacencyConflict(AirsideFlightInSim* pFlight);
	bool IsGateAssignConstraintsConflict(AirsideFlightInSim* pFlight);

	//get outbound route of segments
	bool getOutBoundSeg(AirsideResource* pFrom,RunwayExitInSim* pTakeoffPos, AirsideFlightInSim* pFlight,FlightGroundRouteDirectSegList& voutSegs );
	bool getOutBoundSeg( AirsideResource* pFrom, FlightGroundRouteDirectSegInSim* pCurSeg,AirsideFlightInSim* pFlight,FlightGroundRouteDirectSegList& vSegmets );

	CRunwayTakeoffPositionAssignInSim& getTakeoffPositionAssignment(){ return m_TakeOffPositionAssignment; }

	CStand2GateConstraint* GetStandToGateConstraint(){return m_pStand2gateConstraint;}
	DynamicConflictGraph* getConflictGraph()const{ return m_pConfilctGraph; }
	void RemoveFlightPlanedStandOccupancyTime(AirsideFlightInSim* pFlight,FLIGHT_PARKINGOP_TYPE eType);
protected:
	//int GetAirRouteIntersectWaypointIdx(AirsideFlightInSim * pFlight);
	StandInSim* ResolveStandConflict(AirsideFlightInSim* pFlight,FLIGHT_PARKINGOP_TYPE type/*,  ElapsedTime tStandBuffer*/);
	//1. flight at arrival stand and call this method to make flight move from arrival stand to intermediate stand
	//2. flight at intermediate stand and call this method to make flight move from intermediate stand to departure stand
	//Note: if flight at arrival stand call this method, the flight should call it again
	bool GetNextClearanceOfIntermediateParkingPart(AirsideFlightInSim * pFlight, Clearance& newclearance,ClearanceItem& lastClearanceItem);	
	bool ArrivalFlightIntermediateStandProcess(AirsideFlightInSim * pFlight, Clearance& newclearance,ClearanceItem& lastClearanceItem);
	bool DepartureFlightIntermediateStandProcess(AirsideFlightInSim * pFlight, Clearance& newclearance,ClearanceItem& lastClearanceItem);


	TaxiRouteInSim* getFlightOutBoundRoute(AirsideResource* pFromRes,RunwayExitInSim* pTakeoffPos, AirsideFlightInSim* pFlt);

	
protected:
	//resources
	AirsideResourceManager * m_pResources;
	//separations
	AircraftClassificationManager * m_pAircraftClassifications;
	//AircraftSeparationManager *     m_pAircraftSeparationManager;

	CInTrailSeparationInSim*        m_pInTrailSeparationInSim;

	//
	FlightPlanInSim*  m_pFlightPlan;
	CAirportDatabase * m_pAirportDB;
	//CInboundRouteInSimList   *m_pInboundRouteInSimList;
	//COutboundRouteInSimList  *m_pOutboundrouteInSimList;
	In_OutBoundRouteAssignmentInSim* m_pBoundRouteAssignment;
	CFlightTypeRunwayAssignmentStrategiesInSim m_RunwayAssignmentStrategies; 
	CRunwayExitAssignmentStrategiesInSim m_RunwayExitAssignmentStrategies;
	CSID_STARAssignmentInSim m_SID_STARAssignment;
	CRunwayTakeoffPositionAssignInSim m_TakeOffPositionAssignment;

	CTaxiInterruptTimes * m_pInterruptTimes;			//time setting wait at interrupt line
	
	CDynamicStandAssignmentInSim* m_pDynamicStandReassignment;
	COccupiedAssignedStandActionInSim* m_pOccupiedAssignedStandActionInSim;
	FlightServiceReqirementInSim* m_pFlightServiceRequirement;
	PushBackClearanceInSim* m_pPushBackClearance;
	TemporaryParkingInSim* m_pTemporaryParking;
	StandBufferInSim* m_pStandBuffer; 

	//CFinalApproachController m_finalAppraochController;
	ArrivalDelayTriggerInSim* m_pArrivalDelayTrigger;
	std::vector<AirsideFlightInSim* > m_vFlights;
	CTaxiwayConflictResolutionInEngine *m_pTaxiwayConflictResolution;
	
	//CMissApproachFindClearanceInConcern m_missApproachController;

	CRunwaySystem* m_pRunwaysController;

	AirportStatusManager* m_pAirportStatusManager;
	InboundStatus* m_pInboundStatus;
	OutboundStatus* m_pOutboundStatus;
	TakeoffQueueStatus* m_pTakeoffQueueStatus;
	TakeoffSequencingInSim* m_pTakeoffSequencingInSim;
	RunwayOccupancyStatus* m_pRunwayOccupancyStatus;

	CStand2GateConstraint* m_pStand2gateConstraint;

	//the policy the boardingCall
//	CBoardingCallPolicy* m_pBoardingCallPolicy ;
//	CPassengerTakeOffCallPolicy* m_pPaxTakeOffFlightPolicy ;

	//Push&towOperationSpecs
	Push_TowOperationSpecInSim* m_pPushAndTowOperationSpecInSim;


	TowoffStandStrategyContainer m_towoffStandConstraint;
	CTowOffStandAssignmentDataList* m_pTowOffCriteria;


	//deice management
	WeatherInSim* m_pWeatherScript;
	CAircraftInitSurfaceSettingInSim* m_aircftSurSet;
	CDeiceAndAnticeManager * m_pDeiceManager;
	CAircraftDeicingAndAnti_icingsSettings * m_pDeiceDemand;
	FltOperatingDoorSpecInSim* m_pFltOperatingDoorSpec;
	DynamicConflictGraph* m_pConfilctGraph;

	//try to fit the Priority and set the flight deice place
	bool FlightFitPrority(AirsideFlightInSim* pFlight, CDeiceAndAnticeInitionalSurfaceCondition* pStragy, PriorityType priT);
	bool _FitDeicePriority(AirsideFlightInSim* pFlight, DeicePadPriority* pDeicepad);
	bool _FitDeicePriority(AirsideFlightInSim* pFlight, DepStandPriority* pStand);
	bool _FitDeicePriority(AirsideFlightInSim* pFlight, LeadOutPriority* pLeadout);
	bool _FitDeicePriority(AirsideFlightInSim* pFlight, RemoteStandPriority* pRemoteStand);
};
