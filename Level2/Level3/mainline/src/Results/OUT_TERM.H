
#ifndef OUTPUT_TERMINAL_H
#define OUTPUT_TERMINAL_H

#include "../common/template.h"
#include "eventlog.h"
#include "resultsdll.h"
#include "../common/term_bin.h"
class CSimAndReportManager;
class CElevatorLog;
class CEventLogBufManager;
class ResourceElementLog;
class CPaxDestDiagnoseLog;
class CBridgeConnectorLog;
class CRetailLog;
class TempMobileEventLog;

class OutputTerminal
{
public:
	EventLog<BagEventStruct>* m_pBagEventLog;
	EventLog<FlightEventStruct>* m_pFltEventLog;
	
	//mobile event log
	EventLog<MobEventStruct>* m_pMobEventLog;
	//temporary mobile element event log 
	TempMobileEventLog *m_pTempMobEventLog;


	EventLog<ProcEventStruct>* m_pProcEventLog;
	EventLog<TrainEventStruct>* m_pTrainEventLog;
	EventLog<ElevatorEventStruct>* m_pElevatorEventLog;
	EventLog<ResEventStruct>* m_pResourceEventLog;

	EventLog<BridgeConnectorEventStruct>* m_pBridgeConnectorEventLog;
	EventLog<RetailEventStruct>* m_pRetailEventLog;

    PaxLog *paxLog;
    FlightLog *flightLog;
    BagLog *bagLog;
    ProcLog *procLog;
	CTrainLog* m_pTrainLog;
	CElevatorLog* m_pElevatorLog;
	ResourceElementLog * resourceLog;
	//bridge connector log
	CBridgeConnectorLog *m_pBridgeConnectorLog;
	//retail log
	CRetailLog	*m_pRetailLog;
	// diagnose log
	CPaxDestDiagnoseLog* m_pDiagnosLog;

    StringDictionary *outStrDict;
    PassengerTypeList *outTypeList;
    CategoryList *outCatList;
	CEventLogBufManager* m_pLogBufManager;
protected:
	CSimAndReportManager* m_pSimAndReportManager;
public:
	CEventLogBufManager* GetEventLogBufmanager(){ return m_pLogBufManager; }
	CSimAndReportManager* GetSimReportManager(){ return m_pSimAndReportManager;	};
	int resultsLoaded();
    OutputTerminal ();
    virtual ~OutputTerminal ();

	// call by start of simulation 
	void clearLogs ();

	// Description: call by report to load log if necessary
	// Exception:	FileVersionTooNewError
    void openLogs ( const CString& _csProjPath );
    
	// call at the end of simulation
	void closeLogs ( const CString& _csProjPath );


	void closeLogs ();
};


#endif
