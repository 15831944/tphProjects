#include "StdAfx.h"
#include "AirsideDistanceTravelReport.h"
#include "AirsideDistanceTravelParam.h"
#include "../Common/AirlineManager.h"
#include <math.h>
//#include "comutil.h"
#include "../Common/fileman.h"
#include "../Common/point.h"
#include <limits>
#include "AirsideFlightDistanceTravelReportDetailResult.h"
#include "AirsideFlightDistanceTravelReportSummaryResult.h"
#include "../MFCExControl/XListCtrl.h"
#include "../MFCExControl/SortableHeaderCtrl.h"
#include "../Common/UnitsManager.h"


using namespace ADODB;


CAirsideDistanceTravelReport::CAirsideDistanceTravelReport(CBGetLogFilePath pFunc)
:CAirsideBaseReport(pFunc)
,m_pResult(NULL)
,m_pParameter(NULL)
{
	//m_minimum = -1;
	//m_average = -1;
	//m_maximum = -1;
	//m_count	   = 0;

}

CAirsideDistanceTravelReport::~CAirsideDistanceTravelReport()
{
	
}

void CAirsideDistanceTravelReport::ClearDistanceTravelReportItems()
{
	m_vResult.clear();
}

void CAirsideDistanceTravelReport::GenerateReport(CParameters * parameter)
{	
	m_pParameter = parameter;

	m_vResult.clear();

	CAirsideDistanceTravelParam *pParam = (CAirsideDistanceTravelParam *)parameter;
	ASSERT(pParam != NULL);
	

	long lStartTime = (long)pParam->getStartTime();
	long lEndTime = (long)pParam->getEndTime();


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


	int nFltLogCount = fltLog.getCount();
	for (int i =0; i < nFltLogCount; ++i)
	{	
		double dTotalDistance = 0.0f;

		AirsideFlightLogEntry logEntry;
		logEntry.SetEventLog(&airsideFlightEventLog);
		fltLog.getItem(logEntry,i);
		AirsideFlightDescStruct	fltDesc = logEntry.GetAirsideDescStruct();
		if (fltDesc.startTime == 0 && fltDesc.endTime == 0)
			continue;
		if (fltDesc.startTime > lEndTime)
			continue;
		if (fltDesc.endTime < lStartTime)
			continue;

		FlightConstraint fltConstraint;
		if (IsFitConstraint(fltDesc,parameter,fltConstraint) == false)
			continue;



		int nEventCount = logEntry.getCount();
		Point pointstart,pointEnd;

		if (nEventCount >1)
		{
			AirsideFlightEventStruct event = logEntry.getEvent(0);
			pointstart.setPoint(event.x,event.y,static_cast<int>(event.z));
		}

		for (int nEvent = 1; nEvent < nEventCount; ++nEvent)
		{
			AirsideFlightEventStruct event = logEntry.getEvent(nEvent);
			
			pointEnd.setPoint(event.x,event.y,static_cast<int>(event.z));

			dTotalDistance += pointstart.getDistanceTo(pointEnd);
			
			pointstart = pointEnd;
		}

		DistanceTravelReportItem item(m_AirportDB);
		item.fltCons = fltConstraint;
		item.fltDesc = logEntry.GetAirsideDescStruct();
		item.distance = dTotalDistance;

		m_vResult.push_back(item);

	}

	return;
}

void CAirsideDistanceTravelReport::CalcDistance()
{
	

}

double CAirsideDistanceTravelReport::distancePoint(double x1,double y1,double z1 , double x2 ,double y2 ,double z2)
{
	double result;
	double xSquared = (x1 - x2)*(x1 - x2);
	double ySquared = (y1 - y2)*(y1 - y2);
	long double sumOfSquares = (long double)(xSquared + ySquared);
	result = sqrtl (sumOfSquares);
	return result;
}

long CAirsideDistanceTravelReport::GetCount(double dstart ,double dend)
{
	long nCount = 0L;
	for (size_t i =0; i < m_vResult.size(); ++i)
	{
		if (m_vResult[i].distance >= dstart && m_vResult[i].distance < dend)
		{
			nCount += 1;
		}

	}
	return nCount;
}

double CAirsideDistanceTravelReport::GetMaxDisantce()
{
	double dMax = 0L;
	for (size_t i =0; i < m_vResult.size(); ++i)
	{
		if (m_vResult[i].distance > dMax)
		{
			dMax = m_vResult[i].distance;
		}
	}
	return dMax;
}

double CAirsideDistanceTravelReport::GetMinDisantce()
{
	double dMin = (std::numeric_limits<double>::max)();
	for (size_t i = 0; i < m_vResult.size(); ++i)
	{
		if (m_vResult[i].distance < dMin)
		{
			dMin = m_vResult[i].distance;
		}
	}
	return dMin;
}

