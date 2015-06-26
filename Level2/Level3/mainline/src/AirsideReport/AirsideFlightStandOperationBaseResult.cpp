#include "StdAfx.h"
#include "AirsideFlightStandOperationBaseResult.h"
#include "Parameters.h"
#include "StandOperationData.h"
#include "FlightStandOperationChartResult.h"
#include "FlightStandOperationParameter.h"
#include "AirsideFlightStandOperationReport.h"
#include "../Results/AirsideFlightEventLog.h"
#include "StandOperationDataProcessor.h"
#include "FlightStandOperationDetailChartReport.h"
#include "FlightStandOperationSummaryChartReport.h"
#include "StandOperationDataCalculation.h"

static LPCTSTR const sSummaryDataType[] =
{
	_T("Sched stand utilization time"),
	_T("Sched stand idle time"),

	_T("Actual stand utilization time"),
	_T("Actual stand idle time"),

	_T("Stand delay time"),
	_T("Stand conflict")
};
///////////base result class//////////////////////////////////////////////////////////////////////////////////
CAirsideFlightStandOperationBaseResult::CAirsideFlightStandOperationBaseResult()
:m_pChartResult(NULL)
,m_nUnuseActualStandCount(0)
,m_nUnuseScheduleStandCount(0)
,m_pStandOperationProcessor(NULL)
{

}

CAirsideFlightStandOperationBaseResult::~CAirsideFlightStandOperationBaseResult()
{
	
}

void CAirsideFlightStandOperationBaseResult::setLoadFromFile( bool bLoad )
{
	m_bLoadFromFile = bLoad;
}

bool CAirsideFlightStandOperationBaseResult::IsLoadFromFile()
{
	return m_bLoadFromFile;

}

void CAirsideFlightStandOperationBaseResult::SetCBGetFilePath( CBGetLogFilePath pFunc )
{
	m_pCBGetLogFilePath= pFunc;
}

//void CAirsideFlightStandOperationBaseResult::clear()
//{
//	for (int i = 0; i < (int)m_vResult.size(); i++)
//	{
//		delete m_vResult[i];
//	}
//	m_vResult.clear();
//}

CString CAirsideFlightStandOperationBaseResult::FormatDHMS( long lSecs ) /* format Day.Hour:Minute:Second */
{
	CString strTime;
#if 0 // original simple
	ElapsedTime t(lSecs);
	strTime.Format(_T("Day%d %02d:%02d:%02d"),t.GetDay(),t.GetHour(),t.GetMinute(),t.GetSecond());
#else // for speed
	long lS = lSecs%60;
	lSecs /= 60;
	long lM = lSecs%60;
	lSecs /= 60;
	long lH = lSecs%24;
	long lD = lSecs/24;
	strTime.Format(_T("Day%d %02d:%02d:%02d"), ++lD, lH, lM, lS);
#endif
	return strTime;
}

CString CAirsideFlightStandOperationBaseResult::FormatHMS( long lSecs ) /* format Hour:Minute:Second */
{
	CString strTime;
#if 0 // original simple
	ElapsedTime t(lSecs);
	strTime.Format(_T("%02d:%02d:%02d"),t.GetHour(),t.GetMinute(),t.GetSecond());
#else // for speed
	if (lSecs >= 0)
	{
		long lS = lSecs%60;
		lSecs /= 60;
		long lM = lSecs%60;
		lSecs /= 60;
		long lH = lSecs%24;
		strTime.Format(_T("%02d:%02d:%02d"), lH, lM, lS);
	}
	else // takecare of the negtive number
	{
		lSecs = -lSecs;
		long lS = lSecs%60;
		lSecs /= 60;
		long lM = lSecs%60;
		lSecs /= 60;
		long lH = lSecs%24;
		strTime.Format(_T("-%02d:%02d:%02d"), lH, lM, lS);
	}
#endif
	return strTime;
}

CString CAirsideFlightStandOperationBaseResult::PrintHMS( double dSecs )
{
	CString strTime;

	if (dSecs >= 0)
	{
		long hours = (long) (dSecs / 3600L);
		long min = (long) ((dSecs - (hours * 3600L)) / 60L);
		long sec = (long) (dSecs - (hours * 3600L) - (min * 60L));
		strTime.Format(_T("%d:%02d:%02d"), hours, min, sec);
	}
	else // takecare of the negtive number
	{
		dSecs = -dSecs;
		long hours = (long) (dSecs / 3600L);
		long min = (long) ((dSecs - (hours * 3600L)) / 60L);
		long sec = (long) (dSecs - (hours * 3600L) - (min * 60L));
		strTime.Format(_T("-%d:%02d:%02d"), hours, min, sec);
	}

	return strTime;
}

