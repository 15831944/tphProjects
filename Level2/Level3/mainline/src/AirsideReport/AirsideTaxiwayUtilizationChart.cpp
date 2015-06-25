#include "stdafx.h"
#include "AirsideTaxiwayUtilizationChart.h"
#include "TaxiwayUtilizationData.h"
#include "TaxiwayItem.h"
#include "AirsideTaxiwayUtilizationPara.h"
///////////////////////////////////////////////////////////////////////////////////////////////
std::vector<CString> GetFlightInfo(std::vector<CTaxiwayUtilizationData*>& vResult,CTaxiwayUtilizationData* pItem)
{
	std::vector<CString> vFlightInfo;
	int nCount = (int)vResult.size();
	if (pItem)
	{
		for (int i = 0; i < nCount; i++)
		{
			CTaxiwayUtilizationDetailData* pData = (CTaxiwayUtilizationDetailData*)vResult[i];
			if (pData->m_sTaxiway == pItem->m_sTaxiway)
			{
				FlightInfoList& flightInfoList = pData->m_vFlightInfo;
				for (int j = 0; j < (int)flightInfoList.size(); j++)
				{
					TaxiwayUtilizationFlightInfo& flightInfo = flightInfoList[j];
					CString sFlightInfo(_T(""));
					sFlightInfo.Format(_T("%s %s"),flightInfo.m_sAirline,flightInfo.m_sActype);
					if (std::find(vFlightInfo.begin(),vFlightInfo.end(),sFlightInfo) == vFlightInfo.end())
					{
						vFlightInfo.push_back(sFlightInfo);
					}
				}
			}
		}
	}
	else
	{
		for (int i = 0; i < nCount; i++)
		{
			CTaxiwayUtilizationDetailData* pData = (CTaxiwayUtilizationDetailData*)vResult[i];
			FlightInfoList& flightInfoList = pData->m_vFlightInfo;
			for (int j = 0; j < (int)flightInfoList.size(); j++)
			{
				TaxiwayUtilizationFlightInfo& flightInfo = flightInfoList[j];
				CString sFlightInfo(_T(""));
				sFlightInfo.Format(_T("%s %s"),flightInfo.m_sAirline,flightInfo.m_sActype);
				if (std::find(vFlightInfo.begin(),vFlightInfo.end(),sFlightInfo) == vFlightInfo.end())
				{
					vFlightInfo.push_back(sFlightInfo);
				}
			}
		}
	}
	return vFlightInfo;
}

//base class//////////////////////////////////////////////////////////////////////////////////
CCTaxiwayUtilizationBaseChart::CCTaxiwayUtilizationBaseChart()
{

}

CCTaxiwayUtilizationBaseChart::~CCTaxiwayUtilizationBaseChart()
{

}
//CTaxiwayUtilizationDetailMovementChart////////////////////////////////////////////////////////
CTaxiwayUtilizationDetailMovementChart::CTaxiwayUtilizationDetailMovementChart()
{

}

CTaxiwayUtilizationDetailMovementChart::~CTaxiwayUtilizationDetailMovementChart()
{

}

