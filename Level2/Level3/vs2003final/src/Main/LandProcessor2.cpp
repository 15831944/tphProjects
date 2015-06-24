#include "StdAfx.h"
#include ".\landprocessor2.h"
#include "./ProcRenderer.h"
#include "./SelectionHandler.h"
#include "./TermPlanDoc.h"

CLandProcessor2::CLandProcessor2(void)
{
	m_pRenderer = NULL;
}

CLandProcessor2::~CLandProcessor2(void)
{
}


void CLandProcessor2::DrawOGL(C3DView * _pView ,double dAlt, BOOL* pbDO, UINT nDrawMask)
{
	glEnable(GL_LIGHTING);
	GetRenderer()->Render(_pView);	
	glDisable(GL_LIGHTING);
}
void CLandProcessor2::DrawSelectOGL(CTermPlanDoc* pDoc, double* dAlt)
{
		
	GetRenderer()->RenderSelect(pDoc->Get3DView(),m_nSelectName);
}


CProcessor2 *  CLandProcessor2::GetCopy()
{
	CProcessor2*  pNewProc2= new CLandProcessor2;
	//set floor to active floor
	pNewProc2->SetFloor(GetFloor());
	//set location, scale and rotatio
	pNewProc2->SetLocation(GetLocation());
	pNewProc2->SetScale(GetScale());
	pNewProc2->SetRotation(GetRotation());
	//set shape
	pNewProc2->SetShape(GetShape());	
	//set dispproperties
	pNewProc2->m_dispProperties = m_dispProperties;
	return pNewProc2;
	
}