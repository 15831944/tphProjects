#ifndef YIELSIGN_PROC_DEF
#define YIELSIGN_PROC_DEF

#pragma once
#include "landcontroldevice.h"

class YieldSignProc :
	public LandControlDevice
{
public:
	YieldSignProc(void);
	virtual ~YieldSignProc(void);
	//processor type
	virtual int getProcessorType (void) const { return YieldDeviceProcessor; }
	//processor name
	virtual const char *getProcessorName (void) const { return m_strTypeName; }

	//virtual bool HasProperty(ProcessorProperty propType)const;
	virtual LandfieldProcessor * GetCopy() ;
public:
	const static CString m_strTypeName; // "TrafficLight"

};

#endif