#include "StdAfx.h"

#include ".\airsidevehicleinsim.h"
#include "VehiclePoolInSim.h"
#include "Results/OutputAirside.h"
#include "AirsideFlightInSim.h"
#include "StandInSim.h"
#include "common/ProbabilityDistribution.h"
#include "Clearance.h"
#include "VehicleMoveEvent.h"
#include "FlightServiceEvent.h"
#include "VehicleRouteResourceManager.h"
#include "../../Common/ARCUnit.h"
#include "FlightServiceRoute.h"
#include ".\InputAirside\VehicleSpecifications.h"
#include "../SimulationDiagnoseDelivery.h"
#include "../../Common/BizierCurve.h"
#include "FlightServiceInformation.h"
#include <fstream>
#include <iostream>
#include "PaxBusServiceGateEvent.h"
#include "AirsideMobElementWriteLogItem.h"
#include "AirsideMobElementWriteLogEvent.h"
#include "../../Common/pollygon.h"
#include "StandLeadInLineInSim.h"
#include "../../Results/AirsideVehicleLogItem.h"
#include "SimulationErrorShow.h"
#include "AirsidePaxBusParkSpotInSim.h"
#include "../BagCartsParkingSpotInSim.h"
#include "../BagCartsServiceParkingSpotEvent.h"

#define _DEBUGLOG 1
//////////////////////////////////////////////////////////////////////////
#if _DEBUGLOG
class VehicleLogFile
{
public:	
	static void Log(AirsideVehicleInSim* pVehicle, const Clearance& clearance)
	{
		//log to flight file
		CString debugfileName;
		debugfileName.Format( "C:\\VehicleDebug\\%d-%d.txt",pVehicle->GetVehicleTypeID(),pVehicle->GetID() );
		std::ofstream outfile;
		outfile.open(debugfileName,std::ios::app);
		outfile << pVehicle->GetTime()<<"," << long(pVehicle->GetTime())<<std::endl;
		
		AirsideFlightInSim* pFlight =pVehicle->GetServiceFlight() ;
		if(pFlight)
		{
			outfile<<"Service Flight:"<< pFlight->GetUID() << std::endl;
		}
		else
		{
			outfile<<"Service Flight:NULL" << std::endl;
		}

		outfile << pVehicle->GetPosition().getX() <<" ," <<pVehicle->GetPosition().getY() << std::endl;
	
		outfile << clearance;
		outfile.close();
	}

	static void InitLog(AirsideVehicleInSim* pVehicle)
	{
		CString debugfileName;
		debugfileName.Format( "C:\\VehicleDebug\\%d-%d.txt",pVehicle->GetVehicleTypeID(),pVehicle->GetID() );
		std::ofstream outfile;
		outfile.open(debugfileName);
		outfile<<pVehicle->GetID()<<std::endl;
		outfile<<pVehicle->GetVehicleWidth() << "\t" << pVehicle->GetVehicleHeight() << '\t' << pVehicle->GetVehicleWidth()  << std::endl;
		outfile.close();

	}
	static void Log(AirsideVehicleInSim* pVehicle,const CPoint2008& pos , const ElapsedTime& time)
	{
		CString debugfileName;
		debugfileName.Format( "C:\\VehicleDebug\\%d-%d.txt",pVehicle->GetVehicleTypeID(),pVehicle->GetID() );
		std::ofstream outfile;
		outfile.open(debugfileName,std::ios::app);
		outfile << pVehicle->GetTime()<<"," << long(pVehicle->GetTime());
		outfile <<'('<< pos.getX() <<','<< pos.getY() <<',' <<pos.getZ()<< ')'<<std::endl;
	}
};
#endif

AirsideVehicleInSim::AirsideVehicleInSim( int id,int nPrjID,CVehicleSpecificationItem *pVehicleSpecItem)
{

	m_bAvailable = true;
	m_pServiceFlight = NULL;
	m_pVehiclePool = NULL;
	m_pOutput = NULL;

	m_vSpeed = 0;
	m_vehicleMode = OnVehicleBirth;
	m_id = id;
	m_pRouteResMan = NULL;
	m_vStuffPercent = 100.0;
	m_vSpeed = pVehicleSpecItem->getMaxSpeed()*100000/3600;

	//m_pVehicleSpecifications = new CVehicleSpecifications;
	//m_pVehicleSpecifications->ReadData(nPrjID);
	m_pVehicleSpecificationItem = pVehicleSpecItem;//m_pVehicleSpecifications->GetVehicleItemByID(desc.id);
	m_pCurrentPointIndex = -1;
	m_dist = 0.0;

	m_nVehicleTypeID =-1;
	m_nServicePointCount = 0;

#if _DEBUGLOG
	VehicleLogFile::InitLog(this);
#endif

	//m_wakeupCaller.SetVehicle(this);
	m_IsArriveStand= FALSE ;
	m_pNextEvent = NULL;

	m_bExecuteWriteLogEvent = false;

	m_pReportlogItem = NULL;

	m_pServiceTimeDistribution = NULL;
}
CPoint2008 AirsideVehicleInSim::GetRanddomPoint()
{
	Point _point[4] ;
	DOUBLE length = GetVehicleLength()*0.8;
	DOUBLE width = GetVehicleWidth()*0.8 ;
	_point[0].setX(-(length/2)) ;
	_point[0].setY(width/2) ;

	_point[1].setX(-(length/2)) ;
	_point[1].setY(-(width/2)) ;


	_point[2].setX((length/2)) ;
	_point[2].setY((width/2)) ;


	_point[3].setX((length/2)) ;
	_point[3].setY(-(width/2)) ;
    
	Pollygon randomgen ;
	randomgen.init(4,_point) ;
	CPoint2008 outPt;
	outPt.setPoint(randomgen.getRandPoint().getX(),randomgen.getRandPoint().getY(),randomgen.getRandPoint().getZ());
	return  outPt;
}


