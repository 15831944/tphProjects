#include "stdafx.h"
#include "results\out_term.h"
#include "results\paxlog.h"
#include "results\fltlog.h"
#include "results\proclog.h"
#include "results\baglog.h"
#include "results\trainlog.h"
#include "Results\RetailLog.h"
#include "inputs\typelist.h"
#include <CommonData/procid.h>
#include "inputs\pax_cnst.h"
#include "common\category.h"
#include "..\common\strdict.h"
#include "..\common\projectmanager.h"
#include "..\common\progressbar.h"
#include "common\SimAndReportManager.h"
#include "results\ElevatorLog.h"
#include "EventLogBufManager.h"
#include "ResourceElementLog.h"
#include "ResourceElementLogEntry.h"
#include "PaxDestDiagnoseLog.h"
#include "Common/ARCTracker.h"
#include "BridgeConnectorLog.h"
#include "MultiFileEventLog.h"
#include "TempMobileEventLog.h"


OutputTerminal::OutputTerminal()
{
	PLACE_METHOD_TRACK_STRING();
	m_pBagEventLog = new EventLog<BagEventStruct>;
	m_pFltEventLog = new EventLog<FlightEventStruct>;
//	m_pMobEventLog = new EventLog<MobEventStruct>;
	m_pMobEventLog = new MultiFileEventLog;
	m_pTempMobEventLog = new TempMobileEventLog;

	m_pProcEventLog = new EventLog<ProcEventStruct>;
	m_pTrainEventLog = new EventLog<TrainEventStruct>;
	m_pElevatorEventLog = new EventLog<ElevatorEventStruct>;
	m_pResourceEventLog = new EventLog<ResEventStruct>;
	m_pBridgeConnectorEventLog = new EventLog<BridgeConnectorEventStruct>;
	m_pRetailEventLog = new EventLog<RetailEventStruct>;

    paxLog = new PaxLog( m_pMobEventLog, m_pTempMobEventLog );
    flightLog = new FlightLog( m_pFltEventLog );
    procLog = new ProcLog( m_pProcEventLog );
    bagLog = new BagLog( m_pBagEventLog );
	m_pTrainLog = new CTrainLog( m_pTrainEventLog );
	m_pElevatorLog = new CElevatorLog( m_pElevatorEventLog );
	resourceLog = new ResourceElementLog( m_pResourceEventLog );
	m_pBridgeConnectorLog = new CBridgeConnectorLog(m_pBridgeConnectorEventLog);
	m_pRetailLog = new CRetailLog(m_pRetailEventLog);



	m_pDiagnosLog = new CPaxDestDiagnoseLog();

	m_pSimAndReportManager = new CSimAndReportManager;
	m_pLogBufManager = new CEventLogBufManager( this );
    outStrDict = NULL;
    outTypeList = NULL;
    outCatList = NULL;
	
}

OutputTerminal::~OutputTerminal()
{
    delete paxLog;
    delete flightLog;
    delete procLog;
    delete bagLog;
	delete m_pTrainLog;
	delete m_pElevatorLog;
	delete resourceLog;
	delete m_pBridgeConnectorLog;

	if (m_pDiagnosLog)
		delete m_pDiagnosLog;

    if (outStrDict)
        delete outStrDict;
    if (outTypeList)
        delete outTypeList;
    if (outCatList)
        delete outCatList;

	delete m_pTrainEventLog;
	delete m_pBagEventLog;
	delete m_pFltEventLog;
	delete m_pMobEventLog;
	delete m_pProcEventLog;
	delete m_pElevatorEventLog;
	delete m_pBridgeConnectorEventLog;
	delete m_pRetailEventLog;
	delete m_pRetailLog;
	delete m_pResourceEventLog;
	delete m_pLogBufManager;
	delete m_pSimAndReportManager;
}

// call by start of simulation 
void OutputTerminal::clearLogs ()
{
    if (outStrDict)
	{
        delete outStrDict;
		outStrDict = NULL;
	}
    if (outTypeList)
	{
        delete outTypeList;
		outTypeList = NULL;
	}
    if (outCatList)
	{
        delete outCatList;
		outCatList = NULL;
	}

    paxLog->clearLogs();
    paxLog->clearEvents();

    flightLog->clearLogs();

    procLog->clearLogs();
    procLog->clearEvents();

    bagLog->clearLogs();

	m_pTrainLog->clearLogs();
	m_pTrainLog->clearEvents();

	m_pElevatorLog->clearLogs();
	m_pElevatorLog->clearEvents();

	m_pBridgeConnectorLog->clearLogs();
	m_pBridgeConnectorLog->clearEvents();

	m_pRetailLog->clearLogs();
	m_pRetailLog->clearEvents();

	resourceLog->clearLogs();
	resourceLog->clearEvents();

	m_pDiagnosLog->clearLog();
	m_pDiagnosLog->closeDiagnoseInfoFile();
	
	m_pLogBufManager->clearLogBuffer();
}


