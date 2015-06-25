#pragma once
#include "airsidemultiplerunresult.h"
class CParameters;
class C2DChartData;

struct StandMultipleOperationData
{
	StandMultipleOperationData()
		:m_lOccupiedTime(0l)
		,m_lDelayEnter(0l)
		,m_lDelayLeaving(0l)
		,m_fltmode('A')
	{

	}

	bool IsConflict()const
	{
		if (!m_sSchedName.IsEmpty() && !m_sActualName.IsEmpty() && m_sSchedName != m_sActualName)
			return true;

		if (m_lDelayEnter > 0l)
			return true;

		return false;
	}

	bool IsDelay()const
	{
		if (m_lDelayEnter > 0l)
			return true;

		if (m_lDelayLeaving > 0l)
			return true;

		return false;
	}

	CString m_sSchedName;
	CString m_sActualName;
	long m_lOccupiedTime;
	long m_lDelayEnter;
	long m_lDelayLeaving;
	char m_fltmode;
};

typedef std::map<CString,std::vector<StandMultipleOperationData>> MapMultiRunStandOperationData;
class AIRSIDEREPORT_API CAirsideStandMultiRunOperatinResult :
	public CAirsideMultipleRunResult
{
public:
	CAirsideStandMultiRunOperatinResult(void);
	~CAirsideStandMultiRunOperatinResult(void);

	virtual void LoadMultipleRunReport(CParameters* pParameter);
	virtual void InitListHead(CXListCtrl& cxListCtrl,CParameters * parameter,  int iType = 0,CSortableHeaderCtrl* piSHC=NULL);
	virtual void FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter, int iType = 0);
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter, int iType = 0);


private:
	void BuildDetailStandOperationOccupancy(CParameters* pParameter,MapMultiRunStandOperationData standOperationData);
	void BuildDetailStandOperationIdel(CParameters* pParameter,MapMultiRunStandOperationData standOperationData);
	void BuildDetailStandOperationPencentage(CParameters* pParameter,MapMultiRunStandOperationData standOperationData);
	void BuildDetailStandOperationDelay(CParameters* pParameter,MultiRunDetailMap& mapDetailData,MapMultiRunStandOperationData standOperationData,char fltMode);
	void BuildDetailStandOperationConflict(CParameters* pParameter,MultiRunDetailMap& mapDetailData,MapMultiRunStandOperationData standOperationData,char fltMode);

	void InitDetailListHead(CXListCtrl& cxListCtrl,MultiRunDetailMap mapDetailData,CSortableHeaderCtrl* piSHC=NULL);
	void FillDetailListTimeContent(CXListCtrl& cxListCtrl,MultiRunDetailMap mapDetailData);
	void FilllDetailDelayConflictContent(CXListCtrl& cxListCtrl,MultiRunDetailMap mapDetailData,int iType);

	void Generate3DChartCountData(MultiRunDetailMap mapDetailData,CARC3DChart& chartWnd, CParameters *pParameter);
	void Generate3DChartConflictData(MultiRunDetailMap mapDetailData,CARC3DChart& chartWnd, CParameters *pParameter,int iType);
	void Generate3DChartDelayData(MultiRunDetailMap mapDetailData,CARC3DChart& chartWnd, CParameters *pParameter,int iType);

	void SetDetail3DChartString( C2DChartData& c2dGraphData,CParameters *pParameter,int iType = 0 );
	void SetDetail3DConflictChartString(C2DChartData& c2dGraphData,CParameters *pParameter,int iType = 0);
	void SetDetail3DDelayChartString(C2DChartData& c2dGraphData, CParameters *pParameter,int iType = 0);
	void SetDetail3DIdleChartString(C2DChartData& c2dGraphData, CParameters *pParameter);
	void SetDetail3DRatioChartString(C2DChartData& c2dGraphData, CParameters *pParameter);
	void SetDetail3DOccupancyChartString(C2DChartData& c2dGraphData, CParameters *pParameter);

	int GetIntervalCount(long iStart, long iEnd, std::vector<long> vData,long iIgnore = 0)const;
	void ClearData();
private:
	MultiRunDetailMap m_standOccupMap;
	MultiRunDetailMap m_standidlemap;
	MultiRunDetailMap m_standRatiomap;
	MultiRunDetailMap m_standArrDelayMap;
	MultiRunDetailMap m_standDepDelayMap;
	MultiRunDetailMap m_standArrConflictsMap;
	MultiRunDetailMap m_standDepConfictsMap;
};

