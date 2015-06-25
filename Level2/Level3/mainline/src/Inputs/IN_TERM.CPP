#include "stdafx.h"
#include "in_term.h"
#include "main\mainfrm.h"
#include "schedule.h"
#include "fltdata.h"
//***#include "sectlist.h"
#include "common\category.h"
#include "hubbing.h"
#include "typelist.h"
#include "distlist.h"
#include "paxdata.h"
#include "BridgeConnectorPaxData.h"
#include "bagdata.h"
#include "engine\proclist.h"
#include "procdata.h"
#include "srvctime.h"
#include "paxflow.h"
#include "assigndb.h"
#include "acdata.h"
#include "movrdata.h"
#include "common\projectmanager.h"
#include "common\WinMsg.h"
#include "simparameter.h"
#include "common\ProgressBar.h"
#include "sectlist.h"
#include "common\AirportsManager.h"
#include "HubbingDatabase.h"
#include "GatePriorityInfo.h"
#include "FlightPriorityInfo.h"
#include "BagGateAssign.h"
#include "RailWayData.h"
#include "StationLayout.h"
#include "AllCarSchedule.h"
#include "SideMoveWalkDataSet.h"
#include "SideMoveWalkPaxDist.h"
#include "PipeDataSet.h"
#include "TerminalProcessorTypeSet.h"
#include "MobileElemTypeStrDB.h"
#include "ProcessorSwitchTimeDB.h"
#include "GateMappingDB.h"
#include "resourcepool.h"
#include "proctoresource.h"


#include "PaxFlowConvertor.h"
#include "AllPaxTypeFlow.h"
#include "SubFlowList.h"
#include "AllGreetingProcessors.h"
#include "reports\ReportParaDB.h"
#include "engine\Monitor.h"
#include "engine\autoroster.h"
#include "engine\AutoCreateProcessorContainer.h"
#include "inputs\UserName.h"
#include "FlightScheduleCriteriaDB.h"
#include "common\airportdatabase.h"
#include "areasportals.h"
#include "PaxBulkInfo.h"
#include "airsideinput.h"
#include "Structurelist.h"
#include "InputFlowSync.h"
#include "../InputAirside/InputAirside.h"
#include "IntermediateStandAssign.h"
#include "../InputOnBoard/AircaftLayOut.h"

#include "../Inputs/NonPaxRelativePosSpec.h"
#include "../Inputs/TrayHost.h"
#include "Common/ARCTracker.h"
#include "../InputOnboard/AffinityGroup.h"
#include "AutoRosterRule.h"
#include "../Inputs/ProductStockingList.h"

extern const CString LOGSTRING_LOADPROJECT = "LOADPROJECT";

InputTerminal *inputTerminal = NULL;
char InputTerminal::m_csDisplayMsg[512];


