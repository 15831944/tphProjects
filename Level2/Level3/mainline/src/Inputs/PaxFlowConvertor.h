// PaxFlowConvertor.h: interface for the CPaxFlowConvertor class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PAXFLOWCONVERTOR_H__CA3284EF_41E7_4A78_99EC_0C8FE61F339F__INCLUDED_)
#define AFX_PAXFLOWCONVERTOR_H__CA3284EF_41E7_4A78_99EC_0C8FE61F339F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CAllPaxTypeFlow;
class CSinglePaxTypeFlow;
class Constraint;
//class PassengerFlowDatabase;
class ProcessorID;
class InputTerminal;
class PassengerFlowDatabase;
class Flight;
class CProcessorDestinationList;
class CPaxFlowConvertor  
{
private:
	//PassengerFlowDatabase* m_pPaxFlowDB;
	//PassengerFlowDatabase* m_pStationPaxFlowDB;
	InputTerminal* m_pTerm;
	CSinglePaxTypeFlow* m_pTempNonPaxFlow;//record non pax single flow
public:
	CPaxFlowConvertor();
	virtual ~CPaxFlowConvertor();
public:
	void SetInputTerm( InputTerminal* _pTerm ) { m_pTerm = _pTerm;	} ;
	InputTerminal* GetInputTerm(){ return m_pTerm;	};

	virtual void ToDigraphStructure( CAllPaxTypeFlow& _allPaxTypeFlow ) const ;
	virtual void ToDigraphStructureFromSinglePaxDB( CAllPaxTypeFlow& _allPaxTypeFlow,PassengerFlowDatabase* _pPaxFlowDB ) const;
	virtual void ToOldStructure( const CAllPaxTypeFlow& _allPaxTypeFlow )const;
	
	virtual void BuildHierarchyFlow( const CSinglePaxTypeFlow& _sourceFlow ,CSinglePaxTypeFlow& _resultFlow ) const;
	virtual void BuildSinglePaxFlow( const CSinglePaxTypeFlow& _oldFlow ,const CSinglePaxTypeFlow& _compareFlow ,CSinglePaxTypeFlow& _resultFlow ) const;

	virtual bool BuildSpecFlightFlowFromGateAssign( CAllPaxTypeFlow& _allPaxTypeFlow )const;
	virtual void ToOldFlightFlow( const CAllPaxTypeFlow& _allPaxTypeFlow  )const;

	// combine _nonPaxFlow and _paxFlow into a new flow. create new _nonPaxFlow
	// input: _paxFlow _nonPaxFlow 
	// output: _nonPaxFlow _resultFlow
	virtual void BuildAttachedNonPaxFlow( const CSinglePaxTypeFlow& _paxFlow, CSinglePaxTypeFlow& _nonPaxFlow, CSinglePaxTypeFlow& _resultFlow ) const;

	// build _resultFlow from _paxFlow,_nonPaxFlow,_operatePaxFlow in order to store it into file
	virtual void BuildNonPaxFlowToFile( const CSinglePaxTypeFlow& _paxFlow, const CSinglePaxTypeFlow& _nonPaxFlow, const CSinglePaxTypeFlow& _operatePaxFlow, CSinglePaxTypeFlow& _resultFlow ) const;



private:
	
	bool IsStation( const ProcessorID& _procID )const ;
	CSinglePaxTypeFlow* GetRelatedPaxTypeFlow( CAllPaxTypeFlow& _allPaxTypeFlow ,const Constraint*  _pConstrain )const;
	void MergeHierarchyFlow( const CSinglePaxTypeFlow& _sourceFlow,
		CSinglePaxTypeFlow& _resultFlow,
		const ProcessorID& _sourceProc,
		std::vector<const CProcessorDestinationList* >& vVisitedPair ) const;

	bool HandTurroundFlight( Flight* _pTurrountFlight ,CAllPaxTypeFlow& _allPaxTypeFlow,int _iGateIdx )const;

    //create _resultFlow  depond on _paxFlow and _nonPaxFlow's information , _nonPaxFlow will change if need
	// input: _paxFlow _nonPaxFlow  _fromProc
	// output: _nonPaxFlow _resultFlow	
	void CombineNonPaxFlow( const CSinglePaxTypeFlow& _paxFlow, CSinglePaxTypeFlow& _nonPaxFlow, CSinglePaxTypeFlow& _resultFlow , const ProcessorID& _fromProc ) const;

	void DividNonPaxFlow( const CSinglePaxTypeFlow& _paxFlow, const CSinglePaxTypeFlow& _nonPaxFlow, const CSinglePaxTypeFlow& _operatePaxFlow, CSinglePaxTypeFlow& _resultFlow , const ProcessorID& _fromProc ) const;
	
	

};

#endif // !defined(AFX_PAXFLOWCONVERTOR_H__CA3284EF_41E7_4A78_99EC_0C8FE61F339F__INCLUDED_)
