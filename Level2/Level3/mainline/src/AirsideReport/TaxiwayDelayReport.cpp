#include "StdAfx.h"
#include ".\taxiwaydelayreport.h"
#include "TaxiwayDelayParameters.h"
#include "../InputAirside/IntersectionNode.h"
#include "TaxiwayDelayReportResult.h"
#include "../Reports/StatisticalTools.h"
#include "../Results/AirsideFlightNodeLog.h"

void CTaxiwayFligthDelayItem::AddDelayLogData(FlightConflictReportData::ConflictDataItem* _conflictItem)
{
	std::vector<FlightConflictReportData::ConflictDataItem*>::iterator iter = std::find(m_TaxiwayDelayIndex.begin() ,m_TaxiwayDelayIndex.end(),_conflictItem) ;
	if(iter == m_TaxiwayDelayIndex.end())
		m_TaxiwayDelayIndex.push_back(_conflictItem) ;
}

void CTaxiwayFligthDelayItem::GetTaxiwayDelayTimeByIntervalTime(ElapsedTime _start , ElapsedTime _end , std::vector<CONFIICTREPORTDATA_ITEM*>& _lastDelay)
{
	CONFIICTREPORTDATA_ITEM* item = NULL ;
	size_t nCount = m_TaxiwayDelayIndex.size();
	for (size_t i = 0 ; i < nCount ; i++)
	{
		//ElapsedTime _OutTime ;
		item = m_TaxiwayDelayIndex.at(i);
		if(item->m_tTime > _end || item->m_tTime < _start)
			continue ;

		if(item->m_tTime >= _start && item->m_tTime < _end)
		{
			//_OutTime = (min(item->m_tTime + item->m_tDelay,_end) - item->m_tTime) ;
			//_lastDelay[item] = /*_OutTime*/item->m_tDelay ;
			_lastDelay.push_back(item);
		}
		//if(item->m_tTime < _start && (item->m_tTime + item->m_tDelay) > _start)
		//{
		//	_OutTime = min(item->m_tTime + item->m_tDelay,_end) -_start ;
		//	_lastDelay[item] = _OutTime ;
		//}
	}
}

//////////////////////////////////////////////////////////////////////////
CTaxiwayFligthDelayItem*  CTaxiwaySegmentItem::FindFlightDelayItemByFlightID(int _flightId)
{
	CTaxiwayFligthDelayItem* flightDelayItem =  NULL ;
	for ( int i = 0 ;i < (int)m_FlightDelay.size() ;i++)
	{
		flightDelayItem = m_FlightDelay[i] ;

		if(flightDelayItem->GetFlightUniqeID() == _flightId)
			return flightDelayItem ;
	}
	return NULL ;
}

CTaxiwayFligthDelayItem* CTaxiwaySegmentItem::AddFlightDelayItem(int _flightID)
{
	CTaxiwayFligthDelayItem* flightDelayItem = FindFlightDelayItemByFlightID(_flightID) ;
	if(flightDelayItem == NULL)
	{
		flightDelayItem = new CTaxiwayFligthDelayItem ;
		flightDelayItem->SetFlightUniqeID(_flightID) ;
		m_FlightDelay.push_back(flightDelayItem) ;
	}
	return flightDelayItem ;
}

void CTaxiwaySegmentItem::AddDelayLogData(FlightConflictReportData::ConflictDataItem* _conflictItem)
{
	if(_conflictItem == NULL)
		return ;
	CTaxiwayFligthDelayItem* flightDelayItem = FindFlightDelayItemByFlightID(_conflictItem->m_FlightID) ;
	if(flightDelayItem == NULL)
	{
		flightDelayItem = new CTaxiwayFligthDelayItem ;
		flightDelayItem->SetFlightUniqeID(_conflictItem->m_FlightID) ;
		m_FlightDelay.push_back(flightDelayItem) ;
	}
     //flightDelayItem = AddFlightDelayItem(_conflictItem->m_FlightID) ;
	 flightDelayItem->IsDeparture(_conflictItem->m_IsDep) ;
	 CString flightID = _conflictItem->m_strACName;
	 //flightID.Format(_T("%s-%d"),_conflictItem->m_strACName ,_conflictItem->m_FlightID) ;
	 flightDelayItem->SetFlightID(flightID) ;
	 flightDelayItem->AddDelayLogData(_conflictItem) ;
}

CTaxiwaySegmentItem::CTaxiwaySegmentItem(const CReportTaxiwaySegment& reportSeg) 
:m_ReportSegment(reportSeg)
{

}

CTaxiwaySegmentItem::~CTaxiwaySegmentItem()
{
	ClearData() ;
}
//////////////////////////////////////////////////////////////////////////
void CTaxiwayDelayReportDataItem::InitIntersectionSegment(CTaxiwaySelect* _TaxiwaySelect)
{
	if(_TaxiwaySelect->GetIntersectionSegment()->empty())
	{
		CReportTaxiwaySegment segment(-1,-1);
		CTaxiwaySegmentItem* intersectionItem = new CTaxiwaySegmentItem(segment);
		//intersectionItem->SetFromIntersectionID(-1) ;
		//intersectionItem->SetFromInsectionName(_T("Start")) ;

		//intersectionItem->SetToInsectionID(-1) ;
		//intersectionItem->SetToInsectionName(_T("End")) ;
		m_SegmentData.push_back(intersectionItem) ;
		return ;
	}

	CTaxiwaySelect::TY_DATA_ITER iter =	_TaxiwaySelect->GetIntersectionSegment()->begin() ;
	for ( ; iter != _TaxiwaySelect->GetIntersectionSegment()->end() ;iter++)
	{
		CTaxiwaySegmentItem* intersectionItem = new CTaxiwaySegmentItem(*iter) ;
		//intersectionItem->SetFromIntersectionID((*iter).m_FormIntersectionID) ;
		//intersectionItem->SetFromInsectionName((*iter).m_FromIntersection) ;

		//intersectionItem->SetToInsectionID((*iter).m_ToIntersectionID) ;
		//intersectionItem->SetToInsectionName((*iter).m_ToIntersection) ;

		m_SegmentData.push_back(intersectionItem) ;
	}
}

