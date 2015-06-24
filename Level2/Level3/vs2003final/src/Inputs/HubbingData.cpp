// HubbingData.cpp: implementation of the CHubbingData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HubbingData.h"
#include "Probab.h"
#include "fltdist.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHubbingData::CHubbingData()
{
    m_pTransiting = NULL;
    m_pTransferring = NULL;
	m_pTransferDest = NULL;
}

CHubbingData::~CHubbingData()
{
    delete m_pTransiting;
    delete m_pTransferring;
	delete m_pTransferDest;
}


void CHubbingData::WipeIntrinsic()
{
	m_const.setIntrinsicType( 0 );
}



// fill the empty item with default value
void CHubbingData::FillEmptyWithDefault()
{
    if( !m_pTransiting )
		m_pTransiting = new ConstantDistribution( 10 );		
	if( !m_pTransferring )
	    m_pTransferring = new ConstantDistribution( 20 );
	if( !m_pTransferDest )
		m_pTransferDest = new FlightTypeDistribution;
}

//return -1,if not find
//return i,if find 
int CHubbingData::FindBestFlight(const FlightConstraint &_aFlight)
{
	int nCount = m_pTransferDest->getCount();
	for ( int i = 0; i < nCount; i++ )
		if (m_pTransferDest->getGroup(i)->getFlightConstraint()->fits(_aFlight,false))
		{
			return i;
		}

		return -1;
}