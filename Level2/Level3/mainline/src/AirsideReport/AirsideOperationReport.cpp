#include "stdafx.h"
#include "AirsideOperationReport.h"
#include "AirsideOperationParam.h"
#include <limits>
#include "AirsideOperationResult.h"

const CString CAirsideOperationReport::strFltOperationType[] = 
{
	_T("ARR"),
		_T("DEP"),
		_T("T/A")

};
CAirsideOperationReport::CAirsideOperationReport(CBGetLogFilePath pFunc):CAirsideBaseReport(pFunc)
{
	m_pAirsideOperationResult = NULL;
}

CAirsideOperationReport::~CAirsideOperationReport(void)
{
	if (NULL != m_pAirsideOperationResult)
	{
		delete m_pAirsideOperationResult;
		m_pAirsideOperationResult = NULL;
	}
}

void CAirsideOperationReport::GenerateReport(CParameters * parameter)
{ 
	m_vOperationData.clear();

	CAirsideOperationParam *pParam = (CAirsideOperationParam  *)parameter;
	ASSERT(pParam != NULL);

	long lStartTime = pParam->getStartTime().asSeconds()*100;
	long lEndTime = pParam->getEndTime().asSeconds()*100;

	//read log
	CString strDescFilepath = (*m_pCBGetLogFilePath)(AirsideDescFile);
	CString strEventFilePath = (*m_pCBGetLogFilePath)(AirsideEventFile);
	if (strEventFilePath.IsEmpty() || strDescFilepath.IsEmpty())
		return;

	EventLog<AirsideFlightEventStruct> airsideFlightEventLog;
	AirsideFlightLog fltLog;
	fltLog.SetEventLog(&airsideFlightEventLog);
	fltLog.LoadDataIfNecessary(strDescFilepath);
	fltLog.LoadEventDataIfNecessary(strEventFilePath);
	
	size_t nFltConsCount = parameter->getFlightConstraintCount();

	for (size_t nfltCons =0; nfltCons < nFltConsCount; ++ nfltCons)
	{
		FlightConstraint paramFltCons = parameter->getFlightConstraint(nfltCons);

		int nFltLogCount = fltLog.getCount();
		for (int i =0; i < nFltLogCount; ++i)
		{
			long lDelayTime = 0L;

			AirsideFlightLogEntry logEntry;
			logEntry.SetEventLog(&airsideFlightEventLog);
			fltLog.getItem(logEntry,i);
			AirsideFlightDescStruct	fltDesc = logEntry.GetAirsideDescStruct();
	//		FlightConstraint fltCons;
	//		if (!fits(parameter,fltDesc,fltCons))
	//			continue;

			if(!paramFltCons.fits(fltDesc))
				continue;

			if (fltDesc.startTime == 0 && fltDesc.endTime == 0)
				continue;

			if (fltDesc.endTime < lStartTime || fltDesc.startTime>lEndTime)//not in the time range
				continue;

			if (getOperationType(fltDesc) == FltOperationType_TA)
			{
				AirsideFltOperationItem arrOperationItem(m_AirportDB);
				arrOperationItem.operationType = FltOperationType_Arr;

				arrOperationItem.fltcons = paramFltCons;
				arrOperationItem.fltDesc = fltDesc;

				arrOperationItem.actualArrTime = fltDesc.arrTime;
				arrOperationItem.arrDelay = fltDesc.arrTime - arrOperationItem.actualArrTime;

				arrOperationItem.actualDepTime = 0;
				arrOperationItem.DepDelay = 0;
				arrOperationItem.strFltID =_T("A") + CString(fltDesc._arrID.GetValue());
				m_vOperationData.push_back(arrOperationItem);

				AirsideFltOperationItem depOperationItem(m_AirportDB);
				depOperationItem.operationType = FltOperationType_Dep;

				depOperationItem.fltcons = paramFltCons;
				depOperationItem.fltDesc = fltDesc;

				depOperationItem.actualDepTime = fltDesc.depTime;
				depOperationItem.DepDelay = depOperationItem.actualDepTime - fltDesc.depTime;

				depOperationItem.actualArrTime = 0;
				depOperationItem.arrDelay = 0;
				depOperationItem.strFltID =_T("D") + CString(fltDesc._depID.GetValue());
				m_vOperationData.push_back(depOperationItem);
			}
			else
			{
				AirsideFltOperationItem operationItem(m_AirportDB);
				operationItem.operationType = getOperationType(fltDesc);

				operationItem.fltcons = paramFltCons;
				operationItem.fltDesc = fltDesc;

				operationItem.actualArrTime = fltDesc.arrTime;
				operationItem.arrDelay = fltDesc.arrTime - operationItem.actualArrTime;

				operationItem.actualDepTime = fltDesc.depTime;
				operationItem.DepDelay = fltDesc.depTime - operationItem.actualDepTime;

				if (operationItem.operationType == FltOperationType_Arr)
				{
					operationItem.strFltID =_T("A") + CString(fltDesc._arrID.GetValue());
				}
				else if (operationItem.operationType == FltOperationType_Dep)
				{
					operationItem.strFltID =_T("D") + CString(fltDesc._depID.GetValue());
				}
				m_vOperationData.push_back(operationItem);
			}
		}
	}
}

