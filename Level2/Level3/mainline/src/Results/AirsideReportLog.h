#pragma once

#include ".\ARCBaseLog.h"
#include ".\AirsideVehicleLogItem.h"
#include ".\AirsideFlightLogItem.h"

class CSimAndReportManager;
class  Terminal;
class AirsideReportLog 
{
public:
	typedef ARCBaseLog<AirsideFlightLogItem> AirsideFlightLogData;
	typedef ARCBaseLog<AirsideVehicleLogItem> AirsideVehicleLogData;
	typedef ARCBaseLog<AirsideALTObjectLogItem> AirsideALTObjectLogData;

	AirsideReportLog(void);
	~AirsideReportLog(void);

	CSimAndReportManager* GetSimReportManager();



	void OpenRead(Terminal* pTerminal,const CString& _csProjPath);

	void OpenWrite(Terminal* pTerminal,const CString& _csProjPath);
	void AddFlightLogItem(AirsideFlightLogItem& fltLog);
	void AddVehicleLogItem(AirsideVehicleLogItem& vehLog);
	void AddAltObjectLogItem(AirsideALTObjectLogItem& objLog);
	void CloseSave(const CString& _csProjPath);

	AirsideFlightLogData mFlightLogData;
	AirsideVehicleLogData mVehicleLogData;
	AirsideALTObjectLogData mAltObjectLogData;
protected:
    CSimAndReportManager* m_pSimAndReportManager;
	

};
