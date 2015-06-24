#pragma once
#include "inputsdll.h"
#include "Common/elaptime.h"

enum OpType
{
	ARR_OP = 0,		//only arrival
	DEP_OP,			//only departure
	ARR_DEP_OP,			//arrival and departure
	INTPAKRING_OP,	//intermediate parking
	NO_OP

};

class ARCFlight;
class FlightForAssignment;
class ALTObjectID;
class ProcessorID;

class INPUTS_TRANSFER CFlightOperationForGateAssign
{
public:
	enum StandAssignmentErrorType
	{
		StandIntermdiateTimeError,
		StandBufferError,
		StandAdjacenceError,
		StandOverlapError,
		StandConstraintError,
	};

	struct FlightAssignToStandErrorMessage 
	{
		StandAssignmentErrorType m_emType;
		CString m_strError;
	};

	typedef std::vector<FlightAssignToStandErrorMessage> FlightAssignErrorMessageList;

	bool IsSelected(void);
	void SetSelected( bool _bSelected );
	CFlightOperationForGateAssign( FlightForAssignment* _flight, OpType eType);
	~CFlightOperationForGateAssign();

	bool operator < (const CFlightOperationForGateAssign& aFlight) const;
	bool operator > (const CFlightOperationForGateAssign& aFlight) const;
	bool operator == (const CFlightOperationForGateAssign& aFlight) const;
	bool operator != (const CFlightOperationForGateAssign& aFlight) const;

	ARCFlight* getFlight();
	OpType getOpType();
	ElapsedTime GetStartTime();
	ElapsedTime GetEndTime();

	void SetTimeRange(ElapsedTime tStart, ElapsedTime tEnd);

	void EmptyStand();
	void EmptyGate();

	void SetStand(const ALTObjectID& standID);
	void SetGate(const ProcessorID& gateID);

	FlightForAssignment* GetAssignmentFlight();

	void SetGateIdx(int nIdx);
	int GetGateIdx();

	void AddErrorMessage(StandAssignmentErrorType emType,const CString& strError);
	void ClearErrorMessage();
	bool FlightAssignToStandValid(CString& strError);
	bool FlightAssignToStandValid()const;
private:
	bool m_bSelected;
	FlightForAssignment* m_pFlight;
	OpType m_eOpType;
	ElapsedTime m_tStartTime;
	ElapsedTime m_tEndTime;
	int m_nGateIdx;
	FlightAssignErrorMessageList m_vErrorMessageList;
};

class INPUTS_TRANSFER FlightForAssignment
{
public:
	FlightForAssignment(ARCFlight* _flight);
	~FlightForAssignment();

	bool IsSelected(void);
	void SetSelected( bool _bSelected );

	void SetFlightArrPart(CFlightOperationForGateAssign* pflight);
	void SetFlightDepPart(CFlightOperationForGateAssign* pflight);
	void SetFlightIntPart(CFlightOperationForGateAssign* pflight);
	void ClearGateIdx();

	void EmptyStand();
	void EmptyGate();

	ARCFlight* getFlight();
	CFlightOperationForGateAssign* GetFlightArrPart();
	CFlightOperationForGateAssign* GetFlightDepPart();
	CFlightOperationForGateAssign* GetFlightIntPart();


	void ClearAllOperations();

	ElapsedTime getStartTime();
	ElapsedTime getEndTime();

	bool getArrStartTime(ElapsedTime& startTime);
	bool getIntStartTime(ElapsedTime& startTime,CString& strError);
	bool getDepStartTime(ElapsedTime& startTime);

	bool getArrEndTime(ElapsedTime& endTime);
	bool getIntEndTime(ElapsedTime& endTime,CString& strError);
	bool getDepEndTime(ElapsedTime& endTime);

	void SetFailAssignReason(const CString& strReason);
	const CString& GetFailAssignReason();

	
private:
	bool m_bSelected;
	ARCFlight* m_pFlight;
	CFlightOperationForGateAssign* m_pFlightArr;
	CFlightOperationForGateAssign* m_pFlightDep;
	CFlightOperationForGateAssign* m_pFlightInt;
	CString m_strFailAssignReason;
};

