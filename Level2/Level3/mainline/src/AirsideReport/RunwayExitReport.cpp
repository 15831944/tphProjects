#include "StdAfx.h"
#include ".\runwayexitreport.h"
#include "RunwayExitParameter.h"
#include "RunwayExitResult.h"
#include "../Results/AirsideFlightEventLog.h"
#include "../Reports/StatisticalTools.h"
#include "../Results/AirsideFlightLogItem.h"
#include "../InputAirside/Runway.h" 
#include "../InputAirside/RunwayExit.h"
//////////////////////////////////////////////////////////////////////////
int CRunwayExitRepDataItem::GetMaxUsager(CString& _whichsegment)
{
		CTimeIntervalFlightData* PTimeIntervalData = NULL ;
		int maxusager = 0 ;
		int ndx = 0 ;
		for (int i = 0 ; i < (int)m_RunwayExitData.size() ;i++)
		{
			PTimeIntervalData = m_RunwayExitData[i] ;
			if((int)PTimeIntervalData->m_FlightData.size() > maxusager )
			{
				maxusager = (int)PTimeIntervalData->m_FlightData.size() ;
				ndx = i ;
			}
		}
		if(ndx >= 0 && ndx < (int)m_RunwayExitData.size())
		{
			 PTimeIntervalData = m_RunwayExitData[ndx] ;
			_whichsegment.Format(_T("%s-%s"),PTimeIntervalData->m_startTime.printTime(),PTimeIntervalData->m_endTime.printTime() ) ;
		}
		return maxusager ;
}
int CRunwayExitRepDataItem::GetMinUsager(CString& _whichsegment)
{
	CTimeIntervalFlightData* PTimeIntervalData = NULL ;
	int maxusager = 0xFFFF ;
	int ndx = 0 ;
	for (int i = 0 ; i < (int)m_RunwayExitData.size() ;i++)
	{
		PTimeIntervalData = m_RunwayExitData[i] ;
		if((int)PTimeIntervalData->m_FlightData.size() < maxusager )
		{
			maxusager = (int)PTimeIntervalData->m_FlightData.size() ;
			ndx = i ;
		}
	}
	if(ndx >= 0 && ndx < (int)m_RunwayExitData.size())
	{
		 PTimeIntervalData = m_RunwayExitData[ndx] ;
		_whichsegment.Format(_T("%s-%s"),PTimeIntervalData->m_startTime.printTime(),PTimeIntervalData->m_endTime.printTime() ) ;
	}
	return maxusager ;
}

int CRunwayExitRepDataItem::GetMeanUsager()
{
	int valUsger = 0 ;
	int ndx = 0 ;
	CTimeIntervalFlightData* PTimeIntervalData = NULL ;
	for (int i = 0 ; i < (int)m_RunwayExitData.size() ;i++)
	{
		PTimeIntervalData = m_RunwayExitData[i] ;
		valUsger += (int)PTimeIntervalData->m_FlightData.size() ;
	}
	if(m_RunwayExitData.empty())
		return valUsger ;
	return valUsger/(int)m_RunwayExitData.size() ;
}

//////////////////////////////////////////////////////////////////////////
CRunwayExitRepDataItem::~CRunwayExitRepDataItem()
{
	for (int i = 0 ; i < (int)m_RunwayExitData.size() ;i++)
	{
		delete m_RunwayExitData[i] ;
	}
	m_RunwayExitData.clear() ;
}

