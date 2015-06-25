#include "StdAfx.h"
#include "FlightActivityBaseResult.h"
#include "Parameters.h"
#include "AirsideReportBaseResult.h"
#include "FlightActivityParam.h"
#include "AirsideFlightActivityReportBaseResult.h"
#include "AirsideFlightSummaryActivityReportBaseResult.h"
#include <limits>
#include "../Common/ARCUnit.h"
#include "../InputAirside/ARCUnitManager.h"
#include "..\InputAirside\IntersectionNode.h"
#include "..\InputAirside\RunwayExit.h"
///////////////////////////////////////////////////////////////////////////////////////////////
CFlightActivityBaseResult::CFlightActivityBaseResult()
{

}

CFlightActivityBaseResult::~CFlightActivityBaseResult()
{

}

void CFlightActivityBaseResult::SetCBGetFilePath(CBGetLogFilePath pFunc)
{
	m_pCBGetLogFilePath = pFunc;
}

void CFlightActivityBaseResult::setLoadFromFile(bool bLoad)
{
	m_bLoadFromFile = bLoad;
}

bool CFlightActivityBaseResult::IsLoadFromFile()
{
	return m_bLoadFromFile;
}
///////////////////////////////////////////////////////////////////////////////////////////////
CFlightDetailActivityResult::CFlightDetailActivityResult()
:m_pResult(NULL)
{

}

CFlightDetailActivityResult::~CFlightDetailActivityResult()
{

}

void CFlightDetailActivityResult::GenerateResult(CBGetLogFilePath pCBGetLogFilePath,CParameters *pParameter)
{
	m_vResult.clear();

	CFlightActivityParam *pParam = (CFlightActivityParam *)pParameter;
	ASSERT(pParam != NULL);
	
	long lStartTime = pParam->getStartTime().asSeconds()*100;
	long lEndTime = pParam->getEndTime().asSeconds()*100;
	
	//read log
	CString strDescFilepath = (*pCBGetLogFilePath)(AirsideDescFile);
	CString strEventFilePath = (*pCBGetLogFilePath)(AirsideEventFile);
	if (strEventFilePath.IsEmpty() || strDescFilepath.IsEmpty())
	 	return;
	
	EventLog<AirsideFlightEventStruct> airsideFlightEventLog;
	AirsideFlightLog fltLog;
	fltLog.SetEventLog(&airsideFlightEventLog);
	fltLog.LoadDataIfNecessary(strDescFilepath);
	fltLog.LoadEventDataIfNecessary(strEventFilePath);
	
	int nFltLogCount = fltLog.getCount();
	for (int i =0; i < nFltLogCount; ++i)
	{
	
	 	long lDelayTime = 0L;
	
	 	AirsideFlightLogEntry logEntry;
	 	logEntry.SetEventLog(&airsideFlightEventLog);
	 	fltLog.getItem(logEntry,i);
	 	AirsideFlightDescStruct	fltDesc = logEntry.GetAirsideDescStruct();
	 	FlightConstraint fltCons;
	 	if (!fits(pParameter,fltDesc,fltCons))
	 		continue;
	
	 	if (fltDesc.startTime == 0 && fltDesc.endTime == 0)
	 		continue;
	
	 	if (fltDesc.endTime < lStartTime || fltDesc.startTime>lEndTime)//not in the time range
	 		continue;
	
	 	//new report item
	 	FltActItem fltAct;
	 	fltAct.ID = (int)m_vResult.size();
	 	fltAct.fltDesc = fltDesc;
	 	fltAct.entryTime = static_cast<long>(fltDesc.startTime/100);
	 	fltAct.exitTime = static_cast<long>(fltDesc.endTime/100);
	 	fltAct.durationTime = fltAct.exitTime -fltAct.entryTime;
	
	 	int nEventCount = logEntry.getCount();
	 	bool bAdd = false;

		FltActTraceItem lastTraceItem;
	 	for (int nEvent = 0; nEvent < nEventCount; ++nEvent)
	 	{	
			
			AirsideFlightEventStruct event = logEntry.getEvent(nEvent);


			//birth event
			if(nEvent == 0)
			{
				FltActTraceItem item;
				item.nObjType = 3;
				item.entryTime = static_cast<long>(event.time/100);
				fltAct.vTrace.push_back(item);
				lastTraceItem = item;
			}
			//entry resource event
			if (event.eventCode == 'e'|| event.mode == OnHeldAtStand || event.mode == OnTakeOffWaitEnterRunway \
				|| event.mode == OnTakeOffEnterRunway || event.mode == OnTakeoff)
			{	
				//no duplicate items
				if(lastTraceItem.nObjType == 0 && lastTraceItem.nObjID == event.nodeNO )
					continue;
				if(lastTraceItem.nObjType == 1 && lastTraceItem.nObjID == event.intNodeID )
					continue;

				FltActTraceItem item;
				if (event.nodeNO >0)
				{
					item.nObjType = 0;
					item.nObjID = event.nodeNO;
					item.entryTime = static_cast<long>(event.time/100);
				}
				else if (event.intNodeID >0)
				{	
					if (event.mode == OnExitRunway)
					{
						item.nObjType = 5;
					}
					else
					{
						item.nObjType = 1;
					}
					
					item.nObjID = event.intNodeID;
					item.entryTime = static_cast<long>(event.time/100);
				}
				else
					continue;

				fltAct.vTrace.push_back(item);
				lastTraceItem = item;
	 		}

			//Death event
			if(nEvent == nEventCount - 1)
			{
				FltActTraceItem item;
				item.nObjType = 4;
				item.entryTime = static_cast<long>(event.time/100);
				lastTraceItem = item;
	 			fltAct.vTrace.push_back(item);
			}
	 	}
	 	AddToResult(pParam, fltAct);
	}
}

void CFlightDetailActivityResult::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{
	if (m_pResult)
	{
		m_pResult->Draw3DChart(chartWnd,pParameter);
	}
}

void CFlightDetailActivityResult::AddToResult(CParameters* param, FltActItem& fltActItem)
{
	ASSERT(param != NULL);

	int nFltConsCount = (int)param->getFlightConstraintCount();
	for(int i=0; i<nFltConsCount; i++)
	{
		if (param->getFlightConstraint(i).fits(fltActItem.fltDesc))
		{
			param->getFlightConstraint(i).screenPrint(fltActItem.strFltType.GetBuffer(1024));
			fltActItem.strFltType.ReleaseBuffer();

			m_vResult.push_back(fltActItem);
		}
	}
}

bool CFlightDetailActivityResult::fits(CParameters * parameter,const AirsideFlightDescStruct& fltDesc,FlightConstraint& fltCons)
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

long CFlightDetailActivityResult::GetMaxTime(subReportType subType)
{
	long lMaxTime = (std::numeric_limits<long>::min)();
	std::vector<FltActItem>::iterator iter = m_vResult.begin();
	std::vector<FltActItem>::iterator iterEnd = m_vResult.end();

	if (subType == FAR_SysEntryDist)
	{
		for (;iter != iterEnd; ++iter)
		{
			if ((*iter).entryTime > lMaxTime)
				lMaxTime = (*iter).entryTime;
		}
	}
	else if (subType == FAR_SysExitDist)
	{
		for (;iter != iterEnd; ++iter)
		{
			if ((*iter).exitTime > lMaxTime)
				lMaxTime = (*iter).exitTime;
		}
	}

	return lMaxTime;

}

