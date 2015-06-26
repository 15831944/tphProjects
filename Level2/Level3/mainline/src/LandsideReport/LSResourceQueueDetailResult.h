#pragma once
#include "lsresourcequeueresult.h"

class LANDSIDEREPORT_API LSResourceQueueDetailResult :
	public LSResourceQueueResult
{
public:
	LSResourceQueueDetailResult(void);
	~LSResourceQueueDetailResult(void);

public:
	enum ChartType
	{
		CT_ResourceQueueLen = 0,
	};
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
	virtual void InitListHead( CXListCtrl& cxListCtrl, LandsideBaseParam * parameter, CSortableHeaderCtrl* piSHC /*= NULL*/ );

	//-----------------------------------------------------------------------------------------
	//Summary:
	//		insert result of onboard report into list ctrl
	//Parameter:
	//		cxListCtrl: output parameter and target list ctrl to operate
	//		parameter: input parameter let knows how to fill list ctrl
	//------------------------------------------------------------------------------------------
	virtual void FillListContent( CXListCtrl& cxListCtrl, LandsideBaseParam * parameter );

	virtual LSGraphChartTypeList GetChartList() const;
	CString GetXtitle( ElapsedTime& eStartTime ) const;

private:
	void GenerateChartList(LandsideBaseParam * parameter);

protected:

	LSGraphChartTypeList m_chartList;
};
