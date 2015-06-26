#pragma once
#include "airsidemultiplerunresult.h"
#include "AirsideReportNode.h"
#include "MFCExControl\XListCtrl.h"
#include "Parameters.h"
#include "ChartDataDefine.h"
#include "CARC3DChart.h"

#include "AirsideReportNode.h"
#include "MFCExControl\XListCtrl.h"
#include "Parameters.h"

class C2DChartData;

struct RunwayTimeValue
{
	long m_eTime;
	long m_iOperation;
};

struct ClassificationValue
{
	CString m_strLandTrail;
	long m_lFlightCount;
};

typedef std::map<CString,std::vector<ClassificationValue>> mapRunwayMark;
typedef std::map<CString,mapRunwayMark> mapLandTrailOperation;

typedef std::map<CString,std::vector<RunwayTimeValue>> mapRunwayTimeValue;
typedef std::map<CString,mapRunwayTimeValue> mapRunwayTimeOperation;

typedef std::map<CString,mapLoadResult> maRunwayLoadOperation;
typedef std::map<CString,MultiRunDetailMap> mapRunwayDetailOperation;


class RumwayMarkWithLandingTakeOff
{
public:
    RumwayMarkWithLandingTakeOff(){m_mark.m_enumRunwayMark = RUNWAYMARK_FIRST; m_mark.m_nRunwayID = -1;}
    ~RumwayMarkWithLandingTakeOff(){}

    RumwayMarkWithLandingTakeOff& operator=(const RumwayMarkWithLandingTakeOff& other)
    {
        m_mark = other.m_mark;
        m_strLandingTakeoff = other.m_strLandingTakeoff;
        return *this;
    }

    RumwayMarkWithLandingTakeOff& operator=(const RumwayMarkWithLandingTakeOff& other)
    {
        m_mark = other.m_mark;
        m_strLandingTakeoff = other.m_strLandingTakeoff;
        return *this;
    }
	
	CString GetMarkOperation()const
	{
		return m_mark.m_strMarkName + m_strLandingTakeoff;
	}
public:
    CAirsideReportRunwayMark m_mark;
    CString m_strLandingTakeoff;
};

class SummaryRunwayOperationReportItem
{
public:
    SummaryRunwayOperationReportItem()
    {
        m_minCount = 0;
        m_strMinInterval = _T("");
        m_nAverageCount = 0;
        m_maxCount = 0;
        m_strMaxInterval = _T("");
        m_nQ1 = 0;
        m_nQ2 = 0;
        m_nQ3 = 0;
        m_nP1 = 0;
        m_nP5 = 0;
        m_nP10 = 0;
        m_nP90 = 0;
        m_nP95 = 0;
        m_nP99 = 0;
        m_nStdDev = 0;
    }
    ~SummaryRunwayOperationReportItem(){}
	CString GetMarkOperation()const
	{
		return m_mark.m_strMarkName + m_strLandingTakeoff;
	}
public:
    CAirsideReportRunwayMark m_mark;
    CString m_strLandingTakeoff;
};
>>>>>>> .r5324

<<<<<<< .mine
public:
    CAirsideReportRunwayMark m_runWaymark;
    //min
    int m_minCount;
    //the time range that the min operations take place
    CString m_strMinInterval;
    int m_nAverageCount;
    int m_maxCount;
    CString m_strMaxInterval;
=======
class SummaryRunwayOperationReportItem
{
public:
    SummaryRunwayOperationReportItem()
    {
        m_minCount = 0;
        m_strMinInterval = _T("");
        m_nAverageCount = 0;
        m_maxCount = 0;
        m_strMaxInterval = _T("");
        m_nQ1 = 0;
        m_nQ2 = 0;
        m_nQ3 = 0;
        m_nP1 = 0;
        m_nP5 = 0;
        m_nP10 = 0;
        m_nP90 = 0;
        m_nP95 = 0;
        m_nP99 = 0;
        m_nStdDev = 0;
    }
    ~SummaryRunwayOperationReportItem(){}
>>>>>>> .r5324

<<<<<<< .mine
    int m_nQ1;
    int m_nQ2;
    int m_nQ3;
    int m_nP1;
    int m_nP5;
    int m_nP10;
    int m_nP90;
    int m_nP95;
    int m_nP99;
    int m_nStdDev;
};
=======
public:
    CAirsideReportRunwayMark m_runWaymark;
    //min
    int m_minCount;
    //the time range that the min operations take place
    CString m_strMinInterval;
    int m_nAverageCount;
    int m_maxCount;
    CString m_strMaxInterval;
>>>>>>> .r5324

<<<<<<< .mine
typedef std::vector<SummaryRunwayOperationReportItem> vectorSummaryRunwayOpRepItem;
typedef std::map<CString, vectorSummaryRunwayOpRepItem> mapSummaryData;
=======
    int m_nQ1;
    int m_nQ2;
    int m_nQ3;
    int m_nP1;
    int m_nP5;
    int m_nP10;
    int m_nP90;
    int m_nP95;
    int m_nP99;
    int m_nStdDev;
};
>>>>>>> .r5324

