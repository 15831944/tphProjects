// PorcLabourCostDateBase.cpp: implementation of the CPorcLabourCostDateBase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ProcLabourCostDataBase.h"
#include <assert.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProcLabourCostDataBase::CProcLabourCostDataBase()
{

}

CProcLabourCostDataBase::~CProcLabourCostDataBase()
{

}

void CProcLabourCostDataBase::AddItem( CProcLabourCost procLabourCost )
{
	m_vProcLabour.push_back( procLabourCost );

}

void CProcLabourCostDataBase::EraseItem( int nIdx )
{
	m_vProcLabour.erase( m_vProcLabour.begin() + nIdx );
}

int CProcLabourCostDataBase::GetSize()
{
	return m_vProcLabour.size();
}

void CProcLabourCostDataBase::Clear()
{
	m_vProcLabour.clear();
}

CProcLabourCost CProcLabourCostDataBase::GetItem( int nIdx )
{
	assert( nIdx >= 0 && nIdx < m_vProcLabour.size() );
	return m_vProcLabour[ nIdx ];
}

void CProcLabourCostDataBase::ModifyItem( CProcLabourCost procLabourCost, int nIdx )
{
	m_vProcLabour[ nIdx ] = procLabourCost;
}

