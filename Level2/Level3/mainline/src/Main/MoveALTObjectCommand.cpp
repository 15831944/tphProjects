// MoveALTObjectCommand.cpp: implementation of the MoveALTObjectCommand class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "termplan.h"
#include "..\InputAirside\ALTObject.h"
#include "TermPlanDoc.h"
#include "MoveALTObjectCommand.h"

#include "ALTObjectCommandUtil.h"
#include "ViewMsg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MoveALTObjectCommand::MoveALTObjectCommand(std::vector< ref_ptr<ALTObject3D> >& altobjectlist, CTermPlanDoc* pDoc, double dx, double dy) :
	m_dx(dx), m_dy(dy), m_pDoc(pDoc)
{
	ASSERT(m_pDoc!=NULL);
	FillALTObjectIDVector(m_altobjectidlist, altobjectlist);
}

MoveALTObjectCommand::~MoveALTObjectCommand()
{
}

void MoveALTObjectCommand::OnDo()
{
	std::vector< ref_ptr<ALTObject3D> > altobjectlist;
	FillALTObjectVector(altobjectlist, m_altobjectidlist);
	for(std::vector< ref_ptr<ALTObject3D> >::iterator it=altobjectlist.begin(); it!=altobjectlist.end(); ++it) {
		(*it)->OnMove(m_dx, m_dy);
		(*it)->FlushChange();
	}
	m_pDoc->UpdateAllViews(NULL,VM_UPDATA_ALTOBJECTS,(CObject*)(&m_altobjectidlist));	
}

void MoveALTObjectCommand::OnUndo()
{
	std::vector< ref_ptr<ALTObject3D> > altobjectlist;
	FillALTObjectVector(altobjectlist, m_altobjectidlist);
	for(std::vector< ref_ptr<ALTObject3D> >::iterator it=altobjectlist.begin(); it!=altobjectlist.end(); ++it) {
		(*it)->OnMove(-m_dx, -m_dy);
		(*it)->FlushChange();
	}
	m_pDoc->UpdateAllViews(NULL,VM_UPDATA_ALTOBJECTS,(CObject*)(&m_altobjectidlist));		
}

CString MoveALTObjectCommand::GetCommandString()
{
	CString strRet;
	strRet.Format("MOVE [%.2f, %.2f] > ", m_dx, m_dy);
	CString strALTObject;
	for(std::vector<UINT>::iterator it=m_altobjectidlist.begin(); it!=m_altobjectidlist.end(); ++it) {
		strALTObject.Format("%d ",(*it));
		strRet+=strALTObject;
	}
	return strRet;
}