// Description: call by report to load log if necessary
// Exception:	FileOpenError, StringError, FileVersionTooNewError
void OutputTerminal::openLogs ( const CString& _csProjPath )
{
	clearLogs();

    char filename[_MAX_PATH];
	
	CProgressBar bar( "Loading Logs Data For The First Time", 100, 9, TRUE, 0 );
	
	CString sFileName = m_pSimAndReportManager->GetCurrentLogsFileName( PaxDescFile, _csProjPath );	

	paxLog->LoadDataIfNecessary(sFileName);	
	bar.StepIt();

	sFileName = m_pSimAndReportManager->GetCurrentLogsFileName( PaxEventFile, _csProjPath );	
	paxLog->LoadEventDataIfNecessary(sFileName);
	m_pLogBufManager->setDetaTimeBaseFileSize( sFileName );
	bar.StepIt();
	
	sFileName = m_pSimAndReportManager->GetCurrentLogsFileName( FlightDescFile, _csProjPath );
	flightLog->LoadDataIfNecessary(sFileName);
	bar.StepIt();

	
	sFileName = m_pSimAndReportManager->GetCurrentLogsFileName( ProcDescFile, _csProjPath );
	procLog->LoadDataIfNecessary(sFileName);
	bar.StepIt();

	sFileName = m_pSimAndReportManager->GetCurrentLogsFileName( ProcEventFile, _csProjPath );
	procLog->LoadEventDataIfNecessary(sFileName);
	bar.StepIt();

	sFileName = m_pSimAndReportManager->GetCurrentLogsFileName( BagDescFile, _csProjPath );	
	bagLog->LoadDataIfNecessary(sFileName);
	bar.StepIt();

	sFileName = m_pSimAndReportManager->GetCurrentLogsFileName( TrainDescFile, _csProjPath );	
	m_pTrainLog->LoadDataIfNecessary(sFileName);	
	bar.StepIt();


	sFileName = m_pSimAndReportManager->GetCurrentLogsFileName( ElevatorDescFile, _csProjPath );
	m_pElevatorLog->LoadDataIfNecessary( sFileName );
	bar.StepIt();


	sFileName = m_pSimAndReportManager->GetCurrentLogsFileName( TrainEventFile, _csProjPath );	
	m_pTrainLog->LoadEventDataIfNecessary(sFileName);
	bar.StepIt();


	sFileName = m_pSimAndReportManager->GetCurrentLogsFileName( ElevatorEventFile, _csProjPath );
	m_pElevatorLog->LoadEventDataIfNecessary( sFileName );
	bar.StepIt();
	

	sFileName = m_pSimAndReportManager->GetCurrentLogsFileName( BridgeConnectorDescFile, _csProjPath );
	m_pBridgeConnectorLog->LoadDataIfNecessary( sFileName );
	bar.StepIt();

	sFileName = m_pSimAndReportManager->GetCurrentLogsFileName( BridgeConnectorEventFile, _csProjPath );
	m_pBridgeConnectorLog->LoadEventDataIfNecessary( sFileName );
	bar.StepIt();


	sFileName = m_pSimAndReportManager->GetCurrentLogsFileName( RetailActivityDescFile, _csProjPath );
	m_pRetailLog->LoadDataIfNecessary( sFileName );
	bar.StepIt();

	sFileName = m_pSimAndReportManager->GetCurrentLogsFileName( RetailActivityEventFile, _csProjPath );
	m_pRetailLog->LoadEventDataIfNecessary( sFileName );
	bar.StepIt();

	sFileName = m_pSimAndReportManager->GetCurrentLogsFileName( ResourceDescFile, _csProjPath );
	resourceLog->LoadDataIfNecessary( sFileName );
	bar.StepIt();

	sFileName = m_pSimAndReportManager->GetCurrentLogsFileName( ResourceEventFile, _csProjPath );
	resourceLog->LoadEventDataIfNecessary( sFileName );
	bar.StepIt();

	sFileName = m_pSimAndReportManager->GetCurrentLogsFileName( DiagnoseLogFile, _csProjPath );
	m_pDiagnosLog->loadDataIfNeed( sFileName );
	bar.StepIt();

	sFileName = m_pSimAndReportManager->GetCurrentLogsFileName( DiagnoseInfoFile, _csProjPath );
	m_pDiagnosLog->openDiagnoseInfoFile( sFileName );
	bar.StepIt();

    // init mirror files
    if (!outStrDict)
	{
        outStrDict = new StringDictionary;
		outStrDict->addString( "ALL TYPES" ); // for backward compatible
	}
     PROJMANAGER->getFileName (_csProjPath, StringDictBinFile, filename);
    outStrDict->retrieveDictionary (filename);
	bar.StepIt();

    if (!outTypeList)
        outTypeList = new PassengerTypeList( outStrDict );
    outTypeList->setFileType (TypeListBinFile);
    outTypeList->loadDataSet(_csProjPath);
	bar.StepIt();

    if (!outCatList)
        outCatList = new CategoryList;
    outCatList->setFileType (CategoryBinFile);
    outCatList->loadDataSet(_csProjPath);
	bar.StepIt();

//&    initStatics();
}
/*
void OutputTerminal::initStatics (void)
{
    if (outTypeList)
        outTypeList->initStrDict (outStrDict);
    ProcessorID::initStrDict (outStrDict);
    PassengerConstraint::initStrDict (outStrDict);
    PassengerConstraint::initTypeList (outTypeList);
    FlightConstraint::initSectors (outSectorList);
    FlightConstraint::initCategories (outCatList);
}
*/