long CFlightDetailActivityResult::GetMinTime(subReportType subType)
{
	long lMinTime = (std::numeric_limits<long>::max)();
	std::vector<FltActItem>::iterator iter = m_vResult.begin();
	std::vector<FltActItem>::iterator iterEnd = m_vResult.end();

	if (subType == FAR_SysEntryDist)
	{
		for (;iter != iterEnd; ++iter)
		{
			if ((*iter).entryTime < lMinTime)
				lMinTime = (*iter).entryTime;
		}
	}
	else if (subType == FAR_SysExitDist)
	{
		for (;iter != iterEnd; ++iter)
		{
			if ((*iter).exitTime < lMinTime)
				lMinTime = (*iter).exitTime;
		}
	}

	return lMinTime;
}

long CFlightDetailActivityResult::GetCount(subReportType subType,long start ,long end)
{
	long lCount = 0L;
	std::vector<FltActItem>::iterator iter = m_vResult.begin();
	std::vector<FltActItem>::iterator iterEnd = m_vResult.end();

	if (subType == FAR_SysEntryDist)
	{
		for (;iter != iterEnd; ++iter)
		{
			if ((*iter).entryTime < end && (*iter).entryTime > start)
				lCount += 1;
		}
	}
	else if (subType == FAR_SysExitDist)
	{
		for (;iter != iterEnd; ++iter)
		{
			if ((*iter).exitTime < end && (*iter).exitTime > start)
				lCount += 1;
		}
	}

	return lCount;
}

void CFlightDetailActivityResult::SetReportResult(CFlightActivityBaseResult* pResult)
{
	m_vResult.clear();
	if (pResult && pResult->GetReportType() == ASReportType_Detail)
	{
		m_vResult = ((CFlightDetailActivityResult*)pResult)->m_vResult;
	}
}

void CFlightDetailActivityResult::RefreshReport(CParameters * parameter)
{
	CFlightActivityParam* pParam = (CFlightActivityParam*)parameter;
	ASSERT(pParam != NULL);

	if (NULL != m_pResult)
	{
		delete m_pResult;
		m_pResult = NULL;
	}

	switch(pParam->getSubType())
	{
	case FAR_SysEntryDist:
		{
			m_pResult = new CAirsideFlightActivityReportEntryResult;
			CAirsideFlightActivityReportEntryResult* pResult = (CAirsideFlightActivityReportEntryResult*)m_pResult;
			pResult->GenerateResult(m_vResult, parameter);
		}
		break;

	case FAR_SysExitDist:
		{
			m_pResult = new CAirsideFlightActivityReportExitResult;
			CAirsideFlightActivityReportExitResult* pResult = (CAirsideFlightActivityReportExitResult*)m_pResult;
			pResult->GenerateResult(m_vResult, parameter);
		}
		break;

	default:
		break;
	}
}

void CFlightDetailActivityResult::InitListHead(CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType, CSortableHeaderCtrl* piSHC)
{
	cxListCtrl.DeleteAllItems();

	while(cxListCtrl.GetHeaderCtrl()->GetItemCount() >0)
		cxListCtrl.DeleteColumn(0);

	cxListCtrl.InsertColumn(0, _T("Airline"), LVCFMT_LEFT, 100);	
	cxListCtrl.InsertColumn(1, _T("Arrival ID"), LVCFMT_LEFT, 100);
	cxListCtrl.InsertColumn(2, _T("Departure ID"), LVCFMT_LEFT, 100);
	cxListCtrl.InsertColumn(3, _T("Entry Time"),LVCFMT_LEFT,100);
	cxListCtrl.InsertColumn(4, _T("Exit Time"),LVCFMT_LEFT,100);
	cxListCtrl.InsertColumn(5, _T("Duration Time"),LVCFMT_LEFT,100);
	cxListCtrl.InsertColumn(6, _T("Flight Type"),LVCFMT_LEFT,120);
	if (piSHC)
	{
		piSHC->ResetAll();
		piSHC->SetDataType(0,dtSTRING);
		piSHC->SetDataType(1,dtSTRING);
		piSHC->SetDataType(2,dtSTRING);
		piSHC->SetDataType(3,dtSTRING);
		piSHC->SetDataType(4,dtSTRING);
		piSHC->SetDataType(5,dtTIME);
		piSHC->SetDataType(6,dtSTRING);

	}
}

void CFlightDetailActivityResult::FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter)
{
	for (int i=0; i<(int)m_vResult.size(); i++)
	{
		FltActItem fltActItem = m_vResult[i];

		////incorrect airline
		//if (!fltActItem.fltDesc._airline.HasValue())
		//{
		//	continue;
		//}

		//airline
		CString strAirline(_T(""));
		strAirline.Format(_T("%s"), fltActItem.fltDesc._airline.GetValue());
		cxListCtrl.InsertItem(i, strAirline);
		cxListCtrl.SetItemData(i, fltActItem.ID);

		//arrival ID
		CString strArrivalID(_T(""));
		if (strlen(fltActItem.fltDesc.arrID) > 0)
		{
			strArrivalID.Format(_T("%s%s"), fltActItem.fltDesc._airline.GetValue(), fltActItem.fltDesc.arrID);
			cxListCtrl.SetItemText(i, 1,strArrivalID);
		}

		//depature ID
		CString strDepatureID(_T(""));
		if (strlen(fltActItem.fltDesc.depID) > 0)
		{
			strDepatureID.Format(_T("%s%s"), fltActItem.fltDesc._airline.GetValue(), fltActItem.fltDesc.depID);
			cxListCtrl.SetItemText(i, 2,strDepatureID);
		}

		//entry time
		ElapsedTime estEntryTime(fltActItem.entryTime);
		CString strEntryTime(_T(""));
		strEntryTime.Format(_T("Day%d %02d:%02d:%02d"), estEntryTime.GetDay(), estEntryTime.GetHour(), estEntryTime.GetMinute(), estEntryTime.GetSecond());
		cxListCtrl.SetItemText(i, 3,strEntryTime);

		//Exit time
		ElapsedTime estExitTime(fltActItem.exitTime);
		CString strExitTime(_T(""));
		strExitTime.Format(_T("Day%d %02d:%02d:%02d"), estExitTime.GetDay(), estExitTime.GetHour(), estExitTime.GetMinute(), estExitTime.GetSecond());
		cxListCtrl.SetItemText(i, 4,strExitTime);

		//duration time
		ElapsedTime estDurationTime(fltActItem.durationTime);
		CString strDurationTime(_T(""));
		strDurationTime.Format(_T("%s"), estDurationTime.printTime());
		cxListCtrl.SetItemText(i, 5,strDurationTime);
		cxListCtrl.SetCheckbox(i, 5, FALSE);

		//flight type
		cxListCtrl.SetItemText(i, 6,fltActItem.strFltType);
	}
}