void CTaxiwayUtilizationDetailMovementChart::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{
	CString strLabel = _T("");
	C2DChartData c2dGraphData;
	CAirsideTaxiwayUtilizationPara* pPara = (CAirsideTaxiwayUtilizationPara*)pParameter;
	CTaxiwayUtilizationData* pItem = pPara->GetCurrentSelect();
	CString strFooterName(_T(""));
	if (pItem)
	{
		CString strText(_T(""));
		strText.Format(_T("Number of movements taxiway %s"),pItem->m_sTaxiway);

		strFooterName.Format(_T("Number of movements taxiway %s \n intersection %s to intersection %s"),pItem->m_sTaxiway,pItem->m_sStartNode,\
			pItem->m_sEndNode);
		c2dGraphData.m_strChartTitle = strText;
	}
	else
	{
		c2dGraphData.m_strChartTitle = _T("Number of movements(All taxiways)");
		strFooterName = _T("Number of movements(All taxiways)");
	}
	
	c2dGraphData.m_strYtitle = _T("Movements per interval");
	c2dGraphData.m_strXtitle = _T("Data and Time of day");	
	c2dGraphData.m_vrXTickTitle = m_vTimeRange;

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("Detailed Taxiway Utilization %s;%s %s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime(),strFooterName);
	c2dGraphData.m_strFooter = strFooter;

	std::vector<TaxiwayUtilizationData>::iterator iter = m_vTaxiwayDatal.begin();
	for (; iter!=m_vTaxiwayDatal.end(); ++iter)
	{
		CString strLegend = _T("");
		strLegend = (*iter).m_sFlightInfo;
		c2dGraphData.m_vrLegend.push_back(strLegend);
		c2dGraphData.m_vr2DChartData.push_back((*iter).m_Data);
	}
	chartWnd.DrawChart(c2dGraphData);
}

void CTaxiwayUtilizationDetailMovementChart::GenerateResult(std::vector<CTaxiwayUtilizationData*>& vResult,CParameters *pParameter)
{
	int intervalSize = 0 ;
	ElapsedTime IntervalTime ;
	IntervalTime = (pParameter->getEndTime() - pParameter->getStartTime()) ;
	intervalSize = IntervalTime.asSeconds() / pParameter->getInterval() ;

	long lUserIntervalTime = pParameter->getInterval();
	ElapsedTime estUserIntervalTime = ElapsedTime(lUserIntervalTime);

	CString timeInterval ;
	ElapsedTime startTime = pParameter->getStartTime();
	ElapsedTime endtime ;
	for (int i = 0 ; i < intervalSize ;i++)
	{
		endtime = startTime + ElapsedTime(pParameter->getInterval()) ;
		timeInterval.Format(_T("%s-%s"),startTime.printTime(),endtime.printTime()) ;
		startTime = endtime ;
		m_vTimeRange.push_back(timeInterval);
	}

	CAirsideTaxiwayUtilizationPara* pPara = (CAirsideTaxiwayUtilizationPara*)pParameter;
	CTaxiwayUtilizationData* pItem = pPara->GetCurrentSelect();
	std::vector<CString>vFlightInfo = GetFlightInfo(vResult,pItem);
	ElapsedTime estMinDelayTime = pParameter->getStartTime();
	if (pItem)
	{
		for (int i = 0; i < (int)vFlightInfo.size() && i < 20; i++)
		{
			TaxiwayUtilizationData item;
			item.m_sFlightInfo = vFlightInfo[i];
			for (long j=0; j<intervalSize; j++)
			{
				int nMovements = 0;
				ElapsedTime estTempMinDelayTime = estMinDelayTime + ElapsedTime(estUserIntervalTime.asSeconds()*j);
				for (int n = 0; n < (int)vResult.size(); n++)
				{
					CTaxiwayUtilizationDetailData* pData = (CTaxiwayUtilizationDetailData*)vResult[n];
					if (pData->m_sTaxiway == pItem->m_sTaxiway && pData->m_intervalStart == estTempMinDelayTime.asSeconds())
					{
						FlightInfoList& flightInfoList = pData->m_vFlightInfo;
						for (int m = 0; m < (int)flightInfoList.size(); m++)
						{
							TaxiwayUtilizationFlightInfo& flightInfo = flightInfoList[m];
							CString sFlightInfo = flightInfo.m_sAirline + CString(_T(" ")) + flightInfo.m_sActype;
							if (sFlightInfo == item.m_sFlightInfo)
							{
								nMovements += flightInfo.m_lMovements;
							}
						}
					}
				}
				item.m_Data.push_back(nMovements);
			}
			m_vTaxiwayDatal.push_back(item);
		}
	}
	else
	{
		for (int i = 0; i < (int)vFlightInfo.size() && i < 20; i++)
		{
			TaxiwayUtilizationData item;
			item.m_sFlightInfo = vFlightInfo[i];
			for (long j=0; j<intervalSize; j++)
			{
				int nMovements = 0;
				ElapsedTime estTempMinDelayTime = estMinDelayTime + ElapsedTime(estUserIntervalTime.asSeconds()*j);
				for (int n = 0; n < (int)vResult.size(); n++)
				{
					CTaxiwayUtilizationDetailData* pData = (CTaxiwayUtilizationDetailData*)vResult[n];
					if (pData->m_intervalStart == estTempMinDelayTime.asSeconds())
					{
						FlightInfoList& flightInfoList = pData->m_vFlightInfo;
						for (int m = 0; m < (int)flightInfoList.size(); m++)
						{
							TaxiwayUtilizationFlightInfo& flightInfo = flightInfoList[m];
							CString sFlightInfo = flightInfo.m_sAirline + CString(_T(" ")) + flightInfo.m_sActype;
							if (sFlightInfo == item.m_sFlightInfo)
							{
								nMovements += flightInfo.m_lMovements;
							}
						}
					}
				}
				item.m_Data.push_back(nMovements);
			}
			m_vTaxiwayDatal.push_back(item);
		}
	}
}

/////CTaxiwayUtilizationDetailSpeedChart///////////////////////////////////////////////////////
CTaxiwayUtilizationDetailSpeedChart::CTaxiwayUtilizationDetailSpeedChart()
{

}

CTaxiwayUtilizationDetailSpeedChart::~CTaxiwayUtilizationDetailSpeedChart()
{

}

void CTaxiwayUtilizationDetailSpeedChart::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{
	CString strLabel = _T("");
	C2DChartData c2dGraphData;
	

	CAirsideTaxiwayUtilizationPara* pPara = (CAirsideTaxiwayUtilizationPara*)pParameter;
	CTaxiwayUtilizationData* pItem = pPara->GetCurrentSelect();
	CString strFooterName(_T(""));
	if (pItem)
	{
		CString strText(_T(""));
		strText.Format(_T("Average speed on taxiway %s"),pItem->m_sTaxiway);

		strFooterName.Format(_T("Average speed on taxiway %s \n intersection %s to intersection %s"),pItem->m_sTaxiway,pItem->m_sStartNode,\
			pItem->m_sEndNode);
		c2dGraphData.m_strChartTitle = strText;
	}
	else
	{
		c2dGraphData.m_strChartTitle = _T("Average speed (All taxiways)");
		strFooterName = _T("Average speed (All taxiways)");
	}
	c2dGraphData.m_strYtitle = _T("Speed(knots)");
	c2dGraphData.m_strXtitle = _T("Data and Time of day");	
	c2dGraphData.m_vrXTickTitle = m_vTimeRange;

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("Detailed Taxiway Utilization %s;%s %s"), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime(),strFooterName);
	c2dGraphData.m_strFooter = strFooter;

	std::vector<TaxiwayUtilizationData>::iterator iter = m_vTaxiwayDatal.begin();
	for (; iter!=m_vTaxiwayDatal.end(); ++iter)
	{
		CString strLegend = _T("");
		strLegend = (*iter).m_sFlightInfo;
		c2dGraphData.m_vrLegend.push_back(strLegend);
		c2dGraphData.m_vr2DChartData.push_back((*iter).m_Data);
	}
	chartWnd.DrawChart(c2dGraphData);
}

