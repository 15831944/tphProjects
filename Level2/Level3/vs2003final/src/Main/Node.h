// Node.h: interface for the CNode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NODE_H__0C4CD225_3AF7_11D4_9305_0080C8F982B1__INCLUDED_)
#define AFX_NODE_H__0C4CD225_3AF7_11D4_9305_0080C8F982B1__INCLUDED_

#pragma warning( disable : 4786 )
#include <vector>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CNode : public CObject
{
	DECLARE_SERIAL( CNode )
public:
	//CNode();
	CNode();
	CNode(CNode& node);
	virtual ~CNode();

	virtual CString Name() { return m_strName; };
	virtual void Name(CString strName) { m_strName = strName; };

	CNode* Parent() { return m_pParent; };

	bool Exist(const CNode* pNode);
	int GetLeafCount();
	virtual CNode* GetChildByIdx(int idx) { return m_vChildren[idx]; };
	virtual CNode* GetChildByName(CString strName);
	virtual int GetChildCount() { return (int)m_vChildren.size(); };
	virtual BOOL AddChild(CNode* pChild);	// adds a child to the node, returns TRUE if sucessful
	virtual BOOL AddChildAfter(CNode* pChild, CNode* pAfterNode);
	virtual BOOL AddChildBefore(CNode* pChild, CNode* pBeforeNode);
	virtual BOOL RemoveChild(CNode* pChild);// removes a child from the node, returns TRUE if sucessful
	virtual BOOL RemoveChild(int i);
	virtual BOOL DeleteAllChildren();

	virtual void Serialize(CArchive& ar);

protected:
	CNode(CString strName);
	CNode* m_pParent;
	std::vector<CNode*> m_vChildren;
	CString m_strName;
};

#endif // !defined(AFX_NODE_H__0C4CD225_3AF7_11D4_9305_0080C8F982B1__INCLUDED_)
