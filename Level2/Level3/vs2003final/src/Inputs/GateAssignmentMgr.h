// GateAssignmentMgr.h: interface for the CGateAssignmentMgr class.

#pragma once
#include <CommonData/procid.h>
#include "inputsdll.h"
#include "engine\process.h"
#include "FLIGHT.H"
#include "GateAssignmentConstraints.h"
#include "ref_ptr_bee.h"
#include "Stand2GateConstraint.h"
#include "FlightForGateAssign.h"
#include "AssignedGate.h"

class Terminal;
class InputTerminal;
class CTowOffStandAssignmentDataList;
class ParkingStandBufferList;
class ALTObjectID;

struct FlightOperationPostion
{
	FlightOperationPostion()
		:mnLine(-1)
		,mnItem(-1)
	{

	}
	int mnLine;
	int mnItem;
};

class INPUTS_TRANSFER CGateAssignmentMgr  
{
public:
	std::vector<ref_ptr_bee<CAssignGate> > m_vectGate;
	std::vector<FlightForAssignment*> m_vectUnassignedFlight;

	int m_nProjID;
public:
	CGateAssignmentMgr(int nProjID);
	virtual ~CGateAssignmentMgr(void);

	virtual void Save( InputTerminal* _pInTerm, const CString& _csProjPath );

	virtual void LoadData(InputTerminal* _pInTerm, const PROJECTINFO& ProjInfo) =0;
	virtual void ResetGateContent(OpType eType,InputTerminal* _pInTerm) = 0;
	virtual void RemoveSelectedFlight() =0;
	virtual	void RemoveAssignedFlight(FlightForAssignment* pFlight) =0;
	virtual bool IsFlightOperationFitInGate( int _nGateIdx, CFlightOperationForGateAssign* pFlight) =0;
	virtual int AssignSelectedFlightToGate(int _nGateIdx, std::vector<int>& vGateIdx) =0;


	void SetAssignedFlightSelected( int _nGateIndex, bool _bOnly );	// set for all flight on the gate.
	void SetAssignedFlightSelected( int _nGateIndex, int _nFlightIndex, bool _bOnly );
	void SetUnAssignedFlightSelected(int _nFlightIndex, int _nState);
	void GetUnAssignedFlightSelected(std::vector<FlightForAssignment*>& vFlightAssignmentList);
	FlightForAssignment* getSelectedFlight(int _nGateIndex, int _nFlightIndex);

	void GetUnassignedFlight(std::vector<CFlightOperationForGateAssign*>& vFlightForGateAssignList);
	std::vector<FlightForAssignment*>& GetUnassignedScheduleFlightList();

	CAssignGate* GetGate( int _nIdx );
	int GetGateCount(void);
	FlightOperationPostion GetFlightOperationPostion(CFlightOperationForGateAssign& flight);

	void AdjustAssignedFlight(std::vector<CFlightOperationForGateAssign*>& vUnAssignedOperations);		
	// unassign all flight
	int UnassignAllFlight( void );
	//////////////////////////////////////////////////////////////////////////
	void sortAllGateByName(void);
	
	void ReverseSort(std::vector<CFlightOperationForGateAssign*>& vFlights,bool bReverseSort);

	//return NULL, if not find
	//else return gate pointer
	CAssignGate*  GetGate(const CString& strName);

protected:
	std::vector<FlightForAssignment*> m_vAssignedFlight;

};
