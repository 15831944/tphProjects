#pragma once

#include <afx.h>

#include "results\Out_term.h"
#include "common\AIRSIDE_BIN.h"
#include "results\AirsideFlightLog.h"
#include "../Results/AirsideVehicleLog.h"
#include "Results/AirsideFlightDelayLog.h"
#include "./Results/AirsideRunwayOperationLog.h"
#include "../Results/AirsideFlightStairLog.h"
#include "../Results/OnboardDoorLog.h"



class CAirsideEventLogBufferManager;
class CARCportEngine;

class CAirsideSimLogs
{
public:
	CAirsideSimLogs(void);
	virtual ~CAirsideSimLogs(void);
public:

	//EventLog<AirsideFlightEventStruct> m_airsideFlightEventLogAnim;
	//AirsideFlightLog m_airsideFlightLogAnim;

	//flight delay log
	CAirsideFlightDelayLog& GetFlightDelayLog();
	CSimAndReportManager* GetSimReportManager();
	AirsideRunwayOperationLog& GetRunwayOperationLog();
	AirsideFlightStairLogs& GetFlightStairLog();
	OnboardDoorsLog& GetFlightDoorLog();
public:
	void CreateLogFiles(CARCportEngine* pEngine,const CString& _csProjPath);

	void OpenLogs(const CString& _csProjPath);

	void ClearLogs();

	//void SetTerminal(Terminal *pTerminal){   m_pTerminal = pTerminal ;}
	void CloseLogs();

	void CloseAndSaveLogs(const CString& _csProjPath);
	//Flight Events
	EventLog<AirsideFlightEventStruct> m_airsideFlightEventLog;
	//Vehicle Events
	EventLog<AirsideVehicleEventStruct> m_airsideVehicleEventLog;
	AirsideFlightLog m_airsideFlightLog;
	AirsideVehicleLog m_airsideVehicleLog;

protected:
	//for vehicle
	void ClearAirsideVehicleLog();
	void CreateAirsideVehicleLogFile(CARCportEngine* pEngine,const CString& strProjPath);
	void ClearAirsideFlightLog();
	void CreateAirsideFlightLogFile(CARCportEngine* pEngine,const CString& strProjPath);

	void OpenFlightLogs(const CString& _csProjPath);
	void OpenVehicleLogs(const CString& _csProjPath);
	void OpenFlightStairLogs(const CString& _csProjPath);

	//close log without save data
	void CloseAirsideVehicleLog();	
	//close log without save data
	void CloseAirsideFilghtLog();

	
	//close log with save data
	void CloseAirsideFilghtLog(const CString& strProjPath);
	//close log with save data
	void CloseAirsideVehicleLog(const CString& strProjPath);	


	//CAirsideEventLogBufferManager *m_pAirsideEventLogBufferManager; 
	

protected:
	//Terminal *m_pTerminal;

	CSimAndReportManager* m_pSimAndReportManager;


	CAirsideFlightDelayLog m_flightDelayLog;


	//runway operation log
	AirsideRunwayOperationLog m_runwayOperationLog;
	AirsideFlightStairLogs m_flightStairlog;

	OnboardDoorsLog m_doorLog;

};
