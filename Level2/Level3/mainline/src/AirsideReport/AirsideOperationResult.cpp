#include "StdAfx.h"
#include ".\airsideoperationresult.h"
#include "Parameters.h"
#include "AirsideOperationParam.h"

CAirsideOperationResult::CAirsideOperationResult(void)
{
}

CAirsideOperationResult::~CAirsideOperationResult(void)
{
	ClearAllData();
}

void CAirsideOperationResult::GenerateResult(std::vector<CAirsideOperationReport::AirsideFltOperationItem>& airsideFltOperationItem,CParameters *pParameter)
{
	ClearAllData();
	ASSERT(pParameter != NULL);

	ElapsedTime estMinDelayTime = pParameter->getStartTime();
	ElapsedTime estMaxDelayTime = pParameter->getEndTime();

	long lMinDelayTime = estMinDelayTime.asSeconds();
	long lMaxDelayTime = estMaxDelayTime.asSeconds();

	//delay time error
	if (lMaxDelayTime < lMinDelayTime)
	{
		return;
	}

	m_estStartTime = estMinDelayTime;
	m_estEndTime   = estMaxDelayTime;

	long lUserIntervalTime = pParameter->getInterval();
	ElapsedTime estUserIntervalTime = ElapsedTime(lUserIntervalTime);

	long lDelayTimeSegmentCount = 0;             //the count of the delayTime segment
	if (0 < lUserIntervalTime)
	{
		lDelayTimeSegmentCount = (lMaxDelayTime - lMinDelayTime) / (lUserIntervalTime);
		estMinDelayTime = ElapsedTime((lMinDelayTime - lMinDelayTime%(lUserIntervalTime)));

	//	lDelayTimeSegmentCount++;
	}
	else
	{
		lDelayTimeSegmentCount= ClacTimeRange(estMaxDelayTime, estMinDelayTime, estUserIntervalTime);
	}

	COperationReportDataItem* pReportDataItem = new COperationReportDataItem;
	pReportDataItem->enumOperationType = OPERATIONTYPE_ARR;
	m_vOperationdReportData.push_back(pReportDataItem);

	pReportDataItem = new COperationReportDataItem;
	pReportDataItem->enumOperationType = OPERATIONTYPE_DEP;
	m_vOperationdReportData.push_back(pReportDataItem);

	//pReportDataItem = new COperationReportDataItem;
	//pReportDataItem->enumOperationType = OPERATIONTYPE_TA;
	//m_vOperationdReportData.push_back(pReportDataItem);

	for (long i=0; i<lDelayTimeSegmentCount; i++)
	{
		ElapsedTime estTempMinDelayTime = estMinDelayTime + ElapsedTime(estUserIntervalTime.asSeconds()*i);
		ElapsedTime estTempMaxDelayTime = estMinDelayTime + ElapsedTime(estUserIntervalTime.asSeconds()*(i + 1));

		CString strTimeRange = _T("");
		strTimeRange.Format(_T("%02d:%02d-%02d:%02d"), estTempMinDelayTime.asHours(), estTempMinDelayTime.GetMinute(), estTempMaxDelayTime.asHours(), estTempMaxDelayTime.GetMinute());
		m_vTimeRange.push_back(strTimeRange);

		for (int j=0; j<(int)m_vOperationdReportData.size(); j++)
		{
			COperationReportDataItem* pItem = m_vOperationdReportData[j];

			pItem->m_vNumOfFlight.push_back(GetFlightNum(airsideFltOperationItem, pItem->enumOperationType, estTempMinDelayTime, estTempMaxDelayTime, pParameter));
		}
	}
}

int CAirsideOperationResult::GetFlightNum(std::vector<CAirsideOperationReport::AirsideFltOperationItem>& airsideFltOperationItem, 
										  ENUMFLIGHTOPERATIONTYPE operationType, ElapsedTime& estStartTime, ElapsedTime& estEndTime, CParameters* pParameter)
{
	ASSERT(pParameter);
	
	int nFlightCount = 0;
	for (int j=0; j<(int)pParameter->getFlightConstraintCount(); j++)
	{
		FlightConstraint flightConstraint = pParameter->getFlightConstraint(j);

		for (int i=0; i<(int)airsideFltOperationItem.size(); i++)
		{
			if (operationType == OPERATIONTYPE_ARR
				&& airsideFltOperationItem[i].operationType == CAirsideOperationReport::FltOperationType_Arr)
			{
				if (flightConstraint.fits(airsideFltOperationItem[i].fltcons)
					&& estStartTime.asSeconds()*100 < airsideFltOperationItem[i].actualArrTime
					&& airsideFltOperationItem[i].actualArrTime <= estEndTime.asSeconds()*100)
				{
					nFlightCount++;
				}
			}
			else if (operationType == OPERATIONTYPE_DEP
				&& airsideFltOperationItem[i].operationType == CAirsideOperationReport::FltOperationType_Dep)
			{
				if (flightConstraint.fits(airsideFltOperationItem[i].fltcons)
					&& estStartTime.asSeconds()*100 < airsideFltOperationItem[i].actualDepTime
					&& airsideFltOperationItem[i].actualDepTime <= estEndTime.asSeconds()*100)
				{
					nFlightCount++;
				}
			}
			//else if (operationType == OPERATIONTYPE_TA
			//	&& airsideFltOperationItem[i].operationType == CAirsideOperationReport::FltOperationType_TA)
			//{
			//	if (flightConstraint.fits(airsideFltOperationItem[i].fltcons)
			//		&& estStartTime.asSeconds()*100 < airsideFltOperationItem[i].actualArrTime
			//		&& airsideFltOperationItem[i].actualArrTime <= estEndTime.asSeconds()*100)
			//	{
			//		nFlightCount++;
			//	}
			//}
		}
	}

	return nFlightCount;
}

