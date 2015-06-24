#include "StdAfx.h"
#include ".\runwaycrossspecificationinsim.h"
#include "RunwayInSim.h"
#include "../../InputAirside/WaveCrossingSpecification.h"
#include "AirsideFlightInSim.h"
#include "RemoveAirsideMobileElementEventEvent.h"
#include "WakeUpFlightWaitingTakeOffEvent.h"
#include "WakeUpFlightWaitingLandingEvent.h"
#include "WakeUpRunwayEvent.h"
#include "AirsideFlightInSim.h"
#include "AirspaceResource.h"
#include "../../Common/CARCUnit.h"
#define _DEBUGWAVECROSSLOG 0

//#include "../../Database/DBElementCollection_Impl.h"

CRunwayCrossSpecificationInSim::CRunwayCrossSpecificationInSim(RunwayInSim *pRunway)
{
	ASSERT(pRunway);
	m_pRunwayInSim = pRunway;
	//user settings
	m_pWaveCrossSpec = NULL;
}

CRunwayCrossSpecificationInSim::~CRunwayCrossSpecificationInSim(void)
{
}
//CRunwayCrossSpecificationInSim::CheckNeedWaveCross() structure

	//get next flight in the queue

	//if landing

		//check wave cross state
			
			//if minimum

				//check hold short position's queue
			
			//else
				
				//check queue is full or not

	//if take off
		
		//check  wave cross state
			
			//if minimum
				
				//check distance of next flight from current position to touch down or time to touch down

			//else
				
				//suspend landing or take off flight, do wave cross operation
BOOL CRunwayCrossSpecificationInSim::CheckNeedWaveCross(AirsideFlightInSim *pCurFlight,const ElapsedTime& eEventTime)
{
	if(m_pRunwayInSim == NULL)
		return FALSE;

	if (pCurFlight == NULL)
		return FALSE;

	if (m_pWaveCrossSpec == NULL)
		return FALSE;

	//all ready in wave crossing
	if(m_pRunwayInSim->GetRunwayState() == RunwayInSim::RunwayState_WaveCross)
		return FALSE;

#ifdef _DEBUGWAVECROSSLOG


	CString strFlightUID;
	strFlightUID.Format(_T("%d  "),pCurFlight->GetUID());

	ofsstream echoFile ("d:\\wavecross.log", stdios::app);
	echoFile<<"CRunwayCrossSpecificationInSim::CheckNeedWaveCross: "
		<<"flight id: "<<strFlightUID
		<<"flight time: "<<pCurFlight->GetTime().printTime()
		<<"event time: "<<eEventTime.printTime()<<"\n";
	echoFile.close();
#endif



	//get next flight in the queue
	std::vector<AirsideFlightInSim *> vAfterFlight =	m_pRunwayInSim->GetNextFlightAfter(pCurFlight);
	if(vAfterFlight.size() == 0)
	{
		CheckTakeOffWaveCross(pCurFlight,NULL,vAfterFlight,eEventTime);
		CheckLandWaveCross(pCurFlight,NULL,vAfterFlight,eEventTime);
		return FALSE;
	}
	
	AirsideFlightInSim *pNextFlight = vAfterFlight.at(0);
	if(pNextFlight == NULL)
		return FALSE;

	BOOL bNextFlightIsLanding = FALSE;
	if(pNextFlight->GetMode() < OnTaxiToRunway)
		bNextFlightIsLanding = TRUE;
	
	//if landing
	if(bNextFlightIsLanding)
	{
		CheckLandWaveCross(pCurFlight,pNextFlight,vAfterFlight,eEventTime);

	}
	else//if take off
	{	
		CheckTakeOffWaveCross(pCurFlight,pNextFlight,vAfterFlight,eEventTime);
	}

	return FALSE;
}

void CRunwayCrossSpecificationInSim::AddHoldPositionInSim( HoldPositionInSim *pHoldPositionInSim )
{
	ASSERT(pHoldPositionInSim != NULL);
	if (pHoldPositionInSim != NULL)
	{
		pHoldPositionInSim->SetWaveCrossHold(true);
		m_vHoldPositionInSim.push_back(pHoldPositionInSim);
	}
}

