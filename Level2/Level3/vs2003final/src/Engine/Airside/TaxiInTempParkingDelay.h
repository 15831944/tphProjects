#pragma once
#include "./AirsideFlightDelay.h"

class TempParkingInSim;
class AirsideFlightInSim;
class ElapsedTime;

/************************************************************************/
//  
/************************************************************************/
class TaxiInTempParkingDelay  : public AirsideFlightDelay
{
public:
	TaxiInTempParkingDelay();
	TaxiInTempParkingDelay( int parkingType , int nFltID, const ElapsedTime& tTime);

	virtual DelayType GetType()const{ return enumTaxiTempParkDelay ; }

	virtual bool IsIdentical(const AirsideFlightDelay* otherDelay)const;

	virtual void WriteDelayReason(ArctermFile& outFile );
	virtual void ReadDelayReason(ArctermFile& inFile);

protected:
	int m_parkingType;		 
};