double AirsideVehicleInSim::GetVehicleRandomZ()
{
	double dHeight = GetVehicleHeight() * 0.8;
	double dRange = dHeight - 1.2;

	return 1.2 + dRange *((double)rand() / RAND_MAX);
}

AirsideVehicleInSim::~AirsideVehicleInSim(void)
{

	//delete m_pVehicleSpecifications;
	//debugfile.close();
	delete m_pReportlogItem;

}


void AirsideVehicleInSim::WakeUp(const ElapsedTime& tTime)
{		
}

void AirsideVehicleInSim::WirteLog(const CPoint2008& pos, const double speed, const ElapsedTime& t, bool bPushBack)
{
//#ifndef _DEBUG
//	if(GetResource() && GetResource()->GetType() == AirsideResource::ResType_VehiclePool) 
//		return;
//#endif 
	AirsideVehicleEventStruct newEvent;
	memset(&newEvent,0,sizeof(AirsideVehicleEventStruct));

	newEvent.x = (float) pos.getX();
	newEvent.y =(float) pos.getY();
	newEvent.z = (float)pos.getZ();
	newEvent.time = (long)t;
	newEvent.IsBackUp = bPushBack;
	
	newEvent.speed = speed;
	newEvent.mode = GetMode() ;
	newEvent.m_ServerFlightID = GetServiceFlight()?GetServiceFlight()->GetUID():-1 ;
	if(newEvent.m_ServerFlightID!=-1)
	{
		Flight *pFlightInput=GetServiceFlight()->GetFlightInput();
		if (GetServiceFlight()->IsArrival())
		{
			pFlightInput->getArrID(newEvent.serverFlightID);
			strcpy(newEvent.serveStand, pFlightInput->getArrStand().GetIDString().Left(FLIGHTIDLEN -1 ).GetBuffer());
		}else if (GetServiceFlight()->IsDeparture())
		{
			pFlightInput->getDepID(newEvent.serverFlightID);
			strcpy(newEvent.serveStand, pFlightInput->getDepStand().GetIDString().Left(FLIGHTIDLEN -1 ).GetBuffer());

		}else
		{
			pFlightInput->getFullID(newEvent.serverFlightID);
			strcpy(newEvent.serveStand,_T(""));
		}
		pFlightInput->getAirline(newEvent.serverFlightAirline);
	}else
	{
		strcpy(newEvent.serverFlightID,_T(""));
		strcpy(newEvent.serveStand,_T(""));
	}
	
	

#if _DEBUGLOG
	VehicleLogFile::Log(this, pos,t);
#endif
//
//	CAirsideVehicleWriteLogItem *pVehicleLogItem = new CAirsideVehicleWriteLogItem(this);
//	pVehicleLogItem->m_logItem = newEvent;
//
//	pVehicleLogItem->m_state.m_dist =  m_dist;
//	pVehicleLogItem->m_state.m_tCurrentTime	= m_tCurrentTime  ;
//	pVehicleLogItem->m_state.m_pPosition = m_pPosition;
//pVehicleLogItem->m_state.m_vSpeed	= m_vSpeed  ;    //cm/s
//pVehicleLogItem->m_state.m_vDirection =	m_vDirection  ;  //
//	pVehicleLogItem->m_state.m_vehicleMode  = m_vehicleMode;
//	pVehicleLogItem->m_state.m_pResource = m_pResource;
//	pVehicleLogItem->m_state.tMinTurnAroundTime = tMinTurnAroundTime;
//
//
//	pVehicleLogItem->m_state.m_vStuffPercent = m_vStuffPercent;
//	pVehicleLogItem->m_state.m_pServiceFlight = m_pServiceFlight;
//	pVehicleLogItem->m_state.m_tLeavePoolTime = m_tLeavePoolTime;
//	pVehicleLogItem->m_state.m_bAvailable = m_bAvailable;
//	pVehicleLogItem->m_state.m_nServicePointCount = m_nServicePointCount;
//	pVehicleLogItem->m_state.m_pServiceTimeDistribution = m_pServiceTimeDistribution;
//	pVehicleLogItem->m_state.m_pCurrentPointIndex = m_pCurrentPointIndex;

	//m_lstWirteLog.AddItem(pVehicleLogItem);
	//m_LogEntry.addEvent(newEvent);
	m_pOutput->LogVehicleEvent(this,newEvent);	

	m_PreEventTime = t ;
	m_PreEventPoint = pos ;
}

