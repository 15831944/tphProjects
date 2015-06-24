// RotateALTObjectCommand.cpp: implementation of the RotateALTObjectCommand class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "termplan.h"
#include "TermPlanDoc.h"
#include "..\InputAirside\ALTObject.h"
#include "RotateALTObjectCommand.h"

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

RotateALTObjectCommand::RotateALTObjectCommand(std::vector< ref_ptr<ALTObject3D> >& altobjectlist, CTermPlanDoc* pDoc, double dr) :
	m_dr(dr), m_pDoc(pDoc)
{
	ASSERT(m_pDoc!=NULL);
	FillALTObjectIDVector(m_altobjectidlist, altobjectlist);
}

RotateALTObjectCommand::~RotateALTObjectCommand()
{
}

void RotateALTObjectCommand::OnDo()
{
	std::vector< ref_ptr<ALTObject3D> > altobjectlist;
	FillALTObjectVector(altobjectlist, m_altobjectidlist);

	int c=altobjectlist.size();
	if(c == 0)
		return;
	//const ARCPoint3& point = altobjectlist[c-1]->GetLocation();
	
	for(std::vector< ref_ptr<ALTObject3D> >::iterator it=altobjectlist.begin(); it!=altobjectlist.end(); ++it) {
		//const ARCVector3 loc3D = (*it)->GetLocation();
		//ARCPoint2 loc2D, point2D;
		//loc2D = loc3D;
		//point2D = point;
		//loc2D.Rotate(-m_dr, point2D);
		//(*it)->OnMove(loc2D[VX]-loc3D[VX], loc2D[VY]-loc3D[VY]);
		(*it)->OnRotate(m_dr);
		(*it)->FlushChange();	
	}
	m_pDoc->UpdateAllViews(NULL,VM_UPDATA_ALTOBJECTS,(CObject*)(&m_altobjectidlist));	
}

void RotateALTObjectCommand::OnUndo()
{
	std::vector< ref_ptr<ALTObject3D> > altobjectlist;
	FillALTObjectVector(altobjectlist, m_altobjectidlist);

	int c=altobjectlist.size();
	if(c == 0)
		return;
	//const ARCPoint3& point = altobjectlist[c-1]->GetLocation();

	for(std::vector< ref_ptr<ALTObject3D> >::iterator it=altobjectlist.begin(); it!=altobjectlist.end(); ++it) {
		//const ARCVector3 loc3D = (*it)->GetLocation();
		//ARCPoint2 loc2D, point2D;
		//loc2D = loc3D;
		//point2D = point;
		//loc2D.Rotate(m_dr, point2D);
		//(*it)->OnMove(loc2D[VX]-loc3D[VX], loc2D[VY]-loc3D[VY]);
		(*it)->OnRotate(-m_dr);
		(*it)->FlushChange();	
	}
	m_pDoc->UpdateAllViews(NULL,VM_UPDATA_ALTOBJECTS,(CObject*)(&m_altobjectidlist));	
}

CString RotateALTObjectCommand::GetCommandString()
{
	CString strRet;
	strRet.Format("ROTATE [%.2f] > ", m_dr);
	CString strALTObject;
	for(std::vector<UINT>::iterator it=m_altobjectidlist.begin(); it!=m_altobjectidlist.end(); ++it) {
		strALTObject.Format("%d ",(*it));
		strRet+=strALTObject;
	}

	return strRet;
}