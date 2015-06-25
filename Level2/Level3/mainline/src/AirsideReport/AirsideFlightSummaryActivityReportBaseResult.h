#pragma  once
#include <vector>
#include "FlightActivityBaseResult.h"
class CParameters;
class AIRSIDEREPORT_API CAirsideFlightSummaryActivityReportBaseResult
{
public:
	CAirsideFlightSummaryActivityReportBaseResult();
	virtual ~CAirsideFlightSummaryActivityReportBaseResult();
	class ActivityItem
	{
	public:
		ActivityItem()
			:sFlightLable(_T(""))
		{

		}
		~ActivityItem()
		{

		}
	public:
		std::vector<double> vAcData;
		CString sFlightLable;
	};
	virtual void GenerateResult(std::vector<CFlightSummaryActivityResult::FlightActItem>& vResult,CParameters *pParameter) = 0;
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter) = 0;
	double GetSpeed(CFlightSummaryActivityResult::FlightActItem& fltActItem,long _time);
	double GetAcc(CFlightSummaryActivityResult::FlightActItem& fltActItem,long _time);
	double GetAccSpeed(CFlightSummaryActivityResult::FlightActItem& fltActItem,long _time);

	long GetMaxTime(CFlightSummaryActivityResult::FlightActItem& fltActItem);
	long GetMinTime(CFlightSummaryActivityResult::FlightActItem& fltActItem);
	long GetAllIntervalTime(std::vector<CFlightSummaryActivityResult::FlightActItem>& vResult);

	long GetActivityItemCount() const { return m_vData.size(); }
protected:
	std::vector<CString> m_vTimeRange;
	std::vector<ActivityItem>m_vData;
	std::map<CFlightSummaryActivityResult::FlightActItem,long>m_vMaxTime;
	std::map<CFlightSummaryActivityResult::FlightActItem,long>m_vMinTime;
};

class AIRSIDEREPORT_API CAirsideFlightSummaryActivityReportATResult : public CAirsideFlightSummaryActivityReportBaseResult
{
public:
	CAirsideFlightSummaryActivityReportATResult();
	virtual ~CAirsideFlightSummaryActivityReportATResult();

	virtual void GenerateResult(std::vector<CFlightSummaryActivityResult::FlightActItem>& vResult,CParameters *pParameter);
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);
private:
};

class AIRSIDEREPORT_API CAirsideFlightSummaryActivityReportDTResult: public CAirsideFlightSummaryActivityReportBaseResult
{
public:
	CAirsideFlightSummaryActivityReportDTResult();
	virtual ~CAirsideFlightSummaryActivityReportDTResult();

	virtual void GenerateResult(std::vector<CFlightSummaryActivityResult::FlightActItem>& vResult,CParameters *pParameter);
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);
private:
};

class AIRSIDEREPORT_API CAirsideFlightSummaryActivityReportVTResult: public CAirsideFlightSummaryActivityReportBaseResult
{
public:
	CAirsideFlightSummaryActivityReportVTResult();
	virtual ~CAirsideFlightSummaryActivityReportVTResult();

	virtual void GenerateResult(std::vector<CFlightSummaryActivityResult::FlightActItem>& vResult,CParameters *pParameter);
	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);
private:
};