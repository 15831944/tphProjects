#include "StdAfx.h"
#include "AirsideFlightSummaryActivityReportBaseResult.h"
#include "Parameters.h"
#include <limits>
#include "FlightActivityParam.h"
#include "../Common/ARCUnit.h"
#include "../InputAirside/ARCUnitManager.h"
//////////////////////////////////////////////////////////////////////////////////////////////

// set as the max sample points number, to limit the data set scale
static const long s_lMaxSamplePointsNum = 1000/*(std::numeric_limits<short>::max)()*/;


CAirsideFlightSummaryActivityReportBaseResult::CAirsideFlightSummaryActivityReportBaseResult()
{

}

CAirsideFlightSummaryActivityReportBaseResult::~CAirsideFlightSummaryActivityReportBaseResult()
{

}

double CAirsideFlightSummaryActivityReportBaseResult::GetSpeed(CFlightSummaryActivityResult::FlightActItem& fltActItem,long _time)
{
	long minTime = m_vMinTime[fltActItem];
	long maxTime = m_vMaxTime[fltActItem];

	double accSpeed = ARCUnit::ConvertLength(GetAccSpeed(fltActItem,_time),ARCUnit::FEET,ARCUnit::M);

	_time += minTime;
	long firstTime;
	long nextTime;
	size_t nCount = fltActItem.vItem.size();
	double m_Speed = 0 ;
	if (_time >= maxTime)
	{
		return fltActItem.vItem[nCount - 1].dSpeed;
	}
	if (_time >= minTime && _time <maxTime)
	{
		for (size_t j = 0; j < nCount; j++)
		{
			firstTime = fltActItem.vItem[j].tTime;
			if (j == nCount - 1)
			{
				return 0.0;
			}
			else
			{
				nextTime = fltActItem.vItem[j+1].tTime;
			}

			if (firstTime <= _time && _time< nextTime)
			{
				return (fltActItem.vItem[j].dSpeed + ARCUnit::ConvertVelocity(accSpeed,ARCUnit::MpS,ARCUnit::KNOT));
					 
			}
		}
	}

	return 0.0;
}

double CAirsideFlightSummaryActivityReportBaseResult::GetAccSpeed(CFlightSummaryActivityResult::FlightActItem& fltActItem,long _time)
{
	long minTime = m_vMinTime[fltActItem];
	long maxTime = m_vMaxTime[fltActItem];
	long startTime;
	long endTime;

	_time += minTime;
	if (_time >= minTime && _time <= maxTime)
	{
		size_t nCount = fltActItem.vATItem.size();
		for (size_t j = 0; j < nCount; j++)
		{
			CFlightSummaryActivityResult::ATItem item = fltActItem.vATItem[j];

			startTime = item.startTime;
			endTime  = item.endTime;

			if (startTime <= _time && _time< endTime)
			{
				return fltActItem.vATItem[j].dAcc * ((_time - startTime)/100);

			
			}
		}
	}
	return 0.0;
}

double CAirsideFlightSummaryActivityReportBaseResult::GetAcc(CFlightSummaryActivityResult::FlightActItem& fltActItem,long _time)
{
	long minTime = m_vMinTime[fltActItem];
	long maxTime = m_vMaxTime[fltActItem];
	long startTime;
	long endTime;

	_time += minTime;
	if (_time >= minTime && _time <= maxTime)
	{
		size_t nCount = fltActItem.vATItem.size();
		for (size_t j = 0; j < nCount; j++)
		{
			CFlightSummaryActivityResult::ATItem item = fltActItem.vATItem[j];
			
			startTime = item.startTime;
			endTime  = item.endTime;

			if (startTime <= _time && _time< endTime)
			{
				return  fltActItem.vATItem[j].dAcc;
	
			}
		}
	}
   return 0.0;
}

long CAirsideFlightSummaryActivityReportBaseResult::GetMaxTime(CFlightSummaryActivityResult::FlightActItem& fltActItem)
{
	long lMaxTime = (std::numeric_limits<long>::min)();

	int nCount = fltActItem.vItem.size();
	for (int j = 0; j < nCount; j++)
	{
		if (fltActItem.vItem[j].tTime > lMaxTime)
			lMaxTime = fltActItem.vItem[j].tTime;
	}
	
	
	return lMaxTime;
}

