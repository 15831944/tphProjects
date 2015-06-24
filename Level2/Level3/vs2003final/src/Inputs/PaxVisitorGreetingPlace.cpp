// PaxVisitorGreetingPlace.cpp: implementation of the CPaxVisitorGreetingPlace class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "PaxVisitorGreetingPlace.h"
#include <algorithm>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPaxVisitorGreetingPlace::CPaxVisitorGreetingPlace()
{

}

CPaxVisitorGreetingPlace::~CPaxVisitorGreetingPlace()
{
	m_vVisitorGreetingPlace.clear();
}
void CPaxVisitorGreetingPlace::SortVisitorPlace()
{
	std::sort( m_vVisitorGreetingPlace.begin(), m_vVisitorGreetingPlace.end() );
}
void CPaxVisitorGreetingPlace::InsertGreetingPlace( const CMobileElemConstraint& _visitorType, std::vector<ProcessorID>& _vGreetingPlace )
{
	std::vector<CGreetingProcessors>::iterator iter = m_vVisitorGreetingPlace.begin();
	std::vector<CGreetingProcessors>::iterator iterEnd = m_vVisitorGreetingPlace.end();
	for( ; iter!=iterEnd; ++iter )
	{
		if( iter->GetVisitorType() == _visitorType )
		{
			* ( iter->GetVisitorGreetingPlace() ) = _vGreetingPlace;
			return;
		}
	}

	CGreetingProcessors temp( _visitorType );
	* ( temp.GetVisitorGreetingPlace() ) = _vGreetingPlace;
	m_vVisitorGreetingPlace.push_back( temp );
	
}
bool CPaxVisitorGreetingPlace::GetPaxVisitorGreetingPlace( const CMobileElemConstraint& _visitorConstraint, std::vector<ProcessorID>& _vGreetingPlace )
{
	std::vector<CGreetingProcessors>::iterator iter = m_vVisitorGreetingPlace.begin();
	std::vector<CGreetingProcessors>::iterator iterEnd = m_vVisitorGreetingPlace.end();
	for( ; iter!=iterEnd; ++iter )
	{
		if( iter->GetVisitorType().fits(  _visitorConstraint ) )
		{
			_vGreetingPlace = * ( iter->GetVisitorGreetingPlace() );
			return true;
		}
	}

	return false;
}


#ifdef _DEBUG
void CPaxVisitorGreetingPlace::TraceInfo()const
{
 TRACE("\n@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
	//char str[256 ];
	CString str1;
	m_paxType.screenPrint( str1,0,256 );
	 TRACE(" pax type = %s", str1.GetBuffer(0) );
	std::vector<CGreetingProcessors>::const_iterator iter = m_vVisitorGreetingPlace.begin();
	std::vector<CGreetingProcessors>::const_iterator iterEnd = m_vVisitorGreetingPlace.end();
	for( ; iter!=iterEnd; ++iter )
	{
		iter->TraceInfo();
	}

 TRACE("\n@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
}
#endif