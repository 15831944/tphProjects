#include "StdAfx.h"
#include ".\outputairside.h"
#include "../engine/terminal.h"
#include "../Reports/SimGeneralPara.h"
#include "../Inputs/SimParameter.h"
#include "../Inputs/IN_TERM.H"
#include "./../Engine/AirsideLogDatabase.h"
#include "./../Results/AirsideSimLogs.h"
#include "./../Results/AirsideFlightLog.h"
#include "./../Common/SimAndReportManager.h"
#include "../Engine/Airside/AirsideVehicleInSim.h"
#include "../Engine/Airside/AirsideFlightDelay.h"
#include "../Engine/Airside/AirsideFlightInSim.h"
#include "./AirsideEventLogBuffer.h"
#include "../Common/ARCTracker.h"
#include "AirsideFlightStairLog.h"
#include "../Engine/Airside/AirsideFlightStairsLog.h"
#include "../engine/ARCportEngine.h"
#include "../Results/OnboardDoorLog.h"

bool OutputAirside::LogFlightEntry( AirsideFlightInSim* fltID, const AirsideFlightDescStruct& logEntryDesc )
{	
	memcpy((char*)&m_vDescList[fltID],(char*)&logEntryDesc,sizeof(AirsideFlightDescStruct));
	return true;
}

bool OutputAirside::LogFlightEvent( AirsideFlightInSim* fltID, const AirsideFlightEventStruct& logItem )
{
	//AirsideFlightLogEntry entry;
	m_vEventListList[fltID].push_back(logItem); 	
	return true;
}

bool OutputAirside::LogVehicleEntry(AirsideVehicleInSim* Vehicleid, const AirsideVehicleDescStruct& logEntryDesc )
{
	memcpy((char*)&m_vVehicleDescList[Vehicleid],(char*)&logEntryDesc,sizeof(AirsideVehicleDescStruct));
	return true;
}

bool OutputAirside::LogVehicleEvent(AirsideVehicleInSim* Vehicleid, const AirsideVehicleEventStruct& logItem)
{
	m_vVehicleEventListList[Vehicleid].push_back(logItem); 	
	return true;
}

bool OutputAirside::FlushLog(int nPrjID)
{
	PLACE_TRACK_STRING("2010223-10:58:14");
	FlushFlightLog(nPrjID);
	PLACE_TRACK_STRING("2010223-10:58:15");
	FlushVehicleLog(nPrjID);
	PLACE_TRACK_STRING("2010223-10:58:16");

	return true;
}

bool OutputAirside::FlushVehicleLog(int nPrjID)
{
	PLACE_METHOD_TRACK_STRING();
	long startPos = 0;
	std::map<AirsideVehicleInSim*, AirsideVehicleDescStruct>::iterator itr;

	for(itr = m_vVehicleDescList.begin(); itr!=m_vVehicleDescList.end();itr++ )
	{
		AirsideVehicleInSim* vehicleid = (*itr).first;
		AirsideVehicleDescStruct& vehicleDesc = (*itr).second;
		const AirsideVehicleEventList & vehicleEventList = m_vVehicleEventListList[vehicleid];

		vehicleDesc.startPos = startPos;
		vehicleDesc.endPos = startPos + (vehicleEventList.size()-1) * sizeof(AirsideVehicleEventStruct);
		startPos += vehicleEventList.size() * sizeof(AirsideVehicleEventStruct);

		vehicleDesc.startTime = vehicleEventList.size() ? vehicleEventList.begin()->time : 0;
		vehicleDesc.endTime = vehicleEventList.size() ? vehicleEventList.rbegin()->time : 0;

		AirsideVehicleLogEntry entry;								//
		entry.SetEventLog(&m_airsideLog.m_airsideVehicleEventLog);
		entry.setEventList(vehicleEventList);
		entry.clearLog();

		m_airsideLog.m_airsideVehicleLog.addItem(vehicleDesc);		

	}

	return true;

}

void OutputAirside::Clear()
{
	m_vEventListList.clear();
	m_vDescList.clear();
	m_vVehicleEventListList.clear();
	m_vVehicleDescList.clear();
	//clear airside logs
	m_airsideLog.ClearLogs();

}

void OutputAirside::PrepareLog( CARCportEngine* pEngine,const CString& _csProjPath )
{
	PLACE_METHOD_TRACK_STRING();
	//m_airsideLog.SetTerminal(pInterm);

	Terminal* pInterm = pEngine->getTerminal();
	m_airsideLog.ClearLogs();

	int iResult = m_airsideLog.GetSimReportManager()->getSubSimResultCout();
	
	m_airsideLog.GetSimReportManager()->addNewSimResult(pInterm->m_pSimParam,_csProjPath,pInterm);		

	m_airsideLog.GetSimReportManager()->SetCurrentSimResult(pEngine->GetRunNumber() - 1);
	m_airsideLog.CloseAndSaveLogs(_csProjPath);
	m_airsideLog.CreateLogFiles(pEngine,_csProjPath);
	

//	m_airsideLog.OpenLogs(_csProjPath);
	m_reportLog.OpenWrite(pInterm,_csProjPath);
	Clear();	
	
}


