// DeleteProcessorsCommand.cpp: implementation of the DeleteProcessorsCommand class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "termplan.h"
#include "TermPlanDoc.h"
#include "DeleteProcessorsCommand.h"

#include "ProcessorCommandUtil.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DeleteProcessorsCommand::DeleteProcessorsCommand(std::vector<CProcessor2*>& proclist, CTermPlanDoc* pDoc) :
	m_pDoc(pDoc)
{
	ASSERT(m_pDoc!=NULL);
	FillProcIDVector(m_procidsToDelete, proclist);
}

DeleteProcessorsCommand::~DeleteProcessorsCommand()
{
}

void DeleteProcessorsCommand::Do()
{
	std::vector<CProcessor2*> procsToDelete;
	FillProc2Vector(procsToDelete, m_procidsToDelete, m_pDoc);
	m_pDoc->DeleteProcessors(procsToDelete);
}

void DeleteProcessorsCommand::Undo()
{
	//restore processors??
}

CString DeleteProcessorsCommand::GetCommandString()
{
	CString sRet = "COPY > ";
	CString sProc;
	for(std::vector<ProcessorID>::iterator it=m_procidsToDelete.begin(); it!=m_procidsToDelete.end(); ++it) {
		sProc.Format("%s ",(*it).GetIDString());
		sRet+=sProc;
	}

	return sRet;
}