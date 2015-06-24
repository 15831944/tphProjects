#include "StdAfx.h"
#include ".\flightperformancemanager.h"
#include "common\elaptime.h"
#include "../../Inputs/flight.h"
#include "../../Common/ARCUnit.h"
#include "./Common/AirportDatabase.h"
#include "AirsideFlightInSim.h"
#include "../../InputAirside/TowOperationPerformance.h"
#include <time.h>
#include <stdlib.h>
#include "../../InputAirside/StandService.h"
#include "../../Common/ProDistrubutionData.h"
#include "../../Common/ProbabilityDistribution.h"

using namespace ns_AirsideInput;
FlightPerformanceManager::FlightPerformanceManager()
{
	 m_pvFlightPerformanceCruise = NULL;
	 m_pvFlightPerformanceTerminal = NULL;
	 m_pPerformLandings = NULL;
	 m_pDepClimbOuts = NULL;
	 m_pPerformTakeOffs = NULL;
	 m_pApproachLands = NULL;
	 m_pTaxiInbound = NULL;
	 m_pTaxiOutbound = NULL;
	 m_pTowingPerform = NULL;
	 m_pStandServiceList = NULL;
	 m_pEngineStartList = NULL;
}

FlightPerformanceManager::~FlightPerformanceManager()
{
	delete m_pvFlightPerformanceCruise;
	delete m_pvFlightPerformanceTerminal;
	delete m_pPerformLandings;
	delete m_pDepClimbOuts;
	delete m_pPerformTakeOffs;
	delete m_pApproachLands;
	delete m_pTaxiInbound;
	delete m_pTaxiOutbound;
	delete m_pTowingPerform;
	if (m_pStandServiceList)
	{
		delete m_pStandServiceList;
		m_pStandServiceList = NULL;
	}

	if (m_pEngineStartList)
	{
		delete m_pEngineStartList;
		m_pEngineStartList = NULL;
	}
}

bool FlightPerformanceManager::Init(int nPrjID, CAirportDatabase* pAirportDB)
{
	m_pvFlightPerformanceCruise = new vFlightPerformanceCruise(pAirportDB);
	m_pvFlightPerformanceCruise->ReadData(nPrjID);
	m_pvFlightPerformanceCruise->SortFlightType();

	m_pvFlightPerformanceTerminal = new vFlightPerformanceTerminal(pAirportDB);
	m_pvFlightPerformanceTerminal->ReadData(nPrjID);
	m_pvFlightPerformanceTerminal->SortFlightType();

	m_pPerformLandings = new PerformLandings(pAirportDB);
	m_pPerformLandings->ReadData(nPrjID);
	m_pPerformLandings->SortFlightType();

	m_pDepClimbOuts = new DepClimbOuts(pAirportDB);
	m_pDepClimbOuts->ReadData(nPrjID);
	m_pDepClimbOuts->SortFlightType();

	m_pPerformTakeOffs = new PerformTakeOffs(pAirportDB);
	m_pPerformTakeOffs->ReadData(nPrjID);
	m_pPerformTakeOffs->SortFlightType();

	m_pApproachLands = new ApproachLands(pAirportDB);
	m_pApproachLands->ReadData(nPrjID);
	m_pApproachLands->SortFlightType();

	m_pTaxiInbound = new CTaxiInbound(pAirportDB);
	m_pTaxiInbound->ReadData(nPrjID);
	m_pTaxiInbound->GetTaxiInboundNECList()->SortFlightType();

	m_pTaxiOutbound = new CTaxiOutbound(pAirportDB);
	m_pTaxiOutbound->ReadData(nPrjID);
	m_pTaxiOutbound->SortFlightType();

	m_pTowingPerform = new TowOperationPerformanceList(pAirportDB);
	m_pTowingPerform->ReadData(nPrjID);
	m_pTowingPerform->SortFlightType();

	m_pStandServiceList = new StandServiceList(pAirportDB);
	m_pStandServiceList->ReadData(nPrjID);
	m_pStandServiceList->SortFlightType();

	m_pEngineStartList = new CEngineParametersList(pAirportDB);
	m_pEngineStartList->ReadData(nPrjID);

	return true;

}

CFlightPerformanceCruise* FlightPerformanceManager::GetFlightPerformanceCruiseData(AirsideFlightInSim *pflight)const
{
	int nCount = static_cast<int>(m_pvFlightPerformanceCruise->size());
	for (int i = 0; i< nCount; i++)
	{
		CFlightPerformanceCruise *pCruise =(*m_pvFlightPerformanceCruise)[i];
		FlightConstraint& fltCnst = pCruise->GetFltType();
		if(pflight->fits(fltCnst))
			return pCruise;
	}
	return NULL;
}

CFlightPerformanceTerminal* FlightPerformanceManager::GetFlightPerformanceTerminalData(AirsideFlightInSim *pflight)const
{
	int nCount = static_cast<int>(m_pvFlightPerformanceTerminal->size());
	for (int i = 0; i< nCount; i++)
	{
		CFlightPerformanceTerminal *pTerminal =(*m_pvFlightPerformanceTerminal)[i];
		FlightConstraint& fltCnst = pTerminal->GetFltType();
		if(pflight->fits(fltCnst))
			return pTerminal;
	}
	return NULL;
}