void AirsideVehicleInSim::FlushLog(const ElapsedTime& endTime)
{
	if(GetMode() != OnVehicleTerminate)
	{

		AirsideVehicleEventStruct newEvent;
		memset(&newEvent,0,sizeof(AirsideVehicleEventStruct));

		CPoint2008& pos = GetPosition();
		newEvent.x = (float) pos.getX();
		newEvent.y =(float) pos.getY();
		newEvent.z = (float)pos.getZ();
		newEvent.time = (long)endTime;
		newEvent.IsBackUp = false;
		newEvent.speed = 0;
		newEvent.mode = GetMode() ;
		newEvent.m_ServerFlightID = GetServiceFlight()?GetServiceFlight()->GetUID():-1 ;
#if _DEBUGLOG
		VehicleLogFile::Log(this, pos,endTime);
#endif
		m_pOutput->LogVehicleEvent(this,newEvent);

	}
		//WirteLog(GetPosition(),GetSpeed(),endTime);
	FlushLogToFile();
}

void AirsideVehicleInSim::GetNextCommand()
{
	SetAvailable(true);
	IsArrivedAtStand(FALSE);
	m_pVehiclePool->GetNextServiceRequest();
}

void AirsideVehicleInSim::ReturnVehiclePool(ProbabilityDistribution* pTurnAroundDistribute)
{	
	if(GetMode() == OnBackPool)
		return;
	
	if (pTurnAroundDistribute)
		tMinTurnAroundTime = long( pTurnAroundDistribute->getRandomValue());
	
	else
		tMinTurnAroundTime = 0L;

	//return pool and stay in pool at least tMinTurnAroundTime
	AirsideResource * AtRes = GetResource();
	if( GetResource() && GetResource()->GetType() == AirsideResource::ResType_FlightServiceRingRoute)
	{
		CFlightServiceRoute * pRoute = (CFlightServiceRoute*) GetResource();		
		AtRes = pRoute->m_pStand;		
	}
	
	if( AtRes && m_pVehiclePool )
	{
		m_pRouteResMan->GetVehicleRoute(AtRes,m_pVehiclePool,m_Route);
	}

	VehicleRouteInSim resltRoute;
	if( !m_pRouteResMan->GetVehicleRoute(AtRes,m_pVehiclePool,resltRoute) )
	{	
		CString strWarn;
		CString standName = AtRes->PrintResource();
		CString poolName = m_pVehiclePool->GetPoolInput()->GetObjNameString();
		strWarn.Format("Can't Find Route From %s to %s", standName, poolName);
		CString strError ="VEHICLE ERROR";
		AirsideSimErrorShown::VehicleSimWarning(this,strWarn,strError);
		//return;
	}
	else
	{
		m_Route = resltRoute;
		if (GetResource())
		{
			if(GetResource()->GetType() == AirsideResource::ResType_PaxBusParking)
			{
				AirsidePaxBusParkSpotInSim* pParkSpotInSim = (AirsidePaxBusParkSpotInSim*)GetResource();

				CPoint2008 ptLocation;
				if (m_Route.GetVehicleBeginPos(ptLocation))
				{
					pParkSpotInSim->GetExitParkSpotClearance(this,GetTime(),ptLocation);
				}
			}
			else if (GetResource()->GetType() == AirsideResource::ResType_BagCartsParkingPosition)
			{
				CBagCartsParkingSpotInSim * pBagCartsSpotInSim = (CBagCartsParkingSpotInSim*)GetResource();

				CPoint2008 ptLocation;
				if (m_Route.GetVehicleBeginPos(ptLocation))
				{
					pBagCartsSpotInSim->GetExitParkSpotClearance(this,GetTime(),ptLocation);
				}
			}
			
		}

	}

	m_Route.SetMode(OnBackPool);
	SetMode(OnBackPool);
	VehicleMoveEvent* newEvent = new VehicleMoveEvent(this);
	newEvent->setTime(GetTime());
	newEvent->addEvent(); 

}

void AirsideVehicleInSim::SetServiceFlight(AirsideFlightInSim* pFlight, double vStuffPercent)
{

	StandInSim* pStand = NULL;	
	if (pFlight->GetResource() && pFlight->GetResource()->GetType() == AirsideResource::ResType_Stand)
	{
		pStand = (StandInSim*)pFlight->GetResource();
	}
	else if (pFlight->GetResource() && pFlight->GetResource()->GetType() == AirsideResource::ResType_StandLeadInLine)
	{
		pStand = ((StandLeadInLineInSim*)pFlight->GetResource())->GetStandInSim();
	}
	else
	{
		pStand = pFlight->GetOperationParkingStand();
	}

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
			CString resName = "UNKNOWN";
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
			if(AtResource->GetType() == AirsideResource::ResType_VehicleStretchLane)
			{
				VehicleLaneInSim* pLane = (VehicleLaneInSim*)AtResource;
				resName = pLane->PrintResource();
			}
			if(AtResource->GetType() == AirsideResource::ResType_VehicleRoadIntersection)
			{
				VehicleRoadIntersectionInSim * pIntersect = (VehicleRoadIntersectionInSim*)AtResource;
				resName = pIntersect->GetNodeInput().GetName();
			}
			
			strWarn.Format("Can't Find Route From %s to %s", resName, standName);
			CString strError ="VEHICLE ERROR";
			AirsideSimErrorShown::VehicleSimWarning(this,strWarn,strError);
			//CFlightServiceInformation *pServiceInfo = pFlight->GetServicingInformation();
			//if (pServiceInfo)
			//{
			//	CFlightServiceRoute *pRoute = pServiceInfo->GetFlightServiceRoute();
			//	if(pRoute)
			//		pRoute->SetUnreachableVehicleType(GetVehicleTypeID());
			//}
			//return;			
		}
	}
	//else
	{
		m_Route = resltRoute;
	}

	//
	if(m_bAvailable == false)
		return;

	m_Route.SetMode(OnMoveToService);
	SetMode(OnMoveToService);

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
	SetMode(OnMoveToService) ;
	WirteLog(GetPosition(),GetSpeed(),GetTime());
	GenerateNextEvent(newEvent);
	//newEvent->addEvent();
	if (m_pResource->GetType() == AirsideResource::ResType_VehiclePool)
	{		
		SetMode(OnBeginToService);
		WirteLog(GetPosition(),GetSpeed(),m_tLeavePoolTime);
	}else
	{
		SetMode(OnBeginToService);
		WirteLog(GetPosition(),GetSpeed(),GetTime());
	}
	SetMode(OnMoveToService) ;
}