CAirsideOperationReport::FltOperationType CAirsideOperationReport::getOperationType(const AirsideFlightDescStruct& fltDesc)
{
	FltOperationType opType;
	if(fltDesc._arrID.HasValue() >0 && fltDesc._depID.HasValue() == 0)
		opType = FltOperationType_Arr;
	else if (fltDesc._arrID.HasValue() ==0 && fltDesc._depID.HasValue() >0)
		opType = FltOperationType_Dep;
	else
		opType = FltOperationType_TA;

	return opType;
}

bool CAirsideOperationReport::fits(CParameters * parameter,const AirsideFlightDescStruct& fltDesc,FlightConstraint& fltCons)
{
	size_t nFltConsCount = parameter->getFlightConstraintCount();
	for (size_t nfltCons =0; nfltCons < nFltConsCount; ++ nfltCons)
	{
		if(parameter->getFlightConstraint(nfltCons).fits(fltDesc))
		{
			fltCons = parameter->getFlightConstraint(nfltCons);
			return true;
		}
	}

	return false;
}

long CAirsideOperationReport::GetMaxTime()
{
	long dMax = 0L;
	std::vector<AirsideFltOperationItem>::iterator iter = m_vOperationData.begin();
	for (;iter!=m_vOperationData.end(); ++iter)
	{
		if((*iter).fltDesc.arrTime >dMax)
			dMax = (*iter).fltDesc.arrTime;

		if ((*iter).fltDesc.depTime >dMax)
			dMax = (*iter).fltDesc.depTime;
	}

	return dMax;
}
long CAirsideOperationReport::GetMinTime()
{
	long dMin = (std::numeric_limits<long>::max)();
	std::vector<AirsideFltOperationItem>::iterator iter = m_vOperationData.begin();
	for (;iter!=m_vOperationData.end(); ++iter)
	{
		if( (*iter).fltDesc.arrTime >0 && (*iter).fltDesc.arrTime < dMin)
			dMin = (*iter).fltDesc.arrTime;

		if ((*iter).fltDesc.depTime > 0 && (*iter).fltDesc.depTime <dMin)
			dMin = (*iter).fltDesc.depTime;
	}
	return dMin;
}

void CAirsideOperationReport::RefreshReport(CParameters * parameter)
{
	ASSERT(parameter != NULL);

	if ((int)m_vOperationData.size() < 1)
	{
		return;
	}

	CAirsideOperationParam* pParam = (CAirsideOperationParam*)parameter;

	if (NULL != m_pAirsideOperationResult)
	{
		delete m_pAirsideOperationResult;
		m_pAirsideOperationResult = NULL;
	}

	m_pAirsideOperationResult = new CAirsideOperationResult;
	CAirsideOperationResult* pAirsideOperationResult = (CAirsideOperationResult*)m_pAirsideOperationResult;
	pAirsideOperationResult->GenerateResult(m_vOperationData, parameter);
}

