#include "stdafx.h"
#include "AirsideFlightOperationalMultiRunResult.h"
#include "FlightOperationalReport.h"
#include "FlightOperationalParam.h"
#include "CARC3DChart.h"
#include "../common/FLT_CNST.H"
#include "../Common/ARCUnit.h"

CAirsideFlightOperationalMultiRunResult::CAirsideFlightOperationalMultiRunResult(void)
{
}


CAirsideFlightOperationalMultiRunResult::~CAirsideFlightOperationalMultiRunResult(void)
{
	ClearData();
}

void CAirsideFlightOperationalMultiRunResult::LoadMultipleRunReport( CParameters* pParameter )
{
	ClearData();
	mapSimReport::iterator iter = m_mapSimReport.begin();
	for (; iter != m_mapSimReport.end(); ++iter)
	{
		CFlightOperationalReport* pFlightOpReport = (CFlightOperationalReport*)iter->second;
		CString strSimResult = iter->first;

	    std::vector<FltOperationalItem>& opItem = pFlightOpReport->GetResult();
		std::vector<FltOperationalItem>::iterator opIter = opItem.begin();
		for (; opIter != opItem.end(); ++opIter)
		{
			CString strFlightType;
			opIter->m_fltCons.screenPrint(strFlightType.GetBuffer(1024));
			strFlightType.ReleaseBuffer();
			double dAirDistance = ARCUnit::ConvertLength(opIter->m_fAirDis, ARCUnit::CM, ARCUnit::KM);
			m_mapDetailAirDistance[strSimResult][strFlightType] += dAirDistance;
			double dGroundDistance =  ARCUnit::ConvertLength(opIter->m_fGroundDis, ARCUnit::CM, ARCUnit::M);
			m_mapDetailGroundDistance[strSimResult][strFlightType] += dGroundDistance;
		}
	}
}

void CAirsideFlightOperationalMultiRunResult::InitListHead( CXListCtrl& cxListCtrl,CParameters * parameter, int iType /*= 0*/,CSortableHeaderCtrl* piSHC/*=NULL*/ )
{
	cxListCtrl.DeleteAllItems();

	while(cxListCtrl.GetHeaderCtrl()->GetItemCount() >0)
		cxListCtrl.DeleteColumn(0);

	if (parameter->getReportType() == ASReportType_Detail)
	{
		DWORD headStyle = LVCFMT_CENTER;
		headStyle &= ~HDF_OWNERDRAW;
		cxListCtrl.InsertColumn(0,"",headStyle,20);

		headStyle = LVCFMT_LEFT;
		headStyle &= ~HDF_OWNERDRAW;
		cxListCtrl.InsertColumn(1, _T("SimResult"), headStyle, 80);

		CFlightOperationalParam* pFlightOpPara = (CFlightOperationalParam*)parameter;
		switch (pFlightOpPara->getSubType())
		{
		case CFlightOperationalReport::OPERATIONAL_FLIGHTTYPEVSAIRDISTANCE:
			InitDetailListHead(cxListCtrl,m_mapDetailAirDistance,0,piSHC);
			break;
		case CFlightOperationalReport::OPERATIONAL_FLIGHTTYPEVSGROUNDDISTANCE:
			InitDetailListHead(cxListCtrl,m_mapDetailGroundDistance,1,piSHC);	
			break;
		default:
			break;
		}
	}
}

void CAirsideFlightOperationalMultiRunResult::FillListContent( CXListCtrl& cxListCtrl, CParameters * parameter, int iType /*= 0*/ )
{
	ASSERT(parameter);
	if (parameter->getReportType() == ASReportType_Detail)
	{
		CFlightOperationalParam* pFlightOpPara = (CFlightOperationalParam*)parameter;
		switch (pFlightOpPara->getSubType())
		{
		case CFlightOperationalReport::OPERATIONAL_FLIGHTTYPEVSAIRDISTANCE:
			FillDetailListContent(cxListCtrl,m_mapDetailAirDistance);
			break;
		case CFlightOperationalReport::OPERATIONAL_FLIGHTTYPEVSGROUNDDISTANCE:
			FillDetailListContent(cxListCtrl,m_mapDetailGroundDistance);
			break;
		default:
			break;
		}
	}
}

void CAirsideFlightOperationalMultiRunResult::Draw3DChart( CARC3DChart& chartWnd, CParameters *pParameter, int iType /*= 0*/ )
{
	if (pParameter->getReportType() == ASReportType_Detail)
	{
		CFlightOperationalParam* pFlightOpPara = (CFlightOperationalParam*)pParameter;
		switch (pFlightOpPara->getSubType())
		{
		case CFlightOperationalReport::OPERATIONAL_FLIGHTTYPEVSAIRDISTANCE:
			Generate3DChartContentData(m_mapDetailAirDistance,chartWnd,pParameter);
			break;
		case CFlightOperationalReport::OPERATIONAL_FLIGHTTYPEVSGROUNDDISTANCE:
			Generate3DChartContentData(m_mapDetailGroundDistance,chartWnd,pParameter);
			break;
		default:
			break;
		}
	}
}

