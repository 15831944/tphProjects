#include "StdAfx.h"
#include ".\lsvehicletotaldelaydetailresult.h"
#include "LandsideBaseParam.h"
#include "../MFCExControl/SortableHeaderCtrl.h"
#include "../MFCExControl/XListCtrl.h"
#include "..\results\LandsideVehicleDelayLog.h"
#include "AirsideReport/CARC3DChart.h"
#include <algorithm>
#include "../Common/UTIL.H"

LSVehicleTotalDelayDetailResult::LSVehicleTotalDelayDetailResult(void)
{
}

LSVehicleTotalDelayDetailResult::~LSVehicleTotalDelayDetailResult(void)
{
}

void LSVehicleTotalDelayDetailResult::Draw3DChart( CARC3DChart& chartWnd, LandsideBaseParam *pParameter )
{
	//draw chart
	if(pParameter == NULL)
		return;

	//get chart type
	int nChartType = pParameter->getSubType();

	LSVehicleTotalDelayDetailResult::ChartResult chartResult;

	int nDataCount = (int)m_vResult.size();
	for (int i = 0; i < nDataCount; i++)
	{
		TotalDelayItem& dataItem = m_vResult[i];

		switch (nChartType)
		{

		case CT_TOTALDELAY:
			chartResult.AddTime(dataItem.m_strVehicleTypeName,dataItem.m_eTotalDelayTime);
			break;
		case CT_ACTUAL:
			chartResult.AddTime(dataItem.m_strVehicleTypeName,dataItem.m_eActualTimeSpend);
			break;
		case CT_IDEAL:
			chartResult.AddTime(dataItem.m_strVehicleTypeName,dataItem.m_eIdealTimeSpent);
			break;
		default:
			{
				ASSERT(0);
			}
		}

	}


	//chartResult;
	chartResult.Draw3DChart(chartWnd,pParameter);
}

void LSVehicleTotalDelayDetailResult::GenerateResult( CBGetLogFilePath pFunc,LandsideBaseParam* pParameter, InputLandside *pLandisde )
{
	LSVehicleTotalDelayResult::GenerateResult(pFunc,pParameter,pLandisde);
}

void LSVehicleTotalDelayDetailResult::RefreshReport( LandsideBaseParam * parameter )
{

}

void LSVehicleTotalDelayDetailResult::InitListHead( CXListCtrl& cxListCtrl, LandsideBaseParam * parameter, CSortableHeaderCtrl* piSHC /*= NULL*/ )
{
	ASSERT(piSHC != NULL);
	if(piSHC == NULL)
		return;

	piSHC->ResetAll();

	//pax id

	int colIndex = 0;
	cxListCtrl.InsertColumn(colIndex, "Vehicle ID", LVCFMT_LEFT,50);
	piSHC->SetDataType(colIndex, dtINT );

	colIndex += 1;
	cxListCtrl.InsertColumn(colIndex, "Vehicle Type", LVCFMT_LEFT,100);
	piSHC->SetDataType(colIndex,  dtSTRING);

	colIndex += 1;
	cxListCtrl.InsertColumn(colIndex, "Entry Time", LVCFMT_LEFT,100);
	piSHC->SetDataType(colIndex, dtTIME );

	colIndex += 1;
	cxListCtrl.InsertColumn(colIndex, "Entry Resource", LVCFMT_LEFT,100);
	piSHC->SetDataType(colIndex, dtSTRING );


	colIndex += 1;
	cxListCtrl.InsertColumn(colIndex, "Exit Time", LVCFMT_LEFT,100);
	piSHC->SetDataType(colIndex, dtTIME );

	colIndex += 1;
	cxListCtrl.InsertColumn(colIndex, "Exit Resource", LVCFMT_LEFT,100);
	piSHC->SetDataType(colIndex, dtSTRING );

	colIndex += 1;
	cxListCtrl.InsertColumn(colIndex, "Total Delay Time", LVCFMT_LEFT,100);
	piSHC->SetDataType(colIndex, dtTIME );


	colIndex += 1;
	cxListCtrl.InsertColumn(colIndex, "Ideal Arrive Time", LVCFMT_LEFT,100);
	piSHC->SetDataType(colIndex, dtTIME );

	colIndex += 1;
	cxListCtrl.InsertColumn(colIndex, "Actual Time Spent", LVCFMT_LEFT,100);
	piSHC->SetDataType(colIndex, dtTIME );


	colIndex += 1;
	cxListCtrl.InsertColumn(colIndex, "Ideal Time Spent ", LVCFMT_LEFT,100);
	piSHC->SetDataType(colIndex, dtTIME );

	colIndex += 1;
	cxListCtrl.InsertColumn(colIndex,"Vehicle Complete Type",LVCFMT_LEFT,150);
	piSHC->SetDataType(colIndex,dtSTRING);

}

