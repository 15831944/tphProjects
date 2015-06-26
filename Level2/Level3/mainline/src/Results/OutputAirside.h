#pragma once
#include "./../Common/AIRSIDE_BIN.h"
#include "./AirsideFlightLog.h"
#include "./AirsideSimLogs.h"
#include "./AirsideFlightDelayLog.h"
#include "./AirsideRunwayOperationLog.h"
#include <map>
#include "./AirsideReportLog.h"


typedef std::vector<AirsideFlightEventStruct> AirsideFlightEventList;
typedef std::vector<AirsideVehicleEventStruct> AirsideVehicleEventList;
class Terminal;
class Flight;
class AirsideFlightInSim;
class AirsideVehicleInSim;
class AirsideFlightDelay;
class AirsideFlightStairsLog;
class CARCportEngine;
class OnboardDoorLog;
class OutputAirside
{
public:
	OutputAirside();
	virtual ~OutputAirside();
	bool LogFlightEvent(AirsideFlightInSim* fltID, const AirsideFlightEventStruct& logItem);
	bool LogFlightEntry(AirsideFlightInSim* fltID, const AirsideFlightDescStruct& logEntryDesc);

	bool LogVehicleEvent(AirsideVehicleInSim* Vehicleid, const AirsideVehicleEventStruct& logItem);
	bool LogVehicleEntry(AirsideVehicleInSim* Vehicleid, const AirsideVehicleDescStruct& logItem);

	bool FlushLog(int nPrjID);
	bool FlushVehicleLog(int nPrjID);
	bool FlushFlightLog(int nPrjID);

	void PrepareLog(CARCportEngine* pEngine, const CString& _csProjPath);
	void SaveLog(const CString& _csProjPath);	

	void Clear();
	//log runway operation log
	int LogRunwayOperationDelay(AirsideRunwayOperationLog::RunwayLogItem* pLogItem);
	int LogFlightDelay(AirsideFlightDelay* delay);
	int LogFlightStair(AirsideFlightStairsLog* pLog);
	int LogFlightDoor(OnboardDoorLog* pLog);
	CAirsideSimLogs m_airsideLog;

	CAirsideEventLogBufferManager * GetLogBuffer()const{ return m_pAirsideEventLogBufferManager; } 


	void PrepareAnimLogs( const CString& _csProjPath);


	AirsideReportLog m_reportLog;

protected:
	int m_nPrjID;
	std::map<AirsideFlightInSim*, AirsideFlightEventList> m_vEventListList;
	std::map<AirsideFlightInSim*, AirsideFlightDescStruct> m_vDescList;
	CAirsideEventLogBufferManager *m_pAirsideEventLogBufferManager; 

	std::map<AirsideVehicleInSim*, AirsideVehicleEventList> m_vVehicleEventListList;
	std::map<AirsideVehicleInSim*, AirsideVehicleDescStruct> m_vVehicleDescList;

};