void OutputTerminal::closeLogs ()
{
	PLACE_METHOD_TRACK_STRING();
	m_pBagEventLog->closeEventFile();
	m_pFltEventLog->closeEventFile();
	m_pMobEventLog->closeEventFile();
	m_pProcEventLog->closeEventFile();
	m_pTrainEventLog->closeEventFile();
	m_pElevatorEventLog->closeEventFile();
	m_pResourceEventLog->closeEventFile();
	m_pDiagnosLog->closeDiagnoseInfoFile();
	m_pRetailEventLog->closeEventFile();
}

// call at the end of simulation
void OutputTerminal::closeLogs ( const CString& _csProjPath )
{
	PLACE_METHOD_TRACK_STRING();
	CString sFileName = m_pSimAndReportManager->GetCurrentLogsFileName( PaxEventFile, _csProjPath );
    paxLog->SaveEventToFile( sFileName );


	sFileName = m_pSimAndReportManager->GetCurrentLogsFileName( PaxDescFile, _csProjPath );
	paxLog->SaveToFile( sFileName );


	
	sFileName = m_pSimAndReportManager->GetCurrentLogsFileName( FlightDescFile, _csProjPath );
	flightLog->SaveToFile( sFileName );


	sFileName = m_pSimAndReportManager->GetCurrentLogsFileName( ProcEventFile, _csProjPath );
    procLog->SaveEventToFile( sFileName );
    

	sFileName = m_pSimAndReportManager->GetCurrentLogsFileName( ProcDescFile, _csProjPath );
	procLog->SaveToFile( sFileName );
    

	sFileName = m_pSimAndReportManager->GetCurrentLogsFileName( BagDescFile, _csProjPath );
	bagLog->SaveToFile( sFileName );


	sFileName = m_pSimAndReportManager->GetCurrentLogsFileName( TrainEventFile, _csProjPath );
    m_pTrainLog->SaveEventToFile( sFileName );


	sFileName = m_pSimAndReportManager->GetCurrentLogsFileName( ElevatorEventFile, _csProjPath );
	m_pElevatorLog->SaveEventToFile( sFileName );

	sFileName = m_pSimAndReportManager->GetCurrentLogsFileName( ElevatorDescFile, _csProjPath );
	m_pElevatorLog->SaveToFile( sFileName );
   
	sFileName = m_pSimAndReportManager->GetCurrentLogsFileName( BridgeConnectorEventFile, _csProjPath );
	m_pBridgeConnectorLog->SaveEventToFile( sFileName );


	sFileName = m_pSimAndReportManager->GetCurrentLogsFileName( RetailActivityDescFile, _csProjPath );
	m_pRetailLog->SaveToFile( sFileName );

	sFileName = m_pSimAndReportManager->GetCurrentLogsFileName( RetailActivityEventFile, _csProjPath );
	m_pRetailLog->SaveEventToFile( sFileName );

	sFileName = m_pSimAndReportManager->GetCurrentLogsFileName( TrainDescFile, _csProjPath );
	m_pTrainLog->SaveToFile( sFileName );


	sFileName = m_pSimAndReportManager->GetCurrentLogsFileName( BridgeConnectorDescFile, _csProjPath );
    m_pBridgeConnectorLog->SaveToFile( sFileName );

	sFileName = m_pSimAndReportManager->GetCurrentLogsFileName( ResourceEventFile, _csProjPath );
	resourceLog->SaveEventToFile( sFileName );

	sFileName = m_pSimAndReportManager->GetCurrentLogsFileName( ResourceDescFile, _csProjPath );
	resourceLog->SaveToFile( sFileName );

	sFileName = m_pSimAndReportManager->GetCurrentLogsFileName( DiagnoseLogFile, _csProjPath );
	m_pDiagnosLog->saveLog( sFileName );

	sFileName = m_pSimAndReportManager->GetCurrentLogsFileName( DiagnoseInfoFile, _csProjPath );
	m_pDiagnosLog->closeDiagnoseInfoFile();
}


int OutputTerminal::resultsLoaded()
{
	return outStrDict != NULL;
}
