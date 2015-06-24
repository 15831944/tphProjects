#pragma once
#include "procrenderer.h"

class SectorRenderer :
	public ProcessorRenderer
{
public:
	SectorRenderer(CProcessor2 * pProc2);

	virtual void Render(C3DView* pView);
	virtual void RenderSelect(C3DView * pView,int selidx) ;
	void RenderShadow(C3DView* pView);


	virtual ~SectorRenderer(void);
};