void CAirsideOperationReport::InitListHead(CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType, CSortableHeaderCtrl* piSHC)
{
	cxListCtrl.DeleteAllItems();

	while(cxListCtrl.GetHeaderCtrl()->GetItemCount() >0)
		cxListCtrl.DeleteColumn(0);

	cxListCtrl.InsertColumn(0, _T("Flight ID"), LVCFMT_LEFT, 70);
	cxListCtrl.InsertColumn(1, _T("Operation"), LVCFMT_LEFT, 70);
	cxListCtrl.InsertColumn(2, _T("Aircraft Type"), LVCFMT_LEFT, 70);
	cxListCtrl.InsertColumn(3, _T("Capacity"), LVCFMT_LEFT, 70);
	cxListCtrl.InsertColumn(4, _T("Schedule Arr Time"), LVCFMT_LEFT, 70);
	cxListCtrl.InsertColumn(5, _T("Schedule Dep Time"), LVCFMT_LEFT, 70);
	cxListCtrl.InsertColumn(6, _T("Load Factor"), LVCFMT_LEFT, 70);
	cxListCtrl.InsertColumn(7, _T("Load"), LVCFMT_LEFT, 70);
	cxListCtrl.InsertColumn(8, _T("Actual Arr Time"), LVCFMT_LEFT, 70);
	cxListCtrl.InsertColumn(9, _T("Actual Dep Time"), LVCFMT_LEFT, 70);
	cxListCtrl.InsertColumn(10, _T("Delay Arr"), LVCFMT_LEFT, 70);
	cxListCtrl.InsertColumn(11, _T("Delay Dep"), LVCFMT_LEFT, 70);
	cxListCtrl.InsertColumn(12, _T("Gate Occupancy"), LVCFMT_LEFT, 70);
	cxListCtrl.InsertColumn(13, _T("Flight Type"), LVCFMT_LEFT, 70);

	if (piSHC)
	{
		piSHC->ResetAll();
		piSHC->SetDataType(0,dtSTRING);
		piSHC->SetDataType(1,dtSTRING);
		piSHC->SetDataType(2,dtSTRING);
		piSHC->SetDataType(3,dtINT);
		piSHC->SetDataType(4,dtSTRING);
		piSHC->SetDataType(5,dtSTRING);
		piSHC->SetDataType(6,dtDEC);
		piSHC->SetDataType(7,dtDEC);
		piSHC->SetDataType(8,dtSTRING);
		piSHC->SetDataType(9,dtSTRING);
		piSHC->SetDataType(10,dtTIME);
		piSHC->SetDataType(11,dtTIME);
		piSHC->SetDataType(12,dtTIME);
		piSHC->SetDataType(13,dtSTRING);
	}
}

