#ifndef _CLANDPROCESSOR2_DEF
#define _CLANDPROCESSOR2_DEF

#pragma once
#include "processor2.h"
class ProcessorRenderer;

class CLandProcessor2 :
	public CProcessor2
{
public:
	CLandProcessor2(void);
	virtual ~CLandProcessor2(void);

	
	virtual void DrawOGL(C3DView * _pView ,double dAlt, BOOL* pbDO, UINT nDrawMask);	
	virtual void DrawSelectOGL(CTermPlanDoc* pDoc, double* dAlt);
	
	virtual CProcessor2 * GetCopy();
	

	
};


#endif