
/*****
*
*   Module:     InputTerminal
*   Author:     Lincoln Fellingham
*   Date:       Jan 96
*   Purpose:    Defines a single, cohesive object that contains all
*               input databases in ARCTERM. Typically, one global
*               InputTerminal will be allocated, and reloaded as
*               necessary with various data sets
*
*               This class has also been defined to allow multiple
*               instances to be loaded with different data sets,
*               permitting comparison of inputs or data transfers
*               between them, although only one can be the "current
*               terminal" at a time
*
*               Note that all data sets are defined as public. This is
*               done to provide external access to all other ARCTERM
*               modules, similar to a GLOBAL structure commonly used in C
*
*****/

#ifndef TERMINAL_INPUT_DATA_H
#define TERMINAL_INPUT_DATA_H

#include "..\common\template.h"
#include "inputsdll.h"

//
class CAirsideInput;
class CPipeDataSet;
class CTerminalProcessorTypeSet;
class CSubFlowList;
class CAllGreetingProcessors;
class CReportParaDB;
class CMonitor;
class CAutoRoster;
class CAutoCreateProcessorContainer;
class CUserNameDataSet;
class FlightScheduleCriteriaDB;
class CResourcePoolDataSet;
class CProcToResourceDataSet;
class CAirportDatabase;
class CAreas;
class CPaxBulkInfo;
class CStructureList;
class WallShapeList;
class CInputFlowSync;

class IntermediateStandAssign;
class CNonPaxRelativePosSpec;
class TrayHostList;
class CAffinityGroup;
class CProcRosterRules;
class ProductStockingList;
#include <common/ENV_MODE.h>

class INPUTS_TRANSFER InputTerminal
{
public:
	static char m_csDisplayMsg[512];

	CSimParameter* m_pSimParam;
	
	// OnBoard Config handle
	void* m_pVoidOnBoardConfig;

    // Miscellaneous Parameters
	StringDictionary *inStrDict;
	StringDictionary *inUndefineShapeStrDict;
	StringDictionary *inDefineShapeStrDict;

    // Flight data
    FlightSchedule *flightSchedule;
	FlightScheduleCriteriaDB* m_pFlightScheduleCriteriaDB;
    FlightData *flightData;
//    SectorList *inSectorList;	//replace its action by CAirportsManager::m_vSector.[6/21/2004]
    CHubbingDatabase* m_pHubbing;

    // Passenger data
    PassengerTypeList *inTypeList;
    PassengerDistributionList *paxDistList;
    PassengerData *paxDataList;

	// Bridge Connector passenger data.
	BridgeConnectorPaxData* bcPaxData;
    // Baggage Data
    BaggageData *bagData;

    // Processor data
    ProcessorList *procList;
    MiscDatabase *miscData;
    ServiceTimeDatabase *serviceTimeDB;
    PassengerFlowDatabase* m_pPassengerFlowDB;
    PassengerFlowDatabase* m_pStationPaxFlowDB;

	PassengerFlowDatabase* m_pSpecificFlightPaxFlowDB;

    ProcAssignDatabase *procAssignDB;
    PeopleMoverDataSet *peopleMovers;
	CSideMoveWalkPaxDist* m_pMovingSideWalkPaxDist;

	// surface area
	CStructureList* m_pStructureList;
	WallShapeList * m_pWallShapeList;

	//priority assignment info
	GatePriorityInfo *gatePriorityInfo;
	FlightPriorityInfo *flightPriorityInfo;

	//Baggage device info
	BagGateAssign *bagGateAssignDB;

	//all railway info
	CRailWayData* pRailWay;
	CStationLayout* pStationLayoutSample;
	CAllCarSchedule* m_pAllCarSchedule;

	//pipe data set
	CPipeDataSet* m_pPipeDataSet;
	CTerminalProcessorTypeSet* m_pTerminalProcessorTypeSet;
	//moving side walk info
	CSideMoveWalkDataSet* m_pMovingSideWalk;	

	CMobileElemTypeStrDB* m_pMobElemTypeStrDB;
	CGateMappingDB* m_pGateMappingDB;
	CSubFlowList* m_pSubFlowList;

	CAllGreetingProcessors* m_pGreetingProcessors;

