// CopyProcessorsCommand.cpp: implementation of the CopyProcessorsCommand class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "termplan.h"
#include "TermPlanDoc.h"
#include "CopyProcessorsCommand.h"

#include "ProcessorCommandUtil.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CopyProcessorsCommand::CopyProcessorsCommand(std::vector<CProcessor2*>& proclist, CTermPlanDoc* pDoc) :
	m_pDoc(pDoc)
{
	ASSERT(m_pDoc!=NULL);
	FillProcIDVector(m_procidsToCopy, proclist);
}

CopyProcessorsCommand::~CopyProcessorsCommand()
{
}

void CopyProcessorsCommand::Do()
{
	std::vector<CProcessor2*> procsToCopy;
	FillProc2Vector(procsToCopy, m_procidsToCopy, m_pDoc);
	m_pDoc->CopyProcessors(procsToCopy);
	FillProcIDVector(m_procidsToDelete, m_pDoc->m_vSelectedProcessors);
}

void CopyProcessorsCommand::Undo()
{
	std::vector<CProcessor2*> procsToCopy;
	std::vector<CProcessor2*> procsToDelete;
	FillProc2Vector(procsToCopy, m_procidsToCopy, m_pDoc);
	FillProc2Vector(procsToDelete, m_procidsToDelete, m_pDoc);
	//need to delete processors m_procsToDelete and reselect original processors (m_procsToCopy)
	m_pDoc->DeleteProcessors(procsToDelete);
	m_pDoc->SelectProcessors(procsToCopy);
}

CString CopyProcessorsCommand::GetCommandString()
{
	CString sRet = "COPY > ";
	CString sProc;
	for(std::vector<ProcessorID>::iterator it=m_procidsToCopy.begin(); it!=m_procidsToCopy.end(); ++it) {
		sProc.Format("%s ",(*it).GetIDString());
		sRet+=sProc;
	}

	return sRet;
}