void CRunwayCrossSpecificationInSim::SetWaveCrossSpecificationItem( CWaveCrossingSpecificationItem * pWaveCrossSpecItem )
{
	ASSERT(pWaveCrossSpecItem != NULL);
	m_pWaveCrossSpec = pWaveCrossSpecItem;
}

BOOL CRunwayCrossSpecificationInSim::NotifyWaveCross( AirsideFlightInSim *pCurFlight,const ElapsedTime& eTime )
{
	return TRUE;
}

BOOL CRunwayCrossSpecificationInSim::CheckLandWaveCross( AirsideFlightInSim *pCurFlight,AirsideFlightInSim *pNextFlight,std::vector<AirsideFlightInSim *>& vAfterFlight,const ElapsedTime& eEventTime )
{
	//check wave cross state
	BOOL bMinimum = m_pWaveCrossSpec->GetLandingSettings().GetMinimum();
	//if minimum
	if(bMinimum)
	{
#ifdef _DEBUGWAVECROSSLOG
		CString strFlightUID;
		if(pNextFlight != NULL)
			strFlightUID.Format(_T("%d  "),pNextFlight->GetUID());
		else
			strFlightUID.Format(_T("Have no next flight "));

		ofsstream echoFile ("d:\\wavecross.log", stdios::app);
		echoFile<<"CheckLandWaveCross: "
			<<"add wait flight event,minimum, flight id: "<<strFlightUID
			<<"   time: "<<eEventTime.printTime()<<"\n";
		echoFile.close();
#endif

		//check distance of next flight from current position to touch down or time to touch down
		//get next 

		//check hold short position's queue
		BOOL bHasWaitingFlight = FALSE;
		std::vector<HoldPositionInSim *>::iterator iter = m_vHoldPositionInSim.begin();
		for (; iter != m_vHoldPositionInSim.end(); ++iter)
		{
			if((*iter)->GetQueueLength() > 0)//have no flight waiting in the hold position
			{
				bHasWaitingFlight = TRUE;
				break;
			}
		}
		#ifdef _DEBUGWAVECROSSLOG
				//CString strFlightUID;
				//if(pNextFlight != NULL)
				//	strFlightUID.Format(_T("%d  "),pNextFlight->GetUID());
				//else
				//	strFlightUID.Format(_T("Have no next flight "));

				ofsstream echoFile1 ("d:\\wavecross.log", stdios::app);
				echoFile1<<"CheckLandWaveCross: "
					<<"add wait flight event,minimum, flight id: "<<strFlightUID
					<<"   time: "<<eEventTime.printTime()
					<<"   minimum: "<<(int)bHasWaitingFlight<<"\n";
				echoFile1.close();
		#endif
		if(bHasWaitingFlight == FALSE)
			return FALSE;

		int nAllowMiles = m_pWaveCrossSpec->GetLandingSettings().GetMinMiles();
		int nAllowSeconds = m_pWaveCrossSpec->GetLandingSettings().GetMinSecons();
		ElapsedTime eAllowSeconds;
		eAllowSeconds.SetSecond(nAllowSeconds);
		
		double dAllowDistance =  CARCLengthUnit::ConvertLength(AU_LENGTH_NAUTICALMILE,AU_LENGTH_CENTIMETER,nAllowMiles);
		//need implement
		ElapsedTime eCurrentSecondsToTouchDown(0L);
		if(pNextFlight)
			eCurrentSecondsToTouchDown = pNextFlight->GetSTAR()->GetLatestRunwayAvalableTime(pNextFlight,eAllowSeconds,dAllowDistance);
		//if(eCurrentSecondsToTouchDown > eEventTime + eAllowSeconds)
		{	
			//calculate safe time
			ElapsedTime eRunwaySafeTime = eCurrentSecondsToTouchDown; // - eAllowSeconds;

			m_pRunwayInSim->SetLandingSafeTime(eRunwaySafeTime);


#ifdef _DEBUGWAVECROSSLOG
			CString strFlightUID;
			if(pNextFlight != NULL)
				strFlightUID.Format(_T("%d  "),pNextFlight->GetUID());
			else
				strFlightUID.Format(_T("Have no next flight "));

			ofsstream echoFile ("d:\\wavecross.log", stdios::app);
			echoFile<<"CheckLandWaveCross: "
				<<"add wait flight event,minimum, flight id: "<<strFlightUID
				<<"  runway safe time: "<<eRunwaySafeTime.printTime()
				<<"   time: "<<eEventTime.printTime()<<"\n";

			echoFile.close();
#endif

			//wake up the queue
			iter = m_vHoldPositionInSim.begin();
			for (; iter != m_vHoldPositionInSim.end(); ++iter)
			{
				if(*iter == NULL)
					continue;

				IntersectionNodeInSim *pNode = (*iter)->GetNode();
				ElapsedTime eWaitingEventTime;
				if(pNode)
				{
					OccupancyInstance occupancyInstance =  pNode->GetOccupyInstance(pCurFlight);
					eWaitingEventTime = occupancyInstance.GetExitTime();
				}
				AirsideFlightInSim *pWaitFlight =(*iter)->GetFirstFlightOfQueue();
				if(pWaitFlight)
				{
					//m_pRunwayInSim->AddWaveCrossFlight(pWaitFlight);
					CWakeUpFlightWaitingLandingEvent *pWakeUpEvent = new CWakeUpFlightWaitingLandingEvent(pWaitFlight);
					pWakeUpEvent->setTime(eWaitingEventTime);
					pWaitFlight->SetTime(eWaitingEventTime);
					pWakeUpEvent->addEvent();

#ifdef _DEBUGWAVECROSSLOG
					CString strFlightUID;
					strFlightUID.Format(_T("%d  "),pWaitFlight->GetUID());

					ofsstream echoFile ("d:\\wavecross.log", stdios::app);
					echoFile<<"CheckLandWaveCross: "
						<<"add wait flight event,minimum, flight id: "<<strFlightUID
						<<"  runway safe time: "<<eRunwaySafeTime.printTime()
						<<"   time: "<<eEventTime.printTime()<<"\n";

					echoFile.close();
#endif

				}
			}
		}

	}
	else//else
	{
		//suspend landing or take off flight, do wave cross operation
		int nAllowedWaves = m_pWaveCrossSpec->GetLandingSettings().GetAllowWaves();
		//check hold short position's queue
		BOOL bNeedWaitingFlight = FALSE;
		std::vector<HoldPositionInSim *>::iterator iter = m_vHoldPositionInSim.begin();
		for (; iter != m_vHoldPositionInSim.end(); ++iter)
		{
			if((*iter)->GetQueueLength() > nAllowedWaves)//have no flight waiting in the hold position
			{
				bNeedWaitingFlight = TRUE;
				break;
			}
		}

	#ifdef _DEBUGWAVECROSSLOG
			CString strFlightUID;
			if(pNextFlight != NULL)
				strFlightUID.Format(_T("%d  "),pNextFlight->GetUID());
			else
				strFlightUID.Format(_T("Have no next flight "));

			ofsstream echoFile ("d:\\wavecross.log", stdios::app);
			echoFile<<"CheckLandWaveCross: "
				<<"add wait flight event,minimum, flight id: "<<strFlightUID
				<<"   time: "<<eEventTime.printTime()
				<<"   maxinum: "<< (int)bNeedWaitingFlight<<"\n";
			echoFile.close();
	#endif
		if(bNeedWaitingFlight == FALSE)
			return FALSE;

		//
		int nSeconds = m_pWaveCrossSpec->GetLandingSettings().GetAllowSeconds();


		//add waiting flight
		//m_pRunwayInSim->AddWaitingFlight(pNextFlight);
		//AddAfterFlightToRunwayWaitingFlight(vAfterFlight,eEventTime);

		AirsideFlightInSim *pLastFlightCannotHold = pCurFlight;
		std::vector<AirsideFlightInSim *>::iterator iterAfterFlight = vAfterFlight.begin();
		for (;iterAfterFlight != vAfterFlight.end(); ++iterAfterFlight)
		{
			AirsideFlightInSim *pNextLandFlight = *iterAfterFlight;
			if(pNextLandFlight == NULL)
				continue;
	
			if(pNextLandFlight->GetMode() <= OnFinalApproach)
			{
				//cannot wait in hold
				if(pNextLandFlight->WaitingInHold(ElapsedTime(10.0)) == false)
				{
					pCurFlight = pNextLandFlight;
					continue;
				}
				ClearanceItem landFltItem(pNextLandFlight->GetResource(),pNextLandFlight->GetMode(),pNextLandFlight->GetDistInResource());
				landFltItem.SetTime(pNextLandFlight->GetTime());
				landFltItem.SetSpeed(pNextLandFlight->GetSpeed());
				landFltItem.SetPosition(pNextLandFlight->GetPosition());
				pNextLandFlight->StartDelay(landFltItem, NULL, FlightConflict::HOLD_AIR, FlightConflict::RUNWAY,FltDelayReason_AirHold,_T("Holding delay"));	//holding delay
			}
			else 
			{
				pLastFlightCannotHold = pNextLandFlight;
				continue;
			}

			//clear next flights' occupancy table in runway
			m_pRunwayInSim->RemoverRunwayOccupancyInfo(pNextLandFlight);


			//delete pNextFlight write log event
			CRemoveAirsideMobileElementEventEvent *pRemoveEvent = 
				new CRemoveAirsideMobileElementEventEvent(pNextLandFlight);
			pRemoveEvent->setTime(eEventTime);
			pRemoveEvent->addEvent();

			//add this flight to wait list
			m_pRunwayInSim->AddWaitingFlight(pNextLandFlight);
			#ifdef _DEBUGWAVECROSSLOG

						CString strFlightUID;
						strFlightUID.Format(_T("%d  "),pNextLandFlight->GetUID());

						ofsstream echoFile ("d:\\wavecross.log", stdios::app);
						echoFile<<"CheckLandWaveCross: "
							<<"add wait flight event,maximum, flight id: "<<strFlightUID
							<<"   time: "<<eEventTime.printTime()<<"\n";
						echoFile.close();
			#endif


		}


		CWakeUpRunwayEvent *pWakeUpRunwayEvent = new CWakeUpRunwayEvent(NULL,m_pRunwayInSim);
		ElapsedTime eWakeUpTime;
		eWakeUpTime.set(nSeconds);
		eWakeUpTime += eEventTime;
		pWakeUpRunwayEvent->setTime(eWakeUpTime);
		pWakeUpRunwayEvent->addEvent();
		m_pRunwayInSim->SetWakeupRunwayEvent(pWakeUpRunwayEvent);

		//set runway's state to cross runway
		m_pRunwayInSim->SetRunwayState(RunwayInSim::RunwayState_WaveCross);
		
		//wake up the queue
		iter = m_vHoldPositionInSim.begin();
		for (; iter != m_vHoldPositionInSim.end(); ++iter)
		{
			if(*iter == NULL)
				continue;

			IntersectionNodeInSim *pNode = (*iter)->GetNode();
			ElapsedTime eWaitingEventTime;
			if(pNode)
			{
				OccupancyInstance occupancyInstance =  pNode->GetOccupyInstance(pLastFlightCannotHold);
				eWaitingEventTime = occupancyInstance.GetExitTime();
			}

			int nQueueLength = (*iter)->GetQueueLength();
			if(nQueueLength > nAllowedWaves)
				nQueueLength = nAllowedWaves ;

			AirsideFlightInSim *pWaitFlight =(*iter)->GetFirstFlightOfQueue();
			if(pWaitFlight)
			{
				m_pRunwayInSim->AddWaveCrossHold(*iter,nQueueLength);
				//m_pRunwayInSim->AddWaveCrossFlight(pWaitFlight);
				CWakeUpFlightWaitingLandingEvent *pWakeUpEvent = new CWakeUpFlightWaitingLandingEvent(pWaitFlight);
				pWakeUpEvent->setTime(eWaitingEventTime);
				pWaitFlight->SetTime(eWaitingEventTime);
				pWakeUpEvent->addEvent();
			}
//#ifdef _DEBUGWAVECROSSLOG
//			CString strFlightUID;
//			if(pWaitFlight)
//				strFlightUID.Format(_T("%d  "),pWaitFlight->GetUID());
//
//			ofsstream echoFile ("d:\\wavecross.log", stdios::app);
//			echoFile<<"CheckTakeOffWaveCross: "
//				<<"add wait flight event, maximum, flight id: "<<strFlightUID
//				<<"   time: "<<eWaitingEventTime.printTime();
//			echoFile.close();
//
//			CString strWaitingFlight;
//#endif
//			for (int nFlight = 1; nFlight < nQueueLength; ++nFlight)
//			{
//				AirsideFlightInSim *pWaitFlight = (*iter)->GetFlightInQueue(nFlight);
//				if(pWaitFlight)
//					m_pRunwayInSim->AddWaveCrossFlight(pWaitFlight);
//#ifdef _DEBUGWAVECROSSLOG
//				CString strFlightUID;
//				strFlightUID.Format(_T("%d  "),pWaitFlight->GetUID());
//				strWaitingFlight += strFlightUID;
//#endif
//			}
//		
//#ifdef _DEBUGWAVECROSSLOG
//		ofsstream echoFile1 ("d:\\wavecross.log", stdios::app);
//		echoFile1<<"add flight id: "<< strWaitingFlight<<"\n";
//		echoFile1.close();
//#endif
		
		}
	}
	return TRUE;
}

