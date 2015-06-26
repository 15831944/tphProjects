#include "StdAfx.h"
#include ".\lsthroughputdetailresult.h"
#include "..\MFCExControl\SortableHeaderCtrl.h"
#include "..\MFCExControl\XListCtrl.h"
#include "LandsideBaseParam.h"
#include "..\Results\LandsideVehicleDelayLog.h"
#include "..\Common\ProgressBar.h"
#include "..\Landside\InputLandside.h"
#include "..\Common\Point2008.h"
#include "..\Common\Line2008.h"
#include "LSVehicleThroughputParam.h"
#include "AirsideReport/CARC3DChart.h"


LSThroughputDetailResult::LSThroughputDetailResult(void)
{
}

LSThroughputDetailResult::~LSThroughputDetailResult(void)
{
}

void LSThroughputDetailResult::Draw3DChart( CARC3DChart& chartWnd, LandsideBaseParam *pParameter )
{
	if (pParameter == NULL)
		return;

	if (pParameter->getSubType() == CT_PAXSERVED)
	{
		DrawPaxServedChart(chartWnd,pParameter);
	}
	
}

void LSThroughputDetailResult::GenerateResult( CBGetLogFilePath pFunc,LandsideBaseParam* pParameter, InputLandside *pLandisde )
{
	m_vDetailResult.clear();//clear result before

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

			//add to result
			ThroughputDetailData tgData;
			tgData.m_iType = 0;
			tgData.m_resouceName = pRes->getName().GetIDString();
			InitResultData(tgData,pParameter);

			ThroughputResourceData rsData;
			rsData.m_nVehicleID = element.GetVehicleDesc().vehicleID;
			rsData.m_nEventIndex = nEvent;

			ThroughputResourceData preRsData = rsData;
			preRsData.m_nEventIndex--;

			for (unsigned iResult = 0; iResult < tgData.m_vInterval.size(); iResult++)
			{
				ThroughputInterval& tgInterval = tgData.m_vInterval[iResult];
				if (nextEvent.time < tgInterval.m_eStartTime.getPrecisely() 
					|| nextEvent.time > tgInterval.m_eEndTime.getPrecisely())
					continue;
				
				//check sequence event exsit in
				std::vector<ThroughputDetailData>::iterator iter = std::find(m_vDetailResult.begin(),m_vDetailResult.end(),tgData);
				if (iter == m_vDetailResult.end())
				{
					tgInterval.m_vThrougput.push_back(rsData);
					m_vDetailResult.push_back(tgData);
				}
				else
				{
					ThroughputInterval& exsitInterval = (*iter).m_vInterval[iResult];
					std::vector<ThroughputResourceData>::iterator resIter = std::find(exsitInterval.m_vThrougput.begin(),exsitInterval.m_vThrougput.end(),preRsData);
					if (resIter == exsitInterval.m_vThrougput.end())
					{
						exsitInterval.m_vThrougput.push_back(rsData);
					}
					else
					{
						(*resIter).m_nEventIndex++;
					}
				}
			}

		}	

	}
}

void LSThroughputDetailResult::RefreshReport( LandsideBaseParam * parameter )
{

}

void LSThroughputDetailResult::InitListHead( CXListCtrl& cxListCtrl, LandsideBaseParam * parameter, CSortableHeaderCtrl* piSHC /*= NULL*/ )
{
	ASSERT(piSHC != NULL);
	if(piSHC == NULL)
		return;

	piSHC->ResetAll();

	int colIndex = 0;
	cxListCtrl.InsertColumn(colIndex, "Resource", LVCFMT_LEFT,50);
	piSHC->SetDataType(colIndex, dtSTRING );

	colIndex += 1;
	cxListCtrl.InsertColumn(colIndex, "Start Time", LVCFMT_LEFT,100);
	piSHC->SetDataType(colIndex,  dtTIME);

	colIndex += 1;
	cxListCtrl.InsertColumn(colIndex, "End Time", LVCFMT_LEFT,100);
	piSHC->SetDataType(colIndex, dtTIME );


	colIndex += 1;
	cxListCtrl.InsertColumn(colIndex, "Vehicle Served", LVCFMT_LEFT,100);
	piSHC->SetDataType(colIndex, dtSTRING );

}

