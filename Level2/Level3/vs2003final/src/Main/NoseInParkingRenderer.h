#ifndef NOSEINPARKING_RENDERER_DEF
#define NOSEINPARKING_RENDERER_DEF



#pragma once
#include "procrenderer.h"


class OglTexture;
class CProcessor2;


class NoseInParkingRenderer :
	public ProcessorRenderer
{
public:
	NoseInParkingRenderer(CProcessor2 * pProc2);
	virtual ~NoseInParkingRenderer(void);
	void GenerateDispList(C3DView * pView);


	virtual void Render(C3DView * pView);
	virtual void RenderSelect(C3DView * pView,int selidx);	
	virtual bool Selectable(){ return true; }

protected:			
	GLuint m_ndisplist;		
	OglTexture * m_pTexture;
};
#endif