PerformLanding* FlightPerformanceManager::GetPerformLandingData(AirsideFlightInSim *pflight)const
{
	std::vector<PerformLanding>& vPerfLands = m_pPerformLandings->GetPerformLandings();
	size_t nCount = vPerfLands.size();
	for (size_t i=0; i<nCount; i++)
	{
		PerformLanding& perfLand = vPerfLands.at(i);
		FlightConstraint& fltCnst = perfLand.m_fltType;
		if(pflight->fits(fltCnst))
			return &perfLand;
	}
	return NULL;
}

DepClimbOut* FlightPerformanceManager::GetDepClimbOutData(AirsideFlightInSim *pflight)const
{
	std::vector<DepClimbOut>& vDepClimbs = m_pDepClimbOuts->GetDepClimbOuts();
	size_t nClimbOutCount = vDepClimbs.size();
	for (size_t i = 0; i < nClimbOutCount; i++)
	{
		DepClimbOut& depClimbOut = vDepClimbs.at(i);
		FlightConstraint& fltCnst = depClimbOut.m_fltType;
		if(pflight->fits(fltCnst))
			return &depClimbOut;
	}
	return NULL;
}

PerformTakeOff* FlightPerformanceManager::GetPerformTakeOffData(AirsideFlightInSim *pflight)const
{
	std::vector<PerformTakeOff>& vTakeoff = m_pPerformTakeOffs->GetTakeoffs();
	size_t nTakeoffCount = vTakeoff.size();
	for (size_t i = 0; i < nTakeoffCount; i++)
	{
		PerformTakeOff& tf = vTakeoff.at(i);
		FlightConstraint& fltCnst = tf.m_fltType;
		if(pflight->fits(fltCnst))
			return &tf;
	}
	return NULL;
}

ApproachLand* FlightPerformanceManager::GetApproachLandData(AirsideFlightInSim *pflight)const
{
	std::vector<ApproachLand>& vAppLoads = m_pApproachLands->GetApproachLands();
	size_t nCount = vAppLoads.size();
	for (size_t i=0; i<nCount; i++)
	{
		ApproachLand& appLand = vAppLoads.at(i);	
		FlightConstraint& fltCnst = appLand.m_fltType;
		if(pflight->fits(fltCnst))
			return &appLand;
	}
	return NULL;
}

CTaxiInboundNEC* FlightPerformanceManager::GetTaxiInboundNECData(const AirsideFlightInSim *pflight)const
{
	CTaxiInboundNECManager* pManager = m_pTaxiInbound->GetTaxiInboundNECList();
	for (int n = 0; n < pManager->GetCount(); n++)
	{
		CTaxiInboundNEC* pTaxiInboundNEC = pManager->GetRecordByIdx(n);
		FlightConstraint& fltCnst = pTaxiInboundNEC->GetFltType();
		if(pflight->fits(fltCnst))
			return pTaxiInboundNEC;
	}
	return NULL;
}

double FlightPerformanceManager::getTaxiInboundIntersectionBuffer(AirsideFlightInSim* pFlight) const
{
	double dIntersectionBuffer = 50.0;
	CTaxiInboundNEC* pTaxiInboundNEC = GetTaxiInboundNECData(pFlight);
	if(pTaxiInboundNEC)
	{
		dIntersectionBuffer = (double)(pTaxiInboundNEC->GetIntersectionBuffer());
	}
		
	return ARCUnit::ConvertLength(dIntersectionBuffer, ARCUnit::M, ARCUnit::CM);
}

double FlightPerformanceManager::getTaxiOutboundIntersectionBuffer(AirsideFlightInSim* pFlight) const
{
	double dIntersectionBuffer = 50.0;
	CTaxiOutboundNEC* pTaxiOutboundNEC = GetTaxiOutboundNECData(pFlight);
	if(pTaxiOutboundNEC)
	{
		dIntersectionBuffer = (double)(pTaxiOutboundNEC->GetIntersectionBuffer());
	}

	return ARCUnit::ConvertLength(dIntersectionBuffer, ARCUnit::M, ARCUnit::CM);
}

CTaxiOutboundNEC* FlightPerformanceManager::GetTaxiOutboundNECData(const AirsideFlightInSim *pflight)const
{
	int nCount = m_pTaxiOutbound->GetCount();
	for (int n = 0; n < nCount; n++)
	{
		CTaxiOutboundNEC* pTaxiOutboundNEC = m_pTaxiOutbound->GetRecordByIdx(n);
		FlightConstraint& fltCnst = pTaxiOutboundNEC->GetFltType();
		if(pflight->fits(fltCnst))
			return pTaxiOutboundNEC;
	}	
	return NULL;
}

//get takeoff position time
double FlightPerformanceManager::getTakeoffPositionTime(AirsideFlightInSim *pflight) const
{
	ASSERT(pflight);

	double dPositionTime = 0.0;
	PerformTakeOff* pTakeOff = GetPerformTakeOffData(pflight);
	if(pTakeOff)
	{
		int nMinTime = (int)pTakeOff->m_nMinPositionTime;
		int nMaxTime = (int)pTakeOff->m_nMaxPositionTime;

		if (nMinTime == nMaxTime)
		{
			dPositionTime = nMinTime;
		}
		else
		{
			dPositionTime = double(nMinTime + rand()%(nMaxTime-nMinTime));
		}
	}

	return dPositionTime;
}
double FlightPerformanceManager::getMinLiftOff(AirsideFlightInSim* pflight)
{
	ASSERT(pflight);

	double dLiftoffSpeed = 150.0;
	PerformTakeOff* pTakeOff = GetPerformTakeOffData(pflight);
	if(pTakeOff)
		dLiftoffSpeed = pTakeOff->m_nMinLiftOff;

	return ARCUnit::ConvertVelocity(dLiftoffSpeed,ARCUnit::KNOT,ARCUnit::CMpS);
}

