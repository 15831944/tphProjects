#include "StdAfx.h"
#include ".\lsthroughputsummaryresult.h"
#include "LandsideBaseParam.h"
#include "..\MFCExControl\SortableHeaderCtrl.h"
#include "..\MFCExControl\XListCtrl.h"
#include "..\Results\LandsideVehicleDelayLog.h"
#include "..\Common\ProgressBar.h"
#include "..\Landside\InputLandside.h"
#include "..\Common\Point2008.h"
#include "..\Common\Line2008.h"
#include "LSVehicleThroughputParam.h"
#include "AirsideReport/CARC3DChart.h"

LSThroughputSummaryResult::LSThroughputSummaryResult(void)
{
}

LSThroughputSummaryResult::~LSThroughputSummaryResult(void)
{
}

void LSThroughputSummaryResult::Draw3DChart( CARC3DChart& chartWnd, LandsideBaseParam *pParameter )
{
	if (pParameter == NULL)
		return;

	switch (pParameter->getSubType())
	{
	case CT_VEHICLEBYGROUP:
		DrawVehicleByGroupChart(chartWnd,pParameter);
		break;
	case CT_VEHICLEPERRES:
		DrawVehiclePerResChart(chartWnd,pParameter);
		break;
	case CT_GROUPTHROUGHPUT:
		DrawGroupThroughputChart(chartWnd,pParameter);
		break;
	case CT_RESTHROUGHPUTPERHOUR:
		DrawResThroughputChart(chartWnd,pParameter);
		break;
	default:
		break;
	}
}

void LSThroughputSummaryResult::GenerateResult( CBGetLogFilePath pFunc,LandsideBaseParam* pParameter, InputLandside *pLandisde )
{
	m_vSummrayResult.clear();//clear result before

	EventLog<LandsideVehicleEventStruct> vehicleEventLog;
	LandsideVehicleLog vehicleLog;
	vehicleLog.SetEventLog(&vehicleEventLog);

	CString szDesPath = (*pFunc)(LandsideDescFile);
	CString szEventPath = (*pFunc)(LandsideEventFile);

	if (szEventPath.IsEmpty() || szEventPath.IsEmpty())
		return;
	//load description
	vehicleLog.LoadDataIfNecessary(szDesPath);
	//load log event
	vehicleLog.LoadEventDataIfNecessary(szEventPath);

	LandsideVehicleLogEntry element;
	element.SetEventLog(vehicleLog.getEventLog());
	long vehicleCount = vehicleLog.getCount();

	ASSERT(pParameter);
	CProgressBar bar( "Generating Vehicle Throughput Report", 100, vehicleCount, TRUE );
	for (long i = 0; i < vehicleCount; i++)
	{
		bar.StepIt();
		vehicleLog.getItem( element, i );

		//check vehicle type
		CHierachyName fName;
		fName.fromString(element.GetVehicleDesc().sName);
		if (!pParameter->FilterName(fName))
			continue;

		//check vehicle alive in this time range
		if(element.getExitTime() < pParameter->getStartTime().getPrecisely() ||
			element.getEntryTime() > pParameter->getEndTime().getPrecisely())
			continue;

		CaculateVehicleCrossPortal(element,pParameter);
		for (long nEvent = 0; nEvent < element.getCount(); nEvent++)
		{
			LandsideVehicleEventStruct nextEvent = element.getEvent(nEvent);
			LandsideFacilityLayoutObject* pRes = pLandisde->getObjectList().getObjectByID(nextEvent.resid);
			if (pRes == NULL)
				continue;
			if(!pParameter->FilterObject(pRes->getName()))
				continue;
			

			ThroughputSummaryData tgData;
			tgData.m_iType = 0;
			tgData.m_resouceName = pRes->getName().GetIDString();
			ThroughputResourceData rsData;
			rsData.m_nVehicleID = element.GetVehicleDesc().vehicleID;
			rsData.m_nEventIndex = nEvent;

			ThroughputResourceData preRsData = rsData;
			preRsData.m_nEventIndex--;

			//check sequence event exsit in
			std::vector<ThroughputSummaryData>::iterator iter = std::find(m_vSummrayResult.begin(),m_vSummrayResult.end(),tgData);
			if (iter == m_vSummrayResult.end())
			{
				tgData.m_vThroughput.push_back(rsData);
				m_vSummrayResult.push_back(tgData);
			}
			else
			{
				std::vector<ThroughputResourceData>::iterator resIter = std::find((*iter).m_vThroughput.begin(),(*iter).m_vThroughput.end(),preRsData);
				if (resIter == (*iter).m_vThroughput.end())
				{
					(*iter).m_vThroughput.push_back(rsData);
				}
				else
				{
					(*resIter).m_nEventIndex++;
				}
			}
		}
	}
}

