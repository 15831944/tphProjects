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
#include "MFCExControl/SortableHeaderCtrl.h"


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


	//load detail report 
	if(_reportData.m_cmpParam.GetReportDetail()==REPORT_TYPE_DETAIL)
	{
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
		const QTimeMap& mapQTime = _reportData.GetResultDetail();
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
	else //load summary report
	{
		//insert column project name and run
		DWORD headStyle = LVCFMT_CENTER;
		headStyle &= ~HDF_OWNERDRAW;

		m_listCtrl.InsertColumn(0,"",headStyle,30);
		GetListHeaderCtrl()->SetDataType( 0,  dtINT);
		m_listCtrl.InsertColumn( 1, _T("RUN"), headStyle, 60);
		GetListHeaderCtrl()->SetDataType( 1,  dtSTRING);

		const int m_nColumnCount = 15;
		CString csColumnLabel[m_nColumnCount];
		int nDefaultColumnWidth[m_nColumnCount];
		int nFormat[m_nColumnCount];
		EDataType type[m_nColumnCount];

	
		csColumnLabel[0] = CString("Min");
		csColumnLabel[1] = CString("Avg");
		csColumnLabel[2] = CString("Max");

		CString str = " (s)";
		csColumnLabel[3] = CString("Q1");
		csColumnLabel[4] = CString("Q2");
		csColumnLabel[5] = CString("Q3");
		csColumnLabel[6] = CString("P1");
		csColumnLabel[7] = CString("P5");
		csColumnLabel[8] = CString("P10");
		csColumnLabel[9] = CString("P90");
		csColumnLabel[10] = CString("P95");
		csColumnLabel[11] = CString("P99");
		csColumnLabel[12] = CString("Sigma");

		csColumnLabel[13] = CString("Passenger Type");
		csColumnLabel[14] = CString("Count");



		nDefaultColumnWidth[0] = 60;
		nDefaultColumnWidth[1] = 60;
		nDefaultColumnWidth[2] = 60;
		nDefaultColumnWidth[3] = 60;
		nDefaultColumnWidth[4] = 60;
		nDefaultColumnWidth[5] = 60;
		nDefaultColumnWidth[6] = 60;
		nDefaultColumnWidth[7] = 60;
		nDefaultColumnWidth[8] = 60;
		nDefaultColumnWidth[9] = 60;
		nDefaultColumnWidth[10] = 60;
		nDefaultColumnWidth[11] = 60;
		nDefaultColumnWidth[12] = 60;
		nDefaultColumnWidth[13] = 120;
		nDefaultColumnWidth[14] = 60;


		for( int i=0; i<15; i++ )
		{
			nFormat[i] = LVCFMT_CENTER; 
		}

		type[0] = dtDATETIME;	
		type[1] = dtDATETIME;
		type[2] = dtDATETIME;
		type[3] = dtDATETIME;
		type[4] = dtDATETIME;
		type[5] = dtDATETIME;
		type[6] = dtDATETIME;
		type[7] = dtDATETIME;
		type[8] = dtDATETIME;
		type[9] = dtDATETIME;
		type[10] = dtDATETIME;
		type[11] = dtDATETIME;
		type[12] = dtDATETIME;
		type[13] = dtSTRING;
		type[14] = dtINT;

		for( int i=0; i<m_nColumnCount; i++ )
		{
			DWORD dwStyle = nFormat[i];
			dwStyle &= ~HDF_OWNERDRAW;

			GetListCtrl().InsertColumn( i+2, csColumnLabel[i], /*nFormat[i]*/dwStyle, nDefaultColumnWidth[i] );
			GetListHeaderCtrl()->SetDataType( i+2, type[i] );
		}

		const MultiRunsReport::Summary::SummaryQTimeList&  dataList = _reportData.GetResultSummary();
		
		
		
		size_t nResultSize = dataList.size();
		const std::vector<CString>& simNamelist =  _reportData.GetSimNameList(); 
		//insert average
		int nItemIndex = 0;
		for (size_t i = 0; i < nResultSize; ++i)
		{	
			CString simName=_T("Error");
			if(i<simNamelist.size())
				simName  = simNamelist[i];

			size_t nPaxTypeCount = dataList[i].size();

			for(size_t j=0;j<nPaxTypeCount;++j)
			{
				const MultiRunsReport::Summary::SummaryQueueTimeValue& data = dataList[i][j];
				CString strIndex;
				strIndex.Format(_T("%d"),i+1);
				GetListCtrl().InsertItem(nItemIndex,strIndex);
				GetListCtrl().SetItemText(nItemIndex,1,simName);


				GetListCtrl().SetItemText(nItemIndex,2,data.eMinimum.printTime());
				GetListCtrl().SetItemText(nItemIndex,3,data.eAverage.printTime());
				GetListCtrl().SetItemText(nItemIndex,4,data.eMaximum.printTime());

				GetListCtrl().SetItemText(nItemIndex,5,data.eQ1.printTime());
				GetListCtrl().SetItemText(nItemIndex,6,data.eQ2.printTime());
				GetListCtrl().SetItemText(nItemIndex,7,data.eQ3.printTime());
				GetListCtrl().SetItemText(nItemIndex,8,data.eP1.printTime());
				GetListCtrl().SetItemText(nItemIndex,9,data.eP5.printTime());
				GetListCtrl().SetItemText(nItemIndex,10,data.eP10.printTime());
				GetListCtrl().SetItemText(nItemIndex,11,data.eP90.printTime());
				GetListCtrl().SetItemText(nItemIndex,12,data.eP95.printTime());
				GetListCtrl().SetItemText(nItemIndex,13,data.eP99.printTime());
				GetListCtrl().SetItemText(nItemIndex,14,data.eSigma.printTime());
				GetListCtrl().SetItemText(nItemIndex,15,data.strPaxType);

				CString strCount;
				strCount.Format(_T("%d"),data.nCount);
				GetListCtrl().SetItemText(nItemIndex,16,strCount);


				GetListCtrl().SetItemData(nItemIndex,nItemIndex);
				nItemIndex++;
			}			
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
	int nColCount = 0;
	m_listCtrl.InsertColumn( 0 , _T("Start Time"), LVCFMT_CENTER, 100); 
	nColCount++;
	m_listCtrl.InsertColumn( 1, _T("End Time"), LVCFMT_CENTER, 100);
	nColCount++;

	CString strColText;
	std::vector<CString>& vSimName = _reportData.GetSimNameList();
	for(int i=0; i<(int)vSimName.size(); i++)
	{
		strColText = vSimName[i];
		m_listCtrl.InsertColumn(nColCount, strColText, LVCFMT_CENTER, 80);
		nColCount ++;
	}
	//set list control
	int nRow = 0, nCol =0;
	std::vector<CompThroughputData> vData = _reportData.GetResult();
	for(std::vector<CompThroughputData>::const_iterator iterLine = vData.begin(); 
		iterLine != vData.end(); iterLine++, nRow++)
	{
		iterLine->etStart.printTime(sData);
		CString str = GetRegularDateTime(sData, TRUE);
		nCol = 0;
		m_listCtrl.InsertItem(nRow, str);
		nCol++;

		iterLine->etEnd.printTime(sData);
		str = GetRegularDateTime(sData, TRUE);
		m_listCtrl.SetItemText(nRow, nCol, str);
		nCol++;

// 		for(std::vector<int>::const_iterator iterLength=iterLine->vPaxServed.begin(); 
// 			iterLength!=iterLine->vPaxServed.end(); iterLength++, nCol++)
// 		{
// 			_stprintf(sData, "%d", *iterLength );
// 			m_listCtrl.SetItemText(nRow, nCol, sData );
// 		}
		for(int i=0; i<(int)vSimName.size(); i++)
		{
			strColText = vSimName[i];
			m_listCtrl.SetItemText(nRow, nCol, strColText);
			nCol ++;
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