double FlightPerformanceManager::getMaxLiftOff(AirsideFlightInSim* pflight)
{
	ASSERT(pflight);

	double dLiftoffSpeed = 200.0;
	PerformTakeOff* pTakeOff = GetPerformTakeOffData(pflight);
	if(pTakeOff)
		dLiftoffSpeed = pTakeOff->m_nMaxLiftOff;

	return ARCUnit::ConvertVelocity(dLiftoffSpeed,ARCUnit::KNOT,ARCUnit::CMpS);
}
//get lift off speed
double FlightPerformanceManager::getLiftoffSpeed(AirsideFlightInSim *pflight) const
{
	ASSERT(pflight);

	double dLiftoffSpeed = 0.0;
	PerformTakeOff* pTakeOff = GetPerformTakeOffData(pflight);
	if(pTakeOff)
	{
		double dMinLiftOffSpeed = pTakeOff->m_nMinLiftOff;
		double dMaxLiftOffSpeed = pTakeOff->m_nMaxLiftOff;

		long lMinLiftOffSpeed = (long)(dMinLiftOffSpeed);
		long lMaxLiftOffSpeed = (long)(dMaxLiftOffSpeed);

		if (lMinLiftOffSpeed == lMaxLiftOffSpeed)
		{
			dLiftoffSpeed = lMinLiftOffSpeed;
		}
		else
		{
			dLiftoffSpeed = (double)(lMinLiftOffSpeed + (lMaxLiftOffSpeed-lMinLiftOffSpeed)*pflight->GetRandomValue());
		}		
	}
	return ARCUnit::ConvertVelocity(dLiftoffSpeed,ARCUnit::KNOT,ARCUnit::CMpS);

}

//get take off roll
double FlightPerformanceManager::getTakeoffRoll(AirsideFlightInSim *pflight) const
{
	ASSERT(pflight);

	double dTakeOffRoll = 0.0;
	PerformTakeOff* pTakeOff = GetPerformTakeOffData(pflight);
	if(pTakeOff)
	{
		double dMinTakeOffRoll = pTakeOff->m_minTakeOffRoll;
		double dMaxTakeOffRoll = pTakeOff->m_maxTakeOffRoll;
		
		dTakeOffRoll = dMinTakeOffRoll + (dMaxTakeOffRoll - dMinTakeOffRoll)* pflight->GetRandomValue();
	}

	return ARCUnit::ConvertLength(dTakeOffRoll,ARCUnit::M,ARCUnit::CM);
}

//get min take off roll
double FlightPerformanceManager::getMinTakeoffRoll(AirsideFlightInSim* pflight)
{
	ASSERT(pflight);

	double dTakeOffRoll = 0.0;
	PerformTakeOff* pTakeOff = GetPerformTakeOffData(pflight);
	if(pTakeOff)
	{
		dTakeOffRoll = pTakeOff->m_minTakeOffRoll;
	}

	return ARCUnit::ConvertLength(dTakeOffRoll,ARCUnit::M,ARCUnit::CM);
}
//get landing speed  
double FlightPerformanceManager::getLandingSpeed(AirsideFlightInSim *pflight)const
{
	double defSpeed = 100;  //knot
	PerformLanding* pLanding = GetPerformLandingData(pflight);
	if(pLanding)
		defSpeed = pLanding->m_dMinTouchDownSpeed+(pLanding->m_dMaxTouchDownSpeed-pLanding->m_dMinTouchDownSpeed)*pflight->GetRandomValue();
	//change to cm/s
	return ARCUnit::ConvertVelocity(defSpeed,ARCUnit::KNOT,ARCUnit::CMpS);
}

double FlightPerformanceManager::getMaxLandingSpeed(AirsideFlightInSim *pflight)const
{
	double defSpeed = 100;  //knot
	PerformLanding* pLanding = GetPerformLandingData(pflight);
	if(pLanding)
		defSpeed = pLanding->m_dMaxTouchDownSpeed;
	//change to cm/s
	return ARCUnit::ConvertVelocity(defSpeed,ARCUnit::KNOT,ARCUnit::CMpS);
}


//deceleration speed
double FlightPerformanceManager::getDecelSpeed(AirsideFlightInSim *pflight)const
{
	double defDecel = 10;
	PerformLanding* pLanding = GetPerformLandingData(pflight);
	if(pLanding)
		defDecel = pLanding->m_dNormalDeceleration+(pLanding->m_dMaxDeceleration-pLanding->m_dNormalDeceleration)*pflight->GetRandomValue();
	return ARCUnit::ConvertVelocity(defDecel,ARCUnit::KNOT,ARCUnit::CMpS);
}

// get distance from threshold to touchdown
double FlightPerformanceManager::getDistThresToTouch(AirsideFlightInSim *pflight)const
{
	double defDist = 100;  //meters
	PerformLanding* pLanding = GetPerformLandingData(pflight);
	if(pLanding)
		defDist = pLanding->m_dMinTouchDownDist+(pLanding->m_dMaxTouchDownDist-pLanding->m_dMinTouchDownDist)*pflight->GetRandomValue();
	return ARCUnit::ConvertLength(defDist,ARCUnit::M,ARCUnit::CM);
}

