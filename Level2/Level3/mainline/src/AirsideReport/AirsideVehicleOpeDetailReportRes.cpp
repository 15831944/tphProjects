#include "StdAfx.h"
#include ".\airsidevehicleopedetailreportres.h"
#include "../Common/UnitsManager.h"
#include "../InputAirside/ARCUnitManager.h"
#include "AirsideVehicleOperaterReport.h"
CGTCtrlDataItem::~CGTCtrlDataItem()
{
	for (int i = 0 ;i < (int)m_IntervalDatas.size();i++)
	{
		delete m_IntervalDatas[i] ;
	}
	m_IntervalDatas.clear() ;
}
void CGTCtrlDataItem::InitIntervalTime(int _size,ElapsedTime _interval)
{
	ElapsedTime startTime ;
	CGTCtrlSubItem* subitem = NULL;
	for (int i = 0 ;i < _size;i++)
	{
		subitem = new CGTCtrlSubItem ;
		subitem->m_start = startTime ;
		subitem->m_end = subitem->m_start + _interval ;
		startTime = subitem->m_end ;
		m_IntervalDatas.push_back(subitem)  ;
	}
}
void CGTCtrlDataItem::AddVehicleCount(ElapsedTime _time)
{
	for (int i = 0 ;i < (int)m_IntervalDatas.size();i++)
	{
		if(m_IntervalDatas[i]->m_start <= _time && m_IntervalDatas[i]->m_end >= _time)
		{
			m_IntervalDatas[i]->m_numberOfvehicle++ ;
			break ;
		}
	}
}
CGTCtrlData::~CGTCtrlData()
{
	for (int i =0 ;i < (int)m_Data.size() ;i++)
	{
		delete m_Data[i] ;
	}
	m_Data.clear() ;
}
BOOL CGTCtrlData::FindByVehicleType(CString& _VehicleType,CGTCtrlDataItem** _PVehicleItem)
{
	for (int i =0 ;i < (int)m_Data.size() ;i++)
	{
		if(m_Data[i]->m_VehicleType.CompareNoCase(_VehicleType) == 0)
		{
			*_PVehicleItem = m_Data[i] ;
			return TRUE ;
		}
	}
	*_PVehicleItem = NULL ;
	return FALSE ;
}
CGTCtrlDataItem* CGTCtrlData::AddItemData(CString& _VehicleType)
{
	CGTCtrlDataItem* item = NULL ;
	if(FindByVehicleType(_VehicleType,&item))
		return NULL ;
	 item = new CGTCtrlDataItem ;
	item->m_VehicleType = _VehicleType ;
	m_Data.push_back(item) ;
	return item ;
}
CGTCtrlDataItem* CGTCtrlData::GetItemData(CString& _VehicleType)
{
	CGTCtrlDataItem* item = NULL ;
	FindByVehicleType(_VehicleType,&item) ;
	return item ;
}
void CGTCtrlData::InitInterval(int _size,ElapsedTime _interval)
{
	for (int i =0 ;i < (int)m_Data.size() ;i++)
	{
		m_Data[i]->InitIntervalTime(_size,_interval) ;
	}
}


