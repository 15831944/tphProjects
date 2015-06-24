#include "StdAfx.h"
#include ".\idmodel.h"
#include "LandsideVehicleInSim.h"

static double V0_INIT_KMH = 120;  //
static double A_INIT_CAR_MSII = 0.5;  //m/s^2
static double B_INIT_MSII = 3.0;  //m/s^2
static double S0_INIT_M = 3;  //m
static double T_INIT_S = 0.5;  //sec
static double MAX_BRAKING = 20.0*100; // m/s^2;
//performance
IDModel::IDModel()
{
	v0=V0_INIT_KMH*100/3.6; //cm/s
	delta=4.0;
	acc=A_INIT_CAR_MSII*100;  //
	dec=B_INIT_MSII*100;  //
	gap=S0_INIT_M*100;
	s1 = 0;
	T =T_INIT_S;  //1.5	

	sqrtaccdec = sqrt(acc*dec);
	//initialize();	
}

double IDModel::_calcAcc( double s, double vhead, double vthis )const
{
	/*if(s<=0)
		return -b;*/

	double delta_v=vthis-vhead;		
	double vel = vthis;
	//double sqrtab  = sqrt(a*b);

	double s_star_raw = gap + s1*sqrt(vel/v0)
		+vel*T + (vel*delta_v)/(2*sqrtaccdec);
	double s_star = (s_star_raw > gap) ? s_star_raw : gap;
	double _acc = acc * (1 - pow((vel/v0),delta) -(s_star*s_star)/(s*s) );
	//System.out.println("s1="+s1);
	if(_acc< -MAX_BRAKING)
		_acc= -MAX_BRAKING;
	return _acc;
}

double IDModel::calAcc( LandsideVehicleInSim* pBehind, LandsideVehicleInSim* pAhead )const
{
	double dDist =  pAhead->getLastState().distInRes - pBehind->getLastState().distInRes - pBehind->GetLength();
	return _calcAcc(dDist, pAhead->getLastState().dSpeed, pBehind->getLastState().dSpeed);
}

//void IDModel::initialize()
//{
//	double dt=0.5;             // relaxation with timestep=0.5 s
//	double kmax=20;            // number of iterations in rlaxation
//	veqTab[0]=0.0;
//	for (int is=1; is<=ismax ; is++){     // table in steps of ds=1 (m)
//
//		double Ve=veqTab[is-1];
//		// if(is>=ismax-3){System.out.println("is="+is+" Ve="+Ve);}
//
//		for (int k=0; k<kmax; k++){
//			double s_star = s0 + s1*sqrt(Ve/v0)+Ve*T;	    
//			double acc=
//				a * (1 - pow((Ve/v0),delta) - (s_star*s_star)/(is*is) );
//			Ve=Ve+acc*dt;
//			if (Ve<0.0){Ve=0.0;}
//		}
//		veqTab[is]=Ve;
//	}
//}

//double IDModel::Veq( double dx )
//{
//	int is = (int) dx;
//	double V=0.0;
//	if (is<ismax){
//		double rest=dx-((double) is);
//		V = (1-rest)*veqTab[is] + rest*veqTab[is+1];
//	}
//	if (is>=ismax){ V=veqTab[ismax];}
//	if (is<=0){V=0.0;}
//	return V;
//}
//////////////////////////////////////////////////////////////////////////
static const boolean EUR_RULES      = true;
static const int     VC_MS = 16;    // crit. velocity where Europ rules 
// kick in (in m/s)

static const double MAIN_BSAVE = 12.;  
static const double MAIN_SMIN = 2.; 
static const double MAIN_BSAVE_SELF = 12.0;

static const double RMP_BSAVE     = 20.; // high to force merging
static const double RMP_SMIN      = 2.;  // min. safe distance
static const double RMP_BIAS_LEFT= 8.;  // high to force merging (8)

static const double LANECL_BSAVE      = 12.; // high to force merging
static const double LANECL_BIAS_RIGHT = 0.7; // negative to force merging
// to the left

