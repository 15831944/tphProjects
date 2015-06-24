#ifndef __STRETCHPROCESSOR_H
#define __STRETCHPROCESSOR_H
#include "../Common/ARCPath.h"
#include "landfieldprocessor.h"
#include <Common/ShapeGenerator.h>

#pragma once

class LaneProc : public LandfieldProcessor
{
public:


};


class StretchProc :
	public LandfieldProcessor
{
public:
	StretchProc(void);
	virtual ~StretchProc(void);
	// do offset for all the position related data. be careful with the derived class.
	//virtual void DoOffset( DistanceUnit _xOffset, DistanceUnit _yOffset ){}

	//virtual void Rotate( DistanceUnit _degree ){}

	//virtual void Mirror( const Path* _p ){}

	//processor type
	virtual int getProcessorType (void) const { return StretchProcessor; }
	//processor name
	virtual const char *getProcessorName (void) const { return m_strTypeName; }
	//processor i/o	
	
	virtual LandfieldProcessor * GetCopy();

	virtual bool HasProperty(ProcessorProperty propType)const;

	ShapeGenerator::WidthPipePath getStretchPipePath();

	ARCPath3 getLanePath(int n);

	virtual void DoSync();

public:
	const static CString m_strTypeName; // "Stretch"

	const static double lanesidewidth;
	const static double lanethickness;
	const static int interpoltCnt;

	ShapeGenerator::WidthPipePath m_widthPipePath;
};

#endif