void CAirsideVehicleOpeDetailReportRes::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{

	C2DChartData c2dGraphData;
	InitGtChartShowMessage(c2dGraphData,pParameter) ;
	ElapsedTime startTime ;
	ElapsedTime endtime ;
	CString Timeranger ;
	for (int i = 0 ; i < m_Intervals ;i++)
	{
		endtime = startTime + m_IntervalTime  ;
		Timeranger.Format(_T("%s-%s"),startTime.printTime(),endtime.printTime()) ;
		startTime = endtime ;
		c2dGraphData.m_vrXTickTitle.push_back(Timeranger) ;
	}
	std::vector<CGTCtrlDataItem*>::iterator iter = m_ChartResult.m_Data.begin();
	CGTCtrlDataItem* GTDataItem = NULL ;
	for (; iter!=m_ChartResult.m_Data.end(); iter++)
	{
		GTDataItem = *iter ;
		c2dGraphData.m_vrLegend.push_back(GTDataItem->m_VehicleType);

		int nSegmentCount = (int)GTDataItem->m_IntervalDatas.size();

		vector<double> vSegmentData;
		double count = 0 ; 
		for (int i=0; i<nSegmentCount; i++)
		{
			count = GTDataItem->m_IntervalDatas[i]->m_numberOfvehicle;
			vSegmentData.push_back(count);
		}

		c2dGraphData.m_vr2DChartData.push_back(vSegmentData);
	}
	chartWnd.DrawChart(c2dGraphData);
}
void CAirsideVehicleOpeDetailReportRes::InitGtChartShowMessage(C2DChartData& c2dGraphData, CParameters *pParameter)
{
	CString strLabel = _T("");
	c2dGraphData.m_strChartTitle.Format(_T(" Number of Vehicle vs %s "),m_TittleName)  ;
	c2dGraphData.m_strYtitle = _T("Vehicle Number");
	c2dGraphData.m_strXtitle = m_TittleName ;

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("%s %s,%s "),m_TittleName, pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
	bool bCommaFlag = true;
	c2dGraphData.m_strFooter = strFooter;
}
//////////////////////////////////////////////////////////////////////////
void CAirsideVehicleOpeDetailReportResForOnRoadTime::GenerateResult()
{
	CalculateTimeInterval() ;
	CGTCtrlDataItem* DataItem = NULL ;
	ElapsedTime time ;
	for (int i = 0 ; i < (int)m_result->size() ; i++)
	{
		DataItem =  m_ChartResult.GetItemData((*m_result)[i].m_VehicleType) ;
		if(DataItem != NULL)
		{
			time.setPrecisely((*m_result)[i].m_TotalOnRouteTime) ;
			DataItem->AddVehicleCount(time) ;
		}
	}
}

void CAirsideVehicleOpeDetailReportResForOnRoadTime::CalculateTimeInterval()
{
	long maxOnRoadTime = 0  ;
	std::vector<CVehicleOperaterDetailReportItem>::iterator iter = m_result->begin() ;
	for( ;iter != m_result->end() ;iter++)
	{
		maxOnRoadTime = MAX(maxOnRoadTime , (*iter).m_TotalOnRouteTime) ;
		m_ChartResult.AddItemData((*iter).m_VehicleType) ;
	}
	if(m_pParameter == NULL)
		return ;
	ElapsedTime interval ; 
	interval.setPrecisely( maxOnRoadTime/5) ;
	interval.set(0,interval.asMinutes(),0) ;
	if(interval.asMinutes() == 0)
		interval.SetMinute(1) ;
	m_Intervals = 5 ;
	while((long)interval * m_Intervals < maxOnRoadTime)
	{
		m_Intervals ++;
	}
	m_IntervalTime = interval ;
	m_ChartResult.InitInterval(m_Intervals,interval) ;
}

//////////////////////////////////////////////////////////////////////////
void CAirsideVehicleOpeDetailReportResForIdleTime::GenerateResult()
{
	CalculateTimeInterval() ;
	CGTCtrlDataItem* DataItem = NULL ;
	ElapsedTime time ;
	for (int i = 0 ; i < (int)m_result->size() ; i++)
	{
		DataItem =  m_ChartResult.GetItemData((*m_result)[i].m_VehicleType) ;
		if(DataItem != NULL)
		{
			time.setPrecisely( (*m_result)[i].m_TotalIdelTime ) ;
			DataItem->AddVehicleCount(time) ;
		}
	}
}

void CAirsideVehicleOpeDetailReportResForIdleTime::CalculateTimeInterval()
{
	long maxIdelTime = 0  ;
	std::vector<CVehicleOperaterDetailReportItem>::iterator iter = m_result->begin() ;
	for( ;iter != m_result->end() ;iter++)
	{
		maxIdelTime = MAX(maxIdelTime , (*iter).m_TotalIdelTime) ;
		m_ChartResult.AddItemData((*iter).m_VehicleType) ;
	}
	if(m_pParameter == NULL)
		return ;
	ElapsedTime interval  ;
	interval.setPrecisely( maxIdelTime/5) ;
	interval.set(0,interval.asMinutes(),0) ;
	if(interval.asMinutes() == 0)
		interval.set(0,1,0) ;
	m_Intervals = 5;
	while((long)interval * m_Intervals < maxIdelTime)
	{	
		m_Intervals ++ ;
	}
	m_IntervalTime = interval ;
	m_ChartResult.InitInterval(m_Intervals,m_IntervalTime) ;
}

