#include "StdAfx.h"
#include ".\bagcartsserviceparkingspotevent.h"
#include "AirsideBaggageTrainInSim.h"
#include "BagCartsParkingSpotInSim.h"
#include "Airside\Clearance.h"
#include "Airside\AirsideFlightInSim.h"
#include "BagCartsBeginServiceParkingSpotEvent.h"

CBagCartsServiceParkingSpotEvent::CBagCartsServiceParkingSpotEvent(AirsideVehicleInSim *pVehicle)
:CAirsideMobileElementEvent(pVehicle)
{
}

CBagCartsServiceParkingSpotEvent::~CBagCartsServiceParkingSpotEvent(void)
{
}

int CBagCartsServiceParkingSpotEvent::Process()
{
	AirsideBaggageTrainInSim *pBaggageTrain = (AirsideBaggageTrainInSim *)m_pMobileElement;

	AirsideFlightInSim *pFlight = pBaggageTrain->GetServiceFlight();
	//move to gate

	//CPaxBusParkingInSim *pParkingPlace = pFlight->GetPaxBusParking(pBaggageTrain->is());
	CBagCartsParkingSpotInSim *pParkingSpotInSim = pBaggageTrain->getBagCartsSpotInSim();
	//service
	if (pParkingSpotInSim)
	{
		if(pBaggageTrain->GetMode() == OnMoveToBagTrainSpot)
		{

			CPoint2008 ptPos = pParkingSpotInSim->GetServicePoint();
			pBaggageTrain->GetResource()->SetExitTime(pBaggageTrain,pBaggageTrain->GetTime());
			ClearanceItem lastItem(pBaggageTrain->GetResource(), OnMoveToBagTrainSpot , pBaggageTrain->GetDistInResource());
			lastItem.SetPosition(pBaggageTrain->GetPosition());
			lastItem.SetTime(pBaggageTrain->GetTime());
			lastItem.SetSpeed(pBaggageTrain->GetSpeed());


			//move to servicePoint
			/*ClearanceItem moveInItem(pParkingSpotInSim,OnMoveInBagTrainSpot,0);
			moveInItem.SetSpeed(pBaggageTrain->GetSpeed());
			moveInItem.SetPosition(ptPos);
			ElapsedTime eMoveInTime = pBaggageTrain->GetTimeBetween(lastItem,moveInItem);
			moveInItem.SetTime(lastItem.GetTime() + eMoveInTime);

			pBaggageTrain->WirteLog(ptPos,pBaggageTrain->GetSpeed(),moveInItem.GetTime());

			pBaggageTrain->SetTime(moveInItem.GetTime());*/
			pParkingSpotInSim->GetEnterParkSpotClearance(pBaggageTrain,lastItem);

			pBaggageTrain->SetResource(pParkingSpotInSim);		
			pParkingSpotInSim->SetEnterTime(pBaggageTrain,pBaggageTrain->GetTime(),OnMoveInBagTrainSpot,pBaggageTrain->GetSpeed());


			Terminal* pterminal = pBaggageTrain->GetServiceFlight()->GetFlightInput()->GetTerminal() ;
			pBaggageTrain->IsArrivedAtStand(FALSE) ;
			CBagCartsBeginServiceParkingSpotEvent* newEvent = new CBagCartsBeginServiceParkingSpotEvent(pBaggageTrain,
				pBaggageTrain->GetTime(),
				pBaggageTrain->getEngine());
			pBaggageTrain->SetMode(OnArriveAtBagTrainSpot);
		/*	pBaggageTrain->SetPosition(ptPos);
			pBaggageTrain->SetSpeed(0.0);
			pBaggageTrain->WirteLog(ptPos,pBaggageTrain->GetSpeed(),moveInItem.GetTime());*/
			pBaggageTrain->GenerateNextEvent(newEvent);

			//newEvent->addEvent();		
			return true;

		}

		if (pBaggageTrain->GetMode() == OnService)
		{

	/*		ClearanceItem lastItem(pBaggageTrain->GetResource(), OnService , pBaggageTrain->GetDistInResource());
			lastItem.SetPosition(pBaggageTrain->GetPosition());
			lastItem.SetTime(getTime());
			lastItem.SetSpeed(0.0);

			pBaggageTrain->WirteLog(pBaggageTrain->GetPosition(),0,pBaggageTrain->GetTime());*/
			////service
			//ClearanceItem serviceItem = lastItem;
			//double dServiceTime = 0;//m_pVehicle->GetServiceTimeDistribution()->getRandomValue();
			//ElapsedTime eServicetime = ElapsedTime((pPaxBus->GetLoadPaxCount()/10.0) *dServiceTime); 
			//serviceItem.SetTime(lastItem.GetTime() + eServicetime);
			//m_pVehicle->WirteLog(m_pVehicle->GetPosition(),m_pVehicle->GetSpeed(),serviceItem.GetTime());
			//m_pVehicle->SetTime(serviceItem.GetTime());

		//	pBaggageTrain->SetResource(pParkingSpotInSim);	
			pBaggageTrain->SetSpeed(pBaggageTrain->GetOnRouteSpeed());
		//	pBaggageTrain->WirteLog(pBaggageTrain->GetPosition(),0,pBaggageTrain->GetTime());
		}
	}
	pBaggageTrain->GetNextCommand();
	////leave
	//ClearanceItem leaveItem = lastItem;
	//
	//ElapsedTime eMoveTime = m_pVehicle->GetTimeBetween(lastItem,serviceItem);
	//serviceItem.SetTime(serviceItem.GetTime() + eMoveTime);
	return 0;
}

int CBagCartsServiceParkingSpotEvent::kill( void )
{
	return 0;
}

const char * CBagCartsServiceParkingSpotEvent::getTypeName( void ) const
{
	return "BagCartServiceParkingSpot";
}

int CBagCartsServiceParkingSpotEvent::getEventType( void ) const
{
	return BagCartServiceParkingSpot;
}
