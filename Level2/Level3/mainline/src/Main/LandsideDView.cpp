#include "StdAfx.h"
#include ".\landsidedview.h"
#include "placement.h"
#include "./LandsideDocument.h"
#include "./StretchRenderer.h"
#include "../Engine/proclist.h"
#include "./TermPlanDoc.h"
#include "./SelectionHandler.h"
#include "./TermPlan.h"
#include "./ShapesManager.h"
#include "./3dModel.h"
#include "../Common/ref_ptr.h"
#include "./3DSFileLoader.h"


LandsideDView::LandsideDView(LandsideDocument* pDoc):m_pDoc(pDoc)
{	
	m_bInEditPointMode = false;
	m_bMovingCtrlPoint = false;
	m_nCtrlPointsId = 0;
}

LandsideDView::~LandsideDView(void)
{
}


void LandsideDView::Render(C3DView  *pView)
{
	RenderProcessors(false);
}
void LandsideDView::RenderProcessors(bool bSelect)
{	
	
	CPROCESSOR2LIST & proc2List = GetDocument()->GetPlacementPtr()->m_vDefined;
	for( size_t i =0;i<proc2List.size();i++)
	{
		proc2List[i]->DrawOGL(GetDocument()->GetTermPlanDoc()->Get3DView(),0, GetDocument()->GetTermPlanDoc()->m_displayoverrides.m_pbProcDisplayOverrides,DRAWALL);
	}	
	
	CString msg;
	CHECK_GL_ERRORS(msg);
	
}


LandsideDocument * LandsideDView::GetDocument()
{
	return m_pDoc;
}

void LandsideDView::ChangeToSelectMode(ProcessorRenderer::SelectMode selmode)
{
	if(selmode == ProcessorRenderer::SelectMain) 
	{
		m_nCtrlPointsId = -1;
		m_bInEditPointMode = false;
		CPROCESSOR2LIST & proc2List = GetDocument()->GetPlacementPtr()->m_vDefined;
		for(int i=0;i<(int)proc2List.size();i++)
		{
			proc2List[i]->GetRenderer()->SetSelectMode(ProcessorRenderer::SelectMain);
		}
	}
	if(selmode == ProcessorRenderer::SelectCtrlPoints)
	{
		m_bInEditPointMode = true;
		CObjectDisplayList & proc2List = GetDocument()->GetSelectProcessor();
		for(int i =0; i <proc2List.GetCount();i++ )
		{
			CObjectDisplay *pObjDisplay = proc2List.GetItem(i);
			if (pObjDisplay &&pObjDisplay->GetType() == ObjectDisplayType_Processor2)
			{
				CProcessor2 *pProc2 = (CProcessor2 *)pObjDisplay;
				pProc2->GetRenderer()->SetSelectMode(ProcessorRenderer::SelectCtrlPoints);
			}

		}
	}
}