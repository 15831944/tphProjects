#include "StdAfx.h"
#include ".\lsvehicleactivityresult.h"
#include "..\Common\ProgressBar.h"
#include "..\Landside\LandsideVehicleLogEntry.h"
#include "..\Landside\LandsideVehicleLog.h"

#include "..\Common\LANDSIDE_BIN.h"
#include "LSVehicleActivityParam.h"

#include "../MFCExControl/SortableHeaderCtrl.h"
#include "../MFCExControl/XListCtrl.h"
#include "../Landside/InputLandside.h"

#include "AirsideReport/CARC3DChart.h"
#include <algorithm>
#include "Common/TERMFILE.H"


LSVehicleActivityResult::LSVehicleActivityResult(void)
{
}

LSVehicleActivityResult::~LSVehicleActivityResult(void)
{
}

void LSVehicleActivityResult::Draw3DChart( CARC3DChart& chartWnd, LandsideBaseParam *pParameter )
{
	if(pParameter == NULL)
		return;

	//get chart type
	int nChartType = pParameter->getSubType();

	LSVehicleActivityResult::ChartResult chartResult;

	int nVehicleCount = static_cast<int>(m_vActItem.size());
	for (int nVehicle = 0; nVehicle < nVehicleCount; ++nVehicle)
	{	
		ActItem& pItem = m_vActItem[nVehicle];

		switch (nChartType)
		{

		case CT_EntryTime:
			chartResult.AddTime(pItem.eEntryTime);
			break;
		case CT_ExitTime:
			chartResult.AddTime(pItem.eExitTime);
			break;

		default:
			{
				ASSERT(0);
			}
		}
	}
	CString strChartName = _T("System Entry Time");

	if(nChartType == CT_ExitTime)
		strChartName = _T("System Exit Time");

	//chartResult;
	chartResult.Draw3DChart(strChartName,chartWnd,pParameter);




}

void LSVehicleActivityResult::GenerateResult(CBGetLogFilePath pFunc, LandsideBaseParam* pParameter, InputLandside *pLandisde)
{
	m_vActItem.clear();

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




	LandsideFacilityLayoutObjectList& layoutList = pLandisde->getObjectList();
	LandsideVehicleLogEntry element;
	element.SetEventLog(vehicleLog.getEventLog());
	long vehicleCount = vehicleLog.getCount();

	ASSERT(pParameter);
	CProgressBar bar( "Generating Vehicle Activity Report", 100, vehicleCount, TRUE );
	for (long i = 0; i < vehicleCount; i++)
	{
		bar.StepIt();
		vehicleLog.getItem( element, i );

		//check the parameter
		//time range
		if(element.getExitTime() < pParameter->getStartTime().getPrecisely() ||
			element.getEntryTime() > pParameter->getEndTime().getPrecisely())
			continue;

// 		if(!pParameter->ContainVehicleType(element.GetVehicleDesc().type))
// 			continue;
		
		CHierachyName vehicleName;
		vehicleName.fromString( element.GetVehicleDesc().sName);
		
		if (!pParameter->FilterName(vehicleName))
			continue;
		
		ActItem vehicleItem;

		vehicleItem.nVehicleID = element.GetVehicleDesc().vehicleID;

	//	CString strVehicleTypeName =LandsideBaseParam::GetVehicleTypeName(pLandisde, element.GetVehicleDesc().type);
		

		vehicleItem.strTypeName  = vehicleName.toString();
		vehicleItem.eEntryTime.setPrecisely(element.getEntryTime());
		vehicleItem.eExitTime.setPrecisely(element.getExitTime());
		vehicleItem.nPaxLoad = element.GetVehicleDesc().paxcount;

		vehicleItem.dMaxSpeed = 0;

		double dDistTravel  = 0.0;

		LandsideVehicleEventStruct curEvent;
		if (element.getCount() > 0)
		{
			curEvent = element.getEvent(0);
		}

		LandsideFacilityLayoutObject* pStartObject = layoutList.getObjectByID(curEvent.resid);
		LandsideFacilityLayoutObject* pEndObject = layoutList.getObjectByID(curEvent.resid);
		for (long nEvent = 1; nEvent < element.getCount(); nEvent++)
		{
			LandsideVehicleEventStruct nextEvent = element.getEvent(nEvent);

			//max speed
			if(nextEvent.speed > vehicleItem.dMaxSpeed)
				vehicleItem.dMaxSpeed = nextEvent.speed;

			LandsideFacilityLayoutObject* pObject = layoutList.getObjectByID(nextEvent.resid);
			//start resource
			if (pStartObject == NULL && pObject)
				pStartObject = pObject;
			
			//end resource
			if (pObject)
				pEndObject = pObject;
			
			//if(nextEvent.state == )
			//total distance -> average speed
			float fDisX = static_cast<float>(nextEvent.x - curEvent.x);
			float fDisY = static_cast<float>(nextEvent.y - curEvent.y);
			float fDisZ = static_cast<float>(nextEvent.z - curEvent.z);
			float fDistance = sqrt(fDisX*fDisX + fDisY*fDisY + fDisZ*fDisZ);

			dDistTravel += fDistance;


			curEvent = nextEvent;
		}

		//retrieve start resource name
		if (pStartObject)
		{
			vehicleItem.sStartRs = pStartObject->getName().GetIDString();
		}
		
		//retrieve end resource name
		if (pEndObject)
		{
			vehicleItem.sEndRs = pEndObject->getName().GetIDString();
		}
		
		//cm->m
		vehicleItem.dTotalDistance = dDistTravel/100.0;

		//ElapsedTime  dDuration = ;
		//if(element.getExitTime()- element.getEntryTime() > 0)
		//	vehicleItem.dAverageSpeed =	dDistTravel/element.

		m_vActItem.push_back(vehicleItem);

	}

}

