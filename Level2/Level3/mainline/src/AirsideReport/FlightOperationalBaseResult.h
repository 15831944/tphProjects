#pragma once
#include "airsidereportbaseresult.h"
#include "OperationalResultData.h"
#include <vector>

class CFlightOperationalBaseResult :
	public CAirsideReportBaseResult
{
public:
	CFlightOperationalBaseResult(void);
	virtual ~CFlightOperationalBaseResult(void); 

	void ClearAllData();

protected:
	vector<COperationalResultData*>  m_pResultData;
	vector<CString>                  m_vTimeRange;
};