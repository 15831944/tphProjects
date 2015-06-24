// TVNode.h: interface for the CTVNode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TVNODE_H__0555739B_3BCE_11D4_9306_0080C8F982B1__INCLUDED_)
#define AFX_TVNODE_H__0555739B_3BCE_11D4_9306_0080C8F982B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Node.h"

#define NODE_HINT_SELCHANGED 0x5A
#define NODE_HINT_NEWNODE 0x5B
#define NODE_HINT_REFRESHNODE 0x5C
#define UPDATETREE_TERMINAL 0X6A
#define UPDATETREE_AIRSIDE  0X6B
#define UPDATETREE_LANDSIDE 0X6C
#define UPDATETREE_ONBOARD 0X6D
class CTVNode : public CNode  
{
	DECLARE_SERIAL( CTVNode )
public:
	typedef struct tagTVNODEINFO
	{
		char name[16];
		CTVNode* pTVNode;
	} TVNODEINFO;

	enum EState { collapsed = 0, expanded = 1 }; 

	CTVNode();
	CTVNode(CTVNode& node);
	CTVNode(CString strName);
	CTVNode(CString strName, UINT nID);
	CTVNode(CString strName, CString sDesc);
	CTVNode(CString strName, CString sDesc, UINT nID);
	virtual ~CTVNode();
	
	virtual void Serialize(CArchive& ar);
	virtual BOOL IsLeaf() { return FALSE; }

	virtual BOOL DefinesColumns() { return FALSE; }
	virtual std::vector<CString>* GetColumnNames() { return NULL; }

	virtual BOOL RemoveChild(int i);
	BOOL RemoveChild(CTVNode* pNode);
	void clearNode();
	//virtual void DrawOGL();

	HTREEITEM m_hItem;
	EState m_eState;
	UINT m_nIDResource;
	CString m_sDescription;
	int m_iImageIdx;
	DWORD m_dwData;
	std::vector<HTREEITEM> m_vItem;

	//CObject* m_pObj;

	friend CArchive& operator>>(CArchive& ar, CTVNode::EState& eState);
	friend CArchive& operator<<(CArchive& ar, CTVNode::EState& eState);
};

/*
inline void CTVNode::DrawOGL()
{
	for(int _i=0; _i<m_vChildren.size(); _i++)
	{
		((CTVNode*) m_vChildren[_i])->DrawOGL();
	}
}
*/

#endif // !defined(AFX_TVNODE_H__0555739B_3BCE_11D4_9306_0080C8F982B1__INCLUDED_)
