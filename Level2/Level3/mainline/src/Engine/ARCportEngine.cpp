#include "StdAfx.h"
#include ".\arcportengine.h"
#include "engine\Terminal.h"
#include "main\ARCPort.h"
#include "Inputs\SimParameter.h"
#include "Common\WinMsg.h"
#include "Engine\Monitor.h"
#include "Engine\AutoRoster.h"
#include "Engine\AutoCreateProcessorContainer.h"
#include "Inputs\PipeDataSet.h"
#include "Inputs\PipeGraph.h"
#include "Common\SimAndReportManager.h"
#include "Reports\SimLevelOfService.h"
#include "Inputs\SubFlowList.h"
#include "main\termplandoc.h"
#include "Inputs\AllGreetingProcessors.h"
#include "main\Floor2.h"
#include "Inputs\PaxBulkInfo.h"
#include "Inputs\Typelist.h"
#include "Inputs\Assigndb.h"
//#include "Landside\LandsideEngine.h"
#include "Main\LandSideDocument.h"
#include "Common\OnBoardException.h"
#include "Engine\Airside\AirsideSimConfig.h"
#include "Engine\FltOperatingDoorSpecInSim.h"
#include "Inputs\GateAssignmentConstraints.h"
#include "Inputs\ResourcePool.h"
#include "Engine\FlightsBoardingCallManager.h"
#include "Engine\FireEvacuationEvent.h"
#include "Inputs\Paxflow.h"

#include "Engine\AutoRoster.h"
#include "Engine\AutoCreateProcessorContainer.h"
#include "Engine\FireEvacuationEvent.h"
#include "Engine\ConvergePair.h"
#include "Engine\TerminalMobElementBehavior.h"
#include "Engine\RetailProcessor.h"
#include "Results\RetailLog.h"
#include "Results\RetailLogEntry.h"

#include "main\AutoRosterDlg.h"
//#include "main\resource.h"
#include "main\termplandoc.h"
#include "main\DynamicCreatedProcDlg.h"
#include "main\Floor2.h"
#include "common\SimAndReportManager.h"
#include "common\mutilreport.h"
#include "common\CodeTimeTest.h"
//#include "common\ARCFatalException.h"
//#include "common\ARCCriticalException.h"
//#include "common\ARCImportantException.h"
#include "common\projectmanager.h"
#include "common\elaptime.h"
#include "common\exeption.h"
#include "common\WinMsg.h"
#include "common\OnBoardException.h"
#include "common\Singleton.h"

#include "inputs\BagGateAssign.h"
#include "inputs\GageLagTimeData.h"
#include "inputs\flightconwithprociddatabase.h"
#include "inputs\PaxBulkInfo.h"
#include "inputs\AllPaxTypeFlow.h"
#include "inputs\resourcepool.h"
#include "inputs\HubbingDatabase.h"
#include "inputs\AllCarSchedule.h"
#include "inputs\PipeDataSet.h"
#include "inputs\paxflow.h"
#include "inputs\MobileElemTypeStrDB.h"
#include "inputs\SubFlowList.h"
#include "inputs\PaxFlowConvertor.h"
#include "inputs\AllGreetingProcessors.h"
#include "inputs\DirectionUtil.h"
#include "inputs\simparameter.h"
#include "inputs\pax_db.h"
#include "inputs\paxdata.h"
#include "inputs\flight.h"
#include "inputs\assigndb.h"
#include "inputs\probab.h"
#include "inputs\distlist.h"
#include "inputs\hubbing.h"
#include "inputs\bagdata.h"
#include "inputs\typelist.h"
#include "common\category.h"
#include "inputs\pipegraph.h"
#include "engine\ElevatorProc.h"
#include "engine\TracePredef.h"
#include "engine\Monitor.h"
#include "engine\terminal.h"
#include "engine\person.h"
#include "engine\boardcal.h"
#include "results\ElevatorLogEntry.h"
#include "results\ElevatorLog.h"
#include "results\resourceelementlog.h"
#include "results\PaxDestDiagnoseLog.h"
#include "results\paxlog.h"
#include "results\paxentry.h"
#include "results\fltlog.h"
#include "results\proclog.h"
#include "results\trainlog.h"
#include "reports\SimLevelOfService.h"
#include "reports\SimAutoReportPara.h"
#include "reports\MutiRunReportAgent.h"
#include "reports\ReportParaDB.h"
#include "reports\ReportParameter.h"
#include "../Engine/BillboardProc.h"
#include "Common/AirportDatabase.h"
#include "Engine/Airside/AirsideSimulation.h"
#include ".\Results\OutputAirside.h"
#include "..\Engine\Airside\AirsideSimConfig.h"

#include "Engine/FlightsBoardingCallManager.h"
#include "Engine/SimulationDiagnoseDelivery.h"
#include "../Inputs/GateAssignmentConstraints.h"
#include "main/LandSideDocument.h"
#include "landside\LandsideOutput.h"
//#include "landside\LandsideEngine.h"
#include "Engine/BoardingCallPolicy.h"
#include "Engine/FlightDelaysUtility.h"
#include "Engine/OnBoard/OnBoardSimEngine.h"
#include "Engine/SimEngineConfig.h"
#include "Engine/MobElementLifecycleManager.h"
#include "Engine/SimEngineCommunicationLayer.h"
#include "Engine/MobElementsDiagnosis.h"
#include "Common/ARCTracker.h"
#include "Engine/FltOperatingDoorSpecInSim.h"

#include "Engine/BridgeConnector.h"
#include "Results/BridgeConnectorLog.h"
#include "Results/BridgeConnectorLogEntry.h"
#include "..\Engine\MovingTrain.h"
#include "Common\SimAndReportManager.h"

#include "Inputs\Paxdist.h"
#include "OnboardSimulation.h"
#include "Engine/LandsideSimulation.h"
#include "Engine/LogContainer.h"

#include "process.h"
#include "procq.h"
#include "Results/MultiFileEventLog.h"
#include "Results/TempMobileEventLog.h"


#include "common\CPPUtil.h"
#include "landside\InputLandside.h"
#include "Common\FileOutPut.h"
#include "Results\Outpax.h"

extern std::vector<ConvergePair *> GreetPool;

class LandsideTerminalLinkageInSimManager;

CARCportEngine::CARCportEngine(void):
m_pOnboardSimulation(NULL)
,m_pLandsideSim(NULL)
{
	m_pARCport = NULL;
	

	PLACE_METHOD_TRACK_STRING();
	m_timeFireEvacuation.set(23,59,59);
	m_bFireOccur = false;
	m_pFltsBoardingCallManager = new FlightsBoardingCallManager;

}

CARCportEngine::~CARCportEngine(void)
{
	delete m_pFltsBoardingCallManager;

	getTerminal()->clearLogs();
}

int CARCportEngine::GetPercent()
{
	return m_nPercent;
}


int CARCportEngine::GetPercentOfRuns()
{
	return m_nPercentOfRuns;
}


void CARCportEngine::SetPercent( int _nPercent )
{
	m_nPercent = _nPercent;
}

void CARCportEngine::SetPercentOfRuns( int _nPercent )
{
	m_nPercentOfRuns = _nPercent;
}

void CARCportEngine::SetRunNumber( int _nRunNumber )
{
	m_nRunNumber = _nRunNumber;
}

int CARCportEngine::GetRunNumber()
{
	return m_nRunNumber;
}

void CARCportEngine::SendSimMessage(const CString &_csMsg)
{
	if(!::IsWindow(m_hWnd))
		return;

	strcpy( m_csDisplayMsg, _csMsg );
	SendMessage( m_hWnd, WM_SIM_STATUS_MSG, GetPercent(), (long)m_csDisplayMsg );
}

void CARCportEngine::SendSimFormatExMessage( FORMATMSGEX& _formatMsgEx  )
{
	if(!::IsWindow(m_hWnd))
		return;

	_formatMsgEx.diag_entry.iSimResultIndex = m_nRunNumber-1;
	SendMessage( m_hWnd, WM_SIM_STATUS_FORMATEX_MSG, GetPercent(), (long)& _formatMsgEx );
}


void CARCportEngine::SendSimFormatMessage( const FORMATMSG& _csFormatMsg )
{
	FORMATMSGEX msgEx;

	msgEx.strTime			= _csFormatMsg.strTime;
	msgEx.strMsgType		= _csFormatMsg.strMsgType;
	msgEx.strProcessorID	= _csFormatMsg.strProcessor;

	// parse the string to get id, mobile, passenger, flight,
	// 3 : pax id
	// 4 : flight id
	// 5 : mob type string
	int begin_pos	= 0;
	int end_pos		= 0;
	CString strPaxType = _csFormatMsg.strMobType;
	if( !strPaxType.IsEmpty() )
	{
		end_pos = strPaxType.Find("<@>");
		if( end_pos == -1 ) // need not to parse
		{
			msgEx.strMobileType = strPaxType;
		}
		else				// need to parse
		{
			int pos = 3;
			while( end_pos!= -1 && pos< 6)
			{
				switch( pos++ ) {
				case 3:
					msgEx.strPaxID = strPaxType.Mid(begin_pos, end_pos-begin_pos);
					break;
				case 4:
					msgEx.strFlightID = strPaxType.Mid(begin_pos, end_pos-begin_pos);
					break;
				case 5:
					msgEx.strMobileType = strPaxType.Mid(begin_pos, end_pos-begin_pos);
					break;
				default:
					break;
				}

				begin_pos = end_pos + 3;
				end_pos = strPaxType.Find( "<@>", begin_pos );
			}
		}
	}
	msgEx.diag_entry = _csFormatMsg.diag_entry;
	msgEx.strOther = _csFormatMsg.strOtherInfo;
	SendSimFormatExMessage( msgEx );
}

void CARCportEngine::SendSimMessageOfRuns()
{
	if(!::IsWindow(m_hWnd))
		return;
	SendMessage( m_hWnd, WM_SIM_STATUS_MSG2, m_nPercentOfRuns, m_nRunNumber );
}
void CARCportEngine::SendSimSortLogMessage(int nPercent, int nCurFile, int nFileCount)
{
	if(!::IsWindow(m_hWnd))
		return;
	std::vector<int> vParam;
	vParam.push_back(GetPercent());//the original simulation percentage
	vParam.push_back(nCurFile);//file number
	vParam.push_back(nFileCount);//total temperate log file count

	//the percentage of current log file proceed
	SendMessage( m_hWnd, WM_SIM_SORTEVENTLOG, nPercent, (long)&vParam );
}

//##ModelId=3E2FAD070388
void CARCportEngine::SendEventMessage( int _nEventNum, ElapsedTime _eCurTime, const char* _szTypeName )
{
	m_eventMsg.nEventNum = _nEventNum;
	char str[64];
	ElapsedTime tmptime(_eCurTime.asSeconds() % WholeDay);
	tmptime.printTime ( str );

	CStartDate sDate = getTerminal()->m_pSimParam->GetStartDate();
	bool bAbsDate;
	COleDateTime date;
	int nDtIdx;
	COleDateTime time;
	CString sDateTimeStr;
	sDate.GetDateTime( _eCurTime, bAbsDate, date, nDtIdx, time );
	if( bAbsDate )
	{
		sDateTimeStr = date.Format(_T("%Y-%m-%d"));
	}
	else
	{
		sDateTimeStr.Format("Day %d", nDtIdx + 1 );
	}
	sDateTimeStr += "  ";
	sDateTimeStr += str;


	m_eventMsg.csEventTime = sDateTimeStr;
	m_eventMsg.csEventType = CString( _szTypeName );
	ElapsedTime oneDay;
	oneDay.setHours( 24 );

	//add by adam 2007-09-26 
	ElapsedTime estStartTime = getTerminal()->m_pSimParam->GetStartTime() - getTerminal()->m_pSimParam->GetInitPeriod();

	if (estStartTime < 0l)
	{
		estStartTime = 0l;
	}
	ElapsedTime estEndTime = getTerminal()->m_pSimParam->GetEndTime() + getTerminal()->m_pSimParam->GetFollowupPeriod();
	//End add by adam 2007-09-26 

	float f = ( _eCurTime - estStartTime ) / ( estEndTime - estStartTime );
	m_nPercent = 27 + int(f * 68);

	//add by adam 2007-10-11
	if (m_nPercent < 27)
	{
		m_nPercent = 27;
	}

	if (95 < m_nPercent)
	{
		m_nPercent = 95;
	}
	//add by adam 2007-10-11

	if(!::IsWindow(m_hWnd))
		return;
	SendMessage( m_hWnd, WM_SIM_STATUS_EVENT, GetPercent(), (long)&m_eventMsg );
}





