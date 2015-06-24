#ifndef __TRAFFIC_LIGHT_PROC
#define __TRAFFIC_LIGHT_PROC

#pragma once
#include "landcontroldevice.h"

class TrafficlightProc :
	public LandControlDevice
{
public:

	TrafficlightProc(void);
	virtual ~TrafficlightProc(void);

	//processor type
	virtual int getProcessorType (void) const { return TrafficLightProceesor; }
	//processor name
	virtual const char *getProcessorName (void) const { return m_strTypeName; }

	//virtual bool HasProperty(ProcessorProperty propType)const;
	virtual LandfieldProcessor * GetCopy() ;

public:
	const static CString m_strTypeName; // "TrafficLight"

};

#endif