void CTaxiwayUtilizationDetailSpeedChart::GenerateResult(std::vector<CTaxiwayUtilizationData*>& vResult,CParameters *pParameter)
{
	int intervalSize = 0 ;
	ElapsedTime IntervalTime ;
	IntervalTime = (pParameter->getEndTime() - pParameter->getStartTime()) ;
	intervalSize = IntervalTime.asSeconds() / pParameter->getInterval() ;

	long lUserIntervalTime = pParameter->getInterval();
	ElapsedTime estUserIntervalTime = ElapsedTime(lUserIntervalTime);

	CString timeInterval ;
	ElapsedTime startTime = pParameter->getStartTime();
	ElapsedTime endtime ;
	for (int i = 0 ; i < intervalSize ;i++)
	{
		endtime = startTime + ElapsedTime(pParameter->getInterval()) ;
		timeInterval.Format(_T("%s-%s"),startTime.printTime(),endtime.printTime()) ;
		startTime = endtime ;
		m_vTimeRange.push_back(timeInterval);
	}

	CAirsideTaxiwayUtilizationPara* pPara = (CAirsideTaxiwayUtilizationPara*)pParameter;
	CTaxiwayUtilizationData* pItem = pPara->GetCurrentSelect();
	std::vector<CString>vFlightInfo = GetFlightInfo(vResult,pItem);
	ElapsedTime estMinDelayTime = pParameter->getStartTime();
	if (pItem)
	{
		for (int i = 0; i < (int)vFlightInfo.size() && i < 20; i++)
		{
			TaxiwayUtilizationData item;
			item.m_sFlightInfo = vFlightInfo[i];
			for (long j=0; j<intervalSize; j++)
			{
				CStatisticalTools<double> statisticalTool;
				ElapsedTime estTempMinDelayTime = estMinDelayTime + ElapsedTime(estUserIntervalTime.asSeconds()*j);
				for (int n = 0; n < (int)vResult.size(); n++)
				{
					CTaxiwayUtilizationDetailData* pData = (CTaxiwayUtilizationDetailData*)vResult[n];
					if (pData->m_sTaxiway == pItem->m_sTaxiway && pData->m_intervalStart == estTempMinDelayTime.asSeconds())
					{
						FlightInfoList& flightInfoList = pData->m_vFlightInfo;
						for (int m = 0; m < (int)flightInfoList.size(); m++)
						{
							TaxiwayUtilizationFlightInfo& flightInfo = flightInfoList[m];
							CString sFlightInfo = flightInfo.m_sAirline + CString(_T(" ")) + flightInfo.m_sActype;
							if (sFlightInfo == item.m_sFlightInfo)
							{
								statisticalTool.AddNewData(flightInfo.m_avaSpeed);
							}
						}
					}
				}
				
				item.m_Data.push_back(statisticalTool.GetAvarage());
			}
			m_vTaxiwayDatal.push_back(item);
		}
	}
	else
	{
		for (int i = 0; i < (int)vFlightInfo.size() && i < 20; i++)
		{
			TaxiwayUtilizationData item;
			item.m_sFlightInfo = vFlightInfo[i];
			for (long j=0; j<intervalSize; j++)
			{
				CStatisticalTools<double> statisticalTool;
				ElapsedTime estTempMinDelayTime = estMinDelayTime + ElapsedTime(estUserIntervalTime.asSeconds()*j);
				for (int n = 0; n < (int)vResult.size(); n++)
				{
					CTaxiwayUtilizationDetailData* pData = (CTaxiwayUtilizationDetailData*)vResult[n];
					if (pData->m_intervalStart == estTempMinDelayTime.asSeconds())
					{
						FlightInfoList& flightInfoList = pData->m_vFlightInfo;
						for (int m = 0; m < (int)flightInfoList.size(); m++)
						{
							TaxiwayUtilizationFlightInfo& flightInfo = flightInfoList[m];
							CString sFlightInfo = flightInfo.m_sAirline + CString(_T(" ")) + flightInfo.m_sActype;
							if (sFlightInfo == item.m_sFlightInfo)
							{
								statisticalTool.AddNewData(flightInfo.m_avaSpeed);
							}
						}
					}
				}
				item.m_Data.push_back(statisticalTool.GetAvarage());
			}
			m_vTaxiwayDatal.push_back(item);
		}
	}
}

