#pragma once
#include "landsidebasereport.h"
#include "LandsideReportAPI.h"



class LANDSIDEREPORT_API LSVehicleActivityReport :
	public LandsideBaseReport
{
public:
	LSVehicleActivityReport(void);
	~LSVehicleActivityReport(void);


public:
	virtual void GenerateReport(CBGetLogFilePath pFunc, InputLandside *pLandside);

	virtual int GetReportType() const;

	virtual CString GetReportFileName();
	virtual void RefreshReport();

	virtual BOOL WriteReportData(ArctermFile& _file);
	virtual BOOL ReadReportData(ArctermFile& _file);


};
