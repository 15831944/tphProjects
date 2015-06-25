#include "StdAfx.h"
#include ".\airsiderunwayutilizationrepordetailresult.h"
#include "AirsideRunwayUtilizationReport.h"


using namespace AirsideRunwayUtilizationReport;

AirsideRunwayUtilizationReport::DetailFlightRunwayResult::~DetailFlightRunwayResult()
{

}

CDetailResult::CDetailResult(void)
{
}

CDetailResult::~CDetailResult(void)
{

}

void AirsideRunwayUtilizationReport::CDetailResult::InitListHead( CXListCtrl& cxListCtrl, CSortableHeaderCtrl* piSHC )
{

	cxListCtrl.DeleteAllItems();

	while(cxListCtrl.GetHeaderCtrl()->GetItemCount() >0)
		cxListCtrl.DeleteColumn(0);

	int nColCount = 14;
	CString strHeader[] = 
	{
		_T("Airline"),
		_T("ID"),
		_T("Flight Type"),
		_T("Runway"),
		_T("Operation"),
		_T("Air Route"),
		_T("T Start"),
		_T("T End"),
		_T("Occupancy actual(sec)"),
		_T("Occupancy min(sec)"),
		_T("Enter @"),
		_T("Exit @"),
		_T("Possible Exit @"),
		_T("T wait @ entry")
	};
	int nHeaderLen[] = 
	{
		60,
		60,
		60,
		60,
		60,
		80,
		80,
		80,
		80,
		80,
		80,
		80,
		80,
		80
	};

	if (piSHC)
		piSHC->ResetAll();

	for (int nCol = 0 ; nCol < nColCount; ++nCol)
	{
		cxListCtrl.InsertColumn(nCol,strHeader[nCol],LVCFMT_LEFT, nHeaderLen[nCol]);
		if (piSHC)
		{
			switch (nCol)
			{
			case 8: case 9: case 13:
				piSHC->SetDataType(nCol,dtINT);
				break;
			default:
				piSHC->SetDataType(nCol,dtSTRING);
				break;
			}
		}
	}
}

void AirsideRunwayUtilizationReport::CDetailResult::FillListContent( CXListCtrl& cxListCtrl, CParameters * parameter )
{
	int nItemCount = (int)m_vDetailResult.size();
	for (int nItem = 0; nItem < nItemCount; ++nItem)
	{
		DetailFlightRunwayResult *pItem = m_vDetailResult[nItem];
		if(pItem == NULL)
			continue;

		//insert the item to list
		cxListCtrl.InsertItem(nItem,pItem->m_strAirline);
		cxListCtrl.SetItemText(nItem,1,pItem->m_strFlightID);
		cxListCtrl.SetItemText(nItem,2,pItem->m_strFlightType);
		cxListCtrl.SetItemText(nItem,3,pItem->m_strMarkName);
		if(pItem->m_enumRunwayOperation == CAirsideRunwayUtilizationReportParam::Landing)
            cxListCtrl.SetItemText(nItem,4,_T("Landing"));
		else
			cxListCtrl.SetItemText(nItem,4,_T("Take off"));

		cxListCtrl.SetItemText(nItem,5,pItem->m_strAirRoute);
		cxListCtrl.SetItemText(nItem,6,pItem->m_eTimeStart.PrintDateTime());
		cxListCtrl.SetItemText(nItem,7,pItem->m_eTimeEnd.PrintDateTime());

		CString strOccupyTime ;
		strOccupyTime.Format(_T("%d"),pItem->m_eTimeOccupancy.asSeconds());
		cxListCtrl.SetItemText(nItem,8,strOccupyTime);

		if(pItem->m_enumRunwayOperation == CAirsideRunwayUtilizationReportParam::Landing)
		{
			CString strOccupyMinTime ;
			strOccupyMinTime.Format(_T("%d"),pItem->m_eMinimumTime.asSeconds());
			cxListCtrl.SetItemText(nItem,9,strOccupyMinTime);
		}


		cxListCtrl.SetItemText(nItem,10,pItem->m_strEnterObject);
		cxListCtrl.SetItemText(nItem,11,pItem->m_strExitObject);
		if(pItem->m_enumRunwayOperation == CAirsideRunwayUtilizationReportParam::Landing)
			cxListCtrl.SetItemText(nItem,12,pItem->m_strPossibleExit);

		if(pItem->m_enumRunwayOperation == CAirsideRunwayUtilizationReportParam::TakeOff)
		{
			CString strWaitTime ;
			strWaitTime.Format(_T("%d"),pItem->m_eTimeWait.asSeconds());
			cxListCtrl.SetItemText(nItem,13,strWaitTime);
		}
	}
}

