#pragma once
#include "ARCLogItem.h"
#include <string>
#include "../Common/AIRSIDE_BIN.h"

class AirsideFlightLogDesc
{
public:
	AirsideFlightLogDesc();
	int id; //unique id
	std::string sArrID; // arrival id
	std::string sDepID; // departure id
	std::string sAcType; //aircraft type
	std::string sAirline; //airline
	
	std::string _origin;    //the airport before the fromAirport
	std::string _arrStopover;   // originating airport code
	std::string _destination;     //the airport after the toAirport
	std::string _depStopover; // to airport code
	
	float arrloadFactor;
	float deploadFactor;
	int nCapacity;

	long schArrTime; //schedule arrival time
	long schDepTime;//schedule departure time
	double dAprchSpeed; //approach speed ,in kts

	long actAtaTime;//ATA
	long actAtdTime;//ATD
	long actDepOn;

	std::string sSchedArrStand;
	long SchedArrOn;
	long SchedArrOff;

	std::string sSchedIntStand;
	long SchedIntOn;
	long SchedIntOff;

	std::string sSchedDepStand;
	long SchedDepOn;
	long SchedDepOff;


	std::string comment;

	void getFlightDescStruct(AirsideFlightDescStruct& descStruct);

	DECLARE_SERIALIZE
};

class AirsideFlightLogItem : public ARCLogItem
{
public:
	AirsideFlightLogItem(void);
	~AirsideFlightLogItem(void);

	AirsideFlightLogDesc mFltDesc;

	DECLARE_SERIALIZE
};
