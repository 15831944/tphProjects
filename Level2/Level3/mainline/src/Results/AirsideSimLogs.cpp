#include "StdAfx.h"

#include ".\airsidesimlogs.h"
#include "EventLogBufManager.h"
#include "common\SimAndReportManager.h"
#include "../engine/terminal.h"
#include "../Reports/SimGeneralPara.h"
#include "../Inputs/SimParameter.h"
#include "./AirsideEventLogBuffer.h"
#include "Engine/ARCportEngine.h"

CAirsideSimLogs::CAirsideSimLogs(void)
{
	m_airsideFlightLog.SetEventLog(&m_airsideFlightEventLog);
	//m_airsideFlightLogAnim.SetEventLog(&m_airsideFlightEventLogAnim);

	m_airsideVehicleLog.SetEventLog(&m_airsideVehicleEventLog);

	m_pSimAndReportManager = new CSimAndReportManager();
	//m_pAirsideEventLogBufferManager = new CAirsideEventLogBufferManager(this);
}

CAirsideSimLogs::~CAirsideSimLogs(void)
{
	//ClearAirsideFlightLog();
	//ClearAirsideVehicleLog();
	delete m_pSimAndReportManager;
}



void CAirsideSimLogs::OpenLogs(const CString& _cstrProjPath)
{
	OpenFlightLogs(_cstrProjPath);
	OpenVehicleLogs(_cstrProjPath);
}

void CAirsideSimLogs::OpenVehicleLogs(const CString& _csProjPath)
{
	CProgressBar bar( "Loading Logs Data For The First Time", 100, 9, TRUE, 0 );

	
	int iResult = GetSimReportManager()->getSubSimResultCout();
	if (iResult < 1)
	{
		return;
	}
//	GetSimReportManager()->SetCurrentSimResult(iResult < 0 ? -1 : 0);

	//clear events
	m_airsideVehicleLog.clearEvents();//ClearAirsideVehicleLog();

	CString sFileName = GetSimReportManager()->GetCurrentLogsFileName(AirsideVehicleDescFile, _csProjPath );	
	m_airsideVehicleLog.LoadDataIfNecessary(sFileName);	
	bar.StepIt();

	sFileName = m_pSimAndReportManager->GetCurrentLogsFileName(AirsideVehicleEventFile, _csProjPath );	
	m_airsideVehicleLog.LoadEventDataIfNecessary(sFileName);
	bar.StepIt();

	//m_pAirsideEventLogBufferManager->SetVehicleLogFilepath(sFileName);
	//m_pAirsideEventLogBufferManager->LoadDataIfNecessary();	
}


void CAirsideSimLogs::CreateAirsideFlightLogFile(CARCportEngine* pEngine,const CString& strProjPath)
{
	CProgressBar bar( "Loading Logs Data For The First Time", 100, 9, TRUE, 0 );

	ASSERT(pEngine != NULL);

	Terminal* pInterm = pEngine->getTerminal();
	ClearAirsideFlightLog();


	int iResult = GetSimReportManager()->getSubSimResultCout();
	GetSimReportManager()->SetCurrentSimResult(pEngine->GetRunNumber() - 1);
	GetSimReportManager()->saveDataSet(strProjPath,false);
	CString sFileName = GetSimReportManager()->GetCurrentLogsFileName( AirsideEventFile, strProjPath );	
	m_airsideFlightLog.createEventFile( sFileName );
	bar.StepIt();
	//sFileName = GetSimReportManager()->GetCurrentLogsFileName( AirsideEventFileAnim, strProjPath );	
	//m_airsideFlightLogAnim.createEventFile( sFileName );
	//bar.StepIt();

	//delay logs
	sFileName = GetSimReportManager()->GetCurrentLogsFileName(AirsideFlightDelayFile, strProjPath);
	m_flightDelayLog.CreateLogFile(sFileName);
	
	//runway resource operation log
	sFileName = GetSimReportManager()->GetCurrentLogsFileName(AirsideRunwayOperationFile, strProjPath);
	m_runwayOperationLog.CreateLogFile(sFileName);

	//stair log
	sFileName = GetSimReportManager()->GetCurrentLogsFileName(FlightStairEventFile,strProjPath);
	m_flightStairlog.CreateLogFile(sFileName);

	//door log
	sFileName = GetSimReportManager()->GetCurrentLogsFileName(FlightDoorEventFile,strProjPath);
	m_doorLog.CreateLogFile(sFileName);

	bar.StepIt();
}