//Main simulation engine function.
//It reloads all settings data and save them to appropriate lists and sets current time. Then the main engine loop function eventList::start() is invoked.
//All data logs created for animation are written and finally closed.
//##ModelId=3E2FAD0703A5
void CARCportEngine::runSimulation (HWND _hWnd, const CString& _csProjPath,const SelectedSimulationData& simSelData , CARCPort *pARCport  )
{
	PLACE_METHOD_TRACK_STRING();

	ASSERT(pARCport != NULL);
	ASSERT(getTerminal() != NULL);
	if(pARCport == NULL ||getTerminal() == NULL)
		return;

	m_pARCport = pARCport;


	// Diagnose delivery helper class, deliver diagnose information to simulation engine dialog
	DiagnoseDelivery()->setHandle(_hWnd);


	// simulation config
	simEngineConfig()->setSimParameter( getTerminal()->m_pSimParam );
	// Airside config
	simSelData.IsAirsideSel() ? simEngineConfig()->simAirsideMode() : 1;
	// Landside config
	simSelData.IsLandsideSel() ? simEngineConfig()->simLandsideMode() : 1;
	// Terminal config
	simSelData.IsTerminalSel() ? simEngineConfig()->simTerminalMode() : 1;
	// OnBoard config
	simSelData.IsOnBoardSel() ? simEngineConfig()->simOnBoardMode() : 1;

	// init Mobile Elements Diagnosis service.
	MobElementsDiagnosis()->Initialize( _csProjPath );


#ifdef CODE_PERFOMANCE_DEBUGE_FLAG
	CCodeTimeTest::InitTools();
#endif
	/*===============================================================

	In old DOS version, the sim engin run in an seperate EXE. it will 
	load input data first. then call runSimulation(). Now the input data
	are in the memory already. We don't have to reload it. just need to 
	make sure all the data is clean.

	void CleanTheInputData();

	===============================================================*/
	// catch all sim engine exception
	CTermPlanDoc* pDoc = (CTermPlanDoc*) ((CMDIChildWnd *)((CMDIFrameWnd*)AfxGetApp()->m_pMainWnd)->GetActiveFrame())->GetActiveDocument();
	assert( pDoc );

	/*m_AirsideSel = simSelData.IsAirsideSel() ;
	m_LandSel = simSelData.IsLandsideSel() ;
	m_OnboardSel = simSelData.IsOnBoardSel() ;
	m_terminal = simSelData.IsTerminalSel() ;*/
	mSimSelectData = simSelData;

	p_simAirside = NULL ;

	try			
	{	
		//set the processor engine pointer
		SetProcListEngine();
		//init array which store every mobile elememnt count 
		int iMobleTypeCount = getTerminal()->m_pMobElemTypeStrDB->GetCount();
		m_vNonPaxCountByType.resize( iMobleTypeCount );

		m_hWnd = _hWnd;
		m_bCancel = false;

		//init visitor , bags etc. rotation data
		Person::initSpacing();

		// write mirrored strDict, typeList, sector & category files to logs dir
		char filename[_MAX_PATH];
		PROJMANAGER->getFileName ( _csProjPath, StringDictBinFile, filename);
		getTerminal()->inStrDict->storeDictionary (filename);

		getTerminal()->inTypeList->setFileType (TypeListBinFile);
		getTerminal()->inTypeList->saveDataSet(_csProjPath, false);
		getTerminal()->inTypeList->setFileType(PassengerDescriptionFile);

		getTerminal()->closeLogs();



		CSimGeneralPara* pSim = getTerminal()->m_pSimParam->GetSimPara();
		getTerminal()->GetSimReportManager()->addNewSimResult( getTerminal()->m_pSimParam ,_csProjPath, getTerminal()  );

		//m_pSimParam->
		getTerminal()->linkFlightData(pDoc->GetProjectID());
		if (!getTerminal()->flightSchedule->RegenerateSchedule(pSim, m_simFlightSchedule))
			return;

		getTerminal()->GetSimReportManager()->SetStartDate(m_simFlightSchedule.GetStartDate());

		if (getTerminal()->flightSchedule->GetStartDate().IsAbsoluteDate())
		{
			COleDateTimeSpan deltaDate = getTerminal()->flightSchedule->GetStartDate().GetDate() - m_simFlightSchedule.GetStartDate().GetDate();
			getTerminal()->GetSimReportManager()->SetUserStartTime( pSim->GetStartTime() + (long)deltaDate.GetTotalSeconds() );		
			getTerminal()->GetSimReportManager()->SetUserEndTime( pSim->GetEndTime() + (long)deltaDate.GetTotalSeconds() ); 
		}
		else
		{
			getTerminal()->GetSimReportManager()->SetUserStartTime(pSim->GetStartTime());
			getTerminal()->GetSimReportManager()->SetUserEndTime(pSim->GetEndTime());
		}
		getTerminal()->GetSimReportManager()->SetAirsideSim(IsAirsideSel());
		//		m_pSimAndReportManager->SetUserStartTime(m_simFlightSchedule.GetUserStartTime());
		//		m_pSimAndReportManager->SetUserEndTime(m_simFlightSchedule.GetUserEndTime());

		// calculate the bisect line, intersection point.
		getTerminal()->m_pPipeDataSet->CalculateAllIntermediatData();
		// calculate needed data of moving sidewalk attached to pipe
		getTerminal()->m_pPipeDataSet->CalculateAllTimePointOnSideWalk();

		int iRunNumber = pSim->GetNumberOfRun();


		// get all floor's altitude
		int nFloorCount = pDoc->GetFloorByMode( EnvMode_Terminal ).m_vFloors.size();
		std::vector<double>vFloorAltitudes;
		for(int i=0; i<nFloorCount; i++) 
		{			
			vFloorAltitudes.push_back( pDoc->GetFloorByMode( EnvMode_Terminal ).m_vFloors[i]->RealAltitude() );		
		}

		getTerminal()->procList->SetDynamicCreateProcFlag( false );
		// init data which TLOS check need
		CSimLevelOfService* pTLOSSim = NULL;
		if( pSim->GetClassType() == "LEVEOFSERVICE" )
		{
			pTLOSSim = (CSimLevelOfService*)pSim;
			getTerminal()->m_pTLOSMonitor->InitTLOSData( );
			getTerminal()->m_pAutoRoster->init( iRunNumber );
			getTerminal()->m_pAutoCreatedProcContainer->init();
			getTerminal()->procList->SetDynamicCreateProcFlag( pTLOSSim->getDynamicCreateProcFlag() ? true : false );
			getTerminal()->procList->InitDynamicCreateProcList( getTerminal() );
		}
		else
		{
			//clear TLOS flag if last time run with TLOS parameter
			getTerminal()->procList->ResetAllTLOSFlag();
		}
		if( !getTerminal()->procList->IfAllBaggageProcDataValid() )
		{
			return ;
		}
		//m_pPassengerFlowDB->InitFlowRules();

		CPaxFlowConvertor tempFlowConvetor;
		tempFlowConvetor.SetInputTerm( getTerminal() );
		CAllPaxTypeFlow tempAllPaxFlow;
		// build source / dest processor of every conveyor processor( or related with conveyor processor in pax flow )
		getTerminal()->procList->InitProcessorSourceDestProcessors( &tempFlowConvetor, &tempAllPaxFlow );

		getTerminal()->procList->initResourcePoolData();

		//load GateAssignmentConstraint for stand reassignment in Airside SimEngine
		GateAssignmentConstraintList gateAssignConstraints;
		gateAssignConstraints.SetInputTerminal(getTerminal());
		gateAssignConstraints.loadDataSet(_csProjPath);
		gateAssignConstraints.GetStandAdjConstraint().loadDataSet(_csProjPath,getTerminal());

		FltOperatingDoorSpecInSim pFltOperatingDoorSpec(getTerminal()->m_pAirportDB);
		//note: if engine run with TLOS which has dynamically create processor function, number of run must be 1
		for( int iRun=0; iRun<iRunNumber; ++iRun )
		{	
			int nPos = 100 / iRunNumber * iRun;
			SetPercentOfRuns( nPos );
			SetRunNumber( iRun + 1 );
			SendSimMessageOfRuns();

			CString sSubName;
			sSubName.Format("%d", iRun );
			sSubName =	strSimResult + sSubName;
			getTerminal()->GetSimReportManager()->SetCurrentSimResult(sSubName );
			InitLogs( _csProjPath );

			// need to update the load base on the random value.
			m_simFlightSchedule.updateFlights( getTerminal()->flightData );

			if( pTLOSSim )
			{
				getTerminal()->m_pAutoRoster->setCurRosterIndex( iRun );
				getTerminal()->m_pAutoRoster->clearUserRoster( getTerminal(), pTLOSSim );

				// init dynamic created
				getTerminal()->procList->InitDynamicCreateProcList( getTerminal() );
			}

			// init area density
			getTerminal()->m_pAreas->InitDataBeforeEngine( &tempFlowConvetor, &tempAllPaxFlow );
			//init all data used by baggage device processor
			//procList->InitAllBaggageProcData();

			//init billboard 
			//std::vector<BaseProcessor *> vecBillboardProc;//billboard processor array
			//GetProcessorList(EnvMode_Terminal)->GetProcessorsByType(vecBillboardProc,BillboardProcessor);

			//int nBillboardProcCount = static_cast<int>(vecBillboardProc.size());
			//for (int i = 0; i < nBillboardProcCount; i++)
			//{
			//	((BillboardProc *)vecBillboardProc[i])->InitWholeExposurePath();
			//}

#ifdef WANT_TO_TRACE_QUUEU_INFO
			getTerminal()->procList->InitQueueTraceFlag();
#endif

			//init undirected graph of pipe system 
			getTerminal()->m_pPipeDataSet->m_pPipeMgr->initGraphMgr( getTerminal(), pDoc->GetFloorByMode( EnvMode_Terminal ).m_vFloors.size() );

			//initialize congestion grid for every floor
			m_pARCport->getCongestionAreaManager().InitGrid(getTerminal(), pDoc->GetFloorCount());

			/////////////////////////////////////////////////
			// prepare data for engine log and events
			getTerminal()->procAssignDB->createDefaultEvent();
			getTerminal()->procAssignDB->computeAbsolutTime( getTerminal(), &m_simFlightSchedule );

			// generate train logs
			GenerateTrains();

			// init flight log from flight schedule
			//flightSchedule->writeBinarySchedule (flightLog);
			m_simFlightSchedule.writeBinarySchedule(getFlightLog());

			// processor in the proclist is the only data the engine will interfaced. 
			//need to refreshed before the engine start.
			getTerminal()->procList->RefreshRelatedDatabase();

			//clear dynamic processor flag
			getTerminal()->procList->ClearAllProcDynamicFlag();

			getTerminal()->procAssignDB->divideEvents( getTerminal() );

			//			m_timeFireEvacuation = m_pSimAndReportManager->GetUserEndTime() + m_pSimParam->GetFollowupPeriod();
			//			int nSimDays = ( m_pSimAndReportManager->GetUserEndTime().asSeconds() + m_pSimParam->GetFollowupPeriod().asSeconds() ) / WholeDay + 1;
			if( !getTerminal()->flightSchedule->GetStartDate().IsAbsoluteDate() )
				getTerminal()->procAssignDB->GenerateEntryForMultipleDays( 3, getTerminal()  );
			/////////////////////////////////////////////////

			SetPercent( 10 );
			SendSimMessage( "Analysis Process Data..." );


			//bagData->BuildFlightAssignToBaggageDevice();


			//build all possible path in every process and set its internal flow
		//	getTerminal()->m_pSubFlowList->BuildAllProcessPossiblePath();				
			getTerminal()->m_pSubFlowList->SetTransitionLink( *getTerminal()->m_pPassengerFlowDB, *getTerminal()->m_pStationPaxFlowDB, *getTerminal()->m_pStationPaxFlowDB );


			//find all place which passenger and it's visitor, hand bag..etc meeting
			CPaxFlowConvertor tempConvetor;
			tempConvetor.SetInputTerm( getTerminal() );
			getTerminal()->m_pGreetingProcessors->FindGreetingPlaceFromAllPaxFlow( tempConvetor );


			//generate passengers
			//initialize bulks
			getTerminal()->m_pPaxBulkInfo->initBulk();

			bool bInitializeNoError = true;

			//clear the statistic data
			for( int i=0; i<iMobleTypeCount; ++i )
			{
				m_vNonPaxCountByType[i] = 0;
			}

			long nPax = 0;
			try
			{
				if(!simEngineConfig()->isSimAirsideModeAlone())
					nPax = generatePassengers(&m_simFlightSchedule);

				if(simEngineConfig()->isSimTerminalModeAlone())
				{
					SendSimMessage( "Analysis Flight delays..." );
					FlightDelaysUtility::impactFlightDelaysEffectToMobElement(getPaxLog(), &m_simFlightSchedule); 
				}
				{
					// SORT MOBILE ELEMENTS
					// all pax and bags must then be sorted by their entry time
					sprintf( m_csDisplayMsg, "Sorting %d Mobile Elements...", getPaxLog()->getCount() );
					SetPercent( 23 );
					SendSimMessage( m_csDisplayMsg );

					//NOTE:: must invoke only once!!!
					CProgressBar bar(_T("Sorting Mobile Elements...") );
					getPaxLog()->sortPassengers(bar);

					//sprintf( m_csDisplayMsg, "Sorting %d Bags...", bagLog->getCount() );
					SetPercent( 24 );
					//SendSimMessage( m_csDisplayMsg );
					getBagLog()->sortBags();
					SetPercent( 25 );
				}

			}
			catch( Exception *except )
			{
				char string[256];
				except->getMessage( string );
				strcpy( m_csDisplayMsg, string );
				SendSimMessage( m_csDisplayMsg );
				delete except;

				nPax = 0;
				bInitializeNoError = false;
			}
			catch( ARCException* arc_exception )
			{
				SendSimFormatMessage(arc_exception->getFormatErrorMsg() );
				delete arc_exception;

				nPax = 0;
				bInitializeNoError = false;
			}

			SetPercent( 16 );
			SendSimMessage( "Analysis Passenger Flow Data..." );
			// init landside simulation			
			cpputil::autoPtrReset(m_pLandsideSim);
			if(simEngineConfig()->isSimLandsideMode())
			{
				try
				{
					InputLandside* pInput = new InputLandside(&pDoc->GetTerminal(), &pDoc->GetFloorByMode(EnvMode_LandSide) );
					pInput->LoadInputData(pDoc->m_ProjInfo.path, pDoc->GetProjectID());
					m_pLandsideSim = new LandsideSimulation(pInput,m_pARCport->GetOutputLandside());
					m_pLandsideSim->Initialize(this, pDoc->GetFloorByMode(EnvMode_LandSide),_csProjPath);				
				}
				catch (Exception* except)
				{
					char string[256];
					except->getMessage (string);
					strcpy( m_csDisplayMsg, string );
					SendSimMessage( m_csDisplayMsg );
					stdcerr << '\n' << string;

					delete except;
				}
			}	

			//init lead to gate logic from pax flow
			CDirectionUtil tempUtil;
			LandsideTerminalLinkageInSimManager *pLinkage = NULL;
			if(m_pLandsideSim)
				pLinkage = m_pLandsideSim->GetLandsideTerminalLinkageManager();
			tempUtil.InitAllProcDirection( tempConvetor, pLinkage);
			getTerminal()->procList->InitAllProcDirection();


			//init every single processor's state
			// by doing this ,all processors,no mater it is in proclist or not, for example, 
			// the entry doors and exit doors of station are not in proclist but need also be
			// init
			getTerminal()->CleanInputData();


			// init all processor's isOpen = FALSE;
			// Roster will open all procesors which use default roster at 0:00, and close it at 24:00
			getTerminal()->procList->InitProcsOpenAvaiFlag();

			// init elevator's necessary data, such as every floor's height....etc		
			// must be called after call procList->RefreshRelatedDatabase();
			if(!getTerminal()->procList->InitAllElevatorData( vFloorAltitudes, getTerminal() ))
			{
				SetPercent(100);
				SetPercentOfRuns( 100 );
				SetRunNumber( iRunNumber );
				SendSimMessageOfRuns();
				return;
			}

			// 			// generate the logs for the all Elevators		
			// 		    GenerateElevatorLogEntry( );


			//because the conveyor architecture is designed in "proxy pattern ", so we must init a real performer before
			// engine use conveyor.
			getTerminal()->procList->InitEveryConveyor( *getTerminal()->miscData );

			
			// generate the logs for the all Elevators		
			GenerateElevatorLogEntry( );



			// init air side simulation
			AirsideSimulation simAirside(pDoc->GetProjectID(),this,_csProjPath,&pFltOperatingDoorSpec) ;
			if(!m_bFireOccur)
			{
				p_simAirside = &simAirside ;
				AirsideSimConfig airsidesimconf;
				airsidesimconf.bOnlySimStand = !simEngineConfig()->isSimAirsideMode();
				simAirside.SetOutputAirside(pDoc->GetOutputAirside());
				try
				{
					simAirside.m_pOuput->PrepareLog(this,_csProjPath);
					//set Airside simEngine requisite data which saved in file
					simAirside.SetGateAssignmentConstraints(&gateAssignConstraints);
					simAirside.Initialize(pDoc->GetProjectID(),airsidesimconf);	
				}
				catch (Exception *except)
				{
					char string[256];
					except->getMessage (string);
					strcpy( m_csDisplayMsg, string );
					SendSimMessage( m_csDisplayMsg );
					stdcerr << '\n' << string;
					delete except;
					bInitializeNoError = false;
				}
				catch( CADOException& e)
				{
					strcpy( m_csDisplayMsg, "There is a database exception raised while init airside engine.");
					SendSimMessage( m_csDisplayMsg );
					//stdcerr << '\n' << string;
					bInitializeNoError = false;

					std::ofstream os;
					os.open(_csProjPath + "\\simulation_failure_dump.log");
					if (os.is_open())
					{
						os << m_csDisplayMsg << std::endl << "Error Message: " << ((const char*)e.ErrorMessage()) << std::endl;
						DUMP_TRACK_TO_STREAM(os);
					}
				}
			}			


			//OnBoard simEngine
			cpputil::autoPtrReset(m_pOnboardSimulation);
			m_pOnboardSimulation = new  OnboardSimulation(this,m_pARCport->GetInputOnboard()); //onBoardSimEngine( pDoc->GetProjectID(),this,_csProjPath );
			if(simEngineConfig()->isSimOnBoardMode())
			{
				try
				{
					m_pOnboardSimulation->Initialize();
				}
				catch(OnBoardSimEngineConfigException* onboard_config_except)
				{
					OnBoardDiagnose* pDiagnose = 
						(OnBoardDiagnose*)onboard_config_except->GetDiagnose();
					DiagnoseDelivery()->DeliveryOnBoardDiagnose(pDiagnose);
					delete onboard_config_except;

					bInitializeNoError = false;
				}
			}

			////simEngineCommLayer()->setSimEngineModes( &simAirside, &landsideengine, this, &onBoardSimEngine);

			if( simEngineConfig()->isSimALTOModes() && bInitializeNoError )
			{	
				m_simBobileelemList.Initialize( nPax );

				ElapsedTime startTime, endTime, simTime;
				EventList simulationEngine;
				try 
				{				
					Event::initEvents (&simulationEngine);

					ElapsedTime estStartTime = getTerminal()->m_pSimParam->GetStartTime() - getTerminal()->m_pSimParam->GetInitPeriod();
					estStartTime = MAX(estStartTime,0L);
					simAirside.SetSimStartTime(estStartTime);				
					

					ElapsedTime estEndTime = getTerminal()->m_pSimParam->GetEndTime() + getTerminal()->m_pSimParam->GetFollowupPeriod();
					simAirside.SetSimEndTime(estEndTime);
					simulationEngine.SetSimulationEndTime(estEndTime);
					
					

					//init airside simulation ,there must have a judge whether user want to run airside simulation ****/					
					if(!m_bFireOccur)
						simAirside.AirsideEventListInitialize( &simulationEngine);	

				
					// init Mobile Elements Lifecycle Management service.
					//if(simEngineConfig()->isSimOnBoardAlone())
					//	MobElementsLifeCycleMgr()->initialize(getPaxLog(), &m_simFlightSchedule);
					//else
					{//TODO:: Generate passenger's logic has problem in code below. need replace in the future. 
						//All pax generation and activation logic implemented in MobElementsLifeCycleMgr object.
						CFlightPaxLogContainer* container = CFlightPaxLogContainer::GetInstance() ;
						container->Reset() ;
						container->SetFlightSchedule(&m_simFlightSchedule) ;
						container->SetPaxLog(getPaxLog()) ;
						container->InitContainer(simEngineConfig()) ;
						ActivePassenger();
					}

					if(simEngineConfig()->isSimTerminalMode())
						TrainEvent.initList(getTerminal()->m_pTrainLog,-1);

					// before run engine, create all resource element
					long _lResourceElementCount =getTerminal()->m_pResourcePoolDB->createResourceElement( getTerminal(), nPax );

					//bagEvent.initBagList (bagLog);
					getTerminal()->procAssignDB->initEvents();

					getTerminal()->procList->writeProcLog();

					//BoardingCallEvent::loadBoardingCalls (procList, &m_simFlightSchedule,	flightData, this);
					m_pFltsBoardingCallManager->Initialize(getTerminal()->procList, &m_simFlightSchedule, getTerminal()->flightData, getTerminal());		

					//		procList->writeProcLog();

					//landside simulation
					if(m_pLandsideSim)
					{	
						m_pLandsideSim->SetEstSimStartEndTime(estStartTime,estEndTime);
						m_pLandsideSim->Start(this);
					}

					SetPercent( 27 );
					SendSimMessage( "Running..." );

					//#define ECHO_PRE
#ifdef ECHO_PRE		
					//	createEchoFiles(_csProjPath);
#endif
					// generate fire evacutaion event if user click the fire button
					if( m_bFireOccur )
					{
						FireEvacuationEvent* pEvent = new FireEvacuationEvent;						
						pEvent->setTime( m_timeFireEvacuation );						
						pEvent->addEvent();						
					}			


					startTime.getSystemTime();
					
					int nprocCount=getTerminal()->procList->getProcessorCount();
					//for (int i = 0; i <nprocCount; ++i)
					//{
					//	Processor *pProc = getTerminal()->procList->getProcessor(i);
					//	//pProc->getTotalQueueLength();
					//	ProcessorQueueBase *baseQueue=pProc->GetQueue();
					//	ProcessorQueue *Pqueue=static_cast<ProcessorQueue*>(baseQueue);
					//	if (Pqueue&&Pqueue->m_approachList.getCount()>0 )
					//	{
					//		//Pqueue->getTotalQueueLength();
					//		Pqueue->ClearAllTempData();//clear person object in m_approachlist in order avoid some persons in it while we click "cancel"button and then start again run simulation
					//		TRACE("Processor index:%d",pProc->getIndex());

					//	}
					//	
					//}
					simTime = simulationEngine.start(this, &m_bCancel, true );
					//end						
				}
				catch (ARCSystemError* system_exception) 
				{
					SendSimFormatMessage( system_exception->getFormatErrorMsg() );
					simulationEngine.kill();
					delete system_exception;
				}
				catch (ARCSimEngineException* sim_exception )
				{
					SendSimFormatMessage( sim_exception->getFormatErrorMsg() );
					simulationEngine.kill();
					delete sim_exception;
				}
				catch ( ARCException * arc_exception )
				{
					SendSimFormatMessage( arc_exception->getFormatErrorMsg() );
					delete arc_exception;
				}
				catch (Exception *except)
				{
					char string[256];
					except->getMessage (string);
					strcpy( m_csDisplayMsg, string );
					SendSimMessage( m_csDisplayMsg );
					stdcerr << '\n' << string;

					delete except;
				}				

				//flush air side log
				if(!m_bFireOccur)
				{
					simAirside.FlushLog();
					if(simAirside.m_pOuput)
					{
						simAirside.m_pOuput->FlushLog(pDoc->GetProjectID());
						simAirside.m_pOuput->SaveLog(_csProjPath);
					}
				}

				//flush landside log
				if(m_pLandsideSim){
					m_pLandsideSim->End(this,simTime,m_bCancel);
					m_pLandsideSim->SaveLog(_csProjPath);
					cpputil::autoPtrReset(m_pLandsideSim);
				}
				//set the static data member as max time to avoid unnecessary fire evacuation logic
				TerminalMobElementBehavior::SetFireEvacuationTime(ElapsedTime(23*3600+59*60+59l) );
				m_bFireOccur = false;						


				SetPercent( 95 );
				int nProc = getTerminal()->procList->getProcessorCount();
				sprintf( m_csDisplayMsg, "Number of Processors: %d", nProc );
				SendSimMessage( m_csDisplayMsg );


				for( int i=0; i<iMobleTypeCount; ++i )
				{
					sprintf( m_csDisplayMsg, "Number of %s: %d", getTerminal()->m_pMobElemTypeStrDB->GetString( i ) , m_vNonPaxCountByType[i]  );
					SendSimMessage( m_csDisplayMsg );
				}

				int nTrain = getTerminal()->m_pTrainLog->getCount();
				sprintf( m_csDisplayMsg, "Number of Trains: %d", nTrain );
				SendSimMessage( m_csDisplayMsg );



				endTime.getSystemTime();
				char str[512];
				(endTime - startTime).printTime( str );
				sprintf( m_csDisplayMsg, "Simulation duration: %s", str );
				SendSimMessage( m_csDisplayMsg );

				SetPercent( 96 );
				strcpy( m_csDisplayMsg, "Testing flow integrity" );
				SendSimMessage( m_csDisplayMsg );
				getTerminal()->procList->clearPassengers (simTime);


				//
				if(m_pOnboardSimulation)
					m_pOnboardSimulation->clearPerson(simTime);



				SetPercent( 98 );
				strcpy( m_csDisplayMsg, "Sorting processor logs" );
				SendSimMessage( m_csDisplayMsg );

				getTerminal()->procList->AddAllDynamicCreatedProcIntoList();
				////when TLOS created processor dynamicly ,we need update proc index in pax log to proc's new index
				//RefreshProcIdxInLogs();//move to CloseLogs()

				try 
				{
					getTerminal()->procList->updateProcLog();
				}
				catch (ARCSystemError *system_exception)
				{
					SendSimFormatMessage( system_exception->getFormatErrorMsg() );
					//					cerr << " of post processing";
					//throw shutDown;
					delete system_exception;
				}

				getTerminal()->procList->FlushElevatorLog();
				getTerminal()->procList->FlushBridgeLog(simTime);
				getTerminal()->procList->FlushRetailProcess();
				getTerminal()->procList->ClearStationMovingTrain();
				FlushMovingTrain();



				//save landside logs
				//landsideengine.SaveEvents();
				//pDoc->GetLandsideDoc()->GetOutput()->GetSimLogs().SaveLogs(_csProjPath);



				// kill resource element, flush resource element and delete all resource element
				getTerminal()->m_pResourcePoolDB->clearAllPoolEngineData( simTime );


				DeleteGreetPoolData();
				GreetPool.clear();

				//clear MovingTrain
				ClearMovingTrain();
				m_simBobileelemList.Clear();


				//getTerminal()->closeLogs( _csProjPath );
				CloseLogs(_csProjPath);

				CSimItem*	pSimItem = getTerminal()->GetSimReportManager()->getSimItem(sSubName  );
				ASSERT( pSimItem );
				pSimItem->setBeginTime( simulationEngine.GetStartTime() );
				pSimItem->setEndTime( simulationEngine.GetEndTime() );	

				if( pSim->GetClassType() =="AUTOREPORT" )
				{
					CMutiRunReportAgent temp;
					temp.Init( _csProjPath, getTerminal() );
					CSimAutoReportPara* pAutoReport = (CSimAutoReportPara*)pSim;
					int iReportCount = pAutoReport->GetReportCount();
					for(int ii=0; ii<iReportCount; ++ii )
					{
						CReportParameter* pPara =  getTerminal()->m_pReportParaDB->GetReportPara( pAutoReport->At( ii ) );
						if( pPara )
						{
							temp.AddReportWhatToGen( pPara->GetReportCategory(), pPara ,pDoc->GetFloorByMode( EnvMode_Terminal ).m_vFloors.size());		
						}

					}
					temp.GenerateAll();
				}		

				//#define ECHO_POST
#ifdef ECHO_POST		
				//	createEchoFiles(_csProjPath);
#endif
			}//	if( nPax > 0 )
			else
			{
				DeleteGreetPoolData();
				GreetPool.clear();

				//clear MovingTrain
				ClearMovingTrain();
				m_simBobileelemList.Clear();

				//getTerminal()->closeLogs( _csProjPath );
				CloseLogs(_csProjPath);
				//				pSimItem->setBeginTime(0);
				//				pSimItem->setEndTime(0);	
			}

			SetPercent( 100 );
			sprintf( m_csDisplayMsg, "End of #%d Simulation Run", iRun+1 );
			SendSimMessage( m_csDisplayMsg );
			SendSimMessage( "//=======================================================" );

			// must reload inputs data
			getTerminal()->procAssignDB->loadDataSet( _csProjPath );



			//------------------------------------------------------------------
			// release service objects( singleton ) which is effective in single running process.
			//DeletionManager::UnRegister( MobElementsLifeCycleMgr() );
			//MobElementsLifeCycleMgr()->ReleaseInstance();
			//DeletionManager::UnRegister( simEngineCommLayer() );
			//simEngineCommLayer()->ReleaseInstance();

		}// for( int iRun=0; iRun<iRunNumber; ++iRun )


		SetPercentOfRuns( 100 );
		SetRunNumber( iRunNumber );
		SendSimMessageOfRuns();

		getTerminal()->GetSimReportManager()->SetInputModified( false );
		getTerminal()->GetSimReportManager()->saveDataSet( _csProjPath, false );

		if( pTLOSSim ) // TLOS sim
		{				
			if( getTerminal()->procList->GetDynamicCreatedProcs() && getTerminal()->procList->GetDynamicCreatedProcs()->getProcessorCount() > 0 )
			{
				getTerminal()->inStrDict->storeDictionary (filename);//may create new string entry
				getTerminal()->m_pAutoCreatedProcContainer->addAutoProc( getTerminal()->procList->GetDynamicCreatedProcsPrototype(), getTerminal()->procList->GetDynamicCreatedProcs() );				
				CDynamicCreatedProcDlg	dlg( getTerminal() );
				dlg.DoModal();
			}

			CAutoRosterDlg dlg( getTerminal(), _csProjPath, pDoc  );
			dlg.DoModal();

			// clear dynamic created processors
			getTerminal()->procList->InitDynamicCreateProcList( getTerminal() );
		}
		// set the current SimResult and update the project bar
		getTerminal()->setCurrentSimResult(0,true);
		getTerminal()->GetSimReportManager()->SetCurrentSimResult(0);
		getTerminal()->setPreSimResult( -1 );


	}
	catch( ARCException* arc_exception)
	{
		SetPercent( 100 );
		SendSimFormatMessage( arc_exception->getFormatErrorMsg() );

		SetPercentOfRuns( 100 );
		SendSimMessageOfRuns();

		delete arc_exception;
	}
	catch( Exception *except )
	{
		SetPercent( 100 );
		char string[256];
		memset(string,0,256);
		except->getMessage (string);
		strcpy( m_csDisplayMsg, string );
		SendSimMessage( m_csDisplayMsg );
		stdcerr << '\n' << string;

		SetPercentOfRuns( 100 );
		SendSimMessageOfRuns();

		delete except;
	}

#ifdef CODE_PERFOMANCE_DEBUGE_FLAG
	CCodeTimeTest::TraceOutResultToFile();
#endif
	m_simFlightSchedule.clear();

	// when simulation done, global service objects need to be delete.
	// such as MobElementsDiagnosis(), simEngineConfig(), DiagnoseDelivery().
	// or exception occur, some service objects in single simulation running process
	// such as MobElementsLifeCycleMgr(), OnBoardSimEngine's service objects
	// is still activated. auto delete these objects. 
	// Delete all service objects in simEngine.
	//DeletionManager::DeleteAll();

	MobElementsDiagnosis()->ReleaseInstance();
	simEngineConfig()->ReleaseInstance();
	DiagnoseDelivery()->ReleaseInstance();

}
void CARCportEngine::ActivePassenger()
{
	PLACE_METHOD_TRACK_STRING();
	if(simEngineConfig()->isSimTerminalModeAlone())
		entryEvent.initList (getPaxLog(), getTerminal()->m_pSimParam->GetPaxCount()) ;
	else
	{
		CPaxGenerator paxgenertator(this) ;
		std::vector<Passenger*> _paxlist ;
		paxgenertator.GenerateNodelayMobileElement(ElapsedTime(),_paxlist,entryEvent);
	}

}