void CAirsideFlightStandOperationBaseResult::GenerateResult( CBGetLogFilePath pCBGetLogFilePath,CParameters *pParameter )
{
	if (pParameter == NULL)
		return;

	//clear();
	if (m_pStandOperationProcessor)
	{
		delete m_pStandOperationProcessor;
		m_pStandOperationProcessor = NULL;
	}

	m_pStandOperationProcessor = new StandOperationDataProcessor(m_pScheduleStand);

	CFlightStandOperationParameter* pStandOpPara = (CFlightStandOperationParameter*)pParameter;

	m_pStandOperationProcessor->LoadDataAndProcess(pCBGetLogFilePath,pParameter);

	m_vResult = m_pStandOperationProcessor->GetData();
	m_nUnuseActualStandCount = m_pStandOperationProcessor->GetUnuseActualStandCount();
	m_nUnuseScheduleStandCount = m_pStandOperationProcessor->GetUnuseScheduleStandCount();
	RefreshReport(pParameter);
}

BOOL CAirsideFlightStandOperationBaseResult::ExportReportData( ArctermFile& _file,CString& Errmsg )
{
	_file.writeField("Stand Operations Report");
	_file.writeLine();

	_file.writeInt(ASReportType_Detail);
	_file.writeLine();

	_file.writeInt(m_nUnuseScheduleStandCount);
	_file.writeInt(m_nUnuseActualStandCount);

	int nCount = (int)m_vResult.size();
	_file.writeInt(nCount);
	_file.writeLine();
	for (int nIndex = 0; nIndex < nCount; nIndex++)
	{
		CStandOperationReportData* pData = m_vResult.at(nIndex);

		_file.writeField(pData->m_sID);
		_file.writeInt(pData->m_lFlightIndex);
		_file.writeField(pData->m_sACType);
		_file.writeChar(pData->m_fltmode);

		_file.writeField(pData->m_sSchedName);
		_file.writeInt(pData->m_lSchedOn);
		_file.writeInt(pData->m_lSchedOff);
		_file.writeInt(pData->m_lSchedOccupancy);
		_file.writeInt(pData->m_lSchedAvailableOccupancy);

		_file.writeField(pData->m_sActualName);
		_file.writeInt(pData->m_lActualOn);
		_file.writeInt(pData->m_lActualOff);
		_file.writeInt(pData->m_lActualOccupancy);
		_file.writeInt(pData->m_lActualAvailableOccupancy);

		_file.writeInt(pData->m_lDelayEnter);
		_file.writeInt(pData->m_lDueStandOccupied);

		_file.writeInt(pData->m_lDelayLeaving);
		_file.writeInt(pData->m_lDueTaxiwayOccupied);
		_file.writeInt(pData->m_lDueGSE);
		_file.writeInt(pData->m_lDuePushbackClearance);

		_file.writeLine();
	}
	return TRUE;
}

BOOL CAirsideFlightStandOperationBaseResult::ImportReportData( ArctermFile& _file,CString& Errmsg )
{
	_file.getLine();
	_file.getInteger(m_nUnuseScheduleStandCount);
	_file.getInteger(m_nUnuseActualStandCount);

	int nCount = 0;
	_file.getInteger(nCount);
	_file.getLine();

	for (int nIndex = 0; nIndex < nCount; nIndex++)
	{
		CStandOperationReportData* pData = new CStandOperationReportData;

		_file.getField(pData->m_sID.GetBuffer(1024),1024);
		pData->m_sID.ReleaseBuffer();
		_file.getInteger(pData->m_lFlightIndex);
		_file.getField(pData->m_sACType.GetBuffer(1024),1024);
		pData->m_sACType.ReleaseBuffer();
		_file.getChar(pData->m_fltmode);

		_file.getField(pData->m_sSchedName.GetBuffer(1024),1024);
		pData->m_sSchedName.ReleaseBuffer();
		_file.getInteger(pData->m_lSchedOn);
		_file.getInteger(pData->m_lSchedOff);
		_file.getInteger(pData->m_lSchedOccupancy);
		_file.getInteger(pData->m_lSchedAvailableOccupancy);

		_file.getField(pData->m_sActualName.GetBuffer(1024),1024);
		pData->m_sActualName.ReleaseBuffer();
		_file.getInteger(pData->m_lActualOn);
		_file.getInteger(pData->m_lActualOff);
		_file.getInteger(pData->m_lActualOccupancy);
		_file.getInteger(pData->m_lActualAvailableOccupancy);

		_file.getInteger(pData->m_lDelayEnter);
		_file.getInteger(pData->m_lDueStandOccupied);

		_file.getInteger(pData->m_lDelayLeaving);
		_file.getInteger(pData->m_lDueTaxiwayOccupied);
		_file.getInteger(pData->m_lDueGSE);
		_file.getInteger(pData->m_lDuePushbackClearance);

		_file.getLine();
		m_vResult.push_back(pData);
	}
	return TRUE;
}

BOOL CAirsideFlightStandOperationBaseResult::ExportListData( ArctermFile& _file,CParameters * parameter )
{
	return TRUE;
}

