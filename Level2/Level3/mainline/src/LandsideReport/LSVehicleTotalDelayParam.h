#pragma once
#include "LandsideBaseParam.h"
class LSVehicleTotalDelayParam : public LandsideBaseParam
{
public:
	LSVehicleTotalDelayParam(void);
	~LSVehicleTotalDelayParam(void);

	virtual CString GetReportParamName();
};