//takeoff rolling distance
double FlightPerformanceManager::getDistEndToTakeoff(AirsideFlightInSim *pflight )const
{
	double defDist = 400;  //meters
	PerformTakeOff* pTakeOff = GetPerformTakeOffData(pflight);
	if(pTakeOff)
		defDist = pTakeOff->m_minTakeOffRoll+(pTakeOff->m_maxTakeOffRoll-pTakeOff->m_minTakeOffRoll)*pflight->GetRandomValue();
	return ARCUnit::ConvertLength(defDist,ARCUnit::M,ARCUnit::CM);
}

//get Exit speed of a specified flight
double FlightPerformanceManager::getExitSpeed (AirsideFlightInSim *pflight)const
{
	double defSpeed = 10;
	PerformLanding* pLanding = GetPerformLandingData(pflight);
	if(pLanding)
		defSpeed = pLanding->m_dMaxSpeed90deg;
	return ARCUnit::ConvertVelocity(defSpeed,ARCUnit::KNOT,ARCUnit::CMpS);
}

//takeoff accelerate speed
double FlightPerformanceManager::getTakeoffAcceleration(AirsideFlightInSim *pflight) const
{
	double defAcc = 20;
	PerformTakeOff* pTakeOff = GetPerformTakeOffData(pflight);
	if(pTakeOff)
		defAcc = pTakeOff->m_nMinAcceleration+(pTakeOff->m_nMaxAcceleration-pTakeOff->m_nMinAcceleration)*pflight->GetRandomValue();
	return ARCUnit::ConvertVelocity(defAcc,ARCUnit::MpS,ARCUnit::CMpS);
}

//takeoff ready time
//ElapsedTime FlightPerformanceManager::getReadyTime(AirsideFlightInSim *pflight)const
//{
//	ElapsedTime defTime = 500L; // 0.01s
//
//	return defTime;
//}

//taxiway in speed
double FlightPerformanceManager::getTaxiInNormalSpeed(const AirsideFlightInSim *pflight)const
{
	double defSpeed = 20;
	CTaxiInboundNEC* pTaxiInboundNEC = GetTaxiInboundNECData(pflight);
	if(pTaxiInboundNEC)
		defSpeed = pTaxiInboundNEC->GetNormalSpeed();//+(pTaxiInboundNEC->GetMaxSpeed()-pTaxiInboundNEC->GetNormalSpeed())*pflight->GetRandomValue();
	return ARCUnit::ConvertVelocity(defSpeed,ARCUnit::KNOT,ARCUnit::CMpS);
}

double FlightPerformanceManager::getTaxiInMaxSpeed( AirsideFlightInSim* pflight ) const
{
	double defSpeed = 20;
	CTaxiInboundNEC* pTaxiInboundNEC = GetTaxiInboundNECData(pflight);
	if(pTaxiInboundNEC)
		defSpeed = pTaxiInboundNEC->GetMaxSpeed();//+(pTaxiInboundNEC->GetMaxSpeed()-pTaxiInboundNEC->GetNormalSpeed())*pflight->GetRandomValue();
	return ARCUnit::ConvertVelocity(defSpeed,ARCUnit::KNOT,ARCUnit::CMpS);

}

double FlightPerformanceManager::getTaxiOutMaxSpeed(AirsideFlightInSim* pflight)const
{
	double defSpeed = 20;
	CTaxiOutboundNEC* pTaxiOutboundNEC = GetTaxiOutboundNECData(pflight);
	if(pTaxiOutboundNEC)
		defSpeed = pTaxiOutboundNEC->GetMaxSpeed();//+(pTaxiOutboundNEC->GetMaxSpeed()-pTaxiOutboundNEC->GetNormalSpeed())*pflight->GetRandomValue();
	return ARCUnit::ConvertVelocity(defSpeed,ARCUnit::KNOT,ARCUnit::CMpS);

}
//taxiway out speed
double FlightPerformanceManager::getTaxiOutNormalSpeed(const AirsideFlightInSim *pflight)const
{
	double defSpeed = 20;
	CTaxiOutboundNEC* pTaxiOutboundNEC = GetTaxiOutboundNECData(pflight);
	if(pTaxiOutboundNEC)
		defSpeed = pTaxiOutboundNEC->GetNormalSpeed();//+(pTaxiOutboundNEC->GetMaxSpeed()-pTaxiOutboundNEC->GetNormalSpeed())*pflight->GetRandomValue();
	return ARCUnit::ConvertVelocity(defSpeed,ARCUnit::KNOT,ARCUnit::CMpS);
}

//Approach speed
double FlightPerformanceManager::getApproachSpeedHorizen(AirsideFlightInSim *pflight)const
{
	double defSpeed = 120;
	ApproachLand* pApproachLand = GetApproachLandData(pflight);
	if(pApproachLand)
		defSpeed = pApproachLand->m_dMinSpeed+(pApproachLand->m_dMaxSpeed-pApproachLand->m_dMinSpeed)*pflight->GetRandomValue();
	return ARCUnit::ConvertVelocity(defSpeed,ARCUnit::KNOT,ARCUnit::CMpS);
}

//double FlightPerformanceManager::getApproachSpeedVertical(AirsideFlightInSim *pflight)const
//{
//	return ARCUnit::ConvertVelocity( tan(3.0) * getApproachSpeedHorizen(pflight) ,ARCUnit::KNOT,ARCUnit::FEETpMIN);
//}

