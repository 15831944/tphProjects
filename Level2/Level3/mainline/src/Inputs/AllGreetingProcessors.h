// AllGreetingProcessors.h: interface for the CAllGreetingProcessors class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ALLGREETINGPROCESSORS_H__75B8164B_EDB2_40BE_895C_6B9C3A264B5E__INCLUDED_)
#define AFX_ALLGREETINGPROCESSORS_H__75B8164B_EDB2_40BE_895C_6B9C3A264B5E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
class CAllPaxTypeFlow;
class CPaxFlowConvertor;
class CMobileElemConstraint;
class CSinglePaxTypeFlow;
//#include "GreetingProcessors.h"
#include "PaxVisitorGreetingPlace.h"
class CAllGreetingProcessors  
{
private:
	std::vector<CPaxVisitorGreetingPlace> m_vAllGreetingProcessors;

public:
	CAllGreetingProcessors();
	virtual ~CAllGreetingProcessors();
public:
//	std::vector<ProcessorID>* operator [] (  const CGreetingKey& _key  ) ;
	void  FindGreetingPlaceFromAllPaxFlow (CPaxFlowConvertor& _convertor );

	void GetGreetingPlace( const CMobileElemConstraint& _paxConstraint, const CMobileElemConstraint& _visitorConstraint, std::vector<ProcessorID>& _vGreetingPlace );


private:
	void FindGreetingPlaceInSingleFlow( const CSinglePaxTypeFlow* _pVisitorFlow, const CMobileElemConstraint* _pConstraint );

	// input: _leafNode    check if leafNode is PROCESS, use the processor in process instead.
	void ExpandProcess( InputTerminal* _pTerm, std::vector<ProcessorID>& _leafNode );

	void RemoveNonHoldingAreaProcessor( InputTerminal* _pTerm ,std::vector<ProcessorID>& _leafNode );

	void InsertPaxVisitorGreetingPlace( const CMobileElemConstraint& _paxConstraint, const CMobileElemConstraint& _visitorConstraint, std::vector<ProcessorID>& _vGreetingPlace );

	void Sort();
	


	
	#ifdef _DEBUG
	void TraceAllResult()const;
	#endif

};

#endif // !defined(AFX_ALLGREETINGPROCESSORS_H__75B8164B_EDB2_40BE_895C_6B9C3A264B5E__INCLUDED_)
