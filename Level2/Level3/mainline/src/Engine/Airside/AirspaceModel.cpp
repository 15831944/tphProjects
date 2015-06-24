#include "stdafx.h"

#include "AirspaceModel.h"
#include "AirportModel.h"

#include "../../Inputs/AirsideInput.h"
#include "../../AirsideInput/AirsideInput.h"
#include "../../AirsideInput/FlightPerformanceCruise.h"
#include "../../AirsideInput/AirRoutes.h"
#include "../../AirsideInput/FlightPerformanceTerminal.h"
#include "../../AirsideInput/DepClimbOut.h"
#include "../FixProc.h"
#include "../HoldProc.h"
#include "Flight.h"
#include "Event.h"
#include "../../Common/ARCUnit.h"
//#include "FlightType.h"
#include "RunwayModel.h"
#include <math.h>
NAMESPACE_AIRSIDEENGINE_BEGINE



//
bool AirspaceModel::Build(CAirsideInput* _inputC,AirsideInput* _inputD){
	m_pCAirsideInput  = _inputC;
	m_pDAirsideInput = _inputD;
	Clear();
	//get fix processor
	ProcessorList* pProcList = _inputC->GetProcessorList();

	const ns_AirsideInput::AirRoutes& airroutes = _inputD->GetAirRoutes();
	const std::vector<ns_AirsideInput::AirRoute>& vAirRoutes =  airroutes.GetAirRoutes();
	std::vector<ns_AirsideInput::AirRoute>::const_iterator routsItr;

	for(int i=0;i<pProcList->getProcessorCount();i++){
		Processor * pProc = pProcList->getProcessor(i);
		if(FixProcessor == pProc->getProcessorType()){
			FixProc * pFix  = (FixProc*)pProc;
			fix * pnewfix = new fix;
			pnewfix->m_pfix  = pFix;
			pnewfix->setStringID(pFix->getIDName());
			m_vfixes.push_back(pnewfix);
		}
		if(HoldProcessor == pProc->getProcessorType()){
			HoldProc * pHold = (HoldProc*)pProc;
			hold * pnewHold = new hold;
			pnewHold->m_pfix = pHold->getFix();
			pnewHold->m_pHold = pHold;
			m_vholds.push_back(pnewHold);
		}
	}
	// get air route 
	for(routsItr= vAirRoutes.begin();routsItr!=vAirRoutes.end();routsItr++){
		airroute * pnewAiroute = new airroute;
		pnewAiroute->setStringID(routsItr->m_strName);
		pnewAiroute->m_RouteType = routsItr->m_Type==ns_AirsideInput::AirRouteType_SID? STAR:SID;
		for(int i=0;i<(int)routsItr->m_vFixes.size();i++){
			pnewAiroute->m_vfixes.push_back(m_vfixes.getEntity(routsItr->m_vFixes[i]));
		}
		pnewAiroute->m_RwID = routsItr->m_strRunwayName;
		m_vroutes.push_back(pnewAiroute);
	}	

	return true;

}
//
void AirspaceModel::Clear(){
	m_vfixes.clear();
	m_vholds.clear();	
	m_vroutes.clear();
}