////////////////CTaxiwayUtilizationDetailOccupancyChart/////////////////////////////////////////
CTaxiwayUtilizationDetailOccupancyChart::CTaxiwayUtilizationDetailOccupancyChart()
{

}

CTaxiwayUtilizationDetailOccupancyChart::~CTaxiwayUtilizationDetailOccupancyChart()
{

}

void CTaxiwayUtilizationDetailOccupancyChart::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{
	CString strLabel = _T("");
	C2DChartData c2dGraphData;
	
	c2dGraphData.m_strYtitle = _T("Occupancy(secs)");
	c2dGraphData.m_strXtitle = _T("Data and Time of day");	
	c2dGraphData.m_vrXTickTitle = m_vTimeRange;

	CAirsideTaxiwayUtilizationPara* pPara = (CAirsideTaxiwayUtilizationPara*)pParameter;
	CTaxiwayUtilizationData* pItem = pPara->GetCurrentSelect();
	CString strFooterName(_T(""));
	if (pItem)
	{
		CString strText(_T(""));
		strText.Format(_T("Occupancy taxiway %s "),pItem->m_sTaxiway);

		strFooterName.Format(_T("Occupancy taxiway %s \n intersection %s to intersection %s"),pItem->m_sTaxiway,pItem->m_sStartNode,\
			pItem->m_sEndNode);

		c2dGraphData.m_strChartTitle = strText;
	}
	else
	{
		c2dGraphData.m_strChartTitle = _T("Occupancy (All taxiways)");
		strFooterName = _T("Occupancy (All taxiways)");
	}
	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("Detailed Taxiway Utilization %s;%s %s"), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime(),strFooterName);
	c2dGraphData.m_strFooter = strFooter;

	std::vector<TaxiwayUtilizationData>::iterator iter = m_vTaxiwayDatal.begin();
	for (; iter!=m_vTaxiwayDatal.end(); ++iter)
	{
		CString strLegend = _T("");
		strLegend = (*iter).m_sFlightInfo;
		c2dGraphData.m_vrLegend.push_back(strLegend);
		c2dGraphData.m_vr2DChartData.push_back((*iter).m_Data);
	}
	chartWnd.DrawChart(c2dGraphData);
}

