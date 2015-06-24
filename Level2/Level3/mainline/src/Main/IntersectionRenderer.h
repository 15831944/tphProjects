#ifndef __INTERSECTION_RENDERER_
#define __INTERSECTION_RENDERER_
#include "./ProcRenderer.h"

#pragma once
class OglTexture;	
class IntersectionProc;	
class CProcessor2;

class IntersectionRenderer : public ProcessorRenderer
{
public:
	IntersectionRenderer(CProcessor2* pProc2);
	virtual ~IntersectionRenderer(void);

protected:
	virtual void Render(C3DView* pView);
	void GenDisplaylist(C3DView* pView);
	virtual void RenderSelect(C3DView * pView,int selidx);
	//virtual void DoSync();
	virtual bool Selectable(){ return true; }

	OglTexture * m_pTexture;
	OglTexture * m_pZebraTexture;

	GLuint m_isurdisplist;
	GLuint m_iboundinglist;
	GLuint m_iZebralist;

	static GLdouble m_dthickness;
};

#endif