InputTerminal::InputTerminal () : m_pAirportDB( NULL ),m_pVoidOnBoardConfig(NULL)
{
	PLACE_METHOD_TRACK_STRING();
    inputTerminal = this;

	m_pSimParam = new CSimParameter;

	inStrDict = new StringDictionary;
	inStrDict->addString( "ALL TYPES" ); // for backward compatible

	inUndefineShapeStrDict = new StringDictionary;
	inUndefineShapeStrDict->addString("ALL TYPES");

	inDefineShapeStrDict = new StringDictionary;
	inDefineShapeStrDict->addString("ALL TYPES");

    // Flight data
    flightSchedule = new FlightSchedule;
	m_pFlightScheduleCriteriaDB = new FlightScheduleCriteriaDB();
//    inSectorList = new SectorList;
    flightData = new FlightData;
    m_pHubbing = new CHubbingDatabase;

    // Passenger data
    inTypeList = new PassengerTypeList( inStrDict );
    paxDistList = new PassengerDistributionList;
    paxDataList = new PassengerData;
	bcPaxData = new BridgeConnectorPaxData;

    // Baggage Data
    bagData = new BaggageData;
    // Processor data
    procList = new ProcessorList( inStrDict );
    miscData = new MiscDatabase;
    serviceTimeDB = new ServiceTimeDatabase;
    m_pPassengerFlowDB = new PassengerFlowDatabase( PassengerFlowFile );
    m_pStationPaxFlowDB = new PassengerFlowDatabase( StationPaxFlowFile );	
	m_pSpecificFlightPaxFlowDB = new PassengerFlowDatabase( SpecificFlightPaxFlowFile );
    procAssignDB = new ProcAssignDatabase;

	m_pStructureList = new CStructureList(TerminalStructureFile);
	m_pWallShapeList = new WallShapeList(TerminalWallShapeFile);
    
    peopleMovers = new PeopleMoverDataSet;

	//Priority Assignment Info
	gatePriorityInfo = new GatePriorityInfo;
	flightPriorityInfo = new FlightPriorityInfo;
	m_pIntermediateStand = new IntermediateStandAssign;

	//Baggate Device Info
	bagGateAssignDB = new BagGateAssign;

	pRailWay=new CRailWayData();
	pStationLayoutSample=new CStationLayout();
	m_pAllCarSchedule= new CAllCarSchedule();

	m_pPipeDataSet = new CPipeDataSet();

	m_pMovingSideWalk = new CSideMoveWalkDataSet();
	m_pMovingSideWalkPaxDist = new CSideMoveWalkPaxDist();

	m_pMobElemTypeStrDB = new CMobileElemTypeStrDB();
	m_pGateMappingDB = new CGateMappingDB();

	m_pSubFlowList = new CSubFlowList();

	m_pGreetingProcessors = new CAllGreetingProcessors();

	m_pReportParaDB = new CReportParaDB;

	m_pTerminalProcessorTypeSet = new CTerminalProcessorTypeSet();
    initStatics();
	
	m_bModifyFlag = false;
	m_iCurrentSimResult = -1;
	m_iPreSimResult = -1;

	m_pTLOSMonitor = new CMonitor( this, m_pSimParam );
	m_pAutoRoster = new CAutoRoster();
	m_pAutoCreatedProcContainer = new CAutoCreateProcessorContainer;
	m_pUserNameListDB = new CUserNameDataSet();
	
	m_pResourcePoolDB = new CResourcePoolDataSet();
	m_pProcToResourceDB = new CProcToResourceDataSet();

	m_pAreas = new CAreas();
	m_pPaxBulkInfo = new CPaxBulkInfo();

	m_AirsideInput = new CAirsideInput(inStrDict, m_pAirportDB, flightSchedule);

	//m_sysMode = EnvMode_Terminal;

	m_pInputFlowSync = new CInputFlowSync;



	//non pax type relative pos to pax
	m_pNonPaxRelPosSpec = new CNonPaxRelativePosSpec;

	m_pTrayHostList = new TrayHostList;

	m_pAffinityGroup = new CAffinityGroup;

	m_pProductStockingList = new ProductStockingList;

    m_pUserBarMan = new CUserShapeBarManager;
}

InputTerminal::~InputTerminal ()
{
    inputTerminal = NULL;

	delete m_pSimParam;

    // Flight data
    delete flightSchedule;
	delete m_pFlightScheduleCriteriaDB;
    delete flightData;
//    delete inSectorList;
//***    delete inCatList;
    delete m_pHubbing;
//***    delete acData;

    // Passenger data
    delete inTypeList;
    delete paxDistList;
    delete paxDataList;
	delete bcPaxData;

    // Baggage Data
    delete bagData;

    // Processor data
    delete procList;
    delete miscData;
    delete serviceTimeDB;
    delete m_pPassengerFlowDB;
	delete m_pStationPaxFlowDB;
	delete m_pSpecificFlightPaxFlowDB;
    delete procAssignDB;
    delete peopleMovers;

	//string dictionary
	delete inStrDict;
	delete inDefineShapeStrDict;
	delete inUndefineShapeStrDict;

	//priority assignment info
	delete gatePriorityInfo;
	delete flightPriorityInfo;	

	//
	delete bagGateAssignDB;
	delete pRailWay;
	delete pStationLayoutSample;
	delete m_pAllCarSchedule;
	
	delete m_pPipeDataSet;
	delete m_pTerminalProcessorTypeSet;
	delete m_pMovingSideWalk;
	delete m_pMovingSideWalkPaxDist;

	delete m_pMobElemTypeStrDB; 
	delete m_pGateMappingDB;

	delete m_pSubFlowList;

	delete m_pGreetingProcessors;

	delete m_pReportParaDB;

	delete m_pTLOSMonitor;
	delete m_pAutoRoster;
	delete m_pAutoCreatedProcContainer;
	delete m_pUserNameListDB;
	
	delete m_pResourcePoolDB;
	delete m_pProcToResourceDB;

	delete m_pAreas;

	delete m_pPaxBulkInfo;

	delete m_AirsideInput;
	m_AirsideInput = NULL;



	delete m_pIntermediateStand;
	m_pIntermediateStand = NULL;

	delete m_pNonPaxRelPosSpec;
	m_pNonPaxRelPosSpec = NULL;

	delete m_pTrayHostList;
	m_pTrayHostList = NULL;

	delete m_pAffinityGroup;
	m_pAffinityGroup = NULL;

	delete m_pProductStockingList;
	m_pProductStockingList = NULL;

    delete m_pUserBarMan;
    m_pUserBarMan = NULL;
}


