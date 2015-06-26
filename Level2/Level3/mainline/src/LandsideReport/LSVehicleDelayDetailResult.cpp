#include "StdAfx.h"
#include ".\lsvehicledelaydetailresult.h"
#include "../MFCExControl/SortableHeaderCtrl.h"
#include "../MFCExControl/XListCtrl.h"
#include "..\results\LandsideVehicleDelayLog.h"

#include "LSVehicleDelayParam.h"

#include "../Landside/InputLandside.h"

#include "AirsideReport/CARC3DChart.h"
#include <algorithm>


LSVehicleDelayDetailResult::LSVehicleDelayDetailResult(void)
{
}

LSVehicleDelayDetailResult::~LSVehicleDelayDetailResult(void)
{
}

void LSVehicleDelayDetailResult::Draw3DChart( CARC3DChart& chartWnd, LandsideBaseParam *pParameter )
{
	//draw chart
	if(pParameter == NULL)
		return;

	//get chart type
	int nChartType = pParameter->getSubType();

	LSVehicleDelayDetailResult::ChartResult chartResult;

	std::map<int, std::vector<DelayItem> >::iterator iter =  m_mapDelayResult.begin();
	for (; iter != m_mapDelayResult.end(); ++iter)
	{
		std::vector<DelayItem> &vItems = (*iter).second;
		
		ElapsedTime eDelayTime;
		int nCount = static_cast<int>(vItems.size());
		for (int nItem = 0; nItem < nCount; ++ nItem)
		{
			DelayItem& dItem = vItems[nItem];
			CString strVehicleType = dItem.m_strTypeName;
			switch (nChartType)
			{
			case CT_DelayTime:
				chartResult.AddChartData(strVehicleType,dItem.m_eDelayTime.getPrecisely());
				break;
			case CT_Reason:
				chartResult.AddChartData(strVehicleType,dItem.m_enumReason);
				break;
			case CT_Operation:
				chartResult.AddChartData(strVehicleType,dItem.m_enumOperation);
				break;
			default:
				{
					ASSERT(0);
				}
			}
		}
		

	}

	//chartResult;
	chartResult.Draw3DChart(chartWnd,pParameter);


}

void LSVehicleDelayDetailResult::GenerateResult( CBGetLogFilePath pFunc,LandsideBaseParam* pParameter, InputLandside *pLandisde )
{
	LSVehicleDelayResult::GenerateResult(pFunc, pParameter, pLandisde);

}

void LSVehicleDelayDetailResult::RefreshReport( LandsideBaseParam * parameter )
{

}

void LSVehicleDelayDetailResult::InitListHead( CXListCtrl& cxListCtrl, LandsideBaseParam * parameter, CSortableHeaderCtrl* piSHC /*= NULL*/ )
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
	cxListCtrl.InsertColumn(colIndex, "Time", LVCFMT_LEFT,100);
	piSHC->SetDataType(colIndex, dtTIME );

	/*colIndex += 1;
	cxListCtrl.InsertColumn(colIndex, "Start Time", LVCFMT_LEFT,100);
	piSHC->SetDataType(colIndex, dtTIME );


	colIndex += 1;
	cxListCtrl.InsertColumn(colIndex, "End Time", LVCFMT_LEFT,100);
	piSHC->SetDataType(colIndex, dtTIME );*/

	//colIndex += 1;
	//cxListCtrl.InsertColumn(colIndex, "From Object", LVCFMT_LEFT,100);
	//piSHC->SetDataType(colIndex, dtSTRING );


	//colIndex += 1;
	//cxListCtrl.InsertColumn(colIndex, "To Object", LVCFMT_LEFT,100);
	//piSHC->SetDataType(colIndex, dtSTRING );

	colIndex += 1;
	cxListCtrl.InsertColumn(colIndex, "Location", LVCFMT_LEFT,100);
	piSHC->SetDataType(colIndex, dtSTRING );

	colIndex += 1;
	cxListCtrl.InsertColumn(colIndex, "Delay Time", LVCFMT_LEFT,100);
	piSHC->SetDataType(colIndex, dtSTRING );


	colIndex += 1;
	cxListCtrl.InsertColumn(colIndex, "Reason ", LVCFMT_LEFT,100);
	piSHC->SetDataType(colIndex, dtSTRING );


	colIndex += 1;
	cxListCtrl.InsertColumn(colIndex, "Operation", LVCFMT_LEFT,100);
	piSHC->SetDataType(colIndex, dtSTRING );

	//colIndex += 1;
	//cxListCtrl.InsertColumn(colIndex,"Vehicle Complete Type",LVCFMT_LEFT,150);
	//piSHC->SetDataType(colIndex,dtSTRING);
	//colIndex += 1;
	//cxListCtrl.InsertColumn(colIndex, "Description", LVCFMT_LEFT,100);
	//piSHC->SetDataType(colIndex, dtSTRING );


}

