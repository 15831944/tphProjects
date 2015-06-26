// SinglePaxTypeFlow.h: interface for the CSinglePaxTypeFlow class.
#pragma once
#include "ProcessorDestinationList.h"
#include "MobileElemConstraint.h"

class CAllPaxTypeFlow;
class CDirectionUtil;

typedef std::vector<CProcessorDestinationList > PAXFLOWIDGRAPH;

class CSinglePaxTypeFlow  
{
protected:
	PAXFLOWIDGRAPH m_vPaxFlowDigraph;
	CMobileElemConstraint* m_pPaxConstrain;
	const CSinglePaxTypeFlow* m_pParentPaxFlow;
	bool m_bHasChanged;
	InputTerminal* m_pTerm;
	std::vector<CFlowDestination> m_vFlowDestion;

public:
	CSinglePaxTypeFlow( InputTerminal* _pTerm );
	CSinglePaxTypeFlow();
	CSinglePaxTypeFlow( const CSinglePaxTypeFlow& _anotherPaxFlow );
	CSinglePaxTypeFlow& operator = ( const CSinglePaxTypeFlow& _anotherPaxFlow );
	virtual ~CSinglePaxTypeFlow();
public:
	bool getUsedProInfo(const ProcessorID& id,InputTerminal *_pTerm,std::vector<CString>& _strVec,const CString& strSubFlowName);//only for subflow check
	bool IfUseThisPipe(int nCurPipeIndex)const;//check singleFlow use pipe condition
	void SetInputTerm( InputTerminal* _pTerm ){ m_pTerm = _pTerm ;};
	
	PAXFLOWIDGRAPH GetFlowDigraph(){return m_vPaxFlowDigraph ;};

	InputTerminal* GetInputTerm(){ return m_pTerm ;};
	void CSinglePaxTypeFlow::BuildVisitSinglTree(const ProcessorID& _rootID);
	// add a destination to an  procssor
	void AddDestProc(const ProcessorID & _sourceProcID,const CFlowDestination& _destProc , bool _bMustEvenPercent = true);

	// add _secondProc to _newProc's destination list  , add _newProc  to _firstProc's destination list and remove _secondProc from _firstProc's destination list
	virtual void InsertBetwwen( const ProcessorID & _firstProcID, const ProcessorID & _secondProcID, const CFlowDestination& _newProc );

	//remove _destProc from _sourceProc's destination list if this pair is derived from father and _sourceProc has only one destination ,that is _destProc, then this operation is not allowed.
	void CutLinkWithSourceProc( const  ProcessorID& _sourceProcID, const ProcessorID& _destProcID );

	// add all destnition proccessors of _oldDestProcId to _soruceProcID and remove all destproc from _oldDestProcId , then remove _oldDestProcID
	void TakeOverDestProc( const  ProcessorID& _sourceProcID, const ProcessorID& _oldDestProcID , bool _bEraseOldProc = false );

	// add all destnition proccessors of _procID to all its source proc and remove all destproc from _procID and then remove _procID
	void TakeOverDestProc(const ProcessorID& _procID );

	//Remove _procID from _proc's all source proc's destination list and remove _procID's all destination list and then remove _procID
	void DeleteProcNode( const ProcessorID& _procID );

	// add a new node which have no source proc and no destination proc
	void AddIsolatedProc( const ProcessorID& _procID );

	//add  _sourceProcID's dest proc into _insertProcID's dest list , and delete _sourceProcID's all dest list ,then add _insertProcID into _sourceProcID's dest list
	virtual void InsertProceoosorAfter( const ProcessorID& _sourceProcID , const ProcessorID& _insertProcID  );

	// make the flow _flowPairProcID's dest ( exclude _operateProcID ) proc's percent balance 
	void EvenRemainDestPercent( const ProcessorID& _flowPairProcID , const ProcessorID& _operateProcID  );
	//  [7/5/2004]void EvenRemainDestPercent( const ProcessorID& _flowPairProcID , const std::vector<ProcessorID>& _vOperateProcID  );
		
	void ClearUselessTree( const ProcessorID& _usefulRootProcID );
	int GetFlowPairCount() const{ return m_vPaxFlowDigraph.size();	};

	const CProcessorDestinationList& GetFlowPairAt( int _iIndex )const;
	bool GetFlowPairAt( const ProcessorID& _procID, CProcessorDestinationList& _flowPair )const;
	CProcessorDestinationList* GetFlowPairAt( int _iIndex );
	CProcessorDestinationList* GetFlowPairAt( const ProcessorID& _procID);

	int GetFlowPairIdx( const ProcessorID& _flowPairID )const;