// Description: Load all Input Data From Default Files
// Exception:	FileVersionTooNewError
void InputTerminal::loadInputs ( const CString& _pProjPath )
{
	
	CProgressBar bar( "Loading Input Data", 100, 16, TRUE );

	m_pMobElemTypeStrDB->SetInputTerminal(this);
	m_pMobElemTypeStrDB->loadDataSet(_pProjPath);

	bagGateAssignDB->SetInputTerminal(this);
	bagGateAssignDB->loadDataSet(_pProjPath);

	//priority assignment info
	gatePriorityInfo->SetInputTerminal(this	);
	gatePriorityInfo->loadDataSet(_pProjPath);
	flightPriorityInfo->SetInputTerminal(this);
	flightPriorityInfo->loadDataSet(_pProjPath);
	m_pIntermediateStand->SetInputTerminal(this);
	m_pIntermediateStand->loadDataSet(_pProjPath);
	bar.StepIt();

	inTypeList->SetInputTerminal( this );
    inTypeList->loadDataSet( _pProjPath );
	bar.StepIt();
	

	flightSchedule->SetInputTerminal( this );
    flightSchedule->loadDataSet( _pProjPath );
	bar.StepIt();

	// echo flight info
	flightSchedule->echo( LoadProject );

	// should be after schedule
	m_pSimParam->SetInputTerminal( this );
	m_pSimParam->SetStartDate( flightSchedule->GetStartDate() );
	m_pSimParam->loadDataSet( _pProjPath );
	bar.StepIt();


	m_pFlightScheduleCriteriaDB->SetInputTerminal( this );
	m_pFlightScheduleCriteriaDB->loadDataSet( _pProjPath );
	bar.StepIt();

//***    acData->loadDataSet( _pProjPath );
/* replace its action by CAirportsManager::m_vSector [6/21/2004]Jason
	inSectorList->SetInputTerminal( this );
	inSectorList->loadDataSet( _pProjPath );
	ImportOldSector( _pProjPath );
*/

//***    inCatList->loadDataSet( _pProjPath );
	flightData->SetInputTerminal( this );
    flightData->loadDataSet( _pProjPath );
	bar.StepIt();
	paxDistList->SetInputTerminal( this );
    paxDistList->loadDataSet( _pProjPath );
	bar.StepIt();

	bagData->SetInputTerminal( this );
    bagData->loadDataSet( _pProjPath );

	bar.StepIt();
	paxDataList->SetInputTerminal( this );
	//bool IsConvertGroupSizeData = CheckConvertPaxGroupSizeData(_pProjPath);
    paxDataList->loadDataSet( _pProjPath );

	bcPaxData->SetInputTerminal(this);
	bcPaxData->loadDataSet(_pProjPath);
	bar.StepIt();


	m_pHubbing->SetInputTerminal( this );
    m_pHubbing->loadDataSet( _pProjPath );
	bar.StepIt();

	// must before procList->loadDataSet( _pProjPath );
	pStationLayoutSample->SetInputTerminal(this);
	pStationLayoutSample->setProjectPath(_pProjPath);
	pStationLayoutSample->loadDataSet(_pProjPath);


	procList->SetInputTerminal( this );
    procList->loadDataSet( _pProjPath );
	procList->RefreshAllStationLocatonData();
	if( procList->DidOffset() )
	{
		procList->saveDataSet( _pProjPath, false );
	}
	bar.StepIt();
    procList->addStartAndEndProcessors();
	procList->addFromAndToGateProcessors();
	

	miscData->SetInputTerminal( this );
	miscData->loadDataSet( _pProjPath );
	bar.StepIt();
	
	m_AirsideInput->InitAirsideInputData(this, _pProjPath);

	serviceTimeDB->SetInputTerminal( this );
    serviceTimeDB->loadDataSet( _pProjPath );
	bar.StepIt();

	
    m_pPassengerFlowDB->SetInputTerminal( this );
    m_pPassengerFlowDB->loadDataSet( _pProjPath );
	
	bar.StepIt();
    m_pStationPaxFlowDB->SetInputTerminal( this );
    m_pStationPaxFlowDB->loadDataSet( _pProjPath );

	m_pSpecificFlightPaxFlowDB->SetInputTerminal( this );
	m_pSpecificFlightPaxFlowDB->loadDataSet( _pProjPath );
	if( m_pSpecificFlightPaxFlowDB->getCount() <=0 )
	{
		CPaxFlowConvertor tempConvertor;
		CAllPaxTypeFlow tempFlow;
		tempConvertor.SetInputTerm( this );
		tempConvertor.BuildSpecFlightFlowFromGateAssign( tempFlow );
		bar.StepIt();
		tempConvertor.ToOldFlightFlow( tempFlow );		
	}

	
	bar.StepIt();
    procAssignDB->SetInputTerminal( this );
    procAssignDB->loadDataSet( _pProjPath );
//	procAssignDB->saveDataSet(_pProjPath,false);
	bar.StepIt();
    //peopleMovers->SetInputTerminal( this );
    //peopleMovers->loadDataSet( _pProjPath );
	//bar.StepIt();

	// load economic data

/*	
	m_pProcLabourCostDB->SetInputTerminal( this );
	m_pProcLabourCostDB->loadDataSet( _pProjPath );*/
    //bar.StepIt();


    // update flight schedule

	//bagData->BuildFlightAssignToBaggageDevice();
	linkFlightData();
//    flightSchedule->sortFlights();

	bar.StepIt();

	pRailWay->SetInputTerminal(this);
	pRailWay->loadDataSet(_pProjPath);

	//if (ECHOLOG->IsEnable() && ECHOLOG->IsLogEnable(RailWayLog))
	{
		CString strLog = _T("");
		strLog = _T("load Railway Data;");
		ECHOLOG->Log(RailWayLog,strLog);
	}

	//must after pRailWay->loadDataSet(_pProjPath);
	m_pAllCarSchedule->SetInputTerminal( this );
	m_pAllCarSchedule->SetSystemProcessorList( procList );
	m_pAllCarSchedule->SetSystemRailWay( pRailWay );
	m_pAllCarSchedule->loadDataSet( _pProjPath );
	

	m_pMovingSideWalk->SetInputTerminal(this);
	m_pMovingSideWalk->loadDataSet(_pProjPath);

	m_pMovingSideWalkPaxDist->SetInputTerminal(this);
	m_pMovingSideWalkPaxDist->loadDataSet(_pProjPath);

	m_pPipeDataSet->SetInputTerminal( this );
	m_pPipeDataSet->loadDataSet( _pProjPath );
	
	m_pTerminalProcessorTypeSet->SetInputTerminal(this);
	m_pTerminalProcessorTypeSet->loadDataSet(_pProjPath);

	m_pGateMappingDB->SetInputTerminal( this );
	m_pGateMappingDB->loadDataSet( _pProjPath );
	
	m_pSubFlowList->SetInputTerminal( this );
	m_pSubFlowList->loadDataSet( _pProjPath );

	m_pReportParaDB->SetInputTerminal( this );
	m_pReportParaDB->loadDataSet( _pProjPath );

	m_pUserNameListDB->SetInputTerminal( this );
	m_pUserNameListDB->loadDataSet( _pProjPath );

	m_pResourcePoolDB->SetInputTerminal( this );
	m_pResourcePoolDB->loadDataSet( _pProjPath );

	m_pProcToResourceDB->SetInputTerminal( this );
	m_pProcToResourceDB->loadDataSet( _pProjPath );
	
	m_pAreas->loadDataSet( _pProjPath );

	
	m_pPaxBulkInfo->SetInputTerminal( this );	
	m_pPaxBulkInfo->loadDataSet( _pProjPath );
	//////////////////////////////////////////////////////////////////////////
	// set modify flag
	setModifyFlag( false );
	procList->setMiscData( *(miscData) );

	
	if(flightSchedule->IsWantToAssignedBagDevice())//version <=2.3 want to assigned BD
	{
		bagData->BuildFlightAssignToBaggageDevice( flightSchedule );
		flightSchedule->saveDataSet(_pProjPath,false);
	}
	else
		bagData->InputFlightAssignBaggageDevice( flightSchedule );// version >=2.4, 

	flightSchedule->assignBaggageCarosels (bagData,this);

	//initialize congestion parameter from file
	InitCongestionParam(_pProjPath);

	m_pInputFlowSync->SetInputTerminal(this);
	m_pInputFlowSync->loadDataSet(_pProjPath);

	m_pNonPaxRelPosSpec->loadDataSet(_pProjPath);

	peopleMovers->SetInputTerminal(this);
	peopleMovers->loadDataSet(_pProjPath);

	m_pTrayHostList->ReadData(m_nProjID);

	m_pAffinityGroup->SetAirportDatabase(m_pAirportDB);
	m_pAffinityGroup->ReadData(inStrDict);

	m_pProductStockingList->ReadData(-1);
	//if (IsConvertGroupSizeData)
	//	m_pAffinityGroup->ConvertDataFromGroupSize(paxDataList->getGroups(),this);

    m_pUserBarMan->loadDataSet(_pProjPath);
}


