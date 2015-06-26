#include "StdAfx.h"
#include ".\airsideflightlogitem.h"
#include <boost/serialization/vector.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <common\exeption.h>
#include "../Common/IDGather.h"

#define  FLIGHTLOGITEM_VERIOSN 1
BOOST_CLASS_VERSION(AirsideFlightLogDesc,FLIGHTLOGITEM_VERIOSN)

void AirsideFlightLogDesc::serialize(boost::archive::text_iarchive &ar, const unsigned int file_version)
{
	if (file_version != FLIGHTLOGITEM_VERIOSN )
		throw new StringError ("Log file out of date, " "rerun simulation");

	ar& id; //unique id
	ar& sArrID; // arrival id
	ar& sDepID; // departure id
	ar& sAcType; //aircraft type
	ar& sAirline; //airline

	ar& _origin;    //the airport before the fromAirport
	ar& _arrStopover;   // originating airport code
	ar& _destination;     //the airport after the toAirport
	ar& _depStopover; // to airport code

	ar& arrloadFactor;
	ar& deploadFactor;
	ar& nCapacity;

	ar& schArrTime; 
	ar& schDepTime;
	ar& dAprchSpeed; //approach speed ,in kts

	ar&actAtaTime;
	ar&actAtdTime;
	ar&actDepOn;

	ar& sSchedArrStand;
	ar& SchedArrOn;
	ar& SchedArrOff;

	ar& sSchedIntStand;
	ar& SchedIntOn;
	ar& SchedIntOff;

	ar& sSchedDepStand;
	ar& SchedDepOn;
	ar& SchedDepOff;

	ar& comment;

}

void AirsideFlightLogDesc::serialize(boost::archive::text_oarchive &ar, const unsigned int file_version)
{
	ar& id; //unique id
	ar& sArrID; // arrival id
	ar& sDepID; // departure id
	ar& sAcType; //aircraft type
	ar& sAirline; //airline

	ar& _origin;    //the airport before the fromAirport
	ar& _arrStopover;   // originating airport code
	ar& _destination;     //the airport after the toAirport
	ar& _depStopover; // to airport code

	ar& arrloadFactor;
	ar& deploadFactor;
	ar& nCapacity;

	ar& schArrTime; 
	ar& schDepTime;
	ar& dAprchSpeed; //approach speed ,in kts

	ar&actAtaTime;
	ar&actAtdTime;
	ar&actDepOn;

	ar& sSchedArrStand;
	ar& SchedArrOn;
	ar& SchedArrOff;

	ar& sSchedIntStand;
	ar& SchedIntOn;
	ar& SchedIntOff;

	ar& sSchedDepStand;
	ar& SchedDepOn;
	ar& SchedDepOff;

	ar& comment;

}

AirsideFlightLogDesc::AirsideFlightLogDesc()
{
	id = -1;
	arrloadFactor = 0;
	deploadFactor = 0;
	nCapacity = 0;
	schArrTime = 0;
	schDepTime = 0;
	dAprchSpeed = 0;
	actAtaTime = -1;
	actAtdTime = -1;
	actDepOn = -1;
	SchedArrOn = 0;
	SchedArrOff = 0;
	SchedIntOn = 0;
	SchedIntOff = 0;
	SchedDepOn = 0;
	SchedDepOff = 0;
	comment = "\n";
}

void AirsideFlightLogDesc::getFlightDescStruct(AirsideFlightDescStruct& descStruct)
{
	descStruct.id = id;
	descStruct._arrID = sArrID.c_str();
	descStruct._depID = sDepID.c_str();
	descStruct._acType = sAcType.c_str();
	descStruct._airline = sAirline.c_str();
	descStruct._origin = _origin.c_str();
	descStruct._arrStopover = _arrStopover.c_str();
	descStruct._destination = _destination.c_str();
	descStruct._depStopover = _depStopover.c_str();
	descStruct.arrloadFactor = arrloadFactor;
	descStruct.deploadFactor = deploadFactor;
	descStruct.nCapacity = nCapacity;
	descStruct.arrTime = schArrTime;
	descStruct.depTime = schDepTime;
	strcpy(descStruct.arrStandPlaned, sSchedArrStand.c_str());
	strcpy(descStruct.intStandPlaned, sSchedIntStand.c_str());
	strcpy(descStruct.depStandPlaned, sSchedDepStand.c_str());
}


//////////////////////////////////////////////////////////////////////////
AirsideFlightLogItem::AirsideFlightLogItem(void)
{
}

AirsideFlightLogItem::~AirsideFlightLogItem(void)
{
}

void AirsideFlightLogItem::serialize(boost::archive::text_iarchive &ar, const unsigned int file_version)
{
	ar & mFltDesc;	
	ARCLogItem::serialize(ar,file_version);

}

void AirsideFlightLogItem::serialize(boost::archive::text_oarchive &ar, const unsigned int file_version)
{
	ar & mFltDesc;	
	ARCLogItem::serialize(ar,file_version);

}