void AirsideRunwayUtilizationReport::CDetailResult::RefreshReport( CParameters * parameter )
{
	CAirsideRunwayUtilizationReportParam *pParam = (CAirsideRunwayUtilizationReportParam *)parameter;
	if(pParam  == NULL)
		return;

	if(m_pChartResult != NULL)
		delete m_pChartResult;
	m_pChartResult = NULL;
	if(pParam->getSubType() == -1 )
		pParam->setSubType(CAirsideRunwayUtilizationReport::ChartType_Detail_RunwayOccupancy);

	if(pParam->getSubType() == CAirsideRunwayUtilizationReport::ChartType_Detail_RunwayOccupancy)
	{
		CDetailRunwayOccupancyChart *pChart = new CDetailRunwayOccupancyChart;
		m_pChartResult  = pChart;
		pChart->GenerateResult(m_vDetailResult,m_vRunwayParamMark);
				
	}
	else if(pParam->getSubType() == CAirsideRunwayUtilizationReport::ChartType_Detail_RunwayUtilization)
	{
		CDetailRunwayUtilizationChart *pChart = new CDetailRunwayUtilizationChart;
		m_pChartResult  = pChart;
		pChart->GenerateResult(m_vDetailResult,m_bParameterDefault);
	}
}
BOOL AirsideRunwayUtilizationReport::CDetailResult::WriteReportData( ArctermFile& _file )
{
	//write summary or detail
	//title
	_file.writeField("Runway Utilization Detail Report");
	_file.writeLine();
	//columns
	_file.writeField(_T("Columns"));
	_file.writeLine();

	_file.writeInt((int)ASReportType_Detail);
	_file.writeLine();
	
	//write parameter marks
	_file.writeInt(m_bParameterDefault?1:0);
	int nMarkCount = (int)m_vRunwayParamMark.size();
	_file.writeInt(nMarkCount);
	for (int nMark = 0; nMark < nMarkCount; ++nMark)
	{
		CAirsideReportRunwayMark& runwayMark = m_vRunwayParamMark[nMark];
		_file.writeInt(runwayMark.m_nRunwayID);
		_file.writeInt((int)runwayMark.m_enumRunwayMark);
		_file.writeField(runwayMark.m_strMarkName);
	}
	_file.writeLine();

	//write report data one by one
	int nResultCount = (int)m_vDetailResult.size();
	_file.writeInt(nResultCount);
	for (int nResult = 0; nResult < nResultCount; ++nResult)
	{
		 DetailFlightRunwayResult *pResult = m_vDetailResult[nResult];
		 if (pResult == NULL)
			continue;
		 pResult->WriteReportData(_file);
		 _file.writeLine();
	}

	return TRUE;
}
BOOL AirsideRunwayUtilizationReport::CDetailResult::ReadReportData( ArctermFile& _file )
{

	//default parameter or not
	int nParameterDefault = 0;
	_file.getInteger(nParameterDefault);
	if(nParameterDefault > 0)
		m_bParameterDefault = true;
	else
		m_bParameterDefault = false;
	//read parameter marks
	int nMarkCount = 0;
	_file.getInteger(nMarkCount);
	for (int nMark = 0; nMark < nMarkCount; ++nMark)
	{
		CAirsideReportRunwayMark runwayMark;
		_file.getInteger(runwayMark.m_nRunwayID);
		_file.getInteger((int&)runwayMark.m_enumRunwayMark);
		_file.getField(runwayMark.m_strMarkName.GetBuffer(1024),1024);
		m_vRunwayParamMark.push_back(runwayMark);
	}
	_file.getLine();

	//write report data one by one
	int nResultCount = 0;
	_file.getInteger(nResultCount);
	for (int nResult = 0; nResult < nResultCount; ++nResult)
	{
		 DetailFlightRunwayResult *pResult = new DetailFlightRunwayResult;
		 if (pResult == NULL)
			continue;
		 pResult->ReadReportData(_file);
		 m_vDetailResult.push_back(pResult);
		 _file.getLine();
	}


	return TRUE;
}
void AirsideRunwayUtilizationReport::CDetailResult::Draw3DChart()
{

}


