#pragma once
#include <map>
#include <vector>
#include "AirsideMultipleRunResult.h"
#include "SummaryData.h"
class ElapsedTime;
class CXListCtrl;
class CParameters;
class CSortableHeaderCtrl;
class CARC3DChart;
class C2DChartData;

struct MultipleRunFlightDelayData
{
	MultipleRunFlightDelayData()
		:m_iStart(0)
		,m_iEnd(0)
		,m_iData(0)
	{

	}
	long m_iStart;
	long m_iEnd;
	long m_iData;
};

struct GroupMinMax
{
	long m_iMin;
	long m_iMax;
};

typedef std::map<int,std::vector<MultipleRunFlightDelayData>> ComponentAndSegmentMap;
typedef std::map<CString,ComponentAndSegmentMap> DelayComponentAndSegmentMap;
typedef std::map<CString,std::vector<MultipleRunFlightDelayData>> DelayDetailMap;
typedef std::map<CString,std::vector<long>> mapLoadResult;
typedef std::map<int,std::vector<long>> mapComponentAndSegmentResult;
typedef std::map<CString,mapComponentAndSegmentResult> mapLoadComponentAndSegment;

typedef std::map<CString, CSummaryData> SummaryDelayResultMap;
typedef std::map<int, CSummaryData> SummaryComponentAndSegmentMap;
typedef std::map<CString, SummaryComponentAndSegmentMap> SummaryCompomentAndSegmentResultMap;

class AIRSIDEREPORT_API CAirsideFlightMutiRunDelayResult : public CAirsideMultipleRunResult
{
public:
	CAirsideFlightMutiRunDelayResult(void);
	~CAirsideFlightMutiRunDelayResult(void);

