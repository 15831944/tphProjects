#include "stdafx.h"
#include "SimDisplayer.h"
#include "Simulation.h"

#include "../../Results/AirsideSimLogs.h"
#include "../../Results/AirsideFlightLog.h"
#include "../AirsideLogDatabase.h"
#include "../../Common/AirlineManager.h"
typedef std::vector<AirsideFlightEventStruct> AirsideFlightEventList;
typedef AirsideFlightEventList::iterator flightEventList_iter;
typedef AirsideFlightEventList::const_iterator flightEventList_const_iter;

NAMESPACE_AIRSIDEENGINE_BEGINE
SimDisplayer::SimDisplayer(){

}
SimDisplayer::SimDisplayer(Simulation * pSimulation){
	m_pSimulation = pSimulation;
}
void SimDisplayer::SetSimulation(Simulation * pSimulation){
	m_pSimulation = pSimulation;
}

bool SimDisplayer::getFlightState(SimFlight* pFlight,const SimClock::TimeType& curTime, ARCVector3& pos, ARCVector3& dir ){
	FlightEventList* pEventList = pFlight->getPEL();
	for(int i =0;i<pEventList->getCount()-1;i++){
		FlightEvent * pEvent = pEventList->getItem(i);
		FlightEvent * pNextEvent = pEventList->getItem(i+1);
		
		if(curTime >= pEvent->getEventTime() && curTime< pNextEvent->getEventTime()  ){
			//interpolate the position and direction
			double ratio = double(pNextEvent->getEventTime() - curTime )/double( pNextEvent->getEventTime() - pEvent->getEventTime() );
			pos = ARCVector3(pEvent->GetEventPos()) * ratio + (1- ratio) * ARCVector3(pNextEvent->GetEventPos());
			
			dir = ARCVector3(pNextEvent->GetEventPos() - pEvent->GetEventPos());
			if(FlightEvent::Pushback == pEvent->GetEventType()) dir = -dir;
			return true;
		}
		
	}
	return false;
}

//convert simulation event to the log format
bool SimDisplayer::WirteLog(CAirsideSimLogs& simlog,int ProjectId)
{
	
	AirsideFlightLog& fltlog = simlog.m_airsideFlightLog;
	AirsideFlightLog& fltlogAnim = simlog.m_airsideFlightLogAnim;	
	fltlog.clearLogs();
	fltlogAnim.clearLogs();
	//database
	CAirsideLogDatabase airsideLogDBOpeartion;
	if (!airsideLogDBOpeartion.DeleteLogDesc(ProjectId,FlightLog))
	{
		AfxMessageBox("Error when Delete description data!");
		return false;	
	}

	if (!airsideLogDBOpeartion.DeleteLog(ProjectId,FlightLog)) 
	{
		AfxMessageBox("Error when Delete Event data!");
		return false;
	}

	long startPos = 0;
	int listsize = 0;
	for(int i =0;i<(int) m_pSimulation->getFlightList()->size();i++ )
	{
		SimFlight * pFlight = m_pSimulation->getFlightList()->at(i).get(); 
		FlightEventList* fltEvents = pFlight->getPEL(); 
		AirsideFlightEventList eventList;

		for(int ii=0;ii<fltEvents->getCount();ii++)
		{
			FlightEvent* fltevent = fltEvents->getItem(ii);
			AirsideFlightEventStruct newEvent; 

			newEvent.time = fltevent->getEventTime();
			Point pos = fltevent->GetEventPos();
			newEvent.x= float(pos.getX()); newEvent.y = float(pos.getY()); newEvent.z = float(pos.getZ());
			
			newEvent.eventCode = 'c';
			newEvent.state = 'd';
			newEvent.IsBackUp = false;
			newEvent.mode = 'a';
			
			newEvent.gateNO = 0;
			newEvent.interval = 0;
		
			if(fltevent->GetEventType() == FlightEvent::Pushback)
				newEvent.IsBackUp = true;

			eventList.push_back(newEvent);
		}
		
		AirsideFlightLogEntry entry;
		entry.SetEventLog(&(simlog.m_airsideFlightEventLogAnim));
		entry.setEventList(eventList, eventList.size());
		entry.clearLog();	
		listsize = (int)eventList.size();

		AirsideFlightLogEntry entryAnim;
		entryAnim.SetEventLog(&(simlog.m_airsideFlightEventLog));
		entryAnim.setEventList(eventList, eventList.size());
		entryAnim.clearLog();	

		char strbuf[256];
		AirsideFlightDescStruct desc;
		memset((char*)&desc, 0, sizeof(AirsideFlightDescStruct));
		Flight * pCFlight = pFlight->GetCFlight();

		pCFlight->getArrID(strbuf); 	strcpy(desc.arrID,strbuf );
		pCFlight->getDepID(strbuf);	strcpy(desc.depID, strbuf);
		pCFlight->getFullID(strbuf); strcpy(desc.flightID, strbuf);
		pCFlight->getACType(strbuf);	strcpy(desc.acType, strbuf);
		//pCFlight->isArriving()?pCFlight->getArrGate().printID(strbuf):pCFlight->getDepGate().printID(strbuf);
		pCFlight->getOrigin(strbuf); strcpy(desc.origin,strbuf);
		pCFlight->getDestination(strbuf); strcpy(desc.dest,strbuf);
		pCFlight->getAirline(strbuf);  desc.airlineNO =(short) CAirlinesManager::GetCode(strbuf);
		desc.userCode = (long)pCFlight;
		desc.startTime = pFlight->getBirthTime();
		desc.endTime = pFlight->getLastTime();

		//lo 
		desc.startPos = startPos;
		desc.endPos  = startPos + (listsize)*sizeof(AirsideFlightEventStruct);
		desc.id = i;
		
		startPos += listsize * sizeof(AirsideFlightEventStruct);
		fltlog.addItem(desc);
		fltlogAnim.addItem(desc);

		if (!airsideLogDBOpeartion.WriteLogDesc(ProjectId,(void*)&desc,FlightLog))
		{
			AfxMessageBox("Error when write description data!");
			return false;
		}
		for(int j =0 ;j<(int)eventList.size();j++){
			if(!airsideLogDBOpeartion.WriteLog(ProjectId,(void*)&desc, (void*)&eventList[j], FlightLog))
			{
				AfxMessageBox("Error when write Event data!");
				return false;
			}
		}
		
		
	}
	


return true;
}


NAMESPACE_AIRSIDEENGINE_END