void CAirsideDistanceTravelReport::InitListHead(CXListCtrl& cxListCtrl, enumASReportType_Detail_Summary reportType, CSortableHeaderCtrl* piSHC)
{
	cxListCtrl.DeleteAllItems();

	while(cxListCtrl.GetHeaderCtrl()->GetItemCount() >0)
		cxListCtrl.DeleteColumn(0);

	if (reportType == ASReportType_Detail)
	{
		cxListCtrl.InsertColumn(0, _T("Airline"), LVCFMT_LEFT, 100, -1);
		
		cxListCtrl.InsertColumn(1, _T("Arrival ID"), LVCFMT_LEFT, 100, -1);
		cxListCtrl.InsertColumn(2, _T("Departure ID"), LVCFMT_LEFT, 100, -1);

		CString strDist;
		strDist.Format("Distance(%s)", m_pParameter->GetUnitManager()->GetLengthUnitString());
		cxListCtrl.InsertColumn(3, strDist, LVCFMT_LEFT, 100, -1);
		cxListCtrl.InsertColumn(4, "Flight Constraint", LVCFMT_LEFT, 100, -1);
		if (piSHC)
		{
			piSHC->ResetAll();
			piSHC->SetDataType(0,dtSTRING);
			piSHC->SetDataType(1,dtSTRING);
			piSHC->SetDataType(2,dtSTRING);
			piSHC->SetDataType(3,dtDEC);
			piSHC->SetDataType(4,dtSTRING);
		}
	}
	else
	{

		cxListCtrl.InsertColumn(0, _T("Flight Type"), LVCFMT_LEFT, 100);

		//Node delay
		CString strUnit = "(" + m_pParameter->GetUnitManager()->GetLengthUnitString() + ")";
		cxListCtrl.InsertColumn(1, _T("Min Dist") + strUnit, LVCFMT_LEFT, 100);
		cxListCtrl.InsertColumn(2, _T("Max Dist") + strUnit, LVCFMT_LEFT, 100);
		cxListCtrl.InsertColumn(3, _T("Average Dist") +strUnit, LVCFMT_LEFT, 100);

		cxListCtrl.InsertColumn(4, _T("Q1") + strUnit, LVCFMT_LEFT, 100);
		cxListCtrl.InsertColumn(5, _T("Q2") + strUnit, LVCFMT_LEFT, 100);
		cxListCtrl.InsertColumn(6, _T("Q3") + strUnit, LVCFMT_LEFT, 100);
		cxListCtrl.InsertColumn(7, _T("P1") + strUnit, LVCFMT_LEFT, 100);
		cxListCtrl.InsertColumn(8, _T("P5") + strUnit, LVCFMT_LEFT, 100);
		cxListCtrl.InsertColumn(9, _T("P10") + strUnit, LVCFMT_LEFT, 100);
		cxListCtrl.InsertColumn(10, _T("P90") + strUnit, LVCFMT_LEFT, 100);
		cxListCtrl.InsertColumn(11, _T("P95") + strUnit, LVCFMT_LEFT, 100);
		cxListCtrl.InsertColumn(12, _T("P99") + strUnit, LVCFMT_LEFT, 100);
		cxListCtrl.InsertColumn(13, _T("sigma") + strUnit, LVCFMT_LEFT, 100);
		if (piSHC)
		{
			piSHC->ResetAll();
			piSHC->SetDataType(0,dtSTRING);
			piSHC->SetDataType(1,dtDEC);
			piSHC->SetDataType(2,dtDEC);
			piSHC->SetDataType(3,dtDEC);
			piSHC->SetDataType(4,dtDEC);
			piSHC->SetDataType(5,dtDEC);
			piSHC->SetDataType(6,dtDEC);
			piSHC->SetDataType(7,dtDEC);
			piSHC->SetDataType(8,dtDEC);
			piSHC->SetDataType(9,dtDEC);
			piSHC->SetDataType(10,dtDEC);
			piSHC->SetDataType(11,dtDEC);
			piSHC->SetDataType(12,dtDEC);
			piSHC->SetDataType(13,dtDEC);
		}
	}
}