BOOL CFlightDetailActivityResult::ExportListData(ArctermFile& _file,CParameters * parameter) 
{
	for (int i=0; i<(int)m_vResult.size(); i++)
	{
		FltActItem fltActItem = m_vResult[i];

		//airline
		CString strAirline(_T(""));
		strAirline.Format(_T("%s"), fltActItem.fltDesc._airline.GetValue());
		_file.writeField(strAirline);

		//arrival ID
		CString strArrivalID(_T(""));

		strArrivalID.Format(_T("%s%s"), fltActItem.fltDesc._airline.GetValue(), fltActItem.fltDesc.arrID);
		_file.writeField(strArrivalID);


		//depature ID
		CString strDepatureID(_T(""));
		strDepatureID.Format(_T("%s%s"), fltActItem.fltDesc._airline.GetValue(), fltActItem.fltDesc.depID);
		_file.writeField(strDepatureID);

		//entry time
		ElapsedTime estEntryTime(fltActItem.entryTime);
		CString strEntryTime(_T(""));
		strEntryTime.Format(_T("Day%d %02d:%02d:%02d"), estEntryTime.GetDay(), estEntryTime.GetHour(), estEntryTime.GetMinute(), estEntryTime.GetSecond());
		_file.writeField(strEntryTime);

		//Exit time
		ElapsedTime estExitTime(fltActItem.exitTime);
		CString strExitTime(_T(""));
		strExitTime.Format(_T("Day%d %02d:%02d:%02d"), estExitTime.GetDay(), estExitTime.GetHour(), estExitTime.GetMinute(), estExitTime.GetSecond());
		_file.writeField(strExitTime);

		//duration time
		ElapsedTime estDurationTime(fltActItem.durationTime);
		CString strDurationTime(_T(""));
		strDurationTime.Format(_T("%s"), estDurationTime.printTime());
		_file.writeField(strDurationTime);

		//flight type
		_file.writeField(fltActItem.strFltType);
		_file.writeLine() ;


		std::vector<CFlightDetailActivityResult::FltActTraceItem> fltActTraceItem = fltActItem.vTrace;

		int nRow = 0;
		for (int i=0; i<(int)fltActTraceItem.size(); i++)
		{
			CString strObjName = _T("NoResource");
			strObjName = GetObjectName(fltActTraceItem[i].nObjType,fltActTraceItem[i].nObjID);
			if(strObjName.CompareNoCase(_T("NoResource")) == 0)
				continue;
			_file.writeField(strObjName) ;
			ElapsedTime estEntryTime(fltActTraceItem[i].entryTime);
			_file.writeTime(estEntryTime,TRUE) ;
			_file.writeLine() ;
		}
	}
	return TRUE ;
}
CString CFlightDetailActivityResult::GetObjectName(int objectType,int objID)
{
	CString strObjName = _T("NoResource");
	if (objectType == 0)
	{
		ALTObject *pObject = ALTObject::ReadObjectByID(objID);
		if (pObject)
		{
			strObjName = pObject->GetObjNameString();
		}
		delete pObject;
	}
	else if(objectType == 1)
	{
		IntersectionNode intersecNode;
		intersecNode.ReadData(objID);
		strObjName = intersecNode.GetName();
	}
	else if(objectType == 3)
	{
		strObjName = _T("Entry System");
	}
	else if(objectType == 4)
	{
		strObjName = _T("Exit System");
	}
	else if (objectType == 5)
	{
		RunwayExit runwayExit;
		runwayExit.ReadData(objID);
		strObjName = runwayExit.GetIntesectionNode().GetName();
	}
	return strObjName;
}
BOOL CFlightDetailActivityResult::WriteReportData(ArctermFile& _file)
{
	_file.writeField("Flight Activity Report");
	_file.writeLine();

	_file.writeInt(ASReportType_Detail);
	_file.writeLine();

	int size = (int)m_vResult.size() ;
	_file.writeInt(size) ;
	_file.writeLine() ;
	for (int i = 0 ; i < size ; i++)
	{
		if(!m_vResult[i].ExportFile(_file))
			return FALSE ;
	}
	return TRUE ;
}

BOOL CFlightDetailActivityResult::ReadReportData(ArctermFile& _file)
{
	int size = 0 ;
	if(!_file.getInteger(size))
		return FALSE ;
	for (int i = 0 ; i < size ;i++)
	{
		_file.getLine() ;
		FltActItem  acitem ;
		acitem.ImportFile(_file) ;
		m_vResult.push_back(acitem) ;
	}
	return TRUE ;
}

BOOL CFlightDetailActivityResult::ExportReportData(ArctermFile& _file,CString& Errmsg)
{
	int size = (int)m_vResult.size() ;
	_file.writeInt(size) ;
	_file.writeLine() ;
	for (int i = 0 ; i < size ; i++)
	{
		if(!m_vResult[i].ExportFile(_file))
			return FALSE ;
	}
	return TRUE ;
}
BOOL  CFlightDetailActivityResult::ImportReportData(ArctermFile& _file,CString& Errmsg)
{
	int size = 0 ;
	if(!_file.getInteger(size))
		return FALSE ;
	for (int i = 0 ; i < size ;i++)
	{
		_file.getLine() ;
		FltActItem  acitem ;
		acitem.ImportFile(_file) ;
		m_vResult.push_back(acitem) ;
	}
	return TRUE ;
}
BOOL CFlightDetailActivityResult::FltActItem::ExportFile(ArctermFile& _file)
{
	_file.writeField(strFltType) ;
	_file.writeInt(entryTime) ;
	_file.writeInt(exitTime) ;
	_file.writeInt(durationTime) ;
	_file.writeLine() ;
	fltDesc.ExportFile(_file) ;

	int size = (int)vTrace.size() ;
	_file.writeInt(size) ;
	_file.writeLine() ;
	for (int i = 0 ; i < size ;i++)
	{
		vTrace[i].ExportFile(_file) ;
	}
	return TRUE ;
}
BOOL CFlightDetailActivityResult::FltActItem::ImportFile(ArctermFile& _file)
{
	TCHAR th[1024] = {0} ;
	_file.getField(th,1024) ;
	strFltType.Format(_T("%s"),th) ;
	_file.getInteger(entryTime) ;
	_file.getInteger(exitTime) ;
	_file.getInteger(durationTime) ;
	_file.getLine() ;
	fltDesc.ImportFile(_file) ;
	_file.getLine() ;

	int size = 0 ;
	if(!_file.getInteger(size) )
		return FALSE ;

	for (int i = 0 ; i < size ;i++)
	{
		_file.getLine() ;
		FltActTraceItem  traceItem ;
		traceItem.ImportFile(_file) ;
		vTrace.push_back(traceItem) ;
	}
	return TRUE ;
}
BOOL CFlightDetailActivityResult::FltActTraceItem::ExportFile(ArctermFile& _file)
{
	_file.writeInt(nObjType) ;
	_file.writeInt(nObjID) ;
	_file.writeInt(entryTime) ;
	_file.writeField(strName) ;
	_file.writeLine() ;
	return TRUE ;
}
BOOL CFlightDetailActivityResult::FltActTraceItem::ImportFile(ArctermFile& _file)
{
	_file.getInteger(nObjType) ;
	_file.getInteger(nObjID) ;
	_file.getInteger(entryTime) ;
	TCHAR th[1024] = {0} ;
	_file.getField(th,1024) ;
	strName.Format(_T("%s"),th) ;
	return TRUE ;
}
////////////////////////////////////////////////////////////////////////////////////////////////
CFlightSummaryActivityResult::CFlightSummaryActivityResult()
:m_pChartResult(NULL)
{

}

CFlightSummaryActivityResult::~CFlightSummaryActivityResult()
{

}

float CFlightSummaryActivityResult::CalcFlightRunDistance(int nFirst, int nSecond,AirsideFlightLogEntry& logEntry)
{
	float fTotalDistance = 0.0;
	AirsideFlightEventStruct firstEvent = logEntry.getEvent(nFirst);
	for (int i = nFirst+1; i <= nSecond; i++)
	{
		AirsideFlightEventStruct secondEvent = logEntry.getEvent(i);
		if (secondEvent.mode != OnTerminate)
		{
			float fDisX = secondEvent.x - firstEvent.x;
			float fDisY = secondEvent.y - firstEvent.y;
			float fDisZ = secondEvent.z - firstEvent.z;
			float fDistance = sqrt(fDisX*fDisX + fDisY*fDisY + fDisZ*fDisZ);
			fTotalDistance += fDistance;
			firstEvent = secondEvent;
		}
	}
	return fTotalDistance;
}

