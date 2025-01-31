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


void CComparativeList::RefreshData(const CComparativeReportResult& _reportData)
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
		RefreshData((const CComparativeQTimeReport&)_reportData);
		break;
	case QueueLengthReport:
		RefreshData((const CComparativeQLengthReport&)_reportData);
		break;
	case ThroughtputReport:
		RefreshData((const CComparativeThroughputReport&)_reportData);
		break;
	case SpaceDensityReport:
		RefreshData((const CComparativeSpaceDensityReport&)_reportData);
		break;
	case PaxCountReport:
		RefreshData((const CComparativePaxCountReport&)_reportData);
		break;
	case AcOperationReport:
		RefreshData((const CComparativeAcOperationReport&)_reportData);
		break;
	case TimeTerminalReport:
		RefreshData((const CComparativeTimeTerminalReport &)_reportData);
		break;
	case DistanceTravelReport:
		RefreshData((const CComparativeDistanceTravelReport&)_reportData);
		break;
	default:
		ASSERT(FALSE);
	}
}

void CComparativeList::RefreshData(const CComparativeQLengthReport& _reportData)
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
	const QLengthMap& mapQLength = _reportData.GetResult();
	int nColCount = 1;
	for(int i=1; i<=(int)m_vModelList.size(); i++)
	{
		_stprintf(sData, _T("Length %d"), i);
		CModelToCompare *pModel = m_vModelList[i-1];
		for (int j = 0; j < pModel->GetSimResultCount(); ++j)
		{
			CString strColText = _T("");
			strColText.Format("%s(%s)",pModel->GetModelName(),pModel->GetSimResult(j));
			m_listCtrl.InsertColumn(nColCount, strColText, LVCFMT_CENTER, 50);
			m_listCtrl.SetColumnWidth(nColCount, 80);

			nColCount += 1;
		}
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
void CComparativeList::RefreshData(const CComparativePaxCountReport& _reportData)
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
	for(int i=1; i<=(int)m_vModelList.size(); i++)
	{
		_stprintf(sData, _T("Count %d"), i);
		CModelToCompare *pModel = m_vModelList[i-1];
		for (int j = 0; j < pModel->GetSimResultCount(); ++j)
		{
			CString strColText = _T("");
			strColText.Format("%s(%s)",pModel->GetModelName(),pModel->GetSimResult(j));
			m_listCtrl.InsertColumn(nColCount, strColText, LVCFMT_CENTER, 50);
			m_listCtrl.SetColumnWidth(nColCount, 80);
			nColCount += 1;
		}
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
void CComparativeList::RefreshData(const CComparativeQTimeReport& _reportData)
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
	for(int i=0; i < (int)m_vModelList.size(); i++)
	{
		_stprintf(sData, _T("Passenger Count %d"), i + 1);
//		m_listCtrl.InsertColumn(i + 1 , m_vModelList[i]->GetModelName(), LVCFMT_CENTER, 50);

		CModelToCompare *pModel = m_vModelList[i];
		for (int j = 0; j < pModel->GetSimResultCount(); ++j)
		{
			CString strColText = _T("");
			strColText.Format("%s(%s)",pModel->GetModelName(),pModel->GetSimResult(j));
			m_listCtrl.InsertColumn(nColCount, strColText, LVCFMT_CENTER, 50);
			m_listCtrl.SetColumnWidth(nColCount, 80);
			nColCount += 1;
		}
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

void CComparativeList::RefreshData(const CComparativeThroughputReport& _reportData)
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

	int iColCount = 2;
	for(int i=0; i<(int)m_vModelList.size(); i++)
	{
		_stprintf(sData, _T("Pax Served %d"), i + 1);
//		m_listCtrl.InsertColumn(i + 2, m_vModelList[i]->GetModelName(), LVCFMT_CENTER, 50);
//		m_listCtrl.SetColumnWidth(i + 2, 80);
		CModelToCompare *pModel = m_vModelList[i];
		for (int j = 0; j < pModel->GetSimResultCount(); ++j)
		{
			CString strColText = _T("");
			strColText.Format("%s(%s)",pModel->GetModelName(),pModel->GetSimResult(j));
			m_listCtrl.InsertColumn(iColCount, strColText, LVCFMT_CENTER, 50);
			m_listCtrl.SetColumnWidth(iColCount, 80);
			iColCount += 1;
		}
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


void CComparativeList::RefreshData(const CComparativeAcOperationReport& _reportData)
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

	int iColCount = 1;
	for(int i=0; i < (int)m_vModelList.size(); i++)
	{
		_stprintf(sData, _T("Operation Count %d"), i + 1);
//		m_listCtrl.InsertColumn(i + 1 , m_vModelList[i]->GetModelName(), LVCFMT_CENTER, 50);
		CModelToCompare *pModel = m_vModelList[i];
		for (int j = 0; j < pModel->GetSimResultCount(); ++j)
		{
			CString strColText = _T("");
			strColText.Format("%s(%s)",pModel->GetModelName(),pModel->GetSimResult(j));
			m_listCtrl.InsertColumn(iColCount, strColText, LVCFMT_CENTER, 50);
			m_listCtrl.SetColumnWidth(iColCount, 80);
			iColCount += 1;
		}
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

void CComparativeList::RefreshData(const CComparativeSpaceDensityReport& _reportData)
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

	int iColCount = 1;
	for(int i=1; i<=(int)m_vModelList.size(); i++)
	{
		_stprintf(sData, _T("Count %d"), i);
//		m_listCtrl.InsertColumn(i, m_vModelList[i-1]->GetModelName(), LVCFMT_CENTER, 50);
//		m_listCtrl.SetColumnWidth(i, 80);
		CModelToCompare *pModel = m_vModelList[i-1];
		for (int j = 0; j < pModel->GetSimResultCount(); ++j)
		{
			CString strColText = _T("");
			strColText.Format("%s(%s)",pModel->GetModelName(),pModel->GetSimResult(j));
			m_listCtrl.InsertColumn(iColCount, strColText, LVCFMT_CENTER, 50);
			m_listCtrl.SetColumnWidth(iColCount, 80);
			iColCount += 1;
		}
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
void CComparativeList::RefreshData(const CComparativeTimeTerminalReport& _reportData)
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
	int iColCount = 1;
	for(int i=0; i < (int)m_vModelList.size(); i++)
	{
		_stprintf(sData, _T("Passenger Count %d"), i + 1);
	//	m_listCtrl.InsertColumn(i + 1 , m_vModelList[i]->GetModelName(), LVCFMT_CENTER, 50);
		CModelToCompare *pModel = m_vModelList[i];
		for (int j = 0; j < pModel->GetSimResultCount(); ++j)
		{
			CString strColText = _T("");
			strColText.Format("%s(%s)",pModel->GetModelName(),pModel->GetSimResult(j));
			m_listCtrl.InsertColumn(iColCount, strColText, LVCFMT_CENTER, 50);
			m_listCtrl.SetColumnWidth(iColCount, 80);
			iColCount += 1;
		}
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


void CComparativeList::RefreshData(const CComparativeDistanceTravelReport& _reportData)
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
	
	int iColCount = 1;
	for(int i=0; i < (int)m_vModelList.size(); i++)
	{
		_stprintf(sData, _T("Passenger Count %d"), i + 1);
//		m_listCtrl.InsertColumn(i + 1 , m_vModelList[i]->GetModelName(), LVCFMT_CENTER, 50);
		CModelToCompare *pModel = m_vModelList[i];
		for (int j = 0; j < pModel->GetSimResultCount(); ++j)
		{
			CString strColText = _T("");
			strColText.Format("%s(%s)",pModel->GetModelName(),pModel->GetSimResult(j));
			m_listCtrl.InsertColumn(iColCount, strColText, LVCFMT_CENTER, 50);
			m_listCtrl.SetColumnWidth(iColCount, 80);
			iColCount += 1;
		}
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






















