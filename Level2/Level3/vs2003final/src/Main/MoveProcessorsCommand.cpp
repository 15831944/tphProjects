// MoveProcessorsCommand.cpp: implementation of the MoveProcessorsCommand class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "termplan.h"
#include "Processor2.h"
#include "TermPlanDoc.h"
#include "MoveProcessorsCommand.h"

#include "ProcessorCommandUtil.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MoveProcessorsCommand::MoveProcessorsCommand(CObjectDisplayList& proclist, CTermPlanDoc* pDoc, double dx, double dy) :
	m_dx(dx), m_dy(dy), m_pDoc(pDoc)
{
	ASSERT(m_pDoc!=NULL);
	FillProcIDVector(m_procidlist, proclist);
}

MoveProcessorsCommand::~MoveProcessorsCommand()
{
}

void MoveProcessorsCommand::OnDo()
{
	std::vector<CProcessor2*> proclist;
	FillProc2Vector(proclist, m_procidlist, m_pDoc);

	for(std::vector<CProcessor2*>::iterator it=proclist.begin(); it!=proclist.end(); ++it) 
	{
		(*it)->SetLocation((*it)->GetLocation()+ARCVector3(m_dx,m_dy,0.0));
		(*it)->OffsetProperties(m_dx, m_dy);

		if( (*it)->GetProcessor() && (*it)->GetProcessor()->getProcessorType() == IntegratedStationProc )
			m_pDoc->AdjustStationLinkedRailWay( (IntegratedStation*)(*it)->GetProcessor() , false);
	}
}

void MoveProcessorsCommand::OnUndo()
{
	std::vector<CProcessor2*> proclist;
	FillProc2Vector(proclist, m_procidlist, m_pDoc);
	for(std::vector<CProcessor2*>::iterator it=proclist.begin(); it!=proclist.end(); ++it)
	{
		(*it)->SetLocation((*it)->GetLocation()+ARCVector3(-m_dx,-m_dy,0.0));
		(*it)->OffsetProperties(-m_dx, -m_dy);

		if( (*it)->GetProcessor() && (*it)->GetProcessor()->getProcessorType() == IntegratedStationProc )
			m_pDoc->AdjustStationLinkedRailWay( (IntegratedStation*)(*it)->GetProcessor() , false);
	}
}

CString MoveProcessorsCommand::GetCommandString()
{
	CString sRet;
	sRet.Format("MOVE [%.2f, %.2f] > ", m_dx, m_dy);
	CString sProc;
	for(std::vector<UINT>::iterator it=m_procidlist.begin(); it!=m_procidlist.end(); ++it) {
		sProc.Format("%d ",(*it));
		sRet+=sProc;
	}

	return sRet;
}