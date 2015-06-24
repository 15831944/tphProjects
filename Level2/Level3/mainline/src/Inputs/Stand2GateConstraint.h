#pragma once 
#include <CommonData/procid.h>
#include "../InputAirside/ALTObject.h"
#include "../Common/dataset.h"
#include "GateAssignPreferenceMan.h"
class ProcessorArray;
class CGateAssignmentMgr;
class CFlightOperationForGateAssign;
class CStand2GateMapping
{
public:
	enum MapType
	{
		MapType_Normal = 0,
		MapType_1to1,
		MapType_Random,


		///
		MapTypeCount

	};
public:
	CStand2GateMapping(void);
	virtual ~CStand2GateMapping(void);

	bool operator < (const CStand2GateMapping& gateMap)const;
public:
	ALTObjectID m_StandID; 

	//really stupid design
	ProcessorID m_ArrGateID;
	ProcessorID m_DepGateID;

	MapType m_enumMappingFlag;//0,normal;1,1:1;2,random mapping;
};

class CStand2GateConstraint :public DataSet,public std::vector<CStand2GateMapping >
{
public:
	CStand2GateConstraint(int nProjID);
	virtual ~CStand2GateConstraint(void);

	virtual const char *getTitle (void) const { return "Stand 2 Gate Mapping"; }
	virtual const char *getHeaders (void) const{  return "size,Stand,Arrival Gate,Departure Gate,IsOne2One,"; }
	virtual void readData (ArctermFile& p_file);
	virtual void writeData (ArctermFile& p_file) const;
	virtual void readObsoleteData(ArctermFile& p_file ){ read2Data(p_file);}
	void read2Data(ArctermFile& p_file);
	virtual void clear (void);

	virtual ProcessorID GetArrGateIDInSim(Flight * pFlight, ALTObjectID &standID);
	virtual ProcessorID GetDepGateIDInSim(Flight * pFlight, ALTObjectID &standID);


	virtual ALTObjectID GetStdGateIDbyArrGate(std::vector<CStand2GateMapping> gateConstraint,ProcessorID &arrGateID);
	virtual ALTObjectID GetStdGateIDbyDepGate(std::vector<CStand2GateMapping> gateConstraint,ProcessorID &depGateID);
	Processor* GetOneToOneProcessor(ProcessorArray& gateArray, const ProcessorID* _pSourceID, int _nDestIdLength ) const;

	std::vector<CStand2GateMapping> m_pStand2Arrgateconstraint;
	std::vector<CStand2GateMapping> m_pStand2Depgateconstraint;

	void setGateAssignManager(CGateAssignmentMgr* pGateAssignManager);

	//get the gate id by flight
	virtual ProcessorID GetArrGateID(CFlightOperationForGateAssign* pFlight);
	virtual ProcessorID GetDepGateID(CFlightOperationForGateAssign* pFlight);
protected:
	ProcessorID GetGateID(CFlightOperationForGateAssign* pFlight, std::vector<CStand2GateMapping>& gateConstraint, CGateAssignPreferenceMan* gatePreference,const ALTObjectID& standID, BOOL bArrival);

private:
	bool GetOneToOneGateID(CFlightOperationForGateAssign* pFlight,CGateAssignPreferenceMan* gatePreference,const std::vector<ProcessorID>& vCandidateGate,ProcessorID& gateProID);
protected:
	int m_nProjID;
	CGateAssignmentMgr* m_pGateAssignManager;
protected:
	CArrivalGateAssignPreferenceMan* m_ArrivalPreference ;
	CDepGateAssignPreferenceMan* m_DepPreference ;
public:
	CArrivalGateAssignPreferenceMan* GetArrivalPreferenceMan() { return m_ArrivalPreference ;} ;
	CDepGateAssignPreferenceMan* GetDepPreferenceMan() { return m_DepPreference ;} ;
};