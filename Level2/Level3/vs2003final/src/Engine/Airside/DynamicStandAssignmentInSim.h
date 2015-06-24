#pragma once
#include "AirsideFlightInSim.h"

class FlightPriorityInfo;

class StandResourceManager;
class CStandAssignmentRegisterInSim;
class ElapsedTime;
class StandInSim;
class GateAssignmentConstraintList;

class CDynamicStandAssignmentInSim
{
public:
	CDynamicStandAssignmentInSim(void);
	~CDynamicStandAssignmentInSim(void);

	StandInSim* StandReassignment( AirsideFlightInSim* pFlight, FLIGHT_PARKINGOP_TYPE _type);
	bool StandAssignConflict(AirsideFlightInSim* pFlight,StandInSim* pTestStand);

	void Init(StandResourceManager* pStandResManager,CTowOffStandAssignmentDataList* pTowCriteria );
	void SetGateAssignmentConstraints(GateAssignmentConstraintList* pConstraints);
	void SetStandAssignmentPriority(FlightPriorityInfo *flightPriorityInfo);

	//modify these method from private to public for term parking
	bool IsGateAssignConstraintsConflict(StandInSim* pStand, AirsideFlightInSim* pFlight);
	StandInSim* IsCurrentOccupiedFlightAdjacencyConflict(StandInSim* pStand,AirsideFlightInSim* pFlight);

private:
	StandInSim* AssignFlightStandWithStandList(AirsideFlightInSim* pFlight,FLIGHT_PARKINGOP_TYPE _type,const std::vector<StandInSim*>& vStandList,
												 const ElapsedTime& tStart, const ElapsedTime& tEnd);


	StandInSim* AssignFlightDepartureStand(AirsideFlightInSim* pFlight);

	bool IsAdjacencyConstraintsConflict(StandInSim* pStand, AirsideFlightInSim* pFlight, ElapsedTime tStart, ElapsedTime tEnd);
	bool GetPriorityGate(AirsideFlightInSim* pFlight, std::vector<StandInSim*>& vStandGroup, bool bCheckState);

	bool GetFlightStartAndEndTime(AirsideFlightInSim* pFlight,StandInSim* pTestStand,ElapsedTime& startTime,ElapsedTime& endTime);


	//stand name based
	StandInSim* CDynamicStandAssignmentInSim::StandNameBasedFlightAdjacencyConflict(StandInSim* pStand,AirsideFlightInSim* pFlight);

	//stand dimension based
	StandInSim* CDynamicStandAssignmentInSim::StandDimensionBasedFlightAdjacencyConflict(StandInSim* pStand,AirsideFlightInSim* pFlight);
private:
	StandResourceManager* m_pStandResManager;
	GateAssignmentConstraintList* m_pGateAssignConstraints;
	FlightPriorityInfo *m_pflightPriorityInfo;
	CTowOffStandAssignmentDataList* m_pTowCriteria;
};