	//get _procID's all source proc
	void GetSourceProc( const ProcessorID& _procID,std::vector<ProcessorID>& _sourceIDVector )const;
	int GetSourceProcCount( const ProcessorID& _procID )const;

	const CMobileElemConstraint* GetPassengerConstrain() const { return m_pPaxConstrain ;};

	bool IfExist( const ProcessorID& _procID ) const ;
	bool IfFits( const ProcessorID& _procID ) const ;
	//if _procID exist or _procID's super group exist in pax flow
	// return idx of pos if exist , else return -1
	int IfProcessGroupExist( const ProcessorID& _procID ) const ;

	bool IfBaggageProcExistInFlow()const;
	bool IfGateProcExistInFlow()const;

	bool IfFlowBeChanged()const { return m_bHasChanged;	};
	void SetParentPaxFlow( const CSinglePaxTypeFlow* _pParentFlow ){ m_pParentPaxFlow = _pParentFlow;	};
	const CSinglePaxTypeFlow* GetParentPaxFlow()const { return m_pParentPaxFlow;	};

	void SetChangedFlag( bool _bFlag ){ m_bHasChanged = _bFlag;	};
	void SetPaxConstrain( const CMobileElemConstraint* _pPaxConstrain );
	
	//get all processors in passenger flow hierarchy 
	void GetAllInvolvedProc( std::vector<ProcessorID>& _sourceIDVector ) const ;
	
	//get first processor equal with _procID in hierarchy flow 
	const CProcessorDestinationList* GetHierarchyFlowPair( const ProcessorID& _procID ,bool& _bOwened ) const;

	void InitPaxConstrain( InputTerminal* _pTerm, CString& _sPaxConstrainName, int _nConstraintVersion );
	
	void AddFowPair( const CProcessorDestinationList& _flowPair );
	void ClearAllPair() { m_vPaxFlowDigraph.clear();	};
	virtual void ClearIsolateNode();
	// clear any tree which is produced by some operation such as CutLinkWithSourceProc
	void ClearSideEffectTree( const ProcessorID& _rootProcID);

	//copy _destPair's all destproc into _sourcepair
	// _bBackInsert = true : add into tail . _bBackInsert = false : clear all _sourcePair's destproc and add
	// _iInheritedFlagType = 0 : inheritedflag don not be changed
	// _iInheritedFlagType = 1 : set inheritedflag = true
	// _iInheritedFlagType = 2 : set inheritedflag = false
	void CopyFlowPair( CProcessorDestinationList* _sourcePair, const CProcessorDestinationList* _destPair , bool _bBackInsert, int _iInheritedFlagType );

	void GetRootProc( std::vector<ProcessorID>& _rootIDVector ) const ;
	void GetLeafNode( std::vector<ProcessorID>& _leafIDVector ) const ;
	void GetLeafNodeEx( std::vector<ProcessorID>& _leafIDVector )  ;

	bool IsRootInHierarchy( const ProcessorID& _testProcID ) const;
	void GetRootProcInHierarchy( std::vector<ProcessorID>& _rootIDVector ) const ;

	void SetAllPairVisitFlag( bool _bVisit );

	friend bool operator < ( const CSinglePaxTypeFlow& _firstPaxFlow ,const CSinglePaxTypeFlow& _anotherPaxFlow );
	void PrintAllStructure()const;
	bool IfFlowValid() const;
	bool IfHaveCircleFromProc( const ProcessorID& _fromProc );
	bool ResetDesitinationFlag(bool bFlag);

	void MergeTwoFlow( const CSinglePaxTypeFlow& _flow );
	void BuildAllPossiblePath( std::vector<CSinglePaxTypeFlow*>& _vAllPossiblePath, CDirectionUtil* pDir = NULL);

	void BuildLineFlowFromVector( std::vector<CFlowDestination>& _vProc );
	CProcessorDestinationList* GetEqualFlowPairAt( const ProcessorID& _procID);
	bool GetEqualFlowPairAt( const ProcessorID& _procID, CProcessorDestinationList& _flowPair )const;
private:
	void FindLeafFromNode( const ProcessorID& _fromProc, CSinglePaxTypeFlow& _resultFlow,std::vector<ProcessorID>& _leafIDVector );
	// divide _tempFlow as a list of single tree ( one father - one child type tree.)
	void BuildAllPathOfSingleTree( const ProcessorID& _rootID , std::vector<CFlowDestination>& _vProc, std::vector<CSinglePaxTypeFlow*>& _vAllPossiblePath, CDirectionUtil* pDir = NULL );
	

	// assumme the SinglePaxTypeFlow is one line flow. not a tree. check if there any repeat processor sublist in the list.
	bool RepeatPattern( std::vector<CFlowDestination>& _vProc );

};