void CAirsideOperationReport::FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter)
{
	int nRowCount = 0;

	for (int i=0; i<(int)m_vOperationData.size(); i++)
	{
		//arrival
		if (m_vOperationData[i].operationType == FltOperationType_Arr)
		{
			//flight ID
			CString strFlight(_T(""));
			//if (m_vOperationData[i].operationType == FltOperationType_TA)
			//{
			//	strFlight.Format("%s%s/%s%s", m_vOperationData[i].fltDesc._airline.GetValue(), m_vOperationData[i].fltDesc.arrID, m_vOperationData[i].fltDesc._airline.GetValue(), m_vOperationData[i].fltDesc.depID);
			//}
			//else
			{
				strFlight.Format("%s%s", m_vOperationData[i].fltDesc._airline.GetValue(), m_vOperationData[i].fltDesc.arrID);
			}
			cxListCtrl.InsertItem(nRowCount, strFlight);

			//Operation
			CString strOperationType(_T(""));
			//if (m_vOperationData[i].operationType == FltOperationType_TA)
			//{
			//	strOperationType = _T("T/A");
			//}
			//else
			{
				strOperationType = _T("ARR");
			}
			cxListCtrl.SetItemText(nRowCount, 1,strOperationType);

			//Aircraft Type
			CString strAircraftType(_T(""));
			strAircraftType.Format(_T("%s"), m_vOperationData[i].fltDesc.acType);
			cxListCtrl.SetItemText(nRowCount, 2,strAircraftType);

			//Capacity
			CString strCapacity(_T(""));
			strCapacity.Format(_T("%d"), m_vOperationData[i].fltDesc.nCapacity);
			cxListCtrl.SetItemText(nRowCount, 3,strCapacity);

			//Schedule Arr Time
			CString strScheduleArrTime(_T(""));
			ElapsedTime estArrTime(long((m_vOperationData[i].actualArrTime+m_vOperationData[i].arrDelay)/100.0+0.5));
			strScheduleArrTime.Format(_T("Day%d %02d:%02d:%02d"), estArrTime.GetDay(), estArrTime.GetHour(), estArrTime.GetMinute(), estArrTime.GetSecond());
			cxListCtrl.SetItemText(nRowCount, 4,strScheduleArrTime);

			//Schedule Dep time

			//Load Factor
			CString strLoadFactor(_T(""));
			strLoadFactor.Format(_T("%.4f"), m_vOperationData[i].fltDesc.arrloadFactor);
			cxListCtrl.SetItemText(nRowCount, 6,strLoadFactor);

			//load
			CString strLoad(_T(""));
			strLoad.Format(_T("%.4f"), m_vOperationData[i].fltDesc.arrloadFactor*m_vOperationData[i].fltDesc.nCapacity);
			cxListCtrl.SetItemText(nRowCount, 7,strLoad);

			//Actual Arr time
			CString strActArrTime(_T(""));
			ElapsedTime estActArrTime(long(m_vOperationData[i].actualArrTime/100.0+0.5));
			strActArrTime.Format(_T("Day%d %02d:%02d:%02d"), estActArrTime.GetDay(), estActArrTime.GetHour(), estActArrTime.GetMinute(), estActArrTime.GetSecond());
			cxListCtrl.SetItemText(nRowCount, 8,strActArrTime);

			//actual dep time

			//delay arr
			CString strDelayArrTime(_T(""));
			ElapsedTime estDelayArrTime(long(m_vOperationData[i].arrDelay/100.0+0.5));
			strDelayArrTime.Format(_T("%s"), estDelayArrTime.printTime());
			cxListCtrl.SetItemText(nRowCount, 10,strDelayArrTime);

			//delay dep

			//gate occupancy
			CString strGateOccupancy(_T(""));
			ElapsedTime estGateOccupancyTime(long(m_vOperationData[i].fltDesc.gateOccupancy/100.0+0.5));
			strGateOccupancy.Format(_T("%02d:%02d:%02d"), (estGateOccupancyTime.GetDay()-1)*24+estGateOccupancyTime.GetHour(), estGateOccupancyTime.GetMinute(), estGateOccupancyTime.GetSecond());
			cxListCtrl.SetItemText(nRowCount, 12,strGateOccupancy);

			//flight type
			CString strFlightType(_T("ARRIVING"));
			cxListCtrl.SetItemText(nRowCount, 13,strFlightType);

			//add one row
			nRowCount++;
		}

		//dep
		if (m_vOperationData[i].operationType == FltOperationType_Dep)
		{
			//flight ID
			CString strFlight(_T(""));
			//if (m_vOperationData[i].operationType == FltOperationType_TA)
			//{
			//	strFlight.Format("%s%s/%s%s", m_vOperationData[i].fltDesc._airline.GetValue(), m_vOperationData[i].fltDesc.arrID, m_vOperationData[i].fltDesc._airline.GetValue(), m_vOperationData[i].fltDesc.depID);
			//}
			//else
			{
				strFlight.Format("%s%s", m_vOperationData[i].fltDesc._airline.GetValue(), m_vOperationData[i].fltDesc.depID);
			}
			cxListCtrl.InsertItem(nRowCount, strFlight);

			//Operation
			CString strOperationType(_T(""));
			//if (m_vOperationData[i].operationType == FltOperationType_TA)
			//{
			//	strOperationType = _T("T/A");
			//}
			//else
			{
				strOperationType = _T("DEP");
			}
			cxListCtrl.SetItemText(nRowCount, 1,strOperationType);

			//Aircraft Type
			CString strAircraftType(_T(""));
			strAircraftType.Format(_T("%s"), m_vOperationData[i].fltDesc.acType);
			cxListCtrl.SetItemText(nRowCount, 2,strAircraftType);

			//Capacity
			CString strCapacity(_T(""));
			strCapacity.Format(_T("%d"), m_vOperationData[i].fltDesc.nCapacity);
			cxListCtrl.SetItemText(nRowCount, 3,strCapacity);

			//Schedule Arr Time

			//Schedule Dep time
			CString strScheduleDepTime(_T(""));
			ElapsedTime estDepTime(long((m_vOperationData[i].actualDepTime+m_vOperationData[i].DepDelay)/100.0+0.5));
			strScheduleDepTime.Format(_T("Day%d %02d:%02d:%02d"), estDepTime.GetDay(), estDepTime.GetHour(), estDepTime.GetMinute(), estDepTime.GetSecond());
			cxListCtrl.SetItemText(nRowCount, 5,strScheduleDepTime);

			//Load Factor
			CString strLoadFactor(_T(""));
			strLoadFactor.Format(_T("%.4f"), m_vOperationData[i].fltDesc.deploadFactor);
			cxListCtrl.SetItemText(nRowCount, 6,strLoadFactor);

			//load
			CString strLoad(_T(""));
			strLoad.Format(_T("%.4f"), m_vOperationData[i].fltDesc.deploadFactor*m_vOperationData[i].fltDesc.nCapacity);
			cxListCtrl.SetItemText(nRowCount, 7,strLoad);

			//Actual Arr time

			//actual dep time
			CString strActDepTime(_T(""));
			ElapsedTime estActDepTime(long(m_vOperationData[i].actualDepTime/100.0+0.5));
			strActDepTime.Format(_T("Day%d %02d:%02d:%02d"), estActDepTime.GetDay(), estActDepTime.GetHour(), estActDepTime.GetMinute(), estActDepTime.GetSecond());
			cxListCtrl.SetItemText(nRowCount, 9,strActDepTime);

			//delay arr

			//delay dep
			CString strDelayDepTime(_T(""));
			ElapsedTime estDelayDepTime(long(m_vOperationData[i].DepDelay/100.0+0.5));
			strDelayDepTime.Format(_T("%s"), estDelayDepTime.printTime());
			cxListCtrl.SetItemText(nRowCount, 11,strDelayDepTime);

			//gate occupancy
			CString strGateOccupancy(_T(""));
			ElapsedTime estGateOccupancyTime(long(m_vOperationData[i].fltDesc.gateOccupancy/100.0+0.5));
			strGateOccupancy.Format(_T("%02d:%02d:%02d"), (estGateOccupancyTime.GetDay()-1)*24+estGateOccupancyTime.GetHour(), estGateOccupancyTime.GetMinute(), estGateOccupancyTime.GetSecond());
			cxListCtrl.SetItemText(nRowCount, 12,strGateOccupancy);

			//flight type
			CString strFlightType(_T("DEPARTING"));
			cxListCtrl.SetItemText(nRowCount, 13,strFlightType);

			//add one row
			nRowCount++;
		}
	}
}
BOOL CAirsideOperationReport::ExportListData(ArctermFile& _file,CParameters * parameter) 
{
	int nRowCount = 0;

	for (int i=0; i<(int)m_vOperationData.size(); i++)
	{
		//arrival
		if (m_vOperationData[i].operationType == FltOperationType_Arr)
		{
			//flight ID
			CString strFlight(_T(""));
			{
				strFlight.Format("%s%s", m_vOperationData[i].fltDesc._airline.GetValue(), m_vOperationData[i].fltDesc.arrID);
			}
			_file.writeField(strFlight);

			//Operation
			CString strOperationType(_T(""));
			{
				strOperationType = _T("ARR");
			}
			_file.writeField(strOperationType);

			//Aircraft Type
			CString strAircraftType(_T(""));
			strAircraftType.Format(_T("%s"), m_vOperationData[i].fltDesc.acType);
			_file.writeField(strAircraftType);

			//Capacity
			CString strCapacity(_T(""));
			strCapacity.Format(_T("%d"), m_vOperationData[i].fltDesc.nCapacity);
			_file.writeField(strCapacity);

			//Schedule Arr Time
			CString strScheduleArrTime(_T(""));
			ElapsedTime estArrTime(long((m_vOperationData[i].actualArrTime+m_vOperationData[i].arrDelay)/100.0+0.5));
			strScheduleArrTime.Format(_T("Day%d %02d:%02d:%02d"), estArrTime.GetDay(), estArrTime.GetHour(), estArrTime.GetMinute(), estArrTime.GetSecond());
			_file.writeField(strScheduleArrTime);

			//Schedule Dep time

			//Load Factor
			CString strLoadFactor(_T(""));
			strLoadFactor.Format(_T("%.4f"), m_vOperationData[i].fltDesc.arrloadFactor);
			_file.writeField(strLoadFactor);

			//load
			CString strLoad(_T(""));
			strLoad.Format(_T("%.4f"), m_vOperationData[i].fltDesc.arrloadFactor*m_vOperationData[i].fltDesc.nCapacity);
			_file.writeField(strLoad);

			//Actual Arr time
			CString strActArrTime(_T(""));
			ElapsedTime estActArrTime(long(m_vOperationData[i].actualArrTime/100.0+0.5));
			strActArrTime.Format(_T("Day%d %02d:%02d:%02d"), estActArrTime.GetDay(), estActArrTime.GetHour(), estActArrTime.GetMinute(), estActArrTime.GetSecond());
			_file.writeField(strActArrTime);

			//actual dep time

			//delay arr
			CString strDelayArrTime(_T(""));
			ElapsedTime estDelayArrTime(long(m_vOperationData[i].arrDelay/100.0+0.5));
			strDelayArrTime.Format(_T("%s"), estDelayArrTime.printTime());
			_file.writeField(strDelayArrTime);

			//delay dep

			//gate occupancy
			CString strGateOccupancy(_T(""));
			ElapsedTime estGateOccupancyTime(long(m_vOperationData[i].fltDesc.gateOccupancy/100.0+0.5));
			strGateOccupancy.Format(_T("%02d:%02d:%02d"), (estGateOccupancyTime.GetDay()-1)*24+estGateOccupancyTime.GetHour(), estGateOccupancyTime.GetMinute(), estGateOccupancyTime.GetSecond());
			_file.writeField(strGateOccupancy);

			//flight type
			CString strFlightType(_T("ARRIVING"));
			_file.writeField(strFlightType);

			//add one row
			nRowCount++;
		}

		//dep
		if (m_vOperationData[i].operationType == FltOperationType_Dep)
		{
			//flight ID
			CString strFlight(_T(""));
			//if (m_vOperationData[i].operationType == FltOperationType_TA)
			//{
			//	strFlight.Format("%s%s/%s%s", m_vOperationData[i].fltDesc._airline.GetValue(), m_vOperationData[i].fltDesc.arrID, m_vOperationData[i].fltDesc._airline.GetValue(), m_vOperationData[i].fltDesc.depID);
			//}
			//else
			{
				strFlight.Format("%s%s", m_vOperationData[i].fltDesc._airline.GetValue(), m_vOperationData[i].fltDesc.depID);
			}
			_file.writeField(strFlight);

			//Operation
			CString strOperationType(_T(""));
			//if (m_vOperationData[i].operationType == FltOperationType_TA)
			//{
			//	strOperationType = _T("T/A");
			//}
			//else
			{
				strOperationType = _T("DEP");
			}
			_file.writeField(strOperationType);

			//Aircraft Type
			CString strAircraftType(_T(""));
			strAircraftType.Format(_T("%s"), m_vOperationData[i].fltDesc.acType);
			_file.writeField(strAircraftType);

			//Capacity
			CString strCapacity(_T(""));
			strCapacity.Format(_T("%d"), m_vOperationData[i].fltDesc.nCapacity);
			_file.writeField(strCapacity);

			//Schedule Arr Time

			//Schedule Dep time
			CString strScheduleDepTime(_T(""));
			ElapsedTime estDepTime(long((m_vOperationData[i].actualDepTime+m_vOperationData[i].DepDelay)/100.0+0.5));
			strScheduleDepTime.Format(_T("Day%d %02d:%02d:%02d"), estDepTime.GetDay(), estDepTime.GetHour(), estDepTime.GetMinute(), estDepTime.GetSecond());
			_file.writeField(strScheduleDepTime);

			//Load Factor
			CString strLoadFactor(_T(""));
			strLoadFactor.Format(_T("%.4f"), m_vOperationData[i].fltDesc.deploadFactor);
			_file.writeField(strLoadFactor);

			//load
			CString strLoad(_T(""));
			strLoad.Format(_T("%.4f"), m_vOperationData[i].fltDesc.deploadFactor*m_vOperationData[i].fltDesc.nCapacity);
			_file.writeField(strLoad);

			//Actual Arr time

			//actual dep time
			CString strActDepTime(_T(""));
			ElapsedTime estActDepTime(long(m_vOperationData[i].actualDepTime/100.0+0.5));
			strActDepTime.Format(_T("Day%d %02d:%02d:%02d"), estActDepTime.GetDay(), estActDepTime.GetHour(), estActDepTime.GetMinute(), estActDepTime.GetSecond());
			_file.writeField(strActDepTime);

			//delay arr

			//delay dep
			CString strDelayDepTime(_T(""));
			ElapsedTime estDelayDepTime(long(m_vOperationData[i].DepDelay/100.0+0.5));
			strDelayDepTime.Format(_T("%s"), estDelayDepTime.printTime());
			_file.writeField(strDelayDepTime);

			//gate occupancy
			CString strGateOccupancy(_T(""));
			ElapsedTime estGateOccupancyTime(long(m_vOperationData[i].fltDesc.gateOccupancy/100.0+0.5));
			strGateOccupancy.Format(_T("%02d:%02d:%02d"), (estGateOccupancyTime.GetDay()-1)*24+estGateOccupancyTime.GetHour(), estGateOccupancyTime.GetMinute(), estGateOccupancyTime.GetSecond());
			_file.writeField(strGateOccupancy);

			//flight type
			CString strFlightType(_T("DEPARTING"));
			_file.writeField(strFlightType);

			//add one row
			nRowCount++;
		}

		_file.writeLine() ;
	}
	return TRUE ;
}
BOOL CAirsideOperationReport::ExportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType )
{
	int size = m_vOperationData.size() ;
	_file.writeInt(size) ;
	_file.writeLine() ;
	for (int i = 0 ; i < size ;i++)
	{
		if(!m_vOperationData[i].ExportFile(_file) )
			return FALSE ;
	}
	return TRUE ;
}
BOOL CAirsideOperationReport::ImportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType )
{
   int size = 0 ;
   if(!_file.getInteger(size))
	   return FALSE ;
   
   for (int i = 0 ; i < size ;i++)
   {
	   _file.getLine() ;
	   AirsideFltOperationItem  fltOperItem(m_AirportDB);
       if(!fltOperItem.ImportFile(_file))
		   return FALSE ;
	   m_vOperationData.push_back(fltOperItem) ;
   }
   return TRUE ;
}
BOOL CAirsideOperationReport::AirsideFltOperationItem::ExportFile(ArctermFile& _file)
{
	_file.writeField(strFltID) ;
	_file.writeInt(actualArrTime) ;
	_file.writeInt(actualDepTime) ;
	_file.writeInt(arrDelay) ;
	_file.writeInt(DepDelay) ;
	TCHAR th[1024] = {0};
	fltcons.WriteSyntaxStringWithVersion(th) ;
	_file.writeField(th) ;
	_file.writeInt((int)operationType) ;
	_file.writeLine() ;

	fltDesc.ExportFile(_file) ;
	return TRUE ;
}
BOOL CAirsideOperationReport::AirsideFltOperationItem::ImportFile(ArctermFile& _file)
{
	TCHAR th[1024] = {0} ;
	_file.getField(th,1024) ;
	strFltID.Format("%s",th) ;
	_file.getInteger(actualArrTime) ;
	_file.getInteger(actualDepTime) ;
	_file.getInteger(arrDelay) ;
	_file.getInteger(DepDelay) ;


	_file.getField(th,1024) ;
	fltcons.setConstraintWithVersion(th) ;
	int type = 0 ;
	_file.getInteger(type) ;
    operationType = (FltOperationType)type ;

	_file.getLine() ;

	fltDesc.ImportFile(_file) ;
	return TRUE ;
}