bool CFlightSummaryActivityResult::IsInvalid(int nLastEvent,AirsideFlightLogEntry& logEntry)
{
	AirsideFlightEventStruct LastEvent = logEntry.getEvent(nLastEvent);
	if (LastEvent.mode != OnTerminate)
	{
		return true;
	}

	return false;
}

int CFlightSummaryActivityResult::GetAccEventCount(int nEvent,AirsideFlightLogEntry& logEntry,int nAcc)
{
	int nCount = 0;
	int nFirstEvent = nEvent++;
	while(nEvent < logEntry.getCount())
	{
		AirsideFlightEventStruct FirstEvent = logEntry.getEvent(nFirstEvent);
		AirsideFlightEventStruct NextEvent = logEntry.getEvent(nEvent);
		double nDeta = NextEvent.speed - FirstEvent.speed;
		if (!(abs(nDeta) > 0.000001 && nAcc * nDeta > 0) || FirstEvent.mode != NextEvent.mode)
		{
			break;
		}
		nFirstEvent = nEvent++;
		nCount++;
	}
	return nCount;
}

void CFlightSummaryActivityResult::GenerateResult(CBGetLogFilePath pCBGetLogFilePath,CParameters *pParameter)
{
	m_vResult.clear();

	CFlightActivityParam *pParam = (CFlightActivityParam *)pParameter;
	ASSERT(pParam != NULL);

	long lStartTime = pParam->getStartTime().asSeconds()*100;
	long lEndTime = pParam->getEndTime().asSeconds()*100;

	//read log
	CString strDescFilepath = (*pCBGetLogFilePath)(AirsideDescFile);
	CString strEventFilePath = (*pCBGetLogFilePath)(AirsideEventFile);
	if (strEventFilePath.IsEmpty() || strDescFilepath.IsEmpty())
		return;

	EventLog<AirsideFlightEventStruct> airsideFlightEventLog;
	AirsideFlightLog fltLog;
	fltLog.SetEventLog(&airsideFlightEventLog);
	fltLog.LoadDataIfNecessary(strDescFilepath);
	fltLog.LoadEventDataIfNecessary(strEventFilePath);

	int nFltLogCount = fltLog.getCount();
	for (int i =0; i < nFltLogCount; ++i)
	{
		AirsideFlightLogEntry logEntry;
		logEntry.SetEventLog(&airsideFlightEventLog);
		fltLog.getItem(logEntry,i);
		AirsideFlightDescStruct	fltDesc = logEntry.GetAirsideDescStruct();
		FlightConstraint fltCons;
		FLTCNSTR_MODE fltConMode;
		if (!fits(pParameter,fltDesc,fltCons,fltConMode))
			continue;

		if (fltDesc.startTime == 0 && fltDesc.endTime == 0)
			continue;

		if (fltDesc.endTime < lStartTime || fltDesc.startTime>lEndTime)//not in the time range
			continue;

		//new report item
		FlightActItem fltArrAct;
		FlightActItem fltDepAct;

		fltArrAct.ID = (int)m_vResult.size();
		fltArrAct.fltDesc = fltDesc;

		fltDepAct.ID = (int)m_vResult.size();
		fltDepAct.fltDesc = fltDesc;

		int nStartEvent = 0;
		int nEventCount = logEntry.getCount();
		bool bExist = false;
		for (int nEvent = nEventCount-1; nEvent >= 0; --nEvent)
		{
			AirsideFlightEventStruct event = logEntry.getEvent(nEvent);
			if (event.nodeNO != -1)
			{
				if (event.state == 'A')
				{
					fltArrAct.EndPostion = event.nodeNO;
					fltArrAct.endTime = event.time;
				}
				else
				{
					fltDepAct.EndPostion = event.nodeNO;
					fltDepAct.endTime = event.time;
				}
				bExist = true;
				break;
			}
		}

		if (!bExist)
		{
			continue;
		}

		bExist = false;
		for (int mEvent = 0; mEvent < nEventCount; mEvent++)
		{
			AirsideFlightEventStruct event = logEntry.getEvent(mEvent);
			if (event.nodeNO != -1)
			{
				if (event.state == 'A')
				{
					fltArrAct.StartPosition = event.nodeNO;
					fltArrAct.startTime = event.time;
				}
				else
				{
					fltDepAct.StartPosition = event.nodeNO;
					fltDepAct.startTime = event.time;
				}

				nStartEvent = mEvent;
				bExist = true;
				break;
			}
		}
		if (!bExist)
		{
			continue;
		}

		int nTranfer = 0;
		for (int nEvent = 1; nEvent < nEventCount; nEvent++)
		{
			AirsideFlightEventStruct preEvent = logEntry.getEvent(nEvent-1);
			AirsideFlightEventStruct curEvent = logEntry.getEvent(nEvent);
			if (curEvent.nodeNO != -1 && curEvent.state == 'D' && preEvent.state == 'A')
			{
				fltArrAct.EndPostion = curEvent.nodeNO;
				fltArrAct.endTime = curEvent.time;

				fltDepAct.StartPosition = curEvent.nodeNO;
				fltDepAct.startTime = curEvent.time;
				nTranfer = nEvent;
				break;
			}
		}
		int nFirstEvent = nStartEvent;
		int nNextEvent = nFirstEvent;
		int nPreCount = 0;
		int nCurCount = 0;
		while ( nNextEvent < nEventCount)
		{
			AirsideFlightEventStruct firstEvent = logEntry.getEvent(nFirstEvent);
			AirsideFlightEventStruct nextEvent = logEntry.getEvent(nNextEvent);
			
			if (abs(firstEvent.speed - nextEvent.speed) > 0.000001 && abs(firstEvent.time - nextEvent.time) > 0.000001)
			{
				VTItem item;
				item.tTime = firstEvent.time;

				ATItem atItem;

				int nAcc = 1;//1 acc -1 dec
				if (nextEvent.speed - firstEvent.speed < 0)
				{
					nAcc = -1;
				}
				
				float fDistance = 0.0;
				nCurCount = GetAccEventCount(nNextEvent,logEntry,nAcc);

				if (nFirstEvent == nStartEvent)
				{
					fDistance = CalcFlightRunDistance(nFirstEvent,nNextEvent - 1,logEntry);
				}
				else
				{
					fDistance = CalcFlightRunDistance(nFirstEvent - nPreCount - 1, nNextEvent -1,logEntry);
				}

				AirsideFlightEventStruct event = logEntry.getEvent(nNextEvent + nCurCount);
				double dDistance = ARCUnit::ConvertLength(fDistance, ARCUnit::CM, ARCUnit::M);
				item.dSpeed = ARCUnit::ConvertVelocity(firstEvent.speed,ARCUnit::CMpS,ARCUnit::KNOT);
				item.dDistance = dDistance;

				AirsideFlightEventStruct midEvent = logEntry.getEvent(nNextEvent - 1);
				atItem.startTime = midEvent.time;
				atItem.endTime = event.time;
				double accTime = (event.time - midEvent.time) / 100.0;
				double dAcc = ARCUnit::ConvertVelocity(event.speed - midEvent.speed,ARCUnit::CMpS,ARCUnit::MpS)/accTime;
				atItem.dAcc = ARCUnit::ConvertLength(dAcc,ARCUnit::M,ARCUnit::FEET);
				
				if (firstEvent.state == 'A' && event.state == 'A')
				{
					fltArrAct.vItem.push_back(item);
					fltArrAct.vATItem.push_back(atItem);
				}
				else if(firstEvent.state == 'A' && event.state == 'D')
				{
					VTItem depVTItem;
					ATItem depATItem;

					AirsideFlightEventStruct tranferEvent = logEntry.getEvent(nTranfer);
					AirsideFlightEventStruct preEvent = logEntry.getEvent(nNextEvent - 1);
					double depDistance = CalcFlightRunDistance(nTranfer,nNextEvent - 1,logEntry);
					depVTItem.dSpeed = ARCUnit::ConvertVelocity(firstEvent.speed,ARCUnit::CMpS,ARCUnit::KNOT);
					depVTItem.dDistance = ARCUnit::ConvertLength(depDistance, ARCUnit::CM, ARCUnit::M);
					depVTItem.tTime = tranferEvent.time;
					double accTime = (event.time - preEvent.time) / 100.0;
					double dAcc = ARCUnit::ConvertVelocity(event.speed - preEvent.speed,ARCUnit::CMpS,ARCUnit::MpS)/accTime;
					depATItem.dAcc = ARCUnit::ConvertLength(dAcc,ARCUnit::M,ARCUnit::FEET);
					depATItem.startTime = preEvent.time;
					depATItem.endTime = event.time;
					fltDepAct.vItem.push_back(depVTItem);
					fltDepAct.vATItem.push_back(depATItem);

					VTItem arrVTItem;
					ATItem arrATItem;
					arrVTItem.dDistance = ARCUnit::ConvertLength(fDistance, ARCUnit::CM, ARCUnit::M) - depDistance;
					arrVTItem.dSpeed =  ARCUnit::ConvertVelocity(firstEvent.speed,ARCUnit::CMpS,ARCUnit::KNOT);
					arrVTItem.tTime = firstEvent.time;
					arrATItem.dAcc = 0.0;
					arrATItem.startTime = firstEvent.time;
					arrATItem.endTime = tranferEvent.time;
					fltArrAct.vItem.push_back(arrVTItem);
					fltArrAct.vATItem.push_back(arrATItem);

					arrVTItem.dDistance = 0.0;
					arrVTItem.dSpeed = 0.0;
					arrVTItem.tTime = tranferEvent.time;
					arrATItem.dAcc = 0.0;
					arrATItem.startTime = tranferEvent.time;
					arrATItem.endTime = tranferEvent.time;
					fltArrAct.vItem.push_back(arrVTItem);
					fltArrAct.vATItem.push_back(arrATItem);
				}
				else
				{
					fltDepAct.vItem.push_back(item);
					fltDepAct.vATItem.push_back(atItem);
				}
				nNextEvent += nCurCount;
				nFirstEvent = nNextEvent;
				nPreCount = nCurCount;
			}
			else
			{
				nNextEvent++;
			}
		}
	
		if (IsInvalid(nEventCount - 1,logEntry))
		{
			continue;
		}

		if (nFirstEvent != nEventCount - 1)
		{
			AirsideFlightEventStruct curEvent = logEntry.getEvent(nFirstEvent);
			AirsideFlightEventStruct preEvent = logEntry.getEvent(nFirstEvent - 1);
			AirsideFlightEventStruct lastEvent = logEntry.getEvent(nEventCount - 1);
			float fLastDistance = 0.0;
			VTItem item;
			item.tTime = curEvent.time;
			item.dSpeed = ARCUnit::ConvertVelocity(curEvent.speed,ARCUnit::CMpS,ARCUnit::KNOT);
			fLastDistance = CalcFlightRunDistance(nFirstEvent,nEventCount - 1,logEntry);
			item.dDistance = ARCUnit::ConvertLength(fLastDistance, ARCUnit::CM, ARCUnit::M);

			ATItem atItem;
			atItem.startTime = preEvent.time;
			atItem.endTime = curEvent.time;
			double accTime = (curEvent.time - preEvent.time) / 100.0;
			double dAcc = ARCUnit::ConvertVelocity(curEvent.speed - preEvent.speed,ARCUnit::CMpS,ARCUnit::MpS)/(accTime*100);
			atItem.dAcc = ARCUnit::ConvertLength(dAcc,ARCUnit::M,ARCUnit::FEET);

			VTItem lastItem;
			lastItem.tTime = lastEvent.time;
			lastItem.dSpeed = 0.0;
			lastItem.dDistance = 0.0;

			ATItem atlastItem;
			atlastItem.startTime = lastItem.tTime;
			atlastItem.endTime = lastItem.tTime;
			atlastItem.dAcc = 0.0;

			if (preEvent.state == 'A')
			{
				fltArrAct.vItem.push_back(item);
				fltArrAct.vATItem.push_back(atItem);

				fltArrAct.vItem.push_back(lastItem);
				fltArrAct.vATItem.push_back(atlastItem);
			}
			else
			{
				fltDepAct.vItem.push_back(item);
				fltDepAct.vATItem.push_back(atItem);
			}
		}
	
		//arrival
		if (strlen(fltArrAct.fltDesc.arrID) > 0 && fltArrAct.vItem.size() > 0 && fltConMode != ENUM_FLTCNSTR_MODE_DEP)
		{
			fltArrAct.bArrOrDeplActivity = true;
			AddToResult(pParam, fltArrAct);
		}
		//departure
		if (strlen(fltDepAct.fltDesc.depID) > 0 && fltDepAct.vItem.size() > 0 && fltConMode != ENUM_FLTCNSTR_MODE_ARR)
		{
			fltDepAct.bArrOrDeplActivity = false;
			AddToResult(pParam, fltDepAct);
		}
	}
	RefreshReport(pParameter);
}

