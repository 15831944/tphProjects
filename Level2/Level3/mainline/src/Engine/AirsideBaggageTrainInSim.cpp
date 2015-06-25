#include "StdAfx.h"
#include ".\airsidebaggagetraininsim.h"
#include "Airside\AirsideFlightInSim.h"
#include "BaggageTrainServiceRequest.h"
#include "Airside\VehiclePoolInSim.h"
#include "BagCartsParkingSpotInSim.h"
#include "InputAirside\AirsideBagCartParkSpot.h"
#include "Airside\VehicleRouteResourceManager.h"
#include "Airside\VehicleMoveEvent.h"
#include "Airside\FlightServiceRoute.h"
#include "AirsideBaggageCartInSim.h"
#include "Pusher.h"
#include "SimpleConveyor.h"
#include "Common\Flt_cnst.h"
#include "Inputs\MobileElemConstraint.h"
#include "Loader.h"
#include "AirsideBaggageBehavior.h"
#include "BagCartsBeginServiceParkingSpotEvent.h"
#include "Airside\FlightServiceEvent.h"
#include "AirsdieFlightBaggageManager.h"
#include "BaggageCartServiceStandtEvent.h"
#include "TERMINAL.H"
#include "BagCartsServiceParkingSpotEvent.h"

AirsideBaggageTrainInSim::AirsideBaggageTrainInSim(int id,int nPrjID,CVehicleSpecificationItem *pVehicleSpecItem, CVehicleSpecificationItem * pCartsSpecifications, CARCportEngine *_pEngine)
:AirsideBaggageTugInSim(id, nPrjID, pVehicleSpecItem)
,m_pCartsSpecifications(pCartsSpecifications)
,m_pEngine(_pEngine)
,m_nProjID(nPrjID)
{
	m_curCartIndex = 0;
	m_pTermProc = NULL;
	m_nBagLoaded = 0;
	m_nBagCount = 0;
}

AirsideBaggageTrainInSim::~AirsideBaggageTrainInSim(void)
{
}

 int  AirsideBaggageTrainInSim::GetVehicleCapacity()
{
	return static_cast<int>(m_pVehicleSpecificationItem->getCapacity() * m_pCartsSpecifications->getCapacity());
}

void AirsideBaggageTrainInSim::SetServiceCount( int nCount )
{
	m_nBagCount = nCount;
	
	m_vCartAssigned.clear();
	for(int i=0;i<(int)m_vBagCarts.size();++i)
	{
		AirsideBaggageCartInSim* cart  = m_vBagCarts.at(i);
		if(nCount>0)
		{
			m_vCartAssigned.push_back(cart);
			nCount-=cart->getCapacity();
		}
		else
			break;
	}
	ASSERT(nCount<=0);

}

