#include "StdAfx.h"
#include ".\airsideflightinsim.h"
#include "AirTrafficController.h"
#include "Clearance.h"
#include "FlightGetClearanceEvent.h"
#include "../../Common/ARCUnit.h"
#include "../../Common/AIRSIDE_BIN.h"
#include "../../Common/AirlineManager.h"
#include "../../Results/OutputAirside.h"
#include "./TaxiwayResource.h"
#include "AirspaceResource.h"
#include "TaxiRouteInSim.h"
#include "StandInSim.h"
#include <fstream>
#include "FlightMovementEvent.h"
#include "ReleaseResourceLockEvent.h"
#include "FlightArrivalStandEvent.h"
#include "VehicleRequestDispatcher.h"
#include "VehicleStretchInSim.h"
#include "FlightServiceInformation.h"
#include "TempParkingSegmentsInSim.h"
#include "../../Common/AirportDatabase.h"
#include "../../Common/ACTypesManager.h"

#include "../SimulationDiagnoseDelivery.h"
#include "../../common/BizierCurve.h"
#include "../../InputAirside/ArrivalETAOffsetList.h"
#include "./AirsideResourceManager.h"
#include "./AirsideSimConfig.h"
#include "../../Common/AirportsManager.h"
#include "StandLeadOutLineInSim.h"
#include "StandLeadInLineInSim.h"
#include "AirRoutePointInSim.h"
#include "ConflictConcernIntersectionInAirspace.h"
#include "HeadingAirRouteSegInSim.h"
#include "../../Common/Path2008.h"
#include "AirportStatusManager.h"
#include "AirsideFlightDelay.h"
#include <common/UTurnPath.h>
#include "SimulationErrorShow.h"
#include "TowTruckServiceRequest.h"
#include "ResourceEntryEvent.h"

#include "VehicleServiceRequest.h"
#include "../../Inputs/flight.h"
#include "AirsideMobElementWriteLogItem.h"
#include "FlightInHoldEvent.h"
#include "AirsideMobElementWriteLogEvent.h"
#include "RunwaySegInSim.h"
#include "CheckRunwayWaveCrossEvent.h"
#include "../../InputAirside/VehicleDefine.h"
#include ".\DeiceVehicleServiceRequest.h"
#include "Push_TowOperationSpecInSim.h"
#include "TowingServiceEvent.h"
#include "../../Common/ARCVector.h"
#include "../../Common/ARCTracker.h"

#include "../../Results/AirsideFlightLogItem.h"

#include "../../InputAirside/TowOffStandAssignmentData.h"
#include "../../InputAirside/ALTObjectGroup.h"
#include "AirsideReportingAreaInSim.h"
#include "../../Results/ARCEventLog.h"
#include "../../Results/AirsideFlightEventLog.h"
#include "../../Results/AirsideFllightRunwayDelayLog.h"
#include "../../Common/ARCPipe.h"
#include "AirsideFlightStairsLog.h"
#include "../FltOperatingDoorSpecInSim.h"
#include "FlightExitStandEvent.h"
#include "AirsideFollowMeCarInSim.h"
#include "../terminal.h"
#include "../BridgeConnector.h"
#include "AirsideRemoveFlightFromQueueEvent.h"
#include "../gate.h"
#include "../../InputAirside/FollowMeConnectionData.h"
#include "FlightMoveToMeetingPointEvent.h"
#include "AirsideFollowMeCarServiceEvent.h"
#include "DynamicConflictGraph.h"
#include "../ARCportEngine.h"
#include <limits>
#include "../OnboardSimulation.h"
#include "../SimEngineConfig.h"
#include "../../InputOnBoard/AircaftLayOut.h"
#include "../../Inputs/Stand2GateConstraint.h"
#include "AirsideSimulation.h"
#include <limits>
#include <Common/FileOutPut.h>
#include "../OnboardFlightInSim.h"
#include "PaxBusServiceRequest.h"
#include "../boardcal.h"
#include "FlightPerformancesInSim.h"
#include "AirsidePassengerBusStrategy.h"
#include "..\AirsdieFlightBaggageManager.h"

#define _DEBUGLOG 1

//log debug //////////////////////////////////////////////////////////////////////////
#if _DEBUGLOG 
#include <iostream>
#include <fstream>

#if _DEBUG
const static CString LOGFOLDER= _T("c:\\flightdebug\\");
#else
const static CString LOGFOLDER= _T("c:\\flightrelease\\");
#endif

class FligthLogFile 
{
public:	
	FligthLogFile()
	{
		CString allfile = LOGFOLDER+_T("AllFlight.txt");
		std::ofstream alloutfile;
		alloutfile.open(allfile);	
		alloutfile << "All Flights Clearance" << std::endl;
		alloutfile.close();
	}


	static CString GetFileName(AirsideFlightInSim* pFlight){
		CString debugfileName;
		debugfileName.Format( "%s%d.txt",LOGFOLDER,pFlight->GetUID());
		return debugfileName;
	}

	static void LogEventNum(AirsideFlightInSim* pFlight)
	{
		CString debugfileName = GetFileName(pFlight);

		//log to flight file
		std::ofstream outfile;
		outfile.open(debugfileName,ios::app);
		outfile <<"<" << Event::getCurEventNum()<<">" << std::endl;
	}

	static void Log(AirsideFlightInSim* pFlight, const Clearance& clearance)
	{
		CString debugfileName = GetFileName(pFlight);

		//log to flight file
		std::ofstream outfile;
		outfile.open(debugfileName,ios::app);
		
		outfile <<"<" << Event::getCurEventNum()<<">" << std::endl;
		outfile << pFlight->GetTime()<<"," << long(pFlight->GetTime())<<std::endl;
		outfile << pFlight->GetPosition().getX() <<" ," <<pFlight->GetPosition().getY() << std::endl;
		outfile << clearance;
		outfile.close();
		//log to all file
		CString allfile = LOGFOLDER+_T("AllFlight.txt");
		std::ofstream alloutfile;
		alloutfile.open(allfile,ios::app);		
		alloutfile<< pFlight->GetCurrentFlightID().GetString()<<"("<<pFlight->GetUID()<<"),"<<pFlight->GetTime()<<"," << long(pFlight->GetTime())<<std::endl;
		alloutfile<<clearance;		
		alloutfile.close();
	}

	static void LogEvent(AirsideFlightInSim* pFlight, const ElapsedTime& eventTime)
	{

		//log to flight file
		CString debugfileName;
		debugfileName.Format( "%s%d.txt",LOGFOLDER,pFlight->GetUID());
		std::ofstream outfile;
		outfile.open(debugfileName,ios::app);
		outfile <<"Event Time:"<<eventTime<<","<<long(eventTime)<<" Flight time: " <<pFlight->GetTime()<<"," << long(pFlight->GetTime())<<std::endl;
		outfile << AIRSIDEMOBILE_MODE_NAME[pFlight->GetMode()]<<"Position:" <<pFlight->GetPosition().getX() <<" ," <<pFlight->GetPosition().getY() << std::endl;

		outfile.close();
		//log to all file
		CString allfile = LOGFOLDER+_T("AllFlight.txt");
		std::ofstream alloutfile;
		alloutfile.open(allfile,ios::app);		
		alloutfile<< pFlight->GetCurrentFlightID().GetString()<<"("<<pFlight->GetUID()<<"),"<<"Event Time:"<<eventTime<<","<<long(eventTime)<<" Flight time: "<<pFlight->GetTime()<<"," << long(pFlight->GetTime())<<" Mode: " <<AIRSIDEMOBILE_MODE_NAME[pFlight->GetMode()]<<"Position:" <<std::endl;
		alloutfile.close();
	}

	static void LogLog(AirsideFlightInSim* pFlight, const AirsideFlightEventStruct& newEvent)
	{		
		CFileOutput fileOut( GetFileName(pFlight) );
		CString strLine; 
		strLine.Format("%d	(%.2f %.2f %.2f)	%d	%d	%c" ,newEvent.time, newEvent.x, newEvent.y, newEvent.z, newEvent.intNodeID, newEvent.nodeNO, newEvent.eventCode);
		fileOut.LogLine(strLine);

	}
	static void InitLog(AirsideFlightInSim* pFlight)
	{
		char fltstr[255];
		pFlight->getFullID(fltstr);
		CString debugfileName = GetFileName(pFlight);		
		std::ofstream outfile;
		outfile.open(debugfileName);
		outfile<<pFlight->GetUID()<<"\t"<<fltstr<<std::endl;
		outfile.close();	
	}

	
};
#endif
#include "FlightOpenDoors.h"
//////////////////////////////////////////////////////////////////////////

AirsideFlightInSim::AirsideFlightInSim(CARCportEngine *pARCPortEngine,Flight* pFlt, int id)
:m_eTOMTimeInRunway(0L)
,m_eTOStartPrepareTime(0L)
,m_ePreFlightExitRunwayTime(0L)
,m_pARCPortEngine(pARCPortEngine)
{
	PLACE_METHOD_TRACK_STRING();
	m_pflightInput= pFlt;
	m_id = id;

	m_pATC = NULL;
	m_pArrivalPlan = NULL;
	m_pSTAR = NULL;
	m_pLandingRunway = NULL;
	m_pRunwayExit = NULL;
	//m_pInBoundRoute = NULL;
	m_pArrParkingStand = NULL;
	m_pDepParkingStand = NULL;
	m_pTemporaryParking =NULL;
	m_pIntermediateParking = NULL;
	//m_pHoldingStand = NULL;
	m_pOutBoundRoute = NULL;
	m_pTakeoffPos = NULL;
	m_pDepatrueRunway = NULL;
	m_pSID = NULL;
	m_pLandingCircuitRoute = NULL;
	m_pTakeoffCircutRoute = NULL;
	m_pDeparturePlan = NULL;
	m_pRouteToTempParking = NULL;
	m_pRouteToStand = NULL;
	m_pCircuitBoundRoute = NULL;
	m_pTowingRoute = NULL;
	m_pFlightServiceInfomation = NULL;
	m_bTowTruckServiced = false;
	m_pFlightPerformanceManager = NULL;
	m_pRouteInQueue = NULL;
	//m_pTemporaryParkingTaxi = NULL;

	//m_nServicePointCount = 0;
	m_bApplyFroServiceRequest = false;
	m_wakeupCaller.SetFlight(this);
	
	m_pArrivalPaxParkingInSim = NULL;
	m_pDepPaxParkingInSim = NULL;
	m_bApplyFrPaxBusServiceRequestArrival = false;
	m_bApplyFrPaxBusServiceRequestDeparture = false;

	m_bApplyTrainServiceRequestArrival = false;
	m_bApplyTrainServiceRequestDeparture = false;

	m_bTowingToIntStand = false;
	m_bTowingToDepStand = false;
	m_bMoveToIntStand = false;
	m_bMoveToDepStand = false;
	m_bTowingToReleasePoint = false;
	mbDeicParking = false;
	m_dRandom = (rand()%100)/100;	
	m_dRandom = (m_dRandom > 0.2? m_dRandom: 0.2);			//the random value is used for get performance data, so it should not be too small
	m_dRandom = (m_dRandom < 0.85? m_dRandom: 0.85);			//the random value is used for get performance data, so it should not be too large
	m_pTowingServiceVehicle = NULL;
	m_pServiceRequestDispatcher = NULL;

	m_pCrossRunwayHold = NULL;

	m_pCrossRunwayHold = NULL;
	m_pTowoffData = NULL;


	ignorLogCount = 0;

	m_pCurrentDelay = NULL; 
	mpCurrentDelayLog = NULL;

	m_strActualArrGate = _T("");
	m_strActualDepGate = _T("");

	InitLogEntry();

	if (IsArrival())
	{
		m_curFlightType = 'A'; 
	}
	else
	{
		m_curFlightType = 'D';
	}

	m_bStairShowState = false;
	m_bFollowMeCarServicing = false;
	m_nPaxCountInFlight = 0;
	m_bWaitPaxTakeOn = false;
	m_pServiceFollowMeCar = NULL;
	m_pRouteToAbandonPoint = NULL;
	m_pRouteToMeetingPoint = NULL;

	m_pPreferredRoutePriority = NULL;

	mOtherFlightMaxWingSpan = 0;

	m_pPaxBusContext = NULL;
	m_pArrPaxBusStrategy = NULL;
	m_pDepPaxBusStrategy = NULL;

	mbReadyForDeice = false;
	m_pBaggageManager = new AirsideFlightBaggageManager;
	m_pBaggageManager->SetFlight(this);

	m_pOpenDoors = NULL;
}

AirsideFlightInSim::~AirsideFlightInSim(void)
{
	PLACE_METHOD_TRACK_STRING();
	if (m_pDeparturePlan)
	{
		delete m_pDeparturePlan;
		m_pDeparturePlan = NULL;
	}

	if (m_pArrivalPlan)
	{
		delete m_pArrivalPlan;
		m_pArrivalPlan = NULL;
	}

	if (m_pRouteToStand)
	{
		delete m_pRouteToStand;
		m_pRouteToStand = NULL;
	}

	if (m_pOutBoundRoute)
	{
		delete m_pOutBoundRoute;
		m_pOutBoundRoute = NULL;
	}

	if (m_pCircuitBoundRoute)
	{
		delete m_pCircuitBoundRoute;
		m_pCircuitBoundRoute = NULL;
	}

	if (m_pRouteToTempParking)
	{
		delete m_pRouteToTempParking;
		m_pRouteToTempParking = NULL;
	}

	if (m_pSID)
	{
		delete m_pSID;	
		m_pSID = NULL;
	}

	if (m_pLandingCircuitRoute)
	{
		delete m_pLandingCircuitRoute;
		m_pLandingCircuitRoute = NULL;
	}

	if (m_pTakeoffCircutRoute)
	{
		delete m_pTakeoffCircutRoute;
		m_pTakeoffCircutRoute = NULL;
	}

	if (m_pSTAR)
	{
		delete m_pSTAR;
		m_pSTAR = NULL;
	}

	if (m_pTowingRoute)
	{
		delete m_pTowingRoute;
		m_pTowingRoute = NULL;
	}

	if (m_pRouteToMeetingPoint)
	{
		delete m_pRouteToMeetingPoint;
		m_pRouteToMeetingPoint = NULL;
	}

	if (m_pRouteToAbandonPoint)
	{
		delete m_pRouteToAbandonPoint;
		m_pRouteToAbandonPoint = NULL;
	}

	delete m_pReportLogItem;

	delete m_pFlightServiceInfomation;
	m_pFlightServiceInfomation = NULL;

	if (m_pPaxBusContext == NULL)
	{
		delete m_pPaxBusContext;
		m_pPaxBusContext = NULL;
	}

	if (m_pArrPaxBusStrategy == NULL)
	{
		delete m_pArrPaxBusStrategy;
		m_pArrPaxBusStrategy = NULL;
	}

	if (m_pDepPaxBusStrategy == NULL)
	{
		delete m_pDepPaxBusStrategy;
		m_pDepPaxBusStrategy = NULL;
	}

	delete m_pFlightPerformanceManager;

	delete m_pBaggageManager;

	cpputil::autoPtrReset(m_pOpenDoors);
}

BOOL AirsideFlightInSim::IsThroughOut() const
{ 
	return false; 
}

void AirsideFlightInSim::WakeUp(const ElapsedTime& tTime)
{
	PLACE_METHOD_TRACK_STRING();

	FligthLogFile::LogEventNum(this);

	if(GetMode() == OnTerminate) 
		return;	

	if(GetMode() == OnTaxiToMeetingPoint)
	{
		if (tTime > GetTime())
			SetTime(tTime);

		FlightMoveToMeetingPointEvent* pNextEvent = new FlightMoveToMeetingPointEvent(this);
		pNextEvent->setTime(GetTime());
		pNextEvent->addEvent();
		return;
	}

	if (m_bFollowMeCarServicing && m_pServiceFollowMeCar)
	{
		SetTime(tTime);			//update time
		m_pServiceFollowMeCar->SetTime(tTime);

		AirsideFollowMeCarServiceEvent* pNextEvent = new AirsideFollowMeCarServiceEvent(this);
		pNextEvent->setTime(GetTime());
		pNextEvent->addEvent();
		return;
	}

	if(tTime < GetTime() )
	{		
		//ASSERT(FALSE);			//error
		return GenerateNextEvent();
	}

	if (m_bTowingToReleasePoint && m_pTowingServiceVehicle)		//towing state
	{
		SetTime(tTime);			//update time
		m_pTowingServiceVehicle->SetTime(tTime);

		TowingServiceEvent* pEvent = new TowingServiceEvent(this);
		pEvent->setTime(tTime);
		pEvent->addEvent();
		return;
	}
	
	if( GetTime() < tTime )
	{
		if (GetResource())
		{

			if (GetResource()->GetType() == AirsideResource::ResType_AirspaceHold)
			{
				ElapsedTime tHoldTime = tTime;
				AirspaceHoldInSim* pHold = (AirspaceHoldInSim*)GetResource();
				FlightInHoldEvent* pEvent = new FlightInHoldEvent(this, pHold, tHoldTime);
			//	pEvent->setTime(GetTime());
				pEvent->setTime(tTime);
				pEvent->addEvent();	
				return;
			}
		}

		SetTime(tTime);
	}

	//1. ask for ATC for next Clearance


	ASSERT( GetAirTrafficController() );	
	long lFindStartTime = static_cast<long>(GetTickCount());	
	Clearance newClearance;
	GetAirTrafficController()->GetNextClearance(this,newClearance) ;
	
	long lFindEndTime = GetTickCount();

	if(newClearance.GetItemCount())
	{		
		//SetTime(tTime);
		WriteLog();
		PerformClearance(newClearance);	
		GenerateNextEvent();
	}
	
	/*long lPerformTime = GetTickCount();
	

	static long totalFindTime = 0L;
	totalFindTime += lFindEndTime - lFindStartTime;
	
	static long totalPerformTime =0L;
	totalPerformTime += lPerformTime - lFindEndTime;

	std::ofstream echoFile ("c:\\find.log", stdios::app);

	echoFile<<lFindEndTime - lFindStartTime<<"          "
			<<lPerformTime - lFindEndTime <<"           "
			<<totalFindTime <<"           "
			<<totalPerformTime <<"           "
			<<"\r\n\r\n";
			
	echoFile.close();*/		     
}


ElapsedTime AirsideFlightInSim::GetBirthTime() const
{	
	if( m_pflightInput->isArriving() )
	{
		return m_arrTime;
	}
	else					
	{		
		return max( m_depTime - m_pflightInput->getServiceTime(), ElapsedTime());
	}
}

void AirsideFlightInSim::SetSpeed(double speed)
{
	m_curState.m_vSpeed = speed;
}

void AirsideFlightInSim::SetPerformance(FlightPerformanceManager* pPerform)
{
	PLACE_METHOD_TRACK_STRING();
	if(m_pFlightPerformanceManager == NULL)
		m_pFlightPerformanceManager = new FlightPerformancesInSim(pPerform);
	
	//m_tTakeoffPositionTime = m_pFlightPerformanceManager->getTakeoffPositionTime(this);
	//m_dLiftOffSpeed = m_pFlightPerformanceManager->getLiftoffSpeed(this);
	//m_dTakeOffRoll = m_pFlightPerformanceManager->getTakeoffRoll(this);
	//m_dPushbackSpd = m_pFlightPerformanceManager->getPushBackSpeed(this);
	//m_tUnhookTime = m_pFlightPerformanceManager->getUnhookandTaxiTime(this);
	//m_dTowingSpeed = m_pFlightPerformanceManager->getTowingSpeed(this);
	//m_tEngineStartTime = m_pFlightPerformanceManager->getEngineStartTime(this);

	AirsideFlightDescStruct& fltDesc = m_LogEntry.GetAirsideDesc();
	fltDesc.dAprchSpeed = m_pFlightPerformanceManager->getAvgApproachSpeedInKnots(this);

}

double AirsideFlightInSim::GetMinSpeed(AirsideMobileElementMode mode)
{
	PLACE_METHOD_TRACK_STRING();
	double minSpeed = 0.0;
	switch(mode) 
	{
	case OnCruiseArr:
	case OnCruiseDep:
	case OnCruiseThrough: 
		{
			minSpeed = GetPerformance()->getMinCruiseSpeed(this);
			break;
		}
	case OnMissApproach:
	case OnTerminal:
		{
			minSpeed = GetPerformance()->getMinTerminalSpeed(this);
			break;
		}
	case OnFinalApproach:
	case OnApproach:
		{
			minSpeed = GetPerformance()->getMinApproachSpeed(this);
			break;
		}

	case OnClimbout:
		{
			minSpeed = GetPerformance()->getMinClimboutSpeed(this);
			break;
		}
	default:
		break;
	}
	return minSpeed;
}

double AirsideFlightInSim::GetMaxSpeed(AirsideMobileElementMode mode)
{
	PLACE_METHOD_TRACK_STRING();
	double maxSpeed = 0.0;
	switch(mode) 
	{
	case OnCruiseArr:
	case OnCruiseDep:
	case OnCruiseThrough: 
		{
			maxSpeed = GetPerformance()->getMaxCruiseSpeed(this);
			break;
		}
	case OnMissApproach:
	case OnTerminal:
		{
			maxSpeed = GetPerformance()->getMaxTerminalSpeed(this);
			break;
		}
	case OnFinalApproach:
	case OnApproach:
		{
			maxSpeed = GetPerformance()->getMaxApproachSpeed(this);
			break;
		}
	case OnClimbout:
		{
			maxSpeed = GetPerformance()->getMaxClimboutSpeed(this);
			break;
		}
	default:
		break;
	}
	return maxSpeed;
}
//write to log 
void AirsideFlightInSim::PerformClearance( const Clearance& clearance )
{
	PLACE_METHOD_TRACK_STRING();
	try
	{	
#if _DEBUGLOG
		FligthLogFile::Log(this, clearance);
#endif

		SaveState();
		//NotifyObservers(GetTime());
		NotifyOtherAgents(SimMessage().setTime(GetTime()));

		if (GetMode() == OnExitRunway)		//cannot leave runway, if it can leave, its mode must be OnTaxiToStand
		{
			ClearanceItem curItem(GetResource(),GetMode(),GetDistInResource());
			curItem.SetSpeed(GetSpeed());
			curItem.SetTime(GetTime());
			curItem.SetPosition(GetPosition());
			AirsideFlightInSim* pConflictFlt = GetRunwayExit()->GetRouteSeg()->GetLastInResourceFlight();
			StartDelay(curItem,pConflictFlt, FlightConflict::STOPPED,FlightConflict::EXIT,FltDelayReason_Stop,_T("Exit runway delay"));			//exit delay
		}
		//if(clearance.GetItemCount())
		//	SetDelayed(NULL);

		ElapsedTime lastTime = GetTime();
		int nCount = clearance.GetItemCount();
		for(int i =0 ;i< nCount;i++)
		{			
			PerformClearanceItem(clearance.ItemAt(i));
		}	
	}
	catch (CException* e)
	{
		CString strTime = GetTime().printTime();
		char fltstr[255];
		getFullID(fltstr);
		CString strID;
		strID.Format("F:%s",fltstr);
		AirsideDiagnose* pNewDiagnose = new AirsideDiagnose(strTime, strID);		
		CString strDetals = _T("Error");
		pNewDiagnose->SetDetails(strDetals);
		throw new AirsideSystemError(pNewDiagnose);			
		delete e;
	}	

}