BOOL CAirsideFlightStandOperationBaseResult::WriteReportData( ArctermFile& _file )
{
	_file.writeField("Stand Operations Report");
	_file.writeLine();

	_file.writeInt(ASReportType_Detail);
	_file.writeLine();

	_file.writeInt(m_nUnuseScheduleStandCount);
	_file.writeInt(m_nUnuseActualStandCount);
	int nCount = (int)m_vResult.size();
	_file.writeInt(nCount);
	_file.writeLine();
	for (int nIndex = 0; nIndex < nCount; nIndex++)
	{
		CStandOperationReportData* pData = m_vResult.at(nIndex);

		_file.writeField(pData->m_sID);
		_file.writeInt(pData->m_lFlightIndex);
		_file.writeField(pData->m_sACType);
		_file.writeChar(pData->m_fltmode);

		_file.writeField(pData->m_sSchedName);
		_file.writeInt(pData->m_lSchedOn);
		_file.writeInt(pData->m_lSchedOff);
		_file.writeInt(pData->m_lSchedOccupancy);
		_file.writeInt(pData->m_lSchedAvailableOccupancy);

		_file.writeField(pData->m_sActualName);
		_file.writeInt(pData->m_lActualOn);
		_file.writeInt(pData->m_lActualOff);
		_file.writeInt(pData->m_lActualOccupancy);
		_file.writeInt(pData->m_lActualAvailableOccupancy);

		_file.writeInt(pData->m_lDelayEnter);
		_file.writeInt(pData->m_lDueStandOccupied);

		_file.writeInt(pData->m_lDelayLeaving);
		_file.writeInt(pData->m_lDueTaxiwayOccupied);
		_file.writeInt(pData->m_lDueGSE);
		_file.writeInt(pData->m_lDuePushbackClearance);

		_file.writeLine();
	}
	return TRUE;
}

BOOL CAirsideFlightStandOperationBaseResult::ReadReportData( ArctermFile& _file )
{
	_file.getInteger(m_nUnuseScheduleStandCount);
	_file.getInteger(m_nUnuseActualStandCount);
	int nCount = 0;
	_file.getInteger(nCount);
	_file.getLine();

	for (int nIndex = 0; nIndex < nCount; nIndex++)
	{
		CStandOperationReportData* pData = new CStandOperationReportData;

		_file.getField(pData->m_sID.GetBuffer(1024),1024);
		pData->m_sID.ReleaseBuffer();
		_file.getInteger(pData->m_lFlightIndex);
		_file.getField(pData->m_sACType.GetBuffer(1024),1024);
		pData->m_sACType.ReleaseBuffer();
		_file.getChar(pData->m_fltmode);

		_file.getField(pData->m_sSchedName.GetBuffer(1024),1024);
		pData->m_sSchedName.ReleaseBuffer();
		_file.getInteger(pData->m_lSchedOn);
		_file.getInteger(pData->m_lSchedOff);
		_file.getInteger(pData->m_lSchedOccupancy);
		_file.getInteger(pData->m_lSchedAvailableOccupancy);

		_file.getField(pData->m_sActualName.GetBuffer(1024),1024);
		pData->m_sActualName.ReleaseBuffer();
		_file.getInteger(pData->m_lActualOn);
		_file.getInteger(pData->m_lActualOff);
		_file.getInteger(pData->m_lActualOccupancy);
		_file.getInteger(pData->m_lActualAvailableOccupancy);

		_file.getInteger(pData->m_lDelayEnter);
		_file.getInteger(pData->m_lDueStandOccupied);

		_file.getInteger(pData->m_lDelayLeaving);
		_file.getInteger(pData->m_lDueTaxiwayOccupied);
		_file.getInteger(pData->m_lDueGSE);
		_file.getInteger(pData->m_lDuePushbackClearance);

		_file.getLine();
		m_vResult.push_back(pData);
	}
	return TRUE;
}

///////////////////////////////detail result//////////////////////////////////////////////////////////
CDetailStandOperationResult::CDetailStandOperationResult()
{

}

CDetailStandOperationResult::~CDetailStandOperationResult()
{

}

void CDetailStandOperationResult::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{
	if (m_pChartResult)
	{
		m_pChartResult->Draw3DChart(chartWnd,pParameter);
	}
}

