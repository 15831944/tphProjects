// PaxVisitorGreetingPlace.h: interface for the CPaxVisitorGreetingPlace class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PAXVISITORGREETINGPLACE_H__B426208F_C9BF_4DD2_9143_14B29795F313__INCLUDED_)
#define AFX_PAXVISITORGREETINGPLACE_H__B426208F_C9BF_4DD2_9143_14B29795F313__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "GreetingProcessors.h"
class CPaxVisitorGreetingPlace  
{
	CMobileElemConstraint m_paxType;
	std::vector<CGreetingProcessors> m_vVisitorGreetingPlace;
public:
	CPaxVisitorGreetingPlace();
	virtual ~CPaxVisitorGreetingPlace();
	bool operator < ( const CPaxVisitorGreetingPlace& _anther )const { return m_paxType < _anther.m_paxType ? true :false;	};
	bool operator == ( const CPaxVisitorGreetingPlace& _anther )const { return m_paxType == _anther.m_paxType ? true : false;	};
public:
	void SortVisitorPlace();
	void InsertGreetingPlace( const CMobileElemConstraint& _visitorType, std::vector<ProcessorID>& _vGreetingPlace );
	CMobileElemConstraint& GetPaxType()  { return m_paxType;	};
	void SetPaxType ( const CMobileElemConstraint& _paxType ){ m_paxType = _paxType;	};
	bool GetPaxVisitorGreetingPlace( const CMobileElemConstraint& _visitorConstraint, std::vector<ProcessorID>& _vGreetingPlace );

	#ifdef _DEBUG
	void TraceInfo()const;
	#endif

	
};

#endif // !defined(AFX_PAXVISITORGREETINGPLACE_H__B426208F_C9BF_4DD2_9143_14B29795F313__INCLUDED_)
