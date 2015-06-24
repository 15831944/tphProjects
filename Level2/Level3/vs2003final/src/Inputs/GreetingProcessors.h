// GreetingProcessors.h: interface for the CGreetingProcessors class.
#pragma once
#include <CommonData/procid.h>
#include "MobileElemConstraint.h"

class CGreetingProcessors  
{
private:
	//CGreetingKey m_key;
	CMobileElemConstraint m_visitorType;
	std::vector<ProcessorID> m_vGreetingPlace;
public:
	//CGreetingProcessors( const CMobileElemConstraint& _paxType, const CMobileElemConstraint& _visitorType );
	CGreetingProcessors( const CMobileElemConstraint& _visitorType );
	virtual ~CGreetingProcessors();
public:
	//const CGreetingKey& key() const { return m_visitorType;	}
	std::vector<ProcessorID>* GetVisitorGreetingPlace() { return &m_vGreetingPlace;}
	CMobileElemConstraint& GetVisitorType(){ return m_visitorType;	};
	//bool operator == ( const CGreetingProcessors& _another )const { return m_visitorType == _another.m_visitorType;}

	bool operator < ( const CGreetingProcessors& _another )const{ return m_visitorType < _another.m_visitorType ? true : false;	}

	#ifdef _DEBUG
	void TraceInfo()const;
	#endif

};
