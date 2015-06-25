#pragma once
#include "flightoperationalbaseresult.h"
#include "FlightOperationalReport.h"

class CFlightOperationalGroundDistanceResult :
	public CFlightOperationalBaseResult
{
public:
	CFlightOperationalGroundDistanceResult(void);
	virtual ~CFlightOperationalGroundDistanceResult(void);

public:
	virtual void GenerateResult(vector<FltOperationalItem>& vResult, CParameters *pParameter);
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);
};