void CTaxiwayDelayReportDataItem::AddDelayLogData(FlightConflictReportData::ConflictDataItem* _conflictItem)
{
	if(_conflictItem == NULL)
		return ;
	CTaxiwaySegmentItem* delayItem = NULL ;
	IntersectionNode _intersectionNode ;
	double fromDis = 0 ;
	double toDis = 0 ;
	double delayFromDis = 0 ;
	double delayToDis = 0 ;
	if(_conflictItem->m_ResourceDes.fromnode == -1 || _conflictItem->m_ResourceDes.tonode == -1)
		return ;

	_intersectionNode.ReadData(_conflictItem->m_ResourceDes.fromnode) ;
	delayFromDis = _intersectionNode.GetDistance(m_Taxiway) ;
	_intersectionNode.ReadData(_conflictItem->m_ResourceDes.tonode);
	delayToDis = _intersectionNode.GetDistance(m_Taxiway) ;

	for (int i = 0 ; i < (int)m_SegmentData.size() ;i++)
	{
		delayItem = m_SegmentData[i] ;
		if(delayItem->GetReportSegment().m_FormNodeID == -1 && delayItem->GetReportSegment().m_ToNodeID == -1)
		{
			delayItem->AddDelayLogData(_conflictItem) ;
			return ;
		}

		//_intersectionNode.ReadData(delayItem->GetFromIntersectionID()) ;
		fromDis =  delayItem->GetReportSegment().m_dFromDist;

		//_intersectionNode.ReadData(delayItem->GetToInsectionID()) ;
		toDis = delayItem->GetReportSegment().m_dToDist ;

		//if(  min(fromDis,toDis) <= delayFromDis &&  delayFromDis<= max(fromDis,toDis))
		//{
		//	delayItem->AddDelayLogData(_conflictItem) ;
		//	return ;
		//}

		if (toDis == -1)		//from a node to end of taxiway
		{
			if (min(delayFromDis,delayToDis) >= fromDis)
			{
				delayItem->AddDelayLogData(_conflictItem) ;
				return;
			}
		}
		else			
		{
			if (toDis > fromDis)		//positive direction
			{
				if (min(delayFromDis,delayToDis) >= fromDis && max(delayFromDis, delayToDis) <= toDis)
				{
					delayItem->AddDelayLogData(_conflictItem) ;
					return;
				}

			}
			else		//negative direction
			{
				if (min(delayFromDis,delayToDis) >= toDis && max(delayFromDis, delayToDis) <= fromDis)
				{
					delayItem->AddDelayLogData(_conflictItem) ;
					return;
				}
			}
		}


	}
}
//////////////////////////////////////////////////////////////////////////

CTaxiwayDelayReportData::CTaxiwayDelayReportData(CTaxiwayDelayParameters* _parameter)
{
  if(_parameter != NULL)
  {
	  CTaxiwayDelayParameters::TY_TAXIWAY_DATA* DataSet =  _parameter->GetTaxiwayData() ;
		CTaxiwayDelayParameters::TY_TAXIWAY_DATA_ITER iter = DataSet->begin() ;
		CTaxiwayDelayReportDataItem* ItemData = NULL ;
	  for ( ;iter< DataSet->end() ;iter++)
	  {
		  ItemData = new CTaxiwayDelayReportDataItem( (*iter)->GetTaxiwayID()) ;
		  ItemData->SetTaxiwayName((*iter)->GetTaxiwayName()) ;
		  ItemData->InitIntersectionSegment((*iter)) ;
		  m_logData.push_back(ItemData) ;
	  }
  }
}

void CTaxiwayDelayReportData::AddDelayLogData(FlightConflictReportData::ConflictDataItem* _conflictItem)
{
	if(_conflictItem == NULL)
		return ;
	AddTaxiwayItem(_conflictItem->m_ResourceDes.resid)->AddDelayLogData(_conflictItem) ;
}

CTaxiwayDelayReportDataItem* CTaxiwayDelayReportData::AddTaxiwayItem(int _taxiID)
{
	for (int i =0 ;i <(int)m_logData.size() ;i++)
	{
		if(m_logData[i]->GetTaxiwayID() == _taxiID)
			return m_logData[i] ;
	}

	CTaxiwayDelayReportDataItem* ItemData = new CTaxiwayDelayReportDataItem(_taxiID) ;
	ALTObject*  taxiwayObject = ALTObject::ReadObjectByID(_taxiID) ;
	if(taxiwayObject)
		ItemData->SetTaxiwayName(taxiwayObject->GetObjNameString()) ;
	///set default segment 
	CReportTaxiwaySegment segment(-1,-1);
	CTaxiwaySegmentItem* intersectionItem = new CTaxiwaySegmentItem(segment) ;
	//intersectionItem->SetFromIntersectionID(-1) ;
	//intersectionItem->SetFromInsectionName(_T("Start")) ;

	//intersectionItem->SetToInsectionID(-1) ;
	//intersectionItem->SetToInsectionName(_T("End")) ;
	ItemData->GetSegmentData()->push_back(intersectionItem) ;
  //////////////////////////////////////////////////////////////////////////
	m_logData.push_back(ItemData) ;
	return ItemData ;
}

//////////////////////////////////////////////////////////////////////////
//taxiway delay report
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

CTaxiwayDelayReport::CDelayLogFilterFunc::CDelayLogFilterFunc(CTaxiwayDelayParameters* _parameter):m_Parameter(_parameter)
{

}

BOOL CTaxiwayDelayReport::CDelayLogFilterFunc::bFit(class_type* ct,const ARCEventLog* _evenlog)
{

	if (!_evenlog->IsA(ct))
		return FALSE;

	AirsideConflictionDelayLog* pConflictLog = (AirsideConflictionDelayLog*)_evenlog;
	if(pConflictLog->mMode <= OnExitRunway || pConflictLog->mMode >= OnTakeOffEnterRunway)
		return FALSE;

	ElapsedTime tTime;
	tTime.setPrecisely(pConflictLog->time) ;
	if (tTime < m_Parameter->getStartTime() || tTime > m_Parameter->getEndTime())
		return FALSE;

	CTaxiwaySelect*  taxiwaySel = NULL ;
	if(!m_Parameter->CheckTheTaxiWayIsSelect(pConflictLog->mAtResource.resid,&taxiwaySel))
		return FALSE;

	if(taxiwaySel == NULL)
		return TRUE ;

	if(pConflictLog->mAtResource.fromnode == -1 || pConflictLog->mAtResource.tonode == -1)
		return FALSE ;

	int nTaxiwayID = taxiwaySel->GetTaxiwayID();
	IntersectionNode  _Node ;
	_Node.ReadData(pConflictLog->mAtResource.fromnode) ;
	double dFromDist = _Node.GetDistance(nTaxiwayID) ;
	_Node.ReadData(pConflictLog->mAtResource.tonode);
	double dToDist = _Node.GetDistance(nTaxiwayID) ;

	return taxiwaySel->IsSegInParaDefine(dFromDist, dToDist) ;

}

