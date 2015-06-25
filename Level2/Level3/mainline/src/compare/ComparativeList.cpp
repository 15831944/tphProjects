// ComparativeList.cpp: implementation of the CComparativeList class.
#include "stdafx.h"
#include "ComparativeList.h"
#include "../compare/ComparativeReportResult.h"
#include "../compare/ComparativeQTimeReport.h"
#include "../compare/ComparativeQLengthReport.h"
#include "../compare/ComparativeThroughputReport.h"
#include "../compare/ComparativeSpaceDensityReport.h"
#include "../compare/ComparativeDistanceTravelReport.h"
#include "../compare/ComparativeTimeTerminalReport.h"
#include "../Engine/terminal.h"
#include "../common/SimAndReportManager.h"
#include "../compare/ComparativePaxCountReport.h"
#include "../compare/ComparativeAcOperationReport.h"
#include "../inputs/simparameter.h"
#include "../compare/ModelsManager.h"
#include "../compare/ModelToCompare.h"


CString CComparativeList::GetRegularDateTime(LPCTSTR elaptimestr, bool needsec)
{
	CSimAndReportManager *ptSim = m_pTerminal->GetSimReportManager();
	CStartDate tstartdate = ptSim->GetStartDate();
	ElapsedTime etime;
	etime.SetTime(elaptimestr);
	
	CString retstr, tstr;
	bool bAbsDate;
	COleDateTime _tdate, _ttime;
	int _nDtIdx;
	tstartdate.GetDateTime(etime, bAbsDate, _tdate, _nDtIdx, _ttime);
	if(!needsec)
		tstr = _ttime.Format(" %H:%M");
	else
		tstr = _ttime.Format(" %H:%M:%S");
	if(bAbsDate)
	{
		retstr = _tdate.Format("%Y-%m-%d");
	}
	else
	{
		retstr.Format("Day%d", _nDtIdx + 1 );
	}
	retstr += tstr;
	
	return retstr;
}


void CComparativeList::RefreshData(const CCmpBaseReport& _reportData)
{
	if(::IsWindow( m_listCtrl.GetSafeHwnd()) == FALSE || (m_listCtrl.GetStyle() & LVS_REPORT) == 0)
		return ;
	
	CHeaderCtrl* pHeader = m_listCtrl.GetHeaderCtrl();
	ASSERT(pHeader);
	for (int i = 0; i <= pHeader->GetItemCount(); i++)
	{
		m_listCtrl.DeleteColumn(0);
	}

	switch( _reportData.GetReportType())
	{
	case QueueTimeReport:
		RefreshData((CComparativeQTimeReport&)_reportData);
		break;
	case QueueLengthReport:
		RefreshData((CComparativeQLengthReport&)_reportData);
		break;
	case ThroughtputReport:
		RefreshData((CComparativeThroughputReport&)_reportData);
		break;
	case SpaceDensityReport:
		RefreshData((CComparativeSpaceDensityReport&)_reportData);
		break;
	case PaxCountReport:
		RefreshData((CComparativePaxCountReport&)_reportData);
		break;
	case AcOperationReport:
		RefreshData((CComparativeAcOperationReport&)_reportData);
		break;
	case TimeTerminalReport:
		RefreshData((CComparativeTimeTerminalReport &)_reportData);
		break;
	case DistanceTravelReport:
		RefreshData((CComparativeDistanceTravelReport&)_reportData);
		break;
	default:
		ASSERT(FALSE);
	}
}

