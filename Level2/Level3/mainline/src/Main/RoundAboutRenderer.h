#ifndef __ROUNDABOUT_RENDERER
#define __ROUNDABOUT_RENDERER
#pragma once
#include "procrenderer.h"
class OglTexture;	
class RoundAboutProc;	
class CProcessor2;

class RoundAboutRenderer :
	public ProcessorRenderer
{
public:
	RoundAboutRenderer(CProcessor2* pProc);
	virtual ~RoundAboutRenderer(void);

	virtual void Render(C3DView* pView);
	void GenDisplaylist(C3DView* pView);
	virtual void RenderSelect(C3DView * pView,int selidx);
	
	virtual bool Selectable(){ return true; }

	OglTexture * m_pTexture;

	static GLdouble m_dthickness;

	GLuint m_iboundinglist ;
	GLuint m_isurdisplist ;
};

#endif