void LSVehicleTotalDelayDetailResult::FillListContent( CXListCtrl& cxListCtrl, LandsideBaseParam * parameter )
{
	int nRowIndex = 0;
	int nDataCount = (int)(m_vResult.size());
	for (int i = 0; i < nDataCount; ++i)
	{
		TotalDelayItem dataItem = m_vResult[i];
		int colIndex = 0;

		CString strText;
		strText.Format(_T("%d"), dataItem.m_nVehicleID);
		cxListCtrl.InsertItem(nRowIndex, strText);

		//
		colIndex += 1;
		cxListCtrl.SetItemText(nRowIndex,colIndex, dataItem.m_strVehicleTypeName);


		//
		colIndex += 1;
		cxListCtrl.SetItemText(nRowIndex,colIndex, dataItem.m_eEntryTime.PrintDateTime());
		//
		colIndex += 1;
		cxListCtrl.SetItemText(nRowIndex,colIndex, dataItem.m_strEntryRs);

		//
		colIndex += 1;
		cxListCtrl.SetItemText(nRowIndex,colIndex, dataItem.m_eExitTime.PrintDateTime());
		//
		colIndex += 1;
		cxListCtrl.SetItemText(nRowIndex,colIndex, dataItem.m_strExitRs);

		//
		colIndex += 1;
		cxListCtrl.SetItemText(nRowIndex,colIndex, dataItem.m_eTotalDelayTime.printTime());

		colIndex += 1;
		cxListCtrl.SetItemText(nRowIndex,colIndex, dataItem.m_eIdealArrTime.PrintDateTime());

		colIndex += 1;
		cxListCtrl.SetItemText(nRowIndex,colIndex, dataItem.m_eActualTimeSpend.printTime());

		colIndex += 1;
		cxListCtrl.SetItemText(nRowIndex,colIndex, dataItem.m_eIdealTimeSpent.printTime());

		colIndex += 1;
		cxListCtrl.SetItemText(nRowIndex,colIndex,dataItem.m_strCompleteTypeName);

		nRowIndex++;
	}
}

LSGraphChartTypeList LSVehicleTotalDelayDetailResult::GetChartList() const
{
	LSGraphChartTypeList chartList;

	chartList.AddItem(CT_TOTALDELAY,"Total delay time");
	chartList.AddItem(CT_ACTUAL,"Actual Time");
	chartList.AddItem(CT_IDEAL,"Ideal Time");

	return  chartList;
}

CString LSVehicleTotalDelayDetailResult::ChartResult::GetChartTitle( int iChart ) const
{
	if (iChart == CT_TOTALDELAY)
	{
		return _T("Vehicle Total Delay Time");
	}
	else if (iChart == CT_ACTUAL)
	{
		return _T("Vehicle Actual Time");
	}
	else
	{
		return _T("Vehicle Ideal Time");
	}
}

CString LSVehicleTotalDelayDetailResult::ChartResult::GetXTitle( int iChart ) const
{
	if (iChart == CT_TOTALDELAY)
	{
		return _T("Delay Time");
	}
	else if (iChart == CT_ACTUAL)
	{
		return _T("Actual Time");
	}
	else
	{
		return _T("Ideal Time");
	}
}

CString LSVehicleTotalDelayDetailResult::ChartResult::GetYTitle( int iChart ) const
{
	return _T("Number of Vehicle");
}

LSVehicleTotalDelayDetailResult::ChartResult::ChartResult( void )
{

}

LSVehicleTotalDelayDetailResult::ChartResult::~ChartResult( void )
{

}

void LSVehicleTotalDelayDetailResult::ChartResult::AddTime( const CString& strVehicleType,const ElapsedTime& eTime )
{
	m_mapDelayResult[strVehicleType].push_back(eTime);
}

