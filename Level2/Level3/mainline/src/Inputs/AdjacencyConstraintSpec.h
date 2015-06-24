#pragma once
#include "Common/dataset.h"
#include "..\Common/ALTObjectID.h"
#include "ref_ptr_bee.h"

class ARCFlight;
class InputTerminal;
class CAssignGate;

class TwoStandFlightOperationCase
{
public:
	TwoStandFlightOperationCase(double dFlightPlan1,double dFlightPlan2);
	virtual ~TwoStandFlightOperationCase();

	virtual bool Available(double dWidth,double dWingTip)const; 
protected:
	double m_dFlightSpan1;
	double m_dFlightSpan2;
};

class ThreeStandFlightOperationCase : public TwoStandFlightOperationCase
{
public:
	ThreeStandFlightOperationCase(double dFlightPlan1,double dFlightPlan2,double dFlightPlan3);
	~ThreeStandFlightOperationCase();

	virtual bool Available(double dWidth,double dWingTip)const; 
private:
	double m_dFlightSpan3;
};

enum ConditionOfUseType 
{
	ConditionType_OneStandOnly,
	ConditionType_ConcurrentUse,
	ConditionType_MideOrSides	
};

class AdjacencyConstraintSpecDate
{
public:
	AdjacencyConstraintSpecDate();
	~AdjacencyConstraintSpecDate();	
public:
    const ALTObjectID& GetFirstGate() const { return m_FirstGate; }
	void SetFirstGate(const ALTObjectID& gate) { m_FirstGate = gate; }

	const ALTObjectID& GetSecondGate() const { return m_SecondGate; }
	void SetSecondGate(const ALTObjectID& gate) { m_SecondGate = gate; }

	const ALTObjectID& GetThirdGate() const { return m_ThirdGate; }
	void SetThirdGate(const ALTObjectID& gate) { m_ThirdGate = gate; }

	ConditionOfUseType GetConType() const { return m_ConditionType; }
	void SetConType(ConditionOfUseType conType) { m_ConditionType = conType; }

	void SetFirstGateIdx(int nIdx) {m_nFirstStandIdx = nIdx;}
	int GetFirstGateIdx()const {return m_nFirstStandIdx;}

	void SetSecondGateIdx(int nIdx) {m_nSecondStandIdx = nIdx;}
	int GetSecondGateIdx()const {return m_nSecondStandIdx;}

	void SetThirdGateIdx(int nIdx) {m_nThirdStandIdx = nIdx;}
	int getThirdGateIdx()const {return m_nThirdStandIdx;}

	CString GetStandString(int nStandIdx)const; 

	bool IsFlightFitContraint(const ARCFlight* pFlight, std::vector<ARCFlight*>& vFlights, int nGateIdx,int nAdjGateIdx, InputTerminal* pInTerm);
	bool IsFlightFitContraint(const ARCFlight* pFlight, std::vector<ARCFlight*>& vFlights, const ALTObjectID& standID,const ALTObjectID& adjStandID, InputTerminal* pInTerm);
	bool IsFlightFitConstraint(const ARCFlight* pFlight, std::vector<ARCFlight*>& vFlights, int nGateIdx, int nAdjGateIdx,InputTerminal* pInTerm,CString& strError);

	bool GetAdjancenyStandList(int nGateIdx,std::vector<int>& vStandList);
	bool GetAdjacencyStandNameList(const ALTObjectID& standID,std::vector<ALTObjectID>& vStandList);
private:
	float GetFlightSpan(const ARCFlight* pFlight, InputTerminal* pInTerm);
	bool ConcurrentValid(double fData,const ARCFlight* pFlight,InputTerminal* pInTerm);
public:
	ALTObjectID m_FirstGate;
	ALTObjectID m_SecondGate;
	ALTObjectID m_ThirdGate;
	ConditionOfUseType m_ConditionType;
	double m_Width;
	double m_Clearance;

	int m_nFirstStandIdx;
	int m_nSecondStandIdx;
	int m_nThirdStandIdx;
};


class AdjacencyConstraintSpecList : public DataSet
{
public:
	AdjacencyConstraintSpecList();
	~AdjacencyConstraintSpecList();

	virtual void clear();
	virtual void readData(ArctermFile& p_file);
	virtual void writeData(ArctermFile& p_file) const;
	virtual void loadDataSet (const CString& _pProjPath);
	virtual const char *getTitle() const { return "Adjacency Constraint Specification"; }
	virtual const char *getHeaders() const { return "Index, Stand1, Stand2, Stand3, Condition Of Use, available Width(m), WingTip Clearance(m)"; }

	int GetCount();
	void AddItem(const AdjacencyConstraintSpecDate& gateAssignCon);
	void DeleteItem(int nIndex);
	AdjacencyConstraintSpecDate& GetItem(int nIndex);

	void InitConstraintGateIdx(std::vector<ref_ptr_bee<CAssignGate> >& vGateList);
	bool IsFlightAndGateFit(const ARCFlight* flight, int nGateIdx, const ElapsedTime& tStart, const ElapsedTime& tEnd, std::vector<ref_ptr_bee<CAssignGate> >& vGateList);
	bool FlightAndGateFitErrorMessage(const ARCFlight* flight, int nGateIdx, const ElapsedTime& tStart, const ElapsedTime& tEnd, std::vector<ref_ptr_bee<CAssignGate> >& vGateList,CString& strError);
protected:
	std::vector<AdjacencyConstraintSpecDate> m_adjacencyConstraintSpec;
};