void LSVehicleActivityResult::RefreshReport( LandsideBaseParam * parameter )
{

}

void LSVehicleActivityResult::InitListHead( CXListCtrl& cxListCtrl, LandsideBaseParam * parameter, CSortableHeaderCtrl* piSHC /*= NULL*/ )
{
	ASSERT(piSHC != NULL);
	if(piSHC == NULL)
		return;

	piSHC->ResetAll();

	//pax id

    int colIndex = 0;
    cxListCtrl.InsertColumn(colIndex, "", LVCFMT_LEFT,30);
    piSHC->SetDataType(colIndex, dtINT );

	colIndex += 1;
	cxListCtrl.InsertColumn(colIndex, "ID", LVCFMT_LEFT,50);
	piSHC->SetDataType(colIndex, dtINT );

	colIndex += 1;
	cxListCtrl.InsertColumn(colIndex, "Type", LVCFMT_LEFT,100);
	piSHC->SetDataType(colIndex,  dtSTRING);

	colIndex += 1;
	cxListCtrl.InsertColumn(colIndex, "Entry Time", LVCFMT_LEFT,100);
	piSHC->SetDataType(colIndex, dtTIME );

	colIndex += 1;
	cxListCtrl.InsertColumn(colIndex,"Entry Resource",LVCFMT_LEFT,150);
	piSHC->SetDataType(colIndex,dtSTRING);

	colIndex += 1;
	cxListCtrl.InsertColumn(colIndex, "Exit Time", LVCFMT_LEFT,100);
	piSHC->SetDataType(colIndex, dtTIME );

	colIndex += 1;
	cxListCtrl.InsertColumn(colIndex,"Exit Resource",LVCFMT_LEFT,150);
	piSHC->SetDataType(colIndex,dtSTRING);

	colIndex += 1;
	cxListCtrl.InsertColumn(colIndex, "Duration", LVCFMT_LEFT,100);
	piSHC->SetDataType(colIndex, dtTIME );

	colIndex += 1;
	cxListCtrl.InsertColumn(colIndex, "Max Speed(m/s)", LVCFMT_LEFT,100);
	piSHC->SetDataType(colIndex, dtINT );

	colIndex += 1;
	cxListCtrl.InsertColumn(colIndex, "Total  Distance(m)", LVCFMT_LEFT,100);
	piSHC->SetDataType(colIndex, dtINT );

	colIndex += 1;
	cxListCtrl.InsertColumn(colIndex, "Pax Count Serviced", LVCFMT_LEFT,100);
	piSHC->SetDataType(colIndex, dtINT );


}