void AirsideBaggageTrainInSim::SetServiceFlight( AirsideFlightInSim* pFlight, double vStuffPercent,BaggageTrainServiceRequest& request )
{
	//arrival or departure
	AirsideMobileElementMode fltMode = pFlight->GetMode();
	m_enumFltOperation = request.getFltOperation();
	m_pBagCartsSpotInSim = request.getParkingSpot();
	m_pTermProc = request.getTermPocessor();

	if (m_enumFltOperation == ARRIVAL_OPERATION)
	{
		pFlight->setArrivalBagCartsParkingSpot(m_pBagCartsSpotInSim);
		//find route to stand
		AirsideVehicleInSim::SetServiceFlight(pFlight,vStuffPercent);

	}
	else 
	{

		CString strDepGate = pFlight->getDepGateInSim();

		if (m_pBagCartsSpotInSim == NULL)
		{
			pFlight->VehicleServiceComplete(this);
			m_pServiceFlight = NULL;
			AirsideVehicleInSim::GetNextCommand();	
			return;
		}

		pFlight->setDeparturelBagCartsParkingSpot(m_pBagCartsSpotInSim);
		//go to service stand and service
		AirsideResource * AtResource = GetResource();
		if( GetResource() && GetResource()->GetType() == AirsideResource::ResType_FlightServiceRingRoute)
		{
			CFlightServiceRoute * pRoute = (CFlightServiceRoute*) GetResource();		
			AtResource = pRoute->m_pStand;		
		}

		VehicleRouteInSim resltRoute;
		if( !m_pRouteResMan->GetVehicleRoute(AtResource,m_pBagCartsSpotInSim,resltRoute) )
		{
			if( AtResource && m_pBagCartsSpotInSim )
			{
				CString strWarn;
				
				CString gateName = pFlight->getDepStand().GetIDString();

				CString resName = "UNKNOWN";
				if(AtResource->GetType() == AirsideResource::ResType_BagCartsParkingPosition)
				{
					CBagCartsParkingSpotInSim * resParkingSpot = (CBagCartsParkingSpotInSim * )AtResource;
					resName = resParkingSpot->GetBagCartsSpotInput()->GetObjNameString();
				}
				if(AtResource->GetType() == AirsideResource::ResType_VehiclePool )
				{
					VehiclePoolInSim* vehiclePool = (VehiclePoolInSim*) AtResource;
					resName = vehiclePool->GetPoolInput()->GetObjNameString();
				}

				strWarn.Format("Can't Find Route From %s to %s", resName, gateName);
				//DiagWarning(this,strWarn);

			}
		}
		else
		{
			m_Route = resltRoute;
		}

		//
		if(m_bAvailable == false)
			return;

		m_Route.SetMode(OnMoveToBagTrainSpot);
		SetMode(OnMoveToBagTrainSpot);
		m_pServiceFlight = pFlight;
		m_bAvailable = false;
		m_vStuffPercent = m_vStuffPercent - vStuffPercent;	
		VehicleMoveEvent* newEvent = new VehicleMoveEvent(this);
		if (m_pResource->GetType() == AirsideResource::ResType_VehiclePool)
		{		
			newEvent->setTime( m_tLeavePoolTime );

		}else
		{
			newEvent->setTime(GetTime());
		}

		SetMode(OnMoveToBagTrainSpot);
		WirteLog(GetPosition(),GetSpeed(),GetTime());
		GenerateNextEvent(newEvent);
		//newEvent->addEvent();
		if (m_pResource->GetType() == AirsideResource::ResType_VehiclePool)
		{		
			SetMode(OnBeginToServiceToGate);
			WirteLog(GetPosition(),GetSpeed(),m_tLeavePoolTime);

		}else
		{
			SetMode(OnBeginToServiceToGate);
			WirteLog(GetPosition(),GetSpeed(),m_tLeavePoolTime);
		}
		SetMode(OnMoveToBagTrainSpot);

	}

}
void AirsideBaggageTrainInSim::GetNextCommand()
{
	if (m_pServiceFlight == NULL)
	{
		AirsideVehicleInSim::GetNextCommand();
		return;
	}

	if(m_enumFltOperation == ARRIVAL_OPERATION)//arrival
	{

		if( GetResource() && GetResource()->GetType() == AirsideResource::ResType_BagCartsParkingPosition)
		{
			//set onbackpool in simulation log , one server has finished .
			AirsideMobileElementMode _mode = GetMode() ;
			SetMode(OnBackPool) ;
			WirteLog(GetPosition(),GetSpeed(),GetTime()) ;
			SetMode(_mode) ;
			//////////////////////////////////////////////////////////////////////////

			m_pServiceFlight = NULL;
			AirsideVehicleInSim::GetNextCommand();	
		}
		else
		{
			if (m_pBagCartsSpotInSim == NULL)
			{
				//set onbackpool in simulation log , one server has finished .
				AirsideMobileElementMode _mode = GetMode() ;
				SetMode(OnBackPool) ;
				WirteLog(GetPosition(),GetSpeed(),GetTime()) ;
				SetMode(_mode) ;
				//////////////////////////////////////////////////////////////////////////

				GetServiceFlight()->VehicleServiceComplete(this);
				m_pServiceFlight = NULL;
				AirsideVehicleInSim::GetNextCommand();	
				return;
			}

			GetServiceFlight()->setArrivalBagCartsParkingSpot(m_pBagCartsSpotInSim);
			//go to service stand and service
			AirsideResource * AtResource = GetResource();
			if( GetResource() && GetResource()->GetType() == AirsideResource::ResType_FlightServiceRingRoute)
			{
				CFlightServiceRoute * pRoute = (CFlightServiceRoute*) GetResource();		
				AtResource = pRoute->m_pStand;		
			}

			VehicleRouteInSim resltRoute;
			if( !m_pRouteResMan->GetVehicleRoute(AtResource,m_pBagCartsSpotInSim,resltRoute) )
			{
				if( AtResource && m_pBagCartsSpotInSim )
				{
					CString strWarn;
					CString gateName = GetServiceFlight()->getDepStand().GetIDString();
					CString resName = "UNKNOW";
					if(AtResource->GetType() == AirsideResource::ResType_BagCartsParkingPosition)
					{
						CBagCartsParkingSpotInSim * resParkingSpot = (CBagCartsParkingSpotInSim * )AtResource;
						resName = resParkingSpot->GetBagCartsSpotInput()->GetObjNameString();
					}
					if(AtResource->GetType() == AirsideResource::ResType_VehiclePool )
					{
						VehiclePoolInSim* vehiclePool = (VehiclePoolInSim*) AtResource;
						resName = vehiclePool->GetPoolInput()->GetObjNameString();
					}

					strWarn.Format("Can't Find Route From %s to %s", resName, gateName);
					//DiagWarning(this,strWarn);

				}
			}
			else
			{
				m_Route = resltRoute;
			}

			//
			//if(m_bAvailable == false)
			//	return;

			m_Route.SetMode(OnMoveToBagTrainSpot);
			SetMode(OnMoveToBagTrainSpot);

			m_bAvailable = false;
			IsArrivedAtStand(FALSE);
			VehicleMoveEvent* newEvent = new VehicleMoveEvent(this);
			if (m_pResource->GetType() == AirsideResource::ResType_VehiclePool)
			{		
				newEvent->setTime( m_tLeavePoolTime );

			}else
			{
				newEvent->setTime(GetTime());
			}

			SetMode(OnMoveToBagTrainSpot);
			WirteLog(GetPosition(),GetSpeed(),GetTime());
			GenerateNextEvent(newEvent);

			//newEvent->addEvent();
		}
	}
	else//departure
	{
		double vStuffPercent = 0;
		AirsideResource * AtResource = GetResource();
		if( GetResource() && GetResource()->GetType() == AirsideResource::ResType_BagCartsParkingPosition)
		{
			StandInSim* pStand = GetServiceFlight()->GetOperationParkingStand();	

			//go to service stand and service
			AirsideResource * AtResource = GetResource();
			if( GetResource() && GetResource()->GetType() == AirsideResource::ResType_FlightServiceRingRoute)
			{
				CFlightServiceRoute * pRoute = (CFlightServiceRoute*) GetResource();		
				AtResource = pRoute->m_pStand;		
			}

			VehicleRouteInSim resltRoute;
			if( !m_pRouteResMan->GetVehicleRoute(AtResource,pStand,resltRoute) )
			{
				if( AtResource && pStand )
				{
					CString strWarn;
					CString standName = pStand->GetStandInput()->GetObjNameString();
					CString resName = "UNKNOW";
					if(AtResource->GetType() == AirsideResource::ResType_Stand)
					{
						StandInSim* resStand = (StandInSim* )AtResource;
						resName = resStand->GetStandInput()->GetObjNameString();
					}
					if(AtResource->GetType() == AirsideResource::ResType_VehiclePool )
					{
						VehiclePoolInSim* vehiclePool = (VehiclePoolInSim*) AtResource;
						resName = vehiclePool->GetPoolInput()->GetObjNameString();
					}

					strWarn.Format("Can't Find Route From %s to %s", resName, standName);
					//DiagWarning(this,strWarn);

				}
			}
			else
			{
				m_Route = resltRoute;
				CBagCartsParkingSpotInSim* pBagCartsSpotInSim = (CBagCartsParkingSpotInSim*)GetResource();

				CPoint2008 ptLocation;
				if (m_Route.GetVehicleBeginPos(ptLocation))
				{
					pBagCartsSpotInSim->GetExitParkSpotClearance(this,GetTime(),ptLocation);
				}
			}

			m_Route.SetMode(OnMoveToService);
			SetMode(OnMoveToService);
			m_pServiceFlight = GetServiceFlight();
			m_bAvailable = false;
			IsArrivedAtStand(FALSE);


			m_vStuffPercent = m_vStuffPercent - vStuffPercent;	
			VehicleMoveEvent* newEvent = new VehicleMoveEvent(this);
			if (m_pResource->GetType() == AirsideResource::ResType_VehiclePool)
			{		
				newEvent->setTime( m_tLeavePoolTime );
			}else
			{
				newEvent->setTime(GetTime());
			}
			SetMode(OnMoveToService);
			WirteLog(GetPosition(),GetSpeed(),GetTime());
			GenerateNextEvent(newEvent);

			//newEvent->addEvent();
		}
		else
		{
			//set onbackpool in simulation log , one server has finished .
			AirsideMobileElementMode _mode = GetMode() ;
			SetMode(OnBackPool) ;
			WirteLog(GetPosition(),GetSpeed(),GetTime()) ;
			SetMode(_mode) ;
			//////////////////////////////////////////////////////////////////////////

			m_pServiceFlight = NULL;

			AirsideVehicleInSim::GetNextCommand();
		}
	}
}

