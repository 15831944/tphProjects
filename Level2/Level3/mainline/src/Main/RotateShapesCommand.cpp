// RotateShapesCommand.cpp: implementation of the RotateShapesCommand class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "termplan.h"
#include "TermPlanDoc.h"
#include "RotateShapesCommand.h"

#include "ProcessorCommandUtil.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RotateShapesCommand::RotateShapesCommand(CObjectDisplayList& proclist, CTermPlanDoc* pDoc, double dr) :
	m_dr(dr), m_pDoc(pDoc)
{
	ASSERT(m_pDoc!=NULL);
	FillProcIDVector(m_procidlist, proclist);
}

RotateShapesCommand::~RotateShapesCommand()
{
}


void RotateShapesCommand::OnDo()
{	
	std::vector<CProcessor2*> proclist;
	FillProc2Vector(proclist, m_procidlist, m_pDoc);

	int c=proclist.size();
	if(c == 0)
		return;
	
	const ARCVector3& point = proclist[c-1]->GetLocation();
	
	for(std::vector<CProcessor2*>::iterator it=proclist.begin(); it!=proclist.end(); ++it) {
		CProcessor2* pProc2 = *it;
		pProc2->SetRotation(pProc2->GetRotation()+m_dr);
	}
}

void RotateShapesCommand::OnUndo()
{
	std::vector<CProcessor2*> proclist;
	FillProc2Vector(proclist, m_procidlist, m_pDoc);

	int c=proclist.size();
	if(c == 0)
		return;
	
	const ARCVector3& point = proclist[c-1]->GetLocation();
	
	for(std::vector<CProcessor2*>::iterator it=proclist.begin(); it!=proclist.end(); ++it) {
		CProcessor2* pProc2 = *it;
		pProc2->SetRotation(pProc2->GetRotation()-m_dr);
	}
}

CString RotateShapesCommand::GetCommandString()
{
	CString sRet;
	sRet.Format("ROTATE SHAPES [%.2f] > ", m_dr);
	CString sProc;
	for(std::vector<UINT>::iterator it=m_procidlist.begin(); it!=m_procidlist.end(); ++it) {
		sProc.Format("%d ",(*it));
		sRet+=sProc;
	}

	return sRet;
}