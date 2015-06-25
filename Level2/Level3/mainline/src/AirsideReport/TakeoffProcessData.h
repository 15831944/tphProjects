#pragma once
#include <vector>
#include "../Reports/StatisticalTools.h"
#include <algorithm>

class ArctermFile;

class CTakeoffProcessDetailData
{
public:
	CTakeoffProcessDetailData(void);

	CString		m_sFlightID;//flight id
	CString		m_sFlightType;//flight constraint relative
	CString		m_sTakeoffPosition;//take off position name
	long		m_lEnterQueue;//time of enter queue
	CString		m_sDistFromQueue;//enter queue position between taxiway segment
	long		m_lExpectTimeToPosition;//expect speed time duration move to hold short line
	long		m_lActualTimeToPosition;//actual time speed duration move to hold short line
	long		m_lTakeoffQueueDelay;//take off queue delay
	long		m_lCountSameQueue;//count of same queue
	long		m_lTotalCountQueue;//total count of all queue
	long		m_lHoldShortLine;//arrival at hold short line
	long		m_lDelayHoldShortLine;//time at hold short line
	long		m_lToPostionTime;//time speed on move to take off position
	long		m_lArrivalPosition;//arrival at take off position
	long		m_lTimeInPosition;//time speed on take off position
	long		m_lTakeoffTime;//time take off
	long		m_lTakeoffDelay;//take off delay

	//-----------------------------------------------------------------------------------------
	//Summary:
	//		load detail data from file
	//parameter:
	//		_file: input parameter and point to file
	//Return:
	//		TURE: success load
	//		FALSE: failed to load
	//-----------------------------------------------------------------------------------------
	BOOL ReadReportData( ArctermFile& _file );
	//-----------------------------------------------------------------------------------------
	//Summary: 
	//		write parameter and report result into file
	//Parameter:
	//		_file: input parameter and point to file
	//Return:
	//		TURE: success write
	//		FALSE: failed	
	//-----------------------------------------------------------------------------------------
	BOOL WriteReportData( ArctermFile& _file );
};

typedef std::vector<CTakeoffProcessDetailData> TakeoffDetailDataList;

class CTakeoffProcessSummaryData
{
public:
	//-----------------------------------------------------------------------------------------
	//Summary:
	//		load detail data from file
	//parameter:
	//		_file: input parameter and point to file
	//Return:
	//		TURE: success load
	//		FALSE: failed to load
	//-----------------------------------------------------------------------------------------
	BOOL ReadReportData( ArctermFile& _file );

	//-----------------------------------------------------------------------------------------
	//Summary: 
	//		write parameter and report result into file
	//Parameter:
	//		_file: input parameter and point to file
	//Return:
	//		TURE: success write
	//		FALSE: failed	
	//-----------------------------------------------------------------------------------------
	BOOL WriteReportData( ArctermFile& _file );

	CStatisticalTools<double>& GetResultDataByType(int emChartType);

	CString						m_sFlightType;
	TakeoffDetailDataList		detailDataList;
	CStatisticalTools<double>	m_TakeoffQueue;
	CStatisticalTools<double>	m_TimeHoldShortLine;
	CStatisticalTools<double>	m_TaixToPosition;
	CStatisticalTools<double>	m_TimeToPosition;
};

typedef std::vector<CTakeoffProcessSummaryData> TakeoffSummaryDataList;