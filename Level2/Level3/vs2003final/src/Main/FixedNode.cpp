// FixedNode.cpp: implementation of the CFixedNode class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TermPlan.h"
#include "FixedNode.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static CString names[] =
{
	_T("Aircraft Categories"), _T("Airports"), _T("Areas"),
	_T("Floors"), _T("Intelligence"), _T("Passenger Types"),
	_T("Portals"), _T("Probability Distributions"), _T("Processor Classes"),
	_T("Reference Points"), _T("Sectors")
};

static long menus[] = 
{
	IDR_CTX_AIRCRAFTCATEGORIES, NULL,
	NULL, IDR_CTX_FLOORS, NULL, NULL,
	NULL, NULL, IDR_CTX_PROCESSORCLASSES,
	NULL, NULL
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFixedNode::CFixedNode()
{
}

CFixedNode::CFixedNode(CFixedNode::FNType type)
: CTVNode(names[type])
{
	m_type = type;
	m_nIDResource = menus[type];
}

CFixedNode::~CFixedNode()
{

}

IMPLEMENT_SERIAL(CFixedNode, CTVNode, 1)

void CFixedNode::Serialize(CArchive& ar)
{
	CTVNode::Serialize(ar);
	if(ar.IsStoring())
	{
		ar << m_type;
	}
	else
	{
		ar >> m_type;
	}
}