//////////////////////////////////////////////////////////////////////////
//AirsideRunwayUtilizationReport
AirsideRunwayUtilizationReport::CDetailRunwayOccupancyChart::CDetailRunwayOccupancyChart()
{

}

AirsideRunwayUtilizationReport::CDetailRunwayOccupancyChart::~CDetailRunwayOccupancyChart()
{

}
void AirsideRunwayUtilizationReport::CDetailRunwayOccupancyChart::GenerateResult( std::vector<DetailFlightRunwayResult *>& vDetailResult,std::vector<CAirsideReportRunwayMark>& vRunwayParamMark )
{
	InitResult(vRunwayParamMark);
	int nRunwayOpCount = (int)vDetailResult.size();
	for (int nRunwayOp = 0; nRunwayOp < nRunwayOpCount; ++nRunwayOp )
	{
		DetailFlightRunwayResult *pRunwayOperation = vDetailResult[nRunwayOp];

		if(pRunwayOperation == NULL)
			continue;

		CAirsideReportRunwayMark runwayMark;
		runwayMark.SetRunway(pRunwayOperation->m_nRunwayID,pRunwayOperation->m_enumRunwayMark,pRunwayOperation->m_strMarkName);

		CDetailRunwayOccupancyItem *pRunwayOcc = GetRunwayResult(runwayMark);
		if(pRunwayOcc == NULL)
			continue;

		if(pRunwayOperation->m_enumRunwayOperation == CAirsideRunwayUtilizationReportParam::Landing)
			pRunwayOcc->m_vLanding.push_back(pRunwayOperation);
		else if(pRunwayOperation->m_enumRunwayOperation == CAirsideRunwayUtilizationReportParam::TakeOff)
			pRunwayOcc->m_vTakeOff.push_back(pRunwayOperation);

	}

}
void AirsideRunwayUtilizationReport::CDetailRunwayOccupancyChart::Draw3DChart( CARC3DChart& chartWnd, CParameters *pParameter )
{
	ElapsedTime eTimeInterval(10L);//interval is 10 seconds

	//min, max occupancy time
	ElapsedTime eMinTime;
	ElapsedTime eMaxTime;

	GetMinMaxOccupancyTime(eMinTime,eMaxTime);
	//
	eMinTime = ElapsedTime((eMinTime.asSeconds() - eMinTime.asSeconds()%eTimeInterval.asSeconds() )* 1L);


	CString strLabel = _T("");
	C2DChartData c2dGraphData;

	c2dGraphData.m_strChartTitle = _T(" Runway Occupancy Report");


	c2dGraphData.m_strYtitle = _T("Number of Aircrafts");
	c2dGraphData.m_strXtitle = _T("Occupancy(seconds)");

	//get all legends
	std::vector<CString> vLegends; 
	int nRunwayOccCount = (int)m_vRunwayOccu.size();
	for (int nRunwayOcc = 0; nRunwayOcc < nRunwayOccCount; ++nRunwayOcc)
	{
		 CDetailRunwayOccupancyItem * pItem = m_vRunwayOccu[nRunwayOcc];
		 if(pItem == NULL)
			 continue;
		 CString strMark = pItem->runwayMark.m_strMarkName;
		 vLegends.push_back(_T("Total Runway ") + strMark);
		 vLegends.push_back(_T("Landings Runway ") + strMark);
		 vLegends.push_back(_T("TakeOffs Runway ") + strMark);

	}
	c2dGraphData.m_vrLegend = vLegends;


	c2dGraphData.m_vr2DChartData.resize(vLegends.size()) ;

	//x tick, runway
	for (; eMinTime < eMaxTime ; eMinTime += eTimeInterval)
	{

		CString strTick;
		strTick.Format(_T("%d - %d"),eMinTime.asSeconds(),(eMinTime + eTimeInterval).asSeconds());
		c2dGraphData.m_vrXTickTitle.push_back(strTick);

		//get the chart data
		int nLegend = 0;
		int nRunwayCount = (int)m_vRunwayOccu.size();
		for (int nRunway = 0; nRunway < nRunwayCount; ++nRunway)
		{
			CDetailRunwayOccupancyItem * pItem = m_vRunwayOccu[nRunway];
			if(pItem == NULL)
				continue;
			int nLandingCount = 0;
			int nTakeOffCount = 0;
			pItem->GetOperationCount(eMinTime,eMinTime + eTimeInterval,nLandingCount,nTakeOffCount);
			c2dGraphData.m_vr2DChartData[nLegend].push_back(nLandingCount + nTakeOffCount);
			nLegend += 1;

			c2dGraphData.m_vr2DChartData[nLegend].push_back(nLandingCount);			
			nLegend += 1;

			c2dGraphData.m_vr2DChartData[nLegend].push_back(nTakeOffCount);
			nLegend += 1;
		}
	}

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("%s %s,%s "), c2dGraphData.m_strChartTitle,
		pParameter->getStartTime().printTime(), 
		pParameter->getEndTime().printTime());


	bool bCommaFlag = true;
	CAirsideRunwayUtilizationReportParam* pParam = (CAirsideRunwayUtilizationReportParam*)pParameter;
	//for (int i=0; i<(int)pParam->getFlightConstraintCount(); i++)
	//{
	//	FlightConstraint fltCons = pParam->getFlightConstraint(i);

	//	CString strFlight(_T(""));
	//	fltCons.screenPrint(strFlight.GetBuffer(1024));

	//	if (bCommaFlag)
	//	{
	//		bCommaFlag = false;
	//		strFooter.AppendFormat("%s", strFlight);
	//	}
	//	else
	//	{
	//		strFooter.AppendFormat(",%s", strFlight);
	//	}
	//}
	c2dGraphData.m_strFooter = strFooter;

	chartWnd.DrawChart(c2dGraphData);



}