void CComparativeList::RefreshData(CComparativeQLengthReport& _reportData)
{
	ASSERT( ::IsWindow(m_listCtrl.GetSafeHwnd()) && m_listCtrl.GetStyle()&LVS_REPORT);
	//clear old data
	m_listCtrl.DeleteAllItems();
	int nColumnCount = m_listCtrl.GetHeaderCtrl()->GetItemCount();
	// Delete all of the columns.
	for (int i=0;i < nColumnCount;i++)
	{
		m_listCtrl.DeleteColumn(0);
	}

	if (_reportData.m_cmpParam.GetReportDetail() == REPORT_TYPE_DETAIL)
	{
		//set header control
		TCHAR sData[32]	= _T("");
		m_listCtrl.InsertColumn( 0 , _T("Time"));
		m_listCtrl.SetColumnWidth(0, 100);
		const QLengthMap& mapQLength = _reportData.GetResult();
		std::vector<CString>& vSimName = _reportData.GetSimNameList();
		int nColCount = 1;
		CString strColText;
		for(int i=0; i<(int)vSimName.size(); i++)
		{
			_stprintf(sData, _T("Length %d"), i+1);
			strColText = vSimName[i];
			m_listCtrl.InsertColumn(nColCount, strColText, LVCFMT_CENTER, 50);
			m_listCtrl.SetColumnWidth(nColCount, 80);
			nColCount += 1;
		}
		//set list control
		int nRow = 0, nCol =0;
		for(QLengthMap::const_iterator iterLine = mapQLength.begin(); iterLine!=mapQLength.end(); iterLine++, nRow++)
		{
			iterLine->first.printTime(sData);
			CString str = GetRegularDateTime(sData, TRUE);
			m_listCtrl.InsertItem(nRow, str );
			//m_listCtrl.InsertItem(nRow, sData );
			const std::vector<int>& vLength = iterLine->second;
			nCol =1;
			for(std::vector<int>::const_iterator iterLength=vLength.begin(); iterLength!=vLength.end(); iterLength++, nCol++)
			{
				_stprintf(sData, "%d", *iterLength );
				m_listCtrl.SetItemText(nRow, nCol, sData );
			}
		}
	}
	else
	{
		m_listCtrl.InsertColumn( 0 , _T("Name"),LVCFMT_CENTER,100);
		m_listCtrl.InsertColumn(1, _T("Time"),LVCFMT_CENTER,100);
		m_listCtrl.InsertColumn(2,_T("Min Queue Length"),LVCFMT_CENTER,50);
		m_listCtrl.InsertColumn(3,_T("Max  Queue Length"),LVCFMT_CENTER,50);
		m_listCtrl.InsertColumn(4,_T("Average  Queue Length"),LVCFMT_CENTER,50);
		m_listCtrl.InsertColumn(5,_T("Total  Queue Length"),LVCFMT_CENTER,50);

		const QLengthSummaryMap& queueLengthSummary = _reportData.GetSummaryResult();
		TCHAR sData[32] = _T("");
		int nRow = 0;
		for(QLengthSummaryMap::const_iterator iterLine = queueLengthSummary.begin(); iterLine!=queueLengthSummary.end(); iterLine++, nRow++)
		{
			iterLine->first.printTime(sData);
			CString strTime = GetRegularDateTime(sData,TRUE);
			const std::vector<QueueLengthGroup>& vLength = iterLine->second;
			for(std::vector<QueueLengthGroup>::const_iterator iterLength=vLength.begin(); iterLength!=vLength.end(); iterLength++)
			{
				m_listCtrl.InsertItem(nRow,iterLength->m_strModelName);
				m_listCtrl.SetItemText(nRow,1,strTime);

				//min length
				CString strMin;
				strMin.Format(_T("%d"),iterLength->m_nMinValue);
				m_listCtrl.SetItemText(nRow,2,strMin);

				//max length
				CString strMax;
				strMax.Format(_T("%d"),iterLength->m_nMaxValue);
				m_listCtrl.SetItemText(nRow,3,strMax);

				//ava length
				CString strAva;
				strAva.Format(_T("%d"),iterLength->m_nAvaValue);
				m_listCtrl.SetItemText(nRow,4,strAva);

				//total length
				CString strTotal;
				strTotal.Format(_T("%d"),iterLength->m_nTotalValue);
				m_listCtrl.SetItemText(nRow,5,strTotal);
			}
		}
	}

}
void CComparativeList::RefreshData(CComparativePaxCountReport& _reportData)
{
	ASSERT( ::IsWindow(m_listCtrl.GetSafeHwnd()) && m_listCtrl.GetStyle()&LVS_REPORT);
	//clear old data
	m_listCtrl.DeleteAllItems();
	int nColumnCount = m_listCtrl.GetHeaderCtrl()->GetItemCount();
	// Delete all of the columns.
	for (int i=0;i < nColumnCount;i++)
	{
		m_listCtrl.DeleteColumn(0);
	}

	//set header control
	TCHAR sData[32]	= _T("");
	m_listCtrl.InsertColumn( 0 , _T("Time"));
	m_listCtrl.SetColumnWidth(0, 100);

	int nColCount = 1;
	CString strColText;
	std::vector<CString>& vSimName = _reportData.GetSimNameList();
	for(int i=0; i<(int)vSimName.size(); i++)
	{
		_stprintf(sData, _T("Count %d"), i+1);
		strColText = vSimName[i];
		m_listCtrl.InsertColumn(nColCount, strColText, LVCFMT_CENTER, 50);
		m_listCtrl.SetColumnWidth(nColCount, 80);
		nColCount += 1;
	}
	//set list control
	int nRow = 0, nCol =0;
	const PaxCountMap& mapPaxDens = _reportData.GetResult();
	for(QLengthMap::const_iterator iterLine = mapPaxDens.begin(); 
		iterLine!=mapPaxDens.end(); iterLine++, nRow++)
	{
		iterLine->first.printTime(sData);
		CString str = GetRegularDateTime(sData, TRUE);
		m_listCtrl.InsertItem(nRow, str );
		//m_listCtrl.InsertItem(nRow, sData );
		const std::vector<int>& vLength = iterLine->second;
		nCol =1;
		for(std::vector<int>::const_iterator iterLength=vLength.begin(); iterLength!=vLength.end(); iterLength++, nCol++)
		{
			_stprintf(sData, "%d", *iterLength );
			m_listCtrl.SetItemText(nRow, nCol, sData );
		}
	}
}
void CComparativeList::RefreshData(CComparativeQTimeReport& _reportData)
{
	ASSERT( ::IsWindow(m_listCtrl.GetSafeHwnd()) &&  m_listCtrl.GetStyle()&LVS_REPORT);
	//clear old data
	m_listCtrl.DeleteAllItems();
	int nColumnCount = m_listCtrl.GetHeaderCtrl()->GetItemCount();
	// Delete all of the columns.
	for (int i=0;i < nColumnCount;i++)
	{
		m_listCtrl.DeleteColumn(0);
	}

	//set header control
	TCHAR sData[32]	= _T("");

	//_stprintf(sData, _T("Total Time"), i*2+1);
	strcpy(sData, _T("Total Time"));
	m_listCtrl.InsertColumn(0, sData, LVCFMT_CENTER, 50);

	std::vector<CString>& vSimName = _reportData.GetSimNameList();
	int nColCount = 1;
	CString strColText;
	for(int i=0; i<(int)vSimName.size(); i++)
	{
		_stprintf(sData, _T("Passenger Count %d"), i+1);
		strColText = vSimName[i];
		m_listCtrl.InsertColumn(nColCount, strColText, LVCFMT_CENTER, 50);
		m_listCtrl.SetColumnWidth(nColCount, 80);
		nColCount += 1;
	}
	//set list control
	
	int nRow = 0, nCol =0;
	//const std::vector<CmpQTimeVector>& vMultiQTime = _reportData.GetResult();
	const QTimeMap& mapQTime = _reportData.GetResult();
	for(QLengthMap::const_iterator iterLine = mapQTime.begin(); iterLine!=mapQTime.end(); iterLine++, nRow++)
	{
		iterLine->first.printTime(sData);
		m_listCtrl.InsertItem(nRow, sData );
		const std::vector<int>& vLength = iterLine->second;
		nCol =1;
		for(std::vector<int>::const_iterator iterLength=vLength.begin(); iterLength!=vLength.end(); iterLength++, nCol++)
		{
			_stprintf(sData, "%d", *iterLength );
			m_listCtrl.SetItemText(nRow, nCol, sData );
		}
	}
}

