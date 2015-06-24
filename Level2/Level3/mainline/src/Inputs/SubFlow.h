// SubFlow.h: interface for the CSubFlow class.
#pragma once

class CAllPaxTypeFlow;
class CSingleProcessFlow;
class CSinglePaxTypeFlow;
class InputTerminal;
class ArctermFile;
class ProcessorID;
class CProcessorDestinationList;
class ProcessorEntry;
class PassengerFlowDatabase;
class ProcessorDistribution;
class ProcessorArray;
class CFlowItemEx;

#include "MobileElemConstraint.h"
#include "Direction.h"

class CSubFlow
{
private:
	CString m_sProcessUnitName;

	CDirection m_Dircetion;

	// the process's inside flow
	CSinglePaxTypeFlow* m_pInternalFlow;

	// all possible flow.
	//every single flow is an one father - one child type tree.
	std::vector<CSinglePaxTypeFlow*> m_vAllPossiblePath;

	
	// the key of map is the paxtype constraint
	typedef std::map< CString, CAllPaxTypeFlow* > PAXTYPE_TO_PAXFLOW;

	// flow of evry paxtype with checkbag in process 
	PAXTYPE_TO_PAXFLOW m_mapPaxWithBagToAllFlow;

	// flow of evry paxtype without checkbag in process 
	PAXTYPE_TO_PAXFLOW m_mapPaxWithoutBagToAllFlow;

	
	ProcessorEntry* m_pTransition;
	ProcessorEntry* m_pTransitionGateDetail;
    ProcessorEntry* m_pStationTransition;
public:
	CSubFlow( InputTerminal* _pTerm = NULL );
	CSubFlow( const CSubFlow& _another );
	CSubFlow& operator = (  const CSubFlow& _another );

	virtual ~CSubFlow();
protected:
	virtual void Clear();
public:
	void SetInputTerm( InputTerminal* _pTerm );
	void SetProcessUnitName( const CString _sName ){ m_sProcessUnitName = _sName;	}
	CString GetProcessUnitName()const { return m_sProcessUnitName;	}

	void AddCanLeadGate(const CMobileElemConstraint &p_type, const ProcessorID& procID, ProcessorList *pProcList);
	BOOL FindCanLeadGate(const CMobileElemConstraint &p_type, const ProcessorID& procID);
	
	BOOL IfHasGateProcessor();
	void GetGateProcessor(std::vector<ProcessorID>& vProcID);

	BOOL IfHasReclaimProcessor();
	void GetReclaimProcessor(std::vector<ProcessorID>& vProcID);

	void PrintGateInfo();

	CSinglePaxTypeFlow* GetInternalFlow(){ return (CSinglePaxTypeFlow*)m_pInternalFlow;	};
	int GetPossiblePathCount ()const { return m_vAllPossiblePath.size();}
	const CSinglePaxTypeFlow* GetPossiblePathAt( int _iIdx );
	

	// Find if PROCESS has path to the destination
	bool CanLeadTo( const CMobileElemConstraint& _paxType,  const ProcessorID& _destProcID,  InputTerminal* _pTerm)const;
	
	//check sub flow percentage valid
	bool IfSubFlowValid()const;
public:

	void ReadData (ArctermFile& p_file);	
	void readObsoleteData (ArctermFile& p_file);	
	void readObsoleteData21 (ArctermFile& p_file);	
	void readObsoleteData24 (ArctermFile& p_file);	

	void WriteData (ArctermFile& p_file) const;

public:
	
	void SetTransitionLink (const PassengerFlowDatabase& database, const PassengerFlowDatabase& _gateDetailFlowDB, const PassengerFlowDatabase& _stationPaxFlowDB);
	ProcessorEntry* GetNormalDestDistribution() { return  m_pTransition;};
	ProcessorDistribution *GetNextDestinations(const CMobileElemConstraint &p_type, int _nInGateDetailMode );

	// build all possible path in process
	void BuildAllPossiblePath();

