#pragma once

class CAirsideReportSummaryData
{
public:
	CAirsideReportSummaryData(void);
	virtual ~CAirsideReportSummaryData(void);

	double   m_dTotal;
	double   m_dMin;
	double   m_dAverage;
	double   m_dMax;
	double   m_dQ1;
	double   m_dQ2;
	double   m_dQ3;
	double   m_dP1;
	double   m_dP5;
	double   m_dP10;
	double   m_dP90;
	double   m_dP95;
	double   m_dP99;
	double   m_dSigma;	
	double   m_dCount;
};
