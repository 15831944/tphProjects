#pragma once
#include "airsidemultiplerunresult.h"
class C2DChartData;

typedef std::map<CString,double> mapTypeDisance;
typedef std::map<CString,mapTypeDisance> mapDetailDistance;
class CAirsideFlightOperationalMultiRunResult : public CAirsideMultipleRunResult
{
public:
	CAirsideFlightOperationalMultiRunResult(void);
	~CAirsideFlightOperationalMultiRunResult(void);

public:
	virtual void LoadMultipleRunReport(CParameters* pParameter);
	virtual void InitListHead(CXListCtrl& cxListCtrl,CParameters * parameter,  int iType = 0,CSortableHeaderCtrl* piSHC=NULL);
	virtual void FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter, int iType = 0);
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter, int iType = 0);

	virtual BOOL WriteReportData( ArctermFile& _file );
	virtual BOOL ReadReportData( ArctermFile& _file );
	virtual CString GetReportFileName()const;
private:
	void ClearData();
	void InitDetailListHead(CXListCtrl& cxListCtrl,mapDetailDistance mapDetailData,int iType = 0,CSortableHeaderCtrl* piSHC=NULL);
	void FillDetailListContent( CXListCtrl& cxListCtrl,mapDetailDistance mapDetailData );
	void Generate3DChartContentData(mapDetailDistance mapDetailData,CARC3DChart& chartWnd, CParameters *pParameter);
	void SetDetail3DChartString(C2DChartData& c2dGraphData,CParameters *pParameter,int iType = 0);
	void SetDetailAirDistance3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter);
	void SetDetailGroundDistance3DChartString(C2DChartData& c2dGraphData, CParameters *pParameter);

	BOOL WriteDetailMap(mapDetailDistance mapDetailData, ArctermFile& _file );
	BOOL ReadDetailMap(mapDetailDistance& mapDetailData,ArctermFile& _file);
private:
	mapDetailDistance m_mapDetailAirDistance;
	mapDetailDistance m_mapDetailGroundDistance;
};