CARCportEngine * AirsideBaggageTrainInSim::getEngine()
{
	return m_pEngine;
}

CBagCartsParkingSpotInSim * AirsideBaggageTrainInSim::getBagCartsSpotInSim() const
{
	return m_pBagCartsSpotInSim;
}

FlightOperation AirsideBaggageTrainInSim::getFlightOperation() const
{
	return m_enumFltOperation;
}
void AirsideBaggageTrainInSim::LoadBaggageFromPusher( ElapsedTime eTime )
{
	if(m_pTermProc == NULL)
		return;

	//get pusher processor
	Pusher *pPusher = NULL;
	if(m_pTermProc->getProcessorType() == ConveyorProc)
	{
		Conveyor *pSimpleConveyor = (Conveyor *) m_pTermProc;
		if(pSimpleConveyor != NULL && pSimpleConveyor->GetSubConveyorType() == PUSHER)
		{
			pPusher = (Pusher*)pSimpleConveyor->GetPerformer();
		}
	}
	if(pPusher == NULL)
		return;

	pPusher->ReportBaggageTrainArrival(this);

	AirsideFlightInSim* pFlight = GetServiceFlight();
	if(pFlight == NULL)
		return;
 
 	AirsideBaggageCartInSim *pCurCartInSim = getCurBaggageCart();
	if(!pCurCartInSim)
		return;


	//FlightConstraint fltCons =pFlight->GetFlightInput()->getType('D');
	//CMobileElemConstraint paxCons;
	//paxCons.MergeFlightConstraint(&fltCons);
	//

	CMobileElemConstraint paxCons;
	Flight* pFlightInput = pFlight->GetFlightInput();
	ASSERT(pFlightInput);
	FlightConstraint fltCons = pFlightInput->getType('D');
	paxCons.SetAirportDB(pFlightInput->GetTerminal()->m_pAirportDB);
	paxCons.setIntrinsicType(2);
	paxCons.SetTypeIndex(2);
	paxCons.MergeFlightConstraint(&fltCons);

	int nCapacity = pCurCartInSim->getCapacity();
	pPusher->ReleaseBaggageToBaggageCart(this, paxCons, nCapacity, eTime);

	//find bags belong to this flight
}
void AirsideBaggageTrainInSim::UnloadBaggageToLoader(ElapsedTime time)
{
	if(m_pTermProc == NULL)
		return;

	////get pusher processor
	//CLoader *pLoader = NULL;
	//if(m_pTermProc->getProcessorType() == ConveyorProc)
	//{
	//	CSimpleConveyor *pSimpleConveyor = (CSimpleConveyor *) m_pTermProc;
	//	if(pSimpleConveyor != NULL && pSimpleConveyor->GetSubConveyorType() == PUSHER)
	//	{
	//		pLoader = (CLoader *)pSimpleConveyor;
	//	}
	//}
	//if(pLoader == NULL)
	//	return;

	//get current cart
	AirsideBaggageCartInSim * pBagCart = getCurBaggageCart();
	if(!pBagCart)
		return;
	
	ElapsedTime eFinishedTime = time;	
	pBagCart->ReleaseBaggage(m_pTermProc, m_pBagCartsSpotInSim, eFinishedTime);

	//move to next cart
	if(isLastCart())
	{
		//finished unload baggage
		m_curCartIndex = 0;
		SetTime(eFinishedTime);
		GetNextCommand();
		return;
	}
	MoveToNextCart(eFinishedTime);
	CBagCartsBeginServiceParkingSpotEvent *pEvent = new CBagCartsBeginServiceParkingSpotEvent(this, eFinishedTime, m_pEngine);
	pEvent->addEvent();
	



}