//climb out speed
double FlightPerformanceManager::getClimboutSpeed(AirsideFlightInSim *pflight)const
{
	double defSpeed = 250;

	return ARCUnit::ConvertVelocity(defSpeed,ARCUnit::KNOT,ARCUnit::CMpS);
}

double FlightPerformanceManager::getMaxClimboutSpeed(AirsideFlightInSim *pflight)const
{
	double defSpeed = 300;

	return ARCUnit::ConvertVelocity(defSpeed,ARCUnit::KNOT,ARCUnit::CMpS);
}

double FlightPerformanceManager::getMinClimboutSpeed(AirsideFlightInSim *pflight)const
{
	double defSpeed = 200;

	return ARCUnit::ConvertVelocity(defSpeed,ARCUnit::KNOT,ARCUnit::CMpS);
}

//double FlightPerformanceManager::getClimboutSpeedVertical(AirsideFlightInSim *pflight)const
//{
//	double defSpeed = 17;
//
//	return 	ARCUnit::ConvertLength(defSpeed,ARCUnit::FEET,ARCUnit::CM);
//}
double FlightPerformanceManager::getMaxLandingDist(AirsideFlightInSim* pflight) const
{
	double defDist = 1000;
	PerformLanding* pLanding = GetPerformLandingData(pflight);
	if(pLanding)
		defDist = pLanding->m_dMaxLandDist;
	return ARCUnit::ConvertLength(defDist,ARCUnit::M,ARCUnit::CM);
}

double FlightPerformanceManager::getMinLandingDist(AirsideFlightInSim* pflight) const
{
	double defDist = 200;
	PerformLanding* pLanding = GetPerformLandingData(pflight);
	if(pLanding)
		defDist = pLanding->m_dMinLandDist;
	return ARCUnit::ConvertLength(defDist,ARCUnit::M,ARCUnit::CM);
}

double FlightPerformanceManager::getMaxTouchDownSpeed(AirsideFlightInSim* pflight) const
{
	double defSpd = 120;
	PerformLanding* pLanding = GetPerformLandingData(pflight);
	if(pLanding)
		defSpd = pLanding->m_dMaxTouchDownSpeed ;
	return ARCUnit::ConvertVelocity(defSpd,ARCUnit::KNOT,ARCUnit::CMpS);
}

double FlightPerformanceManager::getMinTouchDownSpeed(AirsideFlightInSim* pflight) const
{
	double defSpd = 80;
	PerformLanding* pLanding = GetPerformLandingData(pflight);
	if(pLanding)
		defSpd = pLanding->m_dMinTouchDownSpeed;
	return ARCUnit::ConvertVelocity(defSpd,ARCUnit::KNOT,ARCUnit::CMpS);
}

double FlightPerformanceManager::getTouchDownSpeed(AirsideFlightInSim* pflight) const
{
	return getMinTouchDownSpeed(pflight) + (getMaxTouchDownSpeed(pflight) - getMinTouchDownSpeed(pflight))*pflight->GetRandomValue();
}

double FlightPerformanceManager::getMaxTouchDownDist(AirsideFlightInSim* pflight) const
{
	double defDist = 500;
	PerformLanding* pLanding = GetPerformLandingData(pflight);
	if(pLanding)
		defDist = pLanding->m_dMaxTouchDownDist;
	return ARCUnit::ConvertLength(defDist,ARCUnit::M,ARCUnit::CM);
}

double FlightPerformanceManager::getMinTouchDownDist(AirsideFlightInSim* pflight) const
{
	double defDist = 100;
	PerformLanding* pLanding = GetPerformLandingData(pflight);
	if(pLanding)
		defDist = pLanding->m_dMinTouchDownDist;
	return ARCUnit::ConvertLength(defDist,ARCUnit::M,ARCUnit::CM);
}

double FlightPerformanceManager::getMaxLandingDecel( AirsideFlightInSim* pflight )const
{
	double defDecel = 30;
	PerformLanding* pLanding = GetPerformLandingData(pflight);
	if(pLanding)
		defDecel = pLanding->m_dMaxDeceleration;
	return ARCUnit::ConvertVelocity(defDecel,ARCUnit::MpS,ARCUnit::CMpS);
}

double FlightPerformanceManager::getNormalLandingDecel( AirsideFlightInSim* pflight )const
{
	double defDecel = 4;
	PerformLanding* pLanding = GetPerformLandingData(pflight);
	if(pLanding)
		defDecel = pLanding->m_dNormalDeceleration;
	return ARCUnit::ConvertLength(defDecel,ARCUnit::FEET,ARCUnit::CM);
}

double FlightPerformanceManager::getMaxExitSpeedAcute(AirsideFlightInSim* pflight)const
{
	double defSpeed = 30;
	PerformLanding* pLanding = GetPerformLandingData(pflight);
	if(pLanding)
		defSpeed = pLanding->m_dMaxSpeedAcute;
	return ARCUnit::ConvertVelocity(defSpeed,ARCUnit::KNOT,ARCUnit::CMpS);
}

double FlightPerformanceManager::getMaxExitSpeed90deg(AirsideFlightInSim* pflight)const
{
	double defSpeed = 25;
	PerformLanding* pLanding = GetPerformLandingData(pflight);
	if(pLanding)
		defSpeed = pLanding->m_dMaxSpeed90deg;
	return ARCUnit::ConvertVelocity(defSpeed,ARCUnit::KNOT,ARCUnit::CMpS);
}