long CAirsideFlightSummaryActivityReportBaseResult::GetMinTime(CFlightSummaryActivityResult::FlightActItem& fltActItem)
{
	long lMinTime = (std::numeric_limits<long>::max)();
	
	int nCount = fltActItem.vItem.size();
	for (int j = 0; j < nCount; j++)
	{
		if (fltActItem.vItem[j].tTime < lMinTime)
			lMinTime = fltActItem.vItem[j].tTime;
	}
	

	return lMinTime;
}

long CAirsideFlightSummaryActivityReportBaseResult::GetAllIntervalTime(std::vector<CFlightSummaryActivityResult::FlightActItem>& vResult)
{
	m_vMaxTime.clear();
	m_vMinTime.clear();
	long lMaxIntervelTime = (std::numeric_limits<long>::min)();
	std::vector<CFlightSummaryActivityResult::FlightActItem>::iterator iter = vResult.begin();
	std::vector<CFlightSummaryActivityResult::FlightActItem>::iterator iterEnd = vResult.end();
	for (;iter != iterEnd; ++iter)
	{
		long lMaxTime = GetMaxTime(*iter);
		long lMinTime = GetMinTime(*iter);
		lMaxIntervelTime = max(lMaxIntervelTime,lMaxTime - lMinTime);
		m_vMaxTime.insert(std::map<CFlightSummaryActivityResult::FlightActItem,long>::value_type(*iter,lMaxTime));
		m_vMinTime.insert(std::map<CFlightSummaryActivityResult::FlightActItem,long>::value_type(*iter,lMinTime));
	}
	return lMaxIntervelTime;
}
////////////////////////////////////////////////////////////////////////////////////////////////
CAirsideFlightSummaryActivityReportATResult::CAirsideFlightSummaryActivityReportATResult()
{

}

CAirsideFlightSummaryActivityReportATResult::~CAirsideFlightSummaryActivityReportATResult()
{

}

void CAirsideFlightSummaryActivityReportATResult::GenerateResult(std::vector<CFlightSummaryActivityResult::FlightActItem>& vResult,CParameters *pParameter)
{
	m_vData.clear();
	m_vTimeRange.clear();
	ASSERT(pParameter);

	long lUserIntervalTime = 100L; // default to 1 sec
	long lTimeSegmentCount = GetAllIntervalTime(vResult) / (lUserIntervalTime);

	if (lTimeSegmentCount > s_lMaxSamplePointsNum)
	{
		lUserIntervalTime = lUserIntervalTime*lTimeSegmentCount/s_lMaxSamplePointsNum; // change the step
		lTimeSegmentCount = s_lMaxSamplePointsNum;
	}

	ElapsedTime estTempMaxTime;
	CString strTimeRange;
	for (long i=0; i<lTimeSegmentCount; i++)
	{
		estTempMaxTime.setPrecisely(lUserIntervalTime*i + 100L);
		strTimeRange.Format(_T("%02d:%02d"), estTempMaxTime.asHours(), estTempMaxTime.GetMinute());
		m_vTimeRange.push_back(strTimeRange);
	}

	const ARCUnit_Acceleration accUnit = CARCUnitManager::GetInstance().GetCurSelAccelerationUnit();
	for (long j = 0; j < (long)vResult.size(); j++)
	{
		m_vData.push_back(ActivityItem());
		ActivityItem& item = m_vData.back();
		item.sFlightLable.Format(_T("%s%s"), vResult[j].fltDesc._airline.GetValue(),
			vResult[j].bArrOrDeplActivity ? vResult[j].fltDesc._arrID.GetValue() : vResult[j].fltDesc._depID.GetValue()
			);

		for (int i=0; i<lTimeSegmentCount; i++)
		{
			double dAcc = GetAcc(vResult[j],lUserIntervalTime*i);
			item.vAcData.push_back(
				dAcc > 0.0 ? CARCAccelerationUnit::ConvertAcceleration(AU_ACCELERATION_FTPERS2,accUnit,dAcc) : 0.0
				);
		}
	}
}