// generate the logs for the all Elevators
void CARCportEngine::GenerateElevatorLogEntry( )
{
	PLACE_METHOD_TRACK_STRING();
	int iProcCount = getTerminal()->procList->getProcessorCount();
	int nIndex = 0;
	CTermPlanDoc* pDoc = (CTermPlanDoc*) ((CMDIChildWnd *)((CMDIFrameWnd*)AfxGetApp()->m_pMainWnd)->GetActiveFrame())->GetActiveDocument();
	assert( pDoc );
	long nFirstInTime = LONG_MAX;
	for(long j=0; j<getPaxLog()->getCount(); j++) 
	{
		MobLogEntry ple;
		getPaxLog()->getItem(ple,j);

		//MobDescStructEx pdsX;
		const MobDescStruct& pds = ple.GetMobDesc();
		if( pds.startTime < nFirstInTime )
			nFirstInTime = pds.startTime;
	}

	for( int i=0; i<iProcCount; ++i )
	{
		Processor* pProc = getTerminal()->procList->getProcessor( i );
		if( pProc->getProcessorType() == Elevator )
		{
			ElevatorProc* pElevator = (ElevatorProc*)pProc;
			pElevator->GetFloorHeight().clear();

			//	pDoc->GetFloorByMode( EnvMode_Terminal ).
			CString sElevatorName = pElevator->getID()->GetIDString();
			int iLiftCount = pElevator->GetNumberOfLift();	

			CSimGeneralPara* pSim = getTerminal()->m_pSimParam->GetSimPara();

			for( int j=0; j<iLiftCount; ++j )
			{
				CString sLift;
				sLift.Format("%d", j );
				CElevatorLogEntry elevatorEntry;
				elevatorEntry.SetEventLog( getTerminal()->m_pElevatorEventLog );

				elevatorEntry.SetID( sElevatorName + sLift );
				//				elevatorEntry.SetStartTime( pSim->GetStartTime() );// ElapsedTime(0l)
				static ElapsedTime tStartTime;
				tStartTime.setPrecisely(nFirstInTime);
				elevatorEntry.SetStartTime(tStartTime);
				elevatorEntry.SetEndTime( pSim->GetEndTime() );//ElapsedTime(24l*60l*60l -1l )
				elevatorEntry.SetOrientation( pElevator->GetOrientation() );

				elevatorEntry.SetLiftLength(pElevator->GetElevatorAreaLength() );
				elevatorEntry.SetLiftWidth( pElevator->GetElevatorAreaWidth() );
				elevatorEntry.SetIndex( nIndex++ );
				pElevator->GetDataOfLift( j ).SetLogOfLift( elevatorEntry );
				getTerminal()->m_pElevatorLog->addItem( elevatorEntry );
			}
		}
	}





	nIndex = 0;
	for( int i=0; i<iProcCount; ++i )
	{
		Processor* pProc = getTerminal()->procList->getProcessor( i );		
		if(pProc && pProc->getProcessorType() == BridgeConnectorProc)
		{

			BridgeConnector* pBridgeConnector = (BridgeConnector*)pProc;

			//	pDoc->GetFloorByMode( EnvMode_Terminal ).
			ProcessorID sbridgeId = *pBridgeConnector->getID();
			//int iBridgeCount = pBridgeConnector->GetConnectPointCount();	

			CSimGeneralPara* pSim = getTerminal()->m_pSimParam->GetSimPara();

			//for( int j=0; j<iBridgeCount; ++j )
			{
				//CString sLift;
				//sLift.Format("%d", j );
				CBridgeConnectorLogEntry bridgeConnectorEntry;
				bridgeConnectorEntry.SetEventLog( getTerminal()->m_pBridgeConnectorEventLog );

				bridgeConnectorEntry.SetID( sbridgeId  );
				//				elevatorEntry.SetStartTime( pSim->GetStartTime() );// ElapsedTime(0l)
				static ElapsedTime tStartTime;
				tStartTime.setPrecisely(nFirstInTime);
				bridgeConnectorEntry.SetStartTime(tStartTime);
				bridgeConnectorEntry.SetEndTime( pSim->GetEndTime() );//ElapsedTime(24l*60l*60l -1l )

				BridgeConnector::ConnectPoint bridgeConnectPoint = pBridgeConnector->m_connectPoint;//(j);

				bridgeConnectorEntry.SetLocation(bridgeConnectPoint.m_Location);
				bridgeConnectorEntry.SetDirFromTo(bridgeConnectPoint.m_dirFrom,bridgeConnectPoint.m_dirTo);

				bridgeConnectorEntry.SetWidth(bridgeConnectPoint.m_dWidth);
				bridgeConnectorEntry.SetLength(bridgeConnectPoint.m_dLength);

				bridgeConnectorEntry.SetIndex( nIndex++ );
				pBridgeConnector->m_Status.SetLogEntryOfBridge( bridgeConnectorEntry );
				getTerminal()->m_pBridgeConnectorLog->addItem( bridgeConnectorEntry );
			}

		}

	}

	nIndex = 0;
	for( int i=0; i<iProcCount; ++i )
	{
		Processor* pProc = getTerminal()->procList->getProcessor( i );		
		if(pProc && pProc->getProcessorType() == RetailProc)
		{
			RetailProcessor* pRetailProc = (RetailProcessor*)pProc;
			CString sRetailName = pRetailProc->getID()->GetIDString();

			CRetailLogEntry retailEntry;
			retailEntry.SetEventLog( getTerminal()->m_pRetailEventLog );
			retailEntry.SetID(*pRetailProc->getID());
			retailEntry.SetIndex(nIndex++);
			retailEntry.SetProcIndex(pRetailProc->getIndex());
			retailEntry.SetProcessorType(pRetailProc->getProcessorType());
			retailEntry.SetSubType(pRetailProc->GetRetailType());

			pRetailProc->SetLogEntryOfRetail(retailEntry);
			getTerminal()->m_pRetailLog->addItem(retailEntry);
		}
	}

}
// generate the logs for the trains.
//##ModelId=3E2FAD070348
void CARCportEngine::GenerateTrains()
{
	PLACE_METHOD_TRACK_STRING();
	strcpy( m_csDisplayMsg, "Generating Trains..." );
	SetPercent( 7 );
	SendSimMessage( m_csDisplayMsg );


	const ALLCARSCHEDULE* pAllSchedule = getTerminal()->m_pAllCarSchedule->GetAllCarSchedule();
	int nSchdCount = pAllSchedule->size();
	int nTrainID = 0;
	for( int i=0; i<nSchdCount; i++ )
	{
		CSingleCarSchedule* pCarSchedule = pAllSchedule->at( i );	

		int nCarCount = pCarSchedule->GetStationLayout()->GetCarCount();
		DistanceUnit dCarLength =  pCarSchedule->GetStationLayout()->GetCarLength();
		DistanceUnit dCarWidth = pCarSchedule->GetStationLayout()->GetCarWidth();

		// add item into m_pTrainLog

		int nTrains = pCarSchedule->GetTrainNumber();
		for( int m=0; m<nTrains; m++ )
		{
			std::vector<CViehcleMovePath> vMovePath;
			pCarSchedule->GetAllLocationAndTime( m, vMovePath );	

			CTrainLogEntry logEntry;

			//			logEntry.SetEventLog( m_pTrainEventLog );

			CString csTrainId;
			csTrainId.Format( "%s-%02d", pCarSchedule->GetCarName(), m+1 );
			logEntry.SetId( csTrainId );
			// let's add head time offset
			logEntry.SetStartTime( pCarSchedule->GetCarStartTime() + pCarSchedule->GetHeadWayTime()*(long)m);
			logEntry.SetEndTime( pCarSchedule->GetActualEndTime() + pCarSchedule->GetHeadWayTime()*(long)m);
			// 			logEntry.SetStartTime( pCarSchedule->GetCarStartTime() );
			// 			logEntry.SetEndTime( pCarSchedule->GetActualEndTime() );
			logEntry.SetCarNumber( nCarCount );
			logEntry.SetCarLength( dCarLength );
			logEntry.SetCarWidth( dCarWidth );
			logEntry.SetSpeed( pCarSchedule->GetNormalSpeed() );
			logEntry.SetAccelerateSpeed( pCarSchedule->GetAccelerateSpeed() );
			logEntry.SetDecelerateSpeed( pCarSchedule->GetDecelerateSpeed() );
			logEntry.SetCircleTime( pCarSchedule->GetCircleTime() );
			logEntry.SetTurnAroundTime( pCarSchedule->GetTurnAroundTime()*60l );
			logEntry.SetHeadWayTime( pCarSchedule->GetHeadWayTime() );
			logEntry.SetIndexNum( i );
			logEntry.SetTrainID(nTrainID);
			nTrainID++;

			// add event into m_pTrainLog

			// 			int nSegCount = vMovePath.size();
			// 			for( int p=0; p<nSegCount; p++ )
			// 			{
			// 				CViehcleMovePath movePath = vMovePath[p];
			// 				int nPathCount = movePath.GetCount();
			// 				float fPrevHeading;
			// 				for( int z=0; z<nPathCount; z++ )
			// 				{
			// 					CStateTimePoint timePt = movePath.GetItem( z );
			// 					TrainEventStruct event;
			// 					event.time = timePt.GetTime();
			// 					event.x = (float)timePt.getX();
			// 					event.y = (float)timePt.getY();
			// 					event.z = (short)timePt.getZ();
			// 					if( z < nPathCount - 1 )
			// 					{
			// 						CStateTimePoint nextTimePt = movePath.GetItem( z+1 );
			// 						double dNewHeading = ( nextTimePt - timePt ).getHeading();
			// 						event.heading = (float)dNewHeading;
			// 						fPrevHeading = event.heading;
			// 					}
			// 					else
			// 					{
			// 						event.heading = fPrevHeading;
			// 					}
			// 					event.state = timePt.GetState();
			// 					logEntry.addEvent( event );
			// 				}
			// 			}

			// 			TrainEventStruct *log = NULL;
			// 			int nEventCount = logEntry.getCurrentCount();
			// 		    if( nEventCount > 0 )
			// 			{
			// 				logEntry.getLog( log );
			// 				logEntry.setEventList( log, nEventCount );		// this the only way to write the local event to the big list.
			// 				delete [] log;
			// 				log = NULL;
			// 			}
			getTerminal()->m_pTrainLog->addItem( logEntry );
		}
	}	
}