void CFlightSummaryActivityResult::RefreshReport(CParameters * parameter)
{
	CFlightActivityParam* pParam = (CFlightActivityParam*)parameter;
	ASSERT(pParam != NULL);

	if (NULL != m_pChartResult)
	{
		delete m_pChartResult;
		m_pChartResult = NULL;
	}

	switch(pParam->getSubType())
	{
	case ACTIVITY_RESULT_VT:
		{
			m_pChartResult = new CAirsideFlightSummaryActivityReportVTResult;
			CAirsideFlightSummaryActivityReportVTResult* pResult = (CAirsideFlightSummaryActivityReportVTResult*)m_pChartResult;
			pResult->GenerateResult(m_vResult, parameter);
		}
		break;

	case ACTIVITY_RESULT_DT:
		{
			m_pChartResult = new CAirsideFlightSummaryActivityReportDTResult;
			CAirsideFlightSummaryActivityReportDTResult* pResult = (CAirsideFlightSummaryActivityReportDTResult*)m_pChartResult;
			pResult->GenerateResult(m_vResult, parameter);
		}
		break;

	case ACTIVITY_RESULT_AT:
		{
			m_pChartResult = new CAirsideFlightSummaryActivityReportATResult;
			CAirsideFlightSummaryActivityReportATResult* pResult = (CAirsideFlightSummaryActivityReportATResult*)m_pChartResult;
			pResult->GenerateResult(m_vResult, parameter);
		}
		break;
	default:
		break;
	}
}

void CFlightSummaryActivityResult::AddToResult(CParameters* param, FlightActItem& fltActItem)
{
	ASSERT(param != NULL);

	int nFltConsCount = (int)param->getFlightConstraintCount();
	for(int i=0; i<nFltConsCount; i++)
	{
		if (param->getFlightConstraint(i).fits(fltActItem.fltDesc))
		{
			m_vResult.push_back(fltActItem);
			break;
		}
	}
}