static const double BIAS_RIGHT_CAR = 0.1; // right-lane bias  
static const double BIAS_RIGHT_TRUCK = 0.3; 
static const double BIAS_RIGHT_CAR3 = 2; // right-lane bias  
static const double BIAS_RIGHT_TRUCK3 = 1; 
static const double P_FACTOR_CAR = 0.2;   // politeness factor
static const double P_FACTOR_TRUCK = 0.2; 
static const double DB_CAR = 0.3;   // changing thresholds (m/s^2)
static const double DB_TRUCK = 0.2; 
static const double DB_CAR3 = 0.3; // 0.3;  
static const double DB_TRUCK3 = 0.2; // 0.2; //?!! aus irgendeinen Grund

CCarChangeLaneModel::CCarChangeLaneModel()
{
	p=P_FACTOR_CAR; // politeness factor
	db=DB_CAR*100;      // changing threshold
	gapMin=MAIN_SMIN*100;       // minimum safe (net) distance
	bsave=MAIN_BSAVE*100;     // maximum safe braking deceleration
	bsaveSelf = MAIN_BSAVE_SELF*100;

	// asymmetric European parts

	//private double biasRight = BIAS_RIGHT_CAR; //bias (m/s^2) to drive right ln
	biasRight = 0; // overwritten
}

#define  MAX_ACC 10000

bool CCarChangeLaneModel::bChangOk(LandsideVehicleInSim* me, bool bLeft, LandsideVehicleInSim* fOld, 
							  double dNewDistInChangeLane, double dLaneLen, LandsideVehicleInSim* fNew, LandsideVehicleInSim* bNew)
							  
{
	
	//{//check the gap with front vehicle in new lane
	//	double gapFront;
	//	if(fNew)
	//	{ 
	//		gapFront = fNew->getLastState().distInRes - dNewDistInChangeLane - me->GetHalfLength() - fNew->GetHalfLength();
	//	}
	//	else
	//	{
	//		gapFront = dLaneLen - dNewDistInChangeLane - me->GetHalfLength();
	//	}
	//	if(gapFront <= gapMin )
	//		return false;
	//}




	double bNew_acc = MAX_ACC;
	//behind acc differentiation
	if(bNew)
	{
		DistanceUnit sep = dNewDistInChangeLane - bNew->getLastState().distInRes - me->GetHalfLength() - bNew->GetHalfLength(); 
		if(sep <= gapMin )
			return false;
		
		bNew_acc = bNew->getIDModel()._calcAcc(sep, me->getLastState().dSpeed, bNew->getLastState().dSpeed );		
		if(bNew_acc < -bsave)
			return false;
	}

	double bNew_accold = MAX_ACC;
	if(bNew && fNew)
	{
		DistanceUnit sep = fNew->getLastState().distInRes - bNew->getLastState().distInRes - fNew->GetHalfLength() - bNew->GetHalfLength();
		bNew->getIDModel()._calcAcc(sep, fNew->getLastState().dSpeed, bNew->getLastState().dSpeed);
	}

	//my acc differentiation
	double my_acc = MAX_ACC;
	if(fNew)
	{
		DistanceUnit sep = fNew->getLastState().distInRes - dNewDistInChangeLane - me->GetHalfLength() - fNew->GetHalfLength();
		if(sep <= gapMin) 
			return false;
		my_acc = me->getIDModel()._calcAcc(sep, fNew->getLastState().dSpeed, me->getLastState().dSpeed);
		if(my_acc < - bsaveSelf)
			return false;
	}	
	//
	double my_accold = MAX_ACC;
	if(fOld)
	{ 
		DistanceUnit sep = fOld->getLastState().distInRes - me->getLastState().distInRes - me->GetHalfLength() - fOld->GetHalfLength();
		my_accold = me->getIDModel()._calcAcc(sep, fOld->getLastState().dSpeed, me->getLastState().dSpeed);
	}

	

	double my_adv = my_acc - my_accold	+ (bLeft?1:-1)*biasRight;

	double others_disabv = bNew_accold - bNew_acc;
	if(others_disabv < 0)
		others_disabv = 0;

	return (my_adv - p * others_disabv)> db ;
}