BOOL CRunwayCrossSpecificationInSim::CheckTakeOffWaveCross( AirsideFlightInSim *pCurFlight,AirsideFlightInSim *pNextFlight,std::vector<AirsideFlightInSim *>& vAfterFlight,const ElapsedTime& eEventTime )
{
	
	#ifdef _DEBUGWAVECROSSLOG

	CString strFlightUID;
	if(pNextFlight== NULL)
		strFlightUID = "have no next flight";
	else
		strFlightUID.Format(_T("%d  "),pCurFlight->GetUID());
	ofsstream echoFile ("d:\\wavecross.log", stdios::app);
	echoFile<<"CheckTakeOffWaveCross: "
		<<"flight id: "<< strFlightUID
		<<"   time: "<<pCurFlight->GetTime().printTime().GetBuffer()<<"\n";
	echoFile.close();
	#endif


	//check  wave cross state
	BOOL bMinimum = m_pWaveCrossSpec->GetTakeoffSettings().GetMinimum();
	//if minimum
	if (bMinimum)
	{
		//check hold short position's queue
		std::vector<HoldPositionInSim *>::iterator iter = m_vHoldPositionInSim.begin();
		for (; iter != m_vHoldPositionInSim.end(); ++iter)
		{
			if((*iter)->GetQueueLength() == 0)// not fit the conditioning
				return FALSE;
		}

#ifdef _DEBUGWAVECROSSLOG

		CString strFlightUID;
		if(pNextFlight== NULL)
			strFlightUID = "have no next flight";
		else
			strFlightUID.Format(_T("%d  "),pCurFlight->GetUID());
		ofsstream echoFile ("d:\\wavecross.log", stdios::app);
		echoFile<<"CheckTakeOffWaveCross: "
			<<"flight id: "<< strFlightUID
			<<"   time: "<<pCurFlight->GetTime().printTime().GetBuffer()
			<<"  minimum: TRUE"<<"\n";
		echoFile.close();
#endif
		//all queue has one flight 
		//do runway occupancy operation
		//wakeup
		
		//static int nTestFlight = 0;
		//nTestFlight += 1;
		//if(nTestFlight < 2)
		//	return FALSE;

		//set runway state, delay next flight
		m_pRunwayInSim->SetRunwayState(RunwayInSim::RunwayState_WaveCross);

		AddAfterFlightToRunwayWaitingFlight(vAfterFlight,eEventTime);

		//wake up the flight waiting in the node

		iter = m_vHoldPositionInSim.begin();
		for (; iter != m_vHoldPositionInSim.end(); ++iter)
		{
			if(*iter == NULL)
				continue;

			IntersectionNodeInSim *pNode = (*iter)->GetNode();
			ElapsedTime eWaitingEventTime;
			if(pNode)
			{
				OccupancyInstance occupancyInstance =  pNode->GetOccupyInstance(pCurFlight);
				eWaitingEventTime = occupancyInstance.GetExitTime();
			}
			AirsideFlightInSim *pWaitFlight =(*iter)->GetFirstFlightOfQueue();
			if(pWaitFlight)
			{

				//m_pRunwayInSim->AddWaveCrossFlight(pWaitFlight);
				m_pRunwayInSim->AddWaveCrossHold(*iter,1);
				#ifdef _DEBUGWAVECROSSLOG

				CString strFlightUID;
				strFlightUID.Format(_T("%d  "),pWaitFlight->GetUID());

				ofsstream echoFile ("d:\\wavecross.log", stdios::app);
				echoFile<<"CheckTakeOffWaveCross: "
					<<"add wait flight event,minimum, flight id: "<<strFlightUID
					<<"   time: "<<eWaitingEventTime.printTime()<<"\n";
				echoFile.close();
				#endif

				CWakeUpFlightWaitingTakeOffEvent *pWakeUpEvent = new CWakeUpFlightWaitingTakeOffEvent(pWaitFlight);
				pWakeUpEvent->setTime(eWaitingEventTime);
				pWaitFlight->SetTime(eWaitingEventTime);
				pWakeUpEvent->addEvent();


			}
		}
	}
	else//else
	{
		//check queue is full or not
		BOOL bIsQueueFull = FALSE;
		std::vector<HoldPositionInSim *>::iterator iter = m_vHoldPositionInSim.begin();
		for (; iter != m_vHoldPositionInSim.end(); ++iter)
		{
			if((*iter)->IsQueueFull())// not fit the conditioning
			{
				bIsQueueFull = TRUE;
				break;
			}
		}
		//static int nTestCode = 0;
		//nTestCode += 1;

		//if(nTestCode == 3)
		//	bIsQueueFull = true;

		//if(nTestCode ==5)
		//	bIsQueueFull = false;

#ifdef _DEBUGWAVECROSSLOG

		CString strFlightUID;
		if(pNextFlight== NULL)
			strFlightUID = "have no next flight";
		else
			strFlightUID.Format(_T("%d  "),pCurFlight->GetUID());
		ofsstream echoFile ("d:\\wavecross.log", stdios::app);
		echoFile<<"CheckTakeOffWaveCross: "
			<<"flight id: "<< strFlightUID
			<<"   time: "<<pCurFlight->GetTime().printTime().GetBuffer()
			<<"  Maximum: "<<(int)bIsQueueFull<<"\n";
		echoFile.close();
#endif

		if(bIsQueueFull == FALSE)
			return FALSE;

		m_pRunwayInSim->SetRunwayState(RunwayInSim::RunwayState_WaveCross);

		AddAfterFlightToRunwayWaitingFlight(vAfterFlight,eEventTime);

		int nAllowedWaves = m_pWaveCrossSpec->GetTakeoffSettings().GetAllowWaves();

		iter = m_vHoldPositionInSim.begin();
		for (; iter != m_vHoldPositionInSim.end(); ++iter)
		{
			if(*iter == NULL)
				continue;

			IntersectionNodeInSim *pNode = (*iter)->GetNode();
			ElapsedTime eWaitingEventTime;
			if(pNode)
			{
				OccupancyInstance occupancyInstance =  pNode->GetOccupyInstance(pCurFlight);
				eWaitingEventTime = occupancyInstance.GetExitTime();
			}
			int nQueueLength = (*iter)->GetQueueLength();
			if(nQueueLength > nAllowedWaves)
				 nQueueLength = nAllowedWaves ;

			AirsideFlightInSim *pFirstWaitFlight =(*iter)->GetFirstFlightOfQueue();
			if(pFirstWaitFlight)
			{
				m_pRunwayInSim->AddWaveCrossHold(*iter,nQueueLength);
				//m_pRunwayInSim->AddWaveCrossFlight(pFirstWaitFlight);
				CWakeUpFlightWaitingTakeOffEvent *pWakeUpEvent = new CWakeUpFlightWaitingTakeOffEvent(pFirstWaitFlight);
				pWakeUpEvent->setTime(eWaitingEventTime);
				pFirstWaitFlight->SetTime(eWaitingEventTime);
				pWakeUpEvent->addEvent();


			}
//	#ifdef _DEBUGWAVECROSSLOG
//			CString strFlightUID;
//			if(pFirstWaitFlight)
//			strFlightUID.Format(_T("%d  "),pFirstWaitFlight->GetUID());
//
//				ofsstream echoFile ("d:\\wavecross.log", stdios::app);
//				echoFile<<"CheckTakeOffWaveCross: "
//					<<"add wait flight event, maximum, flight id: "<<strFlightUID
//					<<"   time: "<<eWaitingEventTime.printTime();
//				echoFile.close();
//
//				CString strWaitingFlight;
//	#endif
//			for (int nFlight = 1; nFlight < nQueueLength; ++nFlight)
//			{
//				AirsideFlightInSim *pWaitFlight = (*iter)->GetFlightInQueue(nFlight);
//				if(pWaitFlight)
//				{
//					m_pRunwayInSim->AddWaveCrossFlight(pWaitFlight);
//		#ifdef _DEBUGWAVECROSSLOG
//					CString strFlightUID;
//					strFlightUID.Format(_T("%d  "),pWaitFlight->GetUID());
//					strWaitingFlight += strFlightUID;
//		#endif
//				}
//			}
//#ifdef _DEBUGWAVECROSSLOG
//			ofsstream echoFile1 ("d:\\wavecross.log", stdios::app);
//			echoFile1<<"add flight id: "<< strWaitingFlight<<"\n";
//			echoFile1.close();
//#endif
		}
	}
	return TRUE;
}