void CARCportEngine::createEchoFiles (const CString& _csProjPath)
{
	char filename[_MAX_PATH];

	PROJMANAGER->getEchoFileName ( _csProjPath, PaxLogEchoFile, filename);
	getPaxLog()->echoLogFile (filename, _csProjPath, getTerminal() );

	PROJMANAGER->getEchoFileName ( _csProjPath, FlightLogEchoFile, filename);
	getFlightLog()->echoLogFile (filename,_csProjPath, getTerminal());

	PROJMANAGER->getEchoFileName ( _csProjPath, ProcLogEchoFile, filename);
	getProcLog()->echoLogFile (filename, _csProjPath, getTerminal());

	PROJMANAGER->getEchoFileName ( _csProjPath, BagLogEchoFile, filename);
	getBagLog()->echoLogFile (filename, _csProjPath, getTerminal());

	PROJMANAGER->getEchoFileName ( _csProjPath, TrainLogEchoFile, filename);
	getTrainLog()->echoLogFile (filename, _csProjPath, getTerminal());

	PROJMANAGER->getEchoFileName ( _csProjPath, StringDictEchoFile, filename);
	getInStrDict()->storeDictionary (filename);
}


//void PrintMobType(MobLogEntry& entry, char *p_str, InputTerminal* pInterm)
//{
//	{
//		MobDescStruct& item = entry.GetMobDesc();
//		strcat (p_str, "(");
//
//		strcat (p_str, pInterm->inStrDict->getString (item.intrinsic));
//		for (int i = 0; i < MAX_PAX_TYPES && item.paxType[i] != EMPTY_ID; i++)
//		{
//			strcat (p_str, "-");
//			strcat (p_str, pInterm->inStrDict->getString (item.paxType[i]));
//		}
//		strcat (p_str, ")");
//		////////////////////////
//		strcat (p_str, "{");
//
//		int iMobileType = item.mobileType;
//		CString strType = pInterm->m_pMobElemTypeStrDB->GetString( iMobileType );
//		strcat( p_str, (LPCSTR) strType );	
//
//		strcat (p_str, "}");
//	}
//}


long CARCportEngine::generatePassengers (FlightSchedule* _pfs)
{
	PLACE_METHOD_TRACK_STRING();
	//CFileOutput outfile("D:\\debugpax.log");
	//outfile.Log(rand()).Line();

	strcpy( m_csDisplayMsg, "Generating Passengers..." );
	SetPercent( 15 );
	SendSimMessage( m_csDisplayMsg );

	// GENERATE ALL MOBILE ELEMENTS
	// start by generating all arrivals and xfer pax
	// then fill empty space on departing flights with departing pax
	//int numFlights = flightSchedule->getCount();
	int numFlights = _pfs->getCount();
	CProgressBar bar("Generate Passengers for arrival",100,numFlights,FALSE);
	//reset the flag that the data has been initialize for engine
	getTerminal()->paxDistList->ClearEngineInitializeFlag();

	for (int i = 0; i < numFlights; i++)
	{
		generatePaxOneFlight (i, 'A', _pfs);
		bar.StepIt();
	}
	bar.SetText("Generate Passengers for departure");
	bar.SetPos(0);
	for (int ii = 0; ii < numFlights; ii++)
	{
		generatePaxOneFlight (ii, 'D', _pfs);
		bar.StepIt();	
	}

	//bar.SetPos(0);	
	//// SORT MOBILE ELEMENTS
	//// all pax and bags must then be sorted by their entry time
	//sprintf( m_csDisplayMsg, "Sorting %d Mobile Elements...", paxLog->getCount() );
	//SetPercent( 23 );

	//SendSimMessage( m_csDisplayMsg );
	//paxLog->sortPassengers(bar);

	//sprintf( m_csDisplayMsg, "Sorting %d Bags...", bagLog->getCount() );
	//SetPercent( 24 );
	//SendSimMessage( m_csDisplayMsg );
	//bagLog->sortBags();
	//SetPercent( 25 );
	/*if( PaxLog* pPaxLog = getPaxLog())
	{
		MobLogEntry logentry;
		for(int i=0;i<pPaxLog->getCount();++i)
		{
			pPaxLog->getItem(logentry,i);
			char sbuffer[512]="\0";
			
			PrintMobType(logentry,sbuffer,getTerminal());
			
			outfile.Log(logentry.getID()).Log(sbuffer).Line();		
		}
	}*/
	return getPaxLog()->getCount();
}