void CAirsideFlightSummaryActivityReportATResult::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{
	CString strLabel = _T("");
	C2DChartData c2dGraphData;
	c2dGraphData.m_strChartTitle = _T("Flights Activity");
	c2dGraphData.m_strYtitle .Format(_T("Acceleration (%s)"),CARCAccelerationUnit::GetAccelerationUnitString(CARCUnitManager::GetInstance().GetCurSelAccelerationUnit()));
	c2dGraphData.m_strXtitle = _T("Time since start of activity(mins)");	
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

	std::vector<ActivityItem>::iterator iter = m_vData.begin();
	for (; iter!=m_vData.end(); iter++)
	{
		CString strLegend = _T("");
		strLegend = (*iter).sFlightLable;
		c2dGraphData.m_vrLegend.push_back(strLegend);

		/*int nSegmentCount = (int)(*iter).vAcData.size();

		vector<double> vSegmentData;

		for (int i=0; i<nSegmentCount; i++)
		{
			double dData = (*iter).vAcData[i];
			vSegmentData.push_back(dData);
		}*/

		c2dGraphData.m_vr2DChartData.push_back((*iter).vAcData);
	}
	chartWnd.SetAxiLabel(pParameter->getInterval());
	chartWnd.DrawChart(c2dGraphData);
}

/////////////////////////////////////////////////////////////////////////////////////////////////
CAirsideFlightSummaryActivityReportDTResult::CAirsideFlightSummaryActivityReportDTResult()
{

}

CAirsideFlightSummaryActivityReportDTResult::~CAirsideFlightSummaryActivityReportDTResult()
{

}

void CAirsideFlightSummaryActivityReportDTResult::GenerateResult(std::vector<CFlightSummaryActivityResult::FlightActItem>& vResult,CParameters *pParameter)
{
	m_vData.clear();
	m_vTimeRange.clear();
	ASSERT(pParameter);

	long lUserIntervalTime = 100L; // default to 1 sec
	long lTimeSegmentCount = GetAllIntervalTime(vResult) / (lUserIntervalTime);

	if (lTimeSegmentCount > s_lMaxSamplePointsNum)
	{
		lUserIntervalTime = lUserIntervalTime*lTimeSegmentCount/s_lMaxSamplePointsNum; // change the step
		lTimeSegmentCount = s_lMaxSamplePointsNum;
	}

	ElapsedTime estTempMaxTime;
	CString strTimeRange;
	for (long i=0; i<lTimeSegmentCount; i++)
	{
		estTempMaxTime.setPrecisely(lUserIntervalTime*i + 100L);
		strTimeRange.Format(_T("%02d:%02d"), estTempMaxTime.asHours(), estTempMaxTime.GetMinute());
		m_vTimeRange.push_back(strTimeRange);
	}

	const ARCUnit_Acceleration accUnit = CARCUnitManager::GetInstance().GetCurSelAccelerationUnit();
	for (long j = 0; j < (long)vResult.size(); j++)
	{
		m_vData.push_back(ActivityItem());
		ActivityItem& item = m_vData.back();
		item.sFlightLable.Format(_T("%s%s"), vResult[j].fltDesc._airline.GetValue(),
			vResult[j].bArrOrDeplActivity ? vResult[j].fltDesc._arrID.GetValue() : vResult[j].fltDesc._depID.GetValue()
			);

		for (int i=0; i<lTimeSegmentCount; i++)
		{
			double dAcc = GetAcc(vResult[j],lUserIntervalTime*i);
			item.vAcData.push_back(
				dAcc < 0.0 ? CARCAccelerationUnit::ConvertAcceleration(AU_ACCELERATION_FTPERS2,accUnit,-dAcc) : 0.0
				);
		}
	}
}