void AirsideVehicleInSim::MoveOnRoute( const ElapsedTime& time )
{
	//ASSERT(time >= GetTime());
	if(time> GetTime())
	{
		SetTime(time);
	}
	ClearanceItem lastItem(GetResource(),GetMode(),GetDistInResource());
	lastItem.SetSpeed(GetSpeed());
	lastItem.SetTime(GetTime());
	lastItem.SetPosition(GetPosition());
	lastItem.SetAltitude(GetPosition().getZ());


	NotifyOtherAgents(SimMessage().setTime(time));

	RemoveDepend();
	Clearance newClearance;
	//long lFindStartTime = GetTickCount();

	bool bResult = m_Route.FindClearanceInConcern(this,lastItem,20000 ,newClearance);

	//long lFindEndTime = GetTickCount();
	//static long totalMoveTime = 0L;
	//totalMoveTime += lFindEndTime - lFindStartTime;
	//std::ofstream echoFile ("c:\\findveh.log", stdios::app);
	//echoFile<<"AirsideVehicleInSim::MoveOnRoute()     "<<lFindEndTime - lFindStartTime<<"          "
	//	<<totalMoveTime
	//	<<"\r\n";
	//echoFile.close();

	if( bResult )
	{

#if _DEBUGLOG
		VehicleLogFile::Log(this,newClearance);
#endif

		//long lFindStartTime1 = GetTickCount();
		if(newClearance.GetItemCount())
		{
			WirteLog(GetPosition(),GetSpeed(),GetTime());
			PerformClearance(newClearance);	
			VehicleMoveEvent * newEvent = new VehicleMoveEvent(this);
			newEvent->setTime(GetTime());
			//newEvent->addEvent();
			WirteLog(GetPosition(),GetSpeed(),GetTime());
			GenerateNextEvent(newEvent);
		}	

		//long lFindEndTime1 = GetTickCount();
		//static long totalMoveTime1 = 0L;
		//totalMoveTime1 += lFindEndTime1 - lFindStartTime1;
		//std::ofstream echoFile ("c:\\findveh.log", stdios::app);
		//echoFile<<"AirsideVehicleInSim::MoveOnRoute() - performance     "<<lFindEndTime1 - lFindStartTime1<<"          "
		//	<<totalMoveTime1
		//	<<"\r\n";
		//echoFile.close();

	}
	else
	{	
		//long lFindStartTime1 = GetTickCount();

		if(GetMode() == OnBackPool) //arrival at pool now
		{
			Clearance newBackPoolClearance;
			ClearanceItem backPoolItem(m_pVehiclePool,OnParkingPool,0);
			CPoint2008 pos;
			CPoint2008 dir;
			 
			if(m_Route.IsEmpty())
			{
				m_pVehiclePool->BirthOnPool(this, pos,dir);
			}
			else
			{
				CPath2008 path = m_pVehiclePool->ParkingToSpot(this, m_Route.getLastNode());
				path.GetEndPoint(pos);
			}
			
			backPoolItem.SetPosition(pos);	
			
			backPoolItem.SetSpeed(GetSpeed());
			double dspd = GetOnRouteSpeed();
			DistanceUnit dDist = lastItem.DistanceTo(backPoolItem);
			ElapsedTime dT = ElapsedTime(dDist / dspd);

			backPoolItem.SetTime(dT + lastItem.GetTime());
			lastItem = backPoolItem;
			newBackPoolClearance.AddItem(lastItem);
			lastItem.SetTime(lastItem.GetTime() + tMinTurnAroundTime);
			newBackPoolClearance.AddItem(lastItem);
			PerformClearance(newBackPoolClearance);
			ResetServiceCapacity();

			AirsideMobileElementMode mode = GetMode() ;
			SetMode(OnParkingPool) ;
			WirteLog(GetPosition(),GetSpeed(),GetTime());
			SetMode(mode) ;
			GenerateNextEvent(NULL);
			//m_pNextEvent = NULL;
			//AirsideVehicleInSim::GetNextCommand();
		}
		else if( GetMode() == OnMoveToService)
		{
			_ChangeToService();
		}
		else if (GetMode() == OnMoveToGate)
		{
			CPaxBusServiceGateEvent* newEvent = new CPaxBusServiceGateEvent(this);
			SetMode(OnMoveToGate);
			newEvent->setTime(GetTime());
			WirteLog(GetPosition(),GetSpeed(),GetTime());
			//newEvent->addEvent();
			GenerateNextEvent(newEvent);		

		}
		else if(GetMode() == OnMoveToBagTrainSpot)
		{
			CBagCartsServiceParkingSpotEvent* newEvent = new CBagCartsServiceParkingSpotEvent(this);
			SetMode(OnMoveToBagTrainSpot);
			newEvent->setTime(GetTime());
			WirteLog(GetPosition(),GetSpeed(),GetTime());
			//newEvent->addEvent();
			GenerateNextEvent(newEvent);		

		}
		//long lFindEndTime1 = GetTickCount();
		//static long totalMoveTime1 = 0L;
		//totalMoveTime1 += lFindEndTime1 - lFindStartTime1;
		//std::ofstream echoFile ("c:\\findveh.log", stdios::app);
		//echoFile<<"AirsideVehicleInSim::MoveOnRoute() - else     "<<lFindEndTime1 - lFindStartTime1<<"          "
		//	<<totalMoveTime1
		//	<<"\r\n";
		//echoFile.close();



	}

}

