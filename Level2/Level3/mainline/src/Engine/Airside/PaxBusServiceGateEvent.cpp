#include "StdAfx.h"
#include ".\paxbusservicegateevent.h"
#include "AirsideFlightInSim.h"
#include "AirsideVehicleInSim.h"
#include "Clearance.h"
#include "AirsidePaxBusInSim.h"
#include "PaxBusParkingInSim.h"
#include "AirsideFlightInSim.h"
#include "../PaxBusBeginServerEvent.h"
#include "Engine/ARCportEngine.h"
#include "AirsidePaxBusParkSpotInSim.h"



CPaxBusServiceGateEvent::CPaxBusServiceGateEvent(AirsideVehicleInSim *pVehicle)
:CAirsideMobileElementEvent(pVehicle),
m_pVehicle(pVehicle)
{
}

CPaxBusServiceGateEvent::~CPaxBusServiceGateEvent(void)
{
}
int CPaxBusServiceGateEvent::Process()
{
	CAirsidePaxBusInSim *pPaxBus = (CAirsidePaxBusInSim *)m_pVehicle;

	AirsideFlightInSim *pFlight = m_pVehicle->GetServiceFlight();
	//move to gate

//	CPaxBusParkingInSim *pParkingPlace = pFlight->GetPaxBusParking(pPaxBus->IsServiceArrival());
	AirsidePaxBusParkSpotInSim* pParkSpotInSim = pFlight->GetPaxBusParking(pPaxBus->IsServiceArrival());
	//service
	if (pParkSpotInSim)
	{
		if(m_pVehicle->GetMode() == OnMoveToGate)
		{

			CPoint2008 ptPos = pParkSpotInSim->GetDistancePoint(0);
			m_pVehicle->GetResource()->SetExitTime(m_pVehicle,m_pVehicle->GetTime());
			ClearanceItem lastItem(m_pVehicle->GetResource(), OnMoveToGate , m_pVehicle->GetDistInResource() + m_pVehicle->GetLength() * 0.5);
			//lastItem.SetPosition(m_pVehicle->GetPosition());
			lastItem.SetTime(m_pVehicle->GetTime());
			lastItem.SetSpeed(m_pVehicle->GetSpeed());


			//move to servicePoint
			pParkSpotInSim->GetEnterParkSpotClearance(m_pVehicle,lastItem);

// 			ClearanceItem moveInItem(pParkSpotInSim,OnMoveInGate,0);
// 			moveInItem.SetSpeed(m_pVehicle->GetSpeed());
// 			moveInItem.SetPosition(ptPos);
// 			ElapsedTime eMoveInTime = m_pVehicle->GetTimeBetween(lastItem,moveInItem);
// 			moveInItem.SetTime(lastItem.GetTime() + eMoveInTime);
// 
// 			m_pVehicle->WirteLog(ptPos,m_pVehicle->GetSpeed(),moveInItem.GetTime());
		
			m_pVehicle->SetResource(pParkSpotInSim);		
			pParkSpotInSim->SetEnterTime(m_pVehicle,m_pVehicle->GetTime(),OnMoveInGate,m_pVehicle->GetSpeed());


			Terminal* pterminal = m_pVehicle->GetServiceFlight()->GetFlightInput()->GetTerminal() ;
			m_pVehicle->IsArrivedAtStand(FALSE) ;
			CPaxBusBeginServerEvent* newEvent = new CPaxBusBeginServerEvent(pPaxBus,
																			m_pVehicle->GetTime(),
																			pPaxBus->getEngine());
//			m_pVehicle->SetMode(OnArriveAtGate);
// 			m_pVehicle->SetPosition(ptPos);
// 			m_pVehicle->SetSpeed(0.0);
// 			m_pVehicle->WirteLog(ptPos,m_pVehicle->GetSpeed(),moveInItem.GetTime());
			m_pVehicle->SetMode(OnArriveAtGate);
			m_pVehicle->GenerateNextEvent(newEvent);

			//newEvent->addEvent();		
			return true;

		}
		
		if (m_pVehicle->GetMode() == OnService)
		{

// 			ClearanceItem lastItem(m_pVehicle->GetResource(), OnService , m_pVehicle->GetDistInResource());
// 			lastItem.SetPosition(m_pVehicle->GetPosition());
// 			lastItem.SetTime(getTime());
// 			lastItem.SetSpeed(0.0);

		//	pParkSpotInSim->GetExitParkSpotClearance(m_pVehicle,lastItem);
		//	m_pVehicle->WirteLog(m_pVehicle->GetPosition(),0,m_pVehicle->GetTime());
			////service
			//ClearanceItem serviceItem = lastItem;
			//double dServiceTime = 0;//m_pVehicle->GetServiceTimeDistribution()->getRandomValue();
			//ElapsedTime eServicetime = ElapsedTime((pPaxBus->GetLoadPaxCount()/10.0) *dServiceTime); 
			//serviceItem.SetTime(lastItem.GetTime() + eServicetime);
			//m_pVehicle->WirteLog(m_pVehicle->GetPosition(),m_pVehicle->GetSpeed(),serviceItem.GetTime());
			//m_pVehicle->SetTime(serviceItem.GetTime());
			
		//	m_pVehicle->SetResource(pParkSpotInSim);	
			m_pVehicle->SetSpeed(m_pVehicle->GetOnRouteSpeed());
		//	m_pVehicle->WirteLog(m_pVehicle->GetPosition(),0,m_pVehicle->GetTime());
		}
	}
	m_pVehicle->GetNextCommand();
	////leave
	//ClearanceItem leaveItem = lastItem;
	//
	//ElapsedTime eMoveTime = m_pVehicle->GetTimeBetween(lastItem,serviceItem);
	//serviceItem.SetTime(serviceItem.GetTime() + eMoveTime);
	return 0;
}