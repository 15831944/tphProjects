/*****
*
*   Class       BoardingCallEvent
*   Author      Lincoln Fellingham
*   Date        August 1994
*   Purpose     Notifies groups of departing passengers when they can
*               board their aircraft
*
*               A number of calls made for each flight, typically a
*               pre-boarding call for passengers that need assistance,
*               followed by passengers in the rearmost rows, and finally
*               forward rows and business or first class
*
*****/
#include "stdafx.h"
#include "engine\boardcal.h"
#include "inputs\schedule.h"
#include "inputs\flight.h"
#include "inputs\fltdata.h"
#include "inputs\probab.h"
#include "engine\gate.h"
#include "engine\proclist.h"
#include "common\flt_cnst.h"
#include "common\elaptime.h"
#include "engine\hold.h"
#include "common\CodeTimeTest.h"
#include "../Common/ARCTracker.h"
#include "ARCportEngine.h"
#include "terminal.h"
#include <Common/ProbabilityDistribution.h>

//A static function that creates and schedules all boarding calls.
// void BoardingCallEvent::loadBoardingCalls (const ProcessorList *p_procs, const FlightSchedule *p_flights, const FlightData *p_data, InputTerminal* _pInTerm)
// {
//     if (!p_procs->getProcessorsOfType (GateProc))
//         return;
// 
//     const HistogramDistribution *distribution;
//     CMobileElemConstraint paxType(_pInTerm);
// //    GateProcessor *gate;
//     Flight *aFlight;
//     
//     float percent;
//     BoardingCallEvent *event;
// //    int index;
// 	ProcessorArray vHoldingAreas;
// 
// 	_pInTerm->procList->getProcessorsOfType (HoldAreaProc, vHoldingAreas);
// 	int iHoldAreaCount = vHoldingAreas.getCount();
//     for (int i = 0; i < p_flights->getCount(); i++)
//     {
//         aFlight = p_flights->getItem(i);
//         if (aFlight->isDeparting())
//         {
//             ALTObjectID standID = aFlight->getDepStand();
// 			
// 			ProcessorID procID;
// 			procID.SetStrDict(_pInTerm->inStrDict);
// 			procID.setID(standID.GetIDString());
// 			if (procID.isBlank())
// 				continue;
// 			
// 
// 				//should be depgate
//             //gate = (GateProcessor *)p_procs->getProcessor (index);
// 
//          //   if (!gate)
//             //    continue;
// 
// 			std::map<int,ElapsedTime>&mapLastCalls  = aFlight->GetLastCallOfEveryStage();
// 			mapLastCalls.clear();
//             (FlightConstraint &)paxType = aFlight->getType ('D');
// 			ElapsedTime time = aFlight->getDepTime();
// 			for( int k=0; k<iHoldAreaCount; ++k )
// 			{
// 				HoldingArea* pHoldArea = ( HoldingArea* )vHoldingAreas.getItem( k );
// 				int iStage = pHoldArea->getStageID();
// 				if( iStage <= 0 )
// 					continue;
// 				if( mapLastCalls.find( iStage ) == mapLastCalls.end() )//not exsit
// 				{
// 
// //					distribution = p_data->getBoardingCalls (paxType, *(gate->getID()), iStage);
// //					distribution = p_data->getBoardingCalls (paxType, procID, iStage);
// // 					if (!distribution)
// // 						continue;
// 
// // 		            for (int j = 0; j < distribution->getCount(); j++)
// // 		            {
// // 						ElapsedTime tempTime = time;	                
// // 		                tempTime += (long) distribution->getValue(j);
// // 		                percent = (float)((float) (distribution->getProb(j)) / 100.0);
// // 
// // 						if( percent == 1.0 )
// // 						{
// // 							#ifdef _DEBUG
// // 								char temptime[32];
// // 								tempTime.printTime( temptime );
// // ///								// TRACE("\n%s\n",temptime);
// // 							#endif
// // 
// // 							if( pHoldArea->getStageID() == 1 )
// // 							{
// // 								// set time of last call
// // 								aFlight->setLastCall (tempTime);
// // 							}
// // 							
// // 							mapLastCalls.insert( std::map<int,ElapsedTime>::value_type( iStage, tempTime ) );														
// // 						}                
// // 		                event = new BoardingCallEvent;
// // 		                event->setTime (tempTime);
// // //		                event->init (percent, gate, paxType,iStage );
// // 		                event->init (percent, NULL, paxType,iStage );
// // 		                event->addEvent();			
// // 						
// // 						#ifdef _DEBUG
// // 							char temptime[32];
// // 							tempTime.printTime( temptime );
// // //							// TRACE("\n%s\n",temptime);
// // 						#endif
// // 		            }	
// 				}
// 			}
//             
//         }
//     }
// }


int BoardingCallEvent::process (CARCportEngine* _pEngine)
{
	PLACE_METHOD_TRACK_STRING();
	//gate->releaseWaitPax (percent, flight, time, m_iStage);
	Processor *aProc;
	HoldingArea *waitArea;
	if(m_mobElemConst.GetAirportDB() == NULL)
		m_mobElemConst.SetAirportDB(_pEngine->getTerminal()->m_pAirportDB) ;
	//assert( _pInTerm );
	for (int i = 0; i < _pEngine->getTerminal()->procList->getProcessorCount(); i++)
	{
		aProc = _pEngine->getTerminal()->procList->getProcessor(i);
		if (aProc->getProcessorType() == HoldAreaProc)
		{
			waitArea = (HoldingArea *) aProc;

			if( waitArea->getStageID() == m_iStage )// the interesting stage
			{
				waitArea->releasePax (m_mobElemConst, percent, time);
			}            
		}
	}
    return TRUE;
}