bool CFlightSummaryActivityResult::fits(CParameters * parameter,const AirsideFlightDescStruct& fltDesc,FlightConstraint& fltCons,FLTCNSTR_MODE& fltConMode)
{
	bool bArrFit = false;
	bool bDepFit = false;
	size_t nFltConsCount = parameter->getFlightConstraintCount();
	for (size_t nfltCons =0; nfltCons < nFltConsCount; ++ nfltCons)
	{
		if(parameter->getFlightConstraint(nfltCons).fits(fltDesc))
		{
			fltCons = parameter->getFlightConstraint(nfltCons);
			switch (fltCons.GetFltConstraintMode())
			{
			case ENUM_FLTCNSTR_MODE_ALL:
				{
					fltConMode = ENUM_FLTCNSTR_MODE_ALL;
					return true;
				}
				break;
			case ENUM_FLTCNSTR_MODE_ARR:
				{
					bArrFit = true;
				}
				break;
			case ENUM_FLTCNSTR_MODE_DEP:
				{
					bDepFit = true;
				}
				break;
			default:
				{
// 					ASSERT(FALSE);
				}
				break;
			}
			if (bArrFit && bDepFit)
			{
				fltConMode = ENUM_FLTCNSTR_MODE_ALL;
				return true;
			}
		}
	}
	if (bArrFit)
	{
		fltConMode = ENUM_FLTCNSTR_MODE_ARR;
		return true;
	}
	if (bDepFit)
	{
		fltConMode = ENUM_FLTCNSTR_MODE_DEP;
		return true;
	}

	return bArrFit || bDepFit;
}

void CFlightSummaryActivityResult::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{
	if (m_pChartResult)
	{
		static const int s_nDrawingCurveCheckCount = 50;
		if (m_pChartResult->GetActivityItemCount()>s_nDrawingCurveCheckCount)
		{
			CString strText;
			strText.Format(
				_T("You are going to draw %d flights on the graph,\n")
				_T("but for good looking, the count of flights is better no more than %d.\n")
				_T("Click OK to continue, or cancel to give up.")
				, m_pChartResult->GetActivityItemCount()
				, s_nDrawingCurveCheckCount
				);
			if (IDOK != AfxMessageBox(strText, MB_OKCANCEL))
			{
				return;
			}
		}
		m_pChartResult->Draw3DChart(chartWnd,pParameter);
	}
}

void CFlightSummaryActivityResult::InitListHead(CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType, CSortableHeaderCtrl* piSHC)
{
	cxListCtrl.DeleteAllItems();

	while(cxListCtrl.GetHeaderCtrl()->GetItemCount() >0)
		cxListCtrl.DeleteColumn(0);

	cxListCtrl.InsertColumn(0, _T("Airline"), LVCFMT_LEFT, 50);	
	cxListCtrl.InsertColumn(1, _T("ID"), LVCFMT_LEFT, 50);
	cxListCtrl.InsertColumn(2, _T("Operation"), LVCFMT_LEFT, 60);
	cxListCtrl.InsertColumn(3, _T("T start"),LVCFMT_LEFT,100);
	cxListCtrl.InsertColumn(4, _T("Location"),LVCFMT_LEFT,100);
	cxListCtrl.InsertColumn(5, _T("T end"),LVCFMT_LEFT,100);
	cxListCtrl.InsertColumn(6, _T("Location"),LVCFMT_LEFT,120);
	CString StrSpeed ;
	StrSpeed = CARCVelocityUnit::GetVelocityUnitString(CARCUnitManager::GetInstance().GetCurSelVelocityUnit()) ;

	CString StrLength ;
	StrLength =CARCLengthUnit::GetLengthUnitString(CARCUnitManager::GetInstance().GetCurSelLengthUnit()) ;
	
	if (piSHC)
	{
		piSHC->ResetAll();
		piSHC->SetDataType(0,dtSTRING);
		piSHC->SetDataType(1,dtSTRING);
		piSHC->SetDataType(2,dtSTRING);
		piSHC->SetDataType(3,dtSTRING);
		piSHC->SetDataType(4,dtSTRING);
		piSHC->SetDataType(5,dtSTRING);
		piSHC->SetDataType(6,dtSTRING);
	}

	for (int i = 0; i < 100; i++)
	{
		CString strTValue = _T("");
		CString strDValue = _T("");
		strTValue.Format(_T("T%d(%s)"),i+1,StrSpeed);
		strDValue.Format(_T("D%d(%s)"),i+1,StrLength);
		cxListCtrl.InsertColumn(7+2*i,strTValue,LVCFMT_LEFT,70);
		cxListCtrl.InsertColumn(7+2*i+1,strDValue,LVCFMT_LEFT,70);
		if (piSHC)
		{
			piSHC->SetDataType(7+2*i,dtDEC);
			piSHC->SetDataType(7+2*i+1,dtDEC);
		}
	}
}