void LSThroughputSummaryResult::RefreshReport( LandsideBaseParam * parameter )
{
	
}

void LSThroughputSummaryResult::InitListHead( CListCtrl& cxListCtrl, LandsideBaseParam * parameter, CSortableHeaderCtrl* piSHC /*= NULL*/ )
{
	ASSERT(piSHC != NULL);
	if(piSHC == NULL)
		return;

	piSHC->ResetAll();

	int colIndex = 0;
	cxListCtrl.InsertColumn(colIndex, "Resource", LVCFMT_LEFT,50);
	piSHC->SetDataType(colIndex, dtSTRING );

	colIndex += 1;
	cxListCtrl.InsertColumn(colIndex, "Group Size", LVCFMT_LEFT,100);
	piSHC->SetDataType(colIndex,  dtINT);

	colIndex += 1;
	cxListCtrl.InsertColumn(colIndex, "Total vehicle", LVCFMT_LEFT,100);
	piSHC->SetDataType(colIndex, dtINT );


	colIndex += 1;
	cxListCtrl.InsertColumn(colIndex, "Avg vehicle", LVCFMT_LEFT,100);
	piSHC->SetDataType(colIndex, dtINT );

	colIndex += 1;
	cxListCtrl.InsertColumn(colIndex, "Total / Hour", LVCFMT_LEFT,100);
	piSHC->SetDataType(colIndex, dtINT );

	colIndex += 1;
	cxListCtrl.InsertColumn(colIndex, "Avg / Hour", LVCFMT_LEFT,100);
	piSHC->SetDataType(colIndex, dtINT );
}

