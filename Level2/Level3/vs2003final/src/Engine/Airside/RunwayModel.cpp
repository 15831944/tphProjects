#include "StdAfx.h"
#include "RunwayModel.h"
#include "Flight.h"
#include "Event.h"
#include "AirspaceModel.h"
#include "AirportModel.h"
#include "TaxiwayModel.h"
#include "../../Common/ARCUnit.h"
#include "../../AirsideInput/PerformanceTakeoffs.h"
#include "../../AirsideInput/AirsideInput.h"
#include "../../AirsideInput/PerformLandings.h"
#include "../../AirsideInput/RunwayExitPerformance.h"
#include "../../Common/ARCUnit.h"

NAMESPACE_AIRSIDEENGINE_BEGINE
Point Runway::getThresholdPoint(){
	Point reslt = getPhysicalEnd1();
	
	return reslt;	
}

RunwayModel::~RunwayModel(){

}

Runway * RunwayModel::getRunway(const CString& rwname) 
{
	return m_runways.getEntity(rwname);
}

void RunwayModel::DistributeLandingProcess(SimFlight* _flight)
{	
	
	
	FlightEventList* _FEL = _flight->getFEL();
	airroute * pAirroute = m_pAirport->getAirspaceModel()->getArrivalRoute(_flight);
	if(!pAirroute)return ;
	
	Runway * pRunway = m_pAirport->getRunwayModel()->getRunwayList().getEntity(pAirroute->m_RwID);
	if(!pRunway)return;

	double LandingSpeed = getLandingSpeed(_flight);

	//start landing event
	FlightEvent *_event = new FlightEvent(_flight);
	Point thresholdPoint = pRunway->getThresholdPoint();
	SimClock::TimeType prevTime = _FEL->getLastEvent()->GetComputeTime();

	thresholdPoint.setZ(getDistThresToTouch(_flight));
	
	double dS = _FEL->getLastEvent()->GetEventPos().distance(thresholdPoint);
	double avgspeed = 0.5 * ( _FEL->getLastEvent()->GetFlightHorizontalSpeed() + LandingSpeed );
	avgspeed = ARCUnit::ConvertVelocity(avgspeed,ARCUnit::KNOT,ARCUnit::CMpS);
	SimClock::TimeSpanType dT = SimClock::TimeSpanType(dS / avgspeed * 100);

	_event->SetEventType(FlightEvent::ArriveThreshold);
	_event->SetComputeTime(prevTime + dT );
	_event->setEventTime(prevTime + dT);
	_event->SetFlightHorizontalSpeed(LandingSpeed);
	_event->SetFlightVerticalSpeed(m_pAirport->getAirspaceModel()->getApproachSpeedVertical(_flight));
	_event->SetEntityID(pRunway->getStringID());
	_event->SetEventPos(thresholdPoint);				
	_FEL->push_back(_event);	

	//start flare event
	
	//touch down event
	Point touchDownPt = getTouchDownPoint(pRunway,_flight);
	_event = new FlightEvent(_flight);
	prevTime  = _FEL->getLastEvent()->GetComputeTime();
	avgspeed  = 0.5 * ( _FEL->getLastEvent()->GetFlightHorizontalSpeed() + LandingSpeed );
	avgspeed  = ARCUnit::ConvertVelocity(avgspeed,ARCUnit::KNOT,ARCUnit::CMpS);
	dS  = _FEL->getLastEvent()->GetEventPos().distance(touchDownPt);
	dT = SimClock::TimeSpanType(dS / avgspeed * 100);
	
	_event->SetEventType(FlightEvent::Touchdown);
	_event->SetComputeTime(prevTime + dT );
	_event->setEventTime(prevTime + dT);
	_event->SetFlightHorizontalSpeed(LandingSpeed);
	_event->SetFlightVerticalSpeed(0);
	_event->SetEntityID(pRunway->getStringID());
	_event->SetEventPos(touchDownPt);				
	
	_FEL->push_back(_event);
	//exit event
	_event = new FlightEvent(_flight);
	prevTime  = _FEL->getLastEvent()->GetComputeTime();
	RunwayExit *suitExit =  getOutExit(pRunway ,_flight);
	if(!suitExit)return;

	TaxiwayNode * relatNode = m_pAirport->getTaxiwayModel()->getTaixwayNode(suitExit->m_NodeId);
	avgspeed  = 0.5 * ( _FEL->getLastEvent()->GetFlightHorizontalSpeed() + getExitSpeed(suitExit,_flight));
	avgspeed = ARCUnit::ConvertVelocity(avgspeed,ARCUnit::KNOT,ARCUnit::CMpS);
	dS  = _FEL->getLastEvent()->GetEventPos().distance(relatNode->getPosition());	
	dT = SimClock::TimeSpanType(dS / avgspeed * 100);	
	
	_event->SetEventType(FlightEvent::ExitRunway);
	_event->SetComputeTime(prevTime + dT );
	_event->setEventTime(prevTime + dT);
	_event->SetFlightHorizontalSpeed(LandingSpeed);
	_event->SetFlightVerticalSpeed(0);
	_event->SetEntityID(suitExit->getStringID());
	_event->SetEventPos(relatNode->getPosition());	

	_FEL->push_back(_event);
	
	//exit event2
	_event = new FlightEvent(_flight);
	double TaixInspd = m_pAirport->getTaxiwayModel()->getTaxiInSpeed(_flight);
	prevTime = _FEL->getLastEvent()->GetComputeTime();
	avgspeed = 0.5 * ( _FEL->getLastEvent()->GetFlightHorizontalSpeed() + TaixInspd );
	avgspeed  = ARCUnit::ConvertVelocity(avgspeed,ARCUnit::KNOT,ARCUnit::CMpS);
	TaxiwayNode * nextNode =  m_pAirport->getTaxiwayModel()->getTaixwayNode(suitExit->m_NodeIdNext);
	dS = _FEL->getLastEvent()->GetEventPos().distance(nextNode->getPosition());
	dT = SimClock::TimeSpanType(dS/avgspeed *100);

	_event->SetEventType(FlightEvent::TaxiIn);
	_event->SetComputeTime(prevTime + dT );
	_event->setEventTime(prevTime + dT);
	_event->SetFlightHorizontalSpeed(TaixInspd);
	_event->SetFlightVerticalSpeed(0);
	_event->SetEntityID(nextNode->getStringID());
	_event->SetEventPos(nextNode->getPosition());	

	_FEL->push_back(_event);

	for(int i =0;i<_FEL->getCount();i++){
		FlightEvent * event = _FEL->getItem(i);
		i =i ;		
	}
	
}