static CPath2008 GenSmoothPath(const CPoint2008* _inPath,int nCount , DistanceUnit smoothLen)
{
	PLACE_METHOD_TRACK_STRING();
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

#include "..\MobileDynamics.h"

void AirsideFlightInSim::WritePathLog( const  MobileGroundPerform& groundPerform, const CPath2008& path,AirsideFlightState startState,const ClearanceItem& item)
{
	PLACE_METHOD_TRACK_STRING();
	DistanceUnit DDist = path.GetTotalLength();
	ElapsedTime DT = item.GetTime() - startState.m_tTime;
	double dAvgSpeed = DDist/DT.asSeconds();
	
	MobileTravelTrace travelTrace(groundPerform,DDist,startState.m_vSpeed,item.GetSpeed());
	for(int i=1;i<path.getCount()-1;i++)
	{
		DistanceUnit realDist = path.GetIndexDist((float)i);
		travelTrace.addDistItem(realDist);
	}
	travelTrace.BeginUse();
	ElapsedTime dDiffTime = max(DT - travelTrace.getEndTime(),0L);


	bool bUseTraceSpd = (startState.m_vSpeed + item.GetSpeed() )<ARCMath::EPSILON;
	
	
	AirsideFlightState midState = startState;
	AirsideFlightState _curState = m_curState; 
	if(DDist > 0 /*&& ( startState.m_vSpeed + endSpd ) >0*/)
	{
		int nCount = travelTrace.getItemCount();
// 		if (nCount > 1)
// 			dDiffTime /= DDist;	

		for(int i=1;i<nCount-1;++i)
		{
			const MobileTravelTrace::DistSpeedTime& distItem  = travelTrace.ItemAt(i);
			midState.m_pPosition = path.GetDistPoint(distItem.mdist); 
			double r = distItem.mdist/DDist;
			midState.m_dAlt = startState.m_dAlt*(1.0-r) + item.GetAltitude() * r;		//calculate altitude change
			ElapsedTime dT;
			long difftime = dDiffTime.getPrecisely();
			dT.setPrecisely((long)(difftime*r));
			midState.m_tTime = startState.m_tTime + distItem.mtTime + dT;
			midState.m_dist = startState.m_dist + distItem.mdist;
			midState.m_vSpeed = distItem.mdSpd;//;
			midState.m_offsetAngle = 0;
			midState.m_dAcc = startState.m_dAcc;
			midState.m_bPushBack = startState.m_bPushBack;

			bool bNodeLog = (i == (travelTrace.getItemCount()-1)/2 && (item.GetResource() != GetResource()));
			UpdateState(midState,bNodeLog);	
			
			if (m_bTowingToReleasePoint && m_pTowingServiceVehicle)
			{

				double dist = 0;		
				//if (midState.m_fltMode == OnExitStand && midState.m_pResource->GetType() == AirsideResource::ResType_StandLeadOutLine
				//	&& ((StandLeadOutLineInSim*)midState.m_pResource)->IsPushBack())
				if (midState.m_bPushBack)
					dist = dist - (GetLength()*0.5 - m_pTowingServiceVehicle->GetVehicleLength()*0.5);
				else
					dist = dist + (GetLength()*0.5 - m_pTowingServiceVehicle->GetVehicleLength()*0.5);

				CPoint2008 offsetPt(dist,0,0);
				ARCVector2 vDir(midState.m_pPosition- _curState.m_pPosition);
				offsetPt.rotate(-(vDir.AngleFromCoorndinateX()));
				CPoint2008 pos = _curState.m_pPosition + offsetPt ;
				m_pTowingServiceVehicle->WirteLog(pos,_curState.m_vSpeed,_curState.m_tTime);

				pos = midState.m_pPosition + offsetPt ;
				m_pTowingServiceVehicle->WirteLog(pos,midState.m_vSpeed,midState.m_tTime);
				_curState = midState;
			}

			//if (m_bFollowMeCarServicing && m_pServiceFollowMeCar)
			//{
			//	double dist = m_LogEntry.GetAirsideDescStruct().dLength + realDist;

			//	if (dist <= DDist)
			//	{
			//		CPoint2008 pos = path.GetDistPoint(dist) ;
			//		m_pServiceFollowMeCar->WirteLog(pos,midState.m_vSpeed,midState.m_tTime);
			//	}
			//}
		}
	}	


}

void AirsideFlightInSim::WriteMissApproachOnRunwayPathLog( const CPath2008& path,AirsideFlightState startState,const ElapsedTime&  endTime, const double& endSpd)
{
	PLACE_METHOD_TRACK_STRING();
	DistanceUnit DDist = path.GetTotalLength();
	ElapsedTime DT = endTime - startState.m_tTime;

	AirsideFlightState midState = startState;
	if(DDist > 0 && ( startState.m_vSpeed + endSpd ) >0)
	{
		for(int i=1;i<path.getCount()-1;i++)
		{
			DistanceUnit realDist = path.GetIndexDist((float)i);
			double b = realDist / DDist;
			double r;

			r = b;

			midState.m_pPosition = path.getPoint(i); 
			midState.m_dAlt = path.getPoint(i).getZ();
			ElapsedTime dT = ElapsedTime ((double)DT * r);			
			midState.m_tTime = startState.m_tTime + dT;
			midState.m_dist = startState.m_dist + realDist;
			midState.m_vSpeed = startState.m_vSpeed * (1.0-r) + endSpd * r;
			midState.m_offsetAngle = 0;
			midState.m_dAcc = startState.m_dAcc;

			UpdateState(midState,false);				
		}
	}	


}
#include "../../Results/AirsideFlightEventLog.h"

AirsideFlightEventLog* getLogFromClearanceItem(AirsideFlightInSim* pFlight,ClearanceItem& item)
{
	PLACE_METHOD_TRACK_STRING();
	AirsideFlightEventLog* newLog = new AirsideFlightEventLog;
	newLog->time = item.GetTime();
	CPoint2008 pt = item.GetPosition();
	newLog->x = pt.getX(); newLog->y = pt.getY(); newLog->z = pt.getZ();
	newLog->mMode = item.GetMode();
	newLog->dSpd = item.GetSpeed();
	newLog->distInRes = item.GetDistInResource();
	newLog->state = (pFlight->IsArrivingOperation())?AirsideFlightEventLog::ARR:AirsideFlightEventLog::DEP ;
	//get resource information
	if(item.GetResource())
	{
		item.GetResource()->getDesc(newLog->resDesc);		
	}
	if(item.GetMode()==OnExitRunway&& pFlight->GetRunwayExit() )
	{
		newLog->m_ExitId = pFlight->GetRunwayExit()->GetID() ;
	}
	if(item.GetMode() == OnTakeOffEnterRunway && pFlight->GetAndAssignTakeoffPosition())
	{
		newLog->m_ExitId = pFlight->GetAndAssignTakeoffPosition()->GetID() ;
	}
	return newLog;
}

void AirsideFlightInSim::PerformClearanceItem( const ClearanceItem& _item )
{
	PLACE_METHOD_TRACK_STRING();
	ClearanceItem item = _item;
	// have to use the acceleration toward destination
	m_curState.m_dAcc = item.GetAcceleration(); // by Benny, search "forAccOrDecTag" in this .cpp file for help  #1

	AirsideFlightRunwayDelayLog* pLog = NULL;

	//release resource lock
	if(item.GetMode() == OnTerminate )
	{
		if (m_pLandingRunway && GetResource() == m_pLandingRunway)
		{
			m_pLandingRunway->RemoveLeftFlight(this);
		}

		Terminate(item.GetTime());

	}
    if( item.GetMode() >= OnLanding && item.GetMode() <= OnTakeoff || item.GetMode() == OnTaxiToDeice)
	{		
		if(GetAirTrafficController()->GetAirsideResourceManager())
		{
			const ALTAirport& airportInfo = GetAirTrafficController()->GetAirsideResourceManager()->GetAirportResource()->GetAirportInfo();
			item.SetAltitude(airportInfo.getElevation());
		}		
	}

	//log
	m_pReportLogItem->AddEvent(getLogFromClearanceItem(this,item));

	//Do Extra Performance	
	//Runway Register
	if( GetMode()!= item.GetMode() && item.GetMode() == OnLanding)		//enter runway
	{
		//ASSERT(m_id != 15);
		if (GetLandingRunway())
			GetLandingRunway()->AddFlightOnRunway(this);
	}
	if( GetMode() == OnExitRunway && (item.GetMode() == OnTaxiToStand || item.GetMode() == OnTaxiToTempParking || item.GetMode() == OnTaxiToMeetingPoint))		//leave runway
	{
		if (GetLandingRunway())
		{
			ResourceExitEvent* pEvent = new ResourceExitEvent(GetLandingRunway(), this);
			pEvent->setTime(item.GetTime());
			pEvent->addEvent();	
			GetLandingRunway()->SetExitTime(this,item.GetTime());
		}		
	}

	if( GetMode()!=item.GetMode() && item.GetMode() == OnHeldAtTempParking)
	{
		if(m_pRouteToTempParking)
			m_pRouteToTempParking->FlightExitRoute(this,item.GetTime(),item.GetResource() );		
	}
 
	if(m_preState.m_fltMode == OnExitRunway && (item.GetMode() == OnTaxiToStand || item.GetMode() == OnTaxiToMeetingPoint|| item.GetMode() == OnTaxiToTempParking))		//leave runway
	{
		if (mpCurrentDelayLog && mpCurrentDelayLog->mConflictLocation == FlightConflict::EXIT)
		{
			ElapsedTime tDelay = item.GetTime() - m_preState.m_tTime;
			ResourceDesc resDesc;
			GetLandingRunway()->getDesc(resDesc);
			pLog = new AirsideFlightRunwayDelayLog(resDesc,m_preState.m_tTime.asSeconds(), OnLanding, 0l, tDelay.asSeconds(), FlightRunwayDelay::Existing);
			CString strReason;
			strReason.Format("Exit blocked by AC %s",mpCurrentDelayLog->motherMobileDesc.strName.c_str());
			pLog->sReasonDetail = strReason.GetString(); 
		}

		EndDelay(item);		//end exit delay
	}

	if(GetMode() != OnTakeOffEnterRunway && item.GetMode() == OnTakeOffEnterRunway)		//enter runway
	{
		EndDelay(item);		//finish enter runway delay

	}

	if (GetMode() == OnTakeOffEnterRunway && item.GetMode() == OnPreTakeoff)	
	{
		QueueToTakeoffPos* pQueue = m_pDepatrueRunway->GetQueueList().GetAddQueue(m_pTakeoffPos);
		if (pQueue)
			pQueue->RemoveInQueueFlight(this);
		StartDelay(item, NULL, FlightConflict::STOPPED, FlightConflict::RUNWAY,FltDelayReason_Stop,_T("Take off delay"));		//take off delay
	}
	
	if (GetMode() == OnPreTakeoff && item.GetMode() == OnTakeoff)	
	{
		if (m_bTowingToReleasePoint && m_pTowingServiceVehicle)			//towing state
		{
			SetTowingServiceCompleted();
			m_pTowingServiceVehicle->GetCompleteReturnRouteToPool();
			ElapsedTime tUnhook = GetUnhookTime();
			m_pTowingServiceVehicle->WakeUp(GetTime()+ElapsedTime(tUnhook.asSeconds()*0.5));
			m_pTowingServiceVehicle = NULL;
		}
	}

	if (GetMode() == OnTakeoff )		//end take off delay
	{
		if(mpCurrentDelayLog)
		{
			long delayT = ((long)getRealOpTimeOnRunway()) - mpCurrentDelayLog->time;
			mpCurrentDelayLog->mDelayTime = delayT;
			LogEventItem(mpCurrentDelayLog);
			mpCurrentDelayLog = NULL;
		}
		if(m_pTakeoffPos)
			m_pTakeoffPos->RegTakeoffPos(this);
	}
	/////////////////////////////////////////////////////////
	if (GetMode()> OnLanding && GetMode()< OnClimbout && GetResource() && item.GetResource() && (GetMode() != item.GetMode()))
	{
		GetAirTrafficController()->GetAirportStatusManager()->NotifyAirportStatuses(GetResource(),item);
	}

	//Flight Arrival At Stand
	if(GetMode() != item.GetMode() && item.GetMode()== OnHeldAtStand 
		&& GetOperationParkingStand() && GetOperationParkingStand()->IsStandResource(item.GetResource()))	//except intermediate stand parking
	{
		//	wakeup passengers on this flight
		FlightArrivalStandEvent* pFtArrivalEvent = new FlightArrivalStandEvent(this);
		pFtArrivalEvent->SetArrivalFlight(m_pflightInput);
		pFtArrivalEvent->setTime(item.GetTime());
		if(this->IsDeparture())
			pFtArrivalEvent->SetActualDepartureTime(this->GetDepTime());
		pFtArrivalEvent->addEvent();
		// wakeup serving vehicles
// 		if(m_pFlightServiceInfomation)
// 		{
// 			m_pFlightServiceInfomation->GetFlightServiceRoute()->WakeupTheWaitingVehicleInRingRoad(item.GetTime());
// 		}

	}
	//Flight exit At Stand
	if(GetMode() != item.GetMode() && GetMode()== OnHeldAtStand )
	{
		StandInSim* opStand = GetOperationParkingStand();
		if( AirsideResource* atresouce = GetResource() )
		{
			if(atresouce->IsStandResource())
			{
				//	wakeup passengers on this flight
				FlightExitStandEvent* pFtArrivalEvent = new FlightExitStandEvent(this);

				pFtArrivalEvent->SetExitFlight(m_pflightInput);
				pFtArrivalEvent->setTime(item.GetTime());
				if(this->IsDeparture())
					pFtArrivalEvent->SetActualDepartureTime(this->GetDepTime());

				pFtArrivalEvent->addEvent();
			}
			else
			{
				ASSERT(FALSE);
			}
		}		
	}

	//perform general resource change 
	if( GetResource() != item.GetResource() )
	{
		if(GetResource()&& GetResource()->GetType() != AirsideResource::ResType_LogicRunway /*&& GetResource()->GetType() != AirsideResource::ResType_AirspaceHold*/)
		{				
			ReleaseResourceLockEvent * pReleseEvent = new ReleaseResourceLockEvent(this, GetResource());
			pReleseEvent->setTime( GetTime() );
			pReleseEvent->addEvent();		
			GetResource()->SetExitTime(this,GetTime());
		}
		if( item.GetResource() &&item.GetResource()->GetType() != AirsideResource::ResType_LogicRunway )
		{
			item.GetResource()->SetEnterTime(this,item.GetTime(),item.GetMode(),item.GetSpeed());
		}

		//if resource change to runway
		//record runway log
		if(GetResource() != NULL && item.GetResource()!= NULL)
		{
			if(GetResource()->GetType() != AirsideResource::ResType_LogicRunway &&
				item.GetResource()->GetType() == AirsideResource::ResType_LogicRunway )//entry runway
			{

				LogRunwayOperationLog(TRUE,item);
			}
			else if(GetResource()->GetType() == AirsideResource::ResType_LogicRunway &&
				item.GetResource()->GetType() != AirsideResource::ResType_LogicRunway )//exit runway
			{
				LogRunwayOperationLog(FALSE,item);
			}
		}

	}

	//flight exit takeoff Queue && exit taxiway;
	if( item.GetResource() && GetResource()!=item.GetResource() && item.GetResource()->GetType() == AirsideResource::ResType_LogicRunway && item.GetMode() >= OnPreTakeoff )
	{
		LogicRunwayInSim* pRunway = (LogicRunwayInSim*)item.GetResource();
		//pRunway->RemoveFlightInQueue(this);
		CAirsideRemoveFlightFromQueueEvent *pRemoveEvent = new CAirsideRemoveFlightFromQueueEvent(pRunway,this);
		pRemoveEvent->setTime(item.GetTime());
		pRemoveEvent->addEvent();

		if(m_pOutBoundRoute)
			m_pOutBoundRoute->FlightExitRoute(this, item.GetTime());

		if(m_pCircuitBoundRoute){
			m_pCircuitBoundRoute->FlightExitRoute(this,item.GetTime());
			delete m_pCircuitBoundRoute;
			m_pCircuitBoundRoute  = NULL;
		}
		//if(GetTakeoffRunway())
		//{
		//	CCheckRunwayWaveCrossEvent *pCheckEvent = new CCheckRunwayWaveCrossEvent(GetTakeoffRunway()->GetRunwayInSim(), this);
		//	pCheckEvent->setTime(item.GetTime());
		//	pCheckEvent->addEvent();
		//}
	}
	//flight exit takeoff Queue && exit taxiway;
	if( item.GetResource() &&
		GetResource() == item.GetResource() && 
		item.GetResource()->GetType() == AirsideResource::ResType_LogicRunway && 
		GetMode() != OnTakeoff  &&
		item.GetMode() == OnTakeoff)
	{
		if(GetAndAssignTakeoffRunway())
		{
			CCheckRunwayWaveCrossEvent *pCheckEvent = new CCheckRunwayWaveCrossEvent(GetAndAssignTakeoffRunway()->GetRunwayInSim(), this);
			pCheckEvent->setTime(item.GetTime());
			pCheckEvent->addEvent();
		}
	}

	//flight exit route to stand
	if( item.GetResource() && OnHeldAtStand ==item.GetMode() && GetMode()!=item.GetMode() )
	{
		if( m_pRouteToStand )
		{
			m_pRouteToStand->FlightExitRoute(this, item.GetTime() );	
			delete m_pRouteToStand;
			m_pRouteToStand = NULL;
		}
		if( m_pRouteToTempParking)
		{
			m_pRouteToTempParking->FlightExitRoute(this, item.GetTime()  );
			delete m_pRouteToTempParking ;
			m_pRouteToTempParking = NULL;
		}
		if(m_pTowingRoute)
		{
			m_pTowingRoute->FlightExitRoute(this, item.GetTime() );
			delete m_pTowingRoute;
			m_pTowingRoute = NULL;
		}
	}
	
	//flight exit runway
	if( item.GetMode()!= GetMode() && item.GetMode() == OnExitRunway && GetRunwayExit() )
	{
		//GetRunwayExit()->ReleaseExitLock(this, item.GetTime());
		SetDelayed(NULL);

		if(GetLandingRunway())
		{
			CCheckRunwayWaveCrossEvent *pCheckEvent = new CCheckRunwayWaveCrossEvent(GetLandingRunway()->GetRunwayInSim(),this);
			pCheckEvent->setTime(item.GetTime());
			pCheckEvent->addEvent();
		}

	}

	//flight exit temp parking
	if( item.GetResource()!= GetResource() && GetMode() == OnExitTempStand && GetTemporaryParking() )
	{
		GetTemporaryParking()->ReleaseLock(this);
		SetDelayed(NULL);
	}
	//set middle state
	bool bWritePath = false;
	if( GetResource() && item.GetResource())
	{
		bWritePath = true;
		if(GetResource()!=item.GetResource() && GetResource()->GetType() == AirsideResource::ResType_LogicRunway && item.GetResource()->GetType() == GetResource()->GetType() ) //backtrack on runway
		{ //perform backtrack 180dgree turn
			LogicRunwayInSim* pRunway = (LogicRunwayInSim*) GetResource();
			UTurnPath uPath(GetPosition(), pRunway->GetDistancePoint(GetDistInResource() + 2000),GetPosition(), 1000 );
			CPath2008 uTurnPath = uPath.GetPath();
			WritePathLog( mGroundPerform, uTurnPath,m_curState,item );
		}
		else if( GetResource() == item.GetResource() && GetResource()->GetType() == AirsideResource::ResType_TaxiwayDirSeg) //taxing on the same taxiway segment
		{
			TaxiwayDirectSegInSim * taxiseg = (TaxiwayDirectSegInSim* ) GetResource();	
			CPath2008 subPath = GetSubPath(taxiseg->GetPath(),GetDistInResource(),item.GetDistInResource());
			CPath2008 smoothPath = GenSmoothPath(subPath.getPointList(), subPath.getCount(),1000);
			WritePathLog(mGroundPerform,smoothPath, m_curState,item );				
		}
		else if( GetResource() == item.GetResource() && GetResource()->GetType() == AirsideResource::ResType_RunwayDirSeg) //taxing to runway segment
		{
			RunwayDirectSegInSim * taxiseg = (RunwayDirectSegInSim* ) GetResource();	
			CPath2008 subPath = GetSubPath(taxiseg->GetPath(),GetDistInResource(),item.GetDistInResource());
			CPath2008 smoothPath = GenSmoothPath(subPath.getPointList(), subPath.getCount(),1000);
			WritePathLog(mGroundPerform,smoothPath, m_curState,item );				
		}
		else if (GetMode() == OnFinalApproach && item.GetMode() == OnMissApproach				//miss approach path on runway
			&& (item.GetResource()->GetType() == AirsideResource::ResType_AirRouteIntersection || item.GetResource()->GetType() == AirsideResource::ResType_WayPoint))
		{
			CPath2008 MissApproachPath;
			std::vector<CPoint2008> _inputPts;
			std::vector<CPoint2008> _outputPts;

			CPoint2008 curPos = GetPosition();
			curPos.setZ(m_curState.m_dAlt);
			CPoint2008 nextPos = item.GetPosition();
			nextPos.setZ(item.GetAltitude());

			CPath2008 airPath;
			airPath.init(2);
			airPath[0] = GetLandingRunway()->GetOtherPort()->GetDistancePoint(0);
			airPath[1] = nextPos;

			double dRate = (m_curState.m_dAlt - airPath[0].getZ())/(item.GetAltitude() - airPath[0].getZ());
			if (dRate < 0)
			{
				dRate = 0;
			}
			else if(dRate > 1.0)
			{
				dRate = 1.0;
			}

			double dLength = airPath.GetTotalLength()*dRate;

			CPoint2008 point = airPath.GetDistPoint(dLength);


			CPath2008 inputPath;
			inputPath.init(3);
			inputPath[0] = curPos;
			inputPath[1] = point;
			inputPath[2] = nextPos;
			MissApproachPath = GenSmoothPath(inputPath.getPointList(),inputPath.getCount(), 400);

			WriteMissApproachOnRunwayPathLog( MissApproachPath, m_curState, item.GetTime() ,item.GetSpeed());

			//here to write runway operation log
			//entry
			LogRunwayOperationLog(TRUE,item);
			LogRunwayOperationLog(FALSE,item);


		}
		else if( GetResource()->GetType() == AirsideResource::ResType_TaxiwayDirSeg && item.GetResource()->GetType() == AirsideResource::ResType_TaxiwayDirSeg && item.GetResource()!= GetResource() )
		{
			TaxiwayDirectSegInSim * pDirSeg = (TaxiwayDirectSegInSim * )GetResource();
			IntersectionNodeInSim* pExitNode = pDirSeg->GetExitNode();			
			TaxiwayDirectSegInSim* pDestTaxiwaySeg = NULL;
			CPath2008 filetPath;

			do 
			{
				if(item.GetResource()->GetType() == AirsideResource::ResType_TaxiwayDirSeg)
				{
					pDestTaxiwaySeg = (TaxiwayDirectSegInSim*)item.GetResource();
					FilletTaxiway* pFiletTaxiway = pExitNode->GetFilletFromTo(pDirSeg, pDestTaxiwaySeg,GetDistInResource(),item.GetDistInResource());
					//has filet
					if(pFiletTaxiway != NULL)
					{
						CPath2008 thePath = pExitNode->GetFiletRoutePath( *pFiletTaxiway, pDirSeg->GetTaxiwaySeg()->GetTaxiwayID());
						std::vector<CPoint2008> vPts;
						vPts.reserve(thePath.getCount()+1);
						vPts.push_back(GetPosition());
						vPts.insert(vPts.end(),&thePath[0], &thePath[0]+thePath.getCount());
						vPts.push_back(item.GetPosition());
						filetPath.init(vPts.size(), &vPts[0]);
						break;
					}
					else if(pDirSeg->GetTaxiwaySeg()->GetTaxiway() == pDestTaxiwaySeg->GetTaxiwaySeg()->GetTaxiway())
					{
						TaxiwayInSim * pTaxiway = pDirSeg->GetTaxiwaySeg()->GetTaxiway();
						//DistanceUnit distF1 = GetDistInResource();
						//DistanceUnit distT1 = pDirSeg->GetEndDist()-1;
						//CPath2008 filetPath1 = GetSubPath(pDirSeg->GetPath(),distF1,distT1);
						DistanceUnit distF = pDirSeg->GetPosInTaxiway(GetDistInResource());
						
						DistanceUnit distT = pDestTaxiwaySeg->GetPosInTaxiway(item.GetDistInResource());

						if(distT<distF && distT+1<distF)
							distT = distT+1;
						if(distT>distF && distT -1 > distF )
							distT = distT -1;
						//DistanceUnit distF2 = 1;
						//DistanceUnit distT2 = item.GetDistInResource();
						filetPath = GetSubPath(pTaxiway->GetPath(),distF,distT);
						
						/*filetPath.init(filetPath1.getCount()+filetPath2.getCount());
						for(int i=0;i<filetPath1.getCount();i++)filetPath[i]= filetPath1[i];
						for(i=0;i<filetPath2.getCount();i++)filetPath[i+filetPath1.getCount()] = filetPath2[i];*/
						break;
					}
				}

				//does not has filet between taxiway and taxiway or taxiway to stand etc.
				std::vector<CPoint2008> _inputPts;
				std::vector<CPoint2008> _outputPts;
				int nOutCnt = 10;
				_inputPts.push_back(GetPosition());
				_inputPts.push_back(pDirSeg->GetExitNode()->GetNodeInput().GetPosition() );
				_inputPts.push_back(item.GetPosition());

				BizierCurve::GenCurvePoints(_inputPts, _outputPts,nOutCnt);
				CPoint2008 ptList[1024];
				for(size_t i=0; i<_outputPts.size(); i++)
				{
					ptList[i].setX(_outputPts.at(i).getX());
					ptList[i].setY(_outputPts.at(i).getY());
					ptList[i].setZ(0.0);
				}
				filetPath.init ( (int)_outputPts.size(), ptList);

			} while(FALSE);

			WritePathLog( mGroundPerform,filetPath, m_curState, item );
		}
		else if( GetResource()->GetType() == AirsideResource::ResType_TaxiwayDirSeg && item.GetResource()->GetType() == AirsideResource::ResType_RunwayDirSeg )
		{
			RunwayDirectSegInSim* pRunwaySeg = (RunwayDirectSegInSim*)item.GetResource();
			TaxiwayDirectSegInSim* pDirSeg = (TaxiwayDirectSegInSim*)GetResource();

			IntersectionNodeInSim* pNode = pDirSeg->GetExitNode();
			FilletTaxiway* pFiletTaxiway = pNode->GetFilletFromTo(pDirSeg, pRunwaySeg);
			CPath2008 filetPath;
			    //ASSERT(pFiletTaxiway);
			if(pFiletTaxiway != NULL)
			{
				CPath2008 thePath = pNode->GetFiletRoutePath( *pFiletTaxiway, pDirSeg->GetTaxiwaySeg()->GetTaxiwayID());
				std::vector<CPoint2008> vPts;
				vPts.reserve(thePath.getCount()+1);
				vPts.push_back(GetPosition());
				vPts.insert(vPts.end(),&thePath[0], &thePath[0]+thePath.getCount());
				vPts.push_back(item.GetPosition());
				filetPath.init(vPts.size(), &vPts[0]);				
			}
			else
			{//impossibility, avoid logic dis figuration
			 	TRACE("\nError! cannot find filet from taxiway to runway.\n");
			 	std::vector<CPoint2008> _inputPts;
			 	std::vector<CPoint2008> _outputPts;
			 	int nOutCnt = 10;
			 	_inputPts.push_back(GetPosition());
			 	//if (m_pTakeoffPos && m_pTakeoffPos->GetRouteSeg() != pDirSeg)
			 	//{
			 	//	_inputPts.push_back(m_pTakeoffPos->getPos());
			 	//}
			 	//else
			 	{
			 		_inputPts.push_back(pDirSeg->GetExitNode()->GetNodeInput().GetPosition() );
			 	}
			 	_inputPts.push_back(item.GetPosition());
	
			 	BizierCurve::GenCurvePoints(_inputPts, _outputPts,nOutCnt);
			 	CPoint2008 ptList[1024];
			 	for(size_t i=0; i<_outputPts.size(); i++)
			 	{
			 		ptList[i].setX(_outputPts.at(i).getX());
			 		ptList[i].setY(_outputPts.at(i).getY());
			 		ptList[i].setZ(0.0);
			 	}
			 	filetPath.init ( (int)_outputPts.size(), ptList);
			}
			WritePathLog( mGroundPerform,filetPath, m_curState,item);
			//RunwayDirectSegInSim* pDirRunway = (RunwayDirectSegInSim*)item.GetResource();
			//WritePathLog( mGroundPerform,GetFilletPathFromTaxiwayToRwyport(pDirRunway->GetLogicRunwayType()),
			//	m_curState, item);
		}
		else if( GetResource()->GetType() == AirsideResource::ResType_RunwayDirSeg && item.GetResource()->GetType() == AirsideResource::ResType_TaxiwayDirSeg)
		{
			RunwayDirectSegInSim * pDirSeg = (RunwayDirectSegInSim * )GetResource();
			IntersectionNodeInSim* pExitNode = pDirSeg->GetExitNode();			
			TaxiwayDirectSegInSim* pDestTaxiwaySeg = NULL;
			CPath2008 filetPath;

			do 
			{
				if(item.GetResource()->GetType() == AirsideResource::ResType_TaxiwayDirSeg)
				{
					pDestTaxiwaySeg = (TaxiwayDirectSegInSim*)item.GetResource();
					FilletTaxiway* pFiletTaxiway = pExitNode->GetFilletFromTo(pDirSeg, pDestTaxiwaySeg );
					//has filet
					if(pFiletTaxiway != NULL)
					{
						CPath2008 thePath = pExitNode->GetFiletRoutePath( *pFiletTaxiway, pDirSeg->GetRunwaySeg()->GetRunwayID());
						std::vector<CPoint2008> vPts;
						vPts.reserve(thePath.getCount()+1);
						vPts.push_back(GetPosition());
						vPts.insert(vPts.end(),&thePath[0], &thePath[0]+thePath.getCount());
						vPts.push_back(item.GetPosition());
						filetPath.init(vPts.size(), &vPts[0]);
						break;
					}
					//else if(pDirSeg->GetTaxiwaySeg()->GetTaxiway() == pDestTaxiwaySeg->GetTaxiwaySeg()->GetTaxiway())
					//{
					//	TaxiwayInSim * pTaxiway = pDirSeg->GetTaxiwaySeg()->GetTaxiway();
					//	DistanceUnit distF = pDirSeg->GetPosInTaxiway(GetDistInResource());
					//	DistanceUnit distT = pDestTaxiwaySeg->GetPosInTaxiway(item.GetDistInResource());
					//	filetPath = GetSubPath(pTaxiway->GetTaxiwayInput()->GetPath(),distF,distT);
					//	break;
					//}
				}

				//does not has fillet between taxiway and taxiway or taxiway to stand etc.
				std::vector<CPoint2008> _inputPts;
				std::vector<CPoint2008> _outputPts;
				int nOutCnt = 10;
				_inputPts.push_back(GetPosition());
				_inputPts.push_back(pDirSeg->GetExitNode()->GetNodeInput().GetPosition() );
				_inputPts.push_back(item.GetPosition());

				BizierCurve::GenCurvePoints(_inputPts, _outputPts,nOutCnt);
				CPoint2008 ptList[1024];
				for(size_t i=0; i<_outputPts.size(); i++)
				{
					ptList[i].setX(_outputPts.at(i).getX());
					ptList[i].setY(_outputPts.at(i).getY());
					ptList[i].setZ(0.0);
				}
				filetPath.init ( (int)_outputPts.size(), ptList);

			} while(FALSE);

			WritePathLog( mGroundPerform,filetPath, m_curState,item);
		}
		else if( GetResource()->GetType() == AirsideResource::ResType_LogicRunway && item.GetResource()->GetType() == AirsideResource::ResType_TaxiwayDirSeg )  //smooth way from runway to taxiway
		{

			LogicRunwayInSim* pLogicRunway = (LogicRunwayInSim*)GetResource();
			RunwayExitInSim* pRwExit = m_pRunwayExit;


			TaxiwayDirectSegInSim * pDirSeg = (TaxiwayDirectSegInSim*)item.GetResource();
			if(pRwExit && pRwExit->GetRouteSeg()->GetType() == AirsideResource::ResType_TaxiwayDirSeg){
				pDirSeg = (TaxiwayDirectSegInSim*)pRwExit->GetRouteSeg();
			}
			
			IntersectionNodeInSim* pEntryNode = pDirSeg->GetEntryNode();
			FilletTaxiway* pFiletTaxiway = pEntryNode->GetFilletFromTo(pLogicRunway, pDirSeg);
			
			ReleaseResourceLockEvent * pReleseEvent = new ReleaseResourceLockEvent(this, pEntryNode);
			pReleseEvent->setTime( GetTime() );
			pReleseEvent->addEvent();		
			//GetResource()->SetExitTime(this,GetTime());

			CPath2008 filetPath;

			//ASSERT(pFiletTaxiway);
			if(pFiletTaxiway != NULL)
			{
				CPath2008 thePath = pEntryNode->GetFiletRoutePath( *pFiletTaxiway, 
					pLogicRunway->GetRunwayInSim()->GetRunwayInput()->getID());
				std::vector<CPoint2008> vPts;
				vPts.reserve(thePath.getCount()+1);
				vPts.push_back(GetPosition());
				vPts.insert(vPts.end(),&thePath[0], &thePath[0]+thePath.getCount());
				vPts.push_back(item.GetPosition());
				filetPath.init(vPts.size(), &vPts[0]);
			}
			else
			{//impossibility, avoid logic disfiguration
				TRACE("\nError! cannot find filet from taxiway to runway.\n");
				std::vector<CPoint2008> _inputPts;
				std::vector<CPoint2008> _outputPts;
				int nOutCnt = 10;
				_inputPts.push_back(GetPosition());
				_inputPts.push_back(pDirSeg->GetExitNode()->GetNodeInput().GetPosition() );
				_inputPts.push_back(item.GetPosition());

				BizierCurve::GenCurvePoints(_inputPts, _outputPts,nOutCnt);
				CPoint2008 ptList[1024];
				for(size_t i=0; i<_outputPts.size(); i++)
				{
					ptList[i].setX(_outputPts.at(i).getX());
					ptList[i].setY(_outputPts.at(i).getY());
					ptList[i].setZ(0.0);
				}
				filetPath.init ( (int)_outputPts.size(), ptList);
			}
			DistanceUnit distOfPath = filetPath.GetTotalLength();
			double avgSpd = ( GetSpeed() + item.GetSpeed() ) * 0.5;
			
			ElapsedTime dT = ElapsedTime(0L);
			if(avgSpd > 0.0)
				dT = ElapsedTime(distOfPath/avgSpd);
			
			item.SetTime(GetTime()+dT);

			if (pLog)		//runway exit delay log
			{
				pLog->mAvailableTime = item.GetTime().asSeconds();
				LogEventItem(pLog);
			}
			WritePathLog( mGroundPerform,filetPath, m_curState, item );		

		}
		else if( GetResource()->GetType() == AirsideResource::ResType_LogicRunway && item.GetResource()->GetType() == AirsideResource::ResType_RunwayDirSeg )  //smooth way from runway to taxiway
		{

			LogicRunwayInSim* pLogicRunway = (LogicRunwayInSim*)GetResource();
			RunwayDirectSegInSim * pDirSeg = (RunwayDirectSegInSim * )item.GetResource();
			IntersectionNodeInSim* pEntryNode = pDirSeg->GetEntryNode();
			FilletTaxiway* pFiletTaxiway = pEntryNode->GetFilletFromTo(pLogicRunway, pDirSeg);
			
			ReleaseResourceLockEvent * pReleseEvent = new ReleaseResourceLockEvent(this, pEntryNode);
			pReleseEvent->setTime( GetTime() );
			pReleseEvent->addEvent();		
			//GetResource()->SetExitTime(this,GetTime());

			CPath2008 filetPath;

			//ASSERT(pFiletTaxiway);
			if(pFiletTaxiway != NULL)
			{
				CPath2008 thePath = pEntryNode->GetFiletRoutePath( *pFiletTaxiway, 
					pLogicRunway->GetRunwayInSim()->GetRunwayInput()->getID());
				std::vector<CPoint2008> vPts;
				vPts.reserve(thePath.getCount()+1);
				vPts.push_back(GetPosition());
				vPts.insert(vPts.end(),&thePath[0], &thePath[0]+thePath.getCount());
				vPts.push_back(item.GetPosition());
				filetPath.init(vPts.size(), &vPts[0]);
			}
			else
			{//impossibility, avoid logic disfiguration
				TRACE("\nError! cannot find filet from taxiway to runway.\n");
				std::vector<CPoint2008> _inputPts;
				std::vector<CPoint2008> _outputPts;
				int nOutCnt = 10;
				_inputPts.push_back(GetPosition());
				_inputPts.push_back(GetPosition());
				_inputPts.push_back(item.GetPosition());

				BizierCurve::GenCurvePoints(_inputPts, _outputPts,nOutCnt);
				CPoint2008 ptList[1024];
				for(size_t i=0; i<_outputPts.size(); i++)
				{
					ptList[i].setX(_outputPts.at(i).getX());
					ptList[i].setY(_outputPts.at(i).getY());
					ptList[i].setZ(0.0);
				}
				filetPath.init ( (int)_outputPts.size(), ptList);
			}
			DistanceUnit distOfPath = filetPath.GetTotalLength();
			double avgSpd = ( GetSpeed() + item.GetSpeed() ) * 0.5;

			ElapsedTime dT = ElapsedTime(0L);
			if(avgSpd > 0.0)
				dT = ElapsedTime(distOfPath/avgSpd);

			item.SetTime(GetTime()+dT);
			WritePathLog( mGroundPerform,filetPath, m_curState,item);		

		}
		else if( GetResource()->GetType() == AirsideResource::ResType_TaxiwayDirSeg && item.GetResource()->GetType() == AirsideResource::ResType_LogicRunway ) //from taxiway to runway
		{
// 			LogicRunwayInSim* pLogicRunway = (LogicRunwayInSim*)item.GetResource();
// 			IntersectionNodeInSim* pNode = pDirSeg->GetExitNode();
// 			FilletTaxiway* pFiletTaxiway = pNode->GetFilletFromTo(pDirSeg, pLogicRunway);
// 			CPath2008 filetPath;
//             //ASSERT(pFiletTaxiway);
// 			if(pFiletTaxiway != NULL)
// 			{
// 				CPath2008 leftPath;
// 				if(pDirSeg){
// 					leftPath = pDirSeg->GetPath().GetLeftPath( GetDistInResource() );
// 				}
// 
// 				CPath2008 thePath = pNode->GetFiletRoutePath( *pFiletTaxiway, pDirSeg->GetTaxiwayID());
// 				DistanceUnit dDist = leftPath.GetPointDist(thePath.getPoint(0));
// 				CPath2008 pathInTaxiway = leftPath.GetSubPath(0,dDist);
// 				CPath2008 smoothTaxiPath = GenSmoothPath(pathInTaxiway.getPointList(), pathInTaxiway.getCount(),1000);
// 
// 				std::vector<CPoint2008> vPts;
// 				vPts.reserve(thePath.getCount()+smoothTaxiPath.getCount());
// 				
// 				vPts.insert(vPts.end(),&smoothTaxiPath[0], &smoothTaxiPath[0]+smoothTaxiPath.getCount());
// 				vPts.insert(vPts.end(),&thePath[0], &thePath[0]+thePath.getCount());
// 				vPts.push_back(item.GetPosition());
// 				filetPath.init(vPts.size(), &vPts[0]);
// 			}
// 			else
// 			{//impossibility, avoid logic dis figuration
// 				TRACE("\nError! cannot find filet from taxiway to runway.\n");
// 				std::vector<CPoint2008> _inputPts;
// 				std::vector<CPoint2008> _outputPts;
// 				int nOutCnt = 10;
// 				_inputPts.push_back(GetPosition());
// 				if (m_pTakeoffPos && m_pTakeoffPos->GetRouteSeg() != pDirSeg)
// 				{
// 					_inputPts.push_back(m_pTakeoffPos->getPos());
// 				}
// 				else
// 				{
// 					_inputPts.push_back(pDirSeg->GetExitNode()->GetNodeInput().GetPosition() );
// 				}
// 				_inputPts.push_back(item.GetPosition());
// 
// 				BizierCurve::GenCurvePoints(_inputPts, _outputPts,nOutCnt);
// 				CPoint2008 ptList[1024];
// 				for(size_t i=0; i<_outputPts.size(); i++)
// 				{
// 					ptList[i].setX(_outputPts.at(i).getX());
// 					ptList[i].setY(_outputPts.at(i).getY());
// 					ptList[i].setZ(0.0);
// 				}
// 				filetPath.init ( (int)_outputPts.size(), ptList);
// 			}
			LogicRunwayInSim* pLogicRunway = (LogicRunwayInSim*)item.GetResource();
			WritePathLog( mGroundPerform, GetFilletPathFromTaxiwayToRwyport(pLogicRunway->getLogicRunwayType()),m_curState, item);
		}
		else if( GetResource()->GetType() == AirsideResource::ResType_RunwayDirSeg && item.GetResource()->GetType() == AirsideResource::ResType_LogicRunway )
		{
			LogicRunwayInSim* pLogicRunway = (LogicRunwayInSim*)item.GetResource();
			RunwayDirectSegInSim * pDirSeg = (RunwayDirectSegInSim * )GetResource();
			IntersectionNodeInSim* pNode = pDirSeg->GetExitNode();
			FilletTaxiway* pFiletTaxiway = pNode->GetFilletFromTo(pDirSeg, pLogicRunway);

			CPath2008 filetPath;

			//ASSERT(pFiletTaxiway);
			if(pFiletTaxiway != NULL)
			{
				CPath2008 thePath = pNode->GetFiletRoutePath( *pFiletTaxiway, pDirSeg->GetRunwaySeg()->GetRunwayID());
				std::vector<CPoint2008> vPts;
				vPts.reserve(thePath.getCount()+1);
				vPts.push_back(GetPosition());
				vPts.insert(vPts.end(),&thePath[0], &thePath[0]+thePath.getCount());
				vPts.push_back(item.GetPosition());
				filetPath.init(vPts.size(), &vPts[0]);
			}
			else
			{//impossibility, avoid logic dis figuration
				TRACE("\nError! cannot find filet from taxiway to runway.\n");
				std::vector<CPoint2008> _inputPts;
				std::vector<CPoint2008> _outputPts;
				int nOutCnt = 10;
				_inputPts.push_back(GetPosition());
				_inputPts.push_back(pDirSeg->GetExitNode()->GetNodeInput().GetPosition() );
				_inputPts.push_back(item.GetPosition());

				BizierCurve::GenCurvePoints(_inputPts, _outputPts,nOutCnt);
				CPoint2008 ptList[1024];
				for(size_t i=0; i<_outputPts.size(); i++)
				{
					ptList[i].setX(_outputPts.at(i).getX());
					ptList[i].setY(_outputPts.at(i).getY());
					ptList[i].setZ(0.0);
				}
				filetPath.init ( (int)_outputPts.size(), ptList);
			}
			WritePathLog( mGroundPerform,filetPath, m_curState, item );
		}

		else if (GetResource()->GetType() == AirsideResource::ResType_StandLeadInLine && item.GetResource()->GetType() == AirsideResource::ResType_StandLeadOutLine)
		{
			StandLeadInLineInSim* pInLine = (StandLeadInLineInSim*) GetResource();
			StandLeadOutLineInSim* pOutLine = (StandLeadOutLineInSim*) item.GetResource();
			m_curState.m_bPushBack = /*pOutLine->IsPushBack()*/item.IsPushback(); 
			CPath2008 inLinePath =  pInLine->GetPath();
				
			if(!m_curState.m_bPushBack)
			{	
				inLinePath = GetSubPath(pInLine->GetPath(),GetDistInResource(),pInLine->GetEndDist());
				CPoint2008 ptNext = item.GetPosition();
				inLinePath.insertPointAfterAt(ptNext, inLinePath.getCount()-1);
				inLinePath = GenSmoothPath(inLinePath.getPointList(),inLinePath.getCount(), 1000);
				WritePathLog(mGroundPerform, inLinePath, m_curState , item);
			}
		}
		else if( GetResource()->GetType() == AirsideResource::ResType_StandLeadOutLine /*&& GetResource() != item.GetResource()*/ )
		{			
			StandLeadOutLineInSim* pOutLine = (StandLeadOutLineInSim*) GetResource();
			m_curState.m_bPushBack = /*pOutLine->IsPushBack()*/item.IsPushback(); 
			CPath2008 outLinePath =  pOutLine->GetPath();
			if (GetResource() != item.GetResource())
			{
				outLinePath = GetSubPath(pOutLine->GetPath(), GetDistInResource(), pOutLine->GetEndDist() - 1000);

				if(m_curState.m_bPushBack && pOutLine->IsReleasePoint())
				{	
					CPoint2008 pt = item.GetPosition();
					CPoint2008 dir = CPoint2008(pOutLine->GetPath().GetIndexDir(pOutLine->GetPath().getCount()-1.0f));
					dir.rotate(90);
					dir.length(1000);	

					ClearanceItem releaseItem = item;
					releaseItem.SetDistInResource(item.GetDistInResource() - 1000);
					outLinePath.clear();
					outLinePath.init(6);
					outLinePath[0] = GetPosition();
					outLinePath[1] = GetPosition();
					ClearanceItem connerItem(GetResource(),GetMode(),GetDistInResource()-1000);
					outLinePath[2] = connerItem.GetPosition();
					outLinePath[3] = GetPosition()+dir;
					outLinePath[4] = releaseItem.GetPosition();
					outLinePath[5] = item.GetPosition();
					
					m_curState.m_bPushBack = false;
				}
				else
				{
					outLinePath.insertPointAfterAt(item.GetPosition(), outLinePath.getCount() - 1);
				}
	 
				outLinePath = GenSmoothPath(outLinePath.getPointList(),outLinePath.getCount(), 900);

				MobileGroundPerform pushbackPerform;
				pushbackPerform.mNoramlSpd = GetPerformance()->getPushBackSpeed(this);
				pushbackPerform.mAccSpd = GetPerformance()->getPushBackAcc(this);
				pushbackPerform.mDecSpd = GetPerformance()->getPushBackDec(this);

				WritePathLog( pushbackPerform,outLinePath, m_curState , item );
			}
			else
			{
				if(GetDistInResource()!=item.GetDistInResource())
				{
					CPath2008 thePath;
					thePath  = GetSubPath(pOutLine->GetPath(), GetDistInResource(), item.GetDistInResource() );			
					thePath = GenSmoothPath(thePath.getPointList(),thePath.getCount(), 1000);
					WritePathLog( mGroundPerform,thePath, m_curState, item);
				}
			}
			
		}
		else if( item.GetResource()->GetType() == AirsideResource::ResType_StandLeadInLine )
		{
			StandLeadInLineInSim* pInLine = (StandLeadInLineInSim*) item.GetResource();
			CPath2008 thePath;
			if( item.GetResource() != GetResource() )
			{
				CPath2008 subpath = GetSubPath(pInLine->GetPath(), 0, item.GetDistInResource() );
				std::vector<CPoint2008> vPoints;
				vPoints.reserve(subpath.getCount()+1);
				vPoints.push_back(GetPosition());
				vPoints.insert(vPoints.end(), &subpath[0], &subpath[0]+subpath.getCount()); 
				thePath.init(vPoints.size(), &vPoints[0]);
				thePath = GenSmoothPath(thePath.getPointList(),thePath.getCount(), 1000);
				WritePathLog( mGroundPerform,thePath, m_curState, item);
			}
			else
			{
				if(GetDistInResource()!=item.GetDistInResource())
				{
					thePath  = GetSubPath(pInLine->GetPath(), GetDistInResource(), item.GetDistInResource() );			
					thePath = GenSmoothPath(thePath.getPointList(),thePath.getCount(), 1000);
					WritePathLog( mGroundPerform,thePath, m_curState, item);
				}
			}
		}

		else if (item.GetResource()->GetType() == AirsideResource::ResType_AirspaceHold)
		{
			AirspaceHoldInSim* pHold = (AirspaceHoldInSim*)item.GetResource();
			if (item.GetDistInResource() < GetDistInResource())		//loop path
			{
				CPath2008 subPath1 = GetSubPath(pHold->GetPath(),GetDistInResource(),pHold->GetPath().GetTotalLength());
				double dRate = (pHold->GetPath().GetTotalLength() - GetDistInResource())/(GetDistInResource() - item.GetDistInResource());

				ElapsedTime dTime = item.GetTime() - GetTime();
				double dTimeValue = dTime.asSeconds()* dRate;
				double dHeightValue = m_curState.m_dAlt + (item.GetAltitude()-  m_curState.m_dAlt)*dRate;
				ClearanceItem tempItem(item.GetResource(),item.GetMode(),item.GetDistInResource());
				tempItem.SetTime(GetTime() + ElapsedTime(dTimeValue));
				tempItem.SetSpeed(item.GetSpeed());
				tempItem.SetAltitude(dHeightValue);
				WritePathLog( mGroundPerform,subPath1, m_curState,tempItem);

				CPath2008 subPath2 = GetSubPath(pHold->GetPath(), 0.0, item.GetDistInResource());
				WritePathLog( mGroundPerform,subPath2, m_curState,item);
			}
			else
			{
				CPath2008 subPath = GetSubPath(pHold->GetPath(),GetDistInResource(),item.GetDistInResource());
				WritePathLog( mGroundPerform,subPath, m_curState,item);
			}

		}
		else if( GetResource()!= item.GetResource() && item.GetResource()->GetType()==AirsideResource::ResType_DeiceStation) //from taxiway to deice station
		{
			DeicePadInSim* pDeicePad = (DeicePadInSim*) item.GetResource();
			
			CPath2008 thePath;
			CPath2008 subpath = GetSubPath(pDeicePad->GetInPath(), 0, item.GetDistInResource()-1 );
			std::vector<CPoint2008> vPoints;
			vPoints.reserve(subpath.getCount()+1);
			vPoints.push_back(GetPosition());
			vPoints.insert(vPoints.end(), &subpath[0], &subpath[0]+subpath.getCount()); 
			thePath.init(vPoints.size(), &vPoints[0]);			
			thePath = GenSmoothPath(thePath.getPointList(),thePath.getCount(), 1000);
			WritePathLog( mGroundPerform,thePath, m_curState, item);
		}
		else if( GetResource()!= item.GetResource() && GetResource()->GetType() == AirsideResource::ResType_DeiceStation ) //form deice station to taxiway
		{
			DeicePadInSim* pDeicePad = (DeicePadInSim*) GetResource();
			
			CPath2008 thePath = pDeicePad->GetOutPath();
			thePath.insertPointAfterAt(item.GetPosition(), thePath.getCount()-1);
					
			thePath = GenSmoothPath(thePath.getPointList(),thePath.getCount(), 1000);
			double avgSpd = (GetSpeed() + item.GetSpeed()) * 0.5;
			ElapsedTime dT = ElapsedTime(0L);
			if(avgSpd > 0.0)
				dT = ElapsedTime(thePath.GetTotalLength()/avgSpd);

			ElapsedTime endT = m_curState.m_tTime+ dT;
			item.SetTime(endT);
			WritePathLog( mGroundPerform,thePath, m_curState,item);
			
		}
		else{ bWritePath = false; }
	}
	//m_tDelay += item.GetDelayTime();
	//m_nDelayRe = item.GetDelayReason();
		
	AirsideFlightState ItemState;
	ItemState.m_pResource = item.GetResource();
	ItemState.m_fltMode = item.GetMode();
	ItemState.m_pPosition = item.GetPosition();
	ItemState.m_dAlt = item.GetAltitude();
	ItemState.m_tTime = max(item.GetTime(),GetTime());
	ItemState.m_dist = item.GetDistInResource();
	ItemState.m_vSpeed = item.GetSpeed();
	ItemState.m_offsetAngle = item.GetOffsetAngle();
	ItemState.m_nDelayId = item.GetDelayId();
	ItemState.m_dAcc = 0.0;//item.GetAcceleration(); // by Benny, search "forAccOrDecTag" in this .cpp file for help  #2
	ItemState.m_bPushBack = item.IsPushback();

	//if(item.GetResource() && item.GetResource()->GetType() == AirsideResource::ResType_StandLeadOutLine)
	//{
	//	StandLeadOutLineInSim*pOutLine = (StandLeadOutLineInSim*)item.GetResource();
	//	ItemState.m_bPushBack = /*pOutLine->IsPushBack()*/item.IsPushback();
	//}
	
	UpdateState(ItemState,(!bWritePath&& item.GetResource() != GetResource()));

	if (m_bTowingToReleasePoint && m_pTowingServiceVehicle)		//towing state
	{
		//need minus the distance to flight
		double dist = item.GetDistInResource();		
		//if (GetMode() == OnExitStand && item.GetResource()->GetType() == AirsideResource::ResType_StandLeadOutLine
		//	&& ((StandLeadOutLineInSim*)item.GetResource())->IsPushBack())
		if (item.IsPushback())
			dist = dist - (GetLength()*0.5 - m_pTowingServiceVehicle->GetVehicleLength()*0.5);
		else
			dist = dist + (GetLength()*0.5 - m_pTowingServiceVehicle->GetVehicleLength()*0.5);

		ClearanceItem towtruckItem(item.GetResource(),item.GetMode(),dist);
		towtruckItem.SetTime(max(item.GetTime(),GetTime()));
		towtruckItem.SetSpeed(item.GetSpeed());
		CPoint2008 pos = item.GetResource()->GetDistancePoint(dist);
		towtruckItem.SetPosition(pos);

		m_pTowingServiceVehicle->UpdateState(towtruckItem);
		//if (GetMode() == OnExitStand && item.GetResource()->GetType() == AirsideResource::ResType_StandLeadOutLine
		//	&& ((StandLeadOutLineInSim*)item.GetResource())->IsPushBack())
		if (item.IsPushback())
			m_pTowingServiceVehicle->WirteLog(towtruckItem.GetPosition(),towtruckItem.GetSpeed(),towtruckItem.GetTime(),true);
		else
			m_pTowingServiceVehicle->WirteLog(towtruckItem.GetPosition(),towtruckItem.GetSpeed(),towtruckItem.GetTime());

	}

	if (m_bFollowMeCarServicing && m_pServiceFollowMeCar)
	{
		//need add the distance to flight
		double dist = item.GetDistInResource() + m_LogEntry.GetAirsideDescStruct().dLength;		

		ClearanceItem carItem(item.GetResource(),item.GetMode(),dist);
		carItem.SetTime(item.GetTime());
		carItem.SetSpeed(item.GetSpeed());
		CPoint2008 pos = item.GetResource()->GetDistancePoint(dist);
		carItem.SetPosition(pos);

		m_pServiceFollowMeCar->UpdateState(carItem);
		m_pServiceFollowMeCar->WirteLog(carItem.GetPosition(),carItem.GetSpeed(),carItem.GetTime());
	}

	
	if(m_preState.m_fltMode != m_curState.m_fltMode && m_curState.m_fltMode== OnHeldAtStand 
		&& GetOperationParkingStand() && GetOperationParkingStand()->IsStandResource(item.GetResource()))
	{
		OpenDoors(GetTime());
	}

}
//
FlightRouteInSim * AirsideFlightInSim::GetArrFlightPlan()
{
	if(!m_pArrivalPlan)
		m_pATC->AssignARRFlightPlan(this);
	return m_pArrivalPlan;
}

FlightRouteInSim * AirsideFlightInSim::GetDepFlightPlan()
{
	if(!m_pDeparturePlan)
		m_pATC->AssignDEPFlightPlan(this);
	return m_pDeparturePlan;
}

LogicRunwayInSim * AirsideFlightInSim::GetLandingRunway()
{
	if(!m_pLandingRunway){
		m_pATC->AssignLandingRunway(this);
	}
	return m_pLandingRunway;
}

LogicRunwayInSim * AirsideFlightInSim::GetAndAssignTakeoffRunway()
{

	
	if(!m_pDepatrueRunway){
		m_pATC->AssignTakeoffRunway(this);
	}
	return m_pDepatrueRunway;
}

TaxiRouteInSim * AirsideFlightInSim::GetAndAssignOutBoundRoute()
{
	if(!m_pOutBoundRoute){
		m_pATC->AssignOutBoundRoute(this);
	}
	return m_pOutBoundRoute;
}

TaxiRouteInSim* AirsideFlightInSim::GetAndAssignCircuitOutBoundRoute()
{
	if(!m_pOutBoundRoute){
		m_pATC->AssignCircuitOutBoundRoute((AirsideCircuitFlightInSim*)this);
	}
	return m_pOutBoundRoute;
}

TaxiRouteInSim* AirsideFlightInSim::GetAndAssignCircuitInBoundRoute()
{
	if(!m_pRouteToStand){
		m_pATC->AssignCircuitInBoundRoute((AirsideCircuitFlightInSim*)this);
	}
	return m_pRouteToStand;
}

TaxiRouteInSim* AirsideFlightInSim::GetTowingRoute()
{
	return m_pTowingRoute;
}

StandInSim * AirsideFlightInSim::GetOperationParkingStand()
{
	if (IsArrivingOperation())
	{
		return m_pArrParkingStand;
	}


	return m_pDepParkingStand;
}

StandInSim* AirsideFlightInSim::GetPlanedParkingStand(FLIGHT_PARKINGOP_TYPE type)
{
	PLACE_METHOD_TRACK_STRING();
	if ((type == ARR_PARKING && !IsArrival()) || (type == DEP_PARKING && !IsDeparture()))
		return NULL;

	StandResourceManager * pStandRes = m_pATC->GetAirsideResourceManager()->GetAirportResource()->getStandResource();

	ALTObjectID standID;
	if (type == ARR_PARKING)
		standID = getArrStand();
	if (type == DEP_PARKING)
		standID = getDepStand();
	if (type == INT_PARKING)
		standID = m_pflightInput->getIntermediateStand();


	return pStandRes->GetStandByName(standID);
}

RunwayExitInSim * AirsideFlightInSim::GetRunwayExit()
{
	return m_pRunwayExit;
}

RunwayExitInSim * AirsideFlightInSim::GetAndAssignTakeoffPosition()
{
	if(!m_pTakeoffPos){
		m_pATC->AssignTakeoffPosition(this);
	}
	return m_pTakeoffPos;
}	

StandInSim* AirsideFlightInSim::GetIntermediateParking()
{
	if (!m_pIntermediateParking)
	{
		m_pATC->AssignIntermediateParking(this);
	}
	return m_pIntermediateParking;
}

ElapsedTime AirsideFlightInSim::GetTimeBetween( const ClearanceItem& item1, const ClearanceItem& item2 )const
{
	PLACE_METHOD_TRACK_STRING();

	//default calculate time to the 
	ElapsedTime dT;
	double avgspd = (item1.GetSpeed() + item2.GetSpeed()) * 0.5;
	if(avgspd <= 0)
	{
		//ASSERT(false);
		return ElapsedTime(0L);
	}

	if (item2.GetResource() == NULL)
		return 0L;

	DistanceUnit dDist;

	if (item1.GetResource() && (item1.GetResource()->GetType() == AirsideResource::ResType_HeadingAirRouteSegment|| item1.GetResource()->GetType() == AirsideResource::ResType_AirRouteSegment) )
		dDist = (item1.GetPosition()).distance(item2.GetPosition());
	else
		dDist = abs(item1.DistanceTo(item2));

	dT = ElapsedTime(dDist / avgspd);
	return dT;
}

#include <common\FileOutPut.h>

void AirsideFlightInSim::WriteLog( bool bEnterRes /*= false*/)
{
	PLACE_METHOD_TRACK_STRING();
	AirsideFlightEventStruct newEvent;
	memset(&newEvent,0,sizeof(AirsideFlightEventStruct));

	newEvent.nodeNO = -1;
	newEvent.intNodeID = -1;
	newEvent.state = m_curFlightType;
	CPoint2008 p = GetPosition();
	newEvent.x = (float) p.getX();
	newEvent.y =(float) p.getY();
	newEvent.z = (float) m_curState.m_dAlt;
	newEvent.time = GetTime();
	//newEvent.delayTime = m_tDelay;
	//newEvent.reason = (int)m_nDelayRe;
	newEvent.mode = (int)GetMode();
	newEvent.speed = GetSpeed();	
	newEvent.IsBackUp = GetCurState().m_bPushBack ;	
	newEvent.eventCode = bEnterRes?('e'):(' ');
	newEvent.offsetAngle = (float)m_curState.m_offsetAngle;
	newEvent.lDelayIndex = m_curState.m_nDelayId;
	newEvent.dacc = m_curState.m_dAcc;
	newEvent.towOperation = m_bTowingToReleasePoint?true:false;
	newEvent.deiceParking = mbDeicParking?true:false;
// 	newEvent.IsSpeedConstraint = false;
	m_curState.m_nDelayId = 0;
	
	if( m_pSTAR &&  IsArrivingOperation() )
	{
		CString strName = m_pSTAR->GetName();		
		strcpy(newEvent.starsidname,strName.Left(FLIGHTIDLEN) );
	}
	if( m_pSID && IsDepartingOperation() )
	{
		CString strName = m_pSID->GetName();		
		strcpy(newEvent.starsidname,strName.Left(FLIGHTIDLEN) );
	}
	if( m_pLandingRunway && m_curFlightType=='A')
	{
		CString strRw = m_pLandingRunway->PrintResource();			
		strcpy(newEvent.rwcode,strRw.Left(FLIGHTIDLEN));
	}
	if( m_pDepatrueRunway && m_curFlightType == 'D' )
	{
		CString strRw = m_pDepatrueRunway->PrintResource();
		strcpy(newEvent.rwcode,strRw.Left(FLIGHTIDLEN));
	}
	if(GetOperationParkingStand())
	{
		CString strStand = GetOperationParkingStand()->PrintResource();
		strcpy(newEvent.gatecode, strStand.Left(FLIGHTIDLEN));
	}


	AirsideResource* pRes = GetResource();
	if(pRes && pRes->GetType() == AirsideResource::ResType_AirRouteSegment )
	{
		AirRouteSegInSim *pSeg = (AirRouteSegInSim *) pRes;
		AirRoutePointInSim * pWayPoint = NULL;
		pWayPoint = pSeg->GetFirstConcernIntersection()->getInputPoint();
		newEvent.nodeNO = pWayPoint->getID();		
	}
	if(pRes && pRes->GetType() == AirsideResource::ResType_HeadingAirRouteSegment)
	{
		HeadingAirRouteSegInSim *pSeg = (HeadingAirRouteSegInSim *) pRes;
		AirRoutePointInSim * pWayPoint = pSeg->GetFirstIntersection()->getInputPoint() ;
		newEvent.nodeNO = pWayPoint->getID();		
	}
	//if( pRes && (pRes->GetType() == AirsideResource::ResType_FinalApproachSegment) )
	//{
	//	CFinalApproachSegInSim  *pSeg = (CFinalApproachSegInSim *) pRes;
	//	AirRoutePointInSim * pWayPoint = pSeg->GetWayPoint1();
	//	newEvent.nodeNO = pWayPoint->getID();		
	//}

	if(pRes && pRes->GetType() == AirsideResource::ResType_WayPoint)
	{
		AirWayPointInSim * pWaypoint = (AirWayPointInSim*)pRes;
		newEvent.nodeNO = pWaypoint->getID();
	}

	if(pRes &&pRes->GetType() == AirsideResource::ResType_LogicRunway)
	{
		if (GetMode() == OnExitRunway)
		{
		//	newEvent.nodeNO = GetRunwayExit()->GetID();	
			newEvent.intNodeID = GetRunwayExit()->GetID();	
		}
		else if (GetMode() == OnPreTakeoff &&!IsCircuitFlight())
		{
			newEvent.nodeNO = GetAndAssignTakeoffPosition()->GetID();
		}
		else
		{
			LogicRunwayInSim  *pRunway  = (LogicRunwayInSim * ) pRes;
			newEvent.nodeNO = pRunway->GetRunwayInSim()->GetRunwayInput()->getID();
			newEvent.runwayNO = (int)pRunway->getLogicRunwayType();	
		}	
	}

	if(pRes && pRes->GetType() == AirsideResource::ResType_Stand)
	{
		StandInSim * pStand = (StandInSim * )pRes;
		newEvent.nodeNO =  pStand->GetStandInput()->getID();		
	}

	if(pRes && pRes->GetType() == AirsideResource::ResType_TaxiwayDirSeg)
	{
		TaxiwayDirectSegInSim * pSegment = (TaxiwayDirectSegInSim * )pRes;
		newEvent.intNodeID =  pSegment->GetExitNode()->GetNodeInput().GetID();		
	}
	if(pRes && pRes->GetType() == AirsideResource::ResType_StandLeadInLine)
	{
		StandInSim *pStandInSim = ((StandLeadInLineInSim *)pRes)->GetStandInSim();
		if(pStandInSim)
			newEvent.nodeNO =  pStandInSim->GetID();
	}
	if (pRes && pRes->GetType() == AirsideResource::ResType_StandLeadOutLine)
	{
		StandInSim *pStandInSim = ((StandLeadOutLineInSim *)pRes)->GetStandInSim();
		if(pStandInSim)
			newEvent.nodeNO =  pStandInSim->GetID();
	}

	newEvent.IsSpeedConstraint = IsSpeedConstrainedByOtherFlight();


	//AirsideFlightWriteLogItem *pLogItem = new AirsideFlightWriteLogItem(this);
	//pLogItem->m_logItem = newEvent;
	//pLogItem->m_fltState = GetCurState();
	//m_lstWirteLog.AddItem(pLogItem);
	//FligthLogFile::LogLog(this,newEvent);
	FligthLogFile::LogEvent(this,GetTime());

	m_pOutput->LogFlightEvent(this ,newEvent);	

}
//set 
void AirsideFlightInSim::SetArrFlightPlan( FlightRouteInSim*pRoute )
{
	delete m_pArrivalPlan; m_pArrivalPlan = pRoute;
}

void AirsideFlightInSim::SetDepFlightPlan( FlightRouteInSim*pRoute )
{
	delete m_pDeparturePlan; m_pDeparturePlan = pRoute;
}

void AirsideFlightInSim::SetInBoundRoute( TaxiRouteInSim* pRoute )
{
	//delete m_pInBoundRoute; m_pInBoundRoute = pRoute;
}

void AirsideFlightInSim::SetOutBoundRoute( TaxiRouteInSim* pRoute )
{
	if(pRoute!=m_pOutBoundRoute)
	{
		delete m_pOutBoundRoute; 
		m_pOutBoundRoute = pRoute;
	}
}

FlightRouteInSim * AirsideFlightInSim::GetSTAR()
{
	if(!m_pSTAR)
		m_pATC->AssignSTAR(this);
	return m_pSTAR;
		
}

FlightRouteInSim * AirsideFlightInSim::GetSID()
{
	if(!m_pSID)
		m_pATC->AssignSID(this);
	return m_pSID;
}

FlightRouteInSim* AirsideFlightInSim::GetLandingCircuit()
{
	if (!m_pLandingCircuitRoute)
	{
		m_pATC->AssignLandingCircuitRoute(this);
	}
	return m_pLandingCircuitRoute;
}

FlightRouteInSim* AirsideFlightInSim::GetTakeoffCircuit()
{
	if (!m_pTakeoffCircutRoute)
	{
		m_pATC->AssignTakeoffCircuitRoute(this);
	}
	return m_pTakeoffCircutRoute;
}
void AirsideFlightInSim::SetSTAR( FlightRouteInSim * pRoute )
{
	delete m_pSTAR;
	m_pSTAR = pRoute;
}

void AirsideFlightInSim::SetSID( FlightRouteInSim * pRoute )
{
	delete m_pSID ;
	m_pSID = pRoute;
}

void AirsideFlightInSim::SetCircuit(FlightRouteInSim * pRoute)
{

}

void AirsideFlightInSim::FlushLog(const ElapsedTime& endTime)
{
	PLACE_METHOD_TRACK_STRING();
	if(GetMode() != OnTerminate)
	{
		SetTime(endTime);
		AirsideFlightEventStruct newEvent;
		memset(&newEvent,0,sizeof(AirsideFlightEventStruct));

		newEvent.nodeNO = -1;
		newEvent.intNodeID = -1;
		newEvent.state = m_curFlightType;
		CPoint2008 p = GetPosition();
		newEvent.x = (float) p.getX();
		newEvent.y =(float) p.getY();
		newEvent.z = (float) m_curState.m_dAlt;
		newEvent.time = GetTime();
		//newEvent.delayTime = m_tDelay;
		//newEvent.reason = (int)m_nDelayRe;
		newEvent.mode = (int)GetMode();
		newEvent.speed = GetSpeed();	
		newEvent.IsBackUp = GetCurState().m_bPushBack ;	
		//newEvent.eventCode = bEnterRes?('e'):(' ');
		newEvent.offsetAngle = (float)m_curState.m_offsetAngle;
		newEvent.lDelayIndex = m_curState.m_nDelayId;
// 		newEvent.IsSpeedConstraint = false;
		m_curState.m_nDelayId = 0;

		AirsideResource* pRes = GetResource();
		if(pRes && pRes->GetType() == AirsideResource::ResType_AirRouteSegment )
		{
			AirRouteSegInSim *pSeg = (AirRouteSegInSim *) pRes;
			AirRoutePointInSim * pWayPoint = NULL;
			pWayPoint = pSeg->GetFirstConcernIntersection()->getInputPoint();
			newEvent.nodeNO = pWayPoint->getID();		
		}
		if(pRes && pRes->GetType() == AirsideResource::ResType_HeadingAirRouteSegment)
		{
			HeadingAirRouteSegInSim *pSeg = (HeadingAirRouteSegInSim *) pRes;
			AirRoutePointInSim * pWayPoint = pSeg->GetFirstIntersection()->getInputPoint() ;
			newEvent.nodeNO = pWayPoint->getID();		
		}
		//if( pRes && (pRes->GetType() == AirsideResource::ResType_FinalApproachSegment) )
		//{
		//	CFinalApproachSegInSim  *pSeg = (CFinalApproachSegInSim *) pRes;
		//	AirRoutePointInSim * pWayPoint = pSeg->GetWayPoint1();
		//	newEvent.nodeNO = pWayPoint->getID();		
		//}

		if(pRes && pRes->GetType() == AirsideResource::ResType_WayPoint)
		{
			AirWayPointInSim * pWaypoint = (AirWayPointInSim*)pRes;
			newEvent.nodeNO = pWaypoint->getID();
		}

		if(pRes &&pRes->GetType() == AirsideResource::ResType_LogicRunway)
		{
			if (GetMode() == OnExitRunway)
			{
				newEvent.nodeNO = GetRunwayExit()->GetID();	
			}
			else if (GetMode() == OnPreTakeoff)
			{
				newEvent.nodeNO = GetAndAssignTakeoffPosition()->GetID();
			}
			else
			{
				LogicRunwayInSim  *pRunway  = (LogicRunwayInSim * ) pRes;
				newEvent.nodeNO = pRunway->GetRunwayInSim()->GetRunwayInput()->getID();
				newEvent.runwayNO = (int)pRunway->getLogicRunwayType();		
			}
		}

		if(pRes && pRes->GetType() == AirsideResource::ResType_Stand)
		{
			StandInSim * pStand = (StandInSim * )pRes;
			newEvent.nodeNO =  pStand->GetStandInput()->getID();		
		}

		if(pRes && pRes->GetType() == AirsideResource::ResType_TaxiwayDirSeg)
		{
			TaxiwayDirectSegInSim * pSegment = (TaxiwayDirectSegInSim * )pRes;
			newEvent.intNodeID =  pSegment->GetExitNode()->GetNodeInput().GetID();		
		}
		if(pRes && pRes->GetType() == AirsideResource::ResType_StandLeadInLine)
		{
			StandInSim *pStandInSim = ((StandLeadInLineInSim *)pRes)->GetStandInSim();
			if(pStandInSim)
				newEvent.nodeNO =  pStandInSim->GetID();
		}
		if (pRes && pRes->GetType() == AirsideResource::ResType_StandLeadOutLine)
		{
			StandInSim *pStandInSim = ((StandLeadOutLineInSim *)pRes)->GetStandInSim();
			if(pStandInSim)
				newEvent.nodeNO =  pStandInSim->GetID();
		}

		newEvent.IsSpeedConstraint = IsSpeedConstrainedByOtherFlight();

		/*AirsideFlightWriteLogItem *pLogItem = new AirsideFlightWriteLogItem(this);
		pLogItem->m_logItem = newEvent;
		pLogItem->m_fltState = GetCurState();

		m_lstWirteLog.AddItem(pLogItem);*/
		m_pOutput->LogFlightEvent(this ,newEvent);	
	}

	FlushLogToFile();
}

TaxiRouteInSim* AirsideFlightInSim::GetRouteToStand()
{
	if (!m_pRouteToStand)
		m_pATC->AssignRouteToStand(this);
	return m_pRouteToStand;

}

TaxiRouteInSim* AirsideFlightInSim::GetCircuitTaxiwayRoute()
{
	if (!m_pCircuitBoundRoute)
	{
		m_pATC->GetCircuitRoute((AirsideCircuitFlightInSim*)this);
	}

	return m_pCircuitBoundRoute;
}

TaxiRouteInSim* AirsideFlightInSim::GetRouteToTempParking()
{
	if (!m_pRouteToTempParking)
		m_pATC->AssignRouteToTemporaryParking(this);
	return m_pRouteToTempParking;
}

TaxiRouteInSim* AirsideFlightInSim::GetRouteToMeetingPoint()
{
	return m_pRouteToMeetingPoint;
}


void AirsideFlightInSim::ApplyForVehicleService()
{
	PLACE_METHOD_TRACK_STRING();
	if(GetOperationParkingStand() == NULL)
		return;

	if (m_pServiceRequestDispatcher->IsVehicleService() == false)
		return;

	if (m_bApplyFroServiceRequest == false)
	{
		m_pServiceRequestDispatcher->AddServiceRequest(this);
		m_bApplyFroServiceRequest = true;
	}
	m_pServiceRequestDispatcher->ServiceRequestDispatch();
}

void AirsideFlightInSim::ApplyForTowTruckService(StandInSim* pCurStand,const ElapsedTime& tTime)
{
	PLACE_METHOD_TRACK_STRING();
	if (pCurStand == NULL || !m_pServiceRequestDispatcher->IsVehicleService())
		return;

	if (m_pServiceRequestDispatcher->HasTowTruckServiceFlight(this))
	{
		//TOWOPERATIONTYPE Optype = m_pATC->GetPushAndTowOperationSpec()->GetStandOperationType(pCurStand,this);
		//if ( Optype== TOWOPERATIONTYPE_NOOPERATION)		//needn't tow truck
		//	return;

		//TowTruckServiceRequest* pRequest = new TowTruckServiceRequest(Optype, this,tTime);
		TowTruckServiceRequest* pRequest =m_pATC->GetPushAndTowOperationSpec()->GetFlightTowingServiceRequest(this,pCurStand);
		if (pRequest == NULL /*&& pRequest->m_ServiceType != TOWOPERATIONTYPE_PUSHBACKONLY*/)
			return;

		pRequest->m_tApplyTime = tTime;

		m_bTowTruckServiced =false;
		m_pTowingServiceVehicle = NULL;

		m_pServiceRequestDispatcher->AddTowingServiceRequest(pRequest);
		m_pServiceRequestDispatcher->TowTruckServiceRequestDispatch();
	}
}

void AirsideFlightInSim::ApplyForFollowMeCarService(StandInSim* pDest)
{
	if (m_pServiceFollowMeCar == NULL)
	{
		m_pServiceFollowMeCar = m_pServiceRequestDispatcher->AssignFollowMeCarForFlight(this,pDest);
	}
}

void AirsideFlightInSim::SetTowingRoute(TaxiRouteInSim* pTowingRoute)
{
	m_pTowingRoute = pTowingRoute;
}

void AirsideFlightInSim::SetBeginTowingService()
{
	m_bTowingToReleasePoint = true;
}

void AirsideFlightInSim::ApplyForPaxBusService(bool bArrival)
{
	PLACE_METHOD_TRACK_STRING();
	if(GetOperationParkingStand() == NULL)
		return;


	if (m_pServiceRequestDispatcher->IsVehicleService() == false)
		return;
	if(HasBrrdgeServer(bArrival)) // if has bridge , do not need bus server.
		return ;

	if (!m_pServiceRequestDispatcher->HasVehicleServiceFlight(this, VehicleType_PaxTruck))
		return;

	if (m_bApplyFrPaxBusServiceRequestArrival == false && bArrival)
	{
		m_pServiceRequestDispatcher->AddPaxBusServiceRequest(this,bArrival);
		m_bApplyFrPaxBusServiceRequestArrival = true;
	}

	if (m_bApplyFrPaxBusServiceRequestDeparture == false && bArrival== false)
	{
		m_pServiceRequestDispatcher->AddPaxBusServiceRequest(this,bArrival);
		m_bApplyFrPaxBusServiceRequestDeparture = true;
	}
	m_pServiceRequestDispatcher->PaxBusServiceRequestDispatch();
}

void AirsideFlightInSim::ApplyForBaggageTrainService( FlightOperation enumOperation )
{
	//check the conditions

// 	ASSERT(0);
 	//parking stand
 	if(GetOperationParkingStand() == NULL)
 		return;
 	//vehicle service switch
 	if (m_pServiceRequestDispatcher->IsVehicleService() == false)
 		return;
 	
 	//service requirement and pool settings
 	if (!m_pServiceRequestDispatcher->HasVehicleServiceFlight(this, VehicleType_BaggageTug))
 		return;
 
 	//Pusher or Loader
	if (m_bApplyTrainServiceRequestArrival == false && enumOperation == ARRIVAL_OPERATION)
	{
		m_bApplyTrainServiceRequestArrival = true;
		m_pServiceRequestDispatcher->AddBaggageTrainServiceRequest(this, enumOperation);
	}

 	if (m_bApplyTrainServiceRequestDeparture == false && enumOperation == DEPARTURE_OPERATION)
 	{
		m_bApplyTrainServiceRequestDeparture = true;
		m_pServiceRequestDispatcher->AddBaggageTrainServiceRequest(this, enumOperation);
 	}

 	m_pServiceRequestDispatcher->BaggageTrainServiceRequestDispatch();


}

AirsidePaxBusParkSpotInSim * AirsideFlightInSim::GetPaxBusParking(bool bArrival)
{
	if(bArrival)
		return m_pArrivalPaxParkingInSim;
	else 
		return m_pDepPaxParkingInSim;
}


ElapsedTime AirsideFlightInSim::GetEstimateStandTime()  //this function is no correct , need to refactor it later
{
	PLACE_METHOD_TRACK_STRING();
	ElapsedTime tTime = GetTime();
	m_pRouteToStand = GetRouteToStand();
	double dSpeed = GetPerformance()->getTaxiInNormalSpeed(this);
	if(GetMode()==OnHeldAtStand)
		return tTime;

	if (m_pRouteToStand)
	{
		int nCount = m_pRouteToStand->GetItemCount();
		double dLength = 0.0;
		for (int i =0; i < nCount; i++)
		{
			dLength += m_pRouteToStand->GetItem(i).m_distTo - m_pRouteToStand->GetItem(i).m_distFrom;
		}
		tTime += ElapsedTime(dLength/dSpeed) ;
	}
	else
	{
		tTime += ElapsedTime(300L);
	}

	return tTime;
}

bool AirsideFlightInSim::IsGoingToIntersectLane( LaneFltGroundRouteIntersectionInSim& laneIntersect )
{
	PLACE_METHOD_TRACK_STRING();
	TaxiRouteInSim * pTaxiRoute = GetCurrentTaxiRoute();
	if(pTaxiRoute)
	{
		FlightGroundRouteSegInSim * pTaxiSeg = laneIntersect.GetFltRouteSegment();
		if(pTaxiSeg)
		{
			
			AirsideResource * pRes = GetPreState().m_pResource;
			DistanceUnit dist = GetPreState().m_dist;

			{//check positive seg
				FlightGroundRouteDirectSegInSim * pDirSeg = pTaxiSeg->GetPositiveDirSeg();
				int idxAt = pTaxiRoute->GetItemIndex( pRes );
				int idxIntersect = pTaxiRoute->GetItemIndex(pDirSeg);
				if( idxAt >=0 )
				{
					if(idxAt < idxIntersect)
						return true;
					if(idxAt == idxIntersect )
					{
						if( laneIntersect.GetIntersectDistAtTaxiwaySeg() >= dist - 5000) 
						{
							return true;
						}
					}
				}
			}
			{//check negative seg
				FlightGroundRouteDirectSegInSim * pDirSeg2 = pTaxiSeg->GetNegativeDirSeg();
				int idxAt = pTaxiRoute->GetItemIndex( pRes );
				int idxIntersect = pTaxiRoute->GetItemIndex(pDirSeg2);
				if( idxAt >=0 )
				{
					if(idxAt < idxIntersect) return true;
					if( idxAt == idxIntersect )
					{
						if( pDirSeg2->GetPath().GetTotalLength() - laneIntersect.GetIntersectDistAtTaxiwaySeg() >= dist - 5000  )
						{
							return true;
						}
					}
				}
			}
			
			
		}
	}


	return false;
}

TaxiRouteInSim * AirsideFlightInSim::GetCurrentTaxiRoute()
{
	PLACE_METHOD_TRACK_STRING();
	if(GetMode() == OnTaxiToStand)
	{
		return GetRouteToStand();
	}
	if(GetMode() == OnTaxiToRunway)
	{
		return GetAndAssignOutBoundRoute();
	}
	if(GetMode() == OnTaxiToTempParking)
	{
		return GetRouteToTempParking();
	}
	if (GetMode() == OnTaxiToMeetingPoint)
	{
		return GetRouteToMeetingPoint();
	}
	return NULL;
}
bool AirsideFlightInSim::IsAllVehicleServiced()
{
	PLACE_METHOD_TRACK_STRING();
	if(!IsAllVehicleServiceExceptTowingCompleted())
		return false;

	return IsTowingServiceCompleted();
} 

void AirsideFlightInSim::AllVehicleFinishedServicing( const ElapsedTime& time )
{
	PLACE_METHOD_TRACK_STRING();
	if(GetMode() == OnHeldAtStand)
	{
		if(IsGeneralVehicleAndPaxbusServiceCompleted())
		{
			FlightGetClearanceEvent * newEvent = new FlightGetClearanceEvent(this);
			newEvent->setTime( max(time,GetTime()) );
			newEvent->addEvent();
		}
	}

}
int AirsideFlightInSim::fits(const FlightConstraint& _fltcons) const
{		
	return _fltcons.fits( m_LogEntry.GetAirsideDesc(), m_curFlightType );	
}


const DistanceUnit AirsideFlightInSim::defaultLength = 3640;
const DistanceUnit AirsideFlightInSim::defaultWingSpan = 2500;
const DistanceUnit AirsideFlightInSim::defaultWeight = 250;
const DistanceUnit AirsideFlightInSim::defaultHeight = 1200;
const DistanceUnit AirsideFlightInSim::defaultCabinWidth = 500;

void AirsideFlightInSim::InitFlightInfo(CAirportDatabase *pAirportDatabase)
{
	PLACE_METHOD_TRACK_STRING();
	ASSERT(pAirportDatabase != NULL);
	CACTypesManager *pManager = pAirportDatabase->getAcMan();
	if (pManager)
	{
		CString strACType;
		getACType(strACType.GetBuffer(AC_TYPE_LEN));
		strACType.ReleaseBuffer();
		CACType *pACType = pManager->getACTypeItem(strACType);
		if (pACType)
		{
			AirsideFlightDescStruct& descStruct = m_LogEntry.GetAirsideDesc();

			descStruct.dWingSpan = pACType->m_fSpan * SCALE_FACTOR;
			descStruct.dWeight = ARCUnit::KgsToLBS(pACType->m_fMTOW);
			descStruct.dLength = pACType->m_fLen * SCALE_FACTOR;
			descStruct.dHeight = pACType->m_fHeight * SCALE_FACTOR;
			descStruct.dCabinWidth = pACType->m_dCabinWidth*SCALE_FACTOR;
			if ( descStruct.dLength < ARCMath::EPSILON )
			{
				descStruct.dLength = defaultLength;
			}
			if( descStruct.dWingSpan < ARCMath::EPSILON )
			{
				descStruct.dWingSpan = defaultWingSpan;
			}
			if( descStruct.dHeight < ARCMath::EPSILON)
			{
				descStruct.dHeight = defaultHeight;
			}
			if(descStruct.dWeight < ARCMath::EPSILON)
			{
				descStruct.dWeight = defaultWeight;
			}
			if (descStruct.dCabinWidth < ARCMath::EPSILON)
			{
				descStruct.dCabinWidth = defaultCabinWidth;
			}
		}
	}

}

CPoint2008 AirsideFlightInSim::GetPosition( const ElapsedTime t ) const
{
	PLACE_METHOD_TRACK_STRING();
	ASSERT( t <= GetCurState().m_tTime );
	ASSERT( t >= GetPreState().m_tTime );

	if( t >= GetCurState().m_tTime )
		return GetCurState().m_pPosition;
	if( t <= GetPreState().m_tTime )
		return GetPreState().m_pPosition;

	ElapsedTime dT = GetCurState().m_tTime - GetPreState().m_tTime;
	double dRat =  (double)( t - GetPreState().m_tTime )/(double)dT ;
	double dSpd = GetCurState().m_vSpeed * (1-dRat) + GetPreState().m_vSpeed * dRat;	
	dRat = dRat * ( dSpd + GetPreState().m_vSpeed ) / ( GetPreState().m_vSpeed + GetCurState().m_vSpeed );
	return GetCurState().m_pPosition * dRat +  GetPreState().m_pPosition * (1-dRat);
}

ElapsedTime FindArrivalFlightEntryTime(AirsideFlightInSim * pFlight, const CArrivalETAOffsetList& etaoffsets,const ALTAirport& airport, const AirsideSimConfig& simconf)
{
	PLACE_METHOD_TRACK_STRING();
	ASSERT( pFlight->IsArrival());
	ElapsedTime tEntryTime =  pFlight->GetFlightInput()->getArrTime(); 
	
	tEntryTime = max( tEntryTime, ElapsedTime(0L) );
	pFlight->SetTime(tEntryTime);

	if(simconf.bOnlySimStand)
		return  tEntryTime;

	CArrivalETAOffset * pOffset = etaoffsets.FindBestMatchETAOffset( pFlight->GetLogEntry().GetAirsideDesc() );
	if(pOffset)
	{		
		CDistanceAndTimeFromARP * distAndTime = NULL;
		CPoint2008 entryPos = pFlight->GetEntrySystemPos();
		DistanceUnit distToAirport = entryPos.distance( airport.getRefPoint() );

		CDistanceAndTimeFromARP* pDistAndTime = pOffset->GetNearestDistanceItem(distToAirport);
		if(pDistAndTime)
		{
			DistanceUnit distToCircle = distToAirport - ARCUnit::ConvertLength(pDistAndTime->GetDistance(),ARCUnit::NM,ARCUnit::CM);
			double dspeed = 0.5 * ( pFlight->GetPerformance()->getMaxTerminalSpeed(pFlight) + pFlight->GetPerformance()->getMinTerminalSpeed(pFlight) );
			ElapsedTime dT = ElapsedTime(distToCircle / dspeed );
			ElapsedTime timeToStand = (ElapsedTime)(pDistAndTime->GetServiceTimeDistribution()->getRandomValue() * 60.0);
			tEntryTime = pFlight->GetFlightInput()->getArrTime() - timeToStand - dT; 
		}	
	
	}	
		
	return tEntryTime; 
}

	


void AirsideFlightInSim::InitBirth( const CArrivalETAOffsetList& etaoffsets,const AirsideSimConfig& simconf )
{
	PLACE_METHOD_TRACK_STRING();

	//m_pBaggageManager->SetFlight(this);
	{//init default
		if(!IsArrival())
			m_arrTime = max( ElapsedTime(0L), GetFlightInput()->getDepTime() -  GetFlightInput()->getServiceTime());
		else 
			m_arrTime = GetFlightInput()->getArrTime();
		
		if(IsDeparture())
			m_depTime = GetFlightInput()->getDepTime();
		else
			m_depTime = GetFlightInput()->getArrTime() +  GetFlightInput()->getServiceTime();
	}
	
	if(!simconf.bOnlySimStand)//init etaoffset
	{
		if( IsArrival() )
		{
			ALTAirport airportInfo = GetAirTrafficController()->GetAirsideResourceManager()->GetAirportResource()->GetAirportInfo();
			ElapsedTime entryTime = FindArrivalFlightEntryTime(this, etaoffsets, airportInfo, simconf);
			m_arrTime = max(entryTime, ElapsedTime(0L));
		}
		if( IsDeparture() )  // departure
		{
			m_depTime = max( GetFlightInput()->getDepTime(), ElapsedTime(0L) );	
		}
	}


}

CPoint2008 AirsideFlightInSim::GetEntrySystemPos()
{
	PLACE_METHOD_TRACK_STRING();
	if(IsArrival())
	{
		FlightRouteInSim * pFirstAirRoute = GetArrFlightPlan();

		if(! pFirstAirRoute || pFirstAirRoute->GetItemCount() <=0 )
			pFirstAirRoute = GetSTAR();

		if( pFirstAirRoute && pFirstAirRoute->GetItemCount() )
		{
			AirsideResource * pFirstRes = pFirstAirRoute->GetItem(0);
			if(pFirstRes && pFirstRes->GetType() == AirsideResource::ResType_WayPoint)
			{
				AirWayPointInSim * pWayPt = (AirWayPointInSim*)pFirstRes;
				return pWayPt->GetPosition();
			}
			if(pFirstRes && pFirstRes->GetType() == AirsideResource::ResType_AirRouteSegment )
			{
				AirRouteSegInSim * pAirSeg = (AirRouteSegInSim * ) pFirstRes;
				return pAirSeg->GetFirstConcernIntersection()->getInputPoint()->GetPosition();

			}
			if( pFirstRes && pFirstRes->GetType() == AirsideResource::ResType_HeadingAirRouteSegment )
			{
				HeadingAirRouteSegInSim * pAirSeg = (HeadingAirRouteSegInSim * ) pFirstRes;
				return pAirSeg->GetFirstIntersection()->getInputPoint()->GetPosition() ;
			}
		}
		// no enter route go the runway 
		LogicRunwayInSim * pRunway = GetLandingRunway();
		if(pRunway)
		{
			return pRunway->GetDistancePoint(0);
		}		
		
	}
	return CPoint2008(0,0,0);
}

void AirsideFlightInSim::GenerateNextEvent()
{
	PLACE_METHOD_TRACK_STRING();
	if(m_lstWirteLog.GetItemCount() > 0)
	{
		GenerateWriteLogEvent();
	}
	else
	{
		FlightGetClearanceEvent * pNextEvent = new FlightGetClearanceEvent(this);
		pNextEvent->setTime(GetTime());
		pNextEvent->addEvent();
	}

}

void AirsideFlightInSim::InitLogEntry()
{
	PLACE_METHOD_TRACK_STRING();
	//init animation log Entry;
	AirsideFlightDescStruct& fltDesc = m_LogEntry.GetAirsideDesc();	
	memset((char*)&fltDesc, 0, sizeof(AirsideFlightDescStruct));
	char strbuf[512]={'\0',};
	m_pflightInput->getArrID(strbuf); 	strcpy(fltDesc.arrID,strbuf );
	m_pflightInput->getDepID(strbuf);	strcpy(fltDesc.depID, strbuf);
	m_pflightInput->getFullID(strbuf); strcpy(fltDesc.flightID, strbuf);
	m_pflightInput->getACType(strbuf);	strcpy(fltDesc.acType, strbuf);
	m_pflightInput->getOrigin(strbuf); strcpy(fltDesc.origin,strbuf);
	m_pflightInput->getDestination(strbuf); strcpy(fltDesc.dest,strbuf);
	m_pflightInput->getAirline(strbuf);  fltDesc.airlineNO =(short) CAirlinesManager::GetCode(strbuf);
	fltDesc.userCode = (long)m_pflightInput;

	fltDesc.id = m_id;
	fltDesc.nOnboardArrID = -1;
	fltDesc.nOnboardDepID = -1;
	fltDesc._depID = m_pflightInput->getLogEntry().depID;
	fltDesc._acType = m_pflightInput->getLogEntry().acType;
	fltDesc._airline= m_pflightInput->getLogEntry().airline;
	fltDesc._arrID = m_pflightInput->getLogEntry().arrID;
	fltDesc._depStopover = m_pflightInput->getLogEntry().depStopover;
	fltDesc._arrStopover = m_pflightInput->getLogEntry().arrStopover;
	fltDesc._origin = m_pflightInput->getLogEntry().origin;
	fltDesc._destination = m_pflightInput->getLogEntry().destination;
	fltDesc.arrloadFactor = m_pflightInput->getLogEntry().arrloadFactor;
	fltDesc.deploadFactor = m_pflightInput->getLogEntry().deploadFactor;
	fltDesc.nCapacity = m_pflightInput->getLogEntry().capacity;
	strcpy(fltDesc.arrStandPlaned, m_pflightInput->getArrStand().GetIDString().Left(OBJIDLEN -1 ).GetBuffer());
	strcpy(fltDesc.intStandPlaned, m_pflightInput->getIntermediateStand().GetIDString().Left(OBJIDLEN -1).GetBuffer());
	strcpy(fltDesc.depStandPlaned, m_pflightInput->getDepStand().GetIDString().Left(OBJIDLEN -1).GetBuffer());



	//init report log entry
	m_pReportLogItem = new AirsideFlightLogItem;
	AirsideFlightLogDesc& fltLogDesc = m_pReportLogItem->mFltDesc;
	fltLogDesc.id = m_id;
	fltLogDesc.schArrTime = m_pflightInput->getArrTime();
	fltLogDesc.schDepTime = m_pflightInput->getDepTime();
	fltLogDesc.sAirline = fltDesc._airline;
	fltLogDesc.sArrID = fltDesc._arrID.GetValue();
	fltLogDesc.sDepID = fltDesc._depID.GetValue();
	fltLogDesc.sAcType = fltDesc._acType;
	fltLogDesc._arrStopover = fltDesc._arrStopover;
	fltLogDesc._depStopover = fltDesc._depStopover;
	fltLogDesc._destination = fltDesc._destination;
	fltLogDesc._origin = fltDesc._origin;
	fltLogDesc.arrloadFactor = fltDesc.arrloadFactor;
	fltLogDesc.deploadFactor = fltDesc.deploadFactor;
	fltLogDesc.nCapacity = fltDesc.nCapacity;

	fltLogDesc.actAtaTime = m_pflightInput->GetAtaTime();
	fltLogDesc.actAtdTime = m_pflightInput->GetAtdTime();

	if (!m_pflightInput->getArrStand().GetIDString().IsEmpty())
	{
		fltLogDesc.sSchedArrStand = m_pflightInput->getArrStand().GetIDString().GetBuffer();
		fltLogDesc.SchedArrOn = m_pflightInput->getArrTime();
		fltLogDesc.SchedArrOff = m_pflightInput->getArrTime() + m_pflightInput->getArrParkingTime();
	}

	if (!m_pflightInput->getIntermediateStand().GetIDString().IsEmpty())
	{
		fltLogDesc.sSchedIntStand = m_pflightInput->getIntermediateStand().GetIDString().GetBuffer();
		fltLogDesc.SchedIntOn = fltLogDesc.SchedArrOff;
		fltLogDesc.SchedIntOff = m_pflightInput->getDepTime() - m_pflightInput->getDepParkingTime();
	}

	if (!m_pflightInput->getDepStand().GetIDString().IsEmpty())
	{
		fltLogDesc.sSchedDepStand =  m_pflightInput->getDepStand().GetIDString().GetBuffer();
		fltLogDesc.SchedDepOn = m_pflightInput->getDepTime() - m_pflightInput->getDepParkingTime();
		fltLogDesc.SchedDepOff = m_pflightInput->getDepTime();
	}

}

const ALTObjectID AirsideFlightInSim::getStand() const
{
	if (m_curState.m_fltMode < OnEnterStand && IsArrival())
	{
		return m_pflightInput->getArrStand();
	}
	return m_pflightInput->getDepStand();
}

ElapsedTime AirsideFlightInSim::getServiceTime(FLIGHT_PARKINGOP_TYPE type) const
{
	PLACE_METHOD_TRACK_STRING();
	if (type == INT_PARKING)
		return m_pflightInput->getIntParkingTime();
	

	if (type == ARR_PARKING)		
		return m_pflightInput->getArrParkingTime();
	
	
	if (type == DEP_PARKING)		
		return m_pflightInput->getDepParkingTime();
	

	return (GetDepTime() - GetArrTime());

}

void AirsideFlightInSim::UpdateState( const AirsideFlightState& fltState , bool bInNode )
{
	PLACE_METHOD_TRACK_STRING();
	bool bSameResource = ( GetResource() == fltState.m_pResource );
	bool bSamePosition =  (fltState.m_pPosition.distance(GetPosition()) == 0 ) && fltState.m_fltMode == GetMode() && fltState.m_vSpeed  == GetSpeed();

	if( !bSamePosition && ignorLogCount )//write old log
	{
		WriteLog();		
	}

	m_curState = fltState;

	if(fltState.m_fltMode == OnTerminate )
	{
		WriteLog();	
		FlushLog(fltState.m_tTime);	
	}


	if(fltState.m_fltMode == OnBirth || fltState.m_fltMode == OnTerminate )
		return ;

	if( !bSamePosition || !bSameResource ) //write new log
	{
		WriteLog(bInNode);				
		ignorLogCount = 0;
	}
	else
	{		
		ignorLogCount ++;				
	}
}

bool AirsideFlightInSim::GetEstimateLandingTime(ElapsedTime& estimateTime)
{
	if (GetMode() < OnLanding) //the flight is still in the air
	{
		//get
	}

	return false;
}

void AirsideFlightInSim::SetTempParking( TempParkingInSim* pTempParking )
{
	m_pTemporaryParking = pTempParking;
}

void AirsideFlightInSim::InitStatus()
{
	m_curState.m_fltMode = OnBirth;
	if(IsArrival())
		ChangeToArrival();
	else
		ChangeToDeparture();
}

void AirsideFlightInSim::StartLogging()
{
	m_pOutput->LogFlightEntry(this , m_LogEntry.GetAirsideDesc() );
	
#if _DEBUGLOG//debug log
	FligthLogFile::InitLog(this);
#endif
}

AirsideFlightInSim& AirsideFlightInSim::ChangeToArrival()
{
	PLACE_METHOD_TRACK_STRING();
	m_curFlightType = 'A';  
	if(m_pFlightPerformanceManager)
	{
		mGroundPerform.mNoramlSpd = m_pFlightPerformanceManager->getTaxiInNormalSpeed(this);
		mGroundPerform.mMaxSpd = m_pFlightPerformanceManager->getTaxiInMaxSpeed(this);
		mGroundPerform.mAccSpd = m_pFlightPerformanceManager->getTaxiInAccelation(this);
		mGroundPerform.mDecSpd = m_pFlightPerformanceManager->getTaxiInDeceleration(this);
	}
	return *this;
}

AirsideFlightInSim& AirsideFlightInSim::ChangeToDeparture()
{
	PLACE_METHOD_TRACK_STRING();
	m_curFlightType = 'D' ;
	if(m_pFlightPerformanceManager)
	{
		mGroundPerform.mNoramlSpd = m_pFlightPerformanceManager->getTaxiOutNormalSpeed(this);
		mGroundPerform.mMaxSpd = m_pFlightPerformanceManager->getTaxiOutMaxSpeed(this);
		mGroundPerform.mAccSpd = m_pFlightPerformanceManager->getTaxiOutAccelation(this);
		mGroundPerform.mDecSpd = m_pFlightPerformanceManager->getTaxiOutDeceleration(this);
	}
	return *this;
}
void AirsideFlightInSim::AddVehicleServiceRequest(VehicleServiceRequest *pVehicleRequest)
{
	m_vServiceRequest.push_back(pVehicleRequest);
}
void AirsideFlightInSim::VehicleServiceComplete(AirsideVehicleInSim *pVehicle)
{
	PLACE_METHOD_TRACK_STRING();
	for(std::vector<VehicleServiceRequest*>::iterator itr=m_vServiceRequest.begin();itr!=m_vServiceRequest.end();++itr)
	{
		VehicleServiceRequest *pRequest = *itr;
		if(pRequest && pRequest->bHasVehicle(pVehicle))
		{
			pRequest->RemoveServiceVehicle(pVehicle);
			if(pRequest->IsCompleteService())
			{
				if(m_pServiceRequestDispatcher)
					m_pServiceRequestDispatcher->DeleteRequest(pRequest);
				m_vServiceRequest.erase(itr);
				break;
			}
		}
	}
	if(pVehicle->GetVehicleBaseType() == VehicleType_DeicingTruck)
	{
		if( IsAllVehicleServiceExceptTowingCompleted() && GetTime()<= pVehicle->GetTime())
		{
			SetTime(pVehicle->GetTime());
			GenerateNextEvent();
		}
	}
	else if(pVehicle->GetVehicleBaseType() == VehicleType_TowTruck)
	{
		if( IsAllVehicleServiced() && GetTime()<= pVehicle->GetTime())
		{
			SetTime(pVehicle->GetTime());
			GenerateNextEvent();
		}
	}
	else
	{
		if(IsGeneralVehicleAndPaxbusServiceCompleted() && GetTime()<= pVehicle->GetTime())
		{
			SetTime(pVehicle->GetTime());
			GenerateNextEvent();
		}
	}
}

bool AirsideFlightInSim::IsGeneralVehicleAndPaxbusServiceCompleted()
{
	for(size_t i =0; i<m_vServiceRequest.size(); ++i)
	{
		VehicleServiceRequest *pRequest = m_vServiceRequest[i];
		if(pRequest	 && pRequest->GetType()!=VehicleType_DeicingTruck && pRequest->IsCompleteService() == false)
			return false;
	}
	
	return true;
}

void AirsideFlightInSim::SetDelayed( AirsideFlightDelay* pDelay )
{
	PLACE_METHOD_TRACK_STRING();
	//check if the delay is same
	if( pDelay && m_pCurrentDelay )
	{
		if( pDelay->IsIdentical(m_pCurrentDelay) ) //the same delay
		{
			delete pDelay;
		}
		else //the different delay
		{
			m_pCurrentDelay->SetDelayTime(pDelay->GetStartTime() - m_pCurrentDelay->GetStartTime() );
			int nDelatyId = m_pOutput->LogFlightDelay(m_pCurrentDelay);
			m_curState.m_nDelayId = nDelatyId;
		//	WriteLog();
			m_pCurrentDelay = pDelay;			
		}
	}
	else
	{
		if( pDelay!= m_pCurrentDelay ) //different delay
		{
			if(m_pCurrentDelay) // pDelay =NULL;
			{	
				m_pCurrentDelay->SetDelayTime(GetTime() - m_pCurrentDelay->GetStartTime());
				int nDelayId = m_pOutput->LogFlightDelay(m_pCurrentDelay);
				m_curState.m_nDelayId = nDelayId;
			//	WriteLog();
				m_pCurrentDelay = NULL;
			}
			else //
			{
				m_pCurrentDelay = pDelay;
			}
		}
	}

}

CString AirsideFlightInSim::getArrGateInSim( void )
{
	if (m_strActualArrGate != _T(""))
		return m_strActualArrGate;

	return m_pflightInput->getArrGate().GetIDString(); 
}

CString AirsideFlightInSim::getDepGateInSim( void )
{
	if (m_strActualDepGate != _T(""))
		return m_strActualDepGate;

	return m_pflightInput->getDepGate().GetIDString();
}

void AirsideFlightInSim::setArrGateInSim( const CString& strID )
{
	m_strActualArrGate = strID;
}

void AirsideFlightInSim::setDepGateInSim( const CString& strID )
{
	m_strActualDepGate = strID;
}

CString AirsideFlightInSim::GetCurrentFlightID() const
{
	char strbuf[256];
	m_pflightInput->getFullID(strbuf,m_curFlightType);
	return _T(strbuf);
}

void AirsideFlightInSim::InitScheduleEndTime(const ElapsedTime& tEndTime)
{
	m_tScheduleEndTime = tEndTime;
}

ElapsedTime AirsideFlightInSim::getPlanParkingTimeInAirport() const
{
	if (IsArrival() && !IsDeparture())
		return m_tScheduleEndTime - m_arrTime;
	if (!IsArrival() && IsDeparture())
		return m_depTime;
	if (IsTransfer())
		return m_depTime - m_arrTime;

	return 0L;
}

bool AirsideFlightInSim::IsArrivingOperation()
{
	return m_curFlightType == 'A';
}

bool AirsideFlightInSim::IsDepartingOperation()
{
	return m_curFlightType == 'D';
}

bool AirsideFlightInSim::WaitingInHold(ElapsedTime tHoldTime)
{
	PLACE_METHOD_TRACK_STRING();
	if (GetResource() == NULL)
		return false;

	AirspaceHoldInSim* pHold = NULL;
	if (GetResource()->GetType() == AirsideResource::ResType_AirspaceHold)
		pHold = (AirspaceHoldInSim*)GetResource();
	if (GetResource()->GetType() == AirsideResource::ResType_AirRouteSegment)
		pHold = ((AirRouteSegInSim*)GetResource())->GetConnectedHold();
	if (GetResource()->GetType() == AirsideResource::ResType_HeadingAirRouteSegment)
		pHold = ((HeadingAirRouteSegInSim*)GetResource())->GetConnectedHold();
	if (GetResource()->GetType() == AirsideResource::ResType_WayPoint)
		pHold = ((AirWayPointInSim*)GetResource())->GetHold();

	//ASSERT(pHold); //the waypoint must connect with a hold

	if (pHold == NULL)
		return false;

	pHold->AddHoldingFlight(this);

	//ASSERT(m_id != 31);
	FlightInHoldEvent* pEvent = new FlightInHoldEvent(this, pHold, tHoldTime);
	pEvent->setTime(GetTime());
	pEvent->addEvent();	

	return true;
}

void FlightWakeUpCaller::Update( const ElapsedTime& tTime )
{
	//add flight event
	FlightGetClearanceEvent * pNextEvent = new FlightGetClearanceEvent(m_pFlight);
	pNextEvent->setTime(tTime);
	pNextEvent->addEvent();
	ClearSubjects();
}
void AirsideFlightInSim::WritePureLog( AirsideMobElementWriteLogItem* pLogItem )
{
	PLACE_METHOD_TRACK_STRING();
	AirsideFlightWriteLogItem *pFlightLogItem = (AirsideFlightWriteLogItem *)pLogItem;
	if(pFlightLogItem != NULL)
	{

		m_pOutput->LogFlightEvent(this ,pFlightLogItem->m_logItem);
		//m_preState = m_curState;
		//m_curState = pFlightLogItem->m_fltState;
		NotifyOtherAgents(SimMessage().setTime(GetTime()));//NotifyObservers(GetTime());
	
		//CString allfile = "C:\\FlightDebug\\AllFlight_Test.txt";
		//std::ofstream alloutfile;
		//alloutfile.open(allfile,ios::app);	
		//alloutfile << pFlightLogItem->m_logItem.time<<"		"
		//			<< std::endl;
		//alloutfile.close();
	}
	GenerateNextEvent();

}

void AirsideFlightInSim::GenerateWriteLogEvent()
{
	PLACE_METHOD_TRACK_STRING();
	//FlightGetClearanceEvent * pNextEvent = new FlightGetClearanceEvent(this);
	//pNextEvent->setTime(GetTime());
	//pNextEvent->addEvent();

	if(m_lstWirteLog.GetItemCount() > 0)
	{
		AirsideMobElementWriteLogItem *pLogItem = m_lstWirteLog.GetItem();
		CAirsideMobElementWriteLogEvent *pNextEvent = new CAirsideMobElementWriteLogEvent(pLogItem);
		AirsideFlightWriteLogItem *pFlightLogItem = (AirsideFlightWriteLogItem *)pLogItem;
		ElapsedTime eEventTime = GetTime();
		if(pFlightLogItem)
            eEventTime.set(pFlightLogItem->m_logItem.time/TimePrecision);

		pNextEvent->setTime(eEventTime);
		pNextEvent->addEvent();
	
	}
	else
	{
		
		GenerateNextEvent();
	}

}

CPollygon2008 AirsideFlightInSim::GetFlightPosition( const ElapsedTime& eTime )
{
	CPollygon2008 flightPos;

	return flightPos;
}

ElapsedTime AirsideFlightInSim::GetFlightEstimateTime( AirsideResource *pResource,DistanceUnit dDistance )
{
	return GetTime();
}

double AirsideFlightInSim::GetWingspan() const
{
	return m_LogEntry.GetAirsideDescStruct().dWingSpan;
}

double AirsideFlightInSim::GetWeight() const
{
	return m_LogEntry.GetAirsideDescStruct().dWeight;
}

double AirsideFlightInSim::GetLength() const
{
	if (m_bFollowMeCarServicing && m_pServiceFollowMeCar)
	{
		return 2.0* m_LogEntry.GetAirsideDescStruct().dLength;
	}

	return m_LogEntry.GetAirsideDescStruct().dLength;
}

DistanceUnit AirsideFlightInSim::GetIntersectionBuffer() const
{
	if( GetMode() != OnTaxiToRunway )
		return GetPerformance()->getTaxiInboundIntersectionBuffer((AirsideFlightInSim*)this) ;
	
	return GetPerformance()->getTaxiOutboundIntersectionBuffer((AirsideFlightInSim*)this);	
}

void AirsideFlightInSim::RunwayWakeUp( const ElapsedTime& eTime )
{
	//implement code here
}


//////////////////////////////////////////////////////////////////////////
#include ".\holdpositionInsim.h"
void AirsideFlightInSim::EnterQueueToRunwayHold( HoldPositionInSim* pQueue ,bool bMoveToHead)
{	
	PLACE_METHOD_TRACK_STRING();
	if(m_pCrossRunwayHold!=pQueue)
	{
		QuitCrossRunwayQueue();//quit current queue
		m_pCrossRunwayHold = pQueue;
	}

	if(m_pCrossRunwayHold)
	{
		m_pCrossRunwayHold->AddFlightToQueue(this,bMoveToHead);	
		////check the queue is full or not
			////if the queue cannot hold another flight, it is full
			//if(GetRunwayExit())
			//	GetRunwayExit()->CheckRunwayExitFull(this);
	}
}

void AirsideFlightInSim::QuitCrossRunwayQueue()
{
	if(m_pCrossRunwayHold)
	{
		m_pCrossRunwayHold->RemoveFlightInQueue(this);
		m_pCrossRunwayHold->WakeupEnrouteWaitingFlight(GetTime());
	}
	m_pCrossRunwayHold = NULL;
}


void AirsideFlightInSim::Terminate(const ElapsedTime& t)
{
	PLACE_METHOD_TRACK_STRING();
	if(GetMode()<= OnLanding || GetMode()>= OnTaxiToRunway)		//to release the hold lock and runway exit lock
	{
		if( GetAirTrafficController() && GetAirTrafficController()->GetAirsideResourceManager() )
		{
			if(GetAirTrafficController()->GetAirsideResourceManager()->GetAirspaceResource())
			{
				GetAirTrafficController()->GetAirsideResourceManager()->GetAirspaceResource()->ReleaseFlightHoldLock(this);
			}
		}
		if (m_pRunwayExit)
		{
			FlightGroundRouteDirectSegInSim * pDirSeg = m_pRunwayExit->GetRouteSeg();
			ReleaseResourceLockEvent * pReleseEvent = new ReleaseResourceLockEvent(this, pDirSeg);
			pReleseEvent->setTime( t );
			pReleseEvent->addEvent();		
		}
	}
	if(GetResource())
	{				
		ReleaseResourceLockEvent * pReleseEvent = new ReleaseResourceLockEvent(this, GetResource());
		pReleseEvent->setTime( t );
		pReleseEvent->addEvent();		
		GetResource()->SetExitTime(this,t);
	}
	if (GetOperationParkingStand() && GetOperationParkingStand()->GetLockedFlight() == this)
	{
		GetOperationParkingStand()->ReleaseLock(this, t);
	}

	//release all route
	if(m_pRouteInQueue)
		m_pRouteInQueue->FlightExitRoute(this, t);
	if(m_pRouteToStand)
		m_pRouteToStand->FlightExitRoute(this, t);	
	if( m_pRouteToTempParking)
		m_pRouteToTempParking->FlightExitRoute(this, t);
	if(m_pTowingRoute)
		m_pTowingRoute->FlightExitRoute(this, t);


	if(m_pARCPortEngine && m_pARCPortEngine->GetOnboardSimulation())
	{
		bool bArrival = true;
		if(m_curFlightType == 'D')
			bArrival = false;

		m_pARCPortEngine->GetOnboardSimulation()->FlightTerminated(m_pflightInput,bArrival,t);

	}

	
}

void AirsideFlightInSim::ApplyForDeiceSerive(const ElapsedTime& tTime)
{
	PLACE_METHOD_TRACK_STRING();
	if( getDeiceServiceRequest() )
		return;

	FlightDeiceRequirment deiceRequirment;
	GetAirTrafficController()->UpdateFlightDeiceRequirment(this, deiceRequirment);
	if(deiceRequirment.bNeedDeice())
	{
		m_pServiceRequestDispatcher->AddDeiceServiceRequest(this, deiceRequirment);
		m_pServiceRequestDispatcher->ServiceRequestDispatch();

		/*if (GetResource()->GetType() == AirsideResource::ResType_Stand )
		{
			ApplyForTowTruckService((StandInSim*)GetResource(),tTime);
		}
		else if (GetResource()->GetType() == AirsideResource::ResType_StandLeadInLine)
		{
			ApplyForTowTruckService(((StandLeadInLineInSim*)GetResource())->GetStandInSim(),tTime);
		}*/
	}

}

VehicleServiceRequest* AirsideFlightInSim::getServiceRequest( AirsideVehicleInSim* pVehicle )
{
	for(int i=0;i<(int)m_vServiceRequest.size();i++)
	{
		VehicleServiceRequest* pRequest = m_vServiceRequest.at(i);
		if(pRequest->bHasVehicle(pVehicle))
			return pRequest;
	}
	return NULL;
}

VehicleServiceRequest* AirsideFlightInSim::getGeneralServiceRequest( int nVelTypeID )
{
	for(int i=0;i<(int)m_vServiceRequest.size();i++)
	{
		VehicleServiceRequest* pRequest = m_vServiceRequest.at(i);
		if( pRequest->GetServiceVehicleTypeID() == nVelTypeID )
			return pRequest;
	}
	return NULL;
}

void AirsideFlightInSim::RemoveServiceReques( VehicleServiceRequest* pRequest )
{
	std::vector<VehicleServiceRequest*>::iterator itrFind = std::find(m_vServiceRequest.begin(),m_vServiceRequest.end(),pRequest);
	if(itrFind!=m_vServiceRequest.end())
	{
		m_vServiceRequest.erase(itrFind);
		if(m_pServiceRequestDispatcher)
			m_pServiceRequestDispatcher->DeleteRequest(pRequest);
		//delete *itrFind;
	}
}

DeiceVehicleServiceRequest* AirsideFlightInSim::getDeiceServiceRequest()
{
	for(int i=0;i<(int)m_vServiceRequest.size();i++)
	{
		VehicleServiceRequest* pRequest = m_vServiceRequest.at(i);
		if( pRequest->GetType()==VehicleType_DeicingTruck )
			return (DeiceVehicleServiceRequest*)pRequest;
	}
	return NULL;
}

bool AirsideFlightInSim::IsAllVehicleServiceExceptTowingCompleted()
{
	for(size_t i =0; i<m_vServiceRequest.size(); ++i)
	{
		VehicleServiceRequest *pRequest = m_vServiceRequest[i];
		if(DeiceVehicleServiceRequest* deiceRequest = pRequest->toDeiceRequest())
		{
			if(GetDeiceDecision().m_pDeicepadGroup)
			{
				continue;
			}
		}
		if(pRequest	 && pRequest->IsCompleteService() == false)
			return false;
	}

	return true;
}

bool AirsideFlightInSim::IsTowingServiceCompleted()
{
	if (m_pTowingServiceVehicle == NULL&& !m_pServiceRequestDispatcher->UnDispatchTowTruckForFlight(this))		//needn't towing operation
		return true;

	if (m_bTowTruckServiced && m_pTowingServiceVehicle)		//towing operation finished and has get service vehicle
		return true;

	return false;
}

bool AirsideFlightInSim::IsWaitingForTowTruck()
{
	if (m_bTowTruckServiced)		//service finished
		return false;

	if (m_pServiceRequestDispatcher == NULL)
		return false;
	
	if ( m_pTowingServiceVehicle&& !m_pTowingServiceVehicle->IsReadyToService(this))
		return true;		//the tow truck not arrived at stand

	if (m_pTowingServiceVehicle == NULL && m_pServiceRequestDispatcher->UnDispatchTowTruckForFlight(this))
		return true;

	return false;
}

void AirsideFlightInSim::SetTowingServiceCompleted()
{
	m_bTowTruckServiced = true;
	m_bTowingToReleasePoint = false;
}

void AirsideFlightInSim::SetTowingServiceVehicle(AirsideTowTruckInSim* pTowTruck)
{
	m_pTowingServiceVehicle = pTowTruck;
}

bool AirsideFlightInSim::IsActive() const
{
	return GetMode()!=OnTerminate;
}



void AirsideFlightInSim::OnNotify( SimAgent* pAgent, SimMessage& msg )
{
	PLACE_METHOD_TRACK_STRING();
	FlightGetClearanceEvent * pNextEvent = new FlightGetClearanceEvent(this);
	pNextEvent->setTime(msg.getTime());
	pNextEvent->addEvent();
	RemoveDepend();
}

DistanceUnit AirsideFlightInSim::getCurSeparation() const
{
	if(GetMode()<OnHeldAtStand)
		return m_pFlightPerformanceManager->getTaxiInboundSeparationDistInQ((AirsideFlightInSim*)this);
	else 
		return m_pFlightPerformanceManager->getTaxiOutboundSeparationDistInQ((AirsideFlightInSim*)this);
	return 0;
}

RouteDirPath* AirsideFlightInSim::getCurDirPath() const
{
	return GetCurTaxiwayDirectSeg();
}

TaxiwayDirectSegInSim* AirsideFlightInSim::GetCurTaxiwayDirectSeg() const
{
	AirsideResource* pCurRes = GetResource();
	if(pCurRes && pCurRes->GetType()== AirsideResource::ResType_TaxiwayDirSeg)
	{
		return (TaxiwayDirectSegInSim*)pCurRes;
	}
	return NULL;
}

void AirsideFlightInSim::LogRunwayOperationLog( bool bEntryOrExit, ClearanceItem& nextClearanceItem )
{
	PLACE_METHOD_TRACK_STRING();
	//get runway information
	LogicRunwayInSim *pLogicRunway = NULL;
	if(GetMode() < OnTaxiToStand)// landing
		pLogicRunway = GetLandingRunway();
	else if(GetMode() >= OnTaxiToRunway)//take off
		pLogicRunway = GetAndAssignTakeoffRunway();

	if(pLogicRunway  == NULL)
		return;

	//runway information
	AirsideRunwayOperationLog::RunwayLogItem* pRunwayLogItem = new AirsideRunwayOperationLog::RunwayLogItem();
	int nRunwayID = (pLogicRunway->GetRunwayInSim() == NULL )? -1: pLogicRunway->GetRunwayInSim()->GetRunwayID();
	RUNWAY_MARK runwayMark = pLogicRunway->getLogicRunwayType();
	CString strMarkName = pLogicRunway->GetMarkName();

	pRunwayLogItem->SetRunway(nRunwayID,runwayMark,strMarkName);

	pRunwayLogItem->SetEnumEntryOrExit((AirsideRunwayOperationLog::enumEntryOrExit)(bEntryOrExit?0:1));

	//operation
	//write miss approach
	if(nextClearanceItem.GetMode() == OnMissApproach)
	{
		pRunwayLogItem->SetEnumOperation(AirsideRunwayOperationLog::enumOperation_MissApproach);
		if(bEntryOrExit)//entry
		{
			pRunwayLogItem->SetTime(GetTime());
		}
		else
		{
			pRunwayLogItem->SetTime(nextClearanceItem.GetTime());
		}
	}
	else if(GetMode() < OnTaxiToStand)//landing
	{
		pRunwayLogItem->SetEnumOperation(AirsideRunwayOperationLog::enumOperation_Landing);
		//time
		pRunwayLogItem->SetTime(nextClearanceItem.GetTime());

		if(bEntryOrExit)//entry, need to calculate the minimum occupancy time
		{
			std::vector<RunwayExitInSim *> vMaxBreakingRunwayExit;

			if(m_pATC != NULL && m_pATC->GetRunwayExitAssignmentStrategiesInSim() != NULL)
			{
				vMaxBreakingRunwayExit = m_pATC->GetRunwayExitAssignmentStrategiesInSim()->GetMaxBrakingFirstExitOnEitherSide(this,pLogicRunway);
			}
			ASSERT(vMaxBreakingRunwayExit.size() > 0);
			
			RunwayExitInSim *pMBRunwayExit = NULL;//max breaking exit
			std::vector<RunwayExitInSim *>::iterator iterExit = vMaxBreakingRunwayExit.begin();	
			for (; iterExit != vMaxBreakingRunwayExit.end(); ++iterExit)
			{
				if((*iterExit) == NULL || GetRunwayExit() == NULL)
					continue;
				if((*iterExit)->GetExitInput().GetID() != GetRunwayExit()->GetExitInput().GetID() && 
					(int)((*iterExit)->GetExitRunwayPos()) < int(GetRunwayExit()->GetExitRunwayPos()))
				{
					pMBRunwayExit = *iterExit;
					break;
				}
			}
			if(pMBRunwayExit == NULL)
				pMBRunwayExit = GetRunwayExit();

			//need to calculate the time
			if(pMBRunwayExit)
			{
				//get first exit
				double dMinTime = 0.0;
				if(GetPerformance())
				{
					DOUBLE dTouchDist = GetTouchDownDistAtRuwnay(pLogicRunway);//GetPerformance()->getDistThresToTouch(this);
					double dExitDistance = pMBRunwayExit->GetExitRunwayPos();

					double dAngle = pMBRunwayExit->GetAngle();
					double dExitSpeed = 0;
					//get exit speed according to the exits' angel
					if (dAngle >90.0 && dAngle < 180.0 )
					{
						dExitSpeed = GetPerformance()->getMaxExitSpeedAcute(this);
					}
					else if(dAngle == 90.0)
					{
						dExitSpeed = GetPerformance()->getMaxExitSpeed90deg(this);
					}
					else if (dAngle < 90.0 && dAngle > 0.0)
					{
						dExitSpeed = GetPerformance()->getMaxExitSpeedHigh(this);
					}
					double landingSpeed = GetPerformance()->getLandingSpeed(this);

					double dAverageSpeed = (dExitSpeed + landingSpeed)/2;

					dMinTime = (dExitDistance - dTouchDist)/dAverageSpeed;
				}
				pRunwayLogItem->SetPossibleExitInformation(pMBRunwayExit->GetID(), pMBRunwayExit->GetExitInput().GetName(),ElapsedTime(dMinTime));
			}
		}

		//airroute
		if(GetSTAR()&& GetSTAR()->GetAirRouteInSim())
		{
			//air route id
			AirRouteInSim *pAirRoute = GetSTAR()->GetAirRouteInSim();							
			int nRouteID = pAirRoute->GetID();
						
			//air route name
			CString nRouteName = _T("");
			if(pAirRoute->GetAirRouteInput())
				nRouteName = pAirRoute->GetAirRouteInput()->getName();
			
			AirsideRunwayOperationLog::RunwayLogAirRoute logAirRoute;
			logAirRoute.m_nRouteID = nRouteID;
			logAirRoute.m_strRouteName = nRouteName;

			pRunwayLogItem->AddAirRoute(logAirRoute);

		}

	}
	else if(GetMode() >= OnTaxiToRunway)//take off
	{
		pRunwayLogItem->SetEnumOperation(AirsideRunwayOperationLog::enumOperation_Takeoff);
		//time
		pRunwayLogItem->SetTime(nextClearanceItem.GetTime());

		//airroute
		if(GetSID()&& GetSID()->GetAirRouteInSim())
		{
			//air route id
			AirRouteInSim *pAirRoute = GetSID()->GetAirRouteInSim();							
			int nRouteID = pAirRoute->GetID();

			//air route name
			CString nRouteName = _T("");
			if(pAirRoute->GetAirRouteInput())
				nRouteName = pAirRoute->GetAirRouteInput()->getName();

			AirsideRunwayOperationLog::RunwayLogAirRoute logAirRoute;
			logAirRoute.m_nRouteID = nRouteID;
			logAirRoute.m_strRouteName = nRouteName;

			pRunwayLogItem->AddAirRoute(logAirRoute);

		}
	}
	else
	{
		delete pRunwayLogItem;
		pRunwayLogItem = NULL;
		return;
	}
	//flight information
	ASSERT(GetFlightInput() != NULL);
	if(GetFlightInput())
	{
		pRunwayLogItem->SetFlightIndex(m_id);
		CString strFullID;
		GetFlightInput()->getFullID(strFullID.GetBuffer(1024));
		strFullID.ReleaseBuffer();
		pRunwayLogItem->SetFlightID(strFullID);

		pRunwayLogItem->SetFlightType(GetFlightInput()->getLogEntry().acType.GetValue());

	}
	m_pOutput->LogRunwayOperationDelay(pRunwayLogItem);
}

void AirsideFlightInSim::FlushLogToFile()
{
	if(m_pOutput)
		m_pOutput->m_reportLog.AddFlightLogItem(*m_pReportLogItem);
}

bool AirsideFlightInSim::StartDelay( ClearanceItem& startItem, ARCMobileElement* pConflictMobile,FlightConflict::ActionType action,FlightConflict::LocationType conflictLocation,FltDelayReason emFltDelayReason,const CString& sDelayReason )
{
	PLACE_METHOD_TRACK_STRING();
	if(m_id == 10)
	{
		TRACE(sDelayReason);
	}
	ARCMobileDesc mobileDesc; 
	if(pConflictMobile)
	{
		pConflictMobile->getDesc(mobileDesc);
	}
	
	if(mpCurrentDelayLog)
	{		
		if(mpCurrentDelayLog->bTheSameDelay(mobileDesc,action,conflictLocation))
		{
			return false;
		}
		else
		{
			EndDelay(startItem);		//finish fore delay
		}
	}
	//make new delay log
	AirsideConflictionDelayLog* pDelayLog = new AirsideConflictionDelayLog();
	pDelayLog->mAction = action;
	pDelayLog->mConflictLocation = conflictLocation;
	AirsideResource* pRes = startItem.GetResource();
	if(pRes)
		pRes->getDesc(pDelayLog->mAtResource);
	pDelayLog->mdSpd = startItem.GetSpeed();
	pDelayLog->mMode = startItem.GetMode();
	pDelayLog->time = (long)startItem.GetTime();
	pDelayLog->motherMobileDesc = mobileDesc;
	pDelayLog->distInRes = startItem.GetDistInResource();
	pDelayLog->m_sDetailReason = sDelayReason;
	pDelayLog->m_emFlightDelayReason = emFltDelayReason;

	if (IsArrivingOperation())
		pDelayLog->state = AirsideFlightEventLog::ARR;
	else
		pDelayLog->state = AirsideFlightEventLog::DEP;
	if(GetMode() > OnExitRunway && GetMode() < OnTakeoff)		//ground part
	{
		AirsideReportingAreaInSim* pArea = m_pATC->GetAirsideResourceManager()->GetInReportingArea(GetPosition());
		if (pArea)
		{
			pDelayLog->mAreaID = pArea->GetAreaID();
			pDelayLog->sAreaName = pArea->GetAreaInput()->GetObjNameString().GetString();
		}
	}
	else						//airspace part
	{
		CString strName = m_pATC->GetAirsideResourceManager()->GetInSectorNameAndID(GetPosition(),m_curState.m_dAlt, pDelayLog->mAreaID);
		pDelayLog->sAreaName = strName.GetString();
	}


	
	mpCurrentDelayLog = pDelayLog;
	return true;
}

void AirsideFlightInSim::EndDelay( ClearanceItem& endItem )
{
	//calculate the delay time
	if(mpCurrentDelayLog)
	{
		long delayT = (long)endItem.GetTime()  - mpCurrentDelayLog->time;
		mpCurrentDelayLog->mDelayTime = delayT;
		LogEventItem(mpCurrentDelayLog);
		mpCurrentDelayLog = NULL;
	}
}

void AirsideFlightInSim::LogEventItem( ARCEventLog* pLog )
{
	if(pLog && m_pReportLogItem)
		m_pReportLogItem->AddEvent(pLog);
}

const AirsideConflictionDelayLog* AirsideFlightInSim::GetCurDelayLog() const
{
	return mpCurrentDelayLog;
}

void AirsideFlightInSim::getDesc( ARCMobileDesc& desc ) const
{
	desc.mType = ARCMobileDesc::FLIGHT;
	desc.nMobileID = m_id;
	char strName[1024];
	if (m_curFlightType == 'A')
		getFullID(strName, 'A');
	else
		getFullID(strName, 'D');
	desc.strName = strName;
}

double AirsideFlightInSim::GetExitSpeed()
{
	PLACE_METHOD_TRACK_STRING();
	double dExitSpeed = 1000;

	if (GetRunwayExit()->GetAngle() < 90.0)
	{
		dExitSpeed = GetPerformance()->getMaxExitSpeedHigh(this);
	}
	if (GetRunwayExit()->GetAngle() == 90.0)
	{
		dExitSpeed = GetPerformance()->getMaxExitSpeed90deg(this);
	}
	if (GetRunwayExit()->GetAngle() > 90.0)
	{
		dExitSpeed = GetPerformance()->getMaxExitSpeedAcute(this);
	}

	return dExitSpeed;
}

CTowOffStandAssignmentData* AirsideFlightInSim::GetTowOffData()
{
	return m_pTowoffData;
}

void AirsideFlightInSim::SetTowOffData( CTowOffStandAssignmentData* pData )
{
	m_pTowoffData = pData;
}

FLIGHTTOWCRITERIATYPE AirsideFlightInSim::GetTowOffStandType()
{
	PLACE_METHOD_TRACK_STRING();

	if(m_pTowoffData == NULL)
		return NOTTOW;

	if (!IsArrival() && m_pTowoffData != NULL && !m_pflightInput->getIntermediateStand().IsBlank())	//only departure
		return FREEUPSTAND;

	if (!IsDeparture() && m_pTowoffData != NULL
		&& m_pTowoffData->IsTow() && 
		!m_pflightInput->getIntermediateStand().IsBlank())	//only arrival
		return FREEUPSTAND;

	if (!IsDeparture() && !m_pTowoffData->IsTow())		// only arrival
		return NOTTOW;

	if (m_pTowoffData == NULL && getArrStand() == getDepStand())
		return NOTTOW;
	else
	{
		if (!m_pTowoffData->IsTow() || (m_pTowoffData->IsTow() && m_pTowoffData->GetRepositionOrFreeup()))		//not tow
		{
			if ( getArrStand() == getDepStand() )
				return NOTTOW;
		}
		else
		{
			if (GetDepTime() - GetArrTime() >= ElapsedTime(m_pTowoffData->GetTowOffCriteria()*60L))
				return FREEUPSTAND;

			if (m_pTowoffData->GetTowOffCriteria() >= 2*(m_pTowoffData->GetDeplanementTime() + m_pTowoffData->GetEnplanementTime())
				&& (GetDepTime() - GetArrTime()) >= ElapsedTime(m_pTowoffData->GetTowOffCriteria()*60L - 600L))	// 10 mins difference consider
				return FREEUPSTAND;
		}

	}

	return REPOSITIONFORDEP;
}




//bool AirsideFlightInSim::GetOpenDoorAndStairGroundPostions(std::vector< std::pair<CPoint2008, CPoint2008> >& vPoints )
//{
//	PLACE_METHOD_TRACK_STRING();
//	StandInSim* pStandInSim = GetOperationParkingStand();
//	if (pStandInSim == NULL)
//		return false;
//
//	bool bMinus = true;		
//	if (m_curState.m_fltMode == OnHeldAtStand && m_curState.m_pResource->GetType() == AirsideResource::ResType_StandLeadInLine)
//		bMinus = false;
//	else if(m_curState.m_fltMode==OnHeldAtStand && m_curState.m_pResource->GetType() == AirsideResource::ResType_StandLeadOutLine)
//		bMinus = true;
//	else
//	{
//		return false;
//	}
//
//	double dStartDist, dEndDist;  
//	CPath2008 path;
//	CPoint2008 prePoint;
//	if (bMinus)//lead out line
//	{
//		dStartDist = m_curState.m_dist - GetLength()/2.0;
//		dEndDist = m_curState.m_dist + GetLength()/2.0;
//		StandLeadOutLineInSim* pLeadOutLine = (StandLeadOutLineInSim*)(m_curState.m_pResource);
//		if (pLeadOutLine==NULL)
//			return false;
//		path = pLeadOutLine->GetPath();
//	}
//	else // lead in line
//	{
//		dStartDist = GetLength()/2.0 + m_curState.m_dist;
//		dEndDist = m_curState.m_dist - GetLength()/2.0;
//		StandLeadInLineInSim* pLeadInLine = (StandLeadInLineInSim*)(m_curState.m_pResource);
//		if (pLeadInLine==NULL)
//			return false;
//		path = pLeadInLine->GetPath();
//	}
//
//	//get onboard layout door information 
//	if(simEngineConfig()->isSimOnBoardMode())
//	{
//		std::vector<CPoint2008>vDoorPosition;
//		CPoint2008 flightPos = m_curState.m_pPosition;
//		AirsideFlightDescStruct fltDest = m_LogEntry.GetAirsideDesc();
//		bool bArrival = (m_curFlightType == 'A'?true:false);
//		OnboardFlightInSim* pOnboardFlightInSim = m_pARCPortEngine->GetOnboardSimulation()->GetOnboardFlightInSim(this,bArrival);
//		if (pOnboardFlightInSim)
//		{
//			ARCVector2 vDir(path.GetDistDir(m_curState.m_dist));
//			CLine2008 line(m_curState.m_pResource->GetDistancePoint(dStartDist),m_curState.m_pResource->GetDistancePoint(dEndDist));
//			if(pOnboardFlightInSim->SetOnboardDoorConnect(vDir,flightPos,line))
//				return true;
//		}
//	}
//
//
//	FltOperatingDoorSpecInSim* pFltDoorSpec =  GetAirTrafficController()->GetFltOperatingDoorSpec();
//	if (pFltDoorSpec)
//	{
//		std::vector<ACTypeDoorOpen> vOpenDoors = pFltDoorSpec->getFlightOpenDoors();
//		int nDoor = vOpenDoors.size();
//		for (int i =0; i <nDoor; i++)
//		{
//		
//			ACTypeDoor* pACDoor = vOpenDoors.at(i);
//			CPoint2008 doorpoint, groundpoint; 
//			ARCVector3 point;
//			point = flightbody.m_centerPath.getDistancePoint(pACDoor->m_dNoseDist*100);
//			double dStairProjectLength = pACDoor->m_dSillHeight*100;
//			CPoint2008 doorCenterPoint(point[VX],point[VY],point[VZ]);
//
//			if (pACDoor->m_enumDoorDir == ACTypeDoor::RightHand)
//				continue;
//			
//			point = flightbody.m_sidePath2.getDistancePoint(pACDoor->m_dNoseDist*100);
//			
//			doorpoint = CPoint2008(point[VX],point[VY],point[VZ]);
//			groundpoint = doorpoint - doorCenterPoint;
//			groundpoint.Normalize();
//			groundpoint.length(dStairProjectLength + GetCabinWidth()/2.0) ;
//			groundpoint += doorCenterPoint;
//
//
//			doorpoint.setZ((pACDoor->m_dHeight + pACDoor->m_dSillHeight)*100);
//			std::pair<CPoint2008,CPoint2008> pos(doorpoint,groundpoint);
//
//			vPoints.push_back(pos);
//			
//		}
//	}
//	return true;
//}

void AirsideFlightInSim::AddPaxCount( int nCount )
{
	m_nPaxCountInFlight += nCount;
}

int AirsideFlightInSim::GetPaxCount()
{
	return m_nPaxCountInFlight;
}

void AirsideFlightInSim::DecreasePaxNumber(const ElapsedTime& tTime)
{
	m_nPaxCountInFlight--;

	if (m_nPaxCountInFlight ==0)
	{
		//close the doors
		if(m_bStairShowState)
		{
			WriteStairsLog(tTime);
			m_bStairShowState = false;
		}
		
		cpputil::autoPtrReset(m_pOpenDoors);//.clear();//?

		//if find flight wait for passenger take on and the last passenger take on need wakeup flight
		if(m_bWaitPaxTakeOn)
		{
			WakeUp(tTime);
		}
	}
}

void AirsideFlightInSim::SetShowStair(const ElapsedTime& tTime)
{
	if (!m_bStairShowState)
	{
		m_bStairShowState = true;

		OpenDoors(tTime);
		//WriteStairsLog(true, tTime);
	}
}

void AirsideFlightInSim::WriteStairsLog(const ElapsedTime& tTime)
{

	//std::pair<CPoint2008, CPoint2008> stairpos;
	ASSERT(m_pOpenDoors);
	if(!m_pOpenDoors)
		return;

	int nCount = m_pOpenDoors->getCount();//.size();
	for (int i = 0; i < nCount; i++)
	{
		COpenDoorInfo& openDoor = m_pOpenDoors->getDoor(i);//.at(i);
		AirsideFlightStairsLog* pLog = new AirsideFlightStairsLog;
		pLog->m_tStartTime = openDoor.mOpenTime;
		pLog->m_tEndTime = tTime;
		pLog->m_nStairId = i;
		pLog->m_nFlightId = GetUID();
		pLog->m_cFltStatus = m_curFlightType;
		pLog->m_DoorPos = openDoor.mDoorPos;
		pLog->m_GroundPos = openDoor.mGroundPos;		
		m_pOutput->LogFlightStair(pLog);
	}
}

void AirsideFlightInSim::AddPaxInFlight()
{
	m_nPaxCountInFlight++;
}

double AirsideFlightInSim::GetCabinWidth() const
{
	return m_LogEntry.GetAirsideDescStruct().dCabinWidth;
}

AirsideFollowMeCarInSim* AirsideFlightInSim::GetServiceFollowMeCar()
{
	return m_pServiceFollowMeCar;
}

CFlightOpenDoors* AirsideFlightInSim::OpenDoors(const ElapsedTime&  tTime)
{
	PLACE_METHOD_TRACK_STRING();
	CPoint2008 pos;
	ARCVector3 dir;
	if(!GetPosAtStand(pos,dir))
		return NULL;

	DistanceUnit fltLen = GetLength();
	CPath2008 flightPath;
	flightPath.push_back(pos + dir.SetLength(fltLen/2) );
	flightPath.push_back(pos - dir.SetLength(fltLen/2) );
	
	//get onboard layout door information 
	if(simEngineConfig()->isSimOnBoardMode())
	{
		std::vector<CPoint2008>vDoorPosition;
		CPoint2008 flightPos = m_curState.m_pPosition;
		AirsideFlightDescStruct fltDest = m_LogEntry.GetAirsideDesc();
		bool bArrival = (m_curFlightType == 'A'?true:false);
		OnboardFlightInSim* pOnboardFlightInSim = m_pARCPortEngine->GetOnboardSimulation()->GetOnboardFlightInSim(this,bArrival);
		if (pOnboardFlightInSim)
		{			
			CLine2008 line(flightPath[0],flightPath[1]);
			if(pOnboardFlightInSim->SetOnboardDoorConnect(ARCVector2(dir.x,dir.y),flightPos,line))
			{
				return m_pOpenDoors;
			}
		}
	}


	if(m_pOpenDoors)
		return m_pOpenDoors;

	CFlightOpenDoors* openDoors = new CFlightOpenDoors();
	cpputil::autoPtrReset(m_pOpenDoors, openDoors);


	FltOperatingDoorSpecInSim* pFltDoorSpec =  GetAirTrafficController()->GetFltOperatingDoorSpec();
	if (!pFltDoorSpec)
		return m_pOpenDoors;

	//getd door infos
	ARCVector3 cabinLoffset = dir.PerpendicularLCopy().SetLength( GetCabinWidth()*0.5 );

	std::vector<ACTypeDoorOpen> vDoorList = pFltDoorSpec->getFlightOpenDoors(GetFlightInput()->getType(m_curFlightType),GetOperationParkingStand()->GetStandInput()->GetObjectName() );	
	for(size_t i=0;i<vDoorList.size();i++)
	{
		const ACTypeDoorOpen& door = vDoorList.at(i);
		ACTypeDoor::DoorDir openSide = door.openSide;

		ACTypeDoor* pACDoor = door.pDoor;

		CPoint2008 doorCenterPos = flightPath.GetDistPoint(pACDoor->m_dNoseDist*100);
		DistanceUnit doorHeight = (pACDoor->m_dHeight + pACDoor->m_dSillHeight)*100;
		ARCVector3 groundLoffset =  cabinLoffset.SetLength(  doorHeight );

		if(  openSide == ACTypeDoor::RightHand )
		{		
			CPoint2008 doorPos = doorCenterPos - cabinLoffset;
			doorPos.setZ(doorHeight);
			COpenDoorInfo doorInfo;
			doorInfo.m_doorSide = ACTypeDoor::RightHand;
			doorInfo.m_sideIndex = door.index;
			doorInfo.mDoorPos= doorPos;
			doorInfo.mGroundPos= doorPos - groundLoffset;
			doorInfo.mGroundPos.setZ(0);
			doorInfo.mOpenTime = tTime;
			m_pOpenDoors->add(doorInfo);			
		}
		if(  openSide == ACTypeDoor::LeftHand)
		{		
			CPoint2008 doorPos = doorCenterPos + cabinLoffset;
			doorPos.setZ(doorHeight);
			COpenDoorInfo doorInfo;
			doorInfo.m_doorSide = ACTypeDoor::LeftHand;
			doorInfo.m_sideIndex = door.index;
			doorInfo.mDoorPos= doorPos;
			doorInfo.mGroundPos= doorPos + groundLoffset;
			doorInfo.mGroundPos.setZ(0);
			doorInfo.mOpenTime = tTime;
			m_pOpenDoors->add(doorInfo);			
		}	
	}
	return m_pOpenDoors;
}

void AirsideFlightInSim::ConnectBridge(const ElapsedTime&  tTime)
{
	PLACE_METHOD_TRACK_STRING();
	//check the flight has bus service or not
	Terminal* pTerminal =  m_pflightInput->GetTerminal();
	if(pTerminal )
	{
		//get stand
		if(GetOperationParkingStand())//m_pAirsideFlt->GetResource() && m_pAirsideFlt->GetResource()->GetType() == AirsideResource::ResType_StandLeadInLine)
		{
			//StandLeadInLineInSim *pStandLeadInSim = (StandLeadInLineInSim *)m_pAirsideFlt->GetResource();
			//ASSERT(pStandLeadInSim);
			StandInSim* pStandInSim = GetOperationParkingStand();
			//if(pStandInSim)
			{
				ALTObjectID standName;
				pStandInSim->GetStandInput()->getObjName(standName);

				ProcessorList *pProcList =  pTerminal->GetTerminalProcessorList();
				std::vector<BaseProcessor*> vBridgeProcessor;
				if(pProcList)
					pProcList->GetProcessorsByType(vBridgeProcessor,BridgeConnectorProc);

				std::vector<BaseProcessor *>::iterator iterBridge = vBridgeProcessor.begin();
				for (; iterBridge != vBridgeProcessor.end(); ++ iterBridge)
				{
					BridgeConnector *pBridgeConnector = (BridgeConnector *)*iterBridge;
					if(pBridgeConnector && 
						!pBridgeConnector->IsBridgeConnectToFlight(GetFlightInput()->getFlightIndex()) &&
						pBridgeConnector->IsBridgeConnectToStand(standName))
					{
						pBridgeConnector->ConnectFlight(this,tTime);					
					}
				}
			}
		}
	}
}
BOOL AirsideFlightInSim::HasBrrdgeServer(BOOL _arrival)
{
	PLACE_METHOD_TRACK_STRING();
	ProcessorID proID ;
	if(_arrival)
		proID = GetFlightInput()->getArrGate() ;
	else
		proID = GetFlightInput()->getDepGate() ;
	Terminal* pTerminal =  m_pflightInput->GetTerminal();
	if(pTerminal == NULL)
		return FALSE;
	CString promes = proID.GetIDString() ;
	Processor*  GatePro = pTerminal->procList->getProcessor(proID) ;
	if(GatePro == NULL)
		return FALSE ;

	GateProcessor* pGateProc = (GateProcessor*)GatePro;

    return pGateProc->getBridgeConnector(GetFlightInput()->getFlightIndex()) != NULL ;
}

bool AirsideFlightInSim::NeedGenerateBoardingCall(const ElapsedTime& eTime)
{
	int nLastStage = 1;
	std::map<int,ElapsedTime>&mapLastCalls = m_pflightInput->GetLastCallOfEveryStage();
	if( mapLastCalls.find( nLastStage ) != mapLastCalls.end() )//find
	{
		//if the last call of m_pPerson stage have occured, then person should not wait here
		if( mapLastCalls[ nLastStage ] <= eTime )
		{
			return true;
		}
	}

	return false;
}

void AirsideFlightInSim::GenerateBoardingCall()
{
	if (m_pflightInput == NULL)
		return;
	CMobileElemConstraint paxType(m_pflightInput->GetTerminal());
	(FlightConstraint &)paxType = m_pflightInput->getType ('D');
	BoardingCallEvent* event = new BoardingCallEvent;
	event->setTime (GetTime());
	event->init (1.0f, NULL, paxType,1 );
	event->addEvent();
}

void AirsideFlightInSim::SetParkingStand( StandInSim * pStand ,FLIGHT_PARKINGOP_TYPE type)
{
	
	if (type == ARR_PARKING)
	{
		m_pArrParkingStand = pStand;
		
		
		if (pStand &&  pStand->GetStandInput()  ) //changed the stand 
		{
			bool needAssignArrGate = (pStand != GetPlanedParkingStand(ARR_PARKING)) || m_pflightInput->getArrGate().isBlank(); //stand reallocated or orign is blank;
			ALTObjectID standID = pStand->GetStandInput()->GetObjectName();
			
			if(m_pATC && m_pATC->GetStandToGateConstraint() && needAssignArrGate )
			{
				ProcessorID idGate = m_pATC->GetStandToGateConstraint()->GetArrGateIDInSim(GetFlightInput(), standID);
				setArrGateInSim(idGate.GetIDString());
			}
		}		
	}

	if (type == DEP_PARKING)
	{
		m_pDepParkingStand = pStand;

		if (pStand &&  pStand->GetStandInput())
		{
			bool needAssignDepGate = (pStand != GetPlanedParkingStand(DEP_PARKING)) || m_pflightInput->getDepGate().isBlank(); //stand reallocated or orign is blank;

			ALTObjectID standID = pStand->GetStandInput()->GetObjectName();
			if(m_pATC && m_pATC->GetStandToGateConstraint() && needAssignDepGate)
			{
				

				setDepGateInSim(m_pATC->GetStandToGateConstraint()->GetDepGateIDInSim(GetFlightInput(), standID).GetIDString());
			}

			if (NeedGenerateBoardingCall(GetTime()))
			{
				GenerateBoardingCall();
			}
		}

	}
}

DistanceUnit AirsideFlightInSim::GetTouchDownDistAtRuwnay( LogicRunwayInSim* pRunway )
{
	DistanceUnit distRet = GetPerformance()->getDistThresToTouch(this);
	if(pRunway)
		distRet += pRunway->GetLandingThreshold()  ;
	return distRet;
}

void AirsideFlightInSim::SetBeginFollowMeCarService()
{
	m_bFollowMeCarServicing = true;
}

void AirsideFlightInSim::SetFollowMeCarServiceCompleted()
{
	m_bFollowMeCarServicing = false;
	m_pServiceFollowMeCar = NULL;
}

TaxiRouteInSim* AirsideFlightInSim::GetRouteToAbandonPoint()
{
	PLACE_METHOD_TRACK_STRING();
	if (m_pRouteToAbandonPoint == NULL)
	{
		IntersectionNodeInSim* pDestNode = NULL;
		StandInSim* pDestStand = NULL;

		if (GetTemporaryParking())
			pDestStand = ((TempParkingStandInSim*)GetTemporaryParking())->GetStand();
		else
			pDestStand =  GetOperationParkingStand();

		AirportResourceManager* pAirportRes = m_pATC->GetAirsideResourceManager()->GetAirportResource();
		CAbandonmentPoint* pAbandonData = m_pServiceFollowMeCar->GetAbandonData();

		bool bAfterNode = false;
		double dDistToNode = 0;
		if (pAbandonData->m_PointType == CAbandonmentPoint::TY_Intersection)
		{
		  //  pDestNode = pAirportRes->GetIntersectionNodeList().GetNodeByID(pAbandonData->m_ID);
			pDestNode = pAirportRes->GetIntersectionNodeList().GetNodeByName(pAbandonData->m_Name);
			m_pServiceFollowMeCar->SetDestNode(pDestNode);
			if (pAbandonData->m_SelType >0)
				bAfterNode = true;
			dDistToNode = pAbandonData->m_Dis;
		}
		else
		{
			if (pAbandonData->m_Name.CompareNoCase("All") ==0)
			{
				pDestNode = pDestStand->GetMinDistInNode();
				m_pServiceFollowMeCar->SetDestNode(pDestStand);
			}
			else
			{
				ALTObjectID objName(pAbandonData->m_Name);
				if (pDestStand->GetStandInput()->GetObjectName().idFits(objName))
				{
					pDestNode = pDestStand->GetMinDistInNode();
					m_pServiceFollowMeCar->SetDestNode(pDestStand);
				}
				else
				{
					std::vector<StandInSim*> vStands;
					pAirportRes->getStandResource()->GetStandListByStandFamilyName(objName, vStands);
					ASSERT(!vStands.empty());
					pDestNode = vStands.at(0)->GetMinDistInNode();
					m_pServiceFollowMeCar->SetDestNode(vStands.at(0));
				}
			}
		}
		//FlightGroundRouteDirectSegList Taxisegs;
		//double dPathWeight = (std::numeric_limits<double>::max)();
		//pAirportRes->getGroundRouteResourceManager()->GetRoute((FlightGroundRouteDirectSegInSim*)GetResource(),pDestNode,this, Taxisegs, dPathWeight);
		//Taxisegs.insert(Taxisegs.begin(),(FlightGroundRouteDirectSegInSim*)GetResource());
		//
		//if(GetMode() == OnTaxiToStand)
		//{
		//	FlightGroundRouteDirectSegList deSegList;
		//	m_pRouteToStand->GetOverlapRoute(Taxisegs,deSegList);

		//	m_pRouteToAbandonPoint = new TaxiRouteInSim(GetMode(),GetResource(),pDestStand);
		//	m_pRouteToAbandonPoint->AddTaxiwaySegList(deSegList,m_pARCPortEngine->GetAirsideSimulation()->AllowCyclicGroundRoute());
		//}

		//if (GetMode() == OnTaxiToTempParking)
		//{
		//	FlightGroundRouteDirectSegList deSegList;
		//	m_pRouteToTempParking->GetOverlapRoute(Taxisegs,deSegList);
		//	m_pRouteToAbandonPoint = new TaxiRouteInSim(GetMode(),GetResource(),pDestStand);
		//	m_pRouteToAbandonPoint->AddTaxiwaySegList(deSegList,m_pARCPortEngine->GetAirsideSimulation()->AllowCyclicGroundRoute());
		//}
	//	m_pRouteToAbandonPoint->AddTaxiwaySegList(deSegList,m_pARCPortEngine->GetAirsideSimulation()->AllowCyclicGroundRoute());

		//if (GetMode() == OnTaxiToStand)
		//{
		//	if (pDestNode != pDestStand->GetMinDistInNode())
		//	{
		//		m_pRouteToStand->Clear();
		//		Taxisegs.clear();
		//		pAirportRes->getGroundRouteResourceManager()->GetRoute(pDestNode,pDestStand->GetMinDistInNode(),this, 0,Taxisegs, dPathWeight);
		//		m_pRouteToStand->AddTaxiwaySegList(Taxisegs,m_pARCPortEngine->GetAirsideSimulation()->AllowCyclicGroundRoute());
		//	}
		//	else
		//	{

		//	}
		//}

		//if (GetMode() == OnTaxiToTempParking)
		//{
		//	if (pDestNode != pDestStand->GetMinDistInNode())
		//	{
		//		m_pRouteToTempParking->Clear();
		//		Taxisegs.clear();
		//		pAirportRes->getGroundRouteResourceManager()->GetRoute(pDestNode,pDestStand->GetMinDistInNode(),this, 0,Taxisegs,dPathWeight);
		//		m_pRouteToTempParking->AddTaxiwaySegList(Taxisegs,m_pARCPortEngine->GetAirsideSimulation()->AllowCyclicGroundRoute());
		//	}
		//	else
		//	{

		//	}
		//}
		m_pRouteToAbandonPoint = new TaxiRouteInSim(GetMode(),GetResource(),pDestStand);
		const AirsideMeetingPointInSim* pMeetingPoint = m_pServiceFollowMeCar->GetMeetingPoint();
		FlightGroundRouteDirectSegList Taxisegs;
		if (GetMode() == OnTaxiToStand)
		{
			if (m_pRouteToStand->NodeInRoute(pDestNode) == false)
			{
				m_pServiceFollowMeCar->SetDestNode(pDestStand);
				m_pRouteToStand->GetSubRouteToStand(pMeetingPoint,Taxisegs);
				Taxisegs.insert(Taxisegs.begin(),(FlightGroundRouteDirectSegInSim*)GetResource());
				m_pRouteToAbandonPoint->AddTaxiwaySegList(Taxisegs,m_pARCPortEngine->GetAirsideSimulation()->AllowCyclicGroundRoute());
			}
			else
			{
				m_pRouteToStand->GetSubRouteToAboundont(pMeetingPoint,pDestNode,Taxisegs);
				Taxisegs.insert(Taxisegs.begin(),(FlightGroundRouteDirectSegInSim*)GetResource());
				m_pRouteToAbandonPoint->AddTaxiwaySegList(Taxisegs,m_pARCPortEngine->GetAirsideSimulation()->AllowCyclicGroundRoute());
			}
		}

		if (GetMode() == OnTaxiToTempParking)
		{
			if (m_pRouteToTempParking->NodeInRoute(pDestNode) == false)
			{
				m_pServiceFollowMeCar->SetDestNode(pDestStand);
				m_pRouteToTempParking->GetSubRouteToStand(pMeetingPoint,Taxisegs);
				Taxisegs.insert(Taxisegs.begin(),(FlightGroundRouteDirectSegInSim*)GetResource());
				m_pRouteToAbandonPoint->AddTaxiwaySegList(Taxisegs,m_pARCPortEngine->GetAirsideSimulation()->AllowCyclicGroundRoute());
			}
			else
			{
				m_pRouteToTempParking->GetSubRouteToAboundont(pMeetingPoint,pDestNode,Taxisegs);
				Taxisegs.insert(Taxisegs.begin(),(FlightGroundRouteDirectSegInSim*)GetResource());
				m_pRouteToAbandonPoint->AddTaxiwaySegList(Taxisegs,m_pARCPortEngine->GetAirsideSimulation()->AllowCyclicGroundRoute());
			}
		}

	}
	return m_pRouteToAbandonPoint;
}

void AirsideFlightInSim::SetTakeoffPosition( RunwayExitInSim * pExit )
{
	if(m_pTakeoffPos)
	{
		m_pTakeoffPos->UnRegTakeoffPos(this);
	}
	m_pTakeoffPos = pExit;
	if(m_pTakeoffPos)
		m_pTakeoffPos->RegTakeoffPos(this);
}

void AirsideFlightInSim::SetRunwayExit( RunwayExitInSim* pExit )
{
	m_pRunwayExit = pExit;
}

double AirsideFlightInSim::GetSpeedByMode( AirsideMobileElementMode mobileMode ) const
{
	PLACE_METHOD_TRACK_STRING();
	if(OnTowToDestination == mobileMode)
	{
		return GetPerformance()->getTowingSpeed(this);
	}
	if( OnTaxiToRunway == mobileMode || OnTaxiToDeice == mobileMode )
	{
		return GetPerformance()->getTaxiOutNormalSpeed(this);
	}	
	return GetPerformance()->getTaxiInNormalSpeed(this);
}

bool AirsideFlightInSim::IsSpeedConstrainedByOtherFlight()
{
	PLACE_METHOD_TRACK_STRING();
	//------------------------------------------------------------
	// to check whether the flight's speed is delayed or constrained by others
	// 1. constrained
	if ( GetAirTrafficController()->getConflictGraph()->GetLeadMobile(this))
	{
		TaxiRouteInSim* pCurTaxiRoute = GetCurrentTaxiRoute();
		if (pCurTaxiRoute && pCurTaxiRoute->GetFlightOnTaxiwaySegSpeed(this)>mGroundPerform.getNormalSpd())
		{
			return true;
		}
	}
	// 2. Deceleration
	if (m_curState.m_dAcc<0.0)
	{
		return true;
	}
	// 3. delayed
	// 	if ( m_pCurrentDelay && AirsideFlightDelay::enumTaxiLeadFlightDelay == m_pCurrentDelay->GetType() )
	// 	{
	// 		bIsSpeedConstraint = true;
	// 	}
	//------------------------------------------------------------

	return false;
}

bool AirsideFlightInSim::GetTaxiwayConstrainedSpeed(double& dSpeed) const
{
	AirsideResource* pRes = GetResource();
	if (pRes && pRes->GetType() == AirsideResource::ResType_TaxiwayDirSeg)
	{
		TaxiwayDirectSegInSim * pDirSeg = (TaxiwayDirectSegInSim*)pRes;
		return pDirSeg->GetTaxiwaySeg()->GetTaxiwayConstrainedSpeed(this, dSpeed);
	}
	return false;
}

bool AirsideFlightInSim::IsOnSamePreferredRoute( const AirsideFlightInSim* pOtherFlight, CRoutePriority* pExtRoutePriority/* = NULL*/ ) const
{
	if (NULL == pExtRoutePriority)
	{
		pExtRoutePriority = m_pPreferredRoutePriority;
	}
// 	return m_pPreferredRoutePriority && pOtherFlight->m_pPreferredRoutePriority
// 		&& m_pPreferredRoutePriority->getID() == pOtherFlight->m_pPreferredRoutePriority->getID();
	return pExtRoutePriority && pExtRoutePriority == pOtherFlight->m_pPreferredRoutePriority;
}

std::vector<AirsideFlightInSim* > AirsideFlightInSim::FindFlightsOnDiffPreferredRoute( const std::vector<AirsideFlightInSim* >& vecFlights, CRoutePriority* pExtRoutePriority/* = NULL*/ ) const
{
	PLACE_METHOD_TRACK_STRING();
	if (NULL == pExtRoutePriority)
	{
		pExtRoutePriority = m_pPreferredRoutePriority;
	}
	std::vector<AirsideFlightInSim* > vecFlightsOnDiffRoute;
	std::vector<AirsideFlightInSim* >::const_iterator ite = vecFlights.begin();
	std::vector<AirsideFlightInSim* >::const_iterator iteEn = vecFlights.end();
	for (;ite!=iteEn;ite++)
	{
		if (this != *ite && !IsOnSamePreferredRoute(*ite, pExtRoutePriority))
		{
			vecFlightsOnDiffRoute.push_back(*ite);
		}
	}
	return vecFlightsOnDiffRoute;
}

bool AirsideFlightInSim::GetStoppedStartTime( ElapsedTime& stoppedTime ) const
{
	const AirsideConflictionDelayLog* pDelayLog = GetCurDelayLog();
	if (pDelayLog && pDelayLog->mAction == FlightConflict::STOPPED)
	{
		stoppedTime = pDelayLog->time;
		return true;
	}
	return false;
}


// call this method when flight is about to enter runway
CPath2008 AirsideFlightInSim::GetFilletPathFromTaxiwayToRwyport( RUNWAY_MARK  port )
{
	PLACE_METHOD_TRACK_STRING();
	ASSERT(m_pTakeoffPos);
	TaxiwayDirectSegInSim * pDirSeg = (TaxiwayDirectSegInSim*)GetResource();
	CPath2008 fullPath;
	if (pDirSeg->GetOppositeSegment() == m_pTakeoffPos->GetRouteSeg())
	{
		VERIFY(m_pTakeoffPos->getPath(fullPath, port, GetDistInResource()));
	}
	else if (pDirSeg == m_pTakeoffPos->GetRouteSeg()) // should not as exit segment
	{
		ASSERT(FALSE);
	}
	else
	{
		CPath2008 nextPath;
		if (m_pTakeoffPos->getPath(nextPath, port))
		{
			CPath2008 forePath = pDirSeg->GetPath().GetLeftPath(GetDistInResource());
			std::vector<CPoint2008> vPts;
			vPts.reserve(forePath.getCount()+nextPath.getCount());

			vPts.insert(vPts.end(),&forePath[0], &forePath[0]+forePath.getCount());
			vPts.insert(vPts.end(),&nextPath[0], &nextPath[0]+nextPath.getCount());
			fullPath.init(vPts.size(), &vPts[0]);
		}
	}
	return fullPath;

// 	TaxiwayDirectSegInSim * pDirSeg = (TaxiwayDirectSegInSim * )GetResource();
// 	IntersectionNodeInSim* pExitNode = pDirSeg->GetExitNode();			
// 	RunwayDirectSegInSim* pDestRunwaySeg = NULL;
// 	CPath2008 filetPath;
// 
// 	do 
// 	{
// 		if(nextItem.GetResource()->GetType() == AirsideResource::ResType_LogicRunway)
// 		{
// 			pDestRunwaySeg = (RunwayDirectSegInSim*)nextItem.GetResource();
// 			FilletTaxiway* pFiletTaxiway = pExitNode->GetFilletFromTo(pDirSeg, pDestRunwaySeg);
// 			//has fillet
// 			if(pFiletTaxiway != NULL)
// 			{
// 				CPath2008 thePath = pExitNode->GetFiletRoutePath( *pFiletTaxiway, pDirSeg->GetTaxiwaySeg()->GetTaxiwayID());
// 				std::vector<CPoint2008> vPts;
// 				vPts.reserve(thePath.getCount()+1);
// 				vPts.push_back(GetPosition());
// 				vPts.insert(vPts.end(),&thePath[0], &thePath[0]+thePath.getCount());
// 				vPts.push_back(nextItem.GetPosition());
// 				filetPath.init(vPts.size(), &vPts[0]);
// 				break;
// 			}
// 
// 		}
// 		if (m_pTakeoffPos)
// 		{
// 			m_pTakeoffPos->getPath();
// 		}
// 
// 		//does not has fillet between taxiway and taxiway or taxiway to stand etc.
// 		std::vector<CPoint2008> _inputPts;
// 		std::vector<CPoint2008> _outputPts;
// 		int nOutCnt = 10;
// 		_inputPts.push_back(GetPosition());
// 		if (m_pTakeoffPos && m_pTakeoffPos->GetRouteSeg() != pDirSeg)
// 		{
// 			_inputPts.push_back(m_pTakeoffPos->getPos());
// 		}
// 		else
// 		{
// 			_inputPts.push_back(pDirSeg->GetExitNode()->GetNodeInput().GetPosition() );
// 		}
// 		_inputPts.push_back(nextItem.GetPosition());
// 
// 		BizierCurve::GenCurvePoints(_inputPts, _outputPts,nOutCnt);
// 		CPoint2008 ptList[1024];
// 		for(size_t i=0; i<_outputPts.size(); i++)
// 		{
// 			ptList[i].setX(_outputPts.at(i).getX());
// 			ptList[i].setY(_outputPts.at(i).getY());
// 			ptList[i].setZ(0.0);
// 		}
// 		filetPath.init ( (int)_outputPts.size(), ptList);
// 
// 	} while(FALSE);
// 
// 	return filetPath;
}

void AirsideFlightInSim::WakeupAllPaxBusCompleteService()
{
	PLACE_METHOD_TRACK_STRING();
	for(size_t i =0; i<m_vServiceRequest.size(); ++i)
	{
		VehicleServiceRequest *pRequest = m_vServiceRequest[i];
		if(pRequest	 && pRequest->GetType()==VehicleType_PaxTruck)
			pRequest->WakeupAllPaxBusCompleteService(this);
	}
}

void AirsideFlightInSim::GenerateArrivalCloseDoorEvent()
{
	if (m_pARCPortEngine == NULL)
		return;
	
	if(!m_pARCPortEngine->IsOnboardSel())
		return;

	OnboardSimulation* pOnboardSim = m_pARCPortEngine->GetOnboardSimulation();
	if (pOnboardSim == NULL)
		return;
	
	OnboardFlightInSim* pOnboardFlightInSim = pOnboardSim->GetOnboardFlightInSim(this,true);
	if (pOnboardFlightInSim == NULL)
		return;

	pOnboardFlightInSim->GenerateCloseDoorEvent(m_arrTime);
}

void AirsideFlightInSim::GenerateDepartureCloseDoorEvent()
{
	if (m_pARCPortEngine == NULL)
		return;

	if(!m_pARCPortEngine->IsOnboardSel())
		return;

	OnboardSimulation* pOnboardSim = m_pARCPortEngine->GetOnboardSimulation();
	if (pOnboardSim == NULL)
		return;

	OnboardFlightInSim* pOnboardFlightInSim = pOnboardSim->GetOnboardFlightInSim(this,false);
	if (pOnboardFlightInSim == NULL)
		return;
	
	pOnboardFlightInSim->GenerateCloseDoorEvent(m_depTime);
}

//------------------------------------------------------------------------------------------
//Summary:
//		retrieve onboard flight
//------------------------------------------------------------------------------------------
OnboardFlightInSim* AirsideFlightInSim::GetOnboardFlight()
{
	if (m_pARCPortEngine == NULL)
		return NULL;

	if(!m_pARCPortEngine->IsOnboardSel())
		return NULL;

	OnboardSimulation* pOnboardSim = m_pARCPortEngine->GetOnboardSimulation();
	if (pOnboardSim == NULL)
		return NULL;

	bool bArrival = IsArrivingOperation() ? true : false;

	OnboardFlightInSim* pOnboardFlightInSim = pOnboardSim->GetOnboardFlightInSim(this,bArrival);

	return pOnboardFlightInSim;
}

BOOL AirsideFlightInSim::hasPaxBusService(char mode) const
{
	PLACE_METHOD_TRACK_STRING();
	for(size_t i =0; i<m_vServiceRequest.size(); ++i)
	{
		VehicleServiceRequest *pRequest = m_vServiceRequest[i];
		if(pRequest	 && pRequest->GetType()==VehicleType_PaxTruck)
		{
			CPaxBusServiceRequest* pPaxBusRequest = (CPaxBusServiceRequest*)pRequest;
			char requestMode = pPaxBusRequest->IsArrival()?'A':'D';
			if(requestMode == mode )
				return TRUE;
		}
	}
	return FALSE;
}

//check pax bus for departure service
bool AirsideFlightInSim::DeparturePaxBusService()
{
	if(GetDepParkingStand() == NULL)
		return true;

	if (m_pServiceRequestDispatcher->IsVehicleService() == false)
		return false;

	if (!m_pServiceRequestDispatcher->AvailablePaxServiceFlight(this,'D'))
		return false;

	if (m_bApplyFrPaxBusServiceRequestDeparture)
	{
		return hasPaxBusService('D')?true:false;
	}
	return true;
}

bool AirsideFlightInSim::ArrivalPaxBusService()
{
	if(GetArrParkingStand() == NULL)
		return true;

	if (m_pServiceRequestDispatcher->IsVehicleService() == false)
		return false;

	if (!m_pServiceRequestDispatcher->AvailablePaxServiceFlight(this,'A'))
		return false;

	if (m_bApplyFrPaxBusServiceRequestArrival)
	{
		return hasPaxBusService('A')?true:false;
	}
	return true;
}

bool AirsideFlightInSim::NeedMoveToIntStand()const
{
	return m_bTowingToIntStand;
}

void AirsideFlightInSim::ApplyTowingToIntStand()
{
	m_bTowingToIntStand = true;
}

bool AirsideFlightInSim::NeedTowingToDepStand()const
{
	return m_bTowingToDepStand;
}

void AirsideFlightInSim::ApplyTowingToDepStand()
{
	m_bTowingToDepStand = true;
}

bool AirsideFlightInSim::HasTowingToIntStand()const
{
	return m_bMoveToIntStand;
}

bool AirsideFlightInSim::HasTowingToDepStand() const
{
	return m_bMoveToDepStand;
}

void AirsideFlightInSim::TowingToIntStand()
{
	m_bMoveToIntStand = true;
}

void AirsideFlightInSim::TowingToDepStand()
{
	m_bMoveToDepStand = true;
}

double AirsideFlightInSim::GetTakeoffPositionTime()
{
	return m_pFlightPerformanceManager->getTakeoffPositionTime(this);
}

double AirsideFlightInSim::GetLiftoffSpeed()
{
	return m_pFlightPerformanceManager->getLiftoffSpeed(this);
}

double AirsideFlightInSim::GetTakeoffRoll()
{
	return m_pFlightPerformanceManager->getTakeoffRoll(this);
}

double AirsideFlightInSim::GetPushbackSpeed()
{
	return m_pFlightPerformanceManager->getPushBackSpeed(this);
}

ElapsedTime AirsideFlightInSim::GetUnhookTime()
{
	return  m_pFlightPerformanceManager->getUnhookandTaxiTime(this);
}

double AirsideFlightInSim::GetTowingSpeed()
{
	return m_pFlightPerformanceManager->getTowingSpeed(this);
}

FlightPerformancesInSim * AirsideFlightInSim::GetPerformance() const
{
	return  m_pFlightPerformanceManager;
}

ElapsedTime AirsideFlightInSim::getEngineStartTime()
{
	return m_pFlightPerformanceManager->getEngineStartTime(this);
}

void AirsideFlightInSim::InitPaxBusContext(bool bArrival)
{
	if (bArrival)
	{
		if (m_pArrPaxBusStrategy == NULL)
		{
			m_pArrPaxBusStrategy = new AirsideArrPassengerBusStrategy(this);
		}
	}
	else
	{
		if (m_pDepPaxBusStrategy == NULL)
		{
			m_pDepPaxBusStrategy = new AirsideDepPassengerBusStrategy(this);
		}
	}

	if (m_pPaxBusContext == NULL)
	{
		m_pPaxBusContext = new AirsidePassengerBusContext();
	}

	if (bArrival)
	{
		m_pPaxBusContext->SetPaxBusStrategy(m_pArrPaxBusStrategy);
	}
	else
	{
		m_pPaxBusContext->SetPaxBusStrategy(m_pDepPaxBusStrategy);
	}
}
void AirsideFlightInSim::PassengerBusArrive( bool bArrival,const ElapsedTime& time,CAirsidePaxBusInSim* pPaxBus )
{
	if (m_pARCPortEngine == NULL || !m_pARCPortEngine->IsAirsideSel())
		return;

	InitPaxBusContext(bArrival);
	if (m_pPaxBusContext == NULL)
		return;

	bool bGenerateBaggage = m_pBaggageManager->IsGenerateGaggage();
	m_pPaxBusContext->PassengerBusArrive(time,pPaxBus,bGenerateBaggage);
}

void AirsideFlightInSim::PassengerBusLeave( bool bArrival,CAirsidePaxBusInSim* pPaxBus )
{
	if (m_pARCPortEngine == NULL || !m_pARCPortEngine->IsAirsideSel())
		return;

	InitPaxBusContext(bArrival);
	if (m_pPaxBusContext == NULL)
		return;
	m_pPaxBusContext->PassengerBusLeave(pPaxBus);
}

void AirsideFlightInSim::FlightArriveStand( bool bArrival,const ElapsedTime& time)
{
	if (m_pARCPortEngine == NULL || !m_pARCPortEngine->IsAirsideSel())
		return;

	InitPaxBusContext(bArrival);
	if (m_pPaxBusContext == NULL)
		return;

	m_pBaggageManager->FlightArriveStand(bArrival, time);
	bool bGenerateBaggage = m_pBaggageManager->IsGenerateGaggage();
	m_pPaxBusContext->FlightArriveStand(time,bGenerateBaggage);
}

void AirsideFlightInSim::FlightLeaveStand( bool bArrival )
{
	if (m_pARCPortEngine == NULL || !m_pARCPortEngine->IsAirsideSel())
		return;

	InitPaxBusContext(bArrival);
	if (m_pPaxBusContext == NULL)
		return;

	m_pPaxBusContext->FlightLeaveStand();
}

CAirsidePaxBusInSim* AirsideFlightInSim::GetAvailablePassengerBus(Person* pPerson,bool bArrival)
{
	if (m_pARCPortEngine == NULL || !m_pARCPortEngine->IsAirsideSel())
		return NULL;

	InitPaxBusContext(bArrival);
	if (m_pPaxBusContext == NULL)
		return NULL;

	return m_pPaxBusContext->GetAvailablePassengerBus(pPerson);
}

void AirsideFlightInSim::getACType( char *p_str ) const
{
	strcpy (p_str, m_pflightInput->getLogEntry().acType);
}


void AirsideFlightInSim::SetWaitPassengerTag( bool bWait )
{
	m_bWaitPaxTakeOn = bWait;
}

bool AirsideFlightInSim::GetWaitPassengerTag() const
{
	return m_bWaitPaxTakeOn;
}

CBagCartsParkingSpotInSim * AirsideFlightInSim::getArrivalBagCartsParkingSpot() const
{
	return m_pArrivalBagCartsParkingSpot;
}

void AirsideFlightInSim::setArrivalBagCartsParkingSpot( CBagCartsParkingSpotInSim * pSpotInSim )
{
	m_pArrivalBagCartsParkingSpot = pSpotInSim;
}

CBagCartsParkingSpotInSim * AirsideFlightInSim::getDeparturelBagCartsParkingSpot() const
{
	return m_pDeparturelBagCartsParkingSpot;
}

void AirsideFlightInSim::setDeparturelBagCartsParkingSpot( CBagCartsParkingSpotInSim * pSpotInSim )
{
	m_pDeparturelBagCartsParkingSpot = pSpotInSim;
}

AirsideFlightBaggageManager * AirsideFlightInSim::getBaggageManager()
{
	return m_pBaggageManager;
}

BOOL AirsideFlightInSim::getCargoDoorPosition(CPoint2008 &ptCargoDoor)
{	

	ARCVector3 dir; //= path.GetDistDir(m_curState.m_dist);
	CPoint2008 pos; //= m_curState.m_pPosition;
	if(!GetPosAtStand(pos,dir))
		return FALSE;

	DistanceUnit fltLen = GetLength();

	CPath2008 flightPath;
	flightPath.push_back(pos + dir.SetLength(fltLen/2) );
	flightPath.push_back(pos - dir.SetLength(fltLen/2) );

	ARCPipe flightbody(flightPath,GetCabinWidth());
	ptCargoDoor  = flightbody.m_sidePath2.GetDistPos(fltLen*0.8);

	double dDoorZ = 0;
	FltOperatingDoorSpecInSim* pFltDoorSpec =  GetAirTrafficController()->GetFltOperatingDoorSpec();
	if (pFltDoorSpec)
	{
		std::vector<ACTypeDoorOpen> vOpenDoors = pFltDoorSpec->getAllFlightDoors( GetFlightInput()->getType(m_curFlightType) );
		if(!vOpenDoors.empty())
		{
			ACTypeDoor* pACDoor = vOpenDoors.front().pDoor;
			dDoorZ = (pACDoor->m_dHeight + pACDoor->m_dSillHeight-1)*100;
		}
	}

	ptCargoDoor.setZ(dDoorZ);


	return TRUE;
}

BOOL AirsideFlightInSim::GetPosAtStand( CPoint2008& pos ,ARCVector3& dir ) const
{
	if(!m_curState.m_pResource)
		return FALSE;

	
	if (m_curState.m_pResource->GetType() == AirsideResource::ResType_StandLeadOutLine)//lead out line
	{
		StandLeadOutLineInSim* pLeadOutLine = (StandLeadOutLineInSim*)(m_curState.m_pResource);
		if (pLeadOutLine==NULL)
			return false;
		const CPath2008& path = pLeadOutLine->GetPath();
		dir = path.GetDistDir(m_curState.m_dist);
		if(m_curState.m_bPushBack)
			dir = -dir;
	}
	else if(m_curState.m_pResource->GetType() == AirsideResource::ResType_StandLeadInLine) // lead in line
	{
		StandLeadInLineInSim* pLeadInLine = (StandLeadInLineInSim*)(m_curState.m_pResource);
		if (pLeadInLine==NULL)
			return false;
		const CPath2008& path  = pLeadInLine->GetPath();
		dir = path.GetDistDir(m_curState.m_dist);
	}
	else 
		return FALSE;	


	pos = m_curState.m_pPosition;
	return TRUE;
}

void AirsideFlightInSim::CloseDoors()
{
	cpputil::autoPtrReset(m_pOpenDoors);
}

void AirsideFlightInSim::DisConnectBridges(CARCportEngine* _pEngine,const ElapsedTime& t)
{
	Terminal* pTerminal = _pEngine->getTerminal();
	//check the flight has bus service or not
	if(pTerminal )
	{
		//get stand
		ProcessorList *pProcList =  pTerminal->GetTerminalProcessorList();
		std::vector<BaseProcessor*> vBridgeProcessor;
		if(pProcList)
			pProcList->GetProcessorsByType(vBridgeProcessor,BridgeConnectorProc);

		std::vector<BaseProcessor *>::iterator iterBridge = vBridgeProcessor.begin();
		for (; iterBridge != vBridgeProcessor.end(); ++ iterBridge)
		{
			BridgeConnector *pBridgeConnector = (BridgeConnector *)*iterBridge;
			if(pBridgeConnector && pBridgeConnector->IsBridgeConnectToFlight(GetFlightInput()->getFlightIndex()))
			{
				if(_pEngine->IsOnboardSel())
				{
					bool bArrival = IsArrivingOperation();
					OnboardFlightInSim* pOnboardFlightInSim = _pEngine->GetOnboardSimulation()->GetOnboardFlightInSim(this,bArrival);
					if (pOnboardFlightInSim)
					{
						pBridgeConnector->DisOnboardConnect(t);
						//return 0;
					}	
				}

				pBridgeConnector->DisAirsideConnect(t);
				//break;
			}
		}		
	}
	CloseDoors();
}
