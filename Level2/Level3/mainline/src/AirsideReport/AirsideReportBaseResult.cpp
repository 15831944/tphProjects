#include "StdAfx.h"
#include ".\airsidereportbaseresult.h"

CAirsideReportBaseResult::CAirsideReportBaseResult(void)
{
}

CAirsideReportBaseResult::~CAirsideReportBaseResult(void)
{
}
long  CAirsideReportBaseResult::ClacValueRange(double& dMaxValue,double& dMinValue,double& dInterval)
{
	long nColumn = 6;
	if (dMinValue == dMaxValue)
		dMinValue = 0.0;

	double dDiff = dMaxValue - dMinValue;

	//calc step value(10)
	double dStepValue = 1.0f;

	while (static_cast<int>(dDiff/dStepValue) > nColumn)
	{
		dStepValue *= 10;
	}

	dStepValue /= 20;

	//calculate step (5)
	int i = 1;
	dInterval = dStepValue;
	while (static_cast<int>(dDiff/dInterval) > nColumn)
	{
		dInterval += dStepValue ;
	}

	int tempInterval = static_cast<int>(dInterval);
	if (tempInterval == 0)
		dInterval = 1;
	dMinValue = dMinValue-  ((int)dMinValue)%((int)dInterval);
	dMinValue = static_cast<int>(dMinValue);
	dMaxValue = dMaxValue - ((int)dMaxValue)%((int)dInterval) + dInterval;
	dMaxValue = static_cast<int>(dMaxValue);
	return (int)(dDiff/dInterval) + 2;

}