//long CARCportEngine::generateMobileAgents(FlightSchedule* _pfs, std::vector<int> _OnBoardFlightIDs)
//{
//	strcpy( m_csDisplayMsg, "Generating OnBoard Agents..." );
//	SetPercent( 15 );
//	SendSimMessage( m_csDisplayMsg );
//
//	int numFlights = _pfs->getCount();
//	CProgressBar bar("Generate Agents for arrival",100,numFlights,FALSE);
//
//	for (int i = 0; i < numFlights; i++)
//	{
//		std::vector<int>::iterator iter = 
//			std::find( _OnBoardFlightIDs.begin(), _OnBoardFlightIDs.end(), i);
//		if(iter == _OnBoardFlightIDs.end())// not onboard candidate flight.
//			continue;
//
//		generatePaxOneFlight (i, 'A', _pfs);
//		bar.StepIt();
//	}
//
//	bar.SetText("Generate Agents for departure");
//	bar.SetPos(0);
//	for (i = 0; i < numFlights; i++)
//	{
//		std::vector<int>::iterator iter = 
//			std::find( _OnBoardFlightIDs.begin(), _OnBoardFlightIDs.end(), i);
//
//		if(iter == _OnBoardFlightIDs.end())// not onboard candidate flight.
//			continue;
//
//		generatePaxOneFlight (i, 'D', _pfs);
//		bar.StepIt();	
//	}
//	bar.SetPos(0);	
//
//	// all pax and bags must then be sorted by their entry time
//	sprintf( m_csDisplayMsg, "Sorting %d OnBoard Agents...", getPaxLog()->getCount() );
//	SetPercent( 23 );
//
//	SendSimMessage( m_csDisplayMsg );
//	getPaxLog()->sortPassengers(bar);
//
//	SetPercent( 25 );
//
//	return getPaxLog()->getCount();
//
//}
//
// It generates all passengers of p_mode type for the flight p_ndx.
// It uses PassengerDescription tree for the specific flight type to determine breakdown of passenger types.
//##ModelId=3E2FAD0703B5
void CARCportEngine::generatePaxOneFlight (int p_ndx, char p_mode, const FlightSchedule* _pfs)
{
	PLACE_METHOD_TRACK_STRING();

	//Flight* aFlight = flightSchedule->getItem (p_ndx);
	Flight* aFlight = _pfs->getItem (p_ndx);

	if ((aFlight->getFlightMode() != 'T' && aFlight->getFlightMode() != p_mode) ||
		!aFlight->getSpace (p_mode))
		return;

	//clear the checked baggage count information before generating passengers and non-passengers
	if (p_mode == 'A')
	{
		aFlight->setArrBagCount(0);
	}
	else
	{
		aFlight->setDepBagCount(0);
	}
	//aFlight->ResetBaggageCount();

	FlightConstraint flightType = aFlight->getType (p_mode);
	flightType.SetAirportDB(getTerminal()->m_pAirportDB);

	// Determine type distribution tree
	const PaxTypeDistribution *breakdown = NULL;
	breakdown = (PaxTypeDistribution *)(getTerminal()->paxDistList->lookup (flightType));
	if (!breakdown)
	{
		CString sAirID;//[30];
		flightType.screenPrint (sAirID, 0, 30);
		throw new TwoStringError ("No passenger type distribution for ",
			sAirID.GetBuffer(0));
	}

	// Generate breakdown of specific passenger types
	// (256 is max unique types)   //matt set to 256000



	int leafCount = getTerminal()->inTypeList->getLeafCount();
	int * typeCounts = new int[leafCount+1];   //matt

	for (int i = 0; i < leafCount; i++)
		typeCounts[i] = 0;

	// randomly determine how many of each unique type of pax are on this
	// particular flight
	int nSpaceCount = aFlight->getSpace(p_mode);
	for (int ii = 0; ii < aFlight->getSpace (p_mode); ii++)
		typeCounts[(int)breakdown->getEngineRandomValue()]++;


	for( int i3=0; i3<leafCount; ++i3 )
	{
		m_vNonPaxCountByType[ 0 ] += typeCounts[i3];
	}


	int branches[MAX_PAX_TYPES];
	CMobileElemConstraint  paxType;
	paxType.SetInputTerminal( getTerminal() );
	(FlightConstraint &)paxType = flightType;
	paxType.SetMobileElementType(enum_MobileElementType_PAX);
	ProcessorID _standGate;
	_standGate.SetStrDict(getInStrDict());
	// generate passengers for each leaf in tree
	for (int i4 = 0; i4 < leafCount; i4++)
	{
		if(!typeCounts[i4])
			continue;

		getPaxTypeList()->getBranchList (i4, branches);
		paxType.setUserType (branches);
		if (p_mode == 'A' && aFlight->isArriving())
		{
			_standGate.setID(aFlight->getArrStand().GetIDString());
			if (aFlight->isTurnaround() && typeCounts[i4])
			{
				int nArrTransit = generateXiting (paxType, p_ndx, typeCounts[i4],_standGate, _pfs);
				int nCount = aFlight->getArrTransit();
				aFlight->setArrTransit(nCount + nArrTransit) ;
				typeCounts[i4] -= nArrTransit;
			}
			if (typeCounts[i4])
			{
				int nArrTransfer = generateXferring (paxType, p_ndx, typeCounts[i4], _standGate, _pfs);
				int nCount = aFlight->getArrTransfer();
				aFlight->setArrTransfer(nCount + nArrTransfer) ;
				typeCounts[i4] -= nArrTransfer;
			}
			if (typeCounts[i4])
				generateArrivals (paxType, p_ndx, typeCounts[i4], _standGate, _pfs);
		}
		else if (aFlight->isDeparting() && typeCounts[i4])
		{
			_standGate.setID(aFlight->getDepStand().GetIDString());
			generateDepartures (paxType, p_ndx, typeCounts[i4],_standGate , _pfs);
		}				
	}
	delete [] typeCounts;
}

// Percentages of transiting passengers are used for each generating. 
// The generated passengers are attached to paxLog.
// Returns number of generated transiting passengers.
int CARCportEngine::generateXiting ( CMobileElemConstraint  p_type, int p_ndx, int p_count, const ProcessorID& _standGate , const FlightSchedule* _pfs)
{
	int iNoCount = 0;

	if (!getTerminal()->m_pSimParam->GetHubbingFlag())
		return 0;

	Flight *aFlight = _pfs->getItem (p_ndx);
	if (aFlight->getDepTime() <= aFlight->getArrTime())
		return 0;

	p_type.makeArrival();
	float percent = getTerminal()->m_pHubbing->getTransitPercent (p_type) / (float)100.0;
	p_type.makeTransiting();

	int transitCount = 0;
	for (int i = 0; i < p_count; i++)
		if (((double)rand()) / ((double)RAND_MAX) >= 1-percent)
			transitCount++;

	if( percent == 0.0 )
		transitCount = 0;

	//int transitCount = p_count-1;
	if (transitCount > aFlight->getSpace ('D'))
		transitCount = aFlight->getSpace ('D');

	MobLogEntry elem;
	int count = 0;
	for (; count < transitCount; count += elem.getGroupSize())
	{
		int nRet =takeBulk( p_type, elem, transitCount - count,aFlight->getArrTime() );
		if(nRet<0 )//wan't to takeBulk;
		{
			initElement (p_type, elem, transitCount - count);
			elem.setEntryTime (getLeadTime (p_type, aFlight->getArrTime(), _standGate ));
		}
		else if(nRet==0) //can't to takeBulk;
		{
			initElement (p_type, elem, transitCount - count);
			elem.setEntryTime (getLeadTime (p_type, aFlight->getArrTime(), _standGate ));
			//continue;
		}
		// set pointer to flight in flight list
		elem.setArrFlight (p_ndx);
		elem.setDepFlight (p_ndx);
		int nGroupSize = elem.getGroupSize();
		aFlight->addDepPax ( nGroupSize );
		int nCount = aFlight->getDepTransit();
		aFlight->setArrTransit(nCount + nGroupSize);

		//set the speed,temporality not know the number of the baggages
		elem.setSpeed( (float)getSpeed( p_type ) );	

		//////////////////////////////////////////////////add Nopax

		std::vector<BOOL> vNPFlag = getTerminal()->m_pSimParam->GetNoPaxDetailFlag();
		int iVisitorFlag = getTerminal()->m_pSimParam->GetVisitorsFlag();
		MobLogEntry* m_pNopax = new MobLogEntry[getTerminal()->m_pMobElemTypeStrDB->GetCount()];

		//generate all nopax
		int iMobileCount = getTerminal()->m_pMobElemTypeStrDB->GetCount();
		long lCurID = elem.getID() + elem.getGroupSize();
		elem.ReleaseCarryon();
		for( int i=1; i<iMobileCount; i++ )
		{
			bool bGenFlag = TRUE;
			if( i <= NOPAX_COUNT && !vNPFlag[i-1] || !iVisitorFlag )
				bGenFlag = FALSE;
			int iNopaxCount = GenerateNopaxs( p_type, &m_pNopax[i], &elem, i, lCurID, bGenFlag ,_standGate, _pfs);
			elem.SetCarryonCount(iNopaxCount);
			lCurID += iNopaxCount;
			iNoCount += iNopaxCount;
		}

		//reset the speed,according to the number of the baggages
		int nCountBags = elem.getBagCount();
		if (nCountBags > 0)
		{
			elem.setSpeed( (float)getSpeed( p_type ) );		  
		}
		else
			elem.setSpeed( (float)getSpeedWithoutBags(p_type) );

		//save pax and nopax log to paxlog
		for(int ii=0; ii< elem.getGroupSize(); ii++ )
		{
			getPaxLog()->addItem(elem);
			elem.incPax();
			elem.ReleaseCarryon();
		}

		for( int i3=1; i3<iMobileCount; i3++ )
		{
			for( int j =0; j < m_pNopax[i3].getGroupSize(); j++ )
			{
				if(m_pNopax[i3].GetMobileType() == 2 && m_pNopax[i3].getOwnStart())
				{
					ElapsedTime gateTime = (_pfs->getItem( elem.getFlightIndex()))->getArrTime();
					CMobileElemConstraint p_notype(p_type);
					p_notype.SetTypeIndex(2);
					p_notype.SetMobileElementType(enum_MobileElementType_NONPAX);
					m_pNopax[i3].setEntryTime( getLeadTime( p_notype, gateTime, _standGate ));
					m_pNopax[i3].setGroupMember(0);
				}
				getPaxLog()->addItem(m_pNopax[i3]);
				m_pNopax[i3].incPax();
				elem.incPax();
			}
		}
		delete []m_pNopax;
		///////////////////////////////////////////////////////////////////////
		//	  for (int j = 0; j < nGroupSize; j++)
		//		  {
		//				paxLog->addItem (elem);
		//				elem.incPax();
		//		  }

	}
	return count;
	//return count + iNoCount;
}

// Percentages of transferring passengers are used for each generating. 
// The generated passengers are attached to paxLog.
// Returns number of generated transferring passengers.
//##ModelId=3E2FAD0703C7
int CARCportEngine::generateXferring (CMobileElemConstraint p_type, int p_ndx, int p_count, const ProcessorID& _standGate,
								const FlightSchedule* _pfs)
{
	int iNoCount=0;
	if (!getTerminal()->m_pSimParam->GetHubbingFlag())
		return 0;

	//Flight *depFlight, *aFlight = flightSchedule->getItem (p_ndx);
	Flight *depFlight, *aFlight = _pfs->getItem (p_ndx);
	p_type.makeArrival();
	float percent = getTerminal()->m_pHubbing->getTransferPercent (p_type) / (float)100.0;
	//Jan : P(A|B) = P(AB)/P(B); A : X=transfer; B : X!=transit
	float tmpPercent = getTerminal()->m_pHubbing->getTransitPercent (p_type) / (float)100.0;
	percent = percent / (1-tmpPercent);

	int transferCount = 0;
	int tmpCount = int(p_count * tmpPercent);
	p_count -= tmpCount;
	for (int i = 0; i < p_count; i++)
		if (((double)rand()) / ((double)RAND_MAX) >= 1-percent)
			transferCount++;
	//int transferCount = p_count-1;

	MobLogEntry elem;
	int count = 0;
	for (; count < transferCount; count += elem.getGroupSize())
	{
		p_type.makeArrival();
		depFlight = getTerminal()->m_pHubbing->getDepartingFlight ( p_type, p_ndx, &m_simFlightSchedule );
		if (!depFlight)     // no departing flights avail
			return count;

		p_type.makeTransferring();
		int nMaxPaxGenerated = depFlight->getSpace( 'D' );
		nMaxPaxGenerated = min( nMaxPaxGenerated, transferCount - count );
		int nRet =takeBulk( p_type, elem, transferCount - count,aFlight->getArrTime());
		if(nRet<0)
		{
			initElement (p_type, elem, nMaxPaxGenerated);
			// set the entry time
			elem.setEntryTime (getLeadTime (p_type, aFlight->getArrTime(), _standGate));
		}
		else if(nRet==0)
		{
			initElement (p_type, elem, nMaxPaxGenerated );
			// set the entry time
			elem.setEntryTime (getLeadTime (p_type, aFlight->getArrTime(), _standGate));
		}

		// set pointer to flight in flight list
		elem.setArrFlight (p_ndx);
		//elem.setDepFlight (this->flightSchedule->findItem (depFlight));
		elem.setDepFlight (_pfs->findItem (depFlight));

		int nCountBag = elem.getBagCount();
		if (nCountBag > 0)
		{
			elem.setSpeed( (float)getSpeed( p_type ) );		
		}
		else
			elem.setSpeed( (float)getSpeedWithoutBags(p_type) );		  

		//		  elem.setVisitor (-1);

		int nGroupSize = elem.getGroupSize();
		depFlight->addDepPax ( nGroupSize );
		int nCount = depFlight->getDepTransfer();
		depFlight->setDepTransfer(nCount + nGroupSize) ;



		//////////////////////////////////////////////////add Nopax

		std::vector<BOOL> vNPFlag = getTerminal()->m_pSimParam->GetNoPaxDetailFlag();
		int iVisitorFlag = getTerminal()->m_pSimParam->GetVisitorsFlag();
		MobLogEntry* m_pNopax = new MobLogEntry[getTerminal()->m_pMobElemTypeStrDB->GetCount()];

		//generate all nopax
		int iMobileCount = getTerminal()->m_pMobElemTypeStrDB->GetCount();
		long lCurID = elem.getID() + elem.getGroupSize();
		//if(lCurID == 0 || lCurID == 1 || lCurID == 2)
		//{
		// CString str = _T("Test");
		//}

		//need to set departure flight id to be -1,
		//then nopax can set the entry time to be based on arrived flight
		int nDepFlightID = elem.getDepFlight();
		elem.setDepFlight(-1);

		elem.ReleaseCarryon();
		for( int i=1; i<iMobileCount; i++ )
		{
			bool bGenFlag = TRUE;
			if( i <= NOPAX_COUNT && !vNPFlag[i-1] || !iVisitorFlag )
				bGenFlag = FALSE;

			// need to update the p_type
			CMobileElemConstraint xferType = p_type;			
			xferType.initFlightType( depFlight->getLogEntry(), 'D' );
			xferType.initOtherFlightType( aFlight->getLogEntry() );

			int iNopaxCount = GenerateNopaxs( xferType, &m_pNopax[i], &elem, i, lCurID, bGenFlag ,_standGate, _pfs);
			elem.SetCarryonCount(iNopaxCount);
			m_pNopax[i].setDepFlight(nDepFlightID);

			lCurID += iNopaxCount;
			iNoCount += iNopaxCount;
		}
		elem.setDepFlight(nDepFlightID);

		//save pax and nopax log to paxlog
		for(int  ii=0; ii< elem.getGroupSize(); ii++ )
		{
			getPaxLog()->addItem(elem);
			elem.incPax();
			elem.ReleaseCarryon();
		}

		elem.setDepFlight(-1);
		for(int i3=1; i3<iMobileCount; i3++ )
		{
			// 			for( int j =0; j < m_pNopax[i].getGroupSize(); j++ )
			// 			{
			// 				paxLog->addItem(m_pNopax[i]);
			// 				m_pNopax[i].incPax();
			// 				elem.incPax();
			// 			}
			for( int j =0; j < m_pNopax[i3].getGroupSize(); j++ )
			{
				if(m_pNopax[i3].GetMobileType() == 2 && m_pNopax[i3].getOwnStart())
				{
					CMobileElemConstraint xferType = p_type;			
					xferType.initFlightType( depFlight->getLogEntry(), 'D' );
					xferType.initOtherFlightType( aFlight->getLogEntry() );
					ElapsedTime gateTime = (_pfs->getItem( elem.getFlightIndex()))->getArrTime();
					//	CMobileElemConstraint p_notype(p_type);
					xferType.SetTypeIndex(2);
					xferType.SetMobileElementType(enum_MobileElementType_NONPAX);
					m_pNopax[i3].setEntryTime( getLeadTime( xferType, gateTime, _standGate ));
					m_pNopax[i3].setGroupMember(0);
				}
				getPaxLog()->addItem(m_pNopax[i3]);
				m_pNopax[i3].incPax();
				elem.incPax();
			}
		}
		elem.setDepFlight(nDepFlightID);
		delete []m_pNopax;
		///////////////////////////////////////////////////////////////////////
		//
		//		  for (int j = 0; j < nGroupSize; j++)
		//		  {
		//				paxLog->addItem (elem);
		//				elem.incPax();
		//		  }

	}

	return count;
	//return count +iNoCount;
}

