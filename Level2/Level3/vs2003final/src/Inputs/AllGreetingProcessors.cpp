// AllGreetingProcessors.cpp: implementation of the CAllGreetingProcessors class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "AllGreetingProcessors.h"
#include "AllPaxTypeFlow.h"
#include "PaxFlowConvertor.h"
#include "engine\proclist.h"
#include "SubFlow.h"
#include "SubFlowList.h"
#include "../Common/ARCTracker.h"
#include "in_term.h"

#include<algorithm>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAllGreetingProcessors::CAllGreetingProcessors()
{

}

CAllGreetingProcessors::~CAllGreetingProcessors()
{

}
/*
std::vector<ProcessorID>*  CAllGreetingProcessors:: operator [] (  const CGreetingKey& _key  ) 
{
	std::vector<CGreetingProcessors>::iterator iter = m_vAllGreetingProcessors.begin();
	std::vector<CGreetingProcessors>::iterator iterEnd = m_vAllGreetingProcessors.end();
	for(; iter!= iterEnd; ++iter )
	{
		if( iter->key() == _key )
			return iter->GetGreetingPlace();
	}
	return NULL;
}
*/
void CAllGreetingProcessors::GetGreetingPlace( const CMobileElemConstraint& _paxConstraint, const CMobileElemConstraint& _visitorConstraint, std::vector<ProcessorID>& _vGreetingPlace )
{
	_vGreetingPlace.clear();

	std::vector<CPaxVisitorGreetingPlace>::iterator iter= m_vAllGreetingProcessors.begin();
	std::vector<CPaxVisitorGreetingPlace>::iterator iterEnd= m_vAllGreetingProcessors.end();
	for(; iter!=iterEnd; ++iter )
	{
		if( iter->GetPaxType().fits( _paxConstraint ) )
		{
			if( iter->GetPaxVisitorGreetingPlace( _visitorConstraint, _vGreetingPlace ) )
			{
				return;
			}
		}
	}
	
}
void CAllGreetingProcessors::FindGreetingPlaceFromAllPaxFlow (  CPaxFlowConvertor& _convertor )
{
	PLACE_METHOD_TRACK_STRING();
	m_vAllGreetingProcessors.clear();

	CAllPaxTypeFlow _allFlow;
	_allFlow.ClearAll();
	_allFlow.FromOldToDigraphStructure( _convertor );
	//_allFlow.PrintAllStructure();

	//CMobileElemConstraint testPax;
	//CMobileElemConstraint testvisitor;
	int iPaxFlowCount = _allFlow.GetSinglePaxTypeFlowCount();
	for( int i=0; i<iPaxFlowCount; ++i )
	{
		const CSinglePaxTypeFlow* pSingleFlow = _allFlow.GetSinglePaxTypeFlowAt( i );
		const CMobileElemConstraint* pConstraint = pSingleFlow->GetPassengerConstrain();
		int iType = pConstraint->GetTypeIndex();
		if( iType <= 0 )
			continue;				// passenger type

		
		CMobileElemConstraint temp = *pConstraint;
		temp.SetTypeIndex( 0 );// set to paxconstraint
		temp.SetMobileElementType(enum_MobileElementType_PAX);

		
		char str[256 ];
		pConstraint->screenPrint( str );
		 TRACE("\n%s\n",str);

		temp.screenPrint( str );
		 TRACE("\n%s\n",str);
		

		CSinglePaxTypeFlow* pClosestFlow = _allFlow.GetClosestFlow( &temp );
		if( pClosestFlow )
		{
			//testvisitor = *pConstraint;
			//testPax = *pClosestFlow->GetPassengerConstrain();
			FindGreetingPlaceInSingleFlow( pSingleFlow, pClosestFlow->GetPassengerConstrain() );	// pSingleFlow is the visitor's flow
																									// pClosestFlow is the owner's flow
		}						
	}

	Sort();

	#ifdef DEBUG
	TraceAllResult();
	#endif
/*
	std::vector<ProcessorID> vtest;
	char str[ 256 ];
	testPax.screenPrint( str );
	// TRACE("%s",str);
	testvisitor.screenPrint( str );
	// TRACE("%s",str);
	this->GetGreetingPlace( testPax, testvisitor, vtest );
	int iT = vtest.size();
	for( int iFlame=0; iFlame < iT; ++iFlame )
	{
		// TRACE("\n%s\n", vtest[iFlame].GetIDString() );
	}
	*/
}
void CAllGreetingProcessors::Sort()
{
	std::vector<CPaxVisitorGreetingPlace>::iterator iter= m_vAllGreetingProcessors.begin();
	std::vector<CPaxVisitorGreetingPlace>::iterator iterEnd= m_vAllGreetingProcessors.end();
	for(; iter!=iterEnd; ++iter )
	{
		iter->SortVisitorPlace();
	}

	std::sort( m_vAllGreetingProcessors.begin(), m_vAllGreetingProcessors.end() );
}
#ifdef _DEBUG
void CAllGreetingProcessors::TraceAllResult()const
{
	std::vector<CPaxVisitorGreetingPlace>::const_iterator iter= m_vAllGreetingProcessors.begin();
	std::vector<CPaxVisitorGreetingPlace>::const_iterator iterEnd= m_vAllGreetingProcessors.end();
	for(; iter!=iterEnd; ++iter )
	{
		iter->TraceInfo();
	}

}
#endif