void CTaxiwayUtilizationDetailOccupancyChart::GenerateResult(std::vector<CTaxiwayUtilizationData*>& vResult,CParameters *pParameter)
{
	int intervalSize = 0 ;
	ElapsedTime IntervalTime ;
	IntervalTime = (pParameter->getEndTime() - pParameter->getStartTime()) ;
	intervalSize = IntervalTime.asSeconds() / pParameter->getInterval() ;

	long lUserIntervalTime = pParameter->getInterval();
	ElapsedTime estUserIntervalTime = ElapsedTime(lUserIntervalTime);

	CString timeInterval ;
	ElapsedTime startTime = pParameter->getStartTime();
	ElapsedTime endtime ;
	for (int i = 0 ; i < intervalSize ;i++)
	{
		endtime = startTime + ElapsedTime(pParameter->getInterval()) ;
		timeInterval.Format(_T("%s-%s"),startTime.printTime(),endtime.printTime()) ;
		startTime = endtime ;
		m_vTimeRange.push_back(timeInterval);
	}

	CAirsideTaxiwayUtilizationPara* pPara = (CAirsideTaxiwayUtilizationPara*)pParameter;
	CTaxiwayUtilizationData* pItem = pPara->GetCurrentSelect();
	std::vector<CString>vFlightInfo = GetFlightInfo(vResult,pItem);
	ElapsedTime estMinDelayTime = pParameter->getStartTime();
	if (pItem)
	{
		for (int i = 0; i < (int)vFlightInfo.size() && i < 20; i++)
		{
			TaxiwayUtilizationData item;
			item.m_sFlightInfo = vFlightInfo[i];
			for (long j=0; j<intervalSize; j++)
			{
				long lOccupancy = 0;
				ElapsedTime estTempMinDelayTime = estMinDelayTime + ElapsedTime(estUserIntervalTime.asSeconds()*j);
				for (int n = 0; n < (int)vResult.size(); n++)
				{
					CTaxiwayUtilizationDetailData* pData = (CTaxiwayUtilizationDetailData*)vResult[n];
					if (pData->m_sTaxiway == pItem->m_sTaxiway && pData->m_intervalStart == estTempMinDelayTime.asSeconds())
					{
						FlightInfoList& flightInfoList = pData->m_vFlightInfo;
						for (int m = 0; m < (int)flightInfoList.size(); m++)
						{
							TaxiwayUtilizationFlightInfo& flightInfo = flightInfoList[m];
							CString sFlightInfo = flightInfo.m_sAirline + CString(_T(" ")) + flightInfo.m_sActype;
							if (sFlightInfo == item.m_sFlightInfo)
							{
								lOccupancy += flightInfo.m_lOccupancy;
							}
						}
					}
				}
				item.m_Data.push_back(lOccupancy);
			}
			m_vTaxiwayDatal.push_back(item);
		}
	}
	else
	{
		for (int i = 0; i < (int)vFlightInfo.size() && i < 20; i++)
		{
			TaxiwayUtilizationData item;
			item.m_sFlightInfo = vFlightInfo[i];
			for (long j=0; j<intervalSize; j++)
			{
				long lOccupancy = 0;
				ElapsedTime estTempMinDelayTime = estMinDelayTime + ElapsedTime(estUserIntervalTime.asSeconds()*j);
				CString strTime = estTempMinDelayTime.printTime();
				for (int n = 0; n < (int)vResult.size(); n++)
				{
					CTaxiwayUtilizationDetailData* pData = (CTaxiwayUtilizationDetailData*)vResult[n];
					CString strResultTime = ElapsedTime(pData->m_intervalStart).printTime();
					if (pData->m_intervalStart == estTempMinDelayTime.asSeconds())
					{
						FlightInfoList& flightInfoList = pData->m_vFlightInfo;
						for (int m = 0; m < (int)flightInfoList.size(); m++)
						{
							TaxiwayUtilizationFlightInfo& flightInfo = flightInfoList[m];
							CString sFlightInfo = flightInfo.m_sAirline + CString(_T(" ")) + flightInfo.m_sActype;
							if (sFlightInfo == item.m_sFlightInfo)
							{
								lOccupancy += flightInfo.m_lOccupancy;
							}
						}
					}
				}
				item.m_Data.push_back(lOccupancy);
			}
			m_vTaxiwayDatal.push_back(item);
		}
	}
}

///////////////CTaxiwayUtilizationSummaryMovementChart/////////////////////////////////////////
CTaxiwayUtilizationSummaryMovementChart::CTaxiwayUtilizationSummaryMovementChart()
{

}

CTaxiwayUtilizationSummaryMovementChart::~CTaxiwayUtilizationSummaryMovementChart()
{

}