void AirsideBaggageTrainInSim::Initialize(int& nNextVehicleUnqiueID)
{
	int nCartsCount = static_cast<int>(m_pVehicleSpecificationItem->getCapacity());
	for (int nCart = 0; nCart < nCartsCount; ++ nCart)
	{
		AirsideBaggageCartInSim  *pVehicle = new AirsideBaggageCartInSim (nNextVehicleUnqiueID,m_nProjID,m_pCartsSpecifications);
		nNextVehicleUnqiueID += 1;
		m_vBagCarts.push_back(pVehicle);
	}
}

void AirsideBaggageTrainInSim::MoveToNextCart(const ElapsedTime& eTime)
{
	m_curCartIndex += 1;
	
	//move the baggage train 
	//move the next cart to service location
	
}

AirsideBaggageCartInSim * AirsideBaggageTrainInSim::getCurBaggageCart()
{
	int nCartCount = static_cast<int>(m_vCartAssigned.size());
	ASSERT(m_curCartIndex < nCartCount);

	if(m_curCartIndex < nCartCount )
	{
		return m_vCartAssigned.at(m_curCartIndex);
	}
	return NULL;
}

void AirsideBaggageTrainInSim::TransferTheBag( Person *pBag,const ElapsedTime& eTime, ElapsedTime& retTime)
{
	AirsideBaggageBehavior *pBagBehavior = new AirsideBaggageBehavior(pBag, MOVETOBAGCART);
	pBag->setBehavior(pBagBehavior);

	AirsideBaggageCartInSim *pBagCart = getCurBaggageCart();
	ASSERT(pBagCart);

	ElapsedTime eArriveTime = eTime;
	pBagBehavior->MoveToCartFromPusher(pBagCart, eArriveTime);

	retTime = MAX(retTime,eArriveTime);
	//CPoint2008  m_pBagCartsSpotInSim->GetServicePoint();

	
	pBagCart->AddBaggage(pBagBehavior);
	m_nBagLoaded += pBag->GetActiveGroupSize();
	m_nBagLoaded++;
	if (IsReadyToGo())
	{
		//the vehicle start to leave the pusher's parking place
		if(m_pTermProc == NULL)
			return;

		//get pusher processor
		Pusher *pPusher = NULL;
		if(m_pTermProc->getProcessorType() == ConveyorProc)
		{
			Conveyor *pSimpleConveyor = (Conveyor *) m_pTermProc;
			if(pSimpleConveyor != NULL && pSimpleConveyor->GetSubConveyorType() == PUSHER)
			{
				pPusher = (Pusher*)pSimpleConveyor->GetPerformer();
			}
		}
		if(pPusher == NULL)
			return;

		pPusher->ReportBaggageTrainLeave(this);

	
		m_curCartIndex = 0;

		//train leave
		SetTime(retTime);
		WirteLog(GetPosition(),GetSpeed(),GetTime());
		SetMode(OnService);
		CBagCartsServiceParkingSpotEvent* event = new CBagCartsServiceParkingSpotEvent(this);
		event->setTime(retTime) ;
		event->addEvent() ;

	//	GetNextCommand();
		return;
	}
	if(pBagCart->IsFull())
	{
		//move the train 
		//and get the next cart ready to load
		MoveToNextCart(retTime);
		CBagCartsBeginServiceParkingSpotEvent *pEvent = new CBagCartsBeginServiceParkingSpotEvent(this, retTime, m_pEngine);
		pEvent->addEvent();
	}
	


}