// input: _pVisitorFlow    visitor's flow
// input: _pConstraint		owner's passenger contraint.
void CAllGreetingProcessors::FindGreetingPlaceInSingleFlow( const CSinglePaxTypeFlow* _pVisitorFlow, const CMobileElemConstraint* _pConstraint )
{

	_pVisitorFlow->PrintAllStructure();
	std::vector<ProcessorID> vLeafNode;
	CSinglePaxTypeFlow* pFlow = const_cast< CSinglePaxTypeFlow* >( _pVisitorFlow ) ;
	pFlow->GetLeafNodeEx( vLeafNode );				// vLeafNode store the visitor flow leaf node
	//_pVisitorFlow->GetLeafNode( vLeafNode );

	int iLeafNodeCount = vLeafNode.size();
	if( iLeafNodeCount <=0 )
		return;

	if( iLeafNodeCount == 1 )
	{
		InputTerminal* pTerm = (const_cast < CSinglePaxTypeFlow*  >(_pVisitorFlow))->GetInputTerm();
		const Processor* pProcEnd = pTerm->procList->getProcessor( END_PROCESSOR_INDEX );	
		if( *pProcEnd->getID() == vLeafNode[0] )
		{
			return ;
		}
	}

	
	ExpandProcess( pFlow->GetInputTerm(), vLeafNode );
	RemoveNonHoldingAreaProcessor( pFlow->GetInputTerm(), vLeafNode );
	
	if( vLeafNode.size() > 0 )
	{
		InsertPaxVisitorGreetingPlace( *_pConstraint, *_pVisitorFlow->GetPassengerConstrain(), vLeafNode );
	}
	
	//CGreetingProcessors temp( *_pConstraint, *_pVisitorFlow->GetPassengerConstrain() );
	//*temp.GetGreetingPlace() = vLeafNode;
	//m_vAllGreetingProcessors.push_back( temp );
}

void CAllGreetingProcessors::RemoveNonHoldingAreaProcessor( InputTerminal* _pTerm ,std::vector<ProcessorID>& _leafNode )
{
	int iSize = _leafNode.size();
	for( int i=iSize-1; i>=0; --i )
	{
		GroupIndex group = _pTerm->procList->getGroupIndex( _leafNode[ i ] );
		ASSERT( group.start >= 0 );
		TRACE("\r\n %s ",_pTerm->procList->getProcessor( group.start )->getID()->GetIDString());
		if( _pTerm->procList->getProcessor( group.start )->getProcessorType() != HoldAreaProc )
		{
			_leafNode.erase( _leafNode.begin() + i );
		}
	}
	
}
void CAllGreetingProcessors::InsertPaxVisitorGreetingPlace( const CMobileElemConstraint& _paxConstraint, const CMobileElemConstraint& _visitorConstraint, std::vector<ProcessorID>& _vGreetingPlace )
{
	std::vector<CPaxVisitorGreetingPlace>::iterator iter = m_vAllGreetingProcessors.begin();
	std::vector<CPaxVisitorGreetingPlace>::iterator iterEnd = m_vAllGreetingProcessors.end();
	for( ; iter!=iterEnd; ++iter )
	{
		if( iter->GetPaxType() == _paxConstraint )
		{
			iter->InsertGreetingPlace( _visitorConstraint, _vGreetingPlace );
			return;
		}
	}

	CPaxVisitorGreetingPlace temp;
	temp.SetPaxType( _paxConstraint );
	temp.InsertGreetingPlace( _visitorConstraint, _vGreetingPlace );
	m_vAllGreetingProcessors.push_back( temp );
}


// input: _leafNode    check if leafNode is PROCESS, use the processor in process instead.
void CAllGreetingProcessors::ExpandProcess( InputTerminal* _pTerm, std::vector<ProcessorID>& _leafNode )
{
	std::vector<ProcessorID> vProcessNode;
	int iSize = _leafNode.size();

	for( int i=iSize-1; i>=0; --i )
	{
		CSubFlow* pProcess = _pTerm->m_pSubFlowList->GetProcessUnit( _leafNode[ i ].GetIDString() );
		if( pProcess )
		{
			pProcess->GetInternalFlow()->GetLeafNodeEx( vProcessNode );
			_leafNode.erase( _leafNode.begin() + i );
		}
	}
	std::copy( vProcessNode.begin(), vProcessNode.end(), std::back_inserter( _leafNode ) );
	
}