void CTaxiwayUtilizationSummaryMovementChart::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{
 	CString strLabel = _T("");
 	C2DChartData c2dGraphData;
 
 	c2dGraphData.m_strChartTitle = _T("Summary of movements");
 	c2dGraphData.m_strYtitle = _T("Movements per interval");
 	c2dGraphData.m_strXtitle = _T("Taxiway");
 
	
 	int nTaxiwayCount = (int)m_vResult.size();
 	if (nTaxiwayCount == 0)
 		return;
 
 	for (int nTaxiway = 0; nTaxiway < nTaxiwayCount; ++nTaxiway)
 	{
 		c2dGraphData.m_vrXTickTitle.push_back(m_vResult[nTaxiway]->m_sTaxiway);
 	}
 
 
 
 	//set footer
 	CString strFooter(_T(""));
 	strFooter.Format(_T("%s %s,%s "), c2dGraphData.m_strChartTitle,
 		pParameter->getStartTime().printTime(), 
 		pParameter->getEndTime().printTime());
 
 	c2dGraphData.m_strFooter = strFooter;
 
 
 	std::vector<CString> vLegends;
 	//get the legend
 	vLegends.push_back(_T("Min"));
 	vLegends.push_back(_T("Average"));
 	vLegends.push_back(_T("Max"));
 	vLegends.push_back(_T("Q1"));
 	vLegends.push_back(_T("Q2"));
 	vLegends.push_back(_T("Q3"));
 	vLegends.push_back(_T("P1"));
 	vLegends.push_back(_T("P5"));
 	vLegends.push_back(_T("P10"));
 	vLegends.push_back(_T("P90"));
 	vLegends.push_back(_T("P95"));
 	vLegends.push_back(_T("P99"));
 	vLegends.push_back(_T("Sigma"));
 
 
 	std::vector< std::vector<double> > vLegendData;
 	vLegendData.resize(vLegends.size());
 
 
 	for (int nIndex = 0; nIndex < nTaxiwayCount; ++ nIndex)
 	{
 		CTaxiwayUtilizationSummaryData* statisticItem = NULL;
 		statisticItem = (CTaxiwayUtilizationSummaryData*)m_vResult[nIndex];
 
 		if(statisticItem == NULL)
 			continue;
 
 		//min
 		vLegendData[0].push_back((*statisticItem).m_movementData.m_dMinValue);
 		vLegendData[1].push_back((*statisticItem).m_movementData.m_dVerageValue);
 		vLegendData[2].push_back((*statisticItem).m_movementData.m_dMaxValue);
 		vLegendData[3].push_back((*statisticItem).m_movementData.m_statisticalTool.m_dQ1);
 		vLegendData[4].push_back((*statisticItem).m_movementData.m_statisticalTool.m_dQ2);
 		vLegendData[5].push_back((*statisticItem).m_movementData.m_statisticalTool.m_dQ3);
 		vLegendData[6].push_back((*statisticItem).m_movementData.m_statisticalTool.m_dP1);
 		vLegendData[7].push_back((*statisticItem).m_movementData.m_statisticalTool.m_dP5);
 		vLegendData[8].push_back((*statisticItem).m_movementData.m_statisticalTool.m_dP10);
 		vLegendData[9].push_back((*statisticItem).m_movementData.m_statisticalTool.m_dP90);
 		vLegendData[10].push_back((*statisticItem).m_movementData.m_statisticalTool.m_dP95);
 		vLegendData[11].push_back((*statisticItem).m_movementData.m_statisticalTool.m_dP99);
 		vLegendData[12].push_back((*statisticItem).m_movementData.m_statisticalTool.m_dSigma);
 	}
 
 	int nLegendCount = vLegendData.size();
 	for (int nLegendData = 0; nLegendData < nLegendCount; ++ nLegendData)
 	{
 		c2dGraphData.m_vr2DChartData.push_back(vLegendData[nLegendData]);
 	}
 
 	c2dGraphData.m_vrLegend = vLegends;
 
 	chartWnd.DrawChart(c2dGraphData);
}

void CTaxiwayUtilizationSummaryMovementChart::GenerateResult(std::vector<CTaxiwayUtilizationData*>& vResult,CParameters *pParameter)
{
	CAirsideTaxiwayUtilizationPara* pPara = (CAirsideTaxiwayUtilizationPara*)pParameter;
	CTaxiwayUtilizationData* pItem = pPara->GetCurrentSelect();

	if (pItem)
	{
		int nCount = (int)vResult.size();
		for (int i = 0; i < nCount; i++)
		{
			CTaxiwayUtilizationData* pData = vResult[i];
			if (pData->m_sTaxiway == pItem->m_sTaxiway && pData->m_sStartNode == pItem->m_sStartNode
				&& pData->m_sEndNode == pItem->m_sEndNode)
			{
				m_vResult.push_back(pData);
			}
		}
	}
	else
	{
		m_vResult = vResult;
	}
	
}

////////CTaxiwayUtilizationSummarySpeedChart////////////////////////////////////////////////////
CTaxiwayUtilizationSummarySpeedChart::CTaxiwayUtilizationSummarySpeedChart()
{

}

CTaxiwayUtilizationSummarySpeedChart::~CTaxiwayUtilizationSummarySpeedChart()
{

}

