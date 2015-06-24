#include "StdAfx.h"
#include ".\airsidepaxbusinsim.h"
#include ".\InputAirside\VehicleSpecifications.h"
#include "AirsideFlightInSim.h"
#include "Inputs/flight.h"
#include "PaxBusParkingResourceManager.h"
#include "FlightServiceRoute.h"
#include "VehicleRouteResourceManager.h"
#include "VehiclePoolInSim.h"
#include "../SimulationDiagnoseDelivery.h"
#include "VehicleMoveEvent.h"
#include "../pax.h"
#include "FlightServiceEvent.h"
#include "PaxBusServiceGateEvent.h"
#include "../BoardingCallPolicy.h"
#include "AirTrafficController.h"
#include "../process.h"
#include "../hold.h"
#include "../AirsideMobElementBehavior.h"
#include "Engine/TerminalMobElementBehavior.h"
#include "Engine/ARCportEngine.h"




CAirsidePaxBusInSim::CAirsidePaxBusInSim( int id,int nPrjID,CVehicleSpecificationItem *pVehicleSpecItem,
										 CPaxBusParkingResourceManager * pPaxBusParkingManager,CARCportEngine *_pEngine)
:AirsideVehicleInSim(id,nPrjID,pVehicleSpecItem)
,m_pEngine(_pEngine)
{
	m_pPaxBusParkingManager = pPaxBusParkingManager;
	m_nCapacity = static_cast<int>(pVehicleSpecItem->getCapacity());
	m_bServiceArrival = false;
	m_nPaxLoad = 0;
	m_currentpaxNum= 0 ;
	m_takeoff_sepeed.setPrecisely(10);
}