void RunwayModel::DistributeTakeoffProcess(SimFlight* _flight)
{
	FlightEventList* _FEL = _flight->getFEL();
	
	airroute * pdepRoute = m_pAirport->getAirspaceModel()->getDepatureRoute(_flight);
	if(!pdepRoute)return ;

	CString m_RwID = pdepRoute->m_RwID; 
	Runway * pRunway = m_pAirport->getRunwayModel()->getRunwayList().getEntity(m_RwID);
	double TakeoffAcceleration = getTakeoffAcceleration(_flight);
	

	//start Ready for takeoff event
	FlightEvent * pLastEvent = _FEL->getLastEvent();

	FlightEvent *_event = new FlightEvent(_flight);
	Point EndPoint =  pLastEvent->GetEventPos();
	if(EndPoint.distance(pRunway->getPhysicalEnd1()) / pRunway->getPhysicalEnd1().distance(pRunway->getPhysicalEnd2()) > 0.1  )EndPoint = pRunway->getPhysicalEnd1();
	SimClock::TimeType prevTime = pLastEvent->GetComputeTime();
	double dS = EndPoint.distance(pLastEvent->GetEventPos());
	SimClock::TimeSpanType dT = long( dS / ARCUnit::ConvertVelocity(pLastEvent->GetFlightHorizontalSpeed(),ARCUnit::KNOT,ARCUnit::CMpS) *100L );
	_event->SetEventType(FlightEvent::ReadyForTakeoff);
	_event->SetComputeTime(prevTime + getReadyTime(_flight)*100L + dT);
	_event->setEventTime(prevTime + getReadyTime(_flight)*100L + dT );
	_event->SetFlightHorizontalSpeed(0);
	_event->SetFlightVerticalSpeed(0);
	_event->SetEntityID(m_RwID);
	_event->SetEventPos(EndPoint);				
	_FEL->push_back(_event);
	
	//	
	prevTime = _FEL->getLastEvent()->GetComputeTime();
	_event  = new FlightEvent(_flight);
	double _distance = getDistEndToTakeoff(_flight) * SCALE_FACTOR;
	Point rwOrien =  pRunway->getPhysicalEnd2()-pRunway->getPhysicalEnd1();
	rwOrien.length(_distance);
	Point takeoffPt = rwOrien + EndPoint;
	double _accelerate =  getTakeoffAcceleration(_flight);
	

	dT = SimClock::TimeSpanType(sqrt(2.0*_distance/ARCUnit::ConvertVelocity(_accelerate,ARCUnit::KNOT,ARCUnit::CMpS))*100L);

	_event->SetEventType(FlightEvent::Takeoff);
	_event->SetComputeTime(prevTime + dT );
	_event->setEventTime(prevTime + dT);
	_event->SetFlightHorizontalSpeed(_accelerate * dT / 100 );
	_event->SetFlightVerticalSpeed(0);
	_event->SetEntityID(m_RwID);
	_event->SetEventPos(takeoffPt);				
	_FEL->push_back(_event);			


}
RunwayExit  * RunwayModel::getOutExit(Runway * pRunway, SimFlight * _flight)
{
	double dec = getDecelSpeed(_flight);   // knot/s
	FlightEvent * touchdownEvent = _flight->getFEL()->getEventInType(FlightEvent::Touchdown);
	if(!touchdownEvent) return NULL;
	Point touchPt = touchdownEvent->GetEventPos();
	double touchspd  = touchdownEvent->GetFlightHorizontalSpeed();
	touchspd = ARCUnit::ConvertVelocity(touchspd,ARCUnit::KNOT);

	Point phyEnd = pRunway->getPhysicalEnd2();
	
	for(int i=0;i<(int)pRunway->getExitList().size();i++){
		RunwayExit * pExit = pRunway->getExitList().at(i).get();
		Point exitpos = m_pAirport->getTaxiwayModel()->getTaixwayNode(pExit->m_NodeId)->m_pos;

		ARCVector2 fltorien = ARCVector2(touchPt,phyEnd);
		ARCVector2 exitOrien  =  ARCVector2(touchPt,exitpos);
		if(fltorien.GetCosOfTwoVector(exitOrien)<0)continue;

		double dist = exitpos.distance(touchPt);  dist = ARCUnit::ConvertLength(dist,ARCUnit::CM);
		double dt = (sqrt(touchspd*touchspd + 2 * dec * dist) - touchspd)/dec;
		double exitspd = touchspd - dec * dt;
		if( exitspd < getExitSpeed(pExit,_flight) )return pExit;

	}
	if(pRunway->getExitList().size()>0){
		return pRunway->getExitList().rbegin()->get();
	}

	return NULL;
}

