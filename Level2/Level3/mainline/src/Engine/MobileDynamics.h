#pragma once
#include <Common/elaptime.h>
#include <common/ARCMathCommon.h>

class MobileDynamics
{

public:
	MobileDynamics(double _mspd,double _macc, double _mdec);

	ElapsedTime GetCostTime(double _startSpd, double _endSpd, double dDist )const;

	static double getTimeBetweenRat(double bDistRat, double fSpd, double tSpd);

protected:
	double m_dMaxSpeed;
	double m_dMaxAcceleration;
	double m_dMaxDeceleration;
	

};


//the movements of the mobile takes
class MobileGroundPerform;
class MobileTravelTrace
{
public:
	struct DistSpeedTime
	{
		DistSpeedTime(DistanceUnit dist,double dspd){
			mdist = dist; mdSpd = dspd;
		}
		DistanceUnit mdist;
		double mdSpd;
		ElapsedTime mtTime; //offset time
		bool operator <(const DistSpeedTime& other)const{
			return mdist<other.mdist;
		}	
		ElapsedTime timeToNext(const DistSpeedTime& item)const;
	};

	std::vector<DistSpeedTime> mvDistSpdPair;

	MobileTravelTrace(){}
	MobileTravelTrace(const MobileGroundPerform& perform,const DistanceUnit& dDist,const double& spdF, const double& spdT );

	//update the items time
	void BeginUse(); //need to call update items if you want to use this object;

	void addDistItem(DistanceUnit dist) //add new interpolate item
	{	
		mvDistSpdPair.push_back( DistSpeedTime(dist, getDistSpeed(dist)) );		
	}

	void addDistItem(DistanceUnit dist,double dspd) //add new interpolate item
	{	
		mvDistSpdPair.push_back( DistSpeedTime(dist, dspd) );		
	}

	void removeDistItem(DistanceUnit distF,DistanceUnit distT);
	void removeMidItems()
	{
		if(mvDistSpdPair.size()>2)
			mvDistSpdPair.erase(mvDistSpdPair.begin()+1,mvDistSpdPair.end()-1);
	}

	double getDistSpeed(DistanceUnit dist)const//return offset dist the speed
	{
		if(dist<=0)
			return mvDistSpdPair[0].mdSpd;
		for(int i=0;i<getItemCount()-1;++i)
		{
			const DistSpeedTime& thisItem = mvDistSpdPair[i];
			const DistSpeedTime& nextItem = mvDistSpdPair[i+1];
			if(thisItem.mdist < dist && dist <= nextItem.mdist )
			{
				double dtRat = MobileDynamics::getTimeBetweenRat((dist-thisItem.mdist)/(nextItem.mdist-thisItem.mdist), thisItem.mdSpd,nextItem.mdSpd);
				return thisItem.mdSpd*(1.0-dtRat) + nextItem.mdSpd*dtRat;
			}
		}
		return mvDistSpdPair.rbegin()->mdSpd;
	}
	ElapsedTime getDistTime(DistanceUnit dist)const//return offset dist the offset time
	{
		if(dist<=0)
			return mvDistSpdPair[0].mtTime;
		for(int i=0;i<getItemCount()-1;++i)
		{
			const DistSpeedTime& thisItem = mvDistSpdPair[i];
			const DistSpeedTime& nextItem = mvDistSpdPair[i+1];
			if(thisItem.mdist < dist && dist <= nextItem.mdist )
			{
				double dtRat = MobileDynamics::getTimeBetweenRat((dist-thisItem.mdist)/(nextItem.mdist-thisItem.mdist), thisItem.mdSpd,nextItem.mdSpd);
				return ElapsedTime((double)thisItem.mtTime*(1.0-dtRat) + (double)nextItem.mtTime*dtRat);
			}
		}
		return mvDistSpdPair.rbegin()->mtTime;
	}

	double getTimeSpeed(const ElapsedTime& time)const
	{
		if(mvDistSpdPair.empty())return 0;

		if(time<mvDistSpdPair.begin()->mtTime)return mvDistSpdPair.begin()->mdSpd;
		for(int i=0;i<getItemCount()-1;++i)
		{
			const DistSpeedTime& thisItem = mvDistSpdPair[i];
			const DistSpeedTime& nextItem = mvDistSpdPair[i+1];
			if(thisItem.mtTime<time && time<= nextItem.mtTime)
			{
				double dtRat = (time - thisItem.mtTime) / (nextItem.mtTime-thisItem.mtTime);
				return thisItem.mdSpd*(1.0-dtRat) + nextItem.mdSpd*dtRat;
			}
		}	
		return mvDistSpdPair.rbegin()->mdSpd;
	}
	ElapsedTime getEndTime()const
	{
		if(!mvDistSpdPair.empty())
			return mvDistSpdPair.rbegin()->mtTime;
		return ElapsedTime(0L);
	}

	int getItemCount()const{ return (int)mvDistSpdPair.size(); }
	DistSpeedTime& ItemAt(int idx){ return mvDistSpdPair[idx]; }
	const DistSpeedTime& ItemAt(int idx)const{ return mvDistSpdPair[idx]; } 
};

