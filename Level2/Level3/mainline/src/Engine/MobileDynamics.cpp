#include "StdAfx.h"
#include ".\mobiledynamics.h"
#include "../Engine/Airside/ARCMobileElement.h"
#include <algorithm>

MobileDynamics::MobileDynamics( double _mspd,double _macc, double _mdec )
{
	m_dMaxSpeed = _mspd;
	m_dMaxAcceleration = _macc;
	m_dMaxDeceleration = _mdec;
}

ElapsedTime MobileDynamics::GetCostTime( double _startSpd, double _endSpd, double dDist ) const
{
	ASSERT(m_dMaxSpeed >= _startSpd);
	double  _minDist;
	double dspd1 = m_dMaxSpeed - _startSpd;
	double dspd2 = _endSpd - m_dMaxSpeed;

	double dT1,dT2;
	if(dspd1>0)
	{
		dT1 = dspd1 / m_dMaxAcceleration;
	}
	{
		dT1 = - dspd1 / m_dMaxDeceleration;
	}

	if(dspd2 > 0)
	{
		dT2 = dspd2 / m_dMaxAcceleration;
	}
	else
	{
		dT2 = dspd2 / m_dMaxDeceleration;
	}

	_minDist = 0.5 * (m_dMaxSpeed + _startSpd) * dT1 + 0.5 * (m_dMaxSpeed + _endSpd) * dT2;

	if(_minDist > dDist)
	{
		return dT1 + dT2;
	}
	else
	{
		return (dDist - _minDist)/m_dMaxSpeed + dT1 + dT2; 
	}
}

#include <common/ARCMathCommon.h>
double MobileDynamics::getTimeBetweenRat( double bDistRat, double fSpd, double tSpd )
{
	if(tSpd+fSpd>0)
	{
		double a = (tSpd - fSpd)/(tSpd+fSpd);
		double b = 2.0*(fSpd)/(tSpd+fSpd);	
		double c = -bDistRat;
		double s = b*b - 4.0*a*c;
		if( abs(a)<ARCMath::EPSILON && abs(b) >= ARCMath::EPSILON )
		{
			return -c/b;
		}
		if(s>=0 && a!=0 )
		{
			double r1 = (-b+sqrt(s))*0.5/a;
			double r2 = (-b-sqrt(s))*0.5/a;
			if(r1>=0 && r1<=1.0)
				return r1;
			if(r2>=0 && r2<=1.0)
				return r2;
		}

	}
	return bDistRat;
}

void MobileTravelTrace::BeginUse() /*need to call update items if you want to use this object */
{
	//sort
	std::sort(mvDistSpdPair.begin(),mvDistSpdPair.end());
	//
	if(!mvDistSpdPair.empty())
	{	
		mvDistSpdPair.begin()->mtTime = ElapsedTime(0L);
	}
	for(int i=0;i<getItemCount()-1;++i)
	{
		DistSpeedTime& thisItem = mvDistSpdPair[i];
		DistSpeedTime& nextItem = mvDistSpdPair[i+1];
		nextItem.mtTime = thisItem.timeToNext(nextItem);
	}
}

MobileTravelTrace::MobileTravelTrace( const MobileGroundPerform& perform,const DistanceUnit& dDist,const double& spdF, const double& spdT )
{
	//add first item
	mvDistSpdPair.push_back( DistSpeedTime(0,spdF) ); 

	//add mid items
	DistanceUnit distBeginToNorm = perform.getSpdChangeDist(spdF,perform.getNormalSpd());
	DistanceUnit distNormToEnd = perform.getSpdChangeDist(perform.getNormalSpd(),spdT);
	DistanceUnit distBeginToEnd = perform.getSpdChangeDist(spdF,spdT);
	if( int(dDist) < int(distBeginToEnd) )//dist not enough for speed change, then end speed should change
	{
		double tRat = MobileDynamics::getTimeBetweenRat(dDist/distBeginToEnd,spdF, spdT);
		double dMidSpd = spdF*(1.0-tRat) + spdT*tRat;			

		mvDistSpdPair.push_back( DistSpeedTime(dDist,dMidSpd) );
		return;	
	}
	//add mid items
	double dNormSpd = perform.getNormalSpd();
	if( (distBeginToNorm + distNormToEnd) < (dDist) )
	{
		if(abs(spdF-dNormSpd)>ARCMath::EPSILON){				
			mvDistSpdPair.push_back( DistSpeedTime(distBeginToNorm,dNormSpd) );
		}			
		if(abs(spdT-dNormSpd)>ARCMath::EPSILON)
			mvDistSpdPair.push_back( DistSpeedTime(dDist-distNormToEnd,dNormSpd) );
	}
	else
	{
		//if(spdF <dNormSpd && spdT <ARCMath::EPSILON)
		//{
			/*double dR = dDist*0.5/distBeginToNorm;
			double tRat = MobileDynamics::getTimeBetweenRat(dR, spdF,dNormSpd);
			double dMidSpd = spdF*(1.0-tRat) + dNormSpd*tRat;				
		
		mvDistSpdPair.push_back( DistSpeedTime(dDist*0.5,dMidSpd) );*/
		if( abs(perform.mAccSpd+perform.mDecSpd) > ARCMath::EPSILON)
		{
			double dSpdMid =  (2.0*dDist*perform.mAccSpd*perform.mDecSpd + spdF*spdF*perform.mDecSpd + spdT*spdT*perform.mAccSpd) /(perform.mAccSpd+perform.mDecSpd) ;
			dSpdMid = sqrt(dSpdMid);
			double dR = (dSpdMid*dSpdMid-spdF*spdF)*perform.mDecSpd / ( (dSpdMid*dSpdMid-spdT*spdT)*perform.mAccSpd  + (dSpdMid*dSpdMid-spdF*spdF)*perform.mDecSpd );
			mvDistSpdPair.push_back( DistSpeedTime(dDist*dR,dSpdMid) );
		}
	}		
	mvDistSpdPair.push_back( DistSpeedTime(dDist,spdT) );		
	return;
}

ElapsedTime MobileTravelTrace::DistSpeedTime::timeToNext( const DistSpeedTime& item ) const
{
	double dAvgSpd = (item.mdSpd+mdSpd)*0.5;
	DistanceUnit dDist = item.mdist-mdist;
	ElapsedTime dT = ElapsedTime(0L);
	if(dAvgSpd>ARCMath::EPSILON)
	{
		dT = ElapsedTime(dDist/dAvgSpd);
	}
	return mtTime + dT;
}