void LSVehicleDelayDetailResult::FillListContent( CXListCtrl& cxListCtrl, LandsideBaseParam * parameter )
{
	int nRowIndex = 0;
	std::map<int, std::vector<DelayItem> >::iterator iter =  m_mapDelayResult.begin();
	for (; iter != m_mapDelayResult.end(); ++iter)
	{
		std::vector<DelayItem> &vItems = (*iter).second;

		int nCount = static_cast<int>(vItems.size());
		for (int nItem = 0; nItem < nCount; ++ nItem)
		{
			DelayItem& dItem = vItems[nItem];

			int colIndex = 0;

			CString strText;
			strText.Format(_T("%d"), dItem.m_nVehicleID);
			cxListCtrl.InsertItem(nRowIndex, strText);

			//
			colIndex += 1;
			cxListCtrl.SetItemText(nRowIndex,colIndex, dItem.m_strTypeName);

			//
			colIndex += 1;
			cxListCtrl.SetItemText(nRowIndex,colIndex, dItem.m_eIncurredTime.PrintDateTime());

			////
			//colIndex += 1;
			//cxListCtrl.SetItemText(nRowIndex,colIndex, dItem.m_eStartTime.PrintDateTime());


			////
			//colIndex += 1;
			//cxListCtrl.SetItemText(nRowIndex,colIndex, dItem.m_eEndTime.PrintDateTime());

			colIndex += 1;
			cxListCtrl.SetItemText(nRowIndex,colIndex, dItem.m_strResName);

			//
			colIndex += 1;
			cxListCtrl.SetItemText(nRowIndex,colIndex,dItem.m_eDelayTime.printTime());


			colIndex += 1;
			if(dItem.m_enumReason >= LandsideVehicleDelayLog::DR_TrafficLight && dItem.m_enumReason  < LandsideVehicleDelayLog::DR_Count)
				cxListCtrl.SetItemText(nRowIndex,colIndex, LandsideVehicleDelayLog::EnumDelayReasonName[dItem.m_enumReason - LandsideVehicleDelayLog::DR_TrafficLight]);

			colIndex += 1;
			if(dItem.m_enumOperation >= LandsideVehicleDelayLog::DO_Stop && dItem.m_enumOperation < LandsideVehicleDelayLog::DO_Count)
				cxListCtrl.SetItemText(nRowIndex,colIndex, LandsideVehicleDelayLog::EnumDelayOperationName[dItem.m_enumOperation - LandsideVehicleDelayLog::DO_Stop]);

			//colIndex += 1;
			//cxListCtrl.SetItemText(nRowIndex,colIndex, dItem.m_strDesc);


			//colIndex += 1;
			//cxListCtrl.SetItemText(nRowIndex,colIndex,dItem.m_strCompleteTypeName);


			//move to next row
			nRowIndex += 1;
		}
	}

}

LSGraphChartTypeList LSVehicleDelayDetailResult::GetChartList() const
{
	LSGraphChartTypeList chartList;

	chartList.AddItem(CT_DelayTime,"Delay Time");
	chartList.AddItem(CT_Reason,"Delay Reason");
	chartList.AddItem(CT_Operation,"Delay Operation");

	return  chartList;
}



///////////
LSVehicleDelayDetailResult::ChartResult::ChartResult( void )
{

}

LSVehicleDelayDetailResult::ChartResult::~ChartResult( void )
{

}

//void LSVehicleDelayDetailResult::ChartResult::AddTime( const ElapsedTime& eTime )
//{
//	m_vTime.push_back(eTime);
//}

void LSVehicleDelayDetailResult::ChartResult::Draw3DChart(CARC3DChart& chartWnd, LandsideBaseParam *pParameter )
{
	if (pParameter->getSubType() == CT_DelayTime)
	{	
		DrawDelay3DChart(chartWnd,pParameter);
	}
	else if (pParameter->getSubType() == CT_Reason)
	{
		DrawReason3DChart(chartWnd,pParameter);
	}
	else
	{
		DrawOperation3DChart(chartWnd,pParameter);
	}
}

int LSVehicleDelayDetailResult::ChartResult::GetCount(std::vector<int>& vTime, ElapsedTime& eMinTime, ElapsedTime& eMaxTime ) const
{
	ASSERT(eMinTime < eMaxTime);

	int nRangeCount = 0;
	int nCount = static_cast<int>(vTime.size());
	for (int nItem = 0; nItem < nCount; ++ nItem)
	{
		if(vTime[nItem] < eMinTime.getPrecisely())
			continue;
		if(vTime[nItem] > eMaxTime.getPrecisely())
			break;

		nRangeCount += 1;
	}	

	return nRangeCount;
}