void AirsideVehicleInSim::PerformClearance(  Clearance& clearance )
{
	try
	{	
		NotifyOtherAgents(SimMessage().setTime(GetTime()));//NotifyObservers();
		ElapsedTime lastTime = GetTime();
		for(int i =0 ;i< clearance.GetItemCount();i++)
		{
			PerformClearanceItem(clearance.ItemAt(i));
		}
		EchoLog(clearance);
	}
	catch (CException* e)
	{
		delete e;
	}
}


static CPath2008 GenSmoothPath(const CPoint2008* _inPath,int nCount , DistanceUnit smoothLen)
{
	CPath2008 reslt;

	if(nCount<3)
	{
		CPath2008 oldPath;
		oldPath.init(nCount,_inPath);
		return oldPath;
	}
	const static int outPtsCnt = 6;


	std::vector<CPoint2008> _out;
	_out.reserve( (nCount-2)*outPtsCnt + 2);
	_out.push_back(_inPath[0]);


	CPoint2008 ctrPts[3];

	for(int i=1;i<(int)nCount-1;i++)
	{
		ctrPts[0] = _inPath[i-1];
		ctrPts[1] = _inPath[i];
		ctrPts[2] = _inPath[i+1];

		//get the two side point
		ARCVector3 v1 = ctrPts[0] - ctrPts[1];
		ARCVector3 v2 = ctrPts[2] - ctrPts[1];
		double minLen1,minLen2;
		minLen1 = smoothLen;//.45 * (ctrPts[1]-ctrPts[0]).length();
		minLen2	= smoothLen;//.45 * (ctrPts[2]-ctrPts[1]).length();
		if(v1.Length() * 0.45 <smoothLen) 
			minLen1= v1.Length() * 0.45;
		if(v2.Length() * 0.45 <smoothLen) 
			minLen2 = v2.Length() * 0.45;
		v1.SetLength(minLen1);
		v2.SetLength(minLen2);
		ctrPts[0] = ctrPts[1] + v1 ; 
		ctrPts[2] = ctrPts[1] + v2;
		std::vector<CPoint2008> ctrlPoints(ctrPts,ctrPts+3);
		std::vector<CPoint2008> output;
		BizierCurve::GenCurvePoints(ctrlPoints,output,outPtsCnt);
		for(int i = 0;i<outPtsCnt;i++)
		{
			_out.push_back(output[i]);
		}
	}

	_out.push_back( _inPath[nCount-1] );
	reslt.init(_out.size(), &_out[0]);
	return reslt;
}



