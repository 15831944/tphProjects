#include "StdAfx.h"
#include "ARCMobileElement.h"
#include <common\ARCMathCommon.h>

DistanceUnit MobileGroundPerform::getSpdChangeDist( double dStartSpd, double dEndSpd/*=0*/ ) const
{
	ElapsedTime dT = getSpdChangeTime(dStartSpd,dEndSpd);
	return (double)dT*(dEndSpd+dStartSpd)*0.5 ;
}

ElapsedTime MobileGroundPerform::getSpdChangeTime( double dStartSpd, double dEndSpd ) const
{
	if(dStartSpd < dEndSpd && getAccSpd()>ARCMath::EPSILON ) //acc
		return (dEndSpd - dStartSpd)/getAccSpd();
	else if(dEndSpd < dStartSpd && getDecSpd()>ARCMath::EPSILON ) //dec
		return (dStartSpd - dEndSpd)/getDecSpd();
	return ElapsedTime(0L);
}

DistanceUnit MobileGroundPerform::GetDecDist( double dStartSpd, const ElapsedTime& t ) const
{
	ElapsedTime dDecT = getSpdChangeTime(dStartSpd,0);
	double dDecDist = getSpdChangeDist(dStartSpd,0);
	if(t < dDecT) //in the middle of dec
	{
		double tr = t/dDecT;
		double sr = (2-tr)*tr;
		return dDecDist*sr;
	}
	return dDecDist;
}

DistanceUnit MobileGroundPerform::GetAccToNomalDist( double dStartSpd, const ElapsedTime& t ) const
{
	double dEndSpd = getNormalSpd();
	ElapsedTime dAccT = getSpdChangeTime(dStartSpd,dEndSpd );
	double dAccDist = getSpdChangeDist(dStartSpd,dEndSpd);

	if(t<dAccT)
	{
		double tr = t/dAccT;
		double sr = tr;
		if(dStartSpd + dEndSpd >0)
			sr = tr * (dStartSpd*2 + (dEndSpd-dStartSpd)*tr)/(dStartSpd+dEndSpd);
		return dAccDist*sr;
	}
	else if(t>dAccT)
	{
		double dextraDist = (double)(t-dAccT)*dEndSpd;
		return dextraDist + dAccDist;
	}
	return dAccDist;

}

DistanceUnit MobileGroundPerform::minDecDist( double dCurSpeed ) const
{
	return 0.5*dCurSpeed*dCurSpeed/mDecSpd;
}

MobileGroundPerform::MobileGroundPerform()
{
	mMaxBreak = 100*100;
}


double ARCMobileElement::getCurSpdChangeDist( double dEndSpd ) const
{
	return mGroundPerform.getSpdChangeDist(getCurSpeed(),dEndSpd);
}

ElapsedTime ARCMobileElement::getCurSpdChangeTime( double dEndSpd ) const
{
	return mGroundPerform.getSpdChangeTime(getCurSpeed(),dEndSpd);
}
