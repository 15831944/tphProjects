#include "stdafx.h"
#include "AirsideAircraftMutiRunRunwayOperationResult.h"
#include "Common\TERMFILE.H"
#include "AirsideRunwayOperationReportParam.h"
#include "AirsideRunwayOperationsReport.h"

static const CString str[] = 
{
    _T("Min"),
    _T("Min Interval"),
    _T("Average"),
    _T("Max"),
    _T("Max Interval"),
    _T("Q1"),
    _T("Q2"),
    _T("Q3"),
    _T("P1"),
    _T("P5"),
    _T("P10"),
    _T("P90"),
    _T("P95"),
    _T("P99"),
    _T("Std Dev")
};

CAirsideAircraftMutiRunRunwayOperationResult::CAirsideAircraftMutiRunRunwayOperationResult(void)
{
}


CAirsideAircraftMutiRunRunwayOperationResult::~CAirsideAircraftMutiRunRunwayOperationResult(void)
{
}

void CAirsideAircraftMutiRunRunwayOperationResult::LoadMultipleRunReport(CParameters* pParameter)
{
    ClearData();

    ArctermFile _file;
    DelayResultPath::iterator iter = m_mapResultPath.begin();
    for (; iter != m_mapResultPath.end(); ++iter)
    {
        CString strResultPath = iter->second;
        CString strSimResult = iter->first;
        try
        {
            if (_file.openFile(strResultPath.GetString(),READ))
            {
                _file.getLine(); // 'Column'
                int iReportType = 0;
                _file.getInteger(iReportType);
                _file.getLine();
                if (iReportType == ASReportType_Detail)
                {
                }
                else if(iReportType == ASReportType_Summary)
                {
                    //columns
                    int nColCount = 0;
                    _file.getInteger(nColCount);
                    for (int nCol = 0; nCol < nColCount; ++nCol)
                    {
                        CString strCol;
                        _file.getField(strCol.GetBuffer(1024),1024);
                        strCol.ReleaseBuffer();
                        m_lstColumns.push_back(strCol);
                    }

                    _file.getLine();
                    int nRunwwayCount = 0;
                    _file.skipField(1);//skip runway count
                    _file.getInteger(nRunwwayCount);
                    _file.getLine();
                    for (int nRunway = 0; nRunway < nRunwwayCount; ++ nRunway)
                    {
                        RunwayOperationStatisticItem* pRunwayStatistic = new RunwayOperationStatisticItem;
                        CAirsideReportRunwayMark runWaymark;
                        //runway information
                        _file.getInteger(runWaymark.m_nRunwayID);
                        _file.getInteger((int&)runWaymark.m_enumRunwayMark);
                        _file.getField(runWaymark.m_strMarkName.GetBuffer(1024),1024);
                        runWaymark.m_strMarkName.ReleaseBuffer();
                        pRunwayStatistic->m_runWaymark = runWaymark;

                        //landing
                        {
                            StatisticsSummaryItem& landingStastic = pRunwayStatistic->m_landingStastic;
                            _file.getInteger(landingStastic.m_minCount);
                            _file.getField(landingStastic.m_strMinInterval.GetBuffer(1024),1024);
                            landingStastic.m_strMinInterval.ReleaseBuffer();
                            _file.getInteger(landingStastic.m_nAverageCount);
                            _file.getInteger(landingStastic.m_maxCount);
                            _file.getField(landingStastic.m_strMaxInterval.GetBuffer(1024),1024);
                            landingStastic.m_strMaxInterval.ReleaseBuffer();
                            _file.getInteger(landingStastic.m_nQ1);
                            _file.getInteger(landingStastic.m_nQ2);
                            _file.getInteger(landingStastic.m_nQ3);
                            _file.getInteger(landingStastic.m_nP1);
                            _file.getInteger(landingStastic.m_nP5);
                            _file.getInteger(landingStastic.m_nP10);
                            _file.getInteger(landingStastic.m_nP90);
                            _file.getInteger(landingStastic.m_nP95);
                            _file.getInteger(landingStastic.m_nP99);
                            _file.getInteger(landingStastic.m_nStdDev);
                        }

                        //take off 
                        {
                            StatisticsSummaryItem& takeoffStastic = pRunwayStatistic->m_TakeOffStastic;
                            _file.getInteger(takeoffStastic.m_minCount);
                            _file.getField(takeoffStastic.m_strMinInterval.GetBuffer(1024),1024);
                            takeoffStastic.m_strMinInterval.ReleaseBuffer();
                            _file.getInteger(takeoffStastic.m_nAverageCount);
                            _file.getInteger(takeoffStastic.m_maxCount);
                            _file.getField(takeoffStastic.m_strMaxInterval.GetBuffer(1024),1024);
                            takeoffStastic.m_strMaxInterval.ReleaseBuffer();
                            _file.getInteger(takeoffStastic.m_nQ1);
                            _file.getInteger(takeoffStastic.m_nQ2);
                            _file.getInteger(takeoffStastic.m_nQ3);
                            _file.getInteger(takeoffStastic.m_nP1);
                            _file.getInteger(takeoffStastic.m_nP5);
                            _file.getInteger(takeoffStastic.m_nP10);
                            _file.getInteger(takeoffStastic.m_nP90);
                            _file.getInteger(takeoffStastic.m_nP95);
                            _file.getInteger(takeoffStastic.m_nP99);
                            _file.getInteger(takeoffStastic.m_nStdDev);
                        }

                        //movements
                        {
                            StatisticsSummaryItem& movementStastic = pRunwayStatistic->m_MovementStastic;
                            _file.getInteger(movementStastic.m_minCount);
                            _file.getField(movementStastic.m_strMinInterval.GetBuffer(1024),1024);
                            movementStastic.m_strMinInterval.ReleaseBuffer();
                            _file.getInteger(movementStastic.m_nAverageCount);
                            _file.getInteger(movementStastic.m_maxCount);
                            _file.getField(movementStastic.m_strMaxInterval.GetBuffer(1024),1024);
                            movementStastic.m_strMaxInterval.ReleaseBuffer();
                            _file.getInteger(movementStastic.m_nQ1);
                            _file.getInteger(movementStastic.m_nQ2);
                            _file.getInteger(movementStastic.m_nQ3);
                            _file.getInteger(movementStastic.m_nP1);
                            _file.getInteger(movementStastic.m_nP5);
                            _file.getInteger(movementStastic.m_nP10);
                            _file.getInteger(movementStastic.m_nP90);
                            _file.getInteger(movementStastic.m_nP95);
                            _file.getInteger(movementStastic.m_nP99);
                            _file.getInteger(movementStastic.m_nStdDev);
                        }

                        m_summaryTotalData[strSimResult].push_back(pRunwayStatistic);
                        _file.getLine();
                    }
                }
                _file.closeIn();
            }
        }
        catch(...)
        {
            ClearData();
        }
    }
}