void InputTerminal::linkFlightData()
{
	PLACE_METHOD_TRACK_STRING();
    //flightSchedule->updateFlights (flightData);
//    flightSchedule->assignBaggageCarosels (bagData);
//    flightSchedule->initGateIndexes (procList);
	//ASSERT(FALSE);  
	flightSchedule->initGateIndexes (procList);
	
}

void InputTerminal::linkFlightData( int prjID )
{
	linkFlightData();
	flightSchedule->updateFlights (flightData);
	std::vector<int> vAirports;
	InputAirside::GetAirportList(prjID,vAirports);
	if(vAirports.size()>0)
		flightSchedule->initStandIndexes(vAirports.at(0), prjID);

}
void InputTerminal::saveInputs ( const CString& _pProjPath) const
{
	m_pSimParam->saveDataSet(_pProjPath, false);
    inTypeList->saveDataSet(_pProjPath, false);
    paxDistList->saveDataSet(_pProjPath, false);
    flightSchedule->saveDataSet(_pProjPath, false);

	m_pFlightScheduleCriteriaDB->saveDataSet( _pProjPath,false );
//***    acData->saveDataSet(_pProjPath);
//	inSectorList->saveDataSet(_pProjPath, false);//  [6/21/2004]
//***    inCatList->saveDataSet(_pProjPath);
    flightData->saveDataSet(_pProjPath, false);
    bagData->saveDataSet(_pProjPath, false);
    paxDataList->saveDataSet(_pProjPath, false);
	bcPaxData->saveDataSet(_pProjPath, false);
    m_pHubbing->saveDataSet(_pProjPath, false);

    procList->saveDataSet(_pProjPath, false);
	m_AirsideInput->SaveAirsideInputData(_pProjPath, false);

    miscData->saveDataSet(_pProjPath, false);
    serviceTimeDB->saveDataSet(_pProjPath, false);
    m_pPassengerFlowDB->saveDataSet(_pProjPath, false);
    m_pStationPaxFlowDB->saveDataSet(_pProjPath, false);
	m_pSpecificFlightPaxFlowDB->saveDataSet( _pProjPath, false );

    procAssignDB->saveDataSet(_pProjPath, false);
    peopleMovers->saveDataSet(_pProjPath, false);
	gatePriorityInfo->saveDataSet(_pProjPath,false);
	flightPriorityInfo->saveDataSet(_pProjPath, false);
	m_pIntermediateStand->saveDataSet(_pProjPath,false);
	bagGateAssignDB->saveDataSet(_pProjPath, false);

	pRailWay->saveDataSet(_pProjPath,false);
	pStationLayoutSample->saveDataSet(_pProjPath,false);
	m_pAllCarSchedule->saveDataSet( _pProjPath,false );

	m_pMovingSideWalk->saveDataSet(_pProjPath,false );

	m_pMovingSideWalkPaxDist->saveDataSet( _pProjPath, false );

	m_pPipeDataSet->saveDataSet( _pProjPath, false );

	m_pTerminalProcessorTypeSet->saveDataSet(_pProjPath,false);

	m_pMobElemTypeStrDB->saveDataSet( _pProjPath, false );

	m_pReportParaDB->saveDataSet( _pProjPath, false );
	
	m_pGateMappingDB->saveDataSet( _pProjPath,false );
	
	m_pUserNameListDB->saveDataSet( _pProjPath, false );

	m_pResourcePoolDB->saveDataSet( _pProjPath, false );

	m_pProcToResourceDB->saveDataSet( _pProjPath, false );

	m_pPaxBulkInfo->saveDataSet( _pProjPath, false );	

	m_pNonPaxRelPosSpec->saveDataSet(_pProjPath,false);
	
	CAircarftLayoutManager::GetInstance()->WriteLayOutToDataBase() ;
	// economic data
//	m_pProcLabourCostDB->saveDataSet(_pProjPath);
    m_pUserBarMan->saveDataSet(_pProjPath, false);
}