void CFlightSummaryActivityResult::FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter)
{
	cxListCtrl.DeleteAllItems();
	int nIndex = 0;
	for (int i=0; i<(int)m_vResult.size(); i++)
	{
		FlightActItem fltActItem = m_vResult[i];


		int nAdd = ((int)(fltActItem.vItem.size()) % 100 != 0)? 1:0;
		int nCount = ((int)fltActItem.vItem.size()) / 100 + nAdd;
		
		for (int j = 0; j < nCount; j++)
		{
			nIndex = i+j;
			if (fltActItem.bArrOrDeplActivity)
			{
				//airline
				CString strAirline(_T(""));
				strAirline.Format(_T("%s"), fltActItem.fltDesc._airline.GetValue());
				cxListCtrl.InsertItem(nIndex, strAirline);
				cxListCtrl.SetItemData(nIndex, fltActItem.ID);

				//flight ID
				CString strArrivalID(_T(""));
				if (strlen(fltActItem.fltDesc.flightID) > 0)
				{
					strArrivalID.Format(_T("%s%s"),fltActItem.fltDesc._airline.GetValue(),fltActItem.fltDesc.arrID);
					cxListCtrl.SetItemText(nIndex, 1,strArrivalID);
				}

				cxListCtrl.SetItemText(nIndex, 2, _T("A"));

				//T start
				ElapsedTime estStartPlan(long(fltActItem.startTime/100.0+0.5));
				CString strStartPlan(_T(""));
				strStartPlan.Format(_T("Day%d %02d:%02d:%02d"), estStartPlan.GetDay(), estStartPlan.GetHour(), estStartPlan.GetMinute(), estStartPlan.GetSecond());
				cxListCtrl.SetItemText(nIndex,3,strStartPlan);

				//location
				CString strArrLocation = _T("");
				strArrLocation = GetObjName(fltActItem.StartPosition);
				cxListCtrl.SetItemText(nIndex,4,strArrLocation);

				//T end
				ElapsedTime estEndPlan(long(fltActItem.endTime/100.0+0.5));
				CString strEndPlan(_T(""));
				strEndPlan.Format(_T("Day%d %02d:%02d:%02d"), estEndPlan.GetDay(), estEndPlan.GetHour(), estEndPlan.GetMinute(), estEndPlan.GetSecond());
				cxListCtrl.SetItemText(nIndex,5,strEndPlan);	

				//location
				CString strDepLocation(_T(""));
				strDepLocation = GetObjName(fltActItem.EndPostion);
				cxListCtrl.SetItemText(nIndex,6,strDepLocation);

				int nFitCount = 0;
				int nItemCount = (int)fltActItem.vItem.size();
				if ((j+1)*100 > nItemCount)
				{
					nFitCount = nItemCount;
				}
				else
				{
					nFitCount = (j+1)*100;
				}
				int m = 0;
				for (int n = j*100; n < nFitCount; n++)
				{
					CString strSpeed(_T(""));
					strSpeed.Format(_T("%.2f"),CARCVelocityUnit::ConvertVelocity(AU_VELOCITY_KTS,CARCUnitManager::GetInstance().GetCurSelVelocityUnit(),fltActItem.vItem[n].dSpeed));
					cxListCtrl.SetItemText(nIndex,7+2*m,strSpeed);

					CString strDistance(_T(""));
					strDistance.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(AU_LENGTH_METER,CARCUnitManager::GetInstance().GetCurSelLengthUnit(),fltActItem.vItem[n].dDistance));
					cxListCtrl.SetItemText(nIndex,8+2*m,strDistance);
					m++;
				}
			}
			else
			{
				//airline
				CString strAirline(_T(""));
				strAirline.Format(_T("%s"), fltActItem.fltDesc._airline.GetValue());
				cxListCtrl.InsertItem(nIndex, strAirline);
				cxListCtrl.SetItemData(nIndex, fltActItem.ID);
				//depature ID
				CString strDepatureID(_T(""));
				if (strlen(fltActItem.fltDesc.depID) > 0)
				{
					strDepatureID.Format(_T("%s%s"), fltActItem.fltDesc._airline.GetValue(), fltActItem.fltDesc.depID);
					cxListCtrl.SetItemText(nIndex, 1,strDepatureID);
				}

				cxListCtrl.SetItemText(nIndex, 2, _T("D"));

				//T start
				ElapsedTime estStartPlan(long(fltActItem.startTime/100.0+0.5));
				CString strStartPlan(_T(""));
				strStartPlan.Format(_T("Day%d %02d:%02d:%02d"), estStartPlan.GetDay(), estStartPlan.GetHour(), estStartPlan.GetMinute(), estStartPlan.GetSecond());
				cxListCtrl.SetItemText(nIndex,3,strStartPlan);

				//location
				CString strArrLocation = _T("");
				strArrLocation = GetObjName(fltActItem.StartPosition);
				cxListCtrl.SetItemText(nIndex,4,strArrLocation);

				//T end
				ElapsedTime estEndPlan(long(fltActItem.endTime/100.0+0.5));
				CString strEndPlan(_T(""));
				strEndPlan.Format(_T("Day%d %02d:%02d:%02d"), estEndPlan.GetDay(), estEndPlan.GetHour(), estEndPlan.GetMinute(), estEndPlan.GetSecond());
				cxListCtrl.SetItemText(nIndex,5,strEndPlan);	

				//location
				CString strDepLocation(_T(""));
				strDepLocation = GetObjName(fltActItem.EndPostion);
				cxListCtrl.SetItemText(nIndex,6,strDepLocation);

				int nFitCount = 0;
				int nItemCount = (int)fltActItem.vItem.size();
				if ((j+1)*100 > nItemCount)
				{
					nFitCount = nItemCount;
				}
				else
				{
					nFitCount = (j+1)*100;
				}

				int m = 0;
				for (int n = j*100; n < nFitCount; n++)
				{
					CString strSpeed(_T(""));
					strSpeed.Format(_T("%.2f"),CARCVelocityUnit::ConvertVelocity(AU_VELOCITY_KTS,CARCUnitManager::GetInstance().GetCurSelVelocityUnit(),fltActItem.vItem[n].dSpeed));
					cxListCtrl.SetItemText(nIndex,7+2*m,strSpeed);

					CString strDistance(_T(""));
					strDistance.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(AU_LENGTH_METER,CARCUnitManager::GetInstance().GetCurSelLengthUnit(),fltActItem.vItem[n].dDistance));
					cxListCtrl.SetItemText(nIndex,8+2*m,strDistance);
					m++;
				}
			}
		}
	}
	int cxCount = cxListCtrl.GetItemCount();
}

CString CFlightSummaryActivityResult::GetObjName(int nObjectID)
{
	CString strObjName = _T("");
	ALTObject *pObject = ALTObject::ReadObjectByID(nObjectID);
	if (pObject)
	{
		strObjName = pObject->GetObjNameString();
	}
	return strObjName;
}

BOOL CFlightSummaryActivityResult::WriteReportData(ArctermFile& _file)
{
	_file.writeField("Flight Activity Report");
	_file.writeLine();

	_file.writeInt(ASReportType_Summary);
	_file.writeLine();

	int size = (int)m_vResult.size() ;
	_file.writeInt(size) ;
	_file.writeLine() ;
	for (int i = 0 ; i < size ; i++)
	{
		if(!m_vResult[i].ExportFile(_file))
			return FALSE ;
	}
	return TRUE ;
}

BOOL CFlightSummaryActivityResult::ReadReportData(ArctermFile& _file)
{
	m_vResult.clear();
	int size = 0 ;
	if(!_file.getInteger(size))
		return FALSE ;
	for (int i = 0 ; i < size ;i++)
	{
		_file.getLine() ;
		FlightActItem  acitem ;
		acitem.ImportFile(_file) ;
		m_vResult.push_back(acitem) ;
	}
	return TRUE ;
}

BOOL CFlightSummaryActivityResult::ExportReportData(ArctermFile& _file,CString& Errmsg)
{
	_file.writeLine();
	int size = (int)m_vResult.size() ;
	_file.writeInt(size) ;
	_file.writeLine() ;
	for (int i = 0 ; i < size ; i++)
	{
		if(!m_vResult[i].ExportFile(_file))
			return FALSE ;
	}
	return TRUE ;
}

BOOL CFlightSummaryActivityResult::ImportReportData(ArctermFile& _file,CString& Errmsg)
{
	m_vResult.clear();
	int size = 0 ;
	if(!_file.getInteger(size))
		return FALSE ;
	for (int i = 0 ; i < size ;i++)
	{
		_file.getLine() ;
		FlightActItem  acitem ;
		acitem.ImportFile(_file) ;
		m_vResult.push_back(acitem) ;
	}
	return TRUE ;
}