void CDetailStandOperationResult::RefreshReport(CParameters * parameter)
{
	CFlightStandOperationParameter* pParam = (CFlightStandOperationParameter*)parameter;
	ASSERT(pParam != NULL);

	if (NULL != m_pChartResult)
	{
		delete m_pChartResult;
		m_pChartResult = NULL;
	}

	switch(pParam->getSubType())
	{
	case CAirsideFlightStandOperationReport::ChartType_Detail_Occupancy:
		{
			m_pChartResult = new CDetailStandOccupancyUtilizationChartResult;
		}
		break;
	case CAirsideFlightStandOperationReport::ChartType_Detail_Percentage:
		{
			m_pChartResult = new CDetailStandUtilizationPercentageChartResult;
		}
		break;
	case CAirsideFlightStandOperationReport::ChartType_Detail_Conflict:
		{
			m_pChartResult = new CDetailStandConflictChartResult;
		}
		break;
	case CAirsideFlightStandOperationReport::ChartType_Detail_Delay:
		{
			m_pChartResult = new CDetailStandDelayChartResult;
		}
		break;
	case CAirsideFlightStandOperationReport::ChartType_Detail_IdleTime:
		{
			m_pChartResult = new CDetailStandIdleTimeUtiliztionChartResult;
		}
		break;
	default:
		return;
	}

	if (m_pChartResult)
	{
		m_pChartResult->SetUnuseActualStandCount(m_nUnuseActualStandCount);
		m_pChartResult->SetUnuseScheduleStandCount(m_nUnuseScheduleStandCount);
		m_pChartResult->GenerateResult(m_vResult, parameter);
	}
	
}

void CDetailStandOperationResult::InitListHead(CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType,CSortableHeaderCtrl* piSHC)
{
	cxListCtrl.DeleteAllItems();

	while(cxListCtrl.GetHeaderCtrl()->GetItemCount() >0)
		cxListCtrl.DeleteColumn(0);

	cxListCtrl.InsertColumn(0, _T("ID"), LVCFMT_LEFT, 80);	
	cxListCtrl.InsertColumn(1, _T("AC Type"), LVCFMT_LEFT, 50);

	cxListCtrl.InsertColumn(2, _T("Sched Stand"), LVCFMT_LEFT, 80);
	cxListCtrl.InsertColumn(3, _T("Sched On(Day.hh:mm:ss)"),LVCFMT_LEFT,80);
	cxListCtrl.InsertColumn(4, _T("Sched Off(Day.hh:mm:ss)"),LVCFMT_LEFT,80);
	cxListCtrl.InsertColumn(5, _T("Sched occupancy(hh:mm:ss)"),LVCFMT_LEFT,80);

	cxListCtrl.InsertColumn(6, _T("Actual Stand"),LVCFMT_LEFT,80);
	cxListCtrl.InsertColumn(7, _T("Actual On(Day.hh:mm:ss)"),LVCFMT_LEFT,80);
	cxListCtrl.InsertColumn(8, _T("Actual Off(Day.hh:mm:ss)"),LVCFMT_LEFT,80);
	cxListCtrl.InsertColumn(9, _T("Actual occupancy(hh:mm:ss)"),LVCFMT_LEFT,80);

	cxListCtrl.InsertColumn(10, _T("Delay entering(hh:mm:ss)"),LVCFMT_LEFT,80);
	cxListCtrl.InsertColumn(11, _T("Due to stand occupied(hh:mm:ss)"),LVCFMT_LEFT,80);

	cxListCtrl.InsertColumn(12, _T("Delay leaving(hh:mm:ss)"),LVCFMT_LEFT,80);
	cxListCtrl.InsertColumn(13, _T("Due to taxiway occupied(hh:mm:ss)"),LVCFMT_LEFT,80);
	cxListCtrl.InsertColumn(14, _T("Due to GSE(hh:mm:ss)"),LVCFMT_LEFT,80);
	cxListCtrl.InsertColumn(15, _T("Due to pushback clearance(hh:mm:ss)"),LVCFMT_LEFT,80);

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
		piSHC->SetDataType(7,dtSTRING);
		piSHC->SetDataType(8,dtSTRING);
		piSHC->SetDataType(9,dtSTRING);
		piSHC->SetDataType(10,dtSTRING);
		piSHC->SetDataType(11,dtSTRING);
		piSHC->SetDataType(12,dtSTRING);
		piSHC->SetDataType(13,dtSTRING);
		piSHC->SetDataType(14,dtSTRING);
		piSHC->SetDataType(15,dtSTRING);
	}
}