void AirsideRunwayUtilizationReport::CDetailRunwayOccupancyChart::InitResult( std::vector<CAirsideReportRunwayMark>& vRunwayParamMark )
{
	int nRunwayParamCount = (int)vRunwayParamMark.size();
	for (int nRunway = 0; nRunway < nRunwayParamCount; ++nRunway)
	{
		CDetailRunwayOccupancyItem *pItem = new CDetailRunwayOccupancyItem;
		pItem->runwayMark = vRunwayParamMark[nRunway];
		m_vRunwayOccu.push_back(pItem);
	}
}

CDetailRunwayOccupancyItem * AirsideRunwayUtilizationReport::CDetailRunwayOccupancyChart::GetRunwayResult( const CAirsideReportRunwayMark& runwayMark )
{
	int nResultCount = (int)m_vRunwayOccu.size();
	for (int nResult = 0; nResult < nResultCount; ++nResult)
	{
		if(m_vRunwayOccu[nResult] &&
			m_vRunwayOccu[nResult]->runwayMark.m_nRunwayID == runwayMark.m_nRunwayID &&
			runwayMark.m_enumRunwayMark == m_vRunwayOccu[nResult]->runwayMark.m_enumRunwayMark)
			return m_vRunwayOccu[nResult];
	}
	return NULL;
}

void AirsideRunwayUtilizationReport::CDetailRunwayOccupancyChart::GetMinMaxOccupancyTime(  ElapsedTime& eMinTime, ElapsedTime& eMaxTime )
{
	eMinTime = ElapsedTime(24*60*60L);
	eMaxTime = ElapsedTime(0L);
	int nRunwayOpCount = (int)m_vRunwayOccu.size();
	for (int nRunwayOp = 0; nRunwayOp < nRunwayOpCount; ++nRunwayOp )
	{
		CDetailRunwayOccupancyItem *pRunwayOccItem = m_vRunwayOccu[nRunwayOp];

		if(pRunwayOccItem == NULL)
			continue;



		//landing
		std::vector<DetailFlightRunwayResult *>::iterator iter = pRunwayOccItem->m_vLanding.begin();

		for (; iter !=  pRunwayOccItem->m_vLanding.end(); ++iter)
		{	
			if(eMaxTime < (*iter)->m_eTimeOccupancy)
			eMaxTime = (*iter)->m_eTimeOccupancy;

			if(eMinTime > (*iter)->m_eTimeOccupancy)
				eMinTime = (*iter)->m_eTimeOccupancy;
			
		}
		//take off
		iter = pRunwayOccItem->m_vTakeOff.begin();


		for (; iter != pRunwayOccItem->m_vTakeOff.end(); ++iter)
		{
			if(eMaxTime < (*iter)->m_eTimeOccupancy)
				eMaxTime = (*iter)->m_eTimeOccupancy;

			if(eMinTime > (*iter)->m_eTimeOccupancy)
				eMinTime = (*iter)->m_eTimeOccupancy;
		}
	

	}
}


