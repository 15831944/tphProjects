#pragma once
#include "airsidereportbaseresult.h"
class CRunwayExitParameter;
class CRunwayExitRepData;
class CRunwayExitSummaryDataItem;
class CRunwayExitResult :
	public CAirsideReportBaseResult
{
public:
	CRunwayExitResult(void);
	virtual ~CRunwayExitResult(void);
public:
public:
	virtual void GenerateResult() = 0;
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter) = 0;
};
class CRunwayDetailExitResult : public CRunwayExitResult
{
public:
	CRunwayDetailExitResult(CRunwayExitRepData* _ReportBaseData ,CRunwayExitParameter* _Parameter):m_ReportBaseData(_ReportBaseData),m_Parameter(_Parameter),CRunwayExitResult(){};
protected:
	CRunwayExitRepData* m_ReportBaseData ;
	CRunwayExitParameter* m_Parameter ;
public:
	virtual void GenerateResult() {};
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter) ;
};

class CRunwaySummaryExitResult: public CRunwayExitResult
{
public:
	CRunwaySummaryExitResult(std::vector<CRunwayExitSummaryDataItem>* _data,CRunwayExitParameter* _Parameter):m_SummaryData(_data),m_Parameter(_Parameter),CRunwayExitResult(){} ;
protected:
	std::vector<CRunwayExitSummaryDataItem>* m_SummaryData ;
	CRunwayExitParameter* m_Parameter ;
public:
	void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter) ;
	void GenerateResult() {};
};
