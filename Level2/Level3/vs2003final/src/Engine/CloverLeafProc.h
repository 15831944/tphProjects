#ifndef CLOVERLEAFPROCESSOR_DEF
#define CLOVERLEAFPROCESSOR_DEF

#pragma once
#include "landfieldprocessor.h"

/*
The clover leaf have two control path ,
the round one use LandfieldProcessor::m_path,the stretch one 
*/

class CloverLeafProc :
	public LandfieldProcessor
{
public:
	CloverLeafProc(void);
	virtual ~CloverLeafProc(void);
	//processor type
	virtual int getProcessorType (void) const { return CloverLeafProcessor; }
	//processor name
	virtual const char *getProcessorName (void) const { return m_strTypeName; }

	void GeneratePath();
	
	
	virtual bool HasProperty(ProcessorProperty propType)const;

protected:

	ARCPath3 m_roundpath;
public:
	const static CString m_strTypeName; // "CloverLeaf"
	//
};


#endif