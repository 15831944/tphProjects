// GreetingKey.cpp: implementation of the CGreetingKey class.
//
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"

#include "GreetingKey.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGreetingKey::CGreetingKey(  const CMobileElemConstraint& _paxType, const CMobileElemConstraint& _visitorType ) : m_paxType(_paxType),m_visitorType(_visitorType)
{
	//m_paxType =  _paxType ;
	//m_visitorType =  _visitorType ;
}
CGreetingKey::~CGreetingKey()
{

}

bool CGreetingKey::operator == ( const CGreetingKey& _anotherKey )const
{
	return m_paxType == _anotherKey.m_paxType && m_visitorType == _anotherKey.m_visitorType;
}

bool CGreetingKey::operator < ( const CGreetingKey& _anotherKey )const
{
	if( m_paxType == _anotherKey.m_paxType )
	{
		return m_visitorType < _anotherKey.m_visitorType ? true : false;
	}
	else if( m_paxType < _anotherKey.m_paxType )
	{
		return true;
	}
	else
	{
		return false;
	}

}

#ifdef _DEBUG
void CGreetingKey::TraceInfo()const
{
	CString  cstr;
	m_paxType.screenPrint(cstr,0,256);
	// TRACE( "paxType = %s, ", cstr.GetBuffer(0) );
	m_visitorType.screenPrint(cstr,0,256);
    // TRACE( "visitorType = %s", cstr.GetBuffer(0) );
}
#endif