BOOL CFlightSummaryActivityResult::ExportListData(ArctermFile& _file,CParameters * parameter)
{
	int nIndex = 0;
	for (int i=0; i<(int)m_vResult.size(); i++)
	{
		FlightActItem fltActItem = m_vResult[i];


		int nAdd = ((int)(fltActItem.vItem.size()) % 100 != 0)? 1:0;
		int nCount = (int)fltActItem.vItem.size() / 100 + nAdd;

		for (int j = 0; j < nCount; j++)
		{
			nIndex = i*nCount+j;
			if (fltActItem.bArrOrDeplActivity)
			{
				//airline
				CString strAirline(_T(""));
				strAirline.Format(_T("%s"), fltActItem.fltDesc._airline.GetValue());
				_file.writeField(strAirline);
				
				//flight ID
				CString strArrivalID(_T(""));
				if (strlen(fltActItem.fltDesc.flightID) > 0)
				{
					strArrivalID.Format(_T("%s%s"),fltActItem.fltDesc._airline.GetValue(),fltActItem.fltDesc.arrID);
					_file.writeField(strArrivalID);
				}

				_file.writeField(_T("A"));

				//T start
				ElapsedTime estStartPlan(long(fltActItem.startTime/100.0+0.5));
				CString strStartPlan(_T(""));
				strStartPlan.Format(_T("Day%d %02d:%02d:%02d"), estStartPlan.GetDay(), estStartPlan.GetHour(), estStartPlan.GetMinute(), estStartPlan.GetSecond());
				_file.writeField(strStartPlan);

				//location
				CString strArrLocation = _T("");
				strArrLocation = GetObjName(fltActItem.StartPosition);
				_file.writeField(strArrLocation);

				//T end
				ElapsedTime estEndPlan(long(fltActItem.endTime/100.0+0.5));
				CString strEndPlan(_T(""));
				strEndPlan.Format(_T("Day%d %02d:%02d:%02d"), estEndPlan.GetDay(), estEndPlan.GetHour(), estEndPlan.GetMinute(), estEndPlan.GetSecond());
				_file.writeField(strEndPlan);	

				//location
				CString strDepLocation(_T(""));
				strDepLocation = GetObjName(fltActItem.EndPostion);
				_file.writeField(strDepLocation);

				int nFitCount = 0;
				int nItemCount = (int)fltActItem.vItem.size();
				if ((j+1)*100 > nItemCount)
				{
					nFitCount = nItemCount - j*100;
				}
				else
				{
					nFitCount = (j+1)*100;
				}
				for (int n = j*100; n < nFitCount; n++)
				{
					CString strSpeed(_T(""));
					strSpeed.Format(_T("%.2f"),CARCVelocityUnit::ConvertVelocity(AU_VELOCITY_KTS,CARCUnitManager::GetInstance().GetCurSelVelocityUnit(),fltActItem.vItem[n].dSpeed));
					_file.writeField(strSpeed);

					CString strDistance(_T(""));
					strDistance.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(AU_LENGTH_METER,CARCUnitManager::GetInstance().GetCurSelLengthUnit(),fltActItem.vItem[n].dDistance));
					_file.writeField(strDistance);
				}
			}
			else
			{
				//airline
				CString strAirline(_T(""));
				strAirline.Format(_T("%s"), fltActItem.fltDesc._airline.GetValue());
				_file.writeField(strAirline);
				//depature ID
				CString strDepatureID(_T(""));
				if (strlen(fltActItem.fltDesc.depID) > 0)
				{
					strDepatureID.Format(_T("%s%s"), fltActItem.fltDesc._airline.GetValue(), fltActItem.fltDesc.depID);
					_file.writeField(strDepatureID);
				}

				_file.writeField(_T("D"));

				//T start
				ElapsedTime estStartPlan(long(fltActItem.startTime/100.0+0.5));
				CString strStartPlan(_T(""));
				strStartPlan.Format(_T("Day%d %02d:%02d:%02d"), estStartPlan.GetDay(), estStartPlan.GetHour(), estStartPlan.GetMinute(), estStartPlan.GetSecond());
				_file.writeField(strStartPlan);

				//location
				CString strArrLocation = _T("");
				strArrLocation = GetObjName(fltActItem.StartPosition);
				_file.writeField(strArrLocation);

				//T end
				ElapsedTime estEndPlan(long(fltActItem.endTime/100.0+0.5));
				CString strEndPlan(_T(""));
				strEndPlan.Format(_T("Day%d %02d:%02d:%02d"), estEndPlan.GetDay(), estEndPlan.GetHour(), estEndPlan.GetMinute(), estEndPlan.GetSecond());
				_file.writeField(strEndPlan);	

				//location
				CString strDepLocation(_T(""));
				strDepLocation = GetObjName(fltActItem.EndPostion);
				_file.writeField(strDepLocation);

				int nFitCount = 0;
				int nItemCount = (int)fltActItem.vItem.size();
				if ((j+1)*100 > nItemCount)
				{
					nFitCount = nItemCount - j*100;
				}
				else
				{
					nFitCount = (j+1)*100;
				}

				for (int n = j*100; n < nFitCount; n++)
				{
					CString strSpeed(_T(""));
					strSpeed.Format(_T("%.2f"),CARCVelocityUnit::ConvertVelocity(AU_VELOCITY_KTS,CARCUnitManager::GetInstance().GetCurSelVelocityUnit(),fltActItem.vItem[n].dSpeed));
					_file.writeField(strSpeed);

					CString strDistance(_T(""));
					strDistance.Format(_T("%.2f"),CARCLengthUnit::ConvertLength(AU_LENGTH_METER,CARCUnitManager::GetInstance().GetCurSelLengthUnit(),fltActItem.vItem[n].dDistance));
					_file.writeField(strDistance);
				}
			}
			_file.writeLine() ;
		}
	}
	return TRUE;
}

bool CFlightSummaryActivityResult::FlightActItem::operator <(const FlightActItem& item)const
{
	if (item.startTime >= startTime)
	{
		return false;
	}
	return true;
}


BOOL CFlightSummaryActivityResult::FlightActItem::ImportFile(ArctermFile& _file)
{
	int res = 0 ;
	_file.getInteger(res) ;
	bArrOrDeplActivity = res>0?true:false;
	_file.getInteger(startTime);
	_file.getInteger(endTime);
	_file.getInteger(StartPosition);
	_file.getInteger(EndPostion);
	_file.getLine();
	fltDesc.ImportFile(_file);
	_file.getLine();

	int size = 0 ;
	if(!_file.getInteger(size) )
		return FALSE ;

	_file.getLine() ;
	for (int i = 0 ; i < size ;i++)
	{
		VTItem  Item ;
		Item.ImportFile(_file);
		vItem.push_back(Item);
		_file.getLine();
	}

	int atSize = 0;
	if (!_file.getInteger(atSize))
		return FALSE;
	
	for (int ii = 0; ii < atSize; ii++)
	{
		_file.getLine();
		ATItem atItem;
		atItem.ImportFile(_file);
		vATItem.push_back(atItem);
	}

	return TRUE ;
}

BOOL CFlightSummaryActivityResult::FlightActItem::ExportFile(ArctermFile& _file)
{
	_file.writeInt(bArrOrDeplActivity?1:0) ;
	_file.writeInt(startTime);
	_file.writeInt(endTime);
	_file.writeInt(StartPosition);
	_file.writeInt(EndPostion);
	_file.writeLine();
	fltDesc.ExportFile(_file) ;

	int size = (int)vItem.size() ;
	_file.writeInt(size) ;
	_file.writeLine() ;
	for (int i = 0 ; i < size ;i++)
	{
		vItem[i].ExportFile(_file) ;
	}
	int atSize = (int)vATItem.size();
	_file.writeInt(atSize);
	_file.writeLine();
	for (int ii = 0; ii < atSize; ii++)
	{
		vATItem[ii].ExportFile(_file);
	}
	return TRUE ;
}

BOOL CFlightSummaryActivityResult::VTItem::ExportFile(ArctermFile& _file)
{
	_file.writeInt(tTime) ;
	_file.writeDouble(dDistance);
	_file.writeDouble(dSpeed);
	_file.writeLine();
	return TRUE ;
}

BOOL CFlightSummaryActivityResult::VTItem::ImportFile(ArctermFile& _file)
{
	_file.getInteger(tTime);
	_file.getFloat(dDistance);
	_file.getFloat(dSpeed);
	return TRUE ;
}

BOOL CFlightSummaryActivityResult::ATItem::ExportFile(ArctermFile& _file)
{
	_file.writeInt(startTime);
	_file.writeInt(endTime);
	_file.writeDouble(dAcc);
	_file.writeLine();
	return TRUE;
}

BOOL CFlightSummaryActivityResult::ATItem::ImportFile(ArctermFile& _file)
{
	_file.getInteger(startTime);
	_file.getInteger(endTime);
	_file.getFloat(dAcc);
	return TRUE;
}

void CFlightSummaryActivityResult::SetReportResult(CFlightActivityBaseResult* pResult)
{
	m_vResult.clear();
	if (pResult && pResult->GetReportType() == ASReportType_Summary)
	{
		m_vResult = ((CFlightSummaryActivityResult*)pResult)->m_vResult;
	}
}