void CTaxiwayUtilizationSummarySpeedChart::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{
	CString strLabel = _T("");
	C2DChartData c2dGraphData;

	c2dGraphData.m_strChartTitle = _T("Summary of speeds");
	c2dGraphData.m_strYtitle = _T("Speed(knots)");
	c2dGraphData.m_strXtitle = _T("Taxiway");


	int nTaxiwayCount = (int)m_vResult.size();
	if (nTaxiwayCount == 0)
		return;

	for (int nTaxiway = 0; nTaxiway < nTaxiwayCount; ++nTaxiway)
	{
		c2dGraphData.m_vrXTickTitle.push_back(m_vResult[nTaxiway]->m_sTaxiway);
	}



	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("%s %s,%s "), c2dGraphData.m_strChartTitle,
		pParameter->getStartTime().printTime(), 
		pParameter->getEndTime().printTime());

	c2dGraphData.m_strFooter = strFooter;


	std::vector<CString> vLegends;
	//get the legend
	vLegends.push_back(_T("Min"));
	vLegends.push_back(_T("Average"));
	vLegends.push_back(_T("Max"));
	vLegends.push_back(_T("Q1"));
	vLegends.push_back(_T("Q2"));
	vLegends.push_back(_T("Q3"));
	vLegends.push_back(_T("P1"));
	vLegends.push_back(_T("P5"));
	vLegends.push_back(_T("P10"));
	vLegends.push_back(_T("P90"));
	vLegends.push_back(_T("P95"));
	vLegends.push_back(_T("P99"));
	vLegends.push_back(_T("Sigma"));


	std::vector< std::vector<double> > vLegendData;
	vLegendData.resize(vLegends.size());


	for (int nIndex = 0; nIndex < nTaxiwayCount; ++ nIndex)
	{
		CTaxiwayUtilizationSummaryData* statisticItem = NULL;
		statisticItem = (CTaxiwayUtilizationSummaryData*)m_vResult[nIndex];

		if(statisticItem == NULL)
			continue;

		//min
		vLegendData[0].push_back((*statisticItem).m_speedData.m_dMinValue);
		vLegendData[1].push_back((*statisticItem).m_speedData.m_dVerageValue);
		vLegendData[2].push_back((*statisticItem).m_speedData.m_dMaxValue);
		vLegendData[3].push_back((*statisticItem).m_speedData.m_statisticalTool.m_dQ1);
		vLegendData[4].push_back((*statisticItem).m_speedData.m_statisticalTool.m_dQ2);
		vLegendData[5].push_back((*statisticItem).m_speedData.m_statisticalTool.m_dQ3);
		vLegendData[6].push_back((*statisticItem).m_speedData.m_statisticalTool.m_dP1);
		vLegendData[7].push_back((*statisticItem).m_speedData.m_statisticalTool.m_dP5);
		vLegendData[8].push_back((*statisticItem).m_speedData.m_statisticalTool.m_dP10);
		vLegendData[9].push_back((*statisticItem).m_speedData.m_statisticalTool.m_dP90);
		vLegendData[10].push_back((*statisticItem).m_speedData.m_statisticalTool.m_dP95);
		vLegendData[11].push_back((*statisticItem).m_speedData.m_statisticalTool.m_dP99);
		vLegendData[12].push_back((*statisticItem).m_speedData.m_statisticalTool.m_dSigma);
	}

	int nLegendCount = vLegendData.size();
	for (int nLegendData = 0; nLegendData < nLegendCount; ++ nLegendData)
	{
		c2dGraphData.m_vr2DChartData.push_back(vLegendData[nLegendData]);
	}

	c2dGraphData.m_vrLegend = vLegends;

	chartWnd.DrawChart(c2dGraphData);
}

void CTaxiwayUtilizationSummarySpeedChart::GenerateResult(std::vector<CTaxiwayUtilizationData*>& vResult,CParameters *pParameter)
{
	CAirsideTaxiwayUtilizationPara* pPara = (CAirsideTaxiwayUtilizationPara*)pParameter;
	CTaxiwayUtilizationData* pItem = pPara->GetCurrentSelect();

	if (pItem)
	{
		int nCount = (int)vResult.size();
		for (int i = 0; i < nCount; i++)
		{
			CTaxiwayUtilizationData* pData = vResult[i];
			if (pData->m_sTaxiway == pItem->m_sTaxiway && pData->m_sStartNode == pItem->m_sStartNode
				&& pData->m_sEndNode == pItem->m_sEndNode)
			{
				m_vResult.push_back(pData);
			}
		}
	}
	else
	{
		m_vResult = vResult;
	}
}

////////////CTaxiwayUtilizationSummaryOccupancyChart//////////////////////////////////////////
CTaxiwayUtilizationSummaryOccupancyChart::CTaxiwayUtilizationSummaryOccupancyChart()
{

}

CTaxiwayUtilizationSummaryOccupancyChart::~CTaxiwayUtilizationSummaryOccupancyChart()
{

}

