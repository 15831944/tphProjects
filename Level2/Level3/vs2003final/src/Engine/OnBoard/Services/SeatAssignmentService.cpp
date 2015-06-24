#include "StdAfx.h"
#include "SeatAssignmentService.h"
#include "../EnplaneAgent.h"
#include "../DeplaneAgent.h"

SeatAssignmentService::SeatAssignmentService()
{
}

SeatAssignmentService::~SeatAssignmentService()
{
	TRACE("Singleton object: seatAssignService() deleted!\n");
}

void SeatAssignmentService::initAssignmentRules()
{

}

void SeatAssignmentService::registerSeat(carrierSeat* pSeat)
{
	mSeatList.addItem( pSeat );

}

void SeatAssignmentService::registerMobAgent(MobileAgent* pMobAgent)
{
	mMobAgentList.addItem( pMobAgent );
	assignSeats( pMobAgent );
}


bool SeatAssignmentService::assignSeats(MobileAgent* pMobAgent)
{
	int nRandomIndex = random( mSeatList.getCount());

	carrierSeat* pSelectedSeat = mSeatList.getItem( nRandomIndex );
	if( NULL == pSelectedSeat->getOccupant())
	{
        pMobAgent->assignSeat( pSelectedSeat );
		pSelectedSeat->setOccupant( pMobAgent );
		return true;
	}
	
	for (int i = 0 ; i < mSeatList.getCount(); i++)
	{
		carrierSeat* pCurSeat = mSeatList.getItem( i );
		if( NULL == pCurSeat->getOccupant())
		{
			pMobAgent->assignSeat( pCurSeat );
			pCurSeat->setOccupant( pMobAgent );
			return true;
		}
	}

	return false;
}


bool SeatAssignmentService::isAllAgentSeatingDown()
{
	for (int i = 0 ; i < mMobAgentList.getCount(); i++)
	{
		MobileAgent* pMobAgent = mMobAgentList.getItem( i );
		if(!pMobAgent->getAssignedSeat()->isOccupied())
			return false;
	}

	return true;
}

bool SeatAssignmentService::isAllAgentLeaveSeats()
{
	for (int i = 0 ; i < mMobAgentList.getCount(); i++)
	{
		MobileAgent* pMobAgent = mMobAgentList.getItem( i );
		if(pMobAgent->getAssignedSeat()->isOccupied())
			return false;
	}

	return true;
}



