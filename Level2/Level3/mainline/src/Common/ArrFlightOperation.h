#pragma once
#include "flightoperation.h"

//class ArctermFile;
//class StringDictionary;

class COMMON_TRANSFER CArrFlightOperation :public CFlightOperation
{
public:
	CArrFlightOperation(void);
	~CArrFlightOperation(void);


	void SetArrStand(ALTObjectID arrStand);
	void SetArrFlightID(CFlightID arrFlightID);
	void SetArrFlightID(const char *p_str);
	void SetArrTime(long tTime);

	ALTObjectID GetArrStand();
	CFlightID GetArrFlightID();
	long GetArrTime();

	void SetOriginAirport(CAirportCode origin);
	void SetOriginAirport(const char *p_str);
	CAirportCode GetOriginAirport();


	void SetArrStandIdx(short nIdx);
	short GetArrStandIdx();
};
