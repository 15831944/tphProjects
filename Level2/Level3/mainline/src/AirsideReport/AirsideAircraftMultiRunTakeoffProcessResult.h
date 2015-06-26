#pragma once
#include "airsidemultiplerunresult.h"
#include "TakeoffProcessData.h"
class ArctermFile;
class C2DChartData;
struct TakeoffProcessMultiRunData
{
	TakeoffProcessMultiRunData()
		:m_lTakeoffQDelay(0l)
		,m_lTimeToPosition(0l)
		,m_lTakeoffDelay(0l)
	{

	}
	long m_lTakeoffQDelay;
	long m_lTimeToPosition;
	long m_lTakeoffDelay;
};

typedef std::map<CString,std::vector<TakeoffProcessMultiRunData>> MapMultiRunTakeoffProcessData;
typedef std::map<CString,std::vector<CTakeoffProcessDetailData>> mapSummaryLoadResult;

class AIRSIDEREPORT_API CAirsideAircraftMultiRunTakeoffProcessResult :
	public CAirsideMultipleRunResult
{
public:
	CAirsideAircraftMultiRunTakeoffProcessResult(void);
	~CAirsideAircraftMultiRunTakeoffProcessResult(void);

	virtual void LoadMultipleRunReport(CParameters* pParameter);
	virtual void InitListHead(CXListCtrl& cxListCtrl,CParameters * parameter,  int iType = 0,CSortableHeaderCtrl* piSHC=NULL);

	virtual void FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter, int iType = 0);


	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter, int iType = 0);
	virtual BOOL WriteReportData( ArctermFile& _file );
	virtual BOOL ReadReportData( ArctermFile& _file );
	virtual CString GetReportFileName()const;
private:
	void BuillDetailMutipleTakeoffProcess(MultiRunDetailMap& mapDetailData,mapLoadResult mapData,long iInterval);
	void InitDetailListHead(CXListCtrl& cxListCtrl,MultiRunDetailMap mapDetailData,CSortableHeaderCtrl* piSHC=NULL);
	void FillDetailListContent( CXListCtrl& cxListCtrl,MultiRunDetailMap mapDetailData );
	void Generate3DChartData(MultiRunDetailMap mapDetailData,CARC3DChart& chartWnd, CParameters *pParameter);
	void SetDetail3DChartString(C2DChartData& c2dGraphData,CParameters *pParameter);
	void SetDetailTakeoffQDelay3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter);
	void SetDetailTakeoffDelay3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter);
	void SetDetailTimeToPosition3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter);

    void BuillSummaryMutipleTakeoffProcess(const mapSummaryLoadResult& middleMap);
    void InitSummaryListHead(CXListCtrl& cxListCtrl, CSortableHeaderCtrl* piSHC=NULL);
    void FillSummaryListContent(CXListCtrl& cxListCtrl,const MultiRunSummaryMap& mapSum);
    void DrawSummary3DChart(CARC3DChart& chartWnd, CParameters *pParameter);
    void GenerateSummary2DChartData(C2DChartData& c2dGraphData, const MultiRunSummaryMap& multiRunSummaryMap);
    void SetSummaryTakeoffQDelay3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter);
    void SetSummaryHoldShortLind3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter);
    void SetSummaryToPositionTime3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter);
    void SetSummaryArrivalPosition3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter);
    void StatisticSummaryData(CString strSim, const CStatisticalTools<double> &tool, MultiRunSummaryMap &mapSum);

	void ClearData();

	BOOL WriteDetailMap(MultiRunDetailMap mapDetailData, ArctermFile& _file );
	BOOL ReadDetailMap(MultiRunDetailMap& mapDetailData,ArctermFile& _file);

	BOOL WriteSummaryMap(MultiRunSummaryMap mapSummaryData,ArctermFile& _file);
	BOOL ReadSummayMap(MultiRunSummaryMap& mapSummaryData,ArctermFile& _file);
private:
	MultiRunDetailMap m_mapTakeoffQDelay;
	MultiRunDetailMap m_mapTimeToPosition;
	MultiRunDetailMap m_mapTakeoffDelay;

    MultiRunSummaryMap m_mapSumTakeoffQueue;
    MultiRunSummaryMap m_mapSumHoldShortLine;
    MultiRunSummaryMap m_mapSumTaxiToPosition;
    MultiRunSummaryMap m_mapSumTimeToPosition;

};

