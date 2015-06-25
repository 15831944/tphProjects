#pragma once
#include "../Common/elaptime.h"
#include <vector>
#include <map>
#include "../Results/AirsideFlightDelayLog.h"


class AirsideFlightDelay;
class CAirsideFlightDelayLog;


class CAirsideDelayDescLog
{
public:
	CAirsideDelayDescLog(void);
	~CAirsideDelayDescLog(void);
public:
	void LoadData(const CString& strFilePath);
	
	//sort the flight delay data by delay time


	AirsideFlightDelay* GetDelayDesc(int nUniqueID);

protected:
	void AddFlightDelay(AirsideFlightDelay *pDelay);
	void Sort();
protected:
	//int - flight id
	//std::vector<AirsideFlightDelay *> data of one flight
	std::map<int, AirsideFlightDelay * > m_mapLogDesc;

	CString m_strLogFilePath;

	CAirsideFlightDelayLog m_delayLogs;

};