//////////////////////////////////////////////////////////////////////////
CTaxiwayDelayReport::CTaxiwayDelayReport(CBGetLogFilePath pFunc,CTaxiwayDelayParameters* _Paramer ):m_ReportDelayData(NULL),m_BaseResult(NULL),CAirsideBaseReport(pFunc),m_Paramer(_Paramer)
{
	
}
CTaxiwayDelayReport::~CTaxiwayDelayReport()
{
	ClearDetailData() ;
	ClearSummaryData() ;
	if(m_ReportDelayData != NULL)
		delete m_ReportDelayData ;
}
void CTaxiwayDelayReport::ClearDetailData()
{
	for (int i = 0 ; i < (int)m_ReportDetailData.size() ;i++)
	{
		delete m_ReportDetailData[i] ;
	}
	m_ReportDetailData.clear() ;
}
void CTaxiwayDelayReport::ClearSummaryData()
{
	for (int i = 0 ; i < (int)m_ReportSummaryData.size() ;i++)
	{
		delete m_ReportSummaryData[i] ;
	}
	m_ReportSummaryData.clear() ;
}
void CTaxiwayDelayReport::RefreshReport(CParameters * parameter)
{
	if(parameter == NULL)
		return ;
	if (m_BaseResult != NULL)
		delete m_BaseResult ;
	CTaxiwayDelayParameters* Paramertes = (CTaxiwayDelayParameters*)parameter ;
	m_Paramer = Paramertes ;
	if(parameter->getReportType() == ASReportType_Detail)
		m_BaseResult = new CTaxiwayDelayDetailReportResult(&m_ReportDetailData,Paramertes) ;
	else
		m_BaseResult = new CTaxiwayDelaySummaryReportResult(&m_ReportSummaryData,Paramertes) ;

	m_BaseResult->GenerateResult() ;

}
void CTaxiwayDelayReport::GenerateReport(CParameters * parameter)
{
	if(m_ReportDelayData != NULL)
		delete m_ReportDelayData ;


	CTaxiwayDelayParameters* PtaxiwayDelayPar = (CTaxiwayDelayParameters*)parameter ;
	m_ReportDelayData = new CTaxiwayDelayReportData(PtaxiwayDelayPar) ;
		m_Paramer = PtaxiwayDelayPar ;
	//read log
	CString strDescFilepath = (*m_pCBGetLogFilePath)(AirsideFlightDescFileReport);
	CString strEventFilePath = (*m_pCBGetLogFilePath)(AirsideFlightEventFileReport);

	//read flight log 
	mFlightLogData.OpenInput(strDescFilepath.GetString(), strEventFilePath.GetString());
	
	//////////////////////////////////////////////////////////////////////////
	FlightConflictReportData flightConflictReport(parameter,new CTaxiwayDelayReport::CDelayLogFilterFunc(PtaxiwayDelayPar)) ;

	flightConflictReport.LoadData(strDescFilepath,strEventFilePath) ;

	size_t nSize = flightConflictReport.m_vDataItemList.size();
	for (size_t i = 0 ; i < nSize; i++)
	{
		m_ReportDelayData->AddDelayLogData(flightConflictReport.m_vDataItemList[i]) ;
	}

	if(parameter->getReportType() == ASReportType_Detail)
		GenerateDetailReportData(PtaxiwayDelayPar) ;
	else
		GenerateSummaryReportData(PtaxiwayDelayPar) ;

	SaveReportData() ;
}
void CTaxiwayDelayReport::GenerateSummaryReportData(CTaxiwayDelayParameters* PtaxiwayDelayPar)
{
	if(PtaxiwayDelayPar == NULL)
		return ;
	ClearSummaryData() ;
	CTaxiwayDelayReportDataItem* PDelayReportData = NULL;
	CTaxiwayDelayReportSummaryDataItem* reportSummaryDataItem = NULL ;
	CTaxiwaySegmentItem* m_SegmentItem = NULL ;
	for (int i = 0 ; i < (int)m_ReportDelayData->GetDataSet()->size() ;i++)
	{
		PDelayReportData = m_ReportDelayData->GetDataSet()->at(i) ;
		for (int taxiwaysegment = 0 ; taxiwaysegment < (int)PDelayReportData->GetSegmentData()->size() ; taxiwaysegment++)
		{
			m_SegmentItem = PDelayReportData->GetSegmentData()->at(taxiwaysegment) ;

			reportSummaryDataItem = new CTaxiwayDelayReportSummaryDataItem ;
			reportSummaryDataItem->m_Taxiway = PDelayReportData->GetTaxiwayName() ;
			reportSummaryDataItem->m_FromNode = m_SegmentItem->GetReportSegment().m_strFromName ;
			reportSummaryDataItem->m_ToNode = m_SegmentItem->GetReportSegment().m_strToName ;
			reportSummaryDataItem->m_numofaircraft = (int)(m_SegmentItem->GetFlightDelayData()->size()) ;
			AnalysisSummaryDelay(reportSummaryDataItem,m_SegmentItem) ;
			if(reportSummaryDataItem->m_totalDelay.asSeconds() != 0)
				m_ReportSummaryData.push_back(reportSummaryDataItem) ;
		}
	
	}
}
void CTaxiwayDelayReport::AnalysisSummaryDelay(CTaxiwayDelayReportSummaryDataItem* _reportSummaryDataItem , CTaxiwaySegmentItem* _intersectionItem)
{
	if(_reportSummaryDataItem == NULL || _intersectionItem == NULL)
		return ;
	CTaxiwayFligthDelayItem* flightDelayItem = NULL ;
	ElapsedTime minTime ;
	ElapsedTime maxtime ;
	ElapsedTime meantime ;
	ElapsedTime maxdelayStartTime ;
	CString maxdelayflight  ;
	int totaldelayNum = 0 ;
	CStatisticalTools<long> m_DelayTool ;
	for (int i = 0 ; i < (int)_intersectionItem->GetFlightDelayData()->size() ;i++)
	{
		flightDelayItem = _intersectionItem->GetFlightDelayData()->at(i) ;

		for(int delayNum = 0 ; delayNum < (int)flightDelayItem->GetDelayData()->size() ;delayNum++)
		{
			CONFIICTREPORTDATA_ITEM* delayItem = flightDelayItem->GetDelayData()->at(delayNum) ;
			minTime = min(minTime,delayItem->m_tDelay) ;
			if(maxtime < delayItem->m_tDelay)
			{
				maxtime = delayItem->m_tDelay ;
				maxdelayflight = flightDelayItem->GetFlightID() ;
				maxdelayStartTime = delayItem->m_tTime ;
			}
			meantime += delayItem->m_tDelay ;
			totaldelayNum++ ;
			m_DelayTool.AddNewData(delayItem->m_tDelay.asSeconds()) ;
		}
	}
	//set total delay 
	_reportSummaryDataItem->m_totalDelay = meantime ;
	//set min delay 
	_reportSummaryDataItem->m_MinDelay = minTime ;
	//set mean delay
	if(totaldelayNum != 0)
		_reportSummaryDataItem->m_MeanDelay = meantime.asSeconds() / (float)totaldelayNum ;
	//set max delay 
	_reportSummaryDataItem->m_MaxDelay = maxtime ;
	//set max delay start time 
	_reportSummaryDataItem->m_MaxDelayStart = maxdelayStartTime ;
	//set max delay flight 
	_reportSummaryDataItem->m_FlightTyOfmaxDelay =  maxdelayflight;
	//set Q1 
	_reportSummaryDataItem->m_Q1 =ElapsedTime( m_DelayTool.GetPercentile(25)) ;
	//set Q2
	_reportSummaryDataItem->m_Q2 =ElapsedTime( m_DelayTool.GetPercentile(50));
	//set Q3
	_reportSummaryDataItem->m_Q3 =ElapsedTime( m_DelayTool.GetPercentile(75)) ;

	//set p1 
	_reportSummaryDataItem->m_P1 = ElapsedTime( m_DelayTool.GetPercentile(1) );
	_reportSummaryDataItem->m_P5 = ElapsedTime( m_DelayTool.GetPercentile(5) );
	_reportSummaryDataItem->m_P90 =ElapsedTime( m_DelayTool.GetPercentile(90)) ;
	_reportSummaryDataItem->m_P95 =ElapsedTime( m_DelayTool.GetPercentile(95));
	_reportSummaryDataItem->m_P99 =ElapsedTime( m_DelayTool.GetPercentile(99)) ;
}
void CTaxiwayDelayReport::GenerateDetailReportData(CTaxiwayDelayParameters* pDelayPara)
{
	if(pDelayPara == NULL)
		return ;
	if(m_ReportDelayData == NULL)
		return ;
	ClearDetailData() ;
	ElapsedTime intervalTime = pDelayPara->getEndTime() - pDelayPara->getStartTime() ;

	int intervals = intervalTime.asSeconds()/pDelayPara->getInterval();

	if ( (intervalTime.asSeconds() % (pDelayPara->getInterval())) > 0)
		intervals +=1;

	CTaxiwayDelayReportDataItem* PDelayReportData = NULL;
	for (int i = 0 ; i < (int)m_ReportDelayData->GetDataSet()->size() ;i++)
	{
		PDelayReportData = m_ReportDelayData->GetDataSet()->at(i) ;

		for (int j = 0 ; j < (int)PDelayReportData->GetSegmentData()->size() ;j++)
		{
			CTaxiwaySegmentItem* IntersectionItem = PDelayReportData->GetSegmentData()->at( j ) ;

			for (int flightndx = 0 ; flightndx < (int) IntersectionItem->GetFlightDelayData()->size() ; flightndx++)
			{
				CTaxiwayFligthDelayItem* flghtDelayItem = IntersectionItem->GetFlightDelayData()->at(flightndx) ;
				
				std::vector<TAXIWAYSEGINFO> vSegInfos;
				ElapsedTime tIntervalStart = pDelayPara->getStartTime() ;
				ElapsedTime tIntervalEnd = 0L;
				for (int ndx = 0 ; ndx < intervals ;ndx++)
				{
					std::vector<CONFIICTREPORTDATA_ITEM*> logdata ;
					tIntervalEnd = tIntervalStart + ElapsedTime( pDelayPara->getInterval()) ;
					flghtDelayItem->GetTaxiwayDelayTimeByIntervalTime(tIntervalStart,tIntervalEnd,logdata) ;

					if (logdata.empty())
					{
						tIntervalStart = tIntervalEnd ;
						continue;
					}

					vSegInfos.clear();
					GetFlightEnterTimeAndLeaveTimeFormSegment(flghtDelayItem->GetFlightID(),flghtDelayItem->GetFlightUniqeID() ,IntersectionItem->GetReportSegment().m_FormNodeID,\
						IntersectionItem->GetReportSegment().m_ToNodeID,PDelayReportData->GetTaxiwayID(),vSegInfos,pDelayPara) ;

					size_t nSize = vSegInfos.size();
					for (size_t idx = 0; idx < nSize; idx++)
					{
						TAXIWAYSEGINFO segInfo = vSegInfos.at(idx);

						CTaxiwayDelayReportDetailDataItem* detailDataItem = new CTaxiwayDelayReportDetailDataItem ;
						detailDataItem->m_FlightID = flghtDelayItem->GetFlightID() ;
						detailDataItem->m_IsDep = flghtDelayItem->IsDeparture() ;
						detailDataItem->m_TaxiwayName = PDelayReportData->GetTaxiwayName() ;
						detailDataItem->m_FromIntersection = segInfo.strEnterNode ;
						detailDataItem->m_ToIntersection = segInfo.strLeaveNode ;
						detailDataItem->m_startTime =tIntervalStart;
						detailDataItem->m_EndTime = tIntervalEnd;
						detailDataItem->m_EnterSegmentTime = segInfo.tEnter ;
						detailDataItem->m_ExistSegmentTime = segInfo.tLeave ;

						int delaynum = 0 ;
						std::vector<CONFIICTREPORTDATA_ITEM*>::iterator iter = logdata.begin() ;
						for ( ; iter != logdata.end() ; iter++,delaynum++)
						{
							if ((*iter)->m_tTime < detailDataItem->m_EnterSegmentTime || (*iter)->m_tTime > detailDataItem->m_ExistSegmentTime)		//no belong to the segment
									continue;

							detailDataItem->CalculateDelayTime((*iter)->m_tDelay) ;
							detailDataItem->m_numberofdelay++;

							if(delaynum == 0)
							{
								detailDataItem->m_delay1 =(*iter)->m_tDelay ;
								detailDataItem->m_delay1Reason = AirsideConflictionDelayLog::GetDelayReasonByCode((*iter)->m_DelayReason ) ;
							}
							if(delaynum == 1)
							{
								detailDataItem->m_delay2Reason = AirsideConflictionDelayLog::GetDelayReasonByCode( (*iter)->m_DelayReason  ) ;
								detailDataItem->m_delay2 = (*iter)->m_tDelay ;
							}
							if(delaynum == 2)
							{
								detailDataItem->m_delay3Reason = AirsideConflictionDelayLog::GetDelayReasonByCode( (*iter)->m_DelayReason  ) ; 
								detailDataItem->m_delay3 = (*iter)->m_tDelay ;
							}
						}
						//detailDataItem->m_UnInterruptTime = min(detailDataItem->m_EndTime,leavetime) - max(detailDataItem->m_startTime,entertime) - detailDataItem->m_DelayTime ;
						detailDataItem->m_UnInterruptTime = detailDataItem->m_ExistSegmentTime - detailDataItem->m_EnterSegmentTime - detailDataItem->m_DelayTime ;
						m_ReportDetailData.push_back(detailDataItem) ;

					}
				
					//set delay time & set delay reason
					tIntervalStart = tIntervalEnd ;
				}

				
			}
		}
	}
}