void CAirsideFlightOperationalMultiRunResult::ClearData()
{
	m_mapDetailAirDistance.clear();
	m_mapDetailGroundDistance.clear();
}

void CAirsideFlightOperationalMultiRunResult::InitDetailListHead( CXListCtrl& cxListCtrl,mapDetailDistance mapDetailData,int iType/* = 0*/,CSortableHeaderCtrl* piSHC/*=NULL*/ )
{
	if (mapDetailData.empty())
		return;

	mapDetailDistance::iterator iter = mapDetailData.begin();

	if (iter->second.empty())
		return;

	mapTypeDisance::iterator typeIter = iter->second.begin();
	int idx = 0;
	for (; typeIter != iter->second.end(); ++typeIter)
	{
		DWORD dwStyle = LVCFMT_LEFT;
		dwStyle &= ~HDF_OWNERDRAW;

		CString strHeader;
		if (iType == 0)
		{
			strHeader.Format(_T("%s(km)"),typeIter->first);
		}
		else
		{
			strHeader.Format(_T("%s(m)"),typeIter->first);
		}
		cxListCtrl.InsertColumn(2+idx, strHeader, /*LVCFMT_LEFT*/dwStyle, 100);
		idx++;
	}
}

void CAirsideFlightOperationalMultiRunResult::FillDetailListContent( CXListCtrl& cxListCtrl,mapDetailDistance mapDetailData )
{
	mapDetailDistance::iterator iter = mapDetailData.begin();
	int idx = 0;
	for (; iter != mapDetailData.end(); ++iter)
	{
		CString strIndex;
		strIndex.Format(_T("%d"),idx+1);
		cxListCtrl.InsertItem(idx,strIndex);

		CString strSimName = iter->first;
		int nCurSimResult = atoi(strSimName.Mid(9,strSimName.GetLength()));
		CString strRun = _T("");
		strRun.Format(_T("Run%d"),nCurSimResult+1);

		//	wndListCtrl.InsertItem(i, strRun);
		cxListCtrl.SetItemText(idx,1,strRun);
		cxListCtrl.SetItemData(idx,idx);

		mapTypeDisance::iterator typeIter = iter->second.begin();
		int nCol = 0;
		for (; typeIter != iter->second.end(); ++typeIter)
		{
			CString strDistance;
			strDistance.Format(_T("%.2f"),typeIter->second);
			cxListCtrl.SetItemText(idx, nCol + 2, strDistance);
			nCol++;
		}
		idx++;
	}
}

void CAirsideFlightOperationalMultiRunResult::Generate3DChartContentData( mapDetailDistance mapDetailData,CARC3DChart& chartWnd, CParameters *pParameter )
{
	C2DChartData c2dGraphData;

	SetDetail3DChartString(c2dGraphData,pParameter);

	if (mapDetailData.empty() == true)
		return;

	mapDetailDistance::iterator iter = mapDetailData.begin();
	mapTypeDisance::iterator typeIter = iter->second.begin();
	for (; typeIter != iter->second.end(); ++typeIter)
	{
		c2dGraphData.m_vrXTickTitle.push_back(typeIter->first);
	}

	for (iter = mapDetailData.begin(); iter != mapDetailData.end(); ++iter)
	{
		CString strSimName = iter->first;
		int nCurSimResult = atoi(strSimName.Mid(9,strSimName.GetLength()));
		CString strLegend;
		strLegend.Format(_T("Run%d"),nCurSimResult+1);
		c2dGraphData.m_vrLegend.push_back(strLegend);

		std::vector<double>  vData;
		typeIter = iter->second.begin();
		for (; typeIter!= iter->second.end(); ++typeIter)
		{
			vData.push_back(typeIter->second);
		}
		c2dGraphData.m_vr2DChartData.push_back(vData);
	}
	chartWnd.DrawChart(c2dGraphData);
}

