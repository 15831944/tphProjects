#pragma once
#include "lsresourcequeueresult.h"

class LANDSIDEREPORT_API LSResourceQueueSummaryResult :
	public LSResourceQueueResult
{
public:
	class SummaryResQueueItem
	{
	public:
		SummaryResQueueItem()
		{
			m_estMin = 0;
			m_estMax = 0;
			m_estAverage = 0.0;
			m_estQ1 = 0.0;
			m_estQ2 = 0.0;
			m_estQ3 = 0.0;
			m_estP1 = 0.0;
			m_estP5 = 0.0;
			m_estP10 = 0.0;
			m_estP90 = 0.0;
			m_estP95 = 0.0;
			m_estP99 = 0.0;
			m_estSigma = 0.0;
		}
		~SummaryResQueueItem()
		{

		}
	
		CString m_strObject;
		CString m_strVehicleType;
		CStatisticalTools<double> m_SummaryData;
		int   m_estMin;
		int   m_estMax;
		double   m_estAverage;
		double   m_estQ1;
		double   m_estQ2;
		double   m_estQ3;
		double   m_estP1;
		double   m_estP5;
		double   m_estP10;
		double   m_estP90;
		double   m_estP95;
		double   m_estP99;
		double   m_estSigma;	
	};
	LSResourceQueueSummaryResult(void);
	~LSResourceQueueSummaryResult(void);



	virtual void Draw3DChart(CARC3DChart& chartWnd, LandsideBaseParam *pParameter);
	virtual void GenerateResult(CBGetLogFilePath pFunc,LandsideBaseParam* pParameter, InputLandside *pLandisde);
	virtual void RefreshReport(LandsideBaseParam * parameter);

	//-----------------------------------------------------------------------------------------
	//Summary:
	//		load data from file
	//parameter:
	//		_file: input parameter and point to Landside result file
	//Return:
	//		TURE: success load
	//		FALSE: failed to load
	//-----------------------------------------------------------------------------------------
	virtual BOOL ReadReportData( ArctermFile& _file );

	//-----------------------------------------------------------------------------------------
	//Summary: 
	//		write parameter and report result into Landside//Landside.rep
	//Parameter:
	//		_file: input parameter and point to Landside result file
	//Return:
	//		TURE: success write
	//		FALSE: failed	
	//-----------------------------------------------------------------------------------------
	virtual BOOL WriteReportData( ArctermFile& _file );

	//-----------------------------------------------------------------------------------------
	//Summary:
	//		create list control header and set list has sortable
	//Parameter:
	//		cxListCtrl: reference pass, target list ctrl to init
	//		parameter: user input that knows how to create list ctrl header
	//		piSHC: make list ctrl sortable
	//-----------------------------------------------------------------------------------------
	virtual void InitListHead( CListCtrl& cxListCtrl, LandsideBaseParam * parameter, CSortableHeaderCtrl* piSHC /*= NULL*/ );

	//-----------------------------------------------------------------------------------------
	//Summary:
	//		insert result of onboard report into list ctrl
	//Parameter:
	//		cxListCtrl: output parameter and target list ctrl to operate
	//		parameter: input parameter let knows how to fill list ctrl
	//------------------------------------------------------------------------------------------
	virtual void FillListContent( CListCtrl& cxListCtrl, LandsideBaseParam * parameter );

	virtual LSGraphChartTypeList GetChartList() const;
private:
	void GenerateChartList(LandsideBaseParam * parameter);

private:
	std::vector<SummaryResQueueItem> m_ResultData;
	LSGraphChartTypeList m_chartList;

};