//void CTaxiwayDelayReport::SortByDelayTime(std::map<CONFIICTREPORTDATA_ITEM*,ElapsedTime>& _delaydata)
//{
//	//std::sort(_delaydata.begin() , _delaydata.end() ,CFunSortByDelayTime());
//}

void CTaxiwayDelayReport::GetFlightEnterTimeAndLeaveTimeFormSegment(CString _fightID,int _uniqeID,int _fromID,int _ToID,int _taxiway,
																	std::vector<TAXIWAYSEGINFO>& vSegInfo,CTaxiwayDelayParameters* PtaxiwayDelayPar)
{

	int nCount = mFlightLogData.getItemCount();
	BOOL bEnter = FALSE;

	//ElapsedTime tTime = -1L;
	TAXIWAYSEGINFO SegTimeInfo;
	SegTimeInfo.tEnter = -1L;
	SegTimeInfo.tLeave = -1L;

	for (int i = 0; i < nCount; i++)
	{
		mFlightLogData.LoadItem(mFlightLogData.ItemAt(i));
		AirsideFlightLogItem item = mFlightLogData.ItemAt(i);

		if(item.mFltDesc.id == _uniqeID)
		{
			AirsideFlightLogDesc fltdesc = item.mFltDesc;
			int nLogCount = item.mpEventList->mEventList.size();

			//int nExitNodeID = -1;
			//std::vector<AirsideFlightNodeLog*> vNodeLog;
			ARCEventLog* pPreLog = NULL;

			for (int j = 0; j < nLogCount; j++)
			{
				ARCEventLog* pLog = item.mpEventList->mEventList.at(j);

				//if(/*pLog->IsA(typeof(FlightStartNodeDelayLog)) ||*/ pLog->IsA(typeof(FlightCrossNodeLog)))
				//{
				//	vNodeLog.push_back((AirsideFlightNodeLog*)pLog);
				//	continue;
				//}

				if(!pLog->IsA(typeof(AirsideFlightEventLog)))
					continue;

				if (pPreLog && pPreLog->time == pLog->time)		//same time neglect it
					continue;

				pPreLog = pLog;

				AirsideFlightEventLog* pEventLog = (AirsideFlightEventLog*)pLog;
				if ((pEventLog->mMode <= OnExitRunway || pEventLog->mMode >= OnTakeOffEnterRunway) && pEventLog->mMode != OnTaxiToDeice)	//unrelated with taxiway
					continue;
			
				if(pEventLog->resDesc.resid == _taxiway)
				{
					CTaxiwaySelect*  pTaxiway = NULL ;
					if(PtaxiwayDelayPar->CheckTheTaxiWayIsSelect(_taxiway,&pTaxiway))
					{
						//tTime.setPrecisely(pEventLog->time) ;

						IntersectionNode  nodeFrom, nodeTo ;
						nodeFrom.ReadData(pEventLog->resDesc.fromnode) ;
						nodeTo.ReadData(pEventLog->resDesc.tonode);

						if(pTaxiway == NULL)							//used not define filter taxiway 
						{
							if(!bEnter)
							{
								SegTimeInfo.tEnter.setPrecisely(pEventLog->time);
								SegTimeInfo.strEnterNode = nodeFrom.GetName();
								bEnter = TRUE ;
								continue;

							}
							
							if (bEnter)
							{
								//nExitNodeID = pEventLog->resDesc.tonode;
								SegTimeInfo.tLeave.setPrecisely(pEventLog->time);
								SegTimeInfo.strLeaveNode = nodeTo.GetName();
							}
						}
						else						//with taxiway filter
						{
							if(pTaxiway->IsSegInParaDefine(nodeFrom.GetDistance(_taxiway),nodeTo.GetDistance(_taxiway)))
							{
								// entry the segment
								if(!bEnter)
								{
									SegTimeInfo.tEnter.setPrecisely(pEventLog->time);
									SegTimeInfo.strEnterNode = nodeFrom.GetName();
									bEnter = TRUE ;
									continue ;
								}

								if (bEnter)
								{
									SegTimeInfo.tLeave.setPrecisely(pEventLog->time);
									SegTimeInfo.strLeaveNode = nodeTo.GetName();
									//nExitNodeID = pEventLog->resDesc.tonode;
								}
							}
							else	//still in this taxiway ,but out the segment which user defined
							{
								if(bEnter)
								{
									//if (nExitNodeID > -1)
									//{
									//	ElapsedTime tExit = -1L;
									//	size_t nSize = vNodeLog.size();
									//	for (size_t idx = 0; idx < nSize; idx++)
									//	{
									//		if ((vNodeLog.at(idx))->mNodeId == nExitNodeID)
									//		{
									//			tExit = (vNodeLog.at(idx))->time;
									//			break;
									//		}
									//	}

									//	ASSERT(tExit >=0L);
									//	tSegTime.second = tExit;
									//}
									//if (tSegTime.second <0L)
									SegTimeInfo.tLeave.setPrecisely(pEventLog->time);

									ASSERT(SegTimeInfo.tEnter < SegTimeInfo.tLeave);

									vSegInfo.push_back(SegTimeInfo);
									SegTimeInfo.tEnter = -1L;
									SegTimeInfo.tLeave = -1L;
									bEnter = FALSE ;
									continue ;
									
								}
							}

						}
					}
					
				}

				if(pEventLog->resDesc.resid != _taxiway && bEnter)	//walk out the taxiway ,a leave time .
				{	
					//if (nExitNodeID > -1)
					//{
					//	ElapsedTime tExit = -1L;
					//	size_t nSize = vNodeLog.size();
					//	for (size_t idx = 0; idx < nSize; idx++)
					//	{
					//		if ((vNodeLog.at(idx))->mNodeId == nExitNodeID)
					//		{
					//			tExit = (vNodeLog.at(idx))->time;
					//			break;
					//		}
					//	}

					//	ASSERT(tExit >=0L);
					//	tSegTime.second = tExit;
					//}
					//if (tSegTime.second <0L)
					SegTimeInfo.tLeave.setPrecisely(pEventLog->time);

					ASSERT(SegTimeInfo.tEnter < SegTimeInfo.tLeave);

					vSegInfo.push_back(SegTimeInfo);
					SegTimeInfo.tEnter = -1L;
					SegTimeInfo.tLeave = -1L;
					bEnter = FALSE ;
					continue ;
				}				
			}
			return ;
		}
	}
}
void CTaxiwayDelayReport::InitListHead(CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType, CSortableHeaderCtrl* piSHC)
{
	if(reportType == ASReportType_Detail)
		InintListDetailHead(cxListCtrl, piSHC) ;
	else
		InitListSummaryHead(cxListCtrl, piSHC) ;
}
void  CTaxiwayDelayReport::FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter)
{
		if(parameter == NULL)
			return ;
		if(parameter->getReportType() == ASReportType_Detail)
			FillDetailListContent(cxListCtrl,parameter) ;
		else
			FillSummaryListContent(cxListCtrl,parameter) ;
}
void CTaxiwayDelayReport::InintListDetailHead(CXListCtrl& cxListCtrl, CSortableHeaderCtrl* piSHC)
{
	cxListCtrl.DeleteAllItems();

	while(cxListCtrl.GetHeaderCtrl()->GetItemCount() >0)
		cxListCtrl.DeleteColumn(0);

	cxListCtrl.InsertColumn(0, _T("Interval"), LVCFMT_LEFT, 100, -1);

	cxListCtrl.InsertColumn(1, _T("Flight ID"), LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(2, _T("Taxiway"), LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(3, _T("From Intersection"), LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(4, _T("To Intersection"), LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(5,_T("Enter Segment"), LVCFMT_LEFT, 100, -1) ;
	cxListCtrl.InsertColumn(6,_T("Leave Segment"), LVCFMT_LEFT, 100, -1) ;
	cxListCtrl.InsertColumn(7,_T("Uninterrupted duration"), LVCFMT_LEFT, 100, -1) ;
	cxListCtrl.InsertColumn(8,_T("Delay"), LVCFMT_LEFT, 100, -1) ;


	cxListCtrl.InsertColumn(9,_T("Number of interventions"), LVCFMT_LEFT, 100, -1) ;

	cxListCtrl.InsertColumn(10,_T("Delay 1"), LVCFMT_LEFT, 100, -1) ;
	cxListCtrl.InsertColumn(11,_T("Reason 1"), LVCFMT_LEFT, 100, -1) ;

	cxListCtrl.InsertColumn(13,_T("Reason 2"), LVCFMT_LEFT, 100, -1) ;
	cxListCtrl.InsertColumn(12,_T("Delay 2"), LVCFMT_LEFT, 100, -1) ;

	cxListCtrl.InsertColumn(15,_T("Reason 3"),LVCFMT_LEFT,100,-1) ;
	cxListCtrl.InsertColumn(14,_T("Delay 3"),LVCFMT_LEFT,100,-1) ;

	if (piSHC)
	{
		piSHC->ResetAll();
		piSHC->SetDataType(0,dtSTRING);
		piSHC->SetDataType(1,dtSTRING);
		piSHC->SetDataType(2,dtSTRING);
		piSHC->SetDataType(3,dtSTRING);
		piSHC->SetDataType(4,dtSTRING);
		piSHC->SetDataType(5,dtTIME);
		piSHC->SetDataType(6,dtTIME);
		piSHC->SetDataType(7,dtTIME);
		piSHC->SetDataType(8,dtTIME);
		piSHC->SetDataType(9,dtINT);
		piSHC->SetDataType(10,dtSTRING);
		piSHC->SetDataType(11,dtTIME);
		piSHC->SetDataType(12,dtSTRING);
		piSHC->SetDataType(13,dtTIME);
		piSHC->SetDataType(14,dtSTRING);
		piSHC->SetDataType(15,dtTIME);
	}

}
void CTaxiwayDelayReport::InitListSummaryHead(CXListCtrl& cxListCtrl, CSortableHeaderCtrl* piSHC)
{
	cxListCtrl.DeleteAllItems() ;
	while(cxListCtrl.GetHeaderCtrl()->GetItemCount() >0)
		cxListCtrl.DeleteColumn(0);
	cxListCtrl.InsertColumn(0, _T("Index"), LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(1, _T("Taxiway"), LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(2, _T("From Intersection"), LVCFMT_LEFT, 100, -1);
	cxListCtrl.InsertColumn(3, _T("To Intersection"), LVCFMT_LEFT, 100, -1);

	cxListCtrl.InsertColumn(4,_T("Num aircraft"), LVCFMT_LEFT, 100, -1) ;
	cxListCtrl.InsertColumn(5,_T("Min delay"), LVCFMT_LEFT, 100, -1) ;
	cxListCtrl.InsertColumn(6,_T("Mean Delay"), LVCFMT_LEFT, 100, -1) ;
	cxListCtrl.InsertColumn(7,_T("Max Delay"), LVCFMT_LEFT, 100, -1) ;

	cxListCtrl.InsertColumn(8,_T("Flight type of max delay"), LVCFMT_LEFT, 100, -1) ;
	cxListCtrl.InsertColumn(9,_T("Time start max delay"), LVCFMT_LEFT, 100, -1) ;

	cxListCtrl.InsertColumn(10,_T("Q1"), LVCFMT_LEFT, 100,-1) ;
	cxListCtrl.InsertColumn(11,_T("Q2"), LVCFMT_LEFT, 100,-1) ;
	cxListCtrl.InsertColumn(12,_T("Q3"), LVCFMT_LEFT, 100,-1) ;

	cxListCtrl.InsertColumn(13,_T("P1"), LVCFMT_LEFT, 100,-1) ;
	cxListCtrl.InsertColumn(14,_T("P5"), LVCFMT_LEFT, 100,-1) ;
	cxListCtrl.InsertColumn(15,_T("P90"),LVCFMT_LEFT, 100,-1) ;
	cxListCtrl.InsertColumn(16,_T("P95"),LVCFMT_LEFT, 100,-1) ;
	cxListCtrl.InsertColumn(17,_T("P99"),LVCFMT_LEFT, 100,-1) ;

	if (piSHC)
	{
		piSHC->ResetAll();
		piSHC->SetDataType(0,dtINT);
		piSHC->SetDataType(1,dtSTRING);
		piSHC->SetDataType(2,dtSTRING);
		piSHC->SetDataType(3,dtSTRING);
		piSHC->SetDataType(4,dtINT);
		piSHC->SetDataType(5,dtTIME);
		piSHC->SetDataType(6,dtTIME);
		piSHC->SetDataType(7,dtTIME);
		piSHC->SetDataType(8,dtSTRING);
		piSHC->SetDataType(9,dtTIME);
		piSHC->SetDataType(10,dtTIME);
		piSHC->SetDataType(11,dtTIME);
		piSHC->SetDataType(12,dtTIME);
		piSHC->SetDataType(13,dtTIME);
		piSHC->SetDataType(14,dtTIME);
		piSHC->SetDataType(15,dtTIME);
		piSHC->SetDataType(16,dtTIME);
		piSHC->SetDataType(17,dtTIME);
	}
}
void CTaxiwayDelayReport::FillDetailListContent(CXListCtrl& cxListCtrl, CParameters * parameter)
{
	CTaxiwayDelayReportDetailDataItem* detailDataItem = NULL ;
	for ( int i =  0 ; i< (int)m_ReportDetailData.size();i++)
	{
		InsertDetailItem(cxListCtrl,m_ReportDetailData[i]) ;
	}
}

void CTaxiwayDelayReport::InsertDetailItem(CXListCtrl& cxListCtrl,CTaxiwayDelayReportDetailDataItem* detailDataItem)
{
	if(detailDataItem == NULL||detailDataItem->m_DelayTime.asSeconds() == 0)
		return ;
	CString intervalTime ;
	intervalTime.Format(_T("%s-%s"),detailDataItem->m_startTime.printTime(),detailDataItem->m_EndTime.printTime()) ;
	
	int nNewRow = cxListCtrl.GetItemCount();
	int ndx = cxListCtrl.InsertItem(nNewRow,intervalTime) ;

	cxListCtrl.SetItemText(ndx,1,detailDataItem->m_FlightID) ;

	//taxiway name 
	cxListCtrl.SetItemText(ndx,2,detailDataItem->m_TaxiwayName) ;

	//from intersection
	cxListCtrl.SetItemText(ndx,3,detailDataItem->m_FromIntersection) ;
	//to intersection 
	cxListCtrl.SetItemText(ndx,4,detailDataItem->m_ToIntersection) ;
	//enter segment
	cxListCtrl.SetItemText(ndx,5,detailDataItem->m_EnterSegmentTime.printTime());
	//leave segment
	cxListCtrl.SetItemText(ndx,6,detailDataItem->m_ExistSegmentTime.printTime());
	//uninterrupted time
	cxListCtrl.SetItemText(ndx,7,detailDataItem->m_UnInterruptTime.printTime());
	//total delay 
	cxListCtrl.SetItemText(ndx,8,detailDataItem->m_DelayTime.printTime()) ;

	//number of delay
	intervalTime.Format(_T("%d"),detailDataItem->m_numberofdelay) ;
	cxListCtrl.SetItemText(ndx,9,intervalTime) ;

	//delay 1 
	cxListCtrl.SetItemText(ndx,10,detailDataItem->m_delay1.printTime()) ;
	cxListCtrl.SetItemText(ndx,11,detailDataItem->m_delay1Reason) ;

	//delay 2
	cxListCtrl.SetItemText(ndx,12,detailDataItem->m_delay2.printTime()) ;
	cxListCtrl.SetItemText(ndx,13,detailDataItem->m_delay2Reason) ;

	//delay 3
	cxListCtrl.SetItemText(ndx,14,detailDataItem->m_delay3.printTime()) ;
	cxListCtrl.SetItemText(ndx,15,detailDataItem->m_delay3Reason) ;

}
void CTaxiwayDelayReport::FillSummaryListContent(CXListCtrl& cxListCtrl, CParameters * parameter)
{
	if(parameter == NULL)
		return ;
	for (int i = 0 ; i < (int)m_ReportSummaryData.size() ;i++)
	{
		InsertSummaryDataItem(cxListCtrl,m_ReportSummaryData[i],i+1) ;
	}

}
void CTaxiwayDelayReport::InsertSummaryDataItem(CXListCtrl& cxListCtrl,CTaxiwayDelayReportSummaryDataItem* _sunmmaryDataItem,int _index )
{
	if(_sunmmaryDataItem == NULL)
		return ;
	CString listItemName ;
	if(_sunmmaryDataItem->m_totalDelay.asSeconds() == 0 )
		return ;
	//set index
	listItemName.Format(_T("%d"),_index) ;
	int nNewRow = cxListCtrl.GetItemCount();
	int ndx  = cxListCtrl.InsertItem(nNewRow,listItemName) ;
	//taxi way 
	cxListCtrl.SetItemText(ndx,1,_sunmmaryDataItem->m_Taxiway) ;
	//from intersection
	cxListCtrl.SetItemText(ndx,2,_sunmmaryDataItem->m_FromNode) ;
	//to intersection
	cxListCtrl.SetItemText(ndx,3,_sunmmaryDataItem->m_ToNode) ;
	//num of aircraft
	listItemName.Format(_T("%d"),_sunmmaryDataItem->m_numofaircraft) ;
	cxListCtrl.SetItemText(ndx,4,listItemName) ;
	//min delay
	cxListCtrl.SetItemText(ndx,5,_sunmmaryDataItem->m_MinDelay.printTime()) ;
	//mean delay 
	cxListCtrl.SetItemText(ndx,6,_sunmmaryDataItem->m_MeanDelay.printTime()) ;
	//max delay 
	cxListCtrl.SetItemText(ndx,7,_sunmmaryDataItem->m_MaxDelay.printTime()) ;

	//the flight type of max delay

	cxListCtrl.SetItemText(ndx,8,_sunmmaryDataItem->m_FlightTyOfmaxDelay)  ;
	//the start time of max delay
	cxListCtrl.SetItemText(ndx,9,_sunmmaryDataItem->m_MaxDelayStart.printTime()) ;
	//Q1
	cxListCtrl.SetItemText(ndx,10,_sunmmaryDataItem->m_Q1.printTime()) ;
	//Q2
	cxListCtrl.SetItemText(ndx,11,_sunmmaryDataItem->m_Q2.printTime()) ;
	//q3
	cxListCtrl.SetItemText(ndx,12,_sunmmaryDataItem->m_Q3.printTime()) ;

	//p1
	cxListCtrl.SetItemText(ndx,13,_sunmmaryDataItem->m_P1.printTime()) ;

	//p5
	cxListCtrl.SetItemText(ndx,14,_sunmmaryDataItem->m_P5.printTime()) ;

	//p90
	cxListCtrl.SetItemText(ndx,15,_sunmmaryDataItem->m_P90.printTime()) ;
	//p95
	cxListCtrl.SetItemText(ndx,16,_sunmmaryDataItem->m_P95.printTime()) ;
	//p99
	cxListCtrl.SetItemText(ndx,17,_sunmmaryDataItem->m_P99.printTime()) ;

}
BOOL CTaxiwayDelayReport::ExportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType )
{

	if(reportType == ASReportType_Detail)
	{
			_file.writeInt( (int) m_ReportDetailData.size() ) ;
			_file.writeLine() ;
		for (int i = 0 ; i <(int) m_ReportDetailData.size() ; i++)
		{
			m_ReportDetailData[i]->ExportFile(_file) ;
		}
	}else
	{
		_file.writeInt( (int) m_ReportSummaryData.size() ) ;
		_file.writeLine() ;
		for (int i = 0 ; i <(int) m_ReportSummaryData.size() ; i++)
		{
			m_ReportSummaryData[i]->ExportFile(_file) ;
		}
	}
	return TRUE ;
}
BOOL CTaxiwayDelayReport::ImportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType /* = ASReportType_Detail */)
{
	long num = 0 ;
	_file.getInteger(num) ;
	if(reportType == ASReportType_Detail)
	{
		ClearDetailData() ;
		for (int i = 0 ; i < num ; i++)
		{
			_file.getLine() ;
			CTaxiwayDelayReportDetailDataItem* detailItem = new CTaxiwayDelayReportDetailDataItem ;
			detailItem->ImportFile(_file) ;
			m_ReportDetailData.push_back(detailItem) ;
		}
	}else
	{
		ClearSummaryData() ;
		for (int i = 0 ; i <(int)num ; i++)
		{
			_file.getLine() ;
			CTaxiwayDelayReportSummaryDataItem* summaryItem = new CTaxiwayDelayReportSummaryDataItem ;
			summaryItem->ImportFile(_file) ;
			m_ReportSummaryData.push_back(summaryItem) ;
		}
	}
	return TRUE ;
}

CString CTaxiwayDelayReport::GetReportFileName()
{
	return _T("TaxiwayDelayRep\\TaxiwayDelayreport.rep") ;
}

BOOL CTaxiwayDelayReport::WriteReportData( ArctermFile& _file )
{
	_file.writeField("RunwayExit Report");
	_file.writeLine();
	_file.writeField("Paramerate");
	_file.writeLine();
	m_Paramer->ExportFile(_file) ;
	enumASReportType_Detail_Summary ty = m_Paramer->getReportType() ;
	_file.writeInt(ty) ;
	_file.writeLine() ;
	return ExportReportData(_file,CString(),ty) ;
}

BOOL CTaxiwayDelayReport::ReadReportData( ArctermFile& _file )
{
	_file.getLine() ;
	int ty = 0 ;
	m_Paramer->ImportFile(_file) ;
		_file.getLine();
	if(!_file.getInteger(ty))
		return FALSE ;
	_file.getLine() ;

	return ImportReportData(_file,CString(),(enumASReportType_Detail_Summary)ty) ;
}

CTaxiwayDelayReportDetailDataItem::CTaxiwayDelayReportDetailDataItem()
{
	m_numberofdelay = 0;
	m_DelayTime = 0L;
}

void CTaxiwayDelayReportDetailDataItem::CalculateDelayTime( ElapsedTime _dataItem )
{
	m_DelayTime += _dataItem ;
}