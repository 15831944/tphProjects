#ifndef __STRETCH_RENDERERE_
#define __STRETCH_RENDERERE_


#include "../Engine/StretchProc.h"
#include "./OglTextureResource.h"
#include "./ProcRenderer.h"
#include "./SelectionHandler.h"
#include "glRender/glTextureResource.h"
#include "glRender/glShapeResource.h"
#include "glrender/REMesh.h"
#pragma once

class CTexture;
class CProcessor2;



class StretchRenderer: public ProcessorRenderer
	{
	public:
		StretchRenderer(CProcessor2* pProc2);
		virtual ~StretchRenderer(void);
		
		void GenerateDispList(C3DView * pView);
		

		virtual void Render(C3DView * pView);
		virtual void RenderSelect(C3DView * pView,int selidx);
		
		virtual void DoSync();
		virtual bool Selectable(){ return true; }
		
	protected:			
		GLuint m_ndisplist;		
		GLuint m_nboundList;
		CTexture * m_pTexture;


		ref_ptr<Renderer::Mesh> m_pMesh;
	};


#endif