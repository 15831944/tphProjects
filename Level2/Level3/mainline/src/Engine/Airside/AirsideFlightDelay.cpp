#include "StdAfx.h"
#include "../EngineDll.h"
#include ".\airsideflightdelay.h"

#include "../../Common/termfile.h"
#include "../../Common/path.h"
#include "./TaxiInTempParkingDelay.h"
#include "./TaxiwayDirectionLockDelay.h"
#include "FlightConflictAtWaypointDelay.h"
#include "ArrivalDelayTriggersDelay.h"
#include "PushbackClearanceDelay.h"
#include "VehicleServiceDelay.h"


AirsideFlightDelay::AirsideFlightDelay()
{
	m_nFltId	 = -1; 
	m_lUniqueID  = -1;
	m_result = FltDelayReason_Stop;
}
AirsideFlightDelay::~AirsideFlightDelay()
{

}
void AirsideFlightDelay::WriteLog( ArctermFile& outFile )
{
	//int id
	outFile.writeInt(GetUniqueID());
	//delay type
	outFile.writeInt((int) GetType() );
	//who
	outFile.writeInt(GetFlightID());
	//when
	outFile.writeInt((long)GetStartTime());	
	//result
	outFile.writeInt( (long)GetDelayTime() );
	outFile.writeInt( (int)GetResult() );
	//delay reason detail
	WriteDelayReason(outFile);
	//detail string
	outFile.writeField( GetStringDelayReason() );

	outFile.writeLine();
}

AirsideFlightDelay* NewFlightDelay( const AirsideFlightDelay::DelayType& type )
{
	switch(type)
	{
	case AirsideFlightDelay::enumTaxiTempParkDelay : 
		return new TaxiInTempParkingDelay();
		break;
	case AirsideFlightDelay::enumTaxiDirLockDelay :
		return new TaxiwayDirectionLockDelay();
		break;
	case AirsideFlightDelay::enumTaxiLeadFlightDelay :
		return new TaxiLeadFlightDelay();
		break;
	case AirsideFlightDelay::enumTaxiIntersectNodeDelay :
		return new TaxiIntersectNodeDelay();
		break;
	case AirsideFlightDelay::enumArrivalDelayTriggers:
		return new ArrivalDelayTriggersDelay();
		break;
	case AirsideFlightDelay::enumFlightConflictAtWayPoint:
		return new FlightConflictAtWaypointDelay();
		break;
	case AirsideFlightDelay::enumPushbackDelay:
		return new PushbackClearanceDelay();
		break;
	case AirsideFlightDelay::enumVehicleServiceDelay:
		return new VehicleServiceDelay();
		break;
	default :
		break;
	}

	return NULL;
}

const static int MAX_STRBUF_SIZE = 1024;

AirsideFlightDelay* AirsideFlightDelay::ReadLog( ArctermFile& inFile )
{
	long varInt;
	char strbuf[MAX_STRBUF_SIZE];
	//get id
	int uUId;
	inFile.getInteger( uUId );

	//delay type
	inFile.getInteger(varInt);
	AirsideFlightDelay* theDelay = NewFlightDelay((DelayType)varInt);
	if(theDelay) // read data
	{		
		theDelay->SetUniqueID(uUId);
		//who
		inFile.getInteger(varInt);
		theDelay->SetFlightID(varInt);
		////when
		inFile.getInteger(varInt);
		ElapsedTime tTime; tTime.setPrecisely(varInt);
		theDelay->SetStartTime(tTime);		

		//result
		inFile.getInteger(varInt);
		tTime.setPrecisely(varInt);
		theDelay->SetDelayTime(tTime);

		//
		inFile.getInteger(varInt);
		theDelay->SetDelayResult((FltDelayReason)varInt);
		//delay reason
		theDelay->ReadDelayReason(inFile);		
		inFile.getField(strbuf,MAX_STRBUF_SIZE );
		theDelay->SetDetailString(strbuf);
	}
	inFile.getLine();
	return theDelay;
}