double RunwayModel::getDecelSpeed(SimFlight * _flight){
	double defdecSpd  = 10;
	FlightDescStruct& fltdesc = _flight-> GetCFlight()->getLogEntry();
	std::vector<PerformLanding>  vPerformLanding  = m_pAirsideInput->GetPerformLandings()->GetPerformLandings() ;
	for(int i=0;i<(int)vPerformLanding.size();i++)
	{
		PerformLanding performLanding = vPerformLanding.at(i);
		FlightConstraint& fltcont = performLanding.m_fltType;
		if(fltcont.fits(fltdesc))
		{
			defdecSpd = performLanding.m_dMaxDeceleration;
			break;
		}

	}
	return defdecSpd;
}

// get distance from threshold to touchdown
double RunwayModel::getDistThresToTouch( SimFlight * _flight)
{
	double defDist = 10000;
	FlightDescStruct& fltdesc = _flight-> GetCFlight()->getLogEntry();
	std::vector<PerformLanding>  vPerformLanding  = m_pAirsideInput->GetPerformLandings()->GetPerformLandings() ;
	for(int i=0;i<(int)vPerformLanding.size();i++)
	{
		PerformLanding performLanding = vPerformLanding.at(i);
		FlightConstraint& fltcont = performLanding.m_fltType;
		if(fltcont.fits(fltdesc))
		{
			defDist = 0.5*(performLanding.m_dMaxTouchDownDist + performLanding.m_dMinTouchDownDist);
			break;
		}
		
	}
	return defDist;

}

double RunwayModel::getDistEndToTakeoff( SimFlight * _flight)
{
	double defDist = 400; //meters
	FlightDescStruct& fltdesc = _flight-> GetCFlight()->getLogEntry();
	std::vector<PerformTakeOff>  vPerformTakeoff  = m_pAirsideInput->GetPerformTakeOffs()->GetTakeoffs();
	for(int i=0;i<(int)vPerformTakeoff.size();i++)
	{
		PerformTakeOff performTakeoff = vPerformTakeoff.at(i);
		FlightConstraint& fltcont = performTakeoff.m_fltType;
		if(fltcont.fits(fltdesc))
		{
			defDist = 0.5*(performTakeoff.m_nMaxLiftOff + performTakeoff.m_nMinLiftOff);
			break;
		}
	}
	return defDist;
}
// get out exit by the Runway and the flight
RunwayExit * RunwayModel::getInExit(Runway * pRunway ,SimFlight * _flight)
{
	if(pRunway->getExitList().size()<=0)return NULL;
	return pRunway->getExitList().begin()->get();
	
}

