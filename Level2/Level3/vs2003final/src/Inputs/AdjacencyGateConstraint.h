#pragma once
#include <CommonData/procid.h>
#include "Common/dataset.h"
#include <vector>
#include "..\Common/ALTObjectID.h"
#include "Common/elaptime.h"
#include "ref_ptr_bee.h"

enum GateAssignConType
{
	GateConType_Height,
	GateConType_Len,
	GateConType_Span,
	GateConType_MZFW,
	GateConType_OEW,
	GateConType_MTOW,
	GateConType_MLW,
	GateConType_Capacity
};
class ARCFlight;
class CAssignGate;
class GateAssignmentConstraint
{
public:
	GateAssignmentConstraint();
	~GateAssignmentConstraint();

	const ALTObjectID& GetGate() const { return m_Gate; }
	ALTObjectID& GetGate() { return m_Gate; }
	GateAssignConType GetConType() const { return m_conType; }
	float GetMinValue() const { return m_fMinValue; }
	float GetMaxValue() const { return m_fMaxValue; }

	void SetGate(const ALTObjectID& gate) { m_Gate = gate; }
	void SetConType(GateAssignConType conType) { m_conType = conType; }
	void SetMinValue(float fMinValue) { m_fMinValue = fMinValue; }
	void SetMaxValue(float fMaxValue) { m_fMaxValue = fMaxValue; }

	bool IsFlightFit(const ARCFlight& flight, InputTerminal* pInTerm);


protected:
	ALTObjectID m_Gate;
	GateAssignConType m_conType;
	float m_fMinValue;
	float m_fMaxValue;

};

class AdjacencyGateConstraint
{
public:
	AdjacencyGateConstraint( GateAssignmentConstraint* pFirstConstraint, GateAssignmentConstraint* pSecondConstraint);
	AdjacencyGateConstraint(const AdjacencyGateConstraint& adjCon);

	~AdjacencyGateConstraint();

	void SetFirstGateIdx(int nIdx){ m_nFirstGateIdxInAssignmentMgr = nIdx; }		// set index of gate in gate list of AssignmentMgr
	int GetFirstGateIdx(){return m_nFirstGateIdxInAssignmentMgr;}		//get index of gate in gate list of AssignmentMgr

	void SetSecondGateIdx(int nIdx){ m_nSecondGateIdxInAssignmentMgr = nIdx; }		// set index of gate in gate list of AssignmentMgr
	int GetSecondGateIdx(){return m_nSecondGateIdxInAssignmentMgr;}		//get index of gate in gate list of AssignmentMgr

	GateAssignmentConstraint* GetFirstConstraint(){ return m_pFirstConstraint; }
	GateAssignmentConstraint* GetSecondConstraint(){ return m_pSecondConstraint; }

	AdjacencyGateConstraint& operator= (const AdjacencyGateConstraint& _constraint);
	bool IsFlightFitContraint(const ARCFlight* pFlight, std::vector<ARCFlight*>& vFlights, int nGateIdx, InputTerminal* pInTerm);
	bool IsFlightFitConstraint(const ARCFlight* pFlight, std::vector<ARCFlight*>& vFlights, int nGateIdx, InputTerminal* pInTerm,CString& strError);
private:
	int m_nFirstGateIdxInAssignmentMgr;		//to record the gate index of gate list of AssignmentMgr
	int m_nSecondGateIdxInAssignmentMgr;		//to record the gate index of gate list of AssignmentMgr

	GateAssignmentConstraint* m_pFirstConstraint; 
	GateAssignmentConstraint* m_pSecondConstraint;
};

class AdjacencyGateConstraintList : public DataSet
{
public:
	AdjacencyGateConstraintList();
	~AdjacencyGateConstraintList();

	virtual void clear();
	virtual void readData(ArctermFile& p_file);
	virtual void writeData(ArctermFile& p_file) const;

	virtual const char *getTitle() const { return "Adjacency Gate Constraint"; }
	virtual const char *getHeaders() const { return "If Stand, ConType, Min Value, Max Value, Then Stand, ConType, Min Value, Max Value"; }

	int GetCount();
	void AddItem(const AdjacencyGateConstraint& gateAssignCon);
	void DeleteItem(int nIndex);
	AdjacencyGateConstraint& GetItem(int nIndex);

	void InitConstraintGateIdx(std::vector<ref_ptr_bee<CAssignGate> >& vGateList);

	bool IsFlightAndGateFit(const ARCFlight* flight, int nGateIdx, const ElapsedTime& tStart, const ElapsedTime& tEnd, std::vector<ref_ptr_bee<CAssignGate> >& vGateList);
	bool FlightAndGateFitErrorMessage(const ARCFlight* flight, int nGateIdx, const ElapsedTime& tStart, const ElapsedTime& tEnd, std::vector<ref_ptr_bee<CAssignGate> >& vGateList,CString& strError);

protected:
	std::vector<AdjacencyGateConstraint> m_adjacencyConstraints;
};