void CComparativeList::RefreshData(CComparativeThroughputReport& _reportData)
{
	ASSERT( ::IsWindow(m_listCtrl.GetSafeHwnd()) && m_listCtrl.GetStyle()&LVS_REPORT);
	//clear old data
	m_listCtrl.DeleteAllItems();
	int nColumnCount = m_listCtrl.GetHeaderCtrl()->GetItemCount();
	// Delete all of the columns.
	for (int i=0;i < nColumnCount;i++)
	{
		m_listCtrl.DeleteColumn(0);
	}


	//set header control
	TCHAR sData[32]	= _T("");
	m_listCtrl.InsertColumn( 0 , _T("Start Time"));
	m_listCtrl.SetColumnWidth(0, 100);
	m_listCtrl.InsertColumn( 1, _T("End Time"));
	m_listCtrl.SetColumnWidth(1, 100);

	int nColCount = 2;
	CString strColText;
	std::vector<CString>& vSimName = _reportData.GetSimNameList();
	for(int i=0; i<(int)vSimName.size(); i++)
	{
		_stprintf(sData, _T("Pax Served %d"), i+1);
		strColText = vSimName[i];
		m_listCtrl.InsertColumn(nColCount, strColText, LVCFMT_CENTER, 50);
		m_listCtrl.SetColumnWidth(nColCount, 80);
		nColCount += 1;
	}
	//set list control
	int nRow = 0, nCol =0;
	//const CompThroughputData& mapQLength = _reportData.GetResult();
	std::vector<CompThroughputData> vData = _reportData.GetResult();
	for(std::vector<CompThroughputData>::const_iterator iterLine = vData.begin(); 
		iterLine != vData.end(); iterLine++, nRow++)
	{
		iterLine->etStart.printTime(sData);
		CString str = GetRegularDateTime(sData, TRUE);
		m_listCtrl.InsertItem(nRow, str );

		iterLine->etEnd.printTime(sData);
		str = GetRegularDateTime(sData, TRUE);
		m_listCtrl.SetItemText(nRow, 1, str );

		//m_listCtrl.InsertItem(nRow, sData );
		nCol =2;
		for(std::vector<int>::const_iterator iterLength=iterLine->vPaxServed.begin(); 
			iterLength!=iterLine->vPaxServed.end(); iterLength++, nCol++)
		{
			_stprintf(sData, "%d", *iterLength );
			m_listCtrl.SetItemText(nRow, nCol, sData );
		}
	}
}