<<<<<<< .mine
class CAirsideAircraftMutiRunRunwayOperationResult : public CAirsideMultipleRunResult
{
public:
    CAirsideAircraftMutiRunRunwayOperationResult(void);
    ~CAirsideAircraftMutiRunRunwayOperationResult(void);

public:
    virtual void LoadMultipleRunReport(CParameters* pParameter);
    virtual void InitListHead(CXListCtrl& cxListCtrl,CParameters * parameter,  int iType = 0,CSortableHeaderCtrl* piSHC=NULL);
    virtual void FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter, int iType = 0);
    virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter, int iType = 0);

private:
    std::vector<CString> m_lstColumns;

    void DrawSummary3DChart(CARC3DChart& chartWnd, CParameters *pParameter, int iType = 0);
    void GenerateSummary2DChartData(C2DChartData& c2dGraphData, const mapSummaryData& mapSumData, int iType = 0);
    void SetSummaryMovement3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter);
    void SetSummaryLanding3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter);
    void SetSummaryTakeOff3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter);
    void InitSummaryListHead(CXListCtrl& cxListCtrl, CParameters* pParam, CSortableHeaderCtrl* piSHC=NULL);
    void FillSummaryListContent(CXListCtrl& cxListCtrl, const mapSummaryData& mapSum);
    void ClearData();
protected:
    mapSummaryData m_summaryDataLanding;
    mapSummaryData m_summaryDataTakeOff;
    mapSummaryData m_summaryDataMovement;
typedef std::vector<SummaryRunwayOperationReportItem> vectorSummaryRunwayOpRepItem;
typedef std::map<CString, vectorSummaryRunwayOpRepItem> mapSummaryData;

class CAirsideAircraftMutiRunRunwayOperationResult : public CAirsideMultipleRunResult
{
public:
    CAirsideAircraftMutiRunRunwayOperationResult(void);
    ~CAirsideAircraftMutiRunRunwayOperationResult(void);

public:
    virtual void LoadMultipleRunReport(CParameters* pParameter);
    virtual void InitListHead(CXListCtrl& cxListCtrl,CParameters * parameter,  int iType = 0,CSortableHeaderCtrl* piSHC=NULL);
    virtual void FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter, int iType = 0);
    virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter, int iType = 0);
private:
	void ClearData();
	void InitDetailListHead(CXListCtrl& cxListCtrl,mapRunwayDetailOperation mapDetailData,CSortableHeaderCtrl* piSHC=NULL);
	void InitDetailClassificationHead(CXListCtrl& cxListCtrl,mapLandTrailOperation mapDetailData,CSortableHeaderCtrl* piSHC=NULL);
	void FillDetailListOperationContent( CXListCtrl& cxListCtrl,mapRunwayDetailOperation mapDetailData );
	void FillDetailListLandTrailContent(CXListCtrl& cxListCtrl,mapLandTrailOperation mapDetailData);

    void InitSummaryListHead(CXListCtrl& cxListCtrl, CParameters* pParam, CSortableHeaderCtrl* piSHC=NULL);
    void FillSummaryListContent(CXListCtrl& cxListCtrl, const mapSummaryData& mapSum);

	void SetDetail3DChartString(C2DChartData& c2dGraphData,CParameters *pParameter);
	void SetDetailLanding3DChartString(C2DChartData& c2dGraphData,CParameters *pParameter);
	void SetDetailTakeoff3DChartString(C2DChartData& c2dGraphData,CParameters *pParameter);
	void SetDetailLandTrail3DChartString(C2DChartData& c2dGraphData,CParameters *pParameter);
	void SetDetailMovement3DChartString(C2DChartData& c2dGraphData,CParameters *pParameter);

	void BuildDetailMultipleRunwayOperation(mapRunwayDetailOperation& mapDetailData,maRunwayLoadOperation mapData,CParameters* pParameter);
	void BuildDetailMultipleRunwayTimeValue(mapRunwayDetailOperation& mapDetailData,mapRunwayTimeOperation mapData,CParameters* pParameter);

	long GetIntervalTimeValue(long iStart,long iEnd,std::vector<RunwayTimeValue> vData)const;

	void Generate3DChartCountData(mapRunwayDetailOperation mapDetailData,CARC3DChart& chartWnd, CParameters *pParameter,int iType);
	void Generate3DChartLeadTrailData(mapLandTrailOperation mapDetailData,CARC3DChart& chartWnd, CParameters *pParameter,int iType);

private:
	std::vector<CString> m_lstColumns;
    mapSummaryData m_summaryDataLanding;
    mapSummaryData m_summaryDataTakeOff;
    mapSummaryData m_summaryDataMovement;

	mapRunwayDetailOperation m_mapLandingOperation; //landing by runway
	mapRunwayDetailOperation m_mapTakeoffOperation; //takeoff by runway
	mapLandTrailOperation m_mapLandTrailOperation; //land trail operation
	mapRunwayDetailOperation m_mapMovementInterval; //movement interval
};

