// RotateProcessorsCommand.cpp: implementation of the RotateProcessorsCommand class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "termplan.h"
#include "TermPlanDoc.h"
#include "RotateProcessorsCommand.h"

#include "ProcessorCommandUtil.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RotateProcessorsCommand::RotateProcessorsCommand(CObjectDisplayList& proclist, CTermPlanDoc* pDoc, double dr) :
	m_dr(dr), m_pDoc(pDoc)
{
	ASSERT(m_pDoc!=NULL);
	FillProcIDVector(m_procidlist, proclist);
}

RotateProcessorsCommand::~RotateProcessorsCommand()
{
}

void RotateProcessorsCommand::OnDo()
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
		//now rotate CProcessor2::m_vLocation about point
		const ARCVector3 loc3D = pProc2->GetLocation();
		ARCVector2 loc2D, point2D;
		loc2D = loc3D;
		point2D = point;
		loc2D.Rotate(-m_dr, point2D);
		pProc2->SetLocation(ARCVector3(loc2D[VX],loc2D[VY],loc3D[VZ]));
		pProc2->OffsetProperties(loc2D[VX]-loc3D[VX], loc2D[VY]-loc3D[VY]);
		pProc2->RotateProperties(-m_dr, pProc2->GetLocation());

		if(pProc2->GetProcessor() && pProc2->GetProcessor()->getProcessorType() == IntegratedStationProc) {
			m_pDoc->AdjustStationLinkedRailWay(((IntegratedStation*)pProc2->GetProcessor()), false);
		}
	}
}

void RotateProcessorsCommand::OnUndo()
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
		//now rotate CProcessor2::m_vLocation about point
		const ARCVector3 loc3D = pProc2->GetLocation();
		ARCVector2 loc2D, point2D;
		loc2D = loc3D;
		point2D = point;
		loc2D.Rotate(m_dr, point2D);
		pProc2->SetLocation(ARCVector3(loc2D[VX],loc2D[VY],loc3D[VZ]));
		pProc2->OffsetProperties(loc2D[VX]-loc3D[VX], loc2D[VY]-loc3D[VY]);
		pProc2->RotateProperties(m_dr, pProc2->GetLocation());

		if(pProc2->GetProcessor() && pProc2->GetProcessor()->getProcessorType() == IntegratedStationProc) {
			m_pDoc->AdjustStationLinkedRailWay(((IntegratedStation*)pProc2->GetProcessor()), false);
		}
	}
}

CString RotateProcessorsCommand::GetCommandString()
{
	CString sRet;
	sRet.Format("ROTATE [%.2f] > ", m_dr);
	CString sProc;
	for(std::vector<UINT>::iterator it=m_procidlist.begin(); it!=m_procidlist.end(); ++it) {
		sProc.Format("%d ",(*it));
		sRet+=sProc;
	}

	return sRet;
}