//////////////////////////////////////////////////////////////////////////
void CAirsideVehicleOpeDetailReportResForServerTime::GenerateResult()
{
	CalculateTimeInterval() ;
	CGTCtrlDataItem* DataItem = NULL ;
	ElapsedTime time ;
	for (int i = 0 ; i < (int)m_result->size() ; i++)
	{
		DataItem =  m_ChartResult.GetItemData((*m_result)[i].m_VehicleType) ;
		if(DataItem != NULL)
		{
			time.setPrecisely((*m_result)[i].m_ServerTime ) ;
			DataItem->AddVehicleCount(time) ;
		}
	}
}
void CAirsideVehicleOpeDetailReportResForServerTime::CalculateTimeInterval()
{
	long maxServerTime = 0  ;
	std::vector<CVehicleOperaterDetailReportItem>::iterator iter = m_result->begin() ;
	for( ;iter != m_result->end() ;iter++)
	{
		maxServerTime = MAX(maxServerTime , (*iter).m_ServerTime) ;
		m_ChartResult.AddItemData((*iter).m_VehicleType) ;
	}
	if(m_pParameter == NULL)
		return ;
	ElapsedTime interval  ;
	interval.setPrecisely( maxServerTime/5) ;
	interval.set(0,interval.asMinutes(),0) ;
	if(interval.asMinutes() == 0)
		interval.set(0,1,0) ;
	m_Intervals = 5 ;
	while((long)interval * m_Intervals < maxServerTime)
	{
		m_Intervals++;
	}
	m_IntervalTime = interval ;
	m_ChartResult.InitInterval(m_Intervals,interval) ;
}
//////////////////////////////////////////////////////////////////////////
void CAirsideVehicleOpeDetailReportResForServerNumber::GenerateResult()
{
	CalculateTimeInterval() ;
	CGTCtrlDataItem* DataItem = NULL ;
	for (int i = 0 ; i < (int)m_result->size() ; i++)
	{
		DataItem =  m_ChartResult.GetItemData((*m_result)[i].m_VehicleType) ;
		if(DataItem != NULL)
			DataItem->AddVehicleCount(ElapsedTime((long)(*m_result)[i].m_ServerNumber)) ;
	}
}
void CAirsideVehicleOpeDetailReportResForServerNumber::CalculateTimeInterval()
{
	long maxServerNumber = 0  ;
	std::vector<CVehicleOperaterDetailReportItem>::iterator iter = m_result->begin() ;
	for( ;iter != m_result->end() ;iter++)
	{
		maxServerNumber = MAX(maxServerNumber , (*iter).m_ServerNumber) ;
		m_ChartResult.AddItemData((*iter).m_VehicleType) ;
	}
	long Interval = maxServerNumber / 6 ; // default interval number is 6
	if(Interval == 0 || Interval<= 1)
	{
		m_Intervals = 1 ;
		m_IntervalTime = ElapsedTime((long)6) ;
	}
	else
	{
		m_Intervals = 6 ;
		while(Interval * m_Intervals < maxServerNumber)
		{
			m_Intervals++ ;
		}
		m_IntervalTime = ElapsedTime(Interval) ;
	}
	m_ChartResult.InitInterval(m_Intervals,m_IntervalTime) ;
}
void CAirsideVehicleOpeDetailReportResForServerNumber::InitGtChartShowMessage(C2DChartData& c2dGraphData, CParameters *pParameter)
{
	CString strLabel = _T("");
	c2dGraphData.m_strChartTitle.Format(_T(" Number of Vehicle vs %s "),m_TittleName)  ;
	c2dGraphData.m_strYtitle = _T("Number");
	c2dGraphData.m_strXtitle = m_TittleName ;

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("%s %s,%s "),m_TittleName, pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
	bool bCommaFlag = true;
	c2dGraphData.m_strFooter = strFooter;
}
void CAirsideVehicleOpeDetailReportResForServerNumber::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{

	C2DChartData c2dGraphData;
	InitGtChartShowMessage(c2dGraphData,pParameter) ;
	ElapsedTime startTime ;
	ElapsedTime endtime ;
	CString Timeranger ;
	for (int i = 0 ; i < m_Intervals ;i++)
	{
		endtime = startTime + m_IntervalTime ;
		Timeranger.Format(_T("%d-%d"),startTime.asSeconds(),endtime.asSeconds()) ;
		startTime = endtime ;
		c2dGraphData.m_vrXTickTitle.push_back(Timeranger) ;
	}
	std::vector<CGTCtrlDataItem*>::iterator iter = m_ChartResult.m_Data.begin();
	CGTCtrlDataItem* GTDataItem = NULL ;
	for (; iter!=m_ChartResult.m_Data.end(); iter++)
	{
		GTDataItem = *iter ;
		c2dGraphData.m_vrLegend.push_back(GTDataItem->m_VehicleType);

		int nSegmentCount = (int)GTDataItem->m_IntervalDatas.size();

		vector<double> vSegmentData;
		double count = 0 ; 
		for (int i=0; i<nSegmentCount; i++)
		{
			count = GTDataItem->m_IntervalDatas[i]->m_numberOfvehicle;
			vSegmentData.push_back(count);
		}

		c2dGraphData.m_vr2DChartData.push_back(vSegmentData);
	}
	chartWnd.DrawChart(c2dGraphData);
}
//////////////////////////////////////////////////////////////////////////

