#include "StdAfx.h"
#include ".\lsresourcequeuedetailresult.h"
#include "../MFCExControl/SortableHeaderCtrl.h"
#include "../MFCExControl/XListCtrl.h"

#include "AirsideReport/CARC3DChart.h"
#include <algorithm>

#include "LSResourceQueueParam.h"


LSResourceQueueDetailResult::LSResourceQueueDetailResult(void)
{
}

LSResourceQueueDetailResult::~LSResourceQueueDetailResult(void)
{
}


void LSResourceQueueDetailResult::Draw3DChart( CARC3DChart& chartWnd, LandsideBaseParam *pParameter )
{
	//draw chart
	if(pParameter == NULL)
		return;

	CString strLabel = _T("");
	C2DChartData c2dGraphData;
	c2dGraphData.m_strChartTitle = _T("Resource Queue Length Report");
	c2dGraphData.m_strYtitle = _T("Queue Length");
	c2dGraphData.m_strXtitle = _T("Date and Time");	


	ElapsedTime eStartTime = pParameter->getStartTime();
	ElapsedTime eEndTime = pParameter->getEndTime();
	ElapsedTime eInterval(pParameter->getInterval());

	ASSERT(eEndTime >= eStartTime);
	ASSERT(eInterval > ElapsedTime(0L));

	if(eStartTime > eEndTime || eInterval <= ElapsedTime(0L))
		return;

	//get chart type
	int nChartType = pParameter->getSubType();

	CString strVehicleType = pParameter->GetVehicleTypeName(nChartType).toString();
	size_t nResCount = m_vResDetail.size();
	for (size_t nRes = 0; nRes < nResCount; ++ nRes)
	{
		DetailItem& dItem = m_vResDetail[nRes];
		c2dGraphData.m_vrLegend.push_back(dItem.m_strObjName);

		if (dItem.m_strVehicleType != strVehicleType)
			continue;
		
		std::vector<double> dRangeCount;
		size_t nTimeCount = dItem.m_vTimeItems.size();
		for (size_t nTime = 0; nTime < nTimeCount; ++ nTime)
		{
			if(nRes == 0)
				c2dGraphData.m_vrXTickTitle.push_back(GetXtitle(dItem.m_vTimeItems[nTime].m_eTime));
			
			dRangeCount.push_back(dItem.m_vTimeItems[nTime].m_nLen);
		}
		c2dGraphData.m_vr2DChartData.push_back(dRangeCount);

	}




	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("Landside Resource Queue Report %s"), pParameter->GetParameterString());
	c2dGraphData.m_strFooter = strFooter;

	chartWnd.DrawChart(c2dGraphData);



}
CString LSResourceQueueDetailResult::GetXtitle( ElapsedTime& eStartTime ) const
{
	CString strXTitle;
	strXTitle.Format(_T("%s"), eStartTime.printTime());
	return strXTitle;
}

void LSResourceQueueDetailResult::GenerateResult( CBGetLogFilePath pFunc,LandsideBaseParam* pParameter, InputLandside *pLandisde )
{
	m_vResDetail.clear();

	GenerateChartList(pParameter);
	LSResourceQueueResult::GenerateResult(pFunc, pParameter, pLandisde);
	
	//generate the detail report
	
	

}

void LSResourceQueueDetailResult::RefreshReport( LandsideBaseParam * parameter )
{

}

void LSResourceQueueDetailResult::InitListHead( CXListCtrl& cxListCtrl, LandsideBaseParam * parameter, CSortableHeaderCtrl* piSHC /*= NULL*/ )
{
	ASSERT(piSHC != NULL);
	if(piSHC == NULL)
		return;

	piSHC->ResetAll();

	//pax id

	int colIndex = 0;

	cxListCtrl.InsertColumn(colIndex, "Facility", LVCFMT_LEFT,100);
	piSHC->SetDataType(colIndex, dtSTRING );

	colIndex += 1;
	cxListCtrl.InsertColumn(colIndex, "Time", LVCFMT_LEFT,100);
	piSHC->SetDataType(colIndex, dtTIME );

	colIndex += 1;
	cxListCtrl.InsertColumn(colIndex,"Vehicle Type",LVCFMT_LEFT,100);
	piSHC->SetDataType(colIndex,dtSTRING);

	colIndex += 1;
	cxListCtrl.InsertColumn(colIndex, "Q Len", LVCFMT_LEFT,100);
	piSHC->SetDataType(colIndex, dtINT );
	
}