BOOL CRunwayCrossSpecificationInSim::AddAfterFlightToRunwayWaitingFlight( std::vector<AirsideFlightInSim *>& vAfterFlight, const ElapsedTime& eEventTime)
{
	std::vector<AirsideFlightInSim *>::iterator iter = vAfterFlight.begin();
	for (;iter != vAfterFlight.end(); ++iter)
	{
		AirsideFlightInSim *pNextFlight = *iter;
		if(pNextFlight == NULL)
			continue;
		//clear next flights' occupancy table in runway
		m_pRunwayInSim->RemoverRunwayOccupancyInfo(pNextFlight);

		if(pNextFlight->GetMode() <= OnFinalApproach)
		{
			bool bCanHold = pNextFlight->WaitingInHold();
			if (bCanHold)
			{
				ClearanceItem landFltItem(pNextFlight->GetResource(),pNextFlight->GetMode(),pNextFlight->GetDistInResource());
				landFltItem.SetTime(pNextFlight->GetTime());
				landFltItem.SetSpeed(pNextFlight->GetSpeed());
				landFltItem.SetPosition(pNextFlight->GetPosition());
				pNextFlight->StartDelay(landFltItem, NULL, FlightConflict::HOLD_AIR, FlightConflict::RUNWAY,FltDelayReason_AirHold,_T("Holding delay"));	//holding delay
			}

		}
		//delete pNextFlight write log event
		CRemoveAirsideMobileElementEventEvent *pRemoveEvent = 
			new CRemoveAirsideMobileElementEventEvent(pNextFlight);
		pRemoveEvent->setTime(eEventTime);
		pRemoveEvent->addEvent();

		//add this flight to wait list
		m_pRunwayInSim->AddWaitingFlight(pNextFlight);

#ifdef _DEBUGWAVECROSSLOG
		CString strFlightUID;
		strFlightUID.Format(_T("%d  "),pNextFlight->GetUID());

		ofsstream echoFile ("d:\\wavecross.log", stdios::app);
		echoFile<<"AddAfterFlightToRunwayWaitingFlight: "
			<<"add wait flight event, maximum, flight id: "<<strFlightUID
			<<"   time: "<<eEventTime.printTime();
		echoFile.close();

		CString strWaitingFlight;
#endif

	}

	return TRUE;
}
