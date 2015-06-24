#pragma once
#include "InputAirside/WeatherScripts.h"
#include "InputAirside/AircraftDeicingAndAnti_icingsSettings.h"
class ElapsedTime;

class WeatherInSim
{
public:
	WeatherInSim();
	CWeatherScripts m_weatherScript;

	Precipitationtype getPrecipitaionType(const ElapsedTime& t)const;
};