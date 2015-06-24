#ifndef __ROUNDABOUT_PROCESSOR
#define __ROUNDABOUT_PROCESSOR
#include "./LandfieldProcessor.h"
#include <string>
#pragma once

class RoundAboutProc : public LandfieldProcessor
{
public:
	RoundAboutProc(void);
	virtual ~RoundAboutProc(void);
	// do offset for all the position related data. be careful with the derived class.
	virtual void DoOffset( DistanceUnit _xOffset, DistanceUnit _yOffset ){}

	virtual void Rotate( DistanceUnit _degree ){}

	virtual void Mirror( const Path* _p ){}

	//processor type
	virtual int getProcessorType (void) const { return RoundAboutProcessor; }
	//processor name
	virtual const char *getProcessorName (void) const { return m_strTypeName; }
	//processor i/o	
	virtual int readSpecialField(ArctermFile& procFile);
	virtual int writeSpecialField(ArctermFile& procFile) const;
	
	virtual bool HasProperty(ProcessorProperty propType)const;

	//get the path of the roundabout
	virtual ARCPath3 GetPath()const;
protected:
	
public:
	const static CString m_strTypeName; // "RoundAbout"
};
#endif