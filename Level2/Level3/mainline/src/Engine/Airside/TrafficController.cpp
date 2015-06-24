#include "StdAfx.h"


#include "TrafficController.h"


#include "AirsideResource.h"

void TrafficController::ModifyOcuppyTable( FlightInAirsideSimulation * pFlight, AirsideResource * pPreRes, AirsideResource * pNextRes , const ElapsedTime& enterNextResT )
{
	
	if(pPreRes == pNextRes) {

	}
	else{
		if(pPreRes){
			pPreRes->SetExitTime(pFlight,enterNextResT);		
		}	
		if(pNextRes){
			pNextRes->SetEnterTime(pFlight,enterNextResT);
		}
	}
}