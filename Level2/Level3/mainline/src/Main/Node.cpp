// Node.cpp: implementation of the CNode class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TermPlan.h"
#include "Node.h"
#include <deque>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNode::CNode()
{
	m_strName = _T("");
	m_pParent = NULL;
}

CNode::CNode(CNode& node)
{
	m_strName = node.m_strName;
	m_pParent = NULL;
	for(int i=0; i<static_cast<int>(node.m_vChildren.size()); i++)
	{
		m_vChildren.push_back(node.m_vChildren[i]);
	}
}

CNode::CNode(CString strName)
{
	m_strName = strName;
	m_pParent = NULL;
}

CNode::~CNode()
{
	for(int i=0; i<static_cast<int>(m_vChildren.size()); i++)
	{
		delete m_vChildren[i];
		m_vChildren[i] = NULL;
	}
	m_vChildren.clear();
}

IMPLEMENT_SERIAL( CNode, CObject, 1 )

CNode* CNode::GetChildByName(CString strName)
{
	for(int i=0; i<static_cast<int>(m_vChildren.size()); i++)
	{
		if(m_vChildren[i]->Name().CompareNoCase(strName) == 0)
		{
			return m_vChildren[i];
		}
	}
	return NULL;
}

BOOL CNode::AddChild(CNode* pChild)
{
	m_vChildren.push_back(pChild);
	pChild->m_pParent = this;
	return TRUE;
}

BOOL CNode::AddChildAfter(CNode* pChild, CNode* pAfterNode)
{
	std::vector<CNode*>::iterator it;
	for(it=m_vChildren.begin(); it!=m_vChildren.end(); it++) {
		if(*it == pAfterNode) {
			it++;
			break;
		}
	}
	if(it==m_vChildren.end()) {
		AddChild(pChild);
	}
	else {
		m_vChildren.insert(it, pChild);
		pChild->m_pParent = this;
	}
	return TRUE;
}

BOOL CNode::AddChildBefore(CNode* pChild, CNode* pBeforeNode)
{
	std::vector<CNode*>::iterator it;
	for(it=m_vChildren.begin(); it!=m_vChildren.end(); it++) {
		if(*it == pBeforeNode) {
			break;
		}
	}
	if(it==m_vChildren.end()) {
		AddChild(pChild);
	}
	else {
		m_vChildren.insert(it, pChild);
		pChild->m_pParent = this;
	}
	return TRUE;
}

BOOL CNode::RemoveChild(CNode* pChild)
{
	for(int i=0; i<static_cast<int>(m_vChildren.size()); i++)
	{
		if(m_vChildren[i] == pChild)
		{
			//m_vChildren.erase(m_vChildren.begin() + i);
			//pChild->m_pParent = NULL;
			//return TRUE;
			return RemoveChild(i);
		}
	}
	return FALSE;
}

BOOL CNode::RemoveChild(int i)
{
	if(i<0 || i>=static_cast<int>(m_vChildren.size()))
		return FALSE;
	CNode* pChildToRemove = m_vChildren[i];
	m_vChildren.erase(m_vChildren.begin() + i);
	//pChildToRemove->m_pParent = NULL;
	delete pChildToRemove;
	pChildToRemove = NULL;
	return TRUE;
}

BOOL CNode::DeleteAllChildren()
{
	for(int i=0; i<static_cast<int>(m_vChildren.size()); i++)
	{
		delete m_vChildren[i];
		m_vChildren[i] = NULL;
	}
	m_vChildren.clear();
	return TRUE;
}

void CNode::Serialize(CArchive& ar)
{
	CObject::Serialize(ar);
	if(ar.IsStoring())
	{
		ar << m_strName;
		ar << GetChildCount();
		for(int i=0; i<GetChildCount(); i++)	
		{
			ar << GetChildByIdx(i);
		}
	}
	else
	{
		ar >> m_strName;
		int count;
		ar >> count;
		m_vChildren.reserve(count);
		for(int i=0; i<count; i++)	
		{
			CObject* pNewNode;
			ar >> pNewNode;
			AddChild((CNode*) pNewNode);
		}
	}
}

bool CNode::Exist(const CNode* pNode)
{
	CNode* tvNode = NULL;
	std::deque<CNode*> vList;
	vList.push_front(this);
	while(vList.size() != 0) 
	{
		CNode* tvNode = vList[0];
		vList.pop_front();
		if(tvNode == pNode)
		{
			return true;
		}
		for(int i=0; i<tvNode->GetChildCount(); i++) 
		{
			vList.push_front(tvNode->GetChildByIdx(i));
		}
	}
	return false;
}

int CNode::GetLeafCount()
{
	int nCount = 0;
	std::deque<CNode*> vList;
	vList.push_front(this);
	while(vList.size() != 0) 
	{
		CNode* pNode = vList[0];
		vList.pop_front();

		if (pNode->GetChildCount() == 0)
		{
			nCount++;
		}

		for(int i=0; i<pNode->GetChildCount(); i++) 
		{
			vList.push_front(pNode->GetChildByIdx(i));
		}
	}
	return nCount;
}