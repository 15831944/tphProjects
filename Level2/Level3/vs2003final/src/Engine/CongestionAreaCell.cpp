#include "StdAfx.h"
#include ".\congestionareacell.h"
#include "stdafx.h"

CCongestionAreaCell::CCongestionAreaCell(void)
{
	m_PersonsCount = 0;
}

CCongestionAreaCell::~CCongestionAreaCell(void)
{
	m_PersonsCount = 0;
}

void CCongestionAreaCell::LeaveCell( int nPaxCount) 
{ 
	if(m_PersonsCount > 0)
		m_PersonsCount -= nPaxCount;
	if (m_PersonsCount < 0)
	{	
		m_PersonsCount = 0;
	}
//	ASSERT(0 <= --m_PersonsCount);
}	//pop a person;

