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

	CStand2GateConstraint m_std2gateConstraint;
};
