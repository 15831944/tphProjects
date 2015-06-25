#pragma once
#include "flightoperationalbaseresult.h"
#include "FlightOperationalReport.h"

class CFlightOperationalAirDistanceResult :
	public CFlightOperationalBaseResult
{
public:
	CFlightOperationalAirDistanceResult(void);
	virtual ~CFlightOperationalAirDistanceResult(void);

public:
	virtual void GenerateResult(vector<FltOperationalItem>& vResult, CParameters *pParameter);
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);
};
