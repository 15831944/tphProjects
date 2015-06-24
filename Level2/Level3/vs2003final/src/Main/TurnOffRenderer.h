#ifndef __TURNOFFRENDERER_H
#define __TURNOFFRENDERER_H
#pragma once
#include "procrenderer.h"
class OglTexture;	
class TurnOffProc;	
class CProcessor2;
class CTexture;

class TurnOffRenderer :
	public ProcessorRenderer
{
public:
	TurnOffRenderer(CProcessor2* pProc2);
	virtual ~TurnOffRenderer(void);

	virtual void Render(C3DView* pView);
	void GenDisplaylist(C3DView* pView);
	virtual void RenderSelect(C3DView * pView,int selidx);

	virtual bool Selectable(){ return true; }

	CTexture * m_pTexture;

	static GLdouble m_dthickness;

	GLuint m_iboundinglist ;
	GLuint m_isurdisplist ;

};
#endif