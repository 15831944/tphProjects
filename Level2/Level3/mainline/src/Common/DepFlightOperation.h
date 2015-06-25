#pragma once
#include "flightoperation.h"

//class ArctermFile;
//class StringDictionary;

class COMMON_TRANSFER CDepFlightOperation :	public CFlightOperation
{
public:
	CDepFlightOperation(void);
	~CDepFlightOperation(void);

	void SetDepStand(ALTObjectID depStand);
	void SetDepFlightID(CFlightID depFlightID);
	void SetDepFlightID(const char *p_str);
	void SetDepTime(long tTime);

	const ALTObjectID GetDepStand();
	const CFlightID GetDepFlightID();
	const long GetDepTime();

	void SetDestinationAirport(CAirportCode destination);
	void SetDestinationAirport(const char *p_str);
	const CAirportCode GetDestinationAirport();


	void SetDepStandIdx(short nIdx);
	const short GetDepStandIdx();
};