CString LSVehicleDelayDetailResult::ChartResult::GetXtitle( ElapsedTime& eMinTime, ElapsedTime& eMaxTime ) const
{
	CString strXTitle;
	strXTitle.Format(_T("%s-%s"), eMinTime.printTime(), eMaxTime.printTime());
	return strXTitle;
}

ElapsedTime LSVehicleDelayDetailResult::ChartResult::GetMaxTime()
{
	ElapsedTime eMaxTime;
	std::map<CString, std::vector<int> >::iterator iter =  m_mapDelayResult.begin();
	for (; iter != m_mapDelayResult.end(); ++iter)
	{
		eMaxTime = MAX(eMaxTime,GetMaxTime((*iter).second));
	}
	return eMaxTime;
}

ElapsedTime LSVehicleDelayDetailResult::ChartResult::GetMaxTime( std::vector<int>& vTime ) const
{
	ElapsedTime eMaxTime;
	int nCount = static_cast<int>(vTime.size());
	if(nCount > 0)
		eMaxTime.setPrecisely(vTime[0]);

	for (int nItem = 1; nItem < nCount; ++ nItem)
	{
		if(vTime[nItem] > eMaxTime.getPrecisely())
			eMaxTime.setPrecisely(vTime[nItem]);

	}	

	return eMaxTime;
}

void LSVehicleDelayDetailResult::ChartResult::DrawDelay3DChart( CARC3DChart& chartWnd, LandsideBaseParam *pParameter )
{
	SortChartData();

	CString strLabel = _T("");
	C2DChartData c2dGraphData;
	c2dGraphData.m_strChartTitle = _T("Vehicle Delay Time");
	c2dGraphData.m_strYtitle = _T("Vehicle Delay Count");
	c2dGraphData.m_strXtitle = _T("Delay Time");	


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

	std::map<CString, std::vector<int> >::iterator iter =  m_mapDelayResult.begin();
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
	strFooter.Format(_T("Landside Vehicle Delay Report %s"), pParameter->GetParameterString());
	c2dGraphData.m_strFooter = strFooter;

	chartWnd.DrawChart(c2dGraphData);
}

void LSVehicleDelayDetailResult::ChartResult::DrawReason3DChart( CARC3DChart& chartWnd, LandsideBaseParam *pParameter )
{
	SortChartData();

	CString strLabel = _T("");
	C2DChartData c2dGraphData;
	c2dGraphData.m_strChartTitle = _T("Vehicle Delay Reason");;
	c2dGraphData.m_strYtitle = _T("Vehicle Delay Count");
	c2dGraphData.m_strXtitle = _T("Reason");	


	ElapsedTime eStartTime = ElapsedTime(0L);
	ElapsedTime eEndTime = GetMaxTime();
	ElapsedTime eInterval(pParameter->getInterval());

	ASSERT(eEndTime >= eStartTime);
	ASSERT(eInterval > ElapsedTime(0L));

	if(eStartTime > eEndTime || eInterval <= ElapsedTime(0L))
		return;

	c2dGraphData.m_vrXTickTitle.push_back(GetReasonTitle(LandsideVehicleDelayLog::DR_TrafficLight));
	c2dGraphData.m_vrXTickTitle.push_back(GetReasonTitle(LandsideVehicleDelayLog::DR_YeildSign));
	c2dGraphData.m_vrXTickTitle.push_back(GetReasonTitle(LandsideVehicleDelayLog::DR_ForeheadCar));
	c2dGraphData.m_vrXTickTitle.push_back(GetReasonTitle(LandsideVehicleDelayLog::DR_ForCrossWalk));
	std::map<CString, std::vector<int> >::iterator iter =  m_mapDelayResult.begin();
	for (; iter != m_mapDelayResult.end(); ++iter)
	{
		c2dGraphData.m_vrLegend.push_back((*iter).first);
		std::vector<double> dRangeCount;
		//Reason
		{
			dRangeCount.push_back(GetReasonCount((*iter).second,LandsideVehicleDelayLog::DR_TrafficLight));
			dRangeCount.push_back(GetReasonCount((*iter).second,LandsideVehicleDelayLog::DR_YeildSign));
			dRangeCount.push_back(GetReasonCount((*iter).second,LandsideVehicleDelayLog::DR_ForeheadCar));
			dRangeCount.push_back(GetReasonCount((*iter).second,LandsideVehicleDelayLog::DR_ForCrossWalk));
		}
		c2dGraphData.m_vr2DChartData.push_back(dRangeCount);

	}


	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("Landside Vehicle Delay Report %s"), pParameter->GetParameterString());
	c2dGraphData.m_strFooter = strFooter;

	chartWnd.DrawChart(c2dGraphData);
}

