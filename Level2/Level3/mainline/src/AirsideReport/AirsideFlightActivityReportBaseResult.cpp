#include "StdAfx.h"
#include ".\airsideflightactivityreportbaseresult.h"
#include "FlightActivityReport.h"
#include "Parameters.h"
#include "FlightActivityParam.h"

CAirsideFlightActivityReportBaseResult::CAirsideFlightActivityReportBaseResult(void)
{
	m_nSubReportType = CFlightDetailActivityResult::FAR_SysEntryDist;
}

CAirsideFlightActivityReportBaseResult::~CAirsideFlightActivityReportBaseResult(void)
{
}

//////////////////////////////////////////////////////////////////////////
//CAirsideFlightActivityReportEntryResult
CAirsideFlightActivityReportEntryResult::CAirsideFlightActivityReportEntryResult()
{
	m_nSubReportType = CFlightDetailActivityResult::FAR_SysEntryDist;
}

CAirsideFlightActivityReportEntryResult::~CAirsideFlightActivityReportEntryResult()
{

}

void CAirsideFlightActivityReportEntryResult::GenerateResult(vector<CFlightDetailActivityResult::FltActItem>& vResult,CParameters *pParameter)
{
	long minXValue = pParameter->getStartTime().asMinutes();
	long maxXValue   = pParameter->getEndTime().asMinutes();

	minXValue=minXValue-(minXValue%30);
	maxXValue=maxXValue-(maxXValue%30)+30;
	int nScale;//
	int n=(maxXValue-minXValue)%210;//210=30*7;  7 Column
	if(n==0)
	{
		nScale=(maxXValue-minXValue)/210;
	}
	else
	{
		nScale=(maxXValue-minXValue)/210+1;
	}

	int xValue[8];
	int yValue[8];

	for(int i=0;i<8;i++)
	{
		xValue[i]=0;
		yValue[i]=0;
	}

	xValue[0]=minXValue;
	for(int i=1;i<=7;i++)
	{
		xValue[i]=xValue[i-1]+30*nScale;
	}

	vector<CFlightDetailActivityResult::FltActItem>::iterator iter = vResult.begin();
	while (iter != vResult.end())
	{
		for (int i=1; i<=7; i++)
		{
			if ((*iter).entryTime >= xValue[i-1]*60
			    && (*iter).entryTime < xValue[i]*60)
			{
				yValue[i]++;
			}
		}		

		iter++;
	}

	int nCount=7;
	for(int i=7;i>0;i--)
	{
		if(yValue[i]==0)
		{
			nCount--;
		}
		else
		{
			break;
		}
	}

	for (int i=1; i<=nCount; i++)
	{
		CString strTimeRange;
		ElapsedTime estStartTime(xValue[i-1]*60L);
		ElapsedTime estEndTime( xValue[i]*60L);
		strTimeRange.Format("%s-%s",estStartTime.printTime(), estEndTime.printTime());

		m_vTimeRange.push_back(strTimeRange);
		m_vNumOfFlight.push_back(yValue[i]);
	}		
}

