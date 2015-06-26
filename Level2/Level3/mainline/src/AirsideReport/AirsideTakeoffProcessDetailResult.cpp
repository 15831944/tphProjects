#include "StdAfx.h"
#include ".\airsidetakeoffprocessdetailresult.h"
#include "../Common/termfile.h"
#include "CARC3DChart.h"
#include "../MFCExControl/XListCtrl.h"
#include <limits>
#include "Parameters.h"
#include "AirsideTakeoffProcessParameter.h"

CAirsideTakeoffProcessDetailResult::CAirsideTakeoffProcessDetailResult(void)
:m_pChartResult(NULL)
{
}

CAirsideTakeoffProcessDetailResult::~CAirsideTakeoffProcessDetailResult(void)
{
	if (m_pChartResult)
	{
		delete m_pChartResult;
		m_pChartResult = NULL;
	}
}

void CAirsideTakeoffProcessDetailResult::GenerateResult( CBGetLogFilePath pCBGetLogFilePath,CParameters *pParameter )
{
	CAirsideTakeoffProcessResult::GenerateResult(pCBGetLogFilePath,pParameter);
}

void CAirsideTakeoffProcessDetailResult::Draw3DChart( CARC3DChart& chartWnd, CParameters *pParameter )
{
	if (m_pChartResult)
	{
		m_pChartResult->Draw3DChart(chartWnd,pParameter);
	}
}

void CAirsideTakeoffProcessDetailResult::RefreshReport( CParameters * parameter )
{
	CAirsideTakeoffProcessParameter* pParam = (CAirsideTakeoffProcessParameter*)parameter;
	ASSERT(pParam != NULL);

	if (NULL != m_pChartResult)
	{
		delete m_pChartResult;
		m_pChartResult = NULL;
	}

	switch(pParam->getSubType())
	{
	case TakeoffQueueDelay:
		{
			m_pChartResult = new CAirsideTakeoffQueueChart;
		}
		break;
	case TimeToPosition:
		{
			m_pChartResult = new CAirsideTimeToPositionChart;
		}
		break;
	case TakeOffDelay:
		{
			m_pChartResult = new CAirsideTakeoffDelayChart;
		}
		break;
	default:
		ASSERT(0);
		break;
	}

	if (m_pChartResult)
	{
		m_pChartResult->GenerateResult(m_vResult, parameter);
	}
}	

void CAirsideTakeoffProcessDetailResult::InitListHead( CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType,CSortableHeaderCtrl* piSHC)
{
	cxListCtrl.DeleteAllItems();

	while(cxListCtrl.GetHeaderCtrl()->GetItemCount() >0)
		cxListCtrl.DeleteColumn(0);

	cxListCtrl.InsertColumn(0, _T("Flight ID"), LVCFMT_LEFT, 80);	

	cxListCtrl.InsertColumn(1, _T("Takeoff Position"), LVCFMT_LEFT, 100);
	cxListCtrl.InsertColumn(2, _T("Date Enter Q"), LVCFMT_LEFT, 100);
	cxListCtrl.InsertColumn(3, _T("Time Enter Q(hh:mm:ss)"),LVCFMT_LEFT, 100);

	cxListCtrl.InsertColumn(4, _T("Position Enter Q"),LVCFMT_LEFT, 100);
	cxListCtrl.InsertColumn(5, _T("Uninterrupted time to Hold Short(mm:ss)"),LVCFMT_LEFT, 100);
	cxListCtrl.InsertColumn(6, _T("Actual time to Hold short(mm:ss)"),LVCFMT_LEFT, 100);

	cxListCtrl.InsertColumn(7, _T("Take off Q delay(hh:mm:ss)"),LVCFMT_LEFT, 100);
	cxListCtrl.InsertColumn(8, _T("aircraft in Q same Take off Position"),LVCFMT_LEFT, 100);
	cxListCtrl.InsertColumn(9, _T("aircraft in Q all take off positions"),LVCFMT_LEFT, 100);
	cxListCtrl.InsertColumn(10, _T("Time at hold short line(mm:ss)"),LVCFMT_LEFT, 100);
	cxListCtrl.InsertColumn(11, _T("Time taxi to position(mm:ss)"),LVCFMT_LEFT, 100);
	cxListCtrl.InsertColumn(12, _T("Time in position(mm:ss)"),LVCFMT_LEFT, 100);

	cxListCtrl.InsertColumn(13, _T("Take off Date"),LVCFMT_LEFT, 100);
	cxListCtrl.InsertColumn(14, _T("Take off time(hh:mm:ss)"),LVCFMT_LEFT, 100);
	cxListCtrl.InsertColumn(15, _T("Take off delay(mm:ss)"),LVCFMT_LEFT, 100);
}

