// TVNode.cpp: implementation of the CTVNode class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TermPlan.h"

//#include "common.h"

#include "TVNode.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTVNode::CTVNode()
{
	m_hItem = NULL;
	m_nIDResource = IDR_CTX_DEFAULT;
	m_eState = collapsed;
	m_sDescription = "";
	m_iImageIdx = ID_NODEIMG_DEFAULT;
	m_vItem.clear();
}

CTVNode::CTVNode(CTVNode& node) : CNode(node)
{
	m_hItem = node.m_hItem;
	m_nIDResource = node.m_nIDResource;
	m_eState = node.m_eState;
	m_sDescription = node.m_sDescription;
	m_iImageIdx = node.m_iImageIdx;
	m_dwData = node.m_dwData;
	m_vItem.clear();
}

CTVNode::CTVNode(CString strName) : CNode(strName)
{
	m_hItem = NULL;
	m_nIDResource = IDR_CTX_DEFAULT;
	m_eState = collapsed;
	m_sDescription = "";
	m_iImageIdx = ID_NODEIMG_DEFAULT;
	m_dwData = NULL;
	m_vItem.clear();
}

CTVNode::CTVNode(CString strName, UINT nID) : CNode(strName)
{
	m_hItem = NULL;
	m_nIDResource = nID;
	m_eState = collapsed;
	m_sDescription = "";
	m_iImageIdx = ID_NODEIMG_DEFAULT;
	m_dwData = NULL;
	m_vItem.clear();
}

CTVNode::CTVNode(CString strName, CString sDesc) : CNode(strName)
{
	m_hItem = NULL;
	m_nIDResource = IDR_CTX_DEFAULT;
	m_eState = collapsed;
	m_sDescription = sDesc;
	m_iImageIdx = ID_NODEIMG_DEFAULT;
	m_dwData = NULL;
	m_vItem.clear();
}

CTVNode::CTVNode(CString strName, CString sDesc, UINT nID) : CNode(strName)
{
	m_hItem = NULL;
	m_nIDResource = nID;
	m_eState = collapsed;
	m_sDescription = sDesc;
	m_iImageIdx = ID_NODEIMG_DEFAULT;
	m_dwData = NULL;
	m_vItem.clear();
}

CTVNode::~CTVNode()
{

}

IMPLEMENT_SERIAL( CTVNode, CNode, 1 )

void CTVNode::Serialize(CArchive& ar)
{
	CNode::Serialize(ar);
	if(ar.IsStoring())
	{
		ar << m_eState;
		ar << m_nIDResource;
		ar << m_sDescription;
		ar << m_iImageIdx;
	}
	else
	{
		ar >> m_eState;
		ar >> m_nIDResource;
		ar >> m_sDescription;
		ar >> m_iImageIdx;
	}
}

CArchive& operator>>(CArchive& ar, CTVNode::EState& eState)
{
	int val;
	ar.Read(&val, sizeof(val) );
	switch(val)
	{
	case 1:
		eState = CTVNode::collapsed;
		break;
	case 2:
		eState = CTVNode::expanded;
		break;
	}
	return ar;
}

CArchive& operator<<(CArchive& ar, CTVNode::EState& eState)
{
	int val = 0;
	switch(eState)
	{
	case CTVNode::collapsed:
		val = 1;
		break;
	case CTVNode::expanded:
		val = 2;
		break;
	}
	ar.Write(&val, sizeof(val) );
	return ar;
}

BOOL CTVNode::RemoveChild(CTVNode* pNode)
{
	return CNode::RemoveChild(pNode);
}

BOOL CTVNode::RemoveChild(int i)
{
	if(i<0 || i>=static_cast<int>(m_vChildren.size()))
		return FALSE;
	CTVNode* pChildToRemove = (CTVNode*)m_vChildren[i];
	if (pChildToRemove->m_hItem)
	{
		m_vItem.push_back(pChildToRemove->m_hItem);
	}
	m_vChildren.erase(m_vChildren.begin() + i);
	//pChildToRemove->m_pParent = NULL;
	delete pChildToRemove;
	pChildToRemove = NULL;
	return TRUE;
}

void CTVNode::clearNode()
{
	m_vItem.clear();
}