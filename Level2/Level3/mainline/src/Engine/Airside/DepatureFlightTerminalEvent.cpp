#include "StdAfx.h"
#include ".\depatureflightterminalevent.h"
#include "AirsideFlightInSim.h"
#include "./StandResourceManager.h"
#include "./StandLeadInLineInSim.h"
#include "AirTrafficController.h"
#include "AirsideResourceManager.h"
#include "StandLeadOutLineInSim.h"
#include ".\flightperformancemanager.h"
#include "Engine\BridgeConnector.h"
#include "Engine\TERMINAL.H"
#include "FlightArrivalStandEvent.h"

DepatureFlightTerminalEvent::DepatureFlightTerminalEvent(AirsideFlightInSim* flt, StandResourceManager* pStandres)
:CAirsideMobileElementEvent(flt)
{
	m_pCFlight = flt;
	m_pStandResource = pStandres;
	m_bCheckedIntStand = FALSE;

}

DepatureFlightTerminalEvent::~DepatureFlightTerminalEvent(void)
{
}

int DepatureFlightTerminalEvent::process(CARCportEngine* _pEngine)
{
	Clearance theClearance;
	ElapsedTime tEndTime = time;
	m_pCFlight->DisConnectBridges(_pEngine, getTime());

	m_pCFlight->ChangeToDeparture();

	if(!m_bCheckedIntStand)
	{
		m_bCheckedIntStand = TRUE;
		ALTObjectID standID = m_pCFlight->GetFlightInput()->getIntermediateStand();
		if (!standID.IsBlank())
		{
			StandInSim * pStand = m_pStandResource->GetStandByName(standID);

			if(GetStandClearence(pStand,m_pCFlight->GetArrTime()+m_pCFlight->getServiceTime(ARR_PARKING),
				m_pCFlight->GetArrTime()+m_pCFlight->getServiceTime(ARR_PARKING) + m_pCFlight->getServiceTime(INT_PARKING), theClearance))
				tEndTime = m_pCFlight->GetArrTime()+m_pCFlight->getServiceTime(ARR_PARKING) + m_pCFlight->getServiceTime(INT_PARKING);
			m_pCFlight->PerformClearance(theClearance);
			setTime(tEndTime);
			addEvent();
			return FALSE;
		}
	}


	if (m_pCFlight->IsDeparture())
	{
		ALTObjectID standID = m_pCFlight->getDepStand();
		StandInSim * pStand = m_pStandResource->GetStandByName(standID);
		if(pStand == NULL)
			return TRUE;

		m_pCFlight->SetParkingStand(pStand,DEP_PARKING);

		ElapsedTime eDepEnplanTime = m_pCFlight->GetDepTime()-m_pCFlight->getServiceTime(DEP_PARKING);
		if(eDepEnplanTime < ElapsedTime(0L))
			eDepEnplanTime = ElapsedTime(0L);
		eDepEnplanTime = max(eDepEnplanTime,getTime());

// 		BridgeConnectEvent* pBridgeConentEvent = new BridgeConnectEvent(m_pCFlight);
// 		pBridgeConentEvent->setTime(eDepEnplanTime);
// 		pBridgeConentEvent->addEvent();
 
		WaitAtStand(pStand,eDepEnplanTime,m_pCFlight->GetDepTime(),theClearance);

		double pushbackspd =m_pCFlight->GetPerformance()->getTaxiOutNormalSpeed(m_pCFlight);

		StandLeadOutLineInSim* pOutLine = pStand->AssignLeadOutLine(m_pCFlight);
		if(pOutLine == NULL)
			return TRUE;

		double dhalfFlightLen = m_pCFlight->GetLength()*0.5;
		ClearanceItem atStandItem(pOutLine, OnHeldAtStand, dhalfFlightLen-1);
		atStandItem.SetSpeed(pushbackspd);
		DistanceUnit distOutLine = pOutLine->GetEndDist();
		ClearanceItem exitItem(pOutLine, OnExitStand,distOutLine);

		ElapsedTime dExitTime=m_pCFlight->GetTimeBetween(atStandItem,exitItem);



		if(GetStandClearence(pStand,/*eDepEnplanTime,*/m_pCFlight->GetDepTime(),m_pCFlight->GetDepTime()+dExitTime,theClearance))
			tEndTime = m_pCFlight->GetDepTime()+dExitTime;
	}

	ClearanceItem teminatItem(NULL,OnTerminate, 0);
	teminatItem.SetPushback(true);
	teminatItem.SetTime(tEndTime);
	theClearance.AddItem(teminatItem);


	m_pCFlight->PerformClearance(theClearance);
	return TRUE;
}
bool DepatureFlightTerminalEvent::WaitAtStand(StandInSim* pStand,ElapsedTime& tStartTime, ElapsedTime& tEndTime,Clearance& theClearance)
{
	if(pStand)
	{		
		double dhalfFlightLen = m_pCFlight->GetLength()*0.5;

		StandLeadOutLineInSim* pOutLine = pStand->AssignLeadOutLine(m_pCFlight);
		if(pOutLine)
		{
			m_pCFlight->PerformClearance(theClearance);
			theClearance.ClearAllItems();
			ElapsedTime tDepTime = m_pCFlight->GetTime();
			AirsideFlightState flightState;
			AirsideFlightState& curState = m_pCFlight->GetCurState();
			curState = flightState;
			ClearanceItem atStandItem(pOutLine, OnHeldAtStand, dhalfFlightLen-3);
			atStandItem.SetTime(tDepTime);
			atStandItem.SetSpeed(0);
			atStandItem.SetPushback(true);
			theClearance.AddItem(atStandItem);

			atStandItem.SetTime(tStartTime);
			atStandItem.SetDistInResource(dhalfFlightLen-2);
			theClearance.AddItem(atStandItem);

			m_pCFlight->PerformClearance(theClearance);
			theClearance.ClearAllItems();
			m_pCFlight->ConnectBridge(tStartTime);

			atStandItem.SetDistInResource(dhalfFlightLen -1);
			atStandItem.SetTime(tEndTime);
			theClearance.AddItem(atStandItem);

// 			DistanceUnit distOutLine = pOutLine->GetEndDist()- dhalfFlightLen;
			ClearanceItem atStandItem2(pOutLine, OnHeldAtStand,dhalfFlightLen);


			atStandItem2.SetSpeed(0);
			atStandItem2.SetPushback(true);

			ElapsedTime eTime = m_pCFlight->GetTimeBetween(atStandItem,atStandItem2);
			atStandItem2.SetTime( tEndTime + eTime);		
			theClearance.AddItem(atStandItem2);	
		}
		else
		{
			ClearanceItem atStandItem(pStand, OnHeldAtStand, 0);
			atStandItem.SetSpeed(0);
			atStandItem.SetPushback(true);
			atStandItem.SetTime(tStartTime);
			theClearance.AddItem(atStandItem);
			m_pCFlight->PerformClearance(theClearance);
			theClearance.ClearAllItems();
			m_pCFlight->ConnectBridge(tStartTime);

			ClearanceItem atStandItem2(pStand, OnExitStand, 1);
			atStandItem2.SetSpeed(0);
			atStandItem2.SetPushback(true);
			atStandItem2.SetTime( tEndTime);
			theClearance.AddItem(atStandItem2);
		}
	}
	return TRUE;
}
bool DepatureFlightTerminalEvent::GetStandClearence( StandInSim* pStand,ElapsedTime& tStartTime, ElapsedTime& tEndTime,Clearance& theClearance )
{
	if(pStand)
	{
		double dhalfFlightLen = m_pCFlight->GetLength()*0.5;



		//leave item
		StandLeadOutLineInSim* pOutLine = pStand->AssignLeadOutLine(m_pCFlight);
		if(pOutLine)
		{
			ClearanceItem atStandItem(pOutLine, OnHeldAtStand, dhalfFlightLen-1);
			atStandItem.SetPushback(true);
			atStandItem.SetSpeed(0);
			atStandItem.SetTime(tStartTime);
			theClearance.AddItem(atStandItem);

			atStandItem.SetDistInResource(dhalfFlightLen);
			theClearance.AddItem(atStandItem);

			DistanceUnit distOutLine = pOutLine->GetEndDist()/*- dhalfFlightLen*/;
			ClearanceItem exitItem(pOutLine, OnExitStand,distOutLine);
			
			double pushbackspd =m_pCFlight->GetPerformance()->getPushBackSpeed(m_pCFlight);
			atStandItem.SetSpeed(pushbackspd);
			exitItem.SetSpeed(0);
			exitItem.SetPushback(true);

			ElapsedTime pushBackTime = m_pCFlight->GetTimeBetween(atStandItem,exitItem);
			atStandItem.SetTime(tEndTime-pushBackTime);
			theClearance.AddItem(atStandItem);
			
			exitItem.SetTime( tEndTime);		
			theClearance.AddItem(exitItem);				
		}
		else
		{
			ClearanceItem atStandItem(pStand, OnHeldAtStand, 0);
			atStandItem.SetSpeed(0);
			atStandItem.SetPushback(true);
			atStandItem.SetTime(tStartTime);
			theClearance.AddItem(atStandItem);

			ClearanceItem exitItem(pStand, OnExitStand, 1);
			exitItem.SetSpeed(0);
			exitItem.SetPushback(true);
			exitItem.SetTime( tEndTime);
			theClearance.AddItem(exitItem);
		}


		return true;

	}

	return false;
}

int DepatureFlightTerminalEvent::Process()
{
	ASSERT(FALSE);
	return 1;
}