	CReportParaDB* m_pReportParaDB;
	CMonitor* m_pTLOSMonitor;
	CAutoRoster* m_pAutoRoster;
	CAutoCreateProcessorContainer* m_pAutoCreatedProcContainer;
	CUserNameDataSet* m_pUserNameListDB;
	
	// resource info
	CResourcePoolDataSet* m_pResourcePoolDB;
	CProcToResourceDataSet* m_pProcToResourceDB;
	CPaxBulkInfo*   m_pPaxBulkInfo;
	
	CAirportDatabase*		m_pAirportDB;
	//layout for layout view 
	//CAircaftLayOut* m_pLayout ;
	// areas
	CAreas*			m_pAreas;

	CInputFlowSync* m_pInputFlowSync;


	IntermediateStandAssign* m_pIntermediateStand;
	CAffinityGroup* m_pAffinityGroup;
	CProcRosterRules* m_pProcRosterRules;

	ProductStockingList* m_pProductStockingList;

	int m_nProjID;
	CNonPaxRelativePosSpec* GetNonPaxRelativePosSpec()const;
	TrayHostList* GetTrayHostList()const;

protected:
	CNonPaxRelativePosSpec* m_pNonPaxRelPosSpec;
	TrayHostList*			m_pTrayHostList;			
public:
    InputTerminal ();
    virtual ~InputTerminal ();

	// Description: Load all Input Data From Default Files
	// Exception:	FileVersionTooNewError
	virtual void InitCongestionParam(const CString& _pProjPath){}

    void loadInputs ( const CString& _csProjPath );

    void saveInputs ( const CString& _csProjPath) const;
    void initStatics (void);

    // deletes processor and all dependent data
    void deleteProcessor (Processor *p_proc,  const CString& _csProjPath);

    // deletes old processor, adds new processor, and changes all
    // dependent data to refer to new processor
    void replaceProcessor (Processor *p_oldProc, Processor *p_newProc, const CString& _sProjPath);
 
	void replaceProcessor(ProcessorID &_oldProcID, ProcessorID&_newProcID, const CString& _sProjPath);
    // deletes passenger type and all dependent data
    void deletePaxType (int p_level, int p_index);

    // saves all data files that contain passenger type info
    void savePaxData ( const CString& _csProjPath);

	// saves all data about processor identification.
	void SaveProcsData ( const CString& _csProjPath , bool _bUndoParam=false) const;
		
	void linkFlightData(int prjID);
	void linkFlightData();

	// read the DOS version's database
	// move the new entry
	void ImportOldSector( const CString& _pProjPath );

	BOOL ProcessorRename(Processor *pOldProc,const char* newName, const CString& _sProjPath);
	BOOL ProcessorRename(ProcessorID&pOldID,const char* newName,const CString&_sProjPath);
	// clean all the input data	
	void CleanInputData();

	bool IsLogValid();

	void SetLogValid( bool _bValid );

protected:
	bool m_bLogValid;		// need to be checked before anim or report the log.

	bool CheckConvertPaxGroupSizeData(const CString& _pProjPath );

//////////////////////////////////////////////////////////////////////////
// add by bird 2003/6/18	
private:
	// flag of the input data have been modifyed
	bool m_bModifyFlag;		

	// the index of current SimResult. is -1, if no valid SimResult.
	int m_iCurrentSimResult;
	
	// the index of pre SimResult,  is -1, if no valid SimResult.
	int m_iPreSimResult;

	//EnvironmentMode m_sysMode;	
protected:

public:
	void setModifyFlag( bool _bModify );
	bool getModifyFlag( void );

	void setCurrentSimResult( int _index , bool _bNeedToUpdateBar = false);
	int  getCurrentSimResult( void );

	void setPreSimResult( int _index );
	int	 getPreSimResult( void ) const;
public:
	bool HasBrother(const ProcessorID& _idOwn,const ProcessorID&_father);

public:	
	CAirsideInput* m_AirsideInput;
	CAirsideInput*GetAirsideInput(){ return m_AirsideInput;}
	//void SetSystemMode(EnvironmentMode mode) { m_sysMode = mode; }
	//EnvironmentMode GetSystemMode() const { return m_sysMode; }

	ProcessorList* GetProcessorList();
	//read old project;
	ProcessorList* GetTerminalProcessorList();
	ProcessorList* GetAirsideProcessorList();
	CInputFlowSync* GetInputFlowSync() { return m_pInputFlowSync; }
};

#endif
