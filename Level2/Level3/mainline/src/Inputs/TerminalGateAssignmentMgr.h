#pragma once
#include "gateassignmentmgr.h"

class TerminalGateAssignmentMgr :
	public CGateAssignmentMgr
{
public:
	TerminalGateAssignmentMgr(int nProjID);
	~TerminalGateAssignmentMgr(void);

	virtual void LoadData(InputTerminal* _pInTerm, const PROJECTINFO& ProjInfo);
	virtual void ResetGateContent(OpType eType,InputTerminal* _pInTerm);
	virtual void RemoveSelectedFlight();
	virtual void Save( InputTerminal* _pInTerm, const CString& _csProjPath );	
	virtual	void RemoveAssignedFlight(FlightForAssignment* pFlight);	
	virtual bool IsFlightOperationFitInGate( int _nGateIdx, CFlightOperationForGateAssign* pFlight);
	virtual int AssignSelectedFlightToGate(int _nGateIdx, std::vector<int>& vGateIdx);
	bool ProcessAssignFailedError(int _nGateIdx,OpType emType,CString& strError);
	bool CheckFlightOperationFitInGate(int _nGateIdx, CFlightOperationForGateAssign* pFlight,CString& strError );

	bool SetFlightDurationTime(int _nGateIdx,CFlightOperationForGateAssign* pFlightOp);
	CStand2GateConstraint m_std2gateConstraint;
};
