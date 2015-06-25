#pragma once
#include "AirsideFlightDistanceTravelReportBaseResult.h"
#include "../common/FLT_CNST.H"
#include "AirsideReportSummaryData.h"

class CARC3DChart;

class CAirsideFlightDistanceTravelReportSummaryResult :
	public CAirsideFlightDistanceTravelReportBaseResult
{

public:
	class CFltTypeSummaryData
	{
	public:
		CFltTypeSummaryData(FlightConstraint& _fltCons)
		{
			fltCons = _fltCons;
		
		}
		~CFltTypeSummaryData(){}
		
		public:
		FlightConstraint fltCons;
		CAirsideReportSummaryData summaryData;

		std::vector<double> vResult;

		void GenerateSummaryData();

	private:
	}; 

public:
	CAirsideFlightDistanceTravelReportSummaryResult(void);
	~CAirsideFlightDistanceTravelReportSummaryResult(void);



	void GenerateResult(std::vector<CAirsideDistanceTravelReport::DistanceTravelReportItem>& vResult,CParameters *pParameter);

	virtual void Draw3DChart(CARC3DChart& chartWnd, CParameters *pParameter);
	virtual int getType(){ return 1;}

	std::vector<CFltTypeSummaryData> & GetResult(){ return m_vResult;}
protected:
	std::vector<CFltTypeSummaryData> m_vResult;

};