void CAirsideTakeoffProcessDetailResult::FillListContent( CXListCtrl& cxListCtrl, CParameters * parameter )
{
	cxListCtrl.DeleteAllItems();

	for (int i=0; i<(int)m_vResult.size(); i++)
	{
		CTakeoffProcessDetailData& detailData = m_vResult[i];

		//Flight ID
		{
			cxListCtrl.InsertItem(i, detailData.m_sFlightID);
		}
		//Takeoff Position
		{
			cxListCtrl.SetItemText(i,1,detailData.m_sTakeoffPosition);
		}
		//Date Enter Q
		{
			CString strDate;
			ElapsedTime enterTime(long(detailData.m_lEnterQueue/100+0.5));
			strDate.Format(_T("Day%d"),enterTime.GetDay());
			cxListCtrl.SetItemText(i,2,strDate);
		}
		//Time Enter Q
		{
			CString strEnterTime;
			ElapsedTime enterTime(long(detailData.m_lEnterQueue/100+0.5));
			strEnterTime.Format(_T("%02d:%02d:%02d"),enterTime.GetHour(),enterTime.GetMinute(),enterTime.GetSecond());
			cxListCtrl.SetItemText(i,3,strEnterTime);
		}
		//Position Enter Q
		{
			cxListCtrl.SetItemText(i,4,detailData.m_sDistFromQueue);
		}
		//Uninterrupted time to Hold Short
		{
			CString strTime;
			ElapsedTime estTime;
			estTime.setPrecisely(detailData.m_lExpectTimeToPosition);
			strTime.Format(_T("%02d:%02d"),estTime.GetMinute(),estTime.GetSecond());
			cxListCtrl.SetItemText(i,5,strTime);
		}
		//Actual time to Hold short
		{
			CString strTime;
			ElapsedTime estTime;
			estTime.setPrecisely(detailData.m_lActualTimeToPosition);
			strTime.Format(_T("%02d:%02d"),estTime.GetMinute(),estTime.GetSecond());
			cxListCtrl.SetItemText(i,6,strTime);
		}
		//Take off Q delay
		{
			CString  strTime;
			ElapsedTime estTime;
			estTime.setPrecisely(detailData.m_lTakeoffQueueDelay);
			strTime.Format(_T("%02d:%02d:%02d"),estTime.GetHour(),estTime.GetMinute(),estTime.GetSecond());
			cxListCtrl.SetItemText(i,7,strTime);
		}
		//aircraft in Q same Take off Position
		{
			CString strCount;
			strCount.Format(_T("%d"),detailData.m_lCountSameQueue);
			cxListCtrl.SetItemText(i,8,strCount);
		}
		//aircraft in Q all take off positions
		{
			CString strCount;
			strCount.Format(_T("%d"),detailData.m_lTotalCountQueue);
			cxListCtrl.SetItemText(i,9,strCount);
		}
		//Time at hold short line
		{
			CString  strTime;
			ElapsedTime estTime;
			estTime.setPrecisely(detailData.m_lDelayHoldShortLine);
			strTime.Format(_T("%02d:%02d"),estTime.GetMinute(),estTime.GetSecond());
			cxListCtrl.SetItemText(i,10,strTime);
		}
		//Time taxi to position
		{
			CString  strTime;
			ElapsedTime estTime;
			estTime.setPrecisely(detailData.m_lToPostionTime);
			strTime.Format(_T("%02d:%02d"),estTime.GetMinute(),estTime.GetSecond());
			cxListCtrl.SetItemText(i,11,strTime);
		}
		//Time in position
		{
			CString  strTime;
			ElapsedTime estTime;
			estTime.setPrecisely(detailData.m_lTimeInPosition);
			strTime.Format(_T("%02d:%02d"),estTime.GetMinute(),estTime.GetSecond());
			cxListCtrl.SetItemText(i,12,strTime);
		}
		//Take off Date
		{
			CString  strTime;
			ElapsedTime estTime;
			estTime.setPrecisely(detailData.m_lTakeoffDelay);
			strTime.Format(_T("Day%d"),estTime.GetDay());
			cxListCtrl.SetItemText(i,13,strTime);
		}
		//Take off time
		{
			CString  strTime;
			ElapsedTime estTime;
			estTime.setPrecisely(detailData.m_lTakeoffTime);
			strTime.Format(_T("%02d:%02d:%02d"),estTime.GetHour(),estTime.GetMinute(),estTime.GetSecond());
			cxListCtrl.SetItemText(i,14,strTime);
		}
		//Take off delay
		{
			CString  strTime;
			ElapsedTime estTime;
			estTime.setPrecisely(detailData.m_lTakeoffDelay);
			strTime.Format(_T("%02d:%02d"),estTime.GetMinute(),estTime.GetSecond());
			cxListCtrl.SetItemText(i,15,strTime);
		}
	}
}