double FlightPerformanceManager::getMaxExitSpeedHigh(AirsideFlightInSim* pflight)const
{
	double defSpeed = 20;
	PerformLanding* pLanding = GetPerformLandingData(pflight);
	if(pLanding)
		defSpeed = pLanding->m_dMaxSpeedHighSpeed;
	return ARCUnit::ConvertVelocity(defSpeed,ARCUnit::KNOT,ARCUnit::CMpS);
}

double FlightPerformanceManager::getMaxTerminalSpeed(AirsideFlightInSim* pflight)const
{
	double defSpeed = 200;
	CFlightPerformanceTerminal* pTerminal = GetFlightPerformanceTerminalData(pflight);
	if(pTerminal)
		defSpeed = pTerminal->getMaxSpeed();
	return ARCUnit::ConvertVelocity(defSpeed,ARCUnit::KNOT,ARCUnit::CMpS);
}

double FlightPerformanceManager::getMinTerminalSpeed(AirsideFlightInSim* pflight)const
{
	double defSpeed = 100;
	CFlightPerformanceTerminal* pTerminal = GetFlightPerformanceTerminalData(pflight);
	if(pTerminal)
		defSpeed = pTerminal->getMinSpeed();
	return ARCUnit::ConvertVelocity(defSpeed,ARCUnit::KNOT,ARCUnit::CMpS);
}

double FlightPerformanceManager::getMinApproachSpeed(AirsideFlightInSim* pflight)
{
	double defSpeed = 50;
	ApproachLand* pApproach = GetApproachLandData(pflight);
	if (pApproach)
		defSpeed = pApproach->m_dMinSpeed;
	return ARCUnit::ConvertVelocity(defSpeed,ARCUnit::KNOT,ARCUnit::CMpS);
}

double FlightPerformanceManager::getMaxApproachSpeed(AirsideFlightInSim* pflight)
{
	double defSpeed = 150;
	ApproachLand* pApproach = GetApproachLandData(pflight);
	if (pApproach)
		defSpeed = pApproach->m_dMaxSpeed;
	return ARCUnit::ConvertVelocity(defSpeed,ARCUnit::KNOT,ARCUnit::CMpS);
}

double FlightPerformanceManager::getMinHorizontalAccel(AirsideFlightInSim* pflight)const
{
	double defAcc = 20;
	DepClimbOut* pDepClimbOut = GetDepClimbOutData(pflight);
	if(pDepClimbOut)
		defAcc = pDepClimbOut->m_nMinHorAccel;
	return ARCUnit::ConvertVelocity(defAcc,ARCUnit::MpS,ARCUnit::CMpS);
}

double FlightPerformanceManager::getMaxHorizontalAccel(AirsideFlightInSim* pflight)const
{
	double defAcc = 30;
	DepClimbOut* pDepClimbOut = GetDepClimbOutData(pflight);
	if(pDepClimbOut)
		defAcc = pDepClimbOut->m_nMaxHorAccel;
	return ARCUnit::ConvertVelocity(defAcc,ARCUnit::MpS,ARCUnit::CMpS);
}

double FlightPerformanceManager::getMinVerticalSpeed(AirsideFlightInSim* pflight)const
{
	double defSpeed = 15;
	DepClimbOut* pDepClimbOut = GetDepClimbOutData(pflight);
	if(pDepClimbOut)
		defSpeed = pDepClimbOut->m_nMinVerticalSpeedToEntoute;
	return ARCUnit::ConvertVelocity(defSpeed,ARCUnit::KNOT,ARCUnit::CMpS);
}

double FlightPerformanceManager::getMaxVerticalSpeed(AirsideFlightInSim* pflight)const
{
	double defSpeed = 19;
	DepClimbOut* pDepClimbOut = GetDepClimbOutData(pflight);
	if(pDepClimbOut)
		defSpeed = pDepClimbOut->m_nMaxVerticalSpeedToEntoute;
	return ARCUnit::ConvertVelocity(defSpeed,ARCUnit::KNOT,ARCUnit::CMpS);
}

double FlightPerformanceManager::getMaxCruiseSpeed(AirsideFlightInSim* pflight)const
{
	double defSpeed = 250;
	CFlightPerformanceCruise* pCruise = GetFlightPerformanceCruiseData(pflight);
	if(pCruise)
		defSpeed = pCruise->getMaxSpeed();
	return ARCUnit::ConvertVelocity(defSpeed,ARCUnit::KNOT,ARCUnit::CMpS);
}

double FlightPerformanceManager::getMinCruiseSpeed(AirsideFlightInSim* pflight)const
{
	double defSpeed = 150;
	CFlightPerformanceCruise* pCruise = GetFlightPerformanceCruiseData(pflight);
	if(pCruise)
		defSpeed = pCruise->getMinSpeed();
	return ARCUnit::ConvertVelocity(defSpeed,ARCUnit::KNOT,ARCUnit::CMpS);
}

double FlightPerformanceManager::getTaxiOutboundMinSeparationInQ( AirsideFlightInSim* pflight )const
{
	DistanceUnit defDist = 75;
	CTaxiOutboundNEC * pOutBound = GetTaxiOutboundNECData(pflight);
	if(pOutBound)
		defDist = pOutBound->GetMinSeparation();
	return ARCUnit::ConvertLength(defDist, ARCUnit::M, ARCUnit::CM);
}

