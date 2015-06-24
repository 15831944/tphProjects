#include "StdAfx.h"
#include "OnBoardSimEngine.h"
#include "carrierSpaceDiagnosis.h"
#include "../SimEngineConfig.h"
#include "OnBoardSimEngineConfig.h"
#include "../terminal.h"
#include "AgentsAnalyst.h"
#include "OnBoardCandidateFlights.h"
#include "Services/SeatAssignmentService.h"
#include "Services/BroadcastService.h"
#include "../../Common/ARCTracker.h"
#include "Engine/ARCportEngine.h"

//------------------------------------------------------------------------------------------------
//
#include <fstream>
class simEngineInchingSwitch
{
public:
	simEngineInchingSwitch()
		:mTileWidth(70),mTileHeight(70),
		mTravelsalDirectlyCost(10),mTravelsalDiagonalCost(14),
		mShowCarrierDeckGrid(0), mUseExtendBarrier(0),mTileLockIn(1)
	{
	}
	int mTileWidth, mTileHeight, mTravelsalDirectlyCost, mTravelsalDiagonalCost;
	
	int mShowCarrierDeckGrid, mUseExtendBarrier, mTileLockIn;

	/**
	Config File Format:

	PathFinding Config
	TileWidth(cm), TileHeight(cm), TravelsalDirectlyCost, mTravelsalDiagonalCost, \
	ShowBarrierGrid, Extend Barrier, TileLockIn
	50,50,10,14,1,1,1

	**/

	
	void loadConfig(std::string strFilePath){
		std::ifstream _cfg;	
		_cfg.open( strFilePath.c_str(),std::ios::in);
		if(_cfg.bad())
			return;
		char szBuffer[1024] = {0};	
		_cfg.getline(szBuffer,1024);_cfg.getline(szBuffer,1024);_cfg.getline(szBuffer,1024);
		char* b = szBuffer;	char* p = NULL;	int c = 1;
		while ( (p = strchr(b, ',')) != NULL){
			*p = '\0';
			if(c == 1) mTileWidth = atoi(b);
			if(c == 2) mTileHeight = atoi(b);
			if(c == 3) mTravelsalDirectlyCost = atoi(b);
			if(c == 4) mTravelsalDiagonalCost = atoi(b);

			if(c == 5) mShowCarrierDeckGrid = atoi(b);
			if(c == 6) mUseExtendBarrier = atoi(b);
			b = ++p;c++;
		}
		mTileLockIn = atoi(b);
		_cfg.close();
	}
	
};
//------------------------------------------------------------------------------------------------
//
OnBoardSimEngine::OnBoardSimEngine(int nPrjID, CARCportEngine* _pEngine, const CString& strProjPath)
:m_nProjID(nPrjID)
,//m_pInputTerm(_pInputTerm)
m_pEngine(_pEngine)
,m_strProjPath(strProjPath)
{
	m_pSimEngineConfig = new OnBoardSimEngineConfig();
	m_pEngine->getTerminal()->m_pVoidOnBoardConfig = (void*) m_pSimEngineConfig;

}

OnBoardSimEngine::~OnBoardSimEngine(void)
{
	delete m_pSimEngineConfig;
	m_pSimEngineConfig = NULL;
	m_pEngine->getTerminal()->m_pVoidOnBoardConfig = NULL;

	if(simEngineConfig()->isSimOnBoardAlone())
	{
		// cleanup service objects. need unregister from Deletion Manager.
		//DeletionManager::UnRegister(  seatAssignService() );
		seatAssignService()->ReleaseInstance();
		//DeletionManager::UnRegister(  broadcastService() );
		broadcastService()->ReleaseInstance();
		//DeletionManager::UnRegister(  carrierSpaceDiagnosis() );
		carrierSpaceDiagnosis()->ReleaseInstance();
	}
}

void OnBoardSimEngine::Initialize()
{
	PLACE_METHOD_TRACK_STRING();
	//-------------------------------------------------------------------------------
	// for debug purpose.
	if(false)
		carrierSpaceDiagnosis()->enableDiagnosis();
	simEngineInchingSwitch _config;
#ifdef _DEBUG
	_config.loadConfig("D:\\OnBoardSimEngine\\InchingSwitch.cfg");
#endif

	simEngineConfig()->setOnBoardModeTileWidth( _config.mTileWidth );
	simEngineConfig()->setOnBoardModeTileHeight( _config.mTileHeight );
	simEngineConfig()->setTravelsalDirectlyCost(_config.mTravelsalDirectlyCost);
	simEngineConfig()->setTravelsalDiagonalCost(_config.mTravelsalDiagonalCost);

	simEngineConfig()->setUseLockInTile( _config.mTileLockIn);
	simEngineConfig()->setUseExtendBarrier(_config.mUseExtendBarrier);

	// register need diagnosed mobile agents(which is moving in carrier space).
	// nTargetID load from file.
	//carrierSpaceDiagnosis()->registerDiagnosisAgent(nTargetID);

	//-------------------------------------------------------------------------------
	// load configuration data.
	m_pSimEngineConfig->loadConfig(m_nProjID, m_pEngine->getTerminal(), m_strProjPath );


	//-------------------------------------------------------------------------------
	// init service objects in OnBoard simEngine.
	seatAssignService()->initAssignmentRules();
	broadcastService()->Initialize();


	//-------------------------------------------------------------------------------
	// generate onBoard Mode mobile agents
	long pax = generateAgents();

}


int OnBoardSimEngine::eventListInitialize(EventList* _eventlist)
{
	return 0;
}

long OnBoardSimEngine::generateAgents()
{

	FlightSchedule* pFlightSchedule = &m_pEngine->m_simFlightSchedule;
	std::vector< int > onBoardFlightIDs;
	m_pSimEngineConfig->m_pCandidateFlightsInSim->getCandidateFlightIndexInSchedule(
		pFlightSchedule, onBoardFlightIDs);
    
	m_pEngine->getTerminal()->paxLog->clearLogs();

	m_pEngine->generateMobileAgents(&m_pEngine->m_simFlightSchedule, onBoardFlightIDs);

	// re-group to affinity group

	//scheduleEntryEvents(); //directly.


	return m_pEngine->getTerminal()->paxLog->getCount();
}