void InputTerminal::initStatics (void)
{
    inTypeList->initStrDict (inStrDict);
	/*
    ProcessorID::initStrDict (inStrDict);
    PassengerConstraint::initStrDict (inStrDict);
    PassengerConstraint::initTypeList (inTypeList);
    FlightConstraint::initSectors (inSectorList);
    FlightConstraint::initCategories (inCatList);*/
}



// deletes processor and all dependent data, and save this change to file.
void InputTerminal::deleteProcessor (Processor *p_proc,  const CString& _csProjPath )
{
    if (!p_proc)
        return;

    ProcessorID id = procList->getHighestSingleGroup (p_proc);
    flightSchedule->removeAllEntries (id,this);
    miscData->removeAllEntries (id,this);
    serviceTimeDB->removeAllEntries (id,this);
    m_pPassengerFlowDB->removeAllEntries (id);
    m_pStationPaxFlowDB->removeAllEntries (id);
	m_pSpecificFlightPaxFlowDB->removeAllEntries( id);
	
    procAssignDB->removeAllEntries (id,this);
    //flightSchedule->clearGates (id,this);
    peopleMovers->removeAllEntries (id,this);
	m_pMovingSideWalk->removeAllEntries( id,this );
	m_pMovingSideWalkPaxDist->removeAllEntries( id,this );
	
	GetProcessorList()->removeProcessor (p_proc);

    delete p_proc;

	SaveProcsData( _csProjPath );
}