void CAirsideAircraftMutiRunRunwayOperationResult::InitListHead(CXListCtrl& cxListCtrl, CParameters* parameter, int iType, CSortableHeaderCtrl* piSHC)
{
    cxListCtrl.DeleteAllItems();

    while(cxListCtrl.GetHeaderCtrl()->GetItemCount() >0)
        cxListCtrl.DeleteColumn(0);

    if (parameter->getReportType() == ASReportType_Detail)
    {
    }
    else if(parameter->getReportType() == ASReportType_Summary)
    {
        InitSummaryListHead(cxListCtrl, parameter, piSHC);
    }
}

void CAirsideAircraftMutiRunRunwayOperationResult::FillListContent( CXListCtrl& cxListCtrl, CParameters * parameter, int iType /*= 0*/ )
{
    ASSERT(parameter);
    if (parameter->getReportType() == ASReportType_Detail)
    {
    }
    else if(parameter->getReportType() == ASReportType_Summary)
    {

    }
}

void CAirsideAircraftMutiRunRunwayOperationResult::Draw3DChart( CARC3DChart& chartWnd, CParameters *pParameter, int iType /*= 0*/ )
{

}

void CAirsideAircraftMutiRunRunwayOperationResult::ClearData()
{
    m_summaryTotalData.clear();
}

void CAirsideAircraftMutiRunRunwayOperationResult::InitSummaryListHead(CXListCtrl& cxListCtrl, CParameters* pParam, CSortableHeaderCtrl* piSHC)
{
    int nCurCol = 0;
    DWORD headStyle = LVCFMT_CENTER;
    headStyle &= ~HDF_OWNERDRAW;
    cxListCtrl.InsertColumn(nCurCol,"",headStyle,20);
    nCurCol++;

    headStyle = LVCFMT_LEFT;
    headStyle &= ~HDF_OWNERDRAW;
    cxListCtrl.InsertColumn(nCurCol, _T("SimResult"), headStyle, 80);
    if(piSHC)
    {
        piSHC->SetDataType(nCurCol, dtSTRING);
    }
    nCurCol++;

    cxListCtrl.InsertColumn(nCurCol, _T("Runway"), headStyle, 80);
    if(piSHC)
    {
        piSHC->SetDataType(nCurCol, dtSTRING);
    }
    nCurCol++;

    AirsideRunwayOperationReportParam* pRunWayOpParam = (AirsideRunwayOperationReportParam*)pParam;
    int nSubType = pRunWayOpParam->getSubType();
    switch(nSubType)
    {
    case AirsideRunwayOperationsReport::ChartType_Summary_RunwayOperationalStatistic_Operations:
        for(int i=0; i<15; i++)
        {
            cxListCtrl.InsertColumn(nCurCol, str[i]+_T("(TO)"), LVCFMT_LEFT, 100);
            if(piSHC)
            {
                piSHC->SetDataType(nCurCol,dtINT);
            }
            nCurCol++;
        }
        break;
    case AirsideRunwayOperationsReport::ChartType_Summary_RunwayOperationalStatistic_Landing:
        for(int i=0; i<15; i++)
        {
            cxListCtrl.InsertColumn(nCurCol, str[i]+_T("(Landing)"), LVCFMT_LEFT, 100);
            if(piSHC)
            {
                piSHC->SetDataType(nCurCol,dtINT);
            }
            nCurCol++;
        }
        break;
    case AirsideRunwayOperationsReport::ChartType_Summary_RunwayOperationalStatistic_TakeOff:
        for(int i=0; i<15; i++)
        {
            cxListCtrl.InsertColumn(nCurCol, str[i]+_T("(Movements)"), LVCFMT_LEFT, 100);
            if(piSHC)
            {
                piSHC->SetDataType(nCurCol,dtINT);
            }
            nCurCol++;
        }
        break;
    default:
        break;
    }
}

void CAirsideAircraftMutiRunRunwayOperationResult::FillSummaryListContent(CXListCtrl& cxListCtrl,const MultiRunSummaryMap& mapSum)
{

}