void OutputAirside::SaveLog( const CString& _csProjPath )
{
	PLACE_METHOD_TRACK_STRING();
	m_airsideLog.CloseAndSaveLogs(_csProjPath);

	m_airsideLog.GetSimReportManager()->saveDataSet2(_csProjPath,false,2);
	m_airsideLog.GetSimReportManager()->SetCurrentSimResult(0);
	m_airsideLog.GetSimReportManager()->SetInputModified( false );
	m_airsideLog.GetSimReportManager()->saveDataSet( _csProjPath, false );

	//
	m_reportLog.CloseSave(_csProjPath);
}

int OutputAirside::LogFlightDelay( AirsideFlightDelay* delay )
{
	m_airsideLog.GetFlightDelayLog().AddItem(delay);
	int nID =  m_airsideLog.GetFlightDelayLog().GetItemCount();
	delay->SetUniqueID(nID);
	return nID;
}

bool OutputAirside::FlushFlightLog( int nPrjID )
{
	PLACE_METHOD_TRACK_STRING();
	long startPos = 0;
	std::map<AirsideFlightInSim*, AirsideFlightDescStruct>::iterator itr;

	for(itr = m_vDescList.begin(); itr!=m_vDescList.end();itr++ )
	{
		AirsideFlightInSim* fltid = (*itr).first;
		AirsideFlightDescStruct& fltDesc = fltid->GetLogEntry().GetAirsideDesc();
		AirsideFlightEventList & fltEventList = m_vEventListList[fltid];

		fltDesc.startPos = startPos;
		fltDesc.endPos = startPos + (fltEventList.size()-1) * sizeof(AirsideFlightEventStruct);
		startPos += fltEventList.size() * sizeof(AirsideFlightEventStruct);

		fltDesc.startTime = fltEventList.size() ? fltEventList.begin()->time : 0;
		fltDesc.endTime = fltEventList.size() ? fltEventList.rbegin()->time : 0;

		fltDesc.arrTime = fltid->GetArrTime();
		fltDesc.depTime = fltid->GetDepTime();

		AirsideFlightLogEntry entry;
		entry.SetEventLog(&m_airsideLog.m_airsideFlightEventLog);
		entry.setEventList(fltEventList);
		entry.clearLog();

		m_airsideLog.m_airsideFlightLog.addItem(fltDesc);		

		/*AirsideFlightLogEntry entryAnim;
		entryAnim.SetEventLog(&m_airsideLog.m_airsideFlightEventLogAnim);
		entryAnim.setEventList(fltEventList,fltEventList.size());
		entryAnim.clearLog();

		m_airsideLog.m_airsideFlightLogAnim.addItem(fltDesc);*/
	}
	return true;

}

OutputAirside::OutputAirside()
{
	m_pAirsideEventLogBufferManager = new CAirsideEventLogBufferManager(&m_airsideLog);
}

OutputAirside::~OutputAirside()
{
	delete m_pAirsideEventLogBufferManager;
}

void OutputAirside::PrepareAnimLogs( const CString& _csProjPath)
{
	m_airsideLog.OpenLogs(_csProjPath);
	
	CString sFileName = m_airsideLog.GetSimReportManager()->GetCurrentLogsFileName(AirsideVehicleEventFile, _csProjPath );
	m_pAirsideEventLogBufferManager->SetVehicleLogFilepath(sFileName);

	sFileName = m_airsideLog.GetSimReportManager()->GetCurrentLogsFileName(AirsideEventFile,_csProjPath);
	m_pAirsideEventLogBufferManager->SetFlightLogFilePath(sFileName);

	m_pAirsideEventLogBufferManager->LoadDataIfNecessary();	

}

int OutputAirside::LogRunwayOperationDelay( AirsideRunwayOperationLog::RunwayLogItem* pLogItem )
{
	m_airsideLog.GetRunwayOperationLog().AddItem(pLogItem);

	return 0;
}

int OutputAirside::LogFlightStair( AirsideFlightStairsLog* pLog )
{
	m_airsideLog.GetFlightStairLog().AddItem(pLog);

	return 0;
}

int OutputAirside::LogFlightDoor( OnboardDoorLog* pLog )
{
	m_airsideLog.GetFlightDoorLog().AddItem(pLog);

	return 0;
}