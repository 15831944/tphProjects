#include "StdAfx.h"
#include "point.h"
#include "DynamicMovement.h"
#include "ARCMathCommon.h"

ElapsedTime DynamicMovement::GetDistTime( const DistanceUnit& dist ) const
{
	double Trate = getTrate(dist);
	double dT =0;
	if(m_dStartSpd+m_dEndSpd>ARCMath::EPSILON)
	{
		dT = 2.0*m_travelDist/(m_dStartSpd + m_dEndSpd);
	}
	return ElapsedTime(dT*Trate);
}

double DynamicMovement::GetDistSpeed( const DistanceUnit& dist ) const
{
	double Trate = getTrate(dist);
	return m_dStartSpd*(1.0 - Trate) + m_dEndSpd * Trate;
}

double DynamicMovement::getTrate(const DistanceUnit& dist) const
{
	if(m_travelDist<=0)
	{
		//ASSERT(FALSE);
		return 1;
	}

	double Srate = dist/m_travelDist;
	Srate= MAX(0,Srate);
	Srate = MIN(1,Srate);

	double Trate = 0.5;  //the value we are going to get

	if(m_dStartSpd == m_dEndSpd)
		Trate = Srate;
	else
	{		
		double b = 2.0*m_dStartSpd/(m_dEndSpd - m_dStartSpd);
		double c = -Srate*(m_dStartSpd + m_dEndSpd)/(m_dEndSpd - m_dStartSpd);

		double Trate1 = (-b + sqrt(b*b - 4.0*c))*0.5;
		double Trate2 = (-b - sqrt(b*b - 4.0*c))*0.5;
		
		if(m_dEndSpd>m_dStartSpd)
		{
			Trate1 = MAX(0,Trate1);
			Trate1 = MIN(1,Trate1);
			return Trate1;
		}
		else
		{
			Trate2 = MAX(0,Trate2);
			Trate2 = MIN(1,Trate2);
			return Trate2;
		}
		
	}
	return Trate;
}