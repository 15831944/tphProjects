// ProbDistNode.cpp: implementation of the CProbDistNode class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "termplan.h"
#include "ProbDistNode.h"

#include "AllDistributions.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProbDistNode::CProbDistNode(CProbDist* pDist)
{
	m_pDist = pDist;
	switch(pDist->GetType()) {
	case PD_NORMAL:
		m_strName = PD_NAMES[PD_NORMAL];
		break;
	case PD_UNIFORM:
		m_strName = PD_NAMES[PD_UNIFORM];;
		break;
	case PD_CONSTANT:
		m_strName = PD_NAMES[PD_CONSTANT];
		break;
	case PD_BETA:
		m_strName = PD_NAMES[PD_BETA];
		break;
	case PD_EXPONENTIAL:
		m_strName = PD_NAMES[PD_EXPONENTIAL];
		break;
	default:
		m_strName = _T("Unknown");
		break;
	}

	m_nIDResource = IDR_CTX_PROBDIST;

}

CProbDistNode::CProbDistNode()
{
}

CProbDistNode::~CProbDistNode()
{
	delete m_pDist;
}

IMPLEMENT_SERIAL(CProbDistNode, CTVNode, 1)