void CRunwayExitRepDataItem::AddFlightEvenLogData(const CFlightFliterData& _data)
{
	CTimeIntervalFlightData* PTimeIntervalData = NULL ;
	for (int i = 0 ; i < (int)m_RunwayExitData.size() ;i++)
	{
		PTimeIntervalData = m_RunwayExitData[i] ;
		if(PTimeIntervalData->m_startTime<= _data.m_EntryTime && _data.m_EntryTime < PTimeIntervalData->m_endTime)
			PTimeIntervalData->AddData(_data) ;
	}
}
void CRunwayExitRepDataItem::InitTimeIntervalByParamter(CRunwayExitParameter* _parameter)
{
	if(_parameter == NULL)
		return  ;
	int intervalSize = 0 ;
	ElapsedTime IntervalTime ;
	IntervalTime = (_parameter->getEndTime() - _parameter->getStartTime()) ;
	intervalSize = IntervalTime.asSeconds() / _parameter->getInterval() ;

	CTimeIntervalFlightData* PTimeIntervalData = NULL ;
	IntervalTime = _parameter->getStartTime() ;
	for (int i = 0 ; i < intervalSize ; i++)
	{
		PTimeIntervalData = new CTimeIntervalFlightData;
		PTimeIntervalData->m_startTime = IntervalTime ;
		PTimeIntervalData->m_endTime = IntervalTime + ElapsedTime(_parameter->getInterval()) ;
		IntervalTime = PTimeIntervalData->m_endTime ;
		m_RunwayExitData.push_back(PTimeIntervalData) ;
	}
}
//////////////////////////////////////////////////////////////////////////
CRunwayExitRepData::CRunwayExitRepData()
{

}
CRunwayExitRepData::~CRunwayExitRepData()
{
	for (int i = 0 ; i < (int)m_Data.size() ;i++)
	{
		delete m_Data[i] ;
	}
	m_Data.clear() ;
}
void CRunwayExitRepData::ClearAll() 
{
	for (int i = 0 ; i < (int)m_Data.size() ;i++)
	{
		delete m_Data[i] ;
	}
	m_Data.clear() ;
}
CRunwayExitRepDataItem* CRunwayExitRepData::FindRunwayExitRepDataItem(int _runwayID ,int _exitID)
{
	for (int i = 0 ; i < (int)m_Data.size() ;i++ )
	{
		if(m_Data[i]->m_RunwayID == _runwayID && m_Data[i]->m_RunwayExitID == _exitID)
			return m_Data[i] ;
	}
	return NULL ;
}
void CRunwayExitRepData::AddFlightEvenlog(const CFlightFliterData& _data,CRunwayExitParameter* _parameter)
{
	CRunwayExitRepDataItem* PRunwayExitRepData = FindRunwayExitRepDataItem(_data.m_RunwayID,_data.m_RunwayExitID) ;

	if(PRunwayExitRepData == NULL)
	{
		PRunwayExitRepData = new CRunwayExitRepDataItem(_data.m_RunwayID,_data.m_RunwayExitID) ;
		PRunwayExitRepData->m_RunwayExitName = _data.m_RunwayExitName ;
		PRunwayExitRepData->m_RunwayName = _data.m_RunwayName ;
		PRunwayExitRepData->InitTimeIntervalByParamter(_parameter) ;
		m_Data.push_back(PRunwayExitRepData) ;
	}

	PRunwayExitRepData->AddFlightEvenLogData(_data) ;
}

void CRunwayExitRepData::ExportFile( ArctermFile& _file )
{
	_file.writeInt((int)m_Data.size()) ;
	_file.writeLine() ;
	for (int i = 0 ; i < (int)m_Data.size() ;i++)
	{
		m_Data[i]->ExportFile(_file) ;
	}
}

