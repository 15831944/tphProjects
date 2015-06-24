#ifndef __OVERPASSPROC_H
#define __OVERPASSPROC_H
#pragma once
#include "landfieldprocessor.h"

class OverPassProc :
	public LandfieldProcessor
{
public:
	OverPassProc(void);
	virtual ~OverPassProc(void);
	
	virtual void DoOffset( DistanceUnit _xOffset, DistanceUnit _yOffset ){}

	virtual void Rotate( DistanceUnit _degree ){}

	virtual void Mirror( const Path* _p ){}

	//processor type
	virtual int getProcessorType (void) const { return OverPassProcessor; }
	//processor name
	virtual const char *getProcessorName (void) const { return m_strTypeName; }
	//processor i/o	
	virtual int readSpecialField(ArctermFile& procFile);
	virtual int writeSpecialField(ArctermFile& procFile) const;

	virtual bool HasProperty(ProcessorProperty propType);
public:
	const static CString m_strTypeName; // "RoundAbout"

};
#endif