void LSThroughputSummaryResult::FillListContent( CListCtrl& cxListCtrl, LandsideBaseParam * parameter )
{
	if (parameter == NULL)
		return;
	
 	int nRowIndex = 0;
 	LSVehicleThroughputParam* pThroughParam = (LSVehicleThroughputParam*)parameter;
 	for (int i = 0; i < pThroughParam->GetFilterObjectCount(); i++)
 	{
 		ALTObjectID resourceName = pThroughParam->GetFilterObject(i);
		CString strRes = resourceName.GetIDString();
		if (resourceName.IsBlank())
		{
			strRes = _T("All");
		}
 		cxListCtrl.InsertItem(nRowIndex,strRes);
 		
 		int iResCount = GetResourceCount(resourceName);
 		CString strResCount;
 		strResCount.Format(_T("%d"),iResCount);
 		cxListCtrl.SetItemText(nRowIndex,1,strResCount);
 
 		int iVehicleCount = GetResourceVehicleCount(resourceName);
 		CString strVehicleCount;
 		strVehicleCount.Format(_T("%d"),iVehicleCount);
 		cxListCtrl.SetItemText(nRowIndex,2,strVehicleCount);
 
 		float fSingleVehicle = 0.0;
 		if (iResCount != 0)
 		{
 			fSingleVehicle = (float)(iVehicleCount / iResCount);
 		}
 		CString strSingleVehicle;
 		strSingleVehicle.Format(_T("%.2f"),fSingleVehicle);
 		cxListCtrl.SetItemText(nRowIndex,3,strSingleVehicle);
 
 		float hours = (float)(parameter->getEndTime() - parameter->getStartTime()).asHours();
 
 		float fHourVehicle = 0.0;
 		if (hours != 0.0)
 		{
 			fHourVehicle = (float)(iVehicleCount/hours);
 		}
 		CString strHourVehicle;
 		strHourVehicle.Format(_T("%.2f"),fHourVehicle);
 		cxListCtrl.SetItemText(nRowIndex,4,strHourVehicle);
 
 		float fSingleHourVehicle = 0.0;
 		if (hours != 0 && iResCount != 0)
 		{
 			fSingleHourVehicle = (float)(iVehicleCount/hours/iResCount);
 		}
 		CString strSingleHourVehicle;
 		strSingleHourVehicle.Format(_T("%.2f"),fSingleHourVehicle);
 		cxListCtrl.SetItemText(nRowIndex,5,strSingleHourVehicle);
 
 		nRowIndex++;
 	}

	if (pThroughParam->GetPortalName().IsEmpty() == false)
	{
		cxListCtrl.InsertItem(nRowIndex,pThroughParam->GetPortalName());

		int iResCount = 1;
		CString strResCount;
		strResCount.Format(_T("%d"),iResCount);
		cxListCtrl.SetItemText(nRowIndex,1,strResCount);

		int iVehicleCount =GetPortalVehicleCount();
		CString strVehicleCount;
		strVehicleCount.Format(_T("%d"),iVehicleCount);
		cxListCtrl.SetItemText(nRowIndex,2,strVehicleCount);

		float fSingleVehicle = 0.0;
		if (iResCount != 0)
		{
			fSingleVehicle = (float)(iVehicleCount / iResCount);
		}
		CString strSingleVehicle;
		strSingleVehicle.Format(_T("%.2f"),fSingleVehicle);
		cxListCtrl.SetItemText(nRowIndex,3,strSingleVehicle);

		float hours = (float)(parameter->getEndTime() - parameter->getStartTime()).asHours();

		float fHourVehicle = 0.0;
		if (hours != 0.0)
		{
			fHourVehicle = (float)(iVehicleCount/hours);
		}
		CString strHourVehicle;
		strHourVehicle.Format(_T("%.2f"),fHourVehicle);
		cxListCtrl.SetItemText(nRowIndex,4,strHourVehicle);

		float fSingleHourVehicle = 0.0;
		if (hours != 0 && iResCount != 0)
		{
			fSingleHourVehicle = (float)(iVehicleCount/hours/iResCount);
		}
		CString strSingleHourVehicle;
		strSingleHourVehicle.Format(_T("%.2f"),fSingleHourVehicle);
		cxListCtrl.SetItemText(nRowIndex,5,strSingleHourVehicle);

		nRowIndex++;
	}
}

LSGraphChartTypeList LSThroughputSummaryResult::GetChartList() const
{
	LSGraphChartTypeList chartList;

	chartList.AddItem(CT_VEHICLEBYGROUP,"Total Vehicle by Group");
	chartList.AddItem(CT_VEHICLEPERRES,"Vehicle per Resource");
	chartList.AddItem(CT_GROUPTHROUGHPUT,"Group Throughput per Hour");
	chartList.AddItem(CT_RESTHROUGHPUTPERHOUR,"Resource Throughput per Hour");

	return  chartList;
}

BOOL LSThroughputSummaryResult::ReadReportData( ArctermFile& _file )
{

	int nTgCount = 0;
	_file.getInteger(nTgCount);
	_file.getLine();

	for (int j = 0; j < nTgCount; j++)
	{
		ThroughputSummaryData tgData;
		_file.getInteger(tgData.m_iType);
		_file.getField(tgData.m_resouceName.GetBuffer(255),255);
		tgData.m_resouceName.ReleaseBuffer();

		int nRsCount = 0;
		_file.getInteger(nRsCount);
		_file.getLine();
		for (int n = 0; n < nRsCount; n++)
		{
			ThroughputResourceData rsData;
			_file.getInteger(rsData.m_nVehicleID);
			tgData.m_vThroughput.push_back(rsData);
			_file.getLine();
		}

		m_vSummrayResult.push_back(tgData);
	}

	return TRUE;
}