void AirsideVehicleInSim::PerformClearanceItem( ClearanceItem& item )
{
	if(!item.IsWirteLog())
	{
		SetTime(item.GetTime());
		SetResource(item.GetResource());
		SetPosition(item.GetPosition());
		SetSpeed(item.GetSpeed());
		SetDistInResource(item.GetDistInResource());
		SetMode(item.GetMode());
		return;
	}

	if( item.GetMode()!= OnVehicleBirth)
	{
		CPoint2008 newPos = item.GetPosition();

		if( newPos.distance3D(GetPosition()) > 0 )
		{
			if(ignorLogCount)
				WirteLog( GetPosition(),GetSpeed(),GetTime());
				
		}
	}	
	if( GetResource() == item.GetResource() && GetResource()->GetType() == AirsideResource::ResType_VehicleStretchLane)
	{
		VehicleLaneInSim * pLane = (VehicleLaneInSim*)GetResource();		
		CPath2008 subPath = GetSubPath(pLane->GetPath(),GetDistInResource(),item.GetDistInResource());		
		CPath2008 smoothPath = subPath;//GenSmoothPath(subPath,400);
		
		ElapsedTime DT = item.GetTime() - GetTime();
		DistanceUnit DDist = smoothPath.GetTotalLength();

        if(DDist > 0)
		{
			for(int i=1;i<smoothPath.getCount();i++)
			{
				DistanceUnit realDist = smoothPath.GetIndexDist((float)i);
				ElapsedTime dT = ElapsedTime((double)DT * realDist / DDist );
				WirteLog( smoothPath.getPoint(i), item.GetSpeed(), GetTime() + dT) ;
			}			
			
		}			
	}
	if( GetResource() != item.GetResource() 
		&& GetResource() && GetResource()->GetType() == AirsideResource::ResType_VehicleStretchLane 
		&& item.GetResource() && item.GetResource()->GetType() == AirsideResource::ResType_VehicleStretchLane)
	{
		VehicleLaneInSim * laneFrom = (VehicleLaneInSim*) GetResource();
		VehicleLaneInSim * laneTo = (VehicleLaneInSim*)item.GetResource();
		//smooth path 
		std::vector<CPoint2008> _inputPts;
		std::vector<CPoint2008> _outputPts;		
		
		if(laneFrom->GetPath().getCount() >1 && laneTo->GetPath().getCount() >1)
		{
			const CPath2008& pathF = laneFrom->GetPath();
			const CPath2008& pathT = laneTo->GetPath();
			CPoint2008 ptFrom = pathF.GetDistPoint(GetDistInResource()); //getPoint(laneFrom->GetPath().getCount() -1);
			CPoint2008 ptTo = pathT.GetDistPoint(item.GetDistInResource()); //getPoint(0);
			
			ARCVector3 vDirF = pathF.GetDistDir( GetDistInResource() );// ptFrom - laneFrom->GetPath().getPoint(laneFrom->GetPath().getCount() -2);
			ARCVector3 vDirT = CPoint2008(0,0,0) -pathT.GetDistDir( item.GetDistInResource() ); //ptTo - laneTo->GetPath().getPoint(1); 
			DistanceUnit dist = ptFrom.distance(ptTo);
			vDirF.SetLength(dist *0.5);
			vDirT.SetLength(dist * 0.5);

			std::vector<CPoint2008> _inputPts;
			std::vector<CPoint2008> _outputPts;
			int nOutCnt = 10;

			_inputPts.push_back(ptFrom);
			_inputPts.push_back(ptFrom + vDirF);
			_inputPts.push_back(ptTo + vDirT);
			_inputPts.push_back(ptTo);

			BizierCurve::GenCurvePoints(_inputPts,_outputPts,nOutCnt);
			
			CPath2008 smoothpath;
			smoothpath.init(_outputPts.size(), &_outputPts[0]);
			DistanceUnit dLen = smoothpath.GetTotalLength();
			double dSpd = ( item.GetSpeed() + GetSpeed() )*0.5;
			ElapsedTime dT = dLen / dSpd;
			for(int i=1;i <(int)smoothpath.getCount() -1;i++)
			{
				DistanceUnit distT = smoothpath.GetIndexDist((float)i);	
				WirteLog( smoothpath.getPoint(i), item.GetSpeed(), GetTime() + ElapsedTime(distT/dSpd) );
			}
			item.SetTime(dT + GetTime());
		}
	}

	if(item.GetMode()!= OnVehicleTerminate)
	{	
		if(item.GetMode()!= OnBirth)
		{
			CPoint2008 newPos = item.GetPosition();

			if( newPos.distance3D(GetPosition()) > 0 )
			{				
				WirteLog( newPos,item.GetSpeed(),item.GetTime());
				ignorLogCount = 0;
			}
			else
			{				
				if(ignorLogCount==0)
				{
					ignorLogTime = item.GetTime();
				}
				ignorLogCount ++;				
			}

		}
		else
			WirteLog( item.GetPosition(),item.GetSpeed(),item.GetTime());		
		
	}

	AirsideResource * preResource = GetResource();	
	ElapsedTime preTime = GetTime();

	SetTime(item.GetTime());
	SetResource(item.GetResource());
	SetPosition(item.GetPosition());
	SetSpeed(item.GetSpeed());
	SetDistInResource(item.GetDistInResource());
	SetMode(item.GetMode());	

	if( GetResource() != preResource )
	{
		if(preResource)
		{
			preResource->SetExitTime(this,preTime);
		}
		if( GetResource() )
		{
			GetResource()->SetEnterTime(this,GetTime(),GetMode());
		}
	}
	
	WirteLog( GetPosition(),GetSpeed(),GetTime());	

}

DistanceUnit AirsideVehicleInSim::GetSeparationDist( AirsideVehicleInSim * pVehicleAhead ) const
{
	return 3 * SCALE_FACTOR;
}

ElapsedTime AirsideVehicleInSim::GetTimeBetween( const ClearanceItem& item1, const ClearanceItem& item2 ) const
{
	//default calculate time to the 
	ElapsedTime dT;
	double avgspd = (item1.GetSpeed() + item2.GetSpeed()) * 0.5;
	if(avgspd == 0)
	{
		//ASSERT(false);
		return ElapsedTime(0L);
	}
	DistanceUnit dDist;
	dDist = item1.DistanceTo(item2);
	dT = ElapsedTime(dDist / avgspd);
	return dT;
}

double AirsideVehicleInSim::GetOnRouteSpeed() const
{
	double dSpeed = m_pVehicleSpecificationItem->getMaxSpeed() * 0.5 + random( 20 ) - 20 ;
	dSpeed = max(dSpeed, 40);  //at least 40
	return 	ARCUnit::ConvertVelocity(dSpeed,ARCUnit::KMpHR,ARCUnit::CMpS);
}

double AirsideVehicleInSim::GetVehicleLength() const
{
	double dlength = m_pVehicleSpecificationItem->getLength() ;
	return 	ARCUnit::ConvertLength(dlength,ARCUnit::M, ARCUnit::CM);

}