BOOL CAirsideTakeoffProcessDetailResult::ExportReportData( ArctermFile& _file,CString& Errmsg )
{
	return WriteReportData(_file);
}

BOOL CAirsideTakeoffProcessDetailResult::ImportReportData( ArctermFile& _file,CString& Errmsg )
{
	return ReadReportData(_file);
}

BOOL CAirsideTakeoffProcessDetailResult::ExportListData( ArctermFile& _file,CParameters * parameter )
{
	return TRUE;
}

BOOL CAirsideTakeoffProcessDetailResult::WriteReportData( ArctermFile& _file )
{
	_file.writeField("Airside Detail Takeoff Process Report");
	_file.writeLine();

	_file.writeInt(ASReportType_Detail);
	_file.writeLine();

	int size = (int)m_vResult.size() ;
	_file.writeInt(size) ;
	_file.writeLine() ;
	for (int i = 0 ; i < size ; i++)
	{
		m_vResult[i].WriteReportData(_file);
		_file.writeLine();
	}
	return TRUE ;
}

BOOL CAirsideTakeoffProcessDetailResult::ReadReportData( ArctermFile& _file )
{
	int size = 0 ;
	if(!_file.getInteger(size))
		return FALSE;
	for (int i = 0 ; i < size ;i++)
	{
		_file.getLine() ;
		CTakeoffProcessDetailData  acitem ;
		acitem.ReadReportData(_file) ;
		m_vResult.push_back(acitem) ;
	}
	return TRUE ;
}

CAirsideTakeoffProcessDetailChart::CAirsideTakeoffProcessDetailChart()
{

}

CAirsideTakeoffProcessDetailChart::~CAirsideTakeoffProcessDetailChart()
{

}

void CAirsideTakeoffProcessDetailChart::GenerateResult( TakeoffDetailDataList& vResult,CParameters *pParameter )
{
	ASSERT(pParameter);

	long lMin = GetMinData(vResult)/100;
	long lMax = GetMaxData(vResult)/100;
    
    long lUserInterval = pParameter->getInterval();
    long lStart = lMin - lMin%pParameter->getInterval();

    int intervalCount = (lMax - lStart) / lUserInterval;
    if((lMax-lStart)%lUserInterval != 0)
        intervalCount += 1;
    if(intervalCount == 0)
        intervalCount = 1;

    CString strIntervalTime;
    ElapsedTime estStartTime = lStart;
    ElapsedTime estEndTime = lMax;
    for (int i = 0 ; i < intervalCount ;i++)
    {
        ElapsedTime estIntervalStartTime, estIntervalEndTime;
        estIntervalStartTime = lStart + lUserInterval*i;
        estIntervalEndTime = lStart + lUserInterval*(i+1);
        strIntervalTime.Format(_T("%s-%s"),estIntervalStartTime.printTime(),estIntervalEndTime.printTime()) ;
        m_vXAxisTitle.push_back(strIntervalTime);
    }

	ElapsedTime estMinDelayTime = estStartTime;
	detailChartDataList chartDataList;
	InvertDetailToGraphData(vResult,chartDataList);

	for (long j=0; j<(long)m_vXAxisTitle.size(); j++)
	{
		int nCount = 0;
		ElapsedTime estIntervalStartTime = estMinDelayTime + ElapsedTime(lUserInterval*j);
		ElapsedTime estIntervalEndTime = estMinDelayTime + ElapsedTime(lUserInterval*(j + 1));
		for (int n = 0; n < (int)chartDataList.size(); n++)
		{
			DetailChartData& chartData = chartDataList[n];
			for (int m = 0; m < (int)chartData.m_vData.size(); m++)
			{
				double dData = chartData.m_vData[m];
				if (dData >= estIntervalStartTime.asSeconds()*100\
					&&dData < estIntervalEndTime.asSeconds()*100)
				{
					nCount++;
				}
			}	
			AddChartData(m_vData,chartData.m_sName,nCount);
		}
	}
}

