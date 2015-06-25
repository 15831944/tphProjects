#ifndef __LANDSIDE_ND_VIEW
#define __LANDSIDE_ND_VIEW

#include "./ProcRenderer.h"
#include <vector>
#include "./SelectionHandler.h"
#pragma once





	
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
