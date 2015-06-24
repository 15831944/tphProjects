#pragma once
#include "AdjacencyConstraintSpec.h"
#include "AdjacencyGateConstraint.h"

class StandAdjacencyConstraint
{
public:
	StandAdjacencyConstraint();
	~StandAdjacencyConstraint();
	
	bool GetUseFlag(){ return m_bUseOld;};
	void SetUseFlag( bool _bFlag ){ m_bUseOld = _bFlag;};
	void readData();
	void writeData();
	AdjacencyGateConstraintList& GetAdjConstraints() { return m_adjGateConstratins; }
	AdjacencyConstraintSpecList& GetAdjConstraintSpec() { return m_adjConstratinSpec; }

	//------------------------------------------------------------------------------------------------------------------------------
	//Summary:
	//			load adjacency data both name based and dimension based
	//-----------------------------------------------------------------------------------------------------------------------------
	void loadDataSet (const CString& _pProjPath,InputTerminal* _pInTerm);
	//-------------------------------------------------------------------------------------------------------------------------------
	//Summary:
	//			choose use whether stand name base or stand dimension based to fit
	//-------------------------------------------------------------------------------------------------------------------------------
	bool StandAdjacencyFit(const ARCFlight* flight, int nGateIdx, const ElapsedTime& tStart, const ElapsedTime& tEnd, std::vector<ref_ptr_bee<CAssignGate> >& vGateList);

	//-----------------------------------------------------------------------------------------------------------------------------
	//Summary:
	//		give stand adjacency error message
	//-----------------------------------------------------------------------------------------------------------------------------
	bool StandAdjacencyFitErrorMessage(const ARCFlight* flight, int nGateIdx, const ElapsedTime& tStart, const ElapsedTime& tEnd, std::vector<ref_ptr_bee<CAssignGate> >& vGateList,CString& strError);

	//-----------------------------------------------------------------------------------------------------------------------------
	//Summary:
	//		init name based and dimension based gate index
	//------------------------------------------------------------------------------------------------------------------------------
	void InitConstraintGateIdx(std::vector<ref_ptr_bee<CAssignGate> >& vGateList);
private:
	int	m_nID;
	bool m_bUseOld;
	AdjacencyConstraintSpecList m_adjConstratinSpec;
	AdjacencyGateConstraintList m_adjGateConstratins;	
};