void CAirsideTakeoffProcessDetailChart::AddChartData( detailChartDataList& detailData,const CString& sName,double dData )
{
	if (detailData.empty())
	{
		DetailChartData newChartData;
		newChartData.m_sName = sName;
		newChartData.m_vData.push_back(dData);
		detailData.push_back(newChartData);
	}
	else
	{
		DetailChartData& charData = detailData.front();
		charData.m_vData.push_back(dData);
	}
}

CAirsideTakeoffQueueChart::CAirsideTakeoffQueueChart()
{

}

CAirsideTakeoffQueueChart::~CAirsideTakeoffQueueChart()
{

}

void CAirsideTakeoffQueueChart::Draw3DChart( CARC3DChart& chartWnd, CParameters *pParameter )
{
	CString strLabel = _T("");
	C2DChartData c2dGraphData;
	c2dGraphData.m_strChartTitle = _T("Take off Q delay(Detailed)");
	c2dGraphData.m_strYtitle = _T("Number of flights ");
	c2dGraphData.m_strXtitle = _T("Time(HH:MM:SS)");	
	c2dGraphData.m_vrXTickTitle = m_vXAxisTitle;

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("Airside Take off Q delay Report(Detailed) %s "),pParameter->GetParameterString());
	c2dGraphData.m_strFooter = strFooter;

	detailChartDataList::iterator iter = m_vData.begin();
	for (; iter!=m_vData.end(); ++iter)
	{
		c2dGraphData.m_vr2DChartData.push_back((*iter).m_vData);
	}
	chartWnd.DrawChart(c2dGraphData);
}

void CAirsideTakeoffQueueChart::InvertDetailToGraphData( TakeoffDetailDataList& vResult,detailChartDataList& detailData )
{
	size_t nSize = vResult.size();
	for (size_t i = 0; i < nSize; i++)
	{
		CTakeoffProcessDetailData& resultData = vResult[i];
		AddChartData(detailData,resultData.m_sFlightType,resultData.m_lTakeoffQueueDelay);
	}
}

long CAirsideTakeoffQueueChart::GetMinData( TakeoffDetailDataList& dataList )
{
	if(dataList.size() == 0)
		return 0;


	long lMin = (std::numeric_limits<long>::max)();
	for (size_t i = 0; i < dataList.size(); i++)
	{
		CTakeoffProcessDetailData& data = dataList[i];
		if (data.m_lTakeoffQueueDelay < lMin)
		{
			lMin = data.m_lTakeoffQueueDelay;
		}
	}
	return lMin;
}

long CAirsideTakeoffQueueChart::GetMaxData( TakeoffDetailDataList& dataList )
{
	if(dataList.size() == 0)
		return 0;

	long lMax =(std::numeric_limits<long>::min)();
	for (size_t i = 0; i < dataList.size(); i++)
	{
		CTakeoffProcessDetailData& data = dataList[i];
		if (data.m_lTakeoffQueueDelay > lMax)
		{
			lMax = data.m_lTakeoffQueueDelay;
		}
	}
	return lMax;
}

CAirsideTimeToPositionChart::CAirsideTimeToPositionChart()
{

}

CAirsideTimeToPositionChart::~CAirsideTimeToPositionChart()
{

}

