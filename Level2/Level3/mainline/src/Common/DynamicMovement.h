#pragma once
#include <common/elaptime.h>
class DynamicMovement
{
public:
	
	DynamicMovement(double startSpd, double dEndspd, DistanceUnit travleDist)//m/s
	{
		m_dStartSpd = startSpd;
		m_dEndSpd = dEndspd;
		m_travelDist = travleDist;
	}


	ElapsedTime  GetDistTime(const DistanceUnit& dist)const;
	double  GetDistSpeed(const DistanceUnit& dist)const;
	double getTrate(const DistanceUnit& dist)const;
protected:
	double m_dStartSpd;
	double m_dEndSpd;
	DistanceUnit m_travelDist;
	
};