void CAirsideDistanceTravelReport::FillListContent(CXListCtrl& cxListCtrl, CParameters * parameter)
{	
	enumASReportType_Detail_Summary reportType = parameter->getReportType();

	cxListCtrl.DeleteAllItems();
	if (reportType == ASReportType_Detail)
	{

		for (size_t i =0; i < m_vResult.size(); ++i )
		{
			
			cxListCtrl.InsertItem(i, m_vResult[i].fltDesc.flightID);
			cxListCtrl.SetItemText(i, 1,m_vResult[i].fltDesc.arrID);
			cxListCtrl.SetItemText(i, 2,m_vResult[i].fltDesc.depID);
			cxListCtrl.SetItemText(i, 3, FormatDoubleValueToString(m_vResult[i].distance));
			CString strFltCons;
			m_vResult[i].fltCons.screenPrint(strFltCons);
			cxListCtrl.SetItemText(i, 4, strFltCons);
		}
	}
	else // summary 
	{
		if (m_pResult == NULL)
			return;

		CAirsideFlightDistanceTravelReportSummaryResult *pResult = reinterpret_cast<CAirsideFlightDistanceTravelReportSummaryResult *>(m_pResult);
		if(pResult != NULL)
		{
			std::vector<CAirsideFlightDistanceTravelReportSummaryResult::CFltTypeSummaryData>& vData = pResult->GetResult();
			for (size_t i =0; i< vData.size();++i)
			{

				CString strFlightType ;
				vData[i].fltCons.screenPrint(strFlightType);
				cxListCtrl.InsertItem(i, strFlightType);

				cxListCtrl.SetItemText(i, 1,FormatDoubleValueToString(vData[i].summaryData.m_dMin));

				cxListCtrl.SetItemText(i, 2,FormatDoubleValueToString(vData[i].summaryData.m_dMax));
				cxListCtrl.SetItemText(i, 3,FormatDoubleValueToString(vData[i].summaryData.m_dAverage));
				cxListCtrl.SetItemText(i, 4,FormatDoubleValueToString(vData[i].summaryData.m_dQ1));
				cxListCtrl.SetItemText(i, 5,FormatDoubleValueToString(vData[i].summaryData.m_dQ2));
				cxListCtrl.SetItemText(i, 6,FormatDoubleValueToString(vData[i].summaryData.m_dQ3));
				cxListCtrl.SetItemText(i, 7,FormatDoubleValueToString(vData[i].summaryData.m_dP1));
				cxListCtrl.SetItemText(i, 8,FormatDoubleValueToString(vData[i].summaryData.m_dP5));
				cxListCtrl.SetItemText(i, 9,FormatDoubleValueToString(vData[i].summaryData.m_dP10));
				cxListCtrl.SetItemText(i, 10,FormatDoubleValueToString(vData[i].summaryData.m_dP90));
				cxListCtrl.SetItemText(i, 11,FormatDoubleValueToString(vData[i].summaryData.m_dP95));
				cxListCtrl.SetItemText(i, 12,FormatDoubleValueToString(vData[i].summaryData.m_dP99));
				cxListCtrl.SetItemText(i, 13,FormatDoubleValueToString(vData[i].summaryData.m_dSigma));
			}
		}
	}

}
BOOL CAirsideDistanceTravelReport::ExportListData(ArctermFile& _file,CParameters * parameter) 
{
	if(parameter == NULL)
		return FALSE ;
	enumASReportType_Detail_Summary reportType = parameter->getReportType();

	if (reportType == ASReportType_Detail)
	{

		for (size_t i =0; i < m_vResult.size(); ++i )
		{
             _file.writeField(m_vResult[i].fltDesc.flightID) ;
            _file.writeField(m_vResult[i].fltDesc.arrID) ;
			 _file.writeField(m_vResult[i].fltDesc.depID);
			 _file.writeField(FormatDoubleValueToString(m_vResult[i].distance)) ;

			CString strFltCons;
			m_vResult[i].fltCons.screenPrint(strFltCons);
			_file.writeField(strFltCons) ;
			_file.writeLine() ;
		}
	}
	else // summary 
	{
		if (m_pResult == NULL)
			return FALSE;

		CAirsideFlightDistanceTravelReportSummaryResult *pResult = reinterpret_cast<CAirsideFlightDistanceTravelReportSummaryResult *>(m_pResult);
		if(pResult != NULL)
		{
			std::vector<CAirsideFlightDistanceTravelReportSummaryResult::CFltTypeSummaryData>& vData = pResult->GetResult();
			for (size_t i =0; i< vData.size();++i)
			{

				CString strFlightType ;
				vData[i].fltCons.screenPrint(strFlightType);
				 _file.writeField(strFlightType) ;
				  _file.writeField(FormatDoubleValueToString(vData[i].summaryData.m_dMin)) ;
				   _file.writeField(FormatDoubleValueToString(vData[i].summaryData.m_dMax)) ;



				_file.writeField(FormatDoubleValueToString(vData[i].summaryData.m_dAverage));
				_file.writeField(FormatDoubleValueToString(vData[i].summaryData.m_dQ1));
				_file.writeField(FormatDoubleValueToString(vData[i].summaryData.m_dQ2));
				_file.writeField(FormatDoubleValueToString(vData[i].summaryData.m_dQ3));
				_file.writeField(FormatDoubleValueToString(vData[i].summaryData.m_dP1));
				_file.writeField(FormatDoubleValueToString(vData[i].summaryData.m_dP5));
				_file.writeField(FormatDoubleValueToString(vData[i].summaryData.m_dP10));
				_file.writeField(FormatDoubleValueToString(vData[i].summaryData.m_dP90));
				_file.writeField(FormatDoubleValueToString(vData[i].summaryData.m_dP95));
				_file.writeField(FormatDoubleValueToString(vData[i].summaryData.m_dP99));
				_file.writeField(FormatDoubleValueToString(vData[i].summaryData.m_dSigma));
				_file.writeLine() ;
			}
		}
	}
	return true ;
}
void CAirsideDistanceTravelReport::RefreshReport(CParameters * parameter)
{
	if (m_pResult != NULL)
		delete m_pResult ;

	m_pResult = NULL;


	if (parameter->getReportType() == ASReportType_Detail)
	{
		m_pResult = new CAirsideFlightDistanceTravelReportDetailResult();

	}
	else
	{
		m_pResult = new CAirsideFlightDistanceTravelReportSummaryResult();
	}

	if (m_pResult)
		m_pResult->GenerateResult(m_vResult,parameter);

}
bool CAirsideDistanceTravelReport::IsFitConstraint(AirsideFlightDescStruct& fltDesc,CParameters * parameter, FlightConstraint& fltConstraint)
{
	size_t nFltConsCount = parameter->getFlightConstraintCount();
	for (size_t nfltCons =0; nfltCons < nFltConsCount; ++ nfltCons)
	{
		FlightConstraint fltCst = parameter->getFlightConstraint(nfltCons);

		if (fltCst.fits(fltDesc))
		{
			fltConstraint = fltCst;
			return true;
		}
	}

	return false;
}
CString CAirsideDistanceTravelReport::FormatDoubleValueToString(double dValue)
{
	CString strValue;
	strValue.Format(_T("%.2f"),m_pParameter->GetUnitManager()->ConvertLength(UM_LEN_CENTIMETERS, m_pParameter->GetUnitManager()->GetLengthUnits(),dValue));
	return strValue;
}
//void CAirsideDistanceTravelReport::Draw3DChartFromFile(CARC3DChart& chartWnd, ArctermFile& _file,int isdetail) 
//{
//	if (m_pResult != NULL)
//		delete m_pResult ;
//
//	m_pResult = NULL;
//	if (isdetail == ASReportType_Detail)
//	{
//		m_pResult = new CAirsideFlightDistanceTravelReportDetailResult();
//
//	}
//	else
//	{
//		m_pResult = new CAirsideFlightDistanceTravelReportSummaryResult();
//	}
//	m_pResult->Draw3DChartFromFile(chartWnd,_file) ;
//	delete m_pResult ;
//	m_pResult = NULL ;
//}
//void CAirsideDistanceTravelReport::ExportFileFrom3DChart(CARC3DChart& chartWnd,  ArctermFile& _file, CParameters *pParameter) 
//{
//	if (m_pResult == NULL)
//		return ;
//	m_pResult->ExportFileFrom3DChart(_file,pParameter) ;
//}
BOOL  CAirsideDistanceTravelReport::ExportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType )
{
     _file.writeInt((int)m_vResult.size()) ;
	 _file.writeLine() ;
     for (int i = 0 ; i <(int)m_vResult.size() ;i++)
     {
		 m_vResult[i].ExportT0File(_file) ;
     }
	 return TRUE ;
}
BOOL CAirsideDistanceTravelReport::ImportReportData(ArctermFile& _file,CString& Errmsg,enumASReportType_Detail_Summary reportType)
{
	m_vResult.clear() ;
	int size = 0 ;
	_file.getInteger(size) ;
   for(int i = 0 ; i < size ;i++)
   {
	   _file.getLine();
     DistanceTravelReportItem  reportItem(m_AirportDB) ;
	 reportItem.ImportFromFile(_file) ;
     m_vResult.push_back(reportItem) ;
   }
   return TRUE ;
}
BOOL CAirsideDistanceTravelReport::DistanceTravelReportItem::ExportT0File(ArctermFile& _file)
{
	_file.writeDouble(distance) ;
	TCHAR fightTy[1024] = {0} ;
    fltCons.WriteSyntaxStringWithVersion(fightTy) ;
	_file.writeField(fightTy) ;
	_file.writeLine() ;
	fltDesc.ExportFile(_file) ;
	return TRUE ;
}
BOOL CAirsideDistanceTravelReport::DistanceTravelReportItem::ImportFromFile(ArctermFile& _file)
{
	_file.getFloat(distance) ;
	TCHAR th[1024] = {0} ;
	_file.getField(th,1024) ;
	fltCons.setConstraintWithVersion(CString(th)) ;
	_file.getLine() ;
    fltDesc.ImportFile(_file) ;
	return TRUE ;
}