void CAirsideSimLogs::CreateAirsideVehicleLogFile(CARCportEngine* pEngine,const CString& strProjPath)
{
	CProgressBar bar( "Loading Logs Data For The First Time", 100, 9, TRUE, 0 );
	ASSERT(pEngine != NULL);

	Terminal* pInterm = pEngine->getTerminal();
	ClearAirsideVehicleLog();

	//GetSimReportManager()->addNewSimResult(m_pTerminal->m_pSimParam,strProjPath,m_pTerminal);

	int iResult = GetSimReportManager()->getSubSimResultCout();
	GetSimReportManager()->SetCurrentSimResult(pEngine->GetRunNumber() - 1);
	GetSimReportManager()->saveDataSet(strProjPath,false);
	CString sFileName = GetSimReportManager()->GetCurrentLogsFileName( AirsideVehicleEventFile, strProjPath );	
	m_airsideVehicleLog.createEventFile( sFileName );
	bar.StepIt();
}

//close log without save data
void CAirsideSimLogs::CloseAirsideFilghtLog()
{
	m_airsideFlightEventLog.closeEventFile();
	//m_airsideFlightEventLogAnim.closeEventFile();
	m_flightDelayLog.ClearData();
	m_runwayOperationLog.ClearData();
	m_flightStairlog.ClearData();
	m_doorLog.ClearData();
}

void CAirsideSimLogs::CloseAirsideVehicleLog()
{
	m_airsideVehicleEventLog.closeEventFile();
}

//close log with save data
void CAirsideSimLogs::CloseAirsideFilghtLog(const CString& strProjPath)
{
	CString sFileName = "";
	sFileName = GetSimReportManager()->GetCurrentLogsFileName( AirsideDescFile, strProjPath );
	m_airsideFlightLog.SaveToFile( sFileName );

	//sFileName = GetSimReportManager()->GetCurrentLogsFileName( AirsideDescFileAnim, strProjPath );
	//m_airsideFlightLogAnim.SaveToFile( sFileName );

	sFileName = GetSimReportManager()->GetCurrentLogsFileName( AirsideEventFile, strProjPath );
	m_airsideFlightLog.SaveEventToFile( sFileName );

	//sFileName = GetSimReportManager()->GetCurrentLogsFileName( AirsideEventFileAnim, strProjPath );
	//m_airsideFlightLogAnim.SaveEventToFile( sFileName );
 
	sFileName = GetSimReportManager()->GetCurrentLogsFileName( AirsideFlightDelayFile, strProjPath );
	m_flightDelayLog.SaveData(sFileName);
	m_flightDelayLog.ClearData();

	//runway operation log
	sFileName = GetSimReportManager()->GetCurrentLogsFileName( AirsideRunwayOperationFile, strProjPath );
	m_runwayOperationLog.SaveData(sFileName);
	m_runwayOperationLog.ClearData();

	//stair log
	sFileName = GetSimReportManager()->GetCurrentLogsFileName(FlightStairEventFile,strProjPath);
	m_flightStairlog.writeData(sFileName);
	m_flightStairlog.ClearData();

	//door log
	sFileName = GetSimReportManager()->GetCurrentLogsFileName(FlightDoorEventFile,strProjPath);
	m_doorLog.writeData(sFileName);
	m_doorLog.ClearData();

	ClearAirsideFlightLog();
}

void CAirsideSimLogs::CloseAirsideVehicleLog(const CString& strProjPath)
{
	CString sFileName = "";
	sFileName = GetSimReportManager()->GetCurrentLogsFileName( AirsideVehicleEventFile, strProjPath );
	m_airsideVehicleLog.SaveEventToFile( sFileName );

	sFileName = GetSimReportManager()->GetCurrentLogsFileName( AirsideVehicleDescFile, strProjPath );
	m_airsideVehicleLog.SaveToFile( sFileName );

	ClearAirsideVehicleLog();
}