//distribute the FEL to the flight
void AirspaceModel::DistributeArrivalProcess(SimFlight* _flight)
{
	
	airroute * pairroute = getArrivalRoute(_flight);
	if(pairroute){
		FlightEventList * pEventList = _flight->getFEL();
	
		SimClock::TimeSpanType dT = 0;
	// the first event;


		for(int i=0;i<(int)pairroute->m_vfixes.size();i++)
		{
			
			FlightEvent * _event = new FlightEvent(_flight);
			FixProc * pFixProc = pairroute->m_vfixes.at(i).get()->m_pfix;
			FixProc::OperationType operationType = pFixProc->getOperationType();
			Point position;
			double hspeed;
			double vspeed;
			FlightEvent::EventType eventType;
			SimClock::TimeType prevTime = pEventList->getLastEvent()->GetComputeTime();

			position.setX( pFixProc->getServicePoint(0).getX() ) ;
			position.setY( pFixProc->getServicePoint(0).getY() ) ;
			position.setZ( 0.5 * (pFixProc->GetLowerLimit()  + pFixProc->GetUpperLimit()) );
			
			if( operationType == FixProc::Cruise ){
				eventType  = FlightEvent::Cruise;
				hspeed = getCruiseSpeedHorizen(_flight);
				vspeed = 0 ;
			}
			if( operationType == FixProc::Descend){
				eventType  = FlightEvent::Descent;
				hspeed = getTerminalSpeedHorizen(_flight);
				vspeed = 0;
			}
			if( operationType == FixProc::Approach ){
				eventType = FlightEvent::Approach;
				hspeed = getApproachSpeedHorizen(_flight);
				vspeed = getApproachSpeedVertical( _flight );
			}
			SimClock::TimeSpanType dT; 
			
			bool bafterBirth = pEventList->getLastEvent()->GetEventType() == FlightEvent::FlightBirth;
			if(bafterBirth){
				dT = 0;
			}else{
				double dS = pEventList->getLastEvent()->GetEventPos().distance(position);
				double avgspeed = 0.5 * ( hspeed + pEventList->getLastEvent()->GetFlightHorizontalSpeed() );
				avgspeed = ARCUnit::ConvertVelocity(avgspeed,ARCUnit::KNOT,ARCUnit::CMpS);
				dT = (SimClock::TimeSpanType)(dS / avgspeed * 100);
			}						
				
			_event->SetEventType(eventType);
			_event->setEventTime(prevTime + dT);
			_event->SetComputeTime(prevTime +dT);
			_event->SetFlightHorizontalSpeed(hspeed);
			_event->SetFlightVerticalSpeed(vspeed);
			_event->SetEntityID(pairroute->m_vfixes.at(i).get()->m_strID);
			_event->SetEventPos(position);
			pEventList->push_back(_event);		

		}
	}
}

void AirspaceModel::DistributeDepatureProcess(SimFlight *_flight)
{
	airroute * pairroute = getDepatureRoute(_flight);
	if(pairroute)
	{
		FlightEventList * pEventList = _flight->getFEL();
		SimClock::TimeSpanType dT = 0;
		// the first event;

		for(int i = 0; i < (int)pairroute->m_vfixes.size(); i++)
		{
			FlightEvent * _event = new FlightEvent(_flight);
			FixProc * pFixProc = pairroute->m_vfixes.at(i).get()->m_pfix;
			FixProc::OperationType operationType = pFixProc->getOperationType();
			Point position;
			double hspeed;
			double vspeed;
			FlightEvent::EventType eventType;
			SimClock::TimeType prevTime = pEventList->getLastEvent()->GetComputeTime();

			position.setX( pFixProc->getServicePoint(0).getX() ) ;
			position.setY( pFixProc->getServicePoint(0).getY() ) ;
			position.setZ( 0.5 * (pFixProc->GetLowerLimit()  + pFixProc->GetUpperLimit()) );

			if( operationType == FixProc::ClimbOut){
				eventType  = FlightEvent::ClimbOut;
				hspeed = getClimboutSpeed(_flight);
				vspeed= getClimboutSpeedVertical(_flight);
			}else 

			if( operationType == FixProc::Cruise ){
				eventType  = FlightEvent::Cruise;
				hspeed= getCruiseSpeedHorizen(_flight);
				vspeed= 0 ;
			}
			else{
				eventType  = FlightEvent::Cruise;
				hspeed= getCruiseSpeedHorizen(_flight);
				vspeed= 0 ;
			}

			SimClock::TimeSpanType dT; 
			
			double dS = pEventList->getLastEvent()->GetEventPos().distance(position);
			double avgspeed = 0.5 * ( hspeed + pEventList->getLastEvent()->GetFlightHorizontalSpeed() );
			avgspeed = ARCUnit::ConvertVelocity(avgspeed,ARCUnit::KNOT,ARCUnit::CMpS);
			dT = (SimClock::TimeSpanType)(dS / avgspeed * 100 );			


			_event->SetEventType(eventType);
			_event->setEventTime(prevTime + dT);
			_event->SetComputeTime(prevTime +dT);
			_event->SetFlightHorizontalSpeed(hspeed);
			_event->SetFlightVerticalSpeed(vspeed);
			_event->SetEntityID(pairroute->m_vfixes.at(i).get()->m_strID);
			_event->SetEventPos(position);
			pEventList->push_back(_event);	
		}

	}

}
double AirspaceModel::getCruiseSpeedHorizen(SimFlight* _flight){ // get the  cruise speed in knot
	double defspeed =  250 ; //knot
	ns_AirsideInput::vFlightPerformanceCruise* vperformCruise = m_pDAirsideInput->GetFlightPerformanceCruise();
	FlightDescStruct& fltdesc = _flight-> GetCFlight()->getLogEntry();
	for(int i=0;i<(int)vperformCruise->size();i++){
		ns_AirsideInput::CFlightPerformanceCruise  * performCruise = vperformCruise->at(i);
		FlightConstraint& fltcont = performCruise->GetFltType();
		if( fltcont.fits(fltdesc) ){
			defspeed  = 0.5 * (performCruise->getMinSpeed() + performCruise->getMaxSpeed() );
			break;
		}
	}
	return defspeed;
}    
double AirspaceModel::getApproachSpeedHorizen(SimFlight * _flight)     // the landing speed , same as approach speed knot
{
	double defspeed = 120;
	FlightDescStruct& fltdesc = _flight-> GetCFlight()->getLogEntry();
	ApproachLands  * papproach = m_pDAirsideInput->GetApproachLands();
	for(int i=0;i<(int)papproach->GetApproachLands().size();i++){
		ApproachLand papp = papproach->GetApproachLands().at(i);
		FlightConstraint& fltcont = papp.m_fltType;
		if(fltcont.fits(fltdesc)){
			defspeed = 0.5 * (papp.m_dMinSpeed + papp.m_dMaxSpeed);
			break;
		}
	}
	return defspeed;
}

