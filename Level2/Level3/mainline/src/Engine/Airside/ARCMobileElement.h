#pragma once

#include <common/Types.hpp>

#include <common/point.h>

#include "SimAgent.h"
//mobile move on the ground performance
class MobileGroundPerform
{
public:
	DistanceUnit getSpdChangeDist(double dStartSpd, double dEndSpd=0)const;
	ElapsedTime getSpdChangeTime(double dStartSpd, double dEndSpd)const;

	DistanceUnit minDecDist( double dCurSpeed ) const;

	double getDecSpd()const{ return mDecSpd; }
	double getAccSpd()const{ return mAccSpd; }
	double getNormalSpd()const{ return mNoramlSpd; }
	double getMaxSpd()const{ return mMaxSpd; }
	
	//get decelerated dist from spd with time elapsed
	DistanceUnit GetDecDist(double dStartSpd, const ElapsedTime& t)const;
	//get acceleration dist from spd to nomal speed , with time elapsed
	DistanceUnit GetAccToNomalDist(double dStartSpd, const ElapsedTime& t)const;
	MobileGroundPerform();

	
	
	double mDecSpd;
	double mAccSpd;
	double mMaxBreak;
	double mNoramlSpd;
	double mMaxSpd;
};

class RouteDirPath;
class ARCMobileDesc;
class ARCMobileElement : public SimAgent
{
	DynamicClassStatic(ARCMobileElement)
	RegisterInherit(ARCMobileElement,SimAgent)
public:

	//wake up at the time
	virtual void getDesc(ARCMobileDesc& desc)const=0;

	virtual double GetLength()const=0;

	//the performance on route 
	virtual const MobileGroundPerform& getMobilePerform()const{ return mGroundPerform; };
	virtual double getCurSpeed()const=0;	
	virtual ElapsedTime getCurTime()const=0;
	virtual DistanceUnit getCurSeparation()const=0;//the separation other mobile need to keep;
	virtual DistanceUnit getCurDistInDirPath()const=0;
	virtual RouteDirPath* getCurDirPath()const=0;

	virtual double getCurSpdChangeDist(double dEndSpd)const; //get the dist need to change to the speed
	virtual ElapsedTime getCurSpdChangeTime(double dEndSpd)const; //get the time need to change to the end speed;
	MobileGroundPerform mGroundPerform;
};

