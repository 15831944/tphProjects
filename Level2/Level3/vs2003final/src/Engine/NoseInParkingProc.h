#ifndef __NOSEINPARKING_PROC_DEF
#define __NOSEINPARKING_PROC_DEF

#pragma once
#include "landfieldprocessor.h"

class StretchProc;
class NoseInParkingProc :
	public LandfieldProcessor
{
public:
	NoseInParkingProc(void);
	virtual ~NoseInParkingProc(void);
	
	//processor type
	virtual int getProcessorType (void) const { return NoseInParkingProcessor; }
	//processor name
	virtual const char *getProcessorName (void) const { return m_strTypeName; }
	//processor i/o	

	virtual LandfieldProcessor * GetCopy();

	virtual bool HasProperty(ProcessorProperty propType)const;
	
	void GetParkingSpot(std::vector<ParkingSpot>& spots);

	void GetParkingPath(ARCPath3& closeStretchSide, ARCPath3& farToStretchSide)const;

	StretchProc * GetLinkProc();

public:
	const static CString m_strTypeName; // "NoseInParking"	

};


#endif