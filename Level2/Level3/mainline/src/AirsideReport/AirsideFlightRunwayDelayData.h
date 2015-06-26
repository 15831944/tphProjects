#pragma once
#include <utility>
#include <vector>

#include "../Common/elaptime.h"
#include "../Results/AirsideFlightEventLog.h"

class AirsideFlightRunwayDelayReportPara;
class AirsideFlightLogItem;
class AirsideReportLog;
class CDefaultFilterFun;
class CParameters;

class AIRSIDEREPORT_API AirsideFlightRunwayDelayData
{

public:
	AirsideFlightRunwayDelayData(AirsideFlightRunwayDelayReportPara * parameter);
	~AirsideFlightRunwayDelayData(void);

	typedef std::pair<int,CString> DETAILINFO;

	class RunwayDelayItem
	{
	public:
		RunwayDelayItem();
		~RunwayDelayItem();

		int GetDelayInIntervalIndex(long lInterval);
		int GetTimeInIntervalIndex(CParameters* pParameter);
	public:
		int m_nACID;
		int m_nRwyID;
		int m_nRwyMark;
		CString m_strRunwayMark;
		CString m_strACName;
		CString m_sACType;
		AirsideMobileElementMode mMode;
		ElapsedTime m_tStartTime;
		int m_OperationType;
		int m_PositionType;
		long m_lExpectOpTime;
		long m_lRealityOpTime;
		long m_lTotalDelay;

		std::vector<DETAILINFO> m_vReasonDetail;
	};

	void LoadData(const CString& _csDescFileName,const CString& _csEventFileName);
	void ClearData();

    const std::vector<RunwayDelayItem*>& GetDataItemList() { return m_vDataItemList; }

	std::vector<RunwayDelayItem*> m_vDataItemList;

protected:
	void AddRunwayDelay(const AirsideFlightLogItem& item);
	AirsideFlightRunwayDelayReportPara* m_pParameter;
	CDefaultFilterFun* m_filterFun ;

};
