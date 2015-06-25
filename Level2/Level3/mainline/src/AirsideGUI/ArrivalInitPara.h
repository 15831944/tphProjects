#pragma once
typedef struct  
{
	CString strAirline;
	CString strArrID;
	CString strArrGate;
	CString strEnroute;
	CString strSTAR;
	CString strRunway;
	long dScheduleTime;
	long dOffsetTime;
	long dSimTime;
} FlightArrivalInit;