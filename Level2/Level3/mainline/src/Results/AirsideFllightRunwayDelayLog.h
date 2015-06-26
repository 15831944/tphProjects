#pragma once
#include "ARCEventLog.h"
#include "../Engine/Airside/CommonInSim.h"
#include "AirsideFlightEventLog.h"

class AirsideFlightRunwayDelayLog :public ARCEventLog
{
	DynamicClass
	RegisterInherit(AirsideFlightRunwayDelayLog,ARCEventLog)
public:
	AirsideFlightRunwayDelayLog();
	AirsideFlightRunwayDelayLog(const ResourceDesc& _runway, long lTime, AirsideMobileElementMode mode, long lAvailableTime, long lDelayTime, FlightRunwayDelay::PositionType _position);
	~AirsideFlightRunwayDelayLog(void);

	bool bTheSameDelay(const ResourceDesc& _resource,const FlightRunwayDelay::PositionType& _position)const;

	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<ARCEventLog>(*this);
		ar & mAtRunway;
		ar & mMode;
		ar & mAvailableTime;
		ar & mDelayTime;
		ar & mPosition;
		ar & sReasonDetail;
	}

public:
	ResourceDesc mAtRunway;
	AirsideMobileElementMode mMode;
	long mAvailableTime;                                                                                                                      
	long mDelayTime;
	FlightRunwayDelay::PositionType mPosition;
	std::string sReasonDetail;
};
