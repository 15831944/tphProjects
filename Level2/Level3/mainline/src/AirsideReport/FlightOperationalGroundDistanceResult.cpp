#include "StdAfx.h"
#include ".\flightoperationalgrounddistanceresult.h"
#include "Parameters.h"
#include "../common/FLT_CNST.H"
#include "../Common/ARCUnit.h"
#include "FlightOperationalParam.h"

CFlightOperationalGroundDistanceResult::CFlightOperationalGroundDistanceResult(void)
{
}

CFlightOperationalGroundDistanceResult::~CFlightOperationalGroundDistanceResult(void)
{
	ClearAllData();
}

void CFlightOperationalGroundDistanceResult::GenerateResult(vector<FltOperationalItem>& vResult, CParameters *pParameter)
{
	ASSERT(pParameter != NULL);

	ClearAllData();

	ElapsedTime estStartTime = pParameter->getStartTime();
	ElapsedTime estEndTime   = pParameter->getEndTime();

	estStartTime.SetHour(0);
	estStartTime.SetMinute(0);
	estStartTime.SetSecond(0);

	//estEndTime.SetHour(23);
	//estEndTime.SetMinute(59);
	//estEndTime.SetSecond(59);

	CString strTimeRange(_T(""));
	strTimeRange.Format(_T("%s-%s"), estStartTime.printTime(1), estEndTime.printTime(1));
	m_vTimeRange.push_back(strTimeRange);

	int nFltCount = (int)pParameter->getFlightConstraintCount();
	for (int i=0; i<nFltCount; i++)
	{
		COperationalResultData* pOperationalResultData = new COperationalResultData;
		pOperationalResultData->m_fltCons = pParameter->getFlightConstraint(i);
		pOperationalResultData->m_dData = 0.0;
		m_pResultData.push_back(pOperationalResultData);
	}

	for (int i=0; i<(int)m_pResultData.size(); i++)
	{
		CString strFlight(_T(""));
		m_pResultData[i]->m_fltCons.screenPrint(strFlight.GetBuffer(1024));
		strFlight.ReleaseBuffer();

		for (int j=0; j<(int)vResult.size(); j++)
		{
			CString strFlightTwo(_T(""));
			vResult[j].m_fltCons.screenPrint(strFlightTwo.GetBuffer(1024));
			strFlightTwo.ReleaseBuffer();

			if (strFlight.CompareNoCase(strFlightTwo) == 0)
			{
				double dDistance = ARCUnit::ConvertLength(vResult[j].m_fGroundDis, ARCUnit::CM, ARCUnit::M);
				m_pResultData[i]->m_dData += dDistance;
			}
		}
	}
}

void CFlightOperationalGroundDistanceResult::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{
	CString strLabel = _T("");
	C2DChartData c2dGraphData;
	c2dGraphData.m_strChartTitle = _T(" Flight Type vs. Ground Distance ");
	c2dGraphData.m_strYtitle = _T("Ground Distance(m)");
	c2dGraphData.m_strXtitle.Format(_T("Flight Type"));	
	c2dGraphData.m_vrXTickTitle = m_vTimeRange;

	//set footer
	CString strFooter(_T(""));
	strFooter.Format(_T("FLIGHT TYPE VS GROUND DISTANCE %s,%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime());
	bool bCommaFlag = true;
	CFlightOperationalParam* pOperationalParam = (CFlightOperationalParam*)pParameter;
	for (int i=0; i<(int)pOperationalParam->getFlightConstraintCount(); i++)
	{
		FlightConstraint fltCons = pOperationalParam->getFlightConstraint(i);

		CString strFlight(_T(""));
		fltCons.screenPrint(strFlight.GetBuffer(1024));

		if (bCommaFlag)
		{
			bCommaFlag = false;
			strFooter.AppendFormat("%s", strFlight);
		}
		else
		{
			strFooter.AppendFormat(",%s", strFlight);
		}
	}
	c2dGraphData.m_strFooter = strFooter;

	vector<COperationalResultData*>::iterator iter = m_pResultData.begin();
	for (; iter!=m_pResultData.end(); iter++)
	{
		CString strLegend = _T("");
		(*iter)->m_fltCons.screenPrint(strLegend);
		c2dGraphData.m_vrLegend.push_back(strLegend);


		vector<double> vSegmentData;
		vSegmentData.push_back((*iter)->m_dData);

		c2dGraphData.m_vr2DChartData.push_back(vSegmentData);
	}

	chartWnd.DrawChart(c2dGraphData);
}