void LSResourceQueueDetailResult::FillListContent( CXListCtrl& cxListCtrl, LandsideBaseParam * parameter )
{
	int nRowIndex = 0;
	std:: vector<DetailItem>::iterator iter =  m_vResDetail.begin();
	for (; iter != m_vResDetail.end(); ++iter)
	{
		DetailItem& resDetailItem = *iter;
		std::vector<DetailTimeItem> &vItems = resDetailItem.m_vTimeItems;

		int nCount = static_cast<int>(vItems.size());
		for (int nItem = 0; nItem < nCount; ++ nItem)
		{
			DetailTimeItem& dItem = vItems[nItem];

			int colIndex = 0;


			cxListCtrl.InsertItem(nRowIndex, resDetailItem.m_strObjName);

			//
			colIndex += 1;
			cxListCtrl.SetItemText(nRowIndex,colIndex, dItem.m_eTime.PrintDateTime());

			colIndex += 1;
			cxListCtrl.SetItemText(nRowIndex,colIndex,resDetailItem.m_strVehicleType);

			CString strText;
			strText.Format(_T("%d"), dItem.m_nLen);
			//
			colIndex += 1;
			cxListCtrl.SetItemText(nRowIndex,colIndex, strText);

			nRowIndex += 1;
		}
	}

}

void LSResourceQueueDetailResult::GenerateChartList(LandsideBaseParam * parameter)
{
	m_chartList.Clean();
	for (int i = 0; i < parameter->GetVehicleTypeNameCount(); i++)
	{
		CString strVehicleType = parameter->GetVehicleTypeName(i).toString();
		CString strRepTitle;
		strRepTitle.Format(_T("Vehicle Queue length[%s]"),strVehicleType);
		m_chartList.AddItem(i,strRepTitle);
	}
}

LSGraphChartTypeList LSResourceQueueDetailResult::GetChartList() const
{
	return  m_chartList;
}

BOOL LSResourceQueueDetailResult::ReadReportData( ArctermFile& _file )
{
	m_vResDetail.clear();

	while(!_file.isBlank())
	{
		DetailItem curResItem;

		_file.getInteger(curResItem.m_nObjID);
		_file.getField(curResItem.m_strObjName.GetBuffer(1024),1024);
		curResItem.m_strObjName.ReleaseBuffer();
		
		DetailTimeItem dTimeItem;
		_file.getTime(dTimeItem.m_eTime);
		_file.getField(curResItem.m_strVehicleType.GetBuffer(1024),1024);
		curResItem.m_strVehicleType.ReleaseBuffer();

		_file.getInteger(dTimeItem.m_nLen);

		size_t nCount = m_vResDetail.size();
		//no data or the not same object as the previous object	
		if(nCount == 0 ||
			m_vResDetail[nCount -1].m_nObjID != curResItem.m_nObjID)
		{
			//add new
			curResItem.m_vTimeItems.push_back(dTimeItem);
			m_vResDetail.push_back(curResItem);

		}
		else
		{
			//push to last one
			m_vResDetail[nCount - 1].m_vTimeItems.push_back(dTimeItem);
		}
		_file.getLine();
	}
	return TRUE;
}

BOOL LSResourceQueueDetailResult::WriteReportData( ArctermFile& _file )
{

	int nRowIndex = 0;
	std:: vector<DetailItem>::iterator iter =  m_vResDetail.begin();
	for (; iter != m_vResDetail.end(); ++iter)
	{
		DetailItem& resDetailItem = *iter;
		std::vector<DetailTimeItem> &vItems = resDetailItem.m_vTimeItems;

		int nCount = static_cast<int>(vItems.size());
		for (int nItem = 0; nItem < nCount; ++ nItem)
		{
			DetailTimeItem& dItem = vItems[nItem];

			_file.writeInt(resDetailItem.m_nObjID);
			_file.writeField(resDetailItem.m_strObjName);
			_file.writeTime(dItem.m_eTime);
			_file.writeField(resDetailItem.m_strVehicleType);
			_file.writeInt(dItem.m_nLen);

			_file.writeLine();
		}
	}

	return TRUE;
}