bool AirsideBaggageTrainInSim::IsReadyToGo()
{
	if(m_nBagLoaded >= m_nBagCount)
		return true;


	return false;
}

int AirsideBaggageTrainInSim::ArriveAtStand( ElapsedTime time )
{
	if(!getEngine()->IsTerminal())
	{

		SetMode(OnLeaveServicePoint) ;
		CFlightServiceEvent * pNextEvent = new CFlightServiceEvent(this);
		SetTime(time + ElapsedTime(150L));
		pNextEvent->setTime(time+ ElapsedTime(150L));
		pNextEvent->addEvent();
		return 1;
	}

	if(getFlightOperation() == ARRIVAL_OPERATION)
	{
		AirsideFlightInSim* flight = GetServiceFlight() ;
		AirsideFlightBaggageManager *pBaggageManager = flight->getBaggageManager();
		pBaggageManager->GenerateBaggageIfNeed(time);
		LoadBagFromFlight(time) ;
	}
	else
	{
		UnloadBaggageFromCart(time) ;
	}



	return 1 ;
}

void AirsideBaggageTrainInSim::LoadBagFromFlight( ElapsedTime time )
{
	AirsideFlightInSim* flight = GetServiceFlight() ;

	AirsideFlightBaggageManager *pBaggageManager = flight->getBaggageManager();
	if(pBaggageManager)
	{
		ElapsedTime eFinishedTime = time;

		if(pBaggageManager->LoadBagFromFlightAtStand(this, time, eFinishedTime))
		{
			//all bags are loaded
			//move the baggage train to loader processor
// 			SetMode(OnLeaveServicePoint) ;
// 			CFlightServiceEvent * pNextEvent = new CFlightServiceEvent(this);
// 			SetTime(eFinishedTime + ElapsedTime(10L));
// 			pNextEvent->setTime(eFinishedTime+ ElapsedTime(10L));
// 			pNextEvent->addEvent();
			LeaveStandService(eFinishedTime);
		}
		else
		{
			//move to next cart
			//if it is the final cart
			//move the baggage train to loader processor
			if (isLastCart())
			{
				LeaveStandService(eFinishedTime);
			}
			else
			{
				MoveToNextCart(eFinishedTime);
				CBaggageCartServiceStandtEvent *pEvent = new CBaggageCartServiceStandtEvent(this, eFinishedTime);
				pEvent->addEvent();
			}
		}
	}
}

