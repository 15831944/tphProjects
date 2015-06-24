#pragma once
#include "../Common/elaptime.h"
#include "..\common\template.h"
#include "../Common/ALTObjectID.h"
#include "inputsdll.h"
class CBaseFlight
{
public:
	CBaseFlight(void);
	~CBaseFlight(void);

	virtual ElapsedTime getArrTime (void) const =0;
	virtual ElapsedTime getDepTime (void) const=0;
	virtual void getArrID (char *p_str) const =0;

	//void getDepID (char *p_str) const { itoa (logEntry.depID, p_str, 10); }
	virtual void getDepID (char *p_str) const =0;

	virtual int isArriving (void) const =0;
	//int isDeparting (void) const { return logEntry.depID.HasValue(); }
	virtual int isDeparting (void) const =0;

	virtual bool isSameGate()=0;
	virtual bool isSameFlightID() = 0;

};