#pragma once

#include "LandsideReportAPI.h"
#include "GraphChartType.h"
#include "../Common/elaptime.h"
#include "../Reports/StatisticalTools.h"

class CARC3DChart;
class CXListCtrl;
class CSortableHeaderCtrl;
class ArctermFile;
class LandsideBaseParam;
class InputLandside;



class LANDSIDEREPORT_API LandsideBaseResult
{
public:
	LandsideBaseResult(void);
	virtual ~LandsideBaseResult(void);


	virtual void Draw3DChart(CARC3DChart& chartWnd, LandsideBaseParam *pParameter) = 0;
	virtual void GenerateResult(CBGetLogFilePath pFunc, LandsideBaseParam* pParameter, InputLandside *pLandisde) = 0;
	virtual void RefreshReport(LandsideBaseParam * parameter) = 0;
	//-----------------------------------------------------------------------------------------
	//Summary:
	//		create list control header and set list has sortable
	//Parameter:
	//		cxListCtrl: reference pass, target list ctrl to init
	//		parameter: user input that knows how to create list ctrl header
	//		piSHC: make list ctrl sortable
	//-----------------------------------------------------------------------------------------
	virtual void InitListHead( CXListCtrl& cxListCtrl, LandsideBaseParam * parameter, CSortableHeaderCtrl* piSHC /*= NULL*/ ) = 0;

	//-----------------------------------------------------------------------------------------
	//Summary:
	//		insert result of Landside report into list ctrl
	//Parameter:
	//		cxListCtrl: output parameter and target list ctrl to operate
	//		parameter: input parameter let knows how to fill list ctrl
	//------------------------------------------------------------------------------------------
	virtual void FillListContent( CXListCtrl& cxListCtrl, LandsideBaseParam * parameter ) = 0;

	//-----------------------------------------------------------------------------------------
	//Summary:
	//		load data from file
	//parameter:
	//		_file: input parameter and point to Landside result file
	//Return:
	//		TURE: success load
	//		FALSE: failed to load
	//-----------------------------------------------------------------------------------------
	virtual BOOL ReadReportData( ArctermFile& _file ) = 0;

	//-----------------------------------------------------------------------------------------
	//Summary: 
	//		write parameter and report result into Landside//Landside.rep
	//Parameter:
	//		_file: input parameter and point to Landside result file
	//Return:
	//		TURE: success write
	//		FALSE: failed	
	//-----------------------------------------------------------------------------------------
	virtual BOOL WriteReportData( ArctermFile& _file ) = 0;

	virtual LSGraphChartTypeList GetChartList() const;
};

class LandsideSummaryTime
{
public:
	LandsideSummaryTime(void);
	~LandsideSummaryTime(void);

public:
	ElapsedTime   m_estTotal;
	ElapsedTime   m_estMin;
	ElapsedTime   m_estAverage;
	ElapsedTime   m_estMax;
	ElapsedTime   m_estQ1;
	ElapsedTime   m_estQ2;
	ElapsedTime   m_estQ3;
	ElapsedTime   m_estP1;
	ElapsedTime   m_estP5;
	ElapsedTime   m_estP10;
	ElapsedTime   m_estP90;
	ElapsedTime   m_estP95;
	ElapsedTime   m_estP99;
	ElapsedTime   m_estSigma;	
	CStatisticalTools<double> statisticalTool;
};