void AirsideRunwayUtilizationReport::CDetailRunwayOccupancyItem::GetOperationCount( ElapsedTime eOccTimeFrom, ElapsedTime eOccTimeTo, int& nLandingCount, int& nTakeOffCount )
{
	nLandingCount = 0;
	nTakeOffCount = 0;
	//landing
	std::vector<DetailFlightRunwayResult *>::iterator iter = m_vLanding.begin();

	for (; iter != m_vLanding.end(); ++iter)
	{
		if((*iter)->m_eTimeOccupancy >= eOccTimeFrom &&(*iter)->m_eTimeOccupancy < eOccTimeTo)
		{
			nLandingCount += 1;
		}
	}
	//take off
	iter = m_vTakeOff.begin();


	for (; iter != m_vTakeOff.end(); ++iter)
	{
		if((*iter)->m_eTimeOccupancy >= eOccTimeFrom &&(*iter)->m_eTimeOccupancy  < eOccTimeTo)
		{
			nTakeOffCount += 1;
		}
	}


}






















AirsideRunwayUtilizationReport::CDetailRunwayUtilizationChart::CDetailRunwayUtilizationChart()
{
	m_pvDetailResult = NULL;
	m_bParameterIsDefault = true;
}

AirsideRunwayUtilizationReport::CDetailRunwayUtilizationChart::~CDetailRunwayUtilizationChart()
{

}

void AirsideRunwayUtilizationReport::CDetailRunwayUtilizationChart::GenerateResult( std::vector<DetailFlightRunwayResult *>& vDetailResult,bool bParameterIsDefault )
{
	m_pvDetailResult =& vDetailResult;
	m_bParameterIsDefault = bParameterIsDefault;
}

