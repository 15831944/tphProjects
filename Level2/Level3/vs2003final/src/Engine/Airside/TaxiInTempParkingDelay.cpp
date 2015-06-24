#include "StdAfx.h"
#include "../../Common/path.h"
#include "./TaxiInTempParkingDelay.h"



TaxiInTempParkingDelay::TaxiInTempParkingDelay( int parkingType , int nFltID, const ElapsedTime& tTime )
{
	SetStartTime(tTime);
	SetFlightID(nFltID);
}

TaxiInTempParkingDelay::TaxiInTempParkingDelay()
{

}

bool TaxiInTempParkingDelay::IsIdentical( const AirsideFlightDelay* otherDelay ) const
{
	if(!otherDelay )
		return false;

	if(otherDelay && otherDelay->GetType()!= GetType())
		return false;



	return true;
}

void TaxiInTempParkingDelay::WriteDelayReason( ArctermFile& outFile )
{

}



void TaxiInTempParkingDelay::ReadDelayReason( ArctermFile& inFile )
{
	
}