//It generates all arriving pax for the flight p_ndx.
//Greeters are generated for each arriving passenger.
void CARCportEngine::generateArrivals ( CMobileElemConstraint p_type, int p_ndx, int p_count, const ProcessorID& _standGate, 
								 const FlightSchedule* _pfs)
{
	PLACE_METHOD_TRACK_STRING();
	//Flight *aFlight = flightSchedule->getItem (p_ndx);
	Flight *aFlight = _pfs->getItem (p_ndx);
	p_type.makeArrival();

	MobLogEntry elem;
	//for all nopax log
	MobLogEntry* m_pNopax = new MobLogEntry[getTerminal()->m_pMobElemTypeStrDB->GetCount()];
	//MobLogEntry visitor;
	for (int count = 0; count < p_count; count += elem.getGroupSize())
	{
		int nRet=takeBulk(p_type, elem, p_count - count,aFlight->getArrTime());
		if(nRet<0)
		{
			initElement (p_type, elem, p_count - count);
			// set the entry time
			elem.setEntryTime (getLeadTime (p_type, aFlight->getArrTime(), _standGate));
		}
		else if(nRet==0)
		{
			initElement (p_type, elem, p_count - count);
			// set the entry time
			elem.setEntryTime (getLeadTime (p_type, aFlight->getArrTime(), _standGate));
		}

		// set pointer to flight in flight list
		elem.setArrFlight (p_ndx);

		// generate all bags
		elem.setBagCount (createBags (p_type, elem, _pfs));

		//reset cart count
		elem.setCartCount (getCarts (p_type, elem.getGroupSize(), elem.getBagCount())); //Jan98

		int nCountBag = elem.getBagCount();
		if (nCountBag > 0)
		{
			elem.setSpeed( (float)getSpeed( p_type ) );
		}
		else
			elem.setSpeed( (float)getSpeedWithoutBags(p_type) );

		//		  elem.setVisitor (-1);
		elem.SetmobileType( 0 );



		std::vector<BOOL> vNPFlag = getTerminal()->m_pSimParam->GetNoPaxDetailFlag();
		int iVisitorFlag = getTerminal()->m_pSimParam->GetVisitorsFlag();

		//generate all nopax
		int iMobileCount = getTerminal()->m_pMobElemTypeStrDB->GetCount();
		long lCurID = elem.getID() + elem.getGroupSize();
		elem.ReleaseCarryon();
		for( int i=1; i<iMobileCount; i++ )
		{
			bool bGenFlag = TRUE;
			if( i <= NOPAX_COUNT && !vNPFlag[i-1] || !iVisitorFlag )
				bGenFlag = FALSE;
			int iNopaxCount = GenerateNopaxs( p_type, &m_pNopax[i], &elem, i, lCurID, bGenFlag , _standGate, _pfs);
			if(i == 2)//Checked Baggage
			{
				aFlight->setArrBagCount(aFlight->getArrBagCount() + iNopaxCount);
			}

			elem.SetCarryonCount(iNopaxCount);
			lCurID += iNopaxCount;
		}

		//save pax log to paxlog
		for(int ii=0; ii< elem.getGroupSize(); ii++)
		{
			getPaxLog()->addItem(elem);
			elem.incPax();
			elem.ReleaseCarryon();
		}

		for(int i3=1; i3<iMobileCount; i3++ )
		{
			for( int j =0; j < m_pNopax[i3].getGroupSize(); j++ )
			{
				m_pNopax[i3].setArrFlight(p_ndx);
				if(m_pNopax[i3].GetMobileType() == 2 && m_pNopax[i3].getOwnStart())
				{
					ElapsedTime gateTime = (_pfs->getItem( elem.getFlightIndex()))->getArrTime();
					CMobileElemConstraint p_notype(p_type);
					p_notype.SetTypeIndex(2);
					p_notype.SetMobileElementType(enum_MobileElementType_NONPAX);
					m_pNopax[i3].setEntryTime( getLeadTime( p_notype, gateTime, _standGate ));
					m_pNopax[i3].setGroupMember(0);
				}
				getPaxLog()->addItem(m_pNopax[i3]);
				m_pNopax[i3].incPax();
				elem.incPax();
			}
		}
	}

	delete []m_pNopax;
}


//It generates all departing pax for the flight p_ndx.
//Senders are generated for each departing passenger.
void CARCportEngine::generateDepartures (CMobileElemConstraint p_type,
								   int p_ndx, int p_count, const ProcessorID& _standGate, const FlightSchedule* _pfs)
{

	//Flight* aFlight = flightSchedule->getItem (p_ndx);
	Flight* aFlight = _pfs->getItem (p_ndx);
	p_type.makeDeparture();

	MobLogEntry elem;
	//for all nopax log
	MobLogEntry* m_pNopax = new MobLogEntry[getTerminal()->m_pMobElemTypeStrDB->GetCount()];
	//MobLogEntry visitor;
	for (int count = 0; count < p_count; count += elem.getGroupSize())
	{
		int nRet = takeBulk( p_type, elem, p_count - count,aFlight->getDepTime());
		if(nRet<0)
		{
			initElement (p_type, elem, p_count - count);
			// set the entry time
			elem.setEntryTime (getLeadTime (p_type, aFlight->getDepTime(), _standGate));
		}
		else if(nRet==0)
		{
			
			initElement (p_type, elem, p_count - count);
			// set the entry time
			elem.setEntryTime (getLeadTime (p_type, aFlight->getDepTime(), _standGate));
			elem.setBulkCapacityFull(true);
		}
		aFlight->addDepPax (elem.getGroupSize());

		// set pointer to flight in flight list
		elem.setDepFlight (p_ndx);


		// generate all bags
		elem.setBagCount ((!getTerminal()->m_pSimParam->GetBaggageFlag())? 0:
		getBags (p_type, elem.getGroupSize()));

		//reset cart count
		elem.setCartCount (getCarts (p_type, elem.getGroupSize(), elem.getBagCount())); //Jan98

		//set the speed ,temporality not know the number of the baggages
		elem.setSpeed( (float)getSpeed( p_type ) );

		//		  elem.setVisitor (-1);
		elem.SetmobileType( 0 );


		std::vector<BOOL> vNPFlag = getTerminal()->m_pSimParam->GetNoPaxDetailFlag();
		int iVisitorFlag = getTerminal()->m_pSimParam->GetVisitorsFlag();

		//generate all nopax
		int iMobileCount = getTerminal()->m_pMobElemTypeStrDB->GetCount();
		long lCurID = elem.getID() + elem.getGroupSize();
		elem.ReleaseCarryon();
		for( int i=1; i<iMobileCount; i++ )
		{
			bool bGenFlag = TRUE;
			if( i <= NOPAX_COUNT && !vNPFlag[i-1] || !iVisitorFlag )
				bGenFlag = FALSE;
			int iNopaxCount = GenerateNopaxs( p_type, &m_pNopax[i], &elem, i, lCurID, bGenFlag ,_standGate, _pfs);
			if(i == 2)//Checked Baggage
			{
				aFlight->setDepBagCount(aFlight->getDepBagCount() + iNopaxCount);
			}

			elem.SetCarryonCount(iNopaxCount);
			lCurID += iNopaxCount;
		}

		//reset the speed according to the number of the baggages
		int nCountBag = elem.getBagCount();
		if (nCountBag > 0)
		{
			elem.setSpeed( (float)getSpeed( p_type ) );
		}
		else
			elem.setSpeed( (float)getSpeedWithoutBags(p_type));

		//save pax and nopax log to paxlog
		for( int ii=0; ii< elem.getGroupSize(); ii++ )
		{
			getPaxLog()->addItem(elem);
			elem.incPax();
			elem.ReleaseCarryon();
		}

		for(int i3=1; i3<iMobileCount; i3++ )
		{
			for( int j =0; j < m_pNopax[i3].getGroupSize(); j++ )
			{
				getPaxLog()->addItem(m_pNopax[i3]);
				m_pNopax[i3].incPax();
				elem.incPax();
			}
		}
	}
	delete []m_pNopax;
}
// if a bulk can't service for passenger (p_type), return -1; but can't 
int CARCportEngine::takeBulk(CMobileElemConstraint p_type, MobLogEntry& p_elem, int p_max,ElapsedTime& flyTime)
{
	int nBulk=getTerminal()->m_pPaxBulkInfo->findFitBulk(p_type);
	//can't find a proper bulk
	if(nBulk == -1) 
		return -1;

	// set group size, limited to number of passengers of that p_type
	p_elem.setGroupSize ( getGroupSize (p_type, p_max));
	p_elem.setGroupMember (0);
	ElapsedTime time;

	long nfreqIndex;
	int nIndex = getTerminal()->m_pPaxBulkInfo->findValuedBulk(time,nfreqIndex, p_elem.getGroupSize(),p_type,flyTime);
	if(nIndex == -1)
		return 0;//passenger would not be generated

	p_elem.setBulkIndex(nBulk);
	p_elem.setBulkFreqIndex(nfreqIndex);

	//CPaxBulk& aBulk = m_pPaxBulkInfo->m_vectBulkInfo[nIndex];
	//all shuttle buses have leave, or remaining buses is too later.
	//if( aBulk.takeCurrentBulk( time , p_elem.getGroupSize(),flyTime)==false ) return 0;

	// set identification number
	p_elem.setID (getPaxLog()->getCount());

	// set the entry time
	p_elem.setEntryTime ( time );


	// set passenger type
	p_elem.setIntrinsic (p_type.getIntrinsic());

	int types[MAX_PAX_TYPES];
	p_type.getUserType (types);
	p_elem.setPaxType (types);

	// generate all carts
	// carts cannot exceed number of bags
	p_elem.setCartCount (getCarts (p_type, p_elem.getGroupSize(), p_elem.getBagCount()));

	return 1;
}

// It initializes data common to all intrinsic types.
// Group size is limited to number of passengers of that p_type.
// Carts are generated for passenger.
//##ModelId=3E2FAD0703B8
void CARCportEngine::initElement (CMobileElemConstraint p_type, MobLogEntry& p_elem, int p_max)
{
	// set identification number
	long lId = getPaxLog()->getCount();

	if(lId == 30)
	{
		int a = 0;
	}
	p_elem.setID ( lId );

	// set group size, limited to number of passengers of that p_type
	p_elem.setGroupSize (getGroupSize (p_type, p_max));
	p_elem.setGroupMember (0);

	// set passenger type
	p_elem.setIntrinsic (p_type.getIntrinsic());

	int types[MAX_PAX_TYPES];
	p_type.getUserType (types);
	p_elem.setPaxType (types);

	// generate all carts
	// carts cannot exceed number of bags
	p_elem.setCartCount (getCarts (p_type, p_elem.getGroupSize(), p_elem.getBagCount()));

}

int CARCportEngine::GenerateNopaxs( CMobileElemConstraint p_type, MobLogEntry* p_nopax, 
							 MobLogEntry *p_pax, int i_index, long _lCurID, bool _bGenFlog , 
							 const ProcessorID& _standGate, const FlightSchedule* _pfs)
{
	const ConstraintEntryEx* constEntry;
	constEntry = (const ConstraintEntryEx*)( getTerminal()->paxDataList->getNopaxData(i_index)->FindFitEntry( p_type ) );
	if (!constEntry)
	{
		char str1[1024];
		//char str2[256];
		CString str2;
		p_type.screenPrint( str2, 0, 256 );		
		sprintf( str1, "No %s count defined for passenger: %s", getTerminal()->m_pMobElemTypeStrDB->GetString( i_index ), str2.GetBuffer(0) );
		throw new StringError ( str1 );
	}
	const ProbabilityDistribution *visitorDist;
	visitorDist = constEntry->getValue();
	bool flag = constEntry->getFlag();	
	// if flag == true we shall generate visitorCount no_pax for each pax, so to all group pax, we shall generate visitorCount*p_pax.getGroupSize() no_pax
	// else we we shall generate visitorCount no_pax for group pax
	int visitorCount = (int)(visitorDist->getRandomValue());
	if( flag )
		visitorCount *= p_pax->getGroupSize();		

	if( !_bGenFlog || !visitorCount )
	{
		p_nopax->setGroupSize( 0 );

		// add by tutu in 2003-3-4
		if( i_index == 2 )// is check bags
		{
			p_pax->setBagCount( 0 );
		}
		return 0;
	}

	*p_nopax = *p_pax;							// copy the owner's pax_log.
	p_nopax->setEachPaxOwnNopax( flag );
	p_nopax->setID( _lCurID );					// set id next to the owner's id
	p_nopax->setOwner( p_pax->getID() );		// set owner id
	p_nopax->setGroupSize( visitorCount );		// set no_pax count
	p_nopax->setGroupMember( 0 );				// set index in the group

	//set passenger type
	p_nopax->SetmobileType( i_index );			// set which mobile type.

	int types[MAX_PAX_TYPES];		
	p_type.getUserType(types);					// get owner's user type.
	p_nopax->setPaxType(types);					// set nopax's log's pax type = owner's pax type

	//check if there is the sender.
	p_type.SetTypeIndex( i_index );
	p_type.SetMobileElementType(enum_MobileElementType_NONPAX);

	p_nopax->setSpeed((float)getSpeed(p_type));

	// add by tutu in 2003-3-4

	ASSERT( i_index > 0 && i_index < getTerminal()->m_pMobElemTypeStrDB->GetCount() );

	m_vNonPaxCountByType[ i_index ] += visitorCount;


	if( i_index == 2 )// is check bags
	{
		p_pax->setBagCount( visitorCount );
	}

	if( getTerminal()->m_pPassengerFlowDB->HasOwnStart( p_type))
	{
		// set entry time for nopax with start.
		//ElapsedTime gateTime = (flightSchedule->getItem( p_pax->getFlightIndex()))->getArrTime();
		ElapsedTime gateTime;
		if(p_type.isArrival() || p_type.isTurround())
		{
			gateTime = (_pfs->getItem( p_pax->getFlightIndex()))->getArrTime();
		}
		else
		{
			gateTime = (_pfs->getItem( p_pax->getFlightIndex()))->getDepTime();
		}

		p_nopax->setEntryTime( getLeadTime( p_type, gateTime, _standGate ));
	}
	else
	{
		p_nopax->setOwnStart( false );
	}

	return p_nopax->getGroupSize();
}

