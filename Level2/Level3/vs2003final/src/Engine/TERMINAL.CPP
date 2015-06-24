#include "stdafx.h"
//#include <afxtempl.h>
//#include "terminal.h"
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <iostream>
//
//#include "common\SimAndReportManager.h"
//#include "common\mutilreport.h"
//#include "common\CodeTimeTest.h"
//#include "common\ARCFatalException.h"
//#include "common\ARCCriticalException.h"
//#include "common\ARCImportantException.h"
//#include "common\projectmanager.h"
//#include "common\elaptime.h"
//#include "common\exeption.h"
//#include "common\WinMsg.h"
//#include "common\OnBoardException.h"
//#include "common\Singleton.h"
//#include "common\category.h"
//#include "Common/AirportDatabase.h"
//#include "Common/ARCTracker.h"
//
//#include "inputs\BagGateAssign.h"
//#include "inputs\GageLagTimeData.h"
//#include "inputs\flightconwithprociddatabase.h"
//#include "inputs\PaxBulkInfo.h"
//#include "inputs\AllPaxTypeFlow.h"
//#include "inputs\resourcepool.h"
//#include "inputs\HubbingDatabase.h"
//#include "inputs\AllCarSchedule.h"
//#include "inputs\PipeDataSet.h"
//#include "inputs\paxflow.h"
//#include "inputs\MobileElemTypeStrDB.h"
//#include "inputs\SubFlowList.h"
//#include "inputs\PaxFlowConvertor.h"
//#include "inputs\AllGreetingProcessors.h"
//#include "inputs\DirectionUtil.h"
//#include "inputs\simparameter.h"
//#include "inputs\pax_db.h"
//#include "inputs\paxdata.h"
//#include "inputs\flight.h"
//#include "inputs\assigndb.h"
//#include "inputs\probab.h"
//#include "inputs\distlist.h"
//#include "inputs\hubbing.h"
//#include "inputs\bagdata.h"
//#include "inputs\typelist.h"
//#include "inputs\pipegraph.h"
//#include "Inputs/GateAssignmentConstraints.h"
//
//#include "engine\ElevatorProc.h"
//#include "engine\TracePredef.h"
//#include "engine\Monitor.h"
//#include "engine\terminal.h"
//#include "engine\person.h"
//#include "engine\boardcal.h"
//#include "engine/BillboardProc.h"
//#include "Engine\Airside\AirsideSimConfig.h"
//#include "Engine/BridgeConnector.h"
//#include "Engine\MovingTrain.h"
//
//#include "results\ElevatorLogEntry.h"
//#include "results\ElevatorLog.h"
//#include "results\resourceelementlog.h"
//#include "results\PaxDestDiagnoseLog.h"
//#include "results\paxlog.h"
//#include "results\paxentry.h"
//#include "results\fltlog.h"
//#include "results\proclog.h"
//#include "results\trainlog.h"
//#include "Results/BridgeConnectorLog.h"
//#include "Results/BridgeConnectorLogEntry.h"
//
//#include "reports\SimLevelOfService.h"
//#include "reports\SimAutoReportPara.h"
//#include "reports\MutiRunReportAgent.h"
//#include "reports\ReportParaDB.h"
//#include "reports\ReportParameter.h"
//
//#include "Airside/AirsideSimulation.h"
//#include "OnBoard/OnBoardSimEngine.h"
//
//#include ".\Results\OutputAirside.h"
//
//#include "FlightsBoardingCallManager.h"
//#include "SimulationDiagnoseDelivery.h"
//#include "BoardingCallPolicy.h"
//#include "FlightDelaysUtility.h"
//#include "SimEngineConfig.h"
//#include "MobElementLifecycleManager.h"
//#include "SimEngineCommunicationLayer.h"
//#include "MobElementsDiagnosis.h"
//#include "FltOperatingDoorSpecInSim.h"
//#include "AutoRoster.h"
//#include "AutoCreateProcessorContainer.h"
//#include "FireEvacuationEvent.h"
//#include "ConvergePair.h"



