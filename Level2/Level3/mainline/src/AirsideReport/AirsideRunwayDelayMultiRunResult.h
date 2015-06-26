#pragma once
#include "airsidemultiplerunresult.h"
class CParameters;
class C2DChartData;

class AIRSIDEREPORT_API CAirsideRunwayDelayMultiRunResult : public CAirsideMultipleRunResult
{
public:
    CAirsideRunwayDelayMultiRunResult(void);
    ~CAirsideRunwayDelayMultiRunResult(void);

    virtual void LoadMultipleRunReport(CParameters* pParameter);
    virtual void InitListHead(CXListCtrl& cxListCtrl,CParameters * parameter,  int iType = 0,CSortableHeaderCtrl* piSHC=NULL);
    virtual void FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter, int iType = 0);
    virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter, int iType = 0);
};

