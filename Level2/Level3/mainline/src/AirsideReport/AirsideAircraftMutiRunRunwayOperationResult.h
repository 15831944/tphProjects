#pragma once
#include "airsidemultiplerunresult.h"
#include "AirsideReportNode.h"
#include "MFCExControl\XListCtrl.h"
#include "Parameters.h"

class CAirsideAircraftMutiRunRunwayOperationResult : public CAirsideMultipleRunResult
{
public:
    class StatisticsSummaryItem
    {
    public:
        StatisticsSummaryItem()
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
        ~StatisticsSummaryItem(){}

    public:
        //min
        int m_minCount;
        //the time range that the min operations take place
        CString m_strMinInterval;
        int m_nAverageCount;
        int m_maxCount;
        CString m_strMaxInterval;

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

    //time count
    //9:00  20
    class RunwayOperationTimeCount
    {
    public:
        RunwayOperationTimeCount()
        {
            m_eTime = ElapsedTime(0L);
            m_nOperationCount = 0;
            m_nLandingCount = 0;
            m_nTakeOffCount = 0;
        }
        ~RunwayOperationTimeCount(){}

    public:
        ElapsedTime m_eTime;
        int m_nOperationCount;
        int m_nLandingCount;
        int m_nTakeOffCount;
    };

    // one runways' all time count
    class RunwayOperationStatisticItem
    {
    public:
        RunwayOperationStatisticItem(){}
        ~RunwayOperationStatisticItem(){}
    public:
        void Init(ElapsedTime eStartTim, ElapsedTime eEndTime, ElapsedTime eInterval);

    public:
        CAirsideReportRunwayMark m_runWaymark;
        std::vector<RunwayOperationTimeCount> m_vRunwayStatistic;
        StatisticsSummaryItem m_landingStastic;
        StatisticsSummaryItem m_TakeOffStastic;
        StatisticsSummaryItem m_MovementStastic;
    };

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

    void InitSummaryListHead(CXListCtrl& cxListCtrl, CParameters* pParam, CSortableHeaderCtrl* piSHC=NULL);
    void FillSummaryListContent(CXListCtrl& cxListCtrl,const MultiRunSummaryMap& mapSum);
    void ClearData();
protected:
    std::map<CString, std::vector<RunwayOperationStatisticItem*>> m_summaryTotalData;
};

