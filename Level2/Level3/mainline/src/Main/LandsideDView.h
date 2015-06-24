#ifndef __LANDSIDE_ND_VIEW
#define __LANDSIDE_ND_VIEW

#include "./ProcRenderer.h"
#include <vector>
#include "./SelectionHandler.h"
#pragma once

#ifdef _DEBUG
static char g_pszErrorGL[256];

#define CHECK_GL_ERRORS(msg)	\
{ \
	GLenum errorGL; \
	while((errorGL = glGetError()) != GL_NO_ERROR) { \
	sprintf(g_pszErrorGL, "OPENGL ERRORS in %s [%s]\n", (msg), gluErrorString(errorGL)); \
	::OutputDebugString(g_pszErrorGL); \
	} \
}

#else // _DEBUG

#define CHECK_GL_ERRORS(msg)	__noop

#endif


	
	class LandsideDocument;
	class C3DView;
	class LandsideDView
	{
	public:
		LandsideDView(LandsideDocument  * pDoc); 
		virtual ~LandsideDView(void);

	public:
		void Render(C3DView * pView);
		

		void RenderProcessors(bool bSelect);
		LandsideDocument * GetDocument();


		bool m_bInEditPointMode;
		int m_nCtrlPointsId;
		bool m_bMovingCtrlPoint;
		void ChangeToSelectMode(ProcessorRenderer::SelectMode selmode);	


	protected:
		LandsideDocument * m_pDoc;

	
		
		
		
	};



#endif
