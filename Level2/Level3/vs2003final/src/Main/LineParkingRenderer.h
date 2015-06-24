#ifndef __LINEPARKING_RENDERER_DEF
#define __LINEPARKING_RENDERER_DEF


#pragma once
#include "procrenderer.h"

class OglTexture;
class CProcessor2;

class LineParkingRenderer :
	public ProcessorRenderer
{
public:
	LineParkingRenderer(CProcessor2 * pProc2);
	virtual ~LineParkingRenderer(void);

	void GenerateDispList(C3DView * pView);


	virtual void Render(C3DView * pView);
	virtual void RenderSelect(C3DView * pView,int selidx);	
	virtual bool Selectable(){ return true; }

protected:			
	GLuint m_ndisplist;		
	OglTexture * m_pTexture;

};

#endif