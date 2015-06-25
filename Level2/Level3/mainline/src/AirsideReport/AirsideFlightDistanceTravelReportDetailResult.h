#pragma once
#include "AirsideFlightDistanceTravelReportBaseResult.h"


class CARC3DChart;
class CParameters;

class CAirsideFlightDistanceTravelReportDetailResult :
	public CAirsideFlightDistanceTravelReportBaseResult
{
public:
	CAirsideFlightDistanceTravelReportDetailResult(void);
	~CAirsideFlightDistanceTravelReportDetailResult(void);

	void GenerateResult(std::vector<CAirsideDistanceTravelReport::DistanceTravelReportItem>& vResult,CParameters *pParameter);

	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);
	virtual int getType(){ return 0;}


	void GetMinMaxTravelDistance(std::vector<CAirsideDistanceTravelReport::DistanceTravelReportItem>& vResult,double& dMinDist, double& dMaxDist );
	int GetFlightCountTravelDist(std::vector<CAirsideDistanceTravelReport::DistanceTravelReportItem>& vResult,double dMinValue, double dMaxValue,CParameters *pParameter);

protected:
	double m_dMinDist;
	double m_dMaxDist;;
	double m_dInterval;
	std::vector<int> m_vResult;


	double GetConvertValue(double dInput, CParameters *pParameter);
public:
	 void Draw3DChartFromFile(CARC3DChart& chartWnd, ArctermFile& _file)  ;
	 void ExportFileFrom3DChart( ArctermFile& _file, CParameters *pParameter) ;
};