// deletes old processor, adds new processor, and changes all
// dependent data to refer to new processor, and save this change to file.
void InputTerminal::replaceProcessor (Processor *p_oldProc,
    Processor *p_newProc, const CString& _sProjPath)
{
    GetProcessorList()->addProcessor (p_newProc);
    ProcessorID oldID = *p_oldProc->getID();//procList->getHighestSingleGroup (p_oldProc);
    ProcessorID newID = *p_newProc->getID();
	if( ! (oldID == newID ) )
	{
		flightSchedule->replaceAllEntries (oldID, newID,this);
		miscData->replaceAllEntries (oldID, newID,this);
		serviceTimeDB->replaceAllEntries (oldID, newID,this);
		m_pPassengerFlowDB->replaceAllEntries (oldID, newID,this);
		m_pStationPaxFlowDB->replaceAllEntries (oldID, newID,this);
		m_pSpecificFlightPaxFlowDB->replaceAllEntries( oldID, newID,this );
		procAssignDB->replaceAllEntries (oldID, newID,this);
		peopleMovers->replaceAllEntries (oldID, newID,this);
		m_pMovingSideWalk->replaceAllEntries( oldID , newID ,this);
		m_pMovingSideWalkPaxDist->replaceAllEntries( oldID, newID,this );
	}
    
	GetProcessorList()->removeProcessor (p_oldProc);

    delete p_oldProc;
	
	SaveProcsData( _sProjPath);
}
void InputTerminal::replaceProcessor (ProcessorID&oldID,
									  ProcessorID&newID, const CString& _sProjPath)
{
	if( ! (oldID == newID ) )
	{
		flightSchedule->replaceAllEntries (oldID, newID,this);
		miscData->replaceAllEntries (oldID, newID,this);
		serviceTimeDB->replaceAllEntries (oldID, newID,this);
		m_pPassengerFlowDB->replaceAllEntries (oldID, newID,this);
		m_pStationPaxFlowDB->replaceAllEntries (oldID, newID,this);
		m_pSpecificFlightPaxFlowDB->replaceAllEntries( oldID, newID,this );
		procAssignDB->replaceAllEntries (oldID, newID,this);
		peopleMovers->replaceAllEntries (oldID, newID,this);
		m_pMovingSideWalk->replaceAllEntries( oldID , newID ,this);
		m_pMovingSideWalkPaxDist->replaceAllEntries( oldID, newID,this );
	}
    
	GroupIndex proIndex = GetProcessorList()->getGroupIndex(oldID);
	if(proIndex.start!=-1)
	{
		for(int i=proIndex.start;i<=proIndex.end;i++)
           GetProcessorList()->removeProcessor (i);
	}
	
	SaveProcsData( _sProjPath);
}