void LSVehicleTotalDelayDetailResult::ChartResult::Draw3DChart( CARC3DChart& chartWnd, LandsideBaseParam *pParameter )
{
	SortChartData();

	CString strLabel = _T("");
	C2DChartData c2dGraphData;
	c2dGraphData.m_strChartTitle = GetChartTitle(pParameter->getSubType());
	c2dGraphData.m_strYtitle = GetYTitle(pParameter->getSubType());
	c2dGraphData.m_strXtitle = GetXTitle(pParameter->getSubType());	


	ElapsedTime eStartTime = ElapsedTime(0L);
	ElapsedTime eEndTime = GetMaxTime();
	ElapsedTime eInterval(pParameter->getInterval());

	ASSERT(eEndTime >= eStartTime);
	ASSERT(eInterval > ElapsedTime(0L));

	if(eStartTime > eEndTime || eInterval <= ElapsedTime(0L))
		return;

	for (ElapsedTime eTime = eStartTime; eTime < eEndTime; eTime += eInterval)
	{
		c2dGraphData.m_vrXTickTitle.push_back(GetXtitle(eTime, eTime + eInterval));
	}

	std::map<CString, std::vector<ElapsedTime> >::iterator iter =  m_mapDelayResult.begin();
	for (; iter != m_mapDelayResult.end(); ++iter)
	{
		c2dGraphData.m_vrLegend.push_back((*iter).first);
		std::vector<double> dRangeCount;
		for (ElapsedTime eTime = eStartTime; eTime < eEndTime; eTime += eInterval)
		{
			dRangeCount.push_back(GetCount((*iter).second,eTime, eTime + eInterval));
		}
		c2dGraphData.m_vr2DChartData.push_back(dRangeCount);
	}
	

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("Landside Vehicle Total Delay Report %s"), pParameter->GetParameterString());
	c2dGraphData.m_strFooter = strFooter;

	chartWnd.DrawChart(c2dGraphData);
}

int LSVehicleTotalDelayDetailResult::ChartResult::GetCount( std::vector<ElapsedTime>& vTime,ElapsedTime& eMinTime, ElapsedTime& eMaxTime ) const
{
	ASSERT(eMinTime < eMaxTime);

	int nRangeCount = 0;
	int nCount = static_cast<int>(vTime.size());
	for (int nItem = 0; nItem < nCount; ++ nItem)
	{
		if(vTime[nItem] < eMinTime)
			continue;
		if(vTime[nItem] > eMaxTime)
			break;

		nRangeCount += 1;
	}	

	return nRangeCount;
}

CString LSVehicleTotalDelayDetailResult::ChartResult::GetXtitle( ElapsedTime& eMinTime, ElapsedTime& eMaxTime ) const
{
	CString strXTitle;
	strXTitle.Format(_T("%s-%s"), eMinTime.printTime(), eMaxTime.printTime());
	return strXTitle;
}

ElapsedTime LSVehicleTotalDelayDetailResult::ChartResult::GetMaxTime()
{
	ElapsedTime eMaxTime;
	std::map<CString, std::vector<ElapsedTime> >::iterator iter =  m_mapDelayResult.begin();
	for (; iter != m_mapDelayResult.end(); ++iter)
	{
		eMaxTime = MAX(eMaxTime,GetMaxTime((*iter).second));
	}
	return eMaxTime;
}

ElapsedTime LSVehicleTotalDelayDetailResult::ChartResult::GetMaxTime( std::vector<ElapsedTime>& vTime ) const
{
	ElapsedTime eMaxTime;
	int nCount = static_cast<int>(vTime.size());
	if(nCount > 0)
		eMaxTime = vTime[0];

	for (int nItem = 1; nItem < nCount; ++ nItem)
	{
		if(vTime[nItem] > eMaxTime)
			eMaxTime = vTime[nItem];

	}	

	return eMaxTime;
}

void LSVehicleTotalDelayDetailResult::ChartResult::SortChartData()
{
	std::map<CString, std::vector<ElapsedTime> >::iterator iter =  m_mapDelayResult.begin();
	for (; iter != m_mapDelayResult.end(); ++iter)
	{
		std::sort((*iter).second.begin(),(*iter).second.end());
	}
}