void CAirsideVehicleOpeDetailReportResForPoolNumber::GenerateResult()
{
	CalculateTimeInterval() ;
	CGTCtrlDataItem* DataItem = NULL ;
	for (int i = 0 ; i < (int)m_result->size() ; i++)
	{
		DataItem =  m_ChartResult.GetItemData((*m_result)[i].m_VehicleType) ;
		if(DataItem != NULL)
			DataItem->AddVehicleCount(ElapsedTime((long)(*m_result)[i].m_NumberToPool)) ;
	}
}
void CAirsideVehicleOpeDetailReportResForPoolNumber::CalculateTimeInterval()
{
	long maxServerNumber = 0  ;
	std::vector<CVehicleOperaterDetailReportItem>::iterator iter = m_result->begin() ;
	for( ;iter != m_result->end() ;iter++)
	{
		maxServerNumber = MAX(maxServerNumber , (*iter).m_NumberToPool) ;
		m_ChartResult.AddItemData((*iter).m_VehicleType) ;
	}
	long Interval = maxServerNumber / 6 ; // default interval number is 6
	if(Interval == 0 || Interval<= 1)
	{
		m_Intervals = 1 ;
		m_IntervalTime = ElapsedTime((long)6) ;
	}
	else
	{
		m_Intervals = 6 ;
		while(Interval * m_Intervals < maxServerNumber)
		{
			m_Intervals++ ;
		}
		m_IntervalTime = ElapsedTime(Interval) ;
	}
	m_ChartResult.InitInterval(m_Intervals,m_IntervalTime) ;
}
void CAirsideVehicleOpeDetailReportResForPoolNumber::InitGtChartShowMessage(C2DChartData& c2dGraphData, CParameters *pParameter)
{
	CString strLabel = _T("");
	c2dGraphData.m_strChartTitle.Format(_T(" Number of Vehicle vs %s "),m_TittleName)  ;
	c2dGraphData.m_strYtitle = _T("Vehicle Number");
	c2dGraphData.m_strXtitle = m_TittleName ;

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("%s %s,%s "),m_TittleName, pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
	bool bCommaFlag = true;
	c2dGraphData.m_strFooter = strFooter;
}
void CAirsideVehicleOpeDetailReportResForPoolNumber::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{

	C2DChartData c2dGraphData;
	InitGtChartShowMessage(c2dGraphData,pParameter) ;
	ElapsedTime startTime ;
	ElapsedTime endtime ;
	CString Timeranger ;
	for (int i = 0 ; i < m_Intervals ;i++)
	{
		endtime = startTime +m_IntervalTime ;
		Timeranger.Format(_T("%d-%d"),startTime.asSeconds(),endtime.asSeconds()) ;
		startTime = endtime ;
		c2dGraphData.m_vrXTickTitle.push_back(Timeranger) ;
	}
	std::vector<CGTCtrlDataItem*>::iterator iter = m_ChartResult.m_Data.begin();
	CGTCtrlDataItem* GTDataItem = NULL ;
	for (; iter!=m_ChartResult.m_Data.end(); iter++)
	{
		GTDataItem = *iter ;
		c2dGraphData.m_vrLegend.push_back(GTDataItem->m_VehicleType);

		int nSegmentCount = (int)GTDataItem->m_IntervalDatas.size();

		vector<double> vSegmentData;
		double count = 0 ; 
		for (int i=0; i<nSegmentCount; i++)
		{
			count = GTDataItem->m_IntervalDatas[i]->m_numberOfvehicle;
			vSegmentData.push_back(count);
		}

		c2dGraphData.m_vr2DChartData.push_back(vSegmentData);
	}
	chartWnd.DrawChart(c2dGraphData);
}
//////////////////////////////////////////////////////////////////////////
void CAirsideVehicleOpeDetailReportResForTotalDistance::GenerateResult()
{
	CalculateTimeInterval() ;
	CGTCtrlDataItem* DataItem = NULL ;
	for (int i = 0 ; i < (int)m_result->size() ; i++)
	{
		DataItem =  m_ChartResult.GetItemData((*m_result)[i].m_VehicleType) ;
		if(DataItem != NULL)
			DataItem->AddVehicleCount(ElapsedTime((*m_result)[i].m_TotalDistance)) ;
	}
}
void CAirsideVehicleOpeDetailReportResForTotalDistance::CalculateTimeInterval()
{
	double maxServerNumber = 0  ;
	std::vector<CVehicleOperaterDetailReportItem>::iterator iter = m_result->begin() ;
	for( ;iter != m_result->end() ;iter++)
	{
		maxServerNumber = MAX(maxServerNumber , (*iter).m_TotalDistance) ;
		m_ChartResult.AddItemData((*iter).m_VehicleType) ;
	}
	double Interval = maxServerNumber / 6 ; // default interval number is 6
	if(Interval == 0 || Interval<= 1)
	{
		m_Intervals = 1 ;
		m_IntervalTime = ElapsedTime((long)6) ;
	}else
	{
		m_Intervals = 6 ;
		while( (double) ( (int)Interval * m_Intervals ) < maxServerNumber)
		{
			m_Intervals++ ;
		}
		m_IntervalTime = ElapsedTime((long)Interval) ;
	}
	m_ChartResult.InitInterval(m_Intervals,m_IntervalTime) ;
}
void CAirsideVehicleOpeDetailReportResForTotalDistance::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{

	C2DChartData c2dGraphData;
	InitGtChartShowMessage(c2dGraphData,pParameter) ;
	ElapsedTime startTime ;
	ElapsedTime endtime ;
	CString Timeranger ;
	double startDis = 0 ;
	double endDis = 0 ;
	for (int i = 0 ; i < m_Intervals ;i++)
	{
		endtime = startTime +m_IntervalTime ;

		startDis = CARCLengthUnit::ConvertLength(AU_LENGTH_CENTIMETER,CARCUnitManager::GetInstance().GetCurSelLengthUnit(),startTime.asSeconds() ) ;
		endDis = CARCLengthUnit::ConvertLength(AU_LENGTH_CENTIMETER,CARCUnitManager::GetInstance().GetCurSelLengthUnit(),endtime.asSeconds() ) ;

		Timeranger.Format(_T("%0.2f-%0.2f"),startDis,endDis) ;
		startTime = endtime ;
		c2dGraphData.m_vrXTickTitle.push_back(Timeranger) ;
	}
	std::vector<CGTCtrlDataItem*>::iterator iter = m_ChartResult.m_Data.begin();
	CGTCtrlDataItem* GTDataItem = NULL ;
	for (; iter!=m_ChartResult.m_Data.end(); iter++)
	{
		GTDataItem = *iter ;
		c2dGraphData.m_vrLegend.push_back(GTDataItem->m_VehicleType);

		int nSegmentCount = (int)GTDataItem->m_IntervalDatas.size();

		vector<double> vSegmentData;
		double count = 0 ; 
		for (int i=0; i<nSegmentCount; i++)
		{
			count = GTDataItem->m_IntervalDatas[i]->m_numberOfvehicle;
			vSegmentData.push_back(count);
		}

		c2dGraphData.m_vr2DChartData.push_back(vSegmentData);
	}
	chartWnd.DrawChart(c2dGraphData);
}
void CAirsideVehicleOpeDetailReportResForTotalDistance::InitGtChartShowMessage(C2DChartData& c2dGraphData, CParameters *pParameter)
{
	CString strLabel = _T("");
	c2dGraphData.m_strChartTitle.Format(_T(" Number of Vehicle vs %s "),m_TittleName)  ;
	c2dGraphData.m_strYtitle = _T("Vehicle Number");
	c2dGraphData.m_strXtitle = m_TittleName + CARCLengthUnit::GetLengthUnitString(CARCUnitManager::GetInstance().GetCurSelLengthUnit());

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("%s %s,%s "),m_TittleName, pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
	bool bCommaFlag = true;
	c2dGraphData.m_strFooter = strFooter;
}
//////////////////////////////////////////////////////////////////////////
void CAirsideVehicleOpeDetailReportResForFuelConsumed::GenerateResult()
{
	CalculateTimeInterval() ;
	CGTCtrlDataItem* DataItem = NULL ;
	for (int i = 0 ; i < (int)m_result->size() ; i++)
	{
		DataItem =  m_ChartResult.GetItemData((*m_result)[i].m_VehicleType) ;
		if(DataItem != NULL)
			DataItem->AddVehicleCount(ElapsedTime((*m_result)[i].m_FuelConsumed)) ;
	}
}
void CAirsideVehicleOpeDetailReportResForFuelConsumed::CalculateTimeInterval()
{
	long maxServerNumber = 0  ;
	std::vector<CVehicleOperaterDetailReportItem>::iterator iter = m_result->begin() ;
	for( ;iter != m_result->end() ;iter++)
	{
		maxServerNumber = MAX(maxServerNumber , (*iter).m_FuelConsumed) ;
		m_ChartResult.AddItemData((*iter).m_VehicleType) ;
	}
	long Interval = maxServerNumber / 6 ; // default interval number is 6
	if(Interval == 0 || Interval== 1)
	{
		m_Intervals = 1 ;
		m_IntervalTime = ElapsedTime((long)6) ;
	}else
		m_Intervals = 6 ;
	while(Interval * m_Intervals < maxServerNumber)
		{
			m_Intervals ++ ;
		}
		m_IntervalTime = ElapsedTime(Interval) ;
	m_ChartResult.InitInterval(m_Intervals,m_IntervalTime) ;
}
void CAirsideVehicleOpeDetailReportResForFuelConsumed::InitGtChartShowMessage(C2DChartData& c2dGraphData, CParameters *pParameter)
{
	CString strLabel = _T("");
	c2dGraphData.m_strChartTitle.Format(_T(" Number of Vehicle vs %s "),m_TittleName)  ;
	c2dGraphData.m_strYtitle = _T("Vehicle Number");
	c2dGraphData.m_strXtitle = m_TittleName +  _T("liter");

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("%s %s,%s "),m_TittleName, pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
	bool bCommaFlag = true;
	c2dGraphData.m_strFooter = strFooter;
}
void CAirsideVehicleOpeDetailReportResForFuelConsumed::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{

	C2DChartData c2dGraphData;
	InitGtChartShowMessage(c2dGraphData,pParameter) ;
	ElapsedTime startTime ;
	ElapsedTime endtime ;
	CString Timeranger ;
	for (int i = 0 ; i < m_Intervals ;i++)
	{
		endtime = startTime + 	m_IntervalTime ;
		Timeranger.Format(_T("%d-%d"),startTime.asSeconds(),endtime.asSeconds()) ;
		startTime = endtime ;
		c2dGraphData.m_vrXTickTitle.push_back(Timeranger) ;
	}
	std::vector<CGTCtrlDataItem*>::iterator iter = m_ChartResult.m_Data.begin();
	CGTCtrlDataItem* GTDataItem = NULL ;
	for (; iter!=m_ChartResult.m_Data.end(); iter++)
	{
		GTDataItem = *iter ;
		c2dGraphData.m_vrLegend.push_back(GTDataItem->m_VehicleType);

		int nSegmentCount = (int)GTDataItem->m_IntervalDatas.size();

		vector<double> vSegmentData;
		double count = 0 ; 
		for (int i=0; i<nSegmentCount; i++)
		{
			count = GTDataItem->m_IntervalDatas[i]->m_numberOfvehicle;
			vSegmentData.push_back(count);
		}

		c2dGraphData.m_vr2DChartData.push_back(vSegmentData);
	}
	chartWnd.DrawChart(c2dGraphData);
}

