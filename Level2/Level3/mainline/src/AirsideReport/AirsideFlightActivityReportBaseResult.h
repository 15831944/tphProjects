#pragma once
#include "airsidereportbaseresult.h"
#include "FlightActivityReport.h"
#include "FlightActivityBaseResult.h"

class CARC3DChart;
class CParameters;
using namespace std;

class AIRSIDEREPORT_API CAirsideFlightActivityReportBaseResult :
	public CAirsideReportBaseResult
{
public:
	CAirsideFlightActivityReportBaseResult(void);
	~CAirsideFlightActivityReportBaseResult(void);

	virtual void GenerateResult(vector<CFlightDetailActivityResult::FltActItem>& m_vResult,CParameters *pParameter) = 0;

public:
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter) = 0;

protected:
	int                  m_nSubReportType;
	vector<int>          m_vNumOfFlight;
	vector<CString>      m_vTimeRange;
};

class AIRSIDEREPORT_API CAirsideFlightActivityReportEntryResult: public CAirsideFlightActivityReportBaseResult
{

public:
	CAirsideFlightActivityReportEntryResult();
	~CAirsideFlightActivityReportEntryResult();

public:
	virtual void GenerateResult(vector<CFlightDetailActivityResult::FltActItem>& vResult,CParameters *pParameter);
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);

};



class AIRSIDEREPORT_API CAirsideFlightActivityReportExitResult: public CAirsideFlightActivityReportBaseResult
{

public:
	CAirsideFlightActivityReportExitResult();
	~CAirsideFlightActivityReportExitResult();

public:
	virtual void GenerateResult(vector<CFlightDetailActivityResult::FltActItem>& vResult,CParameters *pParameter);
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);

};


