#ifndef __HOLDPROCESSOR_RENDERER_H
#define __HOLDPROCESSOR_RENDERER_H

#pragma once
#include "procrenderer.h"

class HoldRenderer :
	public ProcessorRenderer
{
public:
	HoldRenderer(CProcessor2 * pProc2);

	virtual void Render(C3DView* pView);
	virtual void RenderSelect(C3DView * pView,int selidx) ;
	void RenderShadow(C3DView* pView);

	
	virtual ~HoldRenderer(void);
};


#endif
