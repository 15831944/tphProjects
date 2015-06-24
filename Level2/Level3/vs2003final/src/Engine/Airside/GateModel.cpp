#include "stdafx.h"
#include "../../Inputs/AirsideInput.h"
#include "../../AirsideInput/AirsideInput.h"
#include "../../AirsideInput/PushBack.h"
#include "../../Common/ARCUnit.h"
#include "GateModel.h"
#include "flight.h"
#include "TaxiwayModel.h"

NAMESPACE_AIRSIDEENGINE_BEGINE

GateModel::~GateModel(){

}
void GateModel::Build(CAirsideInput* _inputC, AirsideInput* _inputD)
{
	m_pAirsideInput = _inputD;
}

Gate * GateModel::getGate(SimFlight * pFlight)
{	
	/* CString strGateID = pFlight->GetCFlight()->getGate().GetIDString();
	 Gate* gate = m_vgates.getEntity(strGateID);

	 if(!gate && m_vgates.size()>0)return m_vgates.begin()->get();
	 return gate;*/
	return NULL;
}

Gate * GateModel::getArrGate(SimFlight * pFlight)
{
	//CString strGateID = pFlight->GetCFlight()->getArrGate().GetIDString();
	//Gate* arrgate = m_vgates.getEntity(strGateID);
	//if(!arrgate && m_vgates.size()>0)return m_vgates.begin()->get();
	//return arrgate;
	return NULL;
}

Gate * GateModel::getDepGate(SimFlight * pFlight)
{
	/*CString strGateID = pFlight->GetCFlight()->getDepGate().GetIDString();
	Gate * depgate = m_vgates.getEntity(strGateID);
	if(!depgate && m_vgates.size()>0)return m_vgates.begin()->get();
	return depgate;*/
	return NULL;
}

void GateModel::DistributePushback(SimFlight* _flight)
{
	FlightEvent *_event = new FlightEvent(_flight);
	FlightEventList* _FEL = _flight->getFEL();
	FlightEvent * preEvent = _FEL->getLastEvent();
	
	Gate * pgate = m_vgates.getEntity(preEvent->GetEntityID());
	if(!pgate)return;

    SimClock::TimeType evalTime = _flight->getDepartureTime();
	
	_event->SetEventType(FlightEvent::Pushback);
	_event->SetComputeTime(evalTime);
	_event->setEventTime(evalTime);
	_event->SetFlightHorizontalSpeed(0);
	_event->SetFlightVerticalSpeed(0);
	_event->SetEntityID(pgate->getStringID());
	_event->SetEventPos(pgate->m_pos);				
	_FEL->push_back(_event);	
	//
	_event = new FlightEvent(_flight);
	evalTime += getPushbackTime(_flight)*100;
	
	TaxiwayNode * pTaxiNode = m_pAirport->getTaxiwayModel()->getTaixwayNode(pgate->m_outNodeId);
	if(!pTaxiNode)return;
	
	_event->SetEventType(FlightEvent::TaxiOut);
	_event->SetComputeTime(evalTime);
	_event->setEventTime(evalTime);
	_event->SetFlightHorizontalSpeed(m_pAirport->getTaxiwayModel()->getTaxiOutSpeed(_flight));
	_event->SetEntityID(pTaxiNode->getStringID());
	_event->SetEventPos(pTaxiNode->m_pos);
	_FEL->push_back(_event);


}

bool GateModel::DistributeInGate(SimFlight * _flight){
	FlightEvent * _event = new FlightEvent(_flight);
	FlightEvent *_preEvent = _flight->getFEL()->getLastEvent();
	
	SimClock::TimeType prevTime = _preEvent->GetComputeTime();
	
	Gate * pdestgate = getGate(_flight);
	if(!pdestgate)return false;

	Point gatePos = pdestgate->m_pos;
	gatePos = _preEvent->GetEventPos() * 0.01 + gatePos  * 0.99;
	double dS = gatePos.distance(_preEvent->GetEventPos());
	double avgspd = 0.5 * (_preEvent->GetFlightHorizontalSpeed() );
	avgspd = ARCUnit::ConvertVelocity(avgspd,ARCUnit::KNOT,ARCUnit::CMpS);
	SimClock::TimeSpanType dT = SimClock::TimeSpanType(dS/avgspd * 100);

	_event->SetEventType(FlightEvent::StopInGate);
	_event->SetComputeTime(prevTime + dT);
	_event->setEventTime(prevTime+dT);
	_event->SetFlightHorizontalSpeed(0);
	_event->SetFlightVerticalSpeed(0);
	_event->SetEntityID(pdestgate->getStringID());
	_event->SetEventPos(gatePos);
	_flight->getFEL()->push_back(_event);
	//the out event
	


	return true;
}

int GateModel::getPushbackTime(SimFlight* _flight)
{
	int defTime = 120;
	FlightDescStruct& fltdesc = _flight-> GetCFlight()->getLogEntry();
	if(m_pAirsideInput->GetPushBack()){
		int num = m_pAirsideInput->GetPushBack()->GetCount();
		CPushBackNEC * ppushback;
		for (int i =0;i<num;i++)
		{
			ppushback = m_pAirsideInput->GetPushBack()->GetRecordByIdx(i);
			FlightConstraint& fltcont = ppushback->GetFltType();
			if(fltcont.fits(fltdesc))
			{
				defTime = (ppushback->GetMinTime() + ppushback->GetMaxTime()) / 2;
				break;
			}
		}
	}
	
	return defTime;
}

NAMESPACE_AIRSIDEENGINE_END