//////////////////////////////////////////////////////////////////////////
void CAirsideVehicleOpeDetailReportResForAvgSpeed::GenerateResult()
{
	CalculateTimeInterval() ;
	CGTCtrlDataItem* DataItem = NULL ;
	for (int i = 0 ; i < (int)m_result->size() ; i++)
	{
		DataItem =  m_ChartResult.GetItemData((*m_result)[i].m_VehicleType) ;
		if(DataItem != NULL)
			DataItem->AddVehicleCount(ElapsedTime((*m_result)[i].m_AvgSpeed*36/1000)) ;
	}
}
void CAirsideVehicleOpeDetailReportResForAvgSpeed::CalculateTimeInterval()
{
	double AvgSpeed = 0  ;
	std::vector<CVehicleOperaterDetailReportItem>::iterator iter = m_result->begin() ;
	for( ;iter != m_result->end() ;iter++)
	{
		AvgSpeed = MAX(AvgSpeed , (*iter).m_AvgSpeed) ;
		m_ChartResult.AddItemData((*iter).m_VehicleType) ;
	}
		AvgSpeed =AvgSpeed*36/1000 ;
	double Interval = AvgSpeed / 6 ; // default interval number is 6
	if(Interval == 0 || Interval<= 1)
	{
		m_Intervals = 1 ;
		m_IntervalTime = ElapsedTime((long)6) ;
	}else
	{
		m_Intervals = 6 ;
		while( (double) ( (int)Interval * m_Intervals ) < AvgSpeed)
		{
			m_Intervals++ ;
		}
		m_IntervalTime = ElapsedTime((long)Interval) ;
	}

	m_ChartResult.InitInterval(m_Intervals,m_IntervalTime) ;
}
void CAirsideVehicleOpeDetailReportResForAvgSpeed::InitGtChartShowMessage(C2DChartData& c2dGraphData, CParameters *pParameter)
{
	CString strLabel = _T("");
	c2dGraphData.m_strChartTitle.Format(_T(" Number of Vehicle vs %s "),m_TittleName)  ;
	c2dGraphData.m_strYtitle = _T("Vehicle Number");
	c2dGraphData.m_strXtitle = m_TittleName +  CARCVelocityUnit::GetVelocityUnitString(CARCUnitManager::GetInstance().GetCurSelVelocityUnit());

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("%s %s,%s "),m_TittleName, pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
	bool bCommaFlag = true;
	c2dGraphData.m_strFooter = strFooter;
}
void CAirsideVehicleOpeDetailReportResForAvgSpeed::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{

	C2DChartData c2dGraphData;
	InitGtChartShowMessage(c2dGraphData,pParameter) ;
	ElapsedTime startTime ;
	ElapsedTime endtime ;
	CString Timeranger ;
	for (int i = 0 ; i < m_Intervals ;i++)
	{
		endtime = startTime + 		m_IntervalTime  ;
		Timeranger.Format(_T("%0.0f-%0.0f"),CARCVelocityUnit::ConvertVelocity(AU_VELOCITY_KMPH,CARCUnitManager::GetInstance().GetCurSelVelocityUnit(),startTime.asSeconds()), \
			CARCVelocityUnit::ConvertVelocity(AU_VELOCITY_KMPH,CARCUnitManager::GetInstance().GetCurSelVelocityUnit(),endtime.asSeconds())) ;
		startTime = endtime ;
		c2dGraphData.m_vrXTickTitle.push_back(Timeranger) ;
	}
	std::vector<CGTCtrlDataItem*>::iterator iter = m_ChartResult.m_Data.begin();
	CGTCtrlDataItem* GTDataItem = NULL ;
	for (; iter!=m_ChartResult.m_Data.end(); iter++)
	{
		GTDataItem = *iter ;
		c2dGraphData.m_vrLegend.push_back(GTDataItem->m_VehicleType);

		int nSegmentCount = (int)GTDataItem->m_IntervalDatas.size();

		vector<double> vSegmentData;
		double count = 0 ; 
		for (int i=0; i<nSegmentCount; i++)
		{
			count = GTDataItem->m_IntervalDatas[i]->m_numberOfvehicle;
			vSegmentData.push_back(count);
		}

		c2dGraphData.m_vr2DChartData.push_back(vSegmentData);
	}
	chartWnd.DrawChart(c2dGraphData);
}
//////////////////////////////////////////////////////////////////////////
void CAirsideVehicleOpeDetailReportResForMaxSpeed::GenerateResult()
{
	CalculateTimeInterval() ;
	CGTCtrlDataItem* DataItem = NULL ;
	for (int i = 0 ; i < (int)m_result->size() ; i++)
	{
		DataItem =  m_ChartResult.GetItemData((*m_result)[i].m_VehicleType) ;
		if(DataItem != NULL)
			DataItem->AddVehicleCount(ElapsedTime((*m_result)[i].m_MaxSpeed*36/1000)) ;
	}
}
void CAirsideVehicleOpeDetailReportResForMaxSpeed::CalculateTimeInterval()
{
	double AvgSpeed = 0  ;
	std::vector<CVehicleOperaterDetailReportItem>::iterator iter = m_result->begin() ;
	for( ;iter != m_result->end() ;iter++)
	{
		AvgSpeed = MAX(AvgSpeed , (*iter).m_MaxSpeed) ;
		m_ChartResult.AddItemData((*iter).m_VehicleType) ;
	}
	AvgSpeed = AvgSpeed*36/1000 ;
	double Interval = AvgSpeed / 6 ; // default interval number is 6
	if(Interval == 0 || Interval<= 1)
	{
		m_Intervals = 1 ;
		m_IntervalTime = ElapsedTime((long)6) ;
	}else
	{
		m_Intervals = 6 ;
		while( (double) ( (int)Interval * m_Intervals ) < AvgSpeed)
		{
          m_Intervals++ ;
		}
		m_IntervalTime = ElapsedTime(Interval) ;
	}

	m_ChartResult.InitInterval(m_Intervals,m_IntervalTime) ;
}
void CAirsideVehicleOpeDetailReportResForServerPerForm::InitGtChartShowMessage(C2DChartData& c2dGraphData, CParameters *pParameter)
{
	CString strLabel = _T("");
	c2dGraphData.m_strChartTitle.Format(_T("%s"),m_TittleName)  ;
	c2dGraphData.m_strYtitle = _T("Number of Services");
	c2dGraphData.m_strXtitle = m_TittleName ;

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("%s %s,%s "),m_TittleName, pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
	bool bCommaFlag = true;
	c2dGraphData.m_strFooter = strFooter;
}
void CAirsideVehicleOpeDetailReportResForServerPerForm::GenerateResult()
{
	CalculateTimeInterval() ;
	CVehicleOperaterDetailReportItem reportItem ;
	CGTCtrlDataItem* DataItem = NULL ;
	ElapsedTime time ;
	std::vector<CVehicleOperaterDetailReportItem>::const_iterator iter = m_result->begin() ;
	for (; iter != m_result->end() ;iter++)
	{
		reportItem = *iter ;
		DataItem =  m_ChartResult.GetItemData(reportItem.m_VehicleType) ;
		for (int j = 0 ; j <(int)reportItem.m_ServerSTimes.size();j++ )
		{
			time.setPrecisely(reportItem.m_ServerSTimes[j]) ;
			DataItem->AddVehicleCount(time) ;
		}	
	}
}
void CAirsideVehicleOpeDetailReportResForServerPerForm::CalculateTimeInterval()
{
	std::vector<CVehicleOperaterDetailReportItem>::iterator iter = m_result->begin() ;
	for( ;iter != m_result->end() ;iter++)
	{
		m_ChartResult.AddItemData((*iter).m_VehicleType) ;
	}
	if(m_pParameter == NULL)
		return ;
	ElapsedTime totalTime ;
	totalTime = m_pParameter->getEndTime() - m_pParameter->getStartTime() ;
	ElapsedTime time ;
	time.SetSecond( totalTime.asSeconds() / 6 );

	time.set(0,time.asMinutes(),0) ;
	if(time.asMinutes() == 0)
		time.set(0,1,0) ;

	m_Intervals = 6 ;
	while( (m_Intervals * time.asSeconds()) < m_pParameter->getEndTime().asSeconds() )
	{
		m_Intervals++ ;
	}
	m_IntervalTime = time ;
	m_ChartResult.InitInterval(m_Intervals,m_IntervalTime) ;
};
void CAirsideVehicleOpeDetailReportResForServerPerForm::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter) 
{

	C2DChartData c2dGraphData;
	InitGtChartShowMessage(c2dGraphData,pParameter) ;
	CalculateTimeInterval() ;
	ElapsedTime startTime ;
	ElapsedTime endtime ;
	CString Timeranger ;
	for (int i = 0 ; i < m_Intervals ;i++)
	{
		endtime = startTime + 		m_IntervalTime  ;
		Timeranger.Format(_T("%s-%s"),startTime.printTime(),endtime.printTime()) ;
		startTime = endtime ;
		c2dGraphData.m_vrXTickTitle.push_back(Timeranger) ;
	}
	CGTCtrlDataItem* GTDataItem = NULL ;
	std::vector<CGTCtrlDataItem*>::iterator iter = m_ChartResult.m_Data.begin();
	for (; iter!=m_ChartResult.m_Data.end(); iter++)
	{
		GTDataItem = *iter ;
		c2dGraphData.m_vrLegend.push_back(GTDataItem->m_VehicleType);

		int nSegmentCount = (int)GTDataItem->m_IntervalDatas.size();

		vector<double> vSegmentData;
		double count = 0 ; 
		for (int i=0; i<nSegmentCount; i++)
		{
			count = GTDataItem->m_IntervalDatas[i]->m_numberOfvehicle;
			vSegmentData.push_back(count);
		}
		c2dGraphData.m_vr2DChartData.push_back(vSegmentData);
	}
chartWnd.DrawChart(c2dGraphData);
}