void AirsideBaggageTrainInSim::UnloadBaggageFromCart( ElapsedTime time )
{
	AirsideFlightInSim *pFlight = GetServiceFlight();
	if(pFlight == NULL)
		return;
	CPoint2008 ptCargoDoor;
	pFlight->getCargoDoorPosition(ptCargoDoor);
	//get current cart
	AirsideBaggageCartInSim * pBagCart = getCurBaggageCart();
	if(!pBagCart)
		return;

	ElapsedTime eFinishedTime = time;	
	pBagCart->ReleaseBaggageToFlight(ptCargoDoor, eFinishedTime);

	//move to next cart
	if(isLastCart())
	{
		//finished unload baggage
		m_curCartIndex = 0;
		SetTime(eFinishedTime);
		GetNextCommand();
		return;
	}
	MoveToNextCart(eFinishedTime);
	CBaggageCartServiceStandtEvent *pEvent = new CBaggageCartServiceStandtEvent(this, eFinishedTime);
	pEvent->addEvent();

}


//record trace


void AirsideBaggageTrainInSim::WirteLog( const CPoint2008& p, const double speed, const ElapsedTime& t, bool bPushBack /*= false*/ )
{



	__super::WirteLog(p,speed,t, bPushBack);

	DistanceUnit incDist =0;
	if(m_trace.getCount()>0)
	{
		incDist = m_trace.getPoint(0).distance3D(p);
	}
	m_trace.insertPointBeforeAt(p,0);
	//write cart log
	DistanceUnit cartPosDist = GetVehicleLength()*0.5;
	for(int i=0;i<(int)m_vCartAssigned.size();i++)
	{
		AirsideBaggageCartInSim* cart = m_vCartAssigned.at(i);
		DistanceUnit cartlen = cart->GetVehicleLength();

		
		CPath2008 cartIncPath = m_trace.GetSubPath(cartPosDist+cartlen*0.5,cartPosDist+cartlen*0.5 + incDist);
		cartIncPath.invertList();

		for(int j=1;j<cartIncPath.getCount();j++)
		{	
			DistanceUnit distInPt = cartIncPath.GetIndexDist(j);
			ElapsedTime midT = t - ElapsedTime(incDist/speed -distInPt/speed);


			cart->SetPosition( cartIncPath.getPoint(j) );
			cart->SetSpeed(speed);
			cart->SetTime( midT );

			cart->WirteLog( cartIncPath.getPoint(j), speed, midT, bPushBack);
		}

		cartPosDist += cartlen;
		
	}

	m_trace.Shrink(cartPosDist);
}

