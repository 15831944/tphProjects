// LeafNode.h: interface for the CLeafNode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LEAFNODE_H__731EF4BA_3C85_11D4_9307_0080C8F982B1__INCLUDED_)
#define AFX_LEAFNODE_H__731EF4BA_3C85_11D4_9307_0080C8F982B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TVNode.h"

class CLeafNode : public CTVNode  
{
	DECLARE_SERIAL(CLeafNode)
public:
	CLeafNode();
	virtual ~CLeafNode();

	virtual CNode* GetChildByIdx(int idx) { return NULL; };
	virtual CNode* GetChildByName(CString strName) { return NULL; };
	virtual int GetChildCount() { return 0; };
	virtual BOOL AddChild(CNode* pChild) { return FALSE; };
	virtual BOOL RemoveChild(CNode* pChild) { return FALSE; };

	virtual BOOL IsLeaf() { return TRUE; };

	virtual void Serialize(CArchive& ar);
};

#endif // !defined(AFX_LEAFNODE_H__731EF4BA_3C85_11D4_9307_0080C8F982B1__INCLUDED_)