double FlightPerformanceManager::getTaxiOutboundMaxSeparationInQ( AirsideFlightInSim* pflight )const
{
	DistanceUnit defDist = 150;
	CTaxiOutboundNEC * pOutBound = GetTaxiOutboundNECData(pflight);
	if(pOutBound)
		defDist = pOutBound->GetMaxSeparation();
	return ARCUnit::ConvertLength(defDist, ARCUnit::M, ARCUnit::CM);
}

double FlightPerformanceManager::getTaxiOutboundSeparationDistInQ( AirsideFlightInSim* pflight )const
{
	int nMinSeparationInQ = (int)(getTaxiOutboundMinSeparationInQ(pflight));
	int nMaxSeparationInQ = (int)(getTaxiOutboundMaxSeparationInQ(pflight));

	double dReturnValue = 0.0;
	if (nMinSeparationInQ == nMaxSeparationInQ)
	{
		dReturnValue = getTaxiOutboundMinSeparationInQ(pflight);
	}
	else
	{
		dReturnValue = nMinSeparationInQ + (nMaxSeparationInQ - nMinSeparationInQ)* pflight->GetRandomValue();
	}

	return dReturnValue; 
}


double FlightPerformanceManager::getTaxiInboundMinSeparationInQ( AirsideFlightInSim* pflight )const
{
	DistanceUnit defDist = 75;
	CTaxiInboundNEC * pInBound = GetTaxiInboundNECData(pflight);
	if(pInBound)
		defDist = pInBound->GetMinSeparation();
	return ARCUnit::ConvertLength(defDist, ARCUnit::M, ARCUnit::CM);
}

double FlightPerformanceManager::getTaxiInboundMaxSeparationInQ( AirsideFlightInSim* pflight )const
{
	DistanceUnit defDist = 150;
	CTaxiInboundNEC * pInBound = GetTaxiInboundNECData(pflight);
	if(pInBound)
		defDist = pInBound->GetMaxSeparation();
	return ARCUnit::ConvertLength(defDist, ARCUnit::M, ARCUnit::CM);
}

double FlightPerformanceManager::getTaxiInboundSeparationDistInQ( AirsideFlightInSim* pflight )const
{
	double nMinSeparationInQ = getTaxiInboundMinSeparationInQ(pflight);
	double nMaxSeparationInQ = getTaxiInboundMaxSeparationInQ(pflight);

	double dReturnValue = 0.0;
	if (nMinSeparationInQ == nMaxSeparationInQ)
	{
		dReturnValue = nMaxSeparationInQ;
	}
	else
	{
		dReturnValue = nMinSeparationInQ + (nMaxSeparationInQ - nMinSeparationInQ)* pflight->GetRandomValue();
	}

	return dReturnValue; 
}



double FlightPerformanceManager::getTaxiInAccelation( AirsideFlightInSim * pflight ) const
{
	const static double dMinAcc = 10;
	double dAcc = dMinAcc;
	CTaxiInboundNEC * pInBound = GetTaxiInboundNECData(pflight);
	if(pInBound)
		dAcc = pInBound->GetAcceleration();
	return ARCUnit::ConvertLength(dAcc, ARCUnit::M, ARCUnit::CM);

}

double FlightPerformanceManager::getTaxiInDeceleration( AirsideFlightInSim * pFlight ) const
{
	const static double dMinDec= 10;
	double dDec = dMinDec;
	CTaxiInboundNEC * pInBound = GetTaxiInboundNECData(pFlight);
	if(pInBound)
		dDec = pInBound->GetDeceleration();
	return ARCUnit::ConvertLength(dDec, ARCUnit::M, ARCUnit::CM);

}

double FlightPerformanceManager::getTaxiOutAccelation( AirsideFlightInSim * pflight ) const
{
	const static double dMinAcc = 10; //m/s2
	double dAcc = dMinAcc;
	CTaxiOutboundNEC * pInBound = GetTaxiOutboundNECData(pflight);
	if(pInBound)
		dAcc = pInBound->GetAcceleration();
	return ARCUnit::ConvertLength(dAcc, ARCUnit::M, ARCUnit::CM);


}

double FlightPerformanceManager::getTaxiOutDeceleration( AirsideFlightInSim* pFlight ) const
{
	const static double dMinDec= 10;
	double dDec = dMinDec;
	CTaxiOutboundNEC * pInBound = GetTaxiOutboundNECData(pFlight);
	if(pInBound)
		dDec = pInBound->GetDeceleration();
	return ARCUnit::ConvertLength(dDec, ARCUnit::M, ARCUnit::CM);
}

double FlightPerformanceManager::getPushBackSpeed(AirsideFlightInSim* pFlight) const
{
	double defSpd = 2; //2 m/s
	TowOperationPerformance  *pTowItem = GetTowingData(pFlight);
	if(pTowItem)
	{
		CProDistrubution* pDist = pTowItem->getPushbackSpeed();
		defSpd = pDist->GetProbDistribution()->getRandomValue();
	}
	return defSpd * 100;
}

double FlightPerformanceManager::getPushBackAcc( AirsideFlightInSim* pFlight ) const
{
	double defSpd = 0; //2 m/s
	TowOperationPerformance  *pTowItem = GetTowingData(pFlight);
	if(pTowItem)
	{
		CProDistrubution* pDist = pTowItem->getAcceleration();
		defSpd = pDist->GetProbDistribution()->getRandomValue();
	}
	return defSpd * 100;
}