void CDetailStandOperationResult::FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter)
{
	for (int i=0; i<(int)m_vResult.size(); i++)
	{
		CStandOperationReportData* pData = m_vResult[i];

		if (pData->m_sActualName.IsEmpty() == true)
			continue;
		
		int nIndex = cxListCtrl.InsertItem(i, pData->m_sID);

		cxListCtrl.SetItemText(nIndex,  1, pData->m_sACType);

		cxListCtrl.SetItemText(nIndex,  2, pData->m_sSchedName);
		cxListCtrl.SetItemText(nIndex,  3, FormatDHMS(pData->m_lSchedOn/100));
		cxListCtrl.SetItemText(nIndex,  4, FormatDHMS(pData->m_lSchedOff/100));
		cxListCtrl.SetItemText(nIndex,  5, FormatHMS(pData->m_lSchedOccupancy/100));

		cxListCtrl.SetItemText(nIndex,  6, pData->m_sActualName);	
		cxListCtrl.SetItemText(nIndex,  7, FormatDHMS(pData->m_lActualOn/100));
		cxListCtrl.SetItemText(nIndex,  8, FormatDHMS(pData->m_lActualOff/100));
		cxListCtrl.SetItemText(nIndex,  9, FormatHMS(pData->m_lActualOccupancy/100));

		cxListCtrl.SetItemText(nIndex, 10, FormatHMS(pData->m_lDelayEnter/100));
		cxListCtrl.SetItemText(nIndex, 11, FormatHMS(pData->m_lDueStandOccupied/100));

		cxListCtrl.SetItemText(nIndex, 12, FormatHMS(pData->m_lDelayLeaving/100));
		cxListCtrl.SetItemText(nIndex, 13, FormatHMS(pData->m_lDueTaxiwayOccupied/100));
		cxListCtrl.SetItemText(nIndex, 14, FormatHMS(pData->m_lDueGSE/100));
		cxListCtrl.SetItemText(nIndex, 15, FormatHMS(pData->m_lDuePushbackClearance/100));
	
	}	
}

//////////////////////////////summary result//////////////////////////////////////////////////////////
CSummaryStandOperationResult::CSummaryStandOperationResult()
{

}

CSummaryStandOperationResult::~CSummaryStandOperationResult()
{
	clearData();
}

void CSummaryStandOperationResult::RebuildResultData(CFlightStandOperationParameter* pParam, const std::vector<CStandOperationReportData*>& vStandOpData, const CString& strStandName)
{
	std::vector<CStandOperationReportData*> vSchedFitData;
	std::vector<CStandOperationReportData*> vActualFitData;

	size_t nSize = vStandOpData.size();
	for (size_t i = 0; i < nSize; i++)
	{
		CStandOperationReportData* pOpData = vStandOpData.at(i);

		ALTObjectID SchedStandObj(pOpData->m_sSchedName);
		ALTObjectID ActualStandObj(pOpData->m_sActualName);
		ALTObjectID checkID(strStandName);

		if (SchedStandObj.idFits(checkID) && SchedStandObj.IsBlank() == false)		//Schedule data
			vSchedFitData.push_back(pOpData);

		if (ActualStandObj.idFits(checkID) && ActualStandObj.IsBlank() == false)		//Actual data
			vActualFitData.push_back(pOpData);
	}
	
	std::vector<long> vSchedOccupiedTime;
	std::vector<long> vActualOccupiedTime;
	std::vector<long> vStandDelay;
	std::vector<int> vStandConflict;

	long lDuration = pParam->getEndTime().asSeconds() - pParam->getStartTime().asSeconds();

	CalculateStandOccupancyTime(true, vSchedFitData,vSchedOccupiedTime);
	CalculateStandOccupancyTime(false, vActualFitData, vActualOccupiedTime);
	for (int nScheduleUnuse = 0; nScheduleUnuse < m_nUnuseScheduleStandCount; nScheduleUnuse++)
	{
		vSchedOccupiedTime.push_back(0l);
	}

	for (int nActualUnuse = 0; nActualUnuse < m_nUnuseActualStandCount; nActualUnuse++)
	{
		vActualOccupiedTime.push_back(0l);
	}
	CalculateStandConflict(vActualFitData, vStandConflict);
	CalculateStandDelay(vActualFitData, vStandDelay);

	//Sched stand
	CSummaryStandOperationData* pSchedStandUtilizeSummaryData = new CSummaryStandOperationData;
	pSchedStandUtilizeSummaryData->m_sName = strStandName;
	pSchedStandUtilizeSummaryData->m_eDataType = CSummaryStandOperationData::SchedStandUtilizeSummaryData;

	CSummaryStandOperationData* pSchedStandIdleSummaryData = new CSummaryStandOperationData;
	pSchedStandIdleSummaryData->m_sName = strStandName;
	pSchedStandIdleSummaryData->m_eDataType = CSummaryStandOperationData::SchedStandIdleSummaryData;

	nSize = vSchedOccupiedTime.size();
	for (size_t i =0; i < nSize; i++)
	{
		pSchedStandUtilizeSummaryData->m_dTotal += vSchedOccupiedTime.at(i);
		pSchedStandUtilizeSummaryData->m_SummaryData.AddNewData(vSchedOccupiedTime.at(i));

		long lTime = lDuration - vSchedOccupiedTime.at(i);
		if(lTime < 0)
			lTime = 0;

		pSchedStandIdleSummaryData->m_dTotal += lTime;
		pSchedStandIdleSummaryData->m_SummaryData.AddNewData(lTime);
	}
    pSchedStandUtilizeSummaryData->m_SummaryData.SortData();
    pSchedStandIdleSummaryData->m_SummaryData.SortData();
	m_vSummaryData.push_back(pSchedStandUtilizeSummaryData);
	m_vSummaryData.push_back(pSchedStandIdleSummaryData);

	
	//Actual stand
	CSummaryStandOperationData* pActualStandUtilizeSummaryData = new CSummaryStandOperationData;
	pActualStandUtilizeSummaryData->m_sName = strStandName;
	pActualStandUtilizeSummaryData->m_eDataType = CSummaryStandOperationData::ActualStandUtilizeSummaryData;

	CSummaryStandOperationData* pActualStandIdleSummaryData = new CSummaryStandOperationData;
	pActualStandIdleSummaryData->m_sName = strStandName;
	pActualStandIdleSummaryData->m_eDataType = CSummaryStandOperationData::ActualStandIdleSummaryData;

	nSize = vActualOccupiedTime.size();
	for (size_t i =0; i < nSize; i++)
	{
		pActualStandUtilizeSummaryData->m_dTotal += vActualOccupiedTime.at(i);
		pActualStandUtilizeSummaryData->m_SummaryData.AddNewData(vActualOccupiedTime.at(i));

		long lTime = lDuration - vActualOccupiedTime.at(i);
		if(lTime < 0)
			lTime = 0;

		pActualStandIdleSummaryData->m_dTotal += lTime;
		pActualStandIdleSummaryData->m_SummaryData.AddNewData(lTime);
	}
    pActualStandUtilizeSummaryData->m_SummaryData.SortData();
    pActualStandIdleSummaryData->m_SummaryData.SortData();
	m_vSummaryData.push_back(pActualStandUtilizeSummaryData);
	m_vSummaryData.push_back(pActualStandIdleSummaryData);


	//Conflict
	CSummaryStandOperationData* pStandConflictSummaryData = new CSummaryStandOperationData;
	pStandConflictSummaryData->m_sName = strStandName;
	pStandConflictSummaryData->m_eDataType = CSummaryStandOperationData::StandConflictSummaryData;

	nSize = vStandConflict.size();
	for (size_t i =0; i < nSize; i++)
	{
		int nConfictCount = vStandConflict.at(i);
		pStandConflictSummaryData->m_dTotal += nConfictCount;
		pStandConflictSummaryData->m_SummaryData.AddNewData(nConfictCount);
	}
    pStandConflictSummaryData->m_SummaryData.SortData();
	m_vSummaryData.push_back(pStandConflictSummaryData);


	//Delay
	CSummaryStandOperationData* pStandDelaySummaryData = new CSummaryStandOperationData;
	pStandDelaySummaryData->m_sName = strStandName;
	pStandDelaySummaryData->m_eDataType = CSummaryStandOperationData::StandDelaySummaryData;

	nSize = vStandDelay.size();
	for (size_t i =0; i < nSize; i++)
	{
		pStandDelaySummaryData->m_dTotal += vStandDelay.at(i);
		pStandDelaySummaryData->m_SummaryData.AddNewData(vStandDelay.at(i));
	}
    pStandDelaySummaryData->m_SummaryData.SortData();
	m_vSummaryData.push_back(pStandDelaySummaryData);
}