//void AirsideBaggageTrainInSim::SetOutput( OutputAirside * pOut )
//{
//	m_pOutput = pOut;
//	for(int i=0;i<(int)m_vBagCarts.size();i++)
//	{
//		AirsideBaggageCartInSim* cart = m_vBagCarts.at(i);
//		cart->SetOutput(pOut);
//	}
//}

void AirsideBaggageTrainInSim::SetPosition( const CPoint2008& pos )
{
	__super::SetPosition(pos);
}

void AirsideBaggageTrainInSim::SetSpeed( double speed )
{
	__super::SetSpeed(speed);
	for(int i=0;i<(int)m_vCartAssigned.size();i++)
	{
		AirsideBaggageCartInSim* cart = m_vCartAssigned.at(i);
		cart->SetSpeed(speed);
	}
}

void AirsideBaggageTrainInSim::FlushLog( const ElapsedTime& endTime )
{
	__super::FlushLog(endTime);
	for(int i=0;i<(int)m_vBagCarts.size();i++)
	{
		AirsideBaggageCartInSim* cart = m_vBagCarts.at(i);
		cart->FlushLog(endTime);
	}
}

bool AirsideBaggageTrainInSim::isLastCart() const
{
	int nCount = (int)m_vCartAssigned.size();
	if (m_curCartIndex == nCount - 1)
	{
		return true;
	}
	return false;
}
void AirsideBaggageTrainInSim::InitLogEntry( VehiclePoolInSim*pool, OutputAirside * pOut )
{
	__super::InitLogEntry(pool, pOut);
	for(int i=0;i<(int)m_vBagCarts.size();i++)
	{
		AirsideBaggageCartInSim* cart = m_vBagCarts.at(i);
		cart->InitLogEntry(pool, pOut);
	}
}

void AirsideBaggageTrainInSim::LeaveStandService( const ElapsedTime& leaveTime )
{
	SetMode(OnLeaveServicePoint) ;
	m_curCartIndex = 0;
	CFlightServiceEvent * pNextEvent = new CFlightServiceEvent(this);
	SetTime(leaveTime + ElapsedTime(10L));
	pNextEvent->setTime(leaveTime+ ElapsedTime(10L));
	pNextEvent->addEvent();
}

bool AirsideBaggageTrainInSim::CanServe(const CMobileElemConstraint& mobCons )
{
	AirsideFlightInSim *pServiceFlight = GetServiceFlight();
	if(pServiceFlight == NULL)
		return false;

	if(pServiceFlight->IsArrivingOperation())
	{
		CMobileElemConstraint paxCons;
		Flight* pFlightInput = pServiceFlight->GetFlightInput();
		ASSERT(pFlightInput);
		FlightConstraint fltCons = pFlightInput->getType('A');
		paxCons.SetAirportDB(pFlightInput->GetTerminal()->m_pAirportDB);
		paxCons.MergeFlightConstraint(&fltCons);
		paxCons.setIntrinsicType(1);
		paxCons.SetTypeIndex(2);

		if(paxCons.fits(mobCons))
			return true;

	}
	else if (pServiceFlight->IsDepartingOperation())
	{
		CMobileElemConstraint paxCons;
		Flight* pFlightInput = pServiceFlight->GetFlightInput();
		ASSERT(pFlightInput);
		FlightConstraint fltCons = pFlightInput->getType('D');
		paxCons.SetAirportDB(pFlightInput->GetTerminal()->m_pAirportDB);
		paxCons.setIntrinsicType(2);
		paxCons.SetTypeIndex(2);
		paxCons.MergeFlightConstraint(&fltCons);

		if(paxCons.fits(mobCons))
			return true;


	}

	return false;
}

