void CComparativeList::RefreshData(CComparativeAcOperationReport& _reportData)
{
	ASSERT( ::IsWindow(m_listCtrl.GetSafeHwnd()) &&  m_listCtrl.GetStyle()&LVS_REPORT);
	//clear old data
	m_listCtrl.DeleteAllItems();
	int nColumnCount = m_listCtrl.GetHeaderCtrl()->GetItemCount();
	// Delete all of the columns.
	for (int i=0;i < nColumnCount;i++)
	{
		m_listCtrl.DeleteColumn(0);
	}

	//set header control
	TCHAR sData[32]	= _T("");
	
	strcpy(sData, _T("Time"));
	m_listCtrl.InsertColumn(0, sData, LVCFMT_CENTER, 50);

	int nColCount = 1;
	CString strColText;
	std::vector<CString>& vSimName = _reportData.GetSimNameList();
	for(int i=0; i<(int)vSimName.size(); i++)
	{
		_stprintf(sData, _T("Operation Count %d"), i+1);
		strColText = vSimName[i];
		m_listCtrl.InsertColumn(nColCount, strColText, LVCFMT_CENTER, 50);
		m_listCtrl.SetColumnWidth(nColCount, 80);
		nColCount += 1;
	}
	//set list control

	int nRow = 0, nCol =0;
	//const std::vector<CmpQTimeVector>& vMultiQTime = _reportData.GetResult();
	const AcOperationMap& mapQTime = _reportData.GetResult();
	for(AcOperationMap::const_iterator iterLine = mapQTime.begin(); iterLine!=mapQTime.end(); iterLine++, nRow++)
	{		
//		iterLine->first.printTime(sData);
		ElapsedTime tmptime(iterLine->first.asSeconds() % WholeDay);
		tmptime.printTime ( sData );


		CStartDate sDate = m_pTerminal->m_pSimParam->GetStartDate();
		bool bAbsDate;
		COleDateTime date;
		int nDtIdx;
		COleDateTime time;
		CString sDateTimeStr;
		sDate.GetDateTime( iterLine->first, bAbsDate, date, nDtIdx, time );
		if( bAbsDate )
		{
			sDateTimeStr = date.Format(_T("%Y-%m-%d"));
		}
		else
		{
			sDateTimeStr.Format("Day %d", nDtIdx + 1 );
		}
		sDateTimeStr += " ";
		sDateTimeStr += sData;

		m_listCtrl.InsertItem(nRow, sDateTimeStr );
		const std::vector<int>& vLength = iterLine->second;
		nCol =1;
		for(std::vector<int>::const_iterator iterLength=vLength.begin(); iterLength!=vLength.end(); iterLength++, nCol++)
		{
			_stprintf(sData, "%d", *iterLength );
			m_listCtrl.SetItemText(nRow, nCol, sData );
		}
	}

}

