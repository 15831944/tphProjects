// LeafNode.cpp: implementation of the CLeafNode class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TermPlan.h"
#include "LeafNode.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLeafNode::CLeafNode()
{

}

CLeafNode::~CLeafNode()
{

}

IMPLEMENT_SERIAL(CLeafNode, CTVNode, 1)

void CLeafNode::Serialize(CArchive& ar)
{
	CTVNode::Serialize(ar);
}