BOOL LSThroughputSummaryResult::WriteReportData( ArctermFile& _file )
{
	int nTgCount = (int)m_vSummrayResult.size();
	_file.writeInt(nTgCount);
	_file.writeLine();

	for (int i = 0; i < nTgCount; i++)
	{
		ThroughputSummaryData tgData = m_vSummrayResult[i];
		_file.writeInt(tgData.m_iType);
		_file.writeField(tgData.m_resouceName);
		int nRsCount = (int)tgData.m_vThroughput.size();
		_file.writeInt(nRsCount);
		_file.writeLine();

		for (int j = 0; j < nRsCount; j++)
		{
			ThroughputResourceData rsData = tgData.m_vThroughput[j];
			_file.writeInt(rsData.m_nVehicleID);
			_file.writeLine();
		}
	}
	return TRUE;
}


void LSThroughputSummaryResult::CaculateVehicleCorssResult( const std::vector<LandsideVehicleEventStruct>& vPointEvent,int nVehicleID,LSVehicleThroughputParam* pThroughputPara )
{
	if (pThroughputPara == NULL)
		return;

	int iPointCount = (int)vPointEvent.size();
	if( iPointCount <2 )
		return;

	CPoint2008 prevPoint, curPoint;
	CLine2008 linePortal, lineMiniTrack;
	linePortal.init(pThroughputPara->GetStartPoint(), pThroughputPara->GetEndPoint());
	double dDistance1=0;
	double dDistance2=0;
	ElapsedTime preTime;
	ElapsedTime curTime;
	ElapsedTime tempTime;
	ElapsedTime pntTime;

	for( int i=0 ,j=1; j< iPointCount ; ++i, ++j )
	{
		LandsideVehicleEventStruct vehiclePreEvent = vPointEvent.at(i);
		LandsideVehicleEventStruct vehicleCurEvent = vPointEvent.at(j);
		prevPoint.init(vehiclePreEvent.x,vehiclePreEvent.y,vehiclePreEvent.z);
		curPoint.init(vehicleCurEvent.x,vehicleCurEvent.y,vehicleCurEvent.z);

		preTime = vehiclePreEvent.time;
		curTime = vehicleCurEvent.time;

		lineMiniTrack.init(prevPoint, curPoint);
		if (linePortal.intersects (lineMiniTrack))
		{
			CPoint2008 tempPnt (linePortal.intersection (lineMiniTrack));
			dDistance1 =  tempPnt.distance( prevPoint );
			dDistance2 = tempPnt.distance( curPoint );
			pntTime =preTime;
			tempTime = curTime - preTime ;
			pntTime += (long) (tempTime.asSeconds() * dDistance1 /( dDistance1 + dDistance2 ) );


			//add to result
			ThroughputSummaryData tgData;
			tgData.m_iType = 1;
			tgData.m_resouceName = pThroughputPara->GetPortalName();

			ThroughputResourceData rsData;
			rsData.m_nVehicleID = nVehicleID;

			std::vector<ThroughputSummaryData>::iterator iter = std::find(m_vSummrayResult.begin(),m_vSummrayResult.end(),tgData);
			if (iter == m_vSummrayResult.end())
			{
				tgData.m_vThroughput.push_back(rsData);
				m_vSummrayResult.push_back(tgData);
			}
			else
			{
				(*iter).m_vThroughput.push_back(rsData);
			}
					
		}
	}
}

void LSThroughputSummaryResult::DrawVehicleByGroupChart( CARC3DChart& chartWnd, LandsideBaseParam *pParameter )
{
	CString strLabel = _T("");
	C2DChartData c2dGraphData;
	c2dGraphData.m_strChartTitle = _T("Total Vehicle by Group");
	c2dGraphData.m_strYtitle = _T("Number of Vehicle");
	c2dGraphData.m_strXtitle = _T("Resource");	
	LSVehicleThroughputParam* pThroughParam = (LSVehicleThroughputParam*)pParameter;
	std::vector<double> dRangeCount;
	int iVehicleCount  = 0;
	for (int i = 0; i < pThroughParam->GetFilterObjectCount(); i++)
	{
		ALTObjectID resourceName = pThroughParam->GetFilterObject(i);
		c2dGraphData.m_vrLegend.push_back(resourceName.GetIDString());
		c2dGraphData.m_vrXTickTitle.push_back(resourceName.GetIDString());

		iVehicleCount = GetResourceVehicleCount(resourceName);

		dRangeCount.push_back(iVehicleCount);
	}

	CString strPortal = pThroughParam->GetPortalName();
	int nPortalVehicle = 0;
	if (strPortal.IsEmpty() == false)
	{
		nPortalVehicle = GetPortalVehicleCount();
		c2dGraphData.m_vrLegend.push_back(strPortal);
		c2dGraphData.m_vrXTickTitle.push_back(strPortal);
	}
	
	dRangeCount.push_back(nPortalVehicle);

	c2dGraphData.m_vr2DChartData.push_back(dRangeCount);
	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("Landside Vehicle Throughput Report %s"), pParameter->GetParameterString());
	c2dGraphData.m_strFooter = strFooter;

	chartWnd.DrawChart(c2dGraphData);
}

