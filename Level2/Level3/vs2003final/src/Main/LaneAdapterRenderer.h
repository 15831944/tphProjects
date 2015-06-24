#ifndef __LANEADAPTER_RENDERER_
#define __LANEADAPTER_RENDERER_
#include "./ProcRenderer.h"
#include "glrender/REMesh.h"

#pragma once
class OglTexture;
class LaneAdapterProc;
class CProcessor2;

class LaneAdapterRenderer :
	public ProcessorRenderer
{
public:
	LaneAdapterRenderer(CProcessor2 * pProc);
	virtual ~LaneAdapterRenderer(void);
protected:
	virtual void Render(C3DView* pView);
	void GenDisplaylist(C3DView* pView);
	virtual void RenderSelect(C3DView * pView,int selidx);
	
	virtual bool Selectable(){ return true; }

	OglTexture * m_pTexture;

	GLuint m_isurdisplist;
	GLuint m_iboundinglist;

	static GLdouble m_dthickness;

	ref_ptr<Renderer::Mesh> m_pMesh;

};

#endif