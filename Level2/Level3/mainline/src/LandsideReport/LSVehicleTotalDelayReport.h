#pragma once
#include "landsidebasereport.h"

class LANDSIDEREPORT_API LSVehicleTotalDelayReport : public LandsideBaseReport
{
public:
	LSVehicleTotalDelayReport(void);
	~LSVehicleTotalDelayReport(void);

public:
	virtual void GenerateReport(CBGetLogFilePath pFunc, InputLandside *pLandside);

	virtual int GetReportType() const;

	virtual CString GetReportFileName();
	virtual void RefreshReport();

	virtual BOOL WriteReportData(ArctermFile& _file);
	virtual BOOL ReadReportData(ArctermFile& _file);
};