void CComparativeList::RefreshData(CComparativeSpaceDensityReport& _reportData)
{
	ASSERT( ::IsWindow(m_listCtrl.GetSafeHwnd()) && m_listCtrl.GetStyle()&LVS_REPORT);
	//clear old data
	m_listCtrl.DeleteAllItems();

	int nColumnCount = m_listCtrl.GetHeaderCtrl()->GetItemCount();
	// Delete all of the columns.
	for (int i=0;i < nColumnCount;i++)
	{
		m_listCtrl.DeleteColumn(0);
	}


	//set header control
	TCHAR sData[32]	= _T("");
	m_listCtrl.InsertColumn( 0 , _T("Time"));
	m_listCtrl.SetColumnWidth(0, 100);

	int nColCount = 1;
	CString strColText;
	std::vector<CString>& vSimName = _reportData.GetSimNameList();
	for(int i=0; i<(int)vSimName.size(); i++)
	{
		_stprintf(sData, _T("Count %d"), i+1);
		strColText = vSimName[i];
		m_listCtrl.InsertColumn(nColCount, strColText, LVCFMT_CENTER, 50);
		m_listCtrl.SetColumnWidth(nColCount, 80);
		nColCount += 1;
	}
	//set list control
	int nRow = 0, nCol =0;
	const PaxDensMap& mapPaxDens = _reportData.GetResult();
	for(QLengthMap::const_iterator iterLine = mapPaxDens.begin(); 
		iterLine!=mapPaxDens.end(); iterLine++, nRow++)
	{
		iterLine->first.printTime(sData);
		CString str = GetRegularDateTime(sData, TRUE);
		m_listCtrl.InsertItem(nRow, str );
		//m_listCtrl.InsertItem(nRow, sData );
		const std::vector<int>& vLength = iterLine->second;
		nCol =1;
		for(std::vector<int>::const_iterator iterLength=vLength.begin(); iterLength!=vLength.end(); iterLength++, nCol++)
		{
			_stprintf(sData, "%d", *iterLength );
			m_listCtrl.SetItemText(nRow, nCol, sData );
		}
	}

}
void CComparativeList::RefreshData(CComparativeTimeTerminalReport& _reportData)
{
	ASSERT( ::IsWindow(m_listCtrl.GetSafeHwnd()) &&  m_listCtrl.GetStyle()&LVS_REPORT);
	//clear old data
	m_listCtrl.DeleteAllItems();
	int nColumnCount = m_listCtrl.GetHeaderCtrl()->GetItemCount();
	// Delete all of the columns.
	for (int i=0;i < nColumnCount;i++)
	{
		m_listCtrl.DeleteColumn(0);
	}

	//set header control
	TCHAR sData[32]	= _T("");

	//_stprintf(sData, _T("Total Time"), i*2+1);
	strcpy(sData, _T("Total Time"));
	m_listCtrl.InsertColumn(0, sData, LVCFMT_CENTER, 50);

	int nColCount = 1;
	CString strColText;
	std::vector<CString>& vSimName = _reportData.GetSimNameList();
	for(int i=0; i<(int)vSimName.size(); i++)
	{
		_stprintf(sData, _T("Passenger Count %d"), i+1);
		strColText = vSimName[i];
		m_listCtrl.InsertColumn(nColCount, strColText, LVCFMT_CENTER, 50);
		m_listCtrl.SetColumnWidth(nColCount, 80);
		nColCount += 1;
	}
	//set list control

	int nRow = 0, nCol =0;
	//const std::vector<CmpQTimeVector>& vMultiQTime = _reportData.GetResult();
	const QTimeMap& mapQTime = _reportData.GetResult();
	for(QLengthMap::const_iterator iterLine = mapQTime.begin(); iterLine!=mapQTime.end(); iterLine++, nRow++)
	{
		iterLine->first.printTime(sData);
		m_listCtrl.InsertItem(nRow, sData );
		const std::vector<int>& vLength = iterLine->second;
		nCol =1;
		for(std::vector<int>::const_iterator iterLength=vLength.begin(); iterLength!=vLength.end(); iterLength++, nCol++)
		{
			_stprintf(sData, "%d", *iterLength );
			m_listCtrl.SetItemText(nRow, nCol, sData );
		}
	}
}