void LSThroughputDetailResult::FillListContent( CXListCtrl& cxListCtrl, LandsideBaseParam * parameter )
{
	int nRowIndex = 0;
	for (unsigned i = 0; i < m_vDetailResult.size(); i++)
	{
		ThroughputDetailData tgData = m_vDetailResult[i];
		for (unsigned j = 0; j < tgData.m_vInterval.size(); j++)
		{
			ThroughputInterval tgInterval = tgData.m_vInterval[j];
			cxListCtrl.InsertItem(nRowIndex,tgData.m_resouceName);
			cxListCtrl.SetItemText(nRowIndex,1,tgInterval.m_eStartTime.PrintDateTime());
			cxListCtrl.SetItemText(nRowIndex,2,tgInterval.m_eEndTime.PrintDateTime());
			int nVehicleCount = (int)tgInterval.m_vThrougput.size();
			CString strVehicleCount;
			strVehicleCount.Format(_T("%d"),nVehicleCount);
			cxListCtrl.SetItemText(nRowIndex,3,strVehicleCount);
			nRowIndex++;
		}
	}
}

LSGraphChartTypeList LSThroughputDetailResult::GetChartList() const
{
	LSGraphChartTypeList chartList;

	chartList.AddItem(CT_PAXSERVED,"Vehicle Served");

	return  chartList;
}

void LSThroughputDetailResult::InitResultData(ThroughputDetailData& tgData, LandsideBaseParam* pParameter )
{
	ElapsedTime eStartTime = pParameter->getStartTime();
	ElapsedTime eEndTime = pParameter->getEndTime();

	long intervalSize = 0 ;
	ElapsedTime IntervalTime ;
	IntervalTime = eEndTime - eStartTime;
	intervalSize = IntervalTime.asSeconds() / pParameter->getInterval() ;

	long lUserIntervalTime = pParameter->getInterval();
	ElapsedTime estUserIntervalTime = ElapsedTime(lUserIntervalTime);

	for (ElapsedTime eTime = eStartTime; eTime < eEndTime; eTime += estUserIntervalTime)
	{
		ThroughputInterval tgInterval;
		tgInterval.m_eStartTime = eTime;
		tgInterval.m_eEndTime = eTime + estUserIntervalTime;

		tgData.m_vInterval.push_back(tgInterval);
	}
}

void LSThroughputDetailResult::CaculateVehicleCorssResult( const std::vector<LandsideVehicleEventStruct>& vPointEvent,int nVehicleID,LSVehicleThroughputParam* pThroughputPara )
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
			ThroughputDetailData tgData;
			tgData.m_iType = 1;
			tgData.m_resouceName = pThroughputPara->GetPortalName();
			InitResultData(tgData,pThroughputPara);

			for (unsigned iResult = 0; iResult < tgData.m_vInterval.size(); iResult++)
			{
				ThroughputInterval& tgInterval = tgData.m_vInterval[iResult];
				if (pntTime < tgInterval.m_eStartTime.getPrecisely() 
					|| pntTime > tgInterval.m_eEndTime.getPrecisely())
					continue;

				ThroughputResourceData rsData;
				rsData.m_nVehicleID = nVehicleID;

				std::vector<ThroughputDetailData>::iterator iter = std::find(m_vDetailResult.begin(),m_vDetailResult.end(),tgData);
				if (iter == m_vDetailResult.end())
				{
					tgInterval.m_vThrougput.push_back(rsData);
					m_vDetailResult.push_back(tgData);
				}
				else
				{
					ThroughputInterval& exsitInterval = (*iter).m_vInterval[iResult];
					exsitInterval.m_vThrougput.push_back(rsData);
				}
			}	
		}
	}
}