void LSThroughputSummaryResult::DrawVehiclePerResChart( CARC3DChart& chartWnd, LandsideBaseParam *pParameter )
{
	CString strLabel = _T("");
	C2DChartData c2dGraphData;
	c2dGraphData.m_strChartTitle = _T("Vehicle per Resource");
	c2dGraphData.m_strYtitle = _T("Number of Vehicle");
	c2dGraphData.m_strXtitle = _T("Resource");	

 	size_t nResCount = m_vSummrayResult.size();
	std::vector<double> dRangeCount;
 	for (size_t nRes = 0; nRes < nResCount; ++ nRes)
 	{
 		ThroughputSummaryData tgData = m_vSummrayResult[nRes];
 		c2dGraphData.m_vrLegend.push_back(tgData.m_resouceName);
		c2dGraphData.m_vrXTickTitle.push_back(tgData.m_resouceName);
 		
 		int iResCount = 0;
 		int iVehicleCount = 0;
 		if (tgData.m_iType == 0)
 		{
 			ALTObjectID objID;
 			objID.FromString(tgData.m_resouceName);
 			iResCount = GetResourceCount(objID);
 			iVehicleCount = GetResourceVehicleCount(objID);
 		}
 		else
 		{
 			iResCount = 1;
 			iVehicleCount = (int)tgData.m_vThroughput.size();
 		}
 		float fSingleVehicle = 0.0;
 		if (iResCount != 0)
 		{
 			fSingleVehicle = (float)(iVehicleCount / iResCount);
 		}
 		dRangeCount.push_back(fSingleVehicle);
 		
 	}
	c2dGraphData.m_vr2DChartData.push_back(dRangeCount);
	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("Landside Vehicle Throughput Report %s"), pParameter->GetParameterString());
	c2dGraphData.m_strFooter = strFooter;

	chartWnd.DrawChart(c2dGraphData);
}

void LSThroughputSummaryResult::DrawGroupThroughputChart( CARC3DChart& chartWnd, LandsideBaseParam *pParameter )
{
	CString strLabel = _T("");
	C2DChartData c2dGraphData;
	c2dGraphData.m_strChartTitle = _T("Group Throughput per Hour");
	c2dGraphData.m_strYtitle = _T("Number of Vehicle");
	c2dGraphData.m_strXtitle = _T("Resource");	

	size_t nResCount = m_vSummrayResult.size();
	float hours = (float)(pParameter->getEndTime() - pParameter->getStartTime()).asHours();

	LSVehicleThroughputParam* pThroughParam = (LSVehicleThroughputParam*)pParameter;
	std::vector<double> dRangeCount;
	int iVehicleCount  = 0;
	for (int i = 0; i < pThroughParam->GetFilterObjectCount(); i++)
	{
		ALTObjectID resourceName = pThroughParam->GetFilterObject(i);
		c2dGraphData.m_vrLegend.push_back(resourceName.GetIDString());
		c2dGraphData.m_vrXTickTitle.push_back(resourceName.GetIDString());

		iVehicleCount = GetResourceVehicleCount(resourceName);

		float fHourVehicle = 0.0;
		if (hours != 0.0)
		{
			fHourVehicle = (float)(iVehicleCount/hours);
		}

		dRangeCount.push_back(fHourVehicle);
	}

	CString strPortal = pThroughParam->GetPortalName();
	int nPortalVehicle = 0;
	if (strPortal.IsEmpty() == false)
	{
		nPortalVehicle = GetPortalVehicleCount();
		float fHourVehicle = 0.0;
		if (hours != 0.0)
		{
			fHourVehicle = (float)(nPortalVehicle/hours);
		}
		c2dGraphData.m_vrLegend.push_back(strPortal);
		c2dGraphData.m_vrXTickTitle.push_back(strPortal);
		dRangeCount.push_back(fHourVehicle);

	}
	
	c2dGraphData.m_vr2DChartData.push_back(dRangeCount);

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("Landside Vehicle Throughput Report %s"), pParameter->GetParameterString());
	c2dGraphData.m_strFooter = strFooter;

	chartWnd.DrawChart(c2dGraphData);
}

