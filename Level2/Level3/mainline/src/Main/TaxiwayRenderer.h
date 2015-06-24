#pragma once
#include "procrenderer.h"

class TaxiwayRenderer :
	public ProcessorRenderer
{
public:
	TaxiwayRenderer(CProcessor2 * pProc2);
	virtual ~TaxiwayRenderer(void);
	
	virtual void Render(C3DView* pView);
	virtual void RenderSelect(C3DView * pView,int selidx) ;
	void RenderSegmentDir(C3DView * pView)const;
	void RenderMarkLine(C3DView  *pView);
};