	virtual void LoadMultipleRunReport(long iInterval);
	virtual void InitListHead(CXListCtrl& cxListCtrl,CParameters * parameter,  int iType = 0,CSortableHeaderCtrl* piSHC=NULL);
	virtual void FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter, int iType = 0);
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter, int iType = 0);
private:
	void BuildDetailComponentSegmentData(DelayComponentAndSegmentMap& mapDetailData,mapLoadComponentAndSegment& componentSegmentMapData,long iInterval);
	void BulidDetailMultiRunDelayCount(DelayDetailMap& mapDetailData,mapLoadResult mapData,long iInterval);
	void BulidDetailMultiRunDelayTime( DelayDetailMap& mapDetailData,mapLoadResult mapData,long iInterval );
	void InitDetailListHead(CXListCtrl& cxListCtrl,DelayDetailMap mapDetailData,CSortableHeaderCtrl* piSHC=NULL);
	void InitDetailComponentSegmentListHead(CXListCtrl& cxListCtrl,DelayComponentAndSegmentMap mapDetailData,int iType/*=0*/, CSortableHeaderCtrl* piSHC/*=NULL*/);
	void FillDetailListCountContent(CXListCtrl& cxListCtrl,DelayDetailMap mapDetailData);
	void FillDetailListTimeContent( CXListCtrl& cxListCtrl,DelayDetailMap mapDetailData );
	void FilllDetailComponentSegmentContent(CXListCtrl& cxListCtrl,DelayComponentAndSegmentMap mapDetailData,int iType);
    void InitSummaryListHead(CXListCtrl& cxListCtrl, CSortableHeaderCtrl* piSHC=NULL);
    void FillSummaryListContent(CXListCtrl& cxListCtrl, SummaryDelayResultMap& mapDetailData);
	void FillSummaryComponentSegmentContent(CXListCtrl& cxListCtrl,DelayComponentAndSegmentMap mapDetailData,int iType);
	void ClearData();
	long GetComponentSegmentMaxValue(mapLoadComponentAndSegment& componentSegmentMapData,int iType);
	long GetComponentSegmentMinValue(mapLoadComponentAndSegment& componentSegmentMapData,int iType);
	long GetMapMinValue(mapLoadResult mapData);
	long GetMapMaxValue(mapLoadResult mapData);
	long GetMinValue(std::vector<long> vData)const;
	long GetMaxValue(std::vector<long> vData)const;

	int GetIntervalCount(long iStart, long iEnd, std::vector<long> vData)const;
	long GetIntervalDelayTime(long iStart,long iEnd,std::vector<long> vData)const;

	long ClacTimeRange(ElapsedTime& eMaxTime,ElapsedTime& eMinValue,ElapsedTime& eInterval);

	void GenerateComponentSegmentTimeData(DelayComponentAndSegmentMap mapDetailData,int iType,CARC3DChart& chartWnd, CParameters *pParameter);
	void Generate3DChartCountData(DelayDetailMap mapDetailData,CARC3DChart& chartWnd, CParameters *pParameter);
	void Generate3DChartTimeData(DelayDetailMap mapDetailData,CARC3DChart& chartWnd, CParameters *pParameter);
	//set detail graph string
	void SetDetail3DChartString(C2DChartData& c2dGraphData,CParameters *pParameter,int iType = 0);
	void SetDetailTotalDelay3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter);
	void SetDetailAirDelay3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter);
	void SetDetailTaxiDelay3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter);
	void SetDetailStandDelay3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter);
	void SetDetailTakeoffDelay3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter);
	void SetDetailServiceDelay3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter);
	void SetDetailSegmentDelay3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter,int iType );
	void SetDetailScheduleDelay3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter);
	void SetDetailComponentDelay3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter,int iType );

    void BulidSummaryMultiRunDelayTimeData( SummaryDelayResultMap& mapDetailData,mapLoadResult mapData,long iInterval );
    void GenerateSummary3DChartTimeData(SummaryDelayResultMap& mapSummaryData,CARC3DChart& chartWnd, CParameters *pParameter);

    void BuildSummaryComponentSegmentData(SummaryCompomentAndSegmentResultMap& mapSummaryData, 
        mapLoadComponentAndSegment& componentSegmentMapData,long iInterval);
    void GenerateSummaryComponentSegmentTimeData(SummaryCompomentAndSegmentResultMap& mapDetailData,int iType,
        CARC3DChart& chartWnd, CParameters *pParameter);
    // set summary graph string
    void SetSummary3DChartString(C2DChartData& c2dGraphData,CParameters *pParameter,int iType = 0);
    void SetSummaryTotalDelay3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter);
    void SetSummaryAirDelay3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter);
    void SetSummaryTaxiDelay3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter);
    void SetSummaryStandDelay3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter);
    void SetSummaryTakeoffDelay3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter);
    void SetSummaryServiceDelay3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter);
    void SetSummarySegmentDelay3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter,int iType );
    void SetSummaryScheduleDelay3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter);
    void SetSummaryComponentDelay3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter,int iType );
private:
	//DelayResultPath m_mapResultPath;

	DelayDetailMap m_totalDelayData;
	DelayDetailMap m_airDelayData;
	DelayDetailMap m_taxiDelayData;
	DelayDetailMap m_standDelayData;
	DelayDetailMap m_serviceDelayData;
	DelayDetailMap m_takeoffDelayData;
// 	DelayDetailMap m_segmentDelayData;
// 	DelayDetailMap m_componentDelayData;
	DelayDetailMap m_scheduleDelayData;

	DelayComponentAndSegmentMap m_componentDelayData;
	DelayComponentAndSegmentMap m_segmentDelayData;

    SummaryDelayResultMap m_totalSummaryDelayData;
    SummaryDelayResultMap m_airSummaryDelayData;
    SummaryDelayResultMap m_taxiSummaryDelayData;
    SummaryDelayResultMap m_standSummaryDelayData;
    SummaryDelayResultMap m_serviceSummaryDelayData;
    SummaryDelayResultMap m_takeoffSummaryDelayData;
    SummaryDelayResultMap m_scheduleSummaryDelayData;
    SummaryCompomentAndSegmentResultMap m_segmentSummaryDelayData;
    SummaryCompomentAndSegmentResultMap m_componentSummaryDelayData;
};