void CTaxiwayUtilizationSummaryOccupancyChart::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{
	CString strLabel = _T("");
	C2DChartData c2dGraphData;

	c2dGraphData.m_strChartTitle = _T("Summary of Occupancy");
	c2dGraphData.m_strYtitle = _T("Occupancy(secs)");
	c2dGraphData.m_strXtitle = _T("Taxiway");


	int nTaxiwayCount = (int)m_vResult.size();
	if (nTaxiwayCount == 0)
		return;

	for (int nTaxiway = 0; nTaxiway < nTaxiwayCount; ++nTaxiway)
	{
		c2dGraphData.m_vrXTickTitle.push_back(m_vResult[nTaxiway]->m_sTaxiway);
	}



	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("%s %s,%s "), c2dGraphData.m_strChartTitle,
		pParameter->getStartTime().printTime(), 
		pParameter->getEndTime().printTime());

	c2dGraphData.m_strFooter = strFooter;


	std::vector<CString> vLegends;
	//get the legend
	vLegends.push_back(_T("Min"));
	vLegends.push_back(_T("Average"));
	vLegends.push_back(_T("Max"));
	vLegends.push_back(_T("Q1"));
	vLegends.push_back(_T("Q2"));
	vLegends.push_back(_T("Q3"));
	vLegends.push_back(_T("P1"));
	vLegends.push_back(_T("P5"));
	vLegends.push_back(_T("P10"));
	vLegends.push_back(_T("P90"));
	vLegends.push_back(_T("P95"));
	vLegends.push_back(_T("P99"));
	vLegends.push_back(_T("Sigma"));


	std::vector< std::vector<double> > vLegendData;
	vLegendData.resize(vLegends.size());


	for (int nIndex = 0; nIndex < nTaxiwayCount; ++ nIndex)
	{
		CTaxiwayUtilizationSummaryData* statisticItem = NULL;
		statisticItem = (CTaxiwayUtilizationSummaryData*)m_vResult[nIndex];

		if(statisticItem == NULL)
			continue;

		//min
		vLegendData[0].push_back((*statisticItem).m_occupancyData.m_dMinValue);
		vLegendData[1].push_back((*statisticItem).m_occupancyData.m_dVerageValue);
		vLegendData[2].push_back((*statisticItem).m_occupancyData.m_dMaxValue);
		vLegendData[3].push_back((*statisticItem).m_occupancyData.m_statisticalTool.m_dQ1);
		vLegendData[4].push_back((*statisticItem).m_occupancyData.m_statisticalTool.m_dQ2);
		vLegendData[5].push_back((*statisticItem).m_occupancyData.m_statisticalTool.m_dQ3);
		vLegendData[6].push_back((*statisticItem).m_occupancyData.m_statisticalTool.m_dP1);
		vLegendData[7].push_back((*statisticItem).m_occupancyData.m_statisticalTool.m_dP5);
		vLegendData[8].push_back((*statisticItem).m_occupancyData.m_statisticalTool.m_dP10);
		vLegendData[9].push_back((*statisticItem).m_occupancyData.m_statisticalTool.m_dP90);
		vLegendData[10].push_back((*statisticItem).m_occupancyData.m_statisticalTool.m_dP95);
		vLegendData[11].push_back((*statisticItem).m_occupancyData.m_statisticalTool.m_dP99);
		vLegendData[12].push_back((*statisticItem).m_occupancyData.m_statisticalTool.m_dSigma);
	}

	int nLegendCount = vLegendData.size();
	for (int nLegendData = 0; nLegendData < nLegendCount; ++ nLegendData)
	{
		c2dGraphData.m_vr2DChartData.push_back(vLegendData[nLegendData]);
	}

	c2dGraphData.m_vrLegend = vLegends;

	chartWnd.DrawChart(c2dGraphData);
}

void CTaxiwayUtilizationSummaryOccupancyChart::GenerateResult(std::vector<CTaxiwayUtilizationData*>& vResult,CParameters *pParameter)
{
	CAirsideTaxiwayUtilizationPara* pPara = (CAirsideTaxiwayUtilizationPara*)pParameter;
	CTaxiwayUtilizationData* pItem = pPara->GetCurrentSelect();

	if (pItem)
	{
		int nCount = (int)vResult.size();
		for (int i = 0; i < nCount; i++)
		{
			CTaxiwayUtilizationData* pData = vResult[i];
			if (pData->m_sTaxiway == pItem->m_sTaxiway && pData->m_sStartNode == pItem->m_sStartNode
				&& pData->m_sEndNode == pItem->m_sEndNode)
			{
				m_vResult.push_back(pData);
			}
		}
	}
	else
	{
		m_vResult = vResult;
	}
}