#pragma once
#include <vector>
#include "../Common/elaptime.h"
#include "../Results/AirsideFlightEventLog.h"
#include "../Results/AirsideFlightLogItem.h"


class CParameters;
class AirsideFlightLogItem;
class AirsideReportLog;
class CBaseFilterFun;
//////////////////////////////////////////////////////////////////////////
//the class for read the flight conflict file log 
//////////////////////////////////////////////////////////////////////////
class AIRSIDEREPORT_API FlightConflictReportData
{


public:
	//_filterFun ,the function for filter log when read the file. if null,it will filter by default function .
	FlightConflictReportData(CParameters * parameter,CBaseFilterFun* _filterFun = NULL);
	~FlightConflictReportData(void);

	class ConflictDataItem
	{
	public:
		ConflictDataItem();
		~ConflictDataItem();

		int GetInIntervalIndex( CParameters* pParameter);

	public:
		AirsideFlightLogDesc fltdesc;
		ElapsedTime m_tTime;
		CString m_strACName;
		int m_nACID;
		CString m_str2ndPartyName;
		int m_n2ndPartyID;
		int m_nConflictType;
		ElapsedTime m_tDelay;
		int m_nAreaID;
		CString m_strAreaName;
		int m_nActionType;
		int m_nOperationType;
		int m_nLocationType;
		double m_ResDis ;
		int m_FlightID ; // the flight unique id 
		int m_IsDep ;
		ResourceDesc m_ResourceDes ;
		int m_nMode;
		AirsideConflictionDelayLog::CDelayReason m_DelayReason ;
		FltDelayReason m_emDelayReson;//for flight delay report
		CString m_sDelayReason;//description flight delay reason for flight delay report
	};

	void LoadData(const CString& _csDescFileName,const CString& _csEventFileName);
	void SortItemsByAC();
	void SortItemsByArea();
	void SortItemsByActionType();
	void SortItemsByLocationType();
	void SortItemsByOperationType();

	void AddConflictItem(ConflictDataItem* pItem);
	void ClearData();

	std::vector<ConflictDataItem*> m_vDataItemList;


protected:
	void AddFlightConflict(const AirsideFlightLogItem& item);
	CParameters* m_pParameter;
	CBaseFilterFun* m_filterFun ;
};