/*
int Terminal::GenerateVisitorters( CMobileElemConstraint _paxType, MobLogEntry* _pVisitorLogEntry, MobLogEntry* _pPaxLogEntry )
{
if( !getTerminal()->m_pSimParam->GetVisitorsFlag() )
return 0;
/*
const ProbabilityDistribution *visitorDist;
visitorDist = (paxDataList->getVisitors())->lookup (p_type);
if (!visitorDist)
{
char str[128];
p_type.WriteSyntaxString (str);
throw new TwoStringError ("no visitor distribution for ", str);
}

int visitorCount = (int)(visitorDist->getRandomValue());
if (!visitorCount)
return FALSE;
*/
/*
int nVisitorCount = 1;
*_pVisitorLogEntry = *_pPaxLogEntry;	// copy the owner's paxlog.
_pVisitorLogEntry->setID( _pPaxLogEntry->getID() + _pPaxLogEntry->getGroupSize() );	// set id next to the owner's id
_pVisitorLogEntry->setVisitor( _pPaxLogEntry->getID() );	// set owner id
_pVisitorLogEntry->setGroupSize( nVisitorCount );	// set visitor count
_pVisitorLogEntry->setGroupMember( 0 );		// set index in the group.

// set passenger type
_pVisitorLogEntry->SetmobileType( 1 );		// set which mobile type.

int types[MAX_PAX_TYPES];
_paxType.getUserType (types);			// get owner's user type.
_pVisitorLogEntry->setPaxType (types);	// set visitor's log's pax type = owner's pax type.

// check if there is the sender.
_paxType.SetTypeIndex( 1 );
if( m_pPassengerFlowDB->HasOwnStart( _paxType ) )
{
// set entry time for visitor with start.
ElapsedTime gateTime = (flightSchedule->getItem(_pPaxLogEntry->getFlightIndex()))->getArrTime();
_pVisitorLogEntry->setEntryTime (getLeadTime (_paxType, gateTime));
}
return TRUE;	
}

*/




// Calculates passenger group size according to user definition.
int CARCportEngine::getGroupSize (CMobileElemConstraint p_type, int p_max)
{
	const ProbabilityDistribution *groupSize;
	groupSize = (getTerminal()->paxDataList->getGroups())->FindFit (p_type);
	if (!groupSize)
	{
		//char str[128]; matt
		char str[2560];
		p_type.WriteSyntaxStringWithVersion (str);
		throw new TwoStringError ("no group size distribution for ", str);
	}
	int group = max(((int)(groupSize->getRandomValue())),1);
	if (!group)
		throw new StringError (" group size distribution evaluated to zero");
	return min(group, p_max);
}

//##ModelId=3E2FAD08002D
ElapsedTime CARCportEngine::getLeadTime (CMobileElemConstraint p_type, ElapsedTime p_time , const ProcessorID& _standGate)
{
	const ProbabilityDistribution *leadLagTime;
	if( p_type.isArrival() && p_type.GetTypeIndex() == 2 )
	{
		if( getTerminal()->bagGateAssignDB->IfGateHasHigherPriority() )
		{
			///			// TRACE ("\n%s\n",_standGate.GetIDString() );
			int iBestIdx = getTerminal()->bagData->getGateLagTimeDB()->findBestMatch( _standGate );
			if( iBestIdx < 0 )
				iBestIdx = 0;
			if( iBestIdx >= 0 )
			{
				CGageLagTimeData* pData = (CGageLagTimeData*) (getTerminal()->bagData->getGateLagTimeDB()->getItem( iBestIdx ) );
				leadLagTime = pData->GetDistribution();
			}
			else
			{
				//throw new StringError ("Can not find baggage lead/lag time when gate has higher priority !");
				throw new TwoStringError ("Can not find baggage lead/lag time when gate has higher priority !", _standGate.GetIDString());
			}
		}
		else
		{
			leadLagTime = getTerminal()->bagData->getLagTimeDatabase()->lookup( p_type );
			if (!leadLagTime)
			{
				 CString strErrorMessage;
				 p_type.screenPrint(strErrorMessage,0,128);
				 throw new TwoStringError(" no baggage deliver time for ", strErrorMessage.GetBuffer(0));
			}
		}

	}
	else
	{
		leadLagTime = (getTerminal()->paxDataList->getLeadLagTime())->lookup_with_procid(p_type,_standGate);
	}

	if (!leadLagTime)
	{
		CString strPrint;//char str[128];
		p_type.screenPrint( strPrint, 0, 128 );
		throw new TwoStringError ("no lead/lag time for ", strPrint.GetBuffer(0));
	}

	ElapsedTime leadTime = (float)(60.0 * leadLagTime->getRandomValue());
	leadTime += p_time;
	return (leadTime < (ElapsedTime)0l)? (ElapsedTime) 0l: leadTime;
}

//Calculates bags for p_group with p_type and returns count.
//##ModelId=3E2FAD08003E
int CARCportEngine::getBags (CMobileElemConstraint p_type, int p_group)
{
	/*const ProbabilityDistribution *bagDist;
	bagDist = (paxDataList->getBags())->lookup (p_type);
	if (!bagDist)
	{
	char str[128];
	p_type.WriteSyntaxString (str);
	throw new TwoStringError ("no bag count distribution for ", str);
	}
	return p_group * (int)(bagDist->getRandomValue());*/
	return 0;
}

//Creates bags and attachs them to bagLog.
//Sets bag id, attaches pax to it, assignes flight to it, attachs it baggage carosel, set entry time.
//Returns number of bags in p_type.


//##ModelId=3E2FAD08004B
int CARCportEngine::createBags (CMobileElemConstraint p_type, MobLogEntry& p_elem, const FlightSchedule* _pfs)
{
	if (!getTerminal()->m_pSimParam->GetBaggageFlag())
		return 0;
	int bagCount = getBags (p_type, p_elem.getGroupSize());

	BagLogEntry bag;
	bag.setID (0);
	bag.setPax (p_elem.getID());
	//	// TRACE("Set Pax:  %d;   Get Pax:  %d\n", p_elem.getID(), bag.getPax());


	bag.setFlight (p_elem.getArrFlight());

	//Flight* aFlight = flightSchedule->getItem (bag.getFlight());
	Flight* aFlight = _pfs->getItem (bag.getFlight());
	bag.setProc (aFlight->getBaggageCarosel());
	bag.setExitTime ((ElapsedTime)0l);
	ElapsedTime gateTime (aFlight->getArrTime());

	if (bag.getProc() != -1)
		for (int i = 0; i < bagCount; i++)
		{
			ElapsedTime lagTime = getTerminal()->bagData->getBagLagTime(bag.getFlight(), &m_simFlightSchedule );
			bag.setEntryTime (gateTime+lagTime);
			bag.SetLagTime(lagTime);
			getBagLog()->addItem (bag);
		}
		return bagCount;
}

//Number of carts could be greater than bags. 
//Returns number carts.
//##ModelId=3E2FAD080059
int CARCportEngine::getCarts (CMobileElemConstraint p_type, int p_group, int p_bags)
{
	/*const ProbabilityDistribution *cartDist;
	cartDist = (paxDataList->getCarts())->lookup (p_type);
	if (!cartDist)
	{
	char str[128];
	p_type.WriteSyntaxString (str);
	throw new TwoStringError ("no cart distribution for ", str);
	}

	//Jan98: if the first selected value of cart > bags, to pick up one just
	//less than no. of bags.
	int cartCount = (int)(cartDist->getRandomValue());
	int tmpBags = p_bags / p_group;
	if (cartCount>tmpBags)
	{
	cartCount = 0;
	int loop = 0;
	while(++loop<=50)  //define 50 times loop to obtain a proper cart count
	{
	int tmp = (int)(cartDist->getRandomValue());
	if (tmp<=tmpBags)
	cartCount = max(tmp, cartCount);
	}
	}	  

	#if 0
	//Jan98 : the code in enclosed #if ~ #endif is old;
	for (int i = 0; i < p_group; i++)
	cartCount += (int)(cartDist->getRandomValue());
	return min(cartCount,p_bags);
	#endif

	return p_group*cartCount;*/ //set the total number of carts to a group
	return 0;
}


//##ModelId=3E2FAD080069
DistanceUnit CARCportEngine::getSpeed( CMobileElemConstraint p_type )
{
	CMobileElemConstraintDatabase *speedDB = getTerminal()->paxDataList->getSpeed();
	const ProbabilityDistribution* speedDistr = speedDB->FindFit (p_type);
	if (speedDistr == NULL)
		throw new StringError ("No speed available");

	CMobileElemConstraintDatabase* pImpactSpeedDB = getTerminal()->paxDataList->getImpactSpeed();
	int nCount = pImpactSpeedDB->getCount();
	double dImpact = 0;
	for (int i = 0; i < nCount; i++)//find the matching impact speed of the this passenger type
	{
		ConstraintEntry* pImpactEntry = pImpactSpeedDB->getItem(i);
		const CMobileElemConstraint* pImpactType = (CMobileElemConstraint*)pImpactEntry->getConstraint();
		//		if (p_type.isEqual(pImpactType))
		{
			const ProbabilityDistribution* pImpactSpeed = pImpactEntry->getValue();
			dImpact = pImpactSpeed->getMean()/100;
			//			break;
		}

	}

	DistanceUnit speed;
	if (dImpact ==0)
	{
		speed = (DistanceUnit)speedDistr->getRandomValue();
	}
	else 
		speed = ((DistanceUnit)speedDistr->getRandomValue()) * (1 - dImpact);

	return speed;
}
//get the speed of the passenger without bags
DistanceUnit CARCportEngine::getSpeedWithoutBags( CMobileElemConstraint p_type)
{
	CMobileElemConstraintDatabase *speedDB = getTerminal()->paxDataList->getSpeed();
	const ProbabilityDistribution* speedDistr = speedDB->FindFit (p_type);
	if (speedDistr == NULL)
		throw new StringError ("No speed available");

	DistanceUnit speed;
	speed = (DistanceUnit)speedDistr->getRandomValue();

	return speed;
}


void CARCportEngine::InitLogs( const CString& _csProjPath )
{
	PLACE_METHOD_TRACK_STRING();
	// clear logs
	getPaxLog()->clearLogs();
	getPaxLog()->clearEvents();
	CString sFileName = getTerminal()->GetSimReportManager()->GetCurrentLogsFileName( PaxEventFile, _csProjPath );
	getPaxLog()->createEventFile( sFileName );
	
	//temp lof file
	sFileName = getTerminal()->GetSimReportManager()->GetCurrentLogsFileName( TempMobileElementLogFile, _csProjPath );
	getTerminal()->m_pTempMobEventLog->createEventFile(sFileName);



	getFlightLog()->clearLogs();
	getProcLog()->clearLogs();
	getProcLog()->clearEvents();
	sFileName = getTerminal()->GetSimReportManager()->GetCurrentLogsFileName( ProcEventFile, _csProjPath );
	getProcLog()->createEventFile( sFileName );

	getTrainLog()->clearLogs();
	getTrainLog()->clearEvents();
	sFileName = getTerminal()->GetSimReportManager()->GetCurrentLogsFileName( TrainEventFile, _csProjPath );
	getTrainLog()->createEventFile( sFileName );

	getElevatorLog()->clearLogs();
	getElevatorLog()->clearEvents();
	sFileName = getTerminal()->GetSimReportManager()->GetCurrentLogsFileName( ElevatorEventFile, _csProjPath );
	getElevatorLog()->createEventFile( sFileName );


	getBridgeConnectorLog()->clearLogs();
	getBridgeConnectorLog()->clearEvents();
	sFileName = getTerminal()->GetSimReportManager()->GetCurrentLogsFileName( BridgeConnectorEventFile, _csProjPath );
	getBridgeConnectorLog()->createEventFile( sFileName );

	m_pARCport->getTerminal().m_pRetailLog->clearLogs();
	m_pARCport->getTerminal().m_pRetailLog->clearEvents();
	sFileName = getTerminal()->GetSimReportManager()->GetCurrentLogsFileName( RetailActivityEventFile, _csProjPath );
	m_pARCport->getTerminal().m_pRetailLog->createEventFile( sFileName );

	getResourceLog()->clearLogs();
	getResourceLog()->clearEvents();
	sFileName = getTerminal()->GetSimReportManager()->GetCurrentLogsFileName( ResourceEventFile, _csProjPath );
	getResourceLog()->createEventFile( sFileName );

	getPaxDestDiagnoseLog()->clearLog();
	getPaxDestDiagnoseLog()->closeDiagnoseInfoFile();
	sFileName = getTerminal()->GetSimReportManager()->GetCurrentLogsFileName( DiagnoseInfoFile, _csProjPath );
	getPaxDestDiagnoseLog()->createDiagnoseInfoFile( sFileName );
}