BOOL LSThroughputDetailResult::ReadReportData( ArctermFile& _file )
{
	int nCount = 0;
	_file.getInteger(nCount);
	_file.getLine();
	for (int i = 0; i < nCount; i++)
	{
		ThroughputDetailData tgData;
		_file.getInteger(tgData.m_iType);
		_file.getField(tgData.m_resouceName.GetBuffer(255),255);
		tgData.m_resouceName.ReleaseBuffer();
		int nTgCount = 0;
		_file.getInteger(nTgCount);
		_file.getLine();

		for (int j = 0; j < nTgCount; j++)
		{
			ThroughputInterval tgInterval;
			_file.getTime(tgInterval.m_eStartTime,TRUE);
			_file.getTime(tgInterval.m_eEndTime,TRUE);

			int nRsCount = 0;
			_file.getInteger(nRsCount);
			_file.getLine();
			for (int n = 0; n < nRsCount; n++)
			{
				ThroughputResourceData rsData;
				_file.getInteger(rsData.m_nVehicleID);
				tgInterval.m_vThrougput.push_back(rsData);
				_file.getLine();
			}
			tgData.m_vInterval.push_back(tgInterval);
		}
		m_vDetailResult.push_back(tgData);
	}
	return TRUE;
}

BOOL LSThroughputDetailResult::WriteReportData( ArctermFile& _file )
{
	int nCount = (int)(m_vDetailResult.size());
	_file.writeInt(nCount);
	_file.writeLine();

	for (int i = 0; i < nCount; i++)
	{
		ThroughputDetailData tgData = m_vDetailResult[i];
		_file.writeInt(tgData.m_iType);
		_file.writeField(tgData.m_resouceName);

		int nTgCount = (int)tgData.m_vInterval.size();
		_file.writeInt(nTgCount);
		_file.writeLine();

		for (int j = 0; j < nTgCount; j++)
		{
			ThroughputInterval tgInterval = tgData.m_vInterval[j];
			_file.writeTime(tgInterval.m_eStartTime,TRUE);
			_file.writeTime(tgInterval.m_eEndTime,TRUE);
			int nRsCount = (int)tgInterval.m_vThrougput.size();
			_file.writeInt(nRsCount);
			_file.writeLine();

			for (int n = 0; n < nRsCount; n++)
			{
				ThroughputResourceData rsData = tgInterval.m_vThrougput[n];
				_file.writeInt(rsData.m_nVehicleID);
				_file.writeLine();
			}
		}
	}
	return TRUE;
}

void LSThroughputDetailResult::DrawPaxServedChart( CARC3DChart& chartWnd, LandsideBaseParam *pParameter )
{
	CString strLabel = _T("");
	C2DChartData c2dGraphData;
	c2dGraphData.m_strChartTitle = _T("Resource Throughput");
	c2dGraphData.m_strYtitle = _T("Number of Vehicle");
	c2dGraphData.m_strXtitle = _T("Time of Day");	


	ElapsedTime eStartTime = pParameter->getStartTime();
	ElapsedTime eEndTime = pParameter->getEndTime();
	ElapsedTime eInterval(pParameter->getInterval());

	ASSERT(eEndTime >= eStartTime);
	ASSERT(eInterval > ElapsedTime(0L));

	if(eStartTime > eEndTime || eInterval <= ElapsedTime(0L))
		return;

	
	for (ElapsedTime eTime = eStartTime; eTime < eEndTime; eTime += eInterval)
	{
		c2dGraphData.m_vrXTickTitle.push_back(GetXtitle(eTime, eTime + eInterval));
	}

	for (unsigned i = 0; i < m_vDetailResult.size(); i++)
	{
		ThroughputDetailData tgData = m_vDetailResult[i];
		c2dGraphData.m_vrLegend.push_back(tgData.m_resouceName);

		std::vector<double> dRangeCount;
		for (unsigned j = 0; j < tgData.m_vInterval.size(); j++)
		{
			ThroughputInterval tgInterval = tgData.m_vInterval[j];
			double dVehicleCount = (double)tgInterval.m_vThrougput.size();
			dRangeCount.push_back(dVehicleCount);
		}
		c2dGraphData.m_vr2DChartData.push_back(dRangeCount);
	}

 	
	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("Landside Vehicle Throughput Report %s"), pParameter->GetParameterString());
	c2dGraphData.m_strFooter = strFooter;

	chartWnd.DrawChart(c2dGraphData);
}

CString LSThroughputDetailResult::GetXtitle( ElapsedTime& eMinTime, ElapsedTime& eMaxTime ) const
{
	CString strXTitle;
	strXTitle.Format(_T("%s-%s"), eMinTime.PrintDateTime(), eMaxTime.PrintDateTime());
	return strXTitle;
}
