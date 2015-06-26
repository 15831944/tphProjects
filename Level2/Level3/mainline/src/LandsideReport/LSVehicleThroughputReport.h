#pragma once
#include "LandsideBaseReport.h"

class LANDSIDEREPORT_API LSVehicleThroughputReport : public LandsideBaseReport
{
public:
	LSVehicleThroughputReport(void);
	~LSVehicleThroughputReport(void);

public:
	virtual void GenerateReport(CBGetLogFilePath pFunc, InputLandside *pLandside);

	virtual int GetReportType() const;

	virtual CString GetReportFileName();
	virtual void RefreshReport();

	virtual BOOL WriteReportData(ArctermFile& _file);
	virtual BOOL ReadReportData(ArctermFile& _file);
};
