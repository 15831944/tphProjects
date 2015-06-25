#pragma once
#include "../Reports/StatisticalTools.h"
#include "../Common/elaptime.h"

class ReportDataFilter;
class FlightConflictSummaryData
{
public:
	FlightConflictSummaryData(void);
	~FlightConflictSummaryData(void);

public:

	ReportDataFilter* m_pParaFilter;

	CString m_strAreaName;
	int m_nConflictCount;

	//min
	ElapsedTime m_tTotal;
	ElapsedTime m_tMin;
	ElapsedTime m_tMax;
	CStatisticalTools<double> m_statisticalTool;

	////quarter
	//ElapsedTime m_eQ1;
	//ElapsedTime m_eQ2;
	//ElapsedTime m_eQ3;

	////percentage
	//ElapsedTime m_eP1;
	//ElapsedTime m_eP5;
	//ElapsedTime m_eP10;
	//ElapsedTime m_eP90;
	//ElapsedTime m_eP95;
	//ElapsedTime m_eP99;
	//ElapsedTime m_eStdDev;



};