void CAirsideOperationResult::Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter)
{
	CString strLabel = _T("");
	C2DChartData c2dGraphData;
	c2dGraphData.m_strChartTitle = _T(" Operations ");
	c2dGraphData.m_strYtitle = _T("Number of aircraft operations");
	c2dGraphData.m_strXtitle = _T("Time of Day");	

	c2dGraphData.m_vrXTickTitle = m_vTimeRange;

	//set footer
	CString strFooter(_T(""));
	ElapsedTime estIntervalTime(pParameter->getInterval());
	strFooter.Format(_T("AIRCRAFT OPERATIONS REPORT %s;%s;%s "), pParameter->getStartTime().printTime(), pParameter->getEndTime().printTime(), estIntervalTime.printTime());
	bool bCommaFlag = true;
	CAirsideOperationParam* pAirsideOperationParam = (CAirsideOperationParam*)pParameter;
	for (int i=0; i<(int)pAirsideOperationParam->getFlightConstraintCount(); i++)
	{
		FlightConstraint fltCons = pAirsideOperationParam->getFlightConstraint(i);

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

	vector<COperationReportDataItem*>::iterator iter = m_vOperationdReportData.begin();
	for (; iter!=m_vOperationdReportData.end(); iter++)
	{
		CString strLegend = _T("");
		if ((*iter)->enumOperationType == OPERATIONTYPE_ARR)
		{
			strLegend = _T("ARR");
		}
		else if ((*iter)->enumOperationType == OPERATIONTYPE_DEP)
		{
			strLegend = _T("DEP");
		}
		//else if ((*iter)->enumOperationType == OPERATIONTYPE_TA)
		//{
		//	strLegend = _T("TA");
		//}
		c2dGraphData.m_vrLegend.push_back(strLegend);

		int nSegmentCount = (int)(*iter)->m_vNumOfFlight.size();

		vector<double> vSegmentData;

		for (int i=0; i<nSegmentCount; i++)
		{
			double dData = (*iter)->m_vNumOfFlight[i];
			vSegmentData.push_back(dData);
		}

		c2dGraphData.m_vr2DChartData.push_back(vSegmentData);
	}

	chartWnd.DrawChart(c2dGraphData);
}

void CAirsideOperationResult::ClearAllData()
{
	for (int i=0; i<(int)m_vOperationdReportData.size(); i++)
	{
		delete m_vOperationdReportData[i];
	}

	m_vOperationdReportData.clear();
}

long CAirsideOperationResult::ClacTimeRange(ElapsedTime& eMaxTime,ElapsedTime& eMinValue,ElapsedTime& eInterval)
{
	if (eMinValue == eMaxTime)
	{
		eMinValue = ElapsedTime(0L);
	}
	bool bInSecond = false;
	int _nMinNumCol = 3;
	ElapsedTime timeDiff = eMaxTime - eMinValue;

	ElapsedTime eUnitTimeOnX;

	int nTimeDiffAsHours = timeDiff.asHours();
	int nTimeDiffAsMin = timeDiff.asMinutes();
	int nTimeDiffAsSec = timeDiff.asSeconds();

	if( nTimeDiffAsHours > _nMinNumCol )
	{
		// if hh  > _nMinNumCol, display column up to 2*_nMinNumCol
		int nMinHours = nTimeDiffAsHours / (2*_nMinNumCol) + 1;
		eUnitTimeOnX.set( nMinHours, 0, 0 );
		eMinValue.SetMinute( 0 );
		eMinValue.SetSecond( 0 );
		if( nTimeDiffAsHours > (2*_nMinNumCol) )
			_nMinNumCol = (2*_nMinNumCol);
		else
			_nMinNumCol = nTimeDiffAsHours;
	}
	else if( nTimeDiffAsMin > _nMinNumCol )
	{
		// else if hhmm > nNumCol min, so the column unit should be hhmm % 2 nNumCol min
		int nMinMinutes = nTimeDiffAsMin / ( 2*_nMinNumCol ) + 1;
		eUnitTimeOnX.set( nMinMinutes / 60, nMinMinutes % 60, 0 );
		eMinValue.SetSecond( 0 );
		if( nTimeDiffAsMin > 2 * _nMinNumCol )
			_nMinNumCol = 2 * _nMinNumCol;
		else
			_nMinNumCol = nTimeDiffAsMin;
	}
	else if( nTimeDiffAsSec > _nMinNumCol )
	{
		// be hhmmss % 2 nNumCol sec.
		int mMinSeconds = nTimeDiffAsSec / ( 2*_nMinNumCol ) + 1;
		eUnitTimeOnX.set( mMinSeconds / 3600, mMinSeconds / 60, mMinSeconds % 60 );
		if( nTimeDiffAsSec > 2 * _nMinNumCol )
			_nMinNumCol = 2 * _nMinNumCol;
		else
			_nMinNumCol = nTimeDiffAsSec;
		bInSecond = true;
	}

	eInterval = eUnitTimeOnX;
	if (eInterval == ElapsedTime(0L))
	{
		eInterval = ElapsedTime(60L);
	}

	return _nMinNumCol;
}