void CAirsideFlightOperationalMultiRunResult::SetDetail3DChartString( C2DChartData& c2dGraphData,CParameters *pParameter,int iType /*= 0*/ )
{
	ASSERT(pParameter);
	if (pParameter->getReportType() == ASReportType_Detail)
	{
		CFlightOperationalParam* pFlightOpPara = (CFlightOperationalParam*)pParameter;
		switch (pFlightOpPara->getSubType())
		{
		case CFlightOperationalReport::OPERATIONAL_FLIGHTTYPEVSAIRDISTANCE:
			SetDetailAirDistance3DChartString(c2dGraphData,pParameter);
			break;
		case CFlightOperationalReport::OPERATIONAL_FLIGHTTYPEVSGROUNDDISTANCE:
			SetDetailGroundDistance3DChartString(c2dGraphData,pParameter);
			break;
		default:
			break;
		}
	}
}

void CAirsideFlightOperationalMultiRunResult::SetDetailAirDistance3DChartString( C2DChartData& c2dGraphData, CParameters *pParameter )
{
	c2dGraphData.m_strChartTitle = _T(" Flight Type vs. Air Distance ");
	c2dGraphData.m_strYtitle = _T("Air Distance(km)");
	c2dGraphData.m_strXtitle.Format(_T("Flight Type"));	

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("Mulitple Run Report FLIGHT TYPE VS AIR DISTANCE %s,%s "),GetMultipleRunReportString(),pParameter->GetParameterString());
	c2dGraphData.m_strFooter = strFooter;
}

void CAirsideFlightOperationalMultiRunResult::SetDetailGroundDistance3DChartString( C2DChartData& c2dGraphData, CParameters *pParameter )
{
	c2dGraphData.m_strChartTitle = _T(" Flight Type vs. Ground Distance ");
	c2dGraphData.m_strYtitle = _T("Ground Distance(m)");
	c2dGraphData.m_strXtitle.Format(_T("Flight Type"));	

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("Mulitple Run Report FLIGHT TYPE VS GROUND DISTANCE %s,%s "),GetMultipleRunReportString(),pParameter->GetParameterString());
	c2dGraphData.m_strFooter = strFooter;
}

BOOL CAirsideFlightOperationalMultiRunResult::WriteReportData( ArctermFile& _file )
{
	_file.writeField("Airside Multiple Run Flight Operation Report");//write report string
	_file.writeLine();

	_file.writeField("Detail Result");//write detail type
	_file.writeLine();
	WriteDetailMap(m_mapDetailAirDistance,_file);
	WriteDetailMap(m_mapDetailGroundDistance,_file);;
	return TRUE;
}

BOOL CAirsideFlightOperationalMultiRunResult::ReadReportData( ArctermFile& _file )
{
	ClearData();
	_file.getLine();
	//read detail
	ReadDetailMap(m_mapDetailAirDistance,_file);
	ReadDetailMap(m_mapDetailGroundDistance,_file);
	return TRUE;
}

CString CAirsideFlightOperationalMultiRunResult::GetReportFileName() const
{
	return CString("FlightOperations\\FlightOperations.prm");
}

BOOL CAirsideFlightOperationalMultiRunResult::WriteDetailMap( mapDetailDistance mapDetailData, ArctermFile& _file )
{
	long iSize = (long)mapDetailData.size();
	_file.writeInt(iSize);

	mapDetailDistance::iterator iter = mapDetailData.begin();
	for (; iter != mapDetailData.end(); ++iter)
	{
		CString strSimResult = iter->first;
		_file.writeField(strSimResult.GetBuffer(1024));
		strSimResult.ReleaseBuffer();

		int iCount = (int)iter->second.size();
		_file.writeInt(iCount);

		_file.writeLine();
		mapTypeDisance::iterator mapIter = iter->second.begin();
		for (; mapIter != iter->second.end(); ++mapIter)
		{
			CString strFlightType = mapIter->first;
			_file.writeField(strFlightType.GetBuffer(1024));
			strFlightType.ReleaseBuffer();

			_file.writeDouble(mapIter->second);
			_file.writeLine();
		}
		_file.writeLine();
	}

	return TRUE;
}

BOOL CAirsideFlightOperationalMultiRunResult::ReadDetailMap( mapDetailDistance& mapDetailData,ArctermFile& _file )
{
	int iSize = 0; 
	_file.getInteger(iSize);

	for (int i = 0; i < iSize; i++)
	{
		CString strSimResult;
		_file.getField(strSimResult.GetBuffer(1024),1024);
		strSimResult.ReleaseBuffer();

		int iCount = 0;
		_file.getInteger(iCount);
		_file.getLine();

		for (int j = 0; j < iCount; j++)
		{
			CString strFlightType;
			_file.getField(strFlightType.GetBuffer(1024),1024);
			strFlightType.ReleaseBuffer();

			double dValue = 0.0;
			_file.getFloat(dValue);
			mapDetailData[strSimResult][strFlightType] = dValue;
			_file.getLine();
		}
		_file.getLine();
	}

	return TRUE;
}