CAirsideFlightDelayLog& CAirsideSimLogs::GetFlightDelayLog()
{
	return m_flightDelayLog;
}

void CAirsideSimLogs::OpenFlightLogs( const CString& _csProjPath )
{
	CProgressBar bar( "Loading Logs Data For The First Time", 100, 9, TRUE, 0 );

	int iResult = GetSimReportManager()->getSubSimResultCout();
	if (iResult < 1)
	{
		return;
	}
//	GetSimReportManager()->SetCurrentSimResult(iResult < 0 ? -1 : 0);

	ClearAirsideFlightLog();	

	CString sFileName = GetSimReportManager()->GetCurrentLogsFileName(AirsideDescFile, _csProjPath );	
	m_airsideFlightLog.LoadDataIfNecessary(sFileName);	
	bar.StepIt();

	sFileName = m_pSimAndReportManager->GetCurrentLogsFileName(AirsideEventFile, _csProjPath );	
	m_airsideFlightLog.LoadEventDataIfNecessary(sFileName);
	bar.StepIt();

	//sFileName = GetSimReportManager()->GetCurrentLogsFileName(AirsideDescFileAnim, _cstrProjPath );	
	//m_airsideFlightLogAnim.LoadDataIfNecessary(sFileName);

	bar.StepIt();

	//sFileName = m_pSimAndReportManager->GetCurrentLogsFileName(AirsideEventFileAnim, _cstrProjPath );	
	//m_airsideFlightLogAnim.LoadEventDataIfNecessary(sFileName);
	//bar.StepIt();

	sFileName = m_pSimAndReportManager->GetCurrentLogsFileName(AirsideFlightDelayFile, _csProjPath);
	m_flightDelayLog.LoadData(sFileName);

	bar.StepIt();
	sFileName = m_pSimAndReportManager->GetCurrentLogsFileName(AirsideRunwayOperationFile, _csProjPath);
	m_runwayOperationLog.LoadData(sFileName);

	bar.StepIt();
	sFileName = m_pSimAndReportManager->GetCurrentLogsFileName(FlightStairEventFile, _csProjPath);
	m_flightStairlog.readData(sFileName);

	bar.StepIt();
	sFileName = m_pSimAndReportManager->GetCurrentLogsFileName(FlightDoorEventFile, _csProjPath);
	m_doorLog.readData(sFileName);

}

CSimAndReportManager* CAirsideSimLogs::GetSimReportManager()
{
	return m_pSimAndReportManager;
}

void CAirsideSimLogs::CloseAndSaveLogs( const CString& _csProjPath )
{
	CloseAirsideFilghtLog(_csProjPath);
	CloseAirsideVehicleLog(_csProjPath);
}

void CAirsideSimLogs::ClearAirsideVehicleLog()
{
	m_airsideVehicleLog.clearLogs();
	//m_airsideVehicleEventLog.clearEvents();
}

void CAirsideSimLogs::ClearAirsideFlightLog()
{
	m_airsideFlightLog.clearLogs();
	//m_airsideFlightEventLog.clearEvents();
}

void CAirsideSimLogs::ClearLogs()
{
	ClearAirsideVehicleLog();
	ClearAirsideFlightLog();
}

void CAirsideSimLogs::CreateLogFiles(CARCportEngine* pEngine, const CString& _csProjPath )
{
	//ASSERT(pEngine != NULL);

	//Terminal* pInterm = pEngine->getTerminal();

	CreateAirsideFlightLogFile(pEngine,_csProjPath);
	CreateAirsideVehicleLogFile(pEngine,_csProjPath);
}

AirsideRunwayOperationLog& CAirsideSimLogs::GetRunwayOperationLog()
{
	return m_runwayOperationLog;
}

AirsideFlightStairLogs& CAirsideSimLogs::GetFlightStairLog()
{
	return m_flightStairlog;
}

OnboardDoorsLog& CAirsideSimLogs::GetFlightDoorLog()
{
	return m_doorLog;
}