void CARCportEngine::CloseLogs( const CString& _csProjPath )
{
	PLACE_METHOD_TRACK_STRING();

	CString sFileName;

	sFileName = getTerminal()->GetSimReportManager()->GetCurrentLogsFileName( FlightDescFile, _csProjPath );
	getFlightLog()->SaveToFile( sFileName );


	sFileName = getTerminal()->GetSimReportManager()->GetCurrentLogsFileName( ProcEventFile, _csProjPath );
	getProcLog()->SaveEventToFile( sFileName );


	sFileName = getTerminal()->GetSimReportManager()->GetCurrentLogsFileName( ProcDescFile, _csProjPath );
	getProcLog()->SaveToFile( sFileName );


	sFileName = getTerminal()->GetSimReportManager()->GetCurrentLogsFileName( BagDescFile, _csProjPath );
	getBagLog()->SaveToFile( sFileName );


	sFileName = getTerminal()->GetSimReportManager()->GetCurrentLogsFileName( TrainEventFile, _csProjPath );
	getTrainLog()->SaveEventToFile( sFileName );

	sFileName = getTerminal()->GetSimReportManager()->GetCurrentLogsFileName( TrainDescFile, _csProjPath );
	getTrainLog()->SaveToFile( sFileName );

	sFileName = getTerminal()->GetSimReportManager()->GetCurrentLogsFileName( ElevatorEventFile, _csProjPath );
	getElevatorLog()->SaveEventToFile( sFileName );

	sFileName = getTerminal()->GetSimReportManager()->GetCurrentLogsFileName( ElevatorDescFile, _csProjPath );
	getElevatorLog()->SaveToFile( sFileName );

	sFileName = getTerminal()->GetSimReportManager()->GetCurrentLogsFileName( BridgeConnectorEventFile, _csProjPath );
	getBridgeConnectorLog()->SaveEventToFile( sFileName );


	sFileName = getTerminal()->GetSimReportManager()->GetCurrentLogsFileName( RetailActivityDescFile, _csProjPath );
	m_pARCport->getTerminal().m_pRetailLog->SaveToFile( sFileName );

	sFileName = getTerminal()->GetSimReportManager()->GetCurrentLogsFileName( RetailActivityEventFile, _csProjPath );
	m_pARCport->getTerminal().m_pRetailLog->SaveEventToFile( sFileName );



	sFileName = getTerminal()->GetSimReportManager()->GetCurrentLogsFileName( BridgeConnectorDescFile, _csProjPath );
	getBridgeConnectorLog()->SaveToFile( sFileName );

	sFileName = getTerminal()->GetSimReportManager()->GetCurrentLogsFileName( ResourceEventFile, _csProjPath );
	getResourceLog()->SaveEventToFile( sFileName );

	sFileName = getTerminal()->GetSimReportManager()->GetCurrentLogsFileName( ResourceDescFile, _csProjPath );
	getResourceLog()->SaveToFile( sFileName );

	sFileName = getTerminal()->GetSimReportManager()->GetCurrentLogsFileName( DiagnoseLogFile, _csProjPath );
	getPaxDestDiagnoseLog()->saveLog( sFileName );

	sFileName = getTerminal()->GetSimReportManager()->GetCurrentLogsFileName( DiagnoseInfoFile, _csProjPath );
	getPaxDestDiagnoseLog()->closeDiagnoseInfoFile();




	//write passenger log name
	
	//close temp mobile element log file
	getTerminal()->m_pTempMobEventLog->closeEventFile();
	sFileName = getTerminal()->GetSimReportManager()->GetCurrentLogsFileName( TempMobileElementLogFile, _csProjPath );
	getTerminal()->m_pTempMobEventLog->openEventFile(sFileName);

	//buffer
	std::vector<MobEventStruct> vEventBuffer;
	vEventBuffer.resize(1000);

	int nTempLogFileCount = getTerminal()->m_pTempMobEventLog->getFileCount();
	//CProgressBar  barFile(_T("Sort File"),100, 100, FALSE, 0);
	//barFile.SetPos(0);
	//barFile.SetRange(0, nTempLogFileCount);
	for (int nCurFile = 0; nCurFile < nTempLogFileCount; ++ nCurFile)
	{

		//barFile.StepIt();
		EventLog<MobEventStruct>* pMobileTempLog = getTerminal()->m_pTempMobEventLog->GetEventLog(nCurFile);
		if(pMobileTempLog == NULL)
			continue;


		//std::map<int, MobLogEntry> m_mapIDLogEntry;
		//long lTotalEventCount = pMobileTempLog->getTotalEventCount();
		//CProgressBar  barEvent(_T("Sort Log"),100, 100,FALSE,1);
		//
		//barEvent.SetPos(0);
		//barEvent.SetRange(0, lTotalEventCount/1000);
		//for(long lReadCount = 0; lReadCount < lTotalEventCount; lReadCount += 1000)
		//{	
		//	barEvent.StepIt();

		//	long lCurTimeCount = 1000;	
		//	if(lTotalEventCount - lReadCount < 1000)
		//		lCurTimeCount = lTotalEventCount - lReadCount;
		//	

		//	//get data
		//	pMobileTempLog->setPosition(lReadCount * sizeof(MobEventStruct));
		//	pMobileTempLog->getEvents(&vEventBuffer[0],lCurTimeCount);
		//	
		//	//process data one by one
		//	for (int lEvent = 0; lEvent < lCurTimeCount; ++ lEvent)
		//	{
		//		MobEventStruct& mobEvent = vEventBuffer[lEvent];

		//		std::map<int, MobLogEntry>::iterator iterFind = m_mapIDLogEntry.find(mobEvent.elementID);
		//		if(iterFind != m_mapIDLogEntry.end())
		//		{
		//			MobLogEntry& logEntry = (*iterFind).second;

		//			MobEventStruct* pNewItem = new MobEventStruct(mobEvent);
		//			logEntry.getMobEventList().push_back(pNewItem);


		//			if(mobEvent.state == Death)
		//			{
		//				logEntry.SetEventLog(getTerminal()->m_pMobEventLog);
		//				logEntry.saveEventsToLog();
		//				logEntry.clearLog();
		//				getPaxLog()->updateItem(logEntry, logEntry.getIndex());

		//				m_mapIDLogEntry.erase(iterFind);
		//			}

		//		}
		//		else
		//		{
		//			MobLogEntry logEntry;
		//			if(getPaxLog()->GetItemByID(logEntry,mobEvent.elementID) )
		//			{
		//				MobEventStruct* pNewItem = new MobEventStruct(mobEvent);
		//				//logEntry.getMobEventList().push_back(pNewItem);
		//				m_mapIDLogEntry[mobEvent.elementID] = logEntry;
		//				m_mapIDLogEntry[mobEvent.elementID].getMobEventList().push_back(pNewItem);
		//			}
		//		}
		//	}
		//}

		//std::map<int, MobLogEntry>::iterator iterLeft = m_mapIDLogEntry.begin();
		//for (; iterLeft != m_mapIDLogEntry.end(); ++iterLeft)
		//{
		//	MobLogEntry& logEntry = (*iterLeft).second;

		//	logEntry.SetEventLog(getTerminal()->m_pMobEventLog);
		//	logEntry.saveEventsToLog();
		//	logEntry.clearLog();
		//	getPaxLog()->updateItem(logEntry, logEntry.getIndex());
		//}
		//std::map<int, MobLogEntry> m_mapIDLogEntry;


		std::vector<MobLogEntry *> vFileMobEntry;
		int nPaxNumberInFile = getTerminal()->m_pTempMobEventLog->getPaxPerFile();
		vFileMobEntry.resize(nPaxNumberInFile,NULL);

		//
		long lTotalEventCount = pMobileTempLog->getTotalEventCount();
		CProgressBar  barEvent(_T("Sort Log"),100, 100,FALSE,1);

	//	barEvent.SetPos(0);
	//	barEvent.SetRange(0, lTotalEventCount/1000);
		int nStep = lTotalEventCount/100;
		if(nStep == 0)
			nStep = 1;

		for(long lReadCount = 0; lReadCount < lTotalEventCount; lReadCount += 1000)
		{	
		//	barEvent.StepIt();
			//index + 1
			SendSimSortLogMessage(lReadCount/nStep, nCurFile + 1, nTempLogFileCount );

			long lCurTimeCount = 1000;	
			if(lTotalEventCount - lReadCount < 1000)
				lCurTimeCount = lTotalEventCount - lReadCount;


			//get data
			pMobileTempLog->setPosition(lReadCount * sizeof(MobEventStruct));
			pMobileTempLog->getEvents(&vEventBuffer[0],lCurTimeCount);

			//process data one by one
			for (int lEvent = 0; lEvent < lCurTimeCount; ++ lEvent)
			{
				MobEventStruct& mobEvent = vEventBuffer[lEvent];

				int nPaxIndex = mobEvent.elementID%nPaxNumberInFile;

				MobLogEntry *pLogEntry = vFileMobEntry[nPaxIndex];
				if(pLogEntry)
				{
					MobLogEntry& logEntry = *pLogEntry;
					MobEventStruct* pNewItem = new MobEventStruct(mobEvent);
					logEntry.getMobEventList().push_back(pNewItem);


					if(mobEvent.state == Death)
					{
						logEntry.SetEventLog(getTerminal()->m_pMobEventLog);
						logEntry.saveEventsToLog();
						logEntry.clearLog();
						getPaxLog()->updateItem(logEntry, logEntry.getIndex());

						delete pLogEntry;
						vFileMobEntry[nPaxIndex] = NULL;
					}

				}
				else
				{
					if(mobEvent.state != Death)
					{
						pLogEntry = new MobLogEntry;
						if(getPaxLog()->GetItemByID(*pLogEntry,mobEvent.elementID) )
						{
							MobEventStruct* pNewItem = new MobEventStruct(mobEvent);
							pLogEntry->getMobEventList().push_back(pNewItem);

							vFileMobEntry[nPaxIndex] = pLogEntry;
						}
					}
					else
					{
						//the element died twice times ...
					}
				}
			}
		}

		std::vector<MobLogEntry *>::iterator iter = vFileMobEntry.begin();
		for (; iter != vFileMobEntry.end(); ++iter)
		{
			MobLogEntry* plogEntry = *iter;
			if(plogEntry == NULL)
				continue;

			plogEntry->SetEventLog(getTerminal()->m_pMobEventLog);
			plogEntry->saveEventsToLog();
			plogEntry->clearLog();
			getPaxLog()->updateItem(*plogEntry, plogEntry->getIndex());
			delete plogEntry;
		}	
		vFileMobEntry.clear();


	}

	getTerminal()->m_pTempMobEventLog->closeEventFile();
	getTerminal()->m_pTempMobEventLog->createEventFile(sFileName); //delete temporary files
	//when TLOS created processor dynamicly ,we need update proc index in pax log to proc's new index
	RefreshProcIdxInLogs();

	sFileName = getTerminal()->GetSimReportManager()->GetCurrentLogsFileName( PaxEventFile, _csProjPath );
	getPaxLog()->SaveEventToFile( sFileName );


	sFileName = getTerminal()->GetSimReportManager()->GetCurrentLogsFileName( PaxDescFile, _csProjPath );
	getPaxLog()->SaveToFile( sFileName );

}


//when TLOS created processor dynamicly ,we need update proc index in pax log.etc to new index
void CARCportEngine::RefreshProcIdxInLogs()
{
	PLACE_METHOD_TRACK_STRING();
	if( getTerminal()->procList->GetDynamicCreatedProcs() && getTerminal()->procList->GetDynamicCreatedProcs()->getProcessorCount() )
	{		
		std::map<int,Processor*>& mapDynamicCreateProcList = getTerminal()->procList->GetMapOfDynamicCreatedProcIdx();
		std::map<int,Processor*>& mapProcList = getTerminal()->procList->GetMapOfProcIdx();
		int iPaxLogItemCount = getPaxLog()->getCount();
		for( int i=0; i<iPaxLogItemCount; ++i )
		{
			MobLogEntry paxItem;
			getPaxLog()->getItem( paxItem, i);
			paxItem.SetEventLog( getPaxLog()->getEventLog() );

			int iTrackCount = paxItem.getCount();					
			for( int j=0; j<iTrackCount; ++j )
			{
				MobEventStruct track = paxItem.getEvent( j );
				int iOldProcIdx = track.procNumber;
				if( iOldProcIdx == START_PROCESSOR_INDEX )
					continue;
				if (iOldProcIdx == END_PROCESSOR_INDEX)
					break;

				int iNewIdx = -1;
				//				if( track.bDynamicCreatedProc && (iOldProcIdx < static_cast<int>(mapDynamicCreateProcList.size())))

				if( track.bDynamicCreatedProc)
				{	
					iNewIdx	= mapDynamicCreateProcList[ iOldProcIdx ]->getIndex();				
				}
				else
				{					
					iNewIdx	= mapProcList[ iOldProcIdx ]->getIndex();
				}
				ASSERT( iNewIdx >=0 );
				if( iNewIdx != iOldProcIdx )
				{
					track.procNumber = iNewIdx;
					paxItem.updateEvent( track, j );
				}								
			}
		}
	}
}


void CARCportEngine::DeleteGreetPoolData()
{
	if(GreetPool.empty()) return;
	for(std::vector<ConvergePair*>::iterator it = GreetPool.begin();it!= GreetPool.end();it++)
	{
		delete (*it);
	}
}

//initialize congestion parameter form file
void CARCportEngine::InitCongestionParam(const CString& _pProjPath)
{
	m_pARCport->getCongestionAreaManager().loadDataSet(_pProjPath+"\\INPUT");
}

void CARCportEngine::ReflectOnFlightArriveStand(Flight* pFlight, const ElapsedTime& actualTime,const ElapsedTime& depTime)
{
	int numFlights = m_simFlightSchedule.getCount();
	ASSERT(numFlights > 0);

	// for adjust arriving flight's mobElement entry time
	// wake up mobile element when flight arrive stand
	//if(pFlight->isArriving())
	//	m_pMobElemTermEntryManager->ActiveMobileElementGroup(pFlight, actualTime);

	//for adjust departure flight's boarding call time
	//adjust boardingCall time according to flight? actual departure time
	if(pFlight->isDeparting())
	{
		m_pFltsBoardingCallManager->ScheduleBoardingCallEvents(pFlight, actualTime,depTime);
	}
}


void CARCportEngine::ClearMovingTrain()
{
	for (int i = 0; i < (int)m_simMovingTrainList.size(); i++)
	{
		if (m_simMovingTrainList[i])
		{
			m_simMovingTrainList[i]->M_DELIDX = 0;
			delete m_simMovingTrainList[i];
			m_simMovingTrainList[i] = NULL;
		}
	}
	m_simMovingTrainList.clear();
}

void CARCportEngine::FlushMovingTrain()
{
	for (int i = 0; i < (int)m_simMovingTrainList.size(); i++)
	{
		if (m_simMovingTrainList[i])
		{
			m_simMovingTrainList[i]->FlushLog();
		}
	}
}

Terminal * CARCportEngine::getTerminal()
{
	return &(m_pARCport->getTerminal());
}

PaxLog * CARCportEngine::getPaxLog()
{
	return getTerminal()->paxLog;
}

FlightLog * CARCportEngine::getFlightLog()
{
	return getTerminal()->flightLog;
}

BagLog* CARCportEngine::getBagLog()
{
	return getTerminal()->bagLog;
}

ProcLog* CARCportEngine::getProcLog()
{
	return getTerminal()->procLog;
}

CTrainLog* CARCportEngine::getTrainLog()
{
	return getTerminal()->m_pTrainLog;
}

StringDictionary * CARCportEngine::getInStrDict()
{
	return getTerminal()->inStrDict;
}

BaggageData * CARCportEngine::getBagData()
{
	return getTerminal()->bagData;
}

PassengerTypeList * CARCportEngine::getPaxTypeList()
{
	return getTerminal()->inTypeList;
}

CElevatorLog* CARCportEngine::getElevatorLog()
{
	return getTerminal()->m_pElevatorLog;
}

CBridgeConnectorLog* CARCportEngine::getBridgeConnectorLog()
{
	return getTerminal()->m_pBridgeConnectorLog;
}

ResourceElementLog * CARCportEngine::getResourceLog()
{
	return getTerminal()->resourceLog;
}

CPaxDestDiagnoseLog* CARCportEngine::getPaxDestDiagnoseLog()
{
	return getTerminal()->m_pDiagnosLog;
}

void CARCportEngine::SetProcListEngine()
{
	ASSERT(getTerminal());
	if(getTerminal() == NULL)
		return;
	if(getTerminal()->procList == NULL)
		return;

	int nProcCount =getTerminal()->procList->getProcessorCount();
	
	for (int nProc = 0; nProc < nProcCount; ++nProc)
	{
		Processor *pProc = getTerminal()->procList->getProcessor(nProc);
		pProc->SetEngine(this);
	}


}

CCongestionAreaManager& CARCportEngine::getCongestionAreaManager()
{
	return m_pARCport->getCongestionAreaManager();
}

ProjectCommon * CARCportEngine::getProjectCommon()
{
	return m_pARCport->getProjectCommon();
}


















































































