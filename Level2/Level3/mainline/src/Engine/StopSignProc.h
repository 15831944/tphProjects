#ifndef STOPSIGN_PROC_DEF
#define  STOPSIGN_PROC_DEF


#pragma once
#include "landcontroldevice.h"

class StopSignProc :
	public LandControlDevice
{
public:
	StopSignProc(void);
	virtual ~StopSignProc(void);
	//processor type
	virtual int getProcessorType (void) const { return StopSignProcessor; }
	//processor name
	virtual const char *getProcessorName (void) const { return m_strTypeName; }

	virtual LandfieldProcessor * GetCopy() ;
public:
	const static CString m_strTypeName; // "TrafficLight"

};

#endif