BOOL InputTerminal::ProcessorRename(Processor *pOldProc,const char* newName, const CString& _sProjPath)
{

	if(!GetProcessorList()->IsValidProcessorName(newName))
		return false;

	//ProcessorID oldID = procList->getHighestSingleGroup (pOldProc);
	ProcessorID oldID = *pOldProc->getID();
	ProcessorID newID;
	newID.SetStrDict(inStrDict);
	newID.setID(newName);
    

	if(!GetProcessorList()->RenameProcessor(pOldProc,newName))
		return false;


// 	flightSchedule->replaceAllEntries (oldID, newID,this);
//     miscData->replaceAllEntries (oldID, newID,this);
//     serviceTimeDB->replaceAllEntries (oldID, newID,this);
//     m_pPassengerFlowDB->replaceAllEntries (oldID, newID,this);
//     m_pStationPaxFlowDB->replaceAllEntries (oldID, newID,this);
// 	m_pSpecificFlightPaxFlowDB->replaceAllEntries( oldID,newID,this );
//     procAssignDB->replaceAllEntries (oldID, newID,this);
//     peopleMovers->replaceAllEntries (oldID, newID,this);
	//m_pPipeDataSet->ReplaceMovingSideWalk( )

	SaveProcsData( _sProjPath );

	return true;

	

}
BOOL InputTerminal::ProcessorRename(ProcessorID&oldID,const char* newName, const CString& _sProjPath)
{
	//if(!procList->IsValidProcessorName(newName))
		
	//	return false;
	
	//ProcessorID oldID = procList->getHighestSingleGroup (pOldProc);
	//ProcessorID oldID = *pOldProc->getID();
	ProcessorID newID;
	newID.SetStrDict(inStrDict);
	newID.setID(newName);
	

	//if(!procList->RenameProcessor(pOldProc,newName))
	//	return false;
	
	if(GetProcessorList()->getGroupIndex(newID).start!=-1) 
		return false;
	
	GroupIndex pIndex =GetProcessorList()->getGroupIndex(oldID);
	if(pIndex.start!=-1)
	{
		for(int i=pIndex.start;i<=pIndex.end;i++)
		{
			const ProcessorID p_old = *GetProcessorList()->getProcessor(i)->getID();
			if(!GetProcessorList()->RenameProcessor(GetProcessorList()->getProcessor(i),newName))
				return false;
			const ProcessorID p_new = *GetProcessorList()->getProcessor(i)->getID();
			
			flightSchedule->replaceAllEntries (p_old, p_new,this);
			miscData->replaceAllEntries (p_old, p_new,this);
			serviceTimeDB->replaceAllEntries (p_old, p_new,this);
			m_pPassengerFlowDB->replaceAllEntries (p_old, p_new,this);
			m_pStationPaxFlowDB->replaceAllEntries (p_old, p_new,this);
			m_pSpecificFlightPaxFlowDB->replaceAllEntries( p_old,p_new,this );
			procAssignDB->replaceAllEntries (p_old, p_new,this);
			peopleMovers->replaceAllEntries (p_old, p_new,this);
		}
	}
	//m_pPipeDataSet->ReplaceMovingSideWalk( )
	
	SaveProcsData( _sProjPath );
	
	return true;
	
}
// deletes passenger type and all dependent data
void InputTerminal::deletePaxType (int p_level, int p_index)
{
    paxDataList->deletePaxType (p_level, p_index);
	bcPaxData->deletePaxType(p_level, p_index);
    m_pHubbing->deletePaxType (p_level, p_index);
    serviceTimeDB->deletePaxType (p_level, p_index);
    m_pPassengerFlowDB->deletePaxType (p_level, p_index);
    m_pStationPaxFlowDB->deletePaxType (p_level, p_index);
	m_pSpecificFlightPaxFlowDB->deletePaxType( p_level, p_index );
	
    procAssignDB->deletePaxType (p_level, p_index);
}

// saves all data files that contain passenger type info
void InputTerminal::savePaxData ( const CString& _csProjPath)
{
    paxDataList->saveDataSet(_csProjPath, false);
	bcPaxData->saveDataSet(_csProjPath, false);
    m_pHubbing->saveDataSet(_csProjPath, false);
    serviceTimeDB->saveDataSet(_csProjPath, false);
    m_pPassengerFlowDB->saveDataSet(_csProjPath, false);
    m_pStationPaxFlowDB->saveDataSet(_csProjPath, false);
	m_pSpecificFlightPaxFlowDB->saveDataSet( _csProjPath, false );
    procAssignDB->saveDataSet(_csProjPath, false);
}
// saves all data about processor identification.
void InputTerminal::SaveProcsData ( const CString& _csProjPath, bool _bUndoParam/*=false*/) const
{
	flightSchedule->saveDataSet(_csProjPath, _bUndoParam);
    miscData->saveDataSet(_csProjPath, _bUndoParam);
    serviceTimeDB->saveDataSet(_csProjPath, _bUndoParam);
    m_pPassengerFlowDB->saveDataSet(_csProjPath, _bUndoParam);
    m_pStationPaxFlowDB->saveDataSet(_csProjPath, _bUndoParam);
	m_pSpecificFlightPaxFlowDB->saveDataSet( _csProjPath, _bUndoParam );
    procAssignDB->saveDataSet(_csProjPath, _bUndoParam);
    peopleMovers->saveDataSet(_csProjPath, _bUndoParam);
	m_pMovingSideWalk->saveDataSet( _csProjPath, _bUndoParam);
	m_pMovingSideWalkPaxDist->saveDataSet( _csProjPath, _bUndoParam );
    procList->saveDataSet(_csProjPath, _bUndoParam);
	m_AirsideInput->SaveAirsideInputData(_csProjPath, _bUndoParam);
}