//get Exit speed of a specified flight
double RunwayModel::getExitSpeed (RunwayExit * pExit, SimFlight * _flight)
{
	double defSpd = 10;
	FlightDescStruct& fltdesc = _flight-> GetCFlight()->getLogEntry();
	ns_AirsideInput::vRunwayExitSpeedInfo vPerformRunwayExit  = m_pAirsideInput->GetRunwayExitPerformance()->getRunwayExitSpeedInfo();
	for(int i=0;i<(int)vPerformRunwayExit.size();i++)
	{
		ns_AirsideInput::CRunwayExitSpeedInfo * pperformRunwayExit = vPerformRunwayExit.at(i);
		FlightConstraint& fltcont = pperformRunwayExit->GetFltType();
		if(fltcont.fits(fltdesc))
		{
			defSpd = pperformRunwayExit->getNormalSpeed();
			break;
		}
	}
	return defSpd;
}

//get landing speed
double RunwayModel::getLandingSpeed(SimFlight * _flight)
{
	double defSpd = 100;
	FlightDescStruct& fltdesc = _flight-> GetCFlight()->getLogEntry();
	std::vector<PerformLanding>&  vPerformLanding  = m_pAirsideInput->GetPerformLandings()->GetPerformLandings() ;
	for(int i=0;i<(int)vPerformLanding.size();i++)
	{
		PerformLanding performLanding = vPerformLanding.at(i);
		FlightConstraint& fltcont = performLanding.m_fltType;
		if(fltcont.fits(fltdesc))
		{
			defSpd = 0.5*(performLanding.m_dMinTouchDownSpeed + performLanding.m_dMaxTouchDownSpeed);
			break;
		}
	}
	return defSpd;
}
Point RunwayModel::getTouchDownPoint(Runway * pRunway , SimFlight * _flight){
	Point rwOrien = (pRunway->getPhysicalEnd2()- pRunway->getPhysicalEnd1());
	rwOrien =  rwOrien * 0.2;
	return pRunway->getThresholdPoint() + rwOrien;
}

double RunwayModel::getTakeoffAcceleration(SimFlight *_flight)   // in knot/s
{
	double defAcc = 20;
	FlightDescStruct& fltdesc = _flight-> GetCFlight()->getLogEntry();
	std::vector<PerformTakeOff> vPerformTakeoff  = m_pAirsideInput->GetPerformTakeOffs()->GetTakeoffs();
	for(int i=0;i<(int)vPerformTakeoff.size();i++)
	{
		PerformTakeOff performTakeoff = vPerformTakeoff.at(i);
		FlightConstraint& fltcont = performTakeoff.m_fltType;
		if(fltcont.fits(fltdesc))
		{
			defAcc = 0.5*(performTakeoff.m_nMaxAcceleration + performTakeoff.m_nMinAcceleration);
			break;
		}
	}
	return defAcc;
}

Point RunwayModel::getTakeoffPoint(Runway * pRunway , SimFlight * _flight)
{
	Point rwOrien = (pRunway->getPhysicalEnd2()- pRunway->getPhysicalEnd1());
	rwOrien.length(getDistEndToTakeoff(_flight));
	return rwOrien;
}

void RunwayModel::Build(CAirsideInput* _inputC, AirsideInput* _inputD){
	m_pAirsideInput = _inputD;
}

SimClock::TimeSpanType RunwayModel::getReadyTime(SimFlight * _flight)
{
	SimClock::TimeSpanType defReadyTime = 0;
	FlightDescStruct& fltdesc = _flight-> GetCFlight()->getLogEntry();
	std::vector<PerformTakeOff> vPerformTakeoff  = m_pAirsideInput->GetPerformTakeOffs()->GetTakeoffs();
	for(int i=0;i<(int)vPerformTakeoff.size();i++)
	{
		PerformTakeOff performTakeoff = vPerformTakeoff.at(i);
		FlightConstraint& fltcont = performTakeoff.m_fltType;
		if(fltcont.fits(fltdesc))
		{
			defReadyTime = SimClock::TimeSpanType(0.5*(performTakeoff.m_nMaxPositionTime + performTakeoff.m_nMinPositionTime));
			break;
		}
	}
	return defReadyTime;
}
NAMESPACE_AIRSIDEENGINE_END