double AirsideVehicleInSim::GetVehicleHeight() const
{
	double dlength = m_pVehicleSpecificationItem->getHeight();
	return 	ARCUnit::ConvertLength(dlength,ARCUnit::M, ARCUnit::CM);

}

double AirsideVehicleInSim::GetVehicleWidth() const
{
	double dlength = m_pVehicleSpecificationItem->getWidth();
	return 	ARCUnit::ConvertLength(dlength,ARCUnit::M, ARCUnit::CM);

}

double AirsideVehicleInSim::GetVehicleDec() const
{
	double dec = m_pVehicleSpecificationItem->getDecelration();
	return ARCUnit::ConvertLength(dec,ARCUnit::M, ARCUnit::CM);;
}

double AirsideVehicleInSim::GetVehicleAcc() const
{
	double acc = m_pVehicleSpecificationItem->getAcceleration();
	return ARCUnit::ConvertLength(acc,ARCUnit::M, ARCUnit::CM);;
}

void AirsideVehicleInSim::ResetServiceCapacity()
{
	m_vStuffPercent = 100.0;
}
void AirsideVehicleInSim::EchoLog(const Clearance& clearance) const
{
	if (ECHOLOG->IsEnable() && ECHOLOG->IsLogEnable(VehicleMoveLog) && (m_id == ECHOLOG->GetLogID(VehicleMoveLog)||ECHOLOG->GetLogID(VehicleMoveLog) == -1) )
	{
		for(int i =0 ;i< clearance.GetItemCount();i++)
		{
			const ClearanceItem& item  =clearance.ItemAt(i);
			
			CString strLog;
			strLog.Format(_T("%s,%d,ID = %d,Resource: %s,Dist = %.2f,Mode = %d,X = %.2f, Y = %.2f, Z = %.2f, Speed = %.2f"),
				item.GetTime().printTime(),
				(long)item.GetTime(),
				m_id,
				item.GetResource()->PrintResource(),
				item.GetDistInResource(),
				(int)item.GetMode(),
				item.GetPosition().getX(),
				item.GetPosition().getY(),
				item.GetPosition().getZ(),
				item.GetSpeed()
				);	

			ECHOLOG->Log(VehicleMoveLog,strLog);
		}	
		ECHOLOG->Log(VehicleMoveLog,_T(""));
	}

}

//void AirsideVehicleInSim::NotifyObservers()
//{
//	ObserverListType vObservers = m_vObserverList;
//	for(ObserverListType::iterator itr= vObservers.begin();itr!= vObservers.end();itr++)
//	{
//		(*itr)->Update(GetTime());
//	}
//	m_vObserverList.clear();	
//}

enumVehicleBaseType AirsideVehicleInSim::GetVehicleBaseType()
{
	if (m_pVehicleSpecificationItem)
		return m_pVehicleSpecificationItem->getBaseType();
	return VehicleType_General;
}


//void VehicleWakeupCaller::Update( const ElapsedTime& tTime )
//{
//	VehicleMoveEvent * newEvent = new VehicleMoveEvent(m_pVehicle);
//	newEvent->setTime(tTime);
//	newEvent->addEvent();
//	ClearSubjects();
//}
void AirsideVehicleInSim::WritePureLog( AirsideMobElementWriteLogItem* pLogItem )
{
	CAirsideVehicleWriteLogItem *pVehicleLogItem = (CAirsideVehicleWriteLogItem *)pLogItem;
	if(pVehicleLogItem != NULL)
	{

		m_pOutput->LogVehicleEvent(this ,pVehicleLogItem->m_logItem);


		//m_dist = pVehicleLogItem->m_state.m_dist;
		//m_tCurrentTime = pVehicleLogItem->m_state.m_tCurrentTime;
		//m_pPosition = pVehicleLogItem->m_state.m_pPosition;
		//m_vSpeed = pVehicleLogItem->m_state.m_vSpeed;    //cm/s
		//m_vDirection = pVehicleLogItem->m_state.m_vDirection;  //
		//m_vehicleMode = pVehicleLogItem->m_state.m_vehicleMode;
		//m_pResource = pVehicleLogItem->m_state.m_pResource;
		//tMinTurnAroundTime = pVehicleLogItem->m_state.tMinTurnAroundTime;


		//m_vStuffPercent = pVehicleLogItem->m_state.m_vStuffPercent;
		//m_pServiceFlight = pVehicleLogItem->m_state.m_pServiceFlight;
		//m_tLeavePoolTime = pVehicleLogItem->m_state.m_tLeavePoolTime;
		//m_bAvailable = pVehicleLogItem->m_state.m_bAvailable;
		//m_nServicePointCount = pVehicleLogItem->m_state.m_nServicePointCount;
		//m_pServiceTimeDistribution = pVehicleLogItem->m_state.m_pServiceTimeDistribution;
		//m_pCurrentPointIndex = pVehicleLogItem->m_state.m_pCurrentPointIndex;

		//execute event finished
		m_bExecuteWriteLogEvent = false;


		NotifyOtherAgents(SimMessage().setTime(GetTime()));//NotifyObservers();

		//CString allfile = "C:\\FlightDebug\\AllFlight_Test.txt";
		//std::ofstream alloutfile;
		//alloutfile.open(allfile,ios::app);	
		//alloutfile << pFlightLogItem->m_logItem.time<<"		"
		//			<< std::endl;
		//alloutfile.close();
	}
	GenerateNextEvent(m_pNextEvent);
}