double AirspaceModel::getApproachSpeedVertical(SimFlight * _flight)    // the Approach speed ,  in ft/min 
{
	return ARCUnit::ConvertVelocity( tan(3.0) * getApproachSpeedHorizen(_flight) ,ARCUnit::KNOT,ARCUnit::FEETpMIN);
}

double AirspaceModel::getTerminalSpeedHorizen(SimFlight * _flight)   //get Terminal speed
{
	double defspd = 150 ;
	ns_AirsideInput::vFlightPerformanceTerminal * vperformTerminal  = m_pDAirsideInput->GetFlightperformTerminal();
	FlightDescStruct& fltdesc = _flight->GetCFlight()->getLogEntry();
	for(int i=0;i<(int)vperformTerminal->size();i++){
		ns_AirsideInput::CFlightPerformanceTerminal * pperformTerm = vperformTerminal->at(i);
		FlightConstraint& fltcont = pperformTerm->GetFltType();
		if(fltcont.fits(fltdesc)){
			defspd = 0.5 * (pperformTerm->getMaxSpeed()  + pperformTerm->getMinSpeed()) ;
			break;
		}
	}
	return defspd;
}
airroute* AirspaceModel::getArrivalRoute(SimFlight * _flight){
	airroute * pairroute = m_vroutes.getEntity(_flight->getArrivalRoute());
	if(!pairroute && m_vroutes.size()>0) return m_vroutes.at(0).get(); 
	return pairroute;
}

airroute * AirspaceModel::getDepatureRoute(SimFlight * _flight){
	airroute * pdeproute = m_vroutes.getEntity(_flight->getDepartureRoute());
	if(!pdeproute && m_vroutes.size()>0) return m_vroutes.at(0).get();
	return pdeproute;
}

double AirspaceModel::getClimboutSpeed(SimFlight * _flight)
{
	double defspd = 250;
	std::vector<DepClimbOut> vperformClimbout  = m_pDAirsideInput->GetDepClimbOuts()->GetDepClimbOuts();
	FlightDescStruct& fltdesc = _flight->GetCFlight()->getLogEntry();
	for(int i=0;i<(int)vperformClimbout.size();i++){
		DepClimbOut performClimbout = vperformClimbout.at(i);
		FlightConstraint& fltcont = performClimbout.m_fltType;
		if(fltcont.fits(fltdesc)){
			defspd = 0.5 * (performClimbout.m_nMaxHorAccel  + performClimbout.m_nMinHorAccel) ;
			break;
		}
	}
	return defspd;
}

double AirspaceModel::getClimboutSpeedVertical(SimFlight * _flight)
{
	double defspd = 1700;
	std::vector<DepClimbOut> vperformClimbout  = m_pDAirsideInput->GetDepClimbOuts()->GetDepClimbOuts();
	FlightDescStruct& fltdesc = _flight->GetCFlight()->getLogEntry();
	for(int i=0;i<(int)vperformClimbout.size();i++){
		DepClimbOut performClimbout = vperformClimbout.at(i);
		FlightConstraint& fltcont = performClimbout.m_fltType;
		if(fltcont.fits(fltdesc)){
			defspd = 0.5 * (performClimbout.m_nMaxVerticalSpeedToEntoute + performClimbout.m_nMinVerticalSpeedToEntoute) ;
			break;
		}
	}
	return defspd;
}

NAMESPACE_AIRSIDEENGINE_END