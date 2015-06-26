#pragma once
#include <vector>
#include <utility>
#include "../common/AIRSIDE_BIN.h"
#include "../Results/AirsideFlightLogEntry.h"
#include "../Results/AirsideVehicleLogEntry.h"
#include "Common/point.h"

class CAirsideSimLogs;
template<class  EVENT_STRUCT>
class CAirsideEventLogBuffer
{
public:
	typedef std::vector<EVENT_STRUCT> VECTOREVENT;
	typedef std::pair<long,VECTOREVENT> ELEMENTEVENT;
	typedef std::vector<ELEMENTEVENT> AIRSIDEEVENTLIST;

public:
	CAirsideEventLogBuffer(){}
	virtual ~CAirsideEventLogBuffer(void){}

	AIRSIDEEVENTLIST m_lstElementEvent;

	size_t getElementCount(){ return m_lstElementEvent.size();} 

	void Clear(){ m_lstElementEvent.clear();}

public:
//	void ReadData();
};


class CAirsideEventLogBufferManager
{
public:
	CAirsideEventLogBufferManager(CAirsideSimLogs *pAirsideLog);
	~CAirsideEventLogBufferManager();


public:	
	CAirsideEventLogBuffer<AirsideFlightEventStruct> m_flightLog;
	CAirsideEventLogBuffer<AirsideVehicleEventStruct> m_vehicleLog;



	void LoadDataIfNecessary();

	void SetFlightLogFilePath(const CString& strPath);
	void SetVehicleLogFilepath(const CString& strPath);

protected:
	void LoadFlightLog();
	void LoadVehicleLog();
	void SmoothFlightMotion(CAirsideEventLogBuffer<AirsideFlightEventStruct>& flightLog);
	//calculate middlepoint which has fDistance form startPoit, before:bBeforeOrAfter = false, after:bBeforeOrAfter = true
	void CalcPointOnLine(AirsideFlightEventStruct& startPoint, AirsideFlightEventStruct& endPoint, Point& middlePoint, long& midPointTime, float fDistance);
	bool IsPointOnAirRoute(AirsideFlightEventStruct& eventStruct);
	void SmoothVehicleMotion(CAirsideEventLogBuffer<AirsideVehicleEventStruct>& vehicleLog);
	void CalcVehiclePointOnLine(AirsideVehicleEventStruct& startPoint, AirsideVehicleEventStruct& endPoint, Point& middlePoint, long& midPointTime, float fDistance);
	bool GenerateSmoothFlightPath(const CAirsideEventLogBuffer<AirsideFlightEventStruct>::VECTOREVENT& vSmoothEvent,CAirsideEventLogBuffer<AirsideFlightEventStruct>::VECTOREVENT& vResult)const;
	double GetSmoothLength(double dSpeed, const CPoint2008& dir1,const CPoint2008& dir2)const;

	CAirsideSimLogs *m_pAirsideLog;

	CString m_strFlightLogPath;
	CString m_strVehicleLogPath;

};