#ifndef __INTERSECTION_PROCESSOR_H
#define __INTERSECTION_PROCESSOR_H
#include "./LandfieldProcessor.h"
#include <vector>
class StretchProc;


#pragma once

class ProcessorList;
class IntersectionProc : public LandfieldProcessor
{
public:
	IntersectionProc(void);
	virtual ~IntersectionProc(void);

	//processor type
	virtual int getProcessorType (void) const { return IntersectionProcessor; }
	//processor name
	virtual const char *getProcessorName (void) const { return m_strTypeName; }
	//processor i/o
	virtual int readSpecialField(ArctermFile& procFile);
	virtual int writeSpecialField(ArctermFile& procFile) const;

	virtual bool HasProperty(ProcessorProperty propType)const;
public:
	const static CString m_strTypeName; // "Intersection"
	
};

#endif