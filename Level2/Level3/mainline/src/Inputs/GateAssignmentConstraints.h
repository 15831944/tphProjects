#pragma once
#include "Common/dataset.h"
#include "AdjacencyConstraintSpec.h"
#include "AdjacencyGateConstraint.h"
#include "../InputAirside/ACTypeStandConstraint.h"
#include "StandAdjacencyConstraint.h"

class ARCFlight;
class ProcessorID;
class ParkingStandBufferList;
class CTermPlanDoc;

class GateAssignmentConstraintList : public DataSet
{
public:
	GateAssignmentConstraintList();
	virtual ~GateAssignmentConstraintList();

	virtual void clear();
	virtual void readData(ArctermFile& p_file);
	virtual void writeData(ArctermFile& p_file) const;
	void SetParkingStandBufferList(ParkingStandBufferList* pStandBufferList);

	virtual const char *getTitle() const { return "Gate Assignment Constraint"; }
	virtual const char *getHeaders() const { return "Gate, ConType, Min Value, Max Value"; }

	int GetCount();
	void AddItem(const GateAssignmentConstraint& gateAssignCon);
	void DeleteItem(int nIndex);
	GateAssignmentConstraint& GetItem(int nIndex);

	bool IsFlightAndGateFit(const ARCFlight* flight, const ALTObjectID& gate);
	bool IsFlightAndGateFit(const ARCFlight* pFlight, const ALTObjectID& gate, CString& strError);

	//AdjacencyGateConstraintList& GetAdjConstraints() { return m_adjGateConstratins; }
	//AdjacencyConstraintSpecList& GetAdjConstraintSpec() { return m_adjConstratinSpec; }
	ParkingStandBufferList* GetParkingStandBufferList(){ return m_pParkingStandBufferList; }
	ACTypeStandConstraintList* GetACTypeStandConstraints(){ return &m_ACTypeStandConstraints; }
	StandAdjacencyConstraint& GetStandAdjConstraint(){return m_adjaconstrain;}
	void LoadataStandConstraint(CTermPlanDoc* Doc);

protected:
	std::vector<GateAssignmentConstraint> m_constraints;
	StandAdjacencyConstraint m_adjaconstrain;
	//AdjacencyConstraintSpecList m_adjConstratinSpec;
	//AdjacencyGateConstraintList m_adjGateConstratins;
	ParkingStandBufferList* m_pParkingStandBufferList;
	ACTypeStandConstraintList m_ACTypeStandConstraints;
};
