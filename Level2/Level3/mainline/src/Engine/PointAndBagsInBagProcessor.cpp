// PointAndBagsInBagProcessor.cpp: implementation of the PointAndBagsInBagProcessor class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PointAndBagsInBagProcessor.h"
#include "engine\pax.h"
#include <algorithm>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

PointAndBagsInBagProcessor::PointAndBagsInBagProcessor()
{
	//m_lStepTime = 0;
}

PointAndBagsInBagProcessor::PointAndBagsInBagProcessor( const PointAndBagsInBagProcessor& _another )
{
	m_vBags =  _another.m_vBags;
	m_ptPostion = _another.m_ptPostion;
}

PointAndBagsInBagProcessor::~PointAndBagsInBagProcessor()
{
	m_vBags.clear();
}
void PointAndBagsInBagProcessor::ErasePerson( PersonID _nPersonID )
{
	BAGS::iterator iterFind = std::find( m_vBags.begin(), m_vBags.end(), _nPersonID );
	if( iterFind != m_vBags.end()  )
	{
		m_vBags.erase( iterFind );
	}
}
void PointAndBagsInBagProcessor::PickUpBags( Passenger* _pOwner ,BAGS& _vBags )
{
	if( _pOwner==NULL ) return;

	int iBagsCount = m_vBags.size();
	for( int i=iBagsCount-1; i>=0; --i )
	{
		if( _pOwner->IsMyVisitor( m_vBags[i]  ) )
		{
			_vBags.push_back ( m_vBags[i]  );
			m_vBags.erase( m_vBags.begin() + i );
		}
	}
}