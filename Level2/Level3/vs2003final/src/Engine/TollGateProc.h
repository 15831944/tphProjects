#ifndef TOLLGATEPROC_DEF
#define TOLLGATEPROC_DEF

#pragma once
#include "landcontroldevice.h"

class TollGateProc :
	public LandControlDevice
{
public:
	TollGateProc(void);
	virtual ~TollGateProc(void);

	//processor type
	virtual int getProcessorType (void) const { return TollGateProcessor; }
	//processor name
	virtual const char *getProcessorName (void) const { return m_strTypeName; }

	//virtual bool HasProperty(ProcessorProperty propType)const;

public:
	const static CString m_strTypeName; // "TollGate"
	
	virtual LandfieldProcessor * GetCopy() ;

};

#endif