#pragma once
#include "airsidemultiplerunresult.h"
class CAirsideAircraftMutiRunRunwayOperationResult :
	public CAirsideMultipleRunResult
{
public:
	CAirsideAircraftMutiRunRunwayOperationResult(void);
	~CAirsideAircraftMutiRunRunwayOperationResult(void);

	virtual void LoadMultipleRunReport(CParameters* pParameter);
	virtual void InitListHead(CXListCtrl& cxListCtrl,CParameters * parameter,  int iType = 0,CSortableHeaderCtrl* piSHC=NULL);

	virtual void FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter, int iType = 0);


	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter, int iType = 0);


};