void AirsideVehicleInSim::GenerateWriteLogEvent()
{

	if(m_lstWirteLog.GetItemCount() > 0)
	{
		AirsideMobElementWriteLogItem *pLogItem = m_lstWirteLog.GetItem();
		CAirsideMobElementWriteLogEvent *pNextEvent = new CAirsideMobElementWriteLogEvent(pLogItem);
		CAirsideVehicleWriteLogItem *pFlightLogItem = (CAirsideVehicleWriteLogItem *)pLogItem;
		ElapsedTime eEventTime = GetTime();
		if(pFlightLogItem)
			eEventTime.set(pFlightLogItem->m_logItem.time/TimePrecision);

		pNextEvent->setTime(eEventTime);
		pNextEvent->addEvent();
		
		//new executed next event
		m_bExecuteWriteLogEvent = true;

	}
	else
		GenerateNextEvent(m_pNextEvent);
}

void AirsideVehicleInSim::GenerateNextEvent(AirsideEvent *pNextEvent)
{
	//if(m_pNextEvent != pNextEvent)
	//{
	//	delete m_pNextEvent;
	//	m_pNextEvent = NULL;
	//}
	m_pNextEvent = pNextEvent;
	
	if(m_bExecuteWriteLogEvent)//if the vehicle is in session of write log, cannot add other event
		return;

	if(m_lstWirteLog.GetItemCount() > 0)
	{
		GenerateWriteLogEvent();
	}
	else
	{
		if(m_pNextEvent)
		{
			m_pNextEvent->addEvent();
			m_pNextEvent = NULL;
		}
		else
			GetNextCommand();
	}
}

void AirsideVehicleInSim::_ChangeToService()
{
	CFlightServiceEvent* newEvent = new CFlightServiceEvent(this);
	SetMode(OnMoveToRingRoute);
	newEvent->setTime(GetTime());
	WirteLog(GetPosition(),GetSpeed(),GetTime());
	//newEvent->addEvent();
	GenerateNextEvent(newEvent);
}

void AirsideVehicleInSim::OnNotify( SimAgent* pAgent, SimMessage& msg )
{
	ElapsedTime t = msg.getTime();
	VehicleMoveEvent* pEvent = new VehicleMoveEvent(this);
	pEvent->setTime(t);	
	GenerateNextEvent(pEvent);
}

RouteDirPath* AirsideVehicleInSim::getCurDirPath() const
{
	AirsideResource* pRes = m_pResource;
	/*if(m_pResource->GetType() == AirsideResource::ResType_VehicleStretchLane)
	{
		return (VehicleLaneInSim*)m_pResource;
	}*/
	return NULL;
}

void AirsideVehicleInSim::FlushLogToFile()
{
	m_pOutput->m_reportLog.AddVehicleLogItem(*m_pReportlogItem);
}

void AirsideVehicleInSim::InitLogEntry(VehiclePoolInSim*pool,OutputAirside * pOut)
{
	m_pOutput = pOut;
	SetVehiclePool(pool);

	//
	int ItemID = m_pVehicleSpecificationItem->GetVehicleTypeID();
	int nVehicleTypeId  = (int)m_pVehicleSpecificationItem->getBaseType() ;
	float fuelconsumed = (float)m_pVehicleSpecificationItem->GetFuelCOnsumed() ;

	AirsideVehicleDescStruct desc;
	desc.id = ItemID;
	desc.m_BaseType = nVehicleTypeId;
	desc.poolNO = pool->GetPoolInput()->getID();
	desc.m_VecileTypeID = CVehicleSpecificationItem::GetVehicleIDByVehicleName(m_pVehicleSpecificationItem->getName()) ;
	desc.m_FuelConsumed = fuelconsumed ;
	strcpy(desc.vehicleType,	m_pVehicleSpecificationItem->getName() );

	desc.vehicleheight  = GetVehicleHeight();
	desc.vehiclewidth = GetVehicleWidth();
	desc.vehiclelength = GetVehicleLength();
	desc.IndexID = GetID() ;

	SetVehicleTypeID( ItemID);

	pOut->LogVehicleEntry(this,desc);


	//init log entry
	m_pReportlogItem = new AirsideVehicleLogItem();
	AirsideVehicleLogDesc& descp = m_pReportlogItem->mVehicleDesc;

	descp.id = m_id;
	descp.poolNO = m_pVehiclePool?m_pVehiclePool->GetVehiclePoolID():0;
	descp.vehicleheight = GetVehicleHeight();
	descp.vehiclelength = GetVehicleLength();
	descp.vehiclewidth = GetVehicleWidth();
	descp.vehiclebasetype = GetVehicleBaseType();
	descp.vehicletype = m_nVehicleTypeID;

}

#include "../../Results/AirsideFlightEventLog.h"
void AirsideVehicleInSim::getDesc( ARCMobileDesc& desc ) const
{
	desc.mType = ARCMobileDesc::VEHICLE;
	desc.nMobileID = m_id;
}

void AirsideVehicleInSim::SetAvailable( bool bAvailable )
{
	m_bAvailable = bAvailable;

	if (m_bAvailable)
	{
		m_pServiceFlight = NULL;
	}
}