double FlightPerformanceManager::getPushBackDec( AirsideFlightInSim* pFlight ) const
{
	double defSpd = 0; //2 m/s
	TowOperationPerformance  *pTowItem = GetTowingData(pFlight);
	if(pTowItem)
	{
		CProDistrubution* pDist = pTowItem->getDeceleration();
		defSpd = pDist->GetProbDistribution()->getRandomValue();
	}
	return defSpd * 100;
}


ElapsedTime FlightPerformanceManager::getUnhookandTaxiTime(AirsideFlightInSim* pFlight) const
{
	ElapsedTime defTime = 1*60L ;//3min
	TowOperationPerformance  *pTowItem = GetTowingData(pFlight);
	if(pTowItem)
	{
		CProDistrubution* pDist =pTowItem->getUnhookAndTaxiTime();
		defTime = ElapsedTime(pDist->GetProbDistribution()->getRandomValue()*60);
	}

	return defTime;
}

double FlightPerformanceManager::getTowingSpeed(const AirsideFlightInSim* pFlight) const
{
	double defSpd = 4; //2 m/s
	TowOperationPerformance  *pTowItem = GetTowingData(pFlight);
	if(pTowItem)
	{
		CProDistrubution* pDist = pTowItem->getTowSpeed();
		defSpd = pDist->GetProbDistribution()->getRandomValue();
	}
	return defSpd * 100;
}

TowOperationPerformance * FlightPerformanceManager::GetTowingData( const AirsideFlightInSim* pFlight ) const
{
	for(int i=0;i<(int)m_pTowingPerform->GetElementCount();i++)
	{
		TowOperationPerformance * pNec = m_pTowingPerform->GetItem(i);
		if(pFlight->fits(pNec->GetFltType()))
		{
			return pNec;
		}
	}
	return NULL;
}

StandService* FlightPerformanceManager::GetStandServiceData(AirsideFlightInSim* pFlight) const
{
	if (pFlight == NULL)
		return NULL;

	int nCount = m_pStandServiceList->GetElementCount();
	for(int i =0; i < nCount; i++)
	{
		StandService* pStandService = m_pStandServiceList->GetItem(i);
		if (pStandService && pFlight->fits(pStandService->GetFltType()))
		{
			return pStandService;
		}
	}

	return NULL;
}

CEngineParametersItem* FlightPerformanceManager::GetEngineStartData( AirsideFlightInSim* pFlight ) const
{
	if (pFlight == NULL)
		return NULL;

	int nCount = m_pEngineStartList->getCount();
	for (int i =0; i < nCount; i++)
	{
		CEngineParametersItem* pData = m_pEngineStartList->getItem(i);
		if (pFlight->fits(pData->GetFltType()))
			return pData;
	}

	return NULL;
}

ElapsedTime FlightPerformanceManager::getStandMinTurnAroundTime(AirsideFlightInSim* pFlight) const
{
	StandService* pStandService = GetStandServiceData(pFlight);
	if (pStandService == NULL)
		return 0L;

	double dTime = 0.0;
	if(pStandService->GetTimeProbDistribution())
		dTime =pStandService->GetTimeProbDistribution()->getRandomValue();
	return ElapsedTime(dTime*60.0);
}

double FlightPerformanceManager::getAvgApproachSpeedInKnots( AirsideFlightInSim* pflight )
{
	double dSpd = getMinApproachSpeed(pflight)*0.5 + getMaxApproachSpeed(pflight)*0.5;
	return ARCUnit::ConvertVelocity(dSpd,ARCUnit::CMpS,ARCUnit::KNOT);
}

double FlightPerformanceManager::getCurTaxiSpd( AirsideFlightInSim* pFlight ) const
{
	if(pFlight && pFlight->GetMode() == OnTaxiToRunway)
	{
		return getTaxiOutNormalSpeed(pFlight);
	}
	if(pFlight && pFlight->GetMode() == OnTowToDestination )
	{
		return getTowingSpeed(pFlight);
	}
	//if(pFlight && pFlight->GetMode() == OnTaxiToStand )
	{
		return getTaxiInNormalSpeed(pFlight);
	}
	
}

double FlightPerformanceManager::getCurTaxiAccSpd( AirsideFlightInSim* pFlight ) const
{
if(pFlight && pFlight->GetMode() == OnTaxiToRunway)
	{
		return getTaxiOutAccelation(pFlight);
	}
	if(pFlight && pFlight->GetMode() == OnTowToDestination )
	{
		return getPushBackAcc(pFlight);
	}
	//if(pFlight && pFlight->GetMode() == OnTaxiToStand )
	{
		return getTaxiInAccelation(pFlight);
	}
}

double FlightPerformanceManager::getCurTaxiDecSpd( AirsideFlightInSim* pFlight ) const
{
	if(pFlight && pFlight->GetMode() == OnTaxiToRunway)
	{
		return getTaxiOutDeceleration(pFlight);
	}
	if(pFlight && pFlight->GetMode() == OnTowToDestination )
	{
		return getPushBackDec(pFlight);
	}
	//if(pFlight && pFlight->GetMode() == OnTaxiToStand )
	{
		return getTaxiInDeceleration(pFlight);
	}
}

ElapsedTime FlightPerformanceManager::getEngineStartTime( AirsideFlightInSim* pFlight ) const
{
	CEngineParametersItem* pData = GetEngineStartData(pFlight);
	if (pData != NULL)
	{
		const CProDistrubution& Dist = pData->getStartTime();
		return ElapsedTime(Dist.GetProbDistribution()->getRandomValue());
	}

	return ElapsedTime(20L);
}
