#pragma once
#include "CarFollowingModel.h"
class LandsideVehicleInSim;

class IDModel : public CCarFollowingModel
{
public:
	double v0;   //  cm/s  desired speed
	double acc;   //acceleration  cm/s^2
	double dec;  //deceleration  cm/s^2
	double gap; //Minimum gap  s0  cm
	double T;   //seconds	Time headway?T

	double s1; //no useful default set to 0
	double delta;   //constant 4


	double sqrtaccdec;

	/*double decDist(double v)const
	{
		return v*v*0.5/b;
	}
	double decDist(double v, double t)const
	{
		if( v/b > t)
		{
			return v*t - 0.5*b*t*t;
		}
		return decDist(v);
	}
	double decEndSpd(double v, double t)const
	{
		if(v/b > t)
		{
			return v - b*t;
		}
		return 0;
	}
	double accDist(double v)const
	{
		return (v0*v0 - v*v)*0.5/a;
	}

	double accDist(double v, double t)const
	{
		if( (v0- v)/a> t)
		{
			return v*t + 0.5*a*t*t;
		}
		return accDist(v) + v0*( t- (v0-v)/a );
	}
	double accEndSpd(double v, double t)const
	{
		if( (v0- v)/a> t)
		{
			return v+ a*t;
		}
		return v0;
	}	*/
	

	//const static int ismax=100;          // ve(s)=ve(ismax) for s>ismax assumed
	//double veqTab[ismax+1]; // table in steps of ds=1m

	IDModel();
	// calculate table of equilibrium velocity (s) with relaxation method;
	// veqTab[i] = equilibrium velocity for s=i*ds with ds=1 m, i=0..100
	//void initialize();

	// function for equilibrium velocity using above table; ve(s>ismax)=ve(ismax)
	//double Veq(double dx);

	//s : separation dist  cm
	//vhead: head vehicle speed  cm/s
	//vthis: this vehicle speed  cm/s
	double _calcAcc(double s, double vhead, double vthis)const;

	double calAcc(LandsideVehicleInSim* pBehind, LandsideVehicleInSim* pAhead)const;
};


//////////////////////////////////////////////////////////////////////////
class LandsideVehicleInSim;
class CCarChangeLaneModel
{
public:
	CCarChangeLaneModel();

	bool bChangOk(LandsideVehicleInSim* pVehicle, bool bLeft, LandsideVehicleInSim* pOldFrontVehicle, 
		double dNewDistInChangeLane, double dLaneLen,LandsideVehicleInSim* pNewFrontVeh, LandsideVehicleInSim* pNewBackVeh);

protected:
	double p; // politeness factor
	double db;      // changing threshold
	double gapMin;       // minimum safe (net) distance
	double bsave;     // maximum safe braking deceleration
	double bsaveSelf;

	// asymmetric European parts

	//private double biasRight = BIAS_RIGHT_CAR; //bias (m/s^2) to drive right ln
	double biasRight; // overwritten
};