void CRunwayExitRepData::ImportFile( ArctermFile& _file )
{
	int size = 0 ;
	if(!_file.getInteger(size))
		return ;
	_file.getLine() ;
	for(int i = 0 ; i < size ;i++)
	{
		CRunwayExitRepDataItem* pdata  = new CRunwayExitRepDataItem(-1,-1) ;
		pdata->ImportFile(_file) ;
		m_Data.push_back(pdata) ;
	}
}
//////////////////////////////////////////////////////////////////////////
BOOL CRunwayExitReport::HandetheRunwayExitFlightLog (CRunwayExitParameter* _parameter,const ARCEventLog* _evenlog,AirsideFlightLogDesc& fltdesc)
{
	BOOL res = TRUE ;
	if (!_evenlog->IsA(typeof(AirsideFlightEventLog)))
	{
		res = FALSE ;
		return res ;
	}
	AirsideFlightEventLog* pConflictLog = (AirsideFlightEventLog*)_evenlog;
	if(pConflictLog->mMode != OnExitRunway && pConflictLog->mMode != OnTakeOffEnterRunway)
	{
		res = FALSE ;
		return res ;
	}
	ElapsedTime tTime;
	tTime.setPrecisely(pConflictLog->time) ;
	if (tTime < _parameter->getStartTime() || tTime > _parameter->getEndTime())
	{
		res = FALSE ;
		return res ;
	}
	CRunwayExitItem*  PExitItem = NULL ;
	CRunwayExitItem*  DeExit = NULL;
	PExitItem = _parameter->FindItemByExitID(pConflictLog->m_ExitId) ;
	if(PExitItem == NULL)
	{
		if(_parameter->m_RunwayData.empty())
		{
			if(pConflictLog->resDesc.resType == AirsideResource::ResType_LogicRunway)
			{
				PExitItem = new CRunwayExitItem() ;
				DeExit = PExitItem ;
				Runway runway ;
				runway.ReadObject(pConflictLog->resDesc.resid) ;
				RunwayExit exit ;
				exit.ReadData(pConflictLog->m_ExitId) ;

				PExitItem->SetExitID(pConflictLog->m_ExitId) ;
				PExitItem->SetExitName(exit.GetName()) ;
				PExitItem->SetRunwayID(pConflictLog->resDesc.resid) ;
				PExitItem->SetRunwayName(runway.GetObjectName().GetIDString()) ;
				PExitItem->SetOperationType(_parameter->m_AllOper) ;

			}else
				return FALSE ;
		}else
			return FALSE;
	}
	CRunwayExitItem::Operation_type Operation = PExitItem->GetOperationType() ;

	if(Operation == CRunwayExitItem::Takeoff && pConflictLog->mMode != OnTakeOffEnterRunway)
		return FALSE ;
	if(Operation == CRunwayExitItem::Landing && pConflictLog->mMode != OnExitRunway)
		return FALSE ;
	CFlightFliterData RunwayExitRepData ;

	RunwayExitRepData.m_RunwayID = PExitItem->GetRunwayID() ;
	RunwayExitRepData.m_RunwayName = PExitItem->GetRunwayName() ;
	RunwayExitRepData.m_RunwayExitID = PExitItem->GetExitID() ;
	RunwayExitRepData.m_RunwayExitName = PExitItem->GetExitName() ;
	RunwayExitRepData.m_EntryTime.setPrecisely(pConflictLog->time) ;
	RunwayExitRepData.m_Operation = CRunwayExitItem::GetOperationName(Operation) ;



	if (pConflictLog->state ==  AirsideFlightEventLog::ARR)
		RunwayExitRepData.m_AcID.Format("%s%s",fltdesc.sAirline.c_str(),fltdesc.sArrID.c_str());
	else
		RunwayExitRepData.m_AcID.Format("%s%s",fltdesc.sAirline.c_str(),fltdesc.sDepID.c_str());

	RunwayExitRepData.m_FlightType.Format(_T("%s"), fltdesc.sAcType.c_str()) ;

	m_FilterEventLog.push_back(RunwayExitRepData) ;

	if(DeExit)
		delete DeExit ;
	return TRUE ;
}
//////////////////////////////////////////////////////////////////////////

CRunwayExitReport::CRunwayExitReport(CBGetLogFilePath pFunc,CRunwayExitParameter* _parameter):CAirsideBaseReport(pFunc),m_parameter(_parameter),m_BaseResult(NULL)
{
}

