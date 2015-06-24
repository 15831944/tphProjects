#include "StdAfx.h"
#include ".\processorfamily.h"


ProcessorFamily::ProcessorFamily(void)
: m_nStartIndexInProcList(-1)
, m_nEndIndexInProcList(-1)
{
}

ProcessorFamily::~ProcessorFamily(void)
{
}
int ProcessorFamily::GetStartIndexInProcList(void)
{
	return m_nStartIndexInProcList;
}

int ProcessorFamily::GetEndIndexInProcList(void)
{
	return m_nEndIndexInProcList;
}
void ProcessorFamily::SetStartIndexInProcList( int _nIndex )
{
	m_nStartIndexInProcList = _nIndex;
}

void ProcessorFamily::SetEndIndexInProcList( int _nIndex )
{
	m_nEndIndexInProcList = _nIndex;
}