void CAirsideFlightSummaryActivityReportDTResult::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{
	CString strLabel = _T("");
	C2DChartData c2dGraphData;
	c2dGraphData.m_strChartTitle = _T("Flights Activity");
	c2dGraphData.m_strYtitle .Format(_T("Deceleration (%s)"),CARCAccelerationUnit::GetAccelerationUnitString(CARCUnitManager::GetInstance().GetCurSelAccelerationUnit()));
	c2dGraphData.m_strXtitle = _T("Time since start of activity(mins)");	
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

	std::vector<ActivityItem>::iterator iter = m_vData.begin();
	for (; iter!=m_vData.end(); iter++)
	{
		CString strLegend = _T("");
		strLegend = (*iter).sFlightLable;
		c2dGraphData.m_vrLegend.push_back(strLegend);

		/*int nSegmentCount = (int)(*iter).vAcData.size();

		vector<double> vSegmentData;

		for (int i=0; i<nSegmentCount; i++)
		{
			double dData = (*iter).vAcData[i];
			vSegmentData.push_back(dData);
		}*/

		c2dGraphData.m_vr2DChartData.push_back((*iter).vAcData);
	}
	chartWnd.SetAxiLabel(pParameter->getInterval());
	chartWnd.DrawChart(c2dGraphData);
}
//////////////////////////////////////////////////////////////////////////////////////////////////
CAirsideFlightSummaryActivityReportVTResult::CAirsideFlightSummaryActivityReportVTResult()
{

}

CAirsideFlightSummaryActivityReportVTResult::~CAirsideFlightSummaryActivityReportVTResult()
{

}

void CAirsideFlightSummaryActivityReportVTResult::GenerateResult(std::vector<CFlightSummaryActivityResult::FlightActItem>& vResult,CParameters *pParameter)
{
	m_vData.clear();
	m_vTimeRange.clear();
	ASSERT(pParameter);

	long lUserIntervalTime = 100l;
	long lTimeSegmentCount = GetAllIntervalTime(vResult) / (lUserIntervalTime);
	if (lTimeSegmentCount)
	{
		lTimeSegmentCount++;
	}

	if (lTimeSegmentCount > s_lMaxSamplePointsNum)
	{
		lUserIntervalTime = lUserIntervalTime*lTimeSegmentCount/s_lMaxSamplePointsNum; // change the step
		lTimeSegmentCount = s_lMaxSamplePointsNum;
	}

	ElapsedTime estTempMaxTime;
	CString strTimeRange;
	for (long i=0; i<lTimeSegmentCount; i++)
	{
		estTempMaxTime.setPrecisely(lUserIntervalTime*i + 100L);
		strTimeRange.Format(_T("%02d:%02d"), estTempMaxTime.asHours(), estTempMaxTime.GetMinute());
		m_vTimeRange.push_back(strTimeRange);
	}

	ARCUnit_Velocity velUnit = CARCUnitManager::GetInstance().GetCurSelVelocityUnit();
	for (long j = 0; j < (long)vResult.size(); j++)
	{
		m_vData.push_back(ActivityItem());
		ActivityItem& item = m_vData.back();

		item.sFlightLable.Format(_T("%s%s"), vResult[j].fltDesc._airline.GetValue(),
			vResult[j].bArrOrDeplActivity ? vResult[j].fltDesc._arrID.GetValue() : vResult[j].fltDesc._depID.GetValue()
			);
				
		for (int i=0; i<=lTimeSegmentCount; i++)
		{
			double speed = GetSpeed(vResult[j],lUserIntervalTime*i) ;
			item.vAcData.push_back(CARCVelocityUnit::ConvertVelocity(AU_VELOCITY_KTS, velUnit, speed));
		}
	}
}

void CAirsideFlightSummaryActivityReportVTResult::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{
	CString strLabel = _T("");
	C2DChartData c2dGraphData;
	c2dGraphData.m_strChartTitle = _T("Flights Activity");
	c2dGraphData.m_strYtitle.Format(_T("Velocity (%s)"),CARCVelocityUnit::GetVelocityUnitString(CARCUnitManager::GetInstance().GetCurSelVelocityUnit()));
	c2dGraphData.m_strXtitle = _T("Time since start of activity(mins)");	
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

	std::vector<ActivityItem>::iterator iter = m_vData.begin();
	for (; iter!=m_vData.end(); iter++)
	{
		CString strLegend = _T("");
		strLegend = (*iter).sFlightLable;
		c2dGraphData.m_vrLegend.push_back(strLegend);

		/*int nSegmentCount = (int)(*iter).vAcData.size();

		vector<double> vSegmentData;

		for (int i=0; i<nSegmentCount; i++)
		{
			double dData = (*iter).vAcData[i];
			vSegmentData.push_back(dData);
		}*/

		c2dGraphData.m_vr2DChartData.push_back((*iter).vAcData);
	}

	chartWnd.SetAxiLabel(pParameter->getInterval());
	chartWnd.DrawChart(c2dGraphData);
}