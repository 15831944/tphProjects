#pragma once
#include "landsidebaseparam.h"

class LANDSIDEREPORT_API LSResourceQueueParam :
	public LandsideBaseParam
{
public:
	LSResourceQueueParam(void);
	~LSResourceQueueParam(void);

	virtual CString GetReportParamName();

};