CRunwayExitReport::~CRunwayExitReport(void)
{
}
void CRunwayExitReport::GenerateReport(CParameters * parameter)
{
	m_ReportBaseData.ClearAll() ;
	m_FilterEventLog.clear() ;
	m_SummaryData.clear() ;
	CRunwayExitParameter* PtaxiwayDelayPar = (CRunwayExitParameter*)parameter ;
	

	//read log
	CString strDescFilepath = (*m_pCBGetLogFilePath)(AirsideFlightDescFileReport);
	CString strEventFilePath = (*m_pCBGetLogFilePath)(AirsideFlightEventFileReport);

	//read flight log 
	ARCBaseLog<AirsideFlightLogItem> mFlightLogData;
	mFlightLogData.OpenInput(strDescFilepath,strEventFilePath);
	int nCount = mFlightLogData.getItemCount();
	for (int i = 0; i < nCount; i++)
	{
		
		mFlightLogData.LoadItem(mFlightLogData.ItemAt(i));
		AirsideFlightLogItem item = mFlightLogData.ItemAt(i);
		AirsideFlightLogDesc fltdesc = item.mFltDesc;
		int nLogCount = item.mpEventList->mEventList.size();
		for (int j = 0; j < nLogCount; j++)
		{
			ARCEventLog* pLog = item.mpEventList->mEventList.at(j);
			if(!HandetheRunwayExitFlightLog(PtaxiwayDelayPar,pLog,fltdesc))
				continue ;
		}

	}
	AnalysisRunwayExitFlightLogForReport(PtaxiwayDelayPar) ;
	if(parameter->getReportType() == ASReportType_Detail)
		GenerateDetailReportData(PtaxiwayDelayPar) ;
	else
		GenerateSummaryReportData(PtaxiwayDelayPar) ;

	SaveReportData() ;
}
void CRunwayExitReport::RefreshReport(CParameters * parameter)
{
	if(parameter == NULL)
		return ;
	if (m_BaseResult != NULL)
		delete m_BaseResult ;
	CRunwayExitParameter* Paramertes = (CRunwayExitParameter*)parameter ;

	if(parameter->getReportType() == ASReportType_Detail)
		m_BaseResult = new CRunwayDetailExitResult(&m_ReportBaseData,Paramertes) ;
	else
		m_BaseResult = new CRunwaySummaryExitResult(&m_SummaryData,Paramertes) ;

	m_BaseResult->GenerateResult() ;
}
void CRunwayExitReport::AnalysisRunwayExitFlightLogForReport(CRunwayExitParameter* _parameter) 
{
	for (int i = 0 ; i < (int)m_FilterEventLog.size() ;i++)
	{
		m_ReportBaseData.AddFlightEvenlog(m_FilterEventLog[i],_parameter) ;
	}
}
void CRunwayExitReport::GenerateDetailReportData(CRunwayExitParameter* _paramerer)
{

}
void CRunwayExitReport::InitListHead(CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType, CSortableHeaderCtrl* piSHC)
{
	if(reportType == ASReportType_Detail)
		InitDetailListHead( cxListCtrl, piSHC) ;
	else
		InitSummaryListHead(cxListCtrl, piSHC) ;
}
void CRunwayExitReport::InitDetailListHead(CXListCtrl& cxListCtrl, CSortableHeaderCtrl* piSHC)
{
	cxListCtrl.DeleteAllItems();

	while(cxListCtrl.GetHeaderCtrl()->GetItemCount() >0)
		cxListCtrl.DeleteColumn(0);

	cxListCtrl.InsertColumn(0, _T("Date"), LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(1, _T("Time into exit(hh::mm:ss)"), LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(2, _T("Runway"), LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(3, _T("Exit"), LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(4, _T("Operation"), LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(5, _T("AC ID"), LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(6, _T("Flight Type"), LVCFMT_LEFT, 100, -1);

	if (piSHC)
	{
		piSHC->ResetAll();
		piSHC->SetDataType(0,dtINT);
		piSHC->SetDataType(1,dtTIME);
		piSHC->SetDataType(2,dtSTRING);
		piSHC->SetDataType(3,dtSTRING);
		piSHC->SetDataType(4,dtSTRING);
		piSHC->SetDataType(5,dtSTRING);
		piSHC->SetDataType(6,dtSTRING);
	}
}
void CRunwayExitReport::InitSummaryListHead(CXListCtrl& cxListCtrl, CSortableHeaderCtrl* piSHC)
{
	cxListCtrl.DeleteAllItems();

	while(cxListCtrl.GetHeaderCtrl()->GetItemCount() >0)
		cxListCtrl.DeleteColumn(0);

	cxListCtrl.InsertColumn(0, _T("Runway"), LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(1, _T("Exit"), LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(2, _T("Min usage/Interval"), LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(3,_T("Min usage at interval"), LVCFMT_LEFT, 100, -1);

	cxListCtrl.InsertColumn(4, _T("Mean usage/Interval"), LVCFMT_LEFT, 100, -1);

	cxListCtrl.InsertColumn(5, _T("Max usage/Interval"), LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(6, _T("Max usage at interval"), LVCFMT_LEFT, 100, -1);

	cxListCtrl.InsertColumn(7, _T("Q1"), LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(8, _T("Q2"), LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(9, _T("Q3"), LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(10, _T("P1"), LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(11, _T("P5"), LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(12, _T("P10"), LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(13, _T("P90"), LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(14, _T("P95"), LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(15, _T("P99"), LVCFMT_LEFT, 100, -1);

	if (piSHC)
	{
		piSHC->ResetAll();
		piSHC->SetDataType(0,dtSTRING);
		piSHC->SetDataType(1,dtSTRING);
		piSHC->SetDataType(2,dtINT);
		piSHC->SetDataType(3,dtSTRING);
		piSHC->SetDataType(4,dtINT);
		piSHC->SetDataType(5,dtINT);
		piSHC->SetDataType(6,dtSTRING);
		piSHC->SetDataType(7,dtINT);
		piSHC->SetDataType(8,dtINT);
		piSHC->SetDataType(9,dtINT);
		piSHC->SetDataType(10,dtINT);
		piSHC->SetDataType(11,dtINT);
		piSHC->SetDataType(12,dtINT);
		piSHC->SetDataType(13,dtINT);
		piSHC->SetDataType(14,dtINT);
		piSHC->SetDataType(15,dtINT);
	}
}

void CRunwayExitReport::FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter)
{
	if(parameter->getReportType() == ASReportType_Detail)
		FillDetailListContent(cxListCtrl) ;
	else
		FillSummaryListContent(cxListCtrl) ;
}
void CRunwayExitReport::FillDetailListContent(CXListCtrl& cxListCtrl)
{
	int ndx = -1 ;
	for (int i = 0 ; i < (int)m_FilterEventLog.size() ;i++)
	{
		CFlightFliterData flightFlterData ;
		flightFlterData = m_FilterEventLog[i] ;
		CString dataStr ;
		dataStr.Format(_T("%d"),flightFlterData.m_EntryTime.GetDay()) ;
		ndx = cxListCtrl.InsertItem(0,dataStr) ;
		cxListCtrl.SetItemText(ndx,1,flightFlterData.m_EntryTime.printTime()) ;
		cxListCtrl.SetItemText(ndx,2,flightFlterData.m_RunwayName) ;
		cxListCtrl.SetItemText(ndx,3,flightFlterData.m_RunwayExitName) ;
		cxListCtrl.SetItemText(ndx,4,flightFlterData.m_Operation) ;
		cxListCtrl.SetItemText(ndx,5,flightFlterData.m_AcID);
		cxListCtrl.SetItemText(ndx,6,flightFlterData.m_FlightType) ;
	}
}
void CRunwayExitReport::FillSummaryListContent(CXListCtrl& cxListCtrl)
{
	int ndx = -1 ;
	for (int i = 0 ; i < (int)m_SummaryData.size() ;i++)
	{
		CRunwayExitSummaryDataItem flightFlterData ;
		flightFlterData = m_SummaryData[i] ;
		CString dataStr ;

		ndx = cxListCtrl.InsertItem(0,flightFlterData.m_Runway) ;
		cxListCtrl.SetItemText(ndx,1,flightFlterData.m_ExitName) ;
		dataStr.Format(_T("%d"),flightFlterData.m_MinUsager) ;
		cxListCtrl.SetItemText(ndx,2,dataStr) ;
		cxListCtrl.SetItemText(ndx,3,flightFlterData.m_MinUsagerAt);

			dataStr.Format(_T("%d"),flightFlterData.m_MeanUsager) ;
		cxListCtrl.SetItemText(ndx,4,dataStr) ;

		dataStr.Format(_T("%d"),flightFlterData.m_MaxUsager) ;
		cxListCtrl.SetItemText(ndx,5,dataStr) ;
		cxListCtrl.SetItemText(ndx,6,flightFlterData.m_MaxUsagerAt);
		
		dataStr.Format(_T("%d"),flightFlterData.m_Q1) ;
		cxListCtrl.SetItemText(ndx,7,dataStr) ;

		dataStr.Format(_T("%d"),flightFlterData.m_Q2) ;
		cxListCtrl.SetItemText(ndx,8,dataStr) ;

		dataStr.Format(_T("%d"),flightFlterData.m_Q3) ;
		cxListCtrl.SetItemText(ndx,9,dataStr) ;

		dataStr.Format(_T("%d"),flightFlterData.m_P1) ;
		cxListCtrl.SetItemText(ndx,10,dataStr) ;

		dataStr.Format(_T("%d"),flightFlterData.m_p5) ;
		cxListCtrl.SetItemText(ndx,11,dataStr) ;

		dataStr.Format(_T("%d"),flightFlterData.m_p10) ;
		cxListCtrl.SetItemText(ndx,12,dataStr) ;


		dataStr.Format(_T("%d"),flightFlterData.m_p90) ;
		cxListCtrl.SetItemText(ndx,13,dataStr) ;

		dataStr.Format(_T("%d"),flightFlterData.m_P95) ;
		cxListCtrl.SetItemText(ndx,14,dataStr) ;

		dataStr.Format(_T("%d"),flightFlterData.m_p99) ;
		cxListCtrl.SetItemText(ndx,15,dataStr) ;
	}
}

void CRunwayExitReport::GenerateSummaryReportData( CRunwayExitParameter* _paramerer )
{
	if(_paramerer == NULL)
		return ;
	CRunwayExitRepDataItem* PRunwayItem = NULL ;
	CString strval ;
	CTimeIntervalFlightData* timeIntervalFlightItem = NULL ;
	for (int i = 0 ; i < (int)m_ReportBaseData.GetData()->size() ;i++)
	{
		PRunwayItem = m_ReportBaseData.GetData()->at(i) ;
		CStatisticalTools<long> m_DelayTool ;

		for (int i = 0 ; i < (int)PRunwayItem->GetData()->size() ;i++)
		{
			timeIntervalFlightItem = PRunwayItem->GetData()->at(i) ;
			m_DelayTool.AddNewData(timeIntervalFlightItem->m_FlightData.size()) ;
		}
		CRunwayExitSummaryDataItem PRunwayExitSummary ;
		PRunwayExitSummary.m_ExitName = PRunwayItem->m_RunwayExitName ;
		PRunwayExitSummary.m_Runway = PRunwayItem->m_RunwayName ;
		PRunwayExitSummary.m_MaxUsager = PRunwayItem->GetMaxUsager(strval) ;
		PRunwayExitSummary.m_MaxUsagerAt = strval ;

		PRunwayExitSummary.m_MeanUsager = PRunwayItem->GetMeanUsager() ;

		PRunwayExitSummary.m_MinUsager = PRunwayItem->GetMinUsager(strval) ;
		PRunwayExitSummary.m_MinUsagerAt = strval ;
		
		PRunwayExitSummary.m_Q1 = m_DelayTool.GetPercentile(25) ;
		PRunwayExitSummary.m_Q2 = m_DelayTool.GetPercentile(50) ;
		PRunwayExitSummary.m_Q3 = m_DelayTool.GetPercentile(75) ;

		PRunwayExitSummary.m_P1 = m_DelayTool.GetPercentile(1) ;
		PRunwayExitSummary.m_p5 = m_DelayTool.GetPercentile(5) ;
		PRunwayExitSummary.m_p10 = m_DelayTool.GetPercentile(10) ;
		PRunwayExitSummary.m_p90 = m_DelayTool.GetPercentile(90) ;
		PRunwayExitSummary.m_P95 = m_DelayTool.GetPercentile(95) ;
		PRunwayExitSummary.m_p99 = m_DelayTool.GetPercentile(99) ;

		m_SummaryData.push_back(PRunwayExitSummary) ;
	}
}

BOOL CRunwayExitReport::ExportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType /* = ASReportType_Detail */)
{
	if(reportType == ASReportType_Detail)
	{
		int size =(int) m_FilterEventLog.size() ;
		_file.writeInt(size) ;
		_file.writeLine() ;

		for (int i = 0 ; i < size ;i++)
		{
			CFlightFliterData data = m_FilterEventLog[i] ;
			data.ExportFile(_file) ;
		}
	}
	else
		m_ReportBaseData.ExportFile(_file) ;
	return TRUE ;
}
BOOL CRunwayExitReport::ImportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType /* = ASReportType_Detail */)
{
	m_ReportBaseData.ClearAll() ;
	m_FilterEventLog.clear() ;
	m_SummaryData.clear() ;
	
	if(reportType != ASReportType_Detail)
	{
		m_ReportBaseData.ImportFile(_file) ;
		GenerateSummaryReportData(m_parameter) ;
	}else
	{
		int size = 0 ;
		if(!_file.getInteger(size))
			return FALSE;
		_file.getLine() ;
		for (int i = 0 ; i < size ;i++)
		{
			CFlightFliterData data ;
			data.ImportFile(_file) ;
			m_FilterEventLog.push_back(data) ;
		}
		AnalysisRunwayExitFlightLogForReport(m_parameter) ;
	}
	return TRUE ;
}

CString CRunwayExitReport::GetReportFileName()
{
	return _T("RunwayExitRep\\runwayexitreport.rep") ;
}

BOOL CRunwayExitReport::WriteReportData( ArctermFile& _file )
{
	_file.writeField("RunwayExit Report");
	_file.writeLine();
	_file.writeField("Paramerate");
	_file.writeLine();
	m_parameter->ExportFile(_file) ;
	enumASReportType_Detail_Summary ty = m_parameter->getReportType() ;
	_file.writeInt(ty) ;
	_file.writeLine() ;
	return ExportReportData(_file,CString(),ty) ;
}

BOOL CRunwayExitReport::ReadReportData( ArctermFile& _file )
{
	_file.getLine() ;
	m_parameter->ImportFile(_file) ;
	int type = 0 ;
	_file.getLine() ;
	if(!_file.getInteger(type) )
		return FALSE ;
	_file.getLine() ;
	return ImportReportData(_file,CString(),(enumASReportType_Detail_Summary)type) ;
}