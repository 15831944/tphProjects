#pragma once
#include "landsidebaseparam.h"

class LANDSIDEREPORT_API LSVehicleDelayParam :
	public LandsideBaseParam
{
public:
	LSVehicleDelayParam(void);
	~LSVehicleDelayParam(void);


	virtual CString GetReportParamName();


};