CAirsidePaxBusInSim::~CAirsidePaxBusInSim(void)
{
}
long CAirsidePaxBusInSim::GetVehicleCapacity()
{
	return m_nCapacity;
}
int CAirsidePaxBusInSim::GetLoadPaxCount()
{
	return m_nPaxLoad;
}
void CAirsidePaxBusInSim::SetLoadPaxCount(int lPaxCount)
{
	m_nPaxLoad = lPaxCount;
}
bool CAirsidePaxBusInSim::IsServiceArrival()
{
	return m_bServiceArrival;
}
void CAirsidePaxBusInSim::SetServiceFlight(AirsideFlightInSim* pFlight, double vStuffPercent,bool bArrival)
{
	//arrival or departure
	AirsideMobileElementMode fltMode = pFlight->GetMode();
	m_bServiceArrival = bArrival;
	if (bArrival)
	{
		CString strArrGate =  pFlight->GetFlightInput()->getArrGate().GetIDString();
		//go to stand first  and then go to gate

		//find route to stand
		AirsideVehicleInSim::SetServiceFlight(pFlight,vStuffPercent);

	}
	else
	{

		CString strDepGate = pFlight->getDepGateInSim();

		CPaxBusParkingInSim *pPaxBusparkingInSim = m_pPaxBusParkingManager->GetBestMatch(strDepGate,depgate);
		if (pPaxBusparkingInSim == NULL)
		{
			pFlight->VehicleServiceComplete(this);
			m_pServiceFlight = NULL;
			AirsideVehicleInSim::GetNextCommand();	
			return;
		}

		pFlight->SetDepPaxBusParking(pPaxBusparkingInSim);
		//go to service stand and service
		AirsideResource * AtResource = GetResource();
		if( GetResource() && GetResource()->GetType() == AirsideResource::ResType_FlightServiceRingRoute)
		{
			CFlightServiceRoute * pRoute = (CFlightServiceRoute*) GetResource();		
			AtResource = pRoute->m_pStand;		
		}

		VehicleRouteInSim resltRoute;
		if( !m_pRouteResMan->GetVehicleRoute(AtResource,pPaxBusparkingInSim,resltRoute) )
		{
			if( AtResource && pPaxBusparkingInSim )
			{
				CString strWarn;
				CString gateName = pPaxBusparkingInSim->GetPaxParkingInput()->GetGate();
				CString resName = "UNKNOW";
				if(AtResource->GetType() == AirsideResource::ResType_PaxBusParking)
				{
					CPaxBusParkingInSim * resStand = (CPaxBusParkingInSim * )AtResource;
					resName = resStand->GetPaxParkingInput()->GetGate();
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

		m_Route.SetMode(OnMoveToGate);
		SetMode(OnMoveToGate);
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

		SetMode(OnMoveToGate);
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
		SetMode(OnMoveToGate);
}
}
void CAirsidePaxBusInSim::GetNextCommand()
{
	if (m_pServiceFlight == NULL)
	{
		AirsideVehicleInSim::GetNextCommand();
		return;
	}

	if(m_bServiceArrival)//arrival
	{
		
		if( GetResource() && GetResource()->GetType() == AirsideResource::ResType_PaxBusParking)
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
			CString strArrGate = GetServiceFlight()->getArrGateInSim();

			CPaxBusParkingInSim *pPaxBusparkingInSim = m_pPaxBusParkingManager->GetBestMatch(strArrGate,arrgate);
			
			if (pPaxBusparkingInSim == NULL)
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

			GetServiceFlight()->SetArrivalPaxBusParking(pPaxBusparkingInSim);
			//go to service stand and service
			AirsideResource * AtResource = GetResource();
			if( GetResource() && GetResource()->GetType() == AirsideResource::ResType_FlightServiceRingRoute)
			{
				CFlightServiceRoute * pRoute = (CFlightServiceRoute*) GetResource();		
				AtResource = pRoute->m_pStand;		
			}

			VehicleRouteInSim resltRoute;
			if( !m_pRouteResMan->GetVehicleRoute(AtResource,pPaxBusparkingInSim,resltRoute) )
			{
				if( AtResource && pPaxBusparkingInSim )
				{
					CString strWarn;
					CString gateName = pPaxBusparkingInSim->GetPaxParkingInput()->GetGate();
					CString resName = "UNKNOW";
					if(AtResource->GetType() == AirsideResource::ResType_PaxBusParking)
					{
						CPaxBusParkingInSim * resStand = (CPaxBusParkingInSim * )AtResource;
						resName = resStand->GetPaxParkingInput()->GetGate();
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

			m_Route.SetMode(OnMoveToGate);
			SetMode(OnMoveToGate);
		
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
	
			SetMode(OnMoveToGate);
			WirteLog(GetPosition(),GetSpeed(),GetTime());
			GenerateNextEvent(newEvent);

			//newEvent->addEvent();
		}




	
	}
	else//departure
	{
		double vStuffPercent = 0;
		AirsideResource * AtResource = GetResource();
		if( GetResource() && GetResource()->GetType() == AirsideResource::ResType_PaxBusParking)
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
void  CAirsidePaxBusInSim::TakeOnPasseneger(Person* _pax,ElapsedTime time) 
{
	if(_pax != NULL )
	{
		for (int i = 0 ; i < (int)m_pax.size() ; ++i)
		{
			if(m_pax[i] == _pax)
			{
				m_currentpaxNum ++ ;
				break ;
			}
		}
		TryTakePaxLeave(time);
	}else
		return  ;
}
void CAirsidePaxBusInSim::TakeOffPassenger(ElapsedTime time)
{
	int m_state = 0 ;
	if(IsServiceArrival())
			m_state =TAKE_OFF_BUS_TOGATE ;
	else
	   m_state = TAKE_OFF_BUS_TOFLIGHT ;//sd
	m_currentpaxNum = 0 ;
	SetLoadPaxCount(0) ;
	GeneratePaxTakeOffEvent(m_state,time) ;
}
void CAirsidePaxBusInSim::GeneratePaxTakeOffEvent(int _pax_state,ElapsedTime time)
{
	AirsideMobElementBehavior* p_behavior = NULL ;
	ElapsedTime _last_leaveTime = time ;
	for( int i = 0; i<  (int)m_pax.size() ; ++i)
	{
		p_behavior = 
			(AirsideMobElementBehavior*)m_pax[i]->getBehavior(MobElementBehavior::AirsideBehavior);
		if(p_behavior == NULL)
		{
			m_pax[i]->setBehavior( new AirsideMobElementBehavior( m_pax[i] ));
			p_behavior = (AirsideMobElementBehavior*)(m_pax[i]->getCurrentBehavior());
		}
		p_behavior->setState(_pax_state) ;

		_last_leaveTime = _last_leaveTime + m_takeoff_sepeed * long(i) ;
		m_pax[i]->generateEvent(_last_leaveTime, false);
	}
	m_pax.clear() ;
	SetLoadPaxCount(0) ;
	//,set leave time ,the time of all pax take off the bus 
	SetLeaveTime(_last_leaveTime) ;
	BusMoving(_last_leaveTime) ;
}
void CAirsidePaxBusInSim::WirteLog(const CPoint2008& p, const double speed, const ElapsedTime& t, bool bPushBack)
{
	
	AirsideMobElementBehavior* p_behavior = NULL ;
	AirsideVehicleInSim::WirteLog(p,speed,t,bPushBack) ;
	CPoint2008 point ;
	point.setX(p.getX()) ;
	point.setY(p.getY()) ;
	point.setZ(p.getZ()) ;
    
	ARCVector2 vDir(point- prePos);
	double dangle = vDir.AngleFromCoorndinateX();
	CPoint2008 realPosition ;
	for (int i = 0 ; i < (int)m_pax.size() ; ++i)
	{
		p_behavior = 
			(AirsideMobElementBehavior*)m_pax[i]->getBehavior(MobElementBehavior::AirsideBehavior);
		if(p_behavior == NULL)
		{
			m_pax[i]->setBehavior( new AirsideMobElementBehavior( m_pax[i] ));
			p_behavior = (AirsideMobElementBehavior*)(m_pax[i]->getCurrentBehavior());
		}

		CPoint2008 offsetPt = p_behavior->GetOffSetInBus();
		if(point != prePos)
		{
		 offsetPt.rotate(-(vDir.AngleFromCoorndinateX()));
		 realPosition = prePos + offsetPt ;
		 realPosition.setZ( prePos.getZ()) ;
		 p_behavior->setLocation(realPosition) ;
		 p_behavior->WriteLog(m_preTime,m_preSpeed) ;
		}
		else
		 offsetPt.rotate(getDirect(CPoint2008())) ;
		realPosition = point + offsetPt ;
		realPosition.setZ(point.getZ()) ;
		p_behavior->setLocation(realPosition) ;
		p_behavior->WriteLog(t,speed) ;
	}
	if(prePos != point)
	{
	     ARCVector2 vDir(point- prePos);
	      m_dir = -(vDir.AngleFromCoorndinateX());
		  prePos = point;
	}
    m_preTime = t ;
	m_preSpeed = speed ;
}
double CAirsidePaxBusInSim::getDirect(CPoint2008 _curPoint)
{
	
	return m_dir ;
}
int CAirsidePaxBusInSim::ArriveAtStand(ElapsedTime time)
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
	//m_pServiceFlight->SetShowStair(time);

	if(IsServiceArrival())
       NoticePaxTakeOffFlight(time) ;
	else
		TakeOffPassenger(time) ;
	return 1 ;
}
void CAirsidePaxBusInSim::NoticePaxTakeOffFlight(ElapsedTime time)
{
	AirsideFlightInSim* flight = GetServiceFlight() ;
// 	CPassengerTakeOffCallPolicy* _poilcy =  flight->GetAirTrafficController()->GetPassengerTakeOffFlightPolicy() ;
// 	_poilcy->NoticeBusHasArrived(flight->GetFlightInput(),true,time,this,getEngine()) ;
	flight->PassengerBusArrive(true,time,this);
}
void CAirsidePaxBusInSim::BusMoving(ElapsedTime time)
{
	ElapsedTime _delayTime ;
	_delayTime.SetSecond(10) ;
	time = time + _delayTime ;
	AirsideFlightInSim* flight = GetServiceFlight() ;
	ASSERT(flight != NULL);

	if(IsArrivedAtStand())
	{
// 		CPassengerTakeOffCallPolicy* _poilcy = flight->GetAirTrafficController()->GetPassengerTakeOffFlightPolicy() ;
// 		_poilcy->NoticeBusHasLeaved(flight->GetFlightInput(),true,this) ;
		flight->PassengerBusLeave(true,this);
		SetMode(OnLeaveServicePoint) ;
		CFlightServiceEvent * pNextEvent = new CFlightServiceEvent(this);
		SetTime(time);
		pNextEvent->setTime(time);
		pNextEvent->addEvent();
	}else
	{
	   SetMode(OnLeaveGate) ;
	   WirteLog(GetPosition(),GetSpeed(),time);
       SetMode(OnService) ;
       //notice holdarea bus has leaved
	   NoticeHoldingAreaBusLeave() ;
	   //notice _poilcy bus has leaved 
// 	   CBoardingCallPolicy* _policy = flight->GetAirTrafficController()->GetBoardingCall() ;
// 	   _policy->NoticeBusHasLeaved(flight->GetFlightInput(),false,this) ;
	   flight->PassengerBusLeave(false,this);
	   CPaxBusServiceGateEvent* event = new CPaxBusServiceGateEvent(this);
	   SetTime(time) ;
	   event->setTime(time) ;
	   event->addEvent() ;
	}
}

bool CAirsidePaxBusInSim::havePerson(const Person* pPerson)const
{
	if (std::find(m_pax.begin(),m_pax.end(),pPerson) != m_pax.end())
	{
		return true;
	}

	return false;
}

void CAirsidePaxBusInSim::AddPassenger(Person* _pax)
{
   if(!IsFull())
   {
	   std::vector<Person*>::iterator iter =  std::find(m_pax.begin() ,m_pax.end() ,_pax) ;
	   if(iter != m_pax.end())
		   return ;

	//   TerminalMobElementBehavior* spTerminalBehavior = (TerminalMobElementBehavior*)_pax->getBehavior(MobElementBehavior::TerminalBehavior);
	 //  if (spTerminalBehavior)
	   {
		   m_pax.push_back(_pax) ;
		  // spTerminalBehavior->SetAirsideBus(this) ;
		   m_nPaxLoad++ ;
	   }
   }
}
void CAirsidePaxBusInSim::NoticeHoldingAreaBusLeave()
{
	Terminal* _ter = NULL;
	if(GetServiceFlight())
		_ter = GetServiceFlight()->GetFlightInput()->GetTerminal() ;

	ASSERT(_ter) ;
	if(_ter == NULL)
		return;


	ProcessorArray vHoldingAreas;
	 _ter ->procList->getProcessorsOfType (HoldAreaProc, vHoldingAreas);
	int iHoldAreaCount = vHoldingAreas.getCount();
	HoldingArea* p_holdArea = NULL ;
	for (int i = 0 ; i < iHoldAreaCount ; ++i)
	{
		p_holdArea =(HoldingArea*) vHoldingAreas.getItem(i) ;
		p_holdArea->LogoutServerBus(this) ;
	}
}
BOOL CAirsidePaxBusInSim::IsFull()
{
	int num = 0 ;
	for (int i = 0 ; i < (int)m_pax.size() ;i++)
	{
	//	num++ ;
		num = num + m_pax[i]->GetAdminGroupSize();
	}
	if(num >= GetVehicleCapacity())
		return TRUE;
	else
		return FALSE ;
	//return m_pax.size() == GetVehicleCapacity();
}

CARCportEngine * CAirsidePaxBusInSim::getEngine()
{
	return m_pEngine;
}

//void CAirsidePaxBusInSim::CancelCurService(const ElapsedTime&t)
//{
//	if(!m_pax.empty())
//		return;
//	//set all pax to miss flight
//	//NoticeHoldingAreaBusLeave();
//	//cancel the service to flight
//	if(AirsideFlightInSim* pFlight = GetServiceFlight())
//	{
//		
//		pFlight->GetAirTrafficController()->GetBoardingCall()->NoticeBusCancelService(pFlight->GetFlightInput(),t,this,m_pEngine) ;
//
//		m_pCurrentPointIndex =-1;
//		pFlight->VehicleServiceComplete(this);
//		pFlight->AllVehicleFinishedServicing(t);
//	
//	}
//	//back to pool
//	AirsideVehicleInSim::GetNextCommand();
//	if(!GetServiceFlight())
//	{
//		ReturnVehiclePool(NULL);
//	}
//
//}

void CAirsidePaxBusInSim::CancelTakeOnPassenger( Person* _pax,ElapsedTime t )
{
	std::vector<Person*>::iterator itrFind = std::find(m_pax.begin(),m_pax.end(),_pax);
	if(itrFind!=m_pax.end())
	{
		m_pax.erase(itrFind);
		TryTakePaxLeave(t);
	}
}

BOOL CAirsidePaxBusInSim::TryTakePaxLeave( const ElapsedTime&t )
{
	if(  m_currentpaxNum == m_pax.size() )
		//notice the bus move 
	{
		SetLeaveTime(t) ;
		BusMoving(t) ;
		return TRUE;
	}
	return FALSE;
}
