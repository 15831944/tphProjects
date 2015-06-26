#include "StdAfx.h"
#include <algorithm>
#include "ARCEventLog.h"
#include <boost/serialization/vector.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <common\Types.hpp>
#include "./AirsideFlightEventLog.h"
#include "./AirsideFllightRunwayDelayLog.h"
#include <common/exeption.h>
#include "./AirsideFlightNodeLog.h"

DynamicClassImpl(ARCEventLog)

void ARCEventLog::serialize(boost::archive::text_iarchive &ar, const unsigned int file_version)
{
	ar & time;
}

void ARCEventLog::serialize(boost::archive::text_oarchive &ar, const unsigned int file_version)
{
	ar & time;
}

ARCEventLog::~ARCEventLog()
{

}

ARCEventLog::ARCEventLog()
{
	time = 0L;
}
void ARCEventLogList::clear()
{
	for(int i=0;i<(int)mEventList.size();i++)
		delete mEventList.at(i);
	mEventList.clear();
}

void ARCEventLogList::Sort()
{
	std::sort(mEventList.begin(),mEventList.end(),EventTimeSorter());
}


#define REGISTER_SERIALIZE_TYPE(x) ar.register_type(static_cast<x*>(NULL))

void ARCEventLogList::serialize(boost::archive::text_iarchive &ar, const unsigned int file_version)
{	
	if (file_version != EventLogVersion )
		throw new StringError ("Log file out of date, " "rerun simulation");

	
	ar.register_type(static_cast<AirsideFlightEventLog*>(NULL));
	ar.register_type(static_cast<AirsideConflictionDelayLog*>(NULL));
	ar.register_type(static_cast<AirsideFlightRunwayDelayLog*>(NULL));
	ar.register_type(static_cast<AirsideRunwayCrossigsLog*>(NULL));
	ar.register_type(static_cast<AirsideFlightStandOperationLog*>(NULL));
	REGISTER_SERIALIZE_TYPE(FlightStartNodeDelayLog);
	REGISTER_SERIALIZE_TYPE(FlightCrossNodeLog);
	REGISTER_SERIALIZE_TYPE(AirsideTakeoffProcessLog);
	
	ar & mEventList;
}

void ARCEventLogList::serialize(boost::archive::text_oarchive &ar, const unsigned int file_version)
{
	ar.register_type(static_cast<AirsideFlightEventLog *>(NULL));
	ar.register_type(static_cast<AirsideConflictionDelayLog *>(NULL));
	ar.register_type(static_cast<AirsideFlightRunwayDelayLog*>(NULL));
	ar.register_type(static_cast<AirsideRunwayCrossigsLog*>(NULL));
	ar.register_type(static_cast<AirsideFlightStandOperationLog*>(NULL));
	REGISTER_SERIALIZE_TYPE(FlightStartNodeDelayLog);
	REGISTER_SERIALIZE_TYPE(FlightCrossNodeLog);
	REGISTER_SERIALIZE_TYPE(AirsideTakeoffProcessLog);

	ar & mEventList;
}