void LSVehicleActivityResult::FillListContent( CXListCtrl& cxListCtrl, LandsideBaseParam * parameter )
{
	int nVehicleCount = static_cast<int>(m_vActItem.size());

	for (int nVehicle = 0; nVehicle < nVehicleCount; ++ nVehicle)
	{
		ActItem& vehItem = m_vActItem[nVehicle];

		//passenger index
		CString strText;
		strText.Format(_T("%d"), nVehicle+1);
        cxListCtrl.InsertItem(nVehicle, strText);

        int colIndex = 1;
        strText.Format(_T("%d"), vehItem.nVehicleID);
        cxListCtrl.SetItemText(nVehicle, colIndex, strText);

        colIndex += 1;
        cxListCtrl.SetItemText(nVehicle,colIndex, vehItem.strTypeName);

		//
		colIndex += 1;
		cxListCtrl.SetItemText(nVehicle,colIndex, vehItem.eEntryTime.PrintDateTime());

		//start resource
		colIndex += 1;
		cxListCtrl.SetItemText(nVehicle,colIndex,vehItem.sStartRs);

		//
		colIndex += 1;
		cxListCtrl.SetItemText(nVehicle,colIndex, vehItem.eExitTime.PrintDateTime());

		//end resource
		colIndex += 1;
		cxListCtrl.SetItemText(nVehicle,colIndex,vehItem.sEndRs);

		//duration
		ElapsedTime eDuration = vehItem.eExitTime - vehItem.eEntryTime;
		if (eDuration.getPrecisely() < 0)
		{
			eDuration =ElapsedTime(0L);
			ASSERT(0);
		}

		colIndex += 1;
		cxListCtrl.SetItemText(nVehicle,colIndex, eDuration.printTime());

		//
		colIndex += 1;	
		strText.Format(_T("%.2f"), vehItem.dMaxSpeed/100);//cm/s->m/s
		cxListCtrl.SetItemText(nVehicle,colIndex, strText);

		//
		colIndex += 1;	
		strText.Format(_T("%.2f"), vehItem.dTotalDistance);
		cxListCtrl.SetItemText(nVehicle,colIndex, strText);

		//
		colIndex += 1;
		strText.Format(_T("%d"), vehItem.nPaxLoad);
		cxListCtrl.SetItemText(nVehicle,colIndex, strText);
	}
}

BOOL LSVehicleActivityResult::ReadReportData( ArctermFile& _file )
{

	m_vActItem.clear();
	//data
	int nCount = 0;
	_file.getInteger(nCount);
	_file.getLine();

	for (int nItem = 0; nItem < nCount; ++nItem)
	{
		ActItem actItem;

		_file.getInteger(actItem.nVehicleID);
		_file.getField(actItem.strTypeName.GetBuffer(1024), 1024);
		actItem.strTypeName.ReleaseBuffer();

		_file.getTime(actItem.eEntryTime);
		_file.getField(actItem.sStartRs.GetBuffer(1024),1024);
		actItem.sStartRs.ReleaseBuffer();

		_file.getTime(actItem.eExitTime);
		_file.getField(actItem.sEndRs.GetBuffer(1024),1024);
		actItem.sEndRs.ReleaseBuffer();

		_file.getFloat(actItem.dMaxSpeed);
		_file.getFloat(actItem.dTotalDistance);
		_file.getInteger(actItem.nPaxLoad);
		_file.getFloat(actItem.dAverageSpeed);

		m_vActItem.push_back(actItem);
		_file.getLine();
	}

	return TRUE;
}