void LSThroughputSummaryResult::DrawResThroughputChart( CARC3DChart& chartWnd, LandsideBaseParam *pParameter )
{
	CString strLabel = _T("");
	C2DChartData c2dGraphData;
	c2dGraphData.m_strChartTitle = _T("Resource Throughput per Hour");
	c2dGraphData.m_strYtitle = _T("Number of Vehicle");
	c2dGraphData.m_strXtitle = _T("Resource");	

	size_t nResCount = m_vSummrayResult.size();
	float hours = (float)(pParameter->getEndTime() - pParameter->getStartTime()).asHours();

	std::vector<double> dRangeCount;
 	for (size_t nRes = 0; nRes < nResCount; ++ nRes)
 	{
 		ThroughputSummaryData tgData = m_vSummrayResult[nRes];
 		c2dGraphData.m_vrLegend.push_back(tgData.m_resouceName);
		c2dGraphData.m_vrXTickTitle.push_back(tgData.m_resouceName);
 		
 		int iResCount = 0;
 		int iVehicleCount = 0;
 		if (tgData.m_iType == 0)
 		{
 			ALTObjectID objID;
 			objID.FromString(tgData.m_resouceName);
 			iResCount = GetResourceCount(objID);
 			iVehicleCount = GetResourceVehicleCount(objID);
 		}
 		else
 		{
 			iResCount = 1;
 			iVehicleCount = (int)tgData.m_vThroughput.size();
 		}
 		
 		
 		float fSingleHourVehicle = 0.0;
 		if (hours != 0 && iResCount != 0)
 		{
 			fSingleHourVehicle = (float)(iVehicleCount/hours/iResCount);
 		}
 		dRangeCount.push_back(fSingleHourVehicle);
 	}
 
	c2dGraphData.m_vr2DChartData.push_back(dRangeCount);
 	//set footer
 	CString strFooter(_T(""));
 	strFooter.Format(_T("Landside Vehicle Throughput Report %s"), pParameter->GetParameterString());
 	c2dGraphData.m_strFooter = strFooter;

	chartWnd.DrawChart(c2dGraphData);
}

int LSThroughputSummaryResult::GetResourceCount( const ALTObjectID& objID )
{
	int iResCount = 0;
	for (unsigned i = 0; i < m_vSummrayResult.size(); i++)
	{
		ThroughputSummaryData tgData = m_vSummrayResult[i];
		ALTObjectID resID;
		resID.FromString(tgData.m_resouceName);
		if (resID.idFits(objID))
		{
			iResCount++;
		}
	}
	return iResCount;
}

int LSThroughputSummaryResult::GetResourceVehicleCount( const ALTObjectID& objID )
{
	int iVehicleCount = 0;
	for (unsigned i = 0; i < m_vSummrayResult.size(); i++)
	{
		ThroughputSummaryData tgData = m_vSummrayResult[i];
		ALTObjectID resID;
		resID.FromString(tgData.m_resouceName);
		if (resID.idFits(objID))
		{
			iVehicleCount += (int)tgData.m_vThroughput.size();
		}
	}
	return iVehicleCount;
}

int LSThroughputSummaryResult::GetPortalVehicleCount()
{
	int iVehicleCount = 0;
	for (unsigned i = 0; i < m_vSummrayResult.size(); i++)
	{
		ThroughputSummaryData tgData = m_vSummrayResult[i];
		if (tgData.m_iType == 1)
		{
			iVehicleCount = (int)tgData.m_vThroughput.size();
			break;
		}
	}
	return iVehicleCount;
}
