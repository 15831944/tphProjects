// ScaleProcessorsCommand.cpp: implementation of the ScaleProcessorsCommand class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "termplan.h"
#include "Processor2.h"
#include "TermPlanDoc.h"
#include "ScaleProcessorsCommand.h"

#include "ProcessorCommandUtil.h" 

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
const double SCALE_SENSITIVITY = 0.03;

ScaleProcessorsCommand::ScaleProcessorsCommand(CObjectDisplayList& proclist, CTermPlanDoc* pDoc, double dx, double dy, double dz) :
	m_dx(dx), m_dy(dy), m_dz(dz), m_pDoc(pDoc)
{
	ASSERT(m_pDoc!=NULL);
	FillProcIDVector(m_procidlist, proclist);
}

ScaleProcessorsCommand::~ScaleProcessorsCommand()
{

}

void ScaleProcessorsCommand::OnDo()
{	
	std::vector<CProcessor2*> proclist;
	FillProc2Vector(proclist, m_procidlist, m_pDoc);
	for(std::vector<CProcessor2*>::iterator it=proclist.begin(); it!=proclist.end(); ++it) {
		(*it)->SetScale((*it)->GetScale()+ARCVector3(m_dx,m_dy,m_dz));
	}
}

void ScaleProcessorsCommand::OnUndo()
{
	std::vector<CProcessor2*> proclist;
	FillProc2Vector(proclist, m_procidlist, m_pDoc);
	for(std::vector<CProcessor2*>::iterator it=proclist.begin(); it!=proclist.end(); ++it) {
		(*it)->SetScale((*it)->GetScale()+ARCVector3(-m_dx,-m_dy,-m_dz));
	}
}

CString ScaleProcessorsCommand::GetCommandString()
{
	CString sRet;
	sRet.Format("SCALE [%.2f] > ", m_dx);
	CString sProc;
	for(std::vector<UINT>::iterator it=m_procidlist.begin(); it!=m_procidlist.end(); ++it) {
		sProc.Format("%d ",(*it));
		sRet+=sProc;
	}

	return sRet;
}