BOOL LSVehicleActivityResult::WriteReportData( ArctermFile& _file )
{



	//data
	int nCount = static_cast<int>(m_vActItem.size());

	_file.writeInt(nCount);
	_file.writeLine();


	for (int nItem = 0; nItem < nCount; ++nItem)
	{
		ActItem& actItem = m_vActItem[nItem];

		_file.writeInt(actItem.nVehicleID);
		_file.writeField(actItem.strTypeName);
		_file.writeTime(actItem.eEntryTime);
		_file.writeField(actItem.sStartRs);
		_file.writeTime(actItem.eExitTime);
		_file.writeField(actItem.sEndRs);
		_file.writeDouble(actItem.dMaxSpeed);
		_file.writeDouble(actItem.dTotalDistance);
		_file.writeInt(actItem.nPaxLoad);
		_file.writeDouble(actItem.dAverageSpeed);

		_file.writeLine();
	}





	return TRUE;

}

LSGraphChartTypeList LSVehicleActivityResult::GetChartList() const
{
	LSGraphChartTypeList chartList;

	chartList.AddItem(CT_EntryTime,"System Entry Time");
	chartList.AddItem(CT_ExitTime,"System Exit Time");


	return chartList;
}

///////////
LSVehicleActivityResult::ChartResult::ChartResult( void )
{

}

LSVehicleActivityResult::ChartResult::~ChartResult( void )
{

}

void LSVehicleActivityResult::ChartResult::AddTime( const ElapsedTime& eTime )
{
	m_vTime.push_back(eTime);
}

void LSVehicleActivityResult::ChartResult::Draw3DChart( const CString& strChartName, CARC3DChart& chartWnd, LandsideBaseParam *pParameter )
{
	std::sort(m_vTime.begin(), m_vTime.end());

	CString strLabel = _T("");
	C2DChartData c2dGraphData;
	c2dGraphData.m_strChartTitle = strChartName;
	c2dGraphData.m_strYtitle = _T("Number of Vehicle");
	c2dGraphData.m_strXtitle = _T("Date Time");	


	ElapsedTime eStartTime = pParameter->getStartTime();
	ElapsedTime eEndTime = pParameter->getEndTime();
	ElapsedTime eInterval(pParameter->getInterval());

	std::vector<double> dRangeCount;
	for (ElapsedTime eTime = eStartTime; eTime < eEndTime; eTime += eInterval)
	{
		c2dGraphData.m_vrXTickTitle.push_back(GetXtitle(eTime, eTime + eInterval));
		dRangeCount.push_back(GetCount(eTime, eTime + eInterval));
	}
	c2dGraphData.m_vr2DChartData.push_back(dRangeCount);


	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("Landside Vehicle Activity Report %s"), pParameter->GetParameterString());
	c2dGraphData.m_strFooter = strFooter;

	chartWnd.DrawChart(c2dGraphData);


}

int LSVehicleActivityResult::ChartResult::GetCount( ElapsedTime& eMinTime, ElapsedTime& eMaxTime ) const
{
	ASSERT(eMinTime < eMaxTime);

	int nRangeCount = 0;
	int nCount = static_cast<int>(m_vTime.size());
	for (int nItem = 0; nItem < nCount; ++ nItem)
	{
		if(m_vTime[nItem] < eMinTime)
			continue;
		if(m_vTime[nItem] >= eMaxTime)
			break;

		nRangeCount += 1;
	}	

	return nRangeCount;
}

CString LSVehicleActivityResult::ChartResult::GetXtitle( ElapsedTime& eMinTime, ElapsedTime& eMaxTime ) const
{
	CString strXTitle;
	strXTitle.Format(_T("%s-%s"), eMinTime.PrintDateTime(), eMaxTime.PrintDateTime());
	return strXTitle;
}




