// FlowBelt.cpp: implementation of the CFlowBelt class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "FlowBelt.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFlowBelt::CFlowBelt()
{

}

CFlowBelt::~CFlowBelt()
{

}

// check if the conveyor is vacant
bool CFlowBelt::isVacant( Person* pPerson ) const
{
	int _iEmptySlotCount = m_iCapacity - m_iOccapuiedCount - m_approaching.getCount();
	assert( _iEmptySlotCount >=0 );
	
	return _iEmptySlotCount > 0;
}