	// return a best internal flow of process .
	// used by engine to handle how to move in process
	// assume the _constraint is most detaited type
	CSinglePaxTypeFlow* GetPathInProcess( const CMobileElemConstraint& _constraint , bool _bHasGate ,const ProcessorID& _gateID, bool _bHaveBag, const ProcessorID& _bagDeviceID ,const CMobileElemConstraint& _leadToConstraint);
public:

	void DrawProcessInternalFlow( BOOL* bOn, double* dAlt);
	
private:
	int GetPathIndexInProcess(CAllPaxTypeFlow * pFlow, int iPathCount);
	int GetDestinationIndex(CProcessorDestinationList* pProcList);

	// divide _tempFlow as a list of single tree ( one father - one child type tree.)
	void BuildAllPathOfSingleTree( const ProcessorID& _rootID , CSinglePaxTypeFlow& _tempFlow );

	//get all possible path in process 
	CAllPaxTypeFlow* GetAllPossibleFlowByPaxType( const CMobileElemConstraint& _constraint , bool _bHasGate ,const ProcessorID& _gateID, bool _bHaveBag, const ProcessorID& _bagDeviceID,const CMobileElemConstraint& _leadToConstraint);

	//update _psingleFlow's specific node according to one to one relationship 
	void ReplaceProcessorInPath( CSinglePaxTypeFlow* _pSingleFlow, int _iStartPos , const ProcessorID& _replacedByProcID );

	// return _procID 's father flow pair in _pSingleFlow
	CProcessorDestinationList* GetSourcePair( CSinglePaxTypeFlow* _pSingleFlow, const ProcessorID& _procID );

	//get a specific processorid in sourceid grop which has one to one relationship with dest processorid 
	bool GetOneToOneProcessorID( const ProcessorID& _sourceID, const ProcessorID& _destID, ProcessorID& _resutID )const;

	void TravelPath(  CSinglePaxTypeFlow* _pFlow ,const ProcessorID& _rootID , std::vector<ProcessorID>& _result )const;;
	bool CanLeadToGate( CSinglePaxTypeFlow* _pFlow, const ProcessorID& _gateID ,const CMobileElemConstraint& _leadToConstraint)const;

public:
	//get the next processor for test floor
	Processor* GetNextProcessorForTestFloor(Person* pPerson, Processor* pProc);
	//get the next processor in this sub flow with the current processor 
	Processor* GetNextProcessor(Person* pPerson, ElapsedTime _curTime, bool bHandleChannel = true, bool _bStationOnly = false, bool _bExcludeFlowBeltProc = false);

	//get all pipes will pass to the next processor
	void GetProcessorPipes(Processor* pProc, const ProcessorID& procID, CFlowItemEx& flowItem);
	
	//Test if root processor in sub flow
	bool IsRootProc(const ProcessorID& procID);

	bool getUsedProInfo(const ProcessorID& id,InputTerminal *_pTerm,std::vector<CString>& _strVec);
	bool IfUseThisPipe(int nCurPipeIndex) const;
private:
	InputTerminal* m_pInputTerm;

	typedef std::vector<ProcessorID> VECTPROCID;
	typedef std::map<ProcessorID, bool> MAPPROC1TO1;
	typedef std::map<ProcessorID, MAPPROC1TO1> MAPPROC;
	MAPPROC m_mapLeadToGates;
	MAPPROC m_mapBaggageReclaims;

	void BuildGateRelationships();
	void BuildBaggageRelationships();
	void AddLeadToGateRelationships();

	void AddProcessorIntoMap(MAPPROC& mapProc, const ProcessorID& ProcID, const std::pair<ProcessorID, bool>& pairProc1To1);

	int GetAllProcessorsByType(ProcessorClassList ProcType, VECTPROCID& vProcID);
	void SetValueIfOneToOne(MAPPROC& mapProc, const ProcessorID& ProcID);

	void PrintMapInfo(MAPPROC& mapProc);

	void FilterOutProcessorByGateCondition(Person* pPerson, ProcessorArray& array, const ProcessorID* nextGroup);
	void FilterOutProcessorByBaggageCondition(Person* pPerson, ProcessorArray& array, const ProcessorID* nextGroup);
	Processor* HandleOneToOne(Person* pPerson, const ProcessorArray& array, const ProcessorID* nextGroup);
};