//extern std::vector<ConvergePair *> GreetPool;
//
//
//Terminal::Terminal () :m_AirsideSel(TRUE),m_LandSel(FALSE),m_OnboardSel(FALSE)
//{
//	PLACE_TRACK_STRING(_T("{ee54de7d-5243-4ae8-b612-995dffcb4b1e}"));
//	m_timeFireEvacuation.set(23,59,59);
//	m_bFireOccur = false;
//
//	m_pFltsBoardingCallManager = new FlightsBoardingCallManager;
//
//}
//
////Main simulation engine function.
////It reloads all settings data and save them to appropriate lists and sets current time. Then the main engine loop function eventList::start() is invoked.
////All data logs created for animation are written and finally closed.
////##ModelId=3E2FAD0703A5
//void Terminal::runSimulation (HWND _hWnd, const CString& _csProjPath,const SelectedSimulationData& simSelData )
//{
//	PLACE_TRACK_STRING(_T("{5d68657f-8564-4c7b-b25a-cbffd921d7fd}"));
//
//	// Diagnose delivery helper class, deliver diagnose information to simulation engine dialog
//	DiagnoseDelivery()->setHandle(_hWnd);
//
//
//	// simulation config
//	simEngineConfig()->setSimParameter( m_pSimParam );
//	// Airside config
//	simSelData.IsAirsideSel() ? simEngineConfig()->simAirsideMode() : 1;
//	// Landside config
//	simSelData.IsLandsideSel() ? simEngineConfig()->simLandsideMode() : 1;
//    // Terminal config
//	simSelData.IsTerminalSel() ? simEngineConfig()->simTerminalMode() : 1;
//	// OnBoard config
//	simSelData.IsOnBoardSel() ? simEngineConfig()->simOnBoardMode() : 1;
//
//	// init Mobile Elements Diagnosis service.
//	MobElementsDiagnosis()->Initialize( _csProjPath );
//
//
//#ifdef CODE_PERFOMANCE_DEBUGE_FLAG
//	CCodeTimeTest::InitTools();
//#endif
///*===============================================================
//
//  In old DOS version, the sim engin run in an seperate EXE. it will 
//  load input data first. then call runSimulation(). Now the input data
//  are in the memory already. We don't have to reload it. just need to 
//  make sure all the data is clean.
//
//  void CleanTheInputData();
//  
//===============================================================*/
//// catch all sim engine exception
//	CTermPlanDoc* pDoc = (CTermPlanDoc*) ((CMDIChildWnd *)((CMDIFrameWnd*)AfxGetApp()->m_pMainWnd)->GetActiveFrame())->GetActiveDocument();
//	assert( pDoc );
//
//    m_AirsideSel = simSelData.IsAirsideSel() ;
//	m_LandSel = simSelData.IsLandsideSel() ;
//	m_OnboardSel = simSelData.IsOnBoardSel() ;
//	m_terminal = simSelData.IsTerminalSel() ;
//	p_simAirside = NULL ;
//
//	try			
//	{	
//		//init array which store every mobile elememnt count 
//		int iMobleTypeCount = m_pMobElemTypeStrDB->GetCount();
//		m_vNonPaxCountByType.reserve( iMobleTypeCount );
//
//		m_hWnd = _hWnd;
//		m_bCancel = false;
//
//		//init visitor , bags etc. rotation data
//		Person::initSpacing();
//
//		// write mirrored strDict, typeList, sector & category files to logs dir
//		char filename[_MAX_PATH];
//		PROJMANAGER->getFileName ( _csProjPath, StringDictBinFile, filename);
//		inStrDict->storeDictionary (filename);
//
//		inTypeList->setFileType (TypeListBinFile);
//		inTypeList->saveDataSet(_csProjPath, false);
//		inTypeList->setFileType(PassengerDescriptionFile);
//
//		closeLogs();
//
//
//
//		CSimGeneralPara* pSim = m_pSimParam->GetSimPara();
//		m_pSimAndReportManager->addNewSimResult( m_pSimParam ,_csProjPath, this  );
//		
//		//m_pSimParam->
//		linkFlightData(pDoc->GetProjectID());
//		if (!flightSchedule->RegenerateSchedule(pSim, m_simFlightSchedule))
//			return;
//
//		m_pSimAndReportManager->SetStartDate(m_simFlightSchedule.GetStartDate());
//
//		if (flightSchedule->GetStartDate().IsAbsoluteDate())
//		{
//			COleDateTimeSpan deltaDate = flightSchedule->GetStartDate().GetDate() - m_simFlightSchedule.GetStartDate().GetDate();
//			m_pSimAndReportManager->SetUserStartTime( pSim->GetStartTime() + (long)deltaDate.GetTotalSeconds() );		
//			m_pSimAndReportManager->SetUserEndTime( pSim->GetEndTime() + (long)deltaDate.GetTotalSeconds() ); 
//		}
//		else
//		{
////			if( (pSim->GetStartTime() - pSim->GetInitPeriod()).asSeconds() < 0 )
////			{
////				m_pSimAndReportManager->SetUserStartTime( pSim->GetStartTime() + WholeDay);
////				m_pSimAndReportManager->SetUserEndTime(pSim->GetEndTime() + WholeDay);
////			}
////			else
//			{
//				m_pSimAndReportManager->SetUserStartTime(pSim->GetStartTime());		
//				m_pSimAndReportManager->SetUserEndTime(pSim->GetEndTime()); 
//			}
//		}
//		m_pSimAndReportManager->SetAirsideSim(m_AirsideSel?true:false);
////		m_pSimAndReportManager->SetUserStartTime(m_simFlightSchedule.GetUserStartTime());
////		m_pSimAndReportManager->SetUserEndTime(m_simFlightSchedule.GetUserEndTime());
//
//		// calculate the bisect line, intersection point.
//		m_pPipeDataSet->CalculateAllIntermediatData();
//		// calculate needed data of moving sidewalk attached to pipe
//		m_pPipeDataSet->CalculateAllTimePointOnSideWalk();
//		
//		int iRunNumber = pSim->GetNumberOfRun();
//		
//
//		// get all floor's altitude
//		int nFloorCount = pDoc->GetFloorByMode( EnvMode_Terminal ).m_vFloors.size();
//		std::vector<double>vFloorAltitudes;
//		for(int i=0; i<nFloorCount; i++) 
//		{			
//			vFloorAltitudes.push_back( pDoc->GetFloorByMode( EnvMode_Terminal ).m_vFloors[i]->RealAltitude() );		
//		}
//
//		procList->SetDynamicCreateProcFlag( false );
//		// init data which TLOS check need
//		CSimLevelOfService* pTLOSSim = NULL;
//		if( pSim->GetClassType() == "LEVEOFSERVICE" )
//		{
//			pTLOSSim = (CSimLevelOfService*)pSim;
//			m_pTLOSMonitor->InitTLOSData( );
//			m_pAutoRoster->init( iRunNumber );
//			m_pAutoCreatedProcContainer->init();
//			procList->SetDynamicCreateProcFlag( pTLOSSim->getDynamicCreateProcFlag() ? true : false );
//			procList->InitDynamicCreateProcList( this );
//		}
//		else
//		{
//			//clear TLOS flag if last time run with TLOS parameter
//			procList->ResetAllTLOSFlag();
//		}
//		if( !procList->IfAllBaggageProcDataValid() )
//		{
//			return ;
//		}
//		//m_pPassengerFlowDB->InitFlowRules();
//		
//		CPaxFlowConvertor tempFlowConvetor;
//		tempFlowConvetor.SetInputTerm( this );
//		CAllPaxTypeFlow tempAllPaxFlow;
//		// build source / dest processor of every conveyor processor( or related with conveyor processor in pax flow )
//		procList->InitProcessorSourceDestProcessors( &tempFlowConvetor, &tempAllPaxFlow );
//
//		procList->initResourcePoolData();
//
//		//load GateAssignmentConstraint for stand reassignment in Airside SimEngine
//		GateAssignmentConstraintList gateAssignConstraints;
//		gateAssignConstraints.SetInputTerminal(this);
//		gateAssignConstraints.loadDataSet(_csProjPath);
//		gateAssignConstraints.GetAdjConstraints().loadDataSet(_csProjPath);
//
//		FltOperatingDoorSpecInSim pFltOperatingDoorSpec(m_pAirportDB);
//		//note: if engine run with TLOS which has dynamicly create processor funtion, number of run must be 1
//		for( int iRun=0; iRun<iRunNumber; ++iRun )
//		{	
//			int nPos = 100 / iRunNumber * iRun;
//			SetPercentOfRuns( nPos );
//			SetRunNumber( iRun + 1 );
//			SendSimMessageOfRuns();
//
//			CString sSubName;
//			sSubName.Format("%d", iRun );
//			sSubName =	strSimResult + sSubName;
//			m_pSimAndReportManager->SetCurrentSimResult(sSubName );
//			InitLogs( _csProjPath );
//
//			// need to update the load base on the randon value.
//			m_simFlightSchedule.updateFlights( flightData );
//
//			if( pTLOSSim )
//			{
//				m_pAutoRoster->setCurRosterIndex( iRun );
//				m_pAutoRoster->clearUserRoster( this, pTLOSSim );
//
//				// init dynamic created
//				procList->InitDynamicCreateProcList( this );
//			}
//
//			// init area density
//			m_pAreas->InitDataBeforeEngine( &tempFlowConvetor, &tempAllPaxFlow );
//			//init all data used by baggage device processor
//			//procList->InitAllBaggageProcData();
//
//			//init billboard 
//			//std::vector<BaseProcessor *> vecBillboardProc;//billboard processor array
//			//GetProcessorList(EnvMode_Terminal)->GetProcessorsByType(vecBillboardProc,BillboardProcessor);
//
//			//int nBillboardProcCount = static_cast<int>(vecBillboardProc.size());
//			//for (int i = 0; i < nBillboardProcCount; i++)
//			//{
//			//	((BillboardProc *)vecBillboardProc[i])->InitWholeExposurePath();
//			//}
//
//	#ifdef WANT_TO_TRACE_QUUEU_INFO
//			procList->InitQueueTraceFlag();
//	#endif
//			
//			//init undirected graph of pipe system 
//			m_pPipeDataSet->m_pPipeMgr->initGraphMgr( this, pDoc->GetFloorByMode( EnvMode_Terminal ).m_vFloors.size() );
//			
//			//initialize congestion grid for every floor
//			m_congManager.InitGrid(this, pDoc->GetFloorCount());
//
//			/////////////////////////////////////////////////
//			// prepare data for engine log and events
//			procAssignDB->createDefaultEvent();
//			procAssignDB->computeAbsolutTime( this, &m_simFlightSchedule );
//			
//			// generate train logs
//			GenerateTrains();
//
//			// init flight log from flight schedule
//			//flightSchedule->writeBinarySchedule (flightLog);
//			m_simFlightSchedule.writeBinarySchedule(flightLog);
//
//			// processor in the proclist is the only data the engine will interfaced. 
//			//need to refreshed before the engine start.
//			procList->RefreshRelatedDatabase();
//
//			//clear dynamic processor flag
//			procList->ClearAllProcDynamicFlag();
//
//			procAssignDB->divideEvents( this );
//
////			m_timeFireEvacuation = m_pSimAndReportManager->GetUserEndTime() + m_pSimParam->GetFollowupPeriod();
////			int nSimDays = ( m_pSimAndReportManager->GetUserEndTime().asSeconds() + m_pSimParam->GetFollowupPeriod().asSeconds() ) / WholeDay + 1;
//			if( !flightSchedule->GetStartDate().IsAbsoluteDate() )
//				procAssignDB->GenerateEntryForMultipleDays( 3, this  );
//			/////////////////////////////////////////////////
//
//			SetPercent( 10 );
//			SendSimMessage( "Analysis Process Data..." );
//
//			
//			//bagData->BuildFlightAssignToBaggageDevice();
//
//
//			//build all possible path in every process and set its internal flow
//			m_pSubFlowList->BuildAllProcessPossiblePath();				
//			m_pSubFlowList->SetTransitionLink( *m_pPassengerFlowDB, *m_pStationPaxFlowDB, *m_pStationPaxFlowDB );
//
//
//			//find all place which passenger and it's visitor, hand bag..etc meeting
//			CPaxFlowConvertor tempConvetor;
//			tempConvetor.SetInputTerm( this );
//			this->m_pGreetingProcessors->FindGreetingPlaceFromAllPaxFlow( tempConvetor );
//
//			SetPercent( 14 );
//			SendSimMessage( "Analysis Passenger Flow Data..." );
//			
//			//init lead to gate logic from pax flow
//			CDirectionUtil tempUtil;
//			tempUtil.InitAllProcDirection( tempConvetor );				
//			procList->InitAllProcDirection();
//
//			for( int i=0; i<iMobleTypeCount; ++i )
//			{
//				m_vNonPaxCountByType[i] = 0;
//			}
//
//			//init every single processor's state
//			// by doing this ,all processors,nomater it is in proclist or not, for example, 
//			// the entry doors and exit doors of station are not in proclist but need also be
//			// init
//			CleanInputData();
//
//
//			// init all processor's isOpen = FALSE;
//			// Roster will open all procesors which use default roster at 0:00, and close it at 24:00
//			procList->InitProcsOpenAvaiFlag();
//
//			// init elevator's necessary data, such as every floor's height....etc		
//			// must be called after call procList->RefreshRelatedDatabase();
//			if(!procList->InitAllElevatorData( vFloorAltitudes, this ))
//			{
//				SetPercent(100);
//				SetPercentOfRuns( 100 );
//				SetRunNumber( iRunNumber );
//				SendSimMessageOfRuns();
//				return;
//			}
//
//// 			// generate the logs for the all Elevators		
//// 		    GenerateElevatorLogEntry( );
//		
//
//			//because the conveyor architecture is designed in "proxy pattern ", so we must init a real performer before
//			// engine use conveyor.
//			procList->InitEveryConveyor( *miscData );
//
//			//initialize bulks
//			m_pPaxBulkInfo->initBulk();
//
//			bool bInitializeNoError = true;
//
//
//			long nPax = 0;
//			try
//			{
//				if(simEngineConfig()->isSimTerminalMode())
//					nPax = generatePassengers(&m_simFlightSchedule);
//
//				// generate the logs for the all Elevators		
//				GenerateElevatorLogEntry( );
//
//				if(simEngineConfig()->isSimTerminalModeAlone())
//				{
//					SendSimMessage( "Analysis Flight delays..." );
//					FlightDelaysUtility::impactFlightDelaysEffectToMobElement(paxLog, &m_simFlightSchedule); 
//				}
//				{
//					// SORT MOBILE ELEMENTS
//					// all pax and bags must then be sorted by their entry time
//					sprintf( m_csDisplayMsg, "Sorting %d Mobile Elements...", paxLog->getCount() );
//					SetPercent( 23 );
//					SendSimMessage( m_csDisplayMsg );
//					
//					//NOTE:: must invoke only once!!!
//					CProgressBar bar(_T("Sorting Mobile Elements...") );
//					paxLog->sortPassengers(bar);
//					
//					//sprintf( m_csDisplayMsg, "Sorting %d Bags...", bagLog->getCount() );
//					SetPercent( 24 );
//					//SendSimMessage( m_csDisplayMsg );
//					bagLog->sortBags();
//					SetPercent( 25 );
//				}
//
//			}
//			catch( Exception *except )
//			{
//				char string[256];
//				except->getMessage( string );
//				strcpy( m_csDisplayMsg, string );
//				SendSimMessage( m_csDisplayMsg );
//				delete except;
//
//				nPax = 0;
//				bInitializeNoError = false;
//			}
//			catch( ARCException* arc_exception )
//			{
//				SendSimFormatMessage(arc_exception->getFormatErrorMsg() );
//				delete arc_exception;
//
//				nPax = 0;
//				bInitializeNoError = false;
//			}
//			
//			// init air side simulation
//			AirsideSimulation simAirside(pDoc->GetProjectID(),this,_csProjPath,&pFltOperatingDoorSpec) ;
//			if(!m_bFireOccur)
//			{
//				p_simAirside = &simAirside ;
//				AirsideSimConfig airsidesimconf;
//				airsidesimconf.bOnlySimStand = !simEngineConfig()->isSimAirsideMode();
//				simAirside.SetOutputAirside(pDoc->GetOutputAirside());
//				try{				
//					simAirside.m_pOuput->PrepareLog(this,_csProjPath);				
//					//set Airside simEngine requisite data which saved in file
//					simAirside.SetGateAssignmentConstraints(&gateAssignConstraints);
//					simAirside.Initialize(pDoc->GetProjectID(),airsidesimconf);	
//				}
//				catch (Exception *except)
//				{
//					char string[256];
//					except->getMessage (string);
//					strcpy( m_csDisplayMsg, string );
//					SendSimMessage( m_csDisplayMsg );
//					stdcerr << '\n' << string;
//					delete except;
//					bInitializeNoError = false;
//				}
//				catch( CADOException & )
//				{
//					
//					strcpy( m_csDisplayMsg, "There is a database exception raised while init airside engine.");
//					SendSimMessage( m_csDisplayMsg );
//					//stdcerr << '\n' << string;
//					bInitializeNoError = false;
//				}
//			}			
//			
//			Landside_Engine::LandsideEngine landsideengine(*pDoc->GetLandsideDoc()->GetInput(), *pDoc->GetLandsideDoc()->GetOutput());
//
//			//OnBoard simEngine
//			OnBoardSimEngine onBoardSimEngine( pDoc->GetProjectID(),this,_csProjPath );
//			if(simEngineConfig()->isSimOnBoardAlone())
//			{
//				try{
//					onBoardSimEngine.Initialize();
//				}
//				catch(OnBoardSimEngineConfigException* onboard_config_except){
//					OnBoardDiagnose* pDiagnose = 
//						(OnBoardDiagnose*)onboard_config_except->GetDiagnose();
//					DiagnoseDelivery()->DeliveryOnBoardDiagnose(pDiagnose);
//					delete onboard_config_except;
//
//					bInitializeNoError = false;
//				}
//			}
//				
//            ////simEngineCommLayer()->setSimEngineModes( &simAirside, &landsideengine, this, &onBoardSimEngine);
//			
//			if( simEngineConfig()->isSimALTOModes() && bInitializeNoError )
//			{	
//				m_simBobileelemList.Initialize( nPax );
//
//				ElapsedTime startTime, endTime, simTime;
//				EventList simulationEngine;
//				try 
//				{				
//					Event::initEvents (&simulationEngine);
//
//					ElapsedTime estStartTime = m_pSimParam->GetStartTime() - m_pSimParam->GetInitPeriod();
//
//					if (estStartTime < 0l)
//					{
//						estStartTime = 0l;
//					}
//					
//					simAirside.SetSimStartTime(estStartTime);
//					landsideengine.SetSimStartTime(estStartTime);
//
//					ElapsedTime estEndTime = m_pSimParam->GetEndTime() + m_pSimParam->GetFollowupPeriod();
//					simAirside.SetSimEndTime(estEndTime);
//					simulationEngine.SetSimulationEndTime(estEndTime);
//					landsideengine.SetSimEndTime(estEndTime);
//
//					//init airside simulation ,there must have a judge whether user want to run airside simulation ****/					
//					if(!m_bFireOccur)
//						simAirside.AirsideEventListInitialize( &simulationEngine);
//
//					//init onboard simEngine.
//					if(simEngineConfig()->isSimOnBoardMode())
//						onBoardSimEngine.eventListInitialize( &simulationEngine);
//					
//					if(simEngineConfig()->isSimLandsideMode())
//					{
//						//init landside simulation
//						landsideengine.SetFlightSchedule(flightSchedule);
//						landsideengine.SetProcList(&pDoc->GetLandsideDoc()->GetProcessorList());
//						landsideengine.SetFloors(&pDoc->GetFloorByMode(EnvMode_LandSide));
//						landsideengine.Run();
//						
//					}
//
//					// init Mobile Elements Lifecycle Management service.
//					if(simEngineConfig()->isSimOnBoardAlone())
//						MobElementsLifeCycleMgr()->initialize(paxLog, &m_simFlightSchedule);
//					else
//					{//TODO:: Generate passenger's logic has problem in code below. need replace in the future. 
//					 //All pax generation and activation logic implemented in MobElementsLifeCycleMgr object.
//						CFlightPaxLogContainer* container = CFlightPaxLogContainer::GetInstance() ;
//						container->Reset() ;
//						container->SetFlightSchedule(&m_simFlightSchedule) ;
//						container->SetPaxLog(paxLog) ;
//						container->InitContainer() ;
//						ActivePassenger();
//					}
//                   
//					if(simEngineConfig()->isSimTerminalMode())
//						TrainEvent.initList(m_pTrainLog,-1);
//
//					// before run engine, create all resource element
//					long _lResourceElementCount = m_pResourcePoolDB->createResourceElement( this, nPax );
//
//					//bagEvent.initBagList (bagLog);
//					procAssignDB->initEvents();
//
//					procList->writeProcLog();
//
//					//BoardingCallEvent::loadBoardingCalls (procList, &m_simFlightSchedule,	flightData, this);
//					m_pFltsBoardingCallManager->Initialize(procList, &m_simFlightSchedule, flightData, this);		
//
//			//		procList->writeProcLog();
//
//					
//					SetPercent( 27 );
//					SendSimMessage( "Running..." );
//					
//					//#define ECHO_PRE
//					#ifdef ECHO_PRE		
//						//	createEchoFiles(_csProjPath);
//					#endif
//					
//					
//					// generate fire evacutaion event if user click the fire button
//					if( m_bFireOccur )
//					{
//						FireEvacuationEvent* pEvent = new FireEvacuationEvent;						
//						pEvent->setTime( m_timeFireEvacuation );						
//						pEvent->addEvent();						
//					}			
//			
//
//					startTime.getSystemTime();
//					//engine runing
//					simTime = simulationEngine.start( this, &m_bCancel, true );
//
//					//end						
//				}
//				catch (ARCSystemError* system_exception) 
//				{
//					SendSimFormatMessage( system_exception->getFormatErrorMsg() );
//					simulationEngine.kill();
//					delete system_exception;
//				}
//				catch (ARCSimEngineException* sim_exception )
//				{
//					SendSimFormatMessage( sim_exception->getFormatErrorMsg() );
//					simulationEngine.kill();
//					delete sim_exception;
//				}
//				catch ( ARCException * arc_exception )
//				{
//					SendSimFormatMessage( arc_exception->getFormatErrorMsg() );
//					delete arc_exception;
//				}
//				catch (Exception *except)
//				{
//					char string[256];
//					except->getMessage (string);
//					strcpy( m_csDisplayMsg, string );
//					SendSimMessage( m_csDisplayMsg );
//					stdcerr << '\n' << string;
//
//					delete except;
//				}				
//
//				//flush air side log
//				if(!m_bFireOccur)
//				{
//					simAirside.FlushLog();
//					if(simAirside.m_pOuput)
//					{
//						simAirside.m_pOuput->FlushLog(pDoc->GetProjectID());
//						simAirside.m_pOuput->SaveLog(_csProjPath);
//					}
//				}
//				//set the static data member as max time to avoid unnecessary fire evacuation logic
//				Person::SetFireEvacuationTime(ElapsedTime(23*3600+59*60+59l) );
//				m_bFireOccur = false;						
//
//
//				SetPercent( 95 );
//				int nProc = procList->getProcessorCount();
//				sprintf( m_csDisplayMsg, "Number of Processors: %d", nProc );
//				SendSimMessage( m_csDisplayMsg );
//
//
//				for( int i=0; i<iMobleTypeCount; ++i )
//				{
//					sprintf( m_csDisplayMsg, "Number of %s: %d", m_pMobElemTypeStrDB->GetString( i ) , m_vNonPaxCountByType[i]  );
//					SendSimMessage( m_csDisplayMsg );
//				}
//
//				int nTrain = m_pTrainLog->getCount();
//				sprintf( m_csDisplayMsg, "Number of Trains: %d", nTrain );
//				SendSimMessage( m_csDisplayMsg );
//				
//				
//				
//				endTime.getSystemTime();
//				char str[512];
//				(endTime - startTime).printTime( str );
//				sprintf( m_csDisplayMsg, "Simulation duration: %s", str );
//				SendSimMessage( m_csDisplayMsg );
//
//
//				strcpy( m_csDisplayMsg, "Testing flow integrity" );
//				SendSimMessage( m_csDisplayMsg );
//				procList->clearPassengers (simTime);
//
//				SetPercent( 98 );
//				strcpy( m_csDisplayMsg, "Sorting processor logs" );
//				SendSimMessage( m_csDisplayMsg );
//
//				procList->AddAllDynamicCreatedProcIntoList();
//				//when TLOS created processor dynamicly ,we need update proc index in pax log to proc's new index
//				RefreshProcIdxInLogs();
//
//				try {
//					procList->updateProcLog();
//				}
//				catch (ARCSystemError *system_exception)
//				{
//					SendSimFormatMessage( system_exception->getFormatErrorMsg() );
////					cerr << " of post processing";
//					//throw shutDown;
//					delete system_exception;
//				}
//
//				procList->FlushElevatorLog();
//				procList->FlushBridgeLog();
//				procList->ClearStationMovingTrain();
//				FlushMovingTrain();
//
//				
//
//				//save landside logs
//				landsideengine.SaveEvents();
//				pDoc->GetLandsideDoc()->GetOutput()->GetSimLogs().SaveLogs(_csProjPath);
//				
//
//
//				// kill resource element, flush resource element and delete all resource element
//				m_pResourcePoolDB->clearAllPoolEngineData( simTime );
//
//				closeLogs( _csProjPath );
//
//				CSimItem*	pSimItem = m_pSimAndReportManager->getSimItem(sSubName  );
//				ASSERT( pSimItem );
//				pSimItem->setBeginTime( simulationEngine.GetStartTime() );
//				pSimItem->setEndTime( simulationEngine.GetEndTime() );	
//				
//				if( pSim->GetClassType() =="AUTOREPORT" )
//				{
//					CMutiRunReportAgent temp;
//					temp.Init( _csProjPath, this );
//					CSimAutoReportPara* pAutoReport = (CSimAutoReportPara*)pSim;
//					int iReportCount = pAutoReport->GetReportCount();
//					for( i=0; i<iReportCount; ++i )
//					{
//						CReportParameter* pPara =  m_pReportParaDB->GetReportPara( pAutoReport->At( i ) );
//						if( pPara )
//						{
//							temp.AddReportWhatToGen( pPara->GetReportCategory(), pPara ,pDoc->GetFloorByMode( EnvMode_Terminal ).m_vFloors.size());		
//						}
//						
//					}
//					temp.GenerateAll();
//				}		
//
//				//#define ECHO_POST
//				#ifdef ECHO_POST		
//				//	createEchoFiles(_csProjPath);
//				#endif
//			}//	if( nPax > 0 )
//			else
//			{
//				//add by hans to fix the bug that file don't close when simulation done if the simulation don't have a PAX 
//				
//				closeLogs( _csProjPath );
////				pSimItem->setBeginTime(0);
////				pSimItem->setEndTime(0);	
//			}
//
//			SetPercent( 100 );
//			sprintf( m_csDisplayMsg, "End of #%d Simulation Run", iRun+1 );
//			SendSimMessage( m_csDisplayMsg );
//			SendSimMessage( "//=======================================================" );
//
//			// must reload inputs data
//			procAssignDB->loadDataSet( _csProjPath );
//
//			DeleteGreetPoolData();
//			GreetPool.clear();
//
//			//clear MovingTrain
//			ClearMovingTrain();
//			m_simBobileelemList.Clear();
//
//			//------------------------------------------------------------------
//			// release service objects( singleton ) which is effective in single running process.
//			//DeletionManager::UnRegister( MobElementsLifeCycleMgr() );
//			MobElementsLifeCycleMgr()->ReleaseInstance();
//			//DeletionManager::UnRegister( simEngineCommLayer() );
//			//simEngineCommLayer()->ReleaseInstance();
//
//		}// for( int iRun=0; iRun<iRunNumber; ++iRun )
//
//		
//		SetPercentOfRuns( 100 );
//		SetRunNumber( iRunNumber );
//		SendSimMessageOfRuns();
//
//		m_pSimAndReportManager->SetInputModified( false );
//		m_pSimAndReportManager->saveDataSet( _csProjPath, false );
//		
//		if( pTLOSSim ) // TLOS sim
//		{				
//			if( procList->GetDynamicCreatedProcs() && procList->GetDynamicCreatedProcs()->getProcessorCount() > 0 )
//			{
//				inStrDict->storeDictionary (filename);//may create new string entry
//				m_pAutoCreatedProcContainer->addAutoProc( procList->GetDynamicCreatedProcsPrototype(), procList->GetDynamicCreatedProcs() );				
//				CDynamicCreatedProcDlg	dlg( this );
//				dlg.DoModal();
//			}
//
//			CAutoRosterDlg dlg( this, _csProjPath, pDoc  );
//			dlg.DoModal();
//
//			// clear dynamic created processors
//			procList->InitDynamicCreateProcList( this );
//		}
//		// set the current SimResult and update the project bar
//		setCurrentSimResult(0,true);
//		m_pSimAndReportManager->SetCurrentSimResult(0);
//		setPreSimResult( -1 );
//
//
//	}
//	catch( ARCException* arc_exception)
//	{
//		SetPercent( 100 );
//		SendSimFormatMessage( arc_exception->getFormatErrorMsg() );
//
//		SetPercentOfRuns( 100 );
//		SendSimMessageOfRuns();
//
//		delete arc_exception;
//	}
//	catch( Exception *except )
//	{
//		SetPercent( 100 );
//		char string[256];
//		memset(string,0,256);
//		except->getMessage (string);
//		strcpy( m_csDisplayMsg, string );
//		SendSimMessage( m_csDisplayMsg );
//		stdcerr << '\n' << string;
//		
//		SetPercentOfRuns( 100 );
//		SendSimMessageOfRuns();
//
//		delete except;
//	}
//	
//#ifdef CODE_PERFOMANCE_DEBUGE_FLAG
//	CCodeTimeTest::TraceOutResultToFile();
//#endif
//	m_simFlightSchedule.clear();
//
//	// when simulation done, global service objects need to be delete.
//	// such as MobElementsDiagnosis(), simEngineConfig(), DiagnoseDelivery().
//	// or exception occur, some service objects in single simulation running process
//	// such as MobElementsLifeCycleMgr(), OnBoardSimEngine's service objects
//	// is still activated. auto delete these objects. 
//	// Delete all service objects in simEngine.
//	//DeletionManager::DeleteAll();
//
//	MobElementsDiagnosis()->ReleaseInstance();
//	simEngineConfig()->ReleaseInstance();
//	DiagnoseDelivery()->ReleaseInstance();
//
//}
//
////Several binary files are created for storing animation data.
////##ModelId=3E2FAD070357
//void Terminal::createEchoFiles (const CString& _csProjPath)
//{
//    char filename[_MAX_PATH];
//
//    PROJMANAGER->getEchoFileName ( _csProjPath, PaxLogEchoFile, filename);
//    paxLog->echoLogFile (filename, _csProjPath, this );
//
//    PROJMANAGER->getEchoFileName ( _csProjPath, FlightLogEchoFile, filename);
//    flightLog->echoLogFile (filename,_csProjPath, this);
//
//    PROJMANAGER->getEchoFileName ( _csProjPath, ProcLogEchoFile, filename);
//    procLog->echoLogFile (filename, _csProjPath, this);
//
//    PROJMANAGER->getEchoFileName ( _csProjPath, BagLogEchoFile, filename);
//    bagLog->echoLogFile (filename, _csProjPath, this);
//
//    PROJMANAGER->getEchoFileName ( _csProjPath, TrainLogEchoFile, filename);
//    m_pTrainLog->echoLogFile (filename, _csProjPath, this);
//
//    PROJMANAGER->getEchoFileName ( _csProjPath, StringDictEchoFile, filename);
//    inStrDict->storeDictionary (filename);
//}
//
//
//// Assumes ElementList has already been allocated.
//// It generates all mobile elements. Generating starts from all arrivals then fill empty space on departing flights with departing passengers.
//// After that, it sorts mobile elements. All passengers and bags must be sorted by their entry time.
//// Returns number of passengers.
////##ModelId=3E2FAD0703B4
//long Terminal::generatePassengers (FlightSchedule* _pfs)
//{
//	PLACE_TRACK_STRING(_T("{d977f224-8c69-4582-b439-94ddb1a225d9}"));
//	strcpy( m_csDisplayMsg, "Generating Passengers..." );
//	SetPercent( 15 );
//	SendSimMessage( m_csDisplayMsg );
//	
//    // GENERATE ALL MOBILE ELEMENTS
//    // start by generating all arrivals and xfer pax
//    // then fill empty space on departing flights with departing pax
//    //int numFlights = flightSchedule->getCount();
//	int numFlights = _pfs->getCount();
//	CProgressBar bar("Generate Passengers for arrival",100,numFlights,FALSE);
//	
//    for (int i = 0; i < numFlights; i++)
//    {
//		generatePaxOneFlight (i, 'A', _pfs);
//		bar.StepIt();
//	}
//	bar.SetText("Generate Passengers for departure");
//	bar.SetPos(0);
//    for (i = 0; i < numFlights; i++)
//	{
//		generatePaxOneFlight (i, 'D', _pfs);
//		bar.StepIt();	
//	}
//	
//	//bar.SetPos(0);	
//    //// SORT MOBILE ELEMENTS
//    //// all pax and bags must then be sorted by their entry time
//	//sprintf( m_csDisplayMsg, "Sorting %d Mobile Elements...", paxLog->getCount() );
//	//SetPercent( 23 );
//	
//	//SendSimMessage( m_csDisplayMsg );
//    //paxLog->sortPassengers(bar);
//
//	//sprintf( m_csDisplayMsg, "Sorting %d Bags...", bagLog->getCount() );
//	//SetPercent( 24 );
//	//SendSimMessage( m_csDisplayMsg );
//    //bagLog->sortBags();
//	//SetPercent( 25 );
//
//    return paxLog->getCount();
//}
//
//long Terminal::generateMobileAgents(FlightSchedule* _pfs, std::vector<int> _OnBoardFlightIDs)
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
//	sprintf( m_csDisplayMsg, "Sorting %d OnBoard Agents...", paxLog->getCount() );
//	SetPercent( 23 );
//
//	SendSimMessage( m_csDisplayMsg );
//	paxLog->sortPassengers(bar);
//
//	SetPercent( 25 );
//
//	return paxLog->getCount();
//
//}
//
//// It generates all passengers of p_mode type for the flight p_ndx.
//// It uses PassengerDescription tree for the specific flight type to determine breakdown of passenger types.
////##ModelId=3E2FAD0703B5
//void Terminal::generatePaxOneFlight (int p_ndx, char p_mode, const FlightSchedule* _pfs)
//{
//	PLACE_TRACK_STRING(_T("{169291f7-fb80-4928-9d76-e81adb7bfa04}"));
//
//	m_pPaxBulkInfo->initBulk();
//
//	 //Flight* aFlight = flightSchedule->getItem (p_ndx);
//	Flight* aFlight = _pfs->getItem (p_ndx);
//	 
//	 if ((aFlight->getFlightMode() != 'T' && aFlight->getFlightMode() != p_mode) ||
//		  !aFlight->getSpace (p_mode))
//				return;
//
//	 FlightConstraint flightType = aFlight->getType (p_mode);
//	 flightType.SetAirportDB(this->m_pAirportDB);
//
//	 // Determine type distribution tree
//	 const ProbabilityDistribution *breakdown;
//	 breakdown = paxDistList->lookup (flightType);
//	 if (!breakdown)
//	 {
//		  CString sAirID;//[30];
//		  flightType.screenPrint (sAirID, 0, 30);
//		  throw new TwoStringError ("No passenger type distribution for ",
//				sAirID.GetBuffer(0));
//	 }
//
//	 // Generate breakdown of specific passenger types
//	 // (256 is max unique types)   //matt set to 256000
//
//	 
//
//	 int leafCount = inTypeList->getLeafCount();
//	 int * typeCounts = new int[leafCount+1];   //matt
//
//	 for (int i = 0; i < leafCount; i++)
//		  typeCounts[i] = 0;
//
//	 // randomly determine how many of each unique type of pax are on this
//	 // particular flight
//	 int nSpaceCount = aFlight->getSpace(p_mode);
//	 for (i = 0; i < aFlight->getSpace (p_mode); i++)
//		  typeCounts[(int)breakdown->getRandomValue()]++;
//
//	 
//	 for( i=0; i<leafCount; ++i )
//	 {
//		m_vNonPaxCountByType[ 0 ] += typeCounts[i];
//	 }
//
//	 
//	 int branches[MAX_PAX_TYPES];
//	 CMobileElemConstraint  paxType;
//	 paxType.SetInputTerminal( this );
//	 (FlightConstraint &)paxType = flightType;
//	 paxType.SetMobileElementType(enum_MobileElementType_PAX);
//	 ProcessorID _standGate;
//	 _standGate.SetStrDict(inStrDict);
//	 // generate passengers for each leaf in tree
//	 for (i = 0; i < leafCount; i++)
//	 {
//		  inTypeList->getBranchList (i, branches);
//		  paxType.setUserType (branches);
//		  if (p_mode == 'A' && aFlight->isArriving())
//		  {
//			  _standGate.setID(aFlight->getArrStand().GetIDString());
//			if (aFlight->isTurnaround() && typeCounts[i])
//			{
//				int nArrTransit = generateXiting (paxType, p_ndx, typeCounts[i],_standGate, _pfs);
//				int nCount = aFlight->getArrTransit();
//				aFlight->setArrTransit(nCount + nArrTransit) ;
//				typeCounts[i] -= nArrTransit;
//			}
//			if (typeCounts[i])
//			{
//				int nArrTransfer = generateXferring (paxType, p_ndx, typeCounts[i], _standGate, _pfs);
//				int nCount = aFlight->getArrTransfer();
//				aFlight->setArrTransfer(nCount + nArrTransfer) ;
//				typeCounts[i] -= nArrTransfer;
//			}
//			if (typeCounts[i])
//				 generateArrivals (paxType, p_ndx, typeCounts[i], _standGate, _pfs);
//		  }
//		  else if (aFlight->isDeparting() && typeCounts[i])
//		  {
//			  _standGate.setID(aFlight->getDepStand().GetIDString());
//			  generateDepartures (paxType, p_ndx, typeCounts[i],_standGate , _pfs);
//		  }				
//	 }
//	 delete [] typeCounts;
//}
//
//// Percentages of transitting passengers are used for each generating. 
//// The generated passengers are attached to paxLog.
//// Returns number of generated transitting passengers.
//int Terminal::generateXiting ( CMobileElemConstraint  p_type, int p_ndx, int p_count, const ProcessorID& _standGate , const FlightSchedule* _pfs)
//{
//	int iNoCount = 0;
//	
//	 if (!m_pSimParam->GetHubbingFlag())
//		  return 0;
//
//	 Flight *aFlight = _pfs->getItem (p_ndx);
//	 if (aFlight->getDepTime() <= aFlight->getArrTime())
//		  return 0;
//
//	 p_type.makeTransiting();
//	 float percent = m_pHubbing->getTransitPercent (p_type) / (float)100.0;
//
//	int transitCount = 0;
//	for (int i = 0; i < p_count; i++)
//		  if (((double)rand()) / ((double)RAND_MAX) >= 1-percent)
//				transitCount++;
//		  
//	if( percent == 0.0 )
//		transitCount = 0;
//	
//	//int transitCount = p_count-1;
//	if (transitCount > aFlight->getSpace ('D'))
//		  transitCount = aFlight->getSpace ('D');
//
//	 MobLogEntry elem;
//	 int count = 0;
//	 for (; count < transitCount; count += elem.getGroupSize())
//	 {
//		 int nRet =takeBulk( p_type, elem, transitCount - count,aFlight->getArrTime() );
//		 if(nRet<0 )//wan't to takeBulk;
//		 {
//			 initElement (p_type, elem, transitCount - count);
//			 elem.setEntryTime (getLeadTime (p_type, aFlight->getArrTime(), _standGate ));
//		 }
//         else if(nRet==0) //can't to takeBulk;
//		 {
//			 initElement (p_type, elem, transitCount - count);
//			 elem.setEntryTime (getLeadTime (p_type, aFlight->getArrTime(), _standGate ));
//			 //continue;
//		 }
//		  // set pointer to flight in flight list
//		  elem.setArrFlight (p_ndx);
//		  elem.setDepFlight (p_ndx);
//		  int nGroupSize = elem.getGroupSize();
//		  aFlight->addDepPax ( nGroupSize );
//		  int nCount = aFlight->getDepTransit();
//		  aFlight->setArrTransit(nCount + nGroupSize);
//
//		  //set the speed,temporality not know the number of the baggages
//		  elem.setSpeed( (float)getSpeed( p_type ) );	
//
//		  //////////////////////////////////////////////////add Nopax
//          
//		  std::vector<BOOL> vNPFlag = m_pSimParam->GetNoPaxDetailFlag();
//		  int iVisitorFlag = m_pSimParam->GetVisitorsFlag();
//		  MobLogEntry* m_pNopax = new MobLogEntry[m_pMobElemTypeStrDB->GetCount()];
//
//		  //generate all nopax
//		  int iMobileCount = m_pMobElemTypeStrDB->GetCount();
//		  long lCurID = elem.getID() + elem.getGroupSize();
//		  for( int i=1; i<iMobileCount; i++ )
//		  {
//			  bool bGenFlag = TRUE;
//			  if( i <= NOPAX_COUNT && !vNPFlag[i-1] || !iVisitorFlag )
//				  bGenFlag = FALSE;
//			  int iNopaxCount = GenerateNopaxs( p_type, &m_pNopax[i], &elem, i, lCurID, bGenFlag ,_standGate, _pfs);
//			  lCurID += iNopaxCount;
//			  iNoCount += iNopaxCount;
//		  }
//
//		  //reset the speed,according to the number of the baggages
//		  int nCountBags = elem.getBagCount();
//		  if (nCountBags > 0)
//		  {
//			  elem.setSpeed( (float)getSpeed( p_type ) );		  
//		  }
//		  else
//			  elem.setSpeed( (float)getSpeedWithoutBags(p_type) );
//		  
//		  //save pax and nopax log to paxlog
//		  for( i=0; i< elem.getGroupSize(); i++ )
//		  {
//			  paxLog->addItem(elem);
//			  elem.incPax();
//		  }
//		  
//		  for( i=1; i<iMobileCount; i++ )
//		  {
//			  for( int j =0; j < m_pNopax[i].getGroupSize(); j++ )
//			  {
//				  if(m_pNopax[i].GetMobileType() == 2 && m_pNopax[i].getOwnStart())
//				  {
//					  ElapsedTime gateTime = (_pfs->getItem( elem.getFlightIndex()))->getArrTime();
//					  CMobileElemConstraint p_notype(p_type);
//					  p_notype.SetTypeIndex(2);
//					  p_notype.SetMobileElementType(enum_MobileElementType_NONPAX);
//					  m_pNopax[i].setEntryTime( getLeadTime( p_notype, gateTime, _standGate ));
//					  m_pNopax[i].setGroupMember(0);
//				  }
//				  paxLog->addItem(m_pNopax[i]);
//				  m_pNopax[i].incPax();
//				  elem.incPax();
//			  }
//		  }
//          delete []m_pNopax;
//		  ///////////////////////////////////////////////////////////////////////
//	//	  for (int j = 0; j < nGroupSize; j++)
//	//		  {
//	//				paxLog->addItem (elem);
//	//				elem.incPax();
//	//		  }
//	
//	 }
//	 return count;
//	 //return count + iNoCount;
//}
//
//// Percentages of transferring passengers are used for each generating. 
//// The generated passengers are attached to paxLog.
//// Returns number of generated trasferring passengers.
////##ModelId=3E2FAD0703C7
//int Terminal::generateXferring (CMobileElemConstraint p_type, int p_ndx, int p_count, const ProcessorID& _standGate,
//								const FlightSchedule* _pfs)
//{
//	int iNoCount=0;
//	if (!m_pSimParam->GetHubbingFlag())
//		return 0;
//
//	//Flight *depFlight, *aFlight = flightSchedule->getItem (p_ndx);
//	Flight *depFlight, *aFlight = _pfs->getItem (p_ndx);
//	float percent = m_pHubbing->getTransferPercent (p_type) / (float)100.0;
//	//Jan : P(A|B) = P(AB)/P(B); A : X=transfer; B : X!=transit
//	float tmpPercent = m_pHubbing->getTransitPercent (p_type) / (float)100.0;
//	percent = percent / (1-tmpPercent);
//
//	int transferCount = 0;
//	int tmpCount = int(p_count * tmpPercent);
//	p_count -= tmpCount;
//	for (int i = 0; i < p_count; i++)
//		if (((double)rand()) / ((double)RAND_MAX) >= 1-percent)
//			transferCount++;
//	//int transferCount = p_count-1;
//
//	p_type.makeTransferring();
//	MobLogEntry elem;
//	int count = 0;
//	for (; count < transferCount; count += elem.getGroupSize())
//	{
//		depFlight = m_pHubbing->getDepartingFlight ( p_type, p_ndx, &m_simFlightSchedule );
//		if (!depFlight)     // no departing flights avail
//			return count;
//
//		int nMaxPaxGenerated = depFlight->getSpace( 'D' );
//		nMaxPaxGenerated = min( nMaxPaxGenerated, transferCount - count );
//		int nRet =takeBulk( p_type, elem, transferCount - count,aFlight->getArrTime());
//		if(nRet<0)
//		{
//			initElement (p_type, elem, nMaxPaxGenerated);
//			// set the entry time
//			elem.setEntryTime (getLeadTime (p_type, aFlight->getArrTime(), _standGate));
//		}
//		else if(nRet==0)
//		{
//			initElement (p_type, elem, nMaxPaxGenerated );
//			// set the entry time
//			elem.setEntryTime (getLeadTime (p_type, aFlight->getArrTime(), _standGate));
//		}
//
//		// set pointer to flight in flight list
//		elem.setArrFlight (p_ndx);
//		//elem.setDepFlight (this->flightSchedule->findItem (depFlight));
//		elem.setDepFlight (_pfs->findItem (depFlight));
//
//		int nCountBag = elem.getBagCount();
//		if (nCountBag > 0)
//		{
//			elem.setSpeed( (float)getSpeed( p_type ) );		
//		}
//		else
//			elem.setSpeed( (float)getSpeedWithoutBags(p_type) );		  
//
//		//		  elem.setVisitor (-1);
//
//		int nGroupSize = elem.getGroupSize();
//		depFlight->addDepPax ( nGroupSize );
//		int nCount = depFlight->getDepTransfer();
//		depFlight->setDepTransfer(nCount + nGroupSize) ;
//
//
//
//		//////////////////////////////////////////////////add Nopax
//
//		std::vector<BOOL> vNPFlag = m_pSimParam->GetNoPaxDetailFlag();
//		int iVisitorFlag = m_pSimParam->GetVisitorsFlag();
//		MobLogEntry* m_pNopax = new MobLogEntry[m_pMobElemTypeStrDB->GetCount()];
//
//		//generate all nopax
//		int iMobileCount = m_pMobElemTypeStrDB->GetCount();
//		long lCurID = elem.getID() + elem.getGroupSize();
//		//if(lCurID == 0 || lCurID == 1 || lCurID == 2)
//		//{
//		// CString str = _T("Test");
//		//}
//
//		//need to set departure flight id to be -1,
//		//then nopax can set the entry time to be based on arrived flight
//		int nDepFlightID = elem.getDepFlight();
//		elem.setDepFlight(-1);
//
//		for( int i=1; i<iMobileCount; i++ )
//		{
//			bool bGenFlag = TRUE;
//			if( i <= NOPAX_COUNT && !vNPFlag[i-1] || !iVisitorFlag )
//				bGenFlag = FALSE;
//
//			// need to update the p_type
//			CMobileElemConstraint xferType = p_type;			
//			xferType.initFlightType( depFlight->getLogEntry(), 'D' );
//			xferType.initOtherFlightType( aFlight->getLogEntry() );
//
//			int iNopaxCount = GenerateNopaxs( xferType, &m_pNopax[i], &elem, i, lCurID, bGenFlag ,_standGate, _pfs);
//			m_pNopax[i].setDepFlight(nDepFlightID);
//
//			lCurID += iNopaxCount;
//			iNoCount += iNopaxCount;
//		}
//		elem.setDepFlight(nDepFlightID);
//
//		//save pax and nopax log to paxlog
//		for( i=0; i< elem.getGroupSize(); i++ )
//		{
//			paxLog->addItem(elem);
//			elem.incPax();
//		}
//
//		elem.setDepFlight(-1);
//		for( i=1; i<iMobileCount; i++ )
//		{
//// 			for( int j =0; j < m_pNopax[i].getGroupSize(); j++ )
//// 			{
//// 				paxLog->addItem(m_pNopax[i]);
//// 				m_pNopax[i].incPax();
//// 				elem.incPax();
//// 			}
//			for( int j =0; j < m_pNopax[i].getGroupSize(); j++ )
//			{
//				if(m_pNopax[i].GetMobileType() == 2 && m_pNopax[i].getOwnStart())
//				{
//					CMobileElemConstraint xferType = p_type;			
//					xferType.initFlightType( depFlight->getLogEntry(), 'D' );
//					xferType.initOtherFlightType( aFlight->getLogEntry() );
//					ElapsedTime gateTime = (_pfs->getItem( elem.getFlightIndex()))->getArrTime();
//				//	CMobileElemConstraint p_notype(p_type);
//					xferType.SetTypeIndex(2);
//					xferType.SetMobileElementType(enum_MobileElementType_NONPAX);
//					m_pNopax[i].setEntryTime( getLeadTime( xferType, gateTime, _standGate ));
//					m_pNopax[i].setGroupMember(0);
//				}
//				paxLog->addItem(m_pNopax[i]);
//				m_pNopax[i].incPax();
//				elem.incPax();
//			}
//		}
//		elem.setDepFlight(nDepFlightID);
//		delete []m_pNopax;
//		///////////////////////////////////////////////////////////////////////
//		//
//		//		  for (int j = 0; j < nGroupSize; j++)
//		//		  {
//		//				paxLog->addItem (elem);
//		//				elem.incPax();
//		//		  }
//
//	}
//
//	return count;
//	//return count +iNoCount;
//}
//
////It generates all arriving pax for the flight p_ndx.
////Greeters are generated for each arriving passenger.
//void Terminal::generateArrivals ( CMobileElemConstraint p_type, int p_ndx, int p_count, const ProcessorID& _standGate, 
//								 const FlightSchedule* _pfs)
//{
//	PLACE_TRACK_STRING(_T("{1db85d0c-d2b7-426b-acd0-333d338c16f4}"));
//	//Flight *aFlight = flightSchedule->getItem (p_ndx);
//	Flight *aFlight = _pfs->getItem (p_ndx);
//	p_type.makeArrival();
//	MobLogEntry elem;
//	//for all nopax log
//	MobLogEntry* m_pNopax = new MobLogEntry[m_pMobElemTypeStrDB->GetCount()];
//	//MobLogEntry visitor;
//	for (int count = 0; count < p_count; count += elem.getGroupSize())
//	{
//		int nRet=takeBulk(p_type, elem, p_count - count,aFlight->getArrTime());
//		if(nRet<0)
//		{
//			initElement (p_type, elem, p_count - count);
//			// set the entry time
//			elem.setEntryTime (getLeadTime (p_type, aFlight->getArrTime(), _standGate));
//		}
//		else if(nRet==0)
//		{
//			initElement (p_type, elem, p_count - count);
//			// set the entry time
//			elem.setEntryTime (getLeadTime (p_type, aFlight->getArrTime(), _standGate));
//		}
//
//		// set pointer to flight in flight list
//		elem.setArrFlight (p_ndx);
//
//		// generate all bags
//		elem.setBagCount (createBags (p_type, elem, _pfs));
//
//		//reset cart count
//		elem.setCartCount (getCarts (p_type, elem.getGroupSize(), elem.getBagCount())); //Jan98
//
//		int nCountBag = elem.getBagCount();
//		if (nCountBag > 0)
//		{
//			elem.setSpeed( (float)getSpeed( p_type ) );
//		}
//		else
//			elem.setSpeed( (float)getSpeedWithoutBags(p_type) );
//		
//		//		  elem.setVisitor (-1);
//		elem.SetmobileType( 0 );
//
//
//
//		std::vector<BOOL> vNPFlag = m_pSimParam->GetNoPaxDetailFlag();
//		int iVisitorFlag = m_pSimParam->GetVisitorsFlag();
//
//		//generate all nopax
//		int iMobileCount = m_pMobElemTypeStrDB->GetCount();
//		long lCurID = elem.getID() + elem.getGroupSize();
//		for( int i=1; i<iMobileCount; i++ )
//		{
//			bool bGenFlag = TRUE;
//			if( i <= NOPAX_COUNT && !vNPFlag[i-1] || !iVisitorFlag )
//				bGenFlag = FALSE;
//			int iNopaxCount = GenerateNopaxs( p_type, &m_pNopax[i], &elem, i, lCurID, bGenFlag , _standGate, _pfs);
//			lCurID += iNopaxCount;
//		}
//
//		//save pax log to paxlog
//		for( i=0; i< elem.getGroupSize(); i++)
//		{
//			paxLog->addItem(elem);
//			elem.incPax();
//		}
//
//		for( i=1; i<iMobileCount; i++ )
//		{
//			for( int j =0; j < m_pNopax[i].getGroupSize(); j++ )
//			{
//				m_pNopax[i].setArrFlight(p_ndx);
//				if(m_pNopax[i].GetMobileType() == 2 && m_pNopax[i].getOwnStart())
//				{
//					ElapsedTime gateTime = (_pfs->getItem( elem.getFlightIndex()))->getArrTime();
//					CMobileElemConstraint p_notype(p_type);
//					p_notype.SetTypeIndex(2);
//					p_notype.SetMobileElementType(enum_MobileElementType_NONPAX);
//					m_pNopax[i].setEntryTime( getLeadTime( p_notype, gateTime, _standGate ));
//					m_pNopax[i].setGroupMember(0);
//				}
//				paxLog->addItem(m_pNopax[i]);
//				m_pNopax[i].incPax();
//				elem.incPax();
//			}
//		}
//	}
//
//	 delete []m_pNopax;
//}
//
//
////It generates all departing pax for the flight p_ndx.
////Senders are generated for each departing passenger.
//void Terminal::generateDepartures (CMobileElemConstraint p_type,
//	 int p_ndx, int p_count, const ProcessorID& _standGate, const FlightSchedule* _pfs)
//{
//	
//	//Flight* aFlight = flightSchedule->getItem (p_ndx);
//	Flight* aFlight = _pfs->getItem (p_ndx);
//	p_type.makeDeparture();
//	MobLogEntry elem;
//	//for all nopax log
//	MobLogEntry* m_pNopax = new MobLogEntry[m_pMobElemTypeStrDB->GetCount()];
//	//MobLogEntry visitor;
//	for (int count = 0; count < p_count; count += elem.getGroupSize())
//	{
//		int nRet = takeBulk( p_type, elem, p_count - count,aFlight->getDepTime());
//		if(nRet<0)
//		{
//			initElement (p_type, elem, p_count - count);
//			// set the entry time
//			elem.setEntryTime (getLeadTime (p_type, aFlight->getDepTime(), _standGate));
//		}
//		else if(nRet==0)
//		{
//			continue;
//		//	initElement (p_type, elem, p_count - count);
//			// set the entry time
//		//	elem.setEntryTime (getLeadTime (p_type, aFlight->getDepTime(), _standGate));
//		}
//		aFlight->addDepPax (elem.getGroupSize());
//
//		// set pointer to flight in flight list
//		elem.setDepFlight (p_ndx);
//
//
//		// generate all bags
//		elem.setBagCount ((!m_pSimParam->GetBaggageFlag())? 0:
//			getBags (p_type, elem.getGroupSize()));
//
//		//reset cart count
//		elem.setCartCount (getCarts (p_type, elem.getGroupSize(), elem.getBagCount())); //Jan98
//
//		//set the speed ,temporality not know the number of the baggages
//		elem.setSpeed( (float)getSpeed( p_type ) );
//
//		//		  elem.setVisitor (-1);
//		elem.SetmobileType( 0 );
//
//		  
//		std::vector<BOOL> vNPFlag = m_pSimParam->GetNoPaxDetailFlag();
//		int iVisitorFlag = m_pSimParam->GetVisitorsFlag();
//
//		//generate all nopax
//		int iMobileCount = m_pMobElemTypeStrDB->GetCount();
//		long lCurID = elem.getID() + elem.getGroupSize();
//		for( int i=1; i<iMobileCount; i++ )
//		{
//			bool bGenFlag = TRUE;
//			if( i <= NOPAX_COUNT && !vNPFlag[i-1] || !iVisitorFlag )
//				bGenFlag = FALSE;
//			int iNopaxCount = GenerateNopaxs( p_type, &m_pNopax[i], &elem, i, lCurID, bGenFlag ,_standGate, _pfs);
//			lCurID += iNopaxCount;
//		}
//
//		//reset the speed according to the number of the baggages
//		int nCountBag = elem.getBagCount();
//		if (nCountBag > 0)
//		{
//			elem.setSpeed( (float)getSpeed( p_type ) );
//		}
//		else
//			elem.setSpeed( (float)getSpeedWithoutBags(p_type));
//
//		//save pax and nopax log to paxlog
//		for( i=0; i< elem.getGroupSize(); i++ )
//		{
//			paxLog->addItem(elem);
//			elem.incPax();
//		}
//
//		for( i=1; i<iMobileCount; i++ )
//		{
// 			for( int j =0; j < m_pNopax[i].getGroupSize(); j++ )
// 			{
// 				paxLog->addItem(m_pNopax[i]);
// 				m_pNopax[i].incPax();
// 				elem.incPax();
// 			}
//		}
//	}
//	delete []m_pNopax;
//}
//// if a bulk can't service for passenger (p_type), return -1; but can't 
//int Terminal::takeBulk(CMobileElemConstraint p_type, MobLogEntry& p_elem, int p_max,ElapsedTime& flyTime)
//{
//	int nBulk=m_pPaxBulkInfo->findFitBulk(p_type);
//	//can't find a proper bulk
//	if(nBulk == -1) return -1;
//
//	// set group size, limited to number of passengers of that p_type
//	p_elem.setGroupSize ( getGroupSize (p_type, p_max));
//	p_elem.setGroupMember (0);
//    ElapsedTime time;
//
//	int nIndex = m_pPaxBulkInfo->findValuedBulk(time,p_elem.getGroupSize(),p_type,flyTime);
//	if(nIndex == -1) return 0;
//
//	//CPaxBulk& aBulk = m_pPaxBulkInfo->m_vectBulkInfo[nIndex];
//	//all shuttle buses have leave, or remaining buses is too later.
//	//if( aBulk.takeCurrentBulk( time , p_elem.getGroupSize(),flyTime)==false ) return 0;
//
//	 // set identification number
//	 p_elem.setID (paxLog->getCount());
//
//	 // set the entry time
//	 p_elem.setEntryTime ( time );
//	 
//	 
//	 // set passenger type
//	 p_elem.setIntrinsic (p_type.getIntrinsic());
//	 
//	 int types[MAX_PAX_TYPES];
//	 p_type.getUserType (types);
//	 p_elem.setPaxType (types);
//	 
//	 // generate all carts
//	 // carts cannot exceed number of bags
//	 p_elem.setCartCount (getCarts (p_type, p_elem.getGroupSize(), p_elem.getBagCount()));
//	 
//	 return 1;
//}
//
//// It initializes data common to all intrinsic types.
//// Group size is limited to number of passengers of that p_type.
//// Carts are generated for passenger.
////##ModelId=3E2FAD0703B8
//void Terminal::initElement (CMobileElemConstraint p_type, MobLogEntry& p_elem, int p_max)
//{
//	 // set identification number
//	long lId = paxLog->getCount();
//
//	if(lId == 30)
//	{
//		int a = 0;
//	}
//	 p_elem.setID ( lId );
//
//	 // set group size, limited to number of passengers of that p_type
//	 p_elem.setGroupSize (getGroupSize (p_type, p_max));
//	 p_elem.setGroupMember (0);
//
//	 // set passenger type
//	 p_elem.setIntrinsic (p_type.getIntrinsic());
//
//	 int types[MAX_PAX_TYPES];
//	 p_type.getUserType (types);
//	 p_elem.setPaxType (types);
//
//	 // generate all carts
//	 // carts cannot exceed number of bags
//	 p_elem.setCartCount (getCarts (p_type, p_elem.getGroupSize(), p_elem.getBagCount()));
//
//}
//
//int Terminal::GenerateNopaxs( CMobileElemConstraint p_type, MobLogEntry* p_nopax, 
//							 MobLogEntry *p_pax, int i_index, long _lCurID, bool _bGenFlog , 
//							 const ProcessorID& _standGate, const FlightSchedule* _pfs)
//{
//	const ConstraintEntryEx* constEntry;
//	constEntry = (const ConstraintEntryEx*)( paxDataList->getNopaxData(i_index)->FindFitEntry( p_type ) );
//	if (!constEntry)
//	{
//		char str1[1024];
//		//char str2[256];
//		CString str2;
//		p_type.screenPrint( str2, 0, 256 );		
//		sprintf( str1, "No %s count defined for passenger: %s", m_pMobElemTypeStrDB->GetString( i_index ), str2.GetBuffer(0) );
//		throw new StringError ( str1 );
//	}
//	const ProbabilityDistribution *visitorDist;
//	visitorDist = constEntry->getValue();
//	bool flag = constEntry->getFlag();	
//	// if flag == true we shall generate visitorCount no_pax for each pax, so to all group pax, we shall generate visitorCount*p_pax.getGroupSize() no_pax
//	// else we we shall generate visitorCount no_pax for group pax
//	int visitorCount = (int)(visitorDist->getRandomValue());
//	if( flag )
//		visitorCount *= p_pax->getGroupSize();		
//
//	if( !_bGenFlog || !visitorCount )
//	{
//		p_nopax->setGroupSize( 0 );
//
//		// add by tutu in 2003-3-4
//		if( i_index == 2 )// is check bags
//		{
//			p_pax->setBagCount( 0 );
//		}
//		return 0;
//	}
//
//	*p_nopax = *p_pax;							// copy the owner's pax_log.
//	p_nopax->setEachPaxOwnNopax( flag );
//	p_nopax->setID( _lCurID );					// set id next to the owner's id
//	p_nopax->setOwner( p_pax->getID() );		// set owner id
//	p_nopax->setGroupSize( visitorCount );		// set no_pax count
//	p_nopax->setGroupMember( 0 );				// set index in the group
//
//	//set passenger type
//	p_nopax->SetmobileType( i_index );			// set which mobile type.
//
//	int types[MAX_PAX_TYPES];		
//	p_type.getUserType(types);					// get owner's user type.
//	p_nopax->setPaxType(types);					// set nopax's log's pax type = owner's pax type
//	
//	//check if there is the sender.
//	p_type.SetTypeIndex( i_index );
//	p_type.SetMobileElementType(enum_MobileElementType_NONPAX);
//
//	p_nopax->setSpeed((float)getSpeed(p_type));
//
//	// add by tutu in 2003-3-4
//	
//	ASSERT( i_index > 0 && i_index < m_pMobElemTypeStrDB->GetCount() );
//	
//	m_vNonPaxCountByType[ i_index ] += visitorCount;
//	
//
//	if( i_index == 2 )// is check bags
//	{
//		p_pax->setBagCount( visitorCount );
//	}
//	
//	if( m_pPassengerFlowDB->HasOwnStart( p_type))
//	{
//		// set entry time for nopax with start.
//		//ElapsedTime gateTime = (flightSchedule->getItem( p_pax->getFlightIndex()))->getArrTime();
//		ElapsedTime gateTime = (_pfs->getItem( p_pax->getFlightIndex()))->getArrTime();
//		p_nopax->setEntryTime( getLeadTime( p_type, gateTime, _standGate ));
//	}
//	else
//	{
//		p_nopax->setOwnStart( false );
//	}
//
//	return p_nopax->getGroupSize();
//}
//
// /*
//int Terminal::GenerateVisitorters( CMobileElemConstraint _paxType, MobLogEntry* _pVisitorLogEntry, MobLogEntry* _pPaxLogEntry )
//{
//	if( !m_pSimParam->GetVisitorsFlag() )
//		return 0;
///*
//	 const ProbabilityDistribution *visitorDist;
//	 visitorDist = (paxDataList->getVisitors())->lookup (p_type);
//	 if (!visitorDist)
//	 {
//		  char str[128];
//		  p_type.WriteSyntaxString (str);
//		  throw new TwoStringError ("no visitor distribution for ", str);
//	 }
//
//	 int visitorCount = (int)(visitorDist->getRandomValue());
//	 if (!visitorCount)
//		  return FALSE;
//*/
///*
//	int nVisitorCount = 1;
//	*_pVisitorLogEntry = *_pPaxLogEntry;	// copy the owner's paxlog.
//	_pVisitorLogEntry->setID( _pPaxLogEntry->getID() + _pPaxLogEntry->getGroupSize() );	// set id next to the owner's id
//	_pVisitorLogEntry->setVisitor( _pPaxLogEntry->getID() );	// set owner id
//	_pVisitorLogEntry->setGroupSize( nVisitorCount );	// set visitor count
//	_pVisitorLogEntry->setGroupMember( 0 );		// set index in the group.
//
//	// set passenger type
//	_pVisitorLogEntry->SetmobileType( 1 );		// set which mobile type.
//
//	int types[MAX_PAX_TYPES];
//	_paxType.getUserType (types);			// get owner's user type.
//	_pVisitorLogEntry->setPaxType (types);	// set visitor's log's pax type = owner's pax type.
//
//	// check if there is the sender.
//	_paxType.SetTypeIndex( 1 );
//	if( m_pPassengerFlowDB->HasOwnStart( _paxType ) )
//	{
//		// set entry time for visitor with start.
//		ElapsedTime gateTime = (flightSchedule->getItem(_pPaxLogEntry->getFlightIndex()))->getArrTime();
//		_pVisitorLogEntry->setEntryTime (getLeadTime (_paxType, gateTime));
//	}
//	return TRUE;	
//}
//
//*/
//
//
//
//
//// Calculates passenger group size according to user definition.
//int Terminal::getGroupSize (CMobileElemConstraint p_type, int p_max)
//{
//    const ProbabilityDistribution *groupSize;
//    groupSize = (paxDataList->getGroups())->FindFit (p_type);
//    if (!groupSize)
//    {
//        //char str[128]; matt
//		char str[2560];
//        p_type.WriteSyntaxStringWithVersion (str);
//		  throw new TwoStringError ("no group size distribution for ", str);
//    }
//    int group = max(((int)(groupSize->getRandomValue())),1);
//    if (!group)
//        throw new StringError (" group size distribution evaluated to zero");
//    return min(group, p_max);
//}
//
////##ModelId=3E2FAD08002D
//ElapsedTime Terminal::getLeadTime (CMobileElemConstraint p_type, ElapsedTime p_time , const ProcessorID& _standGate)
//{
//    const ProbabilityDistribution *leadLagTime;
//	if( p_type.isArrival() && p_type.GetTypeIndex() == 2 )
//	{
//		if( bagGateAssignDB->IfGateHasHigherPriority() )
//		{
/////			// TRACE ("\n%s\n",_standGate.GetIDString() );
//			int iBestIdx = bagData->getGateLagTimeDB()->findBestMatch( _standGate );
//			if( iBestIdx < 0 )
//				iBestIdx = 0;
//			if( iBestIdx >= 0 )
//			{
//				CGageLagTimeData* pData = (CGageLagTimeData*) (bagData->getGateLagTimeDB()->getItem( iBestIdx ) );
//				leadLagTime = pData->GetDistribution();
//			}
//			else
//			{
//				//throw new StringError ("Can not find baggage lead/lag time when gate has higher priority !");
//				throw new TwoStringError ("Can not find baggage lead/lag time when gate has higher priority !", _standGate.GetIDString());
//			}
//		}
//		else
//		{
//			leadLagTime = bagData->getLagTimeDatabase()->lookup( p_type );
//		}
//		
//	}
//	else
//	{
//		leadLagTime = (paxDataList->getLeadLagTime())->lookup_with_procid(p_type,_standGate);
//	}
//    
//    if (!leadLagTime)
//    {
//        CString strPrint;//char str[128];
//		p_type.screenPrint( strPrint, 0, 128 );
//        throw new TwoStringError ("no lead/lag time for ", strPrint.GetBuffer(0));
//    }
//
//    ElapsedTime leadTime = (float)(60.0 * leadLagTime->getRandomValue());
//    leadTime += p_time;
//    return (leadTime < (ElapsedTime)0l)? (ElapsedTime) 0l: leadTime;
//}
//
////Calculates bags for p_group with p_type and returns count.
////##ModelId=3E2FAD08003E
//int Terminal::getBags (CMobileElemConstraint p_type, int p_group)
//{
//    /*const ProbabilityDistribution *bagDist;
//    bagDist = (paxDataList->getBags())->lookup (p_type);
//    if (!bagDist)
//    {
//        char str[128];
//        p_type.WriteSyntaxString (str);
//        throw new TwoStringError ("no bag count distribution for ", str);
//    }
//    return p_group * (int)(bagDist->getRandomValue());*/
//	return 0;
//}
//
////Creates bags and attachs them to bagLog.
////Sets bag id, attaches pax to it, assignes flight to it, attachs it baggage carosel, set entry time.
////Returns number of bags in p_type.
//
//
////##ModelId=3E2FAD08004B
//int Terminal::createBags (CMobileElemConstraint p_type, MobLogEntry& p_elem, const FlightSchedule* _pfs)
//{
//    if (!m_pSimParam->GetBaggageFlag())
//        return 0;
//    int bagCount = getBags (p_type, p_elem.getGroupSize());
//
//    BagLogEntry bag;
//    bag.setID (0);
//    bag.setPax (p_elem.getID());
////	// TRACE("Set Pax:  %d;   Get Pax:  %d\n", p_elem.getID(), bag.getPax());
//	
//
//    bag.setFlight (p_elem.getArrFlight());
//
//    //Flight* aFlight = flightSchedule->getItem (bag.getFlight());
//	Flight* aFlight = _pfs->getItem (bag.getFlight());
//    bag.setProc (aFlight->getBaggageCarosel());
//    bag.setExitTime ((ElapsedTime)0l);
//    ElapsedTime gateTime (aFlight->getArrTime());
//
//    if (bag.getProc() != -1)
//        for (int i = 0; i < bagCount; i++)
//        {
//			ElapsedTime lagTime = bagData->getBagLagTime(bag.getFlight(), &m_simFlightSchedule );
//            bag.setEntryTime (gateTime+lagTime);
//			bag.SetLagTime(lagTime);
//            bagLog->addItem (bag);
//        }
//    return bagCount;
//}
//
////Number of carts could be greater than bags. 
////Returns number carts.
////##ModelId=3E2FAD080059
//int Terminal::getCarts (CMobileElemConstraint p_type, int p_group, int p_bags)
//{
//    /*const ProbabilityDistribution *cartDist;
//    cartDist = (paxDataList->getCarts())->lookup (p_type);
//    if (!cartDist)
//    {
//        char str[128];
//        p_type.WriteSyntaxString (str);
//		  throw new TwoStringError ("no cart distribution for ", str);
//	 }
//
//	 //Jan98: if the first selected value of cart > bags, to pick up one just
//	 //less than no. of bags.
//	 int cartCount = (int)(cartDist->getRandomValue());
//	 int tmpBags = p_bags / p_group;
//	 if (cartCount>tmpBags)
//	 {
//		cartCount = 0;
//		int loop = 0;
//		while(++loop<=50)  //define 50 times loop to obtain a proper cart count
//		{
//			int tmp = (int)(cartDist->getRandomValue());
//			if (tmp<=tmpBags)
//				cartCount = max(tmp, cartCount);
//		}
//	 }	  
//
//	 #if 0
//	 //Jan98 : the code in enclosed #if ~ #endif is old;
//	 for (int i = 0; i < p_group; i++)
//		  cartCount += (int)(cartDist->getRandomValue());
//	 return min(cartCount,p_bags);
//	 #endif
//
//	 return p_group*cartCount;*/ //set the total number of carts to a group
//	return 0;
//}
//
//
////##ModelId=3E2FAD080069
//DistanceUnit Terminal::getSpeed( CMobileElemConstraint p_type )
//{
//    CMobileElemConstraintDatabase *speedDB = paxDataList->getSpeed();
//    const ProbabilityDistribution* speedDistr = speedDB->FindFit (p_type);
//    if (speedDistr == NULL)
//        throw new StringError ("No speed available");
//
//	CMobileElemConstraintDatabase* pImpactSpeedDB = paxDataList->getImpactSpeed();
//	int nCount = pImpactSpeedDB->getCount();
//	double dImpact = 0;
//	for (int i = 0; i < nCount; i++)//find the matching impact speed of the this passenger type
//	{
//		ConstraintEntry* pImpactEntry = pImpactSpeedDB->getItem(i);
//		const CMobileElemConstraint* pImpactType = (CMobileElemConstraint*)pImpactEntry->getConstraint();
////		if (p_type.isEqual(pImpactType))
//		{
//			const ProbabilityDistribution* pImpactSpeed = pImpactEntry->getValue();
//			dImpact = pImpactSpeed->getMean()/100;
////			break;
//		}
//
//	}
//
//	DistanceUnit speed;
//	if (dImpact ==0)
//	{
//		speed = (DistanceUnit)speedDistr->getRandomValue();
//	}
//	else 
//		speed = ((DistanceUnit)speedDistr->getRandomValue()) * (1 - dImpact);
//	
//	return speed;
//}
////get the speed of the passenger without bags
//DistanceUnit Terminal::getSpeedWithoutBags( CMobileElemConstraint p_type)
//{
//	CMobileElemConstraintDatabase *speedDB = paxDataList->getSpeed();
//	const ProbabilityDistribution* speedDistr = speedDB->FindFit (p_type);
//	if (speedDistr == NULL)
//		throw new StringError ("No speed available");
//
//	DistanceUnit speed;
//	speed = (DistanceUnit)speedDistr->getRandomValue();
//
//	return speed;
//}
//
//void Terminal::SendSimMessage(const CString &_csMsg)
//{
//	if(!::IsWindow(m_hWnd))
//		return;
//
//	strcpy( m_csDisplayMsg, _csMsg );
//	SendMessage( m_hWnd, WM_SIM_STATUS_MSG, GetPercent(), (long)m_csDisplayMsg );
//}
//
//void Terminal::SendSimFormatExMessage( FORMATMSGEX& _formatMsgEx  )
//{
//	if(!::IsWindow(m_hWnd))
//		return;
//
//	_formatMsgEx.diag_entry.iSimResultIndex = m_nRunNumber-1;
//	SendMessage( m_hWnd, WM_SIM_STATUS_FORMATEX_MSG, GetPercent(), (long)& _formatMsgEx );
//}
//
//void Terminal::SendSimFormatMessage( const FORMATMSG& _csFormatMsg )
//{
//	FORMATMSGEX msgEx;
//
//	msgEx.strTime			= _csFormatMsg.strTime;
//	msgEx.strMsgType		= _csFormatMsg.strMsgType;
//	msgEx.strProcessorID	= _csFormatMsg.strProcessor;
//
//	// parse the string to get id, mobile, passenger, flight,
//	// 3 : pax id
//	// 4 : flight id
//	// 5 : mob type string
//	int begin_pos	= 0;
//	int end_pos		= 0;
//	CString strPaxType = _csFormatMsg.strMobType;
//	if( !strPaxType.IsEmpty() )
//	{
//		end_pos = strPaxType.Find("<@>");
//		if( end_pos == -1 ) // need not to parse
//		{
//			msgEx.strMobileType = strPaxType;
//		}
//		else				// need to parse
//		{
//			int pos = 3;
//			while( end_pos!= -1 && pos< 6)
//			{
//				switch( pos++ ) {
//				case 3:
//					msgEx.strPaxID = strPaxType.Mid(begin_pos, end_pos-begin_pos);
//					break;
//				case 4:
//					msgEx.strFlightID = strPaxType.Mid(begin_pos, end_pos-begin_pos);
//					break;
//				case 5:
//					msgEx.strMobileType = strPaxType.Mid(begin_pos, end_pos-begin_pos);
//					break;
//				default:
//					break;
//				}
//
//				begin_pos = end_pos + 3;
//				end_pos = strPaxType.Find( "<@>", begin_pos );
//			}
//		}
//	}
//	msgEx.diag_entry = _csFormatMsg.diag_entry;
//	msgEx.strOther = _csFormatMsg.strOtherInfo;
//	SendSimFormatExMessage( msgEx );
//}
//
//void Terminal::SendSimMessageOfRuns()
//{
//	if(!::IsWindow(m_hWnd))
//		return;
//	SendMessage( m_hWnd, WM_SIM_STATUS_MSG2, m_nPercentOfRuns, m_nRunNumber );
//}
//
//
////##ModelId=3E2FAD070388
//void Terminal::SendEventMessage( int _nEventNum, ElapsedTime _eCurTime, const char* _szTypeName )
//{
//	m_eventMsg.nEventNum = _nEventNum;
//	char str[64];
//	ElapsedTime tmptime(_eCurTime.asSeconds() % WholeDay);
//	tmptime.printTime ( str );
//
//	CStartDate sDate = m_pSimParam->GetStartDate();
//	bool bAbsDate;
//	COleDateTime date;
//	int nDtIdx;
//	COleDateTime time;
//	CString sDateTimeStr;
//	sDate.GetDateTime( _eCurTime, bAbsDate, date, nDtIdx, time );
//	if( bAbsDate )
//	{
//		sDateTimeStr = date.Format(_T("%Y-%m-%d"));
//	}
//	else
//	{
//		sDateTimeStr.Format("Day %d", nDtIdx + 1 );
//	}
//	sDateTimeStr += "  ";
//	sDateTimeStr += str;
//
//
//	m_eventMsg.csEventTime = sDateTimeStr;
//	m_eventMsg.csEventType = CString( _szTypeName );
//	ElapsedTime oneDay;
//	oneDay.setHours( 24 );
//
//	//add by adam 2007-09-26 
//	ElapsedTime estStartTime = m_pSimParam->GetStartTime() - m_pSimParam->GetInitPeriod();
//
//	if (estStartTime < 0l)
//	{
//		estStartTime = 0l;
//	}
//	ElapsedTime estEndTime = m_pSimParam->GetEndTime() + m_pSimParam->GetFollowupPeriod();
//	//End add by adam 2007-09-26 
//
//	float f = ( _eCurTime - estStartTime ) / ( estEndTime - estStartTime );
//	m_nPercent = 27 + int(f * 68);
//
//	//add by adam 2007-10-11
//	if (m_nPercent < 27)
//	{
//		m_nPercent = 27;
//	}
//
//	if (95 < m_nPercent)
//	{
//		m_nPercent = 95;
//	}
//	//add by adam 2007-10-11
//	
//	if(!::IsWindow(m_hWnd))
//		return;
//	SendMessage( m_hWnd, WM_SIM_STATUS_EVENT, GetPercent(), (long)&m_eventMsg );
//}
//
//
//int Terminal::GetPercent()
//{
//	return m_nPercent;
//}
//
//
//int Terminal::GetPercentOfRuns()
//{
//	return m_nPercentOfRuns;
//}
//
//
//void Terminal::SetPercent( int _nPercent )
//{
//	m_nPercent = _nPercent;
//}
//
//void Terminal::SetPercentOfRuns( int _nPercent )
//{
//	m_nPercentOfRuns = _nPercent;
//}
//
//void Terminal::SetRunNumber( int _nRunNumber )
//{
//	m_nRunNumber = _nRunNumber;
//}
//
//int Terminal::GetRunNumber()
//{
//	return m_nRunNumber;
//}
//
//
////##ModelId=3E2FAD070398
//Terminal::~Terminal ()
//{
//	delete m_pFltsBoardingCallManager;
//
//    clearLogs();
//}
//
//// called in every simulation run
//// make it ready for store all the logs.
//// for now only clear pax / proc logs.
//void Terminal::InitLogs( const CString& _csProjPath )
//{
//	PLACE_TRACK_STRING(_T("{7c02b739-be07-4291-b270-225207b8a446}"));
//	// clear logs
//	paxLog->clearLogs();
//	paxLog->clearEvents();
//	CString sFileName = m_pSimAndReportManager->GetCurrentLogsFileName( PaxEventFile, _csProjPath );
//	paxLog->createEventFile( sFileName );
//
//	flightLog->clearLogs();
//	procLog->clearLogs();
//	procLog->clearEvents();
//	sFileName = m_pSimAndReportManager->GetCurrentLogsFileName( ProcEventFile, _csProjPath );
//	procLog->createEventFile( sFileName );
//
//	m_pTrainLog->clearLogs();
//	m_pTrainLog->clearEvents();
//	sFileName = m_pSimAndReportManager->GetCurrentLogsFileName( TrainEventFile, _csProjPath );
//	m_pTrainLog->createEventFile( sFileName );
//
//	m_pElevatorLog->clearLogs();
//	m_pElevatorLog->clearEvents();
//	sFileName = m_pSimAndReportManager->GetCurrentLogsFileName( ElevatorEventFile, _csProjPath );
//	m_pElevatorLog->createEventFile( sFileName );
//
//
//	m_pBridgeConnectorLog->clearLogs();
//	m_pBridgeConnectorLog->clearEvents();
//	sFileName = m_pSimAndReportManager->GetCurrentLogsFileName( BridgeConnectorEventFile, _csProjPath );
//	m_pBridgeConnectorLog->createEventFile( sFileName );
//
//	resourceLog->clearLogs();
//	resourceLog->clearEvents();
//	sFileName = m_pSimAndReportManager->GetCurrentLogsFileName( ResourceEventFile, _csProjPath );
//	resourceLog->createEventFile( sFileName );
//	
//	m_pDiagnosLog->clearLog();
//	m_pDiagnosLog->closeDiagnoseInfoFile();
//	sFileName = m_pSimAndReportManager->GetCurrentLogsFileName( DiagnoseInfoFile, _csProjPath );
//	m_pDiagnosLog->createDiagnoseInfoFile( sFileName );
//}
////when TLOS created processor dynamicly ,we need update proc index in pax log.etc to new index
//void Terminal::RefreshProcIdxInLogs()
//{
//	PLACE_TRACK_STRING(_T("{518e7cd7-daaa-4158-9c87-30325bfd71db}"));
//	if( procList->GetDynamicCreatedProcs() && procList->GetDynamicCreatedProcs()->getProcessorCount() )
//	{		
//		std::map<int,Processor*>& mapDynamicCreateProcList = procList->GetMapOfDynamicCreatedProcIdx();
//		std::map<int,Processor*>& mapProcList = procList->GetMapOfProcIdx();
//		int iPaxLogItemCount = paxLog->getCount();
//		for( int i=0; i<iPaxLogItemCount; ++i )
//		{
//			MobLogEntry paxItem;
//			paxLog->getItem( paxItem, i);
//			paxItem.SetEventLog( paxLog->getEventLog() );
//			
//			int iTrackCount = paxItem.getCount();					
//			for( int j=0; j<iTrackCount; ++j )
//			{
//				MobEventStruct track = paxItem.getEvent( j );
//				int iOldProcIdx = track.procNumber;
//				if( iOldProcIdx == 32767 )
//					continue;
//				int iNewIdx = -1;
////				if( track.bDynamicCreatedProc && (iOldProcIdx < static_cast<int>(mapDynamicCreateProcList.size())))
//				
//				if( track.bDynamicCreatedProc)
//				{	
//					iNewIdx	= mapDynamicCreateProcList[ iOldProcIdx ]->getIndex();				
//				}
//				else
//				{					
//					iNewIdx	= mapProcList[ iOldProcIdx ]->getIndex();
//				}
//				ASSERT( iNewIdx >=0 );
//				if( iNewIdx != iOldProcIdx )
//				{
//					track.procNumber = iNewIdx;
//					paxItem.updateEvent( track, j );
//				}								
//			}
//		}
//	}
//}
//
//
//void Terminal::DeleteGreetPoolData()
//{
//	if(GreetPool.empty()) return;
//	for(std::vector<ConvergePair*>::iterator it = GreetPool.begin();it!= GreetPool.end();it++)
//	{
//		delete (*it);
//	}
//}
//
////initialize congestion parameter form file
//void Terminal::InitCongestionParam(const CString& _pProjPath)
//{
//	m_congManager.loadDataSet(_pProjPath+"\\INPUT");
//}
//
//void Terminal::ReflectOnFlightArriveStand(Flight* pFlight, const ElapsedTime& actualTime)
//{
//	int numFlights = m_simFlightSchedule.getCount();
//	ASSERT(numFlights > 0);
//
//	// for adjust arriving flight's mobElement entry time
//	// wake up mobile element when flight arrive stand
//	//if(pFlight->isArriving())
//	//	m_pMobElemTermEntryManager->ActiveMobileElementGroup(pFlight, actualTime);
//    
//	//for adjust departure flight's boarding call time
//	//adjust boardingCall time according to flight?¯s actual departure time
//	if(pFlight->isDeparting())
//	{
//		m_pFltsBoardingCallManager->ScheduleBoardingCallEvents(pFlight, actualTime);
//	//	this->GetAirsideSimulation()->GetAirTrafficController()->GetBoardingCall()->NoticeFlightArrived(pFlight,actualTime) ;
//	}
//}
//
//
//void Terminal::ClearMovingTrain()
//{
//	for (int i = 0; i < (int)m_simMovingTrainList.size(); i++)
//	{
//		if (m_simMovingTrainList[i])
//		{
//			m_simMovingTrainList[i]->M_DELIDX = 0;
//			delete m_simMovingTrainList[i];
//			m_simMovingTrainList[i] = NULL;
//		}
//	}
//	m_simMovingTrainList.clear();
//}
//
//void Terminal::FlushMovingTrain()
//{
//	for (int i = 0; i < (int)m_simMovingTrainList.size(); i++)
//	{
//		if (m_simMovingTrainList[i])
//		{
//			m_simMovingTrainList[i]->FlushLog();
//		}
//	}
//}