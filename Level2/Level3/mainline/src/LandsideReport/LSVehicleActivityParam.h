#pragma once
#include "landsidebaseparam.h"
#include "LandsideReportAPI.h"


class LANDSIDEREPORT_API LSVehicleActivityParam :
	public LandsideBaseParam
{
public:
	LSVehicleActivityParam(void);
	~LSVehicleActivityParam(void);

	virtual CString GetReportParamName();
};