void AirsideRunwayUtilizationReport::CDetailRunwayUtilizationChart::Draw3DChart( CARC3DChart& chartWnd, CParameters *pParameter )
{
	if(pParameter == NULL)
		return;

	ElapsedTime eTimeInterval = ElapsedTime(pParameter->getInterval());//interval is 10 seconds

	//min, max occupancy time
	ElapsedTime eStartTime = pParameter->getStartTime();
	ElapsedTime eEndTime = pParameter->getEndTime();



	CString strLabel = _T("");
	C2DChartData c2dGraphData;

	c2dGraphData.m_strChartTitle = _T(" Runway Utilization Report");
	c2dGraphData.m_strYtitle = _T("Time(min)");
	c2dGraphData.m_strXtitle = _T("Time of day");
	if(m_bParameterIsDefault)
		c2dGraphData.m_strXtitle += _T("(Based on partial data only)");


	//get all legends
	std::vector<CString> vLegends; 
	vLegends.push_back(_T("Occupied"));
	vLegends.push_back(_T("Idle"));
	c2dGraphData.m_vrLegend = vLegends;


	c2dGraphData.m_vr2DChartData.resize(vLegends.size()) ;


	int nActiveRunwayCount = GetActiveRunwayCount();
	ElapsedTime nTotalIntTime = ElapsedTime( nActiveRunwayCount * eTimeInterval.asSeconds() * 1L);
	//x tick, runway
	for (;eStartTime < eEndTime + eTimeInterval; eStartTime += eTimeInterval)
	{

		CString strTick;
		strTick.Format(_T("%s - %s"),eStartTime.PrintDateTime(),(eStartTime + eTimeInterval).PrintDateTime());
		c2dGraphData.m_vrXTickTitle.push_back(strTick);

		//get the chart data
		ElapsedTime eTimeOccupied = ElapsedTime(0L);
		GetOccupyTimeBetween(eStartTime,eStartTime + eTimeInterval, eTimeOccupied);

		c2dGraphData.m_vr2DChartData[0].push_back(eTimeOccupied.asMinutes());
		
		c2dGraphData.m_vr2DChartData[1].push_back((nTotalIntTime  - eTimeOccupied).asMinutes());

	}

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("%s %s,%s "), c2dGraphData.m_strChartTitle,
		pParameter->getStartTime().printTime(), 
		pParameter->getEndTime().printTime());


	bool bCommaFlag = true;
	CAirsideRunwayUtilizationReportParam* pParam = (CAirsideRunwayUtilizationReportParam*)pParameter;
	//for (int i=0; i<(int)pParam->getFlightConstraintCount(); i++)
	//{
	//	FlightConstraint fltCons = pParam->getFlightConstraint(i);

	//	CString strFlight(_T(""));
	//	fltCons.screenPrint(strFlight.GetBuffer(1024));

	//	if (bCommaFlag)
	//	{
	//		bCommaFlag = false;
	//		strFooter.AppendFormat("%s", strFlight);
	//	}
	//	else
	//	{
	//		strFooter.AppendFormat(",%s", strFlight);
	//	}
	//}
	c2dGraphData.m_strFooter = strFooter;


	//std::vector<CString> vLegends;
	////get the legend
	//int nLegendCount = m_vRunwayOperationDetail->at(0)->m_vWakeVortexDetailValue.size();
	//for (int nLegend = 0; nLegend < nLegendCount; ++nLegend)
	//{
	//	CString strLegend;
	//	strLegend.Format(_T("%s-%s"),
	//		m_vRunwayOperationDetail->at(0)->m_vWakeVortexDetailValue[nLegend].m_strClassLeadName,
	//		m_vRunwayOperationDetail->at(0)->m_vWakeVortexDetailValue[nLegend].m_strClassTrailName);
	//	vLegends.push_back(strLegend);

	//}


	chartWnd.DrawChart(c2dGraphData);
}

void AirsideRunwayUtilizationReport::CDetailRunwayUtilizationChart::GetOccupyTimeBetween( ElapsedTime eTimeStart, ElapsedTime eTimeEnd, ElapsedTime& eTimeOccupied )
{
	if(m_pvDetailResult == NULL)
		return;

	//std::vector<DetailFlightRunwayResult *> *m_pvDetailResult;

	int nItemCount = (int)m_pvDetailResult->size();
	for (int nItem = 0; nItem < nItemCount; ++nItem)
	{
		
		DetailFlightRunwayResult *pItem = (*m_pvDetailResult)[nItem];
		if(pItem == NULL)
			continue;

		if(pItem->m_eTimeStart >= eTimeStart && pItem->m_eTimeStart < eTimeEnd)
			eTimeOccupied += pItem->m_eTimeOccupancy;
	}

}

//get active runway count
int AirsideRunwayUtilizationReport::CDetailRunwayUtilizationChart::GetActiveRunwayCount()
{
	if(m_pvDetailResult == NULL)
		return 1;

	std::vector<CAirsideReportRunwayMark > vRunwayMark; 

	//for each item
	int nItemCount = (int)m_pvDetailResult->size();
	for (int nItem = 0; nItem < nItemCount; ++nItem)
	{
		
		DetailFlightRunwayResult *pItem = (*m_pvDetailResult)[nItem];
		if(pItem == NULL)
			continue;

		//check exists ot not
		bool bFind = false;
		int nRunwayCount = static_cast<int>(vRunwayMark.size());
		for (int nRunway = 0; nRunway < nRunwayCount; ++nRunway)
		{
			if(vRunwayMark[nRunway].m_nRunwayID == pItem->m_nRunwayID &&
				vRunwayMark[nRunway].m_enumRunwayMark == pItem->m_enumRunwayMark)
			{
				bFind = true;
				break;
			}
		}

		if(bFind)//no find
			continue;
		
		//new runway mark
		CAirsideReportRunwayMark runwayMark;
		runwayMark.m_nRunwayID = pItem->m_nRunwayID;
		runwayMark.m_enumRunwayMark = pItem->m_enumRunwayMark;
		vRunwayMark.push_back(runwayMark);
	}

	return static_cast<int>(vRunwayMark.size());

}



