void CAirsideTimeToPositionChart::Draw3DChart( CARC3DChart& chartWnd, CParameters *pParameter )
{
	CString strLabel = _T("");
	C2DChartData c2dGraphData;
	c2dGraphData.m_strChartTitle = _T("Time to position(Detailed)");
	c2dGraphData.m_strYtitle = _T("Number of flights ");
	c2dGraphData.m_strXtitle = _T("Time(HH:MM:SS)");	
	c2dGraphData.m_vrXTickTitle = m_vXAxisTitle;

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("Airside Time to position Report(Detailed) %s "),pParameter->GetParameterString());
	c2dGraphData.m_strFooter = strFooter;

	detailChartDataList::iterator iter = m_vData.begin();
	for (; iter!=m_vData.end(); ++iter)
	{
		c2dGraphData.m_vr2DChartData.push_back((*iter).m_vData);
	}
	chartWnd.DrawChart(c2dGraphData);
}

long CAirsideTimeToPositionChart::GetMinData( TakeoffDetailDataList& dataList )
{
	long lMin = (std::numeric_limits<long>::max)();
	for (size_t i = 0; i < dataList.size(); i++)
	{
		CTakeoffProcessDetailData& data = dataList[i];
		if (data.m_lToPostionTime < lMin)
		{
			lMin = data.m_lToPostionTime;
		}
	}
	return lMin;
}

long CAirsideTimeToPositionChart::GetMaxData( TakeoffDetailDataList& dataList )
{
	long lMax =(std::numeric_limits<long>::min)();
	for (size_t i = 0; i < dataList.size(); i++)
	{
		CTakeoffProcessDetailData& data = dataList[i];
		if (data.m_lToPostionTime > lMax)
		{
			lMax = data.m_lToPostionTime;
		}
	}
	return lMax;
}

void CAirsideTimeToPositionChart::InvertDetailToGraphData( TakeoffDetailDataList& vResult,detailChartDataList& detailData )
{
	size_t nSize = vResult.size();
	for (size_t i = 0; i < nSize; i++)
	{
		CTakeoffProcessDetailData& resultData = vResult[i];
		AddChartData(detailData,resultData.m_sFlightType,resultData.m_lToPostionTime);
	}
}

CAirsideTakeoffDelayChart::CAirsideTakeoffDelayChart()
{

}

CAirsideTakeoffDelayChart::~CAirsideTakeoffDelayChart()
{

}

void CAirsideTakeoffDelayChart::Draw3DChart( CARC3DChart& chartWnd, CParameters *pParameter )
{
	CString strLabel = _T("");
	C2DChartData c2dGraphData;
	c2dGraphData.m_strChartTitle = _T("Take off delay(Detailed)");
	c2dGraphData.m_strYtitle = _T("Number of flights ");
	c2dGraphData.m_strXtitle = _T("Time(HH:MM:SS)");	
	c2dGraphData.m_vrXTickTitle = m_vXAxisTitle;

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("Airside Take off delay Report(Detailed) %s "),pParameter->GetParameterString());
	c2dGraphData.m_strFooter = strFooter;

	detailChartDataList::iterator iter = m_vData.begin();
	for (; iter!=m_vData.end(); ++iter)
	{
		c2dGraphData.m_vr2DChartData.push_back((*iter).m_vData);
	}
	chartWnd.DrawChart(c2dGraphData);
}

void CAirsideTakeoffDelayChart::InvertDetailToGraphData( TakeoffDetailDataList& vResult,detailChartDataList& detailData )
{
	size_t nSize = vResult.size();
	for (size_t i = 0; i < nSize; i++)
	{
		CTakeoffProcessDetailData& resultData = vResult[i];
		AddChartData(detailData,resultData.m_sFlightType,resultData.m_lTakeoffDelay);
	}
}

long CAirsideTakeoffDelayChart::GetMinData( TakeoffDetailDataList& dataList )
{
	long lMin = (std::numeric_limits<long>::max)();
	for (size_t i = 0; i < dataList.size(); i++)
	{
		CTakeoffProcessDetailData& data = dataList[i];
		if (data.m_lTakeoffDelay < lMin)
		{
			lMin = data.m_lTakeoffDelay;
		}
	}
	return lMin;
}

long CAirsideTakeoffDelayChart::GetMaxData( TakeoffDetailDataList& dataList )
{
	long lMax =(std::numeric_limits<long>::min)();
	for (size_t i = 0; i < dataList.size(); i++)
	{
		CTakeoffProcessDetailData& data = dataList[i];
		if (data.m_lTakeoffDelay > lMax)
		{
			lMax = data.m_lTakeoffDelay;
		}
	}
	return lMax;
}