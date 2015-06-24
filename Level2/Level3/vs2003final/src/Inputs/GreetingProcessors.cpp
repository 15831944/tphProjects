// GreetingProcessors.cpp: implementation of the CGreetingProcessors class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "GreetingProcessors.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGreetingProcessors::CGreetingProcessors(  const CMobileElemConstraint& _visitorType )
: m_visitorType( _visitorType )
{

}

CGreetingProcessors::~CGreetingProcessors()
{
	m_vGreetingPlace.clear();
}
#ifdef _DEBUG
void CGreetingProcessors::TraceInfo()const
{
	TRACE("\n**********\n");
	//char str[256 ];
	CString str1;
	m_visitorType.screenPrint( str1,0,256 ); //MATT
	TRACE(" visitor type = %s", str1 );
	
	std::vector<ProcessorID>::const_iterator iter = m_vGreetingPlace.begin();
	std::vector<ProcessorID>::const_iterator iterEnd = m_vGreetingPlace.end();
	for( ; iter!=iterEnd; ++iter)
	{
		TRACE("%s, ",iter->GetIDString() );
	}


	TRACE("\n**********\n");
	
}
#endif