void LSVehicleDelayDetailResult::ChartResult::DrawOperation3DChart( CARC3DChart& chartWnd, LandsideBaseParam *pParameter )
{
	SortChartData();

	CString strLabel = _T("");
	C2DChartData c2dGraphData;
	c2dGraphData.m_strChartTitle = _T("Vehicle Delay Operaton");
	c2dGraphData.m_strYtitle = _T("Vehicle Delay Count");
	c2dGraphData.m_strXtitle = _T("Operation");	


	ElapsedTime eStartTime = ElapsedTime(0L);
	ElapsedTime eEndTime = GetMaxTime();
	ElapsedTime eInterval(pParameter->getInterval());

	ASSERT(eEndTime >= eStartTime);
	ASSERT(eInterval > ElapsedTime(0L));

	if(eStartTime > eEndTime || eInterval <= ElapsedTime(0L))
		return;


	c2dGraphData.m_vrXTickTitle.push_back(GetOperationTitle(LandsideVehicleDelayLog::DO_Stop));
	c2dGraphData.m_vrXTickTitle.push_back(GetOperationTitle(LandsideVehicleDelayLog::DO_SlowDown));
	std::map<CString, std::vector<int> >::iterator iter =  m_mapDelayResult.begin();
	for (; iter != m_mapDelayResult.end(); ++iter)
	{
		c2dGraphData.m_vrLegend.push_back((*iter).first);
		std::vector<double> dRangeCount;
		//operation 
		{
			dRangeCount.push_back(GetOperationCount((*iter).second,LandsideVehicleDelayLog::DO_Stop));
			dRangeCount.push_back(GetOperationCount((*iter).second,LandsideVehicleDelayLog::DO_SlowDown));
		}
		c2dGraphData.m_vr2DChartData.push_back(dRangeCount);

	}


	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("Landside Vehicle Delay Report %s"), pParameter->GetParameterString());
	c2dGraphData.m_strFooter = strFooter;

	chartWnd.DrawChart(c2dGraphData);
}

void LSVehicleDelayDetailResult::ChartResult::SortChartData()
{
	std::map<CString, std::vector<int> >::iterator iter =  m_mapDelayResult.begin();
	for (; iter != m_mapDelayResult.end(); ++iter)
	{
		std::sort((*iter).second.begin(),(*iter).second.end());
	}
}

int LSVehicleDelayDetailResult::ChartResult::GetReasonCount( std::vector<int>& vReason,int iReason ) const
{
	int nCount = (int)vReason.size();
	int iReasonCount = 0;
	for (int i = 0; i < nCount; i++)
	{
		int iData = vReason[i];
		if (iData == iReason)
		{
			iReasonCount++;
		}
	}
	return iReasonCount;
}

CString LSVehicleDelayDetailResult::ChartResult::GetReasonTitle( LandsideVehicleDelayLog::EnumDelayReason iReason ) const
{
	if (iReason == LandsideVehicleDelayLog::DR_TrafficLight)
	{
		return _T("Traffic Light");
	}
	else if (iReason == LandsideVehicleDelayLog::DR_YeildSign)
	{
		return _T("Yeild Sign");
	}
	else if (iReason == LandsideVehicleDelayLog::DR_ForeheadCar)
	{
		return _T("Forehead Care");
	}
	else
	{
		return _T("Crosswalk");
	}
}

int LSVehicleDelayDetailResult::ChartResult::GetOperationCount( std::vector<int>& vOperation,int iOperaton ) const
{
	int nCount = (int)vOperation.size();
	int iOperationCount = 0;
	for (int i = 0; i < nCount; i++)
	{
		int iData = vOperation[i];
		if (iData == iOperaton)
		{
			iOperationCount++;
		}
	}
	return iOperationCount;
}

CString LSVehicleDelayDetailResult::ChartResult::GetOperationTitle( LandsideVehicleDelayLog::EnumDelayOperation iOperation ) const
{
	if (iOperation == LandsideVehicleDelayLog::DO_SlowDown)
	{
		return _T("Slow Down");
	}
	else
	{
		return _T("Stopped");
	}
}

void LSVehicleDelayDetailResult::ChartResult::AddChartData( const CString& strVehicleType,int iData )
{
	m_mapDelayResult[strVehicleType].push_back(iData);
}

































