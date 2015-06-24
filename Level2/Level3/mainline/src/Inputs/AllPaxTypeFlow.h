// AllPaxTypeFlow.h: interface for the CAllPaxTypeFlow class.
#pragma once
#include "SinglePaxTypeFlow.h"

class CPaxFlowConvertor;
typedef std::vector<CSinglePaxTypeFlow*> ALLPAXFLOW;

class CAllPaxTypeFlow  
{
private:
	ALLPAXFLOW m_vAllPaxFlow;

public:
	CAllPaxTypeFlow();
	virtual ~CAllPaxTypeFlow();
	CAllPaxTypeFlow(const CAllPaxTypeFlow& _another );
	CAllPaxTypeFlow& operator = ( const CAllPaxTypeFlow& _another );

	void FromOldToDigraphStructure( CPaxFlowConvertor& _convertor );
	void FromDigraphToOldStructure( CPaxFlowConvertor& _convertor );
	void BuildHierarchyFlow( CPaxFlowConvertor& _convertor , int _iIdx, CSinglePaxTypeFlow& _resultFlow );
	void BuildSinglePaxFlow( CPaxFlowConvertor& _convertor, int _iOldFlowIdx,const CSinglePaxTypeFlow& _compareFlow ,CSinglePaxTypeFlow& _resultFlow ) const;

	int GetSinglePaxTypeFlowCount() const { return m_vAllPaxFlow.size() ;	};
	const CSinglePaxTypeFlow* GetSinglePaxTypeFlowAt( int _iIdx ) const ;
	CSinglePaxTypeFlow* GetSinglePaxTypeFlowAt( int _iIdx );
	void DeleteSinglePaxTypeFlowAt( int _iIdx , bool _bAutoSort = true );
	void ReplaceSinglePasFlowAt( int _iIdx, CSinglePaxTypeFlow* _pNewPaxFlow );
	void AddPaxFlow( CSinglePaxTypeFlow* _pNewPaxFlow, bool _bAutoBuildHiearachy = true );
	void Sort();
	void InitHierachy();
	bool IfAllFlowValid( bool _bOnlyCheckChangedFlow = false ) const;
	int GetIdxIfExist( const CSinglePaxTypeFlow& _testFlow ) const;
	void ClearAll();

	CSinglePaxTypeFlow* GetClosestFlow( const CMobileElemConstraint* _constraint );
	int GetSingleFlowIndex ( CString _sPrintString ) const ;
	void PrintAllStructure()const;

};
