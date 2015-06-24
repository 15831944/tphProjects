#pragma once
#include "gateassignmentmgr.h"
#include "Common/FLT_CNST.H"
#include "Common/elaptime.h"
#include "InputAirside/TowoffStandStrategyContainer.h"

using namespace std;

class FlightForAssignment;
class CTowOffStandAssignmentData;

class FlightPartAccordingToTowData
{
public:
	struct TimePart 
	{
		ElapsedTime tStart;
		ElapsedTime tEnd;
	};

	void GetFlightTimePartAccordingToTowData(FlightForAssignment* pFlight, CTowOffStandAssignmentData* pFitData);
	const TimePart& GetArrPart(){ return m_ArrPart; }
	const TimePart& GetDepPart(){ return m_DepPart; }
	const TimePart& GetIntPart(){ return m_IntPart; }

private:
	TimePart m_ArrPart;
	TimePart m_IntPart;
	TimePart m_DepPart;

};
class ItinerantFlightSchedule;
class FlightPriorityInfo;
typedef struct _FlightGate FlightGate;

class StandAssignmentMgr :
	public CGateAssignmentMgr
{


public:
	StandAssignmentMgr(int nProjID);
	~StandAssignmentMgr(void);

public:
	virtual void LoadData(InputTerminal* _pInTerm, const PROJECTINFO& ProjInfo);
	virtual void ResetGateContent(OpType eType,InputTerminal* _pInTerm);
	virtual int AssignSelectedFlightToGate(int _nGateIdx, std::vector<int>& vGateIdx);
	bool ProcessAssignFailedError(int _nGateIdx, FlightForAssignment* pFlightAssign,CString& strError);
	virtual bool IsFlightOperationFitInGate( int _nGateIdx, CFlightOperationForGateAssign* pFlight);
	virtual void RemoveAssignedFlight(FlightForAssignment* pFlight);
	virtual void RemoveSelectedFlight();
	virtual void Save( InputTerminal* _pInTerm, const CString& _csProjPath );


	//void ResetFlightGateContent(FlightForAssignment* pFlightInStand,int nGateIdx);

	bool IsFlightPartFitInGate(int _nGateIdx,ARCFlight* pFlight,const ElapsedTime& tStart,const ElapsedTime& tEnd);

	GateAssignmentConstraintList& getConstraint();

	bool IsAWholeFlightFitInGate( int _nGateIdx, FlightForAssignment* pFlight );

	//virtual void  AssignFlightOperationToGate( int _nGateIdx, std::vector<CFlightOperationForGateAssign*>& vOperations );
	//void AssignFlightToGate(int _nGateIdx, std::vector<FlightForAssignment*>& vFlights);
	
	bool AssignFlightAccordingToGatePrioritySequence(int nFltIdx,const std::vector<int>& vGateIdxList);
	bool AssignFlightAccordingToFlightPrioritySequence(int nFltIdx, const FlightPriorityInfo* pFltPriorities);

	int AssignFlightDepPartAccordingToFlightPrioritySequence(FlightForAssignment*pFlight,const ElapsedTime& tStart, const ElapsedTime tEnd, const FlightPriorityInfo* pFltPriorities);
	bool IsAdjacencyConstraintsConflict(ARCFlight* flight,int nGateIdx,const ElapsedTime& tStart, const ElapsedTime& tEnd);
	bool AdjacencyConstraintsConflictErrorMessage(ARCFlight* flight,int nGateIdx,const ElapsedTime& tStart, const ElapsedTime& tEnd,CString& strError);

	bool tryToAssignIntermediateParkingStand(FlightForAssignment* pFlight,CTowOffStandAssignmentData* pTowData);
	int tryToAssignFlightPartToAppointedStandList(FlightForAssignment* pFlight,const ElapsedTime& tStart,const ElapsedTime& tEnd, std::vector<int> vGateIdxList);

	bool AssignFlightToAppointedStandListWithIntermediateStand(FlightForAssignment* pFlight, int nFltIdx, std::vector<int> vGateIdxList, CTowOffStandAssignmentData* pTowData);

	bool isNeedtoTowOff(ARCFlight* pFlight,int nGateIdx);
	void SortUnAssignedFlight();


	//------------------------------------------------------------------------------------------------------------
	//Summary:
	//		assign flight by single gate priority
	//Parameter:
	//		nGateIdx: single priority rule
	//		vGateIdxList: assign dep part with all gate priority
	//return:
	//		true: assign success
	//		false: failed
	//-----------------------------------------------------------------------------------------------------------
	bool AssignFlightArccordingToGatePriority(int nGateIdx,const std::vector<int>& vGateIdxList);

	//-------------------------------------------------------------------------------------------------------------
	//Summary:
	//		assign flight with single flight priority
	//Parameters:
	//		_priority: single flight priority
	//		pFltPriorities: assign dep part with all flight priority
	//-------------------------------------------------------------------------------------------------------------
	bool AssignFlightAccordingToFlightPriority(FlightGate& _priority,const FlightPriorityInfo* pFltPriorities);

	CTowOffStandAssignmentDataList* GetTowoffStandDataList();

private:
	bool InitFlightStandPartsAccordingToTowoffCriteria(FlightForAssignment* pFlight);
	bool IsAssignedFlight(FlightForAssignment* pFlight);
	bool IsValidStandAssignment(FlightForAssignment* pFlight);

	void GetFitStandIdxList(const ALTObjectID& objName, std::vector<int>& vFitStands );
	void AssignFlightToOneAppointedStand(FlightForAssignment* pFlight, int nFltIdx, int nStandIdx);
	void AssignFlightToTwoAppointedStands(FlightForAssignment* pFlight, int nFltIdx, int nArrStandIdx, int nDepStandIdx,const ElapsedTime& tArrParking);
	void SortAssignStandListByFlightCount(std::vector<CAssignStand*>& vStands);

	void CreateAssignFlightErrorMessage(FlightForAssignment* pFlightInStand);
	bool FlightOperationAssignErrorMessage(CFlightOperationForGateAssign* pFlightOperation);
	void AssignFlightToStand(FlightForAssignment* pFligtInStand);

	bool StandBufforValid();
	bool StandConstraintValid();
	bool StandOverlapValid();
	bool StandAdjacenceValid();

	void AddFlightToStand(FlightForAssignment* pFlightInStand);

	void GetFlightOperationTime(ARCFlight* pFlight);


private:
	GateAssignmentConstraintList m_constraints;
	ItinerantFlightSchedule* m_itinerantFlight;
	std::vector<FlightForAssignment*> m_vFlightList;
	FlightPartAccordingToTowData m_TimePartOfFlight;
//	CTowOffStandAssignmentDataList* m_pTowOffStandAssignmentList;
	std::map< CString, std::vector<CAssignStand*> > m_mapPriorityStandFamilyList;
	TowoffStandStrategyContainer m_towoffStandConstraint;
};