//void CSummaryStandOperationResult::GenerateResult(CBGetLogFilePath pCBGetLogFilePath,CParameters *pParameter)
//{
//	if (pParameter == NULL)
//		return;
//
//	clear();
//	StandOperationDataProcessor standOperationAnalysis;
//	standOperationAnalysis.LoadDataAndProcess(pCBGetLogFilePath,pParameter);
//
//	CFlightStandOperationParameter* pStandOpPara = (CFlightStandOperationParameter*)pParameter;
//
//
//
//	RefreshReport(pParameter);
//}

void CSummaryStandOperationResult::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{
	if (m_pChartResult)
	{
		m_pChartResult->Draw3DChart(chartWnd,pParameter);
	}
}

void CSummaryStandOperationResult::RefreshReport(CParameters * parameter)
{
	ASSERT(parameter != NULL);

	clearData();

	CFlightStandOperationParameter* pParam = (CFlightStandOperationParameter*)parameter;

	int nStandCount = pParam->getCount();
	CProgressBar bar(_T("Generate summary stand operations report..."),100,nStandCount,TRUE);
	for (int i = 0; i < nStandCount; i++)
	{
		CString strStandName = pParam->getItem(i).GetIDString();
		RebuildResultData(pParam, m_vResult, strStandName);
		bar.StepIt();
	}

	if (NULL != m_pChartResult)
	{
		delete m_pChartResult;
		m_pChartResult = NULL;
	}

	switch(pParam->getSubType())
	{
	case CAirsideFlightStandOperationReport::ChartType_Summary_Conflict:
		{
			m_pChartResult = new CSummaryStandConflictChartResult;
			((CSummaryStandConflictChartResult*)m_pChartResult)->GenerateResult(m_vSummaryData);
		}
		break;
	case CAirsideFlightStandOperationReport::ChartType_Summary_Delay:
		{
			m_pChartResult = new CSummaryStandDelayChartResult;
			((CSummaryStandDelayChartResult*)m_pChartResult)->GenerateResult(m_vSummaryData);
		}
		break;
	case CAirsideFlightStandOperationReport::ChartType_Summary_SchedUtilization:
		{
			m_pChartResult = new CSummarySchedStandUtilizationChartResult;
			((CSummarySchedStandUtilizationChartResult*)m_pChartResult)->GenerateResult(m_vSummaryData);
		}
		break;
	case CAirsideFlightStandOperationReport::ChartType_Summary_ActualUtilization:
		{
			m_pChartResult = new CSummaryActualStandUtilizationChartResult;
			((CSummaryActualStandUtilizationChartResult*)m_pChartResult)->GenerateResult(m_vSummaryData);
		}
		break;
	case CAirsideFlightStandOperationReport::ChartType_Summary_SchedIdle:
		{
			m_pChartResult = new CSummarySchedStandIdleChartResult;
			((CSummarySchedStandIdleChartResult*)m_pChartResult)->GenerateResult(m_vSummaryData);
		}
		break;
	case CAirsideFlightStandOperationReport::ChartType_Summary_ActualIdle:
		{
			m_pChartResult = new CSummaryActualStandIdleChartResult;
			((CSummaryActualStandIdleChartResult*)m_pChartResult)->GenerateResult(m_vSummaryData);
		}
		break;
	default:
		return;
	}
}

