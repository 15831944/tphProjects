// MSVImpl.cpp: implementation of the CMSVImpl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "termplan.h"
#include "MSVImpl.h"
#include "TermPlanDoc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMSVImpl::CMSVImpl()
{
	m_pTermPlanDoc = NULL;
}

CMSVImpl::~CMSVImpl()
{

}

void CMSVImpl::AddItem(CTVNode *pNode, CTVNode *pAfterNode)
{

}

void CMSVImpl::SetTermPlanDoc(CTermPlanDoc* pDoc)
{
	ASSERT(pDoc != NULL);
	m_pTermPlanDoc = pDoc;
}