void CAirsideFlightActivityReportEntryResult::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{
	CString strLabel = _T("");
	C2DChartData c2dGraphData;
	c2dGraphData.m_strChartTitle = _T(" System Entry Distribution ");
	c2dGraphData.m_strYtitle = _T("Flight Count");
	c2dGraphData.m_strXtitle = _T("Entry Time");	

	c2dGraphData.m_vrXTickTitle = m_vTimeRange;

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("ACTIVITY REPORT %s;%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
	bool bCommaFlag = true;
	CFlightActivityParam* pParam = (CFlightActivityParam*)pParameter;
	for (int i=0; i<(int)pParam->getFlightConstraintCount(); i++)
	{
		FlightConstraint fltCons = pParam->getFlightConstraint(i);

		CString strFlight(_T(""));
		fltCons.screenPrint(strFlight.GetBuffer(1024));

		CString strNewFlight(_T(""));
		//flight isn't default
		if (strFlight.CompareNoCase(_T("default")))
		{
			strNewFlight.Format(_T("(%s)"), strFlight);
			strFlight = strNewFlight;
		}

		if (bCommaFlag)
		{
			bCommaFlag = false;

			strFooter.AppendFormat("%s", strFlight);
		}
		else
		{
			strFooter.AppendFormat(";%s", strFlight);
		}
	}
	c2dGraphData.m_strFooter = strFooter;

	int nSegmentCount = (int)m_vNumOfFlight.size();

	vector<double> vSegmentData;

	for (int i=0; i<nSegmentCount; i++)
	{
		double dData = m_vNumOfFlight[i];
		vSegmentData.push_back(dData);
	}

	c2dGraphData.m_vr2DChartData.push_back(vSegmentData);

	chartWnd.DrawChart(c2dGraphData);
}

//////////////////////////////////////////////////////////////////////////
//
CAirsideFlightActivityReportExitResult::CAirsideFlightActivityReportExitResult()
{
	m_nSubReportType = CFlightDetailActivityResult::FAR_SysExitDist;
}

CAirsideFlightActivityReportExitResult::~CAirsideFlightActivityReportExitResult()
{

}

void CAirsideFlightActivityReportExitResult::GenerateResult(vector<CFlightDetailActivityResult::FltActItem>& vResult,CParameters *pParameter)
{
	long minXValue = pParameter->getStartTime().asMinutes();
	long maxXValue   = pParameter->getEndTime().asMinutes();

	minXValue=minXValue-(minXValue%30);
	maxXValue=maxXValue-(maxXValue%30)+30;
	int nScale;//
	int n=(maxXValue-minXValue)%210;//210=30*7;  7 Column
	if(n==0)
	{
		nScale=(maxXValue-minXValue)/210;
	}
	else
	{
		nScale=(maxXValue-minXValue)/210+1;
	}

	int xValue[8];
	int yValue[8];

	for(int i=0;i<8;i++)
	{
		xValue[i]=0;
		yValue[i]=0;
	}

	xValue[0]=minXValue;
	for(int i=1;i<=7;i++)
	{
		xValue[i]=xValue[i-1]+30*nScale;
	}

	vector<CFlightDetailActivityResult::FltActItem>::iterator iter = vResult.begin();
	while (iter != vResult.end())
	{
		for (int i=1; i<=7; i++)
		{
			if ((*iter).exitTime >= xValue[i-1]*60
			&& (*iter).exitTime < xValue[i]*60)
			{
				yValue[i]++;
			}
		}		

		iter++;
	}

	int nCount=7;
	for(int i=7;i>0;i--)
	{
		if(yValue[i]==0)
		{
			nCount--;
		}
		else
		{
			break;
		}
	}

	for (int i=1; i<=nCount; i++)
	{
		CString strTimeRange;
		ElapsedTime estStartTime(xValue[i-1]*60L);
		ElapsedTime estEndTime( xValue[i]*60L);
		strTimeRange.Format("%s-%s",estStartTime.printTime(), estEndTime.printTime());

		m_vTimeRange.push_back(strTimeRange);
		m_vNumOfFlight.push_back(yValue[i]);
	}		
}

void CAirsideFlightActivityReportExitResult::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{
	CString strLabel = _T("");
	C2DChartData c2dGraphData;
	c2dGraphData.m_strChartTitle = _T(" System Exit Distribution ");
	c2dGraphData.m_strYtitle = _T("Flight Count");
	c2dGraphData.m_strXtitle = _T("Exit Time");	

	c2dGraphData.m_vrXTickTitle = m_vTimeRange;

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("ACTIVITY REPORT %s;%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
	bool bCommaFlag = true;
	CFlightActivityParam* pParam = (CFlightActivityParam*)pParameter;
	for (int i=0; i<(int)pParam->getFlightConstraintCount(); i++)
	{
		FlightConstraint fltCons = pParam->getFlightConstraint(i);

		CString strFlight(_T(""));
		fltCons.screenPrint(strFlight.GetBuffer(1024));

		CString strNewFlight(_T(""));
		//flight isn't default
		if (strFlight.CompareNoCase(_T("default")))
		{
			strNewFlight.Format(_T("(%s)"), strFlight);
			strFlight = strNewFlight;
		}

		if (bCommaFlag)
		{
			bCommaFlag = false;

			strFooter.AppendFormat("%s", strFlight);
		}
		else
		{
			strFooter.AppendFormat(";%s", strFlight);
		}
	}
	c2dGraphData.m_strFooter = strFooter;

	int nSegmentCount = (int)m_vNumOfFlight.size();

	vector<double> vSegmentData;

	for (int i=0; i<nSegmentCount; i++)
	{
		double dData = m_vNumOfFlight[i];
		vSegmentData.push_back(dData);
	}

	c2dGraphData.m_vr2DChartData.push_back(vSegmentData);

	chartWnd.DrawChart(c2dGraphData);
}