void CSummaryStandOperationResult::InitListHead(CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType,CSortableHeaderCtrl* piSHC)
{
	cxListCtrl.DeleteAllItems();

	while(cxListCtrl.GetHeaderCtrl()->GetItemCount() >0)
		cxListCtrl.DeleteColumn(0);

	cxListCtrl.InsertColumn(0, _T("Stands"), LVCFMT_LEFT, 80);	
	cxListCtrl.InsertColumn(1, _T("Data type"), LVCFMT_LEFT, 100);
	cxListCtrl.InsertColumn(2, _T("Total"), LVCFMT_LEFT, 60);
	cxListCtrl.InsertColumn(3, _T("Min"), LVCFMT_LEFT, 60);
	cxListCtrl.InsertColumn(4, _T("Avg"), LVCFMT_LEFT, 60);
	cxListCtrl.InsertColumn(5, _T("Max"),LVCFMT_LEFT, 60);

	cxListCtrl.InsertColumn(6, _T("Q1"),LVCFMT_LEFT, 60);
	cxListCtrl.InsertColumn(7, _T("Q2"),LVCFMT_LEFT, 60);
	cxListCtrl.InsertColumn(8, _T("Q3"),LVCFMT_LEFT, 60);

	cxListCtrl.InsertColumn(9, _T("P1"),LVCFMT_LEFT, 60);
	cxListCtrl.InsertColumn(10, _T("P5"),LVCFMT_LEFT, 60);
	cxListCtrl.InsertColumn(11, _T("P10"),LVCFMT_LEFT, 60);
	cxListCtrl.InsertColumn(12, _T("P90"),LVCFMT_LEFT, 60);
	cxListCtrl.InsertColumn(13, _T("P95"),LVCFMT_LEFT, 60);
	cxListCtrl.InsertColumn(14, _T("P99"),LVCFMT_LEFT, 60);

	cxListCtrl.InsertColumn(15, _T("Std Dev"),LVCFMT_LEFT, 60);

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
		piSHC->SetDataType(7,dtSTRING);
		piSHC->SetDataType(8,dtSTRING);

		piSHC->SetDataType(9,dtSTRING);
		piSHC->SetDataType(10,dtSTRING);
		piSHC->SetDataType(11,dtSTRING);
		piSHC->SetDataType(12,dtSTRING);
		piSHC->SetDataType(13,dtSTRING);
		piSHC->SetDataType(14,dtSTRING);

		piSHC->SetDataType(15,dtSTRING);
	}
}