void CComparativeList::RefreshData(CComparativeDistanceTravelReport& _reportData)
{
	ASSERT( ::IsWindow(m_listCtrl.GetSafeHwnd()) &&  m_listCtrl.GetStyle()&LVS_REPORT);
	//clear old data
	m_listCtrl.DeleteAllItems();
	int nColumnCount = m_listCtrl.GetHeaderCtrl()->GetItemCount();
	// Delete all of the columns.
	for (int i=0;i < nColumnCount;i++)
	{
		m_listCtrl.DeleteColumn(0);
	}

	//set header control
	TCHAR sData[32]	= _T("");

	//_stprintf(sData, _T("Total Time"), i*2+1);
	strcpy(sData, _T("Total Distance"));
	m_listCtrl.InsertColumn(0, sData, LVCFMT_CENTER, 50);
	
	int nColCount = 1;
	CString strColText;
	std::vector<CString>& vSimName = _reportData.GetSimNameList();
	for(int i=0; i<(int)vSimName.size(); i++)
	{
		_stprintf(sData, _T("Passenger Count %d"), i+1);
		strColText = vSimName[i];
		m_listCtrl.InsertColumn(nColCount, strColText, LVCFMT_CENTER, 50);
		m_listCtrl.SetColumnWidth(nColCount, 80);
		nColCount += 1;
	}
	//set list control
	
	int nRow = 0, nCol =0;

	const DistanceMap& mapDistance = _reportData.GetResult();

	for(DistanceMap::const_iterator iterLine = mapDistance.begin(); iterLine!=mapDistance.end(); iterLine++, nRow++)
	{	
		CString strLabel = _T("");
		strLabel.Format("%d",iterLine->first);
		m_listCtrl.InsertItem(nRow, strLabel );
		const std::vector<int>& vLength = iterLine->second;
		nCol =1;
		for(std::vector<int>::const_iterator iterLength=vLength.begin(); iterLength!=vLength.end(); iterLength++, nCol++)
		{
			_stprintf(sData, "%d", *iterLength );
			m_listCtrl.SetItemText(nRow, nCol, sData );
		}
	}
}






















