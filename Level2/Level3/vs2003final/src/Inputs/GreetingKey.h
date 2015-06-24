// GreetingKey.h: interface for the CGreetingKey class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GREETINGKEY_H__1C051C76_883D_4B82_9FA7_89488FB75F40__INCLUDED_)
#define AFX_GREETINGKEY_H__1C051C76_883D_4B82_9FA7_89488FB75F40__INCLUDED_
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "MobileElemConstraint.h"
class CGreetingKey  
{
private:
	CMobileElemConstraint m_paxType;
    CMobileElemConstraint m_visitorType;
public:
	CGreetingKey( const CMobileElemConstraint& _paxType, const CMobileElemConstraint& _visitorType );
	virtual ~CGreetingKey();
public:
	bool operator == ( const CGreetingKey& _anotherKey )const;
	bool operator < ( const CGreetingKey& _anotherKey )const;

	#ifdef _DEBUG
	void TraceInfo()const;
	#endif

};

#endif // !defined(AFX_GREETINGKEY_H__1C051C76_883D_4B82_9FA7_89488FB75F40__INCLUDED_)