void CSummaryStandOperationResult::FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter)
{
	size_t nSize = m_vSummaryData.size();
	for (size_t i=0; i <nSize; i++)
	{
		CSummaryStandOperationData* pSummaryData = m_vSummaryData.at(i);

		CString strName(_T(""));

		if (pSummaryData->m_sName.IsEmpty())
			strName = CString(_T("ALL"));
		else
			strName = pSummaryData->m_sName;
		cxListCtrl.InsertItem(i,strName);

		cxListCtrl.SetItemText(i,1,sSummaryDataType[(int)pSummaryData->m_eDataType]);

		if (pSummaryData->m_eDataType != CSummaryStandOperationData::StandConflictSummaryData)
		{
		//	ElapsedTime tTotal(pSummaryData->m_dTotal);		
		//	cxListCtrl.SetItemText(i,2,tTotal.printTime());
			CString strTotal = PrintHMS(pSummaryData->m_dTotal);
			cxListCtrl.SetItemText(i,2,strTotal);
			

			ElapsedTime tMin(pSummaryData->m_SummaryData.GetMin());		
			cxListCtrl.SetItemText(i,3,tMin.printTime());

			ElapsedTime tAvg(pSummaryData->m_SummaryData.GetAvarage() );		
			cxListCtrl.SetItemText(i,4,tAvg.printTime());

			ElapsedTime tMax(pSummaryData->m_SummaryData.GetMax());		
			cxListCtrl.SetItemText(i,5,tMax.printTime());

			ElapsedTime tQ1(pSummaryData->m_SummaryData.GetPercentile(25) );		
			cxListCtrl.SetItemText(i,6,tQ1.printTime());

			ElapsedTime tQ2(pSummaryData->m_SummaryData.GetPercentile(50) );		
			cxListCtrl.SetItemText(i,7,tQ2.printTime());

			ElapsedTime tQ3(pSummaryData->m_SummaryData.GetPercentile(75) );		
			cxListCtrl.SetItemText(i,8,tQ3.printTime());

			ElapsedTime tP1(pSummaryData->m_SummaryData.GetPercentile(1) );		
			cxListCtrl.SetItemText(i,9,tP1.printTime());

			ElapsedTime tP5(pSummaryData->m_SummaryData.GetPercentile(5) );		
			cxListCtrl.SetItemText(i,10,tP5.printTime());

			ElapsedTime tP10(pSummaryData->m_SummaryData.GetPercentile(10) );		
			cxListCtrl.SetItemText(i,11,tP10.printTime());

			ElapsedTime tP90(pSummaryData->m_SummaryData.GetPercentile(90) );		
			cxListCtrl.SetItemText(i,12,tP90.printTime());

			ElapsedTime tP95(pSummaryData->m_SummaryData.GetPercentile(95) );		
			cxListCtrl.SetItemText(i,13,tP95.printTime());

			ElapsedTime tP99(pSummaryData->m_SummaryData.GetPercentile(99) );		
			cxListCtrl.SetItemText(i,14,tP99.printTime());

			ElapsedTime tSig(pSummaryData->m_SummaryData.GetSigma());		
			cxListCtrl.SetItemText(i,15,tSig.printTime());
		}
		else
		{
			CString strValue;
			strValue.Format("%.2f", pSummaryData->m_dTotal);		
			cxListCtrl.SetItemText(i,2,strValue);

			strValue.Format("%.2f",pSummaryData->m_SummaryData.GetMin());		
			cxListCtrl.SetItemText(i,3,strValue);

			strValue.Format("%.2f",pSummaryData->m_SummaryData.GetAvarage() );		
			cxListCtrl.SetItemText(i,4,strValue);

			strValue.Format("%.2f",pSummaryData->m_SummaryData.GetMax());		
			cxListCtrl.SetItemText(i,5,strValue);

			strValue.Format("%.2f",pSummaryData->m_SummaryData.GetPercentile(25) );		
			cxListCtrl.SetItemText(i,6,strValue);

			strValue.Format("%.2f",pSummaryData->m_SummaryData.GetPercentile(50) );		
			cxListCtrl.SetItemText(i,7,strValue);

			strValue.Format("%.2f",pSummaryData->m_SummaryData.GetPercentile(75) );		
			cxListCtrl.SetItemText(i,8,strValue);

			strValue.Format("%.2f",pSummaryData->m_SummaryData.GetPercentile(1) );		
			cxListCtrl.SetItemText(i,9,strValue);

			strValue.Format("%.2f",pSummaryData->m_SummaryData.GetPercentile(5) );		
			cxListCtrl.SetItemText(i,10,strValue);

			strValue.Format("%.2f",pSummaryData->m_SummaryData.GetPercentile(10) );		
			cxListCtrl.SetItemText(i,11,strValue);

			strValue.Format("%.2f",pSummaryData->m_SummaryData.GetPercentile(90) );		
			cxListCtrl.SetItemText(i,12,strValue);

			strValue.Format("%.2f",pSummaryData->m_SummaryData.GetPercentile(95) );		
			cxListCtrl.SetItemText(i,13,strValue);

			strValue.Format("%.2f",pSummaryData->m_SummaryData.GetPercentile(99) );		
			cxListCtrl.SetItemText(i,14,strValue);

			strValue.Format("%.2f",pSummaryData->m_SummaryData.GetSigma());		
			cxListCtrl.SetItemText(i,15,strValue);
		}
	

	}
}

void CSummaryStandOperationResult::clearData()
{
	size_t nSize = m_vSummaryData.size();
	for (size_t i =0; i < nSize; i++)
	{
		delete m_vSummaryData.at(i);
	}
	m_vSummaryData.clear();
}