// read the DOS version's database
// move the new entry
void InputTerminal::ImportOldSector( const CString& _pProjPath )
{
/* [6/21/2004]
	int nDOSCount = inSectorList->getCount();
	for( int i=nDOSCount-1; i>=0; i-- )
	{
		Sector* pDOSSect = inSectorList->getItem( i );
		char szDOSCode[256];
		pDOSSect->getCode( szDOSCode );

		CSECTORLIST* pAllSectors = _g_GetActiveAirportMan( this )->GetSectorList();
		int nCount = pAllSectors->size();
		for( int m=0; m<nCount; m++ )
		{
			char szCode[256];
			CSector* pSector = pAllSectors->at( m );
			pSector->getCode( szCode );
			if( strcmp( szDOSCode, szCode ) == 0 )
				break;
		}

		if( m == nCount )
		{
			int nAirportCount = pDOSSect->getCount();
			for( int k=0; k<nAirportCount; k++ )
			{
				// new entry
				CAIRPORTFILTER af;
				af.code = pDOSSect->getString( k );
				_g_GetActiveAirportMan( this )->FindOrAddEntry( af.code );
				_g_GetActiveAirportMan( this )->AddSector( szDOSCode, af );

			}

		}
	}

	inSectorList->saveDataSet(_pProjPath, false);
	_g_GetActiveAirportMan( this )->SaveSectorsToFile();

*/
}


// clean all the input data	
void InputTerminal::CleanInputData()
{
	PLACE_METHOD_TRACK_STRING();
	// flightSchedule

	// procList
	procList->CleanData();
	m_AirsideInput->CleanInputData();
}


bool InputTerminal::IsLogValid()
{
	return m_bLogValid;
}

void InputTerminal::SetLogValid( bool _bValid )
{
	m_bLogValid = _bValid;
}

//////////////////////////////////////////////////////////////////////////
// add by bird 2003/6/18
void InputTerminal::setModifyFlag( bool _bModify )
{
	// false --> true
	if( !m_bModifyFlag && _bModify )
	{
		setCurrentSimResult( -1 ,true );
		setPreSimResult( -1 );
	}

	m_bModifyFlag = _bModify;
}

bool InputTerminal::getModifyFlag( void )
{
	return m_bModifyFlag;
}

void InputTerminal::setCurrentSimResult( int _index , bool _bNeedToUpdateBar )
{ 
	m_iCurrentSimResult = _index; 	
	// update the project bar
	if( _bNeedToUpdateBar )
		((CMainFrame*)AfxGetApp()->m_pMainWnd)->UpdateProjSelectBar();
}

int  InputTerminal::getCurrentSimResult( void )
{ 
	return m_iCurrentSimResult;	
}

void InputTerminal::setPreSimResult( int _index )
{
	m_iPreSimResult = _index;
}

int	 InputTerminal::getPreSimResult( void ) const
{
	return m_iPreSimResult;
}
bool InputTerminal::HasBrother(const ProcessorID& _idOwn,const ProcessorID&_father)
{
	
	for(int j=0;j<GetProcessorList()->getProcessorCount();j++) //find brother
	{
		if(_idOwn == *GetProcessorList()->getProcessor(j)->getID()) continue;// is   myself
		
		if(_father.idFits(*GetProcessorList()->getProcessor(j)->getID()))
			return true;
	}
	return false;
	
}

ProcessorList* InputTerminal::GetProcessorList()
{
	/*if(m_sysMode == EnvMode_Terminal)
	{*/
		return GetTerminalProcessorList();
	/*}
	else if (m_sysMode == EnvMode_AirSide)
	{
		return GetAirsideProcessorList();
	}
	else
	{
		ASSERT(0);
	}
	
	return NULL;*/
}

ProcessorList* InputTerminal::GetTerminalProcessorList()
{
	return procList;
}

ProcessorList* InputTerminal::GetAirsideProcessorList()
{
	return m_AirsideInput->GetProcessorList();
}

CNonPaxRelativePosSpec* InputTerminal::GetNonPaxRelativePosSpec() const
{
	return m_pNonPaxRelPosSpec;
}

TrayHostList* InputTerminal::GetTrayHostList()const
{
	return m_pTrayHostList;
}

bool InputTerminal::CheckConvertPaxGroupSizeData(const CString& _pProjPath )
{
	char filename[_MAX_PATH];
 	PROJMANAGER->getFileName (_pProjPath, PassengerDataFile, filename);

	ArctermFile file;
	try 
	{ 
		file.openFile (filename, READ); 
	}
	catch (FileOpenError *exception)
	{
		delete exception;
		paxDataList->initDefaultValues();
		paxDataList->saveDataSet(_pProjPath, false);
		return true;
	}

	float fVersionInFile = file.getVersion();
	file.closeIn();

	paxDataList->loadDataSet( _pProjPath );

